/*
   Copyright (c) 2013 Broadcom Corporation
   All Rights Reserved

    <:label-BRCM:2013:DUAL/GPL:standard

    Unless you and Broadcom execute a separate written software license
    agreement governing use of this software, this software is licensed
    to you under the terms of the GNU General Public License version 2
    (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
    with the following added to such license:

       As a special exception, the copyright holders of this software give
       you permission to link this software with independent modules, and
       to copy and distribute the resulting executable under terms of your
       choice, provided that you also meet, for each linked independent
       module, the terms and conditions of the license of that module.
       An independent module is a module which is not derived from this
       software.  The special exception does not apply to any modifications
       of the software.

    Not withstanding the above, under no circumstances may you combine
    this software in any way with any other Broadcom software provided
    under a license other than the GPL, without Broadcom's express prior
    written consent.

:>
*/

#include "rdd.h"
#include "rdd_tcam_ic.h"
#include "rdd_data_structures_auto.h"
#include "rdp_drv_proj_cntr.h"
#include "rdd_ic_common.h"

/* Max number of FW commands in a single table, including terminator command */
#define RDD_TCAM_MAX_CMD_SET_SIZE         RDD_TCAM_IC_CMD_TABLE_SIZE2

/* Max command transfer size (bytes) */
#define RDD_TCAM_MAX_CMD_SIZE             8

/* Number of distinct classification fields */
#define RDD_TCAM_NUM_FIELDS               (RDPA_IC_LAST_KEY + RDD_TCAM_MAX_GEN_FIELDS)

/*
 * IC field source type.
 * This enum should move to header file shared between RDD and FW
 */
typedef enum rdd_tcam_field_source
{
    RDD_TCAM_FLD_SOURCE_NONE = 0,         /* Used in command table terminator */
    RDD_TCAM_FLD_SOURCE_PARSER = 1,       /* Parser area */
    RDD_TCAM_FLD_SOURCE_PACKET = 2,       /* Packet buffer */
    RDD_TCAM_FLD_SOURCE_L3_HDR = 3,       /* L3 header in packet buffer */
    RDD_TCAM_FLD_SOURCE_L4_HDR = 4,       /* L4 header in packet buffer */
    RDD_TCAM_FLD_INGRESS_PORT = 5,        /* taken from PD */
    RDD_TCAM_FLD_GEM_FLOW = 6,            /* taken from resolution */
    RDD_TCAM_FLD_SOURCE_IP = 7,           /* Ipv6 - CRC , ipv4 - taken from parser */
    RDD_TCAM_FLD_DESTINATION_IP = 8,      /* Ipv6 - CRC , ipv4 - taken from parser */
} rdd_tcam_field_source_t;

/*
 * Static (constant) IC field info
 */
typedef struct rdd_tcam_field_info
{
    rdd_tcam_field_source_t source;     /* Field source */
    uint16_t offset;                    /* Byte offset from the source */
    uint16_t size;                      /* Field size */
    uint32_t mask;                      /* Field mask - only for fields <= 4 bytes */
} rdd_tcam_field_info_t;

/*
 * Dynamic IC field info
 */
typedef struct rdd_tcam_field
{
    uint16_t offset;            /* Byte offset in TCAM record */
    uint16_t use_count;         /* Field use count */
} rdd_tcam_field_t;

/* F/W command (host side representation) */
typedef struct rdd_tcam_fw_cmd
{
    rdd_tcam_field_source_t source;     /* Field source */
    uint16_t src_offset;                /* Byte offset from the source */
    uint8_t tcam_offset;                /* Byte offset in TCAM record */
    uint8_t size;                       /* Field size */
} rdd_tcam_fw_cmd_t;

/* IC table control block */
typedef struct rdd_tcam_table
{
    rdd_tcam_table_id id;                                       /* table id */
    rdpa_ic_fields all_fields_mask;                             /* All used fields mask */
    rdd_tcam_field_t fields[RDD_TCAM_NUM_FIELDS];               /* Field data */
    rdd_tcam_field_info_t gen_fields[RDD_TCAM_MAX_GEN_FIELDS];  /* Generic fields info */
    rdp_tcam_key_area_t used_mask;                              /* Allocated field mask */
    rdd_tcam_fw_cmd_t cmds[RDD_TCAM_MAX_CMD_SET_SIZE - 1];      /* command set, excluding terminator command */
    uint32_t cmd_table_size;                                    /* command table size (in entries) including terminating entry */
    uint32_t active_cmd_table;                                  /* active table index in command table array */
    uint32_t key_size;                                          /* Key size: 32 / 64 bytes */
    uint16_t num_cmds;                                          /* Number of commands in cmds[] array */
    uint16_t max_cmds;                                          /* Max number of commands in cmds[] array */
    const rdd_module_t *module;                                 /* F/W module structure */
    uint32_t classification_result;                             /* invert_match bit action, reason, counter offset */
    rdd_tcam_ic_module module_id;                               /* FW module id */
} rdd_tcam_table_t;

/* Check if field is generic */
#define RDD_TCAM_FIELD_IS_GENERIC(fld) \
    ((int)(fld) >= (int)RDPA_IC_GENERIC_1 && (int)(fld) <= (int)RDPA_IC_GENERIC_2)


/* Translate RDD_IC field index to TCAM_IC field index */
#define RDD_TCAM_RDD_IC_TO_TCAM_IC(fld, gen_indexes) \
    (RDD_TCAM_FIELD_IS_GENERIC(fld) ? (RDPA_IC_LAST_KEY + gen_indexes[fld-(int)RDPA_IC_GENERIC_1]) : fld);

/* Make field mask given its name, assuming that constants name_F_OFFSET and name_F_WIDTH are defined */

