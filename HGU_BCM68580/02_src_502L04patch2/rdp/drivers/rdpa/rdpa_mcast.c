/*
* <:copyright-BRCM:2013-2015:proprietary:standard
* 
*    Copyright (c) 2013-2015 Broadcom 
*    All Rights Reserved
* 
*  This program is the proprietary software of Broadcom and/or its
*  licensors, and may only be used, duplicated, modified or distributed pursuant
*  to the terms and conditions of a separate, written license agreement executed
*  between you and Broadcom (an "Authorized License").  Except as set forth in
*  an Authorized License, Broadcom grants no license (express or implied), right
*  to use, or waiver of any kind with respect to the Software, and Broadcom
*  expressly reserves all rights in and to the Software and all intellectual
*  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
*  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
*  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
* 
*  Except as expressly set forth in the Authorized License,
* 
*  1. This program, including its structure, sequence and organization,
*     constitutes the valuable trade secrets of Broadcom, and you shall use
*     all reasonable efforts to protect the confidentiality thereof, and to
*     use this information only in connection with your use of Broadcom
*     integrated circuit products.
* 
*  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
*     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
*     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
*     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
*     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
*     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
*     PERFORMANCE OF THE SOFTWARE.
* 
*  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
*     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
*     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
*     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
*     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
*     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
*     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
*     LIMITED REMEDY.
 :>
*/

/*
 * rdpa_mcast.c
 *
 *  Created on: April 30, 2013
 *      Author: mdemaria
 */

#include <bdmf_dev.h>
#include <rdd.h>
#ifdef XRDP
#include "rdpa_rdd_map.h"
#else
#include <rdd_ih_defs.h>
#endif
#include <rdpa_api.h>
#include "rdpa_int.h"
#include "rdpa_rdd_inline.h"
#include "rdpa_mcast_ex.h"
#include "rdp_mm.h"
#include "rdpa_egress_tm_inline.h"

#if defined(BDMF_SYSTEM_SIM)
uint32_t *sim_port_header_buffer_g = NULL;
#endif

/***************************************************************************
 * mcast object type
 **************************************************************************/

#if (RDPA_MCAST_MEM_INFO_MAX > 65535)
#error "RDPA_MCAST_MEM_INFO_MAX > 65535"
#endif

#define RDPA_MCAST_MEM_INFO_INVAL 0xFF

typedef struct {
    void *virt_p;
    bdmf_phys_addr_t phys_addr;
} mcast_drv_mem_info_t;

/* mcast object private data */
typedef struct {
    uint32_t num_flows;     /**< Number of configured Multicast flows */
    uint16_t flow_to_mem_info[RDPA_MCAST_MAX_FLOWS];
    mcast_drv_mem_info_t mem_info[RDPA_MCAST_MEM_INFO_MAX];
} mcast_drv_priv_t;

static struct bdmf_object *mcast_object;
static DEFINE_BDMF_FASTLOCK(mcast_lock);


/*
 * mcast memory management funtions
 */

#define RDPA_MCAST_MEM_INFO_DEBUG

static mcast_drv_mem_info_t *__alloc_mem_info(bdmf_index index)
{
    mcast_drv_priv_t *mcast = (mcast_drv_priv_t *)bdmf_obj_data(mcast_object);
    int i;

#if defined(RDPA_MCAST_MEM_INFO_DEBUG)
    BUG_ON(index >= RDPA_MCAST_MAX_FLOWS);
    BUG_ON(mcast->flow_to_mem_info[index] != RDPA_MCAST_MEM_INFO_INVAL);
#endif

    for (i = 0; i < RDPA_MCAST_MEM_INFO_MAX; ++i)
    {
        mcast_drv_mem_info_t *mem_info_p = &mcast->mem_info[i];

        if (mem_info_p->virt_p == NULL)
        {
            mem_info_p->virt_p = (void *)(~0);

            mcast->flow_to_mem_info[index] = i;

            BDMF_TRACE_DBG_OBJ(mcast_object, "index %lu -> mem_info %d\n", index, i);

            return mem_info_p;
        }
    }

#if defined(RDPA_MCAST_MEM_INFO_DEBUG)
    BUG_ON(1);
#endif

    return NULL;
}

static void __free_mem_info(bdmf_index index)
{
    mcast_drv_priv_t *mcast = (mcast_drv_priv_t *)bdmf_obj_data(mcast_object);

#if defined(RDPA_MCAST_MEM_INFO_DEBUG)
    BUG_ON(index >= RDPA_MCAST_MAX_FLOWS);
    BUG_ON(mcast->flow_to_mem_info[index] == RDPA_MCAST_MEM_INFO_INVAL);
    BUG_ON(mcast->flow_to_mem_info[index] >= RDPA_MCAST_MEM_INFO_MAX);
#endif

    BDMF_TRACE_DBG_OBJ(mcast_object, "index %lu -> mem_info %d\n", index, mcast->flow_to_mem_info[index]);

    mcast->mem_info[mcast->flow_to_mem_info[index]].virt_p = NULL;
    mcast->flow_to_mem_info[index] = RDPA_MCAST_MEM_INFO_INVAL;
}

