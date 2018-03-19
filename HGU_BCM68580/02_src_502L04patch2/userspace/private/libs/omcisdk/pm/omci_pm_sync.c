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

#include <net/if.h>
#include <sys/ioctl.h>

#include "owsvc_api.h"

#include "omciobj_defs.h"
#include "mdmlite_api.h"

#include "omci_pm_sync.h"
#include "omci_util.h"

#include "ethswctl_api.h"
#include "gponctl_api.h"

#include "rut_gpon.h"
#include "rut_gpon_model.h"

UINT32 aniGAdminState = 0;
static UINT32 aniGObjID = 0;

UINT32 enetPptpObjIDs[ETHERNET_PORT_MAX];
UINT32 enetPptpAdminState[ETHERNET_PORT_MAX] = {};

#ifdef DMP_X_ITU_ORG_VOICE_1
UINT32 voipPptpObjIDs[VOIP_PORT_MAX] = {INVALID_OBJ_ID, INVALID_OBJ_ID};
UINT32 voipPptpAdminState[VOIP_PORT_MAX] = {0, 0};
#endif // DMP_X_ITU_ORG_VOICE_1

UINT32 ethLanNum = 0;


/*========================= PRIVATE FUNCTIONS ==========================*/


static UBOOL8 pm_getAniGAdminState()
{
    if (ENET_ADMIN_STATE_LOCKED == aniGAdminState)
        return TRUE;
    else
        return FALSE;
}

static UBOOL8 pm_getEnetPptpAdminState(int portIndex)
{
    if (( (UINT32) portIndex < ethLanNum) && (portIndex >= 0))
    {
        if (ENET_ADMIN_STATE_LOCKED == enetPptpAdminState[portIndex])
            return TRUE;
        else if(ENET_ADMIN_STATE_UNLOCKED == enetPptpAdminState[portIndex])
            return FALSE;
    }
    else
    {
        cmsLog_error("Invalid port index: %d", portIndex);
    }

    return FALSE;
}

static void pm_syncEnetPptpAdminStates()
{
    UINT16 portIndex = 0;
    UINT32 pptpObjID = INVALID_OBJ_ID;

    for (portIndex = 0; portIndex < ethLanNum; portIndex ++ )
    {
        pptpObjID = omci_pm_getEnetPptpObjIDs(portIndex);

        // only sync. admin state with valid ethernet port
        if (pptpObjID != INVALID_OBJ_ID)
        {
            if (rutGpon_getRelatedAdminStateWithLock(MDMOID_PPTP_ETHERNET_UNI, pptpObjID))
            {
                enetPptpAdminState[portIndex] = ENET_ADMIN_STATE_LOCKED;
            }
            else
            {
                enetPptpAdminState[portIndex] = ENET_ADMIN_STATE_UNLOCKED;
            }
        }
        else
        {
            cmsLog_debug("pm_syncEnetPptpAdminStates failed, portIndex=%d", portIndex);
        }
    }
}

static void pm_syncAniGAdminStates()
{
    if (rutGpon_getRelatedAdminStateWithLock(MDMOID_ANI_G, 0))
    {
        aniGAdminState = ENET_ADMIN_STATE_LOCKED;
    }
    else
    {
        aniGAdminState = ENET_ADMIN_STATE_UNLOCKED;
    }
}

#ifdef DMP_X_ITU_ORG_VOICE_1

static UBOOL8 pm_getVoipPptpAdminState(int portIndex)
{
    if ((portIndex < VOIP_PORT_MAX)&&(portIndex >= 0))
    {
        if (VOIP_ADMIN_STATE_LOCKED == voipPptpAdminState[portIndex])
            return TRUE;
        else if(VOIP_ADMIN_STATE_UNLOCKED == voipPptpAdminState[portIndex])
            return FALSE;
    }
    else
    {
        cmsLog_error("Invalid port index: %d", portIndex);
    }

    return FALSE;
}

