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


#include "cms.h"
#include "cms_eid.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"
#include "cms_log.h"
#include "service_state_tracker.h"
#include "cellular_internal.h"
#include "cellular_msg.h"
#include "cms_access.h"
#include "ril_access.h"

#include <stdlib.h>
#include <assert.h>

static UBOOL8 mDesiredPowerState;
static struct service_state mNewSS;
static struct service_state mSS;
/*Sarah: extend to RIL_SignalStrength_v6 for more details*/
static SINT32 mSignalStrength;
static UBOOL8 mGotCountryCode;
static int mRegistrationState = -1;
static UBOOL8 mCdmaRoaming = FALSE;
static UBOOL8 mIsInPrl;
static UBOOL8 mDefaultRoamingIndicator;
static int mRoamingIndicator;
static const char* mRegistrationDeniedReason; 
static UBOOL8 mIsSubscriptionFromRuim = TRUE;
static UBOOL8 mVoiceCapable = FALSE;
static struct service_state_polling_context mPollingContext;
static char* mImei = NULL;
static char* mImeiSv = NULL;
static char* mEsn = NULL;
static char* mMeid = NULL;

/** Reason for registration denial. */                                      
static const char REGISTRATION_DENIED_GEN[]  = "General";         
static const char REGISTRATION_DENIED_AUTH[] = "Authentication Failure";
               

static void set_power_state_to_desired();
static void poll_state();
static void poll_state_done();
static void handle_poll_state_result(int what, AsyncResult *ar);
static int reg_code_to_service_state(enum RIL_REG_STATE code);
static void hangup_and_power_off();
static void power_off_radio_safely();
static UBOOL8 radio_state_isAvailable(RadioState state);
static void on_radio_state_changed();
static void on_get_device_identity_done(struct internal_message* msg);
static void get_available_networks(Object* o);
static void on_get_available_networks_done(AsyncResult* ar);
static void on_signal_strength_result(AsyncResult* ar);

static void set_state_out_of_service(struct service_state *s);
static void set_state_off(struct service_state *s); 
static void set_null_state(struct service_state *s, int state);
static void reset_service_state(struct service_state *s);

static UBOOL8 reg_code_is_roaming(int code);
static void use_data_reg_state_for_data_only_devices(); 
static UBOOL8 service_state_compare(struct service_state* a, 
                                 struct service_state *b);
static void cancel_poll_state();
static void dump_service_state(struct service_state *s);
static void dump_signal_strength(const RIL_SignalStrength_v6 *mSignalStrength);
static void free_string(char** s);
static void free_strings(char** s, int numOfStrings);


void service_state_tracker_init()
{
    /* *
     * CellularInterface data structure and related MDM object are to be
     * defined
     * */

    /*
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret;
    CellularInterface obj;

    if ((ret = cmsLck_acquireLockWithTimeout(CELLULAR_APP_TIMEOUT)) 
            != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get lock, ret=%d", ret);
        return;
    }

    
    if ((ret = cmsObj_get(MDMOID_CELLULAR_INTERFACE, &iidStack, 0, 
                    (void **) &obj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("get of CELLULAR_INTERFACE object failed, ret=%d", ret);
    }
    else
    {
	    mDesiredPowerState = obj.enabled; 
        cmsObj_free((void **) &obj);
    }

    cmsLck_releaseLock();
    */
    mDesiredPowerState = TRUE;
    mGotCountryCode = FALSE;
    memset(&mPollingContext, 0x00, sizeof(struct service_state_polling_context));
    memset(&mNewSS, 0x00, sizeof(struct service_state));
    memset(&mSS, 0x00, sizeof(struct service_state));
    mSignalStrength = SIGNAL_STRENGTH_INVALID;
}

void service_state_tracker_process(struct internal_message *msg)
{
    AsyncResult *ar;
    Object *o;
    UBOOL8 enable;

    switch(msg->what)
    {
        case EVENT_RADIO_STATE_CHANGED:
            on_radio_state_changed();
            set_power_state_to_desired();
            poll_state();
            break;

        case EVENT_SIGNAL_STRENGTH_UPDATE:
            o = internal_message_getObject(msg);
            if(o != NULL)
            {
                ar = (AsyncResult *)object_getData(o);
                on_signal_strength_result(ar);
            }
            break;

        case EVENT_NETWORK_STATE_CHANGED:
            poll_state();
            break;

        case EVENT_GET_DEVICE_IDENTITY_DONE:
            on_get_device_identity_done(msg);
            break;

        case EVENT_POLL_STATE_OPERATOR:
        case EVENT_POLL_STATE_REGISTRATION:
        case EVENT_POLL_STATE_GPRS:
            o = internal_message_getObject(msg);
            if(o != NULL)
            {
                ar = (AsyncResult *)object_getData(o);
                handle_poll_state_result(msg->what, ar);
            }
            break;

        case EVENT_RECORD_LOADED:
            poll_state();
            break;

        case EVENT_CMS_ENABLE_INTERFACE:
            o = internal_message_getObject(msg);
            if(o != NULL)
            {
                enable = *(UBOOL8 *)object_getData(o); 
                cmsLog_debug("cms set radio to be %s", enable?"on":"off");
                if(mDesiredPowerState != enable)
                {
                    mDesiredPowerState = enable;
                    set_power_state_to_desired();
                }
            }
            break;

        case EVENT_GET_AVAILABLE_NETWORKS:
            o = internal_message_getObject(msg);
            get_available_networks(o);
            internal_message_setObject(msg, NULL);
            break;

        case EVENT_GET_AVAILABLE_NETWORKS_DONE:
            o = internal_message_getObject(msg);
            if(o != NULL)
            {
                ar = (AsyncResult *)object_getData(o);
                on_get_available_networks_done(ar);
            }
            break;

        default:
            cmsLog_notice("unhandled message!");
            break;

    }
}

