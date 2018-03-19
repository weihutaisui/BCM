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

#ifndef _OMCI_RULE_LLIST_H_
#define _OMCI_RULE_LLIST_H_

#include "os_defs.h"
#include "bcm_common_llist.h"
#include "vlanctl_api.h"

/* MAX number of vlanctl rules per interface */
#define OMCI_NUMBER_OF_RULES_MAX 64

// Keep fields to identify vlanctl rule
typedef struct {
    vlanCtl_direction_t tableDir;
    unsigned int nbrOfTags;
    unsigned int tagRuleId;
} OMCI_RULE_ID;

// ifName - name of interface has these vlanctl rules
// nbrOfNewRules - number of rules in active area
// nbrOfOldRules - number of rules in remove area
// newRulesList -- array contains new rules
// oldRulesList -- arry contains old rules
typedef struct {
    BCM_COMMON_DECLARE_LL_ENTRY ();
    char ifName[CMS_IFNAME_LENGTH];
    UINT32 phyId;
    OmciPhyType phyType;
    UINT32 nbrOfNewRules;
    UINT32 nbrOfOldRules;
    OMCI_RULE_ID newRulesList[OMCI_NUMBER_OF_RULES_MAX];
    OMCI_RULE_ID oldRulesList[OMCI_NUMBER_OF_RULES_MAX];
} OMCI_RULE_ENTRY, *POMCI_RULE_ENTRY;

/**
 * Global OMCI RULE Link List functions
 **/

// create rule entry if interface entry does not exist
CmsRet omci_rule_create(const char *ifName, const UINT32 phyId,
  const OmciPhyType phyType);

// add single rule to new rules list
CmsRet omci_rule_add(const char *ifName,
                     const UINT32 phyId,
                     OmciPhyType phyType,
                     const vlanCtl_direction_t tableDir,
                     const unsigned int nbrOfTags,
                     const unsigned int tagRuleId);

// copy all rules in new rules list to old rules list
CmsRet omci_rule_copy(const char *ifName, const UINT32 phyId);

// remove all rules in old rules list
// but NOT remove rules in new rules list NOR rule entry
CmsRet omci_rule_remove(const char *ifName, const UINT32 phyId);

// remove all rules in old and new rules lists
// for this specific entry
CmsRet omci_rule_delete(const char *ifName, const UINT32 phyId);

// remove all rules in old and new rules lists
// for all entries with this phyId
CmsRet omci_rule_delete_by_phyId(const UINT32 phyId,
  const OmciPhyType phyType);

// return CMSRET_SUCCESS if this specific rule can be found
// otherwise return CMSRET_OBJECT_NOT_FOUND
CmsRet omci_rule_exist(const char *ifName, const UINT32 phyId);

CmsRet omci_rule_init(void);

void omci_rule_exit(void);

void printAllOmciVlanCtlRules(void);


#endif /* _OMCI_RULE_LLIST_H_ */
