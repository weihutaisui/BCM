/***********************************************************************
 *
 *  Copyright (c) 2008 Broadcom
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
#include "ctlutils_api.h"
#include "me_handlers.h"


#define VLANIF_FILE_NAME    "/tmp/fakevlanintf"
#define VLANIF_TEMP_NAME    "/tmp/tempvlanintf"


//=======================  Private GPON CMF functions ========================

// word has bit layout as following
// 31.30.29.28.27.26.25.......1.0
// field has start bit that can be at
// any index number from 0 to 31
// len is the numbe of bits in field
UINT32 getFieldFromWord
    (UINT32 word, UINT8 startBit, UINT8 len)
{
    UINT32 val = 0;

    val = (word << (31 - startBit));
    val = (val >> (32 - len));

    return val;
}

// tagRule must be array of OMCI_TAGGING_RULE_SIZE (16) bytes
// The hexstring in MDM is stored in network order. The byte order conversion
// may be needed after cmsUtl_hexStringToBinaryBuf(), it is case-by-case based.
void getTagRuleInfo
    (const UINT8 *tagRule, OmciExtVlanTagOper_t *tagOper)
{
    UINT32 outerFilter = 0, innerFilter = 0;
    UINT32 outerTreatment = 0, innerTreatment = 0;

    memcpy(&outerFilter, 
        &tagRule[OMCI_OUTER_FILTER_START], OMCI_FILTER_LEN);
    outerFilter = OMCI_NTOHL(&outerFilter);
    memcpy(&innerFilter, 
        &tagRule[OMCI_INNER_FILTER_START], OMCI_FILTER_LEN);
    innerFilter = OMCI_NTOHL(&innerFilter);    
    memcpy(&outerTreatment, 
        &tagRule[OMCI_OUTER_TREATMENT_START], OMCI_TREATMENT_LEN);
    outerTreatment = OMCI_NTOHL(&outerTreatment);
    memcpy(&innerTreatment, 
        &tagRule[OMCI_INNER_TREATMENT_START], OMCI_TREATMENT_LEN);
    innerTreatment = OMCI_NTOHL(&innerTreatment);

    tagOper->outer.filter.prio = getFieldFromWord
        (outerFilter, OMCI_FILTER_PRIO_START, OMCI_FILTER_PRIO_LEN);
    tagOper->outer.filter.vlanId = getFieldFromWord
        (outerFilter, OMCI_FILTER_VLANID_START, OMCI_FILTER_VLANID_LEN);
    tagOper->outer.filter.tpid_de = getFieldFromWord
        (outerFilter, OMCI_FILTER_TPID_DE_START, OMCI_FILTER_TPID_DE_LEN);

    tagOper->inner.filter.prio = getFieldFromWord
        (innerFilter, OMCI_FILTER_PRIO_START, OMCI_FILTER_PRIO_LEN);
    tagOper->inner.filter.vlanId = getFieldFromWord
        (innerFilter, OMCI_FILTER_VLANID_START, OMCI_FILTER_VLANID_LEN);
    tagOper->inner.filter.tpid_de = getFieldFromWord
        (innerFilter, OMCI_FILTER_TPID_DE_START, OMCI_FILTER_TPID_DE_LEN);
    tagOper->etherType = getFieldFromWord
        (innerFilter, OMCI_FILTER_ETHER_TYPE_START, OMCI_FILTER_ETHER_TYPE_LEN);

    tagOper->removeType = getFieldFromWord
        (outerTreatment, OMCI_TREATMENT_REMOVE_START, OMCI_TREATMENT_REMOVE_LEN);
    tagOper->outer.treatment.prio = getFieldFromWord
        (outerTreatment, OMCI_TREATMENT_PRIO_START, OMCI_TREATMENT_PRIO_LEN);
    tagOper->outer.treatment.vlanId = getFieldFromWord
        (outerTreatment, OMCI_TREATMENT_VLANID_START, OMCI_TREATMENT_VLANID_LEN);
    tagOper->outer.treatment.tpid_de = getFieldFromWord
        (outerTreatment, OMCI_TREATMENT_TPID_DE_START, OMCI_TREATMENT_TPID_DE_LEN);

    tagOper->inner.treatment.prio = getFieldFromWord
        (innerTreatment, OMCI_TREATMENT_PRIO_START, OMCI_TREATMENT_PRIO_LEN);
    tagOper->inner.treatment.vlanId = getFieldFromWord
        (innerTreatment, OMCI_TREATMENT_VLANID_START, OMCI_TREATMENT_VLANID_LEN);
    tagOper->inner.treatment.tpid_de = getFieldFromWord
        (innerTreatment, OMCI_TREATMENT_TPID_DE_START, OMCI_TREATMENT_TPID_DE_LEN);

    if (tagOper->inner.filter.prio == OMCI_FILTER_IGNORE)
        tagOper->filterType = OMCI_FILTER_TYPE_UNTAG;
    else if (tagOper->outer.filter.prio == OMCI_FILTER_IGNORE)
        tagOper->filterType = OMCI_FILTER_TYPE_SINGLE_TAG;
    else
        tagOper->filterType = OMCI_FILTER_TYPE_DOUBLE_TAG;

    omciDebugPrint(OMCI_DEBUG_MODULE_VLAN, "===> getTagRuleInfo\n \
    outer.filter.prio=%d,\touter.filter.vlanId=%d,\touter.filter.tpid_de=%d\n \
    inner.filter.prio=%d,\tinner.filter.vlanId=%d,\tinner.filter.tpid_de=%d\n \
    outer.treatment.prio=%d,\touter.treatment.vlanId=%d,\touter.treatment.tpid_de=%d\n \
    inner.treatment.prio=%d,\tinner.treatment.vlanId=%d,\tinner.treatment.tpid_de=%d\n \
    etherType=%d,\t\tremoveType=%d,\t\t\tfilterType=%d\n\n", tagOper->outer.filter.prio, tagOper->outer.filter.vlanId, tagOper->outer.filter.tpid_de, tagOper->inner.filter.prio, tagOper->inner.filter.vlanId, tagOper->inner.filter.tpid_de, tagOper->outer.treatment.prio, tagOper->outer.treatment.vlanId, tagOper->outer.treatment.tpid_de, tagOper->inner.treatment.prio, tagOper->inner.treatment.vlanId, tagOper->inner.treatment.tpid_de, tagOper->etherType, tagOper->removeType, tagOper->filterType);
}

CmsRet getNumberOfTagRules
    (const UINT32 xVlanMeId,
     OmciExtVlanTagOperInfo_t *pExtTagOperInfo)
{
    UBOOL8 found = FALSE, isTagOperExisted = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    ExtendedVlanTaggingOperationConfigurationDataObject *xVlan = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // initialize numberOfEntries to 0 just incase
    // there is no ExtendedVlanTaggingOperationConfigurationDataObject
    pExtTagOperInfo->numberOfEntries = 0;

    // search ExtendedVlanTaggingOperationConfigurationDataObject
    // that has its meId matches with the given meId
    while ((!found) &&
           (_cmsObj_getNext
                (MDMOID_EXTENDED_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA,
                 &iidStack,
                 (void **) &xVlan) == CMSRET_SUCCESS))
    {
        found = (xVlanMeId == xVlan->managedEntityId);

        if (found == TRUE)
        {
            InstanceIdStack iidStackChild = EMPTY_INSTANCE_ID_STACK;
            ReceivedFrameVlanTaggingOperationTableObject *tagRuleEntry;
            UBOOL8 delete = FALSE;
            // if the last 8 bytes are 0xFF then delete this entry
            // note: 1 byte is stored as 2 ascii characters in OMCI database ==>
            // compare 16 bytes, if match then the given entry exists in the table
            if (xVlan->receivedFrameVlanTaggingOperationTable != NULL &&
                strncasecmp(&xVlan->receivedFrameVlanTaggingOperationTable[OMCI_TAGGING_RULE_SIZE],
                           (char *)"FFFFFFFFFFFFFFFF", OMCI_TAGGING_RULE_SIZE) == 0)
                delete = TRUE;
            // walk through vlan tagging operation table
            // to count number of tagging rules
            while (_cmsObj_getNextInSubTree
                       (MDMOID_RECEIVED_FRAME_VLAN_TAGGING_OPERATION_TABLE,
                        &iidStack,
                        &iidStackChild,
                        (void **) &tagRuleEntry) == CMSRET_SUCCESS)
            {
                // is the last xVlan Tagging Oper existed in table already?
                // compare the first 8 bytes of entry that are identified table entry
                // note: 1 byte is stored as 2 ascii characters in OMCI database ==>
                // compare 16 bytes, if match then the given entry exists in the table
                if (xVlan->receivedFrameVlanTaggingOperationTable != NULL &&
                    strncasecmp((char *)xVlan->receivedFrameVlanTaggingOperationTable,
                           (char *)tagRuleEntry->taggingRule, OMCI_TAGGING_RULE_SIZE) == 0)
                {
                    isTagOperExisted = TRUE;
                    if (delete == FALSE)
                        pExtTagOperInfo->numberOfEntries += 1;
                }
                else
                    pExtTagOperInfo->numberOfEntries += 1;
                _cmsObj_free((void **) &tagRuleEntry);
            }
            // the last xVlan Tagging Operation is stored in
            // receivedFrameVlanTaggingOperationTable field of xVlan.
            // Since hexString is type of value in receivedFrameVlanTaggingOperationTable
            // its size should be double the binary size
            if (isTagOperExisted == FALSE &&
                xVlan->receivedFrameVlanTaggingOperationTable != NULL &&
                strlen(xVlan->receivedFrameVlanTaggingOperationTable) == OMCI_TAGGING_RULE_SIZE * 2)
                pExtTagOperInfo->numberOfEntries += 1;
            pExtTagOperInfo->inputTpid = xVlan->inputTpid;
            pExtTagOperInfo->outputTpid = xVlan->outputTpid;
            pExtTagOperInfo->downstreamMode = xVlan->downstreamMode;
            ret = CMSRET_SUCCESS;
        }

        _cmsObj_free((void **) &xVlan);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_VLAN,
                   "===> getNumberOfTagRules, xVlanMeId=%d, numberOfEntries=%d, ret=%d\n",
                    xVlanMeId, pExtTagOperInfo->numberOfEntries, ret);

    return ret;
}

CmsRet getTagRules
    (const UINT32 xVlanMeId,
     OmciExtVlanTagOper_t *tagRules,
     const UINT32 max)
{
    UBOOL8 found = FALSE, isTagOperExisted = FALSE;
    UINT8 *buf = NULL;
    UINT32 bufSize = 0;
    UINT32 counter = 0;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    ExtendedVlanTaggingOperationConfigurationDataObject *xVlan = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // search extended vlan object that has
    // its meId or associatedManagedEntityPointer
    // that matches with the given meId
    while ((!found) &&
           (_cmsObj_getNext
                (MDMOID_EXTENDED_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA,
                 &iidStack,
                 (void **) &xVlan) == CMSRET_SUCCESS))
    {
        found = (xVlanMeId == xVlan->managedEntityId);

        if (found == TRUE)
        {
            InstanceIdStack iidStackChild = EMPTY_INSTANCE_ID_STACK;
            ReceivedFrameVlanTaggingOperationTableObject *tagRuleEntry;
            UBOOL8 delete = FALSE;
            // if the last 8 bytes are 0xFF then delete this entry
            // note: 1 byte is stored as 2 ascii characters in OMCI database ==>
            // compare 16 bytes, if match then the given entry exists in the table
            if (xVlan->receivedFrameVlanTaggingOperationTable != NULL &&
                strncasecmp(&xVlan->receivedFrameVlanTaggingOperationTable[OMCI_TAGGING_RULE_SIZE],
                           (char *)"FFFFFFFFFFFFFFFF", OMCI_TAGGING_RULE_SIZE) == 0)
                delete = TRUE;
            // walk through vlan tagging operation table
            // to convert each tagging rule
            while ((_cmsObj_getNextInSubTree
                        (MDMOID_RECEIVED_FRAME_VLAN_TAGGING_OPERATION_TABLE,
                         &iidStack,
                         &iidStackChild,
                         (void **) &tagRuleEntry) == CMSRET_SUCCESS))
            {
                // is the last xVlan Tagging Oper existed in table already?
                // compare the first 8 bytes of entry that are identified table entry
                // note: 1 byte is stored as 2 ascii characters in OMCI database ==>
                // compare 16 bytes, if match then the given entry exists in the table
                if (xVlan->receivedFrameVlanTaggingOperationTable != NULL &&
                    strncasecmp((char *)xVlan->receivedFrameVlanTaggingOperationTable,
                           (char *)tagRuleEntry->taggingRule, OMCI_TAGGING_RULE_SIZE) == 0)
                {
                    isTagOperExisted = TRUE;
                    //delete the rule
                    if (delete == TRUE)
                    {
                        _cmsObj_free((void **) &tagRuleEntry);
                        continue;
                    }
                    //get modified rule
                    ret = cmsUtl_hexStringToBinaryBuf
                          ((char *)xVlan->receivedFrameVlanTaggingOperationTable, &buf, &bufSize);
                }
                else
                    ret = cmsUtl_hexStringToBinaryBuf
                              ((char *)tagRuleEntry->taggingRule, &buf, &bufSize);
                if (ret != CMSRET_SUCCESS)
                {
                    cmsLog_error("cmsUtl_hexStringToBinaryBuf() return error %d", ret);
                    _cmsObj_free((void **) &tagRuleEntry);
                    _cmsObj_free((void **) &xVlan);
                    goto out;
                }

                if (bufSize == OMCI_TAGGING_RULE_SIZE && counter < max)
                {
                    getTagRuleInfo(buf, &tagRules[counter]);
                    counter += 1;
                }

                CMSMEM_FREE_BUF_AND_NULL_PTR(buf);
                _cmsObj_free((void **) &tagRuleEntry);
            }

            // the last xVlan Tagging Operation is stored in
            // receivedFrameVlanTaggingOperationTable field of xVlan.
            // Since hexString is type of value in receivedFrameVlanTaggingOperationTable
            // its size should be double the binary size
            if (isTagOperExisted == FALSE && counter < max &&
                xVlan->receivedFrameVlanTaggingOperationTable != NULL &&
                strlen(xVlan->receivedFrameVlanTaggingOperationTable) == OMCI_TAGGING_RULE_SIZE * 2)
            {
                ret = cmsUtl_hexStringToBinaryBuf
                          ((char *)xVlan->receivedFrameVlanTaggingOperationTable, &buf, &bufSize);
                if (ret == CMSRET_SUCCESS)
                {
                    if (bufSize == OMCI_TAGGING_RULE_SIZE)
                    {
                        getTagRuleInfo(buf, &tagRules[counter]);
                        counter += 1;
                    }
                    CMSMEM_FREE_BUF_AND_NULL_PTR(buf);
                }
                else
                {
                    cmsLog_error("cmsUtl_hexStringToBinaryBuf() return error %d", ret);
                    _cmsObj_free((void **) &xVlan);
                    goto out;
                }
            }
        }

        _cmsObj_free((void **) &xVlan);
    }

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_VLAN,
                   "===> getTagRules, xVlanMeId=%d, counter=%d, max=%d, ret=%d\n",
                   xVlanMeId, counter, max, ret);

    return ret;
}

// word has bit layout as following
// 15.14.13.12.11.10.9.......1.0
// field has start bit that can be at
// any index number from 0 to 15
// len is the number of bits in field
UINT32 getFieldFromHalfWord
    (UINT16 halfWord, UINT8 startBit, UINT8 len)
{
    UINT16 val = 0;

    val = (halfWord << (15 - startBit));
    val = (val >> (16 - len));

    return val;
}

CmsRet getNumberOfTagFilters
    (const UINT32 ctpMeId,
     const UINT32 bridgeMeId,
     OmciVlanTciFilterInfo_t *pVlanTciFilterInfo)
{
    UBOOL8 found = FALSE;
    UINT32 filterId = 0;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    VlanTaggingFilterDataObject *vlan = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // initialize value of numberOfEntries just in case
    // there is no VlanTaggingFilterDataObject
    pVlanTciFilterInfo->numberOfEntries = 0;

    // get management entity ID of VlanTaggingFilterDataObject
    // from the given GemPortNetworkCtpObject
    ret = rutGpon_getVlanFilterMeIdFromGemPortNetworkCtpMeId
        (ctpMeId, bridgeMeId, &filterId);
    if (ret != CMSRET_SUCCESS)
        goto out;

    while ((!found) &&
           (_cmsObj_getNext
                (MDMOID_VLAN_TAGGING_FILTER_DATA,
                 &iidStack,
                 (void **) &vlan) == CMSRET_SUCCESS))
    {
        found = (filterId == vlan->managedEntityId);

        if (found == TRUE && vlan->numberOfEntries > 0)
        {
            // for our implementation, if VlanTciFilterTbl exists,
            // the default filter must be added as the first entry.
            // So its numberOfEntries must be numberOfEntries in
            // VlanTaggingFilterDataObject + 1
            pVlanTciFilterInfo->numberOfEntries = vlan->numberOfEntries + 1;
            pVlanTciFilterInfo->forwardOperation = vlan->forwardOperation;
            ret = CMSRET_SUCCESS;
        }

        _cmsObj_free((void **) &vlan);
    }

out:

    omciDebugPrint(OMCI_DEBUG_MODULE_VLAN, "===> getNumberOfTagFilters, meId=%d, numberOfEntries=%d, ret=%d\n", ctpMeId, pVlanTciFilterInfo->numberOfEntries, ret);

    return ret;
}

// The hexstring in MDM is stored in network order. The byte order conversion
// may be needed after cmsUtl_hexStringToBinaryBuf(), it is case-by-case based.
CmsRet getTagFilters
    (const UINT32 ctpMeId,
     const UINT32 bridgeMeId,
     OmciVlanTciFilterInfo_t *pVlanTciFilterInfo)
{
    UBOOL8 found = FALSE;
    UINT16 i = 0, j = 0, val = 0;
    UINT32 filterId = 0;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    VlanTaggingFilterDataObject *vlan = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // get management entity ID of VlanTaggingFilterDataObject
    // from the given GemPortNetworkCtpObject
    ret = rutGpon_getVlanFilterMeIdFromGemPortNetworkCtpMeId
        (ctpMeId, bridgeMeId, &filterId);
    if (ret != CMSRET_SUCCESS)
        goto out;

    // search vlan tagging filter data object that has
    // its meId that matches with the given meId
    while ((!found) &&
           (_cmsObj_getNext
                (MDMOID_VLAN_TAGGING_FILTER_DATA,
                 &iidStack,
                 (void **) &vlan) == CMSRET_SUCCESS))
    {
        found = (filterId == vlan->managedEntityId);

        if (found == TRUE && vlan->numberOfEntries > 0)
        {
            UINT8 *buf = NULL;
            UINT32 bufSize = 0;
            ret = cmsUtl_hexStringToBinaryBuf
                      ((char *)vlan->vlanFilterList, &buf, &bufSize);
            if (ret != CMSRET_SUCCESS)
            {
                cmsLog_error("cmsUtl_hexStringToBinaryBuf() return error %d", ret);
                _cmsObj_free((void **) &vlan);
                goto out;
            }
            // for our implementation, if VlanTciFilterTbl exists,
            // the default filter must be added as the first entry.
            // All fields in default filter have values as OMCI_DONT_CARE
            pVlanTciFilterInfo->pVlanTciTbl[0].pbits = OMCI_DONT_CARE;
            pVlanTciFilterInfo->pVlanTciTbl[0].cfi = OMCI_DONT_CARE;
            pVlanTciFilterInfo->pVlanTciTbl[0].vlanId = OMCI_DONT_CARE;

            // for loop should start with i = 1
            for (i = 1, j = 0; i < pVlanTciFilterInfo->numberOfEntries && (j*2) < bufSize; i++, j++)
            {
                val = OMCI_NTOHS(&buf[j*2]);
                pVlanTciFilterInfo->pVlanTciTbl[i].pbits = getFieldFromHalfWord
                    (val, OMCI_TCI_PBITS_START, OMCI_TCI_PBITS_LEN);
                pVlanTciFilterInfo->pVlanTciTbl[i].cfi = getFieldFromHalfWord
                    (val, OMCI_TCI_CFI_START, OMCI_TCI_CFI_LEN);
                pVlanTciFilterInfo->pVlanTciTbl[i].vlanId = getFieldFromHalfWord
                    (val, OMCI_TCI_VLANID_START, OMCI_TCI_VLANID_LEN);

                omciDebugPrint(OMCI_DEBUG_MODULE_VLAN, "===> getTagFilters, pbits=%d, cfi=%d, vlanId=%d, index=%d\n", pVlanTciFilterInfo->pVlanTciTbl[i].pbits, pVlanTciFilterInfo->pVlanTciTbl[i].cfi, pVlanTciFilterInfo->pVlanTciTbl[i].vlanId, i);
            }

            CMSMEM_FREE_BUF_AND_NULL_PTR(buf);
            ret = CMSRET_SUCCESS;
        }

        _cmsObj_free((void **) &vlan);
    }

out:

    omciDebugPrint(OMCI_DEBUG_MODULE_VLAN, "===> getTagFilters, meId=%d, counter=%d, ret=%d\n", ctpMeId, pVlanTciFilterInfo->numberOfEntries, ret);

    return ret;
}

CmsRet allocTagTable
    (void **pTable, UINT32 entrySize, UINT32 numberOfEntries)
{
    CmsRet ret = CMSRET_RESOURCE_EXCEEDED;

    *pTable = cmsMem_alloc
        (entrySize * numberOfEntries, ALLOC_ZEROIZE);

    if (*pTable != NULL)
        ret = CMSRET_SUCCESS;

    return ret;
}

UINT8 mapDscpToPbit
    (const UINT8 *inDscp, const UINT32 pos)
{
    UINT8  pbit = 0, lpbit = 0, rpbit = 0;
    UINT32 bitPos = 0, bytePos = 0;

    bitPos = (pos % OMCI_PRIO_VAL_MAX);
    bytePos = ((pos / OMCI_PRIO_VAL_MAX) * 3);

    switch (bitPos)
    {
        case 0:
            pbit = (inDscp[bytePos] >> 5);
            break;
        case 1:
            pbit = ((inDscp[bytePos] & 0x1C) >> 2);
            break;
        case 2:
            lpbit = (inDscp[bytePos] & 0x03);
            rpbit = ((inDscp[bytePos+1] & 0x80) >> 7);
            pbit = ((lpbit << 1) | rpbit);
            break;
        case 3:
            pbit = ((inDscp[bytePos+1] & 0x70) >> 4);
            break;
        case 4:
            pbit = ((inDscp[bytePos+1] & 0x0E) >> 1);
            break;
        case 5:
            lpbit = (inDscp[bytePos+1] & 0x01);
            rpbit = ((inDscp[bytePos+2] & 0xC0) >> 6);
            pbit = ((lpbit << 2) | rpbit);
            break;
        case 6:
            pbit = ((inDscp[bytePos+2] & 0x38) >> 3);
            break;
        case 7:
            pbit = (inDscp[bytePos+2] & 0x07);
            break;
        default:
            break;
    }

    return (pbit & 0x07);
}

// inDscp is a bit string sequence of 64 3-bit groupings.
// ==> inDscp is array of 24 bytes (24*8 == 64*3 == 192)
// ==> outPbit is array of 64 unsigned bytes.
// The 64 sequence entries respresent the possible values
// of the 6-bit DSCP field. Each 3-bit grouping specifies
// the P-bit value to which the associated DSCP value
// should be mapped
void mapDscpListToPbitList
    (const UINT8 *inDscp, UINT8 *outPbit)
{
    UINT32 i = 0;

    for (i = 0; i < OMCI_DSCP_NUM_MAX; i++)
    {
        outPbit[i] = mapDscpToPbit(inDscp, i);
        omciDebugPrint(OMCI_DEBUG_MODULE_VLAN,
                       "===> rutGpon_mapDscpToPbit, i=%d, outPbit[i]=%d\n",
                       i, outPbit[i]);
    }
}

//=======================  Public GPON CMF functions ========================

CmsRet rutGpon_allocExtTagOperInfo
    (const UINT32 meOid,
     const UINT32 meId,
     OmciExtVlanTagOperInfo_t *pExtTagOperInfo)
{
    UINT32 xVlanMeId = 0;
    CmsRet ret = CMSRET_SUCCESS;

    if (rutGpon_getExtVlanOperMeIdFromUniMeId(meOid, meId, &xVlanMeId) != CMSRET_SUCCESS)
        goto out;

    // get number of tag rules in tag operation table
    getNumberOfTagRules(xVlanMeId, pExtTagOperInfo);

    if (pExtTagOperInfo->numberOfEntries > 0)
    {
        // allocate memory for tag operation table
        ret = allocTagTable
            ((void **)&(pExtTagOperInfo->pTagOperTbl),
             sizeof(OmciExtVlanTagOper_t),
             pExtTagOperInfo->numberOfEntries);

        if (ret == CMSRET_SUCCESS)
        {
            // convert tag rules and store them to tag operation table
            ret = getTagRules
                (xVlanMeId, pExtTagOperInfo->pTagOperTbl, pExtTagOperInfo->numberOfEntries);
            if (ret != CMSRET_SUCCESS)
            {
                CMSMEM_FREE_BUF_AND_NULL_PTR(pExtTagOperInfo->pTagOperTbl);
                pExtTagOperInfo->numberOfEntries = 0;
                if (ret != CMSRET_METHOD_NOT_SUPPORTED)
                    cmsLog_error("Cannot retrieve values for Tagging Rule Operation Table");
            }
        }
        else
        {
            cmsLog_error("Cannot allocate memory for Tagging Rule Operation Table");
            ret = CMSRET_RESOURCE_EXCEEDED;
        }

    }

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_VLAN,
                   "===> rutGpon_allocTagOperTbl, meOid=%d, meId=%d, xVlanMeId=%d, counter=%d, ret=%d\n",
                   meOid, meId, xVlanMeId, pExtTagOperInfo->numberOfEntries, ret);

    return ret;
}

CmsRet rutGpon_allocTagFilterInfo
    (const UINT32 ctpMeId,
     const UINT32 bridgeMeId,
     OmciVlanTciFilterInfo_t *pVlanTciFilterInfo)
{
    CmsRet ret = CMSRET_SUCCESS;

    // get number of tag filters in tag filter table
    getNumberOfTagFilters(ctpMeId, bridgeMeId, pVlanTciFilterInfo);

    if (pVlanTciFilterInfo->numberOfEntries > 0)
    {
        // allocate memory for tag filter table
        ret = allocTagTable
            ((void **)&(pVlanTciFilterInfo->pVlanTciTbl),
             sizeof(OmciVlanTciFilter_t),
             pVlanTciFilterInfo->numberOfEntries);

        if (pVlanTciFilterInfo->pVlanTciTbl != NULL)
        {
            // convert tag filters and store them to tag filter table
            ret = getTagFilters(ctpMeId, bridgeMeId, pVlanTciFilterInfo);
            if (ret != CMSRET_SUCCESS)
            {
                CMSMEM_FREE_BUF_AND_NULL_PTR(pVlanTciFilterInfo->pVlanTciTbl);
                pVlanTciFilterInfo->numberOfEntries = 0;
                if (ret != CMSRET_SUCCESS)
                    cmsLog_error("Cannot retrieve values for Tagging Filter Table");
            }
        }
        else
        {
            cmsLog_error("Cannot allocate memory for Tagging Filter Table");
            ret = CMSRET_RESOURCE_EXCEEDED;
        }

    }

    omciDebugPrint(OMCI_DEBUG_MODULE_VLAN, "===> rutGpon_allocTagFilterInfo, meId=%d, numberOfEntries=%d, ret=%d\n", ctpMeId, pVlanTciFilterInfo->numberOfEntries, ret);

    return ret;
}

CmsRet rutGpon_getTagOperInfo
    (const UINT32 uniOid,
     const UINT32 uniMeId,
     OmciVlanTagOperInfo_t *pVlanTagOperInfo)
{
    UBOOL8 found = FALSE;
    UINT32 operMeId = 0;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    VlanTaggingOperationConfigurationDataObject *vlan = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // initialize tagIsExisted to FALSE just in case
    // there is no VlanTaggingOperationConfigurationDataObject
    pVlanTagOperInfo->tagIsExisted = FALSE;

    if (rutGpon_getVlanOperMeIdFromUniMeId(uniOid, uniMeId, &operMeId) == CMSRET_SUCCESS)
    {
        // search VlanTaggingOperationConfigurationDataObject
        // that has its meId matched with operMeId
        while ((!found) &&
               (_cmsObj_getNext
                   (MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA,
                    &iidStack,
                    (void **) &vlan) == CMSRET_SUCCESS))
        {
            found = (operMeId == vlan->managedEntityId);

            if (found == TRUE)
            {
                pVlanTagOperInfo->tagIsExisted = TRUE;
                pVlanTagOperInfo->upstreamMode = vlan->upstreamVlanTaggingOperationMode;
                UINT16 val = (UINT16)vlan->upstreamVlanTagTciValue;
                pVlanTagOperInfo->upstreamTci.pbits = getFieldFromHalfWord
                    (val, OMCI_TCI_PBITS_START, OMCI_TCI_PBITS_LEN);
                pVlanTagOperInfo->upstreamTci.cfi = getFieldFromHalfWord
                    (val, OMCI_TCI_CFI_START, OMCI_TCI_CFI_LEN);
                pVlanTagOperInfo->upstreamTci.vlanId = getFieldFromHalfWord
                    (val, OMCI_TCI_VLANID_START, OMCI_TCI_VLANID_LEN);
                pVlanTagOperInfo->downstreamMode = vlan->downstreamVlanTaggingOperationMode;
                ret = CMSRET_SUCCESS;
            }

            _cmsObj_free((void **) &vlan);
        }
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_VLAN,
                   "===> rutGpon_getTagOperInfo, uniOid=%d, uniMeId=%d, operMeId=%d, tagIsExisted=%d, upstreamMode=%d, pbits=%d, cfi=%d, vlan=%d, downstreamMode=%d, ret=%d\n",
                   uniOid, uniMeId, operMeId, pVlanTagOperInfo->tagIsExisted, pVlanTagOperInfo->upstreamMode, pVlanTagOperInfo->upstreamTci.pbits, pVlanTagOperInfo->upstreamTci.cfi, pVlanTagOperInfo->upstreamTci.vlanId, pVlanTagOperInfo->downstreamMode, ret);

    return ret;
}

// The hexstring in MDM is stored in network order. The byte order conversion
// may be needed after cmsUtl_hexStringToBinaryBuf(), it is case-by-case based.
CmsRet rutGpon_getTagFilters
    (const VlanTaggingFilterDataObject *vlan,
     OmciVlanTciFilter_t *pVlanTciFilter)
{
    UINT8 *buf = NULL;
    UINT16 i = 0, j = 0, val = 0;
    UINT32 bufSize = 0;

    CmsRet ret = cmsUtl_hexStringToBinaryBuf
                      ((char *)vlan->vlanFilterList, &buf, &bufSize);

    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("cmsUtl_hexStringToBinaryBuf() return error %d", ret);
        goto out;
    }

    for (i = 0, j = 0;
         i < vlan->numberOfEntries &&
         i < OMCI_TCI_VLANID_LEN &&
         (j*2) < bufSize;
         i++, j++)
    {
        val = OMCI_NTOHS(&buf[j*2]);
        pVlanTciFilter[i].pbits = getFieldFromHalfWord
            (val, OMCI_TCI_PBITS_START, OMCI_TCI_PBITS_LEN);
        pVlanTciFilter[i].cfi = getFieldFromHalfWord
            (val, OMCI_TCI_CFI_START, OMCI_TCI_CFI_LEN);
        pVlanTciFilter[i].vlanId = getFieldFromHalfWord
            (val, OMCI_TCI_VLANID_START, OMCI_TCI_VLANID_LEN);

        omciDebugPrint(OMCI_DEBUG_MODULE_VLAN, "===> rutGpon_getTagFilters, pbits=%d, cfi=%d, vlanId=%d, index=%d\n", pVlanTciFilter[i].pbits, pVlanTciFilter[i].cfi, pVlanTciFilter[i].vlanId, i);
    }
    cmsMem_free(buf);

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_VLAN, "===> rutGpon_getTagFilters, vlanMeId=%d, numberOfEntries=%d, ret=%d\n", vlan->managedEntityId, vlan->numberOfEntries, ret);

    return ret;
}

CmsRet rutGpon_getDscpToPbitFromMapperServiceProfile
    (const MapperServiceProfileObject *mapper,
     OmciDscpPbitMappingInfo_t *dscpToPbitInfo)
{
    CmsRet ret = CMSRET_SUCCESS;
    UINT8 *buf = NULL;
    UINT32 bufSize = 0;

    dscpToPbitInfo->unmarkedFrameOption = mapper->unmarkedFrameOption;
    dscpToPbitInfo->defaultPBitMarking = mapper->defaultPBitMarking;

    ret = cmsUtl_hexStringToBinaryBuf
              ((char *)mapper->dscpToPBitMapping, &buf, &bufSize);
    if (ret == CMSRET_SUCCESS)
    {
        if (bufSize == OMCI_DSCP_PBIT_MAPPING_SIZE)
        {
            mapDscpListToPbitList(buf, dscpToPbitInfo->dscpToPbit);
        }
        else
        {
            dscpToPbitInfo->unmarkedFrameOption = OMCI_UNMARKED_FRAME_NONE;
            ret = CMSRET_INVALID_PARAM_VALUE;
        }
        CMSMEM_FREE_BUF_AND_NULL_PTR(buf);
    }

    return ret;
}

CmsRet rutGpon_getDscpToPbitFromExtendedVlanTaggingOperationConfigurationData
    (const ExtendedVlanTaggingOperationConfigurationDataObject *xVlan,
     OmciDscpPbitMappingInfo_t *dscpToPbitInfo)
{
    CmsRet ret = CMSRET_SUCCESS;
    UINT8 *buf = NULL;
    UINT32 bufSize = 0;

    dscpToPbitInfo->unmarkedFrameOption = OMCI_UNMARKED_FRAME_DSCP_TO_PBIT;
    dscpToPbitInfo->defaultPBitMarking = OMCI_PRIO_VAL_MAX;

    ret = cmsUtl_hexStringToBinaryBuf
              ((char *)xVlan->dscpToPBitMapping, &buf, &bufSize);
    if (ret == CMSRET_SUCCESS)
    {
        if (bufSize == OMCI_DSCP_PBIT_MAPPING_SIZE)
        {
            mapDscpListToPbitList(buf, dscpToPbitInfo->dscpToPbit);
        }
        else
        {
            dscpToPbitInfo->unmarkedFrameOption = OMCI_UNMARKED_FRAME_NONE;
            ret = CMSRET_INVALID_PARAM_VALUE;
        }
        CMSMEM_FREE_BUF_AND_NULL_PTR(buf);
    }

    return ret;
}

UINT8 rutGpon_isDefaultRuleAction(OmciExtVlanTagOper_t *pExtVlanTag)
{
    UINT8 ret = FALSE;

    if (pExtVlanTag->outer.filter.vlanId == OMCI_FILTER_VLANID_NONE &&
        pExtVlanTag->inner.filter.vlanId == OMCI_FILTER_VLANID_NONE &&
        pExtVlanTag->etherType == OMCI_FILTER_ETHER_NONE &&
        pExtVlanTag->removeType == OMCI_TREATMENT_REMOVE_NONE &&
        pExtVlanTag->outer.treatment.prio == OMCI_FILTER_IGNORE &&
        pExtVlanTag->inner.treatment.prio == OMCI_FILTER_IGNORE)
    {
        if (pExtVlanTag->outer.filter.prio == OMCI_FILTER_IGNORE &&
            pExtVlanTag->inner.filter.prio == OMCI_FILTER_IGNORE )
        {
            ret = TRUE;
        }
        else if (pExtVlanTag->outer.filter.prio == OMCI_FILTER_IGNORE &&
                 pExtVlanTag->inner.filter.prio == OMCI_FILTER_PRIO_DEFAULT)
        {
            ret = TRUE;
        }
        else if (pExtVlanTag->outer.filter.prio == OMCI_FILTER_PRIO_DEFAULT &&
                 pExtVlanTag->inner.filter.prio == OMCI_FILTER_PRIO_DEFAULT )
        {
            ret = TRUE;
        }
    }

    return ret;
}

void rutGpon_convertVlanToXVlan
    (const OmciVlanTagOperInfo_t *pTagOperInfo,
     OmciExtVlanTagOper_t *pExtVlanTagOper)
{
    UINT32 idx = 0;

    // if untagged frame then insert 1 full tag
    memset(&pExtVlanTagOper[idx], 0, sizeof(OmciExtVlanTagOper_t));
    pExtVlanTagOper[idx].outer.filter.prio = 15;
    pExtVlanTagOper[idx].outer.filter.vlanId = 4096;
    pExtVlanTagOper[idx].inner.filter.prio = 15;
    pExtVlanTagOper[idx].inner.filter.vlanId = 4096;
    pExtVlanTagOper[idx].etherType = OMCI_FILTER_ETHER_NONE;
    pExtVlanTagOper[idx].removeType = OMCI_TREATMENT_REMOVE_NONE;
    pExtVlanTagOper[idx].outer.treatment.prio = 15;
    pExtVlanTagOper[idx].inner.treatment.prio = pTagOperInfo->upstreamTci.pbits;
    pExtVlanTagOper[idx].inner.treatment.vlanId = pTagOperInfo->upstreamTci.vlanId;
    // set inner tpid = output tpid, de = 0
    if (pTagOperInfo->upstreamTci.cfi == 0)
        pExtVlanTagOper[idx].inner.treatment.tpid_de = 6;
    // set inner tpid = output tpid, de = 1
    else
        pExtVlanTagOper[idx].inner.treatment.tpid_de = 7;
    pExtVlanTagOper[idx].filterType = OMCI_FILTER_TYPE_UNTAG;

    idx++;

    memset(&pExtVlanTagOper[idx], 0, sizeof(OmciExtVlanTagOper_t));
    pExtVlanTagOper[idx].outer.filter.prio = 15;
    pExtVlanTagOper[idx].outer.filter.vlanId = 4096;
    pExtVlanTagOper[idx].inner.filter.prio = 8;
    pExtVlanTagOper[idx].inner.filter.vlanId = 4096;
    pExtVlanTagOper[idx].etherType = OMCI_FILTER_ETHER_NONE;
    if (pTagOperInfo->upstreamMode == OMCI_VLAN_UPSTREAM_TAG)
        // if single tagged frame then modify 1 full tag
        pExtVlanTagOper[idx].removeType = OMCI_TREATMENT_REMOVE_OUTER;
    else
        // if single tagged frame then insert 1 full tag
        pExtVlanTagOper[idx].removeType = OMCI_TREATMENT_REMOVE_NONE;
    pExtVlanTagOper[idx].outer.treatment.prio = 15;
    pExtVlanTagOper[idx].inner.treatment.prio = pTagOperInfo->upstreamTci.pbits;
    pExtVlanTagOper[idx].inner.treatment.vlanId = pTagOperInfo->upstreamTci.vlanId;
    // set inner tpid = output tpid, de = 0
    if (pTagOperInfo->upstreamTci.cfi == 0)
        pExtVlanTagOper[idx].inner.treatment.tpid_de = 6;
    // set inner tpid = output tpid, de = 1
    else
        pExtVlanTagOper[idx].inner.treatment.tpid_de = 7;
    pExtVlanTagOper[idx].filterType = OMCI_FILTER_TYPE_SINGLE_TAG;
}

int rutGpon_vlanCtl_createVlanInterface(char *realDevName,
  unsigned int vlanDevId, int isRouted __attribute__((unused)), int isMulticast __attribute__((unused)))
{
#ifdef DESKTOP_LINUX
    char vlanDevName[BUFLEN_128];
    int ret;

    sprintf(vlanDevName, "%s.%d", realDevName, vlanDevId);
    ret = rutGpon_vlanIf_addIf(realDevName);
    ret = (ret < 0) ? ret : rutGpon_vlanIf_addIf(vlanDevName);
    omciDebugPrint(OMCI_DEBUG_MODULE_VLAN,
      "===> vlanCtl_createVlanInterface(), "
      "isRouted=%d, isMulticast=%d, devIfc=%s, vlanIfc=%s\n",
      isRouted, isMulticast, realDevName, vlanDevName);

    return ret;
#else /* DESKTOP_LINUX */
    return vlanCtl_createVlanInterface(realDevName,
        vlanDevId, isRouted, isMulticast);
