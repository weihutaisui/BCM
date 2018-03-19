/***********************************************************************
 *
 * <:copyright-BRCM:2007:proprietary:standard
 * 
 *    Copyright (c) 2007 Broadcom 
 *    All Rights Reserved
 * 
 *  This program is the proprietary software of Broadcom and/or its
 *  licensors, and may only be used, duplicated, modified or distributed pursuant
 *  to the terms and conditions of a separate, written license agreement executed
 *  between you and Broadcom (an "Authorized License").  Except as set forth in
 *  an Authorized License, Broadcom grants no license (express or implied), right
 *  to use, or waiver of any kind with respect to the Software, and Broadcom
 *  expressly reserves all rights in and to the Software and all intellectual
 *  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 *  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 *  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 * 
 *  Except as expressly set forth in the Authorized License,
 * 
 *  1. This program, including its structure, sequence and organization,
 *     constitutes the valuable trade secrets of Broadcom, and you shall use
 *     all reasonable efforts to protect the confidentiality thereof, and to
 *     use this information only in connection with your use of Broadcom
 *     integrated circuit products.
 * 
 *  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
 *     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
 *     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
 *     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
 *     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
 *     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
 *     PERFORMANCE OF THE SOFTWARE.
 * 
 *  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
 *     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
 *     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
 *     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 *     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
 *     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
 *     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
 *     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
 *     LIMITED REMEDY.
 * :>
 *
 ************************************************************************/

#include "cms_util.h"
#include "cms_msg.h"
#include "cms_cli.h"
#include "cli.h"

/*
 * These are validation commands that are only used by the CLI menu.
 */

/***************************************************************************
// Function Name: ValidateMacAddress
// Description  : validate format of the MAC address.
// Parameters   : addr - MAC address.
// Returns      : FALSE - invalid format.
//                TRUE - valid format.
****************************************************************************/
UBOOL8 cli_isMacAddress(char *addr) 
{
   UBOOL8 ret = FALSE;
   int i = 0;
   char *pToken = NULL, *pLast = NULL, *pEnd = NULL;
   char buf[18];
   long num = 0;

   if ( addr == NULL || (strlen(addr) > 18) )
      return ret;

      // need to copy since strtok_r updates string
   strcpy(buf, addr);

   // IP address has the following format
   //   xxx.xxx.xxx.xxx where x is decimal number
   pToken = strtok_r(buf, ":", &pLast);
   if ( pToken == NULL )
      return ret;
   num = strtol(pToken, &pEnd, 16);

   if ( *pEnd == '\0' && num <= 255 ) 
   {
      for ( i = 0; i < 5; i++ ) 
      {
         pToken = strtok_r(NULL, ":", &pLast);
         if ( pToken == NULL )
            break;
         num = strtol(pToken, &pEnd, 16);
         if ( *pEnd != '\0' || num > 255 )
            break;
      }
      if ( i == 5 )
         ret = TRUE;
   }
   return ret;
}

/***************************************************************************
// Function Name: isIpAddress
// Description  : validate format of the IP address.
// Parameters   : buf - IP address.
// Returns      : CLI_FALSE - invalid format.
//                CLI_TRUE - valid format.
****************************************************************************/
UBOOL8 cli_isIpAddress(const char *addr)
{
   if (addr != NULL)
   {
      struct in_addr inp;

      if (inet_aton(addr, &inp))
      {
         return TRUE;
      }
   }
   return FALSE;
}

/***************************************************************************
// Function Name: isNumber
// Description  : validate decimal number from string. 
// Parameters   : buf - decimal number.
// Returns      : CLI_FALSE - invalid decimal number.
//                CLI_TRUE - valid decimal number.
****************************************************************************/
UBOOL8 cli_isNumber(const char *buf)
{
   if ( buf != NULL )
   {
      int size = strlen(buf);
      int i;

      for ( i = 0; i < size; i++ )
      {
         if ( isdigit(buf[i]) == 0 )
         {
            break;
         }
      }
      if ( size > 0 && i == size )
      {
         return TRUE;
      }
   }
   return FALSE;
}

/***************************************************************************
// Function Name: isValidState
// Description  : validate WAN state.
// Parameters   : state - WAN state.
// Returns      : FALSE - invalid format.
//                TRUE - valid format.
****************************************************************************/
UBOOL8 cli_isValidState(const char *state)
{
   if (state != NULL)
   {
      int select = atoi(state);
      return ( select <= 2 && select >= 1 );
   }
   return FALSE;
}

/***************************************************************************
// Function Name: isValidVpi
// Description  : validate VPI range.
// Parameters   : vpi - VPI.
// Returns      : CLI_FALSE - invalid format.
//                CLI_TRUE - valid format.
****************************************************************************/
UBOOL8 cli_isValidVpi(const char *vpi)
{
   if ( cli_isNumber(vpi) )
   {
      int num = atoi(vpi);
      if ( num <= 255 && num >= 0 )
         return TRUE;
      else
         printf("\nvpi is out of range [0-255]\n");
   }
   else
   {
      printf("\nInvalid vpi %s\n", vpi? vpi : "");
   }
   return FALSE;
}

/***************************************************************************
// Function Name: isValidVci
// Description  : validate VCI range.
// Parameters   : vci - VCI.
// Returns      : CLI_FALSE - invalid format.
//                CLI_TRUE - valid format.
****************************************************************************/
UBOOL8 cli_isValidVci(const char *vci)
{
   if ( cli_isNumber(vci) )
   {
      int num = atoi(vci);
      if ( num <= 65535 && num >= 32 )
         return TRUE;
      else
         printf("\nvci is out of range [32-65535]\n");
   }
   else
   {
      printf("\nInvalid vci %s\n", vci? vci : "");
   }
   return FALSE;
}

/***************************************************************************
// Function Name: isValidWanId
// Description  : validate WAN connection ID range.
// Parameters   : id - WAN ID.
// Returns      : CLI_FALSE - invalid format.
//                CLI_TRUE - valid format.
****************************************************************************/
UBOOL8 cli_isValidWanId(const char *id)
{
   if ( cli_isNumber(id) )
   {
      int num = atoi(id);
      if ( num <= IFC_WAN_MAX && num >= 0 )
         return TRUE;
      else
         printf("\nwan id is out of range [0-%d]\n", IFC_WAN_MAX);
   }
   else
   {
      printf("\nInvalid wan id %s\n", id? id : "");
   }
   return FALSE;
}

/***************************************************************************
// Function Name: isValidVlanId
// Description  : validate VLAN ID range.
// Parameters   : id - WAN ID.
// Returns      : CLI_FALSE - invalid format.
//                CLI_TRUE - valid format.
****************************************************************************/
UBOOL8 cli_isValidVlanId(const char *id)
{
   if ( cli_isNumber(id) )
   {
      int num = atoi(id);
      if ( num <= IFC_VLAN_MAX && num >= 0 )
         return TRUE;
      else
         printf("\nvlan id is out of range [0-%d]\n", IFC_VLAN_MAX);
   }
   else
   {
      printf("\nInvalid vlan id %s\n", id? id : "");
   }
   return FALSE;
}

