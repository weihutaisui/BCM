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


#include "data_connection.h"
#include "dc_tracker.h"
#include "ril_access.h"
#include "ril.h"
#include "cellular_msg.h"
#include "timer_manager.h"
#include "cms_log.h"
#include <assert.h>


static void process_connect(struct data_connection *dc);
static void process_disconnect(struct data_connection *dc);
static void process_setup_data_connection_done(struct data_connection *dc,
                                               AsyncResult  *ar);
static void process_deactive_done(struct data_connection *dc, 
                                     AsyncResult *ar);
static void process_lost_connection(struct data_connection *dc);

static void on_connect(struct data_connection *dc);
static void transition_to(struct data_connection *dc, DataConnectionState newState);
static void state_on_exit(struct data_connection *dc);
static void state_on_enter(struct data_connection *dc);
static DcSetupResult on_setup_connection_completed(DataConnection *dc, 
                                                   AsyncResult* ar);


static void tear_down_data(struct data_connection *dc);
static UBOOL8 check_dc_index(int dc_index);
static const char* state_to_string(DataConnectionState state);



/* for now we just allocate 4 data connection entries */
static struct data_connection connection_table[DATA_CONNECTIONS_MAX];

UBOOL8 data_connection_init_entry(int dc_index, struct apn_setting *apn)
{
    struct data_connection *dc;
    if(check_dc_index(dc_index) == TRUE)
    {
        dc = &(connection_table[dc_index]);
        dc->mInitialMaxRetry = DATA_CONNECTIONS_MAX_RETRY;
        dc->mRilRat = RADIO_TECH_LTE;
        memcpy(&dc->apnSetting, apn, sizeof(struct apn_setting));
        dc->mDataRegState = 0;
        dc->mState = DcInactiveState;
        dc->mProfileId = 0;
        dc->mTag = 0;
        dc->mCid = 0;
        dc->mIndex = dc_index;
        return TRUE;
    }
    return FALSE;
}


void data_connection_init()
{
    clear_all_data_connections();
}

void clear_all_data_connections()
{
    int i;
    for(i=0; i<DATA_CONNECTIONS_MAX; i++)
    {
        memset(&connection_table[i], 0x00, sizeof(struct data_connection)); 
    }
}

int find_free_data_connection()
{
    int i;
    for(i=0; i<DATA_CONNECTIONS_MAX; i++)
    {
        if(connection_table[i].mState == DcInactiveState || connection_table[i].mState == DcDisconnectingErrorCreatingConnection)
        {
            return i;
        }
    }
    return -1;
}

int find_data_connection_by_cid(int cid)
{
    int i;
    cmsLog_debug("cid=%d", cid);

    for(i=0; i<DATA_CONNECTIONS_MAX; i++)
    {
        if(connection_table[i].mCid == cid)
            return i;
    }
    return -1;
}

static UBOOL8 check_dc_index(int dc_index)
{
    if(dc_index >= 0 && dc_index < DATA_CONNECTIONS_MAX)
        return TRUE;
    return FALSE;
}

void data_connection_process(struct internal_message* msg)
{
    DataConnection *dc;
    AsyncResult *ar;
    int dc_index;
    Object *o;

    assert(msg);
    o = msg->object;
    
    switch(msg->what)
    {
        case EVENT_DC_CONNECT:
            dc_index = *(int *)object_getData(o);
            if(check_dc_index(dc_index) == TRUE)
            {
                dc = &connection_table[dc_index];
            }
            else
            {
                cmsLog_notice("dc index %d out of bound!\n", dc_index); 
                return;
            }
            process_connect(dc);
            break;
        
        case EVENT_DC_DISCONNECT:
            dc_index = *(int *)object_getData(o);
            if(check_dc_index(dc_index) == TRUE)
            {
                dc = &connection_table[dc_index];
            }
            else
            {
                cmsLog_notice("dc index %d out of bound!\n", dc_index); 
                return;
            }
            process_disconnect(dc);
            break;

        case EVENT_DC_SETUP_DATA_CONNECTION_DONE:
            ar = (AsyncResult *)object_getData(o);
            dc_index = *(int *)object_getData(ar->userObj);
            if(check_dc_index(dc_index) == TRUE)
            {
                DataConnection *dc = &connection_table[dc_index];
                process_setup_data_connection_done(dc, ar);
            }
            break;

        case EVENT_DC_DEACTIVE_DONE:
            ar = (AsyncResult *)object_getData(o);
            dc_index = *(int *)object_getData(ar->userObj);
            cmsLog_debug("dc_index = %d", dc_index);
            if(check_dc_index(dc_index) == TRUE)
            {
                DataConnection *dc = &connection_table[dc_index];
                process_deactive_done(dc, ar);
            }
            break;

        case EVENT_DC_LOST_CONNECTION:
            dc_index = *(int *)object_getData(o);
            if(check_dc_index(dc_index) == TRUE)
            {
                DataConnection *dc = &connection_table[dc_index];
                process_lost_connection(dc);
            }

        default:
            break;

    }
}