static void pm_syncVoidPptpAdminStates()
{
    UINT16 portIndex = 0;
    UINT32 pptpObjID = INVALID_OBJ_ID;

    for (portIndex = 0; portIndex < VOIP_PORT_MAX; portIndex ++ )
    {
        pptpObjID = omci_pm_getVoipPptpObjIDs(portIndex);

        if (pptpObjID != INVALID_OBJ_ID)
        {
            if (rutGpon_getRelatedAdminStateWithLock(MDMOID_PPTP_POTS_UNI, pptpObjID))
            {
                voipPptpAdminState[portIndex] = VOIP_ADMIN_STATE_LOCKED;
            }
            else
            {
                voipPptpAdminState[portIndex] = VOIP_ADMIN_STATE_UNLOCKED;
            }
        }
    }
}

#endif   // DMP_X_ITU_ORG_VOICE_1


/*========================= PUBLIC FUNCTIONS ==========================*/

void omci_pm_initAniGObject(void)
{
    AniGObject *obj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret;

    PUSH_INSTANCE_ID(&iidStack, 1);

    ret = _cmsObj_get(MDMOID_ANI_G, &iidStack, 0, (void**)&obj);
    if (ret == CMSRET_SUCCESS)
    {
        aniGObjID = obj->managedEntityId;
        _cmsObj_free((void**)&obj);
    }
    else
    {
        cmsLog_error("cmsObj_get(MDMOID_ANI_G) failed, ret=%d", ret);
    }
}

UINT32 omci_pm_getAniGObjID(void)
{
    return aniGObjID;
}

void omci_pm_initEnetPptpObjects(void)
{
    UINT32 arrayIndex = 0;
    UINT32 firstMeId = 0;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    PptpEthernetUniObject* objPtr = NULL;
    int i;

    rutGpon_getFirstEthernetMeId(&firstMeId);
    rutGpon_getEthernetLanNum(&ethLanNum);

   for (i=0; i < ETHERNET_PORT_MAX; i++)
       enetPptpObjIDs[i] = INVALID_OBJ_ID;

    // Loop until all MDMOID_PPTP_ETHERNET_UNI objects have been found or no more objects to compare.
    while (_cmsObj_getNextFlags(MDMOID_PPTP_ETHERNET_UNI,
                               &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void**)&objPtr) == CMSRET_SUCCESS)
    {
        arrayIndex = objPtr->managedEntityId - firstMeId;

        if (arrayIndex < ethLanNum)
        {
            // Save this object's managed entity ID.
            enetPptpObjIDs[arrayIndex] = objPtr->managedEntityId;
            // Save this object's admin state.
            enetPptpAdminState[arrayIndex] = objPtr->administrativeState;
        }

        // Free specified object
        _cmsObj_free((void**)&objPtr);
    }
}


UINT16 omci_pm_findEnetPort(UINT16 objID)
{
    UINT16 portIndex = 0;

    if (rutGpon_isVeipPptpUni(MDMOID_PPTP_ETHERNET_UNI, objID) == TRUE)
    {
        portIndex = PORT_VEIP;
    }
    else
    {
        // use enetPptpObjIDs array that is intialized
        // by omci_pm_initEnetPptpObjects() istead of
        // using rutGpon_getFirstEthernetMeId() to
        // avoid getting MDM lock during polling

        for (portIndex = 0;
             (portIndex < ETHERNET_PORT_MAX &&
              enetPptpObjIDs[portIndex] != objID);
             portIndex++)
            ;
    }

    // Return port value (port index) on success,
    // ETHERNET_PORT_MAX on failure.
    return portIndex;
}

