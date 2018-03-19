/******************************************************************************
 *
 * Copyright (c) 2009   Broadcom Corporation
 * All Rights Reserved
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
/***************************************************************************
 *
 *    Copyright (c) 2008-2009, Broadcom Corporation
 *    All Rights Reserved
 *    Confidential Property of Broadcom Corporation
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * Description: Non-generated libmoca functions
 *
 ***************************************************************************/

#include "moca_os.h"

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <mocalib.h>
#include "mocaint.h"

#if defined(__EMU_HOST_20__)
#define MOCA_RELEASE_VERSION "2.0.0"
#else
#define MOCA_RELEASE_VERSION RELEASE_VERSION
#endif

#if defined(STANDALONE)
#include "scratch.h"
#include "moca.h"
#include "nvram.h"
#include "printf.h"
#define NCLIENTS 6 // cli (mocap), IP stack and web server (x2), EPORT_Reinit, tr69
struct moca_ctx g_mocactx[NCLIENTS];
extern unsigned int moca_initialized;
#endif

MOCALIB_CLI void *moca_open(char *ifname)
{
   struct moca_ctx *ctx = NULL;
   MoCAOS_ClientHandle fd;

#if defined(STANDALONE)
   static int first = 1;
   int i;

   if (first)
   {
      memset(g_mocactx, 0, sizeof(g_mocactx));
      first = 0;
   }
   for (i=0;i<NCLIENTS;i++)
      if (g_mocactx[i].valid == 0)
      {
         ctx = &g_mocactx[i];
         memset(ctx, 0, sizeof(*ctx));
         break;
      }

   // In standalone mode, make sure moca rtos is initialized
   // before we go any further.
   while (!moca_initialized) vTaskDelay(5);
#else
   ctx = (struct moca_ctx *)calloc(sizeof(*ctx), 1);
#endif
   if(! ctx)
      return(NULL);
   
   ctx->valid = 1;
   ctx->useMocadHook = 0;
   ctx->lock = MUTEX_INIT();
   
   if(ifname) {
      if(strlen(ifname) >= MoCAOS_IFNAMSIZE)
      {
         MUTEX_FREE(ctx->lock);
         free(ctx);
         return(NULL);
      }
      strncpy(ctx->ifname, ifname, MoCAOS_IFNAMSIZE-1);
   } else {
      strncpy(ctx->ifname, MOCA_DEFAULT_IFNAME, MoCAOS_IFNAMSIZE-1);
   }

   ctx->mocad_rx_fd = MoCAOS_CLIENT_NULL;
   fd = MoCAOS_ConnectToMocad((MoCAOS_Handle) NULL, MOCA_CMD_SOCK_FMT, ctx->ifname);

   if (fd == MoCAOS_CLIENT_NULL) {
      MUTEX_FREE(ctx->lock);
#if !defined(STANDALONE)
      free(ctx);
#else
      ctx->valid = 0;
#endif
      return(NULL);
   }

   ctx->connect_fd = fd;
   return(ctx);
}

MOCALIB_CLI char *moca_get_buf(void *vctx, int *size)
{
   struct moca_ctx *ctx = (struct moca_ctx *) vctx;

   if (size)
      *size = sizeof(ctx->buf);
   
   return (char *)ctx->buf;
}

MOCALIB_CLI char *moca_get_evtbuf(void *vctx, int *size)
{
   struct moca_ctx *ctx = (struct moca_ctx *) vctx;

   if (size)
      *size = sizeof(ctx->evt_buf);
   
   return (char *)ctx->evt_buf;
}

MOCALIB_CLI void moca_get_version(uint32_t *moca, uint32_t *major, uint32_t *minor, uint32_t *patch)
{
   char c[12] = MOCA_RELEASE_VERSION; // ensure this is on the stack, not in flash
   char *d = NULL;

   *moca = 9999;
   *major = 9999;
   *minor = 9999;
   *patch = 0;

   *moca = strtoul(c, &d, 10);

   if (!d || *d != '.')
      return;

   *major = strtoul(d+1, &d, 10);

   if (!d || *d != '.')
      return;

   *minor = strtoul(d+1, &d, 10);

   if (!d || *d != '.')
      return;

   *patch = strtoul(d+1, &d, 10);   
}

