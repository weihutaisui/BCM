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
#include "cms_log.h"
#include "cms_util.h"
#include "ril_access.h"
#include "cms_access.h"
#include "sms_tracker.h"
#include "cellular_msg.h"
#include "cellular_internal.h"
#include "cms_msg_cellular.h"
#include "icc_file_handler.h"

#include <assert.h>


static void handle_new_sms(AsyncResult *ar);

void sms_tracker_init()
{
}


void sms_tracker_process(struct internal_message *msg)
{
    AsyncResult *ar;
    Object *o = internal_message_getObject(msg);

    switch(msg->what)
    {
        case EVENT_NEW_SMS:
            assert(o);
            ar = (AsyncResult *)object_getData(o);
            handle_new_sms(ar);
            break;

        default:
            break;
    }
}

/*
 * SMS message comes in via RIL_UNSOL_RESPONSE_NEW_SMS.
 * It is hexical/decimal value encoded into a string. We need to parse the
 * string to get the real pdu, and dispatch it to the client. 
 */
static void handle_new_sms(AsyncResult *ar)
{
    char *s;
    int s_len, pdu_len;
    UINT8 *pdu;

    assert(ar);

    if(ar->error == 0)
    {    
        assert(ar->result);
        s = (char *)object_getData(ar->result);
        s_len = cmsUtl_strlen(s);
        pdu_len = 0;
        if(s_len > 0)
        {
            pdu = icc_util_hexStringToBytes(s, &pdu_len);
            cms_access_notify_newSms(FALSE, -1, pdu, pdu_len);
        }
    }
}