static mcast_drv_mem_info_t *__get_mem_info(bdmf_index index)
{
    mcast_drv_priv_t *mcast = (mcast_drv_priv_t *)bdmf_obj_data(mcast_object);
    mcast_drv_mem_info_t *mem_info_p;

#if defined(RDPA_MCAST_MEM_INFO_DEBUG)
    BUG_ON(index >= RDPA_MCAST_MAX_FLOWS);
    BUG_ON(mcast->flow_to_mem_info[index] == RDPA_MCAST_MEM_INFO_INVAL);
    BUG_ON(mcast->flow_to_mem_info[index] >= RDPA_MCAST_MEM_INFO_MAX);
#endif

    mem_info_p = &mcast->mem_info[mcast->flow_to_mem_info[index]];

    BDMF_TRACE_DBG_OBJ(mcast_object, "index %lu -> mem_info %d: virt_p 0x%p, phys_addr 0x%llX\n",
                       index, mcast->flow_to_mem_info[index], mem_info_p->virt_p, (long long unsigned int)mem_info_p->phys_addr);

    return mem_info_p;
}


/*
 * mcast object callback funtions
 */

static int mcast_pre_init(struct bdmf_object *mo)
{
    return rdpa_mcast_pre_init_ex();
}

/** This optional callback is called at object init time
 * after initial attributes are set.
 * Its work is:
 * - make sure that all necessary attributes are set and make sense
 * - allocate dynamic resources if any
 * - assign object name if not done in pre_init
 * - finalise object creation
 * If function returns error code !=0, object creation is aborted
 */
static int mcast_post_init(struct bdmf_object *mo)
{
    mcast_drv_priv_t *mcast;
    int i;

    /* save pointer to the mcast object */
    mcast_object = mo;

    /* set object name */
    snprintf(mo->name, sizeof(mo->name), "mcast");

    /* Initialize Flow memory management structures */
    mcast = (mcast_drv_priv_t *)bdmf_obj_data(mcast_object);
    for (i = 0; i < RDPA_MCAST_MAX_FLOWS; ++i)
    {
        mcast->flow_to_mem_info[i] = RDPA_MCAST_MEM_INFO_INVAL;
    }

    return rdpa_mcast_post_init_ex();
}

static void mcast_destroy(struct bdmf_object *mo)
{
    rdpa_mcast_destroy_ex();

    mcast_object = NULL;
}

/** find mcast object */
static int mcast_get(struct bdmf_type *drv, struct bdmf_object *owner,
                     const char *discr, struct bdmf_object **pmo)
{
    if (mcast_object == NULL)
    {
        return BDMF_ERR_NOENT;
    }

    *pmo = mcast_object;

    return 0;
}

static uint8_t __rdpa_if_mask_to_rdd_vport_mask(uint32_t rdpa_port_mask)
{
    rdpa_ports ports = (rdpa_ports)rdpa_port_mask;
    uint32_t rdd_egress_port_vector = rdpa_ports_to_rdd_egress_port_vector(ports, 0);

    rdd_egress_port_vector >>= 1;

    return (uint8_t)rdd_egress_port_vector;
}

static uint32_t __rdd_vport_mask_to_rdpa_if_mask(uint8_t rdd_port_mask)
{
    uint32_t rdd_egress_port_vector = rdd_port_mask << 1;
    rdpa_ports ports = rdpa_rdd_egress_port_vector_to_ports(rdd_egress_port_vector, 0);

    return (uint32_t)ports;
}


/*
 * mcast attribute access
 */

