/***********************************************************************
 *
 *  Copyright (c) 2010 Broadcom
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
#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1

#include "owsvc_api.h"
#include "rut_gpon.h"
#include "rut_gpon_vlan.h"
#include "rut_gpon_service_llist.h"

/**
 * Local OMCI Service LList variables
 **/

static BCM_COMMON_DECLARE_LL(omciServiceLL);

#ifdef OMCI_SERVICE_DUMP
void omci_service_dump(void);
#endif

static UINT32 omci_service_num = 0;

/**
 * Local OMCI Service LList functions
 **/

static void freeOmciServiceEntry(POMCI_SERVICE_ENTRY omciServiceEntry)
{
    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> freeOmciServiceEntry, gemPortIndex=%d, gemPortId=%d, pbits=%d, vlanId=%d, ifName=%s\n",
                   omciServiceEntry->service.linkParams.gemPortIndex, omciServiceEntry->service.linkParams.portID,
                   omciServiceEntry->service.serviceParams.pbits, omciServiceEntry->service.serviceParams.vlanId,
                   omciServiceEntry->service.l2Ifname);

    CMSMEM_FREE_BUF_AND_NULL_PTR(omciServiceEntry);

}

static POMCI_SERVICE_ENTRY allocOmciServiceEntry
    (const OmciServiceMsgBody *pService)
{
    POMCI_SERVICE_ENTRY omciServiceEntry = NULL;

    omciServiceEntry = cmsMem_alloc(sizeof(OMCI_SERVICE_ENTRY), ALLOC_ZEROIZE);

    if (omciServiceEntry != NULL)
    {
        memcpy(&(omciServiceEntry->service), pService, sizeof(OmciServiceMsgBody));
    }

    return omciServiceEntry;
}

static void freeAllOmciServiceEntry(void)
{
    POMCI_SERVICE_ENTRY omciServiceEntry, nextOmciServiceEntry;

    omciServiceEntry = BCM_COMMON_LL_GET_HEAD(omciServiceLL);

    while (omciServiceEntry)
    {
        nextOmciServiceEntry = BCM_COMMON_LL_GET_NEXT(omciServiceEntry);
        BCM_COMMON_LL_REMOVE(&omciServiceLL, omciServiceEntry);
        freeOmciServiceEntry(omciServiceEntry);
        omciServiceEntry = nextOmciServiceEntry;
    };

    BCM_COMMON_LL_INIT(&omciServiceLL);
}

static POMCI_SERVICE_ENTRY findOmciServiceEntryExactMatch
    (const OmciServiceMsgBody *serviceBody)
{
    POMCI_SERVICE_ENTRY searchEntry;

    searchEntry = BCM_COMMON_LL_GET_HEAD(omciServiceLL);
    while (searchEntry)
    {
        if (memcmp(&searchEntry->service, serviceBody,
            sizeof(OmciServiceMsgBody)) == 0)
        {
            omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                "===> Identical entry exists, "
                "gemPortIndex=%d, gemPortId=%d, serviceType=%d, "
                "pbits=%d, vlanId=%d, status=%d, ifName=%s\n",
                searchEntry->service.linkParams.gemPortIndex,
                searchEntry->service.linkParams.portID,
                searchEntry->service.linkParams.serviceType,
                searchEntry->service.serviceParams.pbits,
                searchEntry->service.serviceParams.vlanId,
                searchEntry->service.serviceParams.serviceStatus,
                searchEntry->service.l2Ifname);
            break;
        }
        searchEntry = BCM_COMMON_LL_GET_NEXT(searchEntry);
    }

    return searchEntry;
}

