/***********************************************************************
 *
 *  Copyright (c) 2006  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:standard

 This program is the proprietary software of Broadcom and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").  Except as set forth in
 an Authorized License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and Broadcom
 expressly reserves all rights in and to the Software and all intellectual
 property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use
    all reasonable efforts to protect the confidentiality thereof, and to
    use this information only in connection with your use of Broadcom
    integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
    PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
    LIMITED REMEDY.
:>
 *
 ************************************************************************/

#ifndef __SSK_TIME_H__
#define __SSK_TIME_H__


/*!\file ssk_time.h
 * \brief private header file for network time protocol utility fuctions that are used by ssk.
 *
 */

#include "cms_core.h"
#include "cms_msg.h"


/** Push this list of NTP servers that were dynamically obtained into the
 *  MDM.
 *
 *  @param ntpServerList (IN) one or more NTP servers separated by comma.
 */
void setNtpServers(const char *ntpServerList);
void setNtpServers_igd(const char *ntpServerList);
void setNtpServers_dev2(const char *ntpServerList);

#if defined(SUPPORT_DM_LEGACY98)
#define setNtpServers(v)           setNtpServers_igd((v))
#elif defined(SUPPORT_DM_HYBRID)
#define setNtpServers(v)           setNtpServers_igd((v))
#elif defined(SUPPORT_DM_PURE181)
#define setNtpServers(v)           setNtpServers_dev2((v))
#elif defined(SUPPORT_DM_DETECT)
#define setNtpServers(v)  (cmsMdm_isDataModelDevice2() ? \
                                      setNtpServers_dev2((v)) : \
                                      setNtpServers_igd((v)))
#endif




/** NTP client (ntpd or sntp) has sent ssk a status msg.  Push status into MDM.
 *
 * @param state (IN) new state reported by NTP client
 */
void processTimeStateChanged(UINT32 state);
void processTimeStateChanged_igd(UINT32 state);
void processTimeStateChanged_dev2(UINT32 state);

#if defined(SUPPORT_DM_LEGACY98)
#define processTimeStateChanged(v)     processTimeStateChanged_igd((v))
#elif defined(SUPPORT_DM_HYBRID)
#define processTimeStateChanged(v)     processTimeStateChanged_igd((v))
#elif defined(SUPPORT_DM_PURE181)
#define processTimeStateChanged(v)     processTimeStateChanged_dev2((v))
#elif defined(SUPPORT_DM_DETECT)
#define processTimeStateChanged(v)  (cmsMdm_isDataModelDevice2() ? \
                                      processTimeStateChanged_dev2((v)) : \
                                      processTimeStateChanged_igd((v)))
#endif




#endif /*__SSK_TIME_H__ */
