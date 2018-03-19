/***********************************************************************
 *
 *  Copyright (c) 2014 Broadcom
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
#include "omciobj_defs.h"

#include "omci_pm.h"
#include "omci_pm_llist.h"
#include "omci_th_llist.h"


/*========================= PRIVATE FUNCTIONS ==========================*/

static void pm_getObject(
    UINT16 obj_Type,
    UINT16 obj_ID,
    UBOOL8 currentFlag,
    void **ppData,
    UINT16 *pDataLen)
{
    *pDataLen = omci_pm_getObjTypeCounterBufSize(obj_Type);
    if (*pDataLen == 0)
    {
        cmsLog_error("Unknown object-type:%d", obj_Type);
        *ppData = NULL;
        return;
    }

    *ppData = omci_pm_get(obj_Type, obj_ID, currentFlag);
    if (*ppData == NULL)
    {
        cmsLog_error("omci_pm_get() failed, type:%d, id:%d, flag:%d",
          obj_Type, obj_ID, currentFlag);
    }
}


/*========================= PUBLIC FUNCTIONS ==========================*/

CmsRet omci_pm_createObject(
    UINT16 obj_Type,
    UINT16 obj_ID,
    UINT16 port_ID,
    UINT16 threshold_ID)
{
    CmsRet ret = CMSRET_INVALID_ARGUMENTS;

    // Switch on OMCI PM object identifier.
    switch (obj_Type)
    {
        case MDMOID_ETHERNET_PM_HISTORY_DATA:
        case MDMOID_ETHERNET_PM_HISTORY_DATA2:
        case MDMOID_ETHERNET_PM_HISTORY_DATA3:
        case MDMOID_DOWNSTREAM_ETHERNET_FRAME_PM_HISTORY_DATA:
        case MDMOID_UPSTREAM_ETHERNET_FRAME_PM_HISTORY_DATA:
            // Create Ethernet PM object.
            ret = omci_pm_create(obj_Type, obj_ID, 0, threshold_ID);
            break;
        case MDMOID_GAL_ETHERNET_PM_HISTORY_DATA:
            if (port_ID < GPON_PORT_MAX)
            {
                // Create MDMOID_GAL_ETHERNET_PM_HISTORY_DATA object.
                ret = omci_pm_create(obj_Type, obj_ID, port_ID, threshold_ID);
            }
            else
            {
                // Log error.
                cmsLog_error("MDMOID_GAL_ETHERNET_PM_HISTORY_DATA Illegal port: %d", port_ID);
            }
            break;
        case MDMOID_MOCA_ETHERNET_PM_HISTORY_DATA:
            if (port_ID < MOCA_PORT_MAX)
            {
                // Create MDMOID_MOCA_ETHERNET_PM_HISTORY_DATA object.
                ret = omci_pm_create(obj_Type, obj_ID, port_ID, threshold_ID);
            }
            else
            {
                // Log error.
                cmsLog_error("MDMOID_MOCA_ETHERNET_PM_HISTORY_DATA Illegal port: %d", port_ID);
            }
            break;
        case MDMOID_MOCA_INTERFACE_PM_HISTORY_DATA:
            if (port_ID < MOCA_PORT_MAX)
            {
                // Create MDMOID_MOCA_INTERFACE_PM_HISTORY_DATA object.
                ret = omci_pm_create(obj_Type, obj_ID, port_ID, threshold_ID);
            }
            else
            {
                // Log error.
                cmsLog_error("MDMOID_MOCA_INTERFACE_PM_HISTORY_DATA Illegal port: %d", port_ID);
            }
            break;
        case MDMOID_GEM_PORT_PM_HISTORY_DATA:
            if (port_ID < GPON_PORT_MAX)
            {
                // Create MDMOID_GEM_PORT_PM_HISTORY_DATA object.
                ret = omci_pm_create(obj_Type, obj_ID, port_ID, threshold_ID);
            }
            else
            {
                // Log error.
                cmsLog_error("MDMOID_GEM_PORT_PM_HISTORY_DATA Illegal port: %d", port_ID);
            }
            break;
        case MDMOID_FEC_PM_HISTORY_DATA:
            ret = omci_pm_create(obj_Type, obj_ID, port_ID, threshold_ID);
            break;
        case MDMOID_RTP_PM_HISTORY_DATA:
            ret = omci_pm_create(obj_Type, obj_ID, port_ID, threshold_ID);
            break;
        case MDMOID_IP_HOST_PM_HISTORY_DATA:
            ret = omci_pm_create(obj_Type, obj_ID, port_ID, threshold_ID);
            break;
        case MDMOID_MAC_BRIDGE_PM_HISTORY_DATA:
            ret = omci_pm_create(obj_Type, obj_ID, port_ID, threshold_ID);
            break;
        case MDMOID_MAC_BRIDGE_PORT_PM_HISTORY_DATA:
            ret = omci_pm_create(obj_Type, obj_ID, port_ID, threshold_ID);
            break;
        default:
            // Log error.
            cmsLog_error("Unknown create object-type identifier: %d", obj_Type);
            break;
    }

    return ret;
}

