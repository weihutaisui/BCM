/*
* <:copyright-BRCM:2016:proprietary:gpon
* 
*    Copyright (c) 2016 Broadcom 
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
* :>
*/

#include "gpon_common.h"
#include "gpon_rx_irq.h"
int ag_drv_gpon_rx_irq_grxisr_set(const gpon_rx_irq_grxisr *grxisr)
{
    uint32_t reg_grxisr=0;

#ifdef VALIDATE_PARMS
    if(!grxisr)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, 1);
        return 1;
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
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
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

    return 0;
}

int ag_drv_gpon_rx_irq_grxisr_get(gpon_rx_irq_grxisr *grxisr)
{
    uint32_t reg_grxisr=0;

#ifdef VALIDATE_PARMS
    if(!grxisr)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
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

    return 0;
}

int ag_drv_gpon_rx_irq_grxier_set(const gpon_rx_irq_grxier *grxier)
{
    uint32_t reg_grxier=0;

#ifdef VALIDATE_PARMS
    if(!grxier)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, 1);
        return 1;
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
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
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

    return 0;
}

int ag_drv_gpon_rx_irq_grxier_get(gpon_rx_irq_grxier *grxier)
{
    uint32_t reg_grxier=0;

#ifdef VALIDATE_PARMS
    if(!grxier)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
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

    return 0;
}

int ag_drv_gpon_rx_irq_grxitr_set(uint16_t ist)
{
    uint32_t reg_grxitr=0;

#ifdef VALIDATE_PARMS
    if((ist >= _9BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_grxitr = RU_FIELD_SET(0, GPON_RX_IRQ, GRXITR, IST, reg_grxitr, ist);

    RU_REG_WRITE(0, GPON_RX_IRQ, GRXITR, reg_grxitr);

    return 0;
}

int ag_drv_gpon_rx_irq_grxitr_get(uint16_t *ist)
{
    uint32_t reg_grxitr=0;

#ifdef VALIDATE_PARMS
    if(!ist)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_IRQ, GRXITR, reg_grxitr);

    *ist = RU_FIELD_GET(0, GPON_RX_IRQ, GRXITR, IST, reg_grxitr);

    return 0;
}

