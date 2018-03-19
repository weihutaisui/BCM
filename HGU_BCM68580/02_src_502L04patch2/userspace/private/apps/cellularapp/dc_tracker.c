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


#include <assert.h>
#include <string.h>

#include "cms.h"
#include "cms_log.h"
#include "cms_msg.h"
#include "cms_util.h"

#include "cms_msg_cellular.h"
#include "rut2_cellular.h"

#include "cellular_msg.h"
#include "dc_tracker.h"
#include "data_connection.h"
#include "uicc_controller.h"
#include "cms_access.h"
#include "service_state_tracker.h"
#include "timer_manager.h"

#include "ril_access.h"
#include "ril.h"


/*
 * dc_tracker acts as the manager for all apn_contexts. An apn_context is a 
 * context for a particular apn type's data connection. We can have multiple
 * apn_contexts active in the system. For each apn_context, it can choose from 
 * a list of apn_settings to set up the data call. A data connection is maintained
 * in data_connection module.
 */

static void create_all_apn_list(int application_index);
static void setup_data_on_connectable_apns();
static UBOOL8 try_setup_data(struct apn_context * apnContext);
static void setup_data_connection(struct apn_context *apnContext);
static void tear_down_data_connection(struct apn_context *apnContext);
static void on_data_connection_attached();
static void on_data_state_changed(AsyncResult *ar);
static void on_try_setup_data(int dc_index);
static struct apn_context * dc_tracker_get_context(int dc_index);
static UBOOL8 is_context_connectable(struct apn_context* a);
static UBOOL8 is_context_ready(struct apn_context *a);
static struct apn_context * dc_tracker_findFreeContext();
static struct apn_context * dc_tracker_findContextByIfname(const char* ifname);
static void process_enable_apn(DevCellularAccessPointMsgBody *cms_apn);
static const char* state_to_string(DcState state);
static void reset_apn_context(struct apn_context *a);

static struct apn_setting initialAttachApnSetting; 
static struct apn_context apn_context_table[MAX_APN_CONTEXT];
static UBOOL8 mActivity = DC_ACTIVITY_NONE;
UBOOL8 mRecordLoaded = FALSE;
static ApnSetting mAllApnSettings[APN_AUTO_SEL_MAX_CANDIDATE];
static ApnSetting *mPerTypeApnSettings[APN_TYPE_MAX][APN_AUTO_SEL_MAX_CANDIDATE];
/*value of apnTypeMappingTbl should be in the order of APN_AUTO_SEL_TYPE_XXX*/
char *mApnTypeMappingTbl[APN_TYPE_MAX] = {"default"};


static void switch_to_next_candidate_apn(ApnContext *apnContext)
{
   UINT8 i;
   
   /* return if user specified apn */
   if (apnContext->pApnSetting == &apnContext->mApnSetting)
   {
      return;
   }

   /* Sarah: hard code default here */
   for (i = 0; i < APN_AUTO_SEL_MAX_CANDIDATE_PER_TYPE && mPerTypeApnSettings[APN_TYPE_DEFAULT][i]; i++)
   {
      if (mPerTypeApnSettings[APN_TYPE_DEFAULT][i] == apnContext->pApnSetting)   
      {
        break;
      }
   }
   
   ++i;
   
   if (i < APN_AUTO_SEL_MAX_CANDIDATE_PER_TYPE && mPerTypeApnSettings[APN_TYPE_DEFAULT][i])
   {
      /* swith to next apn */   
      apnContext->pApnSetting = mPerTypeApnSettings[APN_TYPE_DEFAULT][i];
   }
   else
   {
      /* roll back to first apn */   
      apnContext->pApnSetting = mPerTypeApnSettings[APN_TYPE_DEFAULT][0];
   }

   cmsLog_debug("(i=%d) apn = %s, id = %d", i, apnContext->pApnSetting->apn, apnContext->pApnSetting->id);
   
}

