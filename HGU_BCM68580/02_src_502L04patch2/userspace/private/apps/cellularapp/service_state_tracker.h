/*
 <:copyright-BRCM:2015:NONE/NONE:standard
 
    Copyright (c) 2015 Broadcom 
    All Rights Reserved
 
 :>

 ******************************************************************************
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
 */


#ifndef _SERVICE_STATE_TRACKER_H
#define _SERVICE_STATE_TRACKER_H 
#include "cellular_msg.h"

typedef enum service_state_enum
{
    /*Normal operation, the phone is registered with an operator either 
     * in home network or in roaming
     **/
    SERVICE_STATE_IN_SERVICE = 0,
    /*Phone is not registered with any operator, the phone can be currently
     * searching a new operator to register to, or not searching to register
     * at all, or registration denied, or radio signal is not available.
     * */
    SERVICE_STATE_OUT_OF_SERVICE = 1,
    /*The phone is registered and locked.*/
    SERVICE_STATE_EMERGENCY_ONLY = 2,
    /*Radio of telephony is explicitly powered off.*/
    SERVICE_STATE_POWER_OFF = 3
}ServiceStateEnum;
/**
 * RIL level registration state values from ril.h
 * ((const char **)response)[0] is registration state 0-6,
 *              0 - Not registered, MT is not currently searching
 *                  a new operator to register
 *              1 - Registered, home network
 *              2 - Not registered, but MT is currently searching
 *                  a new operator to register
 *              3 - Registration denied
 *              4 - Unknown
 *              5 - Registered, roaming
 *             10 - Same as 0, but indicates that emergency calls
 *                  are enabled.
 *             12 - Same as 2, but indicates that emergency calls
 *                  are enabled.
 *             13 - Same as 3, but indicates that emergency calls
 *                  are enabled.
 *             14 - Same as 4, but indicates that emergency calls
 *                  are enabled.
 */
enum RIL_REG_STATE{
    RIL_REG_STATE_NOT_REG = 0,
    RIL_REG_STATE_HOME = 1,
    RIL_REG_STATE_SEARCHING = 2,
    RIL_REG_STATE_DENIED = 3,
    RIL_REG_STATE_UNKNOWN = 4,
    RIL_REG_STATE_ROAMING = 5,
    RIL_REG_STATE_NOT_REG_EMERGENCY_CALL_ENABLED = 10,
    RIL_REG_STATE_SEARCHING_EMERGENCY_CALL_ENABLED = 12,
    RIL_REG_STATE_DENIED_EMERGENCY_CALL_ENABLED = 13,
    RIL_REG_STATE_UNKNOWN_EMERGENCY_CALL_ENABLED = 14
};


/**
 * Available radio technologies for GSM, UMTS and CDMA.
 * Duplicates the constants from hardware/radio/include/ril.h
 * This should only be used by agents working with the ril.  Others
 * should use the equivalent TelephonyManager.NETWORK_TYPE_*
 */
enum RIL_RADIO_TECHNOLOGY{
    RIL_RADIO_TECHNOLOGY_UNKNOWN = 0,
    RIL_RADIO_TECHNOLOGY_GPRS = 1,
    RIL_RADIO_TECHNOLOGY_EDGE = 2,
    RIL_RADIO_TECHNOLOGY_UMTS = 3,
    RIL_RADIO_TECHNOLOGY_IS95A = 4,
    RIL_RADIO_TECHNOLOGY_IS95B = 5,
    RIL_RADIO_TECHNOLOGY_1xRTT = 6,
    RIL_RADIO_TECHNOLOGY_EVDO_0 = 7,
    RIL_RADIO_TECHNOLOGY_EVDO_A = 8,
    RIL_RADIO_TECHNOLOGY_HSDPA = 9,
    RIL_RADIO_TECHNOLOGY_HSUPA = 10,
    RIL_RADIO_TECHNOLOGY_HSPA = 11,
    RIL_RADIO_TECHNOLOGY_EVDO_B = 12,
    RIL_RADIO_TECHNOLOGY_EHRPD = 13,
    RIL_RADIO_TECHNOLOGY_LTE = 14,
    RIL_RADIO_TECHNOLOGY_HSPAP = 15,
    RIL_RADIO_TECHNOLOGY_GSM = 16
};

/**
 * Available registration states for GSM, UMTS and CDMA.
 */
enum REGISTRATION_STATE{
    REGISTRATION_STATE_NOT_REGISTERED_AND_NOT_SEARCHING = 0,
    REGISTRATION_STATE_HOME_NETWORK = 1,
    REGISTRATION_STATE_NOT_REGISTERED_AND_SEARCHING = 2,
    REGISTRATION_STATE_REGISTRATION_DENIED = 3,
    REGISTRATION_STATE_UNKNOWN = 4,
    REGISTRATION_STATE_ROAMING = 5
};


typedef struct service_state{
    int mServiceState;
    int mVoiceRegState;
    int mDataRegState;
    UBOOL8 mRoaming;
    
    /* Current OperatorInfo */
    char* mOperatorAlphaLong;
    char* mOperatorAlphaShort;
    char* mOperatorNumeric;
    char* mOperatorState;

    UBOOL8 mIsManualNetworkSelection;

    UBOOL8 mIsEmergencyOnly;

    int mRilVoiceRadioTechnology;
    int mRilDataRadioTechnology;

    UBOOL8 mCssIndicator;
    int mNetworkId;
    int mSystemId;
    int mCdmaRoamingIndicator;
    int mCdmaDefaultRoamingIndicator;
    int mCdmaEriIconIndex;
    int mCdmaEriIconMode;
}ServiceState;

typedef struct service_state_polling_context{
    int context;
    int counter;
}ServiceStatePollingContext;

#define SIGNAL_STRENGTH_INVALID 0x7fffffff

void service_state_tracker_init();
void service_state_tracker_process(struct internal_message *msg);
void service_state_tracker_get_roaming(UBOOL8 *roaming);
void service_state_tracker_get_access_tech(int *tech);
void service_state_tracker_get_imei(char *imei, int buf_len);
void service_state_tracker_get_networkInUse(char *network, int buf_len);
void service_state_tracker_get_rssi(int* rssi);


#endif


