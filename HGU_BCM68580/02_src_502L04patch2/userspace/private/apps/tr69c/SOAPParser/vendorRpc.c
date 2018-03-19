#ifdef SUPPORT_TR69C_VENDOR_RPC

/* 
 * <:copyright-BRCM:2014:proprietary:standard 
 * 
 *    Copyright (c) 2014 Broadcom 
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

/*----------------------------------------------------------------------*
 * File Name  : vendorRpc.c
 *
 * Description: Vendor specific RPC routines 
 *----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h> 
#include <syslog.h>

#include "cms_mdm.h"
#include "inc/tr69cdefs.h" 
#include "nanoxml.h"
#include "RPCState_public.h"

void doGetCurrentDataModel(VendorRPCAction *a)
{
   char response[BUFLEN_256]={0};
   char dm[BUFLEN_32]={0};
   char cwmpPrefix[BUFLEN_32]={0};

   getCwmpPrefix(cwmpPrefix);
   sprintf(response,"<%s%s>\n",cwmpPrefix,VENDOR_GET_DATA_MODEL_INFO_RESPONSE);

#if defined(SUPPORT_DM_LEGACY98) || defined(SUPPORT_DM_HYBRID)
   strcat(response,"    <DataModelSupported>TR98</DataModelSupported>\n");
#elif defined(SUPPORT_DM_PURE181)
   strcat(response,"    <DataModelSupported>TR181</DataModelSupported>\n");
#elif defined(SUPPORT_DM_DETECT)
   strcat(response,"    <DataModelSupported>TR98,TR181</DataModelSupported>\n");
#endif

   /* also do the proper indentation to make things easier to read */
   if (cmsMdm_isDataModelDevice2())
   {
      strcat(response,"    <DataModelUsed>TR181</DataModelUsed>\n");
   }
   else
   {
      strcat(response,"    <DataModelUsed>TR98</DataModelUsed>\n");
   }
   sprintf(dm,"</%s%s>\n",cwmpPrefix,VENDOR_GET_DATA_MODEL_INFO_RESPONSE);
   strcat(response,dm);

   sendVendorResponseToAcs(response);
}  /* End of doGetCurrentDataModel */


#endif/* SUPPORT_TR69C_VENDOR_RPC */
