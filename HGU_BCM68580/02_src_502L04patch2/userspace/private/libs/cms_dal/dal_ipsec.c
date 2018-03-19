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
#ifdef SUPPORT_IPSEC
#include "cms.h"
#include "cms_util.h"
#include "mdm_validstrings.h"
#include "cms_obj.h"
#include "cms_dal.h"

CmsRet dalIPSec_setDefaultValues(PWEB_NTWK_VAR pWebVar)
{
   // IPSec Configuration
    strcpy(pWebVar->ipsConnName, "new connection");
    strcpy(pWebVar->ipsTunMode, "esp");
    strcpy(pWebVar->ipsIpver, "4");
    strcpy(pWebVar->ipsLocalGwIf, "None");
    strcpy(pWebVar->ipsRemoteGWAddr, "0.0.0.0");
    strcpy(pWebVar->ipsLocalIPMode, "subnet");
    strcpy(pWebVar->ipsLocalIP, "0.0.0.0");
    strcpy(pWebVar->ipsLocalMask, "255.255.255.0");
    strcpy(pWebVar->ipsRemoteIPMode, "subnet");
    strcpy(pWebVar->ipsRemoteIP, "0.0.0.0");
    strcpy(pWebVar->ipsRemoteMask, "255.255.255.0");
    strcpy(pWebVar->ipsKeyExM, "auto");
    strcpy(pWebVar->ipsAuthM, "pre_shared_key");
    strcpy(pWebVar->ipsPSK, "key");
    strcpy(pWebVar->ipsCertificateName, "");
    strcpy(pWebVar->ipsPerfectFSEn, "disable");
    strcpy(pWebVar->ipsManualEncryptionAlgo, "3des-cbc");
    strcpy(pWebVar->ipsManualEncryptionKey, "");
    strcpy(pWebVar->ipsManualAuthAlgo, "hmac-md5");
    strcpy(pWebVar->ipsManualAuthKey, "");
    strcpy(pWebVar->ipsSPI, "101");
    strcpy(pWebVar->ipsPh1Mode, "main");
    strcpy(pWebVar->ipsPh1EncryptionAlgo, "3des");
    strcpy(pWebVar->ipsPh1IntegrityAlgo, "md5");
    strcpy(pWebVar->ipsPh1DHGroup, "modp1024");
    pWebVar->ipsPh1KeyTime = atoi("3600");
    strcpy(pWebVar->ipsPh2EncryptionAlgo, "3des");
    strcpy(pWebVar->ipsPh2IntegrityAlgo, "hmac_md5");
    strcpy(pWebVar->ipsPh2DHGroup, "modp1024");
    pWebVar->ipsPh2KeyTime = atoi("3600");
    return CMSRET_SUCCESS;

}
#endif

#ifdef DMP_X_BROADCOM_COM_IPSEC_1
extern CmsRet rutIPSec_doSetkeyRacoon_igd(void);
extern CmsRet rutIPSec_doRestartRacoon(void);


