/*
 Copyright 2011, Broadcom Corporation
 <:label-BRCM:2015:proprietary:standard
 
  This program is the proprietary software of Broadcom and/or its
  licensors, and may only be used, duplicated, modified or distributed pursuant
  to the terms and conditions of a separate, written license agreement executed
  between you and Broadcom (an "Authorized License").  Except as set forth in
  an Authorized License, Broadcom grants no license (express or implied), right
  to use, or waiver of any kind with respect to the Software, and Broadcom
  expressly reserves all rights in and to the Software and all intellectual
  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 
  Except as expressly set forth in the Authorized License,
 
  1. This program, including its structure, sequence and organization,
     constitutes the valuable trade secrets of Broadcom, and you shall use
     all reasonable efforts to protect the confidentiality thereof, and to
     use this information only in connection with your use of Broadcom
     integrated circuit products.
 
  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
     PERFORMANCE OF THE SOFTWARE.
 
  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
     LIMITED REMEDY.
 :>
*/


#include "cms_access.h"
#include "cms_log.h"
#include "cms_msg.h"
#include "cms_util.h"
#include "cms_msg_cellular.h"
#include "cellular_internal.h"
#include "dc_tracker.h"
#include "call_tracker.h"
#include "misc_tracker.h"

#include <assert.h>

static SINT32 cms_fd = -1;
extern void *msgHandle;
static struct cms_sync_request *cms_sync_req_table = NULL;

static const char* ril2cms(UINT32 rilReq, UINT32 paraIdx, UINT32 rilVal);

static struct internal_message* process_interface_enable(CmsMsgHeader *cms_msg);
static struct internal_message* process_access_point_enable(CmsMsgHeader *cms_msg);
static struct internal_message* process_call_dial(CmsMsgHeader *cms_msg);
static struct internal_message* process_call_hangup(CmsMsgHeader *cms_msg);
static struct internal_message* process_call_answer(CmsMsgHeader *cms_msg);
static struct internal_message* process_call_get_current(CmsMsgHeader *cms_msg);
static struct internal_message* process_get_availableNetworks(CmsMsgHeader *cms_msg);
static struct internal_message* process_get_ril_request(CmsMsgHeader *cms_msg);

static void process_get_imei(CmsMsgHeader *cms_msg);
static void process_get_imsi(CmsMsgHeader *cms_msg);
static void process_get_networkInUse(CmsMsgHeader *cms_msg);
static void process_get_rssi(CmsMsgHeader *cms_msg);
static void process_get_usim_status(CmsMsgHeader *cms_msg);
static void process_get_usim_iccid(CmsMsgHeader *cms_msg);
static void process_get_usim_operatorNumeric(CmsMsgHeader *cms_msg);
static void process_get_usim_msisdn(CmsMsgHeader *cms_msg);

static int cms_syncreq_generate_token();
static struct cms_sync_request* cms_syncreq_obtain(CmsMsgHeader *cms_request);
static void cms_syncreq_free(struct cms_sync_request *r);
static UBOOL8 cms_syncreq_list_remove(struct cms_sync_request *item);
static UBOOL8 cms_syncreq_list_add(struct cms_sync_request* item);
static struct cms_sync_request* cms_syncreq_list_find(UINT32 serial);

static void resp_get_currentCall(CmsMsgHeader *cms_msg, Object *call_list);
static void resp_get_availableNetworks(CmsMsgHeader *cms_msg, Object *networks);
static void resp_get_ril_req(CmsMsgHeader *cms_msg, CmsRet wordData, Object *obj);

static const char* convert_tech_string(int tech);
static void process_get_roaming(CmsMsgHeader *cms_msg);
static void process_get_current_access_tech(CmsMsgHeader *cms_msg);
static void generate_notify_cms_header(CmsMsgHeader *msgHdr, CmsEntityId dstEid);

static unsigned int seed = 0;