void omci_pm_deleteObject(
    UINT16 obj_Type,
    UINT16 obj_ID)
{
    switch (obj_Type)
    {
        case MDMOID_ETHERNET_PM_HISTORY_DATA:
        case MDMOID_ETHERNET_PM_HISTORY_DATA2:
        case MDMOID_ETHERNET_PM_HISTORY_DATA3:
        case MDMOID_DOWNSTREAM_ETHERNET_FRAME_PM_HISTORY_DATA:
        case MDMOID_UPSTREAM_ETHERNET_FRAME_PM_HISTORY_DATA:
        case MDMOID_FEC_PM_HISTORY_DATA:
        case MDMOID_GEM_PORT_PM_HISTORY_DATA:
        case MDMOID_MOCA_ETHERNET_PM_HISTORY_DATA:
        case MDMOID_MOCA_INTERFACE_PM_HISTORY_DATA:
        case MDMOID_GAL_ETHERNET_PM_HISTORY_DATA:
        case MDMOID_RTP_PM_HISTORY_DATA:
        case MDMOID_IP_HOST_PM_HISTORY_DATA:
        case MDMOID_MAC_BRIDGE_PM_HISTORY_DATA:
        case MDMOID_MAC_BRIDGE_PORT_PM_HISTORY_DATA:
        case MDMOID_ETHERNET_FRAME_EXTENDED_P_M:
            omci_pm_delete(obj_Type, obj_ID);
            break;
        default:
            // Log error.
            cmsLog_error("Unknown delete object-type identifier: %d", obj_Type);
            break;
    }
}

void omci_pm_getObject(
    UINT16 obj_Type,
    UINT16 obj_ID,
    void **ppData,
    UINT16 *pDataLen)
{
    pm_getObject(obj_Type, obj_ID, FALSE, ppData, pDataLen);
}

void omci_pm_getCurrentObject(
    UINT16 obj_Type,
    UINT16 obj_ID,
    void **ppData,
    UINT16 *pDataLen)
{
    pm_getObject(obj_Type, obj_ID, TRUE, ppData, pDataLen);
}

void omci_pm_setObject(
    UINT16 obj_Type,
    UINT16 obj_ID,
    UINT16 threshold_ID)
{
    switch (obj_Type)
    {
        case MDMOID_ETHERNET_PM_HISTORY_DATA:
        case MDMOID_ETHERNET_PM_HISTORY_DATA2:
        case MDMOID_ETHERNET_PM_HISTORY_DATA3:
        case MDMOID_DOWNSTREAM_ETHERNET_FRAME_PM_HISTORY_DATA:
        case MDMOID_UPSTREAM_ETHERNET_FRAME_PM_HISTORY_DATA:
        case MDMOID_FEC_PM_HISTORY_DATA:
        case MDMOID_GEM_PORT_PM_HISTORY_DATA:
        case MDMOID_MOCA_ETHERNET_PM_HISTORY_DATA:
        case MDMOID_MOCA_INTERFACE_PM_HISTORY_DATA:
        case MDMOID_GAL_ETHERNET_PM_HISTORY_DATA:
        case MDMOID_RTP_PM_HISTORY_DATA:
        case MDMOID_IP_HOST_PM_HISTORY_DATA:
        case MDMOID_MAC_BRIDGE_PM_HISTORY_DATA:
        case MDMOID_MAC_BRIDGE_PORT_PM_HISTORY_DATA:
            omci_pm_set(obj_Type, obj_ID, threshold_ID);
            break;
        default:
            // Log error.
            cmsLog_error("Unknown set object-type identifier: %d", obj_Type);
            break;
    }
}

void omci_th_createObject(
    UINT16 obj_Type,
    UINT16 obj_ID,
    const void *pData)
{
    switch (obj_Type)
    {
        case MDMOID_THRESHOLD_DATA1:
        case MDMOID_THRESHOLD_DATA2:
            omci_threshold_create(obj_Type, obj_ID, pData);
            break;
        default:
            // Log error.
            cmsLog_error("Unknown create threshold object-type identifier: %d", obj_Type);
            break;
    }
}

void omci_th_deleteObject(
    UINT16 obj_Type,
    UINT16 obj_ID)
{
    switch (obj_Type)
    {
        case MDMOID_THRESHOLD_DATA1:
        case MDMOID_THRESHOLD_DATA2:
            omci_threshold_delete(obj_ID);
            break;
        default:
            // Log error.
            cmsLog_error("Unknown delete threshold object-type identifier: %d", obj_Type);
            break;
    }
}

