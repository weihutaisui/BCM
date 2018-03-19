/*
* <:copyright-BRCM:2011:proprietary:standard
* 
*    Copyright (c) 2011 Broadcom 
*    All Rights Reserved
* 
*  This program is the proprietary software of Broadcom and/or its
*  licensors, and may only be used, duplicated, modified or distributed pursuant
*  to the terms and conditions of a separate, written license agreement executed
*  between you and Broadcom (an "Authorized License").  Except as set forth in
*  an Authorized License, Broadcom grants no license (express or implied), right
*  to use, or waiver of any kind with respect to the Software, and Broadcom
*  expressly reserves all rights in and to the Software and all intellectual
*  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
*  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
*  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
* 
*  Except as expressly set forth in the Authorized License,
* 
*  1. This program, including its structure, sequence and organization,
*     constitutes the valuable trade secrets of Broadcom, and you shall use
*     all reasonable efforts to protect the confidentiality thereof, and to
*     use this information only in connection with your use of Broadcom
*     integrated circuit products.
* 
*  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
*     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
*     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
*     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
*     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
*     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
*     PERFORMANCE OF THE SOFTWARE.
* 
*  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
*     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
*     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
*     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
*     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
*     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
*     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
*     LIMITED REMEDY.
* :>
 
*/

#include "cms.h"
#include "cms_eid.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"
#include "cms_log.h"
#include "cms_access.h"
#include "ril_access.h"
#include "timer_manager.h"
#include "cellular_msg.h"
#include "cellular_internal.h"

#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#define CELLULARAPP_LOCK_TIMEOUT 2000

static void init_log_level();
static void event_loop();

// Global variables
void *msgHandle=NULL;
void *tmrHandle;
int cms_fd = -1;
int ril_fd = -1;

static void init_log_level()
{
    CellularappCfgObject *obj;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret;

    if ((ret = cmsLck_acquireLockWithTimeout(CELLULARAPP_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
    {
       cmsLog_error("failed to get lock, ret=%d", ret);
       return;
    }

    if ((ret = cmsObj_get(MDMOID_CELLULARAPP_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
    {
       cmsLog_error("get of CELLULARAPP_CFG object failed, ret=%d", ret);
    }
    else
    {
       cmsLog_setLevel(cmsUtl_logLevelStringToEnum(obj->loggingLevel));
       cmsLog_setDestination(cmsUtl_logDestinationStringToEnum(obj->loggingDestination));
       cmsObj_free((void **) &obj);
    }
    cmsLck_releaseLock();
}

int main( int argc, char **argv )
{
    SINT32 c, logLevelNum;
    CmsRet ret;
    CmsLogLevel logLevel = LOG_LEVEL_ERR;
    SINT32 cms_shmid = UNINITIALIZED_SHM_ID;
    UBOOL8 useConfiguredLogLevel = TRUE;

    cmsLog_initWithName(EID_CELLULAR_APP, "cellularapp");

    while ((c = getopt(argc, argv, "v:m:b:s:o:")) != -1)
    {
       switch(c)
       {
          case 'v':
              logLevelNum = atoi(optarg);
              if (logLevelNum == 0)
              {
                  logLevel = LOG_LEVEL_ERR;
              }
              else if (logLevelNum == 1) 
              {
                  logLevel = LOG_LEVEL_NOTICE;
              }
              else 
              {
                  logLevel = LOG_LEVEL_DEBUG;
              }
              cmsLog_setLevel(logLevel);
              useConfiguredLogLevel = FALSE;
              break;

         /* Need to use cms shimId for cmsMdm_init which is different from MDK share memory 
         * created either here or in swmdk.c
         */
         case 'm':
             cms_shmid = atoi(optarg);
             break;


         default:
             break;
       }
    }



    /* ignore some common, problematic signals */
    signal(SIGINT, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);

    if ((ret = cmsMsg_init(EID_CELLULAR_APP, &msgHandle)) != CMSRET_SUCCESS)
    {
        cmsLog_cleanup();
        exit(-1);
    }

    if ((ret = cmsMdm_init(EID_CELLULAR_APP, msgHandle, &cms_shmid)) != CMSRET_SUCCESS)
    {
        cmsMsg_cleanup(&msgHandle);
        cmsLog_cleanup();
        exit(-1);
    }

    if ((ret = cmsTmr_init(&tmrHandle)) != CMSRET_SUCCESS)
    {
        cmsLog_error("cmsTmr_init failed, ret=%d", ret);
        cmsMdm_cleanup();
        cmsMsg_cleanup(&msgHandle);
        cmsLog_cleanup();
        exit(-1);
    }
    

    if(useConfiguredLogLevel == TRUE)
    {
        init_log_level();
    } 

    cms_fd = cms_access_init(msgHandle);
    ril_fd = ril_access_init();
    timer_manager_init();

    cellular_internal_states_init();

    //start the main handler loop thread.  
    event_loop();

    cmsTmr_cleanup(&tmrHandle);
    cmsMdm_cleanup();
    cmsMsg_cleanup(&msgHandle);
    cmsLog_cleanup();
    
    return 0;
}

static void event_loop()
{
    SINT32 rc;
    fd_set readFds;
    
    cmsLog_debug("enter event_loop\n");
    while ( 1 )
    {
		struct timeval tv;
		int max_fd = 0;

		FD_ZERO(&readFds);
        
        max_fd = timer_manager_init_fdset(&readFds);

        if(cms_fd > 0)
        {    
		    FD_SET(cms_fd, &readFds);
		    max_fd = (max_fd > cms_fd)? max_fd:cms_fd;
        }
   
        if(ril_fd > 0)
        {    
		    FD_SET(ril_fd, &readFds);
		    max_fd = (max_fd > ril_fd)? max_fd:ril_fd;
        }

        tv.tv_sec = CELLULAR_PERIODIC_TASK_INTERVAL;
        tv.tv_usec = 0;

		rc = select(max_fd+1, &readFds, NULL, NULL, &tv);
		if (rc > 0) 
		{
            struct internal_message *msg = NULL;
		        
            if(FD_ISSET(cms_fd, &readFds))
			{
                 msg = cms_access_read();
                 if(msg != NULL)
                 {
                     cellular_internal_process(msg);
					 
                     internal_message_free_and_null_ptr(&msg);
                 }
			}

			if(FD_ISSET(ril_fd, &readFds))
			{
			    msg = ril_access_read(ril_fd);
                if(msg != NULL)
                {
                    cellular_internal_process(msg);

                    Object *o = internal_message_getObject(msg);
                    if(o != NULL)
                    {
                        AsyncResult* rr = (AsyncResult *)object_getData(o);
                        
                        if(rr != NULL)
                        { 
                             /* free up the objects pointed to by AsyncResult */
                             if(rr->result != NULL)
                             {
                                 object_freeData(rr->result);
                                 free(rr->result);
                                 rr->result = NULL;
                             }
                             if(rr->userObj != NULL)
                             {
                                 object_freeData(rr->userObj);
                                 free(rr->userObj);
                                 rr->result = NULL;
                             }
                         }
                    }
					
                internal_message_free_and_null_ptr(&msg);
                }
            }
   
            /* Timer expiry check */
            msg = timer_manager_check_expiry(&readFds);

            if(msg != NULL)
            {
                cellular_internal_process(msg);
            }
            
            /* Processing done, free the relevant internal_messages */
            internal_message_free_and_null_ptr(&msg);

            cmsLog_debug("finished current round");
        }
    }
}