static const char* ril2cms(UINT32 rilReq, UINT32 paraIdx, UINT32 rilVal)
{
   switch (rilReq)
   {
      case RIL_REQUEST_VOICE_REGISTRATION_STATE:
         switch (paraIdx)
         {
            case 0:
               switch (rilVal)
               {
                  case 0:return "Not registered, Not searching";
                  case 1:return "Registered, home network";
                  case 2:return "Not registered, searching";
                  case 3:return "Registration denied";
                  case 4:return "Unknown";
                  case 5:return "Registered, roaming";
                  case 10:return "Not registered, Not searching, Emergecy calls enabled";
                  case 12:return "Not registered, searching, Emergecy calls enabled";
                  case 13:return "Registration denied, Emergecy calls enabled";
                  case 14:return "Unknown, Emergecy calls enabled";
               }
            case 13:
               switch (rilVal)
               {
                  case 0:return "General";
                  case 1:return "Authentication Failure";
                  case 2:return "IMSI unknown in HLR";
                  case 3:return "Illegal MS";
                  case 4:return "Illegal ME";
                  case 5:return "PLMN not allowed";
                  case 6:return "Location area not allowed";
                  case 7:return "Roaming not allowed";
                  case 8:return "No Suitable Cells in this Location Area";
                  case 9:return "Network failure";
                  case 10:return "Persistent location update reject";
                  case 11:return "PLMN not allowed";
                  case 12:return "Location area not allowed";
                  case 13:return "Roaming not allowed in this Location Area";
                  case 15:return "No Suitable Cells in this Location Area";
                  case 17:return "Network Failure";
                  case 20:return "MAC Failure";
                  case 21:return "Sync Failure";
                  case 22:return "Congestion";
                  case 23:return "GSM Authentication unacceptable";
                  case 25:return "Not Authorized for this CSG";
                  case 32:return "Service option not supported";
                  case 33:return "Requested service option not subscribed";
                  case 34:return "Service option temporarily out of order";
                  case 38:return "Call cannot be identified";
                  case 95:return "Semantically incorrect message";
                  case 96:return "Invalid mandatory information";
                  case 97:return "Message type non-existent or not implemented";
                  case 98:return "Message not compatible with protocol state";
                  case 99:return "Information element non-existent or not implemented";
                  case 100:return "Conditional IE error";
                  case 101:return "Message not compatible with protocol state";
                  case 111:return "Protocol error, unspecified";   
                  default:                  
				  	if (rilVal>=48 && rilVal<=63) return "Retry upon entry into a new cell";
				  	else return "internal error";

               }
         }
		 
      case RIL_REQUEST_DATA_REGISTRATION_STATE:
         switch (paraIdx)
         {
            case 0:
               switch (rilVal)
               {
                  case 0:return "Not registered, Not searching";
                  case 1:return "Registered, home network";
                  case 2:return "Not registered, searching";
                  case 3:return "Registration denied";
                  case 4:return "Unknown";
                  case 5:return "Registered, roaming";
                  case 10:return "Not registered, Not searching, Emergecy calls enabled";
                  case 12:return "Not registered, searching, Emergecy calls enabled";
                  case 13:return "Registration denied, Emergecy calls enabled";
                  case 14:return "Unknown, Emergecy calls enabled";
               }
            case 3:
              return convert_tech_string(rilVal);
            case 4:
               switch (rilVal)
               {
                  case 0:return "General";
                  case 1:return "Authentication Failure";
                  case 2:return "IMSI unknown in HLR";
                  case 3:return "Illegal MS";
                  case 4:return "Illegal ME";
                  case 5:return "PLMN not allowed";
                  case 6:return "Location area not allowed";
                  case 7:return "GPRS services not allowed";
                  case 8:return "GPRS services and non-GPRS services not allowed";
                  case 9:return "MS identity cannot be derived by the network";
                  case 10:return "Implicitly detached";
                  case 11:return "PLMN not allowed";
                  case 12:return "Location area not allowed";
                  case 13:return "Roaming not allowed in this Location Area";
                  case 14:return "GPRS services not allowed in this PLMN";
                  case 15:return "No Suitable Cells in this Location Area";
                  case 16:return "MSC temporarily not reachable";
                  case 17:return "Network Failure";
                  case 20:return "MAC Failure";
                  case 21:return "Sync Failure";
                  case 22:return "Congestion";
                  case 23:return "GSM Authentication unacceptable";
                  case 25:return "Not Authorized for this CSG";
                  case 32:return "Service option not supported";
                  case 33:return "Requested service option not subscribed";
                  case 34:return "Service option temporarily out of order";
                  case 38:return "Call cannot be identified";
                  case 40:return "No PDP context activated";
                  case 95:return "Semantically incorrect message";
                  case 96:return "Invalid mandatory information";
                  case 97:return "Message type non-existent or not implemented";
                  case 98:return "Message not compatible with protocol state";
                  case 99:return "Information element non-existent or not implemented";
                  case 100:return "Conditional IE error";
                  case 101:return "Message not compatible with protocol state";
                  case 111:return "Protocol error, unspecified";   
                  default:                  
				  	if (rilVal>=48 && rilVal<=63) return "Retry upon entry into a new cell";
				  	else return "internal error";

               }
         }

      case RIL_REQUEST_IMS_REGISTRATION_STATE:
         switch (paraIdx)
         {
            case 0:
               switch (rilVal)
               {
                  case 0:return "Not registered";
                  case 1:return "Registered";
               }
            case 1:
              return convert_tech_string(rilVal);
         }
   }

   return "internal error";   
}

SINT32 cms_access_init(void* msg_handle)
{
    SINT32 commFd = 0;

    if(msg_handle != NULL)
    {
        cmsMsg_getEventHandle(msg_handle, &commFd);
    }

    cms_fd = commFd;
    return cms_fd;
}

static struct internal_message* cms_access_process(CmsMsgHeader *cms_msg)
{
    CmsRet ret;

    struct internal_message* result = NULL;

    switch(cms_msg->type)
    {
        case CMS_MSG_CELLULARAPP_GET_REQUEST:
            switch(cms_msg->wordData)
            {
                case DEVCELL_ROAMINGSTATUS:
                    process_get_roaming(cms_msg);
                    break;
                
                case DEVCELL_INT_CURRACCESSTECH:
                    process_get_current_access_tech(cms_msg);
                    break;

                case DEVCELL_CALL_GET_CURRENT:
                    result = process_call_get_current(cms_msg);
                    break;

                case DEVCELL_INT_IMEI:
                    process_get_imei(cms_msg);
                    break;

                case DEVCELL_INT_USIMIMSI:
                    process_get_imsi(cms_msg);
                    break;

                case DEVCELL_INT_USIMSTATUS:
                    process_get_usim_status(cms_msg);
                    break;

                case DEVCELL_INT_USIMMCCMNC:
                    process_get_usim_operatorNumeric(cms_msg);
                    break;

                case DEVCELL_INT_USIMMSISDN:
                    process_get_usim_msisdn(cms_msg);
                    break;

                case DEVCELL_INT_AVAILNETWORKS:
                    result = process_get_availableNetworks(cms_msg);
                    break;

                case DEVCELL_INT_NETWORKINUSE:
                    process_get_networkInUse(cms_msg);
                    break;

                case DEVCELL_INT_RSSI:
                    process_get_rssi(cms_msg);
                    break;
                     
                case DEVCELL_INT_USIMICCID:
                    process_get_usim_iccid(cms_msg);
                    break;

                default:
                    cmsLog_notice("unsupported cms get request %d", 
                                            cms_msg->wordData);
                    break;

            }
            break;
			
        case CMS_MSG_CELLULARAPP_GET_RIL_REQUEST:
           cmsLog_debug("got CMS_MSG_CELLULARAPP_GET_RIL_REQUEST: %d", cms_msg->wordData);  
           result = process_get_ril_request(cms_msg);
           break;
			
        case CMS_MSG_CELLULARAPP_SET_REQUEST:
            switch(cms_msg->wordData)
            {
                case DEVCELL_INT_ENABLE:
                   result = process_interface_enable(cms_msg);     
                   break;

                case DEVCELL_ACCESSPOINTENABLE:
                   result = process_access_point_enable(cms_msg);
                   break;

                case DEVCELL_CALL_DIAL:
                   result = process_call_dial(cms_msg);
                   break;

                case DEVCELL_CALL_HANGUP:
                   result = process_call_hangup(cms_msg);
                   break;

                case DEVCELL_CALL_ANSWER:
                   result = process_call_answer(cms_msg);
                   break;

                default:
                    if ((ret = cmsMsg_sendReply(msgHandle, cms_msg, 
                                      CMSRET_INTERNAL_ERROR)) 
                       != CMSRET_SUCCESS) 
                   {
                       cmsLog_error("send response for msg 0x%x failed, ret=%d",
                                                          cms_msg->type, ret); 
                   }

                   cmsLog_notice("unhandled cms set request %d!", 
                                                   cms_msg->wordData);
                   break;
            
            }
            break;

        case CMS_MSG_SET_LOG_LEVEL:                                      
           cmsLog_debug("got set log level to %d", cms_msg->wordData);  
           cmsLog_setLevel(cms_msg->wordData);
           if ((ret = cmsMsg_sendReply(msgHandle, cms_msg, CMSRET_SUCCESS)) 
                   != CMSRET_SUCCESS) 
           {
               cmsLog_error("send response for msg 0x%x failed, ret=%d",
                                                          cms_msg->type, ret); 
           }
           break;
        
        case CMS_MSG_SET_LOG_DESTINATION:                                      
           cmsLog_debug("got set log destination to %d", cms_msg->wordData);  
           cmsLog_setDestination(cms_msg->wordData);
           if ((ret = cmsMsg_sendReply(msgHandle, cms_msg, CMSRET_SUCCESS)) != 
                                                    CMSRET_SUCCESS) 
           {
               cmsLog_error("send response for msg 0x%x failed, ret=%d", 
                                                   cms_msg->type, ret); 
           }
           break;

       default:
           cmsLog_debug("ignore cms message type=0x%x", cms_msg->type);
           break;
    }
    return result;
}