static void __rdd_context_create(rdd_vport_id_t rdd_vport,
                                 rdpa_mcast_flow_t *rdpa_mcast_flow,
                                 rdd_mcast_flow_t *rdd_mcast_flow,
                                 rdd_mcast_port_header_buffer_t *rdd_port_header_buffer,
                                 bdmf_boolean is_new_flow,
                                 bdmf_boolean update_port_header)
{
    rdd_mcast_flow_context_t *rdd_mcast_context =
        RDD_MCAST_FLOW_CONTEXT_GET_EX(rdd_mcast_flow);

    if (rdd_vport >= RDD_MCAST_MAX_PORT_CONTEXT_ENTRIES)
    {
        BDMF_TRACE_ERR("Invalid rdd_vport %u\n", rdd_vport);

        return;
    }

    /* Data fill the Multicast RDD Context structure */

    if (is_new_flow)
    {
        memset(rdd_mcast_context, 0xFF, sizeof(rdd_mcast_flow_context_t));

        rdd_mcast_context->multicast_flag = 1;

        rdd_mcast_context->mcast_port_header_buffer_ptr = rdpa_mcast_flow->result.port_header_buffer_addr;

#ifndef WL4908
        rdd_mcast_context->command_list_length_64 = (rdpa_mcast_flow->result.l3_cmd_list_length + 7) >> 3;
#endif
        memcpy(rdd_mcast_context->l3_command_list,
               rdpa_mcast_flow->result.l3_cmd_list,
               RDPA_CMD_LIST_MCAST_L3_LIST_SIZE);
    }

    rdd_mcast_context->is_routed = (rdpa_mcast_flow->result.is_routed) ? 1 : 0;
    rdd_mcast_context->number_of_ports = rdpa_mcast_flow->result.number_of_ports;
    rdd_mcast_context->port_mask = __rdpa_if_mask_to_rdd_vport_mask(rdpa_mcast_flow->result.port_mask);
    rdd_mcast_context->mtu = rdpa_mcast_flow->result.mtu;
    rdd_mcast_context->is_tos_mangle = (rdpa_mcast_flow->result.is_tos_mangle) ? 1 : 0;
    rdd_mcast_context->tos = rdpa_mcast_flow->result.tos;
    rdd_mcast_context->wlan_mcast_clients = rdpa_mcast_flow->result.wlan_mcast_clients;
    rdd_mcast_context->wlan_mcast_index = rdpa_mcast_flow->result.wlan_mcast_fwd_table_index;

    {
        rdpa_mcast_port_context_t *rdpa_port_context;
        rdd_mcast_port_context_t *rdd_port_context = (rdd_mcast_port_context_t *)
            &rdd_mcast_context->port_context[rdd_vport];
        rdpa_if rdpa_port;
        int rc_id, priority, tc;
        rdpa_mcast_rdd_vport_to_rdpa_if_ex(rdd_vport, &rdpa_port);
        rdpa_port_context = &rdpa_mcast_flow->result.port_context[rdpa_port];
        rdd_port_context->state = rdpa_mcast_port_state_cmd_list;
        rdd_port_context->queue = rdpa_port_context->queue;
        rdd_port_context->lag_port = rdpa_port_context->lag_port;
        tc = rdpa_port_context->tc;
        _rdpa_egress_tm_lan_port_queue_to_rdd_tc_check(rdpa_port, rdpa_port_context->queue, &rc_id, &priority, &tc);
        rdd_port_context->is_high_prio = tc;
        rdd_port_context->l2_command_list_length = rdpa_port_context->l2_command_list_length;
        rdd_port_context->l2_header_length = rdpa_port_context->l2_header_length;
        rdd_port_context->l2_push = rdpa_port_context->l2_push;
        rdd_port_context->l2_offset = rdpa_port_context->l2_offset;

        if (update_port_header)
        {
            rdpa_mcast_rdd_port_header_buffer_set_ex(rdd_vport,
                                                     rdpa_port_context,
                                                     rdd_port_header_buffer,
                                                     rdd_port_context);
        }
    }
}

/* "flow" attribute "read" callback */
static int mcast_attr_flow_read(struct bdmf_object *mo, struct bdmf_attr *ad,
                                bdmf_index index, void *val, uint32_t size)
{
    rdpa_mcast_flow_t *rdpa_mcast_flow = (rdpa_mcast_flow_t *)val;
    rdd_mcast_flow_t rdd_mcast_flow;
    rdd_mcast_flow_context_t *rdd_mcast_context =
        RDD_MCAST_FLOW_CONTEXT_GET_EX(&rdd_mcast_flow);
    mcast_drv_mem_info_t *mem_info_p;
    int rc;
    rdd_vport_id_t rdd_vport;
    rdpa_if rdpa_port;

    if (size != sizeof(rdpa_mcast_flow_t))
    {
        BDMF_TRACE_ERR("%s() : Size difference <%d:%d,%d,%d>\n", __FUNCTION__,
            (int)size, (int)sizeof(rdpa_mcast_flow_t),
            (int)sizeof(rdpa_mcast_flow_key_t), (int)sizeof(rdpa_mcast_flow_result_t));
        return BDMF_ERR_NOENT;
    }

    /* Read the RDD flow context from RDD */
    rc = rdpa_mcast_rdd_context_get_ex(index, &rdd_mcast_flow);
    if (rc)
    {
        return rc;
    }

    /* Data fill the RDPA structure */
    memset(rdpa_mcast_flow, 0, sizeof(rdpa_mcast_flow_t));

    mem_info_p = __get_mem_info(index);

    rdpa_mcast_flow->result.port_mask = __rdd_vport_mask_to_rdpa_if_mask(rdd_mcast_context->port_mask);
    rdpa_mcast_flow->result.number_of_ports = rdd_mcast_context->number_of_ports;
    rdpa_mcast_flow->result.is_routed = rdd_mcast_context->is_routed;
    rdpa_mcast_flow->result.mtu = rdd_mcast_context->mtu;
    rdpa_mcast_flow->result.is_tos_mangle = rdd_mcast_context->is_tos_mangle;
    rdpa_mcast_flow->result.tos = rdd_mcast_context->tos;
    rdpa_mcast_flow->result.wlan_mcast_clients = rdd_mcast_context->wlan_mcast_clients;
    rdpa_mcast_flow->result.wlan_mcast_fwd_table_index = rdd_mcast_context->wlan_mcast_index;
    rdpa_mcast_flow->result.port_header_buffer_addr = rdd_mcast_context->mcast_port_header_buffer_ptr;
    rdpa_mcast_flow->result.port_header_buffer_virt = mem_info_p->virt_p;
#if defined(BDMF_SYSTEM_SIM)
    rdpa_mcast_flow->result.port_header_buffer_addr = (uint64_t)mem_info_p->phys_addr;
#endif

/* FIXME! */
#ifndef XRDP
#if defined(RDPA_MCAST_MEM_INFO_DEBUG)
    BUG_ON(rdpa_mcast_flow->result.port_header_buffer_addr != (uint64_t)mem_info_p->phys_addr);
#endif
#endif

    rdpa_mcast_flow->result.l3_cmd_list_length = rdd_mcast_context->command_list_length_64 << 3;
    memcpy(rdpa_mcast_flow->result.l3_cmd_list,
           rdd_mcast_context->l3_command_list,
           RDPA_CMD_LIST_MCAST_L3_LIST_SIZE);

    /* Only loop through the number of EMACs supported by the RDD 
     * Currently only LAN ports are supported but this should be modified when CPU/WLAN support is added.
     * Only 8 LAN ports supported 0-6 (ETH) + 7 (WLAN) */
    for (rdd_vport = 0; rdd_vport < RDD_MCAST_MAX_PORT_CONTEXT_ENTRIES; ++rdd_vport)
    {
        rdpa_mcast_port_context_t *rdpa_port_context;
        rdd_mcast_port_context_t *rdd_port_context = (rdd_mcast_port_context_t *)
            &rdd_mcast_context->port_context[rdd_vport];
        rdd_mcast_port_header_buffer_t *rdd_port_header_buffer = mem_info_p->virt_p;

        rdpa_mcast_rdd_vport_to_rdpa_if_ex(rdd_vport, &rdpa_port);
        if (rdpa_port == rdpa_if_none)
        {
            continue;
        }
        rdpa_port_context = &rdpa_mcast_flow->result.port_context[rdpa_port];

        rc = rdpa_mcast_rdd_port_header_buffer_get_ex(rdd_vport,
                                                      rdd_port_header_buffer,
                                                      rdpa_port_context,
                                                      rdd_port_context);
        if (rc)
        {
            return rc;
        }

        rdpa_port_context->state = rdd_port_context->state;
        rdpa_port_context->l2_command_list_length = rdd_port_context->l2_command_list_length;
        rdpa_port_context->l2_header_length = rdd_port_context->l2_header_length;
        rdpa_port_context->l2_push = rdd_port_context->l2_push;
        rdpa_port_context->l2_offset = rdd_port_context->l2_offset;
        rdpa_port_context->queue = rdd_port_context->queue;
        rdpa_port_context->lag_port = rdd_port_context->lag_port;
        rdpa_port_context->is_wred_high_prio = rdd_port_context->is_high_prio;
    }

    /* Read the RDD flow key from RDD */
    rc = rdpa_mcast_rdd_key_get_ex(index, rdpa_mcast_flow, &rdd_mcast_flow);

    return rc;
}

