/*
 *
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
 * $Id: layer3.c,v 1.6.20.2 2003/10/31 21:31:35 mthawani Exp $
 */

#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"

static int Layer3Forwarding_Init(PService, service_state_t);
static int Layer3_GetVar(struct Service *psvc, int varindex);
static int SetDefaultConnectionService(UFILE *, PService psvc, PAction ac, pvar_entry_t args, int nargs);

static PDevice find_dev_by_udn(const char *udn);
static PService find_svc_by_type(PDevice pdev, char *type);
static PService find_svc_by_name(PDevice pdev, const char *name);

#define GetDefaultConnectionService		DefaultAction

static VarTemplate StateVariables[] = { 
    { "DefaultConnectionService", "", VAR_EVENTED|VAR_STRING }, 
    { 0 } 
};

#define VAR_DefaultConnectionService	0


static Action _GetDefaultConnectionService = { 
    "GetDefaultConnectionService", GetDefaultConnectionService,
        (Param []) {
            {"NewDefaultConnectionService", VAR_DefaultConnectionService, VAR_OUT},
            { 0 }
        }
};

static Action _SetDefaultConnectionService = { 
    "SetDefaultConnectionService", SetDefaultConnectionService,
        (Param []) {
            {"NewDefaultConnectionService", VAR_DefaultConnectionService, VAR_IN},
            { 0 }
        }
};

static PAction Actions[] = {
    &_GetDefaultConnectionService,
    &_SetDefaultConnectionService,
    NULL
};


ServiceTemplate Template_Layer3Forwarding = {
    "Layer3Forwarding:1",
    Layer3Forwarding_Init, 
    Layer3_GetVar,
    NULL,   /* SVCXML */
    ARRAYSIZE(StateVariables)-1, StateVariables,
    Actions, 0,
    "urn:upnp-org:serviceId:Layer3Forwarding."

};

struct Layer3Forwarding {
    struct Service *default_svc;
};


static int Layer3Forwarding_Init(PService psvc, service_state_t state)
{
    struct Layer3Forwarding *pdata;

    switch (state) {
    case SERVICE_CREATE:
	pdata = (struct Layer3Forwarding *) cmsMem_alloc(sizeof(struct Layer3Forwarding), ALLOC_ZEROIZE);
	if (pdata) {
	    pdata->default_svc = NULL;
	    psvc->opaque = (void *) pdata;
	}
	break;
    case SERVICE_DESTROY:
	pdata = (struct Layer3Forwarding *) psvc->opaque;
	cmsMem_free(pdata);
	break;
    } /* end switch */

    return 0;
}


int Layer3_GetVar(struct Service *psvc, int varindex)
{
    struct StateVar *var;
    struct Layer3Forwarding *data = psvc->opaque;

    var = &(psvc->vars[varindex]);

    switch (varindex) {
    case VAR_DefaultConnectionService:
	if (data->default_svc != NULL) {
	    sprintf(var->value, "urn:schemas-upnp-org:service:%s,%s", data->default_svc->template->name, 
		    data->default_svc->device->udn);
	} else {
	    strcpy(var->value, "");
	}
	break;
    } /* end-switch */

    return TRUE;
}

static int SetDefaultConnectionService(UFILE *uclient, PService psvc, 
					PAction ac, pvar_entry_t args, int nargs)
{
    int success = TRUE;
    struct Layer3Forwarding *data = psvc->opaque;
    char *type, *udn;
    struct Service *csvc;
    struct Device *pdev;

    udn = ac->params[0].value;
    type = strsep(&udn, ",");
    
    if (udn == NULL || (pdev = find_dev_by_udn(udn)) == NULL) {
	soap_error(uclient, SOAP_INVALIDDEVICEUUID);
	success = FALSE;
    } else if (type == NULL || (csvc = find_svc_by_type(pdev, type)) == NULL) {
	soap_error(uclient, SOAP_INVALIDSERVICEID);
	success = FALSE;
    } else {
	data->default_svc = csvc;
	mark_changed(psvc, VAR_DefaultConnectionService);
    }

    return success;
}

static PDevice find_dev_by_udn(const char *udn)
{
    PDevice pdev = NULL;
    
    forall_devices(pdev) {
	if (strcmp(pdev->udn, udn) == 0) {
	    break;
	}
    }
    
    return pdev;
}


static PService find_svc_by_type(PDevice pdev, char *type)
{
    char *name = NULL, *p;
    PService psvc = NULL;

    p = rindex(type, ':');
    if (p != 0) {
	*p = '\0';
	p = rindex(type, ':');
	if (p != 0)
	    name = p+1;
    }

    if (name) 
	psvc = find_svc_by_name(pdev, name);

    return psvc;
}

static PService find_svc_by_name(PDevice pdev, const char *name)
{
    PService psvc;

    forall_services(pdev, psvc) {
	if (strcmp(psvc->template->name, name) == 0) {
	    break;
	}
    }

    return psvc;
}
