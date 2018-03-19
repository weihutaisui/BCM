/***********************************************************************
 *
 *  Copyright (c) 2007 Broadcom
 *  All Rights Reserved
 *
 * <:label-BRCM:2007:proprietary:omcid
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
:>
 *
 ************************************************************************/

/*****************************************************************************
*    Description:
*
*      OMCI table attribute related functions.
*
*****************************************************************************/


/* ---- Include Files ----------------------------------------------------- */

#include "owsvc_api.h"
#include "ctlutils_api.h"
#include "me_handlers.h"
#include "omcid_data.h"

#include "omcid_me.h"
#include "omcid_priv.h"

/* ---- Private Constants and Types --------------------------------------- */


/* ---- Macro API definitions --------------------------------------------- */


/* ---- Private Function Prototypes --------------------------------------- */

static void clearRxFrameVlanTagOperRuleUnusedFields(char *ruleP);
static CmsRet getTableAttribute(UINT32 entrySize, const UINT32 oid,
  const InstanceIdStack *pIidStack, void **obj, UINT16 *objSize);
static CmsRet setDynamicAccessControlList(void *obj, InstanceIdStack
  *pIidStack);
static CmsRet setStaticAccessControlList(void *obj, InstanceIdStack
  *pIidStack);


/* ---- Public Variables -------------------------------------------------- */


/* ---- Private Variables ------------------------------------------------- */


/* ---- Functions --------------------------------------------------------- */