UINT16 omci_pm_findUpDnEnetPort(UINT16 objID)
{
    UBOOL8 found = FALSE;
    UINT16 portIndex = ETHERNET_PORT_MAX;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    MacBridgePortConfigDataObject* objPtr;

    // Loop until specified MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA object has been found or no more objects to compare.
    while (found == FALSE &&
           _cmsObj_getNextFlags(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                               &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void**)&objPtr) == CMSRET_SUCCESS)
    {
        // Test for specified MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA object.
        if (objPtr->managedEntityId == objID)
        {
            // Test for Ethernet PPTP-type.
            if (objPtr->tpType == OMCI_BP_TP_PPTP_ETH)
            {
                // Find Ethernet PPTP physical port number.
                if (rutGpon_isVeipPptpUni(MDMOID_PPTP_ETHERNET_UNI, objPtr->tpPointer) == FALSE)
                    portIndex = omci_pm_findEnetPort(objPtr->tpPointer);
                else
                    portIndex = PORT_VEIP;
            }
            else if (objPtr->tpType == OMCI_BP_TP_VEIP)
                portIndex = PORT_VEIP;

            // Signal done searching.
            found = TRUE;
        }

        // Free MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA object
        _cmsObj_free((void**)&objPtr);
    }

    // Return port value (port index) on success, default ETHERNET_PORT_MAX on failure.
    return portIndex;
}

UINT32 omci_pm_getEnetPptpInfo(UINT16 obj_ID)
{
    UINT32 portResult = 0;
    UINT32 portIndex;
    UINT32 pptpObjID;
    int skFd;
    int linkState = 0;
    struct ifreq ifReq;
    struct ethswctl_data ifData;
    struct ethswctl_data *ifDataPtr = &ifData;
    int* tempIntPtr = (int*)&ifData;
    UINT32 line_Speed;
    UINT32 duplex_State;
    IOCTL_MIB_INFO* portMibInfoPtr;

    // Open a basic socket.
    if ((skFd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0)
    {
        // Loop through ENET interfaces to find correct port index.
        for (portIndex = 0; portIndex < ethLanNum; portIndex++)
        {
            // Get port-associated ENET PPTP object ID (or invalid value in case of error).
            pptpObjID = omci_pm_getEnetPptpObjIDs(portIndex);

            // test for matching OMCI object ID.
            if (pptpObjID == (UINT32)obj_ID)
            {
                // Clear IOCTL structure.
                memset(&ifReq, 0, sizeof(ifReq));

                // Setup data pointer.
                ifReq.ifr_data = (char *)ifDataPtr;

                // Setup "eth0-3" string.
                sprintf(ifReq.ifr_name, "eth%d", portIndex);

                // Get link-state through IOCTL & test for successful result.
                if (omciUtil_ioctlWrap(skFd, SIOCGLINKSTATE, &ifReq) == 0)
                {
                    // Read port's current link-state.
                    linkState = *tempIntPtr;

                    // Test for valid ENET connection, do not test sensed-type unless link state is up.
                    if (linkState != 0)
                    {
                        // Get link's sensed type through IOCTL.
                        if (omciUtil_ioctlWrap(skFd, SIOCMIBINFO, &ifReq) == 0)
                        {
                            // Setup MIB info pointer
                            portMibInfoPtr = (IOCTL_MIB_INFO*)&ifData;

                            // Setup sensed line speed on active port.
                            line_Speed = (UINT32)portMibInfoPtr->ulIfSpeed;

                            // Setup sensed line speed on active port.
                            duplex_State = (UINT32)portMibInfoPtr->ulIfDuplex;

                            // Test for 10BaseT.
                            if (line_Speed == SPEED_10MBIT)
                            {
                                // Setup 10BaseT line speed attribute.
                                portResult = OMCI_LINE_SENSE_10;
                            }
                            else
                            {
                                // Test for 100BaseT.
                                if (line_Speed == SPEED_100MBIT)
                                {
                                    // Setup 100BaseT line speed attribute.
                                    portResult = OMCI_LINE_SENSE_100;
                                }
                                else
                                {
                                    // Test for 100BaseT.
                                    if (line_Speed == SPEED_1000MBIT)
                                    {
                                        // Set configuration field to 1gbit.
                                        portResult = OMCI_LINE_SENSE_1000;
                                    }
                                }
                            }

                            // Test for FULL_DUPLEX.
                            if (duplex_State == 0)
                            {
                                // Set configuration field to HALF_DUPLEX.
                                portResult |= OMCI_LINE_HALF_DUPLEX;
                            }
                        }
                    }
                }

                // Done.
                break;
            }
        }

        // Close socket's FD.
        close(skFd);
    }
    else
    {
        cmsLog_error("omci_pm_getEnetPptpInfo  socket open error");
    }

    // Return port's configuration.
    return portResult;
}

UINT16 omci_pm_findGemPort(UINT16 obj_ID)
{
    UBOOL8 found = FALSE;
    UINT16 portValue = GPON_PORT_MAX;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    GemPortNetworkCtpObject* objPtr = NULL;
    BCM_Ploam_GemPortInfo gemPortInfoRec;

    // Loop until all MDMOID_GEM_PORT_NETWORK_CTP objects have been found or no more objects to compare.
    while (found == FALSE &&
           _cmsObj_getNextFlags(MDMOID_GEM_PORT_NETWORK_CTP,
                               &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void**)&objPtr) == CMSRET_SUCCESS)
    {
        // Save this objects ID.
        if (obj_ID == objPtr->managedEntityId)
        {
            // Init data structure.
            memset(&gemPortInfoRec, 0, sizeof(gemPortInfoRec));

            // Setup logical port ID.
            gemPortInfoRec.gemPortID = objPtr->portIdValue;

            // Init physical port index.
            gemPortInfoRec.gemPortIndex = BCM_PLOAM_GEM_PORT_IDX_UNASSIGNED;

            // Attempt to get physical port index & test for success.
            if (gponCtl_getGemPort(&gemPortInfoRec) == 0)
            {
                // Setup port value.
                portValue = gemPortInfoRec.gemPortIndex;
            }

            found = TRUE;
        }

        // Free specified object
        _cmsObj_free((void**)&objPtr);
    }

    // Return valid port on success or INVALID_GPON_PORT on failure.
    return ((found == TRUE) ? portValue : INVALID_GPON_PORT);
}


