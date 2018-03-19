/*
 * Copyright (c) 2003-2012  Broadcom Corporation
 * All Rights Reserved
 *
 * <:label-BRCM:2012:proprietary:standard
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
 * $Id: wancommon.c,v 1.26.20.2 2003/10/31 21:31:35 mthawani Exp $
 */

#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "igd.h"

#include "bcmutils.h"

static int WANCommonInterfaceConfig_Init(PService psvc, service_state_t state);
static int WANCommonInterfaceConfig_GetVar(struct Service *psvc, int varindex);
//static void WANCommonInterface_UpdateStats(timer_t t, PService psvc);



#define GetCommonLinkProperties		DefaultAction

#define NewTotalBytesSent		DefaultAction
#define GetTotalBytesReceived		DefaultAction
#define GetTotalPacketsSent		DefaultAction
#define GetTotalPacketsReceived		DefaultAction
#define GetActiveConnection		DefaultAction
#define GetTotalBytesSent		DefaultAction

#define VAR_WANAccessType		0
#define VAR_Layer1UpstreamMaxBitRate		1
#define VAR_Layer1DownstreamMaxBitRate		2
#define VAR_PhysicalLinkStatus		3
#define VAR_EnabledForInternet		4
#define VAR_TotalBytesSent		5
#define VAR_TotalBytesReceived		6
#define VAR_TotalPacketsSent		7
#define VAR_TotalPacketsReceived		8

static char *WANAccessType_allowedValueList[] = { "DSL", "POTS", "Cable", "Ethernet", "Other", NULL };
static char *PhysicalLinkStatus_allowedValueList[] = { "Up", "Down", "Initializing", "Unavailable", NULL };


static VarTemplate StateVariables[] = {
    { "WANAccessType", "DSL", VAR_STRING|VAR_LIST,  (allowedValue) { WANAccessType_allowedValueList } },
    { "Layer1UpstreamMaxBitRate", "", VAR_ULONG },
    { "Layer1DownstreamMaxBitRate", "", VAR_ULONG },
    { "PhysicalLinkStatus", "", VAR_EVENTED|VAR_STRING|VAR_LIST,  (allowedValue) { PhysicalLinkStatus_allowedValueList } },
    { "EnabledForInternet", "1", VAR_EVENTED|VAR_BOOL },
    { "TotalBytesSent", "", VAR_ULONG },
    { "TotalBytesReceived", "", VAR_ULONG },
    { "TotalPacketsSent", "", VAR_ULONG },
    { "TotalPacketsReceived", "", VAR_ULONG },
    { NULL }
};


static Action _GetCommonLinkProperties = {
    "GetCommonLinkProperties", GetCommonLinkProperties,
   (Param []) {
       {"NewWANAccessType", VAR_WANAccessType, VAR_OUT},
       {"NewLayer1UpstreamMaxBitRate", VAR_Layer1UpstreamMaxBitRate, VAR_OUT},
       {"NewLayer1DownstreamMaxBitRate", VAR_Layer1DownstreamMaxBitRate, VAR_OUT},
       {"NewPhysicalLinkStatus", VAR_PhysicalLinkStatus, VAR_OUT},
       { 0 }
    }
};

static Action _GetTotalBytesReceived = { 
    "GetTotalBytesReceived", GetTotalBytesReceived,
        (Param []) {
            {"NewTotalBytesReceived", VAR_TotalBytesReceived, VAR_OUT},
            { 0 }
        }
};


static Action _GetTotalBytesSent = { 
    "GetTotalBytesSent", GetTotalBytesSent,
        (Param []) {
            {"NewTotalBytesSent", VAR_TotalBytesSent, VAR_OUT},
            { 0 }
        }
};

static Action _GetTotalPacketsReceived = { 
    "GetTotalPacketsReceived", GetTotalPacketsReceived,
        (Param []) {
            {"NewTotalPacketsReceived", VAR_TotalPacketsReceived, VAR_OUT},
            { 0 }
        }
};

static Action _GetTotalPacketsSent = { 
    "GetTotalPacketsSent", GetTotalPacketsSent,
        (Param []) {
            {"NewTotalPacketsSent", VAR_TotalPacketsSent, VAR_OUT},
            { 0 }
        }
};




static PAction Actions[] = {
    &_GetCommonLinkProperties,
    &_GetTotalBytesSent,
    &_GetTotalBytesReceived,
    &_GetTotalPacketsReceived,
    &_GetTotalPacketsSent,
    NULL
};

ServiceTemplate Template_WANCommonInterfaceConfig = {
    "WANCommonInterfaceConfig:1",
    WANCommonInterfaceConfig_Init,
    WANCommonInterfaceConfig_GetVar,
    NULL,   /* SVCXML */
    ARRAYSIZE(StateVariables)-1, StateVariables,
    Actions,
    0,
    "urn:upnp-org:serviceId:WANCommonInterfaceConfig."
};

static int WANCommonInterfaceConfig_Init(PService psvc, service_state_t state)
{
   PWANCommonPrivateData pdata;

   switch (state) {
      case SERVICE_CREATE:
         pdata = (PWANCommonPrivateData) cmsMem_alloc(sizeof(WANCommonPrivateData), ALLOC_ZEROIZE);
         if (pdata) {
            psvc->opaque = pdata;

            /*intialise state variables */
            WANCommonInterface_UpdateVars(psvc);
          
         /*Suresh:TODO resgister with ssk to get notification about
          *  eveneted variables of this service
          *  this is not needed for now as we restart upnp if any of evenetd
          *  variables are changed(just a co-incidence) 
          */

         }
         break;

      case SERVICE_DESTROY:
         pdata = (PWANCommonPrivateData) psvc->opaque;
         cmsMem_free(pdata);
         break;
   } /* end switch */
   return TRUE;
}


static int WANCommonInterfaceConfig_GetVar(struct Service *psvc, int varindex)
{
   static time_t then;
   time_t now;
   CmsRet ret;

   time(&now);/*to avoid too frequent queries to cms*/
   if (now != then) 
   {

      if ((ret = cmsLck_acquireLockWithTimeout(UPNP_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get lock, ret=%d", ret);
         return FALSE;
      } 	

      WANCommonInterface_UpdateVars(psvc);

      cmsLck_releaseLock();

      then = now;
/*    if ((psvc->flags & VAR_CHANGED) == VAR_CHANGED) 
	update_all_subscriptions(psvc);*/
   }

   return TRUE;
}