CmsRet dalIPSec_addTunnel_igd(const PWEB_NTWK_VAR pWebVar)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   IPSecCfgObject *ipsecObj = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   /* adding a new IP filter out entry */
   cmsLog_debug("Adding new IPSec Tunnel entry with %s/%s/%s/%s/%s/%s/%s", \
                pWebVar->ipsConnName, pWebVar->ipsLocalGwIf, pWebVar->ipsLocalIP, 
                pWebVar->ipsLocalMask, pWebVar->ipsRemoteGWAddr, 
                pWebVar->ipsRemoteIP, pWebVar->ipsRemoteMask);

   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_addInstance(MDMOID_IP_SEC_CFG, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new IPSecCfgObject, ret=%d", ret);
      return CMSRET_INTERNAL_ERROR;
   }

   if ((ret = cmsObj_get(MDMOID_IP_SEC_CFG, &iidStack, 0,
       (void **) &ipsecObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get IpsecCfgObject, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_IP_SEC_CFG, &iidStack);
      return CMSRET_INTERNAL_ERROR;
   }

   ipsecObj->enable = TRUE;
   CMSMEM_REPLACE_STRING(ipsecObj->connName, pWebVar->ipsConnName);
   CMSMEM_REPLACE_STRING(ipsecObj->tunMode, pWebVar->ipsTunMode);
   CMSMEM_REPLACE_STRING(ipsecObj->ipVer, pWebVar->ipsIpver);
   CMSMEM_REPLACE_STRING(ipsecObj->localGwIf, pWebVar->ipsLocalGwIf);
   CMSMEM_REPLACE_STRING(ipsecObj->remoteGWAddress, pWebVar->ipsRemoteGWAddr);
   CMSMEM_REPLACE_STRING(ipsecObj->localIPMode, pWebVar->ipsLocalIPMode);
   CMSMEM_REPLACE_STRING(ipsecObj->localIPAddress, pWebVar->ipsLocalIP);
   CMSMEM_REPLACE_STRING(ipsecObj->localMask, pWebVar->ipsLocalMask);
   CMSMEM_REPLACE_STRING(ipsecObj->remoteIPMode, pWebVar->ipsRemoteIPMode);
   CMSMEM_REPLACE_STRING(ipsecObj->remoteIPAddress, pWebVar->ipsRemoteIP);
   CMSMEM_REPLACE_STRING(ipsecObj->remoteMask, pWebVar->ipsRemoteMask);
   CMSMEM_REPLACE_STRING(ipsecObj->keyExM, pWebVar->ipsKeyExM);
   CMSMEM_REPLACE_STRING(ipsecObj->authM, pWebVar->ipsAuthM);
   CMSMEM_REPLACE_STRING(ipsecObj->PSK, pWebVar->ipsPSK);
   CMSMEM_REPLACE_STRING(ipsecObj->certificateName, pWebVar->ipsCertificateName);
   CMSMEM_REPLACE_STRING(ipsecObj->perfectFSEn, pWebVar->ipsPerfectFSEn);
   CMSMEM_REPLACE_STRING(ipsecObj->manualEncryptionAlgo, 
                         pWebVar->ipsManualEncryptionAlgo);
   CMSMEM_REPLACE_STRING(ipsecObj->manualEncryptionKey, 
                         pWebVar->ipsManualEncryptionKey);
   CMSMEM_REPLACE_STRING(ipsecObj->manualAuthAlgo, pWebVar->ipsManualAuthAlgo);
   CMSMEM_REPLACE_STRING(ipsecObj->manualAthKey, pWebVar->ipsManualAuthKey);
   CMSMEM_REPLACE_STRING(ipsecObj->SPI, pWebVar->ipsSPI);
   CMSMEM_REPLACE_STRING(ipsecObj->ph1Mode, pWebVar->ipsPh1Mode);
   CMSMEM_REPLACE_STRING(ipsecObj->ph1EncryptionAlo, pWebVar->ipsPh1EncryptionAlgo);
   CMSMEM_REPLACE_STRING(ipsecObj->ph1IntegrityAlgo, pWebVar->ipsPh1IntegrityAlgo);
   CMSMEM_REPLACE_STRING(ipsecObj->ph1DHGroup, pWebVar->ipsPh1DHGroup);
   ipsecObj->ph1KeyTime = pWebVar->ipsPh1KeyTime;
   CMSMEM_REPLACE_STRING(ipsecObj->ph2EncryptionAlo, pWebVar->ipsPh2EncryptionAlgo);
   CMSMEM_REPLACE_STRING(ipsecObj->ph2IntegrityAlgo, pWebVar->ipsPh2IntegrityAlgo);
   CMSMEM_REPLACE_STRING(ipsecObj->ph2DHGroup, pWebVar->ipsPh2DHGroup);
   ipsecObj->ph2KeyTime = pWebVar->ipsPh2KeyTime;
   ret = cmsObj_set(ipsecObj, &iidStack);
   cmsObj_free((void **) &ipsecObj);

   if (ret != CMSRET_SUCCESS) {
      cmsLog_error("Failed to set ipsecObj configuration, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_IP_SEC_CFG, &iidStack);
      return ret;
   }

   ret = rutIPSec_doSetkeyRacoon_igd();
   if (ret != CMSRET_SUCCESS) {
      cmsLog_error("Failed to setup configuration, ret=%d", ret);
      return ret;
   }

   if ((ret = rutIPSec_doRestartRacoon()) != CMSRET_SUCCESS) {
      cmsLog_error("Failed to restart racoon\n");
   }
   dalIPSec_setDefaultValues(pWebVar);
   return ret;
}


CmsRet dalIPSec_deleteTunnel_igd(const char* connName)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   IPSecCfgObject *ipsecCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 found = FALSE;

   /* deleting a IP filter out entry */
   cmsLog_debug("Deleting a IPSec Tunnel entry with name: %s", connName);

   while ((ret = cmsObj_getNext
         (MDMOID_IP_SEC_CFG, &iidStack, (void **) &ipsecCfg)) == CMSRET_SUCCESS)
   {
      if(!strcmp(connName, ipsecCfg->connName) ) 
      {
         found = TRUE;
         cmsObj_free((void **) &ipsecCfg);
         break;
      }
      else
      {
         cmsObj_free((void **) &ipsecCfg);
      }
   }

   if(found == FALSE) 
   {
      printf("bad tunnel name %s, no entry found", connName);
      cmsLog_debug("bad tunnel name %s, no entry found", connName);
      ret = CMSRET_INVALID_ARGUMENTS;
   }
   else 
   {
      ret = cmsObj_deleteInstance(MDMOID_IP_SEC_CFG, &iidStack);

      if (ret != CMSRET_SUCCESS)
      {
         printf("Failed to delete IPSecCfgObject, ret = %d", ret);
         cmsLog_error("Failed to delete IPSecCfgObject, ret = %d", ret);
      }
   }

   ret = rutIPSec_doSetkeyRacoon_igd();
   if (ret != CMSRET_SUCCESS) {
      printf("Failed to setup configuration, ret=%d", ret);
      cmsLog_error("Failed to setup configuration, ret=%d", ret);
      return ret;
   }

   if ((ret = rutIPSec_doRestartRacoon()) != CMSRET_SUCCESS) {
      printf("Failed to restart racoon\n");
      cmsLog_error("Failed to restart racoon\n");
   }

   return ret;
}
#endif /* DMP_X_BROADCOM_COM_IPSEC_1 */