MOCALIB_CLI void moca_close(void *vctx)
{
   struct moca_ctx *ctx = (struct moca_ctx *) vctx;

   if (vctx == NULL)
      return;

   MUTEX_LOCK(ctx->lock);
   
   ctx->valid = 0;
   
   MoCAOS_CloseClient((MoCAOS_Handle)NULL, ctx->connect_fd);
   MoCAOS_CloseClient((MoCAOS_Handle)NULL, ctx->mocad_rx_fd);

   MUTEX_UNLOCK(ctx->lock);
   MUTEX_FREE(ctx->lock);
#if !defined(STANDALONE)
   free(ctx);
#endif
}

MOCALIB_CLI unsigned long moca_phy_rate(unsigned long nbas, unsigned long cp, unsigned long turbo, unsigned long moca_version)
{
   unsigned int bw;
   unsigned int f1;
   unsigned int f2;

   if (turbo)
      bw = 2000000;
   else
      bw = 1000000;

   if (moca_version == MoCA_VERSION_2_0)
   {
      f1 = 1950;
      f2 = 46;
   }
   else
   {
      f1 = 46;
      f2 = 1;
   }
   return((unsigned long)((nbas * (unsigned long long)bw * f1) / (f2 * (256 + cp))));
}

void __moca_copy_be32(void *out, const void *in, int size)
{
   uintptr_t tin = (uintptr_t)in;
   uintptr_t tout = (uintptr_t)out;
   for(; size > 0; size -= 4, tin += 4, tout += 4) {
      
      uint32_t *dst = (uint32_t *)tout;
      *dst = BE32(*((uint32_t *)tin));
   }
}

MOCALIB_CLI int moca_req(struct moca_ctx *ctx, int wr_len)
{
   int ret;
   int len;
   if(ctx->connect_fd == MoCAOS_CLIENT_NULL) {
      MoCAOS_ClientHandle fd;

      fd = MoCAOS_ConnectToMocad((MoCAOS_Handle) NULL, MOCA_CMD_SOCK_FMT, ctx->ifname);
      if(fd == MoCAOS_CLIENT_NULL)
         return(MOCA_API_CANT_CONNECT);
      ctx->connect_fd = fd;
   }

   if (MoCAOS_SendMMP((MoCAOS_Handle) NULL, (MoCAOS_ClientHandle) ctx->connect_fd, 
         (const unsigned char *)ctx->buf, wr_len) != 0)
      return(MOCA_API_SEND_FAILED);

   while(1) {
      len = sizeof(ctx->buf);
      ret = MoCAOS_ReadMMP((MoCAOS_Handle)NULL, (MoCAOS_ClientHandle)ctx->connect_fd, 
         MoCALIB_TIMEOUT, ctx->buf, &len);
      
      if(ret <= 0) {
         /* try to reconnect later */
         MoCAOS_CloseClient((MoCAOS_Handle)NULL, ctx->connect_fd);
         ctx->connect_fd = MoCAOS_CLIENT_NULL;
         return(MOCA_API_READ_FAILED);
      }
      return(len);
   }
}

int (*__mocad_cmd_hook)(void *vctx, uint32_t msg_type, uint32_t ie_type,
   const void *wr, int wr_len, void *rd, int rd_len, int flags) = NULL;

