/***********************************************************************
 *
 *  Copyright (c) 2007 Broadcom
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

#ifdef DMP_X_ITU_ORG_GPON_1

#include "owsvc_api.h"
#include "rut_gpon.h"
#include "rut_gpon_rule_llist.h"

#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1
#include "rut_gpon_service_llist.h"
#endif    // DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1

/**
 * Local OMCI Rule LList variables
 **/

static BCM_COMMON_DECLARE_LL(omciRuleLL);

void printAllOmciVlanCtlRules(void);


/**
 * Local OMCI Rule LList functions
 **/

static void freeOmciRuleEntry(POMCI_RULE_ENTRY omciRuleEntry)
{
    UINT32 i = 0;

    if (omciRuleEntry == NULL)
        return;

    vlanCtl_init();

    for (i = 0; i < omciRuleEntry->nbrOfOldRules; i++)
    {
        vlanCtl_removeTagRule(omciRuleEntry->ifName,
                              omciRuleEntry->oldRulesList[i].tableDir,
                              omciRuleEntry->oldRulesList[i].nbrOfTags,
                              omciRuleEntry->oldRulesList[i].tagRuleId);
    }

    for (i = 0; i < omciRuleEntry->nbrOfNewRules; i++)
    {
        vlanCtl_removeTagRule(omciRuleEntry->ifName,
                              omciRuleEntry->newRulesList[i].tableDir,
                              omciRuleEntry->newRulesList[i].nbrOfTags,
                              omciRuleEntry->newRulesList[i].tagRuleId);
    }

    vlanCtl_cleanup();

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> freeOmciRuleEntry, ifName=%s, phyId=%d, nbrOfNewRules=%d, nbrOfOldRules=%d\n",
                   omciRuleEntry->ifName, omciRuleEntry->phyId,
                   omciRuleEntry->nbrOfNewRules, omciRuleEntry->nbrOfOldRules);

    CMSMEM_FREE_BUF_AND_NULL_PTR(omciRuleEntry);
}

static POMCI_RULE_ENTRY allocOmciRuleEntry(const char *ifName, const UINT32 phyId)
{
    POMCI_RULE_ENTRY omciRuleEntry = NULL;

    omciRuleEntry = cmsMem_alloc(sizeof(OMCI_RULE_ENTRY), ALLOC_ZEROIZE);

    if (omciRuleEntry != NULL)
    {
        cmsUtl_strncpy(omciRuleEntry->ifName, ifName, CMS_IFNAME_LENGTH);
        omciRuleEntry->phyId = phyId;
    }

    return omciRuleEntry;
}

static void freeAllOmciRuleEntry(void)
{
    POMCI_RULE_ENTRY omciRuleEntry, nextOmciRuleEntry;

    omciRuleEntry = BCM_COMMON_LL_GET_HEAD(omciRuleLL);

    cmsLog_notice("Cleaning-up OMCI Rule Table (0x%08X)", (UINT32)omciRuleEntry);

    while (omciRuleEntry)
    {
        cmsLog_notice("Cleaning-up OMCI Rule Entry for %s", omciRuleEntry->ifName);
        nextOmciRuleEntry = BCM_COMMON_LL_GET_NEXT(omciRuleEntry);
        BCM_COMMON_LL_REMOVE(&omciRuleLL, omciRuleEntry);
        freeOmciRuleEntry(omciRuleEntry);
        omciRuleEntry = nextOmciRuleEntry;
    };

    BCM_COMMON_LL_INIT(&omciRuleLL);

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE, "===> freeAllOmciRuleEntry\n");
}

static POMCI_RULE_ENTRY findOmciRuleEntry(const char *ifName, const UINT32 phyId)
{
    POMCI_RULE_ENTRY omciRuleEntry = BCM_COMMON_LL_GET_HEAD(omciRuleLL);

    while (omciRuleEntry)
    {
        if (omciRuleEntry->phyId == phyId &&
            strcmp(omciRuleEntry->ifName, ifName) == 0)
        {
            omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
               "===> findOmciRuleEntry, ifName=%s, phyId=%d is FOUND\n",
               omciRuleEntry->ifName, omciRuleEntry->phyId);

            break;
        }
        omciRuleEntry = BCM_COMMON_LL_GET_NEXT(omciRuleEntry);
    };

    return omciRuleEntry;
}

