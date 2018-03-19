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
#include "adn_record_loader.h"
#include "cellular_msg.h"
#include "cellular_internal.h"

#include <assert.h>
#include <stdlib.h>

//Constants

/* In an ADN record, everything but the alpha identifier is in a footer 
 * that's 14 bytes.
 */
static const int FOOTER_SIZE_BYTES = 14;

/* Maximum size of the un-extended number field. */
static const int MAX_NUMBER_SIZE_BYTES = 11;

/* Invalid byte code */
#define EF_INVALID_BYTE 0xFF

static const int EXT_RECORD_LENGTH_BYTES = 13;
static const int EXT_RECORD_TYPE_ADDITIONAL_DATA = 2;
static const int EXT_RECORD_TYPE_MASK = 3;
static const int MAX_EXT_CALLED_PARTY_LENGTH = 0xa;

/* ADN offset */
static const int ADN_BCD_NUMBER_LENGTH = 0;
static const int ADN_TON_AND_NPI = 1;
static const int ADN_DIALING_NUMBER_START = 2;
static const int ADN_DIALING_NUMBER_END = 11;
static const int ADN_CAPABILITY_ID = 12;
static const int ADN_EXTENSION_ID = 13;

static const int TOA_International = 0x91;
static const int TOA_Unknown = 0x81;

static const char PAUSE = ',';
static const char WAIT = ';';
static const char WILD = 'N';

static void called_party_bcdToString(UINT8 *bytes, int offset, int length, 
                                     char *buf, int buf_len);

static void called_party_bcdFragmentToString(UINT8 *bytes, int offset, 
                                     int length, char *ret, int ret_len);

static void on_adn_load_done(AsyncResult *ar);
static void on_ext_record_load_done(AsyncResult *ar);
static UBOOL8 adn_record_hasExtendedRecord(struct adn_record *adn);
static void adn_record_appendExtRecord(IccIoResult *data, AdnRecord *adn);
static void adn_record_loader_finished(AdnRecordLoaderContext *context);
static void adn_record_parseRecord(IccIoResult *data, struct adn_record* record);
static char bcdToChar(UINT8 b);

void adn_record_loader_process(struct internal_message *msg)
{
    AsyncResult *ar;
    Object* o = internal_message_getObject(msg);

    switch(msg->what)
    {
        case EVENT_ADN_LOAD_DONE:
            ar = (AsyncResult *)object_getData(o);
            on_adn_load_done(ar);
            break;

        case EVENT_EXT_RECORD_LOAD_DONE:
            ar = (AsyncResult *)object_getData(o);
            on_ext_record_load_done(ar); 
            break;

        default:
            return;
    }
    
}

void adn_record_LoadFromEF(int ef, int extensionEF, int recordNumber, 
                           const char *aid, struct internal_message* msg)
{
    struct adn_record_loader_context adn_loader_context;
    struct internal_message* adn_loader_msg;
    Object *o;

    cmsLog_debug("load 0x%x, recordNumber %d", ef, recordNumber);

    adn_loader_context.mEf = ef;
    adn_loader_context.mExtensionEf = extensionEF;
    adn_loader_context.mRecordNumber = recordNumber;
    adn_loader_context.mPendingExtLoads = 0;
    memset(&(adn_loader_context.adn), 0x00, sizeof(AdnRecord)); 
    adn_loader_context.callback_msg = msg;

    o = object_obtain();
    object_setData(o, &adn_loader_context, sizeof(AdnRecordLoaderContext));
    adn_loader_msg = internal_message_obtain(EVENT_ADN_LOAD_DONE, 
                                             ADN_RECORD_LOADER);

    internal_message_setObject(adn_loader_msg, o);

    icc_file_handler_loadEFLinearFixed(ef, recordNumber, aid, adn_loader_msg);

}

