/***********************************************************************
 *
 *  Copyright (c) 2006-2010  Broadcom Corporation
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

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "dal_network.h"
#include "devctl_atm.h"

#ifdef DMP_IPPING_1
/* This routine is called with Ping diagnostics parameters.
 * It goes set the IPPingDiagObject, which in turns causes a message to be sent to PING 
 * to start doing ping with the input parameters.
 */
CmsRet dalDiag_startStopPing_igd(IPPingDiagObject *pingParms)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   IPPingDiagObject *pingDiag;
   CmsRet ret;

   cmsLog_debug("Enter: pingParms->diagnosticsState %s",pingParms->diagnosticsState);

   if ((ret = cmsObj_get(MDMOID_IP_PING_DIAG, &iidStack, 0, (void **) &pingDiag)) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(pingParms->diagnosticsState, MDMVS_REQUESTED) == 0)
      {
         /* function is called to start a ping test */
         if (cmsUtl_strcmp(pingDiag->diagnosticsState, MDMVS_REQUESTED) == 0)
         {
            
            cmsLog_debug("MDM is already in Resquested state, deny request");
            
            /* a ping is in process, return error */
            ret = CMSRET_REQUEST_DENIED;
         }
         else 
         {
            
            cmsLog_debug("Doing ping now");

            cmsMem_free(pingDiag->diagnosticsState);
            pingDiag->diagnosticsState = cmsMem_strdup(MDMVS_REQUESTED);
            cmsMem_free(pingDiag->interface);
            pingDiag->interface = cmsMem_strdup(pingParms->interface);
            cmsMem_free(pingDiag->host);
            pingDiag->host = cmsMem_strdup(pingParms->host);
            pingDiag->numberOfRepetitions = pingParms->numberOfRepetitions;
            pingDiag->timeout = pingParms->timeout;
            pingDiag->dataBlockSize = pingParms->dataBlockSize;
            pingDiag->DSCP = pingParms->DSCP;

            cmsLog_debug("Calling cmsObj_set");

            ret = cmsObj_set(pingDiag, &iidStack);

            cmsLog_debug("Returned from cmsObj_set, ret %d", ret);

            if (ret != CMSRET_SUCCESS)
            {
               cmsMem_free(pingDiag->diagnosticsState);
               /* needs to be "ERROR_INTERNAL" */
               pingDiag->diagnosticsState = cmsMem_strdup(MDMVS_ERROR);
               ret = cmsObj_set(pingDiag, &iidStack);
               cmsLog_debug("set diagnosticsState to Error if failed to start Ping, ret %d", ret);            
            }
         } /* pingDiag is not running */
      } /* not requested */
      else if ((cmsUtl_strcmp(pingDiag->diagnosticsState, MDMVS_REQUESTED) == 0) &&
               (cmsUtl_strcmp(pingParms->diagnosticsState, MDMVS_NONE) == 0))
      {
         /* stop ping test request */
         cmsMem_free(pingDiag->diagnosticsState);
         pingDiag->diagnosticsState = cmsMem_strdup(MDMVS_NONE);
         ret = cmsObj_set(pingDiag, &iidStack);
      }
      cmsObj_free((void **) &pingDiag);
   } /* get obj success */
   
   return (ret);
}

CmsRet dalDiag_getPingResult_igd(void *pingResult)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   IPPingDiagObject *pingDiag;
   IPPingDiagObject *pResult = (IPPingDiagObject*)pingResult;
   CmsRet ret;
   if ((ret = cmsObj_get(MDMOID_IP_PING_DIAG, &iidStack, 0, (void **) &pingDiag)) == CMSRET_SUCCESS)
   {
      /* when getting results, we are interested in some parameters */
      pResult->diagnosticsState = cmsMem_strdup(pingDiag->diagnosticsState);
      pResult->successCount = pingDiag->successCount;
      pResult->failureCount = pingDiag->failureCount;
      pResult->averageResponseTime = pingDiag->averageResponseTime;
      pResult->maximumResponseTime = pingDiag->maximumResponseTime;
      pResult->minimumResponseTime = pingDiag->minimumResponseTime;
      cmsObj_free((void **) &pingDiag);
   }
   
   return (ret);
}
#endif /* DMP_IPPING_1 */

#ifdef DMP_ADSLWAN_1

/* call devctl to start OAM test on ATM driver, waits for status to return.
 * another way to do this without blocking is to add a MDM object for OAM test data;
 * this routine would just read the OAM test result.   This means SSK would need to
 * get a signal at the kernel fd to read the OAM test results from the driver.  ATM 
 * driver needs to keep a copy of the most recent test data, and also wakeup_monitor_task
 * to ask SSK to read the new results.   This latter approach is avoided since httpd needs 
 * to display the result right away when users decides to run this test on WEBUI.
 */