/* Check if the rule already exists in the old rules list. */
static UINT32 compressOldRulesList(POMCI_RULE_ENTRY omciRuleEntry,
  vlanCtl_direction_t tableDir, unsigned int nbrOfTags, unsigned int tagRuleId)
{
    UINT32 ruleNumBefore = 0;
    UINT32 ruleNumAfter = 0;
    OMCI_RULE_ID *tmpOldRulesListP;
    UINT32 i;

    if (omciRuleEntry->nbrOfOldRules == 0)
    {
        return 0;
    }

    ruleNumBefore = omciRuleEntry->nbrOfOldRules;
    tmpOldRulesListP = cmsMem_alloc(sizeof(OMCI_RULE_ID) * ruleNumBefore, ALLOC_ZEROIZE);

    for (i = 0; i < ruleNumBefore; i++)
    {
        if ((omciRuleEntry->oldRulesList[i].tableDir == tableDir) &&
          (omciRuleEntry->oldRulesList[i].nbrOfTags == nbrOfTags) &&
          (omciRuleEntry->oldRulesList[i].tagRuleId == tagRuleId))
        {
            cmsLog_debug(
              "===> found dup entry, ifName=%s, phyId=%d, dir=%d, "
              "nbrOfTags=%d, tagRuleId=%d, nbrOfOldRules=%d\n",
              omciRuleEntry->ifName, omciRuleEntry->phyId,
              tableDir, nbrOfTags, tagRuleId,
              omciRuleEntry->nbrOfOldRules);
        }
        else
        {
            memcpy(&tmpOldRulesListP[ruleNumAfter], &omciRuleEntry->oldRulesList[i],
              sizeof(OMCI_RULE_ID));
            ruleNumAfter++;
        }
    }

    if (ruleNumAfter != ruleNumBefore)
    {
        if (ruleNumAfter != 0)
        {
            memcpy(&omciRuleEntry->oldRulesList, tmpOldRulesListP,
              ruleNumAfter * sizeof(OMCI_RULE_ID));
        }
        omciRuleEntry->nbrOfOldRules = ruleNumAfter;

        cmsLog_debug(
          "===> ifName=%s, phyId=%d, before=%d, after=%d\n",
          omciRuleEntry->ifName, omciRuleEntry->phyId,
          ruleNumBefore, ruleNumAfter);
    }

    CMSMEM_FREE_BUF_AND_NULL_PTR(tmpOldRulesListP);

    return (ruleNumBefore - ruleNumAfter);
}


/**
 * Global OMCI Rule LList functions
 **/

CmsRet omci_rule_create(const char *ifName, const UINT32 phyId, const OmciPhyType phyType)
{
    CmsRet ret = CMSRET_SUCCESS;
    POMCI_RULE_ENTRY omciRuleEntry = NULL;

    /* allocate a new OMCI Rule entry */
    omciRuleEntry = allocOmciRuleEntry(ifName, phyId);

    if (omciRuleEntry != NULL)
    {
        cmsLog_notice("Create OMCI Rule Entry for %s, phyId=%d, phyType=%d", ifName, phyId, phyType);
        omciRuleEntry->phyType = phyType;
        BCM_COMMON_LL_APPEND(&omciRuleLL, omciRuleEntry);
    }
    else
    {
        cmsLog_error("Could not allocate OMCI Rule Entry memory for %s, phyId=%d", ifName, phyId);
        ret = CMSRET_RESOURCE_EXCEEDED;
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> omci_rule_create, ifName=%s, phyId=%d, phyType=%d, ret=%d\n",
                   ifName, phyId, phyType, ret);
    return ret;
}

