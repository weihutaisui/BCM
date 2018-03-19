/*
* <:copyright-BRCM:2011:proprietary:epon
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
#include "TkPlatform.h"
#include "TkSdkInitApi.h"
#include "Ethernet.h"
#ifdef BRCM_CMS_BUILD
#include "cms.h"
#include "cms_eid.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"
#include "cms_image.h"
#include "cms_boardcmds.h"
#include "cms_boardioctl.h"
#include "cms_log.h"
#else
#include "eponapp.h"
#endif
#include "eponctl_api.h"
#include "rdpactl_api.h"
#include "bcm_epon_cfg.h"
#include <unistd.h>
#include <stdlib.h>

// Global variables

#define TkDbgTrace(TkDbgErrorEnable) TkDbgLogTrace()
extern EponDevInfo *EponDevInfoInit(void);

#ifdef BRCM_CMS_BUILD
void *msgHandle=NULL;
void *tmrHandle;

#define EPONAPP_LOCK_TIMEOUT 5000



static void initLoggingFromConfig(UBOOL8 useConfiguredLogLevel)
{
   EponappCfgObject *obj;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   if ((ret = cmsLck_acquireLockWithTimeout(EPONAPP_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      return;
   }

   if ((ret = cmsObj_get(MDMOID_EPONAPP_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of EPONAPP_CFG object failed, ret=%d", ret);
   }
   else
   {
      if (useConfiguredLogLevel)
      {
         cmsLog_setLevel(cmsUtl_logLevelStringToEnum(obj->loggingLevel));
      }
      cmsLog_setDestination(cmsUtl_logDestinationStringToEnum(obj->loggingDestination));
	  
      cmsObj_free((void **) &obj);
   }

   cmsLck_releaseLock();
}


CmsRet eponappRegisterInterestedEvent(int event)
{
    CmsRet ret;
    CmsMsgHeader msg;

    cmsLog_debug("registering interest for event:%x",event);
    memset(&msg, 0, sizeof(CmsMsgHeader));
    msg.type = CMS_MSG_REGISTER_EVENT_INTEREST;
    msg.src = EID_EPON_APP;
    msg.dst = EID_SMD;
    msg.flags_request = 1;
    msg.wordData = event;

    ret = cmsMsg_sendAndGetReply(msgHandle, &msg);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("could not register for event %x",event);
    }
    return ret;
}
#endif


static int eponapp_init_oamif_hw_addr(MacAddr * if_mac)
{
   char cmd[128];

   sprintf(cmd, "ifconfig %s down", EPON_OAM_IF_NAME);
   system(cmd);

   sprintf(cmd, "ifconfig %s hw ether %02x%02x%02x%02x%02x%02x", 
                                                     EPON_OAM_IF_NAME, 
                                                     if_mac->byte[0],
                                                     if_mac->byte[1],
                                                     if_mac->byte[2],
                                                     if_mac->byte[3],
                                                     if_mac->byte[4],
                                                     if_mac->byte[5]);
   system(cmd);
  
   sprintf(cmd, "ifconfig %s up", EPON_OAM_IF_NAME);
   system(cmd);
  
   return 0; 
}


static char *eponappVendorName[OuiOamEnd] = {
    "bcm", "ctc", "ntt", "dasan", "dpoe", "kt", "pmc"};


static U8 eponappLookupVendorType(char *vendorStr)
{
    U8 i;
    U16 vendorType = 0;

    for (i = 0; i < OuiOamEnd; i ++)
    {
    if (!strncmp(vendorStr, eponappVendorName[i], strlen(eponappVendorName[i])))
        {//find the rigth vendor name
        vendorType = 1<<i;
        break;
        }
    }

    return vendorType;
}


#ifdef BRCM_CMS_BUILD
static void eponappSendPostMdmMsg(void)
{
    char buf[sizeof(CmsMsgHeader) + sizeof(MdmPostActNodeInfo)] = {0};
    CmsRet cmsReturn;
    CmsMsgHeader *msgHdr = (CmsMsgHeader *)buf;
    MdmPostActNodeInfo *msgBody = (MdmPostActNodeInfo *)(msgHdr + 1);  
    
    msgHdr->dst = EID_SSK;
    msgHdr->src = EID_EPON_APP;
    msgHdr->type = CMS_MSG_MDM_POST_ACTIVATING;
    msgHdr->flags_event = 1;
    msgHdr->dataLength = sizeof(MdmPostActNodeInfo);
    msgBody->subType = MDM_POST_ACT_TYPE_FILTER;
    
    // Attempt to send CMS response message & test result.
    cmsReturn = cmsMsg_send(msgHandle, msgHdr);
    if (cmsReturn != CMSRET_SUCCESS)
    {
        // Log error.
        cmsLog_error("Send message failure, cmsResult: %d", cmsReturn);
    }
    else
    {
        cmsLog_debug("Sent EPON App Indication to SSK");
    }     
}

int main( int argc, char **argv )
{
    SINT32 c, logLevelNum;
    CmsRet ret;
    CmsRet cmsReturn;
    CmsMsgHeader msgHdr = EMPTY_MSG_HEADER;
    Bool stand_alone = FALSE;
    EponDevInfo *devInfo = NULL;
    CmsLogLevel logLevel=DEFAULT_LOG_LEVEL;
    SINT32 cms_shmid=UNINITIALIZED_SHM_ID;
    UBOOL8 useConfiguredLogLevel = 1;
    U8 vendorMask = 0;
    rdpa_wan_type wanType;

    /*enable epon application only in case of epon pon activated*/
    ret = rdpaCtl_get_wan_type(rdpa_wan_type_to_if(rdpa_wan_epon), &wanType);
    if (!ret && ((wanType!=rdpa_wan_epon) && (wanType!=rdpa_wan_xepon)))
        return 0;

    cmsLog_init(EID_EPON_APP);

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
          useConfiguredLogLevel = 0;
          break;

         /* Need to use cms shimId for cmsMdm_init which is different from MDK share memory 
         * created either here or in swmdk.c
         */
         case 'm':
         cms_shmid = atoi(optarg);
         break;

         case 'b':
         strncpy(EPON_OAM_IF_NAME, optarg, sizeof(EPON_OAM_IF_NAME));
         break;

         case 's':
         /* stand_alone mode for debugging purpose */
         stand_alone = atoi(optarg);
         break;

         case 'o':
            {
            //parse oam parameter, oam stack maybe support more than one vendor at same time
            //they are separated by '/'
            char vendorArg[32];
            char *valPtr = vendorArg;
            char *tmpPtr;
            strncpy(vendorArg, optarg, sizeof(vendorArg));
            tmpPtr = strtok(valPtr, "/");
            vendorMask = 0;
            while (valPtr)
                {
                vendorMask |= eponappLookupVendorType(valPtr);
                valPtr = tmpPtr;
                tmpPtr = strtok(NULL, "/");
                }
            }
         break;

         default:
         break;
       }
    }


    /* detach from terminal and detach from smd session group. */