static UBOOL8 can_handle_apn_type(ApnSetting *as, ApnType type)
{
   cmsLog_debug("as = %s, type = %d", as->types, type);
   
   if(cmsUtl_strstr(as->types, mApnTypeMappingTbl[type]))
   {
       return TRUE;
   }
   
   return FALSE;
}

static void build_waiting_apns(struct apn_context *context)
{
   int radioTech;
   struct apn_setting *apn_node;
   UINT8 apnType, allApnId, perTypeApnId;
   
   cmsLog_debug("ifname=%s", context->ifname);
   
   /* return if use user specified apn setting */
   if(context->pApnSetting == &context->mApnSetting)
   {
       return;
   } 

   service_state_tracker_get_access_tech(&radioTech);
   
   cmsLog_debug("radioTech=%d", radioTech);
   
//Sarah: skip preferred apn logic here as not nesseary
#if 0   
   operatorNumeric = uicc_controller_getOperatorNumeric(application_index);
   if(operatorNumeric == NULL)
   {
       cmsLog_error("Failed to get operator numeric from uicc controller!");
       return;
   }
   

   if(mPreferredApn != NULL)
   {
       if(can_handle_apn_type(mPreferredApn->setting, 
                              context->context_type))
       {
           ApnSetting* pref_setting = mPreferredApn->setting;
   
           cmsLog_debug("building waiting apn: preferred apn: %s",
                                                      pref_setting->apn);
   
          if(0 == cmsUtl_strcmp(pref_setting->numeric, operatorNumeric))
          {
              if(pref_setting->bearer == 0 || 
                 pref_setting->bearer == radioTech)
              {
                  /* 
                   * Add to the waiting apn list for the specified 
                   * apn context */
                  apn_node = (struct apn_setting_node *)
                              malloc(sizeof(ApnSettingNode));
                  apn_node->setting = copy_apn_setting(pref_setting);
                  apn_node->next = NULL;
                  if(context->waiting_apns == NULL)
                  {
                      context->waiting_apns = apn_node;
                  }               
                  else
                  {
                      add_to_apn_list(context->waiting_apns, apn_node);
                  }
              }
   
          }
       }   
   }
   
   free(operatorNumeric);
   operatorNumeric = NULL; 
   
#endif

   apnType = context->mApnType;

   /* Now parse the mAllApnSettings to see if there is anything to add. */
   for (allApnId = 0, perTypeApnId = 0; 
        allApnId < APN_AUTO_SEL_MAX_CANDIDATE && 
           mAllApnSettings[allApnId].apn && 
           perTypeApnId < APN_AUTO_SEL_MAX_CANDIDATE_PER_TYPE; 
        allApnId++)   
   {
       apn_node = &mAllApnSettings[allApnId];
       if(can_handle_apn_type(apn_node, apnType))
       {
           if( apn_node->bearer == 0 ||  apn_node->bearer == radioTech)
           {
              cmsLog_debug("add candidate apn %s, id=%d", apn_node->apn, apn_node->id);
              mPerTypeApnSettings[apnType][perTypeApnId++] = apn_node;
           } 
       }
   }

   /* set last mPerTypeApnSettings to NULL */
   if (perTypeApnId < APN_AUTO_SEL_MAX_CANDIDATE_PER_TYPE)		
   {
      mPerTypeApnSettings[apnType][perTypeApnId] = NULL;
   } 

   context->pApnSetting = mPerTypeApnSettings[apnType][0];
}


static void init_default_apn(struct apn_setting *s)
{
    assert(s);
    
    memset(s, 0x00, sizeof(struct apn_setting));

    s->carrier = "中国移动 (China Mobile) GPRS";
    s->apn = "cmnet";
    s->proxy = NULL;
    s->mmsc = NULL;
    s->mmsProxy = NULL;
    s->user = NULL;
    s->password = NULL;
    
    s->types = "default,supl";
    s->authType = 0; 
    s->protocol = "IP";
    s->carrierEnabled = TRUE;
}

