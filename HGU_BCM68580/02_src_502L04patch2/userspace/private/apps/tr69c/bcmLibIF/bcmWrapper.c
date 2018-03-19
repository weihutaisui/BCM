/***********************************************************************
 *
 *  Copyright (c) 2006-2007  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:standard

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
 *
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#define _XOPEN_SOURCE
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <syslog.h>

#include "inc/tr69cdefs.h" /* defines for ACS state */
#include "inc/appdefs.h"

#include "bcmWrapper.h"
#include "cms_image.h"
#include "cms_qdm.h"
#include "cms_msg.h"
#include "adslctlapi.h"
#include "bcmConfig.h"

/*#define DEBUG*/

extern int sysGetSdramSize( void );   /* in cms_utils/xboard_api.c */
extern void clearInformEventList(void);
extern ACSState   acsState;
extern void *msgHandle;
extern InformEvList informEvList;
extern UINT32 addInformEventToList(UINT8 event);


#ifdef DMP_DSLDIAGNOSTICS_1
DslLoopDiagData *pDslLoopDiagResults=NULL;
extern CmsRet rutWan_getAdslLoopDiagResultAndLinkUp(DslLoopDiagData *pResult, UINT32 lineId);
#endif

#ifdef SUPPORT_TR69C_AUTONOMOUS_TRANSFER_COMPLETE
extern UBOOL8 doSendAutonTransferComplete;
#endif

struct informEvtTableEntry informEvtStringTable[] = {
   {INFORM_EVENT_BOOTSTRAP,               "0 BOOTSTRAP"},
   {INFORM_EVENT_BOOT,                    "1 BOOT" },
   {INFORM_EVENT_PERIODIC,                "2 PERIODIC"},
   {INFORM_EVENT_SCHEDULED,               "3 SCHEDULED"},
   {INFORM_EVENT_VALUE_CHANGE,            "4 VALUE CHANGE"},
   {INFORM_EVENT_KICKED,                  "5 KICKED"},
   {INFORM_EVENT_CONNECTION_REQUEST,      "6 CONNECTION REQUEST"},
   {INFORM_EVENT_TRANSER_COMPLETE,        "7 TRANSFER COMPLETE"},
   {INFORM_EVENT_DIAGNOSTICS_COMPLETE,    "8 DIAGNOSTICS COMPLETE"},
   {INFORM_EVENT_REBOOT_METHOD,           "M Reboot"},
   {INFORM_EVENT_SCHEDULE_METHOD,         "M ScheduleInform"},
   {INFORM_EVENT_DOWNLOAD_METHOD,         "M Download"},
   {INFORM_EVENT_UPLOAD_METHOD,           "M Upload"},
   {INFORM_EVENT_REQUEST_DOWNLOAD,        "9 REQUEST DOWNLOAD"},
   {INFORM_EVENT_AUTON_TRANSFER_COMPLETE, "10 AUTONOMOUS TRANSFER COMPLETE"},
   {INFORM_EVENT_DU_CHANGE_COMPLETE, "11 DU STATE CHANGE COMPLETE"},
   {INFORM_EVENT_AUTON_DU_CHANGE_COMPLETE, "12 AUTONOMOUS DU STATE CHANGE COMPLETE"},
   {INFORM_EVENT_SCHEDULE_DOWNLOAD_METHOD, "M ScheduleDownload"},
   {INFORM_EVENT_CHANGE_DU_CHANGE_METHOD,  "M ChangeDUState"}
};

#define NUM_INFORM_EVT_STRING_TABLE_ENTRIES (sizeof(informEvtStringTable)/sizeof(struct informEvtTableEntry))

const char *getInformEvtString(UINT32 evt)
{
   UINT32 i;

   for (i=0; i < NUM_INFORM_EVT_STRING_TABLE_ENTRIES; i++)
   {
      if (informEvtStringTable[i].informEvt == evt)
      {
         return informEvtStringTable[i].str;
      }
   }

   cmsLog_error("Unsupported Inform Event value %d", evt);
   return "Internal Error (getInformEvtString)";
}

void setInformState(eInformState state)
{
   cmsLog_debug("set informState=%d", state);

   if (informState != state)
   {
      informState = state;
      saveTR69StatusItems();
   }
}  /* End of setInformState() */

void wrapperReboot(eInformState rebootContactValue)
{
   addInformEventToList(INFORM_EVENT_REBOOT_METHOD);

   setInformState(rebootContactValue);    /* reset on BcmCfm_download error */
   saveTR69StatusItems();

   cmsLog_notice("CPE is REBOOTING with rebootContactValue =%d", rebootContactValue);

   tr69SaveTransferList();
   wrapperReset();
}



