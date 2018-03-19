/***********************************************************************
 *
 *  Copyright (c) 2013  Broadcom Corporation
 *  All Rights Reserved
 *
 * <:label-BRCM:2013:proprietary:standard
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
 *
 * $Change: 160447 $
 ***********************************************************************/

#ifndef _IEEE1905_WLCFG_H_
#define _IEEE1905_WLCFG_H_

#if defined(WIRELESS)
#include "i5ctl.h"
#include "ieee1905_socket.h"
#include "ieee1905_message.h"
#include "ieee1905_tlv.h"

int i5ctlWlcfgHandler(i5_socket_type *psock,t_I5_API_WLCFG_MSG *pMsg);
unsigned int i5WlCfgAreMediaTypesCompatible(unsigned short mediaType1, unsigned short mediaType2);
unsigned short i5WlCfgFetchWirelessIfInfo(char const *ifname, unsigned char *pMediaInfo, int *pMediaLen,
                                          unsigned char *netTechOui, unsigned char *netTechVariant, unsigned char *netTechName, unsigned char *url, int sizeUrl);
char *i5WlcfgGetWlParentInterface(char const *ifname, char *wlParentIf);
int i5WlCfgGetWdsMacFromName(char const * ifname, char *macString, int maxLen);
void i5WlcfgApAutoconfigurationStart(const char *ifname);
void i5WlcfgApAutoconfigurationRenew(const char *ifname);
void i5WlcfgApAutoconfigurationStop(const char *ifname);
int i5WlcfgApAutoConfigProcessMessage( i5_message_type *pmsg, unsigned int freqband, unsigned char *pWscData, int wscDataLen );
int i5WlcfgApAutoconfigurationRenewProcess(i5_message_type * pmsg, unsigned int freqband);
void i5WlCfgInit(void);
void i5WlCfgDeInit(void);
#endif

#endif
