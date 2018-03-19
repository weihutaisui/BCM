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

#include "cms.h"
#include "cms_util.h"
#include "cms_log.h"
#include "ril_access.h"
#include "icc_file_handler.h"
#include "cellular_msg.h"
#include "cellular_internal.h"

#include <assert.h>
#include <stdlib.h>


static void on_get_record_size_done(AsyncResult *ar);
static void on_read_record_done(AsyncResult *ar);
static const char* getEFPath(int efid);
static int hexCharToInt(char c);

#define MF_SIM "3F00"
#define DF_TELECOM "7F10"
#define DF_GSM "7F20"
#define DF_ADF "7FFF"

static const char SIM_TELECOM[] = MF_SIM DF_TELECOM;
static const char SIM_GSM[] = MF_SIM DF_GSM;
static const char SIM_ADF[] = MF_SIM DF_ADF;

void icc_file_handler_init()
{
}


void icc_file_handler_process(struct internal_message *msg)
{
    AsyncResult *ar;
    Object* o = internal_message_getObject(msg);


    switch(msg->what)
    {
        case EVENT_GET_RECORD_SIZE_DONE:
            ar = (AsyncResult *)object_getData(o);
            on_get_record_size_done(ar); 
            break;

        case EVENT_READ_RECORD_DONE:
            ar = (AsyncResult *)object_getData(o);
            on_read_record_done(ar);
            break;

        default:
            break;
    }
}

void icc_file_handler_loadEFLinearFixed(int fileid, int recordNum, 
                                 const char *aid, struct internal_message* msg)
{
    LoadLinearFixedContext lc;
    struct internal_message *response;
    Object *o;

    lc.mEfId = fileid;
    lc.mRecordNum = recordNum;
    lc.mCountRecords = 0;
    lc.mLoadAll = FALSE;
    lc.onLoaded = msg;
    
    o = object_obtain();
    object_setData(o, &lc, sizeof(LoadLinearFixedContext));

    response = internal_message_obtain(EVENT_GET_RECORD_SIZE_DONE,
                                       ICC_FILE_HANDLER);
    internal_message_setObject(response, o);

    ril_access_icc_io_for_app(COMMAND_GET_RESPONSE, fileid, getEFPath(fileid),
                              0, 0, GET_RESPONSE_EF_SIZE_BYTES, NULL, NULL, 
                              aid, response);  
}

static void on_get_record_size_done(AsyncResult *ar)
{ 
    LoadLinearFixedContext *lc;
    IccIoResult *result;
    struct internal_message* msg;
    UINT8 *data;
    int size;

    assert(ar);

    lc = (LoadLinearFixedContext *)object_getData(ar->userObj);

    if(ar->error != 0)
    {
        cmsLog_notice("error code is %d", ar->error);
        goto error_exit;
    }
    else
    { 
        assert(ar->result);
        result = (IccIoResult *)object_getData(ar->result);
        data = result->payload;

        if(TYPE_EF != data[RESPONSE_DATA_FILE_TYPE])
        {
            cmsLog_error("Icc file type mismatch!");
            goto error_exit;
        }

        if(EF_TYPE_LINEAR_FIXED != data[RESPONSE_DATA_STRUCTURE])
        {
            cmsLog_error("Icc file type mismatch!");
            goto error_exit;
        }

        lc->mRecordSize = data[RESPONSE_DATA_RECORD_LENGTH] & 0xFF;

        size = ((data[RESPONSE_DATA_FILE_SIZE_1] & 0xff) << 8) +
               (data[RESPONSE_DATA_FILE_SIZE_2] & 0xff); 

        lc->mCountRecords = size / lc->mRecordSize;

        if(lc->mLoadAll)
        {
            //lc->results = malloc(...);
        }

        /*
         *  put lc into the next round of RIL request in order to track context.
         */   
        msg = internal_message_obtain(EVENT_READ_RECORD_DONE, ICC_FILE_HANDLER);
        internal_message_setObject(msg, ar->userObj);
        /* Set ar->userObj to be NULL so that it won't get freed after current round
         * of event handling.
         * */
        ar->userObj = NULL;

        ril_access_icc_io_for_app(COMMAND_READ_RECORD, lc->mEfId, 
                 getEFPath(lc->mEfId), lc->mRecordNum, READ_RECORD_MODE_ABSOLUTE, 
                 lc->mRecordSize, NULL, NULL, NULL, msg); 
        return;
    }

error_exit:
    /* Since error happens, we free lc's callback message here and 
     * won't proceed to read.
     */
    if(lc->onLoaded != NULL)
    {
        /* notify the requester about the final result of read record. */
        msg = lc->onLoaded;
        internal_message_freeObject(msg);
        free(msg);
        lc->onLoaded = NULL;
    }
    /* lc will be freed after current round of event handling. */
}

