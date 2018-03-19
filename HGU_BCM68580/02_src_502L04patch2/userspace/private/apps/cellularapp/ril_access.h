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


#ifndef _RIL_ACCESS_H
#define _RIL_ACCESS_H 

#include "cms.h"
#include "ril.h"

/* mSerial: used to track the ril response to the original request in ril_access.
 * mRequest: the RIL request used.
 * mResult: user provided callback data.
 * mParcel: the parcel format of the request to be sent to rild.
 * mNext: pointer to the next record in the double-linked list.
 * */
typedef struct ril_request
{
    unsigned int mSerial;
    int mRequest;
    struct internal_message *mResult;
    struct parcel* mParcel;
    struct ril_request* mNext;
}RILRequest;


typedef enum ril_response_type
{
    RESPONSE_SOLICITED = 0,
    RESPONSE_UNSOLICITED = 1
}RilResponseType;

typedef enum radio_state
{
    RADIO_OFF = 0,
    RADIO_UNAVAILABLE = 1,
    RADIO_ON = 2
}RadioState;

typedef enum service_class
{
    SERVICE_CLASS_NONE = 0,
    SERVICE_CLASS_VOICE = (1 << 0),
    SERVICE_CLASS_DATA  = (1 << 1), //synonym for 16+32+64+128
    SERVICE_CLASS_FAX   = (1 << 2),                         
    SERVICE_CLASS_SMS   = (1 << 3),                         
    SERVICE_CLASS_DATA_SYNC = (1 << 4),                        
    SERVICE_CLASS_DATA_ASYNC = (1 << 5),                       
    SERVICE_CLASS_PACKET   = (1 << 6),                        
    SERVICE_CLASS_PAD      = (1 << 7),                         
    SERVICE_CLASS_MAX      = (1 << 7) // Max SERVICE_CLASS value
}ServiceClass;

typedef enum dc_deactivate_reason
{    /* Deactivate data call reasons */                                          
    DEACTIVATE_REASON_NONE = 0,                                             
    DEACTIVATE_REASON_RADIO_OFF = 1,                                        
    DEACTIVATE_REASON_PDP_RESET = 2
}DeactivateReason;

extern const char CB_FACILITY_BAOC[];
extern const char CB_FACILITY_BAOIC[];
extern const char CB_FACILITY_BAOICxH[];
extern const char CB_FACILITY_BAIC[];
extern const char CB_FACILITY_BAICr[];
extern const char CB_FACILITY_BA_ALL[];
extern const char CB_FACILITY_BA_MO[];
extern const char CB_FACILITY_BA_MT[];
extern const char CB_FACILITY_BA_SIM[];
extern const char CB_FACILITY_BA_FD[];
                               
extern const int RIL_MAX_COMMAND_BYTES;

SINT32 ril_access_init(void);
struct internal_message* ril_access_read(SINT32 ril_fd);
RadioState ril_access_get_radio_state();
void ril_access_set_radio_power(UBOOL8 on, struct internal_message* result);
void ril_access_get_voice_registration_state(struct internal_message* result);
void ril_access_get_data_registration_state(struct internal_message* result);
void ril_access_get_operator(struct internal_message* result); 
void ril_access_get_icc_card_status(struct internal_message* result);
void ril_access_query_facility_lock_for_app(const char* facility, const char* password, 
                int serviceClass, const char* appId, struct internal_message* result);
void ril_access_get_imsi_for_app(char *aid, struct internal_message* result);
void ril_access_get_device_identity(struct internal_message* result);
void ril_access_deactivate_data_call(int cid, int reason, 
                                   struct internal_message *result);
void ril_access_setup_data_call(char *radioTechnology, char *profile,
                      char *apn, char *user, char *password, char *authType,
                      char *protocol, struct internal_message *result);
void ril_access_icc_io_for_app(int command, int fileid, const char* path, int p1, int p2,
        int p3, const char* data, const char* pin2, const char* aid, struct internal_message* result);

void ril_access_dial(char *address, int clirMode, RIL_UUS_Info *uusInfo, 
                                               struct internal_message* result);
void ril_access_hangup_connection(int gsmIndex, struct internal_message* result);
void ril_access_accept_call(struct internal_message* result);
void ril_access_get_current_calls(struct internal_message* result);
void ril_access_get_available_networks(struct internal_message* result);
void ril_access_get_operator(struct internal_message* result);
void ril_access_get(SINT32 rilReq, struct internal_message* result);

#endif