void dc_tracker_init()
{
    int i;
    struct apn_context *s;
    
    init_default_apn(&initialAttachApnSetting);

    for(i=0; i<MAX_APN_CONTEXT; i++)
    {
        s = &apn_context_table[i];
        reset_apn_context(s); 
    }

    memset(mAllApnSettings, 0, sizeof(mAllApnSettings));
    memset(mPerTypeApnSettings, 0, sizeof(mPerTypeApnSettings));
	
    /* hard code the default apn context. To be changed later. */
    /*
    s = &apn_context_table[0];
    s->mApnType = "default";
    s->pApnSetting = NULL;
    s->mDataEnabled = TRUE;
    */
}


void dc_tracker_process(struct internal_message *msg)
{
    Object *o;
    AsyncResult *ar;
    int application_index;
    DevCellularAccessPointMsgBody *cms_apn;

    assert(msg);
    switch(msg->what)
    {
        case EVENT_RECORD_LOADED:
            o = internal_message_getObject(msg);
            assert(o);
            application_index = *(int *)object_getData(o);

            mRecordLoaded = TRUE;
            create_all_apn_list(application_index);
            setup_data_on_connectable_apns();
            break;

        case EVENT_DATA_STATE_CHANGED:
            o = internal_message_getObject(msg);
            ar = (AsyncResult *)object_getData(o);
            on_data_state_changed(ar); 
            break;

        case EVENT_DATA_CONNECTION_ATTACHED:
            on_data_connection_attached();
            break;

        case EVENT_TRY_SETUP_DATA:
            o = internal_message_getObject(msg);
            assert(o);
            on_try_setup_data(*(int *)object_getData(o));
            break;

        case EVENT_CMS_ENABLE_APN:
            o = internal_message_getObject(msg);
            assert(o);
            cms_apn = (DevCellularAccessPointMsgBody *)object_getData(o);
            /* the command from cms to enable or disable the apn context */ 
            process_enable_apn(cms_apn);
            break;

        default:
            cmsLog_notice("unhandled event %d", msg->what);
            break;
    }
}

/**                                                                         
 * Based on the sim operator numeric, create a list for all possible        
 * Data Connections and setup the preferredApn.                             
 */  