CmsRet omci_rule_delete(const char *ifName, const UINT32 phyId)
{
    CmsRet ret = CMSRET_SUCCESS;
    POMCI_RULE_ENTRY omciRuleEntry = NULL;

    omciRuleEntry = findOmciRuleEntry(ifName, phyId);

    if (omciRuleEntry != NULL)
    {
        cmsLog_notice("Freeing OMCI Rule Entry for %s, phyId=%d", ifName, phyId);
        BCM_COMMON_LL_REMOVE(&omciRuleLL, omciRuleEntry);
        freeOmciRuleEntry(omciRuleEntry);
    }
    else
    {
        cmsLog_notice("Could not find OMCI Rule Entry for %s, phyId=%d", ifName, phyId);
        ret = CMSRET_OBJECT_NOT_FOUND;
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> omci_rule_delete, ifName=%s, phyId=%d, ret=%d\n",
                   ifName, phyId, ret);
    return ret;
}

CmsRet omci_rule_delete_by_phyId(const UINT32 phyId, const OmciPhyType phyType)
{
    CmsRet ret = CMSRET_SUCCESS;
    POMCI_RULE_ENTRY omciRuleEntry = BCM_COMMON_LL_GET_HEAD(omciRuleLL);
    POMCI_RULE_ENTRY tempOmciRuleEntry = NULL;

    while (omciRuleEntry)
    {
        if ((omciRuleEntry->phyId == phyId) && (omciRuleEntry->phyType == phyType))
        {
            omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
              "===> omci_rule_delete_by_PhyId, Freeing OMCI Rule Entry for ifName %s, phyId=%d",
              omciRuleEntry->ifName, phyId);

            tempOmciRuleEntry = omciRuleEntry;
            omciRuleEntry = BCM_COMMON_LL_GET_NEXT(omciRuleEntry);

            /* Free the found omciRuleEntry */
            BCM_COMMON_LL_REMOVE(&omciRuleLL, tempOmciRuleEntry);
            freeOmciRuleEntry(tempOmciRuleEntry);
        }
        else
        {
            omciRuleEntry = BCM_COMMON_LL_GET_NEXT(omciRuleEntry);
        }
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> omci_rule_delete_by_PhyId, phyId=%d, ret=%d\n",
                   phyId, ret);
    return ret;
}

CmsRet omci_rule_add
    (const char *ifName,
     const UINT32 phyId,
     const OmciPhyType phyType,
     const vlanCtl_direction_t tableDir,
     const unsigned int nbrOfTags,
     const unsigned int tagRuleId)
{
    UINT32 pos = 0;
    CmsRet ret = CMSRET_SUCCESS;
    POMCI_RULE_ENTRY omciRuleEntry = NULL;

    omciRuleEntry = findOmciRuleEntry(ifName, phyId);

    if (omciRuleEntry != NULL)
    {
        cmsLog_notice("Add OMCI Rule Entry for %s, phyId=%d, dir=%d, nbrOfTags=%d, tagRuleId=%d",
                      ifName, phyId, tableDir, nbrOfTags, tagRuleId);
        pos = omciRuleEntry->nbrOfNewRules;
        if (pos < OMCI_NUMBER_OF_RULES_MAX)
        {
            omciRuleEntry->newRulesList[pos].tableDir = tableDir;
            omciRuleEntry->newRulesList[pos].nbrOfTags = nbrOfTags;
            omciRuleEntry->newRulesList[pos].tagRuleId = tagRuleId;
            (omciRuleEntry->nbrOfNewRules)++;
/*
omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
               "===> omci_rule_add, existed ifName=%s, phyId=%d, dir=%d, nbrOfTags=%d, tagRuleId=%d, nbrOfNewRules=%d\n",
               ifName, phyId, tableDir, nbrOfTags, tagRuleId, omciRuleEntry->nbrOfNewRules);
*/
        }
        else
        {
            cmsLog_error("New rule [%d] is over maximum number of OMCI Rule Entries [%d] for %s, phyId=%d",
              pos, OMCI_NUMBER_OF_RULES_MAX, ifName, phyId);
            ret = CMSRET_RESOURCE_EXCEEDED;
            printAllOmciVlanCtlRules();
        }
    }
    else
    {
        // if cannot find interface entry then
        // create it and add again
        ret = omci_rule_create(ifName, phyId, phyType);
        if (ret == CMSRET_SUCCESS)
        {
            omciRuleEntry = findOmciRuleEntry(ifName, phyId);

            if (omciRuleEntry != NULL)
            {
                cmsLog_notice("Add OMCI Rule Entry for %s, phyId=%d, dir=%d, nbrOfTags=%d, tagRuleId=%d",
                      ifName, phyId, tableDir, nbrOfTags, tagRuleId);
                pos = omciRuleEntry->nbrOfNewRules;
                if (pos < OMCI_NUMBER_OF_RULES_MAX)
                {
                    omciRuleEntry->newRulesList[pos].tableDir = tableDir;
                    omciRuleEntry->newRulesList[pos].nbrOfTags = nbrOfTags;
                    omciRuleEntry->newRulesList[pos].tagRuleId = tagRuleId;
                    (omciRuleEntry->nbrOfNewRules)++;
/*
omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
               "===> omci_rule_add, existed ifName=%s, phyId=%d, dir=%d, nbrOfTags=%d, tagRuleId=%d, nbrOfNewRules=%d\n",
               ifName, phyId, tableDir, nbrOfTags, tagRuleId, omciRuleEntry->nbrOfNewRules);
*/
                }
                else
                {
                    cmsLog_error("New rule [%d] is over maximum number of OMCI Rule Entries [%d] for %s, phyId=%d",
                      pos, OMCI_NUMBER_OF_RULES_MAX, ifName, phyId);
                    ret = CMSRET_RESOURCE_EXCEEDED;
                }
            }
            else
            {
                cmsLog_error("Cannot find OMCI Rule Entry for %s, phyId=%d", ifName, phyId);
                ret = CMSRET_OBJECT_NOT_FOUND;
            }
        }
    }

    if (omciRuleEntry != NULL)
    {
        compressOldRulesList(omciRuleEntry, tableDir, nbrOfTags, tagRuleId);

        omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
            "===> omci_rule_add, ifName=%s, phyId=%d, dir=%d, nbrOfTags=%d, tagRuleId=%d, nbrOfNewRules=%d, ret=%d\n",
            ifName, phyId, tableDir, nbrOfTags, tagRuleId, omciRuleEntry->nbrOfNewRules, ret);
    }

    return ret;
}

