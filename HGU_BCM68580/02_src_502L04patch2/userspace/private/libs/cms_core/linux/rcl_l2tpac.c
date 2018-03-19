/***********************************************************************
 *
 *  Copyright (c) 2006-2009  Broadcom Corporation
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

#ifdef DMP_X_BROADCOM_COM_L2TPAC_1

#include "odl.h"
#include "cms_core.h"
#include "cms_util.h"
#include "rcl.h"
#include "rut_util.h"
#include "rut_wan.h"
#include "rut_wanlayer2.h"
#include "rut_l2tpac.h"

/*!\file rcl_l2tpac.c
 * \brief This file contains L2TPAC related functions.
 *
 */


CmsRet rcl_l2tpAcIntfConfigObject(_L2tpAcIntfConfigObject *newObj,
                const _L2tpAcIntfConfigObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))

{
   L2tpAcIntfConfigObject *L2tpAcIntf = NULL;
   InstanceIdStack iidStack0 = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack1 = EMPTY_INSTANCE_ID_STACK;
   void *obj = NULL;
   WanPppConnObject *pppCon=NULL;
   char l2tpAcUsername[BUFLEN_128]="username";
   char l2tpAcPassword[BUFLEN_128]="password";
   CmsRet ret=CMSRET_SUCCESS;

   if ((ret = rut_validateObjects(newObj, currObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("rut_validateObjects returns error. ret=%d", ret);
      return ret;
   }

   /* enable L2tpAc connection */
   if (ENABLE_NEW_OR_ENABLE_EXISTING(newObj, currObj))
   {
      if ((ret = rutWl2_getWanL2tpAcObject(&iidStack0, &L2tpAcIntf)) != CMSRET_SUCCESS)
      {
          cmsLog_error("could not get L2tpAcIntf object");
          return ret;
      }
      else
      {
          cmsObj_free((void **) &L2tpAcIntf);
      
          /* get iidstack of WANConnectionDevice */
          if ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_CONN_DEVICE, &iidStack0, &iidStack1, (void **)&obj)) != CMSRET_SUCCESS)
          {
            cmsLog_error("failed to get WanConnDevice, ret=%d", ret);
            return ret;
          }
          cmsObj_free(&obj);   /* no longer needed */
          INIT_INSTANCE_ID_STACK(&iidStack0);
          if ((ret = cmsObj_getNextInSubTreeFlags(MDMOID_WAN_PPP_CONN, &iidStack1, &iidStack0, OGF_NO_VALUE_UPDATE,(void **) &pppCon)) == CMSRET_SUCCESS)
          {
            //cmsLog_error("\n~~~ pppConn->username=%s, pppConn->password=%s", pppCon->username, pppCon->password);
            
            strcpy(l2tpAcUsername, pppCon->username);
            strcpy(l2tpAcPassword, pppCon->password);
		    if ((ret = rutL2tpAC_start(pppCon, newObj->lnsIpAddress, l2tpAcUsername, l2tpAcPassword)) != CMSRET_SUCCESS)
            {
                cmsLog_error("L2tpAc initialzation failed, ret=%d", ret);
				cmsObj_free((void **) &pppCon);
                return ret;
            }
            cmsObj_free((void **) &pppCon);
          }
          else
          {
            cmsLog_error("failed to get pppCon, ret=%d", ret);
          }
      }
   }
   else if (POTENTIAL_CHANGE_OF_EXISTING(newObj, currObj))
   {
      /* check for change in config params, if changed */
   }
   /* delete L2tpAc connection */
   else if (DELETE_OR_DISABLE_EXISTING(newObj, currObj))
   {
      rutL2tpAC_stop((_L2tpAcIntfConfigObject *) currObj);
   }

   return ret;
}

   
CmsRet rcl_l2tpAcLinkConfigObject(_L2tpAcLinkConfigObject *newObj,
                const _L2tpAcLinkConfigObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret=CMSRET_SUCCESS;

    // cwu
   //system("echo ---ACLink & \n");

   if (ENABLE_NEW_OR_ENABLE_EXISTING(newObj, currObj))
   {
       // cwu
       //system("echo ------ LinkConfigObject_Enable & \n");
   }
   else if (POTENTIAL_CHANGE_OF_EXISTING(newObj, currObj))
   {
      /* check for change in config params, if changed */
   }
   /* delete L2tpAc connection */
   else if (DELETE_OR_DISABLE_EXISTING(newObj, currObj))
   {
    // cwu
      // system("echo ------ LinkConfigObject_Disable & \n");
	  // rutL2tpAC_stop((_L2tpAcIntfConfigObject *) currObj);
   }

   return ret;
}
#endif /* DMP_X_BROADCOM_COM_L2TPAC_1 */
