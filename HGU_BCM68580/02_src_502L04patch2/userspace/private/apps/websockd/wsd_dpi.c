/***********************************************************************
 *
 *  Copyright (c) 2014  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2014:proprietary:standard 

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

#ifdef SUPPORT_WEB_SOCKETS
#ifdef SUPPORT_DPI

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "websockets/libwebsockets.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

#include "dpictl_api.h"
#include "wsd_main.h"

#define TMP_BUFFER_SIZE   4096

static int sprintf_realloc(char **buf, int *pos, int *len, const char *fmt, ...)
{
   int ret;
   static char tmp[TMP_BUFFER_SIZE];
   va_list args;

   va_start(args, fmt);

   ret = vsnprintf(tmp, sizeof(tmp), fmt, args);
   if (ret < 0) {
      printf("%s: error during vsnprintf: %d\n", __func__, ret);
      goto out;
   }

   /* realloc data if required */
   if (*pos + ret >= *len) {
      char *t = *buf;

      *len = *len * 2;
      *buf = realloc(*buf, *len);
      if (!*buf) {
         printf("%s: unable to realloc data to size %d\n", __func__, *len);
         free(t);
         ret = -1;
         goto out;
      }
   }

   memcpy(&(*buf)[*pos], tmp, ret + 1);
   *pos = *pos + ret;

out:
   va_end(args);
   return ret;
}

static void append_response(struct wsd_dpi_state *state, char *data)
{
   struct wsd_dpi_response *el = state->responses;
   struct wsd_dpi_response *response;

   response = malloc(sizeof(*response));
   if(!response) {
      cmsLog_error("%s: couldn't allocate response", __func__);
      return;
   }
   memset(response, 0, sizeof(*response));

   /* Because libsockets, we have to make pre & post padding, so copy our
    * response into the newly padded buffer */
   response->size = strlen(data);
   response->data = malloc(response->size + LWS_SEND_BUFFER_PRE_PADDING + LWS_SEND_BUFFER_POST_PADDING);
   if(!response->data) {
      cmsLog_error("%s: unable to allocate buffer for sending to websocket", __func__);
      free(response);
      return;
   }
   memcpy(&response->data[LWS_SEND_BUFFER_PRE_PADDING], data, response->size);
   response->pos   = LWS_SEND_BUFFER_PRE_PADDING;
   response->size += LWS_SEND_BUFFER_PRE_PADDING;

   /* Append the response to to end of the linked list */
   if(!state->responses) {
      state->responses = response;
      goto done;
   }
   while(el->next)
      el = el->next;
   el->next = response;

done:
   lws_callback_on_writable(state->wsi);
}

static void dpi_handle_sessionkey(struct wsd_dpi_state *state, char *args)
{
   if(!args) {
      cmsLog_error("key command with no session key provided");
      return;
   }

   state->session.sessionKey = atoi(args);
}

static void dpi_get_app_names(struct wsd_dpi_state *state, char *args)
{
   struct dc_table *table = dpictl_get_app_db();
   struct dc_id_name_map *entry;
   char *data;
   int len = TMP_BUFFER_SIZE;
   int pos = 0;
   int ret;
   int i;

   if (!table)
      return;
   entry = (struct dc_id_name_map *) table->data;

   data = malloc(len);
   if (!data) {
      printf("%s: unable to allocate response data\n", __func__);
      goto out_free_table;
   }

   pos += sprintf(data, "[\"appnames\",{");

   for (i = 0; i < table->entries; i++) {
      ret = sprintf_realloc(&data, &pos, &len, "%s\"%u\":\"%s\"",
                            (i ? "," : ""),
                            entry[i].id, entry[i].name);
      if (ret < 0)
         goto out_free_data;
   }

   ret = sprintf_realloc(&data, &pos, &len, "}]");
   if (ret < 0)
      goto out_free_data;

   /* send response to client */
   append_response(state, data);

out_free_data:
   free(data);
out_free_table:
   for (i = 0; i < table->entries; i++)
      free(entry[i].name);
   free(table);
}

