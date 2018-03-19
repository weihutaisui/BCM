/*
* <:copyright-BRCM:2010:proprietary:standard
* 
*    Copyright (c) 2010 Broadcom 
*    All Rights Reserved
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
*/

#include <stdio.h>
#include <stdlib.h>
#include "cms.h"
#include "cms_eid.h"
#include "cms_msg.h"
#include "cms_mem.h"
#if defined(BRCM_CMS_BUILD)
#include "cms_mdm.h"
#include "cms_hexbinary.h"
#include "cms_core.h"
#endif
#if !defined(CHIP_63148) && !defined(CHIP_63138)
#include "bcm_map.h"
#include "bcm_mmap.h"
#endif
#if defined(BCM3382)
#include "3382_map.h"
#include "3382_BatteryRegs.h"
#elif defined(BCM3385)
#include "3385_map.h"
#include "3385_BatteryRegs.h"
#endif

#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include "board.h"
#include "bcmtypes.h"
#include "NonVolSettings.h"
#include "MergedNonVolDefaults.h"
#include "BatteryNonVolSettings.h"
#include "BatteryCommandTable.h"
#include "BatteryThread.h"
#include "bmu_api.h"
#include "alarmBatteryACT.h"

// Global variable used by the Battery code to control prints
// Unused in our code, but certain Bfc modules make references to it
uint16 DebugZone;

// Global variable allowing userspace app to access registers
BCM_MMAP_INFO bcm_mmap_info;