static void create_all_apn_list(int application_index)
{
   char *operatorNumeric;
   char mcc[BUFLEN_4];
   char mnc[BUFLEN_4];
   UINT32 count;
   UINT8 allApnId;
   Dev2CellularApnInfo cellularApnInfo;    
   CmsRet ret;
   
   /* uicc_controller_getOperatorNumeric returns the operator numeric on heap.
    * The user is reponsible to free the memory.
    */
   operatorNumeric = uicc_controller_getOperatorNumeric(application_index);
   if(operatorNumeric == NULL)
   {
       cmsLog_error("Failed to get operator numeric from uicc controller!");
       return;
   }
      
   memset(mcc, 0, sizeof(mcc));	
   memset(mnc, 0, sizeof(mcc));	
   cmsUtl_strncpy(mcc, operatorNumeric, BUFLEN_4);
   cmsUtl_strncpy(mnc, operatorNumeric + 3, BUFLEN_4);

   cmsLog_debug("operatorNumeric=%s, mcc=%s, mnc=%s", operatorNumeric, mcc, mnc);

   free(operatorNumeric);

   if ((ret = rutCellularDB_getApnCount(mcc, mnc, &count)) != CMSRET_SUCCESS)
   {
      cmsLog_error("rutCellularDB_getApnCount returns error:%d", ret);
      return;
   }

   if (count > APN_AUTO_SEL_MAX_CANDIDATE)
   {
      count = APN_AUTO_SEL_MAX_CANDIDATE;
   }
   
   for (allApnId = 0; allApnId < count; allApnId++)
   {
      if ((ret = rutCellularDB_getApnByOffset(mcc, mnc, allApnId, &cellularApnInfo)) != CMSRET_SUCCESS)
      {
         cmsLog_error("rutCellularDB_getApnByOffset returns error:%d", ret);
         return;
      }

      CMSMEM_REPLACE_STRING(mAllApnSettings[allApnId].carrier, cellularApnInfo.carrier);
	  
      CMSMEM_REPLACE_STRING(mAllApnSettings[allApnId].apn, cellularApnInfo.APN);
	  
      CMSMEM_REPLACE_STRING(mAllApnSettings[allApnId].types, cellularApnInfo.type);
	  
      CMSMEM_REPLACE_STRING(mAllApnSettings[allApnId].proxy, cellularApnInfo.proxy);
	  
      mAllApnSettings[allApnId].port = cellularApnInfo.proxyPort;
	  
      CMSMEM_REPLACE_STRING(mAllApnSettings[allApnId].mmsc, cellularApnInfo.mmsc);
	  
      mAllApnSettings[allApnId].mmsPort = cellularApnInfo.mmsport;
	  
      CMSMEM_REPLACE_STRING(mAllApnSettings[allApnId].user, cellularApnInfo.username);
	  
      CMSMEM_REPLACE_STRING(mAllApnSettings[allApnId].password, cellularApnInfo.password);
	  
      mAllApnSettings[allApnId].authType = cellularApnInfo.authtype;
	  
      mAllApnSettings[allApnId].id = cellularApnInfo.id;
	  
      CMSMEM_REPLACE_STRING(mAllApnSettings[allApnId].types, cellularApnInfo.type);
	  
      CMSMEM_REPLACE_STRING(mAllApnSettings[allApnId].protocol, 
                                                cellularApnInfo.enblIpVer == ENBL_IPV4_IPV6 ? "IPV4V6" : 
                                                (cellularApnInfo.enblIpVer == ENBL_IPV4_ONLY ? "IP" : "IPV6"));
	  
      //Sarah: TODO: skip numeric, roamingProtocol
      
      mAllApnSettings[allApnId].carrierEnabled = TRUE;
	  
      mAllApnSettings[allApnId].bearer = cellularApnInfo.bearer;
	  
      cmsLog_debug("add apn(%d) %s, carrier = %s, id=%d", 
                   allApnId, cellularApnInfo.APN, cellularApnInfo.carrier, cellularApnInfo.id);
   }

   if (allApnId < APN_AUTO_SEL_MAX_CANDIDATE)
   {
      /* just free apn here to delimit an available entry */   
      cmsMem_free(mAllApnSettings[allApnId].apn);
   }  
   
#if 0
    /* Setup the preferred apn now. */
    mcc = malloc(4);
    assert(mcc);
    cmsUtl_strncpy(mcc, operatorNumeric, 4);

    /* 
     * str_len of mnc is operatorNumeric's length substracts mcc's length, 
     * and add up the length of the terminating '\0'.
     */
    str_len = cmsUtl_strlen(operatorNumeric) - 3 + 1;
    mnc = malloc(str_len);
    assert(mnc);
    cmsUtl_strncpy(mnc, operatorNumeric+3, str_len);


    mPreferredApn = get_preferred_apn(mcc, mnc);
    if((mPreferredApn != NULL) &&
       (0 != cmsUtl_strcmp(mPreferredApn->setting->numeric, operatorNumeric))) 
    {
        free_apn_list(mPreferredApn);
        mPreferredApn = NULL;
    }
#endif

}

static UBOOL8 is_context_connectable(struct apn_context* a)
{
    return is_context_ready(a) && ((a->mState == DC_STATE_IDLE) ||
                          (a->mState == DC_STATE_SCANNING) ||
                          (a->mState == DC_STATE_RETRYING) ||
                          (a->mState == DC_STATE_FAILED));
}


static UBOOL8 is_context_ready(struct apn_context *a)
{
    return (a->mDataEnabled);
}

static void reset_apn_context(struct apn_context *a)
{
    a->mApnType = APN_TYPE_DEFAULT;

    CMSMEM_FREE_BUF_AND_NULL_PTR(a->ifname);
    
    a->priority = 0;
    a->mState = DC_STATE_IDLE;
    a->pApnSetting = NULL;
    a->mDataEnabled = FALSE;
    a->connection_index = -1;
}

/*
 * The function should loop through all apn contexts and try to set up 
 * data connection for the connectable apn contexts. For now, we just
 * set up the data connection for the default apn context, which is hard-coded.
 */
