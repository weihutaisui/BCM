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
#ifdef DMP_DEVICE2_BASELINE_1

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "dal_network.h"
#include "devctl_atm.h"
#include "cms_qdm.h"

#ifdef DMP_DEVICE2_IPPING_1
/* This routine is called with Ping diagnostics parameters.
 * It goes set the IPPingDiagObject, which in turns causes a message to be sent to PING 
 * to start doing ping with the input parameters.
 */
CmsRet dalDiag_startStopPing_dev2(Dev2IpPingDiagObject *pingParms)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2IpPingDiagObject *pingDiag;
   CmsRet ret;

   cmsLog_debug("Enter: pingParms->diagnosticsState %s",pingParms->diagnosticsState);

   if ((ret = cmsObj_get(MDMOID_DEV2_IP_PING_DIAG, &iidStack, 0, (void **) &pingDiag)) == CMSRET_SUCCESS)
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

CmsRet dalDiag_getPingResult_dev2(void *pingResult)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2IpPingDiagObject *pingDiag;
   Dev2IpPingDiagObject *pResult = (Dev2IpPingDiagObject *)pingResult;
   CmsRet ret;
   if ((ret = cmsObj_get(MDMOID_DEV2_IP_PING_DIAG, &iidStack, 0, (void **) &pingDiag)) == CMSRET_SUCCESS)
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
#endif /* DMP_DEVICE2_IPPING_1 */

#ifdef DMP_DEVICE2_ATMLOOPBACK_1
/* call devctl to start OAM test on ATM driver, waits for status to return.
 * another way to do this without blocking is to add a MDM object for OAM test data;
 * this routine would just read the OAM test result.   This means SSK would need to
 * get a signal at the kernel fd to read the OAM test results from the driver.  ATM 
 * driver needs to keep a copy of the most recent test data, and also wakeup_monitor_task
 * to ask SSK to read the new results.   This latter approach is avoided since httpd needs 
 * to display the result right away when users decides to run this test on WEBUI.
 */
CmsRet dalDiag_doOamLoopback_dev2(int type, PPORT_VPI_VCI_TUPLE addr, UINT32 repetition, UINT32 timeout)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2AtmLinkObject *atmLinkObj = NULL;   
   Dev2AtmDiagnosticsF5LoopbackObject *atmDiagObj; /* it is really any loopback object */
   CmsRet ret = CMSRET_INVALID_ARGUMENTS;
   SINT32 vpi, vci;
   int found=FALSE;
   MdmObjectId oid;
   char *atmLinkFullPathBuf=NULL;

   cmsLog_debug("Enter: type %d, vpi/vci %d/%d, port %d, repetition %d, timeout %d",
                type, addr->vpi, addr->vci, addr->port,repetition, timeout);

   /* Get the iidStack based on PORT/VPI/VCI */
   while (!found && (ret = cmsObj_getNext(MDMOID_DEV2_ATM_LINK, &iidStack, (void **) &atmLinkObj)) == CMSRET_SUCCESS)
   {
      /* basically, we need to look at all the ATM_LINK connections, and see if anyone is using this tdte index */
      if ((ret = cmsUtl_atmVpiVciStrToNum_dev2(atmLinkObj->destinationAddress, &vpi, &vci)) == CMSRET_SUCCESS)
      {
         if ((atmLinkObj->X_BROADCOM_COM_ATMInterfaceId == (UINT32) addr->port) &&
             (vpi==addr->vpi) && (vci == addr->vci))
         {
            cmsLog_debug("found port/vpi/vci %d/%d/%d: iidStack %s",addr->port,vpi,vci,
                         cmsMdm_dumpIidStack(&iidStack));
            if (qdmIntf_intfnameToFullPathLocked(atmLinkObj->name,TRUE,&atmLinkFullPathBuf) == CMSRET_SUCCESS)
            {
               found = TRUE;
            }
         }
      }
      cmsObj_free((void **) &atmLinkObj);
   }
   
   if (found)
   {
      cmsLog_debug("iidStack %s for vpi/vci %d/%d",cmsMdm_dumpIidStack(&iidStack),
                   vpi,vci);

      switch (type)
      {
      case OAM_LB_SEGMENT_TYPE:
         oid = MDMOID_DEV2_ATM_DIAGNOSTICS_F5_LOOPBACK;
         break;
      case OAM_LB_END_TO_END_TYPE:
         oid = MDMOID_DEV2_ATM_DIAGNOSTICS_F5_END_TO_END_LOOPBACK;
         break;
      case OAM_F4_LB_SEGMENT_TYPE:
         oid = MDMOID_DEV2_ATM_DIAGNOSTICS_F4_LOOPBACK;
         break;
      case OAM_F4_LB_END_TO_END_TYPE:
         oid = MDMOID_DEV2_ATM_DIAGNOSTICS_F4_END_TO_END_LOOPBACK;
         break;
      default:
         oid=0;  // supress compiler warning (__builtin_unreachable() not available in 4.4.2 compiler)
         break;
      }
      INIT_INSTANCE_ID_STACK(&iidStack);
      /* there is only one Diagnostics Instance for all ATM link in the data model. Diag is only for one link at a time */
      if ((ret = cmsObj_get(oid, &iidStack, 0, (void **) &atmDiagObj)) == CMSRET_SUCCESS)
      {
         if (cmsUtl_strcmp(atmDiagObj->diagnosticsState, MDMVS_REQUESTED) == 0)
         {
            cmsLog_debug("MDM is already in Resquested state, deny request");
            
            /* a f5 test is in progress, return error */
            cmsObj_free((void **) &atmDiagObj);
            ret = CMSRET_REQUEST_DENIED;
         }
         else 
         {
            cmsLog_debug("Doing OAM loopback request now");

            cmsMem_free(atmDiagObj->diagnosticsState);
            atmDiagObj->diagnosticsState = cmsMem_strdup(MDMVS_REQUESTED);
            CMSMEM_REPLACE_STRING(atmDiagObj->interface,atmLinkFullPathBuf);

            if (repetition == 0)
            {
               atmDiagObj->numberOfRepetitions = OAM_LOOPBACK_DEFAULT_REPETITION;
            }
            else 
            {
               atmDiagObj->numberOfRepetitions = repetition;
            }
            if (timeout == 0)
            {
               atmDiagObj->timeout = OAM_LOOPBACK_DEFAULT_TIMEOUT;
            }
            else
            {
               atmDiagObj->timeout = timeout;
            }

            cmsLog_debug("Calling cmsObj_set");

            ret = cmsObj_set(atmDiagObj, &iidStack);
            cmsObj_free((void **) &atmDiagObj);
            /* set is successful get the results */
            if (cmsObj_get(oid, &iidStack, 0, (void **) &atmDiagObj) == CMSRET_SUCCESS)
            {
               if (atmDiagObj->successCount == atmDiagObj->numberOfRepetitions)
               {
                  ret = CMSRET_SUCCESS;
               }
               else
               {
                  ret = CMSRET_INTERNAL_ERROR;
               }
               cmsObj_free((void **) &atmDiagObj);
            } /* get object */
         } /* diag is not running */
      } /* not requested */
      CMSMEM_FREE_BUF_AND_NULL_PTR(atmLinkFullPathBuf);
   } /* get obj success */
   
   return (ret);
}
#endif /* DMP_DEVICE2_ATMLOOPBACK_1 */

#endif /* DMP_DEVICE2_BASELINE_1 */