static void on_adn_load_done(AsyncResult *ar)
{
    IccIoResult *data;
    Object *result, *userObj;
    struct adn_record_loader_context *context;
    struct internal_message *msg;
    
    result = ar->result;
    userObj = ar->userObj;
    assert(result);
    assert(userObj);

    context = (AdnRecordLoaderContext *)object_getData(userObj);
    data = (IccIoResult *)object_getData(result);
        
    if(ar->error == 0)
    {

        adn_record_parseRecord(data, &(context->adn));
        cmsLog_debug("mNumber=%s", context->adn.mNumber);

        if(adn_record_hasExtendedRecord(&(context->adn)))
        {
            cmsLog_debug("adn record has extended record.");
            context->mPendingExtLoads = 1;
            
            msg = internal_message_obtain(EVENT_EXT_RECORD_LOAD_DONE,
                                          ADN_RECORD_LOADER);
            /* adn_record_loader_context is passed on to the next round of
             * icc file operation.
             */
            internal_message_setObject(msg, userObj);
            ar->userObj = NULL;

            icc_file_handler_loadEFLinearFixed(context->mExtensionEf, 
                                               context->adn.mExtRecord, NULL,
                                               msg);
        }
        else
        {
            adn_record_loader_finished(context);
        }
    }
    else
    {
        /* error in the process of loading adn, free the adn_loader_context's 
         * callback_msg.
         */
        cmsLog_error("");
        internal_message_freeObject(context->callback_msg);
        free(context->callback_msg);
        context->callback_msg = NULL;
    }

}

static void on_ext_record_load_done(AsyncResult *ar)
{
    IccIoResult *data;
    Object *result, *userObj;
    struct adn_record_loader_context *context;
    
    userObj = ar->userObj;
    context = (AdnRecordLoaderContext *)object_getData(userObj);

    if(ar->error == 0)
    {
        result = ar->result;
        assert(result);
        data = (IccIoResult *)object_getData(result);
        adn_record_appendExtRecord(data, &(context->adn));
        context->mPendingExtLoads--;
        adn_record_loader_finished(context);
    }
    else
    {
        /* error in the process of loading adn, free the adn_loader_context's 
         * callback_msg.
         */
        cmsLog_error("");
        internal_message_freeObject(context->callback_msg);
        free(context->callback_msg);
        context->callback_msg = NULL;
    }


}


static UBOOL8 adn_record_hasExtendedRecord(struct adn_record *adn)
{
    assert(adn);
    return ((adn->mExtRecord != 0) && (adn->mExtRecord != 0xff));

}

static void adn_record_loader_finished(AdnRecordLoaderContext *context)
{
    assert(context);

    /* Check the status of AdnRecordLoaderContext to see if the operation has
     * been completed.
     */
    if((context->callback_msg != NULL) && (context->mPendingExtLoads == 0))
    {
        /* Done loading one adn record, send back the response to the user.*/
        AsyncResult user_ar;
        Object *result;
        Object *o;
        struct internal_message *callback_msg;

        callback_msg = context->callback_msg;
        user_ar.userObj = internal_message_getObject(callback_msg);
        user_ar.error = 0;
            
        /* Initialise the result object for the AsyncResult. */
        result = object_obtain();
        assert(result);
        object_setData(result, &(context->adn), sizeof(AdnRecord)); 
        user_ar.result = result;

        /* Wrap AsyncResult as Object as well. */
        o = object_obtain();
        object_setData(o, &user_ar, sizeof(AsyncResult));
        internal_message_setObject(callback_msg, o);
            
        cellular_internal_process(callback_msg);

        /* clean up. */
        if(user_ar.result != NULL)
        {
            object_freeData(user_ar.result);
            free(user_ar.result);
            user_ar.result = NULL;
        }

        if(user_ar.userObj != NULL)
        {
            object_freeData(user_ar.userObj);
            free(user_ar.userObj);
            user_ar.userObj = NULL;
        }

        internal_message_freeObject(callback_msg);
        context->callback_msg = NULL;
    }
}

static void adn_record_appendExtRecord(IccIoResult *data, AdnRecord *adn)
{
    char *buf = NULL;
    int buf_len = 0;

    assert(data);
    assert(adn);
    if(data->length != EXT_RECORD_LENGTH_BYTES)
    {
        return;
    }

    if((data->payload[0] & EXT_RECORD_TYPE_MASK) != 
                           EXT_RECORD_TYPE_ADDITIONAL_DATA)
    {
        return;
    } 

    if((0xff & data->payload[1]) > MAX_EXT_CALLED_PARTY_LENGTH)
    {
        //invalid or empty record
        return;
    }

    buf_len = 1 + (data->length)*2;
    buf = malloc(buf_len);
    assert(buf);

    called_party_bcdFragmentToString(data->payload, 2, 
                                     0xff & data->payload[1], buf, buf_len);

    cmsUtl_strncat(adn->mNumber, BUFLEN_32, buf);

    free(buf);
    
    return;
}