#if 0
/***************************************************************************
// Function Name: isValidDhcpSrvMode
// Description  : validate DHCP server mode.
// Parameters   : mode - DHCP server mode.
// Returns      : FALSE - invalid format.
//                TRUE - valid format.
****************************************************************************/
UBOOL8 cli_isValidDhcpSrvMode(char *state) {
   UBOOL8 ret = FALSE;
   int select = atoi(state);

   if ( select <= 2 && select >= 1 )
      ret = TRUE;
   else if ( select == 3 ) { // DHCP relay
      WAN_CON_ID wanId;
      WAN_CON_INFO wanInfo;

      // init wanId to get WAN info from the beginning
      wanId.vpi = wanId.vci = wanId.conId = 0;   
      while ( BcmDb_getWanInfoNext(&wanId, &wanInfo) == DB_WAN_GET_OK ) {
         if ( (wanInfo.protocol == PROTO_IPOA ||
#if SUPPORT_ETHWAN
               wanInfo.protocol == PROTO_IPOWAN  ||
#endif
               wanInfo.protocol == PROTO_MER) &&
              wanInfo.flag.nat == FALSE &&
              wanInfo.flag.service == TRUE ) {
            ret = TRUE;
            break;
         }
      }
      if ( ret == FALSE )
         printf("DHCP relay only works in non-NATed IPoA or MER mode.\n");
   }

   return ret;
}
#endif

/***************************************************************************
// Function Name: cli_atmAddrToNum
// Description  : convert port.vpi.vci to number.
// Parameters   : atmAddr - string to be converted.
//                     port, vpi, vci - return value
// Returns      : FALSE - invalid format.
//                TRUE - valid format.
****************************************************************************/
UBOOL8 cli_atmAddrToNum(const char *atmAddr, int *port, int *vpi, int *vci)
{
   UBOOL8 ret = FALSE;
   char buf[BUFLEN_256];
   char *pToken = NULL, *pLast = NULL;
      
   
   // need to copy since strtok_r updates string
   strcpy(buf, atmAddr);

   // VCC port
   if ((pToken = strtok_r(buf, ".", &pLast)) != NULL)
   {
      if (cli_isNumber(pToken))
      {
         *port = atoi(pToken);
         // VCC vpi
         if ((pToken = strtok_r(NULL, ".", &pLast)) != NULL)
         {
            if (cli_isValidVpi(pToken))
            {
               *vpi = atoi(pToken);
               // VCC vci
               if ((pToken = strtok_r(NULL, ".", &pLast)) != NULL)
               {
                  if (cli_isValidVci(pToken))
                  {
                     *vci = atoi(pToken);
                     ret = TRUE;
                  }
               }
            }
         }
      }
   }
   return ret;
}

/***************************************************************************
// Function Name: isValidIdleTimeout
// Description  : validate PPP idle timeout.
// Parameters   : timeout - PPP idle timeout.
// Returns      : FALSE - invalid format.
//                TRUE - valid format.
****************************************************************************/
UBOOL8 cli_isValidIdleTimeout(const char *timeout)
{
   if ( cli_isNumber(timeout) )
   {
      int num = atoi(timeout);
      if ( num <= 1090 && num >= 0 )
         return TRUE;
      else
         printf("\nidle timeout is out of range [0-1090]\n");
   }
   else
   {
      printf("\nInvalid idle timeout %s\n", timeout? timeout : "");
   }
   return FALSE;
}

/***************************************************************************
// Function Name: isValidWanServiceName
// Description  : validate WAN service name.
// Parameters   : username - PPP user name.
// Returns      : FALSE - invalid format.
//                TRUE - valid format.
****************************************************************************/
UBOOL8 cli_isValidWanServiceName(const char *service)
{
   if ( service != NULL )
   {
      int len = strlen(service);

      if ( len <= 32 && len >= 0 )
         return TRUE;
      else
         printf("\nlength of service name is out of range [0-32]\n");
   }
   else
   {
      printf("\nInvalid wan service name %s\n", service? service : "");
   }
   return FALSE;
}

/***************************************************************************
// Function Name: isValidPppUserName
// Description  : validate PPP user name.
// Parameters   : username - PPP user name.
// Returns      : FALSE - invalid format.
//                TRUE - valid format.
****************************************************************************/
UBOOL8 cli_isValidPppUserName(const char *username)
{
   if ( username != NULL )
   {
      int len = strlen(username);

      if ( len <= 256 && len >= 0 )
         return TRUE;
      else
         printf("\nlength of username is out of range [0-256]\n");
   }
   else
   {
      printf("\nInvalid username %s\n", username? username : "");
   }
   return FALSE;
}

/***************************************************************************
// Function Name: isValidPppPassword
// Description  : validate PPP password.
// Parameters   : password - PPP password.
// Returns      : FALSE - invalid format.
//                TRUE - valid format.
****************************************************************************/
UBOOL8 cli_isValidPppPassword(const char *password)
{
   if ( password != NULL )
   {
      int len = strlen(password);

      if ( len <= 32 && len >= 0 )
         return TRUE;
      else
         printf("\nlength of password is out of range [0-32]\n");
   }
   else
   {
      printf("\nInvalid password %s\n", password? password : "");
   }
   return FALSE;
}

/***************************************************************************
// Function Name: isValidService
// Description  : validate service control selection.
// Parameters   : service - service control selection string.
//                where service is
//                     1=ftp, 2=http, 3=icmp, 4=snmp, 5=ssh, 6=telnet, 7=tftp
// Returns      : FALSE - invalid format.
//                TRUE - valid format.
****************************************************************************/
UBOOL8 cli_isValidService(const char *service)
{
   int choice = atoi(service);
   return ( choice <= 7 && choice >= 1 );
}


UBOOL8 cli_isValidL2IfName_igd(const char *l2IfName)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   char wanL2IfName[CMS_IFNAME_LENGTH];
   char *p;
   UBOOL8 foundLinkCfg=FALSE;
   UBOOL8 isValid=TRUE;
   
   strncpy(wanL2IfName, l2IfName, CMS_IFNAME_LENGTH-1);
   if ((p = strchr(wanL2IfName, '/')))
   {
      /* now wanL2IfName is "ptm0", "atm0" etc. */
      *p = '\0';
   }
#ifdef DMP_ETHERNETWAN_1
   else if (!(cmsUtl_strstr(wanL2IfName, ETH_IFC_STR)))
#else
   else
#endif
   {
      cmsLog_error("wanL2IfName %s - wrong format", wanL2IfName);
      return FALSE;
   }

#ifdef DMP_BASELINE_1
#ifdef DMP_ETHERNETWAN_1  /* aka SUPPORT_ETHWAN */
   if (cmsUtl_strstr(wanL2IfName, ETH_IFC_STR) != NULL)
   {
      foundLinkCfg = dalEth_getEthIntfByIfName(wanL2IfName, &iidStack, NULL);
   }
#endif
#endif  /* DMP_BASELINE_1 */

#ifdef DMP_ADSLWAN_1   
#ifdef DMP_PTMWAN_1
   if (cmsUtl_strstr(wanL2IfName, PTM_IFC_STR) != NULL)
   {
      foundLinkCfg = dalDsl_getPtmLinkByIfName(wanL2IfName, &iidStack, NULL);
   }      
      
#endif

   if (!foundLinkCfg && 
       (cmsUtl_strstr(wanL2IfName, ATM_IFC_STR) != NULL ||
        cmsUtl_strstr(wanL2IfName, IPOA_IFC_STR) != NULL))
   {
      foundLinkCfg = dalDsl_getDslLinkByIfName(wanL2IfName, &iidStack, NULL);
   }