struct internal_message* cms_access_read()
{   
    CmsRet cmsRet;
    CmsMsgHeader *pMsg = NULL;

    cmsRet = cmsMsg_receiveWithTimeout(msgHandle, &pMsg, 100);
    if(cmsRet == CMSRET_SUCCESS)
    {
        return cms_access_process(pMsg);
    }
    else
    {
        if(cmsRet == CMSRET_DISCONNECTED)
        {
            if(cmsFil_isFilePresent(SMD_SHUTDOWN_IN_PROGRESS))
            {
                /* smd is shutting down, I should quietly exit too. */
                exit(0); 
            }
            else
            {
                cmsLog_error("deteted exit of smd, cellularapp will also exit");
                /* unexpectedly lost connection to smd, */
                exit(-1);
            }
        }
    }
    return NULL;
}

void cms_access_notify_dataStatusChanged(RIL_Data_Call_Response_v6* new_state)
{
    char buf[sizeof(CmsMsgHeader) +
             sizeof(DevCellularDataCallStateChangedBody)] = {0};    
    CmsRet cmsReturn;                                                                                                                                                                                              
    CmsMsgHeader *msgHdr = (CmsMsgHeader *)buf;                                                                                                                                                                    
    DevCellularDataCallStateChangedBody *msgBody; 

    assert(msgHandle);

    msgBody = (DevCellularDataCallStateChangedBody *)(msgHdr + 1);                                                                                                                                        

    generate_notify_cms_header(msgHdr, EID_SSK);
    msgHdr->wordData = DEVCELL_EVENT_DATACALLSTATECHANGED;
    msgHdr->dataLength = sizeof(DevCellularDataCallStateChangedBody); 
    
    msgBody->active = new_state->active;    
    cmsUtl_strncpy(msgBody->ifname, new_state->ifname, BUFLEN_8);                                                                                                                                                 
    cmsUtl_strncpy(msgBody->addresses, new_state->addresses, BUFLEN_128);                                                                                                                                                 
    cmsUtl_strncpy(msgBody->dnses, new_state->dnses, BUFLEN_128);                                                                                                                                                 
    cmsUtl_strncpy(msgBody->gateways, new_state->gateways, BUFLEN_64);                                                                                                                                                 
                                                                                                                                                                                                                   
    cmsLog_notice("ifname=%s, active=%d", msgBody->ifname,                                                                                                                                                 
                                          msgBody->active);                                                                                                                                               
                                                                                                                                                                                                                   
    
    cmsReturn = cmsMsg_send(msgHandle, msgHdr);                                                                                                                                                         
                                                                                                                                                                                                                   
    if (cmsReturn != CMSRET_SUCCESS)                                                                                                                                                                               
    {                                                                                                                                                                                                              
        cmsLog_error("Send cms link status %s fail: %d",                                                                                                                                                           
                      msgBody->ifname,                                                                                                                                                                         
                      cmsReturn);                                                                                                                                                                                  
    }                                                                                                                                                                                                              
    else                                                                                                                                                                                                           
    {                                                                                                                                                                                                              
        cmsLog_notice("send cms link status %s successful",                                                                                                                                                        
                      msgBody->ifname);       
    }
}

static void generate_response_cms_header(CmsMsgHeader *msgHdr, 
         CmsEntityId dstEid, CmsMsgType type, CmsRet ret)
{
    assert(msgHdr);

    msgHdr->dst = dstEid;
    msgHdr->src = EID_CELLULAR_APP;
    msgHdr->type = type;
    msgHdr->flags_event = 0;
    msgHdr->flags_request = 0;
    msgHdr->flags_response = 1;
    msgHdr->wordData = ret; 
} 

static void generate_notify_cms_header(CmsMsgHeader *msgHdr, CmsEntityId dstEid)
{
    assert(msgHdr);
    
    msgHdr->dst = dstEid;
    msgHdr->src = EID_CELLULAR_APP;
    msgHdr->type = CMS_MSG_CELLULARAPP_NOTIFY_EVENT;
    msgHdr->flags_event = 1;
    msgHdr->flags_request = 0;
}

void cms_access_notify_callStateChanged()
{
    char buf[sizeof(CmsMsgHeader)] = {0};
    CmsRet cmsReturn;
    CmsMsgHeader *msgHdr = (CmsMsgHeader *)buf;

    assert(msgHandle);

    generate_notify_cms_header(msgHdr, EID_VOICE);
    msgHdr->wordData = DEVCELL_EVENT_CALLSTATECHANGED;

    cmsReturn = cmsMsg_send(msgHandle, msgHdr);

    if (cmsReturn != CMSRET_SUCCESS)
    {
         cmsLog_error("Send fail: %d", cmsReturn);
    }
    else
    {
         cmsLog_notice("Send successful");
    }
}