CmsRet omci_rule_copy(const char *ifName, const UINT32 phyId)
{
    CmsRet ret = CMSRET_SUCCESS;
    POMCI_RULE_ENTRY omciRuleEntry = NULL;

    omciRuleEntry = findOmciRuleEntry(ifName, phyId);

    if (omciRuleEntry != NULL)
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
               "===> omci_rule_copy, before, ifName=%s, phyId=%d, nbrOfNewRules=%d, nbrOfOldRules=%d\n",
               ifName, phyId, omciRuleEntry->nbrOfNewRules, omciRuleEntry->nbrOfOldRules);

        cmsLog_notice("Copy all rules from active area to remove area for %s, phyId=%d", ifName, phyId);
        // clear old rules list
        memset(&omciRuleEntry->oldRulesList[0],
               0,
               sizeof(OMCI_RULE_ID) * omciRuleEntry->nbrOfOldRules);
        // copy rules in new rules list to old rules list
        memcpy(&omciRuleEntry->oldRulesList[0],
               &omciRuleEntry->newRulesList[0],
               sizeof(OMCI_RULE_ID) * omciRuleEntry->nbrOfNewRules);
        // set number of old rules equal to number of new rules
        omciRuleEntry->nbrOfOldRules = omciRuleEntry->nbrOfNewRules;
        // clear new rules list
        memset(&omciRuleEntry->newRulesList[0],
               0,
               sizeof(OMCI_RULE_ID) * omciRuleEntry->nbrOfNewRules);
        // reset number of active rules
        omciRuleEntry->nbrOfNewRules = 0;

        omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
               "===> omci_rule_copy, after, ifName=%s, phyId=%d, nbrOfNewRules=%d, nbrOfOldRules=%d\n",
               ifName, phyId, omciRuleEntry->nbrOfNewRules, omciRuleEntry->nbrOfOldRules);
    }
    else
    {
        cmsLog_error("Could not find OMCI Rule Entry for %s, phyId=%d", ifName, phyId);
        ret = CMSRET_OBJECT_NOT_FOUND;
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> omci_rule_copy, ifName=%s, phyId=%d, ret=%d\n",
                   ifName, phyId, ret);
    return ret;
}