static void dpi_get_device_names(struct wsd_dpi_state *state, char *args)
{
   struct dc_table *table = dpictl_get_dev_db();
   struct dc_id_name_map *entry;
   char *data;
   int len = TMP_BUFFER_SIZE;
   int pos = 0;
   int ret;
   int i;

   if (!table)
      return;
   entry = (struct dc_id_name_map *) table->data;

   data = malloc(len);
   if (!data) {
      printf("%s: unable to allocate response data\n", __func__);
      goto out_free_table;
   }

   pos += sprintf(data, "[\"devicenames\",{");

   for (i = 0; i < table->entries; i++) {
      ret = sprintf_realloc(&data, &pos, &len, "%s\"%u\":\"%s\"",
                            (i ? "," : ""),
                            entry[i].id, entry[i].name);
      if (ret < 0)
         goto out_free_data;
   }

   ret = sprintf_realloc(&data, &pos, &len, "}]");
   if (ret < 0)
      goto out_free_data;

   /* send response to client */
   append_response(state, data);

out_free_data:
   free(data);
out_free_table:
   for (i = 0; i < table->entries; i++)
      free(entry[i].name);
   free(table);
}

static void dpi_get_flows(struct wsd_dpi_state *state, char *args)
{
   struct dc_table *flows, *qos;
   struct dc_flow *flow;
   struct dc_qos_appinst_stat *appinst;
   char *data;
   int len = TMP_BUFFER_SIZE;
   int pos = 0;
   int ret;
   int i, j;

   /* update qos and flows info */
   dpictl_update_flows();
   dpictl_update_qos();

   flows = dpictl_get_flows();
   if (!flows)
      return;
   flow = (struct dc_flow *) flows->data;

   qos = dpictl_get_qos_appinsts(0);
   if (!qos)
      goto out_free_flows;
   appinst = (struct dc_qos_appinst_stat *) qos->data;

   data = malloc(len);
   if (!data) {
      printf("%s: unable to allocate response data\n", __func__);
      goto out_free_qos;
   }

   pos += sprintf(data, "[\"flows\",[");

   for (i = 0; i < flows->entries; i++) {
      for (j = 0; j < qos->entries; j++) {
         if (flow[i].app.app_id == appinst[j].appinst.app.app_id &&
             memcmp(flow[i].dev.mac, appinst[j].appinst.dev.mac, 6) == 0)
            break;
      }
      ret = sprintf_realloc(&data, &pos, &len,
                            "%s{\"a\":%d,\"m\":\"%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\","
                               "\"d\":%u,"
                               "\"up\":%lu,\"ub\":%llu,\"ut\":\"%s\","
                               "\"dp\":%lu,\"db\":%llu,\"dt\":\"%s\","
                               "\"q\":%d}",
                            (i ? "," : ""),
                            flow[i].app.app_id,
                            flow[i].dev.mac[0], flow[i].dev.mac[1],
                            flow[i].dev.mac[2], flow[i].dev.mac[3],
                            flow[i].dev.mac[4], flow[i].dev.mac[5],
                            flow[i].dev.dev_id,
                            flow[i].us.pkts,
                            flow[i].us.bytes,
                            flow[i].us.tuple,
                            flow[i].ds.pkts,
                            flow[i].ds.bytes,
                            flow[i].ds.tuple,
                            j < qos->entries ? appinst[j].appinst.ds_queue : -1);

      if (ret < 0)
         goto out_free_data;
   }

   ret = sprintf_realloc(&data, &pos, &len, "]]");
   if (ret < 0)
      goto out_free_data;

   /* send response to client */
   append_response(state, data);

out_free_data:
   free(data);
out_free_qos:
   free(qos);
out_free_flows:
   free(flows);
}