void cms_access_notify_ringBackTone(int* data, int length)
{
    char *buf;    
    int *p;
    CmsRet cmsReturn;                                                                                                                                                                                              
    int buf_size;

    buf_size = sizeof(CmsMsgHeader) + length*sizeof(int);
    buf = malloc(buf_size);
    assert(buf);
    memset(buf, 0x00, buf_size);

    CmsMsgHeader *msgHdr = (CmsMsgHeader *)buf;                                                                                                                                                                    

    assert(msgHandle);

    generate_notify_cms_header(msgHdr, EID_VOICE);
    msgHdr->wordData = DEVCELL_EVENT_RINGBACKTONE;
    
    p = (int *)(msgHdr + 1);
    memcpy(p, data, length*sizeof(int));

    cmsReturn = cmsMsg_send(msgHandle, msgHdr);                                                                                                                                                         
                                                                                                                                                                                                                                                                  
    if (cmsReturn != CMSRET_SUCCESS)                                                                                                                                                                               
    {                                                                                                                                                                                                              
         cmsLog_error("Send fail: %d", cmsReturn);                                                                                                                                                                                  
    }                                                                                                                                                                                                              
    else                                                                                                                                                                                                           
    {                                                                                                                                                                                                              
         cmsLog_notice("Send successful"); 
    }

    free(buf);

}

void cms_access_notify_newSms(UBOOL8 isOnSim, SINT32 index, 
                              UINT8* data, int length)
{
    char buf[sizeof(CmsMsgHeader) +
             sizeof(DevCellularNewSmsMsgBody)] = {0};    
    DevCellularNewSmsMsgBody *p;
    CmsRet cmsReturn;                                                                                                                                                                                              

    CmsMsgHeader *msgHdr = (CmsMsgHeader *)buf;                                                                                                                                                                    

    assert(msgHandle);

    generate_notify_cms_header(msgHdr, EID_SSK);
    msgHdr->wordData = DEVCELL_EVENT_SMS_NEW_SMS;
    msgHdr->dataLength = sizeof(DevCellularNewSmsMsgBody);

    p = (DevCellularNewSmsMsgBody *)(msgHdr + 1);
    p->isOnSim = isOnSim;
    if(p->isOnSim == TRUE)
    {
        p->index = index;
    }
    else
    {
        p->index = -1;
    }
    if(length <= BUFLEN_256)
    {
        memcpy(&(p->pdu), data, length * sizeof(UINT8));
        p->length = length;
    }

    cmsReturn = cmsMsg_send(msgHandle, msgHdr);                                                                                                                                                         
                                                                                                                                                                                                                                                                  
    if (cmsReturn != CMSRET_SUCCESS)                                                                                                                                                                               
    {                                                                                                                                                                                                              
         cmsLog_error("Send fail: %d", cmsReturn);                                                                                                                                                                                  
    }                                                                                                                                                                                                              
    else                                                                                                                                                                                                           
    {                                                                                                                                                                                                              
         cmsLog_notice("Send successful"); 
    }

}

static struct internal_message* process_interface_enable(CmsMsgHeader *cms_msg)
{
    struct internal_message* result = NULL;
    struct object *o = NULL;
    UBOOL8 enable;

    enable = *(UBOOL8 *)(cms_msg+1);
    
    cmsLog_debug("enable=%d", enable);

    result = internal_message_obtain(EVENT_CMS_ENABLE_INTERFACE, 
                                     SERVICE_STATE_TRACKER);
    o = object_obtain();
    object_setData(o, &enable, sizeof(UBOOL8));    
    internal_message_setObject(result, o);

    return result;
}

static struct internal_message* process_access_point_enable(
                                                         CmsMsgHeader *cms_msg)
{
    DevCellularAccessPointMsgBody *cms_apn = 
                                  (DevCellularAccessPointMsgBody *)(cms_msg+1);
    struct internal_message *msg;
    struct object *o;
    
    cmsLog_debug("set apn: %s", cms_apn->APN);

    o = object_obtain();
    object_setData(o, cms_apn, sizeof(DevCellularAccessPointMsgBody));
    msg = internal_message_obtain(EVENT_CMS_ENABLE_APN, DC_TRACKER);
    internal_message_setObject(msg, o);

    return msg;
}


static struct internal_message* process_call_dial(CmsMsgHeader *cms_msg)
{
    DevCellularCallDialMsgBody *cms_dial = 
                                  (DevCellularCallDialMsgBody *)(cms_msg+1);
    struct internal_message *msg;
    struct object *o;
    CmsRet ret;

    o = object_obtain();
    object_setData(o, cms_dial, sizeof(DevCellularCallDialMsgBody));
    msg = internal_message_obtain(EVENT_CALL_DIAL, CALL_TRACKER);
    internal_message_setObject(msg, o);

    generate_response_cms_header(cms_msg, cms_msg->src, cms_msg->type, 
                                 CMSRET_SUCCESS); 

    if ((ret = cmsMsg_send(msgHandle, cms_msg)) != CMSRET_SUCCESS) 
    {
        cmsLog_error("send response for msg 0x%x failed, ret=%d",
                                                  cms_msg->type, ret); 
    }
    return msg;

} 


static struct internal_message* process_call_hangup(CmsMsgHeader *cms_msg)
{
    DevCellularCallHangupMsgBody *call_hangup = 
                                  (DevCellularCallHangupMsgBody *)(cms_msg+1);
    struct internal_message *msg;
    struct object *o;
    CmsRet ret;

    o = object_obtain();
    object_setData(o, call_hangup, sizeof(DevCellularCallHangupMsgBody));
    msg = internal_message_obtain(EVENT_CALL_HANGUP, CALL_TRACKER);
    internal_message_setObject(msg, o);

    generate_response_cms_header(cms_msg, cms_msg->src, cms_msg->type, 
                                 CMSRET_SUCCESS); 

    if ((ret = cmsMsg_send(msgHandle, cms_msg)) != CMSRET_SUCCESS) 
    {
        cmsLog_error("send response for msg 0x%x failed, ret=%d",
                                                  cms_msg->type, ret); 
    }
    return msg;

} 

static struct internal_message* process_call_answer(CmsMsgHeader *cms_msg)
{
    struct internal_message *msg;
    CmsRet ret;

    msg = internal_message_obtain(EVENT_CALL_ANSWER, CALL_TRACKER);

    generate_response_cms_header(cms_msg, cms_msg->src, cms_msg->type, 
                                 CMSRET_SUCCESS); 

    if ((ret = cmsMsg_send(msgHandle, cms_msg)) != CMSRET_SUCCESS) 
    {
        cmsLog_error("send response for msg 0x%x failed, ret=%d",
                                                  cms_msg->type, ret); 
    }
    return msg;

} 