/*
 * The state <---> event table has been reversed in the C code. Now for each 
 * EVENT, we handle it for different states we are in. Unlike in object-oriented
 * code where each state implements its own event handler.
 */

static void process_connect(struct data_connection *dc)
{
    cmsLog_debug("dc index=%d", dc->mIndex);
    switch(dc->mState)
    {
        case DcInactiveState:
        case DcRetryingState:
            on_connect(dc);
            transition_to(dc, DcActivatingState);
            break;

        default:
            break;
     }

}

static void process_disconnect(struct data_connection *dc)
{
    cmsLog_debug("dc index=%d", dc->mIndex);
    switch(dc->mState)
    {
        case DcActiveState:
            tear_down_data(dc);
            transition_to(dc, DcDisconnectingState);
            break;
            
        default: 
            break;

    }
}

static void process_setup_data_connection_done(struct data_connection *dc,
                                               AsyncResult  *ar)
{
    DcSetupResult result;
    struct internal_message *msg;
	
    cmsLog_debug("dc index=%d, mState=%d", dc->mIndex, dc->mState);
	
    switch(dc->mState)
    {
        case DcActivatingState:
            result = on_setup_connection_completed(dc, ar);
            switch(result)
            {
                case SUCCESS:
                    transition_to(dc, DcActiveState);
                    break;

                case RETRY:
                    if(dc->mInitialMaxRetry-- > 0)
                    {
                       msg = internal_message_obtain_with_data(EVENT_DC_CONNECT, DATA_CONNECTION, 
                                                 &(dc->mIndex), sizeof(dc->mIndex));
                    					   
                       if(timer_manager_add(5000, msg) == TRUE)
                       {
                          transition_to(dc, DcRetryingState);
                       }
                       else
                       {
                          cmsLog_error("Failed to add timer");
                          internal_message_free_and_null_ptr(&msg);
						   
                          transition_to(dc, DcInactiveState);
                       }
                    }
                    else
                    {
                       transition_to(dc, DcDisconnectingErrorCreatingConnection);
                    }
					
                    break;

                case ERR_BadCommand:
                    transition_to(dc, DcInactiveState);
                    break;

                case ERR_UnacceptableParameter:
                    tear_down_data(dc);
                    transition_to(dc, DcDisconnectingErrorCreatingConnection);
                    break;

                default:
                    cmsLog_notice("set up data connection error %d\n", result);
                    transition_to(dc, DcInactiveState);
                    break;
            }  
            break;

        default: 
            break;
    }
}

static void process_deactive_done(struct data_connection *dc, 
                                     AsyncResult *ar)
{
    cmsLog_debug("dc[%d]: current state is %s", dc->mIndex,
                                       state_to_string(dc->mState));
    switch(dc->mState)
    {
        case DcDisconnectingState:
            transition_to(dc, DcInactiveState);
            break;

        default:
            break;
    }
}

static void process_lost_connection(struct data_connection *dc)
{
    struct internal_message *msg;
    struct object *o;

    switch(dc->mState)
    {
        case DcActiveState:
            o = object_obtain();
            object_setData(o, &(dc->mIndex), sizeof(dc->mIndex));
            msg = internal_message_obtain(EVENT_DC_CONNECT, 
                                          DATA_CONNECTION);
            internal_message_setObject(msg, o);
            o = NULL;

            if(timer_manager_add(2000, msg) == TRUE)
            {
                transition_to(dc, DcRetryingState);
            }
            else
            {
                internal_message_freeObject(msg);
                free(msg);
                msg = NULL;
                transition_to(dc, DcInactiveState);
            }
            break;
        
        default:
            break;
     }
}