/* Note: We assume that the Object returned in the result contains Strings
 * pointed to by pointers. Thus they need special care to release the memory
 * allocated in heap.
 * */
static void handle_poll_state_result(int what, AsyncResult *ar)
{
    Object *o = ar->result;
    struct object *userObj = ar->userObj;
    int context;
    int numOfStrings;
    char **s;

    if(ar->error != 0)
    {
        cancel_poll_state();
    }
    else
    {
        int regState;
        int dataRadioTechnology;
        int dataRegState;
        char **opNames;
        char **states;
        int registrationState = 4;     //[0] registrationState              
        int radioTechnology = -1;      //[3] radioTechnology                
        //int baseStationId = -1;        //[4] baseStationId                  
        int cssIndicator = 0;          //[7] init with 0, because it is treated as a UBOOL8ean
        int systemId = 0;              //[8] systemId                       
        int networkId = 0;             //[9] networkId                      
        int roamingIndicator = -1;     //[10] Roaming indicator             
        int systemIsInPrl = 0;         //[11] Indicates if current system is in PRL
        int defaultRoamingIndicator = 0;  //[12] Is default roaming indicator from PRL
        int reasonForDenial = 0;       //[13] Denial reason if registrationState = 3

        assert(o);
        assert(userObj);

        context = *(int *)object_getData(userObj);
        cmsLog_debug("response's context=%d", context);
        cmsLog_debug("mPollingContext.context=%d", mPollingContext.context);

        numOfStrings = object_dataLength(o)/sizeof(char *);
        s = (char **)object_getData(o);
        if(context != mPollingContext.context)
        {
            /* The response are out of polling context, discard it */
            free_strings(s, numOfStrings);    
            return;
        }

        switch(what)
        {
            case EVENT_POLL_STATE_GPRS:
                regState = RIL_REG_STATE_UNKNOWN;
                dataRadioTechnology = 0;
                
                if(object_containsData(o))
                {
                    
                    regState = atoi(s[0]);
                    if(numOfStrings >= 4 && s[3] != NULL)
                    {
                        dataRadioTechnology = atoi(s[3]);
                    }
                    
                }

                dataRegState = reg_code_to_service_state(regState);
                mNewSS.mDataRegState = dataRegState;
                mNewSS.mRilDataRadioTechnology = dataRadioTechnology;
                cmsLog_debug("new: mDataRegState=%d, mRilDataRadioTechnology=%d",
                              mNewSS.mDataRegState, mNewSS.mRilDataRadioTechnology);

                break;

            case EVENT_POLL_STATE_REGISTRATION:
                states = (char **)object_getData(o);
                numOfStrings = object_dataLength(o)/sizeof(char *);
                
                if (numOfStrings >= 14) 
                {                                          
                    if (states[0] != NULL) 
                    { 
                        registrationState = atoi(states[0]);    
                    }                                                           
                    if (states[3] != NULL) 
                    {                                    
                        radioTechnology = atoi(states[3]);          
                    }                                                           
                    if (states[4] != NULL) 
                    {                                    
                        //baseStationId = atoi(states[4]);            
                    }                                                           
                    if (states[5] != NULL) 
                    {                                    
                        //baseStationLatitude = atoi(states[5]);  
                    }                                                       
                    if (states[6] != NULL) 
                    {                                
                        //baseStationLongitude = atoi(states[6]); 
                    }                                                       
                    // Some carriers only return lat-lngs of 0,0            
                    /*
                    if (baseStationLatitude == 0 && baseStationLongitude == 0) 
                    {
                        baseStationLatitude  = CdmaCellLocation.INVALID_LAT_LONG;
                        baseStationLongitude = CdmaCellLocation.INVALID_LAT_LONG;
                    } 
                    */                                                      
                    if (states[7] != NULL) 
                    {                                
                        cssIndicator = atoi(states[7]);         
                    }                                                       
                    if (states[8] != NULL) 
                    {                                
                        systemId = atoi(states[8]);             
                    }                                                       
                    if (states[9] != NULL) 
                    {                                
                        networkId = atoi(states[9]);            
                    }                                                       
                    if (states[10] != NULL) 
                    {                               
                        roamingIndicator = atoi(states[10]);    
                    }                          
                    if (states[11] != NULL) 
                    { 
                        systemIsInPrl = atoi(states[11]);       
                    }                                                       
                    if (states[12] != NULL) 
                    {                               
                        defaultRoamingIndicator = atoi(states[12]);
                    }                                                       
                    if (states[13] != NULL) 
                    {                               
                        reasonForDenial = atoi(states[13]);     
                    }                         
                }    
                else
                {
                    cmsLog_notice("Warning! Wrong number of parameters returned from"); 
                    cmsLog_notice("RIL_REQUEST_VOICE_REGISTRATION_STATE: expected 14 or more");
                    cmsLog_notice("strings and got %d strings.", numOfStrings);
                }
                mRegistrationState = registrationState;                         
                // When registration state is roaming and TSB58                 
                // roaming indicator is not in the carrier-specified            
                // list of ERIs for home system, mCdmaRoaming is true.          
                mCdmaRoaming = reg_code_is_roaming(registrationState);
                        //&& !isRoamIndForHomeSystem(states[10]);
                mNewSS.mServiceState = reg_code_to_service_state(registrationState);     
                                                                            
                mNewSS.mRilVoiceRadioTechnology = radioTechnology;             
                                                                            
                mNewSS.mCssIndicator = cssIndicator;                           
                mNewSS.mSystemId = systemId;
                mNewSS.mNetworkId = networkId;              
                mRoamingIndicator = roamingIndicator;                           
                mIsInPrl = (systemIsInPrl == 0) ? FALSE : TRUE;                 
                mDefaultRoamingIndicator = defaultRoamingIndicator;             
                                                                            
                if (reasonForDenial == 0) 
                { 
                    mRegistrationDeniedReason = REGISTRATION_DENIED_GEN;
                } 
                else if (reasonForDenial == 1) 
                {                              
                    mRegistrationDeniedReason = REGISTRATION_DENIED_AUTH;
                } 
                else 
                {                                                        
                    mRegistrationDeniedReason = "";                             
                }                                                               
                                                                            
                if (mRegistrationState == 3) 
                {                                  
                    cmsLog_notice("Registration denied:%s", mRegistrationDeniedReason);
                }                                                             
                
                break;

            case EVENT_POLL_STATE_OPERATOR:
                opNames = (char **)object_getData(o);                         
                numOfStrings = object_dataLength(o)/sizeof(char *);

                cmsLog_debug("operators %d strings", numOfStrings);

                if (opNames != NULL && numOfStrings >= 3) 
                {                   
                    // If the NUMERIC field isn't valid use PROPERTY_CDMA_HOME_OPERATOR_NUMERIC
                    if ((opNames[2] == NULL) || (strlen(opNames[2]) < 5)       
                            || !strcmp("00000", opNames[2]))
                    {                  
                        /* opNames[2] = SystemProperties.get(                      
                                CDMAPhone.PROPERTY_CDMA_HOME_OPERATOR_NUMERIC, "00000");
                        */
                    }                                                           

                    free_string(&mNewSS.mOperatorAlphaLong);
                    free_string(&mNewSS.mOperatorAlphaShort);
                    free_string(&mNewSS.mOperatorNumeric);
                    free_string(&mNewSS.mOperatorState);
                                        
                    if (!mIsSubscriptionFromRuim) 
                    {                             
                        // In CDMA in case on NV, the ss.mOperatorAlphaLong is set later with the
                        // ERI text, so here it is ignored what is coming from the modem.
                        mNewSS.mOperatorAlphaLong = NULL;
                        mNewSS.mOperatorAlphaShort = opNames[1];
                        opNames[1] = NULL;

                        mNewSS.mOperatorNumeric = opNames[2];
                        opNames[2] = NULL;
                    } 
                    else
                    {                                                    
                        mNewSS.mOperatorAlphaLong = opNames[0];
                        opNames[0] = NULL;

                        mNewSS.mOperatorAlphaShort = opNames[1];
                        opNames[1] = NULL;

                        mNewSS.mOperatorNumeric = opNames[2];
                        opNames[2] = NULL;
                    }

                    if(numOfStrings > 3)
                    {
                        mNewSS.mOperatorState = opNames[3];
                        opNames[3] = NULL;
                    }

                } 
                else 
                {                                                        
                    printf("EVENT_POLL_STATE_OPERATOR_CDMA: error parsing opNames\n");
                }              
                break;

            default:
                break;
           }

        mPollingContext.counter--;
        
        cmsLog_debug("mPollingContext: context=%d, counter=%d", 
                                               mPollingContext.context,
                                               mPollingContext.counter);
        if(mPollingContext.counter == 0)
        {
            poll_state_done();
        }

        /* The object data contains pointers which points to a string in heap.
         * Need to free them to prevent memory leak, as the overall free 
         * mechanism would not try to do so. */
        free_strings(s, numOfStrings);    

    }
}


