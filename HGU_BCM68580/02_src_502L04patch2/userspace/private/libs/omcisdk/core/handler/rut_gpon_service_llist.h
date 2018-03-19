/***********************************************************************
 *
 *  Copyright (c) 2002 Broadcom
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:omcid

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

#ifndef _OMCI_SERVICE_LLIST_H_
#define _OMCI_SERVICE_LLIST_H_

#include "os_defs.h"
#include "bcm_common_llist.h"


typedef struct {
    BCM_COMMON_DECLARE_LL_ENTRY ();
    OmciServiceMsgBody service;
} OMCI_SERVICE_ENTRY, *POMCI_SERVICE_ENTRY;

/**
 * Global OMCI Service Link List functions
 **/

// create service entry if service does not exist
CmsRet omci_service_create
    (const OmciServiceMsgBody *pService);

// delete service entry that has parameters matched with the given parameters
CmsRet omci_service_delete
    (const SINT32 pbits,
    const SINT32 vlanId,
    const char *ifName);

// delete service entry that has parameters matched with the given parameters
POMCI_SERVICE_ENTRY omci_service_get
    (const SINT32 pbits,
    const SINT32 vlanId,
    const char *ifName);

// return service entry that has parameters matched with the given parameters
// if startEntry is NULL then search from beginning,
// otherwise search from startEntry + 1
POMCI_SERVICE_ENTRY omci_service_getNext
    (const SINT32 pbits,
    const SINT32 vlanId,
    const char *ifName,
    const POMCI_SERVICE_ENTRY startEntry);

// check the existence of serviceId in the list
UBOOL8 omci_service_exist
    (const SINT32 pbits,
    const SINT32 vlanId,
    const char *ifName);

CmsRet omci_service_init(void);

void omci_service_exit(void);
UINT32 omci_service_num_get(void);

typedef CmsRet (*OMCI_SERVICE_DELETE_CALLBACK)(const OmciServiceMsgBody *serviceP);

UINT32 omci_service_delete_by_gemPort(UINT32 gemPort, OMCI_SERVICE_DELETE_CALLBACK cbFuncP);

void printAllOmciServiceRules(void);

#endif /* _OMCI_SERVICE_LLIST_H_ */