static void setup_data_on_connectable_apns()
{
    int i;
    struct apn_context* a;

    for(i=0; i<MAX_APN_CONTEXT; i++)
    {
        a = &apn_context_table[i];
        if(is_context_connectable(a))
        {
            cmsLog_debug("try_setup_data on apn context[%d]: ifname=%s", i, 
                                                            a->ifname);
            try_setup_data(a);
        }
        else
        {
            cmsLog_debug("apn context %d not connectable, skip it...", i);
            cmsLog_debug("mDataEnabled=%d, mState=%s", a->mDataEnabled,
                                                    state_to_string(a->mState));  
        }
    }
}

static UBOOL8 try_setup_data(struct apn_context * apnContext)
{
    cmsLog_debug("apnContext->mState = %d", apnContext->mState);
			
    if(apnContext->mState == DC_STATE_FAILED)
    {
        apnContext->mState = DC_STATE_IDLE;
    }
	
    if(apnContext->mState == DC_STATE_IDLE || apnContext->mState == DC_STATE_SCANNING)
    {
       if(apnContext->mState == DC_STATE_IDLE)    
       {
          build_waiting_apns(apnContext);
		  
          if(apnContext->pApnSetting == NULL || apnContext->pApnSetting->apn == NULL)
          {
              cmsLog_error("empty apns, apnContext->ifname: %s", apnContext->ifname);
              return FALSE;
          }
       }
		
       setup_data_connection(apnContext);
       return TRUE;
    }
	
    return FALSE;
}

/*
 * For each apn context, we need a data connection to represent it's actual 
 * data connection in ril. The structure of "data connection" contains information
 * about the states of current connection.Upon tear down, the data connection
 * would be released to its pool.
 */
static void setup_data_connection(struct apn_context *apnContext)
{
    struct internal_message *msg;
    struct object *o;

    int dc = find_free_data_connection();
    if(dc < 0)
    {
        cmsLog_error("Couldn't find a free data connection to use, abort...");
        return;
    }
    cmsLog_debug("availble data connection slot: %d", dc);

    apnContext->connection_index = dc;
    apnContext->mState = DC_STATE_CONNECTING;
    
    if(data_connection_init_entry(dc, apnContext->pApnSetting) == FALSE)
    {
        cmsLog_error("Failed to initialize data connection entry, abort!");
        return;
    }

    /* instead of direct function call, we pass an internal_message to 
     * data_connection module. And then the message needs to be freed.
     */
    o = object_obtain();
    object_setData(o, (const void*)&dc, sizeof(int));
    msg = internal_message_obtain(EVENT_DC_CONNECT, DATA_CONNECTION);
    internal_message_setObject(msg, o);
    o = NULL;

    data_connection_process(msg);
    internal_message_freeObject(msg);
    free(msg);
}


static void tear_down_data_connection(struct apn_context *apnContext)
{
    struct internal_message *msg;
    struct object *o;
        
    apnContext->mState = DC_STATE_DISCONNECTING;

    o = object_obtain();
    object_setData(o, (const void*)&(apnContext->connection_index), sizeof(int));
    msg = internal_message_obtain(EVENT_DC_DISCONNECT, DATA_CONNECTION);
    internal_message_setObject(msg, o);
    o = NULL;

    data_connection_process(msg);
    internal_message_freeObject(msg);
    free(msg);
}

static void on_data_connection_attached()
{
    cmsLog_debug("data connection is attached.");
    setup_data_on_connectable_apns();
}

/* A mixed logic of DcController and DcTracker's onDataStateChanged(...) 
 * in aosp code. */