/* Returns state of WAN interface to be used by tr69 client */
eWanState getWanState(void)
{
   /* add BCM shared library call here to determine status of outbound I/F*/
   /* mwang: even in the released 3.0.8 code, this function just returns
    * eWAN_ACTIVE. So in most places in the code, we will continue to use this
    * until we understand the implication of fixing it or removing it.
    * In the mean time, there is also a getRealWanState function below. */
   return eWAN_ACTIVE;
}

UBOOL8 matchAddrOnLanSide(const char *urlAddrString)
{
   UBOOL8 match=FALSE;
   char *ipAddrStr=NULL, *ipMaskStr=NULL;
   CmsRet ret;

   ret = getLanIPAddressInfo(&ipAddrStr,&ipMaskStr);
   
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_debug("one or more LAN_IP_CFG parameters are null");
   }
   else
   {
      struct in_addr urlAddr, ipAddr, mask;

      inet_aton(urlAddrString, &urlAddr);
      inet_aton(ipAddrStr, &ipAddr);
      inet_aton(ipMaskStr, &mask);

      if ((urlAddr.s_addr & mask.s_addr) == (ipAddr.s_addr & mask.s_addr))
      {
         cmsLog_debug("urlAddr %s is on LAN side", urlAddrString);
         match = TRUE;
      }
      else
      {
         cmsLog_debug("urlAddr %s is not on LAN side (%s/%s)",
                      urlAddrString,
                      ipAddrStr,
                      ipMaskStr);
      }
      cmsMem_free(ipAddrStr);
      cmsMem_free(ipMaskStr);
   }
   return match;
}

#ifdef DMP_DSLDIAGNOSTICS_1
/* Calls xdsl driver directly to get DSL loop diag's
 * results.  Results are stored in tr69c; so as soon as
 * this process exits, the results are all gone.
 * ACS is expected to initiate DSL loop diag and retrieve
 * the results shortly after that.
 * We cannot afford to store Loop Diagnostics results 
 * in MDM because they are too large.
 */
void getDslLoopDiagResultsAndLinkUp(UINT32 lineId)
{
   if (pDslLoopDiagResults == NULL)
   {
      pDslLoopDiagResults = (DslLoopDiagData*)cmsMem_alloc(sizeof(DslLoopDiagData),ALLOC_ZEROIZE);

      if (pDslLoopDiagResults == NULL)
      {
         cmsLog_error("unable to allocate memory to store DSL Loop results");
         return;
      }
      else
      {
         cmsLog_debug("allocated memory for loop result %d",sizeof(DslLoopDiagData));
      }
   }
   else
   {
      memset(pDslLoopDiagResults,0,sizeof(DslLoopDiagData));
   }
   rutWan_getAdslLoopDiagResultAndLinkUp(pDslLoopDiagResults,lineId);
}