#endif /* DMP_ADSLWAN_1 */

   /* todo: Currently, connMode and vlanMux is not supported in cli.  Need to modified this if these 
   * are supported later on.
   */
   if (foundLinkCfg)
   {
      InstanceIdStack wanConnIidStack=EMPTY_INSTANCE_ID_STACK;   
      InstanceIdStack savedLinkIidStack=iidStack;   
      WanIpConnObject *ipConn=NULL;
      WanPppConnObject *pppConn=NULL;
      
      while (isValid &&
         cmsObj_getNextInSubTreeFlags(MDMOID_WAN_IP_CONN, &iidStack, &wanConnIidStack, OGF_NO_VALUE_UPDATE, (void **)&ipConn) == CMSRET_SUCCESS)
      {
         cmsObj_free((void **) &ipConn);
         isValid = FALSE;
      }

      savedLinkIidStack=iidStack;
      INIT_INSTANCE_ID_STACK(&wanConnIidStack);
      while (isValid &&
         cmsObj_getNextInSubTreeFlags(MDMOID_WAN_PPP_CONN, &savedLinkIidStack, &wanConnIidStack, OGF_NO_VALUE_UPDATE, (void **)&pppConn) == CMSRET_SUCCESS)
      {
         cmsObj_free((void **) &pppConn);
         isValid = FALSE;
      }
   }
   else
   {
      isValid = FALSE;
   }
   
   return isValid;

}


#if 0
/***************************************************************************
// Function Name: isValidLogLevel
// Description  : validate system log level.
// Parameters   : level - system log level.
// Returns      : FALSE - invalid format.
//                TRUE - valid format.
****************************************************************************/
UBOOL8 cli_isValidLogLevel(char *level) {
   UBOOL8 ret = FALSE;
   int select = atoi(level);

   if ( select <= LOG_DEBUG && select >= LOG_EMERG )
      ret = TRUE;

   return ret;
}

/***************************************************************************
// Function Name: isValidLogMode
// Description  : validate system log mode.
// Parameters   : mode - system log mode.
// Returns      : FALSE - invalid format.
//                TRUE - valid format.
****************************************************************************/
UBOOL8 cli_isValidLogMode(char *mode) {
   UBOOL8 ret = FALSE;
   int select = atoi(mode);

   if ( select <= BCM_SYSLOG_LOCAL_REMOTE && select >= BCM_SYSLOG_LOCAL )
      ret = TRUE;

   return ret;
}

/***************************************************************************
// Function Name: isValidSecFilterName
// Description  : validate security filter name.
// Parameters   : name - security filter name.
// Returns      : FALSE - invalid format.
//                TRUE - valid format.
****************************************************************************/
UBOOL8 cli_isValidSecFilterName(char *name) {
   UBOOL8 ret = FALSE;

   if ( name[0] != '\0' )
      ret = TRUE;

   return ret;
}

/***************************************************************************
// Function Name: isValidSecProtocol
// Description  : validate security protocol.
// Parameters   : protocol - security protocol.
// Returns      : FALSE - invalid format.
//                TRUE - valid format.
****************************************************************************/
UBOOL8 cli_isValidSecProtocol(char *protocol) {
   UBOOL8 ret = FALSE;
   int select = atoi(protocol);

   if ( (select <= SEC_PROTO_ICMP && select >= SEC_PROTO_BOTH) ||
        (select == TOS_NONE) )
      ret = TRUE;

   return ret;
}

/***************************************************************************
// Function Name: isValidSecIpAddress
// Description  : validate security IP address.
// Parameters   : addr - security IP address.
// Returns      : FALSE - invalid format.
//                TRUE - valid format.
****************************************************************************/
UBOOL8 cli_isValidSecIpAddress(char *addr) {
   if ( addr == NULL ) return FALSE;

   // security IP address field can be empty
   if ( addr[0] == '\0' ) return TRUE;

   return (BcmCli_isIpAddress(addr));
}

/***************************************************************************
// Function Name: isValidSecPort
// Description  : validate security port.
// Parameters   : port - security port.
// Returns      : FALSE - invalid format.
//                TRUE - valid format.
****************************************************************************/
UBOOL8 cli_isValidSecPort(char *port) {
   if ( port == NULL ) return FALSE;

   // security port field can be empty
   if ( port[0] == '\0' ) return TRUE;

   UBOOL8 ret = FALSE;
   int frmPort = 0, toPort = 0;
   char *cp = NULL, buf[IFC_MEDIUM_LEN];

   strcpy(buf, port);
   cp = strchr(buf, ':');
   if ( cp == NULL ) {
      frmPort = atoi(buf);
      if ( frmPort <= 65535 && frmPort >= 1 )
         ret = TRUE;
   } else {
      *cp = '\0';
      frmPort = atoi(buf);
      if ( frmPort <= 65535 && frmPort >= 1 ) {
         toPort = atoi(++cp);
         if ( toPort <= 65535 && toPort >= 1 ) {
            if ( frmPort <= toPort )
               ret = TRUE;
         }
      }
   }

   return ret;
}

/***************************************************************************
// Function Name: isValidSecWanIfc
// Description  : validate security WAN interface.
// Parameters   : IFC - security WAN interface name.
// Returns      : FALSE - invalid format.
//                TRUE - valid format.
****************************************************************************/
UBOOL8 cli_isValidSecWanIfc(char *ifc) {
   if ( ifc == NULL ) return FALSE;

   // security WAN interface field can be empty
   if ( ifc[0] == '\0' ) return TRUE;

   // "1"  is valid for all interfaces
   if ( strncasecmp(ifc, "1", sizeof("1")) == 0 ) return TRUE;

   UBOOL8 ret = FALSE;
   char interface[IFC_TINY_LEN];
   WAN_CON_ID wanId;
   WAN_CON_INFO wanInfo;

   // init wanId to get WAN info from the beginning
   wanId.vpi = wanId.vci = wanId.conId = 0;
   
   while ( BcmDb_getWanInfoNext(&wanId, &wanInfo) == DB_WAN_GET_OK ) {
      if ( wanInfo.protocol != PROTO_BRIDGE && wanInfo.flag.service == TRUE ) {
         BcmDb_getWanInterfaceName(&wanId, wanInfo.protocol, interface);
         if ( strcasecmp(ifc, interface) == 0 ) {
            ret = TRUE;
            break;
         }
      }
   }

   return ret;
}

/***************************************************************************
// Function Name: isValidMacProtocol
// Description  : validate MAC protocol.
// Parameters   : protocol - MAC protocol.
// Returns      : FALSE - invalid format.
//                TRUE - valid format.
****************************************************************************/
UBOOL8 cli_isValidMacProtocol(char *protocol) {
   UBOOL8 ret = FALSE;
   int select = atoi(protocol);

   if ( select <= SEC_PROTO_NEtBEUI && select >= SEC_PROTO_NONE )
      ret = TRUE;

   return ret;
}