MOCALIB_CLI static int moca_cmd(struct moca_ctx *ctx, uint32_t msg_type, uint32_t ie_type,
   const void *wr, int wr_len, void *rd, int rd_len, int flags)
{
   struct mmp_msg_hdr *mh;
   int ret;
   /* allow mocad to use library calls internally */
   if(ctx->useMocadHook && (__mocad_cmd_hook != NULL))
      return(__mocad_cmd_hook(ctx, msg_type, ie_type, wr, wr_len,
         rd, rd_len, flags));

   MUTEX_LOCK(ctx->lock);
   mh = (struct mmp_msg_hdr *)ctx->buf;
   mh->type = BE32(msg_type);
   mh->length = BE32(wr_len + MOCA_EXTRA_MMP_HDR_LEN);
   mh->ie_type = BE32(ie_type);

   if(flags & FL_SWAP_WR)
      __moca_copy_be32((void *)(mh + 1), wr, wr_len);
   else
      memcpy((void *)(mh + 1), wr, wr_len);

   ret = moca_req(ctx, wr_len + sizeof(*mh));
   if(ret < 0)
      goto out;

   mh = (struct mmp_msg_hdr *)ctx->buf;

   /* sanity checks on reply */

   if(ret <= (int)(sizeof(*mh))) {
      ret = MOCA_API_SHORT_READ;
      goto out;
   }

   ret -= sizeof(*mh);

   if(ret > rd_len) {
      ret = MOCA_API_LONG_READ;
      goto out;
   }

   if((msg_type | MOCA_MMP_MSG_ACK_BIT) != BE32(mh->type)) {
      ret = MOCA_API_INVALID_ACK;
      goto out;
   }

   if(rd && ret) {
      if(flags & FL_SWAP_RD)
         __moca_copy_be32(rd, (void *)(mh + 1), ret);
      else
         memcpy(rd, (void *)(mh + 1), ret);
   }
out:
   MUTEX_UNLOCK(ctx->lock);
   return(ret);
}

MOCALIB_CLI static int moca_get_raw(void *vctx, uint32_t ie_type,
   const void *in, int in_len, void *out, int out_len, int flags)
{
   int ret;

   ret = moca_cmd((struct moca_ctx *)vctx, MOCA_MSG_GET, ie_type,
      in, in_len, out, out_len, flags);
   if(ret < 0)
      return(ret);
   else
      return(MOCA_API_SUCCESS);
}

static inline int moca_get_inout(void *vctx, uint32_t ie_type, 
   const void *in, int in_len, void *out, int out_len)
{
   return(moca_get_raw(vctx, ie_type, in, in_len, out, out_len,
      FL_SWAP_WR | FL_SWAP_RD));
}

static inline int moca_get(void *vctx, 
   uint32_t ie_type, void *out, int out_len)
{
   return(moca_get_raw(vctx, ie_type, NULL, 0, out, out_len,
      FL_SWAP_WR | FL_SWAP_RD));
}

static inline int moca_get_inout_noswap(void *vctx,
   uint32_t ie_type, const void *in, int in_len, void *out, int out_len)
{
   return(moca_get_raw(vctx, ie_type, in, in_len, out, out_len, FL_SWAP_WR));
}

static inline int moca_get_noswap(void *vctx,
   uint32_t ie_type, void *out, int out_len)
{
   return(moca_get_raw(vctx, ie_type, NULL, 0, out, out_len, 0));
}

int (*__mocad_table_cmd_hook)(void * vctx, uint32_t ie_type, uint32_t * in,
   void * out, int struct_len, int max_out_len, int flags) = NULL;