UINT32 omci_pm_getEnetPptpObjIDs(int portIndex)
{
    UINT32 objId = INVALID_OBJ_ID;

    // Test for valid port index.
    if ( (UINT32) portIndex < ethLanNum)
    {
        // use enetPptpObjIDs array that is intialized
        // by omci_pm_initEnetPptpObjects() istead of
        // using rutGpon_getFirstEthernetMeId() to
        // avoid getting MDM lock during polling
        objId = enetPptpObjIDs[portIndex];
    }
    else
    {
        // Report invalid parameter error.
        cmsLog_error("Invalid port index: %d", portIndex);
    }

    // Return valid ENET PPTP object ID.
    return objId;
}

#ifdef DMP_X_ITU_ORG_VOICE_1

void omci_pm_initVoipPptpObjects(void)
{
    UINT32 arrayIndex = 0;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    PptpPotsUniObject* objPtr = NULL;

    // Loop until all MDMOID_PPTP_POTS_UNI objects have been found or no more objects to compare.
    while (arrayIndex < VOIP_PORT_MAX &&
           _cmsObj_getNextFlags(MDMOID_PPTP_POTS_UNI,
                               &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void**)&objPtr) == CMSRET_SUCCESS)
    {
        // Save this object's ID.
        voipPptpObjIDs[arrayIndex] = objPtr->managedEntityId;

        // Save this object's admin state.
        voipPptpAdminState[arrayIndex] = objPtr->administrativeState;

        // Inc. array index
        arrayIndex++;

        // Free specified object
        _cmsObj_free((void**)&objPtr);
    }
}

UINT16 omci_pm_findVoipPort(UINT16 objID)
{
    UINT16 loopIndex = VOIP_PORT_MAX;

    // Loop through port object ID array to find matching IDs, identifying port.
    for (loopIndex = 0;
         (loopIndex < VOIP_PORT_MAX &&
          voipPptpObjIDs[loopIndex] != objID);
         loopIndex++)
        ;

    // Return valid port index on success,
    // VOIP_PORT_MAX on failure.
    return loopIndex;
}