/***************************************************************************
// Function Name: isValidSecMacAddress
// Description  : validate security MAC address.
// Parameters   : addr - security MAC address.
// Returns      : FALSE - invalid format.
//                TRUE - valid format.
****************************************************************************/
UBOOL8 cli_isValidSecMacAddress(char *addr) {
   if ( addr == NULL ) return FALSE;

   // security MAC address field can be empty
   if ( addr[0] == '\0' ) return TRUE;

   return (BcmCli_isMacAddress(addr));
}

/***************************************************************************
// Function Name: isValidMacFrameDirection
// Description  : validate MAC direction.
// Parameters   : direction - MAC direction.
// Returns      : FALSE - invalid format.
//                TRUE - valid format.
****************************************************************************/
UBOOL8 cli_isValidMacFrameDirection(char *direction) {
   UBOOL8 ret = FALSE;
   int select = atoi(direction);

   if ( select <= BOTH && select >= LAN_TO_WAN )
      ret = TRUE;

   return ret;
}

/***************************************************************************
// Function Name: isValidSecBridgeIfc
// Description  : validate security bridge interface.
// Parameters   : IFC - security bridge interface name.
// Returns      : FALSE - invalid format.
//                TRUE - valid format.
****************************************************************************/
UBOOL8 cli_isValidSecBridgeIfc(char *ifc) {
   if ( ifc == NULL ) return FALSE;

   // security bridge interface field can be empty
   if ( ifc[0] == '\0' ) return TRUE;

   // "1"  is valid for all interfaces
   if ( strncasecmp(ifc, "1", sizeof("1")) == 0 ) return TRUE;

   UBOOL8 ret = FALSE;
   char interface[IFC_TINY_LEN];
   WAN_CON_ID wanId;
   WAN_CON_INFO wanInfo;

   // init wanId to get WAN info from the beginning
   wanId.vpi = wanId.vci = wanId.conId = 0;
   
   while ( BcmDb_getWanInfoNext(&wanId, &wanInfo) == DB_WAN_GET_OK ) {
      if ( wanInfo.protocol == PROTO_BRIDGE && wanInfo.flag.service == TRUE ) {
         BcmDb_getWanInterfaceName(&wanId, wanInfo.protocol, interface);
         if ( strcasecmp(ifc, interface) == 0 ) {
            ret = TRUE;
            break;
         }
      }
   }

   return ret;
}

/***************************************************************************
// Function Name: isValidQosPriority
// Description  : validate QoS priority.
// Parameters   : priority - QoS priority.
// Returns      : FALSE - invalid format.
//                TRUE - valid format.
****************************************************************************/
UBOOL8 cli_isValidQosPriority(char *priority) {
   UBOOL8 ret = FALSE;
   int select = atoi(priority);
   extern int glbQosPrioLevel;          // defined in cgimain.c used for how many Qos Level the system support.

   if ( select <= glbQosPrioLevel && select >= QOS_PRIORITY_LOW )
      ret = TRUE;

   return ret;
}

/***************************************************************************
// Function Name: isValidQosPrecedence
// Description  : validate QoS precedence.
// Parameters   : precedence - QoS precedence.
// Returns      : FALSE - invalid format.
//                TRUE - valid format.
****************************************************************************/
UBOOL8 cli_isValidQosPrecedence(char *precedence) {
   UBOOL8 ret = FALSE;
   int select = atoi(precedence);

   if ( (select <= PRECEDENCE_MAX && select >= PRECEDENCE_MIN) ||
        select == PRECEDENCE_NONE )
      ret = TRUE;

   return ret;
}

/***************************************************************************
// Function Name: isValidQosTos
// Description  : validate QoS type of service.
// Parameters   : tos - QoS type of service.
// Returns      : FALSE - invalid format.
//                TRUE - valid format.
****************************************************************************/
UBOOL8 cli_isValidQosTos(char *tos) {
   UBOOL8 ret = FALSE;
   int select = atoi(tos);

   switch ( select ) {
      case TOS_NONE:
      case TOS_NORMAL_SERVICE:
      case TOS_MINIMIZE_COST:
      case TOS_MAXIMIZE_RELIABILITY:
      case TOS_MAXIMIZE_THROUGHPUT:
      case TOS_MINIMIZE_DELAY:
         ret = TRUE;
         break;
   }

   return ret;
}

/***************************************************************************
// Function Name: cliVerifyRipOp
// Description  : validate RIP operation.
// Parameters   : input - RIP operation.
// Returns      : FALSE - invalid format.
//                TRUE - valid format.
****************************************************************************/
UBOOL8 cli_cliVerifyRipOp(char *input)
{
  if ((strcasecmp(input,"1") == 0) ||
      (strcasecmp(input,"2") == 0))
    return TRUE;
  else
    return FALSE;
}

/***************************************************************************
// Function Name: cliVerifyRipVersion
// Description  : validate RIP version.
// Parameters   : input - RIP version.
// Returns      : FALSE - invalid format.
//                TRUE - valid format.
****************************************************************************/
UBOOL8 cli_cliVerifyRipVersion(char *input)
{
  if ((strcasecmp(input,"3") == 0) ||
      (strcasecmp(input,"1")) ||
      (strcasecmp(input,"2")))
    return TRUE;
  else
    return FALSE;
}

#ifdef ETH_CFG
/***************************************************************************
// Function Name: isValidEthMtu
// Description  : Eth Cfg MTU
// Parameters   : state - Eth Cfg state.
// Returns      : FALSE - invalid format.
//                TRUE - valid format.
****************************************************************************/
UBOOL8 cli_isValidEthMtu(char *mtu) {
   UBOOL8 ret = FALSE;
   int value = atoi(mtu);

   if ( value >= ETH_CFG_MTU_MIN && value <= ETH_CFG_MTU_MAX )
      ret = TRUE;

   return ret;
}
#endif
#endif


CmsRet cli_checkQosQueueResources(const PWEB_NTWK_VAR pInfo)
{
   CmsRet ret = CMSRET_SUCCESS;
   UINT32 unUsedQueues;
   
   if ( cmsUtl_strcmp(pInfo->atmServiceCategory, MDMVS_CBR)    == 0 ||
        cmsUtl_strcmp(pInfo->atmServiceCategory, MDMVS_VBR_RT) == 0 )
   {
      printf("\nQoS can only be enabled with PVC that has service category as UBR, UBRWPCR, or VBR-nrt.\n");
      return CMSRET_INVALID_ARGUMENTS;
   }
      
   cmsDal_getNumUnusedQueues(ATM, &unUsedQueues);

   if ( pInfo->serviceId == 0 )
   {
      if (unUsedQueues < 3)
      {  /* assume a new QoS pvc requires 3 queues */
         printf("\nCannot enable QoS since system is run out of queues for PVC.\n");
         ret = CMSRET_RESOURCE_EXCEEDED;
      }
   }
   else
   {
      if (unUsedQueues < 2)
      {  /* assume an Edit of existing pvc (add QoS) requires addtional 2 queues */
         printf("\nCannot enable QoS since system is run out of queues for PVC.\n");
         ret = CMSRET_RESOURCE_EXCEEDED;
      }
   }
   return ret;

}


#ifdef DMP_ADSLWAN_1