static void dpi_get_queues(struct wsd_dpi_state *state, char *args)
{
   struct dc_table *table;
   struct dc_qos_info *info;
   struct dc_qos_queue *entry;
   char *data;
   int len = TMP_BUFFER_SIZE;
   int pos = 0;
   int ret, i;

   /* update qos info and queues */
   dpictl_update_qos_queues();

   info = dpictl_get_qos_info();
   if (!info)
      return;
   table = dpictl_get_qos_queues();
   if (!table)
      goto out_free_info;
   entry = (struct dc_qos_queue *) table->data;

   data = malloc(len);
   if (!data) {
      printf("%s: unable to allocate response data\n", __func__);
      goto out_free_table;
   }

   pos += sprintf(&data[pos],
                  "[\"queues\",{\"num\":%d,\"default\":%d,"
                  "\"bypass\":%d,\"overall_bw\":%u,\"queues\":{",
                  info->ds.num_queues,
                  info->ds.default_queue,
                  info->ds.bypass_queue,
                  info->ds.overall_bw * 1000);

   for (i = 0; i < table->entries; i++) {
      ret = sprintf_realloc(&data, &pos, &len,
                            "%s\"%d\":{\"valid\":%d,\"min\":%u,\"max\":%u}",
                            (i ? "," : ""),
                            entry[i].id,
                            entry[i].valid,
                            entry[i].min_kbps * 1000,
                            entry[i].max_kbps * 1000);
      if (ret < 0)
         goto err_free_data;
   }

   ret = sprintf_realloc(&data, &pos, &len, "}}]");
   if (ret < 0)
      goto err_free_data;

   /* send response to client */
   append_response(state, data);

err_free_data:
   free(data);
out_free_table:
   free(table);
out_free_info:
   free(info);
}

int wsd_callback_dpi(struct lws *wsi, enum lws_callback_reasons reason,
                     void *user, void *in, size_t len)
{
   struct wsd_dpi_state *state = (struct wsd_dpi_state*) user;
   int ret = 0;

   switch(reason)
   {
      case LWS_CALLBACK_ESTABLISHED:
         lwsl_info("%s: LWS_CALLBACK_ESTABLISHED\n", __func__);
         state->session.sessionKey = 0;
         /* save the wsi pointer in our state */
         state->wsi = wsi;
         break;

      case LWS_CALLBACK_CLOSED:
      {
         struct wsd_dpi_response *resp = state->responses;
         while(resp) {
            struct wsd_dpi_response *cur = resp;
            resp = resp->next;
            free(cur);
         }
      }
      break;

      case LWS_CALLBACK_SERVER_WRITEABLE:
      {
         struct wsd_dpi_response *resp = state->responses;
         if(!resp)
            return 0;

         /* write as much as we can */
         ret = lws_write(wsi, &resp->data[resp->pos],
                         resp->size - resp->pos,
                         resp->send_count ? LWS_WRITE_CONTINUATION : LWS_WRITE_TEXT);
         if(ret < 0)
            cmsLog_error("error writing to websocket: %d", ret);

         resp->pos += ret;
         resp->send_count++;
         /* move on to the next response if we are finished */
         if(resp->pos >= resp->size || ret < 0)
         {
            state->responses = resp->next;
            free(resp->data);
            free(resp);
         }

         lws_callback_on_writable(wsi);
      }
      break;

      case LWS_CALLBACK_RECEIVE:
      {
         char *cmd = NULL;
         char *args = NULL;

         cmd = strtok((char*)in, " ");
         if(cmd)
            args = strtok(NULL, " ");
         if(!state->session.sessionKey && strcmp(cmd, "key") != 0) {
            cmsLog_error("command '%s' received without session key", cmd);
            break;
         }

         /* parse the command */
         if(strcmp(cmd, "key") == 0)
            dpi_handle_sessionkey(state, args);
         else if(strcmp(cmd, "appnames") == 0)
            dpi_get_app_names(state, args);
         else if(strcmp(cmd, "devicenames") == 0)
            dpi_get_device_names(state, args);
         else if(strcmp(cmd, "flows") == 0)
            dpi_get_flows(state, args);
         else if(strcmp(cmd, "queues") == 0)
            dpi_get_queues(state, args);
         else
            cmsLog_error("received invalid command '%s'", cmd);
      }
      break;

      default:
         break;
   }

   return ret < 0 ? ret : 0;
}


#endif   // SUPPORT_DPI
#endif   // SUPPORT_WEB_SOCKETS