void cms_access_send_resp(int serial, CmsRet ret, Object *data)
{
    CmsSyncRequest *request;
    CmsMsgHeader *cms_request;
    request = cms_syncreq_list_find(serial);
    if(request == NULL)
    {
        cmsLog_error("Unable to find original cms request for serial %d.",
                      serial);
        return;
    } 
    cms_request = &request->cms_request;

    cmsLog_debug("cms request found: serial %d, req= 0x%x", serial, 
                       cms_request->type);

    switch(cms_request->type)
    {
        case CMS_MSG_CELLULARAPP_GET_REQUEST: 
        {
            switch(cms_request->wordData)
            {
                case DEVCELL_CALL_GET_CURRENT:
                   resp_get_currentCall(cms_request, data);
                   break;

                case DEVCELL_INT_AVAILNETWORKS:
                   resp_get_availableNetworks(cms_request, data);
                   break;
                     
                default:
                   cmsLog_notice("unhandled msg");
                   break; 
            } 
        }

        case CMS_MSG_CELLULARAPP_GET_RIL_REQUEST: 
        {
            resp_get_ril_req(cms_request, ret, data);
            break;
        }
		
        default:
            cmsLog_notice("unhandled msg");
            break;
    }

    if(request != NULL)
    {
        cms_syncreq_free(request);
        request = NULL;
    }
}

static void resp_get_availableNetworks(CmsMsgHeader *cms_msg, Object *networks)
{
    char *combined_string = NULL;
    char *s;
    int i, j;
    char *buf = NULL;
    int buf_size, payload_size;
    int numOfStrings=0, numOfNetworks=0;
    CmsMsgHeader *cms_reply;
    CmsRet ret;

    assert(networks);

    numOfStrings = object_dataLength(networks)/sizeof(char *);
    if(numOfStrings % 4 == 0)
    {    
        numOfNetworks = numOfStrings / 4;
        cmsLog_debug("numOfNetworks = %d", numOfNetworks);
 
        payload_size = numOfNetworks * BUFLEN_64;
        buf_size = sizeof(CmsMsgHeader) + payload_size;
        buf = malloc(buf_size); 
        assert(buf); 
        memset(buf, 0x00, buf_size);

        cms_reply = (CmsMsgHeader *)buf;

        combined_string = (char *)(cms_reply + 1);
        s = (char *)object_getData(networks);

        for(i=0; i<numOfNetworks; i++)
        {
            char network_info[BUFLEN_64] = {0};

            for(j=0; j<4; j++)
            {
                s += i*4+j;
                /* ensure each network info's string length would not exceed 
                 * 64 bytes.
                 */
                cmsUtl_strncat(network_info, BUFLEN_64, s); 
            }

            /* If the network info is not the last one, add comma as a 
             * seperator.
             */
            if(i < numOfNetworks-1)
            {
                cmsUtl_strncat(network_info, BUFLEN_64, ";");
            }

            /* append the network info string to the combined string. */
            cmsUtl_strncat(combined_string, payload_size, 
                           (const char*)&network_info);
        }
    } 
    else
    {
        cms_reply = cms_msg;
    }

    generate_response_cms_header(cms_reply, cms_msg->src, cms_msg->type, 
                                 CMSRET_SUCCESS); 

    cms_reply->dataLength = numOfNetworks * BUFLEN_64;

    if ((ret = cmsMsg_send(msgHandle, cms_reply)) != CMSRET_SUCCESS) 
    {
        cmsLog_error("send response for msg 0x%x failed, ret=%d",
                                                  cms_reply->type, ret); 
    }

    if(buf != NULL)
    {
        free(buf);
        buf = NULL;
    }
}

/*
 * Compose the cms response based on the stored cms request in the syncreq_list
 * and the result data from internal state machine, then send the cms response
 * to the requester process.
 */
static void resp_get_currentCall(CmsMsgHeader *cms_msg, Object *call_list)
{
    void *buf = NULL;
    int buf_size = 0;
    int numOfCalls = 0;
    int i;
    DevCellularCallListMsgBody *call;
    CmsMsgHeader *cms_reply;
    CmsRet ret;
    RIL_Call *src;

    if(call_list != NULL)
    {
        numOfCalls = object_dataLength(call_list)/sizeof(RIL_Call);
        buf_size = sizeof(CmsMsgHeader) + 
                   numOfCalls * sizeof(DevCellularCallListMsgBody);
        buf = malloc(buf_size); 
        assert(buf); 
        memset(buf, 0x00, buf_size);

        cms_reply = (CmsMsgHeader *)buf;
        call = (DevCellularCallListMsgBody *)(cms_reply+1);
        src = (RIL_Call *)object_getData(call_list);

        for(i=0; i<numOfCalls; i++)
        {
            call = call+i;
            src = src + i;

            call->state =  src->state;
            call->index = src->index;
            call->toa = src->toa;
            call->isMpty = src->isMpty;
            call->isMT = src->isMT;
            call->als = src->als;
            call->isVoice = src->isVoice;
            call->isVoicePrivacy = src->isVoicePrivacy;
            cmsUtl_strncpy((char *)&(call->number), src->number, BUFLEN_128);
            call->numberPresentation = src->numberPresentation;
            
            cmsUtl_strncpy((char *)&(call->name), src->name, BUFLEN_128);
            call->namePresentation = src->namePresentation;

            if(src->uusInfo != NULL)
            {
                call->uusType = src->uusInfo->uusType;
                call->uusDcs = src->uusInfo->uusDcs;

                assert(src->uusInfo->uusLength <= BUFLEN_256);
                call->uusLength = src->uusInfo->uusLength;
                memcpy(&(call->uusData), src->uusInfo->uusData, 
                         src->uusInfo->uusLength);        
            }
			
            call->isVideoCall =  src->isVideoCall;
            call->callType = src->callType;
			
            cmsLog_debug("(%d)isVideoCall =%d, callType = %d", 
                         i, call->isVideoCall, call->callType);
       }
       
    }
    else
    {
        cms_reply = cms_msg;
    }

    generate_response_cms_header(cms_reply, cms_msg->src, cms_msg->type, 
                                 CMSRET_SUCCESS); 
    
    cms_reply->wordData = DEVCELL_CALL_GET_CURRENT; 
    cms_reply->dataLength = numOfCalls * (sizeof(DevCellularCallListMsgBody));

    if ((ret = cmsMsg_send(msgHandle, cms_reply)) != CMSRET_SUCCESS) 
    {
        cmsLog_error("send response for msg 0x%x failed, ret=%d",
                                                  cms_reply->type, ret); 
    }

    if(buf != NULL)
    {
        free(buf);
        buf = NULL;
    }

}