UBOOL8 cli_isVccAddrExist_igd(const WEB_NTWK_VAR *webVar)
{
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   WanDslLinkCfgObject  *dslLinkCfg=NULL;
   UBOOL8 found=FALSE;

   cmsLog_debug("looking for port %d vpi %d vci %d",
                webVar->portId, webVar->atmVpi, webVar->atmVci);

   if (dalWan_getDslLinkCfg(webVar, &iidStack, &dslLinkCfg) == TRUE)
   {
      cmsLog_debug("found dslLinkCfg");

      cmsObj_free((void **) &dslLinkCfg);
      found = TRUE;
   }

   return found;
}

#endif


void cli_wanShowInterfaces_igd(void)
{
#ifdef DMP_ADSLWAN_1

   WanDslIntfCfgObject *dslIntf=NULL;
   WanConnDeviceObject *connDevice=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack2 = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret, ret2, ret3;

   while ((ret = cmsObj_getNext(MDMOID_WAN_DSL_INTF_CFG, &iidStack, (void **) &dslIntf)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(dslIntf->linkEncapsulationUsed, MDMVS_G_992_3_ANNEX_K_ATM))
      {
         /* this is the ATM WANDevice */
         UINT32 atmCount = 0;
         SINT32 vpi=0, vci=0;
         WanDslLinkCfgObject *dslLinkCfg=NULL;

         while ((ret2 = cmsObj_getNextInSubTree(MDMOID_WAN_CONN_DEVICE, &iidStack, &iidStack2, (void **) &connDevice)) == CMSRET_SUCCESS)
         {
            if ((ret3 = cmsObj_get(MDMOID_WAN_DSL_LINK_CFG, &iidStack2, 0, (void **) &dslLinkCfg)) == CMSRET_SUCCESS)
            {

               if (atmCount == 0)
               {
                  printf("Type\tPortId\tVPI/VCI\tLink Type\tCateg.\tencap\tEnable QoS\tEnable VLAN Mux\n");
               }

               printf("ATM\t%d\t", dslLinkCfg->X_BROADCOM_COM_ATMInterfaceId);

               cmsUtl_atmVpiVciStrToNum(dslLinkCfg->destinationAddress, &vpi, &vci);
               printf("%d/%d\t", vpi, vci);

               printf("%s\t\t", dslLinkCfg->linkType);

               printf("%s\t", dslLinkCfg->ATMQoS);

               printf("%s\t", dslLinkCfg->ATMEncapsulation);

               printf("%s\t\t", dslLinkCfg->X_BROADCOM_COM_ATMEnbQos ? "Yes" : "No");
/* todo: rewrite with ConnectionMode         printf("%s\t\t", ptmCfg->X_BROADCOM_COM_VlanMuxEnabled ? "Yes" : "No");
*/
               printf("\n");
               
               cmsObj_free((void **) &dslLinkCfg);
               atmCount++;
            }
            else
            {
               cmsLog_error("could not get ATM_LINK_CFG object, ret=%d", ret3);
            }

            cmsObj_free((void **) &connDevice);
         }
      }
#ifdef DMP_PTMWAN_1
      else if (!cmsUtl_strcmp(dslIntf->linkEncapsulationUsed, MDMVS_G_992_3_ANNEX_K_PTM))
      {
         /* this is the PTM WANDevice */
         UINT32 ptmCount=0;
         WanPtmLinkCfgObject *ptmCfg=NULL;
         

         while ((ret2 = cmsObj_getNextInSubTree(MDMOID_WAN_CONN_DEVICE, &iidStack, &iidStack2, (void **) &connDevice)) == CMSRET_SUCCESS)
         {
            if ((ret3 = cmsObj_get(MDMOID_WAN_PTM_LINK_CFG, &iidStack2, 0, (void **) &ptmCfg)) == CMSRET_SUCCESS)
            {
               if (ptmCount == 0)
               {
                  printf("Type\tPortId\tPriority\tEnable QoS\tEnable VLAN Mux\n");
               }

               printf("PTM\t%d\t", ptmCfg->X_BROADCOM_COM_PTMPortId);
               if (ptmCfg->X_BROADCOM_COM_PTMPriorityLow && ptmCfg->X_BROADCOM_COM_PTMPriorityHigh)
               {
                  printf("both\t\t");
               }
               else if (ptmCfg->X_BROADCOM_COM_PTMPriorityLow)
               {
                  printf("normal\t\t");
               }
               else if (ptmCfg->X_BROADCOM_COM_PTMPriorityHigh)
               {
                  printf("high\t\t");
               }
               else
               {
                  printf("none?!\t\t");
                  cmsLog_error("no PTM priority is defined!");
               }

               printf("%s\t\t", ptmCfg->X_BROADCOM_COM_PTMEnbQos ? "Yes" : "No");
/* todo: rewrite with ConnectionMode         printf("%s\t\t", ptmCfg->X_BROADCOM_COM_VlanMuxEnabled ? "Yes" : "No");
*/
               printf("\n");

               cmsObj_free((void **) &ptmCfg);
               ptmCount++;
            }
            else
            {
               cmsLog_error("could not get PTM_LINK_CFG object, ret=%d", ret3);
            }

            cmsObj_free((void **) &connDevice);
         }

      }
#endif
      else
      {
         cmsLog_error("unsupported linkEncapsulation type %s", dslIntf->linkEncapsulationUsed);
      }


      cmsObj_free((void **) &dslIntf);
   }
#endif /* DMP_ADSLWAN_1 */

}