/* Parse the bytes into struct adn_record. */
static void adn_record_parseRecord(IccIoResult *data, struct adn_record* record)
{
    int numberLength;
    int footerOffset;
    UINT8 *payload;

    assert(data);
    assert(record);

    footerOffset = data->length - FOOTER_SIZE_BYTES;
    payload = data->payload;
    numberLength = 0xff & payload[footerOffset];

    if (EF_INVALID_BYTE == numberLength)
    {
        /* Invalid ADN entry - ignore */
        return;
    }
    else if(numberLength > MAX_NUMBER_SIZE_BYTES)
    {
        cmsLog_error("numberLength=%d, which exceeded MAX_NUMBER_SIZE_BYTES",
                      numberLength);
        cmsLog_error("sw1(%d), sw2(%d), efid(%d), index(%d), length(%d), footerOffset(%d)",
                      data->sw1, data->sw2, data->efid, data->index, data->length, footerOffset);
        {
            int i;
            char dump[200], *ptr = dump;
            memset(dump, 0, sizeof(dump));
            for (i = 0; i < data->length; i++)
            {
                ptr += sprintf(ptr, "%02X ", data->payload[i]);
            }
            cmsLog_error("payload: %s", dump);
        }
        return;
    }

    /* Please note 51.011 10.5.1 :
     * "If the Dialling Number/SSC String does not contain a dialing number,
     * e.g. a control string deactivating a service, the TON/NPI byte shall 
     * be set to 'FF' by the ME (see note 2)"
     */
    called_party_bcdToString(payload, footerOffset+1, numberLength, 
                             (char *)&(record->mNumber), BUFLEN_32);
    record->mExtRecord = 0xff & (payload[data->length - 1]);

}

static void called_party_bcdToString(UINT8 *bytes, int offset, int length, 
                                     char *buf, int buf_len)
{
    UBOOL8 prependPlus = FALSE;
    char *ret = NULL;
    int ret_len = 0;

    if(length < 2)
    {
        return;
    }

    /*Only TON field should be taken in consideration */
    if((bytes[offset] & 0xf0) == (TOA_International & 0xf0))
    {
        prependPlus = TRUE;
    }

    ret_len = 1+length*2;
    ret = malloc(ret_len);
    assert(ret);

    called_party_bcdFragmentToString(bytes, offset+1, length-1, ret, ret_len);
    
    if(prependPlus)
    {
        /* To be implemented: 
         * This is an international number and should have a plus prepended 
         * to the dialing number. But there can also be GSM MMI codes as defined
         * in TS 22.030 6.5.2, so we need to handle those also.*/
    }

    cmsUtl_strncpy(buf, ret, buf_len);
    free(ret);
}

/* Converts bytes array into a string array. The returned string is placed in
 * the memory pointed to by ret.
 */
static void called_party_bcdFragmentToString(UINT8 *bytes, int offset, 
                                           int length, char *ret, int ret_len)
{
    int i;
    int index = 0;

    for(i = offset; i < length + offset; i++)
    {
        UINT8 b;
        char c;

        c = bcdToChar(bytes[i]&0xf);

        if(c == 0)
        {
            return;
        }

        if(index < ret_len-1)
        {
            ret[index++] = c;
        }
        
        // FIXME(mkf) TS 23.040 9.1.2.3 says
        // "if a mobile receives 1111 in a position prior to
        // the last semi-octet then processing shall commence with
        // the next semi-octet and the intervening
        // semi-octet shall be ignored"
        // How does this jive with 24.008 10.5.4.7

        b = (UINT8)((bytes[i] >> 4) & 0xf);

        if (b == 0xf && i + 1 == length + offset) 
        {
            //ignore final 0xf
            break;
        }

        c = bcdToChar(b);
        if (c == 0) 
        {
            return;
        }

        if(index < ret_len-1)
        {
            ret[index++] = c;
        }
    }

}

/** returns 0 on invalid value */
static char bcdToChar(UINT8 b) 
{
    if (b < 0xa) 
    {
        return (char)('0' + b);
    } 
    else
    { 
        switch (b) 
        {
            case 0xa: return '*';
            case 0xb: return '#';
            case 0xc: return PAUSE;
            case 0xd: return WILD;
            default: return 0;
        }
    }
}