static void on_data_state_changed(AsyncResult *ar)
{
    RIL_Data_Call_Response_v6 *new_dataCallState;
    UBOOL8 isAnyDataCallActive = FALSE;
    UBOOL8 isAnyDataCallDormant = FALSE;
    int i, cid, dc_index, failcause;

    assert(ar);

    if(ar->error != 0)
    {
        return;
    }

    int numOfData = object_dataLength(ar->result)/sizeof(RIL_Data_Call_Response_v6);
    cmsLog_debug("numOfdata=%d", numOfData);

    /*
     * Check if we should start or stop polling, by looking for dormant and 
     * active connections
     */
    for(i=0; i<numOfData; i++)
    {
        /* get the i-th RIL_Data_Call_Response from the array */
        new_dataCallState = (RIL_Data_Call_Response_v6 *)(object_getData(ar->result) + 
                                     sizeof(RIL_Data_Call_Response_v6)*i);

        cmsLog_debug("cid=%d, status=%d, type=%s, active=%d", new_dataCallState->cid,
                                                   new_dataCallState->status,
                                                   new_dataCallState->type,
                                                   new_dataCallState->active);
        cmsLog_debug("ifname=%s, adresses=%s, dnses=%s, gateways=%s",
                                                   new_dataCallState->ifname,
                                                   new_dataCallState->addresses,
                                                   new_dataCallState->dnses,
                                                   new_dataCallState->gateways);

        cid = new_dataCallState->cid;
        
        dc_index = find_data_connection_by_cid(cid);
        if(dc_index == -1)
        {
            continue;
        }
        
        cmsLog_debug("found data connection index %d", dc_index);

        if(new_dataCallState->active == DATA_CONNECTION_ACTIVE_PH_LINK_INACTIVE)
        {
            /* Inactive: determine if the connection or apn context should be
             * cleaned up or just a notification should be sent.
             */

            struct internal_message *msg;
            struct object *o;
            struct apn_context *context;

            failcause = new_dataCallState->status;
            cmsLog_debug("failcause is %d", failcause);
            switch(failcause)
            {
                case PDP_FAIL_REGULAR_DEACTIVATION:
                    //send restart radio;
                    cmsLog_debug("error message indicate we should restart radio");
                    break;

                case PDP_FAIL_OPERATOR_BARRED:
                case PDP_FAIL_MISSING_UKNOWN_APN:
                case PDP_FAIL_UNKNOWN_PDP_ADDRESS_TYPE:
                case PDP_FAIL_USER_AUTHENTICATION:
                case PDP_FAIL_ACTIVATION_REJECT_GGSN:
                case PDP_FAIL_SERVICE_OPTION_NOT_SUPPORTED:
                case PDP_FAIL_SERVICE_OPTION_NOT_SUBSCRIBED:
                case PDP_FAIL_NSAPI_IN_USE:
                case PDP_FAIL_ONLY_IPV4_ALLOWED:
                case PDP_FAIL_ONLY_IPV6_ALLOWED:
                case PDP_FAIL_PROTOCOL_ERRORS:
                case PDP_FAIL_RADIO_POWER_OFF:
                case PDP_FAIL_TETHERED_CALL_ACTIVE:
                    /* permernant failure, don't retry and clean up data 
                     * connection. */
                    cmsLog_debug("permernant failure, don't retry and clean up");

                    context = dc_tracker_get_context(dc_index);
                    if(context != NULL)
                    {
                        tear_down_data_connection(context);
                    }
                    break;

                default:
                    cmsLog_debug("other failure: retry connection");

                    /* other failure causes: we should retry the connection */
                    msg = internal_message_obtain(EVENT_DC_LOST_CONNECTION,
                                                  DATA_CONNECTION);
                    o = object_obtain();
                    object_setData(o, &dc_index, sizeof(int));
                    internal_message_setObject(msg, o);
                    o = NULL;

                    data_connection_process(msg);
                    internal_message_freeObject(msg);
                    free(msg);
                    msg = NULL;

                    break;
            }     
        }
        else
        {
            /* 
             * It's active so update the data connection link properties via cms. 
             */
            cms_access_notify_dataStatusChanged(new_dataCallState); 

        }


        
        if(new_dataCallState->active == DATA_CONNECTION_ACTIVE_PH_LINK_UP)
        {
            isAnyDataCallActive = TRUE;
        }

        if(new_dataCallState->active == DATA_CONNECTION_ACTIVE_PH_LINK_DOWN)
        {
            isAnyDataCallDormant = TRUE;
        }
        
    }

    cmsLog_debug("isAnyDataCallDormant=%s, isAnyDataCallActive=%s",
                    isAnyDataCallDormant?"TRUE":"FALSE",
                    isAnyDataCallActive?"TRUE":"FALSE");

    if(isAnyDataCallDormant && !isAnyDataCallActive)
    {
        mActivity = DC_ACTIVITY_DORMANT;
        //stopNetStatePoll();
    }
    else
    {
        mActivity = DC_ACTIVITY_NONE;
        //if(isAnyDataCallActive) startNetStatPoll();
    }    


}

