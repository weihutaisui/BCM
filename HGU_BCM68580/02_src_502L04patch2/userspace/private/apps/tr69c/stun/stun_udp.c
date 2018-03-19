/***********************************************************************
 * <:copyright-BRCM:2007-2013:proprietary:standard
 * 
 *    Copyright (c) 2007-2013 Broadcom 
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
 * :> *
 * $Change: 109793 $
 ***********************************************************************/

#ifdef SUPPORT_STUN

/** Includes **/

#include <time.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include "cms_log.h"
#include "cms_tms.h"
#include "cms_tmr.h"
#include "cms_util.h"
#include "cms_core.h"


#include "stun_packet.h"
#include "stun_cms.h"

#include "../inc/tr69cdefs.h"
#include "../inc/utils.h"
#include "../main/informer_public.h"
#include "../main/event.h"
#include "../bcmLibIF/bcmConfig.h"

#include "openssl/hmac.h"
#include "openssl/sha.h"


/** local constants **/

#define STUN_MAX_MESSAGE_SIZE       2048
#define QVALUE_SIZE                 64

const unsigned char STUN_AT_CONNREQBIN_VALUE[] = {0x64, 0x73, 0x6C, 0x66, 0x6F, 0x72, 0x75, 0x6D, 0x2E, 0x6F, 0x72, 0x67, 0x2F, 0x54, 0x52, 0x2D, 0x31, 0x31, 0x31, 0x20};

/** local types **/

typedef struct ts_nat_binding
{
  UINT16 port;
  UINT32 address;
  UINT8 binding_changed; 
  UINT8 nat_detected_changed;
  UINT16 timeout;
} ts_nat_binding;

typedef struct ts_UDPConnReqMsg
{
  char *end_data;
  char *to;
  char *from;
  int  test_len;
  int  count;
  char URI[128];
  char URI_host[QVALUE_SIZE];
  char URI_port[32];
  char ts[QVALUE_SIZE];
  char id[QVALUE_SIZE];
  char un[258];
  char cn[QVALUE_SIZE];
  char sig[QVALUE_SIZE];
  char sha_key[256];
  char sha_text[512];
  char sha_md[64];
  char cmp[64];
  int  ts_f;
  int  id_f;
  int  un_f;
  int  cn_f;
  int  sig_f;
  int  sha_key_len;
  int  sha_text_len;
  int  sha_md_len;
  unsigned long previous_ts;
  unsigned long this_ts;
  char *connection_username; 
  char *connection_password; 
} ts_UDPConnReqMsg;

/** local data **/

static STUNData stunData;
static char new_conn[48];
static int udp_sock;
static stun_pkt* stun_packet=NULL;
static UINT8 stun_retries=0;
static UINT8 stun_unauth_received=0;
static ts_nat_binding nat_binding;

static char previous_ts_str[32] = "";
static char previous_id_str[32] = "";

static ts_UDPConnReqMsg UDP_CR;
static CmsTimestamp tStampLastUDPConnectionRequestAddressNotification;
static UBOOL8 delayedUDPConnectionRequestAddressNotification = FALSE;

/** external data **/

extern ACSState acsState;
extern eInformState informState;
extern void *tmrHandle;

/* forward delcaration */

static UBOOL8 stun_init();
static int  validate_UDPConnectionRequest(char *data, int  len);
static void updateUDPConnReqURL(UINT32 mapped_ip, UINT32 mapped_port);
static int udp_listen(unsigned int ip, int port);
static void stun_timewait_timeout(void *handle);
static void stun_rx_timeout(void *handle);
static int stun_prepareBindingReqPkt(void);
static void stun_sendBindingReq(void *handle);
static void acsUDPConnect(void *handle);
static void stun_send_pkt(UINT8 *data, UINT16 len);
static void stun_bindingchange_timeout(void *handle);
static void handleDelayedUDPConnReqNotif(void *handle);
static UBOOL8 isNeededActiveNotification();
extern void handleConnectionRequest(void);
static void stun_resetUDPConnReqAddress();
static char* stun_writeIp(int ip);
static int stun_readIpfromURL(const char* url);
static void stun_SetUDPConnReqAddress(const char* ip);

/*
 * Format Binary IP addr to String
 */
static char* stun_writeIp(int ip)
{
  static char buf[256];
  writeIp_b(ip, buf);
  return buf;
}

/*
 * set "UDPConnectionRequestAddress"
 */
static void stun_SetUDPConnReqAddress(const char* endpoint)
{
   CMSMEM_REPLACE_STRING(stunData.UDPConnReqAddress, endpoint);
   stunCms_updateParameters(&stunData);
}