#endif /* DESKTOP_LINUX */
}

int rutGpon_vlanCtl_createVlanInterfaceByName(char *realDevName,
  char *vlanDevName, int isRouted __attribute__((unused)), int isMulticast __attribute__((unused)))
{
#ifdef DESKTOP_LINUX
    int ret;

    omciDebugPrint(OMCI_DEBUG_MODULE_VLAN,
      "===> vlanCtl_createVlanInterfaceByName(), "
      "isRouted=%d, isMulticast=%d, devIfc=%s, vlanIfc=%s\n",
      isRouted, isMulticast, realDevName, vlanDevName);

    ret = rutGpon_vlanIf_addIf(realDevName);
    ret = (ret < 0) ? ret : rutGpon_vlanIf_addIf(vlanDevName);

    return ret;
#else /* DESKTOP_LINUX */
    return vlanCtl_createVlanInterfaceByName(realDevName,
        vlanDevName, isRouted, isMulticast);
#endif /* DESKTOP_LINUX */
}

int rutGpon_vlanCtl_deleteVlanInterface(char *vlanDevName)
{
#ifdef DESKTOP_LINUX
    omciDebugPrint(OMCI_DEBUG_MODULE_VLAN,
      "===> vlanCtl_deleteVlanInterface(), devIfc=%s\n",
      vlanDevName);
    return rutGpon_vlanIf_delete(vlanDevName);
#else /* DESKTOP_LINUX */
    return vlanCtl_deleteVlanInterface(vlanDevName);
#endif /* DESKTOP_LINUX */
}