UINT32 omci_pm_getVoipPptpObjIDs(int portIndex)
{
    UINT32 objId = INVALID_OBJ_ID;

    if (portIndex < VOIP_PORT_MAX)
    {
        objId = voipPptpObjIDs[portIndex];
    }
    else
    {
        cmsLog_error("Invalid port index: %d", portIndex);
    }

    return objId;
}

void omci_pm_syncPotsAdminStates(
    UINT32 portNum,
    UBOOL8 isAdminLocked)
{
    if ( portNum < VOIP_PORT_MAX )
    {
        if (isAdminLocked == TRUE)
        {
            voipPptpAdminState[portNum] = VOIP_ADMIN_STATE_LOCKED;
        }
        else
        {
            voipPptpAdminState[portNum] = VOIP_ADMIN_STATE_UNLOCKED;
        }
    }
    else
    {
        cmsLog_error("POTS port is out of range [0-%d], port=%d",
                     VOIP_PORT_MAX, portNum);
    }
}

#endif   // DMP_X_ITU_ORG_VOICE_1

UBOOL8 omci_pm_isAdminStateEnabled(UINT32 mdmObjId, int portIndex)
{
    if (mdmObjId == MDMOID_PPTP_ETHERNET_UNI)
    {
        if (pm_getEnetPptpAdminState(portIndex) == TRUE)
            return FALSE;
        else
            return TRUE;
    }
    else if (mdmObjId == MDMOID_ANI_G)
    {
        if (pm_getAniGAdminState() == TRUE)
            return FALSE;
        else
            return TRUE;
    }

#ifdef DMP_X_ITU_ORG_VOICE_1
    else if (mdmObjId == MDMOID_PPTP_POTS_UNI)
    {
        if (pm_getVoipPptpAdminState(portIndex) == TRUE)
            return FALSE;
        else
            return TRUE;
    }
#endif
    else
    {
        cmsLog_error("Invalid mdmObjId: %d", mdmObjId);
    }

    return FALSE;
}

void omci_pm_syncEnetAdminStates(
    UINT32 portNum,
    UBOOL8 isAdminLocked)
{
    if ( portNum < ethLanNum)
    {
        if (isAdminLocked == TRUE)
        {
            enetPptpAdminState[portNum] = ENET_ADMIN_STATE_LOCKED;
        }
        else
        {
            enetPptpAdminState[portNum] = ENET_ADMIN_STATE_UNLOCKED;
        }
    }
    else
    {
        cmsLog_error("Ethernet port is out of range [0-%d], port=%d",
                     ETHERNET_PORT_MAX, portNum);
    }
}

void omci_pm_syncUniGAdminStates(
    UINT32 uniGMeId,
    UBOOL8 isAdminLocked)
{
    UBOOL8 found = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    PptpEthernetUniObject *pptpEthernet = NULL;
#ifdef DMP_X_ITU_ORG_VOICE_1
    PptpPotsUniObject *pptpPots = NULL;
#endif

    while (found == FALSE &&
           _cmsObj_getNextFlags(MDMOID_PPTP_ETHERNET_UNI,
                               &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void **) &pptpEthernet) == CMSRET_SUCCESS)
    {
        if ((pptpEthernet->managedEntityId == uniGMeId) &&
            (rutGpon_isVeipPptpUni(MDMOID_PPTP_ETHERNET_UNI, uniGMeId) == FALSE))
        {
            UINT32 firstMeId = 0, portNum = 0;
            rutGpon_getFirstEthernetMeId(&firstMeId);
            portNum = pptpEthernet->managedEntityId - firstMeId;
            omci_pm_syncEnetAdminStates(portNum, isAdminLocked);
            found = TRUE;
        }

        _cmsObj_free((void **) &pptpEthernet);
    }