static POMCI_SERVICE_ENTRY findNextOmciServiceEntry
    (const SINT32 pbits,
    const SINT32 vlanId,
    const char *ifName,
    const POMCI_SERVICE_ENTRY startEntry)
{
    POMCI_SERVICE_ENTRY omciServiceEntry;
    UBOOL8 found = FALSE;

    if (ifName == NULL)
        return NULL;

    if (startEntry != NULL)
    {
        // move pointer to start entry + 1
        omciServiceEntry = BCM_COMMON_LL_GET_NEXT(startEntry);
    }
    else
    {
        // move pointer to head
        omciServiceEntry = BCM_COMMON_LL_GET_HEAD(omciServiceLL);
    }

    while (omciServiceEntry)
    {
        if (pbits == OMCI_FILTER_DONT_CARE && vlanId == OMCI_FILTER_DONT_CARE)
            found = (strcmp(omciServiceEntry->service.l2Ifname, ifName) == 0);
        else if (pbits == OMCI_FILTER_DONT_CARE)
            found = ((omciServiceEntry->service.serviceParams.vlanId == vlanId) &&
                (strcmp(omciServiceEntry->service.l2Ifname, ifName) == 0));
        else if (vlanId == OMCI_FILTER_DONT_CARE)
            found = ((omciServiceEntry->service.serviceParams.pbits == pbits) &&
                (strcmp(omciServiceEntry->service.l2Ifname, ifName) == 0));
        else
            found = ((omciServiceEntry->service.serviceParams.pbits == pbits) &&
                (omciServiceEntry->service.serviceParams.vlanId == vlanId) &&
                (strcmp(omciServiceEntry->service.l2Ifname, ifName) == 0));

        if (found == TRUE)
        {
/*
omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
               "===> findNextOmciServiceEntry, pbits=%d, vlanId=%d, ifName=%s is FOUND\n",
               omciServiceEntry->service.pbits, omciServiceEntry->service.vlanId, ifName);
*/
            break;
        }
        omciServiceEntry = BCM_COMMON_LL_GET_NEXT(omciServiceEntry);
    }

    return omciServiceEntry;
}

static POMCI_SERVICE_ENTRY findOmciServiceEntry(
    const SINT32 pbits,
    const SINT32 vlanId,
    const char *ifName)
{
    return findNextOmciServiceEntry(pbits, vlanId, ifName, NULL);
}

/**
 * Global OMCI Service LList functions
 **/

CmsRet omci_service_create
    (const OmciServiceMsgBody *pService)
{
    CmsRet ret = CMSRET_SUCCESS;
    POMCI_SERVICE_ENTRY omciServiceEntry;
    POMCI_SERVICE_ENTRY searchEntry;

    searchEntry = findOmciServiceEntryExactMatch(pService);
    if (searchEntry != NULL)
    {
        cmsLog_notice("Identical entry exists, pbits=%d, vlanId=%d, ifName=%s",
                      pService->serviceParams.pbits,
                      pService->serviceParams.vlanId,
                      pService->l2Ifname);
#ifdef OMCI_SERVICE_DUMP
        omci_service_dump();
#endif
        return ret;
    }

    omciServiceEntry = allocOmciServiceEntry(pService);

    /* allocate a new OMCI Service entry */

    if (omciServiceEntry != NULL)
    {
        cmsLog_notice("Create OMCI Service Entry for pbits=%d, vlanId=%d, ifName=%s",
                      pService->serviceParams.pbits, pService->serviceParams.vlanId, pService->l2Ifname);
        BCM_COMMON_LL_APPEND(&omciServiceLL, omciServiceEntry);
        omci_service_num++;
    }
    else
    {
        cmsLog_error("Could not allocate OMCI Service Entry memory for pbits=%d, vlanId=%d, ifName=%s",
                     pService->serviceParams.pbits, pService->serviceParams.vlanId, pService->l2Ifname);
        ret = CMSRET_RESOURCE_EXCEEDED;
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> omci_service_create, gemPortIndex=%d, gemPortId=%d, pbits=%d, vlanId=%d, ifName=%s, ret=%d\n",
                   pService->linkParams.gemPortIndex, pService->linkParams.portID,
                   pService->serviceParams.pbits, pService->serviceParams.vlanId, pService->l2Ifname, ret);

#ifdef OMCI_SERVICE_DUMP
    omci_service_dump();
#endif

    return ret;
}

