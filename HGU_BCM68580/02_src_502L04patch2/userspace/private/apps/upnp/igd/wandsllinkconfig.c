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
 * $Id: wanpotslinkconfig.c,v 1.4.20.2 2003/10/31 21:31:35 mthawani Exp $
 */

#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "wandsllinkconfig.h"


#define GetModulationType		NotImplemented
#define SetDestinationAddress		NotImplemented
#define GetDestinationAddress		NotImplemented
#define SetATMEncapsulation		NotImplemented
#define GetATMEncapsulation		NotImplemented
#define GetFCSPreserved			NotImplemented
#define SetFCSPreserved			NotImplemented

#define GetDSLLinkInfo			DefaultAction
#define GetAutoConfig			DefaultAction

struct iftab {
    char *name;
    int  if_id;
};

struct iftab iftable[] = {
    { "atm", 1 },
    { "ptm", 2 },
    { "ipoa", 3 },
    { "ppp", 4 },
    { "ppp", 5 },
    { "atm", 6 },
    { "unconfigured", 7 },
    { NULL, 0 }
};

static char *PossibleLinkTypes_allowedValueList[] = { "EoA",
                                                      "EoA",
                                                      "IPoA",
                                                      "PPPoA",
                                                      "PPPoE",
                                                      "CIP",
                                                      "Unconfigured",
                                                      NULL };

static char *LinkStatus_allowedValueList[] = { "Down",
                                               "Up",
                                               "Initializing",
                                               "Unavailable",
                                               NULL };

static VarTemplate StateVariables[] = { 
    { "LinkType", "Unconfigured", VAR_STRING, 
      (allowedValue) { PossibleLinkTypes_allowedValueList } }, 
    { "LinkStatus", "Unavailable", VAR_EVENTED|VAR_STRING,
      (allowedValue) { LinkStatus_allowedValueList } }, 
    { "AutoConfig", "1", VAR_EVENTED|VAR_BOOL }, 
    { "ModulationType", "", VAR_STRING }, 
    { "DestinationAddress", "", VAR_STRING }, 
    { "ATMEncapsulation", "", VAR_STRING }, 
    { "FCSPreserved", "", VAR_BOOL }, 
    { 0 } 
};

#define VAR_LinkType		0
#define VAR_LinkStatus		1
#define VAR_AutoConfig		2
#define VAR_ModulationType	3
#define VAR_DestinationAddress	4
#define VAR_ATMEncapsulation	5
#define VAR_FCSPreserved	6


static int SetDSLLinkType(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
static int WANDSLLinkConfig_GetVar(struct Service *psvc, int varindex);

static Action _SetDSLLinkType = { 
    "SetDSLLinkType", SetDSLLinkType,
    (Param [])    {
            {"NewLinkType", VAR_LinkType, VAR_IN},
            { 0 }
        }
};

static Action _GetDSLLinkInfo = { 
    "GetDSLLinkInfo", GetDSLLinkInfo,
    (Param [])    {
            {"NewLinkType", VAR_LinkType, VAR_OUT},
            {"NewLinkStatus", VAR_LinkStatus, VAR_OUT},
            { 0 }
        }
};

static Action _GetAutoConfig = { 
    "GetAutoConfig", GetAutoConfig,
    (Param [])    {
            {"NewAutoConfig", VAR_AutoConfig, VAR_OUT},
            { 0 }
        }
};

static Action _GetModulationType = { 
    "GetModulationType", GetModulationType,
    (Param [])    {
            {"NewModulationType", VAR_ModulationType, VAR_OUT},
            { 0 }
        }
};

static Action _SetDestinationAddress = { 
    "SetDestinationAddress", SetDestinationAddress,
    (Param [])    {
            {"NewDestinationAddress", VAR_DestinationAddress, VAR_IN},
            { 0 }
        }
};

static Action _GetDestinationAddress = { 
    "GetDestinationAddress", GetDestinationAddress,
    (Param [])    {
            {"NewDestinationAddress", VAR_DestinationAddress, VAR_OUT},
            { 0 }
        }
};

static Action _SetATMEncapsulation = { 
    "SetATMEncapsulation", SetATMEncapsulation,
    (Param [])    {
            {"NewATMEncapsulation", VAR_ATMEncapsulation, VAR_IN},
            { 0 }
        }
};

static Action _GetATMEncapsulation = { 
    "GetATMEncapsulation", GetATMEncapsulation,
    (Param [])    {
            {"NewATMEncapsulation", VAR_ATMEncapsulation, VAR_OUT},
            { 0 }
        }
};

static Action _SetFCSPreserved = { 
    "SetFCSPreserved", SetFCSPreserved,
    (Param [])    {
            {"NewFCSPreserved", VAR_FCSPreserved, VAR_IN},
            { 0 }
        }
};

static Action _GetFCSPreserved = { 
    "GetFCSPreserved", GetFCSPreserved,
    (Param [])    {
            {"NewFCSPreserved", VAR_FCSPreserved, VAR_OUT},
            { 0 }
        }
};

static PAction Actions[] = {
    &_SetDSLLinkType,
    &_GetDSLLinkInfo,
    &_GetAutoConfig,
    &_GetModulationType,
    &_SetDestinationAddress,
    &_GetDestinationAddress,
    &_SetATMEncapsulation,
    &_GetATMEncapsulation,
    &_SetFCSPreserved,
    &_GetFCSPreserved,
    NULL
};


ServiceTemplate Template_WANDSLLinkConfig = {
    "WANDSLLinkConfig:1",
    NULL,
    WANDSLLinkConfig_GetVar,	/* state variable handler */
    NULL,  /* SVCXML */
    ARRAYSIZE(StateVariables)-1, StateVariables,
    Actions
};

static int ifname_to_id ( char *ifname )
{
    int id = 0;

    for ( ; (iftable[id].name != NULL); id++ ) {
        if ( strncasecmp ( iftable[id].name, ifname,
             strlen ( iftable[id].name ) ) == 0 ) {
            return ( iftable[id].if_id );
        }
    }
    return ( id - 1 );
}

static int SetDSLLinkType(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
    int strLen = 0;
    
    strLen = sizeof(psvc->vars[VAR_LinkType].value) - 1;
    strncpy(psvc->vars[VAR_LinkType].value, ac->params[0].value, strLen);
    psvc->vars[VAR_LinkType].value[strLen] = '\0';
    
    return TRUE;
}

static int WANDSLLinkConfig_GetVar(struct Service *psvc, int varindex)
{
    PWANDSLLinkConfigData data = psvc->opaque;
    struct StateVar *var;
    char *cur;
    int if_id;

    var = &(psvc->vars[varindex]);
    switch (varindex) {
    case VAR_LinkType:
	cur = g_wan_ifname;
	if_id = ifname_to_id ( cur );
        sprintf(var->value, "%s", PossibleLinkTypes_allowedValueList[if_id]);
        break;

    case VAR_LinkStatus:
	data->link_status = osl_link_status ( "/dev/bcmadsl0" );
        sprintf(var->value, "%s", LinkStatus_allowedValueList[data->link_status]);
        break;

    case VAR_AutoConfig:
        sprintf(var->value, "%d", 1);
        break;

    } /* end-switch */

    return TRUE;
}