static void resp_get_ril_req(CmsMsgHeader *cms_msg, CmsRet wordData, Object *obj)
{
   UINT8 buf[BUFLEN_512];
   CmsMsgHeader *cms_reply = (CmsMsgHeader *)buf;
   UINT8 *data = (UINT8 *)(cms_reply+1);
   CmsRet ret;

   memset(buf, 0x00, sizeof(buf));

   /*generate cms reply msg header*/   
   generate_response_cms_header(cms_reply, cms_msg->src, 
                                cms_msg->type, CMSRET_SUCCESS); 
   
   cmsLog_debug("reply wordData = %d", wordData);
  
   /*generate cms reply msg body*/   
   if (obj && obj->data && obj->data_len) 
   {
      cmsLog_debug("reply ril request = %d", cms_msg->wordData);
	  
      /*Sarah: Todo: For some obj structure, need translation defined in ril.h -> cms_msg_cellular.h */
      switch(cms_msg->wordData)
      {
         case RIL_REQUEST_VOICE_REGISTRATION_STATE:
         {
             UINT32 numOfStrings;
             char **strArray;
             GetRilReqVoiceRegStateBody *regVoiceRegState = (GetRilReqVoiceRegStateBody *)data;
   
             numOfStrings = obj->data_len/sizeof(char *);
             strArray = (char **)obj->data;
             cms_reply->dataLength = sizeof(GetRilReqVoiceRegStateBody);
             cmsUtl_strncpy(regVoiceRegState->regState, 
                            ril2cms(RIL_REQUEST_VOICE_REGISTRATION_STATE, 0, atoi(strArray[0])), 
                            sizeof(regVoiceRegState->regState));
			 
             cmsLog_debug("reply to RIL_REQUEST_VOICE_REGISTRATION_STATE: regState=(%d)%s", 
                          atoi(strArray[0]), regVoiceRegState->regState);

             if (atoi(strArray[0]) == 3 && numOfStrings > 13)
             {
                cmsUtl_strncpy(regVoiceRegState->regDenyReason, 
                               ril2cms(RIL_REQUEST_VOICE_REGISTRATION_STATE, 13, atoi(strArray[13])), 
                               sizeof(regVoiceRegState->regDenyReason));
                cmsLog_debug("reply to RIL_REQUEST_VOICE_REGISTRATION_STATE: regDenyReason=(%d)%s", 
                             atoi(strArray[13]), regVoiceRegState->regDenyReason);
             }
             break;
         }
      
         case RIL_REQUEST_DATA_REGISTRATION_STATE:
         {
             UINT32 numOfStrings;
             char **strArray;
             GetRilReqDataRegStateBody *regDataRegState = (GetRilReqDataRegStateBody *)data;
   
             numOfStrings = obj->data_len/sizeof(char *);
             strArray = (char **)obj->data;
             cms_reply->dataLength = sizeof(GetRilReqDataRegStateBody);
             cmsUtl_strncpy(regDataRegState->regState, 
                            ril2cms(RIL_REQUEST_DATA_REGISTRATION_STATE, 0, atoi(strArray[0])), 
                            sizeof(regDataRegState->regState));
			 
             cmsLog_debug("reply to RIL_REQUEST_DATA_REGISTRATION_STATE: regState=(%d)%s", 
                          atoi(strArray[0]), regDataRegState->regState);
			 
             if (numOfStrings > 3)
             {
                cmsUtl_strncpy(regDataRegState->availDataRadioTech, 
                            ril2cms(RIL_REQUEST_DATA_REGISTRATION_STATE, 3, atoi(strArray[3])), 
                            sizeof(regDataRegState->availDataRadioTech));
                cmsLog_debug("reply to RIL_REQUEST_DATA_REGISTRATION_STATE: availDataRadioTech=(%s)%s", 
                             strArray[3], regDataRegState->availDataRadioTech);
             }
			 
             if (atoi(strArray[0]) == 3 && numOfStrings > 4)
             {
                cmsUtl_strncpy(regDataRegState->regDenyReason, 
                               ril2cms(RIL_REQUEST_VOICE_REGISTRATION_STATE, 4, atoi(strArray[4])), 
                               sizeof(regDataRegState->regDenyReason));
                cmsLog_debug("reply to RIL_REQUEST_DATA_REGISTRATION_STATE: regDenyReason=(%d)%s", 
                             atoi(strArray[4]), regDataRegState->regDenyReason);
             }
             break;
         }
      
         case RIL_REQUEST_IMS_REGISTRATION_STATE:
         {
             UINT32 numOfInts;
             UINT32 *intArray;
             GetRilReqIMSRegStateBody *regIMSRegState = (GetRilReqIMSRegStateBody *)data;
   
             numOfInts = obj->data_len/sizeof(int *);
             intArray = (UINT32 *)obj->data;
             cms_reply->dataLength = sizeof(GetRilReqIMSRegStateBody);

             cmsUtl_strncpy(regIMSRegState->regState, 
                            ril2cms(RIL_REQUEST_IMS_REGISTRATION_STATE, 0, intArray[0]), 
                            sizeof(regIMSRegState->regState));
             cmsLog_debug("reply to RIL_REQUEST_IMS_REGISTRATION_STATE: regState=(%d)%s", intArray[0], regIMSRegState->regState);
			 
             if (numOfInts > 1) 
             {
                cmsUtl_strncpy(regIMSRegState->radioTechnology, 
                               ril2cms(RIL_REQUEST_IMS_REGISTRATION_STATE, 1, intArray[1]), 
                               sizeof(regIMSRegState->radioTechnology));
                cmsLog_debug("reply to RIL_REQUEST_IMS_REGISTRATION_STATE: radioTechnology=(%d)%s", 
                             intArray[1], regIMSRegState->radioTechnology);
             }   

             break;
         }
        
         default:
             memcpy(data, obj->data, obj->data_len);
             break;
      }
      
   }
            

   /*update wordData with return value*/   
   cms_reply->wordData = (UINT32)wordData; 

   if ((ret = cmsMsg_send(msgHandle, cms_reply)) != CMSRET_SUCCESS) 
   {
       cmsLog_error("send response for msg 0x%x failed, ret=%d",
                    cms_reply->type, ret); 
   }
   
}