CmsRet omci_service_delete
    (const SINT32 pbits, const SINT32 vlanId, const char *ifName)
{
    CmsRet ret = CMSRET_SUCCESS;
    POMCI_SERVICE_ENTRY omciServiceEntry = findOmciServiceEntry(pbits, vlanId, ifName);

    if (omciServiceEntry != NULL)
    {
        cmsLog_notice("Freeing OMCI Service Entry for pbits=%d, vlanId=%d, ifName=%s", pbits, vlanId, ifName);
        BCM_COMMON_LL_REMOVE(&omciServiceLL, omciServiceEntry);
        freeOmciServiceEntry(omciServiceEntry);
        omci_service_num--;
    }
    else
    {
        cmsLog_notice("Could not find OMCI Service Entry for pbits=%d, vlanId=%d, ifName=%s", pbits, vlanId, ifName);
        ret = CMSRET_OBJECT_NOT_FOUND;
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> omci_service_delete, pbits=%d, vlanId=%d, ifName=%s, ret=%d\n",
                   pbits, vlanId, ifName, ret);

#ifdef OMCI_SERVICE_DUMP
    omci_service_dump();
#endif

    return ret;
}

POMCI_SERVICE_ENTRY omci_service_get
    (const SINT32 pbits,
    const SINT32 vlanId,
    const char *ifName)
{
#ifdef OMCI_DEBUG_SUPPORT
    CmsRet ret = CMSRET_SUCCESS;
#endif
    POMCI_SERVICE_ENTRY omciServiceEntry = findOmciServiceEntry(pbits, vlanId, ifName);

    if (omciServiceEntry == NULL)
    {
        cmsLog_notice("Could not find OMCI Service Entry for pbits=%d, vlanId=%d, ifName=%s", pbits, vlanId, ifName);
#ifdef OMCI_DEBUG_SUPPORT
        ret = CMSRET_OBJECT_NOT_FOUND;
#endif
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> omci_service_get, pbits=%d, vlanId=%d, ifName=%s, ret=%d\n",
                   pbits, vlanId, ifName, ret);

    return omciServiceEntry;
}

POMCI_SERVICE_ENTRY omci_service_getNext
    (const SINT32 pbits,
    const SINT32 vlanId,
    const char *ifName,
    const POMCI_SERVICE_ENTRY startEntry)
{
#ifdef OMCI_DEBUG_SUPPORT
    CmsRet ret = CMSRET_SUCCESS;
#endif
    POMCI_SERVICE_ENTRY omciServiceEntry = findNextOmciServiceEntry(pbits, vlanId, ifName, startEntry);

    if (omciServiceEntry == NULL)
    {
        cmsLog_notice("Could not find OMCI Service Entry for pbits=%d, vlanId=%d, ifName=%s", pbits, vlanId, ifName);
#ifdef OMCI_DEBUG_SUPPORT
        ret = CMSRET_OBJECT_NOT_FOUND;
#endif
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> omci_service_getNext, pbits=%d, vlanId=%d, ifName=%s, ret=%d\n",
                   pbits, vlanId, ifName, ret);

    return omciServiceEntry;
}

UBOOL8 omci_service_exist
    (const SINT32 pbits, const SINT32 vlanId, const char *ifName)
{
    UBOOL8 found = FALSE;

    if (findOmciServiceEntry(pbits, vlanId, ifName) != NULL)
        found = TRUE;

    return found;
}

CmsRet omci_service_init(void)
{
    CmsRet ret = CMSRET_SUCCESS;

    BCM_COMMON_LL_INIT(&omciServiceLL);
    omci_service_num = 0;

    return ret;
}

void omci_service_exit(void)
{
    // free all OMCI flows memory
    freeAllOmciServiceEntry();
    omci_service_num = 0;
}