/*
 * use CPE LAN IP address to set "UDPConnectionRequestAddress"
 */
static void stun_resetUDPConnReqAddress()
{
   int ipAddr;
   if (stunCms_getCPELanIPAddr(&ipAddr) != CMSRET_SUCCESS)
   {
      return;
   }

   sprintf(new_conn, "%s:%d", stun_writeIp(ipAddr), stunData.ClientPort);  
     
   stun_SetUDPConnReqAddress(new_conn);
}

/*
 * A notification has arrived: STUN Config params has been changed
 */
void stun_configChanged()
{
  if (stunCms_configChanged(&stunData) != CMSRET_SUCCESS)
  {
     cmsLog_error("stunCms_configChanged failed");   
  }
}

/*
 * Initialize module
 */
static UBOOL8 stun_init()
{
  memset(&stunData, 0, sizeof(stunData));
  if (stunCms_getSTUNConfig(&stunData) != CMSRET_SUCCESS)
  {
     cmsLog_error("stunCms_getParameters failed");
     return FALSE;
  }

  if (stunData.UDPConnReqAddress == NULL)
  {
     stun_resetUDPConnReqAddress();
     stunData.UDPConnReqAddressIsChanged = 1;
  }  
  else
  {
     /* code below to handle a bug in CMS */
     if (strcmp(stunData.UDPConnReqAddress, "(null)") == 0)
     {
        stun_resetUDPConnReqAddress();
        stunData.UDPConnReqAddressIsChanged = 1;
     }
  }

  memset(&nat_binding, 0, sizeof(ts_nat_binding));

  return TRUE;
}

/* Examines the packet data contents for presence of the information 
 * required by TR-111.
 * Specifically:
 *    HTTP GET Request for version HTTP 1.1
 *    Valid URI
 *    Empty PATH
 *    Query String with fields: ts, id, un, cn, and sig
 */
