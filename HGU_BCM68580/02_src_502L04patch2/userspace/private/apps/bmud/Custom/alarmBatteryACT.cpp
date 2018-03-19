/*
* <:copyright-BRCM:2014:proprietary:standard
* 
*    Copyright (c) 2014 Broadcom 
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

#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include "board.h"

#include "alarmBatteryACT.h"
#include "BatteryThread.h"


alarmBatteryACT::alarmBatteryACT()
	: BcmCompletionHandlerACT()
{
	// Subscribe to battery events that we are interested in
	BcmBatteryThread::Singleton().SubscribeEventNote (BcmBatteryThread::kBatteryDepleted, this); 
	BcmBatteryThread::Singleton().SubscribeEventNote (BcmBatteryThread::kBatteryNotDepleted, this); 
	BcmBatteryThread::Singleton().SubscribeEventNote (BcmBatteryThread::kBatteryStatusUpdated, this); 
	BcmBatteryThread::Singleton().SubscribeEventNote (BcmBatteryThread::kBatteryStatusUpdated, this); 
	BcmBatteryThread::Singleton().SubscribeEventNote (BcmBatteryThread::kSwitchingToBatteryPower, this); 
}

alarmBatteryACT::~alarmBatteryACT()
{
}

void alarmBatteryACT::SetDyingGaspInterrupt(int enable)
{
	int brcmBoardFd;
	BOARD_IOCTL_PARMS ioctlParms;

	if( enable )
	{
		/* We want to force the DG interrtupt to be enabled, overriding all 
		* mechanisms which might prevent it from getting enabled*/
		ioctlParms.offset = (int)DG_ENABLE_FORCE;
	}
	else
	{
		/* We want to disable DG interrupt, and prevent any other source
		* from enabling it */
		ioctlParms.offset = (int)DG_DISABLE_PREVENT_ENABLE;
	}

	brcmBoardFd = open(BOARD_DEVICE_NAME, O_WRONLY);
	ioctl(brcmBoardFd, BOARD_IOCTL_SET_DYING_GASP_INTERRUPT, &ioctlParms);
	close(brcmBoardFd);
}

void alarmBatteryACT::HandleEvent( const BcmCompletionEvent &event_code )
{
	static int statusUpdateCountdown = 4; // Wait for 4 updates before checking battery status
	static int batteriesHaveCharge = 0;

	switch( event_code )
	{
		case BcmBatteryThread::kBatteryDepleted:
			if (BcmBatteryThread::Singleton().BatteryController().OperatingOnBattery())
			{
	 			/* This is meant to cause the system to send Dying Gasp message and reboot */
	 			printf("!Battery DG!\n");
	 			SetDyingGaspInterrupt(1);
	 			printf("Dying Gasp Interrupt is enabled: Battery depleted\n"); /* This may not have time to print */
			}
	 		break;

		case BcmBatteryThread::kBatteryNotDepleted:
			if (BcmBatteryThread::Singleton().BatteryController().OperatingOnBattery())
			{
	 			/* This case is not expected to hit, but was added to ensure proper logic */
	 			if (BcmBatteryThread::Singleton().BatteryController().NumberOfValidatedBatteriesWithoutErrors() > 0)
				{
	 	 			SetDyingGaspInterrupt(0);
	 	 			printf("Dying Gasp Interrupt is disabled: Battery no longer depleted\n");
	 			}
			}
	 		break;

		case BcmBatteryThread::kSwitchingToBatteryPower:
	 		/* If we switched to battery power, and made it here, the batteries are holding a charge. */
	 		/* Process the case where we did not know that yet (batteriesHaveCharge is 0) */
			if (!batteriesHaveCharge)
			{
	 	 		/* Even though Dying Gasp Interrupt starts as disabled, go ahead and disable it */
				/* in case this code is hit under other circumstances */
	 	 		SetDyingGaspInterrupt(0);
	 	 		printf("Dying Gasp Interrupt is disabled: Running on battery power\n");
				batteriesHaveCharge = 1;
	 		}
	 		break;

		case BcmBatteryThread::kBatteryStatusUpdated: // Called whether batteries are present or not
	 		int newBatteriesHaveCharge = (BcmBatteryThread::Singleton().BatteryController().PercentChargeRemaining() > 0);

			/* Delay processing of battery update during system initialisation */
	 		if (statusUpdateCountdown > 0)
			{
				--statusUpdateCountdown;
			}

	 		if (statusUpdateCountdown == 1 && !newBatteriesHaveCharge)
			{
				/* No operational batteries were found after waiting sufficiently to discover them */
	 	 		SetDyingGaspInterrupt(1);
	 	 		printf("Dying Gasp Interrupt is enabled: No operational battery found\n");
				batteriesHaveCharge = newBatteriesHaveCharge;
			}

	 		if (statusUpdateCountdown == 0)
			{
	 	 		if (batteriesHaveCharge && !newBatteriesHaveCharge)
				{
					// Determine if batteries are not present. If present, a little bit of charge remains before reaching depleted state
					if (BcmBatteryThread::Singleton().BatteryController().NumberOfValidatedBatteriesWithoutErrors() == 0)
					{
						/* Operational batteries are no longer present */
	 	 	 			SetDyingGaspInterrupt(1);
	 	 	 			printf("Dying Gasp Interrupt is enabled: Operational battery removed\n");
						batteriesHaveCharge = newBatteriesHaveCharge;
					}
				}				
			}

	 		if (!batteriesHaveCharge && newBatteriesHaveCharge)
			{
				/* Operational batteries were just inserted */
	 	 		SetDyingGaspInterrupt(0);
	 	 		printf("Dying Gasp Interrupt is disabled: Operational battery found\n");
				batteriesHaveCharge = newBatteriesHaveCharge;
				statusUpdateCountdown = 0;
			}
	 		break;
	}
}

