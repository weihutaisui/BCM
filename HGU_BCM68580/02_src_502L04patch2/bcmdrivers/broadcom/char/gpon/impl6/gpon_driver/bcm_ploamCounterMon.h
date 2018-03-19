/* 
* <:copyright-BRCM:2007:proprietary:gpon
* 
*    Copyright (c) 2007 Broadcom 
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
#ifndef BCM_PLOAM_ERR_MON_H
#define BCM_PLOAM_ERR_MON_H

#include <bcmtypes.h>
#include "bcm_gponBasicDefs.h"
#include "bcm_ploam_api.h"

void bcm_ploamCounterMonInit(void);

#define BCM_PLOAM_COUNTER_MON_FLAG_ERRORS 1
#define BCM_PLOAM_COUNTER_MON_FLAG_TRAFFIC 2
#define BCM_PLOAM_COUNTER_MON_FLAG_OMCI 4
#define BCM_PLOAM_COUNTER_MON_FLAG_PLOAM 8
#define BCM_PLOAM_COUNTER_MON_FLAG_MCAST 16

/* 68xx GPON DS COUNTER IDs */
typedef enum
{
    BCM_GPON_MACDS_COUNTER_CERR_FEC = 0,  
    BCM_GPON_MACDS_COUNTER_UERR_FEC, 
    BCM_GPON_MACDS_COUNTER_BIP_ERROR, 
    BCM_GPON_MACDS_COUNTER_UERR_PLOAM,
    BCM_GPON_MACDS_COUNTER_NUM_FEC_CW,
    BCM_GPON_MACDS_COUNTER_CBYTE_FEC,
    BCM_GPON_MACDS_COUNTER_MAX
}BCM_GponMacDsCounterIds;

uint32_t bcm_ploamCounterMonGetLinkCounter(BCM_GponMacDsCounterIds counterId, int reset);
uint32_t bcm_ploamCounterMonGetFecCounter(BCM_Ploam_fecCounters* fec_counter);

uint32_t bcm_ploamIOCgetGemCounters( uint32_t reset, uint32_t counterId,
    BCM_Ploam_GemPortCounters * gemCounters);


#endif /*BCM_PLOAM_ERR_MON_H*/