static int validate_UDPConnectionRequest(char *data, int  len)
{
  int i;
  
  memset(&UDP_CR, 0, sizeof(ts_UDPConnReqMsg));
  
  UDP_CR.connection_username = acsState.connReqUser; 
  UDP_CR.connection_password = acsState.connReqPwd; 
            
  UDP_CR.end_data = data + len - 1;
  UDP_CR.test_len = data - UDP_CR.end_data;

  /* Check for "GET " at start */
  if ((strncmp(data, "GET ", 4) != 0))
  {
    return(0);
  }  
  data += 4;

  /* Check for "http://" next  */
  if (strncasecmp( data, "http://", 7 ) != 0)
  {
    return (2);
  }

  /* Copy URI up to '?' char, or end of URI or data buffer */
  UDP_CR.to = UDP_CR.URI;
  UDP_CR.count = 1;
  while ( (*data != '?' ) && (UDP_CR.count<sizeof(UDP_CR.URI)) && (data < UDP_CR.end_data))
  {
    *UDP_CR.to = *data;
    UDP_CR.to++;
    data++;
    UDP_CR.count++;
    if ( UDP_CR.count >= sizeof(UDP_CR.URI) )
    {
      return(3);
    }
    if ( data >= UDP_CR.end_data )
    {
      return(4);
    }
  }
  *UDP_CR.to = '\0';

  /* Extract host portion from URI */
  (void) strcpy(UDP_CR.URI_host,"");
  UDP_CR.to       = UDP_CR.URI_host;
  UDP_CR.from     = UDP_CR.URI + 7;
  UDP_CR.count    = 1;
  while ( (*UDP_CR.from != ':') && (*UDP_CR.from != '/') && (*UDP_CR.from != '\0'))
  {
    *UDP_CR.to = *UDP_CR.from;
    UDP_CR.to++;
    UDP_CR.from++;
    UDP_CR.count++;
    if ( UDP_CR.count >= sizeof(UDP_CR.URI_host) )
    {
      return(5);
    }
  }
  *UDP_CR.to = '\0';

  /* Extract port portion if present */
  (void) strcpy(UDP_CR.URI_port,"");
  if ( *UDP_CR.from == ':' )
  {
    UDP_CR.to     = UDP_CR.URI_port;
    UDP_CR.from  += 1;
    UDP_CR.count  = 1;
    while ( (*UDP_CR.from != '/') && (*UDP_CR.from != '\0'))
    {
      *UDP_CR.to = *UDP_CR.from;
      UDP_CR.to++;
      UDP_CR.from++;
      UDP_CR.count++;
      if ( UDP_CR.count >= sizeof(UDP_CR.URI_port) )
      {
        return(6);
      }
    }
    *UDP_CR.to = '\0';
  }

  /* If URI contains a path component, reject as per TR111 */
  if (*UDP_CR.from == '/')
  {
    return(7);
  }

  /* Now need to parse out query fields */
  UDP_CR.ts[0] = UDP_CR.id[0] = UDP_CR.un[0] = UDP_CR.cn[0] = UDP_CR.sig[0] = '\0';
  UDP_CR.ts_f  = UDP_CR.id_f  = UDP_CR.un_f  = UDP_CR.cn_f  = UDP_CR.sig_f  = 1;

  while ( data < UDP_CR.end_data )
  {
    int limit=0;

    data++;
    if ( data >= UDP_CR.end_data )
    {
      return(8);
    }

    /* recognize next query name<>value pair?
       If yes, set ptrs
       Recognize with any order of pairs */
    if ( strncmp(data,"ts=",3)  == 0 )
    {
      UDP_CR.to    = UDP_CR.ts;
      UDP_CR.ts_f  = 0;
      data += 3;
      limit = sizeof(UDP_CR.ts);
    }
    if ( strncmp(data,"id=",3)  == 0 )
    {
      UDP_CR.to    = UDP_CR.id;
      UDP_CR.id_f  = 0;
      data += 3;
      limit = sizeof(UDP_CR.id);
    }
    if ( strncmp(data,"un=",3)  == 0 )
    {
      UDP_CR.to    = UDP_CR.un;
      UDP_CR.un_f  = 0;
      data += 3;
      limit = sizeof(UDP_CR.un);
    }
    if ( strncmp(data,"cn=",3)  == 0 )
    {
      UDP_CR.to    = UDP_CR.cn;
      UDP_CR.cn_f  = 0;
      data += 3;
      limit = sizeof(UDP_CR.cn);
    }
    if ( strncmp(data,"sig=",4) == 0 )
    {
      UDP_CR.to    = UDP_CR.sig;
      UDP_CR.sig_f = 0;
      data += 4;
      limit = sizeof(UDP_CR.sig);
    }

    /* Copy value */
    UDP_CR.count = 1;
    while ((*data != '&' ) && (*data != ' ') && ( UDP_CR.count<limit ) && ( data < UDP_CR.end_data ))
    {
      *UDP_CR.to = *data;
      UDP_CR.to++;
      data++;
      UDP_CR.count++;
      if ( UDP_CR.count >= limit )
      {
        return(21);
      }

      if ( data >= UDP_CR.end_data )
      {
        return(9);
      }
    }
    *UDP_CR.to = '\0';

    /* got all values, even if null string? */
    if ( (UDP_CR.ts_f == 0) && (UDP_CR.id_f == 0) && (UDP_CR.un_f == 0) && (UDP_CR.cn_f == 0) && (UDP_CR.sig_f == 0) )
      break;
  } /* while */ 

  if ( (UDP_CR.ts_f != 0)  || (strlen(UDP_CR.ts) == 0 ))  /* missing or null ts name<>value */
  {
    return(10);
  }

  if ( (UDP_CR.id_f != 0)  || (strlen(UDP_CR.id) == 0 ))  /* missing or null id name<>value */
  {
    return(11);
  }

  if ( (UDP_CR.un_f != 0)  || (strlen(UDP_CR.un) == 0 ))  /* missing or null un name<>value */
  {
    return(12);
  }

  if ( (UDP_CR.cn_f != 0)  || (strlen(UDP_CR.cn) == 0 ))  /* missing or null cn name<>value */
  {
    return(13);
  }

  if ( (UDP_CR.sig_f != 0) || (strlen(UDP_CR.sig) == 0 )) /* missing or null sig name<>value */
  {
    return(14);
  }

  /* Should have all required query name<>value pairs now */

  /* Skip space terminating query string */
  if ( *data == ' ' )
    data++;

  /* Test if any chars left in recv buffer */
  if ( data >= UDP_CR.end_data )
  {
    return(15);
  }
  /* Check for HTTP Version */
  if (strncmp(data,"HTTP/1.1",8) != 0)
  {
    return(16);
  }

  /* Check that un value  matches connection_username */
  if ( strcmp( UDP_CR.un, UDP_CR.connection_username) != 0 )
  {
    return(17);
  }

  (void)  strcpy( UDP_CR.sha_key, UDP_CR.connection_password);
  UDP_CR.sha_key_len = strlen(UDP_CR.sha_key);

  memset(UDP_CR.sha_text, 0, sizeof(UDP_CR.sha_text));
  sprintf(UDP_CR.sha_text, "%s%s%s%s",UDP_CR.ts,UDP_CR.id,UDP_CR.un,UDP_CR.cn);
  UDP_CR.sha_text_len = strlen(UDP_CR.sha_text);
  UDP_CR.sha_md_len   = 0;

  HMAC_CTX ctx;
  HMAC_CTX_init(&ctx);
  // Using sha1 hash engine here
  HMAC_Init_ex(&ctx, UDP_CR.sha_key, UDP_CR.sha_key_len, EVP_sha1(), NULL);
  HMAC_Update(&ctx, (unsigned char*)&UDP_CR.sha_text, UDP_CR.sha_text_len);
  HMAC_Final(&ctx, (unsigned char*)UDP_CR.sha_md, (unsigned int*)&UDP_CR.sha_md_len);
  HMAC_CTX_cleanup(&ctx);

  UDP_CR.to = UDP_CR.cmp;
  for (i = 0; i < UDP_CR.sha_md_len; i++) 
  {
    sprintf(UDP_CR.to,"%02x", UDP_CR.sha_md[i] & 0xff);
    UDP_CR.to += 2;
  }

  i = strncasecmp( UDP_CR.sig, UDP_CR.cmp, strlen(UDP_CR.sig));
  if ( i !=  0 ) 
  {
    return(18);
  }

  // **********************************************
  // UDP Connection Request Message Is AUTHENTIC
  // **********************************************

  /* Check that ts value is larger than previous_ts
     If previous string is null, this is first time */
  if (strlen(previous_ts_str) != 0 ) 
  {
    UDP_CR.previous_ts = atol(previous_ts_str);
    UDP_CR.this_ts     = atol(UDP_CR.ts);
    if ( UDP_CR.this_ts < UDP_CR.previous_ts ) 
    {
      return(19);
    }
  }
  (void) strcpy( previous_ts_str, UDP_CR.ts );

  /* Check that ID value different from previous
     If previous string is null, this is first time */
  if ( strlen(previous_id_str) != 0 ) 
  {
    if ( strcmp( UDP_CR.id, previous_id_str) == 0 ) 
    {
      return(20);
    }
  }
  (void) strcpy( previous_id_str, UDP_CR.id );

  return(1);
}

