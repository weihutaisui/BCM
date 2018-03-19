/*
 *
 * Copyright (c) 2003-2012  Broadcom Corporation
 * All Rights Reserved
 *
 * <:label-BRCM:2012:proprietary:standard
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
 * $Id: upnp.c,v 1.12.20.2 2003/10/31 21:31:36 mthawani Exp $
 */

#include <signal.h>
#include <stdarg.h>

#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"

#include "cms_log.h"
#include "cms_core.h"
#include "cms_msg.h"
#include "cms_util.h"
#include "pcpiwf.h"

extern void periodic_advertiser(timer_t, ssdp_t sstype);
extern void send_advertisements(ssdp_t);
extern void alarm_handler(int i);
extern void init_event_queue(int);
extern void gena_subscription_reaper(timer_t, void *arg);
extern void uuidstr_create(char *, int);
extern struct net_connection *make_http_socket(struct iface *);
extern struct net_connection *make_ssdp_socket(struct iface *);
extern void delete_all_subscriptions();
extern void delete_all_connections();
extern PDevice init_device(PDevice, PDeviceTemplate,int InstId, ...);
extern void destroy_device(PDevice);
extern int timer_connect(timer_t, VOIDFUNCPTR, uintptr_t);

extern void get_deviceInfo(void);

extern void *g_msgHandle;

static void interrupt_handler(int i);


static struct iface *create_lan_interface(char *);
static int fd_net_set(fd_set *fds);
static void process_receive_fds(fd_set *fds);
static void connection_reaper(timer_t, void *arg);
static void block_alarm();
static void unblock_alarm();
static void generate_uuids();
static struct net_connection *set_smd_listener(void);
static void process_smd_connection(caction_t flag, struct net_connection *nc, void *arg);

struct net_connection *net_connections = NULL;

int global_exit_now = FALSE;

struct iface *global_lans = NULL;

char g_lan_ifname[CMS_IFNAME_LENGTH];
char g_wan_ifname[CMS_IFNAME_LENGTH];
char g_wan_l2ifname[CMS_IFNAME_LENGTH];
int   g_wan0_primary;

Error UPNPErrors[] = {
    { SOAP_INVALID_ACTION, "Invalid Action" },
    { SOAP_INVALID_ARGS, "Invalid Args" },
    { SOAP_INVALID_VAR, "Invalid Var" },
    { SOAP_ACTION_FAILED, "Action Failed" },
    { SOAP_INVALIDDEVICEUUID, "InvalidDeviceUUID" },
    { SOAP_INVALIDSERVICEID, "InvalidServiceID" },
    { 0, NULL }
};

#ifdef BCMDBG
//int upnp_msg_level = 0x1;
int upnp_msg_level = 0xffffffff;
#endif

int mode = PCP_MODE_DISABLE;
char pcpsrv[CMS_IPADDR_LENGTH]="\0";
char pcplocal[CMS_IPADDR_LENGTH]="\0";
uint8_t pcpextaddr[16];