void cli_wanShowDslServices(InstanceIdStack *parentIidStack, const char *vccString, const char *linkType)
{
   InstanceIdStack iidStack;
   WanIpConnObject  *ipConn = NULL;
   WanPppConnObject *pppConn = NULL;

   cmsLog_debug("parentIidStack=%s", cmsMdm_dumpIidStack(parentIidStack));

   INIT_INSTANCE_ID_STACK(&iidStack);

   while (cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN, parentIidStack, &iidStack,
                                  (void **)&ipConn) == CMSRET_SUCCESS)
   {
      UBOOL8  bridgeConn = FALSE;

      if (vccString)
      {
         printf("%s\t", vccString);
      }
      else
      {
         printf("N/A\t");
      }

      // service name
      if ( strlen(ipConn->name) < 8 )
         printf("%s\t\t", ipConn->name);
      else
         printf("%s\t", ipConn->name);

      // interface name
      if ( strlen(ipConn->X_BROADCOM_COM_IfName) < 8 )
         printf("%s\t\t", ipConn->X_BROADCOM_COM_IfName);
      else
         printf("%s\t", ipConn->X_BROADCOM_COM_IfName);

      // protocol
      if (cmsUtl_strcmp(linkType, MDMVS_EOA) == 0)
      {
         if (strcmp(ipConn->connectionType, MDMVS_IP_ROUTED) == 0)
         {
            printf("IPoE\t");
         }
         else
         {
            printf("Bridged\t");
            bridgeConn = TRUE;
         }
      }
      else
      {
         printf("IPoA\t");
      }

#ifdef DMP_X_BROADCOM_COM_IGMP_1
      printf("%s\t", ipConn->X_BROADCOM_COM_IGMPEnabled? "Enable" : "Disable");
      printf("%s\t", ipConn->X_BROADCOM_COM_IGMP_SOURCEEnabled? "Enable" : "Disable");
#else
      printf("Disable\t");
      printf("Disable\t");
#endif /* DMP_X_BROADCOM_COM_IGMP_1 */
#ifdef DMP_X_BROADCOM_COM_MLD_1
      printf("%s\t", ipConn->X_BROADCOM_COM_MLDEnabled? "Enable" : "Disable");
      printf("%s\t", ipConn->X_BROADCOM_COM_MLD_SOURCEEnabled? "Enable" : "Disable");
#endif
      printf("%s\t", ipConn->connectionStatus? ipConn->connectionStatus : "\t");

      // wan IP address
      if (!bridgeConn)
      {
         printf("%s\t", ipConn->externalIPAddress? ipConn->externalIPAddress : "\t"); 
      }

#ifdef DMP_X_BROADCOM_COM_IPV6_1
      printf("%s\t", ipConn->X_BROADCOM_COM_IPv6ConnStatus? ipConn->X_BROADCOM_COM_IPv6ConnStatus : "\t");

      // wan IP address
      if (!bridgeConn)
      {
         printf("%s\t", ipConn->X_BROADCOM_COM_ExternalIPv6Address? ipConn->X_BROADCOM_COM_ExternalIPv6Address : "\t"); 
      }    
#endif

      printf("\n");

      cmsObj_free((void **)&ipConn);
   }
      
   INIT_INSTANCE_ID_STACK(&iidStack);

   while (cmsObj_getNextInSubTree(MDMOID_WAN_PPP_CONN, parentIidStack, &iidStack,
                                  (void **)&pppConn) == CMSRET_SUCCESS)
   {

      if (vccString)
      {
         printf("%s\t", vccString);
      }
      else
      {
         printf("N/A\t");
      }

      // service name
      if ( strlen(pppConn->name) < 8 )
         printf("%s\t\t", pppConn->name);
      else
         printf("%s\t", pppConn->name);
      // interface name
      if ( strlen(pppConn->X_BROADCOM_COM_IfName) < 8 )
         printf("%s\t\t", pppConn->X_BROADCOM_COM_IfName);
      else
         printf("%s\t", pppConn->X_BROADCOM_COM_IfName);

      if (cmsUtl_strcmp(linkType, MDMVS_EOA) == 0)
      {
         printf("PPPoE\t");
      }
      else
      {
         printf("PPPoA\t");
      }

#ifdef DMP_X_BROADCOM_COM_IGMP_1
      printf("%s\t", pppConn->X_BROADCOM_COM_IGMPEnabled? "Enable" : "Disable");
      printf("%s\t", pppConn->X_BROADCOM_COM_IGMP_SOURCEEnabled? "Enable" : "Disable");
#else
      printf("Disable\t");
      printf("Disable\t");
#endif /* DMP_X_BROADCOM_COM_IGMP_1 */
#ifdef DMP_X_BROADCOM_COM_MLD_1
      printf("%s\t", pppConn->X_BROADCOM_COM_MLDEnabled? "Enable" : "Disable");
      printf("%s\t", pppConn->X_BROADCOM_COM_MLD_SOURCEEnabled? "Enable" : "Disable");
#endif
      printf("%s\t", pppConn->connectionStatus? pppConn->connectionStatus : "\t");

      // wan IP address
      printf("%s\t", pppConn->externalIPAddress? pppConn->externalIPAddress : "\t"); 

#ifdef DMP_X_BROADCOM_COM_IPV6_1
      printf("%s\t", pppConn->X_BROADCOM_COM_IPv6ConnStatus? pppConn->X_BROADCOM_COM_IPv6ConnStatus : "\t");
      // wan IP address
      printf("%s\t", pppConn->X_BROADCOM_COM_ExternalIPv6Address? pppConn->X_BROADCOM_COM_ExternalIPv6Address : "\t"); 
#endif

      printf("\n");

      cmsObj_free((void **)&pppConn);
   }  /* while */

}


/***************************************************************************
// Function Name: cli_wanShow
// Description  : show specific or all wan connections.
// Returns      : status.
****************************************************************************/
CmsRet cli_wanShowServices(const char *specificVcc)
{
#define CLI_WAN_SHOW_HEADER_1_1 "VCC\tService\t\tInterface\tProto.\tIGMP\tSrc?\t"
#define CLI_WAN_SHOW_HEADER_1_2 "MLD\tSrc?\t"
#define CLI_WAN_SHOW_HEADER_1_3 "IPv4Status\tIPv4\t\t"
#define CLI_WAN_SHOW_HEADER_1_4 "IPv6Status\tIPv6\t\t"

#define CLI_WAN_SHOW_HEADER_2_1 "\tName\t\tName\t\t\t\t\t\t\t\t"
#define CLI_WAN_SHOW_HEADER_2_2 "\t"
#define CLI_WAN_SHOW_HEADER_2_3 "address\t"
#define CLI_WAN_SHOW_HEADER_2_4 "\t\t\taddress\t"


   // print header
   printf("%s", CLI_WAN_SHOW_HEADER_1_1);
#ifdef DMP_X_BROADCOM_COM_MLD_1
   printf("%s", CLI_WAN_SHOW_HEADER_1_2);
#endif
   printf("%s", CLI_WAN_SHOW_HEADER_1_3);
#ifdef DMP_X_BROADCOM_COM_IPV6_1
   printf("%s", CLI_WAN_SHOW_HEADER_1_4);
#endif
   printf("\n");

   printf("%s", CLI_WAN_SHOW_HEADER_2_1);
#ifdef DMP_X_BROADCOM_COM_MLD_1
   printf("%s", CLI_WAN_SHOW_HEADER_2_2);
#endif
   printf("%s", CLI_WAN_SHOW_HEADER_2_3);
#ifdef DMP_X_BROADCOM_COM_IPV6_1
   printf("%s", CLI_WAN_SHOW_HEADER_2_4);
#endif
   printf("\n");

   return (cli_wanShowServicesSub(specificVcc));

}   