void omci_th_getObject(
    UINT16 obj_Type,
    UINT16 obj_ID,
    void *pData,
    UINT16 *pDataLen)
{
    BCM_OMCIPM_THRESHOLD_ENTRY* thresholdEntryPtr = NULL;
    ThresholdData1Object* pData1Object = NULL;
    ThresholdData2Object* pData2Object = NULL;

    *pDataLen = 0;

    if (pData == NULL)
    {
        return;
    }

    // Switch on OMCI object class. Get current data pointer on success, NULL on error.
    switch (obj_Type)
    {
        case MDMOID_THRESHOLD_DATA1:
            thresholdEntryPtr = omci_threshold_get(obj_ID);

            if (thresholdEntryPtr != NULL)
            {
                pData1Object = (ThresholdData1Object *)pData;
                pData1Object->thresholdValue1 = thresholdEntryPtr->thresholdValue1;
                pData1Object->thresholdValue2 = thresholdEntryPtr->thresholdValue2;
                pData1Object->thresholdValue3 = thresholdEntryPtr->thresholdValue3;
                pData1Object->thresholdValue4 = thresholdEntryPtr->thresholdValue4;
                pData1Object->thresholdValue5 = thresholdEntryPtr->thresholdValue5;
                pData1Object->thresholdValue6 = thresholdEntryPtr->thresholdValue6;
                pData1Object->thresholdValue7 = thresholdEntryPtr->thresholdValue7;

                *pDataLen = sizeof(ThresholdData1Object);
            }
            break;
        case MDMOID_THRESHOLD_DATA2:
            thresholdEntryPtr = omci_threshold_get(obj_ID);

            if (thresholdEntryPtr != NULL)
            {
                pData2Object = (ThresholdData2Object *)pData;
                pData2Object->thresholdValue8  = thresholdEntryPtr->thresholdValue8;
                pData2Object->thresholdValue9  = thresholdEntryPtr->thresholdValue9;
                pData2Object->thresholdValue10 = thresholdEntryPtr->thresholdValue10;
                pData2Object->thresholdValue11 = thresholdEntryPtr->thresholdValue11;
                pData2Object->thresholdValue12 = thresholdEntryPtr->thresholdValue12;
                pData2Object->thresholdValue13 = thresholdEntryPtr->thresholdValue13;
                pData2Object->thresholdValue14 = thresholdEntryPtr->thresholdValue14;

                *pDataLen = sizeof(ThresholdData2Object);
            }
            break;
        default:
            cmsLog_notice("Unknown get threshold object-type identifier: %d", obj_Type);
            break;
    }
}

void omci_th_setObject(
    UINT16 obj_Type,
    UINT16 obj_ID,
    const void *pData)
{
    switch (obj_Type)
    {
        case MDMOID_THRESHOLD_DATA1:
        case MDMOID_THRESHOLD_DATA2:
            omci_threshold_set(obj_Type, obj_ID, pData);
            break;
        default:
            // Log error.
            cmsLog_error("Unknown set threshold object-type identifier: %d", obj_Type);
            break;
    }
}

UBOOL8 omci_th_isObjectExisted(
    UINT16 obj_Type,
    UINT16 obj_ID)
{
    UBOOL8 ret = FALSE;

    switch (obj_Type)
    {
        case MDMOID_THRESHOLD_DATA1:
        case MDMOID_THRESHOLD_DATA2:
            if (omci_threshold_get(obj_ID) != NULL)
            {
                ret = TRUE;
            }
            break;
        default:
            // Log error.
            cmsLog_error("Unknown threshold object-type identifier: %d", obj_Type);
            break;
    }

    return ret;
}

CmsRet omci_extPm_createObject(
    UINT16 obj_Type,
    UINT16 obj_ID,
    UINT8 *control)
{
    CmsRet ret = CMSRET_INVALID_ARGUMENTS;

    // Switch on OMCI PM object identifier.
    switch (obj_Type)
    {
        case MDMOID_ETHERNET_FRAME_EXTENDED_P_M:
            ret = omci_extPm_create(obj_Type, obj_ID, control);
            break;
        default:
            // Log error.
            cmsLog_error("Unknown create object-type identifier: %d", obj_Type);
            break;
    }

    return ret;
}

void omci_extPm_setObject(
    UINT16 obj_Type,
    UINT16 obj_ID,
    UINT8 *control)
{
    switch (obj_Type)
    {
        case MDMOID_ETHERNET_FRAME_EXTENDED_P_M:
            omci_extPm_set(obj_Type, obj_ID, control);
            break;
        default:
            // Log error.
            cmsLog_error("Unknown set object-type identifier: %d", obj_Type);
            break;
    }
}

#endif // DMP_X_ITU_ORG_GPON_1