/* "flow" attribute write callback */
static int mcast_attr_flow_write(struct bdmf_object *mo, struct bdmf_attr *ad,
                                 bdmf_index index, const void *val, uint32_t size)
{
    rdpa_mcast_flow_t *rdpa_mcast_flow = (rdpa_mcast_flow_t *)val;
    rdd_mcast_flow_t rdd_mcast_flow;
    rdd_mcast_flow_context_t *rdd_mcast_context =
        RDD_MCAST_FLOW_CONTEXT_GET_EX(&rdd_mcast_flow);
    bdmf_boolean update_port_header = 1;
    uint8_t port_mask_mask = 0xFF;
    /* Though lan0 EMAC starts with enumeration value 1 but we take it from 0 */
    rdd_vport_id_t rdd_vport;
    /* This variable holds the RDPA port mask in RDD way. The name may be little weird */
    uint8_t rdpa_port_mask_as_rdd = 0; 
    int rc;

    if (mo->state != bdmf_state_active)
    {
        return BDMF_ERR_INVALID_OP;
    }
    if (size != sizeof(rdpa_mcast_flow_t))
    {
        BDMF_TRACE_ERR("%s() : Size difference <%d:%d,%d,%d>\n",
            __FUNCTION__, (int)size, (int)sizeof(rdpa_mcast_flow_t),
            (int)sizeof(rdpa_mcast_flow_key_t),
            (int)sizeof(rdpa_mcast_flow_result_t));
        return BDMF_ERR_NOENT;
    }

    /* Read the flow context from RDD */
    rc = rdpa_mcast_rdd_context_get_ex(index, &rdd_mcast_flow);
    if (rc)
    {
        return rc;
    }

    if (rdpa_mcast_flow->result.wlan_mcast_clients > rdd_mcast_context->wlan_mcast_clients)
    {
        /* Add WLAN Client */

        rdd_vport = RDD_WLAN0_VPORT - 1;  /* Notice the -1 here */
    }
    else if (rdpa_mcast_flow->result.wlan_mcast_clients < rdd_mcast_context->wlan_mcast_clients)
    {
        /* Remove WLAN Client */

        rdd_vport = RDD_WLAN0_VPORT - 1;  /* Notice the -1 here */

        update_port_header = 0;
    }
    else
    {
        /* Ethernet Clients */

        /* Convert the rdpa_port_mask to rdd for easier comparision later in the loop. */
        rdpa_port_mask_as_rdd = __rdpa_if_mask_to_rdd_vport_mask(rdpa_mcast_flow->result.port_mask);

        for (rdd_vport = 0; rdd_vport < RDD_MCAST_MAX_PORT_CONTEXT_ENTRIES-1; ++rdd_vport)
        {
            uint8_t rdd_port_mask = rdd_mcast_context->port_mask & (1 << rdd_vport); 
            uint8_t rdpa_port_mask = rdpa_port_mask_as_rdd & (1 << rdd_vport);

            port_mask_mask &= ~(1 << rdd_vport);

            if (rdd_port_mask ^ rdpa_port_mask)
            {
                if ((rdd_mcast_context->port_mask & port_mask_mask) !=
                    (rdpa_port_mask_as_rdd & port_mask_mask))
                {
                    BDMF_TRACE_ERR("Multiple port changes detected: "
                                   "flow <%lu>, old port_mask 0x%02X, new port_mask 0x%02X\n",
                                   index, rdd_mcast_context->port_mask, rdpa_port_mask_as_rdd);

                    return BDMF_ERR_INVALID_OP;
                }

                if (rdd_port_mask)
                {
                    /* Remove port from Multicast flow */
                    BDMF_TRACE_DBG_OBJ(mo, "Remove port <%u> from Multicast Flow <%lu>\n", rdd_vport, index);

                    /* The port header should not be updated on port removal to avoid race
                       conditions with Runner, which can also update it when the port is a member */
                    update_port_header = 0;
                }
                else
                {
                    /* Add port to Multicast flow */
                    BDMF_TRACE_DBG_OBJ(mo, "Add port <%u> to Multicast Flow <%lu>\n", rdd_vport, index);
                }

                break;
            }
        }
    }

    /* Prepare the connection context to configure in RDD */

    {
        mcast_drv_mem_info_t *mem_info_p = __get_mem_info(index);

        __rdd_context_create(rdd_vport, rdpa_mcast_flow, &rdd_mcast_flow, mem_info_p->virt_p, 0, update_port_header);
    }

    /* Modify the flow in RDD */
    rc = rdpa_mcast_rdd_context_modify_ex(index, &rdd_mcast_flow);

    return rc;
}

