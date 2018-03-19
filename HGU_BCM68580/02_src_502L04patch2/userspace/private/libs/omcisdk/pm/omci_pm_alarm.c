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

#include <fcntl.h>
#include <math.h>
#include <net/if.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include "owsvc_api.h"

#include "omci_alarm_llist.h"
#include "omci_arc_llist.h"
#include "omci_pm_alarm.h"
#include "omci_pm_llist.h"
#include "omci_pm_sync.h"
#include "omci_pm_util.h"

#include "bcm_ploam_api.h"
#include "bcmnet.h"
#include "ethswctl_api.h"
#include "gponctl_api.h"
#include "laser.h"

#include "omciobj_defs.h"
#include "omciutl_cmn.h"
#include "omci_util.h"


/*
 * OMCI ANI-G ME definition:
 * Optical threshold (1 byte) in 0.5 dB increments.
 * Optical level (2 bytes) with 0.002 dB granularity.
 */
#define ANIG_OPTICAL_THR_UNIT_FACTOR 250
#define RX_THR_CONV(thr) (SINT32)~(thr * ANIG_OPTICAL_THR_UNIT_FACTOR)
#define TX_THR_CONV(thr) (SINT32)(thr * ANIG_OPTICAL_THR_UNIT_FACTOR)


/*
 * global variables
 */

/*
 * mutex, signal data for synchronization between
 * omcid main and omcipm thread. They're used in
 * omcid.c, omci_pm_alarm.c
 */
pthread_mutex_t gpon_link_mutex = PTHREAD_MUTEX_INITIALIZER;
int gponLinkChange = 0;

/*
 * local variables
 */

static BCM_OMCIPM_LINE_SENSE_REC enetLineArray[ETHERNET_PORT_MAX];

static BCM_ALARM_SOAK_REC alarmSoakArray[MAX_ALARM_SOAK_LIST];

static UINT8 alarmSeqNumber = INIT_ALARM_SEQ_VALUE;

static UINT16 alarmTotalNumber = 0;

static int enetFd = 0, laserFd = 0;


/*========================= PRIVATE FUNCTIONS ==========================*/

static void alarm_data_reset(void)
{
    memset(enetLineArray, 0, sizeof(enetLineArray));
    memset(alarmSoakArray, 0, sizeof(alarmSoakArray));
}

static UBOOL8 alarm_testDuplication(
    UINT16 class_ID,
    UINT16 obj_ID,
    UINT16 alarm_Bitmap)
{
    UBOOL8 ret = FALSE;
    BCM_OMCIPM_ALARM_ENTRY* alarmEntryPtr = NULL;
    BCM_COMMON_LLIST* pNonTcaList = omci_alarm_getNonTcaListPtr();

    if (pNonTcaList == NULL)
    {
        cmsLog_error("Invalid Head of non TCA alarm link-list");
        return ret;
    }

    alarmEntryPtr = omci_alarm_get(pNonTcaList, class_ID, obj_ID);

    if (alarmEntryPtr != NULL)
    {
        // Test for alarm not previously reported.
        if ((alarmEntryPtr->alarmBitmap & alarm_Bitmap) == 0)
        {
            // Send new alarm for object already listed.
            omci_alarm_send(0, class_ID, obj_ID, alarmEntryPtr->alarmBitmap | alarm_Bitmap);
        }

        // Add new alarm bits
        alarmEntryPtr->alarmBitmap |= alarm_Bitmap;

        // Signal duplicate.
        ret = TRUE;
    }

    return ret;
}

static void alarm_addNonTca(
    UINT16 class_ID,
    UINT16 obj_ID,
    UINT16 alarm_Bitmap)
{
    BCM_COMMON_LLIST* pNonTcaList = omci_alarm_getNonTcaListPtr();

    // Test for duplicate alarm already listed.
    if (alarm_testDuplication(class_ID, obj_ID, alarm_Bitmap) == FALSE)
    {
        // Attempt to create entry.
        if (omci_alarm_create(pNonTcaList,
                              class_ID,
                              obj_ID,
                              alarm_Bitmap) == OMCI_PM_STATUS_SUCCESS)
        {
            // Send new alarm for object already listed.
            omci_alarm_send(0, class_ID, obj_ID, alarm_Bitmap);
        }
    }
}