#ifdef DMP_X_ITU_ORG_VOICE_1
    INIT_INSTANCE_ID_STACK(&iidStack);
    while (found == FALSE &&
           _cmsObj_getNextFlags(MDMOID_PPTP_POTS_UNI,
                               &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void **) &pptpPots) == CMSRET_SUCCESS)
    {
        if (pptpPots->managedEntityId == uniGMeId)
        {
            UINT32 portNum = 0;
            portNum = PEEK_INSTANCE_ID(&iidStack);
            portNum = portNum - 1;
            omci_pm_syncPotsAdminStates(portNum, isAdminLocked);
            found = TRUE;
        }

        _cmsObj_free((void **) &pptpPots);
    }
#endif
}

// it's called by rcl_ontGObject() to store admin state
// to local variables aniGAdminState and enetPptpAdminState
void omci_pm_syncAllAdminStates(void)
{
    pm_syncAniGAdminStates();
    pm_syncEnetPptpAdminStates();
#ifdef DMP_X_ITU_ORG_VOICE_1
    pm_syncVoidPptpAdminStates();
#endif // DMP_X_ITU_ORG_VOICE_1
}

UINT16 omci_pm_findIwtpGemPort(UINT16 objID)
{
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    _GemInterworkingTpObject* objPtr;
    UINT16 portValue = GPON_PORT_MAX;
    UBOOL8 found = FALSE;
    CmsRet ret = CMSRET_SUCCESS;

    while (!found && (ret == CMSRET_SUCCESS))
    {
        ret = _cmsObj_getNext(MDMOID_GEM_INTERWORKING_TP, &iidStack, (void**)&objPtr);
        // Test for success.
        if (ret == CMSRET_SUCCESS)
        {
            // Save this objects ID.
            if (objID == objPtr->managedEntityId)
            {
                found = TRUE;
                portValue = omci_pm_findGemPort(objPtr->gemPortNetworkCtpConnPointer);
            }
        }
        _cmsObj_free((void**)&objPtr);
    }

    return portValue;
}

UINT16 omci_pm_findMcastIwtpGemPort(UINT16 objID)
{
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    _MulticastGemInterworkingTpObject* objPtr;
    UINT16 portValue = GPON_PORT_MAX;
    UBOOL8 found = FALSE;
    CmsRet ret = CMSRET_SUCCESS;

    while (!found && (ret == CMSRET_SUCCESS))
    {
        ret = _cmsObj_getNext(MDMOID_MULTICAST_GEM_INTERWORKING_TP, &iidStack, (void**)&objPtr);
        // Test for success.
        if (ret == CMSRET_SUCCESS)
        {
            // Save this objects ID.
            if (objID == objPtr->managedEntityId)
            {
                found = TRUE;
                portValue = omci_pm_findGemPort(objPtr->gemPortNetworkCtpConnPointer);
            }
        }
        _cmsObj_free((void**)&objPtr);
    }

    return portValue;
}

