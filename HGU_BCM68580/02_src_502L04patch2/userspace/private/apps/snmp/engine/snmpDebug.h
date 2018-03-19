/**********************************************************************************
** <:copyright-BRCM:2017:proprietary:standard
** 
**    Copyright (c) 2017 Broadcom 
**    All Rights Reserved
** 
**  This program is the proprietary software of Broadcom and/or its
**  licensors, and may only be used, duplicated, modified or distributed pursuant
**  to the terms and conditions of a separate, written license agreement executed
**  between you and Broadcom (an "Authorized License").  Except as set forth in
**  an Authorized License, Broadcom grants no license (express or implied), right
**  to use, or waiver of any kind with respect to the Software, and Broadcom
**  expressly reserves all rights in and to the Software and all intellectual
**  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
**  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
**  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
** 
**  Except as expressly set forth in the Authorized License,
** 
**  1. This program, including its structure, sequence and organization,
**     constitutes the valuable trade secrets of Broadcom, and you shall use
**     all reasonable efforts to protect the confidentiality thereof, and to
**     use this information only in connection with your use of Broadcom
**     integrated circuit products.
** 
**  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
**     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
**     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
**     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
**     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
**     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
**     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
**     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
**     PERFORMANCE OF THE SOFTWARE.
** 
**  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
**     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
**     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
**     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
**     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
**     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
**     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
**     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
**     LIMITED REMEDY.
** :>
***********************************************************************************/

#ifndef _SNMPDEBUG_H_
#define _SNMPDEBUG_H_

#include "cms_util.h"

#define BUILD_SNMP_TRANSPORT_DEBUG

/* level flag */
#define SNMP_DEBUG_LEVEL_SNMP             1
/* transport flag */
#define SNMP_DEBUG_LEVEL_TRANSPORT_UDP    1
#define SNMP_DEBUG_LEVEL_TRANSPORT_EOC    2
#define SNMP_DEBUG_LEVEL_TRANSPORT_AAL5   4
#define SNMP_DEBUG_LEVEL_TRANSPORT_ALL    (SNMP_DEBUG_LEVEL_TRANSPORT_UDP | \
                                           SNMP_DEBUG_LEVEL_TRANSPORT_EOC | \
                                           SNMP_DEBUG_LEVEL_TRANSPORT_AAL5)
/* mib flag */
#define SNMP_DEBUG_LEVEL_MIB_CPE          1

#define SNMP_DEBUG            cmsLog_debug
#define PRINT_ERROR           cmsLog_error

typedef struct {
   unsigned char level;
   unsigned char transport;
   unsigned char mib;
} SNMP_DEBUG_FLAG;

extern SNMP_DEBUG_FLAG snmpDebug;

#endif /* _SNMPDEBUG_H_ */