static void alarm_deleteNonTca(
    UINT16 class_ID,
    UINT16 obj_ID,
    UINT16 alarm_Bitmap)
{
    BCM_OMCIPM_ALARM_ENTRY* alarmEntryPtr = NULL;
    BCM_COMMON_LLIST* pNonTcaList = omci_alarm_getNonTcaListPtr();

    alarmEntryPtr = omci_alarm_get(pNonTcaList, class_ID, obj_ID);

    if (alarmEntryPtr != NULL)
    {
        // Clear alarm bit.
        alarmEntryPtr->alarmBitmap &= ~alarm_Bitmap;

        // Send closing alarm for object already listed.
        omci_alarm_send(0, class_ID, obj_ID, alarmEntryPtr->alarmBitmap);

        // Test if all object's alarms clear.
        if (alarmEntryPtr->alarmBitmap == 0)
        {
            // Remove entry from list.
            omci_alarm_delete(pNonTcaList, class_ID, obj_ID);
        }
    }
}

static void alarm_soak(
    UINT16 array_Index,
    UINT16 obj_Type,
    UINT16 obj_ID,
    UINT16 alarm_Bitmap,
    UINT8 alarm_SetFlag)
{
    time_t currentTime = time(NULL);

    // Test if alarm being set, not cleared.
    if (alarm_SetFlag != FALSE)
    {
        // refresh ARC timer under existing ARC.
        omci_arc_refresh_timer(obj_Type, obj_ID);
    }

    // Switch on current alarm soak state.
    switch (alarmSoakArray[array_Index].soakState)
    {
        case ALARM_SOAK_IDLE:
            // Test for new alarm.
            if (alarm_SetFlag == TRUE)
            {
                // Test for GPON alarms which are already soaked.  Send alarm immediately.
                if ((array_Index == ALARM_SOAK_GPON_SF) || (array_Index == ALARM_SOAK_GPON_SD))
                {
                    // Add alarm to non-TCA list & send alarm to OLT.
                    alarm_addNonTca(obj_Type, obj_ID, alarm_Bitmap);

                    // Setup soak ACTIVE state.
                    alarmSoakArray[array_Index].soakState = ALARM_SOAK_ACTIVE;
                }
                else
                {
                    // Setup soak start time for new alarm.
                    alarmSoakArray[array_Index].soakStartTime = currentTime;

                    // Setup soak DECLARE state.
                    alarmSoakArray[array_Index].soakState = ALARM_SOAK_DECLARE;
                }
            }
            break;

        case ALARM_SOAK_ACTIVE:
            // Test for current alarm.
            if (alarm_SetFlag == FALSE)
            {
                // Test for GPON alarms which are already soaked.  Clear alarm immediately.
                if ((array_Index == ALARM_SOAK_GPON_SF) || (array_Index == ALARM_SOAK_GPON_SD))
                {
                    // Remove alarm from non-TCA list.
                    alarm_deleteNonTca(obj_Type, obj_ID, alarm_Bitmap);

                    // Setup soak ACTIVE state.
                    alarmSoakArray[array_Index].soakState = ALARM_SOAK_IDLE;
                }
                else
                {
                    // Setup soak start time for new alarm.
                    alarmSoakArray[array_Index].soakStartTime = currentTime;

                    // Setup soak DECLARE state.
                    alarmSoakArray[array_Index].soakState = ALARM_SOAK_RETIRE;
                }
            }
            break;

        case ALARM_SOAK_DECLARE:
            // Test for new alarm soaking to declare.
            if (alarm_SetFlag == TRUE)
            {
                // Test soak start time for new alarm.
                if (currentTime > (alarmSoakArray[array_Index].soakStartTime + BCM_SOAK_DECLARE_SECS))
                {
                    // Add alarm to non-TCA list & send alarm to OLT.
                    alarm_addNonTca(obj_Type, obj_ID, alarm_Bitmap);

                    // Setup soak ACTIVE state.
                    alarmSoakArray[array_Index].soakState = ALARM_SOAK_ACTIVE;
                }
            }
            else
            {
                // Return to soak IDLE state, alarm was not present long enough to notify OLT.
                alarmSoakArray[array_Index].soakState = ALARM_SOAK_IDLE;
            }
            break;

        case ALARM_SOAK_RETIRE:
            // Test for current alarm soaking to retire.
            if (alarm_SetFlag == FALSE)
            {
                // Test soak start time for new alarm.
                if (currentTime > (alarmSoakArray[array_Index].soakStartTime + BCM_SOAK_RETIRE_SECS))
                {
                    // Remove alarm from non-TCA list.
                    alarm_deleteNonTca(obj_Type, obj_ID, alarm_Bitmap);

                    // Setup soak ACTIVE state.
                    alarmSoakArray[array_Index].soakState = ALARM_SOAK_IDLE;
                }
            }
            else
            {
                // Return to soak ACTIVE state, alarm was not clear long enough to consider retired.
                alarmSoakArray[array_Index].soakState = ALARM_SOAK_ACTIVE;
            }
            break;

        default:
            // Report error.
            cmsLog_error("Invalid alarm soak state: %d", alarmSoakArray[array_Index].soakState);
            break;
    }
}

