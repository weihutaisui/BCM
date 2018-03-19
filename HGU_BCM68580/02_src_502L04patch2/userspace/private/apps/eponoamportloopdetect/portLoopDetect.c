//<:copyright-BRCM:2002:proprietary:epon 
//
//   Copyright (c) 2002 Broadcom 
//   All Rights Reserved
//
// This program is the proprietary software of Broadcom and/or its
// licensors, and may only be used, duplicated, modified or distributed pursuant
// to the terms and conditions of a separate, written license agreement executed
// between you and Broadcom (an "Authorized License").  Except as set forth in
// an Authorized License, Broadcom grants no license (express or implied), right
// to use, or waiver of any kind with respect to the Software, and Broadcom
// expressly reserves all rights in and to the Software and all intellectual
// property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
// NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
// BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
//
// Except as expressly set forth in the Authorized License,
//
// 1. This program, including its structure, sequence and organization,
//    constitutes the valuable trade secrets of Broadcom, and you shall use
//    all reasonable efforts to protect the confidentiality thereof, and to
//    use this information only in connection with your use of Broadcom
//    integrated circuit products.
//
// 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
//    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
//    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
//    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
//    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
//    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
//    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
//    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
//    PERFORMANCE OF THE SOFTWARE.
//
// 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
//    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
//    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
//    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
//    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
//    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
//    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
//    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
//    LIMITED REMEDY.
//:>
#include <sys/sysctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include "TkPlatform.h"
#include "CtcOam.h"
#include "cms_log.h"
#include "cms_msg.h"
#include "cms_mem.h"
#include "cms_tms.h"
#include "ethswctl_api.h"

/*
 * Port Loop Detect App is to handle the CTC PortLoopDetect and
 * PortDisableLooped OAM attributes. It is launched from TkDataRxThread
 * during start up.
 * - create cmsMsgFd for CMS messaging.
 * - create loopDetectFd raw socket for tx/rx loop detect frame.
 * - Process CMS_MSG_PORT_LOOP_DETECT and CMS_MSG_PORT_DISABLE_LOOPED
 *   messages received from TkDataRxThread. 
 * - Every LOOP_DETECT_FRAME_TX_PERIOD, broadcast a loop detect frame out
 *   of each loop detect enabled port.
 * - The loop detect frame is a fake OAM frame with subtype 0. Its source
 *   mac address is that of the loop detect port. Its destination mac
 *   address is broadcast. The first byte of the OAM data is the ifindex
 *   of the loop detect port. 
 * - Listen on loopDetectFd to monitor any detect frame being looped back.
 *   The looped back port is identified by the source mac address and the
 *   ifindex byte of the looped back frame.
 * - Maintain loop detect info structure for each port. Port "looped" will
 *   be set and timestamped when loop is detected. Port "looped" will be
 *   cleared if no looped frame is received for LOOPED_CLEAR_PERIOD.  
 */
 
#define NUM_ETH_PORT                4
#define LOOP_DETECT_FRAME_TX_PERIOD 1000  /* in msec */
#define LOOPED_CLEAR_PERIOD         20000 /* in msec */

#define LOOP_DETECT_FRAME_LEN       60    /* withoud FCS */

/* OAM data offset = ETH_HLEN + subtype(1) + flags(2) + code(1) */
#define OAM_DATA_OFFSET             (ETH_HLEN + 1 + 2 + 1)

/* Data structures */
struct portLoopDetect
{
   SINT32           ifindex;
   UINT8            ifmac[ETH_ALEN];
   UINT32           detectMgmt;     /* either OamCtcActionDisable or OamCtcActionEnable */
   UINT32           loopedAction;   /* either OamCtcLoopedPortEnable or OamCtcLoopedPortDisable */
   CmsTimestamp     timestamp;      /* tms when port loop is detected */
   UBOOL8           looped;         /* TRUE- port looped, FALSE- port not looped */
   UBOOL8           blocked;        /* TRUE- port blocked, FALSE- port not blocked */
};