#ifndef DESKTOP_LINUX
    if (setsid() < 0)
    {
        cmsLog_error("could not detach from terminal");
        exit(-1);
    }
#endif

    /* ignore some common, problematic signals */
    signal(SIGINT, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);

    if(stand_alone == FALSE) 
    {
        if ((ret = cmsMsg_init(EID_EPON_APP, &msgHandle)) != CMSRET_SUCCESS)
        {
            cmsLog_cleanup();
            exit(-1);
        }

        if ((ret = cmsMdm_init(EID_EPON_APP, msgHandle, &cms_shmid)) != CMSRET_SUCCESS)
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
    
        initLoggingFromConfig(useConfiguredLogLevel);
    }

    eponappRegisterInterestedEvent(CMS_MSG_ETH_LINK_UP);
    eponappRegisterInterestedEvent(CMS_MSG_ETH_LINK_DOWN);
    
    devInfo = EponDevInfoInit();
    if (!vendorMask)
        vendorMask = devInfo->oamsel;
    
    eponapp_init_oamif_hw_addr(&devInfo->resbaseEponMac);

    if ( TkOamStackInit(stand_alone, vendorMask) != 0 )
    {
        TkOamStackExit();
        cmsLog_error("TkExtOamTaskInit failed!");
        return ERROR;
    }
    else
    {
        printf( "TkExtOamTaskInit success!\r\n" );           
    }

      
    // Tell the rest of the system that the EPON MAC has completed BOOTING
    if(stand_alone == FALSE)
    {    
        msgHdr.dst = EID_SSK;
        msgHdr.src = EID_EPON_APP;
        msgHdr.type = CMS_MSG_EPONMAC_BOOT_COMPLETE;
        msgHdr.flags_event = 1;
 
        // Attempt to send CMS response message & test result.
        cmsReturn = cmsMsg_send(msgHandle, &msgHdr);
        if (cmsReturn != CMSRET_SUCCESS)
        {
            // Log error.
            cmsLog_error("Send message failure, cmsResult: %d", cmsReturn);
        }
        else
        {
            cmsLog_debug("Sent EPON MAC BOOT Indication to SSK");
        }    
        eponappSendPostMdmMsg();
    }

    while ( 1 )
    {
        sleep(1000);
    }

    if(stand_alone == FALSE)
    {
        cmsTmr_cleanup(&tmrHandle);
        cmsMdm_cleanup();
        cmsMsg_cleanup(&msgHandle);
    }
    cmsLog_cleanup();
}
#else
int main( int argc, char **argv )
{
    //SINT32 c;
    SINT32 c, logLevelNum;
    int ret;
    Bool stand_alone = FALSE;
    EponDevInfo *devInfo = NULL;
    U8 vendorMask = 0;
    rdpa_wan_type wanType;
    CmsLogLevel logLevel=DEFAULT_LOG_LEVEL;
    
    /*enable epon application only in case of epon pon activated*/
    ret = rdpaCtl_get_wan_type(rdpa_wan_type_to_if(rdpa_wan_epon), &wanType);
    if (!ret && ((wanType!=rdpa_wan_epon) && (wanType!=rdpa_wan_xepon)))
        return 0;

    while ((c = getopt(argc, argv, "b:o:")) != -1)
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
                break;
                   
            case 'b':
                strncpy(EPON_OAM_IF_NAME, optarg, sizeof(EPON_OAM_IF_NAME));
                break;

            case 'o':
            {
                //parse oam parameter, oam stack maybe support more than one vendor at same time
                //they are separated by '/'
                char vendorArg[32];
                char *valPtr = vendorArg;
                char *tmpPtr;
                strncpy(vendorArg, optarg, sizeof(vendorArg));
                tmpPtr = strtok(valPtr, "/");
                vendorMask = 0;
                while (valPtr)
                    {
                    vendorMask |= eponappLookupVendorType(valPtr);
                    valPtr = tmpPtr;
                    tmpPtr = strtok(NULL, "/");
                    }
                break;
            }
                
            default:
                break;
        }
    }
    
    cmsLog_setLevel(logLevel);

#ifndef DESKTOP_LINUX
    /* detach from terminal and detach from smd session group. */
    if (setsid() < 0)
    {
        //cmsLog_error("could not detach from terminal");
        exit(-1);
    }
#endif

    /* ignore some common, problematic signals */
    signal(SIGINT, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    
    devInfo = EponDevInfoInit();
    if (!vendorMask)
        vendorMask = devInfo->oamsel;
    
    eponapp_init_oamif_hw_addr(&devInfo->resbaseEponMac);

    if ( TkOamStackInit(stand_alone, vendorMask) != 0 )
    {
        TkOamStackExit();
        cmsLog_error("TkExtOamTaskInit failed!");
        return ERROR;
    }
    else
    {
        printf( "TkExtOamTaskInit success!\r\n" );           
    }
    
    while ( 1 )
    {
        sleep(1000);
    }
}
#endif