CmsRet omci_rule_remove(const char *ifName, const UINT32 phyId)
{
    UINT32 i = 0;
    CmsRet ret = CMSRET_SUCCESS;
    POMCI_RULE_ENTRY omciRuleEntry = NULL;

    omciRuleEntry = findOmciRuleEntry(ifName, phyId);

    if (omciRuleEntry != NULL)
    {
        cmsLog_notice("Remove all rules in remove area for %s, phyId=%d", ifName, phyId);
        vlanCtl_init();
        // remove tag rules in old rules list
        for (i = 0; i < omciRuleEntry->nbrOfOldRules; i++)
        {
            vlanCtl_removeTagRule(omciRuleEntry->ifName,
                                  omciRuleEntry->oldRulesList[i].tableDir,
                                  omciRuleEntry->oldRulesList[i].nbrOfTags,
                                  omciRuleEntry->oldRulesList[i].tagRuleId);

            omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
               "===> omci_rule_remove, ifName=%s, phyId=%d, dir=%d, nbrOfTags=%d, tagRuleId=%d, ret=%d\n",
               ifName, phyId, omciRuleEntry->oldRulesList[i].tableDir,
               omciRuleEntry->oldRulesList[i].nbrOfTags,
               omciRuleEntry->oldRulesList[i].tagRuleId, ret);
        }
        vlanCtl_cleanup();
        // clear tag rules in old rules list
        memset(&omciRuleEntry->oldRulesList[0],
               0,
               sizeof(OMCI_RULE_ID) * omciRuleEntry->nbrOfOldRules);
        // reset number of old rules
        omciRuleEntry->nbrOfOldRules = 0;
    }
    else
    {
        cmsLog_error("Could not find OMCI Rule Entry for %s, phyId=%d", ifName, phyId);
        ret = CMSRET_OBJECT_NOT_FOUND;
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> omci_rule_remove, ifName=%s, phyId=%d, ret=%d\n",
                   ifName, phyId, ret);
    return ret;
}

CmsRet omci_rule_exist(const char *ifName, const UINT32 phyId)
{
    CmsRet ret = CMSRET_SUCCESS;
    POMCI_RULE_ENTRY omciRuleEntry = NULL;

    omciRuleEntry = findOmciRuleEntry(ifName, phyId);

    if (omciRuleEntry == NULL)
    {
        cmsLog_notice("Could not find OMCI Rule Entry for %s, phyId=%d", ifName, phyId);
        ret = CMSRET_OBJECT_NOT_FOUND;
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> omci_rule_exist, ifName=%s, phyId=%d, ret=%d\n",
                   ifName, phyId, ret);
    return ret;
}

CmsRet omci_rule_init(void)
{
    CmsRet ret = CMSRET_SUCCESS;

    BCM_COMMON_LL_INIT(&omciRuleLL);

    return ret;
}

void omci_rule_exit(void)
{
    // free all OMCI rules memory
    freeAllOmciRuleEntry();
}


void printAllOmciVlanCtlRules(void)
{
    POMCI_RULE_ENTRY omciRuleEntry, nextOmciRuleEntry;
    UINT32 i = 0;

    omciRuleEntry = BCM_COMMON_LL_GET_HEAD(omciRuleLL);
    if (omciRuleEntry == NULL)
    {
        return;
    }

    printf("\nOMCI rule list:\n\n");
    printf("   # Ifname       PhyId  Type   NewR#  OldR# \n");
    printf("==== ============ ====== ====== ====== ======\n");
    while (omciRuleEntry)
    {
        i++;
        nextOmciRuleEntry = BCM_COMMON_LL_GET_NEXT(omciRuleEntry);
        printf("%-4d %-12s %6d %6d %6d %6d\n",
          i, omciRuleEntry->ifName, omciRuleEntry->phyId,
          omciRuleEntry->phyType,
          omciRuleEntry->nbrOfNewRules, omciRuleEntry->nbrOfOldRules);
          omciRuleEntry = nextOmciRuleEntry;
    };
    printf("==== ============ ====== ====== ====== ======\n");
}


#endif /* DMP_X_ITU_ORG_GPON_1 */
