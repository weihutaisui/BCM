/***********************************************************************
 *
 *  Copyright (c) 2012-2013  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2013:proprietary:standard

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

#include "tr143_defs.h"
#include "tr143_private.h"

static int sfd;
tProtoCtx *pc;

/*
return:
1) actual size,  if content-length is not specified
2) content-length, if content-length is > 0
3) -1, if timeout or socket close
*/
static int http_readLengthMsg(tProtoCtx *pc, int readLth, int doFlushStream) 
{
   int bufCnt = 0, readCnt = 0;
   int bufLth = readLth;
   char buf[16384];

   set_sockopt_nocopy(pc->fd);

   cmsLog_notice("Payload read started");

   while (readLth <= 0 || bufCnt < readLth)
   {
      if ((readCnt = proto_Readn(pc, buf, (bufLth > sizeof(buf) || readLth <= 0 ) ? sizeof(buf) : bufLth)) > 0)
      {
          //cmsLog_debug("readCnt: %d\n",readCnt);
          bufCnt += readCnt;
          bufLth -= readCnt;
      }
      else
      {
         cmsLog_error("proto_Readn timeout");
         break;
      }
   }

   cmsLog_notice("Payload read ended");
   cmsLog_debug("buf readLth=%d, actual read=%d ", readLth, bufCnt);
   
   if(readCnt <= 0 && readLth > 0)
   {
      cmsLog_error("http_readLengthMsg error");
      return -1;
   }

   if (doFlushStream) proto_Skip(pc);         

   return bufCnt;
}

static int http_readChunkedMsg(tProtoCtx *pc) 
{
   char chunkedBuf[512];   
   int  chunkedSz = 0, readSz = 0, totSz = 0;

   while (1)
   {
      do
      {
         chunkedBuf[0] = '\0';
         readSz = proto_Readline(pc, chunkedBuf, sizeof(chunkedBuf));
         if (readSz <= 0) 
         {
            cmsLog_error("read chunked size error");
            return -1;
         }
      }
      while (readSz > 0 && isxdigit(chunkedBuf[0]) == 0);

      totSz += readSz;
      sscanf(chunkedBuf, "%x", &chunkedSz);

      if (chunkedSz <= 0) break;
      if ((readSz = http_readLengthMsg(pc, chunkedSz, FALSE)) < 0)
      {
         cmsLog_error("http_readLengthMsg error, chunked size = %d, readSz = %d", chunkedSz, readSz);
         return -1;
      }

      totSz += readSz;
   }      

   proto_Skip(pc);         
   return totSz;
}

static int send_http_get()
{
   proto_SendRequest(pc, "GET", uri);
   proto_SendHeader(pc,  "Host", server_addr);
   proto_SendHeader(pc,  "User-Agent", TR143_AGENT_NAME);
   proto_SendHeader(pc,  "Connection", "keep-alive");
   proto_SendRaw(pc, "\r\n", 2);

   return 0;
}  /* End of send_get_request() */


static int send_http_put()
{
   char buf[16384];
   int sndCnt;

   proto_SendRequest(pc, "PUT", uri);
   proto_SendHeader(pc,  "Host", server_addr);
   proto_SendHeader(pc,  "User-Agent", TR143_AGENT_NAME);
   proto_SendHeader(pc,  "Connection", "keep-alive");
   proto_SendHeader(pc,  "Content-Type", "text/xml");
   sprintf(buf, "%d", testFileLength);
   proto_SendHeader(pc,  "Content-Length", buf);
   proto_SendRaw(pc, "\r\n", 2);
   memset(buf, 'a', sizeof(buf));

   set_sockopt_nocopy(pc->fd);

   cmsLog_notice("start send");
   
   get_if_stats(NULL, &diagstats.TotalBytesSent);
   cmsTms_getXSIDateTimeMicroseconds(0, diagstats.BOMTime, sizeof(diagstats.BOMTime));	

   for (sndCnt = testFileLength; sndCnt > 0; sndCnt -= sizeof(buf))
       proto_SendRaw(pc, buf, sndCnt > sizeof(buf) ? sizeof(buf) : sndCnt);

   cmsLog_notice("end send");
   return 0;
}  /* End of send_get_request() */


static int http_GetData()
{
   tHttpHdrs *hdrs;
   int errCode = Completed;

   if ((hdrs = proto_NewHttpHdrs()) == NULL)
   {
      cmsLog_error("http hdr alloc error");
      return Error_TransferFailed;
   }

   if ((diagstats.TestBytesReceived = proto_ParseResponse(pc, hdrs)) < 0) 
   {
      cmsLog_error("error: illegal http response or read failure");
      errCode = Error_TransferFailed;
      goto errOut1;
   }

   diagstats.TestBytesReceived += proto_ParseHdrs(pc, hdrs);

   if (hdrs->status_code != 200 ) 
   {
      cmsLog_error("http reponse %d", hdrs->status_code);
      if (hdrs->status_code == 401)
         errCode = Error_LoginFailed;
      else
         errCode = Error_TransferFailed;
      goto errOut1;
   }

   if (hdrs->TransferEncoding && !strcasecmp(hdrs->TransferEncoding,"chunked"))
   {
      if ((diagstats.TestBytesReceived += http_readChunkedMsg(pc)) < 0)
      {
         cmsLog_error("calling http_readChunkedMsg error");
         errCode = Error_TransferFailed;
      }
   }
   else if (hdrs->content_length > 0)
   {
      if ((diagstats.TestBytesReceived += http_readLengthMsg(pc, hdrs->content_length, FALSE)) < 0)
      {
         cmsLog_error("calling readLengthMsg error, content_length=%d", hdrs->content_length);
         errCode = Error_TransferFailed;
      }
   }	

errOut1:
   proto_FreeHttpHdrs(hdrs);
   return errCode;
}