static void alarm_gpon_link_change(void)
{
    pthread_mutex_lock(&gpon_link_mutex);
    gponLinkChange = TRUE;
    pthread_mutex_unlock(&gpon_link_mutex);
}

/*========================= PUBLIC FUNCTIONS ==========================*/

UINT16 omci_alarm_getTotalNumber(void)
{
    // return alarm total number.
    return alarmTotalNumber;
}

void omci_alarm_setSequenceNumber(UINT8 seqNumber)
{
    // Set new alarm sequence number.
    alarmSeqNumber = seqNumber;
}

UINT8 omci_alarm_getSequenceNumber(void)
{
    UINT8 returnVal = alarmSeqNumber;

    // Inc alarm sequence number & test for overflow.
    if (++alarmSeqNumber == 0)
    {
        // Reset to next valid OMCI-spec value.
        alarmSeqNumber = INIT_ALARM_SEQ_VALUE;
    }

    // Return current
    return returnVal;
}

void omci_alarm_sendRaw(
    char *msgTitle,
    omciPacket *pPacket,
    UINT16 objType,
    UINT16 objID)
{
    // Test if object is under ARC alarm supression.
    if (omci_arc_exist(objType, objID) == FALSE)
    {
        omci_msg_send(msgTitle, pPacket);
    }
}

void omci_alarm_send(
    UINT16 transID,
    UINT16 objType,
    UINT16 objID,
    UINT16 alarmMask)
{
    omciPacket packet;
    OmciMdmOidClassId_t info;

    // Test if object is under ARC alarm supression.
    if (omci_arc_exist(objType, objID) == TRUE)
    {
        cmsLog_notice("objType %d, objID %d is under ARC alarm supression", objType, objID);
        return;
    }

    memset(&packet, 0, sizeof(omciPacket));

    // Setup OMCI transaction identifier MSB & LSB.
    OMCI_HTONS(&packet.tcId, transID);

    // Setup OMCI message type.
    packet.msgType = OMCI_MSG_TYPE_ALARM;
    // Setup OMCI device identifier type.
    packet.devId = OMCI_PACKET_DEV_ID_A;

    // Convert MDM class ID to true-OMCI spec class ID.
    memset(&info, 0, sizeof(OmciMdmOidClassId_t));
    info.mdmOid = objType;
    omciUtl_oidToClassId(&info);

    // Setup OMCI message identifier.
    OMCI_HTONS(&packet.classNo, info.classId);
    OMCI_HTONS(&packet.instId, objID);

    // Setup alarmMask MSB & LSB.
    OMCI_HTONS(&OMCI_PACKET_MSG(&packet)[0], alarmMask);

    // Setup Alarm Sequence Number at message end.
    OMCI_PACKET_MSG(&packet)[OMCI_PACKET_A_MSG_SIZE - 1] = omci_alarm_getSequenceNumber();

    // Attempt to write alarm message to OMCI device descriptor
    omci_msg_send("SendAlarm", &packet);
}