UINT16 omci_pm_find1pMapperGemPort(UINT16 objID)
{
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    MapperServiceProfileObject* mapperObjPtr;
    UINT16 portValue = 0, portIndex;
    UBOOL8 found = FALSE;
    CmsRet ret = CMSRET_SUCCESS;

    while (!found && (ret == CMSRET_SUCCESS))
    {
        ret = _cmsObj_getNext(MDMOID_MAPPER_SERVICE_PROFILE, &iidStack, (void**)&mapperObjPtr);

        // Test for success.
        if (ret == CMSRET_SUCCESS)
        {
            // Save this objects ID.
            if (objID == mapperObjPtr->managedEntityId)
            {
                found = TRUE;
                if (mapperObjPtr->interworkTpPointerPriority0 != 0xffff &&
                    (portIndex = omci_pm_findGemPort(mapperObjPtr->interworkTpPointerPriority0)) < GPON_PORT_MAX)
                  portValue |= (1 << portIndex);
                if (mapperObjPtr->interworkTpPointerPriority1 != 0xffff &&
                    (portIndex = omci_pm_findGemPort(mapperObjPtr->interworkTpPointerPriority1)) < GPON_PORT_MAX)
                  portValue |= (1 << portIndex);
                if (mapperObjPtr->interworkTpPointerPriority2 != 0xffff &&
                    (portIndex = omci_pm_findGemPort(mapperObjPtr->interworkTpPointerPriority2)) < GPON_PORT_MAX)
                  portValue |= (1 << portIndex);
                if (mapperObjPtr->interworkTpPointerPriority3 != 0xffff &&
                    (portIndex = omci_pm_findGemPort(mapperObjPtr->interworkTpPointerPriority3)) < GPON_PORT_MAX)
                  portValue |= (1 << portIndex);
                if (mapperObjPtr->interworkTpPointerPriority4 != 0xffff &&
                    (portIndex = omci_pm_findGemPort(mapperObjPtr->interworkTpPointerPriority4)) < GPON_PORT_MAX)
                  portValue |= (1 << portIndex);
                if (mapperObjPtr->interworkTpPointerPriority5 != 0xffff &&
                    (portIndex = omci_pm_findGemPort(mapperObjPtr->interworkTpPointerPriority5)) < GPON_PORT_MAX)
                  portValue |= (1 << portIndex);
                if (mapperObjPtr->interworkTpPointerPriority6 != 0xffff &&
                    (portIndex = omci_pm_findGemPort(mapperObjPtr->interworkTpPointerPriority6)) < GPON_PORT_MAX)
                  portValue |= (1 << portIndex);
                if (mapperObjPtr->interworkTpPointerPriority7 != 0xffff &&
                    (portIndex = omci_pm_findGemPort(mapperObjPtr->interworkTpPointerPriority7)) < GPON_PORT_MAX)
                  portValue |= (1 << portIndex);
            }
        }
        _cmsObj_free((void**)&mapperObjPtr);
    }

    return portValue;
}

UINT16 omci_pm_findPhysicalPort(UINT16 objID, UINT8 *tpType)
{
    UINT16 portIndex = INVALID_GPON_PORT;
    UINT8 foundFlag = TRUE;
    UINT8 errorFlag = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    MacBridgePortConfigDataObject* objPtr;

    // Loop until specified MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA object has been found or no more objects to compare.
    while ((foundFlag == TRUE) && (errorFlag == FALSE))
    {
        // Attempt to get the next MDM MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA object & test for success.
        if (_cmsObj_getNext(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA, &iidStack, (void**)&objPtr) == CMSRET_SUCCESS)
        {
            // Test for specified MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA object.
            if (objPtr->managedEntityId == objID)
            {
                *tpType = objPtr->tpType;
                // Test for TP-type.
                switch(objPtr->tpType)
                {
                    case MAC_BRIDGE_ETHERNET_PPTP_TYPE:
                        // Find Ethernet PPTP physical port number.
                        portIndex = omci_pm_findEnetPort(objPtr->tpPointer);
                        break;
                    case MAC_BRIDGE_1P_MAPPER_TYPE:
                        portIndex = omci_pm_find1pMapperGemPort(objPtr->tpPointer);
                        break;
                    case MAC_BRIDGE_IWTP_TYPE:
                        portIndex = omci_pm_findIwtpGemPort(objPtr->tpPointer);
                        break;
                    case MAC_BRIDGE_MCAST_IWTP_TYPE:
                        portIndex = omci_pm_findMcastIwtpGemPort(objPtr->tpPointer);
                        break;
                    case MAC_BRIDGE_VEIP_TYPE:
                        portIndex = PORT_VEIP; // veip port
                        break;
                    case MAC_BRIDGE_MOCA_TYPE:
                        break;
                }
                // Signal done searching.
                foundFlag = TRUE;
            }

            // Free MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA object
            _cmsObj_free((void**)&objPtr);
        }
        else
        {
          // Signal error.
          errorFlag = TRUE;
        }
    }

    return portIndex;
}

#endif // DMP_X_ITU_ORG_GPON_1

