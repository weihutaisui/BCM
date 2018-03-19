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

#include "cms_log.h"
#include "cms_util.h"

#include "cellular_msg.h"
#include "service_state_tracker.h"
#include "dc_tracker.h"
#include "data_connection.h"
#include "uicc_controller.h"
#include "icc_file_handler.h"
#include "adn_record_loader.h"
#include "call_tracker.h"
#include "sms_tracker.h"
#include "misc_tracker.h"

#include <assert.h>

//submodule to dispatch various events to their corresponding feature blocks.
//The sub feature blocks are:
//service_state_tracker, dc_tracker, and uicc_controller
void cellular_internal_process(struct internal_message *msg)
{
    cmsLog_debug("dest=%d, what=%d\n", 
                        msg->dest, msg->what);
    switch(msg->dest)
    {
        case SERVICE_STATE_TRACKER:
            service_state_tracker_process(msg);
            break;

        case DC_TRACKER:
            dc_tracker_process(msg);
            break;

        case DATA_CONNECTION:
            data_connection_process(msg);
            break;
        
        case UICC_CONTROLLER:
            uicc_controller_process(msg);
            break;

        case ICC_FILE_HANDLER:
            icc_file_handler_process(msg);
            break;

        case ADN_RECORD_LOADER:
            adn_record_loader_process(msg);
            break;

        case CALL_TRACKER:
            call_tracker_process(msg);
            break;

        case SMS_TRACKER:
            sms_tracker_process(msg);
            break;
			
        case MISC_TRACKER:
            misc_tracker_process(msg);
            break;

        case ALL:
            service_state_tracker_process(msg);
            dc_tracker_process(msg);
            uicc_controller_process(msg);
            icc_file_handler_process(msg);
            adn_record_loader_process(msg);
            call_tracker_process(msg);
            data_connection_process(msg);
            sms_tracker_process(msg);
            break;

        default:
            cmsLog_notice("unhandled msg: dest=%d, what=%d", msg->dest, 
                                                             msg->what);
            break;
     } 
}

void cellular_internal_states_init()
{
    //get from cms the necessay information and
    //initialize internal data structure.
    service_state_tracker_init();
    uicc_controller_init();
    icc_file_handler_init();
    dc_tracker_init();
    data_connection_init();
    call_tracker_init();
    sms_tracker_init();
    misc_tracker_init();
}

void cellular_internal_get_roaming(UBOOL8 *roaming)
{
    assert(roaming);
    service_state_tracker_get_roaming(roaming);
}

void cellular_internal_get_access_tech(int *tech)
{
    assert(tech);
    service_state_tracker_get_access_tech(tech);
}

void cellular_internal_get_imei(char *imei, int buf_len)
{
    assert(imei);
    assert(buf_len > 0);
    service_state_tracker_get_imei(imei, buf_len);
}

void cellular_internal_get_networkInUse(char *network, int buf_len)
{
    assert(network);
    assert(buf_len > 0);
    service_state_tracker_get_networkInUse(network, buf_len);
}


void cellular_internal_get_rssi(int* rssi)
{
    assert(rssi);
    service_state_tracker_get_rssi(rssi);
}

void cellular_internal_get_imsi(char *imsi, int buf_len)
{
    const char *s;

    assert(imsi);
    assert(buf_len > 0);
    /* Hard-code application index to be 0. */
    s = uicc_controller_getImsi(0);
    if(s != NULL)
    {
        cmsUtl_strncpy(imsi, s, buf_len);
    }
}

void cellular_internal_get_operatorNumeric(char *operator, int buf_len)
{
    char *s;

    assert(operator);
    assert(buf_len > 0);
    /* Hard-code application index to be 0. */
    s = uicc_controller_getOperatorNumeric(0);
    if(s != NULL)
    {
        cmsUtl_strncpy(operator, s, buf_len);
        free(s);
        s = NULL;
    }
}

void cellular_internal_get_usim_iccid(char *iccid, int buf_len)
{
    const char *s;

    assert(iccid);
    assert(buf_len > 0);

    s = uicc_controller_getIccId();
    if(s != NULL)
    {
        cmsUtl_strncpy(iccid, s, buf_len);
    }
}

void cellular_internal_get_msisdn(char *msisdn, int buf_len)
{
    const char *s;

    assert(msisdn);
    assert(buf_len > 0);

    s = uicc_controller_get_msisdn();
    if(s != NULL)
    {
        cmsUtl_strncpy(msisdn, s, buf_len);
    }
}

void cellular_internal_get_usim_status(char *card_status, int buf_len)
{
    RIL_CardState t;

    assert(card_status);
    assert(buf_len > 0);

    t = uicc_controller_get_card_status();
    switch(t)
    {
        case RIL_CARDSTATE_PRESENT:
            cmsUtl_strncpy(card_status, "Valid", buf_len);
            break;

        case RIL_CARDSTATE_ERROR:
            cmsUtl_strncpy(card_status, "Error", buf_len);
            break;

        case RIL_CARDSTATE_ABSENT:
        default:
            cmsUtl_strncpy(card_status, "None", buf_len);
            break;
    }
}
