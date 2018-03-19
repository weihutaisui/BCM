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

#include "cms_util.h"
#include "cms_core.h"
#include "cms_dal.h"

/*!\file dal_auth.c
 *
 * This file contains common authentication functions that are used by
 * httpd, telnetd, sshd, consoled.
 *
 * NOTE: these interfaces require the caller to obtain the cms lock 
 */


#if defined(DMP_BASELINE_1)
NetworkAccessMode cmsDal_getNetworkAccessMode_igd(CmsEntityId eid __attribute__((unused)),
                                              const char *addr)
{
   NetworkAccessMode accessMode = NETWORK_ACCESS_DISABLED;
   UBOOL8 isOnLanSide = FALSE;


   if ((addr != NULL) && (strchr(addr, ':') == NULL))
   {
      /*
       * Addr is an IPv4 address (no : character in address).  Check with
       * the data model.
       */

      InstanceIdStack       lanDevIidStack = EMPTY_INSTANCE_ID_STACK;
      InstanceIdStack       ipIfIdStack = EMPTY_INSTANCE_ID_STACK;
      CmsRet                ret;
      LanDevObject          *lanDeviceObj = NULL;
      LanIpIntfObject       *lanIpObj = NULL;

      struct in_addr clntAddr, inAddr, inMask;

#ifdef DESKTOP_LINUX
      /*
       * Many desktop linux tests occur on loopback interface.  Consider that
       * as LAN side.
       */
      if (!strcmp(addr, "127.0.0.1"))
      {
         isOnLanSide = TRUE;
      }
#endif

      clntAddr.s_addr = inet_addr(addr);

      /* If in ppp ip extension, and the ipAddr is same as the ppp externalIpAddress,
      * set isOnLanSide to TRUE to allow lan side access to the modem.
      *
      * NOTE: For ftpd, the auth checking is hardcoded in ftpd (main.c) and will not come to here.  
      * Therefore in ppp ip extension, use ftp update image will not work unless special modification to
      * that part of code is made.
      */
      if (dalWan_isPPPIpExtension())
      {
         WanPppConnObject *pppConn = NULL;
         InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

         if (cmsObj_getNext(MDMOID_WAN_PPP_CONN, &iidStack,(void **) &pppConn) != CMSRET_SUCCESS)
         {
            cmsLog_error("no pppConn in the modem ? Just return FALSE");
            return FALSE;
         }

         if (!cmsUtl_strcmp(pppConn->externalIPAddress, addr))
         {
            isOnLanSide = TRUE;
            cmsLog_debug("Access from LAN side in PPP ip extension.");
         }
         
         cmsObj_free((void **) &pppConn);
      }
      
      while(!isOnLanSide && (ret = cmsObj_getNext(MDMOID_LAN_DEV, &lanDevIidStack, (void **)&lanDeviceObj)) == CMSRET_SUCCESS)
      {
         cmsObj_free((void **)&lanDeviceObj);

         while((ret = cmsObj_getNextInSubTree(MDMOID_LAN_IP_INTF, &lanDevIidStack, &ipIfIdStack, (void **)&lanIpObj)) == CMSRET_SUCCESS)
         {
            inAddr.s_addr = inet_addr(lanIpObj->IPInterfaceIPAddress);
            inMask.s_addr = inet_addr(lanIpObj->IPInterfaceSubnetMask);

            if ( (clntAddr.s_addr & inMask.s_addr) == (inAddr.s_addr & inMask.s_addr) )
            {
               isOnLanSide = TRUE;
            }

            cmsObj_free((void **)&lanIpObj);           
         }
      } 
   }
   else
   {
      /*
       * Addr is an IPv6 address.  Call cmsNet_isAddressOnLanSide, which is
       * IPv6 aware.  It will look directly on Linux interfaces
       */
      isOnLanSide = cmsNet_isAddressOnLanSide(addr);
   }
   cmsLog_debug("%s isOnLanSide=%d", addr, isOnLanSide);

   accessMode = (isOnLanSide) ? NETWORK_ACCESS_LAN_SIDE : NETWORK_ACCESS_WAN_SIDE;

   cmsLog_debug("returning accessMode=%d", accessMode);
   return accessMode;
}

#endif  /* DMP_BASELINE_1 */