CmsRet dalDiag_doOamLoopback_igd(int type, PPORT_VPI_VCI_TUPLE addr, UINT32 repetition, UINT32 timeout)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanDslLinkCfgObject *dslLinkCfg=NULL;
   WanAtm5LoopbackDiagObject *obj;
   CmsRet ret = CMSRET_INVALID_ARGUMENTS;
   SINT32 vpi, vci;
   int found=FALSE;
   MdmObjectId oid;

   cmsLog_debug("Enter: type %d, vpi/vci %d/%d repetition %d, timeout %d",
                type, addr->vpi, addr->vci, repetition, timeout);

   /* Get the iidStack based on PORT/VPI/VCI */
   while ((!found) && 
          (cmsObj_getNext(MDMOID_WAN_DSL_LINK_CFG, &iidStack, (void **) &dslLinkCfg) == CMSRET_SUCCESS))
   {
      cmsUtl_atmVpiVciStrToNum(dslLinkCfg->destinationAddress, &vpi, &vci);

      if ((dslLinkCfg->X_BROADCOM_COM_ATMInterfaceId == (UINT32) addr->port) &&
          (vpi == addr->vpi) && (vci == addr->vci))
      {
         found = TRUE;

         cmsLog_debug("found DSL config port/vpi/vci %d/%d/%d: iidStack %s",addr->port,vpi,vci,
                      cmsMdm_dumpIidStack(&iidStack));
      }
      cmsObj_free((void **) &dslLinkCfg);
   }
   
   if (found)
   {
      cmsLog_debug("iidStack %s for vpi/vci %d/%d",cmsMdm_dumpIidStack(&iidStack),
                   vpi,vci);

      switch (type)
      {
      case OAM_LB_SEGMENT_TYPE:
         oid = MDMOID_WAN_ATM5_LOOPBACK_DIAG;
         break;
      case OAM_LB_END_TO_END_TYPE:
         oid = MDMOID_WAN_ATM_F5_END_TO_END_LOOPBACK_DIAG;
         break;
      case OAM_F4_LB_SEGMENT_TYPE:
         oid = MDMOID_WAN_ATM_F4_LOOPBACK_DIAG;
         break;
      case OAM_F4_LB_END_TO_END_TYPE:
         oid = MDMOID_WAN_ATM_F4_END_TO_END_LOOPBACK_DIAG;
         break;
      default:
         oid=0;  // supress compiler warning (__builtin_unreachable() not available in 4.4.2 compiler)
         break;
      }

      if ((ret = cmsObj_get(oid, &iidStack, 0, (void **) &obj)) == CMSRET_SUCCESS)
      {
         if (cmsUtl_strcmp(obj->diagnosticsState, MDMVS_REQUESTED) == 0)
         {
            cmsLog_debug("MDM is already in Resquested state, deny request");
            
            /* a f5 test is in progress, return error */
            cmsObj_free((void **) &obj);
            ret = CMSRET_REQUEST_DENIED;
         }
         else 
         {
            cmsLog_debug("Doing OAM loopback request now");

            cmsMem_free(obj->diagnosticsState);
            obj->diagnosticsState = cmsMem_strdup(MDMVS_REQUESTED);
            if (repetition == 0)
            {
               obj->numberOfRepetitions = OAM_LOOPBACK_DEFAULT_REPETITION;
            }
            else 
            {
               obj->numberOfRepetitions = repetition;
            }
            if (timeout == 0)
            {
               obj->timeout = OAM_LOOPBACK_DEFAULT_TIMEOUT;
            }
            else
            {
               obj->timeout = timeout;
            }

            cmsLog_debug("Calling cmsObj_set");

            ret = cmsObj_set(obj, &iidStack);
            cmsObj_free((void **) &obj);
            /* set is successful get the results */
            if (cmsObj_get(oid, &iidStack, 0, (void **) &obj) == CMSRET_SUCCESS)
            {
               if (obj->successCount == obj->numberOfRepetitions)
               {
                  ret = CMSRET_SUCCESS;
               }
               else
               {
                  ret = CMSRET_INTERNAL_ERROR;
               }
               cmsObj_free((void **) &obj);
            } /* get object */
         } /* diag is not running */
      } /* not requested */
   } /* get obj success */
   
   return (ret);
}

#endif /* DMP_ADSLWAN_1 */

#ifdef DMP_X_BROADCOM_COM_ETHERNETOAM_1
void dalEthOam_setDefaultCfg(WEB_NTWK_VAR *pWebVar)
{
   pWebVar->eoam3ahEnbl = FALSE;
   pWebVar->eoam3ahIntf[0] = '\0';
   pWebVar->eoam3ahOamId = 1;
   pWebVar->eoam3ahAeEnbl = TRUE;
   pWebVar->eoam3ahVrEnbl = TRUE;
   pWebVar->eoam3ahLeEnbl = TRUE;
   pWebVar->eoam3ahRlEnbl = TRUE;
   pWebVar->eoam3ahAmEnbl = TRUE;
   pWebVar->eoam1agEnbl = FALSE;
   pWebVar->eoam1731Enbl = FALSE;
   pWebVar->eoam1agMdId[0] = '\0';
   pWebVar->eoam1agMdLvl = 0;
   pWebVar->eoam1agMaId[0] = '\0';
   pWebVar->eoam1agCcmInterval = 4;
   pWebVar->eoam1agLocIntf[0] = '\0';
   pWebVar->eoam1agLocMepId = 1;
   pWebVar->eoam1agLocVlanId = -1;
   pWebVar->eoam1agLocCcmEnbl = FALSE;
   pWebVar->eoam1agRemMepId = -1;
}