static void handleDelayedUDPConnReqNotif(void *handle)
{
  cmsLog_debug("Delayed UDPConnectionRequestAddress notif send NOW");

  cmsTms_get(&tStampLastUDPConnectionRequestAddressNotification);

  cmsTmr_cancel(tmrHandle, handleDelayedUDPConnReqNotif, NULL);

  delayedUDPConnectionRequestAddressNotification = FALSE;

  addInformEventToList(INFORM_EVENT_VALUE_CHANGE);
  sendInform(NULL);
}

static UBOOL8 isPermitUDPConnectionRequestAddressNotification()
{
  CmsTimestamp deltaTms;
  CmsTimestamp nowTms;

  cmsTms_get(&nowTms);
  cmsTms_delta(&nowTms, &tStampLastUDPConnectionRequestAddressNotification, &deltaTms);
  UINT32 elapsed = deltaTms.sec;

  cmsLog_debug("elapsed secs= %d", elapsed);
  cmsLog_debug("AddressNotificationLimit secs= %d",stunData.AddressNotificationLimit);

  UBOOL8 retval = elapsed > (stunData.AddressNotificationLimit*1000);
  if (retval==FALSE && delayedUDPConnectionRequestAddressNotification==FALSE)
  {
    delayedUDPConnectionRequestAddressNotification = TRUE;
    /* enable timer to notify "UDPConnectionRequestAddress" change due to
       UDPConnectionRequestAddressNotificationLimit parameter*/
    UINT32 ms = (stunData.AddressNotificationLimit - elapsed) * 1000;
    if (cmsTmr_set(tmrHandle, handleDelayedUDPConnReqNotif, NULL, ms, NULL) != CMSRET_SUCCESS)
    {
      cmsLog_error("error set timer");
      retval = FALSE;
    }
  }

  return (retval);
}

