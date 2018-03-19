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


#include "ril_access.h"
#include "cellular_msg.h"
#include "uicc_controller.h"
#include "icc_file_handler.h"
#include "service_state_tracker.h"
#include "ril.h"
#include "cms.h"
#include "cms_log.h"
#include "cms_util.h"
#include "parcel.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

/* Refer to local_sockets.h for the correct socket directory. 
 * Should modify it to include the header file in the future.
 * */
#define RILD_ADDR "/tmp/socket/rild"

const char CB_FACILITY_BAOC[]         = "AO";                        
const char CB_FACILITY_BAOIC[]        = "OI";                        
const char CB_FACILITY_BAOICxH[]      = "OX";                        
const char CB_FACILITY_BAIC[]         = "AI";                        
const char CB_FACILITY_BAICr[]        = "IR";                        
const char CB_FACILITY_BA_ALL[]       = "AB";                        
const char CB_FACILITY_BA_MO[]        = "AG";                        
const char CB_FACILITY_BA_MT[]        = "AC";                        
const char CB_FACILITY_BA_SIM[]       = "SC";                        
const char CB_FACILITY_BA_FD[]        = "FD";                        
 
const int RIL_MAX_COMMAND_BYTES = (8 * 1024);
 
static struct ril_request *ril_request_table = NULL;
static RadioState mState;
static SINT32 ril_fd;

static SINT32 init_ril_socket();
static struct ril_request* ril_request_obtain(int ril_request, 
        struct internal_message* result);
static void ril_request_free(struct ril_request* request);
static void ril_access_send(struct ril_request* request);
static struct internal_message* process_response(struct parcel *p);
static struct internal_message* process_unsolicited(struct parcel* p);
static struct internal_message* process_solicited(struct parcel *p);

static Object* response_void(struct parcel *p);
static Object* response_ints(struct parcel *p);
static Object* response_datacall_list(struct parcel *p);
static Object* response_icc_card_status(struct parcel *p);
static Object* response_string(struct parcel *p);
static Object* response_strings(struct parcel *p);
static Object* response_operatorInfos(struct parcel *p);
static Object* response_setup_datacall(struct parcel *p);
static Object* response_call_list(struct parcel *p);
static Object* response_ICC_IO(struct parcel *p);
static Object* response_signalStrength(struct parcel *p);

static struct internal_message* notify_voice_network_state_changed();
static struct internal_message* notify_icc_status_changed();
static struct internal_message* notify_restricted_state_changed(struct object *ret);
static struct internal_message* notify_network_state_changed(struct object *ret);
static struct internal_message* notify_call_state_changed();
static struct internal_message* notify_pll_enable(void);
static struct internal_message* notify_pll_disable(void);
static struct internal_message* notify_ringback_tone(struct object *ret);
static struct internal_message* notify_new_sms(struct object *ret);
static struct internal_message* notify_new_sms_on_sim(struct object *ret);
static struct internal_message* notify_signal_strength(struct object *ret);

static RadioState get_radio_state_from_int(int stateInt);
static RIL_Data_Call_Response_v6* get_datacall_response(struct parcel *p, int ver);
static struct internal_message* switch_to_radio_state(RadioState newState);

static UBOOL8 ril_request_list_remove(struct ril_request* item);
static UBOOL8 ril_request_list_add(struct ril_request* item);
static struct ril_request* ril_request_list_find(UINT32 serial);

static const char *request_to_string(int request);

/* error is from RIL_Errno in ril.h.  This function must stay in-sync with ril.h. */
static const char *error_to_string(int error) {
    switch (error)
    {
        case RIL_E_SUCCESS:                             return "SUCCESS";
        case RIL_E_RADIO_NOT_AVAILABLE:                 return "RADIO_NOT_AVAILABLE";
        case RIL_E_GENERIC_FAILURE:                     return "GENERIC_FAILURE";
        case RIL_E_PASSWORD_INCORRECT:                  return "PASSWORD_INCORRECT";
        case RIL_E_SIM_PIN2:                            return "SIM_PIN2";
        case RIL_E_SIM_PUK2:                            return "SIM_PUK2";
        case RIL_E_REQUEST_NOT_SUPPORTED:               return "REQUEST_NOT_SUPPORTED";
        case RIL_E_CANCELLED:                           return "CANCELLED";
        case RIL_E_OP_NOT_ALLOWED_DURING_VOICE_CALL:    return "OP_NOT_ALLOWED_DURING_VOICE_CALL";
        case RIL_E_OP_NOT_ALLOWED_BEFORE_REG_TO_NW:     return "OP_NOT_ALLOWED_BEFORE_REG_TO_NW";
        case RIL_E_SMS_SEND_FAIL_RETRY:                 return "SMS_SEND_FAIL_RETRY";
        case RIL_E_SIM_ABSENT:                          return "SIM_ABSENT";
        case RIL_E_SUBSCRIPTION_NOT_AVAILABLE:          return "SUBSCRIPTION_NOT_AVAILABLE";
        case RIL_E_MODE_NOT_SUPPORTED:                  return "MODE_NOT_SUPPORTED";
        case RIL_E_FDN_CHECK_FAILURE:                   return "FDN_CHECK_FAILURE";
        case RIL_E_ILLEGAL_SIM_OR_ME:                   return "ILLEGAL_SIM_OR_ME";

        default:                                        return "<Unknown>";
    }
}


SINT32 ril_access_init(void)
{

    SINT32 fd = init_ril_socket();
    mState = RADIO_UNAVAILABLE;
    ril_fd = fd;
    return fd;
}
 