MOCALIB_CLI static int moca_get_table(
   void * vctx, uint32_t ie_type, uint32_t * in,
   void * out, int struct_len, int max_out_len, int flags)
{
   uint32_t i = 0;
   int      out_len_left = max_out_len;
   void *   wr = out;
   int      ret = MOCA_API_SUCCESS;
   int      req_len = 0;
   struct mmp_msg_hdr *mh;
   struct moca_ctx * ctx = (struct moca_ctx *)vctx;
   uint32_t * wr_msg;

   /* allow mocad to use library calls internally */
   if(ctx->useMocadHook && (__mocad_table_cmd_hook != NULL))
      return(__mocad_table_cmd_hook(vctx, ie_type, in, out, 
         struct_len, max_out_len, flags));

   MUTEX_LOCK(ctx->lock);

   while (1)
   {
      mh = (struct mmp_msg_hdr *)ctx->buf;
      mh->type = BE32(MOCA_MSG_GET);
      mh->length = BE32(sizeof(i) + MOCA_EXTRA_MMP_HDR_LEN);
      mh->ie_type = BE32(ie_type);

      wr_msg = (uint32_t *)(mh + 1);
      __moca_copy_be32((void *)wr_msg, &i, sizeof(i));

      req_len = sizeof(i) + sizeof(*mh);

      if (in != NULL)
      {
         wr_msg++;
         __moca_copy_be32((void *)wr_msg, in, sizeof(*in));
         req_len += sizeof(*in);
         mh->length = BE32(sizeof(i) + sizeof(*in) + MOCA_EXTRA_MMP_HDR_LEN);
      }

      ret = moca_req(ctx, req_len);
      if(ret < 0)
         goto out;

      mh = (struct mmp_msg_hdr *)ctx->buf;

      /* sanity checks on reply */
      if(ret < (int)(sizeof(*mh))) {
         ret = MOCA_API_SHORT_READ;
         goto out;
      }

      ret -= sizeof(*mh);

      if(ret > out_len_left) {
         ret = MOCA_API_LONG_READ;
         goto out;
      }

      if((MOCA_MSG_GET | MOCA_MMP_MSG_ACK_BIT) != BE32(mh->type)) {
         ret = MOCA_API_INVALID_ACK;
         goto out;
      }

      if ((ret % struct_len) != 0) {
         ret = MOCA_API_INVALID_SIZE;
         goto out;
      }

      if(out && ret) {
         if (flags & FL_SWAP_RD)
            __moca_copy_be32(wr, (void *)(mh + 1), ret);
         else
            memcpy(wr, (void *)(mh + 1), ret);
      }
      i += (ret / struct_len);
      out_len_left -= ret;
      wr = (void *)((uint8_t *)wr + ret);

      if (ret < (int)(MOCA_MAX_CORE_RSP - sizeof(*mh) - struct_len))
      {
         /* We have retrieved all of the data */
         ret = i;
         goto out;
      }
   }
out:
   MUTEX_UNLOCK(ctx->lock);
   return(ret);
}

MOCALIB_CLI static int moca_set_raw(void *vctx, uint32_t ie_type,
   const void *in, int in_len, int flags)
{
   int ret;
   uint32_t ack = 0;

   ret = moca_cmd((struct moca_ctx *)vctx, MOCA_MSG_SET, ie_type,
      in, in_len, &ack, sizeof(ack), flags | FL_SWAP_RD);
   if(ret < 0)
      return(ret);
   return(ack);
}

static inline int moca_set_noswap(void *vctx,
   uint32_t ie_type, const void *in, int in_len)
{
   return(moca_set_raw(vctx, ie_type, in, in_len, 0));
}

static inline int moca_set(void *vctx,
   uint32_t ie_type, const void *in, int in_len)
{
   return(moca_set_raw(vctx, ie_type, in, in_len, FL_SWAP_WR));
}

static int __mocalib_dispatch_event(void *vctx, uint32_t ie_type, void *vin,
   int in_len);

MOCALIB_CLI static int moca_handle_event(struct moca_ctx *ctx, int len)
{
   struct mmp_msg_hdr *mh;
   char *msg;

   mh = (struct mmp_msg_hdr *)ctx->evt_buf;

   while(len > 0) {
      int msg_len;

      if(BE32(mh->type) != MOCA_MSG_TRAP)
         return(MOCA_API_INVALID_TYPE);

      msg_len = BE32(mh->length) - MOCA_EXTRA_MMP_HDR_LEN;
      msg = (char *)(mh + 1);

      /* Check the message length in case it got corrupted for some reason */
      if ( (msg_len < 0) ||
           (((uintptr_t)msg + msg_len) > 
             (uintptr_t)(ctx->evt_buf + sizeof(ctx->evt_buf))) )
      {
         return(MOCA_API_INVALID_SIZE);
      }

      len -= sizeof(struct mmp_msg_hdr);

      if(len < msg_len)
         return(MOCA_API_SHORT_READ);

      __mocalib_dispatch_event(ctx, BE32(mh->ie_type), msg, msg_len);
   
      /* several messages can be aggregated into one read */
      mh = (struct mmp_msg_hdr *)((uintptr_t)msg + msg_len);

      len -= msg_len;
   }
   return(MOCA_API_SUCCESS);
}