#ifdef __cplusplus
extern "C" {
#endif

// Function that mimics the behavior of the Bfc command line interface
void BmuInvokeBfcCommand(char *cmd, BcmBatteryCommandTable *pBatteryCmdTable)
{
    BcmCommandTable *pfActiveCommandTable = pBatteryCmdTable;
    BcmCommandTable *pfSavedCommandTable;
    BcmCommandTable *pfMainCommandTable = pBatteryCmdTable;
    BcmCommandTable::ExecuteResult result = BcmCommandTable::kNotFound;

    char * thisCommand=cmd;
    char * nextCommand;
    unsigned int    charCount;
    
    // Store the existing command table in case we need to temporarily
    // change the active command table
    pfSavedCommandTable = pfActiveCommandTable;

    // Iterate over backquote-delimited commands.
    while (thisCommand != NULL)
    {
        result = BcmCommandTable::kNotFound;
        
        // Find the end of this backquote-delimited command.  Terminate it
        // with a null character and save a pointer to the next command, if
        // one exists.
        nextCommand = strchr( thisCommand, '`' );
        if (nextCommand != NULL)
        {
            *nextCommand = '\0';
            nextCommand++;
        }

        // Skip over leading white-space characters.  This makes it easy to 
        // tell if the user entered a null command between backquotes.
        charCount = strspn( thisCommand, " \t" );
        thisCommand += charCount;

        // Check for root table reference
        while (*thisCommand == '/' || *thisCommand == '\\')
        {
            pfActiveCommandTable = pfMainCommandTable;
            thisCommand++;
        }

        // Look for a / or \ character in the command name, indicating a subtable
        while ((charCount = strcspn(thisCommand, "/\\")) < strcspn(thisCommand, " \t"))
        {
            // Null the separator
            thisCommand[charCount] = '\0';

            if (charCount == 0)
            {
                // Ignore multiple /'s
            }
            // This takes us up one level.
            else if (strcmp(thisCommand, "..") == 0)
            {
                // Don't go above the main table...
                if (pfActiveCommandTable != pfMainCommandTable)
                {
                    pfActiveCommandTable = pfActiveCommandTable->ParentTable();
                }
            }
            // Update our command table or indicate an error
            else if (!pfActiveCommandTable->IsValidSubtable(thisCommand, pfActiveCommandTable))
            {
                gLogMessageRaw
                     << "'" << thisCommand 
                     << "' is not a valid command table.\n\nType 'help' for information about valid commands and tables."
                     << endl;
            
                result = BcmCommandTable::kError;
            }

            // Proceed with remaining command
            thisCommand += (charCount+1);
        }

        
        if ((result == BcmCommandTable::kNotFound) && 
            (pfActiveCommandTable != NULL))
        {
            BcmCommandTable *pSubtable;

            result = pfActiveCommandTable->ExecuteCommand(thisCommand);

            // If it's not in here, then see if they did something like
            // just typing the name of a subtable.
            if ((result == BcmCommandTable::kNotFound) &&
                (pfActiveCommandTable->IsValidSubtable(thisCommand,
                                                       pSubtable) == true))
            {
                gLogMessageRaw
                     << "'" << pSubtable->Name() 
                     << "' is a command table.\n\nType 'help " 
                     << pSubtable->Name() 
                     << "' for information about the table, or\n'cd "
                     << pSubtable->Name() << "' to change to that table.\n";
            }
        }

        // Return to saved command table
        pfActiveCommandTable = pfSavedCommandTable;

        // Break if the command wasn't successful.  In case the user typed two 
        // backquotes (cd non-vol``ls), don't break on an empty string.
        if ((*thisCommand != '\0') && 
            (result != BcmCommandTable::kFound))
            break;

        // Loop again to see if there is another command.
        thisCommand = nextCommand;
    }

    // If the command was not processed, then complain about it.  If it was
    // found, or if there was a different kind of error, then it has already
    // been complained about.
    if (result == BcmCommandTable::kNotFound)
    {
        gLogMessageRaw << "Error - Unknown command:  '"
             << thisCommand << "'" << endl;
        pBatteryCmdTable->Print(cout, BcmCommandTable::kTableInfoAndCommandsRecursive);
    }

}

// Function that extracts status information from the battery controller
void BmuControllerStatusInternalGet(BmuStatus_type *pBmuStatus)
{
    BcmBatteryThread     * pBatteryThread = &BcmBatteryThread::Singleton();
    BcmBatteryController * pBatteryController = &pBatteryThread->BatteryController();
    time_t Now;

    time(&Now);

    strcpy(pBmuStatus->Version, pBatteryController->Version().c_str());
    pBmuStatus->BuildDateTime             = pBatteryController->BuildDateTime();
    pBmuStatus->OperatingOnBattery        = pBatteryController->OperatingOnBattery();    
    pBmuStatus->State                     = (BmuControllerState)pBatteryController->State();
    pBmuStatus->NumberOfPresentBatteries  = pBatteryController->NumberOfPresentBatteries();
    pBmuStatus->InputVoltage              = pBatteryController->InputVoltage();
    pBmuStatus->Temperature               = pBatteryController->Temperature();
    pBmuStatus->EstimatedMinutesRemaining = pBatteryController->EstimatedMinutesRemaining();
    pBmuStatus->BatteryCurrent            = pBatteryController->BatteryCurrent();
    if (pBatteryController->OperatingOnBattery() == false) {
        pBmuStatus->UpsSecondsOnBattery = 0;
    } else {
        pBmuStatus->UpsSecondsOnBattery       = Now - pBatteryThread->PowerSourceStartTime();
    }
}

void BmuBatteryStatusInternalGet(BatteryStatus_type *pBatteryStatus, int battery)
{
    BcmBatteryThread     * pBatteryThread = &BcmBatteryThread::Singleton();
    BcmBatteryController * pBatteryController = &pBatteryThread->BatteryController();

    pBatteryStatus->BatteryPresent = pBatteryController->BatteryPresent(battery);
    if (pBatteryStatus->BatteryPresent) {
        pBatteryStatus->BatteryValid                     = pBatteryController->BatteryValid(battery);
        pBatteryStatus->BatteryBad                       = pBatteryController->BatteryBad(battery);
        pBatteryStatus->BatterySelected                  = pBatteryController->BatterySelected(battery);
        pBatteryStatus->BatteryFullyCharged              = pBatteryController->BatteryFullyCharged(battery);
        pBatteryStatus->BatteryChargeLow                 = pBatteryController->BatteryChargeLow(battery);
        pBatteryStatus->BatteryChargeLowPercent          = pBatteryController->BatteryChargeLowPercent(battery);
        pBatteryStatus->BatteryChargeDepleted            = pBatteryController->BatteryChargeDepleted(battery);
        pBatteryStatus->BatteryChargeStateUnknown        = pBatteryController->BatteryChargeStateUnknown(battery);
        pBatteryStatus->BatteryChargeCapacity            = pBatteryController->BatteryChargeCapacity(battery);
        pBatteryStatus->BatteryActualCapacity            = pBatteryController->BatteryActualCapacity(battery);
        pBatteryStatus->BatteryFullChargeVoltage         = pBatteryController->BatteryFullChargeVoltage(battery);
        pBatteryStatus->BatteryDepletedVoltage           = pBatteryController->BatteryDepletedVoltage(battery);
        pBatteryStatus->BatteryMeasuredVoltage           = pBatteryController->BatteryMeasuredVoltage(battery);
        pBatteryStatus->BatteryPercentCharge             = pBatteryController->BatteryPercentCharge(battery); 
        pBatteryStatus->BatteryEstimatedMinutesRemaining = pBatteryController->BatteryEstimatedMinutesRemaining(battery);
        pBatteryStatus->BatteryTemperature               = pBatteryController->PackTemperature(battery);
        pBatteryStatus->BatteryLifeTestCount             = pBatteryController->BatteryLifeTestCount(battery);
        pBatteryStatus->BatteryLastLifeTest              = pBatteryController->BatteryLastLifeTest(battery);
        pBatteryStatus->BatteryNextLifeTest              = pBatteryController->BatteryNextLifeTest(battery);
        pBatteryStatus->BatteryLifeTestState             = (BmuLifeTestState)pBatteryController->BatteryLifeTestState(battery);
        if (pBatteryStatus->BatteryChargeCapacity > 0)
            pBatteryStatus->BatteryStateofHealth         = 100*pBatteryStatus->BatteryActualCapacity/pBatteryStatus->BatteryChargeCapacity;
        else
            pBatteryStatus->BatteryStateofHealth = 0;
    }
}

void BmuStatusInternalGet(BmuStatus_type *pBmuStatus)
{
    int battery;

    BmuControllerStatusInternalGet(pBmuStatus);
    for (battery = 0; battery < MAX_BATTERIES; ++battery) {
        BmuBatteryStatusInternalGet(&(pBmuStatus->BatteryStatus[battery]), battery);
    }
}

#if defined(BRCM_CMS_BUILD)
// When the CMS configuration changes, BMU needs to update its internal data
void BmuControllerConfigInternalSet(void)
{
    BcmBatteryNonVolSettings *pBcmBatteryNonVolSettings = BcmBatteryNonVolSettings::GetSingletonInstance();
    BatteryManagementConfigurationObject *batteryConfigObj=NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = cmsLck_acquireLock();

    if (ret != CMSRET_SUCCESS)
    {
        printf("%s failed to get lock, ret=%d", __FUNCTION__, ret);
        return;
    }

    if((ret = cmsObj_get(MDMOID_BATTERY_MANAGEMENT_CONFIGURATION, &iidStack, 0, (void **)&batteryConfigObj)) == CMSRET_SUCCESS)
    {
        if (pBcmBatteryNonVolSettings->PollIntervalSeconds() != batteryConfigObj->pollIntervalSeconds)
            pBcmBatteryNonVolSettings->PollIntervalSeconds(batteryConfigObj->pollIntervalSeconds);

        if (pBcmBatteryNonVolSettings->MaxChargePercent() != batteryConfigObj->maxChargePercent)
            pBcmBatteryNonVolSettings->MaxChargePercent(batteryConfigObj->maxChargePercent);

        if (pBcmBatteryNonVolSettings->UpsConfigLowBattTime() != batteryConfigObj->upsConfigLowBattTime)
            pBcmBatteryNonVolSettings->UpsConfigLowBattTime(batteryConfigObj->upsConfigLowBattTime);

        if (pBcmBatteryNonVolSettings->ReplacementThresholdPercent() != batteryConfigObj->replacementThresholdPercent)
            pBcmBatteryNonVolSettings->ReplacementThresholdPercent(batteryConfigObj->replacementThresholdPercent);

        if (pBcmBatteryNonVolSettings->LowChargePercent() != batteryConfigObj->lowChargePercent)
            pBcmBatteryNonVolSettings->LowChargePercent(batteryConfigObj->lowChargePercent);

        if (pBcmBatteryNonVolSettings->LifeTestingEnabled() != batteryConfigObj->lifeTestingEnabled)
            pBcmBatteryNonVolSettings->LifeTestingEnabled(batteryConfigObj->lifeTestingEnabled);

        if (pBcmBatteryNonVolSettings->LifeTestPeriod() != batteryConfigObj->lifeTestPeriod)
            pBcmBatteryNonVolSettings->LifeTestPeriod(batteryConfigObj->lifeTestPeriod);

        if (memcmp(pBcmBatteryNonVolSettings->LifeTestTOD(), batteryConfigObj->lifeTestTOD, 6) != 0)
            pBcmBatteryNonVolSettings->LifeTestTOD(batteryConfigObj->lifeTestTOD);

        if (pBcmBatteryNonVolSettings->PackVoltageSmoothingSteps() != batteryConfigObj->packVoltageSmoothingSteps)
            pBcmBatteryNonVolSettings->PackVoltageSmoothingSteps(batteryConfigObj->packVoltageSmoothingSteps);

        if (pBcmBatteryNonVolSettings->BoardVoltageSmoothingSteps() != batteryConfigObj->boardVoltageSmoothingSteps)
            pBcmBatteryNonVolSettings->BoardVoltageSmoothingSteps(batteryConfigObj->boardVoltageSmoothingSteps);

        if (pBcmBatteryNonVolSettings->PackTemperatureSmoothingSteps() != batteryConfigObj->packTemperatureSmoothingSteps)
            pBcmBatteryNonVolSettings->PackTemperatureSmoothingSteps(batteryConfigObj->packTemperatureSmoothingSteps);

        if (pBcmBatteryNonVolSettings->BoardTemperatureSmoothingSteps() != batteryConfigObj->boardTemperatureSmoothingSteps)
            pBcmBatteryNonVolSettings->BoardTemperatureSmoothingSteps(batteryConfigObj->boardTemperatureSmoothingSteps);

        if (pBcmBatteryNonVolSettings->AllowedFaults() != batteryConfigObj->allowedFaults)
            pBcmBatteryNonVolSettings->AllowedFaults(batteryConfigObj->allowedFaults);

        cmsObj_free((void **)&batteryConfigObj);
    }
    cmsLck_releaseLock();
}

// Function that extracts Battery Non-Vol information
// Note that the index is not the same as the battery index, the nonvol info is identified by guid.
void BmuBatteryNonVolInternalSet(void)
{
    BcmBatteryNonVolSettings *pBcmBatteryNonVolSettings = BcmBatteryNonVolSettings::GetSingletonInstance();
    BatteryManagementBatteryNonVolObject *batteryNonVolObj=NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = cmsLck_acquireLock();

    if (ret != CMSRET_SUCCESS)
    {
        printf("%s failed to get lock, ret=%d", __FUNCTION__, ret);
        return;
    }

    while(cmsObj_getNext(MDMOID_BATTERY_MANAGEMENT_BATTERY_NON_VOL, &iidStack, (void **)&batteryNonVolObj) == CMSRET_SUCCESS)
    {
        TNonVolBatteryInfo *nvi = pBcmBatteryNonVolSettings->GetBattery(batteryNonVolObj->index);
        if (nvi)
        {
            UINT8 * buf = NULL;
            UINT32 buflen = 0;

            nvi->flags = batteryNonVolObj->flags;
            nvi->EstLifeRemaining = batteryNonVolObj->estLifeRemaining;
            nvi->DesignCapacity = batteryNonVolObj->designCapacity;
            nvi->maxWh = (float)batteryNonVolObj->maxMicroWh/1000000;
            nvi->guid = batteryNonVolObj->guid;
            nvi->LifeTestCount = batteryNonVolObj->lifeTestCount;
            nvi->LastLifeTestStarted = batteryNonVolObj->lastLifeTestStarted;
            nvi->NextUpdate = batteryNonVolObj->nextLifeTestUpdate;
            nvi->totalSeconds = batteryNonVolObj->totalSeconds;
            nvi->seriesResistance = batteryNonVolObj->seriesResistance;
            if (batteryNonVolObj->dchgVoltages)
                cmsUtl_hexStringToBinaryBuf(batteryNonVolObj->dchgVoltages, &buf, &buflen);
            if (buflen == sizeof(nvi->dchgVoltages))
                memcpy(nvi->dchgVoltages, buf, sizeof(nvi->dchgVoltages));
            if (buf)
                cmsMem_free(buf);
            cmsObj_free((void **)&batteryNonVolObj);
        }
    }
    cmsLck_releaseLock();
}

void BmuBatteryNonVolInternalGet(void)
{
    BcmBatteryNonVolSettings *pBcmBatteryNonVolSettings = BcmBatteryNonVolSettings::GetSingletonInstance();
    BatteryManagementBatteryNonVolObject *batteryNonVolObj=NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = cmsLck_acquireLock();
    char * hexstr = NULL;
    int saveRequired = 0;

    if (ret != CMSRET_SUCCESS)
    {
        printf("%s failed to get lock, ret=%d", __FUNCTION__, ret);
        return;
    }

    while(cmsObj_getNext(MDMOID_BATTERY_MANAGEMENT_BATTERY_NON_VOL, &iidStack, (void **)&batteryNonVolObj) == CMSRET_SUCCESS)
    {
        TNonVolBatteryInfo *nvi = pBcmBatteryNonVolSettings->GetBattery(batteryNonVolObj->index);

        if (nvi->flags & kBATTERYNVINFOVALID)
        {
            int changed = 0;

            if (batteryNonVolObj->flags != nvi->flags)
            {
                batteryNonVolObj->flags = nvi->flags;
                changed = 1;
            }
            if (batteryNonVolObj->estLifeRemaining != nvi->EstLifeRemaining)
            {
                batteryNonVolObj->estLifeRemaining = nvi->EstLifeRemaining;
                changed = 1;
            }
            if (batteryNonVolObj->designCapacity != nvi->DesignCapacity)
            {
                batteryNonVolObj->designCapacity = nvi->DesignCapacity;
                changed = 1;
            }
            if (batteryNonVolObj->maxMicroWh != (UINT32)(nvi->maxWh * 1000000))
            {
                batteryNonVolObj->maxMicroWh = (UINT32)(nvi->maxWh * 1000000);
                changed = 1;
            }
            if (batteryNonVolObj->guid != nvi->guid)
            {
                batteryNonVolObj->guid = nvi->guid;
                changed = 1;
            }
            if (batteryNonVolObj->lifeTestCount != nvi->LifeTestCount)
            {
                batteryNonVolObj->lifeTestCount = nvi->LifeTestCount;
                changed = 1;
            }
            if (batteryNonVolObj->lastLifeTestStarted != (UINT64)nvi->LastLifeTestStarted)
            {
                batteryNonVolObj->lastLifeTestStarted = (UINT64)nvi->LastLifeTestStarted;
                changed = 1;
            }
            if (batteryNonVolObj->nextLifeTestUpdate != (UINT64)nvi->NextUpdate)
            {
                batteryNonVolObj->nextLifeTestUpdate = (UINT64)nvi->NextUpdate;
                changed = 1;
            }
            if (batteryNonVolObj->totalSeconds != nvi->totalSeconds)
            {
                batteryNonVolObj->totalSeconds = nvi->totalSeconds;
                changed = 1;
            }
            if (batteryNonVolObj->seriesResistance != nvi->seriesResistance)
            {
                batteryNonVolObj->seriesResistance = nvi->seriesResistance;
                changed = 1;
            }
            cmsUtl_binaryBufToHexString((UINT8 *)nvi->dchgVoltages, sizeof(nvi->dchgVoltages), &hexstr);
            if ((batteryNonVolObj->dchgVoltages == NULL) || memcmp((UINT8 *)batteryNonVolObj->dchgVoltages, hexstr, sizeof(nvi->dchgVoltages)) != 0 )
            {
                CMSMEM_REPLACE_STRING_FLAGS(batteryNonVolObj->dchgVoltages, hexstr, ALLOC_SHARED_MEM);
                changed = 1;
            }
            cmsMem_free(hexstr);

            if (changed) {
                ret = cmsObj_set(batteryNonVolObj, &iidStack);
                saveRequired = 1;
            }
            cmsObj_free((void **)&batteryNonVolObj);
        }
    }
    if (saveRequired) {
        cmsMgm_saveConfigToFlash();
    }
    cmsLck_releaseLock();
}
#endif

// Function needed by the Bfc Linux operating system and that could not be implemented through Linux standard i/f
unsigned long TimeMs(void)
{
    unsigned long currentTime;
    int brcmBoardFd;

    brcmBoardFd = open("/dev/brcmboard", O_WRONLY);
    currentTime = ioctl(brcmBoardFd, BOARD_IOCTL_GET_TIMEMS, (void *)NULL);
    close(brcmBoardFd);

    return (currentTime);
}

int IsBatteryEnabled(void)
{
    int battery_enabled;
    int brcmBoardFd;

    brcmBoardFd = open(BOARD_DEVICE_NAME, O_WRONLY);
    battery_enabled = ioctl(brcmBoardFd, BOARD_IOCTL_GET_BATTERY_ENABLE, (void *)NULL);
    close(brcmBoardFd);

    return (battery_enabled);
}

int GetChipRev(void)
{
    BOARD_IOCTL_PARMS ioctlParms;
    int brcmBoardFd;

    brcmBoardFd = open(BOARD_DEVICE_NAME, O_WRONLY);
    ioctl(brcmBoardFd, BOARD_IOCTL_GET_CHIP_REV, &ioctlParms);
    close(brcmBoardFd);
    return ioctlParms.result;
}

// The battery daemon code. Normally does not exit.
int main(int argc, char **argv)
{
    CmsMsgHeader *pMsgIn;
    CmsRet ret = CMSRET_SUCCESS;
    uint8_t buf[sizeof(BmuStatus_type) + sizeof(CmsMsgHeader)];
    CmsMsgHeader *pMsgOut = (CmsMsgHeader *)&buf[0];
#if defined(BRCM_CMS_BUILD)
    SINT32  shmId = UNINITIALIZED_SHM_ID;
    CmsMsgHeader msg = {}; // To get shmId
#endif
    void *msgHandle;

    BcmBatteryCommandTable *pBatteryCmdTable;
    BcmBatteryNonVolSettings *pBcmBatteryNonVolSettings __attribute__((unused));

    if (!IsBatteryEnabled()) {
        daemon(1, 1); // Avoid zombie process
        exit(0);
    }

    printf("Starting Battery Daemon\n");

    // Initialize CMS messaging
    if ((ret = cmsMsg_init(EID_BMUD, &msgHandle)) != CMSRET_SUCCESS) {
        printf("cmsMsg_init failed, ret=%d\n", ret);
        goto done;
    }

#if defined(BRCM_CMS_BUILD)
    msg.src = EID_BMUD;
    msg.dst = EID_SMD;
    msg.type = CMS_MSG_GET_SHMID;
    msg.flags_request = 1;
    msg.dataLength = 0;

    shmId = (SINT32)cmsMsg_sendAndGetReplyWithTimeout(msgHandle, &msg, 5000);
    if (shmId == (SINT32)CMSRET_TIMED_OUT)  /* assumes shmId is never 9809, which is value of CMSRET_TIMED_OUT */
    {
        printf("could not get shmId from smd (shmId=%d)\n", shmId);
        return UNINITIALIZED_SHM_ID;
    }

    if (cmsMdm_initWithAcc(EID_BMUD, 0, msgHandle, &shmId) != CMSRET_SUCCESS)
    {
        printf("cmsMdm_init failed");
        cmsMsg_cleanup(&msgHandle);
        goto done;
    }
#endif

    // Make registers accessible to this app
    bcm_mmap_regs(bcm_mmap_info);

    // Start the battery management hardware and software threads
#if defined(BMU_CLK_EN)
    PERF->blkEnables |= BMU_CLK_EN;
#elif defined(APM_BMU_CLKEN)
    PERF->blkEnables |= APM_BMU_CLKEN;
#elif defined(CHIP_63138) || defined(CHIP_63148)
    // BMU block is enabled in kernel board driver for chips supporting PMC
#else
    #error "Missing definition to enable BMU CLK"
#endif

    pBcmBatteryNonVolSettings = new BcmBatteryNonVolSettings;
    pBatteryCmdTable = new BcmBatteryCommandTable;
    pBatteryCmdTable->AddInstance ("BFC Battery Thread", &BcmBatteryThread::Singleton());

#if defined(BRCM_CMS_BUILD)
    // Set the Controller and Battery Configurations using data from NVRAM
    BmuControllerConfigInternalSet();
    BmuBatteryNonVolInternalSet();
#endif

    // Now start the battery thread
    BcmBatteryThread::Singleton().Start();

    // listen to messages
    while (1) {
        alarmBatteryACT batteryACT; // Handler object for battery alarms

        if ((ret = cmsMsg_receiveWithTimeout(msgHandle, &pMsgIn, 20*1000)) == CMSRET_SUCCESS) {
            // Create response in case it is needed
            memset(&buf[0], 0x0, sizeof(buf));
            pMsgOut->src          = EID_BMUD;
            pMsgOut->dst          = pMsgIn->src;
            pMsgOut->flags_event = 1;

            switch ((int)pMsgIn->type) {
                case CMS_MSG_BMU_CLI:
                    if (pMsgIn->dataLength > 0) {
                        char *pcmd = (char *)(pMsgIn + 1);

                        // pre-parse for redirection
                        if (strncmp(pcmd, ">tty", sizeof ">tty" - 1) == 0) {
                            int ttylen = strcspn(pcmd + 1, " \n\t");
                            char path[sizeof "/dev/" + ttylen];

                            strncat(strcpy(path, "/dev/"), pcmd + 1, ttylen);
                            if (freopen(path, "a", stdout) == 0)
                                perror("open");
                            pcmd += ttylen + 1;
                        }
                        BmuInvokeBfcCommand(pcmd, pBatteryCmdTable);
                        freopen("/dev/console", "w", stdout);
                    }
                    else {
                        BmuInvokeBfcCommand((char *)"help", pBatteryCmdTable);
                    }
                    break;
                case CMS_MSG_BMU_STATUS_GET: // Gets controller and battery status at once
                    pMsgOut->type        = CMS_MSG_BMU_STATUS_GET;
                    pMsgOut->dataLength  = sizeof(BmuStatus_type);
                    BmuStatusInternalGet((BmuStatus_type *)(pMsgOut+1));
                    break;
#if defined(BRCM_CMS_BUILD)
                case CMS_MSG_BMU_CONTROLLER_STATUS_GET:
                    pMsgOut->type        = CMS_MSG_BMU_CONTROLLER_STATUS_GET;
                    pMsgOut->dataLength  = sizeof(BmuStatus_type);
                    BmuControllerStatusInternalGet((BmuStatus_type *)(pMsgOut+1));
                    break;
                case CMS_MSG_BMU_BATTERY_STATUS_GET:
                    pMsgOut->type        = CMS_MSG_BMU_BATTERY_STATUS_GET;
                    pMsgOut->dataLength  = sizeof(BatteryStatus_type);
                    BmuBatteryStatusInternalGet((BatteryStatus_type *)(pMsgOut+1), pMsgIn->wordData);
                    break;
                case CMS_MSG_BMU_CONFIG_CHANGE_NOTIFY:
                    BmuControllerConfigInternalSet();
                    break;
#endif
                default:
                    break;
                }
                CMSMEM_FREE_BUF_AND_NULL_PTR(pMsgIn); 

                // If a response was prepared, send it
                if (pMsgOut->type) {
                ret = cmsMsg_send(msgHandle, pMsgOut);
                if ( CMSRET_SUCCESS != ret )
                {
                    printf("bmud: cmsMsg_send failed, ret=%d", ret);
                }
            }
        }
#if defined(BRCM_CMS_BUILD)
        if (ret == CMSRET_TIMED_OUT) {
            // Check if battery data has changed which would require updating non volatile memory
            BmuBatteryNonVolInternalGet();
        }
#endif
    }

done:
    bcm_munmap_regs(bcm_mmap_info);

    exit( ret );
}

#ifdef __cplusplus
} /* end of extern "C" */
#endif
