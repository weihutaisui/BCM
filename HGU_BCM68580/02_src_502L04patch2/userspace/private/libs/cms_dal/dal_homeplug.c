/***********************************************************************
 *
 *  Copyright (c) 2009  Broadcom Corporation
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
#include "dal.h"

#ifdef SUPPORT_HOMEPLUG

CmsRet dalHomeplug_getCurrentCfg(WEB_NTWK_VAR* webVar)
{
   HomePlugInterfaceObject* HPAVIfaceObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = cmsObj_getNext(MDMOID_HOME_PLUG_INTERFACE, &iidStack, (void **) &HPAVIfaceObj);

   if (ret == CMSRET_SUCCESS)
   {
      /* clean homeplug webVar strings */
      memset(webVar->homeplugStatus, 0, sizeof(webVar->homeplugStatus));
      memset(webVar->homeplugMACAddress, 0, sizeof(webVar->homeplugMACAddress));
      memset(webVar->plcVersion, 0, sizeof(webVar->plcVersion));
      memset(webVar->homeplugVersion, 0, sizeof(webVar->homeplugVersion));
      memset(webVar->homeplugNetworkPassword, 0, sizeof(webVar->homeplugNetworkPassword));
      memset(webVar->homeplugAlias, 0, sizeof(webVar->homeplugAlias));
      /* set homeplug webVar parameters */
      webVar->homeplugMaxBitRate = HPAVIfaceObj->maxBitRate;
      if (HPAVIfaceObj->status != NULL)
      {
        strncpy(webVar->homeplugStatus, HPAVIfaceObj->status, sizeof(webVar->homeplugStatus) - 1);
      }
      if (HPAVIfaceObj->MACAddress != NULL)
      {
        strncpy(webVar->homeplugMACAddress, HPAVIfaceObj->MACAddress, sizeof(webVar->homeplugMACAddress) - 1);
      }
      if (HPAVIfaceObj->firmwareVersion != NULL)
      {
        strncpy(webVar->plcVersion, 
	        HPAVIfaceObj->firmwareVersion, 
	        sizeof(webVar->plcVersion) - 1);
      }
      if (HPAVIfaceObj->version != NULL)
      {
        strncpy(webVar->homeplugVersion, HPAVIfaceObj->version, sizeof(webVar->homeplugVersion) - 1);
      }
      if (HPAVIfaceObj->networkPassword != NULL)
      {
        strncpy(webVar->homeplugNetworkPassword, HPAVIfaceObj->networkPassword, sizeof(webVar->homeplugNetworkPassword) - 1);
      }
      if (HPAVIfaceObj->alias != NULL)
      {
        strncpy(webVar->homeplugAlias, HPAVIfaceObj->alias, sizeof(webVar->homeplugAlias) - 1);
      }
      cmsObj_free((void **) &HPAVIfaceObj); 
   }
   
   return ret;
}

#endif
