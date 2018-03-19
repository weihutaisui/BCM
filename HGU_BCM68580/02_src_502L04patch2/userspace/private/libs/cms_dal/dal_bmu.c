/***********************************************************************
 *
 *  Copyright (c) 2012  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:standard

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


#ifdef SUPPORT_BMU

#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <time.h>

#include "board.h"
#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "bcmtypes.h"
#include "bmu_api.h"

int dalBmu_isEnabled(void)
{
    int battery_enabled;
    int brcmBoardFd;

    brcmBoardFd = open("/dev/brcmboard", O_WRONLY);
    battery_enabled = ioctl(brcmBoardFd, BOARD_IOCTL_GET_BATTERY_ENABLE, (void *)NULL);
    close(brcmBoardFd);

    return (battery_enabled);
}

CmsRet dalGetBmu(WEB_NTWK_VAR *pWebVar, void *msgHandleArg)
{
    BmuStatus_type bmuStatus;
	BatteryStatus_type   * pA;
	BatteryStatus_type   * pB;
    CmsRet ret = CMSRET_SUCCESS;
    char * pState = NULL;

    if (!dalBmu_isEnabled()) {
        strcpy(pWebVar->bmuState, "Not Supported");
        pWebVar->bmuVersion[0] = 0;
        pWebVar->bmuBuildDateTime[0] = 0;
        pWebVar->bmuPowerSource[0] = 0;
        pWebVar->bmuNumberOfPresentBatteries = 0;
        strcpy(pWebVar->bmuInputVoltage, "0");
        pWebVar->bmuTemperature = 0;
        pWebVar->bmuEstimatedMinutesRemaining = 0;
        pWebVar->bmuUpsSecondsOnBattery = 0;
        pWebVar->bmuStatusBatteryA[0] = 0;
        pWebVar->bmuStatusBatteryB[0] = 0;
        pWebVar->bmuCapacityBatteryA = 0;
        pWebVar->bmuCapacityBatteryB = 0;
        strcpy(pWebVar->bmuMeasuredVoltageBatteryA, "0");
        strcpy(pWebVar->bmuMeasuredVoltageBatteryB, "0");
        pWebVar->bmuEstimatedTimeRemainingBatteryA = 0;
        pWebVar->bmuEstimatedTimeRemainingBatteryB = 0;
        return ret;
    }

    if ((ret = BmuMsg_send(msgHandleArg, cmsMsg_getHandleEid(msgHandleArg), CMS_MSG_BMU_STATUS_GET, 0, &bmuStatus)) == CMSRET_SUCCESS)
    {
        strncpy(pWebVar->bmuVersion, bmuStatus.Version, strlen(bmuStatus.Version)+1);
        sprintf(pWebVar->bmuBuildDateTime, "%s",ctime(&bmuStatus.BuildDateTime));
        sprintf(pWebVar->bmuPowerSource, "%s", (bmuStatus.OperatingOnBattery?"Battery":"Utility"));
	    switch(bmuStatus.State)
	    {
            case kBmuInit: pState = "Initialization"; break;
            case kBmuIdle: pState = "Idle"; break;
            case kBmuSleep: pState = "Sleep"; break;
            case kBmuChargeInit: pState = "Charge Init"; break;
            case kBmuPrecharge: pState = "Charge Prequal"; break;
            case kBmuFastCharge: pState = "Fast Charge"; break;
            case kBmuTopoff: pState = "Charge Topoff"; break;
            case kBmuChargeSuspended: pState = "Charge suspended"; break;
            case kBmuDischarge: pState = "Discharge"; break;
            case kBmuEternalSleep: pState = "Eternal sleep"; break;
            case kBmuForcedDischarge: pState = "Forced discharge"; break;
	    }
        if (pState)
            sprintf(pWebVar->bmuState, "%s", pState);
        pWebVar->bmuNumberOfPresentBatteries  = bmuStatus.NumberOfPresentBatteries;
        sprintf(pWebVar->bmuInputVoltage, "%d.%03d", bmuStatus.InputVoltage/1000, bmuStatus.InputVoltage-((bmuStatus.InputVoltage/1000)*1000));
        pWebVar->bmuTemperature               = bmuStatus.Temperature;
        pWebVar->bmuEstimatedMinutesRemaining = bmuStatus.EstimatedMinutesRemaining;
        pWebVar->bmuUpsSecondsOnBattery       = bmuStatus.UpsSecondsOnBattery;

        pA = &bmuStatus.BatteryStatus[0];
        pB = &bmuStatus.BatteryStatus[1];
        sprintf(pWebVar->bmuStatusBatteryA,pA->BatteryPresent?(pA->BatteryBad?"Bad":"Good"):"Not Present");
        sprintf(pWebVar->bmuStatusBatteryB,pB->BatteryPresent?(pB->BatteryBad?"Bad":"Good"):"Not Present");
        pWebVar->bmuCapacityBatteryA = pA->BatteryChargeCapacity;
        pWebVar->bmuCapacityBatteryB = pB->BatteryChargeCapacity;
        sprintf(pWebVar->bmuMeasuredVoltageBatteryA, "%d.%03d", pA->BatteryMeasuredVoltage/1000, pA->BatteryMeasuredVoltage-((pA->BatteryMeasuredVoltage/1000)*1000));
        sprintf(pWebVar->bmuMeasuredVoltageBatteryB, "%d.%03d", pB->BatteryMeasuredVoltage/1000, pB->BatteryMeasuredVoltage-((pB->BatteryMeasuredVoltage/1000)*1000));
        pWebVar->bmuEstimatedTimeRemainingBatteryA = pA->BatteryEstimatedMinutesRemaining;
        pWebVar->bmuEstimatedTimeRemainingBatteryB = pB->BatteryEstimatedMinutesRemaining;
    }
    return ret;
}


#endif  /* SUPPORT_BMU */