BCM_OMCIPM_ALARM_ENTRY* omci_alarm_getFrozen(UINT16 alarmIndex)
{
    BCM_OMCIPM_ALARM_ENTRY* frozenEntry = NULL;
    BCM_COMMON_LLIST* pFrozenList = omci_alarm_getFrozenListPtr();

    frozenEntry = omci_alarm_getByIndex(pFrozenList, alarmIndex);

    // Return next entry on success, NULL on error.
    return frozenEntry;
}

// Freeze current alarms, count alarm entries (depending on ARC flag) and lock alarm state.
UINT16 omci_alarm_getFrozenAll(UINT8 arcFlag)
{
    UBOOL8 addEntryFlag = TRUE;
    BCM_OMCIPM_ENTRY* omcipmEntry = NULL;
    BCM_OMCIPM_ALARM_ENTRY* nonTcaAlarmEntry = NULL;
    BCM_COMMON_LLIST* pFrozenList = omci_alarm_getFrozenListPtr();
    BCM_COMMON_LLIST* pNonTcaList = omci_alarm_getNonTcaListPtr();
    BCM_COMMON_LLIST* pPmList = omci_pm_getListPtr();

    // Release existing frozen alarm records (if any).
    omci_alarm_releaseByHeadList(pFrozenList);

    // Init alarm total number.
    alarmTotalNumber = 0;

    omcipmEntry = BCM_COMMON_LL_GET_HEAD(*pPmList);

    // Loop until PM entry end-of-list.
    while (omcipmEntry != NULL)
    {
        // Test for active alarm.
        if (omcipmEntry->reportedAlarmBits != 0)
        {
            if (omci_alarm_create(pFrozenList,
                                  omcipmEntry->omcipm.objType,
                                  omcipmEntry->omcipm.objID,
                                  omcipmEntry->reportedAlarmBits) == OMCI_PM_STATUS_SUCCESS)
            {
                // Increment alarm total number.
                alarmTotalNumber++;
            }
        }

        // Get list's next entry.
        omcipmEntry = BCM_COMMON_LL_GET_NEXT(omcipmEntry);
    }

    nonTcaAlarmEntry = BCM_COMMON_LL_GET_HEAD(*pNonTcaList);

    // Loop until non-TCA alarm end-of-list.
    while (nonTcaAlarmEntry != NULL)
    {
        // Signal add entry by default.
        addEntryFlag = TRUE;

        // Test if OLT wants only non-ARC alarms.
        if (arcFlag != 0)
        {
            // Test if alarm's object is currently under ARC and ignore if so.
            addEntryFlag = (omci_arc_exist(nonTcaAlarmEntry->objectType, nonTcaAlarmEntry->objectID) == FALSE);
        }

        // Test if OLT wants only non-ARC alarms.
        if (addEntryFlag == TRUE)
        {
            // Attempt to create entry
            if (omci_alarm_create(pFrozenList,
                                  nonTcaAlarmEntry->objectType,
                                  nonTcaAlarmEntry->objectID,
                                  nonTcaAlarmEntry->alarmBitmap) == OMCI_PM_STATUS_SUCCESS)
            {
                // Increment alarm total number.
                alarmTotalNumber++;
            }
        }

        // Get list's next entry.
        nonTcaAlarmEntry = BCM_COMMON_LL_GET_NEXT(nonTcaAlarmEntry);
    }

    // Return alarm total number.
    return alarmTotalNumber;
}