static UBOOL8 isNeededActiveNotification() 
{
  if (nat_binding.binding_changed)      
  {
    UBOOL8 enabled;
    CmsRet ret = stunCms_isActiveNotificationEnabled("UDPConnectionRequestAddress", &enabled);
    if (ret == CMSRET_SUCCESS && enabled)
    {    
      if (isPermitUDPConnectionRequestAddressNotification())
      {
        cmsLog_debug("UDPConnectionRequestAddress notif");      
        cmsTms_get(&tStampLastUDPConnectionRequestAddressNotification);
        return TRUE;
      }
      else
      {  
        cmsLog_debug("UDPConnectionRequestAddress notif DELAYED");            
        return FALSE;
      }
    }
  }
 
  if (nat_binding.nat_detected_changed)
  {
    UBOOL8 enabled;
    CmsRet ret = stunCms_isActiveNotificationEnabled("NATDetected", &enabled);
    if (ret == CMSRET_SUCCESS && enabled)
    {
      cmsLog_debug("NATDetected notif");
      return TRUE;
    }
  }

  return FALSE;
}

static void updateUDPConnReqURL(UINT32 mapped_ip, UINT32 mapped_port)
{ 
  if (stunData.Enable == FALSE)
  {
    /* STUNEnable is false:
     * the host and port portions of the URL MUST represent the local IP 
     * address and port on which the CPE is listening for UDP Connection 
     * Request messages. 
     */
    stun_resetUDPConnReqAddress();
  }
  else
  {
    /* detects a change to the NAT binding  */
    if (nat_binding.address == mapped_ip && nat_binding.port == mapped_port)
    {
      cmsLog_debug("Binding does NOT change, reset 'nat_binding.binding_changed'");
      nat_binding.binding_changed = 0;     
    }
    else
    {
      cmsLog_debug("Binding change from %d.%d.%d.%d:%d to %d.%d.%d.%d:%d",
        (nat_binding.address&0xff000000)>>24,
     	  (nat_binding.address&0x00ff0000)>>16,
      	(nat_binding.address&0x0000ff00)>>8,
      	nat_binding.address&0xff, 
        nat_binding.port,
        (mapped_ip&0xff000000)>>24,
     	  (mapped_ip&0x00ff0000)>>16,
      	(mapped_ip&0x0000ff00)>>8,
      	mapped_ip&0xff,
        mapped_port);
    
      /* avoid initial case when nat_binding.address == 0 */ 
      if (nat_binding.address == 0)
      {
        nat_binding.binding_changed = 0;
      }
      else
      {
        nat_binding.binding_changed = 1;
      }
        
      nat_binding.address = mapped_ip;
      nat_binding.port = mapped_port;  

      nat_binding.timeout = 100;
    }

    /* If the received MAPPED-ADDRESS in the most recent Binding Response 
     * differs from the CPE's source address and port then NATDetected is 
     * true
     */
    nat_binding.nat_detected_changed = 0;
    int lanIPAddr;
    if (stunCms_getCPELanIPAddr(&lanIPAddr) != CMSRET_SUCCESS)
    {
       cmsLog_error("could not get LAN IP addr");
       return;
    }

    if (mapped_ip == lanIPAddr && mapped_port == stunData.ClientPort)
    {
      if (stunData.NATDetected == 1)
      {
        nat_binding.nat_detected_changed = 1;
      }

      stunData.NATDetected = 0;  
    }
    else
    {
      if (stunData.NATDetected == 0)
      {
        nat_binding.nat_detected_changed = 1;
      }

      stunData.NATDetected = 1;
    }

    if (nat_binding.nat_detected_changed)
    {
      cmsLog_debug("NATDetected changed"); 
    }
   
    /* CPE MUST record the value of the MAPPED-ADDRESS attrib in the 
     * most recently received Binding Response. This represents the
     * public IP address and port to which UDP Connection Requests
     * would be sent.
     */
    sprintf(new_conn,"%s:%d",stun_writeIp(mapped_ip),mapped_port);
  }
  
  if (stunData.UDPConnReqAddress)
  {
    if (!streq(stunData.UDPConnReqAddress, new_conn))
    {
      stunData.UDPConnReqAddressIsChanged = 1;      
    }
  }

  stun_SetUDPConnReqAddress(new_conn);

  /* Send inform with INFORM_EVENT_VALUE_CHANGE if necessary*/
  if (isNeededActiveNotification())    
  {
    addInformEventToList(INFORM_EVENT_VALUE_CHANGE);
    sendInform(NULL);
  }

  /* Whenever the CPE detects a change to the NAT binding it MUST immediately 
   * send a Binding Request */
  if (nat_binding.binding_changed || nat_binding.nat_detected_changed)
  {
    stun_sendBindingReq(NULL);
  }
}

/* Init UDP listener socket
 * return -1 for error
 *        != -1 is socket
 */