static UBOOL8 radio_state_isAvailable(RadioState state)
{
    if(state != RADIO_UNAVAILABLE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static void on_radio_state_changed()
{
    RadioState current;

    current = ril_access_get_radio_state();

    if(radio_state_isAvailable(current))
    {
        struct internal_message* msg;
        msg = internal_message_obtain(EVENT_GET_DEVICE_IDENTITY_DONE, 
                                      SERVICE_STATE_TRACKER);
        ril_access_get_device_identity(msg);
    }
    
}

/* Validate the individual signal strength fields as per the range specified in
 * ril.h. Set to invalid any field that is not in the valid range Cdma, evdo, 
 * lte rsrp & rsrq values are sign converted when received from ril interfaces.
 * */
static void signal_strength_validate(RIL_SignalStrength_v6 *signalStrength)
{

   /*Sarah: extend to RIL_SignalStrength_v6 for more details*/
   if (mSS.mRilDataRadioTechnology == RADIO_TECH_LTE)
   {
      mSignalStrength = -signalStrength->LTE_SignalStrength.rsrp;
   }
   else if (signalStrength->GW_SignalStrength.signalStrength < 0 || signalStrength->GW_SignalStrength.signalStrength > 31)
   {
      mSignalStrength = SIGNAL_STRENGTH_INVALID;
   }
   else 
   {
      mSignalStrength = (signalStrength->GW_SignalStrength.signalStrength) * 2 - 113;
   }

#if 0
    // TS 27.007 8.5
    ss->mGsmSignalStrength = (ss->mGsmSignalStrength >= 0) ? 
                                              ss->mGsmSignalStrength : 99;

    ss->mCdmaDbm = (ss->mCdmaDbm > 0) ? -(ss->mCdmaDbm) : -120;
    ss->mCdmaEcio = (ss->mCdmaEcio > 0) ? -(ss->mCdmaEcio) : -160;

    ss->mEvdoDbm = (ss->mEvdoDbm > 0) ? -(ss->mEvdoDbm) : -120;
    ss->mEvdoEcio = (ss->mEvdoEcio >= 0) ? -(ss->mEvdoEcio) : -1;
    ss->mEvdoSnr = ((ss->mEvdoSnr>0) && (ss->mEvdoSnr<=8)) ? ss->mEvdoSnr : -1;

    //TS 36.214 Physical Layer Section 5.1.3, TS 36.331 RRC
    ss->mLteSignalStrength = (ss->mLteSignalStrength >= 0) ? ss->mLteSignalStrength : 9;

    ss->mLteRsrp = ((ss->mLteRsrp >= 44) && (ss->mLteRsrp <= 140)) ?
                                     -(ss->mLteRsrp) : SIGNAL_STRENGTH_INVALID;    

    ss->mLteRsrq = ((ss->mLteRsrq >= 3) && (ss->mLteRsrq <= 20)) ? 
                                     -(ss->mLteRsrq) : SIGNAL_STRENGTH_INVALID;

    ss->mLteRssnr = ((ss->mLteRssnr >= -200) && (ss->mLteRssnr <= 300)) ?
                                     ss->mLteRssnr : SIGNAL_STRENGTH_INVALID;
    //Cqi no Change
#endif    
}

static void dump_signal_strength(const RIL_SignalStrength_v6 *signalStrength)
{
   assert(signalStrength);
   
   cmsLog_debug("GW_SignalStrength.signalStrength = %d, LTE_SignalStrength.rsrp = %d", 
                signalStrength->GW_SignalStrength.signalStrength, 
                signalStrength->LTE_SignalStrength.rsrp);

#if 0	
    cmsLog_debug("GsmSignalStrength=%d", ss->mGsmSignalStrength);
    cmsLog_debug("CdmaDbm=%d", ss->mCdmaDbm);
    cmsLog_debug("CdmaEcio=%d", ss->mCdmaEcio);
    cmsLog_debug("EvdoDbm=%d", ss->mEvdoDbm);
    cmsLog_debug("EvdoEcio=%d", ss->mEvdoEcio);
    cmsLog_debug("EvdoSnr=%d", ss->mEvdoSnr);
    cmsLog_debug("LteSignalStrength=%d", ss->mLteSignalStrength);
    cmsLog_debug("LteRsrp=%d", ss->mLteRsrp);
    cmsLog_debug("LteRsrq=%d", ss->mLteRsrq);
    cmsLog_debug("LteRssnr=%d", ss->mLteRssnr);
    cmsLog_debug("end dump.");
#endif	
}

static void on_signal_strength_result(AsyncResult* ar)
{
    RIL_SignalStrength_v6* signalStrength;
    Object *o;

    assert(ar);
    
    if(ar->error != 0)
    {
        cmsLog_error("error=%d", ar->error);
        return;
    }

    o = ar->result;
    signalStrength = (RIL_SignalStrength_v6 *)object_getData(o);
    assert(signalStrength);

    signal_strength_validate(signalStrength);
    dump_signal_strength(signalStrength);
}

static void on_get_device_identity_done(struct internal_message* msg)
{
    AsyncResult *ar;
    Object *o;
    Object *result;
    char **s = NULL;
    int numOfStrings = 0;

    assert(msg);
    o = internal_message_getObject(msg);
    assert(o);
    ar = (AsyncResult *)object_getData(o);

    if(ar->error != 0)
    {
        cmsLog_error("error on get_device_identity!");
        return;
    }
    else
    {
        result = ar->result;
        assert(result);
        s = (char **)object_getData(result);

        if(object_containsData(result))
        {
            numOfStrings = object_dataLength(result)/sizeof(char *);

            if(numOfStrings == 4)
            {
                /* copy over imei, imeisv, etc.into memory for state tracking.*/
                free_string(&mImei);
                mImei = s[0];
                s[0] = NULL;
                cmsLog_debug("mImei=%s", mImei);

                free_string(&mImeiSv);
                mImeiSv = s[1];
                s[1] = NULL;
                cmsLog_debug("mImeiSv=%s", mImeiSv);

                free_string(&mEsn);
                mEsn = s[2];
                s[2] = NULL;
                cmsLog_debug("mEsn=%s", mEsn);

                free_string(&mMeid);
                mMeid = s[3];
                s[3] = NULL;
                cmsLog_debug("mMeid=%s", mMeid);
            }
            else
            {   
                cmsLog_error("wrong string numbers! %d", numOfStrings);
            }

        }
    }

    if(s != NULL)
    {
        free_strings(s, numOfStrings);
    }
}

static void set_power_state_to_desired()
{
    // If we want it on and it's off, turn it on                            
    if (mDesiredPowerState && 
            ril_access_get_radio_state() == RADIO_OFF)
    { 
        ril_access_set_radio_power(TRUE, NULL);                                      
    } 
    else if (!mDesiredPowerState && ril_access_get_radio_state() == RADIO_ON) 
    {                                                                                 
        // If it's on and available and we want it off gracefully           
        power_off_radio_safely();                                     
    } // Otherwise, we're in the desired state                              
}

static void power_off_radio_safely()
{
    //dc_tracker_cleanUpAllConnections(REASON_RADIO_TURNED_OFF);
    hangup_and_power_off();
}

static void hangup_and_power_off()
{
    //hang up all active voice calls;
    ril_access_set_radio_power(FALSE, NULL);
}

/**                                                                         
 * A complete "service state" from our perspective is                       
 * composed of a handful of separate requests to the radio.                 
 *                                                                          
 * We make all of these requests at once, but then abandon them             
 * and start over again if the radio notifies us that some                  
 * event has changed                                                        
 */                                                                         
static void poll_state() 
{                                     
    /* free the old polling context and allocate a new one would effectively 
     * cause service state tracker to ignore the stale polling response.
     */
    struct internal_message *msg = NULL;
    struct object *o = NULL;

    mPollingContext.context++;
    /* Check integer overflow */
    if(mPollingContext.context < 0)
    {
        mPollingContext.context = 0;
    }
    mPollingContext.counter = 0;

    switch (ril_access_get_radio_state()) {                                          
        case RADIO_UNAVAILABLE:                                                 
            set_state_out_of_service(&mNewSS);                                      
            //mNewCellLoc.setStateInvalid();                                      
            //setSignalStrengthDefaultValues();                                   
            mGotCountryCode = FALSE;                                            

            poll_state_done();                                                    
            break;                                                              

        case RADIO_OFF:                                                         
            set_state_off(&mNewSS);                                               
            //mNewCellLoc.setStateInvalid();                                      
            //setSignalStrengthDefaultValues();                                   
            mGotCountryCode = FALSE;                                            

            poll_state_done();                                                    
            break;                                                              

        default:                                                                
            // Issue all poll-related commands at once, then count              
            // down the responses which are allowed to arrive                   
            // out-of-order.          
            // RIL_REQUEST_OPERATOR is necessary for CDMA                       
            cmsLog_debug("RADIO_ON, send out poll related commands.");
            mPollingContext.counter++;
            o = object_obtain();
            object_setData(o, &(mPollingContext.context), sizeof(int));
            msg = internal_message_obtain(EVENT_POLL_STATE_OPERATOR, 
                    SERVICE_STATE_TRACKER);
            internal_message_setObject(msg, o);
            o = NULL;
            ril_access_get_operator(msg);


            // RIL_REQUEST_VOICE_REGISTRATION_STATE is necessary for CDMA       
            mPollingContext.counter++;
            o = object_obtain();
            object_setData(o, &(mPollingContext.context), sizeof(int));
            msg = internal_message_obtain(EVENT_POLL_STATE_REGISTRATION, 
                   SERVICE_STATE_TRACKER);
            internal_message_setObject(msg, o);
            o = NULL;
            ril_access_get_voice_registration_state(msg);
                                                                                
            // RIL_REQUEST_DATA_REGISTRATION_STATE                              
            mPollingContext.counter++;
            msg = internal_message_obtain(EVENT_POLL_STATE_GPRS,   
                                          SERVICE_STATE_TRACKER);
            o = object_obtain();
            object_setData(o, &(mPollingContext.context), sizeof(int));
            internal_message_setObject(msg, o);
            o = NULL;
            ril_access_get_data_registration_state(msg);
            break;                                                              
        }                                                                       
}      

static void poll_state_done() 
{                                            
    struct internal_message *msg = NULL;
                                                                                
    use_data_reg_state_for_data_only_devices();                                    
                                                                                
    UBOOL8 hasRegistered = (mSS.mVoiceRegState != SERVICE_STATE_IN_SERVICE) &&
                           (mNewSS.mVoiceRegState == SERVICE_STATE_IN_SERVICE);          
                                                                                
    //UBOOL8 hasDeregistered = (mSS.mVoiceRegState == SERVICE_STATE_IN_SERVICE) &&
    //                          (mNewSS.mVoiceRegState != SERVICE_STATE_IN_SERVICE);          
                                                                                
    UBOOL8 hasCdmaDataConnectionAttached =
            (mSS.mDataRegState != SERVICE_STATE_IN_SERVICE)              
            && (mNewSS.mDataRegState == SERVICE_STATE_IN_SERVICE);           
                                                                                
    UBOOL8 hasCdmaDataConnectionDetached =                                 
            (mSS.mDataRegState == SERVICE_STATE_IN_SERVICE)              
            && (mNewSS.mDataRegState != SERVICE_STATE_IN_SERVICE);           
                                                                                
    UBOOL8 hasCdmaDataConnectionChanged =                                  
                       (mSS.mDataRegState != mNewSS.mDataRegState);           
                                                                                
    UBOOL8 hasRilVoiceRadioTechnologyChanged =                             
                (mSS.mRilVoiceRadioTechnology != mNewSS.mRilVoiceRadioTechnology);
                                                                                
    UBOOL8 hasRilDataRadioTechnologyChanged =                              
                (mSS.mRilDataRadioTechnology != mNewSS.mRilDataRadioTechnology);
                                                                                
    UBOOL8 hasChanged = service_state_compare(&mSS, &mNewSS);                               
                                                                                
    UBOOL8 hasRoamingOn = !(mSS.mRoaming) && (mNewSS.mRoaming);  
    UBOOL8 hasRoamingOff = (mSS.mRoaming) && !(mNewSS.mRoaming);       
                                                                                
    struct service_state tss;  
    memcpy(&tss, &mSS, sizeof(struct service_state));    
    memcpy(&mSS, &mNewSS, sizeof(struct service_state));                                                           
   
    //free the original allocated strings in struct service_state to avoid
    //memory leak. 
    reset_service_state(&tss);

    // clean slate for next time                                            
    set_state_out_of_service(&mNewSS);                                          
            
    dump_service_state(&mSS);

    if (hasRilVoiceRadioTechnologyChanged) 
    { 
       // updatePhoneObject();                                                
    }                                                                       
                                                                                
    if (hasRilDataRadioTechnologyChanged) 
    { 
     //    mPhone.setSystemProperty(TelephonyProperties.PROPERTY_DATA_NETWORK_TYPE,
    //          ServiceState.rilRadioTechnologyToString(mSS.getRilDataRadioTechnology()));
    }                                                                       
                                                                                
    if (hasRegistered) 
    {     
        //mNetworkAttachedRegistrants.notifyRegistrants();                    
    } 
    if (hasChanged) 
    {                                                       
                                                                                
         //char* operatorNumeric;                                             
                                                                                
         //operatorNumeric = getOperatorNumeric(&mSS);                         
         mGotCountryCode = TRUE;                                         
                                                                                
        /*notify others that the states of service have changed */        
     }                                                                       
                                                                                
     if (hasCdmaDataConnectionAttached) 
     {                                    
         cmsLog_debug("data connection attached!");
         msg = internal_message_obtain(EVENT_DATA_CONNECTION_ATTACHED, 
                                       DC_TRACKER);
         cellular_internal_process(msg);
         internal_message_freeObject(msg);
         free(msg);
         msg = NULL;
     }                                                                       
                                                                                
     if (hasCdmaDataConnectionDetached) 
     {                                    
         //mDetachedRegistrants.notifyRegistrants();                           
     }                                                                       
                                                                                
     if (hasCdmaDataConnectionChanged || hasRilDataRadioTechnologyChanged) 
     { 
         //notifyDataRegStateRilRadioTechnologyChanged();                      
         // mPhone.notifyDataConnection(NULL);                                  
     }    
     if (hasRoamingOn) 
     {                                                     
         //mRoamingOnRegistrants.notifyRegistrants();                          
     }                                                                       
                                                                                
     if (hasRoamingOff) 
     {                                                    
         //mRoamingOffRegistrants.notifyRegistrants();                         
     }                                                                       
                                                                                
     /*if (hasLocationChanged) 
     {                                               
         //mPhone.notifyLocationChanged();                                     
     } 
    */     
     
}                                                                           
                                                                                                  
static void set_state_out_of_service(struct service_state *s)
{
    set_null_state(s, SERVICE_STATE_OUT_OF_SERVICE);
}

static void set_state_off(struct service_state *s) 
{                                                 
    set_null_state(s, SERVICE_STATE_POWER_OFF);                                          
}  

static void reset_service_state(struct service_state *s)
{
    assert(s);

    free_string(&s->mOperatorAlphaLong);
    free_string(&s->mOperatorAlphaShort);
    free_string(&s->mOperatorNumeric);
    free_string(&s->mOperatorState);

    set_state_out_of_service(s);
}

static void set_null_state(struct service_state *s, int state)
{
     s->mVoiceRegState = state;                                                 
     s->mDataRegState = state;                                                  
     s->mRoaming = FALSE;                                                       
     s->mOperatorAlphaLong = NULL;                                              
     s->mOperatorAlphaShort = NULL;                                             
     s->mOperatorNumeric = NULL;                                                
     s->mOperatorState = NULL;
     s->mIsManualNetworkSelection = FALSE;                                      
     s->mRilVoiceRadioTechnology = 0;                                           
     s->mRilDataRadioTechnology = 0;                                            
     s->mCssIndicator = FALSE;                                                  
     s->mNetworkId = -1;                                                        
     s->mSystemId = -1;                                                         
     s->mCdmaRoamingIndicator = -1;                                             
     s->mCdmaDefaultRoamingIndicator = -1;                                      
     s->mCdmaEriIconIndex = -1;                                                 
     s->mCdmaEriIconMode = -1;                                                  
     s->mIsEmergencyOnly = FALSE;     
}



/** code is registration state from TS 27.007 7.2 */
static int reg_code_to_service_state(enum RIL_REG_STATE code)
{                                           
    switch (code) 
    {                                                         
       case RIL_REG_STATE_NOT_REG: // Not searching and not registered
       case RIL_REG_STATE_NOT_REG_EMERGENCY_CALL_ENABLED:
          return SERVICE_STATE_OUT_OF_SERVICE;                           

       case RIL_REG_STATE_HOME:
          return SERVICE_STATE_IN_SERVICE;                               

       case RIL_REG_STATE_SEARCHING: // 2 is "searching", fall through
       case RIL_REG_STATE_SEARCHING_EMERGENCY_CALL_ENABLED:
       case RIL_REG_STATE_DENIED: // 3 is "registration denied", fall through
       case RIL_REG_STATE_DENIED_EMERGENCY_CALL_ENABLED:
       case RIL_REG_STATE_UNKNOWN: // 4 is "unknown", not valid in current baseband
       case RIL_REG_STATE_UNKNOWN_EMERGENCY_CALL_ENABLED:
          return SERVICE_STATE_OUT_OF_SERVICE;                           

       case RIL_REG_STATE_ROAMING:// 5 is "Registered, roaming"
          return SERVICE_STATE_IN_SERVICE;                               
                                                                                                                                                 
       default:                                                                
          cmsLog_error("reg_code_to_service_state: unexpected service state:%d", code);    
          return SERVICE_STATE_OUT_OF_SERVICE;                               
   }                                                                       
}


/**                                                                         
 * code is registration state 0-5 from TS 27.007 7.2                        
 * returns true if registered roam, false otherwise                         
 */                                                                         
static UBOOL8 reg_code_is_roaming(int code) 
{                                               
    // 5 is  "in service -- roam"                                           
    return 5 == code;                                                       
}

/**                                                                         
 * Some operators have been known to report registration failure            
 * data only devices, to fix that use DataRegState.                         
 */                                                                         
static void use_data_reg_state_for_data_only_devices() 
{                        
    if (mVoiceCapable == FALSE) 
    {                                           
        cmsLog_debug("DataRegState=%d\n", mNewSS.mDataRegState);        

        // TODO: Consider not lying and instead have callers know the difference. 
        mNewSS.mVoiceRegState = mNewSS.mDataRegState;                  
    }                       
}                               

static UBOOL8 service_state_compare(struct service_state* a, 
                                 struct service_state *b)
{
    return memcmp(a, b, sizeof(struct service_state));
}

static void cancel_poll_state()
{
    memset(&mPollingContext, 0x0, sizeof(struct service_state_polling_context));
}

static void dump_service_state(struct service_state *s)
{
    cmsLog_debug("mServiceState=%d", s->mServiceState);
    cmsLog_debug("mVoiceRegState=%d", s->mVoiceRegState);
    cmsLog_debug("mDataRegState=%d", s->mDataRegState);
    cmsLog_debug("mRoaming=%s", s->mRoaming?"TRUE":"FALSE");
    cmsLog_debug("mOperatorAlphaLong=%s", s->mOperatorAlphaLong);
    cmsLog_debug("mOperatorAlphaShort=%s", s->mOperatorAlphaShort);
    cmsLog_debug("mOperatorNumeric=%s", s->mOperatorNumeric);
    cmsLog_debug("mOperatorState=%s", s->mOperatorState);
    cmsLog_debug("mIsManualNetworkSelection=%s", s->mIsManualNetworkSelection?
                                                   "TURE":"FALSE");
    cmsLog_debug("mRilVoiceRadioTechnology=%d", s->mRilVoiceRadioTechnology);
    cmsLog_debug("mRilDataRadioTechnology=%d", s->mRilDataRadioTechnology);
    cmsLog_debug("mCssIndicator=%s", s->mCssIndicator?"TRUE":"FALSE");
    cmsLog_debug("mNetworkId=%d", s->mNetworkId);
    cmsLog_debug("mSystemId=%d", s->mSystemId);
    cmsLog_debug("mCdmaRoamingIndicator=%d", s->mCdmaRoamingIndicator);
    cmsLog_debug("mCdmaEriIconIndex=%d", s->mCdmaEriIconIndex);
    cmsLog_debug("mCdmaEriIconMode=%d", s->mCdmaEriIconMode);

}

static void free_string(char** s)
{
    assert(s);
    if(*s != NULL)
    {
        free(*s);
        *s = NULL;
    }
}

static void free_strings(char** s, int numOfStrings)
{
    int i;

    assert(s);

    for(i=0; i<numOfStrings; i++)
    {
        if(s[i] != NULL)
        {
            free(s[i]);
            s[i] = NULL;
        }
    }
}

static void get_available_networks(Object* o)
{
    struct internal_message *msg;

    assert(o);

    msg = internal_message_obtain(EVENT_GET_AVAILABLE_NETWORKS_DONE, 
                                  SERVICE_STATE_TRACKER);
    assert(msg);
    internal_message_setObject(msg, o);
    ril_access_get_available_networks(msg);
}

static void on_get_available_networks_done(AsyncResult* ar)
{
    Object *result;
    Object *cms_token;

    assert(ar);

    cms_token = ar->userObj;
    result = ar->result;

    if(cms_token != NULL)
    {
        CmsSyncToken *token = (CmsSyncToken *)object_getData(cms_token);
        cms_access_send_resp(token->serial, CMSRET_SUCCESS, result);
    }
}

void service_state_tracker_get_roaming(UBOOL8 *roaming)
{
    assert(roaming);
    *roaming = mSS.mRoaming;
}

void service_state_tracker_get_access_tech(int *tech)
{
    assert(tech);
    *tech = mSS.mRilDataRadioTechnology;
}

void service_state_tracker_get_imei(char *imei, int buf_len)
{
    assert(imei);
    if(mImei != NULL)
    {
        cmsUtl_strncpy(imei, mImei, buf_len);
    }
    else
    {
        cmsLog_notice("imei device identity not retrieved yet.");
    }
}

void service_state_tracker_get_rssi(int* rssi)
{
    assert(rssi);
    *rssi = mSignalStrength;
}

void service_state_tracker_get_networkInUse(char *network, int buf_len)
{
    assert(network);
    if(mSS.mOperatorAlphaLong != NULL)
    {
        cmsUtl_strncpy(network, mSS.mOperatorAlphaLong, buf_len);
    }

    if(mSS.mOperatorAlphaShort != NULL)
    {
        cmsUtl_strncat(network, buf_len, mSS.mOperatorAlphaShort);
    }

    if(mSS.mOperatorNumeric != NULL)
    {
        cmsUtl_strncat(network, buf_len, mSS.mOperatorNumeric);
    }

    if(mSS.mOperatorState != NULL)
    {
        cmsUtl_strncat(network, buf_len, mSS.mOperatorState);
    }
}