#if defined(DMP_DEVICE2_BASELINE_1)
NetworkAccessMode cmsDal_getNetworkAccessMode_dev2(CmsEntityId eid __attribute((unused)),
                                                   const char *addr)
{
   NetworkAccessMode accessMode = NETWORK_ACCESS_DISABLED;
   UBOOL8 isOnLanSide = FALSE;


   if ((addr != NULL) && (strchr(addr, ':') == NULL))
   {
      /*
       * Addr is an IPv4 address (no : character in address).  Check with
       * the data model.
       */
      Dev2IpInterfaceObject *ipIntfObj = NULL;
      InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
      struct in_addr clntAddr, inAddr, inMask;

      clntAddr.s_addr = inet_addr(addr);

#ifdef DESKTOP_LINUX
      /*
       * Many desktop linux tests occur on loopback interface.  Consider that
       * as LAN side.
       */
      if (!strcmp(addr, "127.0.0.1"))
      {
         isOnLanSide = TRUE;
      }
#endif

      /* XXX TODO: PPP IP Extension if (dalWan_isPPPIpExtension()) */

      /*
       * Just look on LAN side.  If not found on LAN side, assume it is
       * a WAN side address.
       */
      while (!isOnLanSide &&
             (cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, &iidStack,
                                  OGF_NO_VALUE_UPDATE,
                                  (void **)&ipIntfObj) == CMSRET_SUCCESS))
      {
         if (ipIntfObj->X_BROADCOM_COM_Upstream == FALSE)
         {
            InstanceIdStack childIidStack=EMPTY_INSTANCE_ID_STACK;
            Dev2Ipv4AddressObject *ipv4Addr=NULL;

            while (!isOnLanSide &&
                   (cmsObj_getNextInSubTreeFlags(MDMOID_DEV2_IPV4_ADDRESS,
                                       &iidStack,
                                       &childIidStack,
                                       OGF_NO_VALUE_UPDATE,
                                       (void **) &ipv4Addr) == CMSRET_SUCCESS))
            {
               if (IS_EMPTY_STRING(ipv4Addr->IPAddress) ||
                   IS_EMPTY_STRING(ipv4Addr->subnetMask))
               {
                  cmsLog_error("Empty IPAddr or subnetMask detected, skip it");
               }
               else
               {
                  inAddr.s_addr = inet_addr(ipv4Addr->IPAddress);
                  inMask.s_addr = inet_addr(ipv4Addr->subnetMask);

                  if ( (clntAddr.s_addr & inMask.s_addr) == (inAddr.s_addr & inMask.s_addr) )
                  {
                     isOnLanSide = TRUE;
                  }
               }

               cmsObj_free((void **) &ipv4Addr);
            }
         }

         cmsObj_free((void **)&ipIntfObj);
      }
   }
   else
   {
      /*
       * Addr is an IPv6 address.  Call the cmsNet function, which is IPv6
       * aware.  It will look directly on Linux interfaces
       */
      isOnLanSide = cmsNet_isAddressOnLanSide(addr);
   }


   accessMode = (isOnLanSide) ? NETWORK_ACCESS_LAN_SIDE : NETWORK_ACCESS_WAN_SIDE;
   cmsLog_debug("%s isOnLanSide=%d accessMode=%d", addr, isOnLanSide, accessMode);

   return accessMode;
}
#endif  /* DMP_DEVICE2_BASELINE_1 */


UBOOL8 cmsDal_authenticate(HttpLoginType *authLevel, NetworkAccessMode accessMode, const char *username, const char *passwd)
{
   UBOOL8 authSuccess=FALSE;
   CmsRet ret;
   LoginCfgObject *loginObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
#ifdef SUPPORT_HASHED_PASSWORDS
   char *encrypted;
#endif

   ret = cmsObj_get(MDMOID_LOGIN_CFG, &iidStack, 0, (void **) &loginObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("get of LOGIN_CFG failed, ret=%d", ret);
      return FALSE;
   }

   switch(accessMode)
   {
   case NETWORK_ACCESS_LAN_SIDE:
   case NETWORK_ACCESS_CONSOLE:
      {
#ifdef SUPPORT_HASHED_PASSWORDS
         char *pPasswd = "";
         if ( 0 == strcmp(loginObj->adminUserName, username) )
         {
            pPasswd = loginObj->adminPasswordHash;
            *authLevel = LOGIN_ADMIN;
         }
         else if ( 0 == strcmp(loginObj->userUserName, username) )
         {
            pPasswd = loginObj->userPasswordHash;
            *authLevel = LOGIN_USER;
         }
         else
         {
            break;
         }
         encrypted = cmsUtil_pwEncrypt( passwd, pPasswd );
         if ( 0 == strcmp(encrypted, pPasswd) )
         {
            authSuccess = TRUE;
         }
#else
         if (!strcmp(loginObj->adminUserName, username) && !strcmp(loginObj->adminPassword, passwd)) 
         {
            *authLevel = LOGIN_ADMIN;
            authSuccess = TRUE;
         }

         else if (!strcmp(loginObj->userUserName, username) && !strcmp(loginObj->userPassword, passwd))
         {
            *authLevel = LOGIN_USER;
            authSuccess = TRUE;
         }
#endif
         break;
      }

   case NETWORK_ACCESS_WAN_SIDE:
      {
#ifdef SUPPORT_HASHED_PASSWORDS
         encrypted = cmsUtil_pwEncrypt( passwd, loginObj->supportPasswordHash );
         if ( strcmp ( encrypted, loginObj->supportPasswordHash ) == 0 )
         {
            authSuccess = TRUE;
         }
#else
         if ((!strcmp(loginObj->supportUserName, username) && !strcmp(loginObj->supportPassword, passwd)))
         {
            authSuccess = TRUE;
         }
#endif
         *authLevel = LOGIN_SUPPORT;
         break;
      }

   case NETWORK_ACCESS_DISABLED:
      /* caller should have caught this, auth definately fails, do nothing. */
      break;
   }

   cmsObj_free((void **) &loginObj);

   return authSuccess;
}