MOCALIB_CLI int moca_init_evt(void *vctx)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   int ret = MOCA_API_SUCCESS;
   MoCAOS_ClientHandle fd = 0;

   if (ctx->mocad_rx_fd == MoCAOS_CLIENT_NULL)
   {
      fd = MoCAOS_ConnectToMocad((MoCAOS_Handle) NULL, MOCA_EVT_SOCK_FMT, ctx->ifname);

      if(fd == MoCAOS_CLIENT_NULL)
         ret = MOCA_API_CANT_CONNECT;
      else
         ctx->mocad_rx_fd = fd;
   }

   return(ret);
}

MOCALIB_CLI void moca_close_evt(void *vctx)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;

   if (ctx->mocad_rx_fd != MoCAOS_CLIENT_NULL) 
   {
      MoCAOS_CloseClient((MoCAOS_Handle)NULL, ctx->mocad_rx_fd);
      ctx->mocad_rx_fd = MoCAOS_CLIENT_NULL;
   }
}

MOCALIB_CLI int moca_event_loop(void *vctx)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   int ret = MOCA_API_ERROR;
   char finish[9] = "DEADDEAD"; // this is on the stack to prevent it from being placed in flash in standalone
   MoCAOS_ClientHandle fd = MoCAOS_CLIENT_NULL;
   int len;

   if (ctx->mocad_rx_fd == MoCAOS_CLIENT_NULL)
   {
      fd = MoCAOS_ConnectToMocad((MoCAOS_Handle) NULL, MOCA_EVT_SOCK_FMT, ctx->ifname);
      if(fd == MoCAOS_CLIENT_NULL)
         goto out1;
      
      ctx->mocad_rx_fd = fd;
   }

   while(1) {
      len = sizeof(ctx->evt_buf);
      ret = MoCAOS_ReadMMP((MoCAOS_Handle) NULL, (MoCAOS_ClientHandle) ctx->mocad_rx_fd, 
         MoCAOS_TIMEOUT_INFINITE, ctx->evt_buf, &len);
      if(ret <= 0) {
         break;
      }

      if (strncmp((char *)ctx->evt_buf, finish, sizeof(ctx->evt_buf)) == 0)
         break;
      
      moca_handle_event(ctx, len);
   }

   ret = MOCA_API_SUCCESS;

   if (fd != MoCAOS_CLIENT_NULL)
   {
      MUTEX_LOCK(ctx->lock);
      MoCAOS_CloseClient((MoCAOS_Handle) NULL, fd);
      MUTEX_UNLOCK(ctx->lock);
   }
out1:
   return(ret);
}


MOCALIB_CLI int moca_event_wait_timeout(void *vctx, uint32_t timeout_sec)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   int ret = MOCA_API_ERROR;
   char finish[9] = "DEADDEAD"; // this is on the stack to prevent it from being placed in flash in standalone
   MoCAOS_ClientHandle fd = MoCAOS_CLIENT_NULL;
   int len;

   if (ctx->mocad_rx_fd == MoCAOS_CLIENT_NULL)
   {
      fd = MoCAOS_ConnectToMocad((MoCAOS_Handle) NULL, MOCA_EVT_SOCK_FMT, ctx->ifname);
      if(fd == MoCAOS_CLIENT_NULL)
         goto out1;
      
      ctx->mocad_rx_fd = fd;
   }

   len = sizeof(ctx->evt_buf);
   ret = MoCAOS_ReadMMP((MoCAOS_Handle) NULL, (MoCAOS_ClientHandle) ctx->mocad_rx_fd, 
         timeout_sec, ctx->evt_buf, &len);
   if(ret <= 0) {
      goto out1;
   }

   if (strncmp((char *)ctx->evt_buf, finish, sizeof(ctx->evt_buf)) == 0)
      goto out1;

   moca_handle_event(ctx, len);

   ret = MOCA_API_SUCCESS;

out1:
   return(ret);
}


