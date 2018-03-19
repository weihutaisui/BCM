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

#ifndef NGPON_TUNE_H_INCLUDED
#define NGPON_TUNE_H_INCLUDED


#include "pon_sm_common.h"
#include "ngpon_general.h"



/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/*   Tune Rx  channel at NGPON2 tunable TRX                                   */
/*                                                                            */
/*   Input:                                                                   */
/*      rx_channel = 0         -- set next channel                            */
/*                   1..4      -- set specific channel                        */
/******************************************************************************/
PON_ERROR_DTE ngpon_tune_rx(int rx_channel);

/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/* Tune Tx  channel at NGPON2 tunable TRX                                     */
/*                                                                            */
/*   Input:                                                                   */
/*      tx_channel = 0         -- set next channel                            */
/*                   1..4      -- set specific channel                        */
/******************************************************************************/
PON_ERROR_DTE ngpon_tune_tx(int tx_channel);

/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/*   Set max NGPON2 channels number                                           */
/*   Shall be called before the state machine attempts to tune                */
/*                                                                            */
/******************************************************************************/
void ngpon_tune_set_max_chn(uint8_t max_chn) ;
/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/* set password required for some NGPON2 TRXes                                */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE ngpon_tune_set_pass(void);

/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/* Get current DS channel number                                              */
/*                                                                            */
/******************************************************************************/
uint8_t ngpon_tune_get_current_rx_channel (void);


/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/* Get current US channel number                                              */
/*                                                                            */
/******************************************************************************/
uint8_t ngpon_tune_get_current_tx_channel (void);



#endif /* NGPON_TUNE_H_INCLUDED */