CmsRet omci_alarm_init(void)
{
#ifndef DESKTOP_LINUX
    // Attempt to open a basic socket for polling ethernet ports.
    if ((enetFd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        cmsLog_error("Cannot open basic socket for polling ethernet ports");
        return CMSRET_INTERNAL_ERROR;
    }

    // Attempt to open laser driver for polling optical signals
    if ((laserFd = open("/dev/laser_dev", O_RDWR)) < 0)
    {
        // Report error.
        cmsLog_error("Cannot open laser driver for polling optical signals");
        return CMSRET_INTERNAL_ERROR;
    }
#endif /* DESKTOP_LINUX */

    alarm_data_reset();

    return CMSRET_SUCCESS;
}

void omci_alarm_poll_gpon(void)
{
    BCM_Ploam_EventStatusInfo eventStatusInfo;
    BCM_Ploam_AlarmStatusInfo alarmStatusInfo;
    UINT32 maskedEventBitmap = 0;
    int result = 0;
    UBOOL8 isGponLinkChanged = FALSE;

    memset(&eventStatusInfo, 0x0, sizeof(BCM_Ploam_EventStatusInfo));
    memset(&alarmStatusInfo, 0x0, sizeof(BCM_Ploam_AlarmStatusInfo));

    // Get GPON event status.
    if ((result = gponCtl_getEventStatus(&eventStatusInfo)) == 0)
    {
        // Mask event bitmap.
        maskedEventBitmap = (eventStatusInfo.eventBitmap & ~eventStatusInfo.eventMask);

        // Test for ALARM event.
        if ((maskedEventBitmap & BCM_PLOAM_EVENT_ALARM) != 0)
        {
            // Attempt to get GPON alarm status.
            if ((result = gponCtl_getAlarmStatus(&alarmStatusInfo)) == 0)
            {
                if(omci_pm_isAdminStateEnabled(MDMOID_ANI_G, 0))
                {
                    // Test (and process) BCM_PLOAM_ALARM_ID_SF alarm.
                    // NOTE: Alarm soaked in GPON driver.
                    alarm_soak(ALARM_SOAK_GPON_SF, MDMOID_ANI_G,
                               omci_pm_getAniGObjID(), BCM_ALARM_ID_ANIG_SF_VAL,
                               ((alarmStatusInfo.alarmEventBitmap & BCM_PLOAM_ALARM_ID_SF) != 0));

                    // Test (and process) BCM_PLOAM_ALARM_ID_SF alarm.
                    // NOTE: Alarm soaked in GPON driver.
                    alarm_soak(ALARM_SOAK_GPON_SD, MDMOID_ANI_G,
                               omci_pm_getAniGObjID(), BCM_ALARM_ID_ANIG_SD_VAL,
                               ((alarmStatusInfo.alarmEventBitmap & BCM_PLOAM_ALARM_ID_SD) != 0));
                }
                // Notify OMCID about link status change
                if (alarmStatusInfo.alarmEventBitmap & (BCM_PLOAM_ALARM_ID_LOS|BCM_PLOAM_ALARM_ID_LOL|BCM_PLOAM_ALARM_ID_LOF))
                {
                    isGponLinkChanged = TRUE;
                    alarm_gpon_link_change();
                }
            }
            else
            {
                // Report error.
                cmsLog_error("gponCtl_getAlarmStatus returned error code %d", result);
            }
        }
        if ((isGponLinkChanged == FALSE) &&
            ((maskedEventBitmap & BCM_PLOAM_EVENT_GEM_STATE_CHANGE) != 0))
        {
            alarm_gpon_link_change();
        }
    }
    else
    {
        // Report error.
        cmsLog_error("gponCtl_getEventStatus returned error code %d", result);
    }
}

void omci_alarm_poll_enet(void)
{
    UINT32 portIndex = 0;
    int linkState = 0;
    struct ifreq ifReq;
    struct ethswctl_data ifData;
    UINT8 lineSpeedAttr = 0;
    UINT32 pptpObjID = INVALID_OBJ_ID;
    UINT32 line_Speed = 0;
    IOCTL_MIB_INFO* portMibInfoPtr = NULL;

    // Loop through ENET interfaces.
    for (portIndex = 0; portIndex < ethLanNum; portIndex++)
    {
        // Convert port index to object ID
        pptpObjID = omci_pm_getEnetPptpObjIDs(portIndex);

        // Test Ethernet PPTP's Administrative State.
        if (pptpObjID != INVALID_OBJ_ID &&
            omci_pm_isAdminStateEnabled(MDMOID_PPTP_ETHERNET_UNI, portIndex) == TRUE)
        {
            // Clear IOCTL structure.
            memset(&ifReq, 0, sizeof(ifReq));

            // Setup data pointer.
            memset(&ifData, 0, sizeof(ifData));
            ifReq.ifr_data = (char *)(&ifData);

            // Setup "eth0-3" string.
            sprintf(ifReq.ifr_name, "eth%d", portIndex);

            // Get link-state through IOCTL & test for successful result.
            if (omciUtil_ioctlWrap(enetFd, SIOCGLINKSTATE, &ifReq) == 0)
            {
                // Read port's current link-state.
                linkState = *((int*)&ifData);
                // Process alarm soak.
                alarm_soak(ALARM_SOAK_ENET_LOS_PORT0 + portIndex,
                           MDMOID_PPTP_ETHERNET_UNI, pptpObjID,
                           BCM_ALARM_ID_LOS_VAL, (linkState == 0));
            }

            // Test for valid ENET connection, do not test sensed-type unless link state is up.
            if (linkState != 0)
            {
                // Clear IOCTL structure.
                memset(&ifReq, 0, sizeof(ifReq));

                // Setup data pointer.
                memset(&ifData, 0, sizeof(ifData));
                ifReq.ifr_data = (char *)(&ifData);

                // Setup "eth0-3" string.
                sprintf(ifReq.ifr_name, "eth%d", portIndex);

                // Get link's sensed type through IOCTL.
                if (omciUtil_ioctlWrap(enetFd, SIOCMIBINFO, &ifReq) == 0)
                {
                    // Setup MIB info pointer
                    portMibInfoPtr = (IOCTL_MIB_INFO*)&ifData;

                    // Setup sensed line speed on active port.
                    line_Speed = (UINT32)portMibInfoPtr->ulIfSpeed;
                }

                // Test if port record has not yet been initialized.
                if (enetLineArray[portIndex].initFlag == FALSE)
                {
                    // Init line speed.
                    enetLineArray[portIndex].lineSpeed = line_Speed;

                    // Set Init flag.
                    enetLineArray[portIndex].initFlag = TRUE;
                }
                else
                {
                    // Test current line speed against previous.
                    if (enetLineArray[portIndex].lineSpeed != line_Speed)
                    {
                        // Set new line speed.
                        enetLineArray[portIndex].lineSpeed = line_Speed;

                        switch (line_Speed)
                        {
                            case SPEED_10MBIT:
                                lineSpeedAttr = OMCI_LINE_SENSE_10;
                                break;
                            case SPEED_100MBIT:
                                lineSpeedAttr = OMCI_LINE_SENSE_100;
                                break;
                            case SPEED_1000MBIT:
                                lineSpeedAttr = OMCI_LINE_SENSE_1000;
                                break;
                            default:
                                lineSpeedAttr = OMCI_LINE_SENSE_UNK;
                                break;
                        }

                        omciUtl_sendAttributeValueChange(MDMOID_PPTP_ETHERNET_UNI, pptpObjID,
                          BCM_AVC_ENET_LINE_VAL, &lineSpeedAttr,
                          sizeof(lineSpeedAttr));
                    }
                }
            }
        }
    }
}

void omci_alarm_poll_optical_signals(void)
{
    SINT32 rxPower = 0, txPower = 0;
    UBOOL8 existed = FALSE;
    UINT16 dataLen = 0;
    UINT32 val = 0;
    ThresholdData1Object data;

    if (omci_pm_isAdminStateEnabled(MDMOID_ANI_G, 0) == FALSE)
    {
        // if admin state is disabled then do nothing
        return;
    }

    existed = omci_th_isObjectExisted(MDMOID_THRESHOLD_DATA1,
                                      MDMOID_ANI_G);
    if (existed == FALSE)
    {
        // if no threshold then do nothing
        return;
    }

    memset(&data, 0, sizeof(ThresholdData1Object));
    omci_th_getObject(MDMOID_THRESHOLD_DATA1,
                      MDMOID_ANI_G,
                      (void *)&data,
                      &dataLen);

    if (omciUtil_ioctlWrap(laserFd, LASER_IOCTL_GET_RX_PWR, &val) >= 0)
    {
        rxPower = convertPointOneMicroWattsToOmcidB("RX", val, -32768, 32767, 2, 1);

        // thresholdValue1 = lowerOptivalThreshold
        if (data.thresholdValue1 != DEFAULT_RX_THRESHOLD_VAL_NP)
        {
            alarm_soak(ALARM_SOAK_ANIG_LO_RX_PWR, MDMOID_ANI_G,
                       omci_pm_getAniGObjID(), BCM_ALARM_ID_ANIG_LO_RX_OPTICAL_PWR,
                       (rxPower <= RX_THR_CONV(data.thresholdValue1)));
        }

        // thresholdValue2 = upperOptivalThreshold
        if (data.thresholdValue2 != DEFAULT_RX_THRESHOLD_VAL_NP)
        {
            alarm_soak(ALARM_SOAK_ANIG_HI_RX_PWR, MDMOID_ANI_G,
                       omci_pm_getAniGObjID(), BCM_ALARM_ID_ANIG_HI_RX_OPTICAL_PWR,
                       (rxPower >= RX_THR_CONV(data.thresholdValue2)));
        }
    }
    else
    {
        cmsLog_error("alarm_poll_optical_signals: LASER_IOCTL_GET_RX_PWR failed\n");
        return;
    }

    if (omciUtil_ioctlWrap(laserFd, LASER_IOCTL_GET_TX_PWR, &val) >= 0)
    {
        txPower = convertPointOneMicroWattsToOmcidB("TX", val, -32768, 32767, 2, 1);

        // thresholdValue3 = lowerTransmitPowerThreshold
        if (data.thresholdValue3 != DEFAULT_TX_THRESHOLD_VAL_NP)
        {
            alarm_soak(ALARM_SOAK_ANIG_LO_TX_PWR, MDMOID_ANI_G,
                       omci_pm_getAniGObjID(), BCM_ALARM_ID_ANIG_LO_TX_OPTICAL_PWR,
                       (txPower <= TX_THR_CONV(data.thresholdValue3)));
        }

        // thresholdValue4 = upperTransmitPowerThreshold
        if (data.thresholdValue4 != DEFAULT_TX_THRESHOLD_VAL_NP)
        {
            alarm_soak(ALARM_SOAK_ANIG_HI_TX_PWR, MDMOID_ANI_G,
                       omci_pm_getAniGObjID(), BCM_ALARM_ID_ANIG_HI_TX_OPTICAL_PWR,
                       (txPower >= TX_THR_CONV(data.thresholdValue4)));
        }
    }
    else
    {
        cmsLog_error("alarm_poll_optical_signals: LASER_IOCTL_GET_TX_PWR failed\n");
    }
}

void omci_alarm_data_reset(void)
{
    alarm_data_reset();
}

void omci_alarm_clear(UINT16 alarmSoakId)
{
    switch (alarmSoakId)
    {
        case ALARM_SOAK_ANIG_LO_RX_PWR:
            alarmSoakArray[alarmSoakId].soakState = ALARM_SOAK_IDLE;
            alarm_deleteNonTca(MDMOID_ANI_G, omci_pm_getAniGObjID(),
              BCM_ALARM_ID_ANIG_LO_RX_OPTICAL_PWR);
            break;
        case ALARM_SOAK_ANIG_HI_RX_PWR:
            alarmSoakArray[alarmSoakId].soakState = ALARM_SOAK_IDLE;
            alarm_deleteNonTca(MDMOID_ANI_G, omci_pm_getAniGObjID(),
              BCM_ALARM_ID_ANIG_HI_RX_OPTICAL_PWR);
            break;
        case ALARM_SOAK_ANIG_LO_TX_PWR:
            alarmSoakArray[alarmSoakId].soakState = ALARM_SOAK_IDLE;
            alarm_deleteNonTca(MDMOID_ANI_G, omci_pm_getAniGObjID(),
              BCM_ALARM_ID_ANIG_LO_TX_OPTICAL_PWR);
            break;
        case ALARM_SOAK_ANIG_HI_TX_PWR:
            alarmSoakArray[alarmSoakId].soakState = ALARM_SOAK_IDLE;
            alarm_deleteNonTca(MDMOID_ANI_G, omci_pm_getAniGObjID(),
              BCM_ALARM_ID_ANIG_HI_TX_OPTICAL_PWR);
            break;
        default:
            cmsLog_error("Invalid alarm soak ID: 0x%x", alarmSoakId);
            break;
    }
}

#endif // DMP_X_ITU_ORG_GPON_1