MOCALIB_CLI int moca_start_event_loop(void * ctx)
{
   MoCAOS_ThreadHandle thread;
   thread = MoCAOS_CreateThread((MoCAOS_ThreadEntry)moca_event_loop, ctx);

   if (thread == MOCAOS_INVALID_THREAD) {
      return(MOCA_API_INVALID_THREAD);
   }

   /* Give the thread a chance to run */
   MoCAOS_MSleep(1);

   return(MOCA_API_SUCCESS);
}

MOCALIB_CLI int moca_wait_for_raw_event(void *vctx, uint32_t timeout_s)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   int ret = MOCA_API_SUCCESS;
   int len;

   len = sizeof(ctx->evt_buf);

   ret = MoCAOS_ReadMMP((MoCAOS_Handle) NULL, ctx->mocad_rx_fd, 
       timeout_s, ctx->evt_buf, &len);

   if (ret < 0)
      return(ret);
   else if (ret == 0)
      return MOCA_API_TIMEOUT;
   else
      return(len);
}

MOCALIB_CLI int moca_wait_for_event(
   void * vctx, uint32_t timeout_s, uint32_t * test)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   int ret = MOCA_API_SUCCESS;
   int len;
   unsigned int time1, time2;

   time2 = time1 = MoCAOS_GetTimeSec();

   do
   {
      len = sizeof(ctx->evt_buf);

      ret = MoCAOS_ReadMMP((MoCAOS_Handle) NULL, ctx->mocad_rx_fd, 
         (timeout_s - (time2 - time1)), ctx->evt_buf, &len);

      if(ret <= 0) {
         break;
      }

      ret = moca_handle_event(ctx, len);

      if (*test) {
         break;
      }

      time2 = MoCAOS_GetTimeSec();

   } while((time2 - time1) < timeout_s);

   if (*test == 0) {
      ret = MOCA_API_TIMEOUT;
   }

   return(ret);
}


MOCALIB_CLI void moca_cancel_event_loop(void *vctx)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   char c[] = "DEADDEAD";
   
   MoCAOS_SendMMP((MoCAOS_Handle) NULL, (MoCAOS_ClientHandle) ctx->mocad_rx_fd, 
         (unsigned char *)c, 9);

   /* Give the thread a chance to end */
   MoCAOS_MSleep(1);
}


void moca_mocad_printf_cb(void *userarg, struct moca_mocad_printf_out *out)
{
   printf("%s",out->msg);
}

#include "mocalib-gen.c"

MOCALIB_CLI const char *moca_l2_error_name(uint32_t l2_error)
{
   switch(l2_error) {
      case MOCA_L2_SUCCESS:
         return("Success");
      case MOCA_L2_TRANSACTION_FAILED:
         return("Transaction failed");
      case MOCA_L2_L2ME_NO_SUPPORT:
         return("L2ME no support");
      case MOCA_L2_PQOS_INGR_NOT_FOUND:
         return("Ingress node not found");
      case MOCA_L2_PQOS_EGR_NOT_FOUND:
         return("Egress node not found");
      case MOCA_L2_TRANSACTION_CANT_START:
         return("Transaction can't start");
   }
   return(NULL);
}

static int __mocalib_dispatch_event(void *vctx, uint32_t ie_type, void *vin,
   int in_len)
{
   /* declared inline so that it doesn't get built into mocad */
   return(mocalib_dispatch_event(vctx, ie_type, vin, in_len));
}

