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
#include "misc_tracker.h"
#include "cellular_msg.h"
#include "cellular_internal.h"
#include "cms_msg_cellular.h"

#include <assert.h>


static void misc_tracker_get_ril_request(Object *internalObj)
{
    struct internal_message *rilMsg;
    MiscTrackerGetRilReq *getRilReq;
	
    getRilReq = (MiscTrackerGetRilReq *)object_getData(internalObj);
	
    rilMsg = internal_message_obtain(EVENT_MISC_GET_RIL_REQUEST_DONE, MISC_TRACKER);
    assert(rilMsg);
    internal_message_setObject(rilMsg, internalObj);	
	
    ril_access_get(getRilReq->rilReq, rilMsg);
}


static void misc_tracker_get_ril_request_done(Object *internalObj)
{
   AsyncResult *ar = (AsyncResult *)object_getData(internalObj);
   Object *result;
   Object *userObj;
   MiscTrackerGetRilReq *getRilReq;
   
   assert(ar);
   
   userObj = ar->userObj;
   result = ar->result;
   
   assert(userObj);
   
   getRilReq = (MiscTrackerGetRilReq *)object_getData(userObj);
   cms_access_send_resp(getRilReq->serial, ar->error?CMSRET_INTERNAL_ERROR:CMSRET_SUCCESS, result);
}


void misc_tracker_init()
{
    //init task if any
}


void misc_tracker_process(struct internal_message *msg)
{
    Object *internalObj = internal_message_getObject(msg);

    assert(internalObj);
	
    switch(msg->what)
    {
        case EVENT_MISC_GET_RIL_REQUEST:
            cmsLog_debug("EVENT_MISC_GET_RIL_REQUEST");
            misc_tracker_get_ril_request(internalObj);
            internal_message_setObject(msg, NULL);
            break;

        case EVENT_MISC_GET_RIL_REQUEST_DONE:
            cmsLog_debug("EVENT_MISC_GET_RIL_REQUEST_DONE");
            misc_tracker_get_ril_request_done(internalObj);
            break;

        default:
            cmsLog_error("unrecognized msg: %d", msg->what);
            break;
    }
}