static int udp_listen(unsigned int ip, int port)
{
  int fd;
  int res;
  struct sockaddr_in addr;

  fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (fd < 0)
  {    
    cmsLog_error("Cannot create UDP socket");
    return -1;
  }

  memset(&addr, 0, sizeof(addr));
  addr.sin_family       = AF_INET;
  addr.sin_addr.s_addr  = htonl(ip);
  addr.sin_port         = htons(port);

  res = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
  if (res < 0)
  {
    cmsLog_error("Cannot bind UDP socket");
    close(fd);
    return -1;
  }

  return fd;
}

static void stun_timewait_timeout(void *handle) 
{
  /* enable send stun req timer again */
  cmsTmr_set(tmrHandle, stun_sendBindingReq, NULL, (stunData.MinKeepAlivePeriod*1000), NULL);
}

static void stun_rx_timeout(void *handle) 
{
  if (stun_retries < stunData.ClientSendRetryTimes)
  {
    stun_sendBindingReq(NULL);
    stun_retries++;
  }
  else
  {
    /* reset counter */
    stun_retries = 0;

    /* after send retry times also fail, then sleep */
    cmsTmr_set(tmrHandle, stun_timewait_timeout, NULL, (stunData.ClientTimeWait*1000), NULL);
  }  
}

static void stun_bindingchange_timeout(void *handle)
{
  if (stun_packet)
  {
    stun_send_pkt(stun_packet->data, stun_packet->len);

    switch (nat_binding.timeout) 
    {
      case 100:
      nat_binding.timeout = 300;
      break;
      case 300:
      nat_binding.timeout = 700;
      break;
      case 700:
      nat_binding.timeout = 1500;
      break;
      case 1500:
      nat_binding.timeout = 3100;
      break;
      case 3100:
      nat_binding.timeout = 4700;
      break;
      case 4700:
      nat_binding.timeout = 6300;
      break;
      case 6300:
      nat_binding.timeout = 7900;
      break;
      case 7900:
      nat_binding.timeout = 9500;
      break;    
      case 9500:
      // At 9500ms, the client considers the transaction to have failed if no 
      // response has been received. 
      nat_binding.timeout = 0;
      break;
    }
    
    if (nat_binding.timeout)
    {
      cmsTmr_set(tmrHandle, stun_bindingchange_timeout, NULL, nat_binding.timeout, NULL);
    }
    else
    {
      /* transaction have failed => enable */
      cmsTmr_set(tmrHandle, stun_sendBindingReq, NULL, (stunData.MinKeepAlivePeriod*1000), NULL);
    }
  }
}

/*
 * Prepare STUN request
 */
static int stun_prepareBindingReqPkt(void)
{
  if (stun_packet)
  {
    stun_pkt_delete(stun_packet);
  }

  stun_packet = stun_pkt_new(STUN_MT_BINREQ);
  if (!stun_packet)
  {
    cmsLog_error("cannot create a new packet");
    return 0;
  }
  
  if (stunData.Username)
  {
    if (!stun_pkt_add_str_attr(stun_packet, STUN_AT_USRNAM, stunData.Username))
    {
      stun_pkt_delete(stun_packet);
      cmsLog_error("cannot add username attr");
      return 0;
    }
  }
  
  if (!stun_pkt_add_byte_attr(stun_packet, STUN_AT_CONNREQBIN, (UINT8*)STUN_AT_CONNREQBIN_VALUE, sizeof(STUN_AT_CONNREQBIN_VALUE)))
  {
    cmsLog_error("cannot add conn req attr");
    stun_pkt_delete(stun_packet);
    return 0;
  }

  if (nat_binding.binding_changed || nat_binding.nat_detected_changed)
  {
    if (!stun_pkt_add_binding_change(stun_packet))
    {
      cmsLog_error("Error Adding BINDING-CHANGE attribute");
      return 0;
    }
    cmsLog_debug("Added BINDING-CHANGE attribute\n");
  }

  if (stun_unauth_received && stunData.Password)
  {
    /* If the CPE has been provisioned with a STUNUsername and STUNPassword in 
     * the ManagementServer object, then if the CPE receives a Binding Error 
     * Response from the STUN server with a fault code of 401 (Unauthorized), 
     * then the CPE MUST resend the Binding Request with the USERNAME and
     * MESSAGE-INTEGRITY attributes */

    UINT32 sha_key_len;   
    UINT8 sha_text[128];
    UINT8 sha_key[SHA_CBLOCK];
    char sha_md[SHA_DIGEST_LENGTH];
    
    memset(sha_md, 0, SHA_DIGEST_LENGTH);
    memset(sha_key, 0, SHA_CBLOCK);
    memset(sha_text, 0, 128);

    /* bump the length by 24 to include the size of the message-integrity */
    UINT16 new_len = stun_packet->data[2];
    new_len = new_len << 8;
    new_len |= stun_packet->data[3];   
    new_len += 24;
    stun_pkt_set_field(&stun_packet->data[2], new_len);

    /* The text used as input to HMAC is the STUN message, including the
       header, up to and including the attribute preceding the MESSAGE-
       INTEGRITY attribute */
    memcpy(sha_text, stun_packet->data, stun_packet->len); 
    
    /* The key used as input to HMAC is STUN password */
    strcpy((char *)sha_key, stunData.Password);
    sha_key_len = strlen(stunData.Password);

    /* HMAC-SHA1 of the STUN message */
    unsigned int len;
    HMAC_CTX ctx;
    HMAC_CTX_init(&ctx);
   
    HMAC_Init_ex(&ctx, sha_key, sha_key_len, EVP_sha1(), NULL);
    HMAC_Update(&ctx, (unsigned char*)&sha_text, stun_packet->len);
    HMAC_Final(&ctx, (unsigned char*)sha_md, &len);
    HMAC_CTX_cleanup(&ctx);

    /* add MESSAGE-INTEGRITY attribute */
    if (!stun_pkt_add_byte_attr(stun_packet, STUN_AT_MSGINT, (UINT8*)sha_md, SHA_DIGEST_LENGTH))
    {
      cmsLog_error("Error Adding MESSAGE-INTEGRITY attribute");
      return 0;
    }
  }

  return 1;
}