static DcSetupResult on_setup_connection_completed(DataConnection *dc, 
                                                AsyncResult* ar)
{
    DcSetupResult result;
    RIL_Data_Call_Response_v6* response;

    assert(ar);

    if(ar->error != 0)
    {
        /* 
         * the error handling could be more elabrate. 
         * However, let's just be rough for now.
         */
        result = ERR_RilError;
    }
    else
    {
        assert(ar->result);
        response = (RIL_Data_Call_Response_v6 *)object_getData(ar->result);

        if(response->status != 0)
        {
            switch(response->status)
            {
                case PDP_FAIL_SERVICE_OPTION_OUT_OF_ORDER:
                case PDP_FAIL_ERROR_UNSPECIFIED:
                    result = RETRY;
                    break;
                
                default:
                    result = ERR_State;
                    break;
            }
        }
        else
        {

            dc->mCid = response->cid;
            /*
             * Notify ssk about link status change
             */
            cmsLog_debug("type = %s", response->type);
            cmsLog_debug("ifname = %s", response->ifname);
            cmsLog_debug("addresses = %s", response->addresses);
            cmsLog_debug("dnses = %s", response->dnses);
            cmsLog_debug("gateways = %s", response->gateways);

            result = SUCCESS;
        }
        /* Have to free the strings here because the overall free in cellularapp.c
         * would not free them. 
         * */ 
        free(response->type);
        response->type = NULL;

        free(response->ifname);
        response->ifname = NULL;

        free(response->addresses);
        response->addresses = NULL;

        free(response->dnses);
        response->dnses = NULL;

        free(response->gateways);
        response->gateways = NULL;

    }
    return result;
}

static void tear_down_data(struct data_connection *dc)
{
    int discReason = DEACTIVATE_REASON_NONE;
    struct internal_message *msg;

    assert(dc);

    msg = internal_message_obtain(EVENT_DC_DEACTIVE_DONE, DATA_CONNECTION);
     
    Object *o = object_obtain();
    object_setData(o, &dc->mIndex, sizeof(int));
    internal_message_setObject(msg, o);
    o = NULL;

    if(ril_access_get_radio_state() == RADIO_ON)
    {
        ril_access_deactivate_data_call(dc->mCid, discReason, msg);
    }
    else
    {
        /* radio is off, set the EVENT_DC_DEACTIVE_DONE immediately */
        AsyncResult ar;
        ar.result = NULL;
        ar.userObj = internal_message_getObject(msg);
       
        Object *wrapper = object_obtain();
        object_setData(wrapper, (void *)&ar, sizeof(AsyncResult)); 
        internal_message_setObject(msg, wrapper);
        wrapper = NULL;

        data_connection_process(msg);

        internal_message_freeObject(msg);
        free(msg);
    } 

}

static void on_connect(struct data_connection *dc)
{
    struct internal_message *result = 
        internal_message_obtain(EVENT_DC_SETUP_DATA_CONNECTION_DONE, 
                                DATA_CONNECTION);

    Object *o = object_obtain();
    object_setData(o, &dc->mIndex, sizeof(int));
    internal_message_setObject(result, o);
    o = NULL;

    char *protocol = dc->apnSetting.protocol;

    char sRilRat[15];
    sprintf(sRilRat, "%d", dc->mRilRat+2);

    char sProfileId[15];
    sprintf(sProfileId, "%d", dc->mProfileId);

    char sAuthType[15];
    int authType = dc->apnSetting.authType;
    sprintf(sAuthType, "%d", authType);

    ril_access_setup_data_call(sRilRat, sProfileId, dc->apnSetting.apn,
            dc->apnSetting.user, dc->apnSetting.password, sAuthType,
            protocol, result);

}


static void transition_to(struct data_connection *dc, DataConnectionState newState)
{
    state_on_exit(dc);
    dc->mState = newState;
    state_on_enter(dc);
}

/* send out notifitations upon leaving the old data connection state */
static void state_on_exit(struct data_connection *dc)
{
    cmsLog_debug("dataconnection %d: exit %s\n", dc->mTag, state_to_string(dc->mState));
}

/* send out notifications upon entering the new data connection state */
static void state_on_enter(struct data_connection *dc)
{
    cmsLog_debug("dataconnection %d: enter %s\n", dc->mIndex, state_to_string(dc->mState));
    dc_tracker_state_change(dc->mIndex, dc->mState);
}

static const char* state_to_string(DataConnectionState state)
{
    switch(state)
    {
        case DcInactiveState:        return "DcInactiveState";
        case DcActivatingState:     return "DcActivatingState";
        case DcActiveState:          return "DcActiveState";
        case DcDisconnectingState:   return "DcDisconnectingState";
        case DcRetryingState:        return "DcRetryingState";
        case DcDisconnectingErrorCreatingConnection: return "DcDisconnectingErrorCreatingConnection";
        default:   return "Dc state unknown";
    }
}