/***************************************************************************
// Function Name: cli_wanShowServicesSub_igd
// Description  : show specific or all wan connections.
// Returns      : status.
****************************************************************************/
CmsRet cli_wanShowServicesSub_igd(const char *specificVcc)
{
   InstanceIdStack commonIidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanCommonIntfCfgObject *commonIntfCfg = NULL;
   char vcc[BUFLEN_32];
   SINT32 vpi, vci;

   while (cmsObj_getNext(MDMOID_WAN_COMMON_INTF_CFG, &commonIidStack, (void **)&commonIntfCfg) == CMSRET_SUCCESS)
   {
      /*
       * Outer-most loop, looks at WANCommonInterfaceConfig to
       * determine if this is a DSL (ATM/PTM) WAN connection, or ethernet connection.
       */

      if (!cmsUtl_strcmp(commonIntfCfg->WANAccessType, MDMVS_DSL))
      {
         /* DSL, either ATM or PTM */
         WanDslIntfCfgObject *dslIntfCfg = NULL;

         cmsObj_get(MDMOID_WAN_DSL_INTF_CFG, &commonIidStack, 0, (void **) &dslIntfCfg);
         cmsLog_debug("link enacap used=%s modulation type=%s",
                      dslIntfCfg->linkEncapsulationUsed,
                      dslIntfCfg->modulationType);

         if (!cmsUtl_strcmp(dslIntfCfg->linkEncapsulationUsed, MDMVS_G_992_3_ANNEX_K_ATM) || /* adsl+atm */
             !cmsUtl_strcmp(dslIntfCfg->linkEncapsulationUsed, MDMVS_G_993_2_ANNEX_K_ATM))    /* vdsl+atm (unlikely) */
         {
            WanDslLinkCfgObject *dslLinkCfg = NULL;

            INIT_INSTANCE_ID_STACK(&iidStack);

            while (cmsObj_getNextInSubTree(MDMOID_WAN_DSL_LINK_CFG, &commonIidStack, &iidStack, (void **)&dslLinkCfg) == CMSRET_SUCCESS)
            {
               // format VCC
               if (cmsUtl_atmVpiVciStrToNum(dslLinkCfg->destinationAddress, &vpi, &vci) != CMSRET_SUCCESS)
               {
                  cmsLog_error("could not convert destinationAddress %s", dslLinkCfg->destinationAddress);
                  strcpy(vcc, "\t");
               }
               else
               {
                  sprintf(vcc, "0.%d.%d", vpi, vci);
               }
      
               if (specificVcc != NULL)
               {
                  /* user requested a specific VCC */
                  if (strcmp(vcc, specificVcc) == 0)
                  {
                     cli_wanShowDslServices(&iidStack, vcc, dslLinkCfg->linkType);
                  }
               }
               else
               {
                  /* user has not requested specific VCC, show all */
                  cli_wanShowDslServices(&iidStack, vcc, dslLinkCfg->linkType);
               }
      
               cmsObj_free((void **)&dslLinkCfg);
            }
         }
#ifdef DMP_PTMWAN_1
         else if (!cmsUtl_strcmp(dslIntfCfg->linkEncapsulationUsed, MDMVS_G_992_3_ANNEX_K_PTM) || /* vdsl+ptm (unlikely)*/
                  !cmsUtl_strcmp(dslIntfCfg->linkEncapsulationUsed, MDMVS_G_993_2_ANNEX_K_PTM))    /* vdsl+ptm */
         {
            WanPtmLinkCfgObject *ptmLinkCfg = NULL;

            INIT_INSTANCE_ID_STACK(&iidStack);

            while (cmsObj_getNextInSubTree(MDMOID_WAN_PTM_LINK_CFG, &commonIidStack, &iidStack, (void **)&ptmLinkCfg) == CMSRET_SUCCESS)
            {
               cmsLog_debug("show all PTM services under %s", cmsMdm_dumpIidStack(&iidStack));
               cli_wanShowDslServices(&iidStack, NULL, MDMVS_EOA);
               cmsObj_free((void **)&ptmLinkCfg);
            }
         }
#endif
         else
         {
            cmsLog_error("unrecognized or unsupported linkEncapsulation %s", dslIntfCfg->linkEncapsulationUsed);
         }

         cmsObj_free((void **) &dslIntfCfg);
      }
#ifdef DMP_ETHERNETWAN_1  /* aka SUPPORT_ETHWAN */
      else if (!cmsUtl_strcmp(commonIntfCfg->WANAccessType, MDMVS_ETHERNET))
      {
         WanEthLinkCfgObject *ethLinkCfg = NULL;

         INIT_INSTANCE_ID_STACK(&iidStack);

         while (cmsObj_getNextInSubTree(MDMOID_WAN_ETH_LINK_CFG, &commonIidStack, &iidStack, (void **)&ethLinkCfg) == CMSRET_SUCCESS)
         {
            cmsLog_debug("show all ETHWAN services under %s", cmsMdm_dumpIidStack(&iidStack));
            cli_wanShowDslServices(&iidStack, NULL, MDMVS_EOA);
            cmsObj_free((void **)&ethLinkCfg);
         }
      }
#endif
#ifdef DMP_X_BROADCOM_COM_MOCAWAN_1
      else if (!cmsUtl_strcmp(commonIntfCfg->WANAccessType, MDMVS_X_BROADCOM_COM_MOCA))
      {
         iidStack = commonIidStack;

         cmsLog_debug("show all MocaWAN services under %s", cmsMdm_dumpIidStack(&iidStack));
         cli_wanShowDslServices(&iidStack, NULL, MDMVS_EOA);
      }
#endif
#ifdef DMP_X_BROADCOM_COM_CELLULARWAN_1
      else if (!cmsUtl_strcmp(commonIntfCfg->WANAccessType, MDMVS_X_BROADCOM_COM_CELLULAR))
      {
         iidStack = commonIidStack;

         cmsLog_debug("show all Cellular services under %s", cmsMdm_dumpIidStack(&iidStack));
         cli_wanShowDslServices(&iidStack, NULL, MDMVS_EOA);
      }
#endif
#ifdef DMP_X_BROADCOM_COM_L2TPAC_1
      else if (!cmsUtl_strcmp(commonIntfCfg->WANAccessType, MDMVS_X_BROADCOM_COM_L2TPAC))
      {
         /* cwu_todo */
      }
#endif /* DMP_X_BROADCOM_COM_L2TPAC_1 */

#ifdef DMP_X_BROADCOM_COM_PONWAN_1

      else if (!cmsUtl_strcmp(commonIntfCfg->WANAccessType, MDMVS_X_BROADCOM_COM_PON))
      {
         /* PON interfaces, (gpon, etc) */
         WanPonIntfObject *ponIntfCfg = NULL;

         cmsObj_get(MDMOID_WAN_PON_INTF, &commonIidStack, 0, (void **) &ponIntfCfg);

#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
         if (!cmsUtl_strcmp(ponIntfCfg->ponType, MDMVS_GPON))
         {
            WanGponLinkCfgObject *gponLinkCfg = NULL;

            INIT_INSTANCE_ID_STACK(&iidStack);

            while (cmsObj_getNextInSubTree(MDMOID_WAN_GPON_LINK_CFG, &commonIidStack, &iidStack, (void **)&gponLinkCfg) == CMSRET_SUCCESS)
            {
               cli_wanShowDslServices(&iidStack, NULL, MDMVS_EOA);
               cmsObj_free((void **)&gponLinkCfg);
            }
         }
#endif

#ifdef DMP_X_BROADCOM_COM_EPONWAN_1
         if (!cmsUtl_strcmp(ponIntfCfg->ponType, MDMVS_EPON))
         {
            WanEponLinkCfgObject *eponLinkCfg = NULL;

            INIT_INSTANCE_ID_STACK(&iidStack);

            while (cmsObj_getNextInSubTree(MDMOID_WAN_EPON_LINK_CFG, &commonIidStack, &iidStack, (void **)&eponLinkCfg) == CMSRET_SUCCESS)
            {
               cli_wanShowDslServices(&iidStack, NULL, MDMVS_EOA);
               cmsObj_free((void **)&eponLinkCfg);
            }
         }
#endif
      }
#endif /* DMP_X_BROADCOM_COM_PONWAN_1 */

      else
      {
         cmsLog_error("unknown or unsupported WAN access type %s", commonIntfCfg->WANAccessType);
      }

      cmsObj_free((void **)&commonIntfCfg);
   }

   return CMSRET_SUCCESS;

}  /* End of cli_wanShow() */