static int stun_readIpfromURL(const char* url)
{
  int res = 0;
  int n = 0;
  int i = 0;

  while (*url && n < 2)
  {
    if (*url == 0x2f) /* '/' */
    {
      ++n;
    }
    url++;
  }

  char buf[256];
  memset(buf, 0, sizeof(buf));

  while (*url)
  {
    if (*url == 0x3a) /* ':' */
    {
      break;
    }
    else
    {
      buf[i] = *url;
      ++i;
    }
    url++;
  }

  struct sockaddr_storage peer_addr;
  dns_lookup(buf, &peer_addr);

  char ipbuff[INET_ADDRSTRLEN];
  inet_ntop(peer_addr.ss_family, &(((struct sockaddr_in *)&peer_addr)->sin_addr), ipbuff, INET_ADDRSTRLEN);

  res = readIp(ipbuff);

  return res;
}


/* 
 * Send STUN packet through udp socket
 */
static void stun_send_pkt(UINT8 *data, UINT16 len)
{
  int server_address=0;
  struct sockaddr_in to;

  /* resolve STUN server address */
  if (stunData.ServerAddress)
  {
		if (strlen(stunData.ServerAddress))
		{
         struct sockaddr_storage peer_addr;
    	   dns_lookup(stunData.ServerAddress, &peer_addr);

         char ipbuff[INET_ADDRSTRLEN];
         inet_ntop(peer_addr.ss_family, &(((struct sockaddr_in *)&peer_addr)->sin_addr), ipbuff, INET_ADDRSTRLEN);

         server_address = readIp(ipbuff);
		}
  }

  /* If no STUNServerAddress is given, the address of the ACS determined from
     the host portion of the ACS URL MUST be used as the STUN server address*/
  if (server_address == 0)
  {
    if (acsState.acsURL)
    {
      server_address = stun_readIpfromURL(acsState.acsURL);
    }
    else
    {
      cmsLog_error("error");
      return;
    }
  }

  /* prepare STUN server address */
  memset(&to, 0, sizeof(to));
  to.sin_family = AF_INET;
  to.sin_port = htons(stunData.ServerPort);
  to.sin_addr.s_addr = htonl(server_address);

  /* send pkt */
  sendto(udp_sock, data, len, 0, (struct sockaddr *)&to, sizeof(to));
}

/*
 * Build and send BINDING REQUEST
 */
static void stun_sendBindingReq(void *handle) 
{
  if (stunData.Enable == 0)
  {
    /* enable send stun req timer again */
    cmsTmr_replaceIfSooner(tmrHandle, stun_sendBindingReq, NULL, (stunData.MinKeepAlivePeriod*1000), NULL);
  }
  else
  {    
    /* prepare STUN request */
    if (stun_prepareBindingReqPkt())
    {
      if (stun_packet)
      {
        /* send request to server */
        stun_send_pkt(stun_packet->data, stun_packet->len);

        /* start reception timeout timer */      
        if (!nat_binding.binding_changed)
        {
          cmsTmr_set(tmrHandle, stun_rx_timeout, NULL, stunData.ClientRecvTimeOut * 1000, NULL);
        }
        else
        {
          cmsTmr_set(tmrHandle, stun_bindingchange_timeout, NULL, nat_binding.timeout, NULL);
        }
      }             
    }
  }
}