/* "flow" attribute add callback */
static int mcast_attr_flow_add(struct bdmf_object *mo, struct bdmf_attr *ad,
                               bdmf_index *index, const void *val, uint32_t size)
{
    mcast_drv_priv_t *mcast = (mcast_drv_priv_t *)bdmf_obj_data(mcast_object);
    rdpa_mcast_flow_t *rdpa_mcast_flow = (rdpa_mcast_flow_t *)val;
    rdd_mcast_flow_t rdd_mcast_flow = {};
    void *port_header_buffer = NULL;
    rdpa_if rdpa_if_port;
    rdd_vport_id_t rdd_vport;
    bdmf_phys_addr_t phys_addr;
    int rc;

    /* Allocate Port Header buffer entry */

    rdpa_if_port = ffs(rdpa_mcast_flow->result.port_mask) - 1;

    if (rdpa_if_port >= rdpa_if_max_mcast_port)
    {
        BDMF_TRACE_ERR("Port is out of range, port = %u\n", rdpa_if_port);

        return BDMF_ERR_RANGE;
    }

    rdpa_mcast_rdpa_if_to_rdd_vport_ex(rdpa_if_port, &rdd_vport);

    /* Allocate memory for the flow Port Header buffer */

    port_header_buffer = rdp_mm_aligned_alloc_atomic(sizeof(rdd_mcast_port_header_buffer_t), &phys_addr);
    if (port_header_buffer == NULL)
    {
        BDMF_TRACE_ERR("Could not allocate Port Header Buffer\n");

        return BDMF_ERR_NOMEM;
    }
    rdpa_mcast_flow->result.port_header_buffer_addr = (uint64_t)phys_addr;

#if defined(BDMF_SYSTEM_SIM)
    sim_port_header_buffer_g = port_header_buffer;
#endif

    BDMF_TRACE_DBG_OBJ(mo, "port_header_buffer %p, rdpa_mcast_flow->result.port_header_buffer_addr 0x%llx\n",
                       port_header_buffer, (long long unsigned int)rdpa_mcast_flow->result.port_header_buffer_addr);

    /* Initialize the Port Header buffer */

    memset(port_header_buffer, 0xFF, sizeof(rdd_mcast_port_header_buffer_t));

    /* Prepare the connection context to configure in RDD */
    __rdd_context_create(rdd_vport, rdpa_mcast_flow,
                         &rdd_mcast_flow, port_header_buffer, 1, 1);

    /* Prepare the connection key to configure in RDD */
    rdpa_mcast_rdd_key_create_ex(rdpa_mcast_flow, &rdd_mcast_flow);

    /* Add the Multicast flow to RDD */
    rc = rdpa_mcast_rdd_flow_add_ex(index, &rdd_mcast_flow);
    if (rc)
    {
        rdp_mm_aligned_free(port_header_buffer, sizeof(rdd_mcast_port_header_buffer_t));

        return rc;
    }

    /* Save flow memory allocation info */
    {
        mcast_drv_mem_info_t *mem_info_p = __alloc_mem_info(*index);

        mem_info_p->virt_p = port_header_buffer;
        mem_info_p->phys_addr = phys_addr;
    }

    bdmf_fastlock_lock(&mcast_lock);
    mcast->num_flows++;
    bdmf_fastlock_unlock(&mcast_lock);

    return 0;
}