CmsRet dslLoopDiag_getParamValue(char *paramName, char **paramValue)
{
   CmsRet ret = CMSRET_SUCCESS;
   char dataStr[32];
   UINT16 *ptr;
   cmsLog_debug("Enter: paramName %s",paramName);

   dataStr[0]='\0';
   if ((pDslLoopDiagResults == NULL) ||
       ((pDslLoopDiagResults != NULL)&& (pDslLoopDiagResults->testCompleted == FALSE)))
   {
      /* there is no change, don't do anything */
      return (ret);
   }
       
   if (strcmp(paramName,"LoopDiagnosticsState") == 0)
   {
      /* unchange, don't need to do anything. */
   }
   else if (strcmp(paramName,"ACTATPds") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->ACTATPds);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"ACTATPus") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->ACTATPus);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"HLINSCds") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->HLINSCds);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"SNRMTds") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->SNRMTds);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"SNRMTus") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->SNRMTus);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"QLNMTds") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->QLNMTds);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"QLNMTus") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->QLNMTus);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"HLOGMTds") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->HLOGMTds);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"HLOGMTus") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->HLOGMTus);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }

   else if (strcmp(paramName,"HLINGds") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->HLINGds);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"HLINGus") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->HLINGus);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"HLOGGds") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->HLOGGds);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"HLOGGus") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->HLOGGus);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"QLNGds") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->QLNGds);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"QLNGus") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->QLNGus);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"SNRGds") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->SNRGds);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"SNRGus") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->SNRGus);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"LATNpbds") == 0)
   {
      /* LATNpbds and LATNds are the same */
      ptr = &pDslLoopDiagResults->LATNds[0];
      sprintf(dataStr,"%d,%d,%d,%d,%d",ptr[0],ptr[1],ptr[2],ptr[3],ptr[4]);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"LATNpbus") == 0)
   {
      /* LATNpbus and LATNus are the same */
      ptr = &pDslLoopDiagResults->LATNus[0];
      sprintf(dataStr,"%d,%d,%d,%d,%d",ptr[0],ptr[1],ptr[2],ptr[3],ptr[4]);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"SATNds") == 0)
   {
      ptr = &pDslLoopDiagResults->SATNds[0];
      sprintf(dataStr,"%d,%d,%d,%d,%d",ptr[0],ptr[1],ptr[2],ptr[3],ptr[4]);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }   
   else if (strcmp(paramName,"SATNus") == 0)
   {
      ptr = &pDslLoopDiagResults->SATNus[0];
      sprintf(dataStr,"%d,%d,%d,%d,%d",ptr[0],ptr[1],ptr[2],ptr[3],ptr[4]);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"SNRMpbds") == 0)
   {
      sprintf(dataStr,"%.5d",(int)pDslLoopDiagResults->SNRMpbds);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"SNRMpbus") == 0)
   {
      sprintf(dataStr,"%.5d",(int)pDslLoopDiagResults->SNRMpbus);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }

   else if (strcmp(paramName,"HLINpsds") == 0)
   {
      ret = (cmsAdsl_formatHLINString(paramValue, pDslLoopDiagResults->HLINpsds,MAX_PS_STRING));
   }
   else if (strcmp(paramName,"HLINpsus") == 0)
   {
      ret = (cmsAdsl_formatHLINString(paramValue, pDslLoopDiagResults->HLINpsus,MAX_PS_STRING));
   }
   else if (strcmp(paramName,"QLNpsds") == 0)
   {
      ret = (cmsAdsl_formatSubCarrierDataString(paramValue,(void*)pDslLoopDiagResults->QLNpsds,(char*)"QLNpsds",MAX_QLN_STRING));
   }
   else if (strcmp(paramName,"QLNpsus") == 0)
   {
      ret = (cmsAdsl_formatSubCarrierDataString(paramValue,(void*)pDslLoopDiagResults->QLNpsus,(char*)"QLNpsus",MAX_QLN_STRING));
   }
   else if (strcmp(paramName,"SNRpsds") == 0)
   {
      ret = (cmsAdsl_formatSubCarrierDataString(paramValue,(void*)pDslLoopDiagResults->SNRpsds,(char*)"SNRpsds",MAX_PS_STRING));
   }
   else if (strcmp(paramName,"SNRpsus") == 0)
   {
      ret = (cmsAdsl_formatSubCarrierDataString(paramValue,(void*)pDslLoopDiagResults->SNRpsus,(char*)"SNRpsus",MAX_PS_STRING));
   }
   else if (strcmp(paramName,"HLOGpsds") == 0)
   {
      ret = (cmsAdsl_formatSubCarrierDataString(paramValue,(void*)pDslLoopDiagResults->HLOGpsds,(char*)"HLOGpsds",MAX_LOGARITHMIC_STRING));
   }
   else if (strcmp(paramName,"HLOGpsus") == 0)
   {
      ret = (cmsAdsl_formatSubCarrierDataString(paramValue,(void*)pDslLoopDiagResults->HLOGpsus,(char*)"HLOGpsus",MAX_LOGARITHMIC_STRING));
   }
   else if (strcmp(paramName,"BITSpsds") == 0)
   {
      ret = (cmsAdsl_formatSubCarrierDataString(paramValue,(void*)pDslLoopDiagResults->BITSpsds,(char*)"BITSpsds",MAX_PS_STRING));
   }
   else if (strcmp(paramName,"GAINSpsds") == 0)
   {
      ret = (cmsAdsl_formatPertoneGroupQ4String(paramValue,(void**)pDslLoopDiagResults->GAINSpsds,MAX_PS_STRING));
   }

   cmsLog_debug("End: paramValue %s",paramValue);
   return ret;
}

void freeDslLoopDiagResults(void)
{
   if (pDslLoopDiagResults != NULL)
   {
      cmsMem_free(pDslLoopDiagResults);
   }
}

#endif /* DMP_DSLDIAGNOSTICS_1 */



