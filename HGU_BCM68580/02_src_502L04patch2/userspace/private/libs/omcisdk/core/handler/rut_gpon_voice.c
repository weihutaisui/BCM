/***********************************************************************
 *
 *  Copyright (c) 2011 Broadcom
 *  All Rights Reserved
 *
<:label-BRCM:2011:proprietary:omcid

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

#ifdef DMP_X_ITU_ORG_GPON_1
#ifdef DMP_X_ITU_ORG_VOICE_1

#include "owsvc_api.h"

#include "mdmlite_api.h"
#include "me_handlers.h"
#include "ctlutils_api.h"
#include "owrut_api.h"


static VOIP_CONFIG_ID_ENUM voipConfigContext;


CmsRet rutGpon_SetDialPlan(NetworkDialPlanTableObject* dialPlanRecPtr,
     const InstanceIdStack* iidStackPtr);
UBOOL8 FindVoiceME(UINT16 obj_Type, UINT16 obj_ID, UBOOL8 voiceLineArray[]);


//*******************************************************************
//*******************************************************************
//*******************************************************************

static CmsRet getVoipPort
    (const UINT32 pptpPotsMeId,
     UINT32 *portIndex)
{
    UBOOL8 found = FALSE;
    PptpPotsUniObject *pptpPots = NULL;
    InstanceIdStack iidPots = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    *portIndex = 0;

    // search PptpPotsUniObject
    while ((!found) &&
           (_cmsObj_getNext(MDMOID_PPTP_POTS_UNI,
                           &iidPots,
                           (void **) &pptpPots) == CMSRET_SUCCESS))
    {
        found = (pptpPotsMeId == pptpPots->managedEntityId);
        // if PptpPotsUniObject is not found then
        // increasing index number
        if (!found)
            (*portIndex)++;
        _cmsObj_free((void **) &pptpPots);
    }

    if (found)
        ret = CMSRET_SUCCESS;

    return ret;
}

#ifdef DMP_X_ITU_ORG_VOICE_SIP_1
static CmsRet largeStringToString
    (const LargeStringObject *largeString,
     char *buf)
{
    UINT32 partNum = 1;
    CmsRet ret = CMSRET_SUCCESS;

    // buf must have length equal or greater than 375 bytes
    buf[0] = '\0';

    if (partNum++ <= largeString->numberOfParts &&
        largeString->part1 != NULL)
        strcpy(buf, largeString->part1);
    else
        goto out;

    if (partNum++ <= largeString->numberOfParts &&
        largeString->part2 != NULL)
        strcat(buf, largeString->part2);
    else
        goto out;

    if (partNum++ <= largeString->numberOfParts &&
        largeString->part3 != NULL)
        strcat(buf, largeString->part3);
    else
        goto out;

    if (partNum++ <= largeString->numberOfParts &&
        largeString->part4 != NULL)
        strcat(buf, largeString->part4);
    else
        goto out;

    if (partNum++ <= largeString->numberOfParts &&
        largeString->part5 != NULL)
        strcat(buf, largeString->part5);
    else
        goto out;

    if (partNum++ <= largeString->numberOfParts &&
        largeString->part6 != NULL)
        strcat(buf, largeString->part6);
    else
        goto out;

    if (partNum++ <= largeString->numberOfParts &&
        largeString->part7 != NULL)
        strcat(buf, largeString->part7);
    else
        goto out;

    if (partNum++ <= largeString->numberOfParts &&
        largeString->part8 != NULL)
        strcat(buf, largeString->part8);
    else
        goto out;

    if (partNum++ <= largeString->numberOfParts &&
        largeString->part9 != NULL)
        strcat(buf, largeString->part9);
    else
        goto out;

    if (partNum++ <= largeString->numberOfParts &&
        largeString->part10 != NULL)
        strcat(buf, largeString->part10);
    else
        goto out;

    if (partNum++ <= largeString->numberOfParts &&
        largeString->part11 != NULL)
        strcat(buf, largeString->part11);
    else
        goto out;

    if (partNum++ <= largeString->numberOfParts &&
        largeString->part12 != NULL)
        strcat(buf, largeString->part12);
    else
        goto out;

    if (partNum++ <= largeString->numberOfParts &&
        largeString->part13 != NULL)
        strcat(buf, largeString->part13);
    else
        goto out;

    if (partNum++ <= largeString->numberOfParts &&
        largeString->part14 != NULL)
        strcat(buf, largeString->part14);
    else
        goto out;

    if (partNum++ <= largeString->numberOfParts &&
        largeString->part15 != NULL)
        strcat(buf, largeString->part15);

out:
    return ret;
}


static CmsRet updateSipRegistrar
    (const UINT32 port __attribute__((unused)),
     const UINT32 lgStrMeId __attribute__((unused)))
{
#ifdef DMP_VOICE_SERVICE_1
    char buf[BUFLEN_1024];
    UBOOL8 found = FALSE;
    LargeStringObject *largeString = NULL;
    InstanceIdStack iidLargeString = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // search LargeStringObject
    while ((!found) &&
           (_cmsObj_getNext(MDMOID_LARGE_STRING,
                           &iidLargeString,
                           (void **) &largeString) == CMSRET_SUCCESS))
    {
        found = (lgStrMeId == largeString->managedEntityId);
        // if LargeStringObject for addressPointer is found then
        if (found == TRUE)
        {
            largeStringToString(largeString, buf);
            _owapi_rut_setSipRegistrarAddress(port, buf);
            ret = CMSRET_SUCCESS;
        }
        _cmsObj_free((void **) &largeString);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_VOICE,
                   "===> updateSipRegistrar, port=%d, lgStrMeId=%d, found=%d, ret=%d\n",
                   port, lgStrMeId, found, ret);

    return ret;
#else
    return CMSRET_SUCCESS;
#endif
}


static void updateAuthSecMethod(const AuthenticationSecurityMethodObject* authSecPtr, const int voiceLineIndex)
{
  UINT8* bin = NULL;
  UINT32 binSize;
  char buf1[BUFLEN_128], buf2[BUFLEN_64];

  // Init buffer.
  memset(buf1, 0, BUFLEN_128);
  memset(buf2, 0, BUFLEN_64);

  // Convert MDM username1 string to normal ASCII C-string.
  cmsUtl_hexStringToBinaryBuf(authSecPtr->username1, &bin, &binSize);

  // Copy username1 string to temp buffer.
  memcpy(buf1, bin, binSize);

  // Release temporary CMS buffer.
  cmsMem_free(bin);

  // Convert MDM username2 string to normal ASCII C-string.
  cmsUtl_hexStringToBinaryBuf(authSecPtr->username2, &bin, &binSize);

  // Copy username2 string to temp buffer.
  memcpy(buf2, bin, binSize);

  // Release temporary CMS buffer.
  cmsMem_free(bin);

  // concatenate to form username
  strcat(buf1, buf2);
  
  // Set voice line username in TR104.
  _owapi_rut_setSipAuthUsername(voiceLineIndex, buf1);

  // Init buffer.
  memset(buf1, 0, BUFLEN_128);

  // Convert MDM password string to normal ASCII C-string.
  cmsUtl_hexStringToBinaryBuf(authSecPtr->password, &bin, &binSize);

  // Copy password string to temp buffer.
  memcpy(buf1, bin, binSize);

  // Release temporary CMS buffer.
  cmsMem_free(bin);

  // Set voice line password in TR104.
  _owapi_rut_setSipAuthPassword(voiceLineIndex, buf1);

  omciDebugPrint(OMCI_DEBUG_MODULE_VOICE,
                 "===> updateAuthSecMethod, username1=%s, username2=%s, password=%s, voiceLineIndex=%d\n",
                 authSecPtr->username1, authSecPtr->username2, authSecPtr->password, voiceLineIndex);
}


static CmsRet getDnsInfo
    (const SipAgentConfigDataObject* sipAgentPtr,
     char *dns1,
     char *dns2)
{
  CmsRet ret = CMSRET_OBJECT_NOT_FOUND;
  UBOOL8 foundPrimaryDns = FALSE, foundSecondaryDns = FALSE;
  struct in_addr inAddr;
  LargeStringObject* largeStringPtr;
  InstanceIdStack iidLargeString = EMPTY_INSTANCE_ID_STACK;

  // if the first 2 bytes of primarySipDns are 0 (0.0.xxx.xxx) then
  // it's pointer to LargeStringObject that represents IPv6 primary DNS address.
  if ((sipAgentPtr->primarySipDns & 0xFFFF0000) == 0)
  {
    // search for LargeStringObject that represents IPv6 primary DNS address.
    while (foundPrimaryDns == FALSE &&
           _cmsObj_getNext(MDMOID_LARGE_STRING, &iidLargeString, (void**)&largeStringPtr) == CMSRET_SUCCESS)
    {
      // Test if LargeStringObject is IPv6 primary DNS address pointer.
      if (sipAgentPtr->primarySipDns == largeStringPtr->managedEntityId)
      {
          // Convert MDMOID_LARGE_STRING to local C-string.
          largeStringToString(largeStringPtr, dns1);

          // Signal primary DNS Address found.
          foundPrimaryDns = TRUE;
      }

      // Release MDMOID_LARGE_STRING record.
      _cmsObj_free((void**)&largeStringPtr);
    }
  }
  else
  {
    // primarySipDns represents IPv4 primary DNS address
    inAddr.s_addr = sipAgentPtr->primarySipDns;
    strcpy(dns1, inet_ntoa(inAddr));

    // Signal primary DNS Address found.
    foundPrimaryDns = TRUE;
  }

  // if the first 2 bytes of secondarySipDns are 0 (0.0.xxx.xxx) then
  // it's pointer to LargeStringObject that represents IPv6 secondary DNS address.
  if ((sipAgentPtr->secondarySipDns & 0xFFFF0000) == 0)
  {
    INIT_INSTANCE_ID_STACK(&iidLargeString);
    // search for LargeStringObject that represents IPv6 secondary DNS address.
    while (foundSecondaryDns == FALSE &&
           _cmsObj_getNext(MDMOID_LARGE_STRING, &iidLargeString, (void**)&largeStringPtr) == CMSRET_SUCCESS)
    {
      // Test if LargeStringObject is IPv6 secondary DNS address pointer.
      if (sipAgentPtr->secondarySipDns == largeStringPtr->managedEntityId)
      {
          // Convert MDMOID_LARGE_STRING to local C-string.
          largeStringToString(largeStringPtr, dns2);

          // Signal secondary DNS Address found.
          foundSecondaryDns = TRUE;
      }

      // Release MDMOID_LARGE_STRING record.
      _cmsObj_free((void**)&largeStringPtr);
    }
  }
  else
  {
    // secondarySipDns represents IPv4 secondary DNS address
    inAddr.s_addr = sipAgentPtr->secondarySipDns;
    strcpy(dns2, inet_ntoa(inAddr));

    // Signal secondary DNS Address found.
    foundSecondaryDns = TRUE;
  }

  if (foundPrimaryDns == TRUE || foundSecondaryDns == TRUE)
    ret = CMSRET_SUCCESS;

  return ret;
}


static CmsRet Process_TcpUdpConfigDataObject(const TcpUdpConfigDataObject* tcpUdpPtr, const int voiceLineIndex)
{
  UBOOL8 found = FALSE;
  IpHostConfigDataObject* ipHostPtr = NULL;
  InstanceIdStack iidIpHost = EMPTY_INSTANCE_ID_STACK;
  CmsRet setupResult = CMSRET_OBJECT_NOT_FOUND;

  // Search for specified IpHostConfigDataObject
  while ((found == FALSE) && (_cmsObj_getNext(MDMOID_IP_HOST_CONFIG_DATA, &iidIpHost, (void**)&ipHostPtr) == CMSRET_SUCCESS))
  {
    // Set local object found flag based on match between this TcpUdpConfigDataObject and MDMOID_IP_HOST_CONFIG_DATA.
    found = (tcpUdpPtr->ipHostPointer == ipHostPtr->managedEntityId);

    // Test if this MDMOID_IP_HOST_CONFIG_DATA ME matches TcpUdpConfigDataObject pointer.
    if (found == TRUE)
    {
        // Process IpHostConfigDataObject configuration.
        struct in_addr inAddr;
        inAddr.s_addr = ipHostPtr->currentAddress;
        setupResult = _owapi_rut_setVoipBoundIfNameAddress(
          ipHostPtr->managedEntityId, inet_ntoa(inAddr));
    }

    // Release MDMOID_IP_HOST_CONFIG_DATA record.
    _cmsObj_free((void**)&ipHostPtr);
  }

  omciDebugPrint(OMCI_DEBUG_MODULE_VOICE,
                 "===> Process_TcpUdpConfigDataObject, ipHostPointer=%d, voiceLineIndex=%d, found=%d, setupResult=%d\n",
                 tcpUdpPtr->ipHostPointer, voiceLineIndex, found, setupResult);

  // Signal CMSRET_SUCCESS if all required components of TcpUdpConfigDataObject are configured.
  return setupResult;
}


static CmsRet Process_SipAgentConfigDataObject(const SipAgentConfigDataObject* sipAgentPtr, const int voiceLineIndex)
{
  char buf[BUFLEN_1024];
  char dns1[CMS_IPADDR_LENGTH];
  char dns2[CMS_IPADDR_LENGTH];
  UBOOL8 netAddrFound = FALSE;
  UBOOL8 tcpUdpFound = FALSE;
  UBOOL8 sipProxyServerAddrFound = FALSE;
  UBOOL8 outboundProxyServerAddrFound = FALSE;
  LargeStringObject* largeStringPtr;
  NetworkAddressObject* netAddrPtr;
  TcpUdpConfigDataObject* tcpUdpPtr;
  InstanceIdStack iidLargeString = EMPTY_INSTANCE_ID_STACK;
  InstanceIdStack iidNetAddr = EMPTY_INSTANCE_ID_STACK;
  InstanceIdStack iidTcpUdp = EMPTY_INSTANCE_ID_STACK;
  CmsRet setupResult = CMSRET_OBJECT_NOT_FOUND;

  // Setup SIP registration times.
  _owapi_rut_setSipReregHeadStartTime(voiceLineIndex, sipAgentPtr->sipRegRegHeadStartTime);
  _owapi_rut_setSipRegisterExpirationTime(voiceLineIndex, sipAgentPtr->sipRegExpTime);

  // Setup SIP DNS.
  memset(dns1, 0, CMS_IPADDR_LENGTH);
  memset(dns2, 0, CMS_IPADDR_LENGTH);
  if (getDnsInfo(sipAgentPtr, dns1, dns2) == CMSRET_SUCCESS)
    _owapi_rut_configDns(dns1, dns2);

  // Find LargeStringObjects that represent SIP addresses.
  while (_cmsObj_getNext(MDMOID_LARGE_STRING, &iidLargeString, (void**)&largeStringPtr) == CMSRET_SUCCESS)
  {
    // Test if LargeStringObject is Proxy Server Address Pointer.
    if (sipAgentPtr->proxyServerAddressPointer == largeStringPtr->managedEntityId)
    {
      // Convert MDMOID_LARGE_STRING to local C-string.
      largeStringToString(largeStringPtr, buf);

      // Setup SIP Proxy Server Address.
      _owapi_rut_setSipProxyServerAddress(voiceLineIndex, buf);

      // Signal SIP Proxy Server Address found.
      sipProxyServerAddrFound = TRUE;
    }

    // Test if LargeStringObject is Outbound Proxy Address Pointer.
    if (sipAgentPtr->outboundProxyAddressPointer == largeStringPtr->managedEntityId)
    {
      // Convert MDMOID_LARGE_STRING to local C-string.
      largeStringToString(largeStringPtr, buf);

      // Setup outbound Proxy Server Address.
      _owapi_rut_setSipOutboundProxyAddress(voiceLineIndex, buf);

      // Signal outbound Proxy Server Address found.
      outboundProxyServerAddrFound = TRUE;
    }

    // Release MDMOID_LARGE_STRING record.
    _cmsObj_free((void**)&largeStringPtr);

    // Test if done searching for SIP addresses.
    if ((sipProxyServerAddrFound == TRUE) && (outboundProxyServerAddrFound == TRUE))
    {
      // Done, both SIP addresses found.
      break;
    }
  }

  // Test for success searching for SIP addresses.
  if ((sipProxyServerAddrFound == TRUE) || (outboundProxyServerAddrFound == TRUE))
  {
    // Search for NetworkAddressObject.
    while ((netAddrFound == FALSE) && (_cmsObj_getNext(MDMOID_NETWORK_ADDRESS, &iidNetAddr, (void**)&netAddrPtr) == CMSRET_SUCCESS))
    {
      // Set local object found flag based on match between this SIP Agent and MDMOID_NETWORK_ADDRESS.
      netAddrFound = (sipAgentPtr->sipRegistrar == netAddrPtr->managedEntityId);

      // Test for specified MDMOID_NETWORK_ADDRESS.
      if (netAddrFound == TRUE)
      {
        // Set SIP Registrar address.
        updateSipRegistrar(voiceLineIndex, netAddrPtr->addressPointer);
      }

      // Release MDMOID_NETWORK_ADDRESS record.
      _cmsObj_free((void**)&netAddrPtr);
    }

    // Test for success finding MDMOID_NETWORK_ADDRESS.
    if (netAddrFound == TRUE)
    {
      // Search for TcpUdpConfigDataObject.
      while ((tcpUdpFound == FALSE) && (_cmsObj_getNext(MDMOID_TCP_UDP_CONFIG_DATA, &iidTcpUdp, (void**)&tcpUdpPtr) == CMSRET_SUCCESS))
      {
        // Set local object found flag based on match between this SIP Agent and MDMOID_TCP_UDP_CONFIG_DATA.
        tcpUdpFound = (sipAgentPtr->tcpUdpPointer == tcpUdpPtr->managedEntityId);

        // Test for specified MDMOID_TCP_UDP_CONFIG_DATA.
        if (tcpUdpFound == TRUE)
        {
          // Process TcpUdpConfigDataObject.
          setupResult = Process_TcpUdpConfigDataObject(tcpUdpPtr, voiceLineIndex);
        }

        // Release MDMOID_NETWORK_ADDRESS record.
        _cmsObj_free((void**)&tcpUdpPtr);
      }
    }
  }

  omciDebugPrint(OMCI_DEBUG_MODULE_VOICE,
                 "===> Process_SipAgentConfigDataObject, proxyServerAddressPointer=%d, outboundProxyAddressPointer=%d, voiceLineIndex=%d, sipProxyServerAddrFound=%d, outboundProxyServerAddrFound=%d, netAddrFound=%d, tcpUdpFound=%d, setupResult=%d\n",
                 sipAgentPtr->proxyServerAddressPointer, sipAgentPtr->outboundProxyAddressPointer, voiceLineIndex, sipProxyServerAddrFound, outboundProxyServerAddrFound, netAddrFound, tcpUdpFound, setupResult);

  // Signal CMSRET_SUCCESS if all required components of MDMOID_SIP_AGENT_CONFIG_DATA are configured.
  return setupResult;
}


static CmsRet Process_SipUserDataObject(const SipUserDataObject* sipUserDataPtr, const int voiceLineIndex)
{
  char buf[BUFLEN_1024];
  UBOOL8 dialFound = FALSE;
  UBOOL8 appFound = FALSE;
  UBOOL8 authFound = FALSE;
  UBOOL8 agentFound = FALSE;
  UBOOL8 sipUserPartAorFound = FALSE;
  NetworkDialPlanTableObject* dialObjPtr;
  VoIpAppServiceProfileObject* appObjPtr;
  AuthenticationSecurityMethodObject* authObjPtr;
  SipAgentConfigDataObject* sipAgentPtr;
  LargeStringObject* largeStringPtr;
  InstanceIdStack iidDial = EMPTY_INSTANCE_ID_STACK;
  InstanceIdStack iidApp = EMPTY_INSTANCE_ID_STACK;
  InstanceIdStack iidAuth = EMPTY_INSTANCE_ID_STACK;
  InstanceIdStack iidSipAgent = EMPTY_INSTANCE_ID_STACK;
  InstanceIdStack iidLargeString = EMPTY_INSTANCE_ID_STACK;
  CmsRet setupResult = CMSRET_OBJECT_NOT_FOUND;

  // Initialize username and password to empty strings in case AuthenticationSecurityMethodObject not found.
  _owapi_rut_setSipAuthUsername(voiceLineIndex, "");
  _owapi_rut_setSipAuthPassword(voiceLineIndex, "");
  _owapi_rut_setSipUserPartAor(voiceLineIndex, "");

  // Search for associated NetworkDialPlanTableObject pointed to by MDMOID_SIP_USER_DATA.
  while ((dialFound == FALSE) && (_cmsObj_getNext(MDMOID_NETWORK_DIAL_PLAN_TABLE, &iidDial, (void**)&dialObjPtr) == CMSRET_SUCCESS))
  {
    // Set local object found flag based on match between this MDMOID_SIP_USER_DATA and MDMOID_NETWORK_DIAL_PLAN_TABLE.
    dialFound = (sipUserDataPtr->networkDialPlanPointer == dialObjPtr->managedEntityId);

    // Test if this MDMOID_VO_IP_APP_SERVICE_PROFILE ME matches current MDMOID_SIP_USER_DATA pointer.
    if (dialFound == TRUE)
    {
      rutGpon_SetDialPlan(dialObjPtr, &iidDial);
    }

    // Release MDMOID_NETWORK_DIAL_PLAN_TABLE record.
    _cmsObj_free((void**)&dialObjPtr);
  }

  // Search for associated VoIpAppServiceProfileObject pointed to by MDMOID_SIP_USER_DATA.
  while ((appFound == FALSE) && (_cmsObj_getNext(MDMOID_VO_IP_APP_SERVICE_PROFILE, &iidApp, (void**)&appObjPtr) == CMSRET_SUCCESS))
  {
    // Set local object found flag based on match between this MDMOID_SIP_USER_DATA and MDMOID_VO_IP_APP_SERVICE_PROFILE.
    appFound = (sipUserDataPtr->appServiceProfilePointer == appObjPtr->managedEntityId);

    // Test if this MDMOID_VO_IP_APP_SERVICE_PROFILE ME matches current MDMOID_SIP_USER_DATA pointer.
    if (appFound == TRUE)
    {
      _owapi_rut_MapVoIpApplServiceProfile(voiceLineIndex, appObjPtr);
    }

    // Release MDMOID_VO_IP_APP_SERVICE_PROFILE record.
    _cmsObj_free((void**)&appObjPtr);
  }

  // Search for associated AuthenticationSecurityMethodObject pointed to by MDMOID_SIP_USER_DATA.
  while ((authFound == FALSE) && (_cmsObj_getNext(MDMOID_AUTHENTICATION_SECURITY_METHOD, &iidAuth, (void**)&authObjPtr) == CMSRET_SUCCESS))
  {
    // Set local object found flag based on match between this MDMOID_SIP_USER_DATA and MDMOID_AUTHENTICATION_SECURITY_METHOD.
    authFound = (sipUserDataPtr->usernamePassword == authObjPtr->managedEntityId);

    // Test if this MDMOID_AUTHENTICATION_SECURITY_METHOD ME matches current MDMOID_SIP_USER_DATA pointer.
    if (authFound == TRUE)
    {
      // Process AuthenticationSecurityMethodObject.
      updateAuthSecMethod(authObjPtr, voiceLineIndex);

      // Find LargeStringObjects that represent UserPartAor.
      while (sipUserPartAorFound == FALSE &&
             _cmsObj_getNext(MDMOID_LARGE_STRING, &iidLargeString, (void**)&largeStringPtr) == CMSRET_SUCCESS)
      {
        // Test if LargeStringObject is Proxy Server Address Pointer.
        if (sipUserDataPtr->userPartAor == largeStringPtr->managedEntityId)
        {
          // Convert MDMOID_LARGE_STRING to local C-string.
          largeStringToString(largeStringPtr, buf);

          // Setup SIP user part AOR.
          _owapi_rut_setSipUserPartAor(voiceLineIndex, buf);

          // Signal SIP user part AOR.
          sipUserPartAorFound = TRUE;
        }

        // Release MDMOID_LARGE_STRING record.
        _cmsObj_free((void**)&largeStringPtr);
      }

      // Setup NULL SIP address strings in case MDMOID_SIP_AGENT_CONFIG_DATA not found.
      _owapi_rut_setSipProxyServerAddress(voiceLineIndex, VOIP_STOP_ADDR_STRING);
      _owapi_rut_setSipOutboundProxyAddress(voiceLineIndex, VOIP_STOP_ADDR_STRING);
      _owapi_rut_setSipRegistrarAddress(voiceLineIndex, VOIP_STOP_ADDR_STRING);

      // Find associated SipAgentConfigDataObject.
      while ((agentFound == FALSE) && (_cmsObj_getNext(MDMOID_SIP_AGENT_CONFIG_DATA, &iidSipAgent, (void**)&sipAgentPtr) == CMSRET_SUCCESS))
  {
        // Set local object found flag based on match between MDMOID_SIP_USER_DATA and MDMOID_SIP_AGENT_CONFIG_DATA.
        agentFound = (sipUserDataPtr->sipAgentPointer == sipAgentPtr->managedEntityId);

        // Test if this MDMOID_SIP_AGENT_CONFIG_DATA ME matches current MDMOID_SIP_USER_DATA pointer.
        if (agentFound == TRUE)
    {
          // Process the SipAgentConfigDataObject since it is associated with this voice line.
          setupResult = Process_SipAgentConfigDataObject(sipAgentPtr, voiceLineIndex);
    }

        // Release MDMOID_SIP_AGENT_CONFIG_DATA record.
        _cmsObj_free((void**)&sipAgentPtr);
  }
    }

    // Release AuthenticationSecurityMethodObject record.
    _cmsObj_free((void**)&authObjPtr);
  }

  omciDebugPrint(OMCI_DEBUG_MODULE_VOICE,
                 "===> Process_SipUserDataObject, usernamePassword=%d, voiceLineIndex=%d, setupResult=%d\n",
                 sipUserDataPtr->usernamePassword, voiceLineIndex, setupResult);

  // Signal CMSRET_SUCCESS if all required components of MDMOID_SIP_USER_DATA are configured.
  return setupResult;
}
#endif // DMP_X_ITU_ORG_VOICE_SIP_1

static CmsRet Process_VoIpMediaProfileObject(const VoIpMediaProfileObject* mediaProfilePtr, const int voiceLineIndex)
{
  UBOOL8 found = FALSE;
  VoiceServiceObject* voiceServicePtr = NULL;
  RtpProfileDataObject* rtpProfileDataPtr = NULL;
  InstanceIdStack iidVoiceService = EMPTY_INSTANCE_ID_STACK;
  InstanceIdStack iidRtpProfileData = EMPTY_INSTANCE_ID_STACK;
  CmsRet setupResult = CMSRET_OBJECT_NOT_FOUND;

  // Search for specified VoiceServiceObject
  while ((found == FALSE) && (_cmsObj_getNext(MDMOID_VOICE_SERVICE, &iidVoiceService, (void**)&voiceServicePtr) == CMSRET_SUCCESS))
  {
    found = (mediaProfilePtr->voiceServiceProfilePointer == voiceServicePtr->managedEntityId);

    if (found == TRUE)
    {
      _owapi_rut_MapVoiceServiceProfile(voiceLineIndex, voiceServicePtr);
      setupResult = CMSRET_SUCCESS;
    }

    // Release MDMOID_VOICE_SERVICE record.
    _cmsObj_free((void**)&voiceServicePtr);
  }

  found = FALSE;

  // Search for specified RtpProfileDataObject
  while ((found == FALSE) && (_cmsObj_getNext(MDMOID_RTP_PROFILE_DATA, &iidRtpProfileData, (void**)&rtpProfileDataPtr) == CMSRET_SUCCESS))
  {
    found = (mediaProfilePtr->rtpProfilePointer == rtpProfileDataPtr->managedEntityId);

    if (found == TRUE)
    {
      _owapi_rut_MapRtpProfileData(rtpProfileDataPtr);
      setupResult = CMSRET_SUCCESS;
    }

    // Release MDMOID_RTP_PROFILE_DATA record.
    _cmsObj_free((void**)&rtpProfileDataPtr);
  }

  omciDebugPrint(OMCI_DEBUG_MODULE_VOICE,
                 "===> Process_VoIpMediaProfileObject, voiceServiceProfilePointer=%d, rtpProfilePointer=%d, voiceLineIndex=%d, found=%d, setupResult=%d\n",
                 mediaProfilePtr->voiceServiceProfilePointer, mediaProfilePtr->rtpProfilePointer, voiceLineIndex, found, setupResult);

  return setupResult;
}

static CmsRet Process_VoiceCtpObject(const VoIpVoiceCtpObject* voiceCtpPtr, const int voiceLineIndex)
{
  UBOOL8 found = FALSE;
  VoIpMediaProfileObject* mediaProfilePtr = NULL;
  InstanceIdStack iidMediaProfile = EMPTY_INSTANCE_ID_STACK;
  CmsRet setupResult = CMSRET_OBJECT_NOT_FOUND;

  // Search for specified VoIpMediaProfileObject
  while ((found == FALSE) && (_cmsObj_getNext(MDMOID_VO_IP_MEDIA_PROFILE, &iidMediaProfile, (void**)&mediaProfilePtr) == CMSRET_SUCCESS))
  {
    found = (voiceCtpPtr->voIpMediaProfilePointer == mediaProfilePtr->managedEntityId);

    if (found == TRUE)
    {
      _owapi_rut_MapVoIpMediaProfile(voiceLineIndex, mediaProfilePtr);
      Process_VoIpMediaProfileObject(mediaProfilePtr, voiceLineIndex);
      setupResult = CMSRET_SUCCESS;
    }

    // Release MDMOID_VO_IP_MEDIA_PROFILE record.
    _cmsObj_free((void**)&mediaProfilePtr);
  }

  omciDebugPrint(OMCI_DEBUG_MODULE_VOICE,
                 "===> Process_VoiceCtpObject, voIpMediaProfilePointer=%d, voiceLineIndex=%d, found=%d, setupResult=%d\n",
                 voiceCtpPtr->voIpMediaProfilePointer, voiceLineIndex, found, setupResult);

  return setupResult;
}


CmsRet rutGpon_setupOmciVoipObjects(void)
{
  UBOOL8 found;
  UBOOL8 voiceLineFlagArray[MAX_VOICE_LINES];
  int voiceLineIndex = 0;
  PptpPotsUniObject* pptpPotsPtr = NULL;
  VoIpVoiceCtpObject* voiceCtpPtr = NULL;
#ifdef DMP_X_ITU_ORG_VOICE_SIP_1
  SipUserDataObject* sipUserDataPtr = NULL;
#endif
  InstanceIdStack iidPots = EMPTY_INSTANCE_ID_STACK;
  InstanceIdStack iidVoiceCtp = EMPTY_INSTANCE_ID_STACK;
#ifdef DMP_X_ITU_ORG_VOICE_SIP_1
  InstanceIdStack iidSipUserData = EMPTY_INSTANCE_ID_STACK;
#endif
  CmsRet setupResult = CMSRET_OBJECT_NOT_FOUND;

  // Initialize voice line setup flag array.
  memset(voiceLineFlagArray, FALSE, sizeof(voiceLineFlagArray));

  // Loop through each PPTP POTS ME insuring that all necessary configuration is available.
  while (_cmsObj_getNext(MDMOID_PPTP_POTS_UNI, &iidPots, (void**)&pptpPotsPtr) == CMSRET_SUCCESS)
  {
    found = FALSE;
    // Initialize (or re-initialize) iidStack.
    memset(&iidVoiceCtp, 0, sizeof(InstanceIdStack));

    // Search for VoIP Voice CTP object.
    while ((found == FALSE) && _cmsObj_getNext(MDMOID_VO_IP_VOICE_CTP, &iidVoiceCtp,(void**)&voiceCtpPtr) == CMSRET_SUCCESS)
    {
      found = (voiceCtpPtr->pptpPointer == pptpPotsPtr->managedEntityId);
      if (found == TRUE)
      {
        Process_VoiceCtpObject(voiceCtpPtr, voiceLineIndex);
      }

      // Release MDMOID_VO_IP_VOICE_CTP ME data structure.
      _cmsObj_free((void**)&voiceCtpPtr);
    }

    found = FALSE;
#ifdef DMP_X_ITU_ORG_VOICE_SIP_1
    // Initialize (or re-initialize) iidStack.
    memset(&iidSipUserData, 0, sizeof(InstanceIdStack));

    // Search for associated MDMOID_SIP_USER_DATA that points to MDMOID_PPTP_POTS_UNI.
    while ((found == FALSE) && (_cmsObj_getNext(MDMOID_SIP_USER_DATA, &iidSipUserData, (void**)&sipUserDataPtr) == CMSRET_SUCCESS))
    {
      // Set local object found flag based on match between this MDMOID_SIP_USER_DATA and MDMOID_PPTP_POTS_UNI.
      found = (sipUserDataPtr->pptpPointer == pptpPotsPtr->managedEntityId);

      // Test if this MDMOID_SIP_USER_DATA ME matches current MDMOID_PPTP_POTS_UNI.
      if (found == TRUE)
      {
        // Process the SipUserDataObject and its associated MEs (for this line only) & test for configuration success.
        if (Process_SipUserDataObject(sipUserDataPtr, voiceLineIndex) == CMSRET_SUCCESS)
        {
          // Signal that this voice line is configured.
          voiceLineFlagArray[voiceLineIndex] = TRUE;
        }
      }

      // Release MDMOID_SIP_USER_DATA ME data structure.
      _cmsObj_free((void**)&sipUserDataPtr);
    }
#endif // DMP_X_ITU_ORG_VOICE_SIP_1

    // Increment voice line index.
    voiceLineIndex++;

    // Release MDMOID_PPTP_POTS_UNI object.
    _cmsObj_free((void**)&pptpPotsPtr);
  }

  // Loop through voice line setup flags to see if any have been configured.
  for (voiceLineIndex = 0;voiceLineIndex < MAX_VOICE_LINES;voiceLineIndex++)
  {
    // Test for success configuring at least one voice line.
    if (voiceLineFlagArray[voiceLineIndex] == TRUE)
    {
      // Done, success configuring this voice line.
      setupResult = CMSRET_SUCCESS;

      // restart voice stack.
      _owapi_rut_SendUploadComplete();

      // Done, no need to examine other lines.
      break;
    }
  }

  omciDebugPrint(OMCI_DEBUG_MODULE_VOICE,
                 "===> rutGpon_setupOmciVoipObjects, setupResult=%d\n",
                 setupResult);

  // Signal CMSRET_SUCCESS if at least one voice line has been configured.
  return setupResult;
}


static int GetVoiceLineIndex(UINT16 obj_ID)
{
  UINT32 voiceLineIndex;

  // Attempt to get voice line index.
  if (getVoipPort(obj_ID, &voiceLineIndex) == CMSRET_SUCCESS)
  {
    // Return valid voice line index.
    return (int)voiceLineIndex;
  }

  // Return invalid voice line index.
  return (MAX_VOICE_LINES - 1);
}


static UBOOL8 TestVoiceObjectID_PptpPots(UINT16 pptpPotsID, UINT16 obj_Type, UINT16 obj_ID)
{
  PptpPotsUniObject* objPtr;
  InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

  // Test if this MDMOID_PPTP_POTS_UNI object is object in question.
  if ((obj_Type == MDMOID_PPTP_POTS_UNI) && (obj_ID == pptpPotsID))
  {
    // Found MDMOID_PPTP_POTS_UNI object that is searched for.
    return TRUE;
  }

  // Search for MDMOID_PPTP_POTS_UNI object.
  while (_cmsObj_getNext(MDMOID_PPTP_POTS_UNI, &iidStack,(void**)&objPtr) == CMSRET_SUCCESS)
  {
    // Test for specified MDMOID_PPTP_POTS_UNI object.
    if (objPtr->managedEntityId == pptpPotsID)
    {
      // Test GEM Interworking TP object.
      if ((obj_Type == MDMOID_GEM_INTERWORKING_TP) && (obj_ID == objPtr->interworkingTpPointer))
      {
        // Set provisioning value context for generic MEs (i.e. MDMOID_LARGE_STRING).
        voipConfigContext = VOIP_CONFIG_ID_GEM_INTERWORKING_TP;

        // Return success, specified VoIP Feature Access Codes object affects voice line(s).
        return TRUE;
      }
    }
  }

  // Return failure, specified object not found in this path.
  return FALSE;
}


static UBOOL8 TestVoiceObjectID_VoipMediaProfile(UINT16 mediaProfileID, UINT16 obj_Type, UINT16 obj_ID)
{
  VoIpMediaProfileObject* objPtr;
  InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

  // Test if this MDMOID_VO_IP_MEDIA_PROFILE object is object in question.
  if ((obj_Type == MDMOID_VO_IP_MEDIA_PROFILE) && (obj_ID == mediaProfileID))
  {
    // Found MDMOID_VO_IP_MEDIA_PROFILE object that is searched for.
    return TRUE;
  }

  // Search for VoIP Media Profile object.
  while (_cmsObj_getNext(MDMOID_VO_IP_MEDIA_PROFILE, &iidStack,(void**)&objPtr) == CMSRET_SUCCESS)
  {
    // Test for specified MDMOID_VO_IP_MEDIA_PROFILE object.
    if (objPtr->managedEntityId == mediaProfileID)
    {
      // Test RTP Profile Data object.
      if ((obj_Type == MDMOID_RTP_PROFILE_DATA) && (obj_ID == objPtr->rtpProfilePointer))
      {
        // Return success, specified RTP Profile Data object affects voice line(s).
        return TRUE;
      }
      else
      {
        // Test Voice Service Profile object.
        if ((obj_Type == MDMOID_VOICE_SERVICE) && (obj_ID == objPtr->voiceServiceProfilePointer))
        {
          // Return success, specified Voice Service Profile object affects voice line(s).
          return TRUE;
        }
        else
        {
          // Done, specified object not found in this path.
          break;
        }
      }
    }
  }

  // Return failure, specified object not found in this path.
  return FALSE;
}


#ifdef DMP_X_ITU_ORG_VOICE_SIP_1
static UBOOL8 TestVoiceObjectID_SipAgentConfigData(UINT16 sipAgentConfigDataID, UINT16 obj_Type, UINT16 obj_ID)
{
  SipAgentConfigDataObject* objPtr;
  InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
  UBOOL8 objFound = FALSE;

  // Test SIP Agent Config Data object.
  if ((obj_Type == MDMOID_SIP_AGENT_CONFIG_DATA) && (obj_ID == sipAgentConfigDataID))
  {
    // Return success, specified SIP Agent Config Data object affects voice line(s).
    return TRUE;
  }
  else
  {
    // Search for SIP Agent Config Data object.
    while ((objFound == FALSE) && (_cmsObj_getNext(MDMOID_SIP_AGENT_CONFIG_DATA, &iidStack,(void**)&objPtr) == CMSRET_SUCCESS))
    {
      // Test for specified MDMOID_SIP_USER_DATA object.
      if (objPtr->managedEntityId == sipAgentConfigDataID)
      {
        // Signal MDM object search complete.
        objFound = TRUE;

        // Test for MDMOID_LARGE_STRING object (proxy server address, outbound proxy address, or Host Part URI).
        if (obj_Type == MDMOID_LARGE_STRING)
        {
          // Test if MDMOID_LARGE_STRING object matches proxy server address.
          if (obj_ID == objPtr->proxyServerAddressPointer)
          {
            // Set provisioning value context for generic MEs (i.e. MDMOID_LARGE_STRING).
            voipConfigContext = VOIP_CONFIG_ID_PROXY_SERVER_ADDR;

            // Return success, specified MDMOID_LARGE_STRING object affects voice line(s).
            return TRUE;
          }
          else
          {
            // Test if MDMOID_LARGE_STRING object matches outbound proxy address.
            if (obj_ID == objPtr->outboundProxyAddressPointer)
            {
              // Set provisioning value context for generic MEs (i.e. MDMOID_LARGE_STRING).
              voipConfigContext = VOIP_CONFIG_ID_OUTBOUND_PROXY_ADDR;

              // Return success, specified MDMOID_LARGE_STRING object affects voice line(s).
              return TRUE;
            }
            else
            {
              // Test if MDMOID_LARGE_STRING object matches Host Part URI.
              if (obj_ID == objPtr->hostPartUri)
              {
                // Set provisioning value context for generic MEs (i.e. MDMOID_LARGE_STRING).
                voipConfigContext = VOIP_CONFIG_ID_HOST_PART_URI;

                // Return success, specified MDMOID_LARGE_STRING object affects voice line(s).
                return TRUE;
              }
            }
          }
        }
        else
        {
          // Test for MDMOID_NETWORK_ADDRESS object (SIP Registrar address).
          if ((obj_Type == MDMOID_NETWORK_ADDRESS) && (obj_ID == objPtr->sipRegistrar))
          {
            // Set provisioning value context for generic MEs (i.e. MDMOID_LARGE_STRING).
            voipConfigContext = VOIP_CONFIG_ID_SIP_REGISTRAR;

            // Return success, specified MDMOID_NETWORK_ADDRESS object affects voice line(s).
            return TRUE;
          }
          else
          {
            // Test for MDMOID_TCP_UDP_CONFIG_DATA object (SIP Registrar address).
            if ((obj_Type == MDMOID_TCP_UDP_CONFIG_DATA) && (obj_ID == objPtr->tcpUdpPointer))
            {
              // Set provisioning value context for generic MEs (i.e. MDMOID_LARGE_STRING).
              voipConfigContext = VOIP_CONFIG_ID_TCP_UDP;

              // Return success, specified MDMOID_TCP_UDP_CONFIG_DATA object affects voice line(s).
              return TRUE;
            }
          }
        }
      }
    }
  }

  // Return failure, specified object not found in this path.
  return FALSE;
}


static UBOOL8 TestVoiceObjectID_VoipApplicationServiceProfile(UINT16 voipApplicationServiceProfileID, UINT16 obj_Type, UINT16 obj_ID)
{
  VoIpAppServiceProfileObject* objPtr;
  InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
  UBOOL8 objFound = FALSE;

  // Test if this MDMOID_VO_IP_APP_SERVICE_PROFILE object is object in question.
  if ((obj_Type == MDMOID_VO_IP_APP_SERVICE_PROFILE) && (obj_ID == voipApplicationServiceProfileID))
  {
    // Found MDMOID_VO_IP_APP_SERVICE_PROFILE object that is being searched for.
    return TRUE;
  }
  else
  {
    // Search for Voip Application Service Profile object.
    while ((objFound == FALSE) && (_cmsObj_getNext(MDMOID_VO_IP_APP_SERVICE_PROFILE, &iidStack,(void**)&objPtr) == CMSRET_SUCCESS))
    {
      // Test for specified MDMOID_VO_IP_APP_SERVICE_PROFILE object.
      if (objPtr->managedEntityId == voipApplicationServiceProfileID)
      {
        // Signal MDM object search complete.
        objFound = TRUE;

        // Test for MDMOID_NETWORK_ADDRESS object (Direct Connect Uri, outbound proxy address, or Host Part URI).
        if (obj_Type == MDMOID_NETWORK_ADDRESS)
        {
          // Test if MDMOID_NETWORK_ADDRESS object matches Direct Connect Uri.
          if (obj_ID == objPtr->directConnectUriPointer)
          {
            // Set provisioning value context for generic MEs (i.e. MDMOID_LARGE_STRING).
            voipConfigContext = VOIP_CONFIG_ID_DIRECT_CONNECT_URI;

            // Return success, specified MDMOID_NETWORK_ADDRESS object affects voice line(s).
            return TRUE;
          }
          else
          {
            // Test if MDMOID_NETWORK_ADDRESS object matches Bridged Line Agent Uri.
            if (obj_ID == objPtr->bridgedLineAgentUriPointer)
            {
              // Set provisioning value context for generic MEs (i.e. MDMOID_LARGE_STRING).
              voipConfigContext = VOIP_CONFIG_ID_DIRECT_CONNECT_URI;

              // Return success, specified MDMOID_NETWORK_ADDRESS object affects voice line(s).
              return TRUE;
            }
            else
            {
              // Test if MDMOID_NETWORK_ADDRESS object matches Conference Factory URI.
              if (obj_ID == objPtr->conferenceFactoryUriPointer)
              {
                // Set provisioning value context for generic MEs (i.e. MDMOID_LARGE_STRING).
                voipConfigContext = VOIP_CONFIG_ID_CONFERENCE_FACTORY_URI;

                // Return success, specified MDMOID_NETWORK_ADDRESS object affects voice line(s).
                return TRUE;
              }
            }
          }
        }
      }
    }
  }

  // Return failure, specified object not found in this path.
  return FALSE;
}


static UBOOL8 TestVoiceObjectID_SipUserData(UINT16 sipUserDataID, UINT16 obj_Type, UINT16 obj_ID)
{
  SipUserDataObject* objPtr;
  InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
  UBOOL8 objFound = FALSE;

  // Test if this MDMOID_SIP_USER_DATA object is object in question.
  if ((obj_Type == MDMOID_SIP_USER_DATA) && (obj_ID == sipUserDataID))
  {
    // Found MDMOID_SIP_USER_DATA object that is searched for.
    return TRUE;
  }
  else
  {
    // Search for SIP User Data object.
    while ((objFound == FALSE) && (_cmsObj_getNext(MDMOID_SIP_USER_DATA, &iidStack,(void**)&objPtr) == CMSRET_SUCCESS))
    {
      // Test for specified MDMOID_SIP_USER_DATA object.
      if (objPtr->managedEntityId == sipUserDataID)
      {
        // Signal MDM object search complete.
        objFound = TRUE;

        // Test VoIP Feature Access Codes object.
        if ((obj_Type == MDMOID_VOICE_FEATURE_ACCESS_CODES) && (obj_ID == objPtr->featureCodePointer))
        {
          // Return success, specified VoIP Feature Access Codes object affects voice line(s).
          return TRUE;
        }
        else
        {
          // Test if SIP Agent Config Data object is (or points to) specified object.
          if (TestVoiceObjectID_SipAgentConfigData(objPtr->sipAgentPointer, obj_Type, obj_ID) == TRUE)
          {
            // Return success, specified VoIP Feature Access Codes object affects voice line(s).
            return TRUE;
          }
          else
          {
            // Test if VoIP Application Service Profile object is (or points to) specified object.
            if (TestVoiceObjectID_VoipApplicationServiceProfile(objPtr->appServiceProfilePointer, obj_Type, obj_ID) == TRUE)
            {
              // Return success, specified VoIP Feature Access Codes object affects voice line(s).
              return TRUE;
            }
            else
            {
              // Test if Network Dial Plan Table object is specified object.
              if ((obj_Type == MDMOID_NETWORK_DIAL_PLAN_TABLE) && (obj_ID == objPtr->networkDialPlanPointer))
              {
                // Return success, specified Network Dial Plan Table object affects voice line(s).
                return TRUE;
              }
              else
              {
                // Test if Authentication Security Method object is specified object.
                if ((obj_Type == MDMOID_AUTHENTICATION_SECURITY_METHOD) && (obj_ID == objPtr->usernamePassword))
                {
                  // Set provisioning value context for generic MEs (i.e. MDMOID_LARGE_STRING).
                  voipConfigContext = VOIP_CONFIG_ID_AUTH_SEC_METHOD;

                  // Return success, specified Authentication Security Method object affects voice line(s).
                  return TRUE;
                }
                else
                {
                  // Test if Large String object is specified object.
                  if ((obj_Type == MDMOID_LARGE_STRING) && (obj_ID == objPtr->userPartAor))
                  {
                    // Set provisioning value context for generic MEs (i.e. MDMOID_LARGE_STRING).
                    voipConfigContext = VOIP_CONFIG_ID_USER_PART_AOR;

                    // Return success, specified Large String object affects voice line(s).
                    return TRUE;
                  }
                  else
                  {
                    // Test if MDMOID_NETWORK_ADDRESS object is specified object.
                    if ((obj_Type == MDMOID_NETWORK_ADDRESS) && (obj_ID == objPtr->voiceMailServerSipUri))
                    {
                      // Set provisioning value context for generic MEs (i.e. MDMOID_LARGE_STRING).
                      voipConfigContext = VOIP_CONFIG_ID_VOICEMAIL_SERVER_SIP_URI;

                      // Return success, specified VoiceMail Server Sip Uri object affects voice line(s).
                      return TRUE;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  // Return failure, specified object not found in this path.
  return FALSE;
}


static UBOOL8 TestVoiceObjectID_SipUserData_Root(UINT16 obj_Type, UINT16 obj_ID, UBOOL8* lineFlagArrayPtr)
{
  SipUserDataObject* objPtr;
  InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
  int voiceLineIndex;
  UBOOL8 result = FALSE;

  // Get all SIP User Data objects.
  while (_cmsObj_getNext(MDMOID_SIP_USER_DATA, &iidStack,(void**)&objPtr) == CMSRET_SUCCESS)
  {
    // Find voice line index associated with this MDMOID_SIP_USER_DATA.
    voiceLineIndex = GetVoiceLineIndex(objPtr->pptpPointer);

    // Test for specified MDMOID_SIP_USER_DATA object.
    if ((obj_Type == MDMOID_SIP_USER_DATA) && (objPtr->managedEntityId == obj_ID))
    {
      // Signal which line is affected.
      lineFlagArrayPtr[voiceLineIndex] = TRUE;

      // Return result, MDMOID_SIP_USER_DATA object can only affect a single line.
      return TRUE;
    }
    else
    {
      // Test if PPTP POTS UNI object is (or points to) specified object.
      if (TestVoiceObjectID_PptpPots(objPtr->pptpPointer, obj_Type, obj_ID) == TRUE)
      {
        // Signal which line is affected.
        lineFlagArrayPtr[voiceLineIndex] = TRUE;

        // Setup success result, specified PPTP POTS UNI object affects voice line(s).
        result = TRUE;
      }
      else
      {
        // Test VoIP Feature Access Codes object.
        if ((obj_Type == MDMOID_VOICE_FEATURE_ACCESS_CODES) && (obj_ID == objPtr->featureCodePointer))
        {
          // Signal which line is affected.
          lineFlagArrayPtr[voiceLineIndex] = TRUE;

          // Setup success result, specified VoIP Feature Access Codes object affects voice line(s).
          result = TRUE;
        }
        else
        {
          // Test if SIP Agent Config Data object is (or points to) specified object.
          if (TestVoiceObjectID_SipAgentConfigData(objPtr->sipAgentPointer, obj_Type, obj_ID) == TRUE)
          {
            // Signal which line is affected.
            lineFlagArrayPtr[voiceLineIndex] = TRUE;

            // Setup success result, specified VoIP Feature Access Codes object affects voice line(s).
            result = TRUE;
          }
          else
          {
            // Test if VoIP Application Service Profile object is (or points to) specified object.
            if (TestVoiceObjectID_VoipApplicationServiceProfile(objPtr->appServiceProfilePointer, obj_Type, obj_ID) == TRUE)
            {
              // Signal which line is affected.
              lineFlagArrayPtr[voiceLineIndex] = TRUE;

              // Setup success result, specified VoIP Feature Access Codes object affects voice line(s).
              result = TRUE;
            }
            else
            {
              // Test if Network Dial Plan Table object is specified object.
              if ((obj_Type == MDMOID_NETWORK_DIAL_PLAN_TABLE) && (obj_ID == objPtr->networkDialPlanPointer))
              {
                // Signal which line is affected.
                lineFlagArrayPtr[voiceLineIndex] = TRUE;

                // Setup success result, specified Network Dial Plan Table object affects voice line(s).
                result = TRUE;
              }
              else
              {
                // Test if Authentication Security Method object is specified object.
                if ((obj_Type == MDMOID_AUTHENTICATION_SECURITY_METHOD) && (obj_ID == objPtr->usernamePassword))
                {
                  // Signal which line is affected.
                  lineFlagArrayPtr[voiceLineIndex] = TRUE;

                  // Setup success result, specified Authentication Security Method object affects voice line(s).
                  result = TRUE;
                }
                else
                {
                  // Test if Large String object is specified object.
                  if ((obj_Type == MDMOID_LARGE_STRING) && (obj_ID == objPtr->userPartAor))
                  {
                    // Signal which line is affected.
                    lineFlagArrayPtr[voiceLineIndex] = TRUE;

                    // Setup success result, specified Large String object affects voice line(s).
                    result = TRUE;
                  }
                  else
                  {
                    // Test if MDMOID_NETWORK_ADDRESS object is specified object.
                    if ((obj_Type == MDMOID_NETWORK_ADDRESS) && (obj_ID == objPtr->voiceMailServerSipUri))
                    {
                      // Signal which line is affected.
                      lineFlagArrayPtr[voiceLineIndex] = TRUE;

                      // Setup success result, specified VoiceMail Server Sip Uri object affects voice line(s).
                      result = TRUE;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  // Return result, TRUE if specified object found in path, FALSE if not.
  return result;
}
#endif // DMP_X_ITU_ORG_VOICE_SIP_1


static UBOOL8 TestVoiceObjectID(UINT16 obj_Type, UINT16 obj_ID, UBOOL8 voiceLineArray[])
{
  VoIpVoiceCtpObject* objPtr;
  InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
  UBOOL8 objFound = FALSE;
  int voiceLineIndex;

  // Search for VoIP Voice CTP object.
  while (_cmsObj_getNext(MDMOID_VO_IP_VOICE_CTP, &iidStack,(void**)&objPtr) == CMSRET_SUCCESS)
  {
    // Set object found flag (no need to test SIP_USER_DATA objects).
    objFound = TRUE;

    // Find voice line index associated with this MDMOID_VO_IP_VOICE_CTP.
    voiceLineIndex = GetVoiceLineIndex(objPtr->pptpPointer);

    // Test if MDMOID_PPTP_POTS_UNI (or objects under) is object in question.
    if (TestVoiceObjectID_PptpPots(objPtr->pptpPointer, obj_Type, obj_ID) == TRUE)
    {
      // Signal which line is affected.
      voiceLineArray[voiceLineIndex] = TRUE;

      // Done: MDMOID_PPTP_POTS_UNI object can only affect a single line.
      return TRUE;
    }

    // Test if VoIP Media Profile (or objects under) is object in question.
    if (TestVoiceObjectID_VoipMediaProfile(objPtr->voIpMediaProfilePointer, obj_Type, obj_ID) == TRUE)
    {
      // Signal which voice line is affected (maybe both).
      voiceLineArray[voiceLineIndex] = TRUE;
    }
    else
    {
#ifdef DMP_X_ITU_ORG_VOICE_SIP_1
      // Test if SIP User Data (or objects under) is object in question.
      if (TestVoiceObjectID_SipUserData(objPtr->userProtocolPointer, obj_Type, obj_ID) == TRUE)
      {
        // Signal which voice line is affected (maybe both).
        voiceLineArray[voiceLineIndex] = TRUE;
      }
#endif // DMP_X_ITU_ORG_VOICE_SIP_1
    }
  }

  // Test if any VoIP Voice CTP objects found.
  if (objFound == FALSE)
  {
#ifdef DMP_X_ITU_ORG_VOICE_SIP_1
    // Test SIP_USER_DATA objects since VoIP Voice CTP objects are not used.
    return TestVoiceObjectID_SipUserData_Root(obj_Type, obj_ID, voiceLineArray);
#endif // DMP_X_ITU_ORG_VOICE_SIP_1
  }

  // Return FALSE because this object type cannot affect voice stack configuration.
  return FALSE;
}


UBOOL8 FindVoiceME(UINT16 obj_Type, UINT16 obj_ID, UBOOL8 voiceLineArray[])
{
  // Init provisioning value context for generic MEs (i.e. MDMOID_LARGE_STRING).
  voipConfigContext = VOIP_CONFIG_ID_NULL;

  // Init voice line array so we know which line(s) affected.
  voiceLineArray[0] = voiceLineArray[1] = voiceLineArray[2] = 0;

  // Test if this object/ID will affect voice stack.
  return TestVoiceObjectID(obj_Type, obj_ID, voiceLineArray);
}


#ifdef DMP_X_ITU_ORG_VOICE_SIP_1
static int GetTokenLength(UINT8* bufferPtr, int maxLen)
{
  int loopIndex;

  // Loop through buffer characters trying to find last non-NULL, non-space character.
  for (loopIndex = maxLen;loopIndex > 0;--loopIndex)
  {
    // Test for NULL or ASCII space char.
    if ((bufferPtr[loopIndex - 1] != ' ') && (bufferPtr[loopIndex - 1] != 0))
    {
      // Done.
      return loopIndex;
    }
  }

  // Error.
  return 0;
}


static UBOOL8 GetNetDialPlan(UINT8 dialPlanStrArray[], const InstanceIdStack* iidStackPtr)
{
  CmsRet cmsResult = CMSRET_SUCCESS;
  InstanceIdStack iidStackTable = EMPTY_INSTANCE_ID_STACK;
  DialPlanTableObject* dialPlanEntryPtr;
  UINT8* binBufPtr;
  UINT32 binBufLen;
  UINT32 totalTokenLen = 0;
  int tokenLength;

  // Clear CMS message Dial Plan string buffer.
  memset(dialPlanStrArray, 0, MAX_DIAL_PLAN_STR_SIZE);

  // Get sub-entry MDMOID_DIAL_PLAN_TABLE
  while (_cmsObj_getNextInSubTree(MDMOID_DIAL_PLAN_TABLE, iidStackPtr, &iidStackTable, (void**)&dialPlanEntryPtr) == CMSRET_SUCCESS)
  {
    // Test for 'Add' command (ignore 'Remove' command entries).
    if (dialPlanEntryPtr->action == DIAL_PLAN_ADD_COMMAND)
    {
      // Attempt to convert HEXBINARY array to binary Dial Plan token string & test for success.
      cmsResult = cmsUtl_hexStringToBinaryBuf(dialPlanEntryPtr->dialPlanTable, &binBufPtr, &binBufLen);
      if (cmsResult == CMSRET_SUCCESS)
      {
        // Test for valid pointer and length.
        if ((binBufPtr != NULL) && (binBufLen > 0))
        {
          // Get ASCII token sub-string length (test for trailing zeros or ASCII spaces).
          tokenLength = GetTokenLength(binBufPtr, binBufLen);

          // Add token to dial plan string.
          memcpy(&dialPlanStrArray[totalTokenLen], binBufPtr, tokenLength);

          // Add delimeter "|"
          memcpy(&dialPlanStrArray[totalTokenLen+tokenLength], "|", 1);
          tokenLength++;

          // Inc total token length.
          totalTokenLen += tokenLength;
        }
      }
      else
      {
        // Signal error.
        cmsLog_error("Could not convert binary string back to HEXBINARY dialPlanTable, cmsResult = %d", cmsResult);
      }

      // Release temp memory for HEXBINARY string conversion.
      cmsMem_free(binBufPtr);
    }

    // Release MDMOID_DIAL_PLAN_TABLE record.
    _cmsObj_free((void **)&dialPlanEntryPtr);
  }

  // remove last delimeter "|"
  if (totalTokenLen > 0 && dialPlanStrArray[totalTokenLen-1] == '|')
  {
    totalTokenLen--;
    dialPlanStrArray[totalTokenLen] = '\0';
  }

  // return TRUE if valid tokens available.
  return (totalTokenLen > 0);
}

static DialPlanTableObject* FindDialPlanToken(UINT8 dialPlanID, InstanceIdStack* iidStackPtr)
{
  InstanceIdStack iidStack;
  InstanceIdStack iidStackTable = EMPTY_INSTANCE_ID_STACK;
  DialPlanTableObject* dialPlanEntryPtr;

  // Create InstanceIdStack copy.
  memcpy(&iidStack, iidStackPtr, sizeof(InstanceIdStack));

  // Get sub-entry MDMOID_DIAL_PLAN_TABLE
  while (_cmsObj_getNextInSubTree(MDMOID_DIAL_PLAN_TABLE, &iidStack, &iidStackTable, (void**)&dialPlanEntryPtr) == CMSRET_SUCCESS)
  {
    // Test for specified Dial Plan Token string.
    if (dialPlanEntryPtr->dialPlanId == dialPlanID)
    {
      // Write correct IID stack back to caller.
      *iidStackPtr = iidStackTable;

      // Specified Dial Plan Token string found.
      return dialPlanEntryPtr;
    }

    // Release MDMOID_DIAL_PLAN_TABLE record.
    _cmsObj_free((void **)&dialPlanEntryPtr);
  }

  // Specified Dial Plan Token string not found.
  return NULL;
}


static CmsRet UpdateDialPlanTokenStr(UINT8* binBufPtr, UINT32 binBufLen, const InstanceIdStack* iidStackPtr, NetworkDialPlanTableObject* dialPlanRecPtr)
{
  CmsRet cmsResult = CMSRET_SUCCESS;
  DialPlanTableObject* dialPlanEntryPtr;
  char* hexStrPtr = NULL;
  InstanceIdStack iidStack;
  InstanceIdStack iidStackRec;

  // Create InstanceIdStack copy for updating Network Dial Plan ME.
  memcpy(&iidStackRec, iidStackPtr, sizeof(InstanceIdStack));

  // Create InstanceIdStack copy.
  memcpy(&iidStack, iidStackPtr, sizeof(InstanceIdStack));

  // Attempt to find dial plan token string.
  dialPlanEntryPtr = FindDialPlanToken(binBufPtr[0], &iidStack);

  // Test for 'add' command.
  if (binBufPtr[1] == DIAL_PLAN_ADD_COMMAND)
  {
    // Test for non-existing dial plan token string.
    if (dialPlanEntryPtr == NULL)
    {
      // Test for room in dial plan table.
      if (dialPlanRecPtr->dialPlanNumber < dialPlanRecPtr->dialPlanTableMaxSize)
      {
        // Add new MDMOID_DIAL_PLAN_TABLE ME to MDM & test for success.
        cmsResult = _cmsObj_addInstance(MDMOID_DIAL_PLAN_TABLE, &iidStack);
        if (cmsResult == CMSRET_SUCCESS)
        {
          // Get created MDMOID_DIAL_PLAN_TABLE ME & test for success.
          cmsResult = _cmsObj_get(MDMOID_DIAL_PLAN_TABLE, &iidStack, 0, (void**)&dialPlanEntryPtr);
          if (cmsResult == CMSRET_SUCCESS)
          {
            // Inc current dial plan number.
            dialPlanRecPtr->dialPlanNumber++;

            // Attempt to write Network Dial Plan ME back to MDM & test for success.
            cmsResult = _cmsObj_setNoRclCallback(dialPlanRecPtr, &iidStackRec);
            if (cmsResult != CMSRET_SUCCESS)
            {
              // Signal error.
              cmsLog_error("Could not update MDMOID_NETWORK_DIAL_PLAN_TABLE, cmsResult = %d", cmsResult);
            }
          }
          else
          {
            // Signal error.
            dialPlanEntryPtr = NULL;

            // Signal error.
            cmsLog_error("Could not get just-added MDMOID_DIAL_PLAN_TABLE, cmsResult = %d", cmsResult);
          }
        }
        else
        {
          // Signal error.
          cmsLog_error("Could not add MDMOID_DIAL_PLAN_TABLE, cmsResult = %d", cmsResult);
        }
      }
      else
      {
        // Signal error.
        cmsLog_error("Could not add another MDMOID_DIAL_PLAN_TABLE, dialPlanNum=%d, maxSize=%d",
          dialPlanRecPtr->dialPlanNumber, dialPlanRecPtr->dialPlanTableMaxSize);
      }
    }

    // Test for valid MDMOID_DIAL_PLAN_TABLE ME pointer.
    if (dialPlanEntryPtr != NULL)
    {
      // Setup new MDMOID_DIAL_PLAN_TABLE ME.
      dialPlanEntryPtr->dialPlanId = binBufPtr[0];
      dialPlanEntryPtr->action = binBufPtr[1];

      // Convert binary buffer back to HEXBINARY string & test for success.
      cmsResult = cmsUtl_binaryBufToHexString(&binBufPtr[2], binBufLen - 2, &hexStrPtr);
      if (cmsResult == CMSRET_SUCCESS)
      {
        // Copy just token string into table entry.
        dialPlanEntryPtr->dialPlanTable = hexStrPtr;
      }
      else
      {
        // Signal error.
        cmsLog_error("Could not convert binary string back to HEXBINARY dialPlanTable, cmsResult = %d", cmsResult);
      }
    }
  }
  else
  {
    // Delete dial plan. / Test for non-existing dial plan token string.
    if (dialPlanEntryPtr != NULL)
    {
      // Signal that this dial plan token entry has been 'removed'.
      dialPlanEntryPtr->action = DIAL_PLAN_REMOVE_COMMAND;
    }
  }

  // Test for valid MDMOID_DIAL_PLAN_TABLE ME pointer.
  if (dialPlanEntryPtr != NULL)
  {
    // Attempt to write ME back to MDM & test for success.
    cmsResult = _cmsObj_setNoRclCallback(dialPlanEntryPtr, &iidStack);
    if (cmsResult != CMSRET_SUCCESS)
    {
      // Signal error.
      cmsLog_error("Could not set MDMOID_DIAL_PLAN_TABLE, cmsResult = %d", cmsResult);
    }

    // Release created MDMOID_DIAL_PLAN_TABLE ME.
    _cmsObj_free((void**)&dialPlanEntryPtr);
  }

  // Return combined operation result.
  return cmsResult;
}


CmsRet rutGpon_SetDialPlan
    (NetworkDialPlanTableObject* dialPlanRecPtr,
     const InstanceIdStack* iidStackPtr)
{
   CmsRet cmsResult = CMSRET_SUCCESS;
   UINT8* binBufPtr;
   UINT32 binBufLen;
   UINT8 dialPlanStrArray[MAX_DIAL_PLAN_STR_SIZE];

   memset(dialPlanStrArray, 0, sizeof(dialPlanStrArray));

   // Attempt to convert HEXBINARY array to binary Dial Plan string & test for success.
   cmsResult = cmsUtl_hexStringToBinaryBuf(dialPlanRecPtr->dialPlanTable, &binBufPtr, &binBufLen);
   if (cmsResult != CMSRET_SUCCESS)
   {
      // Signal error.
      cmsLog_error("Could not convert HEXBINARY dialPlanTable, cmsResult = %d", cmsResult);
      return cmsResult;
   }

   // Test for NULL entry.
   if (binBufPtr[0] != 0)
   {
      // Attempt to convert HEXBINARY array to binary Dial Plan string & test for success.
      cmsResult = UpdateDialPlanTokenStr(binBufPtr, binBufLen, iidStackPtr, dialPlanRecPtr);
      if (cmsResult != CMSRET_SUCCESS)
      {
         // Free temporary Dial Plan string memory allocated by CMS library.
         CMSMEM_FREE_BUF_AND_NULL_PTR(binBufPtr);

         // Signal error.
         cmsLog_error("Could not update MDMOID_DIAL_PLAN_TABLE, cmsResult = %d", cmsResult);
         return cmsResult;
      }

      // Concatenate all tokens into single dial plan string, set CMS message length appropriately & test for valid Dial Plan string.
      if (GetNetDialPlan(dialPlanStrArray, iidStackPtr) != TRUE)
      {
         cmsLog_error("Failed to cat Dial String");
      }

      // Free temporary Dial Plan string memory allocated by CMS library.
      CMSMEM_FREE_BUF_AND_NULL_PTR(binBufPtr);
   }

   // Setup TR-104 RTP Profile Data object.
   _owapi_rut_MapNetworkDialPlan(dialPlanStrArray, dialPlanRecPtr->criticalDialTimeout, dialPlanRecPtr->partialDialTimeout);

   // Return operation result.
   return cmsResult;
}
#endif // DMP_X_ITU_ORG_VOICE_SIP_1


#endif // DMP_X_ITU_ORG_VOICE_1
#endif // DMP_X_ITU_ORG_GPON_1