MOCALIB_CLI const char * moca_decision_string(uint32_t value)
{
   static char buf[11];
   
   switch (value)
   {
      case MOCA_PQOS_DECISION_SUCCESS:
         return "DECISION_SUCCESS";
         break;
      case MOCA_PQOS_DECISION_FLOW_EXISTS:
         return "DECISION_FLOW_EXISTS";
         break;
      case MOCA_PQOS_DECISION_INSUF_INGR_BW:
         return "DECISION_INSUF_INGR_BW";
         break;
      case MOCA_PQOS_DECISION_INSUF_EGR_BW: 
         return "DECISION_INSUF_EGR_BW"; 
         break;
      case MOCA_PQOS_DECISION_TOO_MANY_FLOWS: 
         return "DECISION_TOO_MANY_FLOWS"; 
         break;
      case MOCA_PQOS_DECISION_INVALID_TSPEC: 
         return "DECISION_INVALID_TSPEC"; 
         break;
      case MOCA_PQOS_DECISION_INVALID_DA: 
         return "DECISION_INVALID_DA"; 
         break;
      case MOCA_PQOS_DECISION_LEASE_EXPIRED: 
         return "DECISION_LEASE_EXPIRED"; 
         break;
      case MOCA_PQOS_DECISION_INVALID_BURST_SIZE:
         return "DECISION_INVALID_BURST_SIZE";
         break;
      case MOCA_PQOS_DECISION_UNUSABLE_LINK:
         return "DECISION_UNUSABLE_LINK";
         break;
      case MOCA_PQOS_DECISION_INGRESS_TOO_MANY_MCAST_FLOWS:
         return "DECISION_INGRESS_TOO_MANY_MCAST_FLOWS";
         break;
      case MOCA_PQOS_DECISION_INGRESS_DUPLICATE_FLOW:
         return "DECISION_INGRESS_DUPLICATE_FLOW";
         break;
      case MOCA_PQOS_DECISION_INGRESS_M1_NOTOK:
         return "DECISION_INGRESS_M1_NOTOK";
         break;
      case MOCA_PQOS_DECISION_FLOW_NOT_FOUND: 
         return "DECISION_FLOW_NOT_FOUND"; 
         break;
      case MOCA_PQOS_DECISION_INSUF_AGGR_STPS: 
         return "DECISION_INSUF_AGGR_STPS"; 
         break;
      case MOCA_PQOS_DECISION_INSUF_AGGR_TXPS: 
         return "DECISION_INSUF_AGGR_TXPS"; 
         break; 
      case MOCA_PQOS_DECISION_RETRY_NOTOK: 
         return "DECISION_RETRY_NOTOK";
         break;

      default:
#if defined(__gnu_linux__)
         snprintf(buf, sizeof(buf), "0x%x", value);
#else
         sprintf(buf, "0x%x", (unsigned int) value);
#endif         
         return buf;
         break;
   }
}

MOCALIB_CLI uint32_t moca_count_bits(uint32_t val)
{
   uint32_t count = 0;

   while (val)
   {
      if (val & 0x1)
         count++;

      val >>= 1;
   }

   return(count);
}

MOCALIB_CLI void mocacli_to_bits(char *str, uint32_t * out, uint32_t size)
{
  uint32_t i, j;
  uint32_t start = 0;
  uint32_t digit = 0;
  uint32_t count =0;
  char *tmpstr = str;
   
  *str = '\0';
  for (i = 0; i < size; i++)
  {
     start = i * 32;
     for (j = 0; j < 32; j++)
     {
        if (out[i] &(1 << j))
        {
           digit = (31 - j) + start;
           count++;
           str += sprintf(str, "%d,", (int)digit);
        } 
     }
   }
   if (count == (size * 32))
   {
      str = tmpstr;
      sprintf(str, "0-%d ", (int)(count - 1));
   }
 
}

int moca_set_persistent(void *ctx)
{
#ifdef DSL_MOCA
   return(moca_set_message(ctx, MOCA_UPDATE_FLASH_INIT));
#elif defined(STANDALONE)
   char *str = SCRATCH_GetBuffer();
   unsigned int x;
   memset(str, 0, MAX_MOCA_SETTINGS);
   moca_write_nvram(ctx, str, MAX_MOCA_SETTINGS);
   x=NVRAM_Lock();
   NVRAM_SetItem(NVRAM_MOCA_SETTINGS, (unsigned int *)str, strlen(str)+1);
   NVRAM_Unlock(x);
   SCRATCH_FreeBuffer();
   return(0);
#else
   char cfgstr[20480];
   char *str = cfgstr;
   
   strcpy(str, "mocacfg set  -c \" ");
   moca_write_nvram(ctx, str + strlen(str), (uint32_t)(sizeof(cfgstr) - strlen(str)));
   strcpy(str + strlen(str), " \" ");
   return(system(cfgstr));
#endif
}
