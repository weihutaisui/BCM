/***********************************************************************
 *
 *  Copyright (c) 2008  Broadcom Corporation
 *  All Rights Reserved
 *
 <:label-BRCM:2011:proprietary:standard
 
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


#ifdef SUPPORT_QUICKSETUP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/syslog.h>
#include <net/if_arp.h>
#include <net/route.h>
#include "cgi_cmd.h"
#include "cgi_ntwk.h"
#include "cgi_main.h"
#include "syscall.h"
#include "sysdiag.h"
#include "httpd.h"
#include "cms_util.h"
#include "cgi_main.h"
#include "cms_dal.h"
#include "cms_boardcmds.h"
#include "cgi_util.h"
#include "devctl_xtm.h"

#define MAX_PPP_AUTH_RETRIES 5

extern UBOOL8 glbQuickSetupEnabled;
extern int wlgetVirtIntfNo( int idx );
/* Keep #define WL_MAX_NUM_SSID here for Mbss Tag */

struct atmIface_s{
   UINT16 vpi;
   UINT16 vci;
};

struct atmIface_s atmIfaceList[] = {{9,35},{3,33},{0,35},{0,36}};

#define MAX_ATM_LINKS (sizeof(atmIfaceList)/sizeof(struct atmIface_s))


static CmsRet cgiSkipQuickSetup(void)
{
   HttpdCfgObject *obj;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   glbQuickSetupEnabled = FALSE;
   /*set in MDM */
   if ((ret = cmsObj_get(MDMOID_HTTPD_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of HTTPD_CFG object failed, ret=%d", ret);
   }
   else
   {
      obj->quickSetupEnabled = glbQuickSetupEnabled;

      if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("set of HTTPD_CFG object failed, ret=%d", ret);
      }

      cmsObj_free((void **) &obj);
   }
   return ret;

}

CmsRet cgiAutoAtmAdd_igd(char *query __attribute__((unused)),
                         FILE *fs __attribute__((unused)))
{
#ifdef DMP_ADSLWAN_1
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack1 = EMPTY_INSTANCE_ID_STACK;
   WanPppConnObject     *wanpppObj = NULL;           
   CmsRet ret = CMSRET_SUCCESS;
   UINT32 i;
   int authRetries=0;
   UBOOL8 linkActive = FALSE;
   UBOOL8 authSucc = TRUE;

   cmsLog_debug("--Entered---");
   glbWebVar.quicksetupErrCode = 0; 

   glbWebVar.portId = 0;/*path0*/
   cmsUtl_strncpy(glbWebVar.linkType,"EoA", sizeof(glbWebVar.linkType));/*EoA*/
   glbWebVar.encapMode = 0;/*LLC/SNAP-BRIDGING */
   glbWebVar.connMode = 0;/*Single Service*/
   cmsUtl_strncpy(glbWebVar.atmServiceCategory,"UBR", sizeof(glbWebVar.atmServiceCategory));/*UBR with out PCR*/
   glbWebVar.atmPeakCellRate = 0; /*0 for UBR*/
   glbWebVar.atmSustainedCellRate = 0; /*0 for UBR*/
   glbWebVar.atmMaxBurstSize = 0; /*0 for UBR*/
   glbWebVar.enblQos = 0; /*disabled*/

   for(i=0; i < MAX_ATM_LINKS; i++){
      glbWebVar.atmVpi = atmIfaceList[i].vpi;
      glbWebVar.atmVci = atmIfaceList[i].vci;

      if (dalDsl_addAtmInterface(&glbWebVar) != CMSRET_SUCCESS)
      {
         glbWebVar.quicksetupErrCode = QS_ATM_ADD_ERROR; 
         do_ej("/webs/quicksetuptesterr.html", fs);
         return CMSRET_INTERNAL_ERROR;
      }
      else
      {
         WanAtm5LoopbackDiagObject *atm5DiagObj;

         /*atm interface added check if the link is up */

         INIT_INSTANCE_ID_STACK(&iidStack);
         if (dalDsl_getDslLinkByIfName("atm0", &iidStack, NULL) == FALSE)
         {
            cmsLog_debug("Interface %s not found.", "atm0");
            glbWebVar.quicksetupErrCode = QS_ATM_ADD_ERROR; 
            do_ej("/webs/quicksetuptesterr.html", fs);
            return CMSRET_INTERNAL_ERROR;
         }

         cmsLog_debug("iidstack =%s",cmsMdm_dumpIidStack(&iidStack));
         iidStack1 =iidStack;

         if ((ret = cmsObj_get(MDMOID_WAN_ATM5_LOOPBACK_DIAG, &iidStack1, 0, (void **) &atm5DiagObj)) != CMSRET_SUCCESS)
         {
            cmsLog_error("failed to get Atm5LoopbackDiag ret=%d", ret);
            cmsObj_deleteInstance(MDMOID_WAN_CONN_DEVICE, &iidStack);

            glbWebVar.quicksetupErrCode = QS_ATM_ADD_ERROR; 
            do_ej("/webs/quicksetuptesterr.html", fs);
            return ret;
         }

         /* request to perform link diagnostics */
         REPLACE_STRING_IF_NOT_EQUAL(atm5DiagObj->diagnosticsState, MDMVS_REQUESTED);
         atm5DiagObj->numberOfRepetitions = 1;
         atm5DiagObj->timeout = 1000  /* msec */;


         if ((ret = cmsObj_set(atm5DiagObj, &iidStack1)) != CMSRET_SUCCESS)
         {
            cmsLog_error("failed to set Atm5LoopbackDiag ret=%d", ret);
            cmsObj_free((void **) &atm5DiagObj);
            cmsObj_deleteInstance(MDMOID_WAN_CONN_DEVICE, &iidStack);
            glbWebVar.quicksetupErrCode = QS_ATM_ADD_ERROR; 
            do_ej("/webs/quicksetuptesterr.html", fs);
            return ret;
         }

         cmsObj_free((void **) &atm5DiagObj);

         /* check the results of diagnosis */

         if ((ret = cmsObj_get(MDMOID_WAN_ATM5_LOOPBACK_DIAG, &iidStack1, 0, (void **) &atm5DiagObj)) != CMSRET_SUCCESS)
         {
            cmsLog_error("failed to get Atm5LoopbackDiag ret=%d", ret);
            cmsObj_free((void **) &atm5DiagObj);
            cmsObj_deleteInstance(MDMOID_WAN_CONN_DEVICE, &iidStack);
            glbWebVar.quicksetupErrCode = QS_ATM_ADD_ERROR; 
            do_ej("/webs/quicksetuptesterr.html", fs);
            return ret;
         }
         if (atm5DiagObj->successCount == atm5DiagObj->numberOfRepetitions)
         {
            linkActive = TRUE;
            cmsObj_free((void **) &atm5DiagObj);
            break;
         }
         else
         {
            /* remove the WAN_CONNECTION_DEVICE */
            cmsObj_deleteInstance(MDMOID_WAN_CONN_DEVICE, &iidStack);
            cmsObj_free((void **) &atm5DiagObj);
            cmsLog_debug("link not active for vpi/vci:%d/%d",atmIfaceList[i].vpi,atmIfaceList[i].vci);
         }


      }
   }

   if(linkActive == TRUE )
   {

      cmsLog_debug("dalDsl_addAtmInterface ok.");

      /* ADD ppp/IP connection */

      cmsUtl_strncpy(glbWebVar.wanL2IfName,"atm0/",sizeof(glbWebVar.wanL2IfName));
      glbWebVar.ntwkPrtcl = PROTO_PPPOE;

      cmsUtl_strncpy(glbWebVar.serviceName,"ppoe_atm0",sizeof(glbWebVar.serviceName));/*use proper service name */

      /* set ppp username /password  from the quicksetup page*/
      cgiGetValueByName(query, "pppUserName", glbWebVar.pppUserName);
      cgiGetValueByName(query, "pppPassword", glbWebVar.pppPassword);



      glbWebVar.enblOnDemand = 0;
      glbWebVar.pppTimeOut = 0;

      glbWebVar.useStaticIpAddress = 0;
      cmsUtl_strncpy(glbWebVar.pppLocalIpAddress, "0.0.0.0",sizeof(glbWebVar.pppLocalIpAddress));

      glbWebVar.pppIpExtension = 0;
      glbWebVar.enblFirewall = 1;
      glbWebVar.enblNat = 1;

      glbWebVar.enblFullcone = 1;
      glbWebVar.pppAuthMethod = 0;/*auto*/
      glbWebVar.pppAuthErrorRetry = 0;
      glbWebVar.enblPppDebug = 0;
#ifdef DMP_X_BROADCOM_COM_IGMP_1
      glbWebVar.enblIgmp = 0;
      glbWebVar.enblIgmpMcastSource = 0;
#endif
#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
      glbWebVar.noMcastVlanFilter = 0;
#endif
#ifdef DMP_X_BROADCOM_COM_MLD_1
      glbWebVar.enblMld = 0;
      glbWebVar.enblMldMcastSource = 0;
#endif
      cmsUtl_strncpy(glbWebVar.defaultGatewayList,"ppp0",sizeof(glbWebVar.defaultGatewayList));
      /*if static DNS is to be used set dnsIfcsList to null and dnsPrimary and secondary to proper values*/
      cmsUtl_strncpy(glbWebVar.dnsIfcsList,"ppp0",sizeof(glbWebVar.dnsIfcsList));
      cmsUtl_strncpy(glbWebVar.dnsPrimary,"0.0.0.0",sizeof(glbWebVar.dnsPrimary));
      cmsUtl_strncpy(glbWebVar.dnsSecondary,"0.0.0.0",sizeof(glbWebVar.dnsSecondary));

      if ((ret = dalWan_addService(&glbWebVar)) != CMSRET_SUCCESS)
      {
         cmsLog_error("dalWan_addInterface failed, ret=%d", ret);
         cmsObj_deleteInstance(MDMOID_WAN_CONN_DEVICE, &iidStack);
         glbWebVar.quicksetupErrCode = QS_PPP_ADD_ERROR; 
         do_ej("/webs/quicksetuptesterr.html", fs);
         return ret;
      }
      /*handle authentication failure */

      PUSH_INSTANCE_ID(&iidStack1,1);

      while(authRetries <= MAX_PPP_AUTH_RETRIES){

         cmsLck_releaseLock();
         authRetries++;
         sleep(3);
         if ((ret = cmsLck_acquireLockWithTimeout(HTTPD_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
         {
            cmsLog_error("could not acquire lock, ret=%d", ret);
            continue;/*try again*/
         }


         if ((ret = cmsObj_get(MDMOID_WAN_PPP_CONN, &iidStack1, 0, (void **)&wanpppObj)) != CMSRET_SUCCESS)
         {
            cmsObj_deleteInstance(MDMOID_WAN_CONN_DEVICE, &iidStack);
            glbWebVar.quicksetupErrCode = QS_PPP_ADD_ERROR; 
            do_ej("/webs/quicksetuptesterr.html", fs);
            return CMSRET_INTERNAL_ERROR;

         }

         if( cmsUtl_strcmp(wanpppObj->lastConnectionError,MDMVS_ERROR_AUTHENTICATION_FAILURE) == 0)
         {
            authSucc = FALSE;
            cmsObj_free((void **) &wanpppObj);
            break;
         }
         else if(cmsUtl_strcmp(wanpppObj->connectionStatus, MDMVS_CONNECTED) == 0){
            authSucc = TRUE;
            cmsObj_free((void **) &wanpppObj);
            break;
         }

         cmsObj_free((void **) &wanpppObj);
      }

      /*if for some reason(ex:PPP server is down)the PPP negatotiation has not yet started at this point
       * we consider it as authentication success and proceed 
       */

      if(authSucc == FALSE)
      {
         cmsLog_debug("PPP authentication failed ");
         cmsObj_deleteInstance(MDMOID_WAN_PPP_CONN, &iidStack1);
         cmsObj_deleteInstance(MDMOID_WAN_CONN_DEVICE, &iidStack);
         glbWebVar.quicksetupErrCode = QS_PPP_AUTH_ERROR; 
         do_ej("/webs/quicksetuptesterr.html", fs);
         return CMSRET_INTERNAL_ERROR;
      } 


#ifdef BRCM_WLAN
      /* Setup  Wireless config */
      {
         char tmpBuf[2048]; 
         char ssid[64];
         char *p;
         int Wl_Max_Num_Ssid = wlgetVirtIntfNo(0);

         cgiGetValueByName(query, "wlSsid", ssid);

         /*currently we set only ssid, chage oter variables as necessary ex:wlCountry */
         sprintf(tmpBuf,"/webs/quicksetup.html?wlSsidIdx=0&wlEnbl=1&wlHide=0&wlAPIsolation=0&wlSsid=%s&wlCountry=US&wlMaxAssoc=16&wlDisableWme=0&wlEnableWmf=0",ssid);
         for (i=1; i<(UINT32)Wl_Max_Num_Ssid; i++) {
            p = &tmpBuf[strlen(tmpBuf)];
            sprintf(p,"&wlEnbl_wl0v%d=0&wlSsid_wl0v%d=wl0_Guest%d&wlHide_wl0v%d=0&wlAPIsolation_wl0v%d=0&wlDisableWme_wl0v%d=0&wlEnableWmf_wl0v%d=0&wlMaxAssoc_wl0v%d=16", i, i, i, i, i, i, i, i);
         }

         do_wl_cgi(tmpBuf, fs);

         /*handle failure case of WLAN */
         if(glbWebVar.quicksetupErrCode == QS_WLAN_ADD_ERROR)
         {
            cmsObj_deleteInstance(MDMOID_WAN_PPP_CONN, &iidStack1);
            cmsObj_deleteInstance(MDMOID_WAN_CONN_DEVICE, &iidStack);
            do_ej("/webs/quicksetuptesterr.html", fs);
            return CMSRET_INTERNAL_ERROR;

         }
      }
#endif /* BRCM_WLAN */
   }
   else
   {
      glbWebVar.quicksetupErrCode = QS_ATM_ADD_ERROR; 
      do_ej("/webs/quicksetuptesterr.html", fs);
      return CMSRET_INTERNAL_ERROR;

   }


   /*quick setup successful,skip it from next time */
   cgiSkipQuickSetup();
   /*
    * dsl intf add was successful, tell handle_request to save the config
    * before releasing the lock.
    */
   glbSaveConfigNeeded = TRUE;
   do_ej("/webs/quicksetuptestsucc.html", fs);
   return CMSRET_SUCCESS;
#else
   /* if a none DSL board has quick setup enable, just do nothing */
   cgiSkipQuickSetup();
   return CMSRET_SUCCESS;
#endif /* ADSL_WAN */

}


void cgiAutoAtmCfg(char *query, FILE *fs) 
{
   char action[BUFLEN_256];

   cmsLog_debug("--Entered---");
   cgiGetValueByName(query, "action", action);

   if (cmsUtl_strcmp(action, "add") == 0)
   {
      if (cgiAutoAtmAdd(query, fs) != CMSRET_SUCCESS) 
      {
         return;
      }
   }
   else if (cmsUtl_strcmp(action, "skip") == 0)
   {
      /*skip the quick setup */

      if (cgiSkipQuickSetup() == CMSRET_SUCCESS)
      {
         glbSaveConfigNeeded = TRUE;
         do_ej("/webs/main.html", fs);
      }

      return;
   }
}
#endif  /*  SUPPORT_QUICKSETUP */

