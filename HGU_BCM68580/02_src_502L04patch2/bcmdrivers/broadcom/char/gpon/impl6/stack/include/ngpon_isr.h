/*
* <:copyright-BRCM:2015:proprietary:gpon
* 
*    Copyright (c) 2015 Broadcom 
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

#ifndef NGPON_ISR_H_INCLUDED
#define NGPON_ISR_H_INCLUDED


#ifdef __cplusplus
extern "C"
{
#endif

#include "pon_sm_common.h"
#include "ngpon_ploam.h"
#include "ngpon_general.h"



/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/*   This function is the interrupt service routine (ISR) for the XGPON ONU   */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE ngpon_mac_isr(void * xi_ploam_message_ptr, PON_INTERRUPTS_MASK_DTE xi_interrupt_vector);

/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/* Clear Rx and Tx interrupts status                                          */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE clear_interrupts (void);

/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/*   Clear/Mask/Unmask Rogue ONU interrupts status                            */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE  ngpon_cfg_rogue_onu_interrupts (PON_ROGUE_ONU_TX_INT_CMD cmd, int rogue_onu_diff, 
                                               int rogue_onu_level) ;
/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/* Disable Rx and Tx interrupts                                               */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE disable_interrupts (bool rx, bool tx);

/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/* Enable Rx and Tx interrupts                                               */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE enable_interrupts (bool rx, bool tx);


/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/* Rx interrupts handler                                                      */
/*                                                                            */
/******************************************************************************/
void rx_int_handler(void);


/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/* Downstream PLOAMs handler                                                  */
/*                                                                            */
/******************************************************************************/
void ds_ploam_hanlder(NGPON_DS_PLOAM * ploam_ptr);

/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/* PMD handler                                                  */
/*                                                                            */
/******************************************************************************/
void pmd_handler(void);


/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/* Calculate OMCI MIC for DS direction                                        */
/*                                                                            */
/******************************************************************************/
uint32_t ngpon_calc_ds_omci_mic(uint8_t *buffer, uint32_t length);

/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/* Calculate OMCI MIC for US direction                                        */
/*                                                                            */
/******************************************************************************/
uint32_t ngpon_calc_us_omci_mic(uint8_t *buffer, uint32_t length);




#ifdef __cplusplus
}
#endif


#endif /* NGPON_ISR_H_INCLUDED */
