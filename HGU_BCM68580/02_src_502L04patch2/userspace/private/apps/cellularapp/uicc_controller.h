/*
 <:copyright-BRCM:2015:NONE/NONE:standard
 
    Copyright (c) 2015 Broadcom 
    All Rights Reserved
 
 :>

 *****************************************************************************
 * Copyright (C) 2005 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ****************************************************************************

*/

#ifndef _UICC_CONTROLLER_H
#define _UICC_CONTROLLER_H 

#include "cms.h"
#include "ril.h"
#include "cellular_msg.h"

/**
 * This class is responsible for keeping all knowledge about
 * Universal Integrated Circuit Card (UICC), also know as SIM's,
 * in the system. It is also used as API to get appropriate
 * applications to pass them to phone and service trackers.
 *
 * When UiccController receives "unsol_sim_status_changed" ril notifications, 
 * UiccController will call getIccCardStatus (GET_SIM_STATUS). Based on the
 * response of GET_SIM_STATUS request appropriate tree of uicc objects will be
 * created.
 *
 * Following is class diagram for uicc classes in android:
 *
 *                       UiccController
 *                            #
 *                            |
 *                        UiccCard
 *                          #   #
 *                          |   ------------------
 *                    UiccCardApplication    CatService
 *                      #            #
 *                      |            |
 *                 IccRecords    IccFileHandler
 *                 ^ ^ ^           ^ ^ ^ ^ ^
 *    SIMRecords---- | |           | | | | ---SIMFileHandler
 *    RuimRecords----- |           | | | ----RuimFileHandler
 *    IsimUiccRecords---           | | -----UsimFileHandler
 *                                 | ------CsimFileHandler
 *                                 ----IsimFileHandler
 *
 * However, the cellularapp would simplify above diagram to contain only 
 * UiccController (which monitors various events), and UiccCardApplication, 
 * which contains information about SIM status. For now let's see if the 
 * simplified picture work or not.
 *
 */

typedef struct uicc_card_application
{
    UBOOL8 mIccFdnEnabled;
    UBOOL8 mIccFdnAvailable;
    UBOOL8 mIccLockEnabled;
    char *mImsi;
    int mMncLength;
    RIL_AppStatus mAppStatus;
}UiccCardApplication;

typedef struct uicc_card_status
{
    RIL_CardState card_state;
    RIL_PinState  universal_pin_state;             /* applicable to USIM and CSIM: RIL_PINSTATE_xxx */
    int           gsm_umts_subscription_app_index; /* value < RIL_CARD_MAX_APPS, -1 if none */
    int           cdma_subscription_app_index;     /* value < RIL_CARD_MAX_APPS, -1 if none */
    int           ims_subscription_app_index;      /* value < RIL_CARD_MAX_APPS, -1 if none */
    int           num_applications;                /* value <= RIL_CARD_MAX_APPS */
    UiccCardApplication applications[RIL_CARD_MAX_APPS];
}UiccCardStatus;

/* Markers for mncLength */
typedef enum 
{
    MNC_LENGTH_UNINITIALIZED = -1,
    MNC_LENGTH_UNKNOWN = 0
}MncLengthMarker;


void uicc_controller_process(struct internal_message *msg);
void uicc_controller_init();
char *uicc_controller_getOperatorNumeric(int application_index);
const char* uicc_controller_getIccId();
const char* uicc_controller_get_msisdn();   
const char* uicc_controller_getImsi(int application_index);
RIL_CardState uicc_controller_get_card_status();

#endif