void dalEthOam_getAllCfg(WEB_NTWK_VAR *pWebVar)
{
   InstanceIdStack ieee8023ahCfgIidStack;
   Ieee8023ahCfgObject *ieee8023ahCfg = NULL;

   InstanceIdStack ieee8021agCfgIidStack;
   Ieee8021agCfgObject *ieee8021agCfg = NULL;

   InstanceIdStack localMepIidStack;
   LocalMepObject *localMep = NULL;

   InstanceIdStack remoteMepIidStack;
   RemoteMepObject *remoteMep = NULL;

   CmsRet ret = CMSRET_SUCCESS;

   dalEthOam_setDefaultCfg(pWebVar);
  
   /* MDMOID_IEEE8023AH_CFG */
   INIT_INSTANCE_ID_STACK(&ieee8023ahCfgIidStack);
   if ((ret = cmsObj_get(MDMOID_IEEE8023AH_CFG, &ieee8023ahCfgIidStack, 0, (void **)&ieee8023ahCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of MDMOID_IEEE8023AH_CFG failed, ret=%d", ret);
      return;
   }
   
   pWebVar->eoam3ahEnbl = ieee8023ahCfg->enabled;
   if(ieee8023ahCfg->ifName){
      strncpy(pWebVar->eoam3ahIntf, ieee8023ahCfg->ifName, sizeof(pWebVar->eoam3ahIntf));
   }
   pWebVar->eoam3ahOamId = ieee8023ahCfg->oamId;
   pWebVar->eoam3ahAeEnbl = ieee8023ahCfg->autoEventEnabled;
   pWebVar->eoam3ahVrEnbl = ieee8023ahCfg->variableRetrievalEnabled;
   pWebVar->eoam3ahLeEnbl = ieee8023ahCfg->linkEventsEnabled;
   pWebVar->eoam3ahRlEnbl = ieee8023ahCfg->remoteLoopbackEnabled;
   pWebVar->eoam3ahAmEnbl = ieee8023ahCfg->activeModeEnabled;
   cmsObj_free((void **) &ieee8023ahCfg);

   /* MDMOID_IEEE8021AG_CFG */
   INIT_INSTANCE_ID_STACK(&ieee8021agCfgIidStack);
   if ((ret = cmsObj_get(MDMOID_IEEE8021AG_CFG, &ieee8021agCfgIidStack, 0, (void **)&ieee8021agCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get <MDMOID_IEEE8021AG_CFG> returns error. ret=%d", ret);
      return;
   }
   pWebVar->eoam1agEnbl = ieee8021agCfg->enabled;
   pWebVar->eoam1731Enbl = ieee8021agCfg->Y1731Enabled;
   if(ieee8021agCfg->mdId){
      strncpy(pWebVar->eoam1agMdId, ieee8021agCfg->mdId, sizeof(pWebVar->eoam1agMdId));
   }
   pWebVar->eoam1agMdLvl = ieee8021agCfg->mdLevel;
   if(ieee8021agCfg->maId){
      strncpy(pWebVar->eoam1agMaId, ieee8021agCfg->maId, sizeof(pWebVar->eoam1agMaId));
   }
   pWebVar->eoam1agCcmInterval = ieee8021agCfg->ccmInterval;
   cmsObj_free((void **) &ieee8021agCfg);

   /* MDMOID_LOCAL_MEP */
   INIT_INSTANCE_ID_STACK(&localMepIidStack);
   if ((ret = cmsObj_getNext(MDMOID_LOCAL_MEP, &localMepIidStack, (void **)&localMep)) == CMSRET_SUCCESS)
   {
      if(localMep->ifName){
         strncpy(pWebVar->eoam1agLocIntf, localMep->ifName, sizeof(pWebVar->eoam1agLocIntf));
      }
      pWebVar->eoam1agLocMepId = localMep->mepId;

      pWebVar->eoam1agLocVlanId = localMep->vlanId;
      pWebVar->eoam1agLocCcmEnbl = localMep->ccmEnabled;
      cmsObj_free((void **) &localMep);
   }

   /* MDMOID_REMOTE_MEP */
   INIT_INSTANCE_ID_STACK(&remoteMepIidStack);
   if ((ret = cmsObj_getNext(MDMOID_REMOTE_MEP, &remoteMepIidStack, (void **)&remoteMep)) == CMSRET_SUCCESS)
   {
      pWebVar->eoam1agRemMepId = remoteMep->mepId;
      cmsObj_free((void **) &remoteMep); 
   }
}
#endif