/* Interface file routines for desktop Linux emulation. */

int rutGpon_vlanIf_addIf(char *ifname)
{
    char line[BUFLEN_128];
    FILE *fp;
    char *buf;
    UBOOL8 found = FALSE;

    if (!(fp = fopen(VLANIF_FILE_NAME, "a+")))
    {
        cmsLog_error("Failed to open %s for write", VLANIF_FILE_NAME);
        return -1;
    }

    while (fgets(line, BUFLEN_128, fp))
    {
        buf = line;
        while (*buf == ' ')
        {
            buf++;
        }

        if (strncmp(ifname, buf, strlen(ifname)) == 0)
        {
            cmsLog_notice("Found if name:%s in %s\n", ifname, VLANIF_FILE_NAME);
            found = TRUE;
        }
    }

    if (found != TRUE)
    {
        fprintf(fp, "%s\n", ifname);
    }

    fclose(fp);
    return (int)found;
}

int rutGpon_vlanIf_search( const char * ifname)
{
    char line[BUFLEN_128];
    FILE *fp;
    char *buf;
    UBOOL8 found = FALSE;

    if (!(fp = fopen(VLANIF_FILE_NAME, "r")))
    {
        cmsLog_error("Failed to open %s for read", VLANIF_FILE_NAME);
        return -1;
    }

    while (fgets(line, BUFLEN_128, fp))
    {
        buf = line;
        while (*buf == ' ')
        {
            buf++;
        }

        if (strncmp(ifname, buf, strlen(ifname)) == 0)
        {
            cmsLog_notice("Found if name:%s in %s\n", ifname, VLANIF_FILE_NAME);
            found = TRUE;
        }
    }

    if (found != TRUE)
    {
        cmsLog_debug("Not found interface name:%s in %s\n",
          ifname, VLANIF_FILE_NAME);
    }

    fclose(fp);
    return (int)found;
}

int rutGpon_vlanIf_delete(char *ifname)
{
    char line[BUFLEN_128];
    FILE *fpo;
    FILE *fpt;
    char *buf;
    UBOOL8 found = FALSE;

    if (!(fpo = fopen(VLANIF_FILE_NAME, "r")))
    {
        cmsLog_error("Failed to open %s for read", VLANIF_FILE_NAME);
        return -1;
    }

    if (!(fpt = fopen(VLANIF_TEMP_NAME, "w")))
    {
        fclose(fpo);
        cmsLog_error("Failed to open %s for write", VLANIF_TEMP_NAME);
        return -1;
    }

    while (fgets(line, BUFLEN_128, fpo))
    {
        buf = line;
        while (*buf == ' ')
        {
            buf++;
        }

        if (strncmp(ifname, buf, strlen(ifname)) != 0)
        {
            fprintf(fpt, "%s", buf);
        }
        else
        {
            found = TRUE;
        }
    }

    fclose(fpo);
    fclose(fpt);

    remove(VLANIF_FILE_NAME);
    rename(VLANIF_TEMP_NAME, VLANIF_FILE_NAME);
    return (int)found;
}

#endif /* DMP_X_ITU_ORG_GPON_1 */