static struct internal_message* process_call_get_current(CmsMsgHeader *cms_msg)
{
    struct internal_message *msg = NULL;
    struct cms_sync_request *req = NULL;
    CmsSyncToken token;
    Object *o;

    req = cms_syncreq_obtain(cms_msg); 
    assert(req);

    msg = internal_message_obtain(EVENT_CALL_GET_CURRENT, CALL_TRACKER);
    
    o = object_obtain();

    token.data = NULL;
    token.serial = req->serial;
    object_setData(o, &token, sizeof(CmsSyncToken));
    
    internal_message_setObject(msg, o);

    return msg;

} 

static struct internal_message* process_get_availableNetworks(CmsMsgHeader *cms_msg)
{
    struct internal_message *msg = NULL;
    struct cms_sync_request *req = NULL;
    CmsSyncToken token;
    Object *o;

    req = cms_syncreq_obtain(cms_msg); 
    assert(req);

    msg = internal_message_obtain(EVENT_GET_AVAILABLE_NETWORKS, 
                                  SERVICE_STATE_TRACKER);
    
    o = object_obtain();

    token.data = NULL;
    token.serial = req->serial;
    object_setData(o, &token, sizeof(CmsSyncToken));
    
    internal_message_setObject(msg, o);

    return msg;

} 

static void process_get_roaming(CmsMsgHeader *cms_msg)
{
    char buf[sizeof(CmsMsgHeader) + BUFLEN_16] = {0};
    CmsMsgHeader *cms_reply = (CmsMsgHeader *)buf;
    UBOOL8 roaming;
    
    generate_response_cms_header(cms_reply, cms_msg->src, cms_msg->type, 
                                 CMSRET_SUCCESS); 

    cms_reply->dataLength = BUFLEN_16;

    cellular_internal_get_roaming(&roaming);
    if(roaming == FALSE)
    {
        strcpy((char *)(cms_reply+1), "Home");
    }
    else
    {
        strcpy((char *)(cms_reply+1), "Roaming");
    }
    cmsLog_debug("return roaming=%s", (char *)(cms_reply+1));

    cmsMsg_send(msgHandle, cms_reply); 
}

static const char* convert_tech_string(int tech)
{
    const char* result = NULL;

    switch(tech)
    {
        case RADIO_TECH_GPRS:
            result = "GPRS";
            break;

        case RADIO_TECH_EDGE:
            result = "EDGE";
            break;

        case RADIO_TECH_UMTS:
            result = "UMTS";
            break;

        case RADIO_TECH_HSPA:
            result = "UMTSHSPA";
            break;
        
        case RADIO_TECH_LTE:
            result = "LTE";
            break;

        default:
            result = "UNKNOWN";
            break;
                 
    }

    return result;

}

static void process_get_current_access_tech(CmsMsgHeader *cms_msg)
{
    char buf[sizeof(CmsMsgHeader) + BUFLEN_256] = {0};
    CmsMsgHeader *cms_reply = (CmsMsgHeader *)buf;
    int tech;
    const char* tech_string;
    
    generate_response_cms_header(cms_reply, cms_msg->src, cms_msg->type, 
                                 CMSRET_SUCCESS); 

    cms_reply->dataLength = BUFLEN_256;
    
    cellular_internal_get_access_tech(&tech);
    tech_string = convert_tech_string(tech);

    cmsUtl_strncpy((char *)(cms_reply+1), tech_string, BUFLEN_256);
    
    cmsLog_debug("return tech=%s", (char *)(cms_reply+1));
    
    cmsMsg_send(msgHandle, cms_reply); 

}


static void process_get_imei(CmsMsgHeader *cms_msg)
{
    char buf[sizeof(CmsMsgHeader) + BUFLEN_16] = {0};
    CmsMsgHeader *cms_reply = (CmsMsgHeader *)buf;

    char *imei = (char *)(cms_reply + 1);
    
    generate_response_cms_header(cms_reply, cms_msg->src, cms_msg->type, 
                                 CMSRET_SUCCESS); 

    cms_reply->dataLength = BUFLEN_16;
    
    cellular_internal_get_imei(imei, BUFLEN_16);
    
    cmsLog_debug("return imei=%s", imei);
    
    cmsMsg_send(msgHandle, cms_reply); 

}

static void process_get_imsi(CmsMsgHeader *cms_msg)
{
    char buf[sizeof(CmsMsgHeader) + BUFLEN_16] = {0};
    CmsMsgHeader *cms_reply = (CmsMsgHeader *)buf;

    char *imsi = (char *)(cms_reply + 1);
    
    generate_response_cms_header(cms_reply, cms_msg->src, cms_msg->type, 
                                 CMSRET_SUCCESS); 

    cms_reply->dataLength = BUFLEN_16;
    
    cellular_internal_get_imsi(imsi, BUFLEN_16);
    
    cmsLog_debug("return imsi=%s", imsi);
    
    cmsMsg_send(msgHandle, cms_reply); 

}

static void process_get_usim_operatorNumeric(CmsMsgHeader *cms_msg)
{
    char buf[sizeof(CmsMsgHeader) + BUFLEN_8] = {0};
    CmsMsgHeader *cms_reply = (CmsMsgHeader *)buf;

    char *operator = (char *)(cms_reply + 1);
    
    generate_response_cms_header(cms_reply, cms_msg->src, cms_msg->type, 
                                 CMSRET_SUCCESS); 

    cms_reply->dataLength = BUFLEN_8;
    
    cellular_internal_get_operatorNumeric(operator, BUFLEN_8);
    
    cmsLog_debug("return operator=%s", operator);
    
    cmsMsg_send(msgHandle, cms_reply); 

}

static void process_get_usim_msisdn(CmsMsgHeader *cms_msg)
{
    char buf[sizeof(CmsMsgHeader) + BUFLEN_16] = {0};
    CmsMsgHeader *cms_reply = (CmsMsgHeader *)buf;

    char *msisdn = (char *)(cms_reply + 1);
    
    generate_response_cms_header(cms_reply, cms_msg->src, cms_msg->type, 
                                 CMSRET_SUCCESS); 

    cms_reply->dataLength = BUFLEN_16;
    
    cellular_internal_get_msisdn(msisdn, BUFLEN_16);
    
    cmsLog_debug("return msisdn=%s", msisdn);
    
    cmsMsg_send(msgHandle, cms_reply); 

}
/* Get the network in use from service state tracker, as the information is 
 * monitored there.
 */