/* Byte width */
#define RDD_TCAM_FIELD_WIDTH(_name) \
    ((_name ## _F_WIDTH <= 8) ? 1 : (_name ## _F_WIDTH <= 16) ? 2 : \
        (_name ## _F_WIDTH <= 32) ? 4 : (_name ## _F_WIDTH / 8))

/* Mask in FIELD_WIDTH sized area */
#define RDD_TCAM_FIELD_MASK(_name) \
    (((1LL << _name ## _F_WIDTH) - 1) << \
        ((_name ## _F_OFFSET >= RDD_TCAM_FIELD_WIDTH(_name) * 8) ? \
            (_name ## _F_OFFSET - RDD_TCAM_FIELD_WIDTH(_name) * 8) : _name ## _F_OFFSET))

/* Field info */
#define RDD_TCAM_FIELD_INFO(_name, _src)   \
    { .size = RDD_TCAM_FIELD_WIDTH(_name), \
      .mask = RDD_TCAM_FIELD_MASK(_name),  \
      .offset = _name ## _OFFSET + ((_src == RDD_TCAM_FLD_SOURCE_PARSER) ? offsetof(RDD_PACKET_BUFFER_DTS, parser_result) : 0), \
      .source = _src }

#define RDD_TCAM_FIELD_IP_INFO(_name, _src)   \
    { .size = RDD_TCAM_FIELD_WIDTH(_name), \
      .mask = RDD_TCAM_FIELD_MASK(_name),  \
      .offset = _name ## _OFFSET + offsetof(RDD_PACKET_BUFFER_DTS, parser_result), \
      .source = _src }

/* Field info with custom mask */
#define RDD_TCAM_FIELD_INFO_MASK(_name, _src, _mask)    \
    { .size = RDD_TCAM_FIELD_WIDTH(_name),              \
      .mask = _mask,                                    \
      .offset = _name ## _OFFSET + ((_src == RDD_TCAM_FLD_SOURCE_PARSER) ? offsetof(RDD_PACKET_BUFFER_DTS, parser_result) : 0), \
      .source = _src }

/* Field info with custom size and mask */
#define RDD_TCAM_FIELD_INFO_SIZE_MASK(_name, _size, _src, _mask)    \
    { .size = RDD_TCAM_FIELD_WIDTH(_size),              \
      .mask = _mask,                                    \
      .offset = _name ## _OFFSET + ((_src == RDD_TCAM_FLD_SOURCE_PARSER) ? offsetof(RDD_PACKET_BUFFER_DTS, parser_result) : 0), \
      .source = _src }

/* Field-info table.
 */
static const rdd_tcam_field_info_t rdd_tcam_fields[] =
{
    [RDPA_IC_L3_PROTOCOL] = RDD_TCAM_FIELD_INFO(PARSER_RESULT_LAYER3_PROTOCOL, RDD_TCAM_FLD_SOURCE_PARSER),
    [RDPA_IC_IP_PROTOCOL] = RDD_TCAM_FIELD_INFO(PARSER_RESULT_PROTOCOL, RDD_TCAM_FLD_SOURCE_PARSER),
    [RDPA_IC_DSCP] = RDD_TCAM_FIELD_INFO_MASK(PARSER_RESULT_TOS, RDD_TCAM_FLD_SOURCE_PARSER, 0xfc),
    [RDPA_IC_SRC_IP] = RDD_TCAM_FIELD_IP_INFO(PARSER_RESULT_SOURCE_IP, RDD_TCAM_FLD_SOURCE_IP),
    [RDPA_IC_DST_IP] = RDD_TCAM_FIELD_IP_INFO(PARSER_RESULT_DESTINATION_IP, RDD_TCAM_FLD_DESTINATION_IP),
    [RDPA_IC_SRC_PORT] = RDD_TCAM_FIELD_INFO(PARSER_RESULT_SOURCE_PORT, RDD_TCAM_FLD_SOURCE_PARSER),
    [RDPA_IC_DST_PORT] = RDD_TCAM_FIELD_INFO(PARSER_RESULT_DESTINATION_PORT, RDD_TCAM_FLD_SOURCE_PARSER),

    [RDPA_IC_INGRESS_PORT] = { .size = 1, .mask = 0xff, .offset = 0, .source = RDD_TCAM_FLD_INGRESS_PORT },
    [RDPA_IC_OUTER_VID] = RDD_TCAM_FIELD_INFO_MASK(PARSER_RESULT_OUTER_VLAN, RDD_TCAM_FLD_SOURCE_PARSER, 0xfff),
    [RDPA_IC_INNER_VID] = RDD_TCAM_FIELD_INFO_MASK(PARSER_RESULT_INNER_VLAN, RDD_TCAM_FLD_SOURCE_PARSER, 0xfff),
    [RDPA_IC_OUTER_PBIT] = RDD_TCAM_FIELD_INFO_SIZE_MASK(PARSER_RESULT_OUTER_VLAN, VLAN_TAG_PBITS, RDD_TCAM_FLD_SOURCE_PARSER, 0xe0),
    [RDPA_IC_INNER_PBIT] = RDD_TCAM_FIELD_INFO_SIZE_MASK(PARSER_RESULT_INNER_VLAN, VLAN_TAG_PBITS, RDD_TCAM_FLD_SOURCE_PARSER, 0xe0),
    [RDPA_IC_NUM_OF_VLANS] = RDD_TCAM_FIELD_INFO(PARSER_RESULT_VLANS_NUM, RDD_TCAM_FLD_SOURCE_PARSER),

    [RDPA_IC_OUTER_TPID] =  { .offset = 12, .size = 2, .mask = 0xffff, .source = RDD_TCAM_FLD_SOURCE_PACKET },
    [RDPA_IC_INNER_TPID] =  { .offset = 16, .size = 2, .mask = 0xffff, .source = RDD_TCAM_FLD_SOURCE_PACKET },

    [RDPA_IC_DST_MAC] = { .source = RDD_TCAM_FLD_SOURCE_PARSER,
                          .offset = PARSER_RESULT_DA1_2_OFFSET + offsetof(RDD_PACKET_BUFFER_DTS, parser_result),
                          .size = 6 },
    [RDPA_IC_SRC_MAC] = { .source = RDD_TCAM_FLD_SOURCE_PARSER,
                          .offset = PARSER_RESULT_SA1_2_OFFSET + offsetof(RDD_PACKET_BUFFER_DTS, parser_result),
                          .size = 6 },
    [RDPA_IC_IPV6_FLOW_LABEL] = { .offset = 0, .size = 4, .mask = 0x000fffff, .source = RDD_TCAM_FLD_SOURCE_L3_HDR },
    [RDPA_IC_ETHER_TYPE] = RDD_TCAM_FIELD_INFO(PARSER_RESULT_ETHERTYPE, RDD_TCAM_FLD_SOURCE_PARSER),
    [RDPA_IC_SSID] = { },                                                                                               /* ??? */
    [RDPA_IC_GEM_FLOW] = { .size = 1, .mask = 0xff, .offset = 0, .source = RDD_TCAM_FLD_GEM_FLOW },
};

/* IC tables */
static rdd_tcam_table_t rdd_tcam_tables[RDD_TCAM_MAX_TABLES];

static uint32_t shadow_cmd_table;        /* shadow table index in command table array */
static bdmf_boolean fw_initialized;


/******************************************************************************/
/*                                                                            */
/*                            Global Variables                                */
/*                                                                            */
/******************************************************************************/


/******************************************************************************/
/*                                                                            */
/*                            F/W tables configuration helpers                */
/*                                                                            */
/******************************************************************************/

/* Determine FW cmdid for the given command */
static rdd_tcam_ic_cmd_type _rdd_tcam_fw_cmd_id(const rdd_tcam_fw_cmd_t *cmd)
{
    rdd_tcam_ic_cmd_type cmdid = TCAM_IC_CMD_TYPE_LAST;

    switch (cmd->source)
    {
    case RDD_TCAM_FLD_SOURCE_PARSER:
        switch (cmd->size)
        {
        case 1:
            cmdid = TCAM_IC_CMD_TYPE_PARSER_1;
            break;
        case 2:
            cmdid = TCAM_IC_CMD_TYPE_PARSER_2;
            break;
        case 4:
            cmdid = TCAM_IC_CMD_TYPE_PARSER_4;
            break;
        case 6:
            cmdid = TCAM_IC_CMD_TYPE_PARSER_6;
            break;
        case 8:
            cmdid = TCAM_IC_CMD_TYPE_PARSER_8;
            break;
        default:
            BDMF_TRACE_ERR("Internal error!!! Can't map source: %d and size %u to FW cmdid\n", cmd->source, cmd->size);
            break;
        }
        break;

    case RDD_TCAM_FLD_SOURCE_PACKET:
        cmdid = TCAM_IC_CMD_TYPE_PACKET;
        break;

    case RDD_TCAM_FLD_SOURCE_L3_HDR:
        cmdid = TCAM_IC_CMD_TYPE_L3;
        break;

    case RDD_TCAM_FLD_SOURCE_L4_HDR:
        cmdid = TCAM_IC_CMD_TYPE_L4;
        break;

    case RDD_TCAM_FLD_INGRESS_PORT:
        cmdid = TCAM_IC_CMD_TYPE_INGRESS_PORT;
        break;

    case RDD_TCAM_FLD_GEM_FLOW:
        cmdid = TCAM_IC_CMD_TYPE_GEM_FLOW;
        break;

    case RDD_TCAM_FLD_SOURCE_IP:
        cmdid = TCAM_IC_CMD_TYPE_SOURCE_IP;
        break;

    case RDD_TCAM_FLD_DESTINATION_IP:
        cmdid = TCAM_IC_CMD_TYPE_DESTINATION_IP;
        break;

    default:
        BDMF_TRACE_ERR("Internal error!!! Can't map source: %d to FW cmdid\n", cmd->source);
        break;
    }

    return cmdid;
}

/* Update command table */
static void _rdd_tcam_fw_cmd_table_update(rdd_tcam_table_t *table)
{
    uint32_t cmd_table_address;
    uint32_t new_shadow_table;
    uint32_t cmd_entry;
    int i;

    /* Write command table to the shadow */
    cmd_entry = shadow_cmd_table * RDD_TCAM_MAX_CMD_SET_SIZE;
    for (i = 0; i < table->num_cmds; i++, ++cmd_entry)
    {
        rdd_tcam_ic_cmd_type cmd_id;
        cmd_id = _rdd_tcam_fw_cmd_id(&table->cmds[i]);
        RDD_TCAM_IC_CMD_SRC_OFFSET_WRITE_G(table->cmds[i].src_offset, RDD_TCAM_IC_CMD_TABLE_ADDRESS_ARR, cmd_entry);
        RDD_TCAM_IC_CMD_SIZE_WRITE_G(table->cmds[i].size, RDD_TCAM_IC_CMD_TABLE_ADDRESS_ARR, cmd_entry);
        RDD_TCAM_IC_CMD_CMDID_WRITE_G(cmd_id, RDD_TCAM_IC_CMD_TABLE_ADDRESS_ARR, cmd_entry);
        RDD_TCAM_IC_CMD_DST_OFFSET_WRITE_G(table->cmds[i].tcam_offset, RDD_TCAM_IC_CMD_TABLE_ADDRESS_ARR, cmd_entry);
        RDD_TCAM_IC_CMD_TERMINATOR_WRITE_G(0, RDD_TCAM_IC_CMD_TABLE_ADDRESS_ARR, cmd_entry);
    }

    /* Write terminating command */
    /* SRC_OFFSET in terminating command contains table_id */
    RDD_TCAM_IC_CMD_SRC_OFFSET_WRITE_G(table->id, RDD_TCAM_IC_CMD_TABLE_ADDRESS_ARR, cmd_entry);
    RDD_TCAM_IC_CMD_SIZE_WRITE_G(1, RDD_TCAM_IC_CMD_TABLE_ADDRESS_ARR, cmd_entry);
    RDD_TCAM_IC_CMD_TERMINATOR_WRITE_G(1, RDD_TCAM_IC_CMD_TABLE_ADDRESS_ARR, cmd_entry);
    RDD_TCAM_IC_CMD_CMDID_WRITE_G(0, RDD_TCAM_IC_CMD_TABLE_ADDRESS_ARR, cmd_entry); /* Don't care */
    RDD_TCAM_IC_CMD_DST_OFFSET_WRITE_G(table->key_size - 1, RDD_TCAM_IC_CMD_TABLE_ADDRESS_ARR, cmd_entry);

    /* Exchange active and shadow tables */
    new_shadow_table = table->active_cmd_table;
    table->active_cmd_table = shadow_cmd_table;
    shadow_cmd_table = new_shadow_table;
    for (i = 0; i < GROUPED_EN_SEGMENTS_NUM; i++)
    {
        if (RDD_TCAM_IC_CMD_TABLE_ADDRESS_ARR[i] != INVALID_TABLE_ADDRESS)
        {
            cmd_table_address = RDD_TCAM_IC_CMD_TABLE_ADDRESS_ARR[i] + RDD_TCAM_MAX_CMD_SET_SIZE * sizeof(RDD_TCAM_IC_CMD_DTS) * table->active_cmd_table;
            RDD_TCAM_IC_CFG_CMD_TABLE_WRITE(cmd_table_address, ((uint8_t *)RDD_TCAM_IC_CFG_TABLE_PTR(i) +
		        (sizeof(RDD_TCAM_IC_CFG_DTS) * table->module_id)));
        }
    }
}

/* Update classification result */
static void _rdd_tcam_fw_classification_result_update(rdd_tcam_table_t *table)
{
    RDD_TCAM_IC_CFG_CLASSIFICATION_RESULT_WRITE_G(table->classification_result,
        table->module->cfg_ptr, table->module_id);
}

/* module init */
static void _rdd_tcam_fw_module_init(rdd_tcam_table_t *table)
{
    const rdd_tcam_table_parm_t *params = table->module->params;

    if (!fw_initialized)
    {
        shadow_cmd_table = TCAM_IC_MODULE_LAST + 1;
        fw_initialized = 1;
    }

    /* Set-up (empty) command table  */
    _rdd_tcam_fw_cmd_table_update(table);

    /* Make sure that scratch offset is 64-byte aligned */
    BUG_ON((params->scratch_offset + offsetof(RDD_PACKET_BUFFER_DTS, scratch) + TCAM_IC_SCRATCH_KEY_OFFSET) & 63);

    /* Module init. Set up module configuration */
    RDD_TCAM_IC_CFG_RES_OFFSET_WRITE_G(table->module->res_offset, table->module->cfg_ptr, table->module_id);
    RDD_TCAM_IC_CFG_CONTEXT_OFFSET_WRITE_G(table->module->context_offset, table->module->cfg_ptr, table->module_id);
    RDD_TCAM_IC_CFG_CLASSIFICATION_RESULT_WRITE_G(table->classification_result, table->module->cfg_ptr, table->module_id);
    RDD_TCAM_IC_CFG_KEY_SIZE_WRITE_G(table->key_size, table->module->cfg_ptr, table->module_id);
    RDD_TCAM_IC_CFG_SCRATCH_OFFSET_WRITE_G(params->scratch_offset, table->module->cfg_ptr, table->module_id);
}


/******************************************************************************/
/*                                                                            */
/*                            Internal helpers                                */
/*                                                                            */
/******************************************************************************/

/* Prepare TCAM key and mask areas */
static bdmf_error_t _rdd_tcam_prepare_tcam_areas(rdd_tcam_table_id table_id, rdp_tcam_key_area_t *tcam_key,
    rdp_tcam_key_area_t *tcam_mask)
{
    rdd_tcam_table_t *table = &rdd_tcam_tables[table_id];

    memset(tcam_key, 0, sizeof(rdp_tcam_key_area_t));
    memset(tcam_mask, 0, sizeof(rdp_tcam_key_area_t));

    /* key size in bytes */
    tcam_key->b[table->key_size-1]     = (uint8_t)table_id;
    tcam_mask->b[table->key_size-1]    = 0xff;

    return BDMF_ERR_OK;
}

/* Get IC field info given the IC table and field index */
static const rdd_tcam_field_info_t *_rdd_tcam_field_info(rdd_tcam_table_id table_id, int tcam_fld)
{
    rdd_tcam_table_t *table = &rdd_tcam_tables[table_id];
    const rdd_tcam_field_info_t *fld_info;

    /* Generic fields must be configured before use */
    if (tcam_fld >= RDPA_IC_LAST_KEY)
    {
        fld_info = &table->gen_fields[tcam_fld - RDPA_IC_LAST_KEY];
    }
    else
    {
        fld_info = &rdd_tcam_fields[tcam_fld];
    }
    if (!fld_info->size)
    {
        BDMF_TRACE_ERR("Field %d is unconfigured\n", tcam_fld);
        return NULL;
    }
    return fld_info;
}

/* Allocate room for new IC field in TCAM key */
static bdmf_error_t _rdd_tcam_field_allocate(rdd_tcam_table_id table_id, int tcam_fld)
{
    static uint8_t zero[8] = {};
    rdd_tcam_table_t *table = &rdd_tcam_tables[table_id];
    const rdd_tcam_field_info_t *fld_info = _rdd_tcam_field_info(table_id, tcam_fld);
    int i;

    /* Got unconfigured field? RDPA should've caught it */
    if (!fld_info)
        return BDMF_ERR_INTERNAL;

    /* Find unused room in TCAM key.
     * 1 byte fields are added from the right
     * other sizes are added from the left
     */
    if (fld_info->size == 1)
    {
        for (i = table->key_size-2; i >= 0; i--)
        {
            if (!table->used_mask.b[i])
                break;
        }
    }
    else
    {
        int align = (fld_info->size == 2) ? 2 : 4;
        for (i = 0; i < table->key_size-1; i += align)
        {
            if (!memcmp(&table->used_mask.b[i], zero, fld_info->size))
                break;
        }
    }

    /* Key area overfloew ? */
    if (i < 0 || i >= table->key_size - 1)
    {
        BDMF_TRACE_ERR("Can't add classification flow. TCAM key record is full\n");
        return BDMF_ERR_OVERFLOW;
    }

    table->fields[tcam_fld].offset = i;

    /* mark room allocated for the field in TCAM area as reserved */
    for (; i < table->fields[tcam_fld].offset + fld_info->size; i++)
        table->used_mask.b[i] = 0xff;

    return BDMF_ERR_OK;
}

/* Decrement field reference count. Return number of fields that are no longer referenced */
static int _rdd_tcam_unreference_fields(rdd_tcam_table_id table_id, rdpa_ic_fields fields, const uint8_t gen_indexes[])
{
    static uint8_t zero[8] = {};
    rdd_tcam_table_t *table = &rdd_tcam_tables[table_id];
    int num_deleted = 0;
    rdd_tcam_field_t *fld_data;
    int fld;
    int tcam_fld;

    /* Go over fields in key mask and un-reference them.
     * If field becomes unused - command table should be rebuilt
     */
    while ((fld = ffs(fields)) != 0)
    {
        --fld; /* 1-based --> 0-based */

        /* Map RDD_IC_ field index to TCAM_IC field index */
        tcam_fld = RDD_TCAM_RDD_IC_TO_TCAM_IC(fld, gen_indexes);

        fld_data = &table->fields[tcam_fld];

        BUG_ON(!fld_data->use_count);

        /* Reduce usecount. If field becomes unreferenced - update TCAM mask */
        --fld_data->use_count;
        if (!fld_data->use_count)
        {
            const rdd_tcam_field_info_t *fld_info = _rdd_tcam_field_info(table_id, tcam_fld);
            /* Clear field from TCAM mask */
            memcpy(&table->used_mask.b[fld_data->offset], zero, fld_info->size);
            fld_data->offset = 0;
            table->all_fields_mask &= ~(1 << tcam_fld);
            ++num_deleted;
        }
        fields &= ~(1 << fld);
    }
    return num_deleted;
}

/* Copy key field to tcam record and update mask */
static void _rdd_tcam_copy_field_to_tcam(rdd_tcam_table_id table_id, int fld,
    int tcam_fld, const rdpa_ic_key_t *rule_key,
    rdp_tcam_key_area_t *tcam_key, rdp_tcam_key_area_t *tcam_mask)
{
    static bdmf_mac_t mac_mask = { .b = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }  };
    rdd_tcam_table_t *table = &rdd_tcam_tables[table_id];
    rdd_tcam_field_t *fld_data = &table->fields[tcam_fld];
    const rdd_tcam_field_info_t *fld_info = _rdd_tcam_field_info(table_id, tcam_fld);
    uint8_t *key = &tcam_key->b[fld_data->offset];
    uint8_t *mask = &tcam_mask->b[fld_data->offset];
    uint32_t ip_addr;

    /* Set mask for sizes 1, 2 and 4 */
    switch (fld_info->size)
    {
    case 1:
        *mask = fld_info->mask;
        break;
    case 2:
        *(uint16_t *)mask = cpu_to_be16(fld_info->mask);
        break;
    case 4:
        *(uint32_t *)mask = cpu_to_be32(fld_info->mask);
        break;
    default:
        break;
    }

    /* Set field value */
    switch (fld)
    {
    case RDPA_IC_L3_PROTOCOL:
        *key = rule_key->l3_protocol << PARSER_RESULT_LAYER3_PROTOCOL_F_OFFSET;
        break;

    case RDPA_IC_IP_PROTOCOL:
        *key = rule_key->protocol;
        break;

    case RDPA_IC_DSCP:
        /* shifted for FW (loads 1 byte) */
        *key = (rule_key->dscp)<<2;
        break;

    case RDPA_IC_SRC_IP:
        if (rule_key->src_ip.family == bdmf_ip_family_ipv6)
            rdd_crc_ipv6_addr_calc(&rule_key->src_ip, &ip_addr);
        else
            ip_addr = rule_key->src_ip.addr.ipv4;
        *(uint32_t *)key = cpu_to_be32(ip_addr);
        break;

    case RDPA_IC_DST_IP:
        if (rule_key->dst_ip.family == bdmf_ip_family_ipv6)
            rdd_crc_ipv6_addr_calc(&rule_key->dst_ip, &ip_addr);
        else
            ip_addr = rule_key->dst_ip.addr.ipv4;
        *(uint32_t *)key = cpu_to_be32(ip_addr);
        break;

    case RDPA_IC_SRC_PORT:
        *(uint16_t *)key = cpu_to_be16(rule_key->src_port);
        break;

    case RDPA_IC_DST_PORT:
        *(uint16_t *)key = cpu_to_be16(rule_key->dst_port);
        break;

    case RDPA_IC_INGRESS_PORT:
        *key = rule_key->ingress_port;
        break;

    case RDPA_IC_GEM_FLOW:
        *key = rule_key->gem_flow;
        break;

    case RDPA_IC_OUTER_VID:
        *(uint16_t *)key = cpu_to_be16(rule_key->outer_vid);
        break;

    case RDPA_IC_INNER_VID:
        *(uint16_t *)key = cpu_to_be16(rule_key->inner_vid);
        break;

    case RDPA_IC_OUTER_PBIT:
        /* shifted for FW (loads 1 byte) */
        *key = (rule_key->outer_pbits)<<5;
        break;

    case RDPA_IC_INNER_PBIT:
        /* shifted for FW (loads 1 byte) */
        *key = (rule_key->inner_pbits)<<5;
        break;

    case RDPA_IC_NUM_OF_VLANS:
        *key = rule_key->number_of_vlans;
        break;

    case RDPA_IC_OUTER_TPID:
        *(uint16_t *)key = cpu_to_be16(rule_key->outer_tpid);
        break;

    case RDPA_IC_INNER_TPID:
        *(uint16_t *)key = cpu_to_be16(rule_key->inner_tpid);
        break;

    case RDPA_IC_DST_MAC:
        *(bdmf_mac_t *)key = rule_key->dst_mac;
        *(bdmf_mac_t *)mask = mac_mask;
        break;

    case RDPA_IC_SRC_MAC:
        *(bdmf_mac_t *)key = rule_key->src_mac;
        *(bdmf_mac_t *)mask = mac_mask;
        break;

    case RDPA_IC_GENERIC_1:
        *(uint32_t *)key = cpu_to_be32(rule_key->generic_key_1);
        if (rule_key->generic_mask != 0)
        {
            *(uint32_t *)mask = cpu_to_be32(rule_key->generic_mask);
        }
        break;

    case RDPA_IC_GENERIC_2:
        *(uint32_t *)key = cpu_to_be32(rule_key->generic_key_2);
        if (rule_key->generic_mask != 0)
        {
            *(uint32_t *)mask = cpu_to_be32(rule_key->generic_mask);
        }
        break;

    case RDPA_IC_IPV6_FLOW_LABEL:
        *(uint32_t *)key = cpu_to_be32(rule_key->ipv6_flow_label);
        break;

    case RDPA_IC_ETHER_TYPE:
        *(uint16_t *)key = cpu_to_be16(rule_key->etype);
        break;

    case RDPA_IC_SSID:
        *key = rule_key->ssid;
        break;

    default:
        BDMF_TRACE_ERR("Unexpected field %d\n", fld);
        break;
    }
}

/* (Re)build F/W command table */
static bdmf_error_t _rdd_tcam_build_cmd_table(rdd_tcam_table_id table_id)
{
    /* This function builds S/W mirror of F/W command table.
     * Multiple IC fields are merged into a single command where possible
     * Commands are sorted in order of increasing offset in tcam key
     */
    rdd_tcam_table_t *table = &rdd_tcam_tables[table_id];
    rdpa_ic_fields key_mask = table->all_fields_mask;
    const rdd_tcam_field_info_t *fld_info;
    rdd_tcam_field_t *fld_data;
    rdd_tcam_fw_cmd_t *cmd = NULL;
    int num_cmds = 0;
    bdmf_error_t err = BDMF_ERR_OK;
    int tcam_fld;

    /* Go over fields in all-key mask */

    /* Go over fields in all-key mask */
    while (key_mask)
    {
        /* Find field with min offset */
        uint32_t tmp_mask = key_mask;
        uint32_t min_offset = 64; /* big number */
        int tmp_fld = 0;

        tcam_fld = 0;
        while ((tmp_fld = ffs(tmp_mask)) != 0)
        {
            --tmp_fld; /* 1-based --> 0-based */
            if (table->fields[tmp_fld].offset < min_offset)
            {
                min_offset = table->fields[tmp_fld].offset;
                tcam_fld = tmp_fld;
            }
            tmp_mask &= ~(1 << tmp_fld);
        }

        fld_data = &table->fields[tcam_fld];
        fld_info = _rdd_tcam_field_info(table_id, tcam_fld);

        /* Try to merge with previous command if possible */
        if (!(tcam_fld >= RDPA_IC_LAST_KEY) &&
            cmd                                                 &&
            cmd->source == fld_info->source                     &&
            cmd->src_offset + cmd->size == fld_info->offset     &&
            cmd->tcam_offset + cmd->size == fld_data->offset    &&
            (cmd->size + fld_info->size == 2 || cmd->size + fld_info->size == 4 || cmd->size + fld_info->size == 8))
        {
            cmd->size += fld_info->size;
            key_mask &= ~(1 << tcam_fld);
            continue;
        }

        /* Unable to merge. Create new command */
        if (num_cmds == table->max_cmds)
        {
            err = BDMF_ERR_TOO_MANY;
            break;
        }

        cmd = &table->cmds[num_cmds++];
        cmd->source = fld_info->source;
        cmd->size = fld_info->size;
        cmd->src_offset = fld_info->offset;
        cmd->tcam_offset = fld_data->offset;

        /* to the next field */
        key_mask &= ~(1 << tcam_fld);
    }
    table->num_cmds = num_cmds;

    return err;
}

/* Copy command table to SRAM and activate it */
static void _rdd_tcam_activate_cmd_table(rdd_tcam_table_id table_id)
{
    rdd_tcam_table_t *table = &rdd_tcam_tables[table_id];
    _rdd_tcam_fw_cmd_table_update(table);
}

/* Optimize search key. Remove "derived fields" */
static void _rdd_tcam_key_optimize(rdd_tcam_table_id table_id, rdpa_ic_fields *rule_key_mask)
{
    /* Remove:
     * - L3 protocol type if src_ip, dst_ip or l4 protocol fields are present
     * - other optimizations are TBD
     */
    if (*rule_key_mask & (RDPA_IC_MASK_SRC_IP | RDPA_IC_MASK_DST_IP | RDPA_IC_MASK_SRC_PORT | RDPA_IC_MASK_DST_PORT))
        *rule_key_mask &= ~RDPA_IC_MASK_L3_PROTOCOL;
}



/******************************************************************************/
/*                                                                            */
/*                            External interface                              */
/*                                                                            */
/******************************************************************************/

/****************************************************************************************
 * module->init callback
 *****************************************************************************************/
int rdd_tcam_module_init(const rdd_module_t *module)
{
    const rdd_tcam_table_parm_t *table_parms = (const rdd_tcam_table_parm_t  *)module->params;
    rdd_tcam_table_id table_id = table_parms->module_id;
    rdd_tcam_table_t *table = &rdd_tcam_tables[table_id];
    bdmf_error_t rc;

    BUG_ON((unsigned)table_id >= RDD_TCAM_MAX_TABLES);
    if ((unsigned)table_id >= RDD_TCAM_MAX_TABLES)
    {
        return (int)BDMF_ERR_PARM;
    }

    /* If 1st call - init TCAM driver in 256 bit mode (default) */
    if (!fw_initialized)
        drv_tcam_mode_set(RDP_TCAM_KEY_256);

    rc = rdd_tcam_table_create(table_id, table_parms);
    BUG_ON(rc != BDMF_ERR_OK);

    table->module = module;
    table->module_id = table_parms->module_id;

    /* Init classification module */
    _rdd_tcam_fw_module_init(table);

    return rc;
}

/* Create IC table */
bdmf_error_t rdd_tcam_table_create(
    rdd_tcam_table_id             table_id,
    const rdd_tcam_table_parm_t  *table_parms)
{
    rdd_tcam_table_t *table = &rdd_tcam_tables[table_id];
    bdmf_error_t err;

    RDD_BTRACE("table_id = %d, table_parms = %p\n", table_id, table_parms);

    /* Parameter validation */
    if ((unsigned)table_id >= RDD_TCAM_MAX_TABLES || !table_parms ||
        table_parms->max_cmds >= RDD_TCAM_MAX_CMD_SET_SIZE)
    {
        return BDMF_ERR_PARM;
    }
    else
    {
        RDD_TRACE("table_parms = { max_cmds = %d, invert_match = %d, module_id = %d, "
            "scratch_offset = %d }\n",
            table_parms->max_cmds, table_parms->invert_match, table_parms->module_id, table_parms->scratch_offset);
    }

    /* Check if table has already been created */
    if (table->max_cmds)
        return BDMF_ERR_ALREADY;

    err = drv_tcam_keysize_get(&table->key_size);
    if (err)
    {
        BDMF_TRACE_RET(err, "TCAM driver must be initialized first\n");
    }

    table->max_cmds = table_parms->max_cmds ? table_parms->max_cmds : RDD_TCAM_MAX_CMD_SET_SIZE - 1;
    table->cmd_table_size = table->max_cmds + 1;
    table->active_cmd_table = table_id;
    table->id = table_id;

    return BDMF_ERR_OK;
}


/* Destroy IC table */
bdmf_error_t rdd_tcam_table_destroy(
    rdd_tcam_table_id             table_id)
{
    rdd_tcam_table_t *table = &rdd_tcam_tables[table_id];

    RDD_BTRACE("table_id = %d\n", table_id);

    /* Parameter validation */
    if ((unsigned)table_id >= RDD_TCAM_MAX_TABLES)
        return BDMF_ERR_PARM;

    /* Check if table has already been created */
    if (!table->max_cmds)
        return BDMF_ERR_NOENT;

    /* Make sure that the table is not in use */
    if (table->all_fields_mask)
        return BDMF_ERR_STATE;

    memset(table, 0, sizeof(*table));

    return BDMF_ERR_OK;
}


/* Set "invert_match" property */
bdmf_error_t rdd_tcam_table_invert_match_set(
    rdd_tcam_table_id             table_id,
    bdmf_boolean                  invert_match)
{
    rdd_tcam_table_t *table = &rdd_tcam_tables[table_id];

    RDD_BTRACE("table_id = %d, invert_match = %d\n", table_id, invert_match);

    /* Parameter validation */
    if ((unsigned)table_id >= RDD_TCAM_MAX_TABLES)
        return BDMF_ERR_PARM;

    /* Check if table has already been created */
    if (!table->max_cmds)
        return BDMF_ERR_NOENT;

    /* Make sure that the table is not in use */
    if (table->all_fields_mask)
        return BDMF_ERR_STATE;

    /* Set "INVERT_MATCH" bit in classification_result */
    if (invert_match)
        table->classification_result |= TCAM_HIT_MISS_INVERT_INVERT;
    else
        table->classification_result &= ~TCAM_HIT_MISS_INVERT_INVERT;

    /* Update FW tables */
    _rdd_tcam_fw_classification_result_update(table);

    return BDMF_ERR_OK;
}


/* Get "invert_match" property */
bdmf_error_t rdd_tcam_table_invert_match_get(
    rdd_tcam_table_id             table_id,
    bdmf_boolean                 *invert_match)
{
    rdd_tcam_table_t *table = &rdd_tcam_tables[table_id];

    RDD_BTRACE("table_id = %d, invert_match = %p\n", table_id, invert_match);

    /* Parameter validation */
    if ((unsigned)table_id >= RDD_TCAM_MAX_TABLES)
        return BDMF_ERR_PARM;

    /* Check if table has already been created */
    if (!table->max_cmds)
        return BDMF_ERR_NOENT;

    *invert_match = (table->classification_result & TCAM_HIT_MISS_INVERT_INVERT) != 0;

    return BDMF_ERR_OK;
}

/* Add ingress classification rule */
bdmf_error_t rdd_tcam_rule_add(
    rdd_tcam_table_id table_id,
    rdpa_ic_fields rule_key_mask,
    const uint8_t gen_indexes[],
    const rdpa_ic_key_t *rule_key,
    rdp_tcam_key_area_t *tcam_key,
    rdp_tcam_key_area_t *tcam_mask)
{
    bdmf_error_t err;
    rdpa_ic_fields key_mask;
    rdd_tcam_table_t *table = &rdd_tcam_tables[table_id];
    int new_commands = 0;
    int fld;
    int tcam_fld;

    RDD_BTRACE("table_id = %d, rule_key_mask = %x, rule_key = %p, tcam_key = %p, tcam_mask = %p\n",
        table_id, rule_key_mask, rule_key, tcam_key, tcam_mask);

    /* Parameter validation */
    if ((unsigned)table_id >= RDD_TCAM_MAX_TABLES    ||
        !rule_key_mask                               ||
        !gen_indexes                                 ||
        !rule_key                                    ||
        !tcam_key                                    ||
        !tcam_mask)
    {
        return BDMF_ERR_PARM;
    }

    /* Make sure that table exists */
    if (!table->max_cmds)
    {
        return BDMF_ERR_NOENT;
    }

    err = _rdd_tcam_prepare_tcam_areas(table_id, tcam_key, tcam_mask);
    if (err)
        return err;

    /* Optimize search key. Remove "derived fields" */
    _rdd_tcam_key_optimize(table_id, &rule_key_mask);

    /* Go over active fields. If the fields are already used - there is no need
     * to rebuild command table.
     */
    key_mask = rule_key_mask;
    while ((fld = ffs(key_mask)) != 0)
    {
        --fld; /* 1-based --> 0-based */

        /* Map RDD_IC_ field index to TCAM_IC field index */
        tcam_fld = RDD_TCAM_RDD_IC_TO_TCAM_IC(fld, gen_indexes);

        /* sanity */
        if (tcam_fld >= RDD_TCAM_NUM_FIELDS)
        {
            BDMF_TRACE_ERR("fld %u is insane. tcam_fld %u >= %u\n", fld, tcam_fld, RDPA_IC_LAST_KEY);
            err = BDMF_ERR_PARM;
            break;
        }

        /* Allocate room in TCAM record if field is not already referenced */
        if (!table->fields[tcam_fld].use_count)
        {
            ++new_commands;
            err = _rdd_tcam_field_allocate(table_id, tcam_fld);
            if (err)
                break;
            table->all_fields_mask |= (1 << tcam_fld);
        }

        /* Copy field data into TCAM record */
        _rdd_tcam_copy_field_to_tcam(table_id, fld, tcam_fld, rule_key, tcam_key, tcam_mask);

        ++table->fields[tcam_fld].use_count;

        key_mask &= ~(1 << fld);
    }

    /* Re-build command table if necessary */
    if (!err && new_commands)
    {
        err = _rdd_tcam_build_cmd_table(table_id);
    }

    /* Roll back if error */
    if (err)
    {
        _rdd_tcam_unreference_fields(table_id, key_mask ^ rule_key_mask, gen_indexes);
        return err;
    }

    /* Finally activate the new command table in the F/W - if command set was changed */
    if (new_commands)
    {
        _rdd_tcam_activate_cmd_table(table_id);
    }

    return BDMF_ERR_OK;
}


/* Get low-level TCAM key and mask for existing rule. */
bdmf_error_t rdd_tcam_rule_key_get(
    rdd_tcam_table_id        table_id,
    rdpa_ic_fields           rule_key_mask,
    const uint8_t            gen_indexes[],
    const rdpa_ic_key_t     *rule_key,
    rdp_tcam_key_area_t     *tcam_key,
    rdp_tcam_key_area_t     *tcam_mask)
{
    rdd_tcam_table_t *table = &rdd_tcam_tables[table_id];
    bdmf_error_t err;
    int fld;
    int tcam_fld;

    RDD_BTRACE("table_id = %d, rule_key_mask = %x, rule_key = %p, tcam_key = %p, tcam_mask = %p\n",
        table_id, rule_key_mask, rule_key, tcam_key, tcam_mask);

    /* Parameter validation */
    if ((unsigned)table_id >= RDD_TCAM_MAX_TABLES    ||
        !rule_key_mask                               ||
        !gen_indexes                                 ||
        !rule_key                                    ||
        !tcam_key                                    ||
        !tcam_mask)
    {
        return BDMF_ERR_PARM;
    }
	
    /* Optimize search key. Remove "derived fields" */
    _rdd_tcam_key_optimize(table_id, &rule_key_mask);

    /* Make sure that all fields in rule_key_mask are in use */
    if ((rule_key_mask & RDPA_IC_MASK_GENERIC_1) || (rule_key_mask & RDPA_IC_MASK_GENERIC_2))
    {
        if (!(table->all_fields_mask >> RDPA_IC_LAST_KEY))
            return BDMF_ERR_PARM;
    }
    else if (rule_key_mask & ~table->all_fields_mask)
        return BDMF_ERR_PARM;

    err = _rdd_tcam_prepare_tcam_areas(table_id, tcam_key, tcam_mask);
    if (err)
        return err;

    /* Go over fields in key mask and un-reference them.
     * If field becomes unused - command table should be rebuilt
     */
    while ((fld = ffs(rule_key_mask)) != 0)
    {
        --fld; /* 1-based --> 0-based */

        /* Map RDD_IC_ field index to TCAM_IC field index */
        tcam_fld = RDD_TCAM_RDD_IC_TO_TCAM_IC(fld, gen_indexes);

        BUG_ON(!table->fields[tcam_fld].use_count);

        /* Copy field to TCAM key */
        _rdd_tcam_copy_field_to_tcam(table_id, fld, tcam_fld, rule_key, tcam_key, tcam_mask);

        rule_key_mask &= ~(1 << fld);
    }

    return BDMF_ERR_OK;
}


/* Delete ingress classification rule */
bdmf_error_t rdd_tcam_rule_delete(
    rdd_tcam_table_id        table_id,
    rdpa_ic_fields           rule_key_mask,
    const uint8_t            gen_indexes[])
{
    rdd_tcam_table_t *table = &rdd_tcam_tables[table_id];
    int delete_commands;

    RDD_BTRACE("table_id = %d, rule_key_mask = %x\n", table_id, rule_key_mask);

    /* Parameter validation */
    if ((unsigned)table_id >= RDD_TCAM_MAX_TABLES ||
        !rule_key_mask                            ||
        !gen_indexes)
    {
        return BDMF_ERR_PARM;
    }

    /* Optimize search key. Remove "derived fields" */
    _rdd_tcam_key_optimize(table_id, &rule_key_mask);

    /* Make sure that all fields in rule_key_mask are in use */
    if ((rule_key_mask & RDPA_IC_MASK_GENERIC_1) || (rule_key_mask & RDPA_IC_MASK_GENERIC_2))
    {
        if (!(table->all_fields_mask >> (RDPA_IC_LAST_KEY)))
            return BDMF_ERR_PARM;
    }
    else if (rule_key_mask & ~table->all_fields_mask)
        return BDMF_ERR_PARM;

    /* Reduce field reference count. Fields that are no longer referenced are removed from TCAM mask */
    delete_commands = _rdd_tcam_unreference_fields(table_id, rule_key_mask, gen_indexes);

    /* Rebuild and apply command table if needed */
    if (delete_commands)
    {
        /* Rebuild command table */
        _rdd_tcam_build_cmd_table(table_id);

        /* Activate command table */
        _rdd_tcam_activate_cmd_table(table_id);
    }

    return BDMF_ERR_OK;
}

/* Map RDPA offset type to RDD_IC source type */
static rdd_tcam_field_source_t _rdd_tcam_rdpa_offset_type_to_source_type(rdpa_offset_t offset_type)
{
    rdd_tcam_field_source_t src;

    switch (offset_type)
    {
    case RDPA_OFFSET_L2:
        src = RDD_TCAM_FLD_SOURCE_PACKET;
        break;
    case RDPA_OFFSET_L3:
        src = RDD_TCAM_FLD_SOURCE_L3_HDR;
        break;
    case RDPA_OFFSET_L4:
        src = RDD_TCAM_FLD_SOURCE_L4_HDR;
        break;
    default:
        BDMF_TRACE_ERR("Internal error. Can't map RDPA offset %d\n", (int)offset_type);
        src = RDD_TCAM_FLD_SOURCE_NONE;
    }

    return src;
}

/* Map RDD_IC source type to RDPA offset type */
static rdpa_offset_t _rdd_tcam_source_type_to_rdpa_offset_type(rdd_tcam_field_source_t src)
{
    rdpa_offset_t offset_type;

    switch (src)
    {
    case RDD_TCAM_FLD_SOURCE_PACKET:
        offset_type = RDPA_OFFSET_L2;
        break;
    case RDD_TCAM_FLD_SOURCE_L3_HDR:
        offset_type = RDPA_OFFSET_L3;
        break;
    case RDD_TCAM_FLD_SOURCE_L4_HDR:
        offset_type = RDPA_OFFSET_L4;
        break;
    default:
        BDMF_TRACE_ERR("Internal error. Can't map IC source %d\n", (int)src);
        offset_type = RDPA_OFFSET_L2;
    }

    return offset_type;
}


/* Set generic key configuration */
bdmf_error_t rdd_tcam_generic_key_set(
    rdd_tcam_table_id                   table_id,
    uint8_t                             gen_index,
    const rdpa_ic_gen_rule_cfg_t       *cfg)
{
    rdd_tcam_table_t *table = &rdd_tcam_tables[table_id];
    rdd_tcam_field_info_t *fld_info = &table->gen_fields[gen_index];
    int tcam_fld;

    RDD_BTRACE("table_id = %d, gen_index = %d, cfg = %p\n", table_id, gen_index, cfg);

    /* Parameter validation */
    if ((unsigned)table_id >= RDD_TCAM_MAX_TABLES ||
        gen_index >= RDD_TCAM_MAX_GEN_FIELDS      ||
        !cfg)
    {
        return BDMF_ERR_PARM;
    }

    RDD_TRACE("cfg = { type = %d, offset = %d, mask = 0x%x}\n", cfg->type, cfg->offset, cfg->mask);

    /* Check if table has already been created */
    if (!table->max_cmds)
    {
        return BDMF_ERR_NOENT;
    }

    /* Can't change configuration of generic field in use */
    tcam_fld = RDPA_IC_LAST_KEY + gen_index;
    if (table->all_fields_mask & (1 << tcam_fld))
    {
        if (fld_info->source != _rdd_tcam_rdpa_offset_type_to_source_type(cfg->type) ||
            fld_info->offset != cfg->offset                                          ||
            fld_info->mask   != cfg->mask                                            ||
            fld_info->size   != sizeof(uint32_t))
        {
            return BDMF_ERR_STATE;
        }
    }

    fld_info->source = _rdd_tcam_rdpa_offset_type_to_source_type(cfg->type);
    fld_info->offset = cfg->offset;
    fld_info->mask = cfg->mask;
    fld_info->size = sizeof(uint32_t);

    return BDMF_ERR_OK;
}


/* Get generic key configuration */
bdmf_error_t rdd_tcam_generic_key_get(
    rdd_tcam_table_id                   table_id,
    uint8_t                             gen_index,
    rdpa_ic_gen_rule_cfg_t             *cfg)
{
    rdd_tcam_table_t *table = &rdd_tcam_tables[table_id];
    rdd_tcam_field_info_t *fld_info = &table->gen_fields[gen_index];

    RDD_BTRACE("table_id = %d, gen_index = %d, cfg = %p\n", table_id, gen_index, cfg);

    /* Parameter validation */
    if ((unsigned)table_id >= RDD_TCAM_MAX_TABLES ||
        gen_index >= RDD_TCAM_MAX_GEN_FIELDS      ||
        !cfg)
    {
        return BDMF_ERR_PARM;
    }

    RDD_TRACE("cfg = { type = %d, offset = %d, mask = 0x%x}\n", cfg->type, cfg->offset, cfg->mask);

    if (!fld_info->size)
        return BDMF_ERR_NOENT;

    cfg->offset = fld_info->offset;
    cfg->mask = fld_info->mask;
    cfg->type = _rdd_tcam_source_type_to_rdpa_offset_type(fld_info->source);

    return BDMF_ERR_OK;
}

void rdd_tcam_ic_result_entry_compose(uint16_t index, const rdd_ic_context_t *ctx, uint8_t *entry)
{
    RDD_BTRACE("index = %d, ctx = %p, entry = %p\n", index, ctx, entry);
    RDD_TRACE("ctx values to be used = { priority = %d, tx_flow = %d, egress_port = %d, include_mcast = %d policer_id = %x }\n",
        ctx->priority, ctx->tx_flow, ctx->egress_port, ctx->include_mcast, (int)ctx->policer);
	rdd_ic_result_entry_compose(index, ctx, entry);
}

void rdd_ic_debug_mode_enable(bdmf_boolean enable)
{
    RDD_BTRACE("enable = %d\n", enable);

    GROUP_MWRITE_8(RDD_IC_DEBUG_COUNTER_MODE_ADDRESS_ARR, 0, enable);
}

void rdd_ic_mcast_enable(bdmf_boolean enable)
{
    RDD_BTRACE("enable = %d\n", enable);

#if !defined(BCM63158)
    GROUP_MWRITE_8(RDD_IC_MCAST_ENABLE_ADDRESS_ARR, 0, enable);
#endif
}