static void on_try_setup_data(int dc_index)
{
    struct apn_context *context;
	
    cmsLog_debug("dc_index = %d", dc_index);
	
    context = dc_tracker_get_context(dc_index);	
		
    if(context == NULL)
    {
        cmsLog_error("Couldn't find apn context for index %d", dc_index);
        return;
    }
	
    try_setup_data(context);
}

/* Called by data_connection to notify dc_tracker about state change. */
void dc_tracker_state_change(int dc_index, DataConnectionState new_state)
{
    struct apn_context *context;
    context = dc_tracker_get_context(dc_index);
    if(context == NULL)
    {
        cmsLog_error("Couldn't find the coresponding apn context!");
        return;
    }
	
   cmsLog_debug("%s ==> %s", state_to_string(context->mState), state_to_string(new_state));
   
    /* loop through apn contexts to find the corresponding one and update its
     * status, based on state change of its data connection
    */
    switch(new_state)
    {
        case DcInactiveState:
        case DcDisconnectingErrorCreatingConnection:
           if(!context->mDataEnabled)
           {
              context->mState = DC_STATE_IDLE;
           }
           else
           {
              struct internal_message *msg;
	
              switch_to_next_candidate_apn(context);           
   
              msg = internal_message_obtain_with_data(EVENT_TRY_SETUP_DATA, DC_TRACKER, 
                                        &(context->connection_index), sizeof(context->connection_index));
			  
              if(timer_manager_add(2000, msg) == TRUE)
              {
                 context->mState = DC_STATE_SCANNING;
              }
              else
              {
                 cmsLog_error("Failed to add timer for apn context scan.");
                 internal_message_free_and_null_ptr(&msg);
                 context->mState = DC_STATE_IDLE;
              }
            }
 			
            break;

        case DcActivatingState:
            context->mState = DC_STATE_CONNECTING;
            break;

        case DcActiveState:
            context->mState = DC_STATE_CONNECTED;

            /* if auto selected apn then set preferred-apn.xml*/
            if (context->pApnSetting != &context->mApnSetting)
            {
               rutCellular_setPrefdApn(context->pApnSetting->id, NULL);
            }
			
            break;
            
        case DcRetryingState:
            context->mState = DC_STATE_RETRYING;  
            break;

        case DcDisconnectingState:
            context->mState = DC_STATE_DISCONNECTING;
            break;

        default:
            break;  

    } 
    cmsLog_notice("new apn context ifname: %s, state: %s",
                                      context->ifname, 
                                      state_to_string(context->mState));
}

static struct apn_context * dc_tracker_findFreeContext()
{
    int i;
    struct apn_context *a;

    for(i=0; i<MAX_APN_CONTEXT; i++)
    {
        a = &apn_context_table[i];
        if((a->mDataEnabled == FALSE) && (a->mState == DC_STATE_IDLE))
        {
            reset_apn_context(a);
            cmsLog_debug("get free context %d", i);
            return a;
        }
        
    }
    return NULL;
}

static struct apn_context * dc_tracker_findContextByIfname(const char* ifname)
{
    int i;
    struct apn_context *a;

    for(i=0; i<MAX_APN_CONTEXT; i++)
    {
        a = &apn_context_table[i];
        if(a->mDataEnabled == TRUE)
        {
            cmsLog_debug("index=%d, ifname=%s", i, a->ifname);
            if(cmsUtl_strcmp(a->ifname, ifname) == 0)
            {
                return a;
            }
        }
        
    }
    return NULL;
}