static void process_get_networkInUse(CmsMsgHeader *cms_msg)
{
    char buf[sizeof(CmsMsgHeader) + BUFLEN_64] = {0};
    CmsMsgHeader *cms_reply = (CmsMsgHeader *)buf;

    char *network = (char *)(cms_reply + 1);
    
    generate_response_cms_header(cms_reply, cms_msg->src, cms_msg->type, 
                                 CMSRET_SUCCESS); 

    cms_reply->dataLength = BUFLEN_64;
    
    cellular_internal_get_networkInUse(network, BUFLEN_64);
    
    cmsLog_debug("return networkInUse=%s", network);
    
    cmsMsg_send(msgHandle, cms_reply); 

}

/* Get the signal strength in dbm. */
static void process_get_rssi(CmsMsgHeader *cms_msg)
{
    char buf[sizeof(CmsMsgHeader) + sizeof(SINT32)] = {0};
    SINT32 rssi = 0;

    CmsMsgHeader *cms_reply = (CmsMsgHeader *)buf;
    
    generate_response_cms_header(cms_reply, cms_msg->src, cms_msg->type, 
                                 CMSRET_SUCCESS); 

    cms_reply->dataLength = sizeof(SINT32);

    cellular_internal_get_rssi(&rssi);

    *(SINT32 *)(cms_reply+1) = rssi;

    cmsLog_debug("return rssi=%d", rssi);
    
    cmsMsg_send(msgHandle, cms_reply); 

}

/* Get the SIM card status in string. */
static void process_get_usim_status(CmsMsgHeader *cms_msg)
{
    char buf[sizeof(CmsMsgHeader) + sizeof(BUFLEN_16)] = {0};
    CmsMsgHeader *cms_reply = (CmsMsgHeader *)buf;
    char* card_status = NULL;

    generate_response_cms_header(cms_reply, cms_msg->src, cms_msg->type, 
                                 CMSRET_SUCCESS); 

    cms_reply->dataLength = BUFLEN_16; 

    card_status = (char *)(cms_reply+1);
    cellular_internal_get_usim_status(card_status, BUFLEN_16);

    cmsLog_debug("return card_status=%s", card_status);
    
    cmsMsg_send(msgHandle, cms_reply); 

}

/* Get the ICCID */
static void process_get_usim_iccid(CmsMsgHeader *cms_msg)
{
    char buf[sizeof(CmsMsgHeader) + sizeof(BUFLEN_16)] = {0};
    CmsMsgHeader *cms_reply = (CmsMsgHeader *)buf;
    char* iccid = NULL;

    generate_response_cms_header(cms_reply, cms_msg->src, cms_msg->type, 
                                 CMSRET_SUCCESS); 

    cms_reply->dataLength = BUFLEN_16; 

    iccid = (char *)(cms_reply+1);
    cellular_internal_get_usim_iccid(iccid, BUFLEN_16);

    cmsLog_debug("return iccid=%s", iccid);
    
    cmsMsg_send(msgHandle, cms_reply); 

}

static struct internal_message* process_get_ril_request(CmsMsgHeader *cms_msg)
{
    struct internal_message *msg = NULL;
    struct cms_sync_request *req = NULL;
    MiscTrackerGetRilReq getRilReq;
    Object *o;

    req = cms_syncreq_obtain(cms_msg); 
    assert(req);

    msg = internal_message_obtain(EVENT_MISC_GET_RIL_REQUEST, 
                                  MISC_TRACKER);
    
    o = object_obtain();

    getRilReq.rilReq = (SINT32)cms_msg->wordData;
    getRilReq.serial = req->serial;
    object_setData(o, &getRilReq, sizeof(MiscTrackerGetRilReq));
    
    internal_message_setObject(msg, o);

    return msg;

} 

/*return the generated token in incrementally manner */
static int cms_syncreq_generate_token()
{
    /* Possible wrap around the unsigned integar value. On a 32-bit machine, if
     * we assume a new token is needed every second, it takes 136 years to use 
     * it up. So should be considered as unique token in this sense;
     */
    seed++;

    return seed;
}

static struct cms_sync_request* cms_syncreq_obtain(CmsMsgHeader *cms_request)
{
    struct cms_sync_request *request;

    assert(cms_request);

    request = malloc(sizeof(CmsSyncRequest));
    assert(request);
    request->serial = cms_syncreq_generate_token();
    memcpy(&request->cms_request, cms_request, sizeof(CmsMsgHeader));
    request->next = NULL;

    if(cms_syncreq_list_add(request))
    {
        return request;
    }
    else
    {
        free(request);
        return NULL;
    }
}  

static UBOOL8 cms_syncreq_list_add(struct cms_sync_request* item)
{
    struct cms_sync_request* i;

    i = cms_sync_req_table;

    if(i == NULL)
    {
        cms_sync_req_table = item;
        return TRUE;
    }

    while((i->next != NULL) && (i->serial != item->serial))
    {
        i = i->next;
    }
    
    if((i->next == NULL) && (i->serial != item->serial))
    {
        i->next = item;
        return TRUE;
    }

    return FALSE;
}

/* Use the serial number as index to find the original cms_sync_request in the list */
static struct cms_sync_request* cms_syncreq_list_find(UINT32 serial)
{
    struct cms_sync_request* i = cms_sync_req_table;
    while(i != NULL)
    {
        if(i->serial == serial)
        {
            return i;
        }
        i = i->next;
    }
    return NULL;
}

static UBOOL8 cms_syncreq_list_remove(struct cms_sync_request *item)
{
    struct cms_sync_request *i;
    struct cms_sync_request *j;
    int serial;

    assert(item);

    serial = item->serial;

    i = j = cms_sync_req_table;

    while((j != NULL) && (j->serial != serial))
    {
        i = j;
        j = j->next;
    }

    if((j != i) && (j != NULL))
    {
        /* *
         * we have found the item, which is pointed by j 
         * and i points to its precedant item.
         * */
        i->next = j->next;
        return TRUE;
    }
    else if((j == i) && (j->serial == serial))
    {
        /*item matched on the head of list.*/ 
        cms_sync_req_table = j->next;
        return TRUE;
    }
    else
    {
       cmsLog_notice("item not found in list\n" );
       return FALSE;
    }
}

static void cms_syncreq_free(struct cms_sync_request *r)
{
    assert(r);

    cms_syncreq_list_remove(r);

    free(r);
}