int httpDownloadDiag (void)
{
   struct sockaddr_in s_in;
   int errCode = Completed;
   unsigned int rxbytes;

   if (server_port == 0) server_port = 80;

   memset(&s_in, 0, sizeof(struct sockaddr_in));
   s_in.sin_family = AF_INET;
   inet_pton(AF_INET, server_addr, &s_in.sin_addr);
   s_in.sin_port=htons(server_port);	
   cmsTms_getXSIDateTimeMicroseconds(0, diagstats.TCPOpenRequestTime, sizeof(diagstats.TCPOpenRequestTime));
   sfd = open_conn_socket(&s_in);
   cmsTms_getXSIDateTimeMicroseconds(0, diagstats.TCPOpenResponseTime, sizeof(diagstats.TCPOpenResponseTime));
   if (sfd <= 0) 
   {
      cmsLog_error("open socket error");
      errCode = Error_InitConnectionFailed;
      goto errOut1;
   }
   cmsLog_notice("====>connected to server");

   if ((pc = proto_NewCtx(sfd)) == NULL)
   {
      cmsLog_error("proto_NewCtx error");
      errCode = Error_InitConnectionFailed;
      goto errOut1;
   }

   cmsTms_getXSIDateTimeMicroseconds(0, diagstats.ROMTime, sizeof(diagstats.ROMTime));	

   if (send_http_get() < 0)
   {
      cmsLog_error("send get request error");
      errCode = Error_InitConnectionFailed;
      goto errOut2;
   }
   cmsLog_notice("====>http get sent");

   get_if_stats(&rxbytes, NULL);
   if (select_with_timeout(sfd, 1))
   {
      cmsLog_error("http get reponse error");
      errCode = Error_NoResponse;
      goto errOut2;
   }

   cmsTms_getXSIDateTimeMicroseconds(0, diagstats.BOMTime, sizeof(diagstats.BOMTime));	

   errCode = http_GetData();

   get_if_stats(&diagstats.TotalBytesReceived, NULL);
   diagstats.TotalBytesReceived -= rxbytes;
   cmsTms_getXSIDateTimeMicroseconds(0, diagstats.EOMTime, sizeof(diagstats.EOMTime));	

errOut2:
   proto_FreeCtx(pc);
errOut1:
   close(sfd);
   cmsLog_debug("return:%d", errCode);
   return errCode;	
}



int httpUploadDiag (void)
{
   struct sockaddr_in s_in;
   int errCode = Completed;
   tHttpHdrs *hdrs;
   unsigned int txbytes;

   if (server_port == 0) server_port = 80;

   memset(&s_in, 0, sizeof(struct sockaddr_in));
   s_in.sin_family = AF_INET;
   inet_pton(AF_INET, server_addr, &s_in.sin_addr);
   s_in.sin_port=htons(server_port);	
   cmsTms_getXSIDateTimeMicroseconds(0, diagstats.TCPOpenRequestTime, sizeof(diagstats.TCPOpenRequestTime));
   sfd = open_conn_socket(&s_in);
   cmsTms_getXSIDateTimeMicroseconds(0, diagstats.TCPOpenResponseTime, sizeof(diagstats.TCPOpenResponseTime));
   if (sfd <= 0) 
   {
      cmsLog_error("open socket error");
      errCode = Error_InitConnectionFailed;
      goto errOut1;
   }
   cmsLog_notice("====>connected to server");

   if ((pc = proto_NewCtx(sfd)) == NULL)
   {
      cmsLog_error("proto_NewCtx error");
      errCode = Error_InitConnectionFailed;
      goto errOut1;
   }

   cmsTms_getXSIDateTimeMicroseconds(0, diagstats.ROMTime, sizeof(diagstats.ROMTime));
   if (send_http_put() < 0)
   {
      cmsLog_error("send get request error");
      errCode = Error_InitConnectionFailed;
      goto errOut2;
   }
   cmsLog_notice("====>http put sent");

   if (select_with_timeout(sfd, 1))
   {
      cmsLog_error("http get reponse error");
      errCode = Error_NoResponse;
      goto errOut2;
   }

   cmsTms_getXSIDateTimeMicroseconds(0, diagstats.EOMTime, sizeof(diagstats.EOMTime));
   get_if_stats(NULL, &txbytes);
   diagstats.TotalBytesSent = txbytes -diagstats.TotalBytesSent;

   if ((hdrs = proto_NewHttpHdrs()) == NULL)
   {
      cmsLog_error("http hdr alloc error");
      errCode = Error_TransferFailed;
      goto errOut2;
   }

   if (proto_ParseResponse(pc, hdrs) < 0) 
   {
      cmsLog_error("error: illegal http response or read failure");
      errCode = Error_TransferFailed;
      goto errOut3;
   }

   proto_ParseHdrs(pc, hdrs);

   if (hdrs->status_code != 100 &&  // Continue status might be returned by Microsoft-IIS/5.1
         hdrs->status_code != 201 &&   // Created status is returned by Microsoft-IIS/5.1
         hdrs->status_code != 204 &&   // No content status is returned by Apache/2.2.2
         hdrs->status_code != 200 ) 
   {
      cmsLog_error("http reponse %d", hdrs->status_code);
      if (hdrs->status_code == 401)
         errCode = Error_LoginFailed;
      else
         errCode = Error_TransferFailed;
   }	
   else 
   {
      errCode = Completed;
   }

errOut3:	
   proto_FreeHttpHdrs(hdrs);
errOut2:
   proto_FreeCtx(pc);
errOut1:
   close(sfd);
   cmsLog_debug("return:%d", errCode);
   return errCode;	
}