/* Static variables */
static struct portLoopDetect loopDetect[NUM_ETH_PORT];
static void     *msgHandle   = NULL;
static SINT32   cmsMsgFd     = -1;
static SINT32   loopDetectFd = -1;
static UINT8    rxbuf[TK_MAX_RX_TX_DATA_LENGTH];
static UINT8    txbuf[LOOP_DETECT_FRAME_LEN];


/* Static functions */
static CmsRet loopDetectInit(void);
static void   loopDetectTxbufInit(void);
static void   loopDetectUpdate(void);
static CmsRet loopDetectTx(void);
static CmsRet loopDetectRx(void);
static CmsRet processCmsMsg(CmsMsgHeader *msg);
static CmsRet processPortLoopDetect(SINT32 port, UINT32 mgmt);
static CmsRet processPortDisableLooped(SINT32 port, UINT32 action);
static void   dumpFrame(void *buf, UINT32 len);


int main(int argc, char **argv)
{
   CmsRet         ret = CMSRET_SUCCESS;
   CmsLogLevel    logLevel = DEFAULT_LOG_LEVEL;
   SINT32         c, logLevelNum;
   SINT32         n, maxfd;
   UINT32         msec;
   fd_set         rfds;
   struct timeval tm;
   CmsTimestamp   currTms, nextTms;
   
   /* initialize CMS logging.*/
   cmsLog_init(EID_EPON_OAM_PORT_LOOP_DETECT);
 
   while ((c = getopt(argc, argv, "v:")) != -1)
   {
      switch(c)
      {
      case 'v':
         logLevelNum = atoi(optarg);
         if (logLevelNum == 0) logLevel = LOG_LEVEL_ERR;
         else if (logLevelNum == 1) logLevel = LOG_LEVEL_NOTICE;
         else logLevel = LOG_LEVEL_DEBUG;
         break;

      default:
         break;
      }
   }

   /* set log level */    
   cmsLog_setLevel(logLevel);
   
   /* detach from terminal and detach from smd session group. */
   if (setsid() < 0)
   {
      cmsLog_error("could not detach from terminal");
      exit(-1);
   }

   /* ignore some common, problematic signals */
   signal(SIGINT, SIG_IGN);
   signal(SIGPIPE, SIG_IGN);

   /* Connect to smd */
   if ((ret = cmsMsg_init(EID_EPON_OAM_PORT_LOOP_DETECT, &msgHandle)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMsg_init returns error. ret=%d", ret);
      cmsLog_cleanup();
      exit(-1);
   }

   if ((ret = cmsMsg_getEventHandle(msgHandle, &cmsMsgFd)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMsg_getEventHandle returns error. ret=%d", ret);
      cmsLog_cleanup();
      cmsMsg_cleanup(&msgHandle);
      exit(-1);
   }
   
   if ((ret = loopDetectInit()) != CMSRET_SUCCESS)
   {
      cmsLog_error("loopDetectInit returns error. ret=%d", ret);
      cmsLog_cleanup();
      cmsMsg_cleanup(&msgHandle);
      exit(-1);
   }
   
   cmsLog_debug("Port Loop Detect App is running. cmsMsgFd=%d loopDetectFd=%d", cmsMsgFd, loopDetectFd);
   
   /* get the next time to send loop detect frame */
   cmsTms_get(&nextTms);
   cmsTms_addMilliSeconds(&nextTms, LOOP_DETECT_FRAME_TX_PERIOD);

   while (1)
   {
      /* Add all sockets to listen to */
      FD_ZERO(&rfds);

      FD_SET(cmsMsgFd, &rfds);
      maxfd = cmsMsgFd;

      FD_SET(loopDetectFd, &rfds);
      if (maxfd < loopDetectFd)
         maxfd = loopDetectFd;

      /* calculate timeout value for select */
      cmsTms_get(&currTms);
      msec = cmsTms_deltaInMilliSeconds(&nextTms, &currTms);
      if (msec < 10 || msec == 0xFFFFFFFF)
      {
         /* currTms is close to or just passed nextTms.
          * Send out loop detect frame to activated loop detect port.
          */
         loopDetectTx();
      
         /* update nextTms */
         cmsTms_get(&nextTms);
         cmsTms_addMilliSeconds(&nextTms, LOOP_DETECT_FRAME_TX_PERIOD);
         
         /* start another full timeout */
         tm.tv_sec  = 0;
         tm.tv_usec = LOOP_DETECT_FRAME_TX_PERIOD * 1000;
      }
      else
      {
         /* start partial timeout till nextTms */
         tm.tv_sec  = 0;
         tm.tv_usec = msec * 1000;  /* convert from ms to us */
      }
      
      n = select(maxfd+1, &rfds, NULL, NULL, &tm);
      if (n > 0)
      {
         if (FD_ISSET(cmsMsgFd, &rfds))
         {
            CmsMsgHeader *msg;
            
            if ((ret = cmsMsg_receiveWithTimeout(msgHandle, &msg, 100)) == CMSRET_SUCCESS)
            {
               processCmsMsg(msg);
               CMSMEM_FREE_BUF_AND_NULL_PTR(msg);
            }
            else
            {
               cmsLog_error("cmsMsg_receiveWithTimeout ret=%d", ret);
            }
         }
         
         if (FD_ISSET(loopDetectFd, &rfds))
         {
            loopDetectRx();
         }
      }
      
      /* update loop detect status */
      loopDetectUpdate();
      
   }  /* while (1) */
    
   return 0;
   
}  /* End of main() */

CmsRet loopDetectInit(void)
{
   CmsRet ret = CMSRET_SUCCESS;
   struct ifreq req;
   SINT32 p;

   for (p = 0; p < NUM_ETH_PORT; p++)
   {
      bzero(&loopDetect[p], sizeof(struct portLoopDetect));
   }

   if ((loopDetectFd = socket(AF_PACKET, SOCK_RAW, htons(TK_CAP_PKT_TYPE))) < 0)
   {
      cmsLog_error("failed to create raw socket!");
      return CMSRET_INTERNAL_ERROR;
   }

   for (p = 0; p < NUM_ETH_PORT; p++)
   {
	   bzero(&req, sizeof(req));

// bcm_ifname_get was not implemented.
//      if (bcm_ifname_get(0, p, req.ifr_name) != 0)
//      {
//         cmsLog_error("failed to get ifname");
//         close(loopDetectFd);
//         ret = CMSRET_INTERNAL_ERROR;
//         break;
//      }
      sprintf(req.ifr_name, "eth%d", (int)p);
      
      if (ioctl(loopDetectFd, SIOCGIFHWADDR, &req) < 0)
      {
         cmsLog_error("failed to get %s hw address!", req.ifr_name);
         close(loopDetectFd);
         ret = CMSRET_INTERNAL_ERROR;
         break;
      }

      if (ioctl(loopDetectFd, SIOCGIFINDEX, &req) < 0)
      {
         cmsLog_error("failed to get %s ifindex!", req.ifr_name);
         close(loopDetectFd);
         ret = CMSRET_INTERNAL_ERROR;
         break;
      }
      cmsLog_debug("%s ifindex=%d mac= %02x:%02x:%02x:%02x:%02x:%02x",
                   req.ifr_name,
                   req.ifr_ifindex,
                   req.ifr_hwaddr.sa_data[0],
                   req.ifr_hwaddr.sa_data[1],
                   req.ifr_hwaddr.sa_data[2],
                   req.ifr_hwaddr.sa_data[3],
                   req.ifr_hwaddr.sa_data[4],
                   req.ifr_hwaddr.sa_data[5]);

      loopDetect[p].ifindex      = req.ifr_ifindex;
      memcpy(loopDetect[p].ifmac, req.ifr_hwaddr.sa_data, ETH_ALEN);
      loopDetect[p].detectMgmt   = OamCtcActionDisable; /* default */
      loopDetect[p].loopedAction = OamCtcLoopedPortEnable;
      loopDetect[p].timestamp.sec  = 0;
      loopDetect[p].timestamp.nsec = 0;
      loopDetect[p].looped       = FALSE;
      loopDetect[p].blocked      = FALSE;
   }
    
   /* initialize loop detect test frame */
   if (ret == CMSRET_SUCCESS)
   {
      loopDetectTxbufInit();
   }
   else
   {
      close(loopDetectFd);
   }
    
   return ret;
    
}  /* End of loopDetectInit() */

void loopDetectTxbufInit(void)
{
   /* pointer to ethenet header */
   struct ethhdr *eh = (struct ethhdr *)txbuf;
	
   /* userdata in ethernet frame */
   UINT8 *data = txbuf + ETH_HLEN;
	
   /* broadcast frame */
   eh->h_dest[0] = 0xff;
   eh->h_dest[1] = 0xff;
   eh->h_dest[2] = 0xff;
   eh->h_dest[3] = 0xff;
   eh->h_dest[4] = 0xff;
   eh->h_dest[5] = 0xff;

   /* source mac will be filled in when transmit */
   
   eh->h_proto = htons(TK_CAP_PKT_TYPE);
   
   /* fill data with zero */
   bzero(data, LOOP_DETECT_FRAME_LEN-ETH_HLEN);

}  /* End of loopDetectTxbufInit() */

CmsRet loopDetectRx(void)
{
   SINT32 len;
   SINT32 p;

   len = recvfrom(loopDetectFd, rxbuf, TK_MAX_RX_TX_DATA_LENGTH, 0, NULL, NULL);
   if (len != LOOP_DETECT_FRAME_LEN + ETH_FCS_LEN)    /* receive frame len includes FCS */
   {
      return CMSRET_INTERNAL_ERROR;
   }   
   cmsLog_notice("Received frame from loopDetectFd");
   dumpFrame(rxbuf, len);

   for (p = 0; p < NUM_ETH_PORT; p++)
   {
      /* If the receive frame source mac is from one of the eth interface,
       * a loop is detected.
       */ 
      if (memcmp(rxbuf+ETH_ALEN, loopDetect[p].ifmac, ETH_ALEN) == 0 &&
          rxbuf[OAM_DATA_OFFSET] == (UINT8)(loopDetect[p].ifindex))
      {
         /* do nothing if loop detect is not activated */
         if (loopDetect[p].detectMgmt == OamCtcActionEnable)
         {
            cmsLog_debug("port %d is looped", p);
            
	         cmsTms_get(&(loopDetect[p].timestamp));
            loopDetect[p].looped = TRUE;
            
            if (loopDetect[p].loopedAction == OamCtcLoopedPortDisable &&
                !loopDetect[p].blocked)
            {
               /* disable port */
               cmsLog_debug("block looped port %d", p);
               bcm_port_traffic_control_set(0, bcm_enet_map_oam_idx_to_phys_port(p), PORT_RXDISABLE | PORT_TXDISABLE);
               loopDetect[p].blocked = TRUE;
            }
         }
         break;
      }
   }
   return CMSRET_SUCCESS;
   
}  /* End of loopDetectRx() */

void loopDetectUpdate(void)
{
   CmsTimestamp currTms;
   UINT32 msec;
   SINT32 p;
   
   for (p = 0; p < NUM_ETH_PORT; p++)
   {
      if (loopDetect[p].looped)
      {
         cmsTms_get(&currTms);
         msec = cmsTms_deltaInMilliSeconds(&currTms, &(loopDetect[p].timestamp));
         if (msec >= LOOPED_CLEAR_PERIOD)
         {
            /* port looped clear */
            cmsLog_debug("port %d looped cleared after %d msec", p, LOOPED_CLEAR_PERIOD);
            loopDetect[p].timestamp.sec  = 0;
            loopDetect[p].timestamp.nsec = 0;
            loopDetect[p].looped         = FALSE;
            
            if (loopDetect[p].blocked)
            {
               /* enable port */
               cmsLog_debug("un-block port %d after looped cleared", p);
               bcm_port_traffic_control_set(0, bcm_enet_map_oam_idx_to_phys_port(p), 0x0);
               loopDetect[p].blocked = FALSE;
            }
         }
      }
   }
}  /* End of loopDetectUpdate() */

CmsRet loopDetectTx(void)
{
   CmsRet ret = CMSRET_SUCCESS;
	struct sockaddr_ll sll;
   SINT32 p;
   
   for (p = 0; p < NUM_ETH_PORT; p++)
   {
      if (loopDetect[p].detectMgmt == OamCtcActionEnable &&
          !loopDetect[p].blocked)
      {
         /* transmit loop detect test frame out of the port */
         cmsLog_debug("Transmit loop detect frame out of port %d", p);   
         
         /* fill in source mac */
         memcpy(txbuf+ETH_ALEN, loopDetect[p].ifmac, ETH_ALEN);
         
         /* insert ifindex in the first byte of the OAM payload */
         txbuf[OAM_DATA_OFFSET] = loopDetect[p].ifindex;

	      /* set socket address parameters */
	      bzero(&sll, sizeof(sll));
         
	      sll.sll_family   = AF_PACKET;
	      sll.sll_protocol = htons(TK_CAP_PKT_TYPE);
	      sll.sll_halen    = ETH_ALEN;
	      sll.sll_ifindex  = loopDetect[p].ifindex;
	      sll.sll_pkttype  = PACKET_OTHERHOST;
         memcpy(sll.sll_addr, txbuf, ETH_ALEN);

	      if ((sendto(loopDetectFd, txbuf, LOOP_DETECT_FRAME_LEN, 0,
                     (struct sockaddr *)&sll, sizeof(sll))) < 0)
         {
            cmsLog_error("failed to send to RAW socket!");
            ret = CMSRET_INTERNAL_ERROR;
       
            /* close socket?? */
	      }
      }
   }
   
	return ret;
   
}  /* End of loopDetectTx() */

CmsRet processCmsMsg(CmsMsgHeader *msg)
{
   CmsRet ret = CMSRET_SUCCESS;

   switch (msg->type)
   {
   case CMS_MSG_SET_LOG_LEVEL:
      cmsLog_debug("got CMS_MSG_SET_LOG_LEVEL, data=%d", msg->wordData);
      
      cmsLog_setLevel(msg->wordData);
      
      if ((ret = cmsMsg_sendReply(msgHandle, msg, CMSRET_SUCCESS)) != CMSRET_SUCCESS)
      {
         cmsLog_error("send response for msg 0x%x failed, ret=%d", msg->type, ret);
      }
      break;

   case CMS_MSG_SET_LOG_DESTINATION:
      cmsLog_debug("got CMS_MSG_SET_LOG_DESTINATION, data=%d", msg->wordData);
      
      cmsLog_setDestination(msg->wordData);
      
      if ((ret = cmsMsg_sendReply(msgHandle, msg, CMSRET_SUCCESS)) != CMSRET_SUCCESS)
      {
         cmsLog_error("send response for msg 0x%x failed, ret=%d", msg->type, ret);
      }
      break;

   case CMS_MSG_PORT_LOOP_DETECT:
   {
      SINT32 port = msg->wordData;
      UINT32 mgmt = *(UINT32 *)(msg+1);
      
      cmsLog_debug("got CMS_MSG_PORT_LOOP_DETECT, port %d, mgmt %d", port, mgmt);
      
      if ((ret = processPortLoopDetect(port, mgmt)) != CMSRET_SUCCESS)
      {
         cmsLog_error("processPortLoopDetect returns error, ret=%d", ret);
      }
      break;
   }      
   case CMS_MSG_PORT_DISABLE_LOOPED:
   {
      SINT32 port   = msg->wordData;
      UINT32 action = *(UINT32 *)(msg+1);
      
      cmsLog_debug("got CMS_MSG_PORT_DISABLE_LOOPED, port %d, action %d", port, action);
      
      if ((ret = processPortDisableLooped(port, action)) != CMSRET_SUCCESS)
      {
         cmsLog_error("processPortDisableLooped returns error, ret=%d", ret);
      }
      break;
   }      
         
   default:
      cmsLog_debug("ingore msg 0x%x", msg->type);
      break;
   }
   
   return ret;
   
}  /* End of processCmsMsg() */

CmsRet processPortLoopDetect(SINT32 port, UINT32 mgmt)
{
   CmsRet ret = CMSRET_SUCCESS;
   
   if (port < 0 || port >= NUM_ETH_PORT)
   {
      cmsLog_error("Invalid port number");
      ret = CMSRET_INVALID_ARGUMENTS;
   }
   if (mgmt != OamCtcActionDisable &&
       mgmt != OamCtcActionEnable)
   {
      cmsLog_error("Invalid management value"); 
      ret = CMSRET_INVALID_ARGUMENTS;
   }

   if (ret == CMSRET_SUCCESS)
   {
      if (loopDetect[port].detectMgmt != mgmt)
      {
         cmsLog_debug("port %d loop detect management changes from %d to %d",
                      port, loopDetect[port].detectMgmt, mgmt);
         loopDetect[port].detectMgmt = mgmt;
      
         /* reinitialize loop detect status regardless changing
          * from Activate to Deactivate or vice versa.
          */
         cmsLog_debug("init loop detect status");
         loopDetect[port].timestamp.sec  = 0;
         loopDetect[port].timestamp.nsec = 0;
         loopDetect[port].looped         = FALSE;
         
         if (loopDetect[port].blocked)
         {
            /* enable port */
            cmsLog_debug("un-block port %d", port);
            bcm_port_traffic_control_set(0, bcm_enet_map_oam_idx_to_phys_port(port), 0x0);
            loopDetect[port].blocked = FALSE;
         }
      }
   }
   
   return ret;
   
}  /* End of processPortLoopDetect() */

CmsRet processPortDisableLooped(SINT32 port, UINT32 action)
{
   CmsRet ret = CMSRET_SUCCESS;
   
   if (port < 0 || port >= NUM_ETH_PORT)
   {
      cmsLog_error("Invalid port number");
      ret = CMSRET_INVALID_ARGUMENTS;
   }
   if (action != OamCtcLoopedPortEnable &&
       action != OamCtcLoopedPortDisable)
   {
      cmsLog_error("Invalid action value"); 
      ret = CMSRET_INVALID_ARGUMENTS;
   }

   if (ret == CMSRET_SUCCESS)
   {
      if (loopDetect[port].loopedAction != action)
      {
         cmsLog_debug("port %d looped action changes from %d to %d",
                      port, loopDetect[port].loopedAction, action);
         loopDetect[port].loopedAction = action;

         /* do nothing if currently port loop is not detected */            
         if (loopDetect[port].detectMgmt == OamCtcActionEnable &&
             loopDetect[port].looped)
         {
            if (action == OamCtcLoopedPortDisable &&
                !loopDetect[port].blocked)
            {
               /* disable port */
               cmsLog_debug("block looped port %d", port);
               bcm_port_traffic_control_set(0, bcm_enet_map_oam_idx_to_phys_port(port), PORT_RXDISABLE | PORT_TXDISABLE);
               loopDetect[port].blocked = TRUE;
            }
            else if (action == OamCtcLoopedPortEnable)
            {
               /* automatically re-enable port since action changes from
                * OamCtcLoopedPortDisable to OamCtcLoopedPortEnable ???
                */
               cmsLog_debug("un-block looped port %d", port);
               bcm_port_traffic_control_set(0, bcm_enet_map_oam_idx_to_phys_port(port), 0x0);
               loopDetect[port].blocked = FALSE;
            }
         }
      }
   }

   return ret;
      
}  /* End of processPortDisableLooped() */

void dumpFrame(void *buf, UINT32 len)
{
   UINT32 i;
   UINT8  *p = (UINT8 *)buf;

   if (cmsLog_getLevel() < LOG_LEVEL_NOTICE)
      return;

   for (i = 0; i < len; i++)
   {
      printf("%02X ", *(p+i));
      if ((i+1)%8==0)
         printf(" ");
      if ((i+1)%16==0)
         printf("\n");
   }
   printf("\n\n");
           
}  /* End of dumpFrame() */	