/*
 * A client is trying to connect to us
 */
static void acsUDPConnect(void *handle)
{
  int fd = (int)handle;
  struct sockaddr si_other;
  socklen_t slen=sizeof(si_other);
  int size;
  UINT8* response;

  if (fd != udp_sock)
  {
    cmsLog_error("invalid handle");
    return;
  }

  response = (UINT8 *)calloc(STUN_MAX_MESSAGE_SIZE, sizeof(UINT8));
  if (!response)
  {
    cmsLog_error("calloc error");
    return;
  }

  size = recvfrom(
    fd, 
    response, 
    STUN_MAX_MESSAGE_SIZE, 
    0, 
    &si_other, 
    &slen);

  if (size > 0)
  {
    /* Because STUN responses and UDP Connection Requests will be received on 
       the same UDP port, the CPE MUST appropriately distinguish STUN messages 
       from UDP Connection Requests using the content of the messages 
       themselves. 
       As the first byte of all STUN messages is either 0 or 1, and the first 
       byte of the UDP Connection Request is always an ASCII encoded alphabetic
       letter, the CPE MAY use this distinction to distinguish between these 
       messages. */ 
    if (response[0] == 0 || response[0] == 1)
    {
      /* stop timeout timers */
      if (!nat_binding.binding_changed)
      {
        cmsTmr_cancel(tmrHandle, stun_rx_timeout, NULL);
      }
      else
      {
        nat_binding.binding_changed = 0;
        cmsTmr_cancel(tmrHandle, stun_bindingchange_timeout, NULL);
      }
            
      /* reset counter */
      stun_retries = 0;
   
      /* enable send stun req timer again */
      cmsTmr_replaceIfSooner(tmrHandle, stun_sendBindingReq, NULL, (stunData.MinKeepAlivePeriod*1000), NULL);

      /* Parse received packet as per STUN */    
      stun_pkt_resp* pkt_resp = stun_pkt_parse_resp(response, size);
      if (pkt_resp)
      {
        if (pkt_resp->errcode != STUN_EC_UNAUTH)
        {
          updateUDPConnReqURL(pkt_resp->address, pkt_resp->port);
        }
        else
        {        
          stun_unauth_received=1;          
        }
        free(pkt_resp); pkt_resp = NULL;                        
      }            
    }
    else
    {      
      /* Parse UDP Connection Requests 
         
        --- TR69 Amendment 4 - G.2.1.4  ---
         There is no response message associated with a UDP Connection Request 
         message. 
         When the CPE receives a UDP Connection Request message, it MUST both 
         authenticate and validate the message.

         The CPE MUST ignore a UDP Connection Request that is not successfully 
         authenticated or validated. */
      int ret = validate_UDPConnectionRequest((char *)response, strlen((char *)response));
      if (ret == 1)
      { 
        /* Whenever a CPE receives and successfully authenticates and validates a
         UDP Connection Request, it MUST follow the same requirements as for a 
         TCP-based Connection Request. */        
        handleConnectionRequest();
      }    
      else
      {
        cmsLog_error("Parse UDP Connection Requests failed - ret: %d",ret); 
      }
    }
  }
  
  free(response);
}

/* 
 * ACS UDP Connection Request callback
 */
void startACSUDPcallback(void *handle) 
{
  cmsLog_debug("STUN Client started...\n");
  stun_init();

  /* create UDP socket and bind it to acsUDPConnect */
  udp_sock = udp_listen(INADDR_ANY, stunData.ClientPort);
  if(udp_sock < 0)
  {
    cmsLog_error("cannot create UDP socket, retry in 5 sec"); 
    cmsTmr_set(tmrHandle, startACSUDPcallback, NULL, 5000, NULL); /* retry in 5 sec */
  }
  else 
  {
    setListener(udp_sock, acsUDPConnect, (void *)udp_sock );
  }

  /* send BINDING REQ to STUN server */
  cmsTmr_set(tmrHandle, stun_sendBindingReq, NULL, (stunData.MinKeepAlivePeriod*1000), NULL);
}

#endif // SUPPORT_STUN