/* "flow" attribute delete callback */
static int mcast_attr_flow_delete(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index)
{
    mcast_drv_priv_t *mcast = (mcast_drv_priv_t *)bdmf_obj_data(mcast_object);
    rdd_mcast_flow_t rdd_mcast_flow;
    mcast_drv_mem_info_t *mem_info_p;
    int rc;

    /* Read the RDD flow context from RDD */
    rc = rdpa_mcast_rdd_context_get_ex(index, &rdd_mcast_flow);
    if (rc)
    {
        return rc;
    }

    /* Free the flow Port header buffer */

    mem_info_p = __get_mem_info(index);

    rdp_mm_aligned_free(mem_info_p->virt_p, sizeof(rdd_mcast_port_header_buffer_t));

    __free_mem_info(index);

    /* Delete the flow from RDD */

    rc = rdpa_mcast_rdd_flow_delete_ex(index);
    if (rc)
    {
        return rc;
    }

    bdmf_fastlock_lock(&mcast_lock);
    mcast->num_flows--;
    bdmf_fastlock_unlock(&mcast_lock);

    return 0;
}

/* "flow" attribute find callback.
 * Updates *index, can update *val as well
 */
static int mcast_attr_flow_find(struct bdmf_object *mo, struct bdmf_attr *ad,
                                bdmf_index *index, void *val, uint32_t size)
{
    rdpa_mcast_flow_t *rdpa_mcast_flow = (rdpa_mcast_flow_t *)val;
    rdd_mcast_flow_t rdd_mcast_flow = {};
    int rc;

    rdpa_mcast_rdd_key_create_ex(rdpa_mcast_flow, &rdd_mcast_flow);

    rc = rdpa_mcast_rdd_flow_find_ex(index, &rdd_mcast_flow);

    return rc;
}

/* "flow_stats" attribute "read" callback */
static int mcast_attr_flow_stats_read(struct bdmf_object *mo, struct bdmf_attr *ad,
                                      bdmf_index index, void *val, uint32_t size)
{
    rdpa_stat_t *stat = (rdpa_stat_t *)val;
    rdd_mcast_flow_t rdd_mcast_flow;
    rdd_mcast_flow_context_t *rdd_mcast_context =
        RDD_MCAST_FLOW_CONTEXT_GET_EX(&rdd_mcast_flow);
    int rc;

    /* Read the flow stats from the RDD */
    rc = rdpa_mcast_rdd_flow_stats_get_ex(index, &rdd_mcast_flow);
    if (rc)
    {
        return rc;
    }

    stat->packets = rdd_mcast_context->flow_hits;
    stat->bytes = rdd_mcast_context->flow_bytes;

    return rc;
}

/*  ip_flow_key aggregate type */
struct bdmf_aggr_type mcast_flow_key_type = {
    .name = "mcast_flow_key", .struct_name = "rdpa_mcast_flow_key_t",
    .help = "Multicast Flow Key",
    .fields = (struct bdmf_attr[])
    {
        { .name = "src_ip", .help = "Source IPv4/IPv6 address", .size = sizeof(bdmf_ip_t),
            .type = bdmf_attr_ip_addr, .offset = offsetof(rdpa_mcast_flow_key_t, src_ip)
        },
        { .name = "dst_ip", .help = "Destination IPv4/IPv6 address", .size = sizeof(bdmf_ip_t),
            .type = bdmf_attr_ip_addr, .offset = offsetof(rdpa_mcast_flow_key_t, dst_ip)
        },
        { .name = "protocol", .help = "IP protocol", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_mcast_flow_key_t, protocol),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "num_vlan_tags", .help = "Number of VLAN Tags", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_mcast_flow_key_t, num_vlan_tags),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "outer_vlan_id", .help = "Outer VLAN ID (0xFFFF == *)", .size = sizeof(uint16_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_mcast_flow_key_t, outer_vlan_id),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "inner_vlan_id", .help = "Inner VLAN ID (0xFFFF == *)", .size = sizeof(uint16_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_mcast_flow_key_t, inner_vlan_id),
            .flags = BDMF_ATTR_UNSIGNED
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE(mcast_flow_key_type);

const bdmf_attr_enum_table_t rdpa_mcast_port_state_enum_table =
{
    .type_name = "rdpa_mcast_port_state", .help = "Port State",
    .values = {
        {"CMD", rdpa_mcast_port_state_cmd_list},
        {"HDR", rdpa_mcast_port_state_header},
        {NULL, 0}
    }
};

/*  ip_flow_key aggregate type */
struct bdmf_aggr_type mcast_port_context_type = {
    .name = "mcast_port_context", .struct_name = "rdpa_mcast_port_context_t",
    .help = "Multicast Port Context",
    .fields = (struct bdmf_attr[])
    {
        { .name = "state", .help = "Port State", .type = bdmf_attr_enum,
            .ts.enum_table = &rdpa_mcast_port_state_enum_table,
            .size = sizeof(rdpa_mcast_port_state_t), .offset = offsetof(rdpa_mcast_port_context_t, state)
        },
        { .name = "port_header", .help = "L2 Command List / L2 Header", .size = sizeof(rdpa_mcast_port_header_t),
            .type = bdmf_attr_buffer, .offset = offsetof(rdpa_mcast_port_context_t, port_header)
        },
        { .name = "l2_command_list_length", .help = "L2 Command List length", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_mcast_port_context_t, l2_command_list_length),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "l2_header_length", .help = "Tx L2 Header length", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_mcast_port_context_t, l2_header_length),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "l2_push", .help = "Tx L2 Header Push/Pull flag", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_mcast_port_context_t, l2_push),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "l2_offset", .help = "Tx L2 Header Offset", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_mcast_port_context_t, l2_offset),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "queue", .help = "Egress Queue", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_mcast_port_context_t, queue),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "lag_port", .help = "LAG Port", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_mcast_port_context_t, lag_port),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "is_wred_high_prio", .help = "1: High Priority for WRED; 0: Low Priority", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_mcast_port_context_t, is_wred_high_prio),
            .flags = BDMF_ATTR_UNSIGNED
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE(mcast_port_context_type);