/***************************************************************************
// Function Name: cli_wanIfaceInfo
// Description  : get wan interface information, it is modified from cgiGetWanInterfaceInfo
//Parameters   :
//    type : the type of wan interface
//    iface : the list of interface info
// Returns      :  none
****************************************************************************/
void cli_wanIfaceInfo(char *type, char *iface) 
{
   InstanceIdStack iidStack;
   InstanceIdStack parentIidStack = EMPTY_INSTANCE_ID_STACK;
   WanDslLinkCfgObject *dslLinkCfg = NULL;   
   WanPppConnObject *pppConn = NULL;   
   WanIpConnObject  *ipConn = NULL;
   char tempStr[BUFLEN_32]={0};
   char ifcListStr[BUFLEN_128]={0};
   UBOOL8 usedForDns = FALSE;
   UBOOL8 needEoAPvc = FALSE;
   UBOOL8 firewallEnabledInfo = FALSE;
   UBOOL8 bridgeIfcInfo = FALSE;
   
   iface[0] = ifcListStr[0]; 
   
   /* for interface grouping, all pvc including bridge are needed */
   if (cmsUtl_strcmp(iface, "allEoAPvc") == 0)
   {
      needEoAPvc = TRUE;
   }
   
   /* for checking interfaces with firewall enabled */
   if (cmsUtl_strcmp(iface, "firewall") == 0)
   {
      firewallEnabledInfo = TRUE;
   }
   /* for checking bridged PVCs */
   else if (cmsUtl_strcmp(iface, "bridge") == 0)
   {
      bridgeIfcInfo = TRUE;
   }
   
   /* for dns info only */
   if (cmsUtl_strcmp(iface, "forDns") == 0 || cmsUtl_strcmp(iface, "forDnsPlusOne") == 0)
   {
      usedForDns = TRUE;
   }

   
   while (cmsObj_getNext(MDMOID_WAN_DSL_LINK_CFG, &parentIidStack,
                     (void **)&dslLinkCfg) == CMSRET_SUCCESS)
   {
      
      INIT_INSTANCE_ID_STACK(&iidStack);
      /* get the related ipConn obj */
      while (cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN, &parentIidStack, &iidStack,
                                       (void **)&ipConn) == CMSRET_SUCCESS)
      {
         /* for checking interfaces with firewall enabled */
         if ( firewallEnabledInfo == TRUE )
         {
            if ( ipConn->X_BROADCOM_COM_FirewallEnabled == TRUE )
            {
               snprintf(tempStr, sizeof(tempStr), "%s|",  ipConn->X_BROADCOM_COM_IfName);
               strncat(ifcListStr, tempStr, sizeof(ifcListStr));
            }
            cmsObj_free((void **) &ipConn);
            continue;
         }
         /* for checking bridged PVCs */
         else if ( bridgeIfcInfo == TRUE )
         {
            if ( cmsUtl_strcmp(ipConn->connectionType, MDMVS_IP_BRIDGED) == 0 )
            {
               snprintf(tempStr, sizeof(tempStr), "%s|",  ipConn->X_BROADCOM_COM_IfName);
               strncat(ifcListStr, tempStr, sizeof(ifcListStr));
            }
            cmsObj_free((void **) &ipConn);
            continue;
         }
	  
         /* if not all EoA pvc needed, do the following
          * skip 1) bridge protocol  
          * OR 2) used for dns and protocol is protocol is IPoA
          * OR 3) used for dns and static IPoE 
          */
         if ((needEoAPvc == FALSE) &&
              ((cmsUtl_strcmp(ipConn->connectionType, MDMVS_IP_BRIDGED) == 0) ||
              (usedForDns && cmsUtl_strcmp(dslLinkCfg->linkType, MDMVS_IPOA) == 0) ||
              (usedForDns && cmsDal_getWANProtocolFromIpConn(dslLinkCfg, ipConn) == CMS_WAN_TYPE_STATIC_IPOE 
               && cmsUtl_strcmp(ipConn->addressingType, MDMVS_STATIC) ==0)))
         {
            cmsObj_free((void **) &ipConn);
            continue;
         }
         else
         {
            /*  skip on
              * 1). if needEoA is TRUE but it is IpoA 
              * 2). if needEoA is TRUE and the wan interface is already in the interface group (not br0)
              */
            if (((needEoAPvc == TRUE) && (cmsUtl_strcmp(dslLinkCfg->linkType, MDMVS_IPOA) == 0)) ||
               ((needEoAPvc == TRUE) && (dalPMap_isWanUsedForIntfGroup(ipConn->X_BROADCOM_COM_IfName) == TRUE)))
            {
               cmsObj_free((void **) &ipConn);
               continue;
            }
            snprintf(tempStr, sizeof(tempStr), "%s|", ipConn->X_BROADCOM_COM_IfName);
            strncat(ifcListStr, tempStr, sizeof(ifcListStr));
         }
         cmsObj_free((void **) &ipConn);
      }

      if (bridgeIfcInfo == FALSE)
      {
         /* For non-bridged PVCs */
         INIT_INSTANCE_ID_STACK(&iidStack);
         while (cmsObj_getNextInSubTree(MDMOID_WAN_PPP_CONN, &parentIidStack, &iidStack,
                                          (void **)&pppConn) == CMSRET_SUCCESS)
         {
            /* for checking interfaces with firewall enabled */
            if ( firewallEnabledInfo == TRUE )
            {
               if ( pppConn->X_BROADCOM_COM_FirewallEnabled == TRUE )
               {
                  snprintf(tempStr, sizeof(tempStr), "%s|", pppConn->X_BROADCOM_COM_IfName);
                  strncat(ifcListStr, tempStr, sizeof(ifcListStr));
               }
               cmsObj_free((void **) &pppConn);
               continue;
            }
   	   
               /*  skip on
                 * 1). if needEoA is TRUE but it is PPPoA (not EoA)
                 * 2). if needEoA is TRUE and the wan interface is already in the interface group (not br0)
                 */
            if (((needEoAPvc == TRUE) && (cmsUtl_strcmp(dslLinkCfg->linkType, MDMVS_PPPOA) == 0))  ||
               ((needEoAPvc == TRUE) && (dalPMap_isWanUsedForIntfGroup(pppConn->X_BROADCOM_COM_IfName) == TRUE)))
            {
               cmsObj_free((void **) &pppConn);
               continue;
            }
            snprintf(tempStr, sizeof(tempStr), "%s|", pppConn->X_BROADCOM_COM_IfName);
            strncat(ifcListStr, tempStr, sizeof(ifcListStr));
            cmsObj_free((void **) &pppConn);
         }
      }
      
     /* free dslLinkCfg */
      cmsObj_free((void **) &dslLinkCfg);

   }

   /* for checking interfaces with firewall enabled or bridged PVCs */
   if (firewallEnabledInfo == TRUE || bridgeIfcInfo == TRUE)
   {
      if (ifcListStr[0] != '\0')
      {
         strcat(iface, ifcListStr);
      }  

      int len = strlen(iface);
      if (len > 0)
      {
         iface[len-1] = '\0';   /* remove the last '|' character   */
      }
      
      return;
   }

   /* append the No Interface for multiSubnet */
   if (needEoAPvc == TRUE)
   {
      strncat(ifcListStr, "None|", sizeof(ifcListStr));      
   }
   
   if (ifcListStr[0] != '\0')
   {
      strcat(iface, ifcListStr);
   }  

      
   int len = strlen(iface);
   if (len > 0)
   {
      iface[len-1] = '\0';   /* remove the last '|' character   */
   }
   
   cmsLog_debug("pvc list: %s", iface);
   
}