static struct apn_context * dc_tracker_get_context(int dc_index)
{
    int i;
    for(i=0; i<MAX_APN_CONTEXT; i++)
    {
        if(dc_index == apn_context_table[i].connection_index)
        {
            return &apn_context_table[i];
        }
    }
    return NULL;
}

/* 
 * Handle the apn set requests from CMS. Essentially this operation means we 
 * would tearDown or setup data call without asking any questions.
 */
static void process_enable_apn(DevCellularAccessPointMsgBody *cms_apn)
{
    struct apn_context* context;
    
    assert(cms_apn);

    cmsLog_debug("apn=%s, ifname=%s, enable=%d", cms_apn->APN,
                                                 cms_apn->ifname, 
                                                 cms_apn->enable);
    
    if(cms_apn->enable == FALSE)
    {
        context = dc_tracker_findContextByIfname(cms_apn->ifname);
        if(context != NULL)
        {
            /* 
             * we have found the apn context coresponding to the ifname. 
             * disable it and tear down the data call for now.
             */ 
           cmsLog_debug("disable apn context for %s", context->ifname); 
           context->mDataEnabled = FALSE;
           tear_down_data_connection(context);
        }
        else
        {
            cmsLog_error("Failed to find apn context for %s", cms_apn->ifname);
        }
    }
    else
    {
        if ((context = dc_tracker_findContextByIfname(cms_apn->ifname)) == NULL)
        {
           context = dc_tracker_findFreeContext();
        }
        else
        {
            cmsLog_error("ifname %s already exist for apn %s, ignore request", cms_apn->ifname,
                         context->pApnSetting ? context->pApnSetting->apn : "unknown");
            return;
        }
        
        if(context != NULL)
        {
            CMSMEM_REPLACE_STRING(context->ifname, cms_apn->ifname);
            context->mDataEnabled = TRUE;
	              
            cmsLog_debug("try enable apn context for %s", cms_apn->ifname);

            if (context->ifname && cmsUtl_strcmp(cms_apn->APN, CELLULAR_AUTO_SEL_APN_NAME))
            {
               struct apn_setting *as = &(context->mApnSetting);
   			
               /* Need to copy over the apn setting as the one contained in cms
                * msg payload would be released after this round of processing.
                */
              CMSMEM_REPLACE_STRING(as->apn, cms_apn->APN);
			   
               switch(cms_apn->ipProtocalMode)
               {
                   case ENBL_IPV4_ONLY:
                       as->protocol = "IP";
                       break;
                  
                   case ENBL_IPV4_IPV6:
                       as->protocol = "IPV4V6";
                       break;
                 
                   case ENBL_IPV6_ONLY:
                       as->protocol = "IPV6";
                       break;
   
                   default:
                       as->protocol = NULL;
                       break;
   
               } 
               as->authType = 0;
               as->carrierEnabled = TRUE;   //??
               
               context->pApnSetting = as;
            }
            else
            {
               /*Sarah: hard code to default, may need extend cms msg later*/            
               context->mApnType = APN_TYPE_DEFAULT;
            }
			
            if(mRecordLoaded == TRUE)
            {
                setup_data_on_connectable_apns();
            }
            else
            {
                cmsLog_debug("mRecordLoaded is false, pending request");
            }
        }
    }
}

static const char* state_to_string(DcState state)
{
    switch(state)
    {
        case DC_STATE_IDLE: return "DC_STATE_IDLE";
        case DC_STATE_CONNECTING: return "DC_STATE_CONNECTING";
        case DC_STATE_SCANNING: return "DC_STATE_SCANNING";
        case DC_STATE_CONNECTED: return "DC_STATE_CONNECTED";
        case DC_STATE_DISCONNECTING: return "DC_STATE_DISCONNECTING";
        case DC_STATE_FAILED: return "DC_STATE_FAILED";
        case DC_STATE_RETRYING: return "DC_STATE_RETRYING";
        default: return "Unknown";
    }
}