/*  ip_flow_result aggregate type */
struct bdmf_aggr_type mcast_flow_result_type = 
{
    .name = "mcast_flow_result", .struct_name = "rdpa_mcast_flow_result_t",
    .help = "Multicast Flow Result",
    .size = sizeof(rdpa_mcast_flow_result_t),
    .fields = (struct bdmf_attr[])
    {
        {.name = "rdpa_ifs", .help = "Bit mask of ports that joined this flow", .size = sizeof(uint32_t),
            .type = bdmf_attr_enum_mask, .ts.enum_table = &rdpa_if_enum_table, 
            .offset = offsetof(rdpa_mcast_flow_result_t, port_mask)
        },
        { .name = "number_of_ports", .help = "Number of ports that joined this flow", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_mcast_flow_result_t, number_of_ports),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "is_routed", .help = "1: Routed Flow; 0: Bridged Flow", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_mcast_flow_result_t, is_routed),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "mtu", .help = "Egress Port MTU", .size = sizeof(uint16_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_mcast_flow_result_t, mtu),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "is_tos_mangle", .help = "1: Mangle ToS; 0: No Mangle ToS", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_mcast_flow_result_t, is_tos_mangle),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "tos", .help = "Rx ToS", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_mcast_flow_result_t, tos),
            .flags = BDMF_ATTR_UNSIGNED | BDMF_ATTR_HEX_FORMAT
        },
        { .name = "port_header_buffer_addr", .help = "Port Header Buffer Physical Address", .size = sizeof(uint64_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_mcast_flow_result_t, port_header_buffer_addr),
            .flags = BDMF_ATTR_UNSIGNED | BDMF_ATTR_HEX_FORMAT
        },
        { .name = "port_header_buffer_virt", .help = "Port Header Buffer Virtual Address", .size = sizeof(void *),
            .type = bdmf_attr_pointer, .offset = offsetof(rdpa_mcast_flow_result_t, port_header_buffer_virt),
            .flags = BDMF_ATTR_UNSIGNED | BDMF_ATTR_HEX_FORMAT
        },
        { .name = "wlan_mcast_clients", .help = "WLAN Multicast Forwarding Table Clients", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_mcast_flow_result_t, wlan_mcast_clients),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "wlan_mcast_fwd_table_index", .help = "WLAN Multicast Forwarding Table Index", .size = sizeof(bdmf_index),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_mcast_flow_result_t, wlan_mcast_fwd_table_index),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "l3_cmd_list_length", .help = "L3 Command List Length", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_mcast_flow_result_t, l3_cmd_list_length),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "l3_cmd_list", .help = "L3 Command List", .size = RDPA_CMD_LIST_MCAST_L3_LIST_SIZE,
            .type = bdmf_attr_buffer, .offset = offsetof(rdpa_mcast_flow_result_t, l3_cmd_list)
        },
        { .name = "lan0_context", .help = "Port Context", .type = bdmf_attr_aggregate,
            .ts.aggr_type_name = "mcast_port_context",
            .offset = offsetof(rdpa_mcast_flow_result_t, port_context[rdpa_if_lan0])
        },
        { .name = "lan1_context", .help = "Port Context", .type = bdmf_attr_aggregate,
            .ts.aggr_type_name = "mcast_port_context",
            .offset = offsetof(rdpa_mcast_flow_result_t, port_context[rdpa_if_lan1])
        },
        { .name = "lan2_context", .help = "Port Context", .type = bdmf_attr_aggregate,
            .ts.aggr_type_name = "mcast_port_context",
            .offset = offsetof(rdpa_mcast_flow_result_t, port_context[rdpa_if_lan2])
        },
        { .name = "lan3_context", .help = "Port Context", .type = bdmf_attr_aggregate,
            .ts.aggr_type_name = "mcast_port_context",
            .offset = offsetof(rdpa_mcast_flow_result_t, port_context[rdpa_if_lan3])
        },
        { .name = "lan4_context", .help = "Port Context", .type = bdmf_attr_aggregate,
            .ts.aggr_type_name = "mcast_port_context",
            .offset = offsetof(rdpa_mcast_flow_result_t, port_context[rdpa_if_lan4])
        },
        { .name = "lan5_context", .help = "Port Context", .type = bdmf_attr_aggregate,
            .ts.aggr_type_name = "mcast_port_context",
            .offset = offsetof(rdpa_mcast_flow_result_t, port_context[rdpa_if_lan5])
        },
        { .name = "lan6_context", .help = "Port Context", .type = bdmf_attr_aggregate,
            .ts.aggr_type_name = "mcast_port_context",
            .offset = offsetof(rdpa_mcast_flow_result_t, port_context[rdpa_if_lan6])
        },
        { .name = "lan7_context", .help = "WLAN Port Context", .type = bdmf_attr_aggregate,
            .ts.aggr_type_name = "mcast_port_context",
            .offset = offsetof(rdpa_mcast_flow_result_t, port_context[rdpa_if_lan7])
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE(mcast_flow_result_type);

/*  ip_flow_info aggregate type */
struct bdmf_aggr_type mcast_flow_type = {
    .name = "mcast_flow", .struct_name = "rdpa_mcast_flow_t",
    .help = "Multicast Flow (key+result)",
    .size = sizeof(rdpa_mcast_flow_t),
    .fields = (struct bdmf_attr[]) {
        { .name = "key", .help = "Multicast flow key", .type = bdmf_attr_aggregate,
            .ts.aggr_type_name = "mcast_flow_key", .offset = offsetof(rdpa_mcast_flow_t, key)
        },
        { .name = "result", .help = "Multicast flow result", .type = bdmf_attr_aggregate,
            .ts.aggr_type_name = "mcast_flow_result", .offset = offsetof(rdpa_mcast_flow_t, result)
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE(mcast_flow_type);

/* Object attribute descriptors */
static struct bdmf_attr mcast_attrs[] = {
    { .name = "nflows", .help = "Number of configured Multicast flows",
      .type = bdmf_attr_number, .flags = BDMF_ATTR_READ | BDMF_ATTR_CONFIG,
      .size = sizeof(uint32_t), .offset = offsetof(mcast_drv_priv_t, num_flows)
    },
    { .name = "flow", .help = "Multicast flow entry",
      .type = bdmf_attr_aggregate, .ts.aggr_type_name = "mcast_flow", .array_size = RDPA_MCAST_MAX_FLOWS,
      .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG | BDMF_ATTR_NOLOCK,
      .read = mcast_attr_flow_read, .write = mcast_attr_flow_write,
      .add = mcast_attr_flow_add, .del = mcast_attr_flow_delete,
      .find = mcast_attr_flow_find
    },
    { .name = "flow_stat", .help = "Multicast flow entry statistics",
      .type = bdmf_attr_aggregate, .ts.aggr_type_name = "rdpa_stat", .array_size = RDPA_MCAST_MAX_FLOWS,
      .flags = BDMF_ATTR_READ | BDMF_ATTR_STAT | BDMF_ATTR_NOLOCK,
      .read = mcast_attr_flow_stats_read
    },
    BDMF_ATTR_LAST
};


static int mcast_drv_init(struct bdmf_type *drv);
static void mcast_drv_exit(struct bdmf_type *drv);

struct bdmf_type mcast_drv = {
    .name = "mcast",
    .parent = "system",
    .description = "Multicast Flow Manager",
    .drv_init = mcast_drv_init,
    .drv_exit = mcast_drv_exit,
    .pre_init = mcast_pre_init,
    .post_init = mcast_post_init,
    .destroy = mcast_destroy,
    .get = mcast_get,
    .extra_size = sizeof(mcast_drv_priv_t),
    .aattr = mcast_attrs,
    .max_objs = 1,
};
DECLARE_BDMF_TYPE(rdpa_mcast, mcast_drv);

/* Init module. Cater for GPL layer */
static int mcast_drv_init(struct bdmf_type *drv)
{
    rdpa_mcast_flow_result_t dummy;

/* FIXME! */
#ifndef XRDP
    uint32_t rdd_mcast_l3_list_offset = offsetof(rdd_mcast_flow_context_t,
                                                 l3_command_list);

    if (RDPA_CMD_LIST_MCAST_L3_LIST_OFFSET != rdd_mcast_l3_list_offset)
    {
        BDMF_TRACE_ERR("MCAST_L3_LIST_OFFSET mismatch: RDPA %u, RDD %u",
                       RDPA_CMD_LIST_MCAST_L3_LIST_OFFSET, rdd_mcast_l3_list_offset);

        return BDMF_ERR_INTERNAL;
    }
#endif

#ifdef BDMF_DRIVER_GPL_LAYER
    f_rdpa_mcast_drv = rdpa_mcast_drv;
    f_rdpa_mcast_get = rdpa_mcast_get;
#endif
    /* Multicast flow has port_mask for rdpa_if that could include all lan, wlan and cpu port */
    BUG_ON((sizeof(dummy.port_mask)*8) < rdpa_if_max_mcast_port);

    return 0;
}

/* Exit module. Cater for GPL layer */
static void mcast_drv_exit(struct bdmf_type *drv)
{
#ifdef BDMF_DRIVER_GPL_LAYER
    f_rdpa_mcast_drv = NULL;
    f_rdpa_mcast_get = NULL;
#endif
}

/***************************************************************************
 * Functions declared in auto-generated header
 **************************************************************************/

/** Get mcast object by key
 * \return  Object handle or NULL if not found
 */
int rdpa_mcast_get(bdmf_object_handle *_obj_)
{
    if (mcast_object == NULL)
    {
        return BDMF_ERR_NOENT;
    }

    bdmf_get(mcast_object);

    *_obj_ = mcast_object;

    return 0;
}
