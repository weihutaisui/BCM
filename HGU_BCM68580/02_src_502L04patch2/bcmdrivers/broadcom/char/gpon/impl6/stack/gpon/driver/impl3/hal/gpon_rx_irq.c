/*
   Copyright (c) 2015 Broadcom Corporation
   All Rights Reserved

    <:label-BRCM:2015:DUAL/GPL:standard

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

#include "rdp_common.h"
#include "drivers_common_ag.h"
#include "gpon_rx_irq.h"
bdmf_error_t ag_drv_gpon_rx_irq_grxisr_set(const gpon_rx_irq_grxisr *grxisr)
{
    uint32_t reg_grxisr=0;

#ifdef VALIDATE_PARMS
    if(!grxisr)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((grxisr->rxfifo >= _1BITS_MAX_VAL_) ||
       (grxisr->tdmfifo >= _1BITS_MAX_VAL_) ||
       (grxisr->aes >= _1BITS_MAX_VAL_) ||
       (grxisr->accfifo >= _1BITS_MAX_VAL_) ||
       (grxisr->lofchng >= _1BITS_MAX_VAL_) ||
       (grxisr->lcdgchng >= _1BITS_MAX_VAL_) ||
       (grxisr->fecchng >= _1BITS_MAX_VAL_) ||
       (grxisr->tod >= _1BITS_MAX_VAL_) ||
       (grxisr->fwi >= _1BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_grxisr = RU_FIELD_SET(0, GPON_RX_IRQ, GRXISR, RXFIFO, reg_grxisr, grxisr->rxfifo);
    reg_grxisr = RU_FIELD_SET(0, GPON_RX_IRQ, GRXISR, TDMFIFO, reg_grxisr, grxisr->tdmfifo);
    reg_grxisr = RU_FIELD_SET(0, GPON_RX_IRQ, GRXISR, AES, reg_grxisr, grxisr->aes);
    reg_grxisr = RU_FIELD_SET(0, GPON_RX_IRQ, GRXISR, ACCFIFO, reg_grxisr, grxisr->accfifo);
    reg_grxisr = RU_FIELD_SET(0, GPON_RX_IRQ, GRXISR, LOFCHNG, reg_grxisr, grxisr->lofchng);
    reg_grxisr = RU_FIELD_SET(0, GPON_RX_IRQ, GRXISR, LCDGCHNG, reg_grxisr, grxisr->lcdgchng);
    reg_grxisr = RU_FIELD_SET(0, GPON_RX_IRQ, GRXISR, FECCHNG, reg_grxisr, grxisr->fecchng);
    reg_grxisr = RU_FIELD_SET(0, GPON_RX_IRQ, GRXISR, TOD, reg_grxisr, grxisr->tod);
    reg_grxisr = RU_FIELD_SET(0, GPON_RX_IRQ, GRXISR, FWI, reg_grxisr, grxisr->fwi);

    RU_REG_WRITE(0, GPON_RX_IRQ, GRXISR, reg_grxisr);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_irq_grxisr_get(gpon_rx_irq_grxisr *grxisr)
{
    uint32_t reg_grxisr=0;

#ifdef VALIDATE_PARMS
    if(!grxisr)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_IRQ, GRXISR, reg_grxisr);

    grxisr->rxfifo = RU_FIELD_GET(0, GPON_RX_IRQ, GRXISR, RXFIFO, reg_grxisr);
    grxisr->tdmfifo = RU_FIELD_GET(0, GPON_RX_IRQ, GRXISR, TDMFIFO, reg_grxisr);
    grxisr->aes = RU_FIELD_GET(0, GPON_RX_IRQ, GRXISR, AES, reg_grxisr);
    grxisr->accfifo = RU_FIELD_GET(0, GPON_RX_IRQ, GRXISR, ACCFIFO, reg_grxisr);
    grxisr->lofchng = RU_FIELD_GET(0, GPON_RX_IRQ, GRXISR, LOFCHNG, reg_grxisr);
    grxisr->lcdgchng = RU_FIELD_GET(0, GPON_RX_IRQ, GRXISR, LCDGCHNG, reg_grxisr);
    grxisr->fecchng = RU_FIELD_GET(0, GPON_RX_IRQ, GRXISR, FECCHNG, reg_grxisr);
    grxisr->tod = RU_FIELD_GET(0, GPON_RX_IRQ, GRXISR, TOD, reg_grxisr);
    grxisr->fwi = RU_FIELD_GET(0, GPON_RX_IRQ, GRXISR, FWI, reg_grxisr);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_irq_grxier_set(const gpon_rx_irq_grxier *grxier)
{
    uint32_t reg_grxier=0;

#ifdef VALIDATE_PARMS
    if(!grxier)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((grxier->rxfifo >= _1BITS_MAX_VAL_) ||
       (grxier->tdmfifo >= _1BITS_MAX_VAL_) ||
       (grxier->aes >= _1BITS_MAX_VAL_) ||
       (grxier->accfifo >= _1BITS_MAX_VAL_) ||
       (grxier->lofchng >= _1BITS_MAX_VAL_) ||
       (grxier->lcdgchng >= _1BITS_MAX_VAL_) ||
       (grxier->fecchng >= _1BITS_MAX_VAL_) ||
       (grxier->tod >= _1BITS_MAX_VAL_) ||
       (grxier->fwi >= _1BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_grxier = RU_FIELD_SET(0, GPON_RX_IRQ, GRXIER, RXFIFO, reg_grxier, grxier->rxfifo);
    reg_grxier = RU_FIELD_SET(0, GPON_RX_IRQ, GRXIER, TDMFIFO, reg_grxier, grxier->tdmfifo);
    reg_grxier = RU_FIELD_SET(0, GPON_RX_IRQ, GRXIER, AES, reg_grxier, grxier->aes);
    reg_grxier = RU_FIELD_SET(0, GPON_RX_IRQ, GRXIER, ACCFIFO, reg_grxier, grxier->accfifo);
    reg_grxier = RU_FIELD_SET(0, GPON_RX_IRQ, GRXIER, LOFCHNG, reg_grxier, grxier->lofchng);
    reg_grxier = RU_FIELD_SET(0, GPON_RX_IRQ, GRXIER, LCDGCHNG, reg_grxier, grxier->lcdgchng);
    reg_grxier = RU_FIELD_SET(0, GPON_RX_IRQ, GRXIER, FECCHNG, reg_grxier, grxier->fecchng);
    reg_grxier = RU_FIELD_SET(0, GPON_RX_IRQ, GRXIER, TOD, reg_grxier, grxier->tod);
    reg_grxier = RU_FIELD_SET(0, GPON_RX_IRQ, GRXIER, FWI, reg_grxier, grxier->fwi);

    RU_REG_WRITE(0, GPON_RX_IRQ, GRXIER, reg_grxier);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_irq_grxier_get(gpon_rx_irq_grxier *grxier)
{
    uint32_t reg_grxier=0;

#ifdef VALIDATE_PARMS
    if(!grxier)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_IRQ, GRXIER, reg_grxier);

    grxier->rxfifo = RU_FIELD_GET(0, GPON_RX_IRQ, GRXIER, RXFIFO, reg_grxier);
    grxier->tdmfifo = RU_FIELD_GET(0, GPON_RX_IRQ, GRXIER, TDMFIFO, reg_grxier);
    grxier->aes = RU_FIELD_GET(0, GPON_RX_IRQ, GRXIER, AES, reg_grxier);
    grxier->accfifo = RU_FIELD_GET(0, GPON_RX_IRQ, GRXIER, ACCFIFO, reg_grxier);
    grxier->lofchng = RU_FIELD_GET(0, GPON_RX_IRQ, GRXIER, LOFCHNG, reg_grxier);
    grxier->lcdgchng = RU_FIELD_GET(0, GPON_RX_IRQ, GRXIER, LCDGCHNG, reg_grxier);
    grxier->fecchng = RU_FIELD_GET(0, GPON_RX_IRQ, GRXIER, FECCHNG, reg_grxier);
    grxier->tod = RU_FIELD_GET(0, GPON_RX_IRQ, GRXIER, TOD, reg_grxier);
    grxier->fwi = RU_FIELD_GET(0, GPON_RX_IRQ, GRXIER, FWI, reg_grxier);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_irq_grxitr_set(uint16_t ist)
{
    uint32_t reg_grxitr=0;

#ifdef VALIDATE_PARMS
    if((ist >= _9BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_grxitr = RU_FIELD_SET(0, GPON_RX_IRQ, GRXITR, IST, reg_grxitr, ist);

    RU_REG_WRITE(0, GPON_RX_IRQ, GRXITR, reg_grxitr);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_irq_grxitr_get(uint16_t *ist)
{
    uint32_t reg_grxitr=0;

#ifdef VALIDATE_PARMS
    if(!ist)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_IRQ, GRXITR, reg_grxitr);

    *ist = RU_FIELD_GET(0, GPON_RX_IRQ, GRXITR, IST, reg_grxitr);

    return BDMF_ERR_OK;
}

#ifdef USE_BDMF_SHELL
enum
{
    BDMF_grxisr,
    BDMF_grxier,
    BDMF_grxitr,
};

typedef enum
{
    bdmf_address_grxisr,
    bdmf_address_grxier,
    bdmf_address_grxitr,
}
bdmf_address;

static int bcm_gpon_rx_irq_cli_set(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    case BDMF_grxisr:
    {
        gpon_rx_irq_grxisr grxisr = { .rxfifo=parm[1].value.unumber, .tdmfifo=parm[2].value.unumber, .aes=parm[3].value.unumber, .accfifo=parm[4].value.unumber, .lofchng=parm[5].value.unumber, .lcdgchng=parm[6].value.unumber, .fecchng=parm[7].value.unumber, .tod=parm[8].value.unumber, .fwi=parm[9].value.unumber};
        err = ag_drv_gpon_rx_irq_grxisr_set(&grxisr);
        break;
    }
    case BDMF_grxier:
    {
        gpon_rx_irq_grxier grxier = { .rxfifo=parm[1].value.unumber, .tdmfifo=parm[2].value.unumber, .aes=parm[3].value.unumber, .accfifo=parm[4].value.unumber, .lofchng=parm[5].value.unumber, .lcdgchng=parm[6].value.unumber, .fecchng=parm[7].value.unumber, .tod=parm[8].value.unumber, .fwi=parm[9].value.unumber};
        err = ag_drv_gpon_rx_irq_grxier_set(&grxier);
        break;
    }
    case BDMF_grxitr:
        err = ag_drv_gpon_rx_irq_grxitr_set(parm[1].value.unumber);
        break;
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_rx_irq_cli_get(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    case BDMF_grxisr:
    {
        gpon_rx_irq_grxisr grxisr;
        err = ag_drv_gpon_rx_irq_grxisr_get(&grxisr);
        bdmf_session_print(session, "rxfifo = %u = 0x%x\n", grxisr.rxfifo, grxisr.rxfifo);
        bdmf_session_print(session, "tdmfifo = %u = 0x%x\n", grxisr.tdmfifo, grxisr.tdmfifo);
        bdmf_session_print(session, "aes = %u = 0x%x\n", grxisr.aes, grxisr.aes);
        bdmf_session_print(session, "accfifo = %u = 0x%x\n", grxisr.accfifo, grxisr.accfifo);
        bdmf_session_print(session, "lofchng = %u = 0x%x\n", grxisr.lofchng, grxisr.lofchng);
        bdmf_session_print(session, "lcdgchng = %u = 0x%x\n", grxisr.lcdgchng, grxisr.lcdgchng);
        bdmf_session_print(session, "fecchng = %u = 0x%x\n", grxisr.fecchng, grxisr.fecchng);
        bdmf_session_print(session, "tod = %u = 0x%x\n", grxisr.tod, grxisr.tod);
        bdmf_session_print(session, "fwi = %u = 0x%x\n", grxisr.fwi, grxisr.fwi);
        break;
    }
    case BDMF_grxier:
    {
        gpon_rx_irq_grxier grxier;
        err = ag_drv_gpon_rx_irq_grxier_get(&grxier);
        bdmf_session_print(session, "rxfifo = %u = 0x%x\n", grxier.rxfifo, grxier.rxfifo);
        bdmf_session_print(session, "tdmfifo = %u = 0x%x\n", grxier.tdmfifo, grxier.tdmfifo);
        bdmf_session_print(session, "aes = %u = 0x%x\n", grxier.aes, grxier.aes);
        bdmf_session_print(session, "accfifo = %u = 0x%x\n", grxier.accfifo, grxier.accfifo);
        bdmf_session_print(session, "lofchng = %u = 0x%x\n", grxier.lofchng, grxier.lofchng);
        bdmf_session_print(session, "lcdgchng = %u = 0x%x\n", grxier.lcdgchng, grxier.lcdgchng);
        bdmf_session_print(session, "fecchng = %u = 0x%x\n", grxier.fecchng, grxier.fecchng);
        bdmf_session_print(session, "tod = %u = 0x%x\n", grxier.tod, grxier.tod);
        bdmf_session_print(session, "fwi = %u = 0x%x\n", grxier.fwi, grxier.fwi);
        break;
    }
    case BDMF_grxitr:
    {
        uint16_t ist;
        err = ag_drv_gpon_rx_irq_grxitr_get(&ist);
        bdmf_session_print(session, "ist = %u = 0x%x\n", ist, ist);
        break;
    }
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_rx_irq_cli_test(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_test_method m = parm[0].value.unumber;
    bdmf_error_t err = BDMF_ERR_OK;

    {
        gpon_rx_irq_grxisr grxisr = {.rxfifo=gtmv(m, 1), .tdmfifo=gtmv(m, 1), .aes=gtmv(m, 1), .accfifo=gtmv(m, 1), .lofchng=gtmv(m, 1), .lcdgchng=gtmv(m, 1), .fecchng=gtmv(m, 1), .tod=gtmv(m, 1), .fwi=gtmv(m, 1)};
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_irq_grxisr_set( %u %u %u %u %u %u %u %u %u)\n", grxisr.rxfifo, grxisr.tdmfifo, grxisr.aes, grxisr.accfifo, grxisr.lofchng, grxisr.lcdgchng, grxisr.fecchng, grxisr.tod, grxisr.fwi);
        if(!err) ag_drv_gpon_rx_irq_grxisr_set(&grxisr);
        if(!err) ag_drv_gpon_rx_irq_grxisr_get( &grxisr);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_irq_grxisr_get( %u %u %u %u %u %u %u %u %u)\n", grxisr.rxfifo, grxisr.tdmfifo, grxisr.aes, grxisr.accfifo, grxisr.lofchng, grxisr.lcdgchng, grxisr.fecchng, grxisr.tod, grxisr.fwi);
        if(err || grxisr.rxfifo!=gtmv(m, 1) || grxisr.tdmfifo!=gtmv(m, 1) || grxisr.aes!=gtmv(m, 1) || grxisr.accfifo!=gtmv(m, 1) || grxisr.lofchng!=gtmv(m, 1) || grxisr.lcdgchng!=gtmv(m, 1) || grxisr.fecchng!=gtmv(m, 1) || grxisr.tod!=gtmv(m, 1) || grxisr.fwi!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    {
        gpon_rx_irq_grxier grxier = {.rxfifo=gtmv(m, 1), .tdmfifo=gtmv(m, 1), .aes=gtmv(m, 1), .accfifo=gtmv(m, 1), .lofchng=gtmv(m, 1), .lcdgchng=gtmv(m, 1), .fecchng=gtmv(m, 1), .tod=gtmv(m, 1), .fwi=gtmv(m, 1)};
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_irq_grxier_set( %u %u %u %u %u %u %u %u %u)\n", grxier.rxfifo, grxier.tdmfifo, grxier.aes, grxier.accfifo, grxier.lofchng, grxier.lcdgchng, grxier.fecchng, grxier.tod, grxier.fwi);
        if(!err) ag_drv_gpon_rx_irq_grxier_set(&grxier);
        if(!err) ag_drv_gpon_rx_irq_grxier_get( &grxier);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_irq_grxier_get( %u %u %u %u %u %u %u %u %u)\n", grxier.rxfifo, grxier.tdmfifo, grxier.aes, grxier.accfifo, grxier.lofchng, grxier.lcdgchng, grxier.fecchng, grxier.tod, grxier.fwi);
        if(err || grxier.rxfifo!=gtmv(m, 1) || grxier.tdmfifo!=gtmv(m, 1) || grxier.aes!=gtmv(m, 1) || grxier.accfifo!=gtmv(m, 1) || grxier.lofchng!=gtmv(m, 1) || grxier.lcdgchng!=gtmv(m, 1) || grxier.fecchng!=gtmv(m, 1) || grxier.tod!=gtmv(m, 1) || grxier.fwi!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint16_t ist=gtmv(m, 9);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_irq_grxitr_set( %u)\n", ist);
        if(!err) ag_drv_gpon_rx_irq_grxitr_set(ist);
        if(!err) ag_drv_gpon_rx_irq_grxitr_get( &ist);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_irq_grxitr_get( %u)\n", ist);
        if(err || ist!=gtmv(m, 9))
            return err ? err : BDMF_ERR_IO;
    }
    return err;
}

static int bcm_gpon_rx_irq_cli_address(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    uint32_t i;
    uint32_t j;
    uint32_t index1_start=0;
    uint32_t index1_stop;
    uint32_t index2_start=0;
    uint32_t index2_stop;
    bdmfmon_cmd_parm_t * bdmf_parm;
    const ru_reg_rec * reg;
    const ru_block_rec * blk;
    const char * enum_string = bdmfmon_enum_parm_stringval(session, 0, parm[0].value.unumber);

    if(!enum_string)
        return BDMF_ERR_INTERNAL;

    switch (parm[0].value.unumber)
    {
    case bdmf_address_grxisr : reg = &RU_REG(GPON_RX_IRQ, GRXISR); blk = &RU_BLK(GPON_RX_IRQ); break;
    case bdmf_address_grxier : reg = &RU_REG(GPON_RX_IRQ, GRXIER); blk = &RU_BLK(GPON_RX_IRQ); break;
    case bdmf_address_grxitr : reg = &RU_REG(GPON_RX_IRQ, GRXITR); blk = &RU_BLK(GPON_RX_IRQ); break;
    default :
        return BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    if((bdmf_parm = bdmfmon_find_named_parm(session,"index1")))
    {
        index1_start = bdmf_parm->value.unumber;
        index1_stop = index1_start + 1;
    }
    else
        index1_stop = blk->addr_count;
    if((bdmf_parm = bdmfmon_find_named_parm(session,"index2")))
    {
        index2_start = bdmf_parm->value.unumber;
        index2_stop = index2_start + 1;
    }
    else
        index2_stop = reg->ram_count + 1;
    if(index1_stop > blk->addr_count)
    {
        bdmf_session_print(session, "index1 (%u) is out of range (%u).\n", index1_stop, blk->addr_count);
        return BDMF_ERR_RANGE;
    }
    if(index2_stop > (reg->ram_count + 1))
    {
        bdmf_session_print(session, "index2 (%u) is out of range (%u).\n", index2_stop, reg->ram_count + 1);
        return BDMF_ERR_RANGE;
    }
    if(reg->ram_count)
        for (i = index1_start; i < index1_stop; i++)
        {
            bdmf_session_print(session, "index1 = %u\n", i);
            for (j = index2_start; j < index2_stop; j++)
                bdmf_session_print(session, 	 "(%5u) 0x%08X\n", j, (uint32_t)((uint32_t*)(blk->addr[i] + reg->addr) + j));
        }
    else
        for (i = index1_start; i < index1_stop; i++)
            bdmf_session_print(session, "(%3u) 0x%16lX\n", i, blk->addr[i]+reg->addr);
    return 0;
}

bdmfmon_handle_t ag_drv_gpon_rx_irq_cli_init(bdmfmon_handle_t driver_dir)
{
    bdmfmon_handle_t dir;

    if ((dir = bdmfmon_dir_find(driver_dir, "gpon_rx_irq"))!=NULL)
        return dir;
    dir = bdmfmon_dir_add(driver_dir, "gpon_rx_irq", "gpon_rx_irq", BDMF_ACCESS_ADMIN, NULL);

    {
        static bdmfmon_cmd_parm_t set_grxisr[]={
            BDMFMON_MAKE_PARM("rxfifo", "rxfifo", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("tdmfifo", "tdmfifo", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("aes", "aes", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("accfifo", "accfifo", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("lofchng", "lofchng", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("lcdgchng", "lcdgchng", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("fecchng", "fecchng", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("tod", "tod", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("fwi", "fwi", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_grxier[]={
            BDMFMON_MAKE_PARM("rxfifo", "rxfifo", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("tdmfifo", "tdmfifo", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("aes", "aes", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("accfifo", "accfifo", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("lofchng", "lofchng", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("lcdgchng", "lcdgchng", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("fecchng", "fecchng", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("tod", "tod", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("fwi", "fwi", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_grxitr[]={
            BDMFMON_MAKE_PARM("ist", "ist", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="grxisr", .val=BDMF_grxisr, .parms=set_grxisr },
            { .name="grxier", .val=BDMF_grxier, .parms=set_grxier },
            { .name="grxitr", .val=BDMF_grxitr, .parms=set_grxitr },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "set", "set", bcm_gpon_rx_irq_cli_set,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_cmd_parm_t set_default[]={
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="grxisr", .val=BDMF_grxisr, .parms=set_default },
            { .name="grxier", .val=BDMF_grxier, .parms=set_default },
            { .name="grxitr", .val=BDMF_grxitr, .parms=set_default },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "get", "get", bcm_gpon_rx_irq_cli_get,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_test_method[] = {
            { .name="low" , .val=bdmf_test_method_low },
            { .name="mid" , .val=bdmf_test_method_mid },
            { .name="high" , .val=bdmf_test_method_high },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "test", "test", bcm_gpon_rx_irq_cli_test,
            BDMFMON_MAKE_PARM_ENUM("method", "low: 0000, mid: 1000, high: 1111", enum_table_test_method, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_address[] = {
            { .name="GRXISR" , .val=bdmf_address_grxisr },
            { .name="GRXIER" , .val=bdmf_address_grxier },
            { .name="GRXITR" , .val=bdmf_address_grxitr },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "address", "address", bcm_gpon_rx_irq_cli_address,
            BDMFMON_MAKE_PARM_ENUM("method", "method", enum_table_address, 0),
            BDMFMON_MAKE_PARM("index2", "onu_id/alloc_id/port_id/etc...", BDMFMON_PARM_NUMBER, BDMFMON_PARM_FLAG_OPTIONAL));
    }
    return dir;
}
#endif /* USE_BDMF_SHELL */