/*****************************************************************************
*  FUNCTION:  clearRxFrameVlanTagOperRuleUnusedFields
*  PURPOSE:   Set unused fields in RxFrameVlanTagOperRule to 0 before 
*             comparison.
*  PARAMETERS:
*      ruleP - pointer to the EXT VLAN RxFrameVlanTagOperRule.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
static void clearRxFrameVlanTagOperRuleUnusedFields(char *ruleP)
{
    UINT16 i = 0;

    if ((ruleP[0] == 'f' || ruleP[0] == 'F') ||
      (ruleP[0] == 'e' || ruleP[0] == 'E'))
    {
        for (i = 1; i < 8; i++)
        {
            ruleP[i] = '0';
        }
    }
    else
    {
        ruleP[5] = '0';
        ruleP[6] = '0';
        ruleP[7] = '0';
    }

    if ((ruleP[8] == 'f' || ruleP[8] == 'F') ||
      (ruleP[8] == 'e' || ruleP[8] == 'E'))
    {
        for (i = 9; i < 15; i++)
        {
            ruleP[i] = '0';
        }
    }
    else
    {
        ruleP[13] = '0';
        ruleP[14] = '0';
    }
}

/*****************************************************************************
*  FUNCTION:  getTableAttribute
*  PURPOSE:   Get table attribute command handler.
*  PARAMETERS:
*      entrySize - table attribute size.
*      oid - object id.
*      pIidStack - pointer to InstanceIdStack.
*      obj - pointer to the OMCI object.
*      objSize - returned object size.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
static CmsRet getTableAttribute(UINT32 entrySize, const UINT32 oid,
  const InstanceIdStack *pIidStack, void **obj, UINT16 *objSize)
{
    CmsRet ret = CMSRET_SUCCESS;
    InstanceIdStack iid = EMPTY_INSTANCE_ID_STACK;
    void *table = NULL;
    char *hexStr = NULL;
    UINT8 *buf = NULL, *entry = NULL;
    UINT16 i = 0, j = 0;
    UINT32 bufLen = 0;

    *objSize = 0;

    while (_cmsObj_getNextInSubTree(oid, pIidStack, &iid, (void**)&table) == CMSRET_SUCCESS)
    {
        i++;
        _cmsObj_free((void **)&table);
    }

    if (i > 0)
    {
        *objSize = (entrySize/2)*i;
        buf = *obj = cmsMem_alloc(*objSize, ALLOC_ZEROIZE);
        if (*obj != NULL)
        {
            i = 0;
            INIT_INSTANCE_ID_STACK(&iid);
            while (_cmsObj_getNextInSubTree(oid, pIidStack, &iid, (void**) &table) == CMSRET_SUCCESS)
            {
                switch (oid)
                {
                    case MDMOID_DYNAMIC_ACCESS_CONTROL_LIST_TABLE:
                        hexStr = ((DynamicAccessControlListTableObject *)table)->dynamicAccessControlEntry;
                        break;
                    case MDMOID_STATIC_ACCESS_CONTROL_LIST_TABLE:
                        hexStr = ((StaticAccessControlListTableObject *)table)->staticAccessControlEntry;
                        break;
                    case MDMOID_LOST_GROUPS_LIST_TABLE:
                        hexStr = ((LostGroupsListTableObject *)table)->lostGroupsListEntry;
                        break;
                    case MDMOID_ACTIVE_GROUPS_LIST_TABLE:
                        hexStr = ((ActiveGroupsListTableObject *)table)->activeGroupsListEntry;
                        break;
                    case MDMOID_GEM_INTERWORKING_TP_MULTICAST_ADDRESS_TABLE:
                        hexStr = ((GemInterworkingTpMulticastAddressTableObject *)table)->multicastAddressEntry;
                        break;
                    case MDMOID_RECEIVED_FRAME_VLAN_TAGGING_OPERATION_TABLE:
                        hexStr = ((ReceivedFrameVlanTaggingOperationTableObject *)table)->taggingRule;
                        break;
                    case MDMOID_MAC_FILTER_TABLE:
                        hexStr = ((MacFilterTableObject *)table)->macFilterEntry;
                        break;
                    case MDMOID_BRIDGE_TABLE:
                        hexStr = ((BridgeTableObject *)table)->bridgeEntry;
                        break;
                    case MDMOID_IPV6_CURRENT_ADDRESS_TABLE:
                        hexStr = ((Ipv6CurrentAddressTableObject *)table)->currentAddressEntry;
                        break;
                    case MDMOID_IPV6_CURRENT_DEFAULT_ROUTER_TABLE:
                        hexStr = ((Ipv6CurrentDefaultRouterTableObject *)table)->currentDefaultRouterEntry;
                        break;
                    case MDMOID_IPV6_CURRENT_DNS_TABLE:
                        hexStr = ((Ipv6CurrentDnsTableObject *)table)->currentDnsEntry;
                        break;
                    case MDMOID_IPV6_CURRENT_ONLINK_PREFIX_TABLE:
                        hexStr = ((Ipv6CurrentOnlinkPrefixTableObject *)table)->currentOnlinkPrefixTableEntry;
                        break;
                    case MDMOID_BROADCAST_KEY_TABLE:
                        hexStr = ((BroadcastKeyTableObject *)table)->broadcastKey;
                        break;
                    default :
                        hexStr = NULL;
                        break;
                }
                if (hexStr != NULL)
                {
                    ret = cmsUtl_hexStringToBinaryBuf(hexStr, &entry, &bufLen);
                    if (ret == CMSRET_SUCCESS)
                    {
                        for (j = 0; j < bufLen; j++)
                            buf[(bufLen*i)+j] = entry[j];
                        i++;
                        cmsMem_free(entry);
                    }
                }
                _cmsObj_free((void **)&table);
            }
        }
        else
        {
            *objSize = 0;
            ret = CMSRET_RESOURCE_EXCEEDED;
        }
    }
    else
        ret = CMSRET_INVALID_PARAM_VALUE;

    return ret;
}

/*****************************************************************************
*  FUNCTION:  getOmciMe
*  PURPOSE:   Get/Get-Next command handler.
*  PARAMETERS:
*      obj - pointer to the OMCI object.
*      objSize - returned object size.
*      pIidStack - pointer to InstanceIdStack.
*      attrMask - attribute mask.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet getOmciMe(void **obj, UINT16 *objSize,
  InstanceIdStack *pIidStack __attribute__((unused)), UINT16 attrMask)
{
    CmsRet ret = CMSRET_SUCCESS;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    OmciObject *omciObj = NULL;
    UINT8 *meTypes = NULL, *msgTypes = NULL, *buf = NULL;
    UINT32 meTypesSize = 0, msgTypesSize = 0;

    if ((attrMask & OMCI_ME_ATTRIB_1) == 0 &&
        (attrMask & OMCI_ME_ATTRIB_2) == 0)
    {
        cmsLog_error("invalid attribute mask 0x%04x", attrMask);
        return CMSRET_INVALID_PARAM_NAME;
    }

    if ((ret = _cmsObj_get(MDMOID_OMCI, &iidStack, 0, (void **)&omciObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("get of object id %d failed, ret=%d", MDMOID_OMCI, ret);
        return ret;
    }

    if ((attrMask & OMCI_ME_ATTRIB_1) != 0)
    {
        (void)cmsUtl_hexStringToBinaryBuf(omciObj->meTypeTable, &meTypes, &meTypesSize);
        *objSize += meTypesSize;
    }

    if ((attrMask & OMCI_ME_ATTRIB_2) != 0)
    {
        (void)cmsUtl_hexStringToBinaryBuf(omciObj->messageTypeTable, &msgTypes, &msgTypesSize);
        *objSize += msgTypesSize;
    }

    _cmsObj_free((void **)&omciObj);

    buf = *obj = cmsMem_alloc(*objSize, ALLOC_ZEROIZE);
    if (buf != NULL)
    {
        UINT32 i = 0, j = 0;
        if ((attrMask & OMCI_ME_ATTRIB_1) != 0)
        {
            for (i = 0, j = 0; i < meTypesSize; i++, j++)
            {
                buf[j] = meTypes[i];
            }
        }
        if ((attrMask & OMCI_ME_ATTRIB_2) != 0)
        {
            for (i = 0; i < msgTypesSize; i++, j++)
            {
                buf[j] = msgTypes[i];
            }
        }
    }
    else
    {
        *objSize = 0;
        ret = CMSRET_RESOURCE_EXCEEDED;
    }

    CMSMEM_FREE_BUF_AND_NULL_PTR(meTypes);
    CMSMEM_FREE_BUF_AND_NULL_PTR(msgTypes);

    return ret;
}

/*****************************************************************************
*  FUNCTION:  getMulticastOperationsProfile
*  PURPOSE:   Get/Get-Next command handler.
*  PARAMETERS:
*      obj - pointer to the OMCI object.
*      objSize - returned object size.
*      pIidStack - pointer to InstanceIdStack.
*      attrMask - attribute mask.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet getMulticastOperationsProfile(void **obj, UINT16 *objSize,
  InstanceIdStack *pIidStack, UINT16 attrMask)
{
    CmsRet ret = CMSRET_INVALID_PARAM_NAME;

    if ((attrMask & OMCI_ME_ATTRIB_7) != 0)
    {
        ret = getTableAttribute(OMCI_ENTRY_SIZE_48,
          MDMOID_DYNAMIC_ACCESS_CONTROL_LIST_TABLE, pIidStack, obj, objSize);
    }
    else if ((attrMask & OMCI_ME_ATTRIB_8) != 0)
    {
        ret = getTableAttribute(OMCI_ENTRY_SIZE_48,
          MDMOID_STATIC_ACCESS_CONTROL_LIST_TABLE, pIidStack, obj, objSize);
    }
    else if ((attrMask & OMCI_ME_ATTRIB_9) != 0)
    {
        ret = getTableAttribute(OMCI_ENTRY_SIZE_20,
          MDMOID_LOST_GROUPS_LIST_TABLE, pIidStack, obj, objSize);
    }
    else
    {
        cmsLog_error("invalid attribute mask 0x%04x", attrMask);
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  setDynamicAccessControlList
*  PURPOSE:   Set table attribute command handler.
*  PARAMETERS:
*      obj - pointer to the OMCI object.
*      pIidStack - pointer to InstanceIdStack.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
static CmsRet setDynamicAccessControlList(void *obj, InstanceIdStack *pIidStack)
{
    CmsRet ret = CMSRET_SUCCESS;
    InstanceIdStack iid = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidPrev = EMPTY_INSTANCE_ID_STACK;
    DynamicAccessControlListTableObject *dacEntry = NULL;
    MulticastOperationsProfileObject *pObject = (MulticastOperationsProfileObject*)obj;
    UINT16 i = 0, entryNum = 0;
    UINT32 dacSize = 0;
    UINT8 *dac = NULL, oper = 0;
    UBOOL8 found = FALSE;
    char *hexString = NULL;
    char singleHexString[OMCI_ENTRY_SIZE_48 + 1];

    if (pObject == NULL ||
        pObject->dynamicAccessControlListTable == NULL ||
        strcmp(pObject->dynamicAccessControlListTable, "00") == 0 ||
        strcmp(pObject->dynamicAccessControlListTable, "") == 0)
        return ret;

    entryNum = strlen(pObject->dynamicAccessControlListTable) / OMCI_ENTRY_SIZE_48;

    for (i = 0; i < entryNum; i++)
    {
        found = FALSE;
        // convert dynamicAccessControlListTable to dac
        strncpy(singleHexString,
          &pObject->dynamicAccessControlListTable[i * OMCI_ENTRY_SIZE_48],
          OMCI_ENTRY_SIZE_48);
        singleHexString[OMCI_ENTRY_SIZE_48] = 0;
        ret = cmsUtl_hexStringToBinaryBuf((const char*)singleHexString, &dac, &dacSize);
        if (dacSize != (OMCI_ENTRY_SIZE_48/2))
        {
            if (ret == CMSRET_SUCCESS)
            {
                cmsMem_free(dac);
            }
            continue;
        }

        // operation is the two MSBs of table index field (first 2 bytes)
        oper = dac[0];
        oper = ((oper >> 6) & 0x03);
        // clear operation bits in dac
        dac[0] &= 0x3F;
        // convert dac back to dynamicAccessControlListTable
        (void)cmsUtl_binaryBufToHexString(dac, dacSize, &hexString);
        memcpy(&singleHexString, hexString, OMCI_ENTRY_SIZE_48);
        singleHexString[OMCI_ENTRY_SIZE_48] = 0;
        // free temporary memory
        cmsMem_free(hexString);
        cmsMem_free(dac);

        INIT_INSTANCE_ID_STACK(&iid);
        switch (oper)
        {
            case 1:
            case 2:
                // search for the given entry in the table
                while ((found == FALSE) &&
                       (_cmsObj_getNextInSubTree(MDMOID_DYNAMIC_ACCESS_CONTROL_LIST_TABLE,
                                                pIidStack, &iid,
                                                (void**)&dacEntry) == CMSRET_SUCCESS))
                {
                    // compare table index: first 2 bytes ==> first 4 characters
                    if (strncmp((const char*)singleHexString,
                                &dacEntry->dynamicAccessControlEntry[0], 4) == 0)
                    {
                        // the given entry exists in the table
                        found = TRUE;
                        // if operation is add then modify entry
                        if (oper == 1)
                        {
                            memcpy(dacEntry->dynamicAccessControlEntry,
                                   &singleHexString,
                                   OMCI_ENTRY_SIZE_48);
                            ret = _cmsObj_set(dacEntry, &iid);
                        }
                        // operation is delete
                        else
                            // do not call cmsMem_free(dacEntry->dynamicAccessControlEntry) since
                            // it is free later in cmsObj_free((void **)&dacEntry)
                            ret = _cmsObj_deleteInstance(MDMOID_DYNAMIC_ACCESS_CONTROL_LIST_TABLE, &iid);
                    }

                    _cmsObj_free((void**)&dacEntry);
                }

                // if not found and oepration is add then create new entry
                if (found == FALSE && oper == 1)
                {
                    memcpy(&iid, pIidStack, sizeof(InstanceIdStack));
                    // add given entry to the table
                    ret = _cmsObj_addInstance(MDMOID_DYNAMIC_ACCESS_CONTROL_LIST_TABLE, &iid);
                    if (ret == CMSRET_SUCCESS)
                    {
                        ret = _cmsObj_get(MDMOID_DYNAMIC_ACCESS_CONTROL_LIST_TABLE, &iid, 0, (void**)&dacEntry);
                        if (ret == CMSRET_SUCCESS)
                        {
                            cmsMem_free(dacEntry->dynamicAccessControlEntry);

                            dacEntry->dynamicAccessControlEntry = cmsMem_alloc(OMCI_ENTRY_SIZE_48+1,
                                                                      ALLOC_ZEROIZE);
                            if (dacEntry->dynamicAccessControlEntry == NULL)
                            {
                                cmsLog_error("failed to allocate memory");
                                ret = CMSRET_INTERNAL_ERROR;
                            }
                            else
                            {
                                memcpy(dacEntry->dynamicAccessControlEntry,
                                       &singleHexString,
                                       OMCI_ENTRY_SIZE_48);
                                ret = _cmsObj_set(dacEntry, &iid);
                            }
                            _cmsObj_free((void **)&dacEntry);
                        }
                        else
                            cmsLog_error("Could not get Dynamic Access Control List Table object, ret = %d", ret);
                    }
                    else
                        cmsLog_error("Could not add Dynamic Access Control List Table object, ret = %d", ret);
                }
                break;
            case 3:
                iidPrev = iid;
                // clear all entries in the table
                while (_cmsObj_getNextInSubTree(MDMOID_DYNAMIC_ACCESS_CONTROL_LIST_TABLE,
                                               pIidStack, &iid,
                                               (void**)&dacEntry) == CMSRET_SUCCESS)
                {
                    // do not call cmsMem_free(dacEntry->dynamicAccessControlEntry) since
                    // it is free later in cmsObj_free((void **)&dacEntry)
                    ret = _cmsObj_deleteInstance(MDMOID_DYNAMIC_ACCESS_CONTROL_LIST_TABLE, &iid);
                    // since this instance has been deleted, we want to set the iidStack to
                    // the previous instance, so that we can continue to do getNext.
                    iid = iidPrev;
                    _cmsObj_free((void**)&dacEntry);
                }
                break;
            default:
                break;
        }
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  setStaticAccessControlList
*  PURPOSE:   Set table attribute command handler.
*  PARAMETERS:
*      obj - pointer to the OMCI object.
*      pIidStack - pointer to InstanceIdStack.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
static CmsRet setStaticAccessControlList(void *obj, InstanceIdStack *pIidStack)
{
    CmsRet ret = CMSRET_SUCCESS;
    InstanceIdStack iid = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidPrev = EMPTY_INSTANCE_ID_STACK;
    StaticAccessControlListTableObject *sacEntry = NULL;
    MulticastOperationsProfileObject *pObject = (MulticastOperationsProfileObject*)obj;
    UINT16 i = 0, entryNum = 0;
    UINT32 sacSize = 0;
    UINT8 *sac = NULL, oper = 0;
    UBOOL8 found = FALSE;
    char *hexString = NULL;
    char singleHexString[OMCI_ENTRY_SIZE_48 + 1];

    if (pObject == NULL ||
        pObject->staticAccessControlListTable == NULL ||
        strcmp(pObject->staticAccessControlListTable, "00") == 0 ||
        strcmp(pObject->staticAccessControlListTable, "") == 0)
        return ret;

    entryNum = strlen(pObject->staticAccessControlListTable) / OMCI_ENTRY_SIZE_48;

    for (i = 0; i < entryNum; i++)
    {
        found = FALSE;
        // convert staticAccessControlListTable to sac
        strncpy(singleHexString,
          &pObject->dynamicAccessControlListTable[i * OMCI_ENTRY_SIZE_48],
          OMCI_ENTRY_SIZE_48);
        singleHexString[OMCI_ENTRY_SIZE_48] = 0;
        ret = cmsUtl_hexStringToBinaryBuf((const char*)&singleHexString, &sac, &sacSize);
        if (sacSize != (OMCI_ENTRY_SIZE_48/2))
        {
            if (ret == CMSRET_SUCCESS)
            {
                cmsMem_free(sac);
            }
            continue;
        }
        // operation is the two MSBs of table index field (first 2 bytes)
        oper = sac[0];
        oper = ((oper >> 6) & 0x03);
        // clear operation bits in sac
        sac[0] &= 0x3F;
        // convert sac back to staticAccessControlListTable
        (void)cmsUtl_binaryBufToHexString(sac, sacSize, &hexString);
        memcpy(&singleHexString, hexString, OMCI_ENTRY_SIZE_48);
        singleHexString[OMCI_ENTRY_SIZE_48] = 0;
        // free temporary memory
        cmsMem_free(hexString);
        cmsMem_free(sac);

        INIT_INSTANCE_ID_STACK(&iid);
        switch (oper)
        {
            case 1:
            case 2:
                // search for the given entry in the table
                while ((found == FALSE) &&
                       (_cmsObj_getNextInSubTree(MDMOID_STATIC_ACCESS_CONTROL_LIST_TABLE,
                                                pIidStack, &iid,
                                                (void**)&sacEntry) == CMSRET_SUCCESS))
                {
                    // compare table index: first 2 bytes ==> first 4 characters
                    if (strncmp((const char*)&singleHexString,
                                &sacEntry->staticAccessControlEntry[0], 4) == 0)
                    {
                        // the given entry exists in the table
                        found = TRUE;
                        // if operation is add then modify entry
                        if (oper == 1)
                        {
                            memcpy(sacEntry->staticAccessControlEntry,
                                   &singleHexString,
                                   OMCI_ENTRY_SIZE_48);
                            ret = _cmsObj_set(sacEntry, &iid);
                        }
                        // operation is delete
                        else
                            // do not call cmsMem_free(sacEntry->staticAccessControlEntry) since
                            // it is free later in _cmsObj_free((void **)&sacEntry)
                            ret = _cmsObj_deleteInstance(MDMOID_STATIC_ACCESS_CONTROL_LIST_TABLE, &iid);
                    }

                    _cmsObj_free((void **)&sacEntry);
                }

                // if not found and oepration is add then create new entry
                if (found == FALSE && oper == 1)
                {
                    memcpy(&iid, pIidStack, sizeof(InstanceIdStack));
                    // add given entry to the table
                    ret = _cmsObj_addInstance(MDMOID_STATIC_ACCESS_CONTROL_LIST_TABLE, &iid);
                    if (ret == CMSRET_SUCCESS)
                    {
                        ret = _cmsObj_get(MDMOID_STATIC_ACCESS_CONTROL_LIST_TABLE, &iid, 0, (void **) &sacEntry);
                        if (ret == CMSRET_SUCCESS)
                        {
                            cmsMem_free(sacEntry->staticAccessControlEntry);
                            sacEntry->staticAccessControlEntry = cmsMem_alloc(OMCI_ENTRY_SIZE_48+1,
                                                                      ALLOC_ZEROIZE);
                            if (sacEntry->staticAccessControlEntry == NULL)
                            {
                                cmsLog_error("failed to allocate memory");
                                ret = CMSRET_INTERNAL_ERROR;
                            }
                            else
                            {
                                memcpy(sacEntry->staticAccessControlEntry,
                                       &singleHexString,
                                       OMCI_ENTRY_SIZE_48);
                                ret = _cmsObj_set(sacEntry, &iid);
                            }
                            _cmsObj_free((void **)&sacEntry);
                        }
                        else
                            cmsLog_error("Could not get Static Access Control List Table object, ret = %d", ret);
                    }
                    else
                        cmsLog_error("Could not add Static Access Control List Table object, ret = %d", ret);
                }
                break;
            case 3:
                iidPrev = iid;
                // clear all entries in the table
                while (_cmsObj_getNextInSubTree(MDMOID_STATIC_ACCESS_CONTROL_LIST_TABLE,
                                               pIidStack, &iid,
                                               (void**)&sacEntry) == CMSRET_SUCCESS)
                {
                    // do not call cmsMem_free(sacEntry->staticAccessControlEntry) since
                    // it is free later in _cmsObj_free((void **)&sacEntry)
                    ret = _cmsObj_deleteInstance(MDMOID_STATIC_ACCESS_CONTROL_LIST_TABLE, &iid);
                    // since this instance has been deleted, we want to set the iidStack to
                    // the previous instance, so that we can continue to do getNext.
                    iid = iidPrev;
                    _cmsObj_free((void **)&sacEntry);
                }
                break;
            default:
                break;
        }
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  setMulticastOperationsProfile
*  PURPOSE:   Set table attribute command handler.
*  PARAMETERS:
*      mdmObj - pointer to the OMCI object.
*      pIidStack - pointer to InstanceIdStack.
*      attrMask - attribute mask.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet setMulticastOperationsProfile(void *mdmObj, InstanceIdStack
  *pIidStack, UINT16 attrMask)
{
    CmsRet ret = CMSRET_INVALID_PARAM_NAME;

    if ((attrMask & OMCI_ME_ATTRIB_7) != 0)
    {
        ret = setDynamicAccessControlList(mdmObj, pIidStack);
    }
    else if ((attrMask & OMCI_ME_ATTRIB_8) != 0)
    {
        ret = setStaticAccessControlList(mdmObj, pIidStack);
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  getMulticastSubscriberMonitor
*  PURPOSE:   Get/Get-Next command handler.
*  PARAMETERS:
*      obj - pointer to the OMCI object.
*      objSize - returned object size.
*      pIidStack - pointer to InstanceIdStack.
*      attrMask - attribute mask.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet getMulticastSubscriberMonitor(void **obj, UINT16 *objSize,
  InstanceIdStack *pIidStack, UINT16 attrMask)
{
    CmsRet ret = CMSRET_INVALID_PARAM_NAME;

    if ((attrMask & OMCI_ME_ATTRIB_5) != 0)
    {
        ret = getTableAttribute(OMCI_ENTRY_SIZE_48,
          MDMOID_ACTIVE_GROUPS_LIST_TABLE, pIidStack, obj, objSize);
    }
    else
    {
        cmsLog_error("invalid attribute mask 0x%04x", attrMask);
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  getMulticastGemInterworkingTp
*  PURPOSE:   Get/Get-Next command handler.
*  PARAMETERS:
*      obj - pointer to the OMCI object.
*      objSize - returned object size.
*      pIidStack - pointer to InstanceIdStack.
*      attrMask - attribute mask.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet getMulticastGemInterworkingTp(void **obj, UINT16 *objSize,
  InstanceIdStack *pIidStack, UINT16 attrMask)
{
    CmsRet ret = CMSRET_INVALID_PARAM_NAME;

    if ((attrMask & OMCI_ME_ATTRIB_9) != 0)
    {
        ret = getTableAttribute(OMCI_ENTRY_SIZE_24,
          MDMOID_GEM_INTERWORKING_TP_MULTICAST_ADDRESS_TABLE, pIidStack, 
          obj, objSize);
    }
    else
    {
        cmsLog_error("invalid attribute mask 0x%04x", attrMask);
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  setMulticastGemInterworkingTp
*  PURPOSE:   Set table attribute command handler.
*  PARAMETERS:
*      mdmObj - pointer to the OMCI object.
*      pIidStack - pointer to InstanceIdStack.
*      attrMask - attribute mask.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet setMulticastGemInterworkingTp(void *mdmObj,
  InstanceIdStack *pIidStack, UINT16 attrMask)
{
    CmsRet ret = CMSRET_SUCCESS;
    InstanceIdStack iid = EMPTY_INSTANCE_ID_STACK;
    GemInterworkingTpMulticastAddressTableObject *addrTblEntry = NULL;
    MulticastGemInterworkingTpObject *pObject = (MulticastGemInterworkingTpObject*)mdmObj;
    UINT16 i = 0, j = 0, entryNum = 0;
    UINT32 addrSize = 0;
    UINT8 *addr = NULL;
    UBOOL8 found = FALSE, delete = FALSE;
    char entry[OMCI_ENTRY_SIZE_24+1];

    if ((attrMask & OMCI_ME_ATTRIB_9) == 0 ||
        pObject == NULL ||
        pObject->multicastAddressField == NULL ||
        strcmp(pObject->multicastAddressField, "00") == 0 ||
        strcmp(pObject->multicastAddressField, "") == 0)
        return ret;

    entryNum = strlen(pObject->multicastAddressField) / OMCI_ENTRY_SIZE_24;

    for (i = 0; i < entryNum; i++)
    {
        found = FALSE;
        memset(entry, 0, OMCI_ENTRY_SIZE_24+1);
        strncpy(entry, &pObject->multicastAddressField[i*OMCI_ENTRY_SIZE_24],
          OMCI_ENTRY_SIZE_24);
        ret = cmsUtl_hexStringToBinaryBuf(entry, &addr, &addrSize);
        if (addrSize != (OMCI_ENTRY_SIZE_24/2))
        {
            if (ret == CMSRET_SUCCESS)
            {
                cmsMem_free(addr);
            }
            continue;
        }

        for (j = 4; j < addrSize; j++)
        {
            if (addr[j] != 0x00)
                break;
        }
        delete = (j == addrSize) ? TRUE : FALSE;
        cmsMem_free(addr);

        INIT_INSTANCE_ID_STACK(&iid);
        // search for the given entry in the table
        while (_cmsObj_getNextInSubTree(
          MDMOID_GEM_INTERWORKING_TP_MULTICAST_ADDRESS_TABLE,
          pIidStack, &iid, (void**)&addrTblEntry) == CMSRET_SUCCESS)
        {
            // the given entry exists in the table
            if (strncmp((const char*)&entry,
              &addrTblEntry->multicastAddressEntry[0], 8) == 0)
            {
                found = TRUE;
                // if either addresses is not zero then modify entry
                if (delete == FALSE)
                {
                    memcpy(addrTblEntry->multicastAddressEntry,
                      &entry,
                      OMCI_ENTRY_SIZE_24);
                    ret = _cmsObj_set(addrTblEntry, &iid);
                }
                // if start range address and stop range address are zero then delete entry
                else
                {
                    // do not call cmsMem_free(addrTblEntry->multicastAddressEntry) since
                    // it is free later in cmsObj_free((void **)&addrTblEntry)
                    ret = _cmsObj_deleteInstance(
                      MDMOID_GEM_INTERWORKING_TP_MULTICAST_ADDRESS_TABLE, &iid);
                }
            }
            _cmsObj_free((void**)&addrTblEntry);
            if (found == TRUE)
                break;
        }

        // the given entry does not exist in the table
        if (found == FALSE && delete == FALSE)
        {
            memcpy(&iid, pIidStack, sizeof(InstanceIdStack));
            // add given entry to the table
            ret = _cmsObj_addInstance(
              MDMOID_GEM_INTERWORKING_TP_MULTICAST_ADDRESS_TABLE, &iid);
            if (ret == CMSRET_SUCCESS)
            {
                ret = _cmsObj_get(MDMOID_GEM_INTERWORKING_TP_MULTICAST_ADDRESS_TABLE,
                  &iid, 0, (void**)&addrTblEntry);
                if (ret == CMSRET_SUCCESS)
                {
                    cmsMem_free(addrTblEntry->multicastAddressEntry);
                    addrTblEntry->multicastAddressEntry =
                      cmsMem_alloc(OMCI_ENTRY_SIZE_24 + 1, ALLOC_ZEROIZE);
                    if (addrTblEntry->multicastAddressEntry == NULL)
                    {
                        cmsLog_error("failed to allocate memory");
                        ret = CMSRET_INTERNAL_ERROR;
                    }
                    else
                    {
                        memcpy(addrTblEntry->multicastAddressEntry, &entry,
                          OMCI_ENTRY_SIZE_24);
                        ret = _cmsObj_set(addrTblEntry, &iid);
                    }
                    _cmsObj_free((void**)&addrTblEntry);
                }
                else
                    cmsLog_error("Could not get GEM IWTP Multicast Address Table object, ret = %d", ret);
            }
            else
                cmsLog_error("Could not add GEM IWTP Multicast Address Table object, ret = %d", ret);
        }
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  getExtendedVlanTaggingOperationConfigurationData
*  PURPOSE:   Get/Get-Next command handler.
*  PARAMETERS:
*      obj - pointer to the OMCI object.
*      objSize - returned object size.
*      pIidStack - pointer to InstanceIdStack.
*      attrMask - attribute mask.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet getExtendedVlanTaggingOperationConfigurationData(void **obj,
  UINT16 *objSize, InstanceIdStack *pIidStack, UINT16 attrMask)
{
    CmsRet ret = CMSRET_INVALID_PARAM_NAME;

    if ((attrMask & OMCI_ME_ATTRIB_6) != 0)
    {
        ret = getTableAttribute(OMCI_ENTRY_SIZE_32,
          MDMOID_RECEIVED_FRAME_VLAN_TAGGING_OPERATION_TABLE,
          pIidStack, obj, objSize);
    }
    else
    {
        cmsLog_error("Invalid attribute mask 0x%x", attrMask);
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  setExtendedVlanTaggingOperationConfigurationData
*  PURPOSE:   Set table attribute command handler.
*  PARAMETERS:
*      mdmObj - pointer to the OMCI object.
*      pIidStack - pointer to InstanceIdStack.
*      attrMask - attribute mask.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet setExtendedVlanTaggingOperationConfigurationData(void *mdmObj,
  InstanceIdStack *pIidStack, UINT16 attrMask)
{
    CmsRet ret = CMSRET_SUCCESS;
    InstanceIdStack iid = EMPTY_INSTANCE_ID_STACK;
    ReceivedFrameVlanTaggingOperationTableObject *tagTblEntry = NULL;
    ExtendedVlanTaggingOperationConfigurationDataObject *pObject =
      (ExtendedVlanTaggingOperationConfigurationDataObject*)mdmObj;
    UBOOL8 found = FALSE, delete = FALSE;
    UINT16 i = 0;
    UINT16 j = 0;
    UINT16 entryNum = 0;
    char singleHexString[OMCI_ENTRY_SIZE_32 + 1];
    UINT32 addrSize = 0;
    UINT8 *addr = NULL;
    char ruleNew[OMCI_ENTRY_SIZE_32+1];
    char ruleOld[OMCI_ENTRY_SIZE_32+1];

    if ((attrMask & OMCI_ME_ATTRIB_6) == 0 ||
        pObject == NULL ||
        pObject->receivedFrameVlanTaggingOperationTable == NULL ||
        strcmp(pObject->receivedFrameVlanTaggingOperationTable, "00") == 0 ||
        strcmp(pObject->receivedFrameVlanTaggingOperationTable, "") == 0)
    {
        /*
         * Not a real issue. The ONU may receive a SET command on other
         * non table attributes.
         */
        cmsLog_debug("No table attribute");
        return ret;
    }

    entryNum = strlen(pObject->receivedFrameVlanTaggingOperationTable)
      / OMCI_ENTRY_SIZE_32;

    for (j = 0; j < entryNum; j++)
    {
        found = FALSE;
        addrSize = 0;
        strncpy(singleHexString,
          &pObject->receivedFrameVlanTaggingOperationTable[j * OMCI_ENTRY_SIZE_32],
          OMCI_ENTRY_SIZE_32);
        singleHexString[OMCI_ENTRY_SIZE_32] = 0;

        ret = cmsUtl_hexStringToBinaryBuf((const char*)singleHexString,
          &addr, &addrSize);
        if (addrSize != (OMCI_ENTRY_SIZE_32/2))
        {
            if (ret == CMSRET_SUCCESS)
            {
                cmsMem_free(addr);
            }
            continue;
        }

        // if the last 8 bytes are 0xFF then delete this entry
        for (i = 8; i < addrSize; i++)
        {
            if (addr[i] != 0xFF)
            {
                break;
            }
        }

        delete = (i == addrSize) ? TRUE : FALSE;

        strcpy(ruleNew, singleHexString);
        clearRxFrameVlanTagOperRuleUnusedFields(&ruleNew[0]);
        cmsMem_free(addr);

        // search for the given entry in the table
        INIT_INSTANCE_ID_STACK(&iid);
        while (_cmsObj_getNextInSubTree(MDMOID_RECEIVED_FRAME_VLAN_TAGGING_OPERATION_TABLE,
                                       pIidStack, &iid,
                                       (void**)&tagTblEntry) == CMSRET_SUCCESS)
        {
            strcpy(ruleOld, tagTblEntry->taggingRule);
            clearRxFrameVlanTagOperRuleUnusedFields(&ruleOld[0]);

            // compare the first 8 bytes of entry that are identified table entry
            // note: 1 byte is stored as 2 ascii characters in OMCI database ==>
            // compare 16 bytes, if match then the given entry exists in the table
            if (strncmp(ruleNew, ruleOld, 16) == 0)
            {
                found = TRUE;
                if (delete == FALSE)
                {
                    memcpy(tagTblEntry->taggingRule,
                           &singleHexString,
                           OMCI_ENTRY_SIZE_32);
                    ret = _cmsObj_set(tagTblEntry, &iid);
                }
                else
                {
                    // do not call cmsMem_free(tagTblEntry->taggingRule) since
                    // it is free later in _cmsObj_free((void **)&tagTblEntry)
                    ret = _cmsObj_deleteInstance
                        (MDMOID_RECEIVED_FRAME_VLAN_TAGGING_OPERATION_TABLE, &iid);
                }
            }
            _cmsObj_free((void**)&tagTblEntry);
            if (found == TRUE)
            {
                break;
            }
        }

        // the given entry does not exist in the table
        if (found == FALSE && delete == FALSE)
        {
            memcpy(&iid, pIidStack, sizeof(InstanceIdStack));
            // add given entry to the table
            ret = _cmsObj_addInstance
                (MDMOID_RECEIVED_FRAME_VLAN_TAGGING_OPERATION_TABLE, &iid);
            if (ret == CMSRET_SUCCESS)
            {
                ret = _cmsObj_get(MDMOID_RECEIVED_FRAME_VLAN_TAGGING_OPERATION_TABLE,
                                 &iid, 0, (void**)&tagTblEntry);
                if (ret == CMSRET_SUCCESS)
                {
                    cmsMem_free(tagTblEntry->taggingRule);
                    tagTblEntry->taggingRule = cmsMem_alloc(OMCI_ENTRY_SIZE_32+1,
                                                            ALLOC_ZEROIZE);
                    if (tagTblEntry->taggingRule == NULL)
                    {
                        cmsLog_error("failed to allocate memory");
                        ret = CMSRET_INTERNAL_ERROR;
                    }
                    else
                    {
                        memcpy(tagTblEntry->taggingRule,
                               &singleHexString,
                               OMCI_ENTRY_SIZE_32);
                        ret = _cmsObj_set(tagTblEntry, &iid);
                    }
                    _cmsObj_free((void**)&tagTblEntry);
                }
                else
                {
                    cmsLog_error
                    ("Could not get VLAN Tagging Operation Table object, ret = %d", ret);
                }
            }
            else
            {
                cmsLog_error
                    ("Could not add VLAN Tagging Operation Table object, ret = %d", ret);
            }
        }
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  getBroadcastKeyTableData
*  PURPOSE:   Get/Get-Next command handler.
*  PARAMETERS:
*      obj - pointer to the OMCI object.
*      objSize - returned object size.
*      pIidStack - pointer to InstanceIdStack.
*      attrMask - attribute mask.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet getBroadcastKeyTableData(void **obj, UINT16 *objSize,
  InstanceIdStack *pIidStack, UINT16 attrMask)
{
    CmsRet ret = CMSRET_INVALID_PARAM_NAME;

    if ((attrMask & OMCI_ME_ATTRIB_11) != 0)
    {
        ret = getTableAttribute(OMCI_ENTRY_SIZE_36,
          MDMOID_BROADCAST_KEY_TABLE, pIidStack, obj, objSize);
    }
    else
    {
        cmsLog_error("invalid attribute mask 0x%04x", attrMask);
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  setBroadcastKeyTableData
*  PURPOSE:   Set table attribute command handler.
*  PARAMETERS:
*      mdmObj - pointer to the OMCI object.
*      pIidStack - pointer to InstanceIdStack.
*      attrMask - attribute mask.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet setBroadcastKeyTableData(void *mdmObj, InstanceIdStack *pIidStack,
  UINT16 attrMask)
{
    CmsRet ret = CMSRET_SUCCESS;
    InstanceIdStack iid = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidPrev;
    BroadcastKeyTableObject *broadcastTbl = NULL;
    ESCObject *pObject = (ESCObject *) mdmObj;
    UINT32 addrSize = 0, i, j;
    UINT8 *addr = NULL;
    UBOOL8 found = FALSE;
    char entry[OMCI_ENTRY_SIZE_36+1];
    UINT16 entryNum = 0;
    enum action_t {
        act_set      = 0x00,
        act_clear    = 0x01,
        act_cleartbl = 0x02,
        act_rsrvd    = 0x03,
    } action;

    if ((attrMask & OMCI_ME_ATTRIB_11) == 0 ||
        pObject == NULL ||
        pObject->broadcastKeyTable == NULL)
    {
        return ret;
    }

    entryNum = strlen(pObject->broadcastKeyTable) / OMCI_ENTRY_SIZE_36;

    for (j = 0; j < entryNum; j++)
    {
        found = FALSE;
        strncpy(entry, &pObject->broadcastKeyTable[j * OMCI_ENTRY_SIZE_36],
          OMCI_ENTRY_SIZE_36);
        entry[OMCI_ENTRY_SIZE_36] = 0;
        ret = cmsUtl_hexStringToBinaryBuf(entry, &addr, &addrSize);
        if (addrSize != (OMCI_ENTRY_SIZE_36/2))
        {
            if (ret == CMSRET_SUCCESS)
            {
                cmsMem_free(addr);
            }
            continue;
        }

        action = addr[0] & 0x03;
        // clear action for later
        addr[0] = addr[0] & ~0x03;
        // bin2hex
        for (i = 0, j = 0; i < addrSize; i++, j += 2)
        {
            sprintf(&((entry)[j]), "%02x", addr[i]);
        }

        cmsMem_free(addr);

        INIT_INSTANCE_ID_STACK(&iid);
        iidPrev = iid;
        // search for the given entry in the table
        while (_cmsObj_getNextInSubTree(MDMOID_BROADCAST_KEY_TABLE,
                    pIidStack, &iid,
                    (void**)&broadcastTbl) == CMSRET_SUCCESS)
        {
            // match row identifier byte
            if (strncmp(&entry[2], &broadcastTbl->broadcastKey[2], 2) == 0)
            {
                found = TRUE;

                if (action == act_set)
                {
                    memcpy(broadcastTbl->broadcastKey,
                            entry,
                            OMCI_ENTRY_SIZE_36);
                    ret = _cmsObj_set(broadcastTbl, &iid);
                }
                else if (action == act_clear)
                {
                    // do not call cmsMem_free(broadcastTbl->broadcastKeyTable) since
                    // it is free later in _cmsObj_free((void **)&broadcastTbl)
                    ret = _cmsObj_deleteInstance(MDMOID_BROADCAST_KEY_TABLE, &iid);
                }
            }

            if (action == act_cleartbl)
            {
                ret = _cmsObj_deleteInstance(MDMOID_BROADCAST_KEY_TABLE, &iid);
                // since this instance has been deleted, we want to set the iidStack to
                // the previous instance, so that we can continue to do getNext.
                iid = iidPrev;
            }

            _cmsObj_free((void**)&broadcastTbl);
            if (found == TRUE)
            {
                break;
            }
        }

        // the given entry does not exist in the table
        if (found == FALSE && action == act_set)
        {
            memcpy(&iid, pIidStack, sizeof(InstanceIdStack));
            // add given entry to the table
            ret = _cmsObj_addInstance(MDMOID_BROADCAST_KEY_TABLE, &iid);
            if (ret == CMSRET_SUCCESS)
            {
                ret = _cmsObj_get(MDMOID_BROADCAST_KEY_TABLE,
                        &iid, 0, (void**)&broadcastTbl);
                if (ret == CMSRET_SUCCESS)
                {
                    cmsMem_free(broadcastTbl->broadcastKey);
                    broadcastTbl->broadcastKey = cmsMem_alloc(OMCI_ENTRY_SIZE_36 + 1,
                      ALLOC_ZEROIZE);
                    if (broadcastTbl->broadcastKey == NULL)
                    {
                        cmsLog_error("failed to allocate memory");
                        ret = CMSRET_INTERNAL_ERROR;
                    }
                    else
                    {
                        memcpy(broadcastTbl->broadcastKey, entry, OMCI_ENTRY_SIZE_36);
                        ret = _cmsObj_set(broadcastTbl, &iid);
                    }
                    _cmsObj_free((void**)&broadcastTbl);
                }
                else
                {
                    cmsLog_error("Could not get BroadcastKeyTable object, ret = %d", ret);
                }
            }
            else
            {
                cmsLog_error("Could not add BroadcastKeyTable object, ret = %d", ret);
            }
        }
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  getMacBridgePortFilterTableData
*  PURPOSE:   Get/Get-Next command handler.
*  PARAMETERS:
*      obj - pointer to the OMCI object.
*      objSize - returned object size.
*      pIidStack - pointer to InstanceIdStack.
*      attrMask - attribute mask.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet getMacBridgePortFilterTableData(void **obj, UINT16 *objSize,
  InstanceIdStack *pIidStack, UINT16 attrMask)
{
    CmsRet ret = CMSRET_INVALID_PARAM_NAME;

    if ((attrMask & OMCI_ME_ATTRIB_1) != 0)
    {
        ret = getTableAttribute(OMCI_ENTRY_SIZE_16, MDMOID_MAC_FILTER_TABLE,
          pIidStack, obj, objSize);
    }
    else
    {
        cmsLog_error("invalid attribute mask 0x%04x", attrMask);
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  setMacBridgePortFilterTableData
*  PURPOSE:   Set table attribute command handler.
*  PARAMETERS:
*      mdmObj - pointer to the OMCI object.
*      pIidStack - pointer to InstanceIdStack.
*      attrMask - attribute mask.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet setMacBridgePortFilterTableData(void *mdmObj,
  InstanceIdStack *pIidStack, UINT16 attrMask)
{
    CmsRet ret = CMSRET_SUCCESS;
    InstanceIdStack iid = EMPTY_INSTANCE_ID_STACK;
    MacFilterTableObject *filterTbl = NULL;
    MacBridgePortFilterTableDataObject *pObject =
        (MacBridgePortFilterTableDataObject *) mdmObj;
    UINT16 i = 0, entryNum = 0;
    UINT32 addrSize = 0;
    UINT8 *addr = NULL;
    UBOOL8 found = FALSE, delete = FALSE;
    char entry[OMCI_ENTRY_SIZE_16+1];

    if ((attrMask & OMCI_ME_ATTRIB_1) == 0 ||
        pObject == NULL ||
        pObject->macFilterTable == NULL ||
        strcmp(pObject->macFilterTable, "00") == 0 ||
        strcmp(pObject->macFilterTable, "") == 0)
    {
        return ret;
    }

    entryNum = strlen(pObject->macFilterTable) / OMCI_ENTRY_SIZE_16;

    for (i = 0; i < entryNum; i++)
    {
        found = FALSE;
        memset(entry, 0, OMCI_ENTRY_SIZE_16+1);
        strncpy(entry,
                &pObject->macFilterTable[i*OMCI_ENTRY_SIZE_16],
                OMCI_ENTRY_SIZE_16);
        entry[OMCI_ENTRY_SIZE_16] = 0;
        ret = cmsUtl_hexStringToBinaryBuf(entry, &addr, &addrSize);
        if (addrSize != (OMCI_ENTRY_SIZE_16/2))
        {
            if (ret == CMSRET_SUCCESS)
            {
                cmsMem_free(addr);
            }
            continue;
        }

        // if the 8th bit of the filter byte (2nd byte: addr[1])
        // is 0 then delete is true
        delete = ((addr[1] & 0x01) == FALSE) ? TRUE : FALSE;
        cmsMem_free(addr);

        INIT_INSTANCE_ID_STACK(&iid);
        // search for the given entry in the table
        while (_cmsObj_getNextInSubTree(MDMOID_MAC_FILTER_TABLE,
                                       pIidStack, &iid,
                                       (void**)&filterTbl) == CMSRET_SUCCESS)
        {
            // compare the entry number which is the 1st byte
            // note: 1 byte is stored as 2 ascii characters in OMCI database ==>
            // compare 2 bytes, if match then the given entry exists in the table
            if (strncmp(entry, &filterTbl->macFilterEntry[0], 2) == 0)
            {
                found = TRUE;
                // if either addresses is not zero then modify entry
                if (delete == FALSE)
                {
                    memcpy(filterTbl->macFilterEntry,
                           &entry,
                           OMCI_ENTRY_SIZE_16);
                    ret = _cmsObj_set(filterTbl, &iid);
                }
                else
                {
                    // do not call cmsMem_free(filterTbl->macFilterEntry) since
                    // it is free later in _cmsObj_free((void **)&filterTbl)
                    ret = _cmsObj_deleteInstance(MDMOID_MAC_FILTER_TABLE, &iid);
                }
            }
            _cmsObj_free((void**)&filterTbl);
            if (found == TRUE)
            {
                break;
            }
        }

        // the given entry does not exist in the table
        if (found == FALSE && delete == FALSE)
        {
            memcpy(&iid, pIidStack, sizeof(InstanceIdStack));
            // add given entry to the table
            ret = _cmsObj_addInstance(MDMOID_MAC_FILTER_TABLE, &iid);
            if (ret == CMSRET_SUCCESS)
            {
                ret = _cmsObj_get(MDMOID_MAC_FILTER_TABLE,
                                 &iid, 0, (void**)&filterTbl);
                if (ret == CMSRET_SUCCESS)
                {
                    cmsMem_free(filterTbl->macFilterEntry);
                    filterTbl->macFilterEntry = cmsMem_alloc(OMCI_ENTRY_SIZE_16 + 1,
                      ALLOC_ZEROIZE);
                    if (filterTbl->macFilterEntry == NULL)
                    {
                        cmsLog_error("failed to allocate memory");
                        ret = CMSRET_INTERNAL_ERROR;
                    }
                    else
                    {
                        memcpy(filterTbl->macFilterEntry,
                               &entry,
                               OMCI_ENTRY_SIZE_16);
                        ret = _cmsObj_set(filterTbl, &iid);
                    }
                    _cmsObj_free((void**)&filterTbl);
                }
                else
                {
                    cmsLog_error("Could not get MacFilterTable object, ret = %d", ret);
                }
            }
            else
            {
                cmsLog_error("Could not add MacFilterTable object, ret = %d", ret);
            }
        }
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  getMacBridgePortBridgeTableData
*  PURPOSE:   Get/Get-Next command handler.
*  PARAMETERS:
*      obj - pointer to the OMCI object.
*      objSize - returned object size.
*      pIidStack - pointer to InstanceIdStack.
*      attrMask - attribute mask.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet getMacBridgePortBridgeTableData(void **obj, UINT16 *objSize,
  InstanceIdStack *pIidStack, UINT16 attrMask)
{
    CmsRet ret = CMSRET_INVALID_PARAM_NAME;

    if ((attrMask & OMCI_ME_ATTRIB_1) != 0)
    {
        ret = getTableAttribute(OMCI_ENTRY_SIZE_16,
          MDMOID_BRIDGE_TABLE, pIidStack, obj, objSize);
    }
    else
    {
        cmsLog_error("invalid attribute mask 0x%04x", attrMask);
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  getIpv6HostConfigData
*  PURPOSE:   Get/Get-Next command handler.
*  PARAMETERS:
*      obj - pointer to the OMCI object.
*      objSize - returned object size.
*      pIidStack - pointer to InstanceIdStack.
*      attrMask - attribute mask.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet getIpv6HostConfigData(void **obj, UINT16 *objSize,
  InstanceIdStack *pIidStack, UINT16 attrMask)
{
    CmsRet ret = CMSRET_INVALID_PARAM_NAME;

    if ((attrMask & OMCI_ME_ATTRIB_9) != 0)
    {
        ret = getTableAttribute(OMCI_ENTRY_SIZE_48,
          MDMOID_IPV6_CURRENT_ADDRESS_TABLE, pIidStack, obj, objSize);
    }
    else if ((attrMask & OMCI_ME_ATTRIB_10) != 0)
    {
        ret = getTableAttribute(OMCI_ENTRY_SIZE_32,
          MDMOID_IPV6_CURRENT_DEFAULT_ROUTER_TABLE, pIidStack, obj, objSize);
    }
    else if ((attrMask & OMCI_ME_ATTRIB_11) != 0)
    {
        ret = getTableAttribute(OMCI_ENTRY_SIZE_32,
          MDMOID_IPV6_CURRENT_DNS_TABLE, pIidStack, obj, objSize);
    }
    else if ((attrMask & OMCI_ME_ATTRIB_14) != 0)
    {
        ret = getTableAttribute(OMCI_ENTRY_SIZE_52,
          MDMOID_IPV6_CURRENT_ONLINK_PREFIX_TABLE, pIidStack, obj, objSize);
    }
    else
    {
        cmsLog_error("invalid attribute mask 0x%04x", attrMask);
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  getOnuRemoteDebug
*  PURPOSE:   Get/Get-Next command handler.
*  PARAMETERS:
*      obj - pointer to the OMCI object.
*      objSize - returned object size.
*      pIidStack - pointer to InstanceIdStack.
*      attrMask - attribute mask.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet getOnuRemoteDebug(void **obj, UINT16 *objSize,
  InstanceIdStack *pIidStack __attribute__((unused)), UINT16 attrMask)
{
    UINT8 *buf = NULL;
    CmsRet ret = CMSRET_INVALID_PARAM_NAME;

    *objSize = 0;
    if ((attrMask & OMCI_ME_ATTRIB_3) != 0)
    {
        buf = *obj = cmsMem_alloc(OMCI_RMTDBG_RSP_LEN_MAX, ALLOC_ZEROIZE);
        if (buf != NULL)
        {
            *objSize = debugRspGet((char*)buf);
            ret = CMSRET_SUCCESS;
        }
    }

    return ret;
}