static SINT32 init_ril_socket()
{
    SINT32 ril_fd = 0;
    int rc = 0;
    struct sockaddr_un serverAddr;
    int serverLen;

    if ((ril_fd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0)
    {
       cmsLog_error("Could not create socket");
       return ril_fd;
    }

    bzero((char *)&serverAddr, sizeof(serverAddr));
    serverAddr.sun_family = AF_LOCAL;
    strcpy(serverAddr.sun_path, RILD_ADDR);
    serverLen = strlen(serverAddr.sun_path) + sizeof(serverAddr.sun_family);

    rc = connect(ril_fd, (struct sockaddr *)&serverAddr, serverLen);
    if (rc != 0)
    {
       cmsLog_error("connect to %s failed, rc=%d errno=%d", RILD_ADDR, rc, errno);
       close(ril_fd);
       return rc;
    }

    return ril_fd;
}

void ril_access_set_radio_power(UBOOL8 on, struct internal_message* result)
{
    struct ril_request* rr = ril_request_obtain(RIL_REQUEST_RADIO_POWER, result);
    parcel_writeInt32(rr->mParcel, 1);
    parcel_writeInt32(rr->mParcel, on? 1 : 0);

    ril_access_send(rr);
}

void ril_access_get_available_networks(struct internal_message* result)
{
    struct ril_request *rr =
        ril_request_obtain(RIL_REQUEST_QUERY_AVAILABLE_NETWORKS, result);              
                                                                                
    ril_access_send(rr); 
}

void ril_access_get_operator(struct internal_message* result)    
{
    struct ril_request *rr = ril_request_obtain(RIL_REQUEST_OPERATOR, result);              
                                                                                
    ril_access_send(rr); 
}

void ril_access_get_signal_strength(struct internal_message* result)    
{
    struct ril_request *rr = 
        ril_request_obtain(RIL_REQUEST_SIGNAL_STRENGTH, result);              
                                                                                
    ril_access_send(rr); 
}

void ril_access_get_voice_registration_state(struct internal_message* result)
{
    struct ril_request* rr                                                           
                = ril_request_obtain(RIL_REQUEST_VOICE_REGISTRATION_STATE, result);
                                                                                
    ril_access_send(rr);  

}

void ril_access_get_data_registration_state(struct internal_message* result)
{
    struct ril_request* rr                                                           
                = ril_request_obtain(RIL_REQUEST_DATA_REGISTRATION_STATE, result);
                                                                                
    ril_access_send(rr);  
}

void ril_access_get_icc_card_status(struct internal_message* result)
{
    struct ril_request* rr                                                           
                = ril_request_obtain(RIL_REQUEST_GET_SIM_STATUS, result);
                                                                                
    ril_access_send(rr);  
}

void ril_access_query_facility_lock_for_app(const char* facility, const char* password, 
                int serviceClass, const char* appId, struct internal_message* result) 
{                                     
#define STR_LEN 15
    struct ril_request* rr = 
                ril_request_obtain(RIL_REQUEST_QUERY_FACILITY_LOCK, result);
    char buffer[STR_LEN];
    int len = 0;
    
    /* initialize the buffer on stack to be all 0s */
    memset(&buffer, 0x0, STR_LEN);

    // count strings                                                            
    parcel_writeInt32(rr->mParcel, 4);                                                     
                                                                                
    parcel_writeCString(rr->mParcel, facility);                                           
    parcel_writeCString(rr->mParcel, password);                                           
                               
    /* make sure we don't overflow the buffer. The write size include the 
     * terminating '\0' 
     * */    
    len = snprintf((char *)&buffer, STR_LEN, "%d", serviceClass);

    /* check buffer again to make sure we have a correctly terminated string */
    if(len == STR_LEN)
    {
        /*
         * The buffer is not NULL terminated properly.
         * Free rr and return.
         */
        ril_request_free(rr);
        return;
    }

    parcel_writeCString(rr->mParcel, (char *)&buffer);                     
    parcel_writeCString(rr->mParcel, appId);                                              
                                                                                
    ril_access_send(rr);                                                                   
}                    


void ril_access_get_device_identity(struct internal_message* result)
{
    struct ril_request* rr = 
             ril_request_obtain(RIL_REQUEST_DEVICE_IDENTITY, result);

    ril_access_send(rr);
}

void ril_access_get_imsi_for_app(char *aid, struct internal_message* result)
{
    struct ril_request* rr                                                           
        = ril_request_obtain(RIL_REQUEST_GET_IMSI, result);

    parcel_writeInt32(rr->mParcel, 1);
    parcel_writeCString(rr->mParcel, aid); 
    ril_access_send(rr);  
}

void ril_access_icc_io_for_app(int command, int fileid, const char* path, int p1, int p2,
        int p3, const char* data, const char* pin2, const char* aid, struct internal_message* result)
{
    struct ril_request* rr                                                           
        = ril_request_obtain(RIL_REQUEST_SIM_IO, result);

    cmsLog_debug("command=0x%x", command);
    cmsLog_debug("fileid = 0x%x", fileid);
    cmsLog_debug("path=%s", (path==NULL)?"":path);
    cmsLog_debug("p1=%d", p1);
    cmsLog_debug("p2=%d", p2);
    cmsLog_debug("p3=%d", p3);
    cmsLog_debug("data=%s", (data==NULL)?"":data);
    cmsLog_debug("pin2=%s", (pin2==NULL)?"":pin2);
    cmsLog_debug("aid=%s", (aid==NULL)?"":aid);

    parcel_writeInt32(rr->mParcel, command);
    parcel_writeInt32(rr->mParcel, fileid);
    parcel_writeCString(rr->mParcel, path);
    parcel_writeInt32(rr->mParcel, p1);
    parcel_writeInt32(rr->mParcel, p2);
    parcel_writeInt32(rr->mParcel, p3);
    parcel_writeCString(rr->mParcel, data);
    parcel_writeCString(rr->mParcel, pin2);
    parcel_writeCString(rr->mParcel, aid);

    ril_access_send(rr);  
}

void ril_access_setup_data_call(char *radioTechnology, char *profile,
                      char *apn, char *user, char *password, char *authType,
                      char *protocol, struct internal_message *result)
{
    struct ril_request* rr                                                           
        = ril_request_obtain(RIL_REQUEST_SETUP_DATA_CALL, result);

    cmsLog_debug("radioTechnology=%s", radioTechnology);
    cmsLog_debug("profile=%s", profile);
    cmsLog_debug("apn=%s", apn);
    cmsLog_debug("user=%s", (user==NULL)?"":user);
    cmsLog_debug("passord=%s", (password==NULL)?"":password);
    cmsLog_debug("authType=%s", authType);
    cmsLog_debug("protocol=%s", protocol);
    
    parcel_writeInt32(rr->mParcel, 7);

    parcel_writeCString(rr->mParcel, radioTechnology);
    parcel_writeCString(rr->mParcel, profile);
    parcel_writeCString(rr->mParcel, apn);
    parcel_writeCString(rr->mParcel, user);
    parcel_writeCString(rr->mParcel, password);
    parcel_writeCString(rr->mParcel, authType);
    parcel_writeCString(rr->mParcel, protocol);

    ril_access_send(rr);  

}

void ril_access_deactivate_data_call(int cid, int reason, 
        struct internal_message *result)
{
#define STR_LEN 15
    struct ril_request* rr                                                           
        = ril_request_obtain(RIL_REQUEST_DEACTIVATE_DATA_CALL, result);
    char buffer[STR_LEN];
    int len = 0;

    memset((char *)buffer, 0x0, STR_LEN);

    parcel_writeInt32(rr->mParcel, 2);

    len = snprintf((char *)&buffer, STR_LEN, "%d", cid);
    if(len == STR_LEN)
    {
        goto fail;
    }

    parcel_writeCString(rr->mParcel, (char *)&buffer);
    
    len = snprintf((char *)&buffer, STR_LEN, "%d", reason);
    if(len == STR_LEN)
    {
        goto fail;
    }
    parcel_writeCString(rr->mParcel, (char *)&buffer);

    ril_access_send(rr);
    return;

fail:
    ril_request_free(rr);
    return;
}

static int mPreferredNetworkType = PREF_NET_TYPE_LTE_GSM_WCDMA;

void ril_access_set_preferred_network_type(int networkType, struct internal_message* result)
{
    struct ril_request* request = ril_request_obtain(
                           RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE, result);
    parcel_writeInt32(request->mParcel, 1);
    parcel_writeInt32(request->mParcel, networkType);

    mPreferredNetworkType = networkType;
    ril_access_send(request);
}

void ril_access_set_cell_info_list_rate(int rateInMillis, struct internal_message* result)
{
    struct ril_request* request = ril_request_obtain(
            RIL_REQUEST_SET_UNSOL_CELL_INFO_LIST_RATE, result);
    parcel_writeInt32(request->mParcel, 1);
    parcel_writeInt32(request->mParcel, rateInMillis);

    ril_access_send(request);

}

void ril_access_dial(char *address, int clirMode, RIL_UUS_Info *uusInfo, 
                                               struct internal_message* result)
{

    struct ril_request* request = ril_request_obtain(RIL_REQUEST_DIAL, result);
    parcel_writeCString(request->mParcel, address);
    parcel_writeInt32(request->mParcel, clirMode);

    if(uusInfo == NULL)
    {
        parcel_writeInt32(request->mParcel, 0); 
    }
    else
    {
        parcel_writeInt32(request->mParcel, 1);
        parcel_writeInt32(request->mParcel, uusInfo->uusType);
        parcel_writeInt32(request->mParcel, uusInfo->uusDcs);
        parcel_write(request->mParcel, uusInfo->uusData, uusInfo->uusLength);
    }

    ril_access_send(request);
}

void ril_access_accept_call(struct internal_message* result)
{
    struct ril_request* request = ril_request_obtain(RIL_REQUEST_ANSWER, result);


    ril_access_send(request);

}

void ril_access_hangup_connection(int gsmIndex, struct internal_message* result)
{
    struct ril_request* request;


    cmsLog_debug("gsmIndex=%d", gsmIndex);

    /* Hangup for established call */
    if (gsmIndex > 0) {
        request = ril_request_obtain(RIL_REQUEST_HANGUP, result);
        parcel_writeInt32(request->mParcel, 1); 
        parcel_writeInt32(request->mParcel, gsmIndex); 
    }
    /* Hangup for un-established call */
    else {
        request = ril_request_obtain(RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND, result);
    }
    ril_access_send(request);

}

void ril_access_get_current_calls(struct internal_message* result)
{
    struct ril_request* request = ril_request_obtain(
                                        RIL_REQUEST_GET_CURRENT_CALLS, result);

    ril_access_send(request);
}


void ril_access_get(SINT32 rilReq, struct internal_message* result)
{
    struct ril_request* request = ril_request_obtain(rilReq, result);

    cmsLog_debug("ril_access_send:(%d)%s", request_to_string(rilReq));
	
    ril_access_send(request);
}

/**                                                                         
 * Reads in a single RIL message off the wire. A RIL message consists       
 * of a 4-byte little-endian length and a subsequent series of bytes.       
 * The final message (length header omitted) is read into                   
 * <code>buffer</code> and the length of the final message (less header)    
 * is returned. A return value of -1 indicates end-of-stream.                                                                      
 *                                                                                                                                 
 * @param is non-NULL; Stream to read from                                                                                         
 * @param buffer Buffer to fill in. Must be as large as maximum                                                                    
 * message size, or an ArrayOutOfBounds exception will be thrown.                                                                  
 * @return Length of message less header, or -1 on end of stream.                                                                  
 */             
static int read_ril_message(SINT32 fd, char* buffer)            
{                                                
    int countRead;                                                          
    int offset;                                                             
    int remaining;                                                          
    int messageLength;                                                      

    // First, read in the length of the message                             
    offset = 0;                                                             
    remaining = 4;                                                          
    do {                                                                    
        countRead = read(fd, buffer+offset, remaining);                     

        if (countRead < 0 ) {                                               
            cmsLog_error("Hit EOS reading message length\n");         
            return -1;                                                      
        }                                                                   

        offset += countRead;                                                
        remaining -= countRead;                                             
    } while (remaining > 0);                                                

    messageLength = ((buffer[0] & 0xff) << 24)                              
        | ((buffer[1] & 0xff) << 16)                                    
        | ((buffer[2] & 0xff) << 8)                                     
        | (buffer[3] & 0xff);                                           

    cmsLog_debug("messageLength = %d\n", messageLength);

    /* Guard against stack buffer overflow */
    if(messageLength >= RIL_MAX_COMMAND_BYTES)
    {
        return -1;
    }

    // Then, re-use the buffer and read in the message itself               
    offset = 0;                                                             
    remaining = messageLength;

    do {                                                                    
        countRead = read(fd, buffer+offset, remaining);                     

        if (countRead < 0 ) 
        {                                               
            cmsLog_error("Hit EOS reading message.  messageLength=%d", messageLength);
            cmsLog_error("remaining=%d\n", remaining);                           
            return -1;                                                      
        }                                                                   

        offset += countRead;                                                
        remaining -= countRead;                                             
    } while (remaining > 0);                                                

    return messageLength;
}

struct internal_message* ril_access_read(SINT32 ril_fd) //<< suman: cellular app reading RIL messages
{
    int length;
    char buffer[RIL_MAX_COMMAND_BYTES];
    struct internal_message* msg;

    length = read_ril_message(ril_fd, (char *)buffer);
    if(length < 0)
    {
        return NULL;
    }
    /* parcel_obtain would return the parcel from heap.
     * need to free it after the usage.
     * */
    struct parcel *p = parcel_obtain();
    parcel_setData(p, (uint8_t *)buffer, length);
    msg = process_response(p);
    parcel_free(p);

    return msg;
}

static struct internal_message* process_response(struct parcel *p)
{
    int type;
    struct internal_message* msg = NULL;
   
    parcel_readInt32(p, &type);
   
    if(type == RESPONSE_UNSOLICITED)
    {
        msg = process_unsolicited(p);
    }
    else if(type == RESPONSE_SOLICITED)
    {
        msg = process_solicited(p);
    }

    return msg;
}

static void ril_access_send(struct ril_request* request)
{
    UINT8 dataLength[4];
    int parcel_size;

    assert(request);
    if(ril_fd <= 0)
    {
        cmsLog_error("ril socket not created yet!");
        return;
    }
    cmsLog_debug("sending %s to rild\n", request_to_string(request->mRequest));

    parcel_size = parcel_dataSize(request->mParcel);
    if(parcel_size > RIL_MAX_COMMAND_BYTES)
    {
        cmsLog_error("parcel data size exceeds maximum limit!\n");
        parcel_free(request->mParcel);
        return;
    } 

    dataLength[0] = dataLength[1] = 0;
    dataLength[2] = (parcel_size >> 8) & 0xff;
    dataLength[3] = parcel_size & 0xff;

    write(ril_fd, &dataLength, 4);
    write(ril_fd, parcel_data(request->mParcel), parcel_size);

    /* *
     * after the sending, we have no use for mParcel in the request 
     * But the ril_request itself is still needed to process solicited
     * response. Free the parcel to save memory.
     * */
    parcel_free(request->mParcel);
    request->mParcel = NULL;
}


/*return the generated token in incrementally manner */
static unsigned int ril_request_generate_token()
{
    static unsigned int seed = 256;
	
    /*Token less than 256 is considered as test token by RILD, here wind back to 256*/
    if (seed == 0) 
    {
       seed = 256;
    }
	
    return seed++;
}

/* 
 * assume that the "result" is allocated on the heap by the user of
 * ril_access. 
 * */
static struct ril_request* ril_request_obtain(int ril_request, 
        struct internal_message* result)
{
    struct ril_request *rr;
    rr = (struct ril_request*) malloc(sizeof(struct ril_request));
    rr->mSerial = ril_request_generate_token();
    rr->mRequest = ril_request;
    rr->mResult = result;
    rr->mNext = NULL;
    rr->mParcel = parcel_obtain();
    parcel_writeInt32(rr->mParcel, rr->mRequest);
    parcel_writeInt32(rr->mParcel, rr->mSerial);

    if(ril_request_list_add(rr) == TRUE)
    {
        cmsLog_debug("added new request %s, token=%d", 
                     request_to_string(ril_request), rr->mSerial);
        if(result != NULL)
        {
            cmsLog_debug("callback internal message dest=%d, what=%d", 
                                       result->dest, result->what);
        }
    }
    else
    {
        /* Unexpected error, ril_request is not added to the list...*/
        ril_request_free(rr);
        rr = NULL;
    }
    return rr;
}

static void ril_request_free(struct ril_request* rr)
{
    assert(rr);

    if(rr->mResult != NULL)
    {
        /* the mResult was previously allocated in the heap by the RIL 
         * request user. Thus we have to free it after the response
         * processing. */
        internal_message_freeObject(rr->mResult);
        free(rr->mResult);
        rr->mResult = NULL;
    }

    if(rr->mParcel != NULL)
    {
        parcel_free(rr->mParcel);
        rr->mParcel = NULL;
    }

    ril_request_list_remove(rr);
    free(rr);
}

static struct internal_message* process_unsolicited(struct parcel* p)
{
    Object *ret = NULL;
    int response;
    int t;
    struct internal_message* msg = NULL;
    
    parcel_readInt32(p, &response);

    cmsLog_debug("response=%s\n", request_to_string(response));

    switch(response)
    {
        case RIL_UNSOL_CALL_RING:
            break;
        case RIL_UNSOL_RESPONSE_PLL_STATUS_INFO:
            ret = response_void(p);
            break;
        case RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED:
            ret = response_void(p);
            break;

        case RIL_UNSOL_RIL_CONNECTED: 
            ret = response_ints(p); 
            break;

        case RIL_UNSOL_RESTRICTED_STATE_CHANGED:
            ret = response_ints(p);
            break;

        case RIL_UNSOL_DATA_CALL_LIST_CHANGED:
            ret = response_datacall_list(p);
            break;
                
        case RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED:
            ret = response_void(p);
            break;
        
        case RIL_UNSOL_RINGBACK_TONE:
            ret = response_ints(p);
            break;

        case RIL_UNSOL_RESPONSE_NEW_SMS:
            ret = response_string(p);
            break;

        case RIL_UNSOL_RESPONSE_NEW_SMS_ON_SIM:
            ret = response_ints(p);
            break;

        case RIL_UNSOL_SIGNAL_STRENGTH:
            ret = response_signalStrength(p);
            break;

        default:
            break;
    }

    switch(response)
    {
        case RIL_UNSOL_RIL_CONNECTED:
            ril_access_set_radio_power(FALSE, NULL);
            ril_access_set_preferred_network_type(mPreferredNetworkType, NULL);
            ril_access_set_cell_info_list_rate(65535, NULL);    
            msg = NULL;
            break;

        case RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED:
            parcel_readInt32(p, &t);
            RadioState newState = get_radio_state_from_int(t);
            msg = switch_to_radio_state(newState);
            break;

        case RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED:
            msg = notify_voice_network_state_changed(); 
            break;

        case RIL_UNSOL_RESTRICTED_STATE_CHANGED:
            msg = notify_restricted_state_changed(ret);
            break;

        case RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED:
            msg = notify_icc_status_changed();
            break;

        case RIL_UNSOL_DATA_CALL_LIST_CHANGED:
            msg = notify_network_state_changed(ret);
            break;

        case RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED:
            msg = notify_call_state_changed();
            break;

        case RIL_UNSOL_RINGBACK_TONE:
            msg = notify_ringback_tone(ret);
            break;

        case RIL_UNSOL_RESPONSE_NEW_SMS:
            msg = notify_new_sms(ret);
            break;

        case RIL_UNSOL_RESPONSE_NEW_SMS_ON_SIM:
            if(object_dataLength(ret)/sizeof(int) == 1)
            {
                msg = notify_new_sms_on_sim(ret);
            }
            else
            {
                cmsLog_error("NEW_SMS_ON_SIM ERROR with wrong length!");
                msg = NULL;
            }
            break;

        case RIL_UNSOL_SIGNAL_STRENGTH:
            msg = notify_signal_strength(ret);
            break;

        case RIL_UNSOL_RESPONSE_PLL_STATUS_INFO:
            parcel_readInt32(p, &t);
            if (t == 1) {
              msg = notify_pll_enable();
            }
            else {
              msg = notify_pll_disable();
            }
            break;

        default:
            if((ret != NULL) && (msg == NULL))
            {
                cmsLog_notice("unhandled unsolicited message, but ret is not NULL");
                object_freeData(ret);
                free(ret);
                ret = NULL;
            }
            break;
    }

    /* 
     * The data contained in ret should now be hooked up to internal_message.
     * It would be freed after the internal_message is consumed
     */
    return msg;
}

/* Use the serial number as index to find the original ril_request in the list */
static struct ril_request* ril_request_list_find(UINT32 serial)
{
    struct ril_request* i = ril_request_table;
    while(i != NULL)
    {
        if(i->mSerial == serial)
        {
            return i;
        }
        i = i->mNext;
    }
    return NULL;
}

static UBOOL8 ril_request_list_add(struct ril_request* item)
{
    struct ril_request* i;

    i = ril_request_table;

    if(i == NULL)
    {
        ril_request_table = item;
        return TRUE;
    }

    while((i->mNext != NULL) && (i->mSerial != item->mSerial))
    {
        i = i->mNext;
    }
    
    if((i->mNext == NULL) && (i->mSerial != item->mSerial))
    {
        i->mNext = item;
        return TRUE;
    }
    return FALSE;
}

static UBOOL8 ril_request_list_remove(struct ril_request* item)
{
    struct ril_request* i;
    struct ril_request* j;
    unsigned int serial;

    assert(item);
    
    serial = item->mSerial;

    i = j = ril_request_table;

    while(( j != NULL) && (j->mSerial != serial))
    {
        i = j;
        j = j->mNext;
    }

    if((j != i) && (j != NULL))
    {
        /* *
         * we have found the item, which is pointed by j 
         * and i points to its precedant item.
         * */
        i->mNext = j->mNext;
        return TRUE;
    }
    else if((j == i) && (j->mSerial == serial))
    {
        /*item matched on the head of list.*/ 
        ril_request_table = j->mNext;
        return TRUE;
    }
    else
    {
       cmsLog_notice("item not found in list\n" );
       return FALSE;
    }
}


static struct internal_message* process_solicited(struct parcel *p)
{
    int serial, error = 0;                                                                                                             
    RILRequest *rr;                                                                                                                 
    struct internal_message *msg = NULL;
    Object* ret = NULL;                                                                                                             

    parcel_readInt32(p, &serial);                                                                                                          
    parcel_readInt32(p, &error);    

    rr = ril_request_list_find(serial);                                                                                     

    if (rr == NULL)
    {                                                                                                              
        cmsLog_error("Unexpected solicited response! sn:%d, error:%d\n ", 
                serial, error);                                                                            
        return NULL;                                                                                                               
    }

    cmsLog_debug("response to %s, serial=%d\n", 
                        request_to_string(rr->mRequest), rr->mSerial);
    
    if(rr->mResult != NULL)
    {
        cmsLog_debug("callback dest=%d, what=%d", rr->mResult->dest,
                                                  rr->mResult->what);
    }

    if (error == 0 || parcel_dataAvail(p) > 0) 
    {                                                                                         
        // either command succeeds or command fails but with data payload                                                          
        switch (rr->mRequest) 
        {                                                                                                
            case RIL_REQUEST_GET_SIM_STATUS: 
                ret =  response_icc_card_status(p); 
                break;                                                   

            case RIL_REQUEST_GET_IMSI: 
                ret =  response_string(p);
                break;         

            case RIL_REQUEST_DEVICE_IDENTITY: 
                ret = response_strings(p); 
                break;

            case RIL_REQUEST_SIM_IO: 
                ret = response_ICC_IO(p); 
                break;

            case RIL_REQUEST_SIGNAL_STRENGTH: 
                ret =  response_signalStrength(p); 
                break;

            case RIL_REQUEST_VOICE_REGISTRATION_STATE: 
                ret =  response_strings(p); 
                break;                                               

            case RIL_REQUEST_DATA_REGISTRATION_STATE: 
                ret =  response_strings(p);
                break;                                                

            case RIL_REQUEST_OPERATOR: 
                ret =  response_strings(p); 
                break;                                                               

            case RIL_REQUEST_RADIO_POWER: 
                ret =  response_void(p); 
                break;                                                               

            case RIL_REQUEST_SETUP_DATA_CALL: 
                ret =  response_setup_datacall(p); 
                break;                                                  

            case RIL_REQUEST_DEACTIVATE_DATA_CALL: 
                ret =  response_void(p); 
                break;                                                      

            case RIL_REQUEST_QUERY_FACILITY_LOCK: 
                ret =  response_ints(p); 
                break;

            case RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE: 
                ret =  response_ints(p); 
                break;

            case RIL_REQUEST_QUERY_AVAILABLE_NETWORKS : 
                ret =  response_operatorInfos(p); 
                break;

            case RIL_REQUEST_BASEBAND_VERSION: 
                ret =  response_string(p); 
                break; 

            case RIL_REQUEST_DATA_CALL_LIST: 
                ret =  response_datacall_list(p); 
                break;

            case RIL_REQUEST_RESET_RADIO: 
                ret =  response_void(p); 
                break;        

            case RIL_REQUEST_VOICE_RADIO_TECH: 
                ret = response_ints(p); 
                break;    

            //case RIL_REQUEST_GET_CELL_INFO_LIST: ret = responseCellInfoList(p); break;
            case RIL_REQUEST_SET_UNSOL_CELL_INFO_LIST_RATE: 
                ret = response_void(p); 
                break;

            case RIL_REQUEST_SET_INITIAL_ATTACH_APN: 
                ret = response_void(p); 
                break;

            case RIL_REQUEST_IMS_REGISTRATION_STATE: 
                ret = response_ints(p); 
                break;

            case RIL_REQUEST_DIAL: 
                ret = response_void(p); 
                break;

            case RIL_REQUEST_HANGUP: 
                ret = response_void(p); 
                break;

            case RIL_REQUEST_ANSWER: 
                ret = response_void(p); 
                break;

            case RIL_REQUEST_GET_CURRENT_CALLS:
                ret = response_call_list(p); 
                break;
                
            default: 
                break;
       } 
    }        

    if (error != 0) 
    {      
        /* What to do with errors? To be decided. */
        cmsLog_error("error = %d:%s, response to %s, serial=%d\n",
                            error, error_to_string(error), request_to_string(rr->mRequest), rr->mSerial);
        if(rr->mResult != NULL)
        {
            cmsLog_error("error related callback dest=%d, what=%d", 
                          rr->mResult->dest, rr->mResult->what);
        }
    }
    

    if (rr->mResult != NULL) 
    {                                           
        AsyncResult ar;
        
        msg = rr->mResult;
            
        /* Set rr's mResult to NULL so that the internal message would not
         * be freed by ril_request_free. It will be freed in event_loop, 
         * after the processing of internal state machine.
         * */
        rr->mResult = NULL;

        /* *
         * need to wrap up the result of ril and callback data
         * into AsyncResult data structure.
         * */
        ar.error = error;
        ar.result = ret;
        ar.userObj = msg->object;
        msg->object = NULL;
            
        Object *o = object_obtain();
        object_setData(o, &ar, sizeof(AsyncResult));
        internal_message_setObject(msg, o);
        o = NULL;
    }

    if (rr != NULL)
    {
        /* *
         * the rr's parcel should have been freed after the request was sent
         * out, but the internal_message data is still within rr.
         * we just invoke the ril_request_free method to make
         * sure we have a "clean" free. 
         * */
        ril_request_free(rr);
        rr = NULL;
    }
        
    return msg;                    
}

/*should release the Object data once it has been used */
static Object* response_ints(struct parcel *p)
{
    int i, numInts; 
    Object *response;    
    int *data;                                                          
                                                                                      
    parcel_readInt32(p, &numInts);                                                          
                                                                                        
    data = malloc(sizeof(int) * numInts);                                                   
                                                                                
    for(i=0; i<numInts; i++) 
    { 
        parcel_readInt32(p, &data[i]);                                          
    }                                                                       
              
    response = object_obtain();
    object_setData(response, data, numInts*sizeof(int));
    free(data);

    return response;                                                             
}                   

static Object* response_void(struct parcel *p)
{
    return NULL;
}

static Object* response_string(struct parcel *p)
{
    const char* s;
    Object *response;
    s = parcel_readCString(p);

    response = object_obtain();
    if(s != NULL)
    {
        object_setData(response, s, strlen(s)+1);
    } 
    return response;
    
}
static Object* response_operatorInfos(struct parcel *p)
{
    Object* s = response_strings(p);
    Object* ret = NULL;

    if(object_containsData(s))
    { 
        int numOfStrings = object_dataLength(s)/sizeof(char *);
        if(numOfStrings % 4 != 0)
        {
            int i;
            cmsLog_error(
             "invalid response. Got %d strings while expecting multiple of 4", 
             numOfStrings);
            /* as the result is invald, free the allocated memory.
             * First of all, free each string in the string array.
             */
            for(i=0; i<numOfStrings; i++)
            {
                char *p = (char *)object_getData(s) + i;
                if(p != NULL)
                {
                    free(p);
                    p = NULL;
                }
            }
            /* Free the string array itself. */
            object_freeData(s);
            free(s);
            s = NULL;
        }
        else
        {
           ret = s; 
        }

    }
    return ret;
}


static Object* response_strings(struct parcel *p)
{
    int numOfStrings;
    const char *s;
    char **t;
    Object *response = NULL;
    int i;

    parcel_readInt32(p, &numOfStrings);
    cmsLog_debug("parcel contains %d strings", numOfStrings);

    t = (char **)malloc(numOfStrings * sizeof(char *));
    
    if(t == NULL)
        return NULL;
    
    for(i=0; i<numOfStrings; i++)
    {
        s = parcel_readCString(p);
       
        if(s == NULL)
        {
            s = "";
        } 

        t[i] = (char *)malloc(strlen(s)+1);

        if(t[i] != NULL)
        {
            bzero(t[i], strlen(s)+1);
            strcpy(t[i], s);
        }
        else
        {
            cmsLog_error("failed to allocate memory for string, abort!");
        }

        cmsLog_debug("%dth: %s", i, t[i]);
    }
    
    response = object_obtain();
    object_setData(response, t, numOfStrings * sizeof(char *));
 
    /* 
     * The strings are now referenced by object data when we call 
     * object_setData above 
     */
    free(t);
    t = NULL;

    return response;
}

static Object* response_signalStrength(struct parcel *p)
{
    RIL_SignalStrength_v6 signalStrength;
    Object* response;

    parcel_readInt32(p, &signalStrength.GW_SignalStrength.signalStrength); 
    parcel_readInt32(p, &signalStrength.GW_SignalStrength.bitErrorRate); 
	
    parcel_readInt32(p, &signalStrength.CDMA_SignalStrength.dbm); 
    parcel_readInt32(p, &signalStrength.CDMA_SignalStrength.ecio); 

    parcel_readInt32(p, &signalStrength.EVDO_SignalStrength.dbm); 
    parcel_readInt32(p, &signalStrength.EVDO_SignalStrength.ecio); 
    parcel_readInt32(p, &signalStrength.EVDO_SignalStrength.signalNoiseRatio); 

    parcel_readInt32(p, &signalStrength.LTE_SignalStrength.signalStrength); 
    parcel_readInt32(p, &signalStrength.LTE_SignalStrength.rsrp); 
    parcel_readInt32(p, &signalStrength.LTE_SignalStrength.rsrq); 
    parcel_readInt32(p, &signalStrength.LTE_SignalStrength.rssnr); 
    parcel_readInt32(p, &signalStrength.LTE_SignalStrength.cqi); 
	
    response = object_obtain_with_data(&signalStrength, sizeof(signalStrength));

    return response;

}

static Object* response_setup_datacall(struct parcel *p)
{
    int ver, num;
    Object *response;
    RIL_Data_Call_Response_v6 *dataCall;
    
    parcel_readInt32(p, &ver);
    parcel_readInt32(p, &num);

    response = object_obtain(); 

    dataCall = get_datacall_response(p, ver);
    object_setData(response, (void *)dataCall, 
                             sizeof(RIL_Data_Call_Response_v6));
    free(dataCall);
    return response;
}

static Object* response_datacall_list(struct parcel *p)
{
    Object* response;
    RIL_Data_Call_Response_v6 *t;
    int ver, num;
    int i;

    parcel_readInt32(p, &ver);
    parcel_readInt32(p, &num);
    
    cmsLog_debug("ver=%d, num=%d", ver, num);

    response = object_obtain(); 
    
    for(i=0; i<num; i++)
    {
        t = get_datacall_response(p, ver);
        object_appendData(response, t, sizeof(RIL_Data_Call_Response_v6));
        free(t);
    }
    return response;
}

static Object* response_call_list(struct parcel *p)
{
    Object* response = NULL;
    int num = 0;
    int temp, i;
    int voiceSettings;
    const char *s = NULL;
    RIL_Call* ril_call_list = NULL;
    RIL_Call* dc = NULL;

    response = object_obtain();

    if(parcel_dataAvail(p) > 0)
    {
        parcel_readInt32(p, &num);
        cmsLog_debug("read %d RIL_Call", num);

        ril_call_list = malloc(num * sizeof(RIL_Call));
        assert(ril_call_list);
        memset(ril_call_list, 0x00, num*sizeof(RIL_Call));

        for(i=0; i<num; i++)
        {
            dc = ril_call_list+i;
            parcel_readInt32(p, &temp);
            dc->state = temp;

            parcel_readInt32(p, &temp);
            dc->index = temp;

            parcel_readInt32(p, &temp);
            dc->toa = temp;

            parcel_readInt32(p, &temp);
            dc->isMpty = temp;

            parcel_readInt32(p, &temp);
            dc->isMT = temp;

            parcel_readInt32(p, &temp);
            dc->als = temp;

            parcel_readInt32(p, &voiceSettings);
            dc->isVoice = (0 == voiceSettings)? FALSE:TRUE;

            parcel_readInt32(p, &temp);
            dc->isVoicePrivacy = (0 != temp);

            s = parcel_readCString(p);
            if(s == NULL)
            {
                s = "";
            }
            cmsLog_debug("read string %s, len = %d", s, strlen(s));
            dc->number = (char *)malloc(strlen(s)+1);
            assert(dc->number);
            strcpy(dc->number, s);
            
            parcel_readInt32(p, &temp);
            dc->numberPresentation = temp;

            s = parcel_readCString(p);
            if(s == NULL)
            {
                s = "";
            }
            cmsLog_debug("read string %s, len = %d", s, strlen(s));
            dc->name = (char *)malloc(strlen(s)+1);
            assert(dc->name);
            strcpy(dc->name, s);

            parcel_readInt32(p, &temp);
            dc->namePresentation = temp;

            parcel_readInt32(p, &temp);
            if(temp == 1)
            {
                int byte_size;

                dc->uusInfo = malloc(sizeof(RIL_UUS_Info));
                assert(dc->uusInfo);

                parcel_readInt32(p, &temp);
                dc->uusInfo->uusType = temp;

                parcel_readInt32(p, &temp);
                dc->uusInfo->uusDcs = temp;
   
                byte_size = parcel_dataAvail(p);
                dc->uusInfo->uusLength = byte_size;

                dc->uusInfo->uusData = malloc(byte_size);
                assert(dc->uusInfo->uusData);
                parcel_read(p, dc->uusInfo->uusData, byte_size);

            }
			
            parcel_readInt32(p, &temp);
            cmsLog_debug("(%d)isVideoCall = %d", i, temp);
            dc->isVideoCall = temp;
			
            parcel_readInt32(p, &temp);
            cmsLog_debug("(%d)callType = %d", i, temp);
            dc->callType = temp;
        }

        object_setData(response, ril_call_list, num*(sizeof(RIL_Call)));
        free(ril_call_list);
    }
    
    return response;

}

static Object* response_icc_card_status(struct parcel *p)
{
    RIL_CardStatus_v6 cardStatus;
    Object *response;
    int t, i;
    int numApplications;

    parcel_readInt32(p, &t);
    cardStatus.card_state = t;
    
    parcel_readInt32(p, &t);
    cardStatus.universal_pin_state = t;

    parcel_readInt32(p, &t);
    cardStatus.gsm_umts_subscription_app_index = t;
    
    parcel_readInt32(p, &t);
    cardStatus.cdma_subscription_app_index = t;

    parcel_readInt32(p, &t);
    cardStatus.ims_subscription_app_index = t;

    parcel_readInt32(p, &numApplications);
    cardStatus.num_applications = numApplications;

    cmsLog_debug("read RIL_CardStatus_v6:");
    cmsLog_debug("card_state=%d", cardStatus.card_state);
    cmsLog_debug("universal_pin_state=%d", cardStatus.universal_pin_state);
    cmsLog_debug("gsm_umts_subscription_app_index=%d", 
                                   cardStatus.gsm_umts_subscription_app_index);
    cmsLog_debug("cdma_subscription_app_index=%d", 
                                   cardStatus.cdma_subscription_app_index);
    cmsLog_debug("ims_subscription_app_index=%d", 
                                   cardStatus.ims_subscription_app_index);
    cmsLog_debug("num_applications=%d", cardStatus.num_applications);

    if(numApplications > RIL_CARD_MAX_APPS)
    {
        numApplications = RIL_CARD_MAX_APPS;
    }

    for(i=0; i<numApplications; i++)
    {
        const char* s;
        RIL_AppStatus *c = &cardStatus.applications[i];

        cmsLog_debug("application[%d]:", i);

        parcel_readInt32(p, &t);
        c->app_type = t;
        cmsLog_debug("app_type=%d:", c->app_type);

        parcel_readInt32(p, &t);
        c->app_state = t;
        cmsLog_debug("app_state=%d:", c->app_state);
        
        parcel_readInt32(p, &t);
        c->perso_substate = t;
        cmsLog_debug("perso_substate=%d:", c->perso_substate);

        s = parcel_readCString(p);
        if(s == NULL)
        {
            s = "";
        }
        cmsLog_debug("read string %s, len = %d", s, strlen(s));
        c->aid_ptr = (char *)malloc(strlen(s)+1);
        strcpy(c->aid_ptr, s);
        cmsLog_debug("aid_ptr=%s:", c->aid_ptr);
        
        s = parcel_readCString(p);
        if(s == NULL)
        {
            s="";
        }
        cmsLog_debug("read string %s, len = %d", s, strlen(s));
        c->app_label_ptr = (char *)malloc(strlen(s)+1);
        strcpy(c->app_label_ptr, s);
        cmsLog_debug("app_label_ptr=%s:", c->app_label_ptr);

        parcel_readInt32(p, &t);
        c->pin1 = t;
        cmsLog_debug("pin1=%d:", c->pin1);

        parcel_readInt32(p, &t); 
        c->pin2 = t;
        cmsLog_debug("pin2=%d:", c->pin2);

    }
    response = object_obtain();
    object_setData(response, (void *)&cardStatus, sizeof(RIL_CardStatus_v6));
    return response;
}


/* get_datacall_response would allocate a new RIL_Data_Call_Response using the 
 * information contained in parcel and ver.
 * */
static RIL_Data_Call_Response_v6* get_datacall_response(struct parcel *p, int ver)
{
    const char *t;
    /* data structure RIL_Data_Call_Response_v6 is defined in ril.h */
    RIL_Data_Call_Response_v6* dataCall = malloc(sizeof(
                                                  RIL_Data_Call_Response_v6));
    
    memset(dataCall, 0x00, sizeof(RIL_Data_Call_Response_v6));

    parcel_readInt32(p, &dataCall->status);
    parcel_readInt32(p, &dataCall->suggestedRetryTime);
    parcel_readInt32(p, &dataCall->cid);
    parcel_readInt32(p, &dataCall->active);
    
    t = parcel_readCString(p);
    if(t == NULL)
    {
        t = "";
    }
    dataCall->type = (char *)malloc(strlen(t)+1);
    assert(dataCall->type);
    strcpy(dataCall->type, t);

    t = parcel_readCString(p);
    if(t == NULL)
    {
        t = "";
    }
    dataCall->ifname = (char *)malloc(strlen(t)+1);
    assert(dataCall->ifname);
    strcpy(dataCall->ifname, t);

    t = parcel_readCString(p);
    if(t == NULL)
    {
        t = "";
    }
    dataCall->addresses = (char *)malloc(strlen(t)+1);
    assert(dataCall->addresses);
    strcpy(dataCall->addresses, t);

    t = parcel_readCString(p);
    if(t == NULL)
    {
        t = "";
    }
    dataCall->dnses = (char *)malloc(strlen(t)+1);
    assert(dataCall->dnses);
    strcpy(dataCall->dnses, t);

    t = parcel_readCString(p);
    if(t == NULL)
    {
        t = "";
    }
    dataCall->gateways = (char *)malloc(strlen(t)+1);
    assert(dataCall->gateways);
    strcpy(dataCall->gateways, t);
    return dataCall;
}

static RadioState get_radio_state_from_int(int stateInt)
{
    RadioState state = RADIO_OFF;
    
    cmsLog_debug("stateInt=%d", stateInt);
    switch(stateInt)
    {
        case 0: 
            state = RADIO_OFF; 
            break;
        case 1:
            state = RADIO_UNAVAILABLE;
            break;
        case 10:
            state = RADIO_ON;
            break;

        default:
            break;
    }
    return state;
}

static Object *response_ICC_IO(struct parcel *p)
{
    int sw1, sw2;
    int i;
    const char *s = NULL;
    IccIoResult result;

    struct object *o = NULL;

    parcel_readInt32(p, &sw1);
    parcel_readInt32(p, &sw2);
    
    s = parcel_readCString(p);
    cmsLog_debug("sw1=%d, sw2=%d, s=%s", sw1, sw2, s);

    result.sw1 = sw1;
    result.sw2 = sw2;
    result.payload = icc_util_hexStringToBytes(s, &(result.length));
    for(i=0; i<result.length; i++)
    {
        cmsLog_debug("[%d]:%d", i, result.payload[i]);
    }

    o = object_obtain();
    object_setData(o, &result, sizeof(IccIoResult));
    return o;
}

//**                                                   
 /**                                                                         
  * Store new RadioState and send notification based on the changes          
  *                                                                          
  * This function is called only by RIL.java when receiving unsolicited         
  * RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED                                   
  *                                                                          
  * RadioState has 3 values : RADIO_OFF, RADIO_UNAVAILABLE, RADIO_ON.        
  *                                                                          
  * @param newState new RadioState decoded from RIL_UNSOL_RADIO_STATE_CHANGED
 */                                                                         
static struct internal_message* switch_to_radio_state(RadioState newState)
{                      
     RadioState oldState;                                                    
     struct internal_message* msg = NULL;

     oldState = mState;                                                  
     mState = newState;                                                  

     if(oldState == mState)
     {
         // no state transition;
         return NULL;
     }
                                                                                
     cmsLog_notice("radio state changed!");
     /* There is a state transition of radio */
     msg = internal_message_obtain(EVENT_RADIO_STATE_CHANGED, ALL);
    
     return msg;
}                      


RadioState ril_access_get_radio_state()
{
    return mState;
}

static struct internal_message* notify_voice_network_state_changed()
{
    struct internal_message *msg;
    msg = internal_message_obtain(EVENT_NETWORK_STATE_CHANGED, ALL);
    return msg;
}

static struct internal_message* notify_icc_status_changed()
{
    struct internal_message *msg;
    msg = internal_message_obtain(EVENT_ICC_STATUS_CHANGED, ALL);
    return msg;
}

static struct internal_message* notify_restricted_state_changed(struct object *ret)
{
    struct internal_message *msg;
    AsyncResult ar;
    Object *o;

    ar.error = 0;
    ar.result = ret;
    ar.userObj = NULL;

    msg = internal_message_obtain(EVENT_RESTRICTED_STATE_CHANGED, ALL);

    o = object_obtain();
    object_setData(o, &ar, sizeof(AsyncResult));
    internal_message_setObject(msg, o);
    o = NULL;
 
    return msg;
}

static struct internal_message* notify_network_state_changed(struct object *ret)
{
    struct internal_message *msg;
    AsyncResult ar;
    Object *o;

    ar.error = 0;
    ar.result = ret;
    ar.userObj = NULL;
    
    msg = internal_message_obtain(EVENT_DATA_STATE_CHANGED, ALL);
    
    o = object_obtain();
    object_setData(o, &ar, sizeof(AsyncResult));
    internal_message_setObject(msg, o);
    o = NULL;

    return msg;
}

static struct internal_message* notify_call_state_changed()
{
    struct internal_message *msg;
    msg = internal_message_obtain(EVENT_CALL_STATE_CHANGED, CALL_TRACKER);
    return msg;
}

static struct internal_message* notify_pll_enable()
{
    struct internal_message *msg;
    msg = internal_message_obtain(EVENT_PLL_ENABLE, CALL_TRACKER);
    return msg;
}

static struct internal_message* notify_pll_disable()
{
    struct internal_message *msg;
    msg = internal_message_obtain(EVENT_PLL_DISABLE, CALL_TRACKER);
    return msg;
}

static struct internal_message* notify_ringback_tone(struct object *ret)
{
    struct internal_message *msg;
    AsyncResult ar;
    Object *o;

    ar.error = 0;
    ar.result = ret;
    ar.userObj = NULL;
    
    msg = internal_message_obtain(EVENT_CALL_RINGBACK_TONE, CALL_TRACKER);
    
    o = object_obtain();
    object_setData(o, &ar, sizeof(AsyncResult));
    internal_message_setObject(msg, o);
    o = NULL;

    return msg;

}

static struct internal_message* notify_new_sms(struct object *ret)
{
    struct internal_message *msg;
    AsyncResult ar;
    Object *o;

    ar.error = 0;
    ar.result = ret;
    ar.userObj = NULL;
    
    msg = internal_message_obtain(EVENT_NEW_SMS, SMS_TRACKER);
    
    o = object_obtain();
    object_setData(o, &ar, sizeof(AsyncResult));
    internal_message_setObject(msg, o);
    o = NULL;

    return msg;

}

static struct internal_message* notify_new_sms_on_sim(struct object *ret)
{
    struct internal_message *msg;
    AsyncResult ar;
    Object *o;

    ar.error = 0;
    ar.result = ret;
    ar.userObj = NULL;
    
    msg = internal_message_obtain(EVENT_NEW_SMS_ON_SIM, UICC_CONTROLLER);
    
    o = object_obtain();
    object_setData(o, &ar, sizeof(AsyncResult));
    internal_message_setObject(msg, o);
    o = NULL;

    return msg;

}

static struct internal_message* notify_signal_strength(struct object *ret)
{
    struct internal_message *msg;
    AsyncResult ar;
    Object *o;

    ar.error = 0;
    ar.result = ret;
    ar.userObj = NULL;
    
    msg = internal_message_obtain(EVENT_SIGNAL_STRENGTH_UPDATE, ALL);
    
    o = object_obtain();
    object_setData(o, &ar, sizeof(AsyncResult));
    internal_message_setObject(msg, o);
    o = NULL;

    return msg;

}


static const char *request_to_string(int request) 
{
/*
 cat libs/telephony/ril_commands.h \
 | egrep "^ *{RIL_" \
 | sed -re 's/\{RIL_([^,]+),[^,]+,([^}]+).+/case RIL_\1: return "\1";/'


 cat libs/telephony/ril_unsol_commands.h \
 | egrep "^ *{RIL_" \
 | sed -re 's/\{RIL_([^,]+),([^}]+).+/case RIL_\1: return "\1";/'

*/
    switch(request) {
        case RIL_REQUEST_GET_SIM_STATUS: return "GET_SIM_STATUS";
        case RIL_REQUEST_ENTER_SIM_PIN: return "ENTER_SIM_PIN";
        case RIL_REQUEST_ENTER_SIM_PUK: return "ENTER_SIM_PUK";
        case RIL_REQUEST_ENTER_SIM_PIN2: return "ENTER_SIM_PIN2";
        case RIL_REQUEST_ENTER_SIM_PUK2: return "ENTER_SIM_PUK2";
        case RIL_REQUEST_CHANGE_SIM_PIN: return "CHANGE_SIM_PIN";
        case RIL_REQUEST_CHANGE_SIM_PIN2: return "CHANGE_SIM_PIN2";
        case RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION: return "ENTER_NETWORK_DEPERSONALIZATION";
        case RIL_REQUEST_GET_CURRENT_CALLS: return "GET_CURRENT_CALLS";
        case RIL_REQUEST_DIAL: return "DIAL";
        case RIL_REQUEST_GET_IMSI: return "GET_IMSI";
        case RIL_REQUEST_HANGUP: return "HANGUP";
        case RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND: return "HANGUP_WAITING_OR_BACKGROUND";
        case RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND: return "HANGUP_FOREGROUND_RESUME_BACKGROUND";
        case RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE: return "SWITCH_WAITING_OR_HOLDING_AND_ACTIVE";
        case RIL_REQUEST_CONFERENCE: return "CONFERENCE";
        case RIL_REQUEST_UDUB: return "UDUB";
        case RIL_REQUEST_LAST_CALL_FAIL_CAUSE: return "LAST_CALL_FAIL_CAUSE";
        case RIL_REQUEST_SIGNAL_STRENGTH: return "SIGNAL_STRENGTH";
        case RIL_REQUEST_VOICE_REGISTRATION_STATE: return "VOICE_REGISTRATION_STATE";
        case RIL_REQUEST_DATA_REGISTRATION_STATE: return "DATA_REGISTRATION_STATE";
        case RIL_REQUEST_OPERATOR: return "OPERATOR";
        case RIL_REQUEST_RADIO_POWER: return "RADIO_POWER";
        case RIL_REQUEST_DTMF: return "DTMF";
        case RIL_REQUEST_SEND_SMS: return "SEND_SMS";
        case RIL_REQUEST_SEND_SMS_EXPECT_MORE: return "SEND_SMS_EXPECT_MORE";
        case RIL_REQUEST_SETUP_DATA_CALL: return "SETUP_DATA_CALL";
        case RIL_REQUEST_SIM_IO: return "SIM_IO";
        case RIL_REQUEST_SEND_USSD: return "SEND_USSD";
        case RIL_REQUEST_CANCEL_USSD: return "CANCEL_USSD";
        case RIL_REQUEST_GET_CLIR: return "GET_CLIR";
        case RIL_REQUEST_SET_CLIR: return "SET_CLIR";
        case RIL_REQUEST_QUERY_CALL_FORWARD_STATUS: return "QUERY_CALL_FORWARD_STATUS";
        case RIL_REQUEST_SET_CALL_FORWARD: return "SET_CALL_FORWARD";
        case RIL_REQUEST_QUERY_CALL_WAITING: return "QUERY_CALL_WAITING";
        case RIL_REQUEST_SET_CALL_WAITING: return "SET_CALL_WAITING";
        case RIL_REQUEST_SMS_ACKNOWLEDGE: return "SMS_ACKNOWLEDGE";
        case RIL_REQUEST_GET_IMEI: return "GET_IMEI";
        case RIL_REQUEST_GET_IMEISV: return "GET_IMEISV";
        case RIL_REQUEST_ANSWER: return "ANSWER";
        case RIL_REQUEST_DEACTIVATE_DATA_CALL: return "DEACTIVATE_DATA_CALL";
        case RIL_REQUEST_QUERY_FACILITY_LOCK: return "QUERY_FACILITY_LOCK";
        case RIL_REQUEST_SET_FACILITY_LOCK: return "SET_FACILITY_LOCK";
        case RIL_REQUEST_CHANGE_BARRING_PASSWORD: return "CHANGE_BARRING_PASSWORD";
        case RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE: return "QUERY_NETWORK_SELECTION_MODE";
        case RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC: return "SET_NETWORK_SELECTION_AUTOMATIC";
        case RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL: return "SET_NETWORK_SELECTION_MANUAL";
        case RIL_REQUEST_QUERY_AVAILABLE_NETWORKS : return "QUERY_AVAILABLE_NETWORKS ";
        case RIL_REQUEST_DTMF_START: return "DTMF_START";
        case RIL_REQUEST_DTMF_STOP: return "DTMF_STOP";
        case RIL_REQUEST_BASEBAND_VERSION: return "BASEBAND_VERSION";
        case RIL_REQUEST_SEPARATE_CONNECTION: return "SEPARATE_CONNECTION";
        case RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE: return "SET_PREFERRED_NETWORK_TYPE";
        case RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE: return "GET_PREFERRED_NETWORK_TYPE";
        case RIL_REQUEST_GET_NEIGHBORING_CELL_IDS: return "GET_NEIGHBORING_CELL_IDS";
        case RIL_REQUEST_SET_MUTE: return "SET_MUTE";
        case RIL_REQUEST_GET_MUTE: return "GET_MUTE";
        case RIL_REQUEST_QUERY_CLIP: return "QUERY_CLIP";
        case RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE: return "LAST_DATA_CALL_FAIL_CAUSE";
        case RIL_REQUEST_DATA_CALL_LIST: return "DATA_CALL_LIST";
        case RIL_REQUEST_RESET_RADIO: return "RESET_RADIO";
        case RIL_REQUEST_OEM_HOOK_RAW: return "OEM_HOOK_RAW";
        case RIL_REQUEST_OEM_HOOK_STRINGS: return "OEM_HOOK_STRINGS";
        case RIL_REQUEST_SET_BAND_MODE: return "SET_BAND_MODE";
        case RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE: return "QUERY_AVAILABLE_BAND_MODE";
        case RIL_REQUEST_STK_GET_PROFILE: return "STK_GET_PROFILE";
        case RIL_REQUEST_STK_SET_PROFILE: return "STK_SET_PROFILE";
        case RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND: return "STK_SEND_ENVELOPE_COMMAND";
        case RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE: return "STK_SEND_TERMINAL_RESPONSE";
        case RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM: return "STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM";
        case RIL_REQUEST_SCREEN_STATE: return "SCREEN_STATE";
        case RIL_REQUEST_EXPLICIT_CALL_TRANSFER: return "EXPLICIT_CALL_TRANSFER";
        case RIL_REQUEST_SET_LOCATION_UPDATES: return "SET_LOCATION_UPDATES";
        case RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE:return"CDMA_SET_SUBSCRIPTION_SOURCE";
        case RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE:return"CDMA_SET_ROAMING_PREFERENCE";
        case RIL_REQUEST_CDMA_QUERY_ROAMING_PREFERENCE:return"CDMA_QUERY_ROAMING_PREFERENCE";
        case RIL_REQUEST_SET_TTY_MODE:return"SET_TTY_MODE";
        case RIL_REQUEST_QUERY_TTY_MODE:return"QUERY_TTY_MODE";
        case RIL_REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE:return"CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE";
        case RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE:return"CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE";
        case RIL_REQUEST_CDMA_FLASH:return"CDMA_FLASH";
        case RIL_REQUEST_CDMA_BURST_DTMF:return"CDMA_BURST_DTMF";
        case RIL_REQUEST_CDMA_SEND_SMS:return"CDMA_SEND_SMS";
        case RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE:return"CDMA_SMS_ACKNOWLEDGE";
        case RIL_REQUEST_GSM_GET_BROADCAST_SMS_CONFIG:return"GSM_GET_BROADCAST_SMS_CONFIG";
        case RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG:return"GSM_SET_BROADCAST_SMS_CONFIG";
        case RIL_REQUEST_CDMA_GET_BROADCAST_SMS_CONFIG:return "CDMA_GET_BROADCAST_SMS_CONFIG";
        case RIL_REQUEST_CDMA_SET_BROADCAST_SMS_CONFIG:return "CDMA_SET_BROADCAST_SMS_CONFIG";
        case RIL_REQUEST_CDMA_SMS_BROADCAST_ACTIVATION:return "CDMA_SMS_BROADCAST_ACTIVATION";
        case RIL_REQUEST_CDMA_VALIDATE_AND_WRITE_AKEY: return"CDMA_VALIDATE_AND_WRITE_AKEY";
        case RIL_REQUEST_CDMA_SUBSCRIPTION: return"CDMA_SUBSCRIPTION";
        case RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM: return "CDMA_WRITE_SMS_TO_RUIM";
        case RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM: return "CDMA_DELETE_SMS_ON_RUIM";
        case RIL_REQUEST_DEVICE_IDENTITY: return "DEVICE_IDENTITY";
        case RIL_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE: return "EXIT_EMERGENCY_CALLBACK_MODE";
        case RIL_REQUEST_GET_SMSC_ADDRESS: return "GET_SMSC_ADDRESS";
        case RIL_REQUEST_SET_SMSC_ADDRESS: return "SET_SMSC_ADDRESS";
        case RIL_REQUEST_REPORT_SMS_MEMORY_STATUS: return "REPORT_SMS_MEMORY_STATUS";
        case RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING: return "REPORT_STK_SERVICE_IS_RUNNING";
        case RIL_REQUEST_CDMA_GET_SUBSCRIPTION_SOURCE: return "CDMA_GET_SUBSCRIPTION_SOURCE";
        case RIL_REQUEST_ISIM_AUTHENTICATION: return "ISIM_AUTHENTICATION";
        case RIL_REQUEST_ACKNOWLEDGE_INCOMING_GSM_SMS_WITH_PDU: return "RIL_REQUEST_ACKNOWLEDGE_INCOMING_GSM_SMS_WITH_PDU";
        case RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS: return "RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS";
        case RIL_REQUEST_VOICE_RADIO_TECH: return "VOICE_RADIO_TECH";
        case RIL_REQUEST_GET_CELL_INFO_LIST: return"GET_CELL_INFO_LIST";
        case RIL_REQUEST_SET_UNSOL_CELL_INFO_LIST_RATE: return"SET_UNSOL_CELL_INFO_LIST_RATE";
        case RIL_REQUEST_SET_INITIAL_ATTACH_APN: return "RIL_REQUEST_SET_INITIAL_ATTACH_APN";
        case RIL_REQUEST_IMS_SETTING_MODIFY: return "RIL_REQUEST_IMS_SETTING_MODIFY";
        case RIL_REQUEST_IMS_REGISTRATION_STATE: return "IMS_REGISTRATION_STATE";
        case RIL_REQUEST_IMS_SEND_SMS: return "IMS_SEND_SMS";
        case RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC: return "SIM_TRANSMIT_APDU_BASIC";
        case RIL_REQUEST_SIM_OPEN_CHANNEL: return "SIM_OPEN_CHANNEL";
        case RIL_REQUEST_SIM_CLOSE_CHANNEL: return "SIM_CLOSE_CHANNEL";
        case RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL: return "SIM_TRANSMIT_APDU_CHANNEL";
        case RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED: return "UNSOL_RESPONSE_RADIO_STATE_CHANGED";
        case RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED: return "UNSOL_RESPONSE_CALL_STATE_CHANGED";
        case RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED: return "UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED";
        case RIL_UNSOL_RESPONSE_NEW_SMS: return "UNSOL_RESPONSE_NEW_SMS";
        case RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT: return "UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT";
        case RIL_UNSOL_RESPONSE_NEW_SMS_ON_SIM: return "UNSOL_RESPONSE_NEW_SMS_ON_SIM";
        case RIL_UNSOL_ON_USSD: return "UNSOL_ON_USSD";
        case RIL_UNSOL_ON_USSD_REQUEST: return "UNSOL_ON_USSD_REQUEST(obsolete)";
        case RIL_UNSOL_NITZ_TIME_RECEIVED: return "UNSOL_NITZ_TIME_RECEIVED";
        case RIL_UNSOL_SIGNAL_STRENGTH: return "UNSOL_SIGNAL_STRENGTH";
        case RIL_UNSOL_STK_SESSION_END: return "UNSOL_STK_SESSION_END";
        case RIL_UNSOL_STK_PROACTIVE_COMMAND: return "UNSOL_STK_PROACTIVE_COMMAND";
        case RIL_UNSOL_STK_EVENT_NOTIFY: return "UNSOL_STK_EVENT_NOTIFY";
        case RIL_UNSOL_STK_CALL_SETUP: return "UNSOL_STK_CALL_SETUP";
        case RIL_UNSOL_SIM_SMS_STORAGE_FULL: return "UNSOL_SIM_SMS_STORAGE_FUL";
        case RIL_UNSOL_SIM_REFRESH: return "UNSOL_SIM_REFRESH";
        case RIL_UNSOL_DATA_CALL_LIST_CHANGED: return "UNSOL_DATA_CALL_LIST_CHANGED";
        case RIL_UNSOL_CALL_RING: return "UNSOL_CALL_RING";
        case RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED: return "UNSOL_RESPONSE_SIM_STATUS_CHANGED";
        case RIL_UNSOL_RESPONSE_CDMA_NEW_SMS: return "UNSOL_NEW_CDMA_SMS";
        case RIL_UNSOL_RESPONSE_NEW_BROADCAST_SMS: return "UNSOL_NEW_BROADCAST_SMS";
        case RIL_UNSOL_CDMA_RUIM_SMS_STORAGE_FULL: return "UNSOL_CDMA_RUIM_SMS_STORAGE_FULL";
        case RIL_UNSOL_RESTRICTED_STATE_CHANGED: return "UNSOL_RESTRICTED_STATE_CHANGED";
        case RIL_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE: return "UNSOL_ENTER_EMERGENCY_CALLBACK_MODE";
        case RIL_UNSOL_CDMA_CALL_WAITING: return "UNSOL_CDMA_CALL_WAITING";
        case RIL_UNSOL_CDMA_OTA_PROVISION_STATUS: return "UNSOL_CDMA_OTA_PROVISION_STATUS";
        case RIL_UNSOL_CDMA_INFO_REC: return "UNSOL_CDMA_INFO_REC";
        case RIL_UNSOL_OEM_HOOK_RAW: return "UNSOL_OEM_HOOK_RAW";
        case RIL_UNSOL_RINGBACK_TONE: return "UNSOL_RINGBACK_TONE";
        case RIL_UNSOL_RESEND_INCALL_MUTE: return "UNSOL_RESEND_INCALL_MUTE";
        case RIL_UNSOL_CDMA_SUBSCRIPTION_SOURCE_CHANGED: return "UNSOL_CDMA_SUBSCRIPTION_SOURCE_CHANGED";
        case RIL_UNSOL_CDMA_PRL_CHANGED: return "UNSOL_CDMA_PRL_CHANGED";
        case RIL_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE: return "UNSOL_EXIT_EMERGENCY_CALLBACK_MODE";
        case RIL_UNSOL_RIL_CONNECTED: return "UNSOL_RIL_CONNECTED";
        case RIL_UNSOL_VOICE_RADIO_TECH_CHANGED: return "UNSOL_VOICE_RADIO_TECH_CHANGED";
        case RIL_UNSOL_CELL_INFO_LIST: return "UNSOL_CELL_INFO_LIST";
        case RIL_UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED: return "RESPONSE_IMS_NETWORK_STATE_CHANGED";
        default: return "<unknown request>";
    }
}

