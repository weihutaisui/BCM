/***********************************************************************
 *
 *  Copyright (c) 2007-2008  Broadcom Corporation
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
#ifdef DMP_DEVICE2_IPSEC_1
#include "cms.h"
#include "cms_util.h"
#include "mdm_validstrings.h"
#include "cms_obj.h"
#include "cms_dal.h"


CmsRet dalIPSec_addTunnel_dev2(const PWEB_NTWK_VAR pWebVar)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2IpsecObject *ipsecObj = NULL;
   Dev2IpsecProfileObject *profileObj = NULL;
   Dev2IpsecFilterObject *filterObj = NULL;
   char *profilePath=NULL;

   cmsLog_debug("Adding new IPSec Tunnel entry with %s/%s/%s/%s/%s/%s/%s", \
                pWebVar->ipsConnName, pWebVar->ipsLocalGwIf,pWebVar->ipsLocalIP,
                pWebVar->ipsLocalMask, pWebVar->ipsRemoteGWAddr, 
                pWebVar->ipsRemoteIP, pWebVar->ipsRemoteMask);

   /* Set Device.IPsec.Enable = TRUE */
   if ((ret = cmsObj_get(MDMOID_DEV2_IPSEC, &iidStack, 0,
                         (void **) &ipsecObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get ipsecObj, ret=%d", ret);
      return ret;
   }

   ipsecObj->enable = TRUE;
   if ( (ret = cmsObj_set(ipsecObj, &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error("Failed to set Dev2IpsecObject");
      cmsObj_free((void **) &ipsecObj);
      return ret;
   }

   cmsObj_free((void **) &ipsecObj);

   /* Add Profile object and fill in data for manual/IKEv1 setting */
   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_addInstance(MDMOID_DEV2_IPSEC_PROFILE, &iidStack)) !=
        CMSRET_SUCCESS)
   {
      cmsLog_error("could not create MDMOID_DEV2_IPSEC_PROFILE, ret=%d", ret);
      return ret;
   }

   if ((ret = cmsObj_get(MDMOID_DEV2_IPSEC_PROFILE, &iidStack, 0,
                         (void **) &profileObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get profileObj, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_IPSEC_PROFILE, &iidStack);
      return ret;
   }

   CMSMEM_REPLACE_STRING(profileObj->remoteEndpoints, pWebVar->ipsRemoteGWAddr);
   if (!cmsUtl_strcmp(pWebVar->ipsTunMode, "esp"))
   {
      CMSMEM_REPLACE_STRING(profileObj->protocol, "ESP");
   }
   else
   {
      CMSMEM_REPLACE_STRING(profileObj->protocol, "AH");
   }
   CMSMEM_REPLACE_STRING(profileObj->X_BROADCOM_COM_KeyExchangeMode,
                         pWebVar->ipsKeyExM);
   CMSMEM_REPLACE_STRING(profileObj->X_BROADCOM_COM_LocalIfName,
                         pWebVar->ipsLocalGwIf);

   if ( (ret = cmsObj_set(profileObj, &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error("Failed to set Dev2IpsecProfileObject");
      cmsObj_free((void **) &profileObj);
      cmsObj_deleteInstance(MDMOID_DEV2_IPSEC_PROFILE, &iidStack);
      return ret;
   }

   cmsObj_free((void **) &profileObj);

   if (!cmsUtl_strcmp(pWebVar->ipsKeyExM, "manual"))
   {
      ManualModeCfgObject *manualObj = NULL;

      if ((ret = cmsObj_get(MDMOID_MANUAL_MODE_CFG, &iidStack, 0,
                            (void **) &manualObj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to get ManualModeCfgObject, ret=%d", ret);
         return ret;
      }

      CMSMEM_REPLACE_STRING(manualObj->allowedEncryptionAlgorithms, 
                            pWebVar->ipsManualEncryptionAlgo);
      CMSMEM_REPLACE_STRING(manualObj->encryptionKey, 
                            pWebVar->ipsManualEncryptionKey);
      CMSMEM_REPLACE_STRING(manualObj->allowedIntegrityAlgorithms,
                            pWebVar->ipsManualAuthAlgo);
      CMSMEM_REPLACE_STRING(manualObj->authenticationKey,
                            pWebVar->ipsManualAuthKey);
      manualObj->SPI = atoi(pWebVar->ipsSPI);

      if ( (ret = cmsObj_set(manualObj, &iidStack)) != CMSRET_SUCCESS )
      {
         cmsLog_error("Failed to set ManualModeCfgObject");
      }

      cmsObj_free((void **) &manualObj);
   }
   else if (!cmsUtl_strcmp(pWebVar->ipsKeyExM, "auto"))
   {
      Ikev1CfgObject *ikeObj = NULL;

      if ((ret = cmsObj_get(MDMOID_IKEV1_CFG, &iidStack, 0,
                            (void **) &ikeObj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to get Ikev1CfgObject, ret=%d", ret);
         return ret;
      }

      CMSMEM_REPLACE_STRING(ikeObj->authenticationMethod, pWebVar->ipsAuthM);
      CMSMEM_REPLACE_STRING(ikeObj->preSharedKey, pWebVar->ipsPSK);
      CMSMEM_REPLACE_STRING(ikeObj->certificateName,
                            pWebVar->ipsCertificateName);
      if (!cmsUtl_strcmp(pWebVar->ipsPerfectFSEn, "enable"))
      {
         ikeObj->perfectFSEn = TRUE;
      }
      else
      {
         ikeObj->perfectFSEn = FALSE;
      }
      CMSMEM_REPLACE_STRING(ikeObj->phase1Mode, pWebVar->ipsPh1Mode);
      CMSMEM_REPLACE_STRING(ikeObj->phase1AllowedEncryptionAlgorithms,
                            pWebVar->ipsPh1EncryptionAlgo);
      CMSMEM_REPLACE_STRING(ikeObj->phase1AllowedIntegrityAlgorithms,
                            pWebVar->ipsPh1IntegrityAlgo);
      CMSMEM_REPLACE_STRING(ikeObj->phase1AllowedDiffieHellmanGroupTransforms,
                            pWebVar->ipsPh1DHGroup);
      ikeObj->phase1KeyTime = pWebVar->ipsPh1KeyTime;
      CMSMEM_REPLACE_STRING(ikeObj->phase2AllowedEncryptionAlgorithms,
                            pWebVar->ipsPh2EncryptionAlgo);
      CMSMEM_REPLACE_STRING(ikeObj->phase2AllowedIntegrityAlgorithms,
                            pWebVar->ipsPh2IntegrityAlgo);
      CMSMEM_REPLACE_STRING(ikeObj->phase2AllowedDiffieHellmanGroupTransforms,
                            pWebVar->ipsPh2DHGroup);
      ikeObj->phase2KeyTime = pWebVar->ipsPh2KeyTime;

      if ( (ret = cmsObj_set(ikeObj, &iidStack)) != CMSRET_SUCCESS )
      {
         cmsLog_error("Failed to set Ikev1CfgObject");
      }

      cmsObj_free((void **) &ikeObj);
   }

   /* Get the fullpath of the new Profile object */
   {
      MdmPathDescriptor profilePathDesc;

      memset(&profilePathDesc, 0, sizeof(MdmPathDescriptor));
      profilePathDesc.oid = MDMOID_DEV2_IPSEC_PROFILE;
      profilePathDesc.iidStack = iidStack;
      if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&profilePathDesc,
                                            &profilePath)) != CMSRET_SUCCESS)
      {
         cmsLog_error("pathDescriptorToFullPathNoEndDot error: ret=%d", ret);
         return ret;
      }
   }

   /* Add Filter object and fill in data */
   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_addInstance(MDMOID_DEV2_IPSEC_FILTER, &iidStack)) !=
        CMSRET_SUCCESS)
   {
      cmsLog_error("could not create MDMOID_DEV2_IPSEC_PROFILE, ret=%d", ret);
      CMSMEM_FREE_BUF_AND_NULL_PTR(profilePath);
      return ret;
   }

   if ((ret = cmsObj_get(MDMOID_DEV2_IPSEC_FILTER, &iidStack, 0,
                         (void **) &filterObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get filterObj, ret=%d", ret);
      CMSMEM_FREE_BUF_AND_NULL_PTR(profilePath);
      cmsObj_deleteInstance(MDMOID_DEV2_IPSEC_FILTER, &iidStack);
      return ret;
   }

   filterObj->enable = TRUE;
   filterObj->allInterfaces = TRUE;
   CMSMEM_REPLACE_STRING(filterObj->X_BROADCOM_COM_TunnelName,
                         pWebVar->ipsConnName);
   CMSMEM_REPLACE_STRING(filterObj->sourceIP, pWebVar->ipsLocalIP);
   if (!cmsUtl_strcmp(pWebVar->ipsLocalIPMode, "subnet"))
   {
      CMSMEM_REPLACE_STRING(filterObj->sourceMask, pWebVar->ipsLocalMask);
   }
   else
   {
      if (!cmsUtl_strcmp(pWebVar->ipsIpver, "4"))
      {
         CMSMEM_REPLACE_STRING(filterObj->sourceMask, "255.255.255.255");
      }
      else
      {
         CMSMEM_REPLACE_STRING(filterObj->sourceMask, "128");
      }
   }

   CMSMEM_REPLACE_STRING(filterObj->destIP, pWebVar->ipsRemoteIP);
   if (!cmsUtl_strcmp(pWebVar->ipsRemoteIPMode, "subnet"))
   {
      CMSMEM_REPLACE_STRING(filterObj->destMask, pWebVar->ipsRemoteMask);
   }
   else
   {
      if (!cmsUtl_strcmp(pWebVar->ipsIpver, "4"))
      {
         CMSMEM_REPLACE_STRING(filterObj->destMask, "255.255.255.255");
      }
      else
      {
         CMSMEM_REPLACE_STRING(filterObj->destMask, "128");
      }
   }

   CMSMEM_REPLACE_STRING(filterObj->profile, profilePath);
   CMSMEM_REPLACE_STRING(filterObj->processingChoice, MDMVS_PROTECT);
   CMSMEM_FREE_BUF_AND_NULL_PTR(profilePath);

   if ( (ret = cmsObj_set(filterObj, &iidStack)) != CMSRET_SUCCESS )
   {
      cmsObj_deleteInstance(MDMOID_DEV2_IPSEC_FILTER, &iidStack);
      cmsLog_error("Failed to set filterObj");
   }

   cmsObj_free((void **) &filterObj);

   dalIPSec_setDefaultValues(pWebVar);
   return ret;
}


CmsRet dalIPSec_deleteTunnel_dev2(const char* connName)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2IpsecFilterObject *filterObj = NULL;
   UBOOL8 found = FALSE;

   /* deleting a IP filter out entry */
   cmsLog_debug("Deleting a IPSec Tunnel entry with name: %s", connName);

   while (cmsObj_getNextFlags(MDMOID_DEV2_IPSEC_FILTER, &iidStack, 
                 OGF_NO_VALUE_UPDATE, (void **)&filterObj) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(connName, filterObj->X_BROADCOM_COM_TunnelName))
      {
         found = TRUE;
         break;
      }
      cmsObj_free((void **) &filterObj);
   }

   if(found == FALSE) 
   {
      cmsLog_error("bad tunnel name %s, no entry found", connName);
      ret = CMSRET_INVALID_ARGUMENTS;
   }
   else 
   {
      MdmPathDescriptor pathDesc;

      /* Delete associated profile object */
      INIT_PATH_DESCRIPTOR(&pathDesc);
      if (cmsMdm_fullPathToPathDescriptor(filterObj->profile, &pathDesc) == 
          CMSRET_SUCCESS)
      {
         ret = cmsObj_deleteInstance(pathDesc.oid, &pathDesc.iidStack);

         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("cannot delete Dev2IpsecProfileObject, ret=%d", ret);
         }
      }
      else
      {
         cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s", 
                      filterObj->profile);
      }

      cmsObj_free((void **) &filterObj);

      ret = cmsObj_deleteInstance(MDMOID_DEV2_IPSEC_FILTER, &iidStack);

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to delete Dev2IpsecFilterObject, ret = %d", ret);
      }
   }

   return ret;
}
#endif /* DMP_DEVICE2_IPSEC_1 */