int upnp_main(PDeviceTemplate pdevtmpl, char *ifname, char *wanif, char *wanif_l2) 
{
    fd_set rfds;
    int n;
    struct  itimerspec  timer;
    struct iface *lanif;
    timer_t td1, td2, td3;
    PDevice device;
    CmsRet ret;

   UPNP_TRACE(("Entered upnp_main pdevtmpl=0x%" PRIxPTR " \"%s\" ifname=0x%" PRIxPTR " \"%s\"\n", 
		(uintptr_t) pdevtmpl, (pdevtmpl ? pdevtmpl->type : "NULL"),
		(uintptr_t) ifname, (ifname ? ifname : "NULL")));

   soap_register_errors(UPNPErrors);
   
#ifdef DESKTOP_LINUX
  /*assuming lan interface as eth0 */
   ifname = "eth0" ;	
#endif
  
    if (mode == PCP_MODE_DSLITE)
    {
        if (!cmsUtl_isValidIpAddress(AF_INET6, pcpsrv) || 
            !cmsUtl_isValidIpAddress(AF_INET6, pcplocal))
        {
            UPNP_ERROR(("invalid pcp address srv:\"%s\"  local:\"%s\"\n", pcpsrv, pcplocal));
            exit(1);
        }
    }
    else if (mode == PCP_MODE_NAT444)
    {
        if (!cmsUtl_isValidIpAddress(AF_INET, pcpsrv) || 
            !cmsUtl_isValidIpAddress(AF_INET, pcplocal))
        {
            UPNP_ERROR(("invalid pcp address srv:\"%s\"  local:\"%s\"\n", pcpsrv, pcplocal));
            exit(1);
        }
    }

   if ((lanif = create_lan_interface(ifname)) == NULL) {
	   UPNP_ERROR(("cannot use LAN interface \"%s\"\n", ifname));
   	exit(1);
   } else {
	   lanif->next = global_lans;
   	global_lans = lanif;
   }

   strcpy(g_lan_ifname,ifname); 
   strcpy(g_wan_ifname,wanif); 
   strcpy(g_wan_l2ifname,wanif_l2); 

   //g_wan0_primary =1;

    //PCP TODO
    memset(pcpextaddr, 0, 16);
    if (mode != PCP_MODE_DISABLE)
    {
        pcp_boot();
    }

   UPNP_TRACE(("Initializing devices for IGD\n"));

      if ((ret = cmsLck_acquireLockWithTimeout(UPNP_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get lock, ret=%d", ret);
         return 0;
      }

   get_deviceInfo();
   device = init_device(NULL, pdevtmpl,0);

   cmsLck_releaseLock();

   UPNP_TRACE(("Generating UUIDs.\n"));
   generate_uuids();
   UPNP_TRACE(("Finished generating UUIDs.\n"));
   fflush(stdout);

   block_alarm();

   /*
    * detach from the terminal so we don't catch the user typing control-c.
    */
   if (setsid() == -1)
   {
      cmsLog_error("Could not detach from terminal");
   }
   
   signal(SIGINT, interrupt_handler);
   signal(SIGTERM, interrupt_handler);
    
   memset(&timer, 0, sizeof(timer));
   timer.it_interval.tv_sec = 30;
   timer.it_value.tv_sec = 30;
   td1 = enqueue_event(&timer, (event_callback_t)connection_reaper, NULL);

   memset(&timer, 0, sizeof(timer));
   timer.it_interval.tv_sec = 30;
   timer.it_value.tv_sec = 30;
   td2 = enqueue_event(&timer, (event_callback_t)gena_subscription_reaper, NULL);

   memset(&timer, 0, sizeof(timer));
   timer.it_interval.tv_sec = UPNP_REFRESH;
   timer.it_value.tv_sec = UPNP_REFRESH;
   td3 = enqueue_event(&timer, (event_callback_t)periodic_advertiser, (void *) SSDP_ALIVE);

   UPNP_TRACE(("Sending initial advertisements.\n"));
   send_advertisements(SSDP_BYEBYE);
   send_advertisements(SSDP_BYEBYE);
   send_advertisements(SSDP_BYEBYE);
   send_advertisements(SSDP_ALIVE);
   send_advertisements(SSDP_ALIVE);
   send_advertisements(SSDP_ALIVE);

   // enter the top of the event loop.
   UPNP_TRACE(("Entering upnp loop\n"));
   /*setup listener for messages from smd */
   if(!set_smd_listener()){
       goto Exit;
   }   
   global_exit_now = FALSE;
   while (global_exit_now == FALSE) {
	   FD_ZERO(&rfds);
   	n = fd_net_set(&rfds);

	   unblock_alarm();
   	n = select(n, &rfds, NULL, NULL, NULL);
	   block_alarm();

	   if (global_exit_now)
	      break;
   	if (n > 0) 
	      process_receive_fds(&rfds);
   }

Exit:
   UPNP_TRACE(("Exiting upnp loop\n"));

   /* cancel timers */
   timer_delete(td1);
   timer_delete(td2);
   timer_delete(td3);

   delete_all_subscriptions();
   delete_all_connections();

   //PCP
    if (mode != PCP_MODE_DISABLE)
    {
        struct transaction *t, *tmp;
        UPNP_TRACE(("exit and remove all t\n"));
        LIST_FOREACH_SAFE(t, &pcp_transactions, chain, tmp) {
          (void) pcp_close(&t->pcp, FALSE);
          LIST_REMOVE(t, chain);
          pcp_free((void *)t);
        }

        LIST_FOREACH_SAFE(t, &pcp_renew_transactions, renew_chain, tmp) {
          LIST_REMOVE(t, renew_chain);
          timer_delete(t->tr);
          (void) pcp_close(&t->pcp, TRUE);
          pcp_free((void *)t);
        }
    }

   destroy_device(device);

   return 0;
}


void interrupt_handler(int i)
{
   cmsLog_debug("====>Enter i=%d", i);
//    signal(SIGINT, SIG_DFL);
   send_advertisements(SSDP_BYEBYE);
   global_exit_now = TRUE;
}


/* 
   Strip characters in 'reject' from start and end of the string 'str'.
   Return a pointer to the start of the modified string.
*/
char *strip_chars(char *str, char *reject)
{
   char *end;

   str += strspn(str, reject);
   end = &str[strlen(str)-1];
   while (end > str && strpbrk(end, reject))
      *end-- = '\0';

   return str;
}



static struct iface *create_lan_interface(char *ifname)
{
   struct net_connection *nc;
   struct iface *pif = NULL;

   for (pif = (struct iface *)cmsMem_alloc(sizeof(struct iface), ALLOC_ZEROIZE);
        pif != NULL;
        cmsMem_free(pif), pif = NULL) {

      pif->ifname = ifname;

      if ( !osl_ifaddr(ifname, &pif->inaddr) ) {
         UPNP_ERROR(("osl_ifaddr failed.\n"));
         continue;
      }

      if ((nc = make_http_socket(pif)) == NULL) {
         continue;
      }

      if ((nc = make_ssdp_socket(pif)) == NULL) {
         // need to destroy the http connection here...
         continue;
      }
      pif->ssdp_connection = nc;

      break;
   }

   return pif;
}


void reinit_lan_interface(struct iface *pif)
{
   struct net_connection *nc;

   do {
      delete_all_connections();
	
      /*setup listener for messages from smd */
      if(!set_smd_listener()){
         goto Exit;
      }   

      if ( !osl_ifaddr(pif->ifname, &pif->inaddr) ) {
         UPNP_ERROR(("osl_ifaddr failed.\n"));
         goto Exit;
      }

      if ((nc = make_http_socket(pif)) == NULL) {
         goto Exit;
      }

      if ((nc = make_ssdp_socket(pif)) == NULL) {
         // need to destroy the http connection here...
         goto Exit;
      }
      pif->ssdp_connection = nc;

   } while (0);

   return;

Exit:
  exit(1);
}



/* Construct the bitmask of file descriptors that will be passed to select(2).
   For each network connection that we are maintaining, set the
   appropriate bit in a mask that will be used with a BSD-style select
   call. */
static int fd_net_set(fd_set *fds)
{
   struct net_connection *net;
   int max = 0;

   for (net = net_connections; net; net = net->next) {
      if (net->fd > max)
	      max = net->fd;
      FD_SET(net->fd, fds);
   }

   //PCP
   if (mode != PCP_MODE_DISABLE)
   {
       int s;
       struct transaction *t;

       LIST_FOREACH(t, &pcp_transactions, chain) {
          if (pcp_getsocket(&t->pcp, &s) != PCP_OK)
             continue;

          if (s > max)
	          max = s;

          FD_SET(s, fds);
       }
   }

   return (max + 1);
}

#define PCP_LIST_PREV(elm, field)   ((elm)->field.le_prev)
static void process_receive_fds(fd_set *fds)
{
   struct net_connection *net, *next;

   for (net = net_connections; net; net = next) {
      next = net->next;
      if (FD_ISSET(net->fd, fds)) {
         assert(net->func);
         (*(net->func))(CONNECTION_RECV, net, net->arg);
      }
   }

   if (mode != PCP_MODE_DISABLE)
   {
       struct transaction *t;
       int s;

       LIST_FOREACH(t, &pcp_transactions, chain) {
          if (pcp_getsocket(&t->pcp, &s) != PCP_OK)
             continue;

          if (FD_ISSET(s, fds))
          {
            ProcessPCPResponse(t);
            {
                struct transaction *tmp = PCP_LIST_PREV(t, chain);

                LIST_REMOVE(t, chain);
                /* 
                 * Non-Renew case: UPnP error should be handled, free t
                 * Renew case: timer for renew should be set, so t and t->pcp cannot be freed
                 */
                if (t->status != PCP_RENEW_STATE)
                {
                    (void) pcp_close(&t->pcp, FALSE);

                    /*
                     * Error could happen during renew period. We will just stop.
                     * TODO: Retransmit??
                     */
                    if (t->type & TTYPE_PCP_RENEW)
                    {
                        LIST_REMOVE(t, renew_chain);
                    }

                    pcp_free((void *)t);
                }
                else
                {
                    if (close(t->pcp.s) < 0)
                    {
                        UPNP_ERROR(("fali to close pcp socket!\n"));
                    }
                }

                t = tmp;
            }
          }
       }
   }
}

/* Big hammer to delete all existing connections. 
   Typically this should only be used in preparation for shutting down the UPNP server. 
*/
void delete_all_connections()
{
   struct net_connection **pnet, *net;

   pnet = &net_connections; 
   while (*pnet) {
      net = *pnet;
      *pnet = (*pnet)->next;
      assert(net->func);

      (*(net->func))(CONNECTION_DELETE, net, net->arg);
   }
}

void remove_net_connection(int fd)
{
   struct net_connection *net, **pnet;

   pnet = &net_connections;
   while (*pnet) {
      if ((*pnet)->fd == fd) {
         net = *pnet;
         *pnet = (*pnet)->next;
         assert(net->func);
         (*(net->func))(CONNECTION_DELETE, net, net->arg);
         break;
      }
      pnet = &((*pnet)->next);
   }
}


static void connection_reaper(timer_t t, void *arg)
{
   struct net_connection **pnet, *net;
   time_t now;
   int i = 0;

   now = time(NULL);
   pnet = &net_connections;
   while (*pnet) {
      i++;
      if ((*pnet)->expires && (*pnet)->expires < now) {
         net = *pnet;
         *pnet = (*pnet)->next;
         assert(net->func);
         (*(net->func))(CONNECTION_DELETE, net, net->arg);
         continue;
      }
      pnet = &((*pnet)->next);
   }
}

static struct net_connection *set_smd_listener(void)
{
   struct net_connection *c = NULL;
//   struct sockaddr_in sockaddr;
   int fd;

   cmsMsg_getEventHandle(g_msgHandle, &fd);

   if (fd < 0) 
      goto Error;


   c = (struct net_connection *) cmsMem_alloc(sizeof(struct net_connection), ALLOC_ZEROIZE);
   if (c == NULL) {
      UPNP_ERROR((" %s cannot cmsMem_alloc.\n", __FUNCTION__));
      goto Error;
   }


   c->fd = fd;
   c->expires = 0;  // never expires.
   c->func = (CONNECTION_HANDLER) process_smd_connection;
   c->arg = (void *)(intptr_t) fd;

   c->next = net_connections;
   net_connections = (struct net_connection *) c;

   return c;

Error:
   /* cleanup code */
   UPNP_ERROR(("%s failed - err %d\n", __FUNCTION__, errno));

   return NULL;
}

static void process_smd_connection(caction_t flag, struct net_connection *nc, void *arg)
{
   CmsMsgHeader *msg=NULL;
   CmsRet ret;
   void *msgHandle = g_msgHandle;
   switch (flag) {

      case CONNECTION_RECV: 
         if ((ret = cmsMsg_receive(msgHandle, &msg)) != CMSRET_SUCCESS)
         {
            if (ret == CMSRET_DISCONNECTED)
            {
               if (!cmsFil_isFilePresent(SMD_SHUTDOWN_IN_PROGRESS)) {
                   cmsLog_error("detected exit of smd, upnp will also exit");
               }
               exit(1);
            }
            else
            {
               cmsLog_error("error during cmsMsg_receive, ret=%d", ret);
            }
         } else {
            switch(msg->type)
            {
               case CMS_MSG_SET_LOG_LEVEL:
                  cmsLog_setLevel(msg->wordData);
                  if ((ret = cmsMsg_sendReply(msgHandle, msg, CMSRET_SUCCESS)) != CMSRET_SUCCESS)
                  {
                     cmsLog_error("send response for msg 0x%x failed, ret=%d", msg->type, ret);
                  }
                  break;

               case CMS_MSG_SET_LOG_DESTINATION:
                  cmsLog_setDestination(msg->wordData);
                  if ((ret = cmsMsg_sendReply(msgHandle, msg, CMSRET_SUCCESS)) != CMSRET_SUCCESS)
                  {
                     cmsLog_error("send response for msg 0x%x failed, ret=%d", msg->type, ret);
                  }
                  break;
               default:
                  cmsLog_error("cannot handle msg type 0x%x", msg->type);
                  break;
            }
            CMSMEM_FREE_BUF_AND_NULL_PTR(msg);
         }

         break;

      case CONNECTION_DELETE: 
         cmsMem_free(nc);
         break;
   } /* end switch */
}



timer_t enqueue_event(struct itimerspec *value, event_callback_t func, void *arg)
{
   timer_t          td;

   timer_create(CLOCK_REALTIME, NULL, &td);

   timer_connect(td, (VOIDFUNCPTR) func, (uintptr_t) arg);

   timer_settime(td, 0, value, NULL);

   return(td);
}


static void block_alarm()
{
   sigset_t set;

   sigemptyset(&set);
   sigaddset(&set, SIGALRM);
   sigprocmask(SIG_BLOCK, &set, NULL);
}

static void unblock_alarm()
{
   sigset_t set;

   sigemptyset(&set);
   sigaddset(&set, SIGALRM);
   sigprocmask(SIG_UNBLOCK, &set, NULL);
}


// Create the unique device identifiers for each device.
static void generate_uuids()
{
   char *udn;
   PDevice pdev;

   forall_devices(pdev) {
      udn = cmsMem_alloc(50, ALLOC_ZEROIZE);
      if (udn != NULL) {
         strcpy(udn, "uuid:");
         cmsUtl_generateUuidStrFromRandom(udn+5, 50 - 5);
      }

      // the device definition contains a unique string that should be substituted whenever 
      // the http server sends out the XML description document.
      pdev->udn = udn;
   }
}


static void delayed_call_helper(timer_t t, void *arg)
{
   voidfp_t f = (voidfp_t) arg;

   timer_delete(t);
   (*f)();
}

void delayed_call(uint seconds, voidfp_t f)
{
   struct  itimerspec  timer;

   memset(&timer, 0, sizeof(timer));
   timer.it_interval.tv_sec = 0;
   timer.it_value.tv_sec = seconds;
   enqueue_event(&timer, (event_callback_t)delayed_call_helper, f);
}

/** findActionParamByName
 *  input parameter: Action pointer, and Var Name .
 *  output: returns pointer to struct Param of this variable. NULL if not found.
 */
struct Param *findActionParamByName(PAction ac, char *varName)
{
   struct Param *ptr;
   int index = 0;

   while (((ptr = &ac->params[index]) != NULL) && (ptr->name != NULL))
   {
      if (!cmsUtl_strcmp(ptr->name,varName))
      {
         return ptr;
      }
      index++;
   } /* while */
   return ((struct Param*)NULL);
}

int OutputCharValueToAC(PAction ac, char *varName, char *value)
{
   int errorinfo = 0;
   struct Param *pParams;

   pParams = findActionParamByName(ac,varName);
   if (pParams != NULL)
   {
      if( value != NULL )
      {
         REPLACE_STRING_IF_NOT_EQUAL(pParams->value,value);
      }
      else
      {
         REPLACE_STRING_IF_NOT_EQUAL(pParams->value,"");
      }
   }
   else
   {
      errorinfo = SOAP_ACTION_FAILED;
   }
   cmsLog_debug("error info for %s =%d\n",varName, errorinfo);
   return errorinfo;
}

int OutputNumValueToAC(PAction ac, char* varName, int value)
{
   int errorinfo = 0;
   struct Param *pParams;
   char tmp[BUFLEN_32];

   memset(tmp, 0, BUFLEN_32);
   sprintf(tmp, "%d", value);

   pParams = findActionParamByName(ac,varName);
   if (pParams != NULL)
   {
      REPLACE_STRING_IF_NOT_EQUAL(pParams->value,tmp);
   }
   else
   {
      errorinfo = SOAP_ACTION_FAILED;
   }
   cmsLog_debug("error info for %s =%d\n",varName, errorinfo);
   return errorinfo;
}