UINT32 omci_service_delete_by_gemPort(UINT32 gemPort, OMCI_SERVICE_DELETE_CALLBACK cbFuncP)
{
    POMCI_SERVICE_ENTRY omciServiceEntry = BCM_COMMON_LL_GET_HEAD(omciServiceLL);
    POMCI_SERVICE_ENTRY tmpEntry = NULL;
    UINT32 count = 0;

    while (omciServiceEntry)
    {
        if (omciServiceEntry->service.linkParams.portID == gemPort)
        {
            omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
              "\n===> omci_service_delete_by_gemPort: "
              "gemIdx[%d], gemPort[%d], pbits[%d], vlanId[%d]\n\n",
              omciServiceEntry->service.linkParams.gemPortIndex,
              omciServiceEntry->service.linkParams.portID,
              omciServiceEntry->service.serviceParams.pbits,
              omciServiceEntry->service.serviceParams.vlanId);

            omciServiceEntry->service.serviceParams.serviceStatus = FALSE;
            tmpEntry = omciServiceEntry;
            omciServiceEntry = BCM_COMMON_LL_GET_NEXT(omciServiceEntry);

            if (cbFuncP != NULL)
            {
                cbFuncP(&tmpEntry->service);
            }

            /* Free the found omciServiceEntry */
            BCM_COMMON_LL_REMOVE(&omciServiceLL, tmpEntry);
            freeOmciServiceEntry(tmpEntry);
            count++;
        }
        else
        {
            omciServiceEntry = BCM_COMMON_LL_GET_NEXT(omciServiceEntry);
        }
    }

    omci_service_num -= count;
    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
      "===> omci_service_delete_by_gemPort, gemPort=%d, count=%d, left=%d\n",
      gemPort, count, omci_service_num_get());

    return count;
}


UINT32 omci_service_num_get(void)
{
   return omci_service_num;
}


void printAllOmciServiceRules(void)
{
    POMCI_SERVICE_ENTRY searchEntry = BCM_COMMON_LL_GET_HEAD(omciServiceLL);
    UINT32 i = 0;

    if (searchEntry == NULL)
    {
        return;
    }

    printf("\nOMCI ONU-RG service list:\n\n");
    printf("   # Ifname       Status GemIdx  PortID  SvcType VID  PBIT\n");
    printf("==== ============ ====== ======= ======= ======= ==== ====\n");
    while (searchEntry)
    {
        i++;
        printf("%-4d %-12s %6d %7d %7d %7d %4d %4d\n",
          i, searchEntry->service.l2Ifname,
          searchEntry->service.serviceParams.serviceStatus,
          searchEntry->service.linkParams.gemPortIndex,
          searchEntry->service.linkParams.portID,
          searchEntry->service.linkParams.serviceType,
          searchEntry->service.serviceParams.vlanId,
          searchEntry->service.serviceParams.pbits);
        searchEntry = BCM_COMMON_LL_GET_NEXT(searchEntry);
    };
    printf("==== ============ ====== ======= ======= ======= ==== ====\n");
}

#ifdef OMCI_SERVICE_DUMP
void omci_service_dump(void)
{
    UINT32 totalNum = 0;
    POMCI_SERVICE_ENTRY searchEntry = BCM_COMMON_LL_GET_HEAD(omciServiceLL);

    while (searchEntry)
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
            "===> omci_service_dump, gemPortIndex=%d, gemPortId=%d, serviceType=%d, "
            "pbits=%d, vlanId=%d, status=%d, ifName=%s\n",
            searchEntry->service.linkParams.gemPortIndex,
            searchEntry->service.linkParams.portID,
            searchEntry->service.linkParams.serviceType,
            searchEntry->service.serviceParams.pbits,
            searchEntry->service.serviceParams.vlanId,
            searchEntry->service.serviceParams.serviceStatus,
            searchEntry->service.l2Ifname);
        totalNum++;
        searchEntry = BCM_COMMON_LL_GET_NEXT(searchEntry);
    };

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
        "===> omci_service_dump, total num=%d\n", totalNum);
}
#endif /* OMCI_SERVICE_DUMP */

#endif /* DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1 */