static void on_read_record_done(AsyncResult *ar)
{
    LoadLinearFixedContext *lc;
    IccIoResult *icc_result;
    Object *userObj;
    Object *result;
    struct internal_message *msg;

    assert(ar);
    userObj = ar->userObj;
    result = ar->result;

    if(userObj != NULL)
    {
        lc = (LoadLinearFixedContext *)object_getData(userObj);
        assert(lc);
        
        if(ar->error == 0)
        {
            assert(result);
            icc_result = (IccIoResult *)object_getData(result);
            assert(icc_result);

            /* Do we really need efid and index in IccIoResult? TBD. */
            icc_result->efid = lc->mEfId;
            icc_result->index = lc->mRecordNum;
        }

        if(lc->mLoadAll == FALSE)
        {
            if(lc->onLoaded != NULL)
            {
                AsyncResult callback_ar;
                Object *o;

                /* notify the requester about the final result of read record. */
                msg = lc->onLoaded;
                
                callback_ar.userObj = internal_message_getObject(msg);
                callback_ar.result = ar->result;
                ar->result = NULL;
                
                callback_ar.error = ar->error;
                
                o = object_obtain();
                object_setData(o, &callback_ar, sizeof(AsyncResult));

                internal_message_setObject(msg, o);
                cellular_internal_process(msg);

                /* callback msg consumed by the requester, free it now. */
                if(callback_ar.userObj != NULL)
                {
                    object_freeData(callback_ar.userObj);
                    free(callback_ar.userObj);
                    callback_ar.userObj = NULL;
                }

                if(callback_ar.result != NULL)
                {
                    object_freeData(callback_ar.result);
                    free(callback_ar.result);
                    callback_ar.result = NULL;
                }

                internal_message_freeObject(msg);
                free(msg);
                lc->onLoaded = NULL;
             }
            /* lc will be freed after current round of event handling. */
        }
    }
}

/* get EF path for usim card. */
static const char* getEFPath(int efid)
{
    switch(efid)
    {
        case EF_SMS: 
        case EF_MSISDN: 
           return SIM_ADF; 
/*
        case EF_EXT6:
        case EF_MWIS:
        case EF_MBI:
        case EF_SPN:
        case EF_AD:
        case EF_MBDN:
        case EF_PNN:
        case EF_SPDI:
        case EF_SST:
        case EF_CFIS:
        case EF_GID1:
            return SIM_GSM;

        case EF_MAILBOX_CPHS:
        case EF_VOICE_MAIL_INDICATOR_CPHS:
        case EF_CFF_CPHS:
        case EF_SPN_CPHS:
        case EF_SPN_SHORT_CPHS:
        case EF_INFO_CPHS:
        case EF_CSP_CPHS:
            return SIM_GSM;
*/
        default:
           break;        
   }
   /* get common path for file id. ?? */
   return NULL; 
}

UINT8* icc_util_hexStringToBytes(const char *s, int* pdu_len)
{
    UINT8 *pdu;
    int sz;
    int i;

    if(s == NULL)
    {
        *pdu_len = 0;
        return NULL;
    }
    sz = cmsUtl_strlen(s);

    assert(sz > 0);
    pdu = malloc(sz/2);
    assert(pdu);
    *pdu_len = sz/2;

    for(i=0; i<sz; i+=2)
    {
        pdu[i/2] = (UINT8)((hexCharToInt(s[i]) << 4) | hexCharToInt(s[i+1]));
    }
    return pdu;
}

static int hexCharToInt(char c)
{
    if(c >= '0' && c <= '9') return (c - '0');
    if(c >= 'A' && c <= 'F') return (c - 'A' + 10);
    if(c >= 'a' && c <= 'f') return (c - 'a' + 10);

    cmsLog_error("unhandled char!");
    return -1;
}

