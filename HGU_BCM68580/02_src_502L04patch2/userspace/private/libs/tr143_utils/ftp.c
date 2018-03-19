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

FILE *sfp;
static int sfd, dfd;


static int ftpcmd(char *s1, char *s2, FILE *fp, char *buf, int bufLen)
{
   fd_set fdset;
   struct timeval tm;
   int ret;

   cmsLog_debug("ftpcmd:buflen=%d", bufLen);

   if (s1) 
   {
      if (!s2) s2="";
      fprintf(fp, "%s%s\r\n", s1, s2);
      fflush(fp);
   }

   FD_ZERO(&fdset);
   FD_SET(sfd, &fdset);
   tm.tv_sec = TR143_SESSION_TIMEOUT;
   tm.tv_usec = 0;

   ret = select(sfd + 1, &fdset, NULL, NULL, &tm);
   if ( ret <= 0 || !FD_ISSET(sfd, &fdset)) 
   {
       cmsLog_error("select time out");
       return -1;
   }

   do 	
   {
      if (fgets(buf, bufLen - 2, fp) == NULL) 
      {
         cmsLog_error("fgets error");
         return -1;
      }

      cmsLog_debug("fgets:%s", buf);
   } while (!isdigit(buf[0]) || buf[3] != ' ');

   return atoi(buf);
}

int ftpUploadDiag (void)
{
   struct sockaddr_in s_in;
   char *s, buf[16384];
   char *filename;
   int n, errCode = Completed;
   int port;
   unsigned int txbytes;

   memset(&s_in, 0, sizeof(struct sockaddr_in));
   s_in.sin_family = AF_INET;
   inet_pton(AF_INET, server_addr, &s_in.sin_addr);
   s_in.sin_port=htons(21);	
   cmsTms_getXSIDateTimeMicroseconds(0, diagstats.TCPOpenRequestTime, sizeof(diagstats.TCPOpenRequestTime));
   sfd = open_conn_socket(&s_in);
   cmsTms_getXSIDateTimeMicroseconds(0, diagstats.TCPOpenResponseTime, sizeof(diagstats.TCPOpenResponseTime));
   if (sfd <= 0 || ((sfp = fdopen(sfd, "r+")) == NULL)) 
   {
      cmsLog_error("open control socket error");
      return Error_InitConnectionFailed;
   }

   if (ftpcmd(NULL, NULL, sfp, buf, sizeof(buf)) != 220)
   {
      cmsLog_error("server ready error");
      errCode = Error_InitConnectionFailed;
      goto errOut2;
   }

   switch(ftpcmd("USER ", "anonymous", sfp, buf, sizeof(buf))) {
   case 230:
      break;
   case 331:
      if (ftpcmd("PASS ", "anonymous", sfp, buf, sizeof(buf)) == 230) break;
      cmsLog_error("auth-pass error");
      errCode = Error_LoginFailed;
      goto errOut2;
   default:
      cmsLog_error("auth-user error");
      errCode = Error_PasswordRequestFailed;
      goto errOut2;
   }

   if (ftpcmd("TYPE I", NULL, sfp, buf, sizeof(buf)) != 200)
   {
      cmsLog_error("transfer mode error");
      errCode = Error_NoTransferMode;
      goto errOut2;
   }


   if (ftpcmd("PASV", NULL, sfp, buf, sizeof(buf)) !=  227)
   {
      cmsLog_error("PASV error");
      errCode = Error_NoPASV;
      goto errOut2;
   }
   s = strrchr(buf, ',');
   *s = 0;
   port = atoi(s+1);
   s = strrchr(buf, ',');
   port += atoi(s+1) * 256;
   memset(&s_in, 0, sizeof(struct sockaddr_in));
   s_in.sin_family = AF_INET;
   inet_pton(AF_INET, server_addr, &s_in.sin_addr);
   s_in.sin_port = htons(port);
   dfd = open_conn_socket(&s_in);
   if (dfd <= 0) 
   {
      cmsLog_error("open data socket error");
      errCode = Error_NoResponse;
      goto errOut2;
   }

   if ((filename = strrchr(uri, '/')) != NULL)
   {
      *filename = '\0';
      filename++;
      if (uri[0] != '\0' && ftpcmd("CWD ", uri, sfp, buf, sizeof(buf)) != 250) 
      {
         cmsLog_error("CWD to %s error", uri);
         errCode = Error_NoCWD;
         goto errOut1;
      }
   }
   else
      filename = uri;

   cmsTms_getXSIDateTimeMicroseconds(0, diagstats.ROMTime, sizeof(diagstats.ROMTime));
   if (ftpcmd("STOR ", filename, sfp, buf, sizeof(buf)) > 150)
   {
      cmsLog_error("STOR error");
      errCode = Error_NoSTOR;
      goto errOut1;
   }

   memset(buf, 'a', sizeof(buf));

   cmsLog_notice("start transmision");
   
   set_sockopt_nocopy(dfd);
   
   get_if_stats(NULL, &txbytes);
 
   cmsTms_getXSIDateTimeMicroseconds(0, diagstats.BOMTime, sizeof(diagstats.BOMTime));
   
   while (testFileLength > 0) 
   {
      //cmsLog_debug("store file size:%d\n", testFileLength);
      n =write(dfd, buf, (testFileLength > sizeof(buf))?sizeof(buf):testFileLength);
      if (n<=0) 
      {
         cmsLog_error("file write error, %d", n);
         errCode = Error_TransferFailed;
         goto errOut1;
      }
      testFileLength -= n;
   }		

   cmsTms_getXSIDateTimeMicroseconds(0, diagstats.EOMTime, sizeof(diagstats.EOMTime));

   cmsLog_notice("end transmision\n");
   close(dfd);

   get_if_stats(NULL, &diagstats.TotalBytesSent);
   diagstats.TotalBytesSent -= txbytes;


   if (ftpcmd(NULL, NULL, sfp, buf, sizeof(buf)) != 226)
   {
      cmsLog_error("read file send ok error");
      errCode = Error_TransferFailed;
   }

   ftpcmd("QUIT", NULL, sfp, buf, sizeof(buf));
   goto errOut2;

errOut1:
   close(dfd);
errOut2:
   close(sfd);
   fclose(sfp);
   cmsLog_debug("return errCode:%d", errCode);
   return errCode;	
}

int ftpDownloadDiag (void)
{
   struct sockaddr_in s_in;
   char *s, *filename, buf[16384];
   unsigned long filesize;
   int n, errCode = Completed;
   int port;
   int firstRead = 1;
   unsigned int rxbytes;

   if (server_port == 0) server_port = 21;
   filename =  (uri[0] == '/') ? &uri[1] : uri;

   memset(&s_in, 0, sizeof(struct sockaddr_in));
   s_in.sin_family = AF_INET;
   inet_pton(AF_INET, server_addr, &s_in.sin_addr);
   s_in.sin_port=htons(server_port);	
   cmsTms_getXSIDateTimeMicroseconds(0, diagstats.TCPOpenRequestTime, sizeof(diagstats.TCPOpenRequestTime));
   sfd = open_conn_socket(&s_in);
   cmsTms_getXSIDateTimeMicroseconds(0, diagstats.TCPOpenResponseTime, sizeof(diagstats.TCPOpenResponseTime));
   if (sfd <= 0 || ((sfp = fdopen(sfd, "r+")) == NULL)) 
   {
      cmsLog_error("open control socket error");
      return Error_InitConnectionFailed;
   }

   if (ftpcmd(NULL, NULL, sfp, buf, sizeof(buf)) != 220)
   {
      cmsLog_error("server ready error");
      errCode = Error_InitConnectionFailed;
      goto errOut2;
   }

   switch(ftpcmd("USER ", "anonymous", sfp, buf, sizeof(buf))) {
   case 230:
      break;
   case 331:
      if (ftpcmd("PASS ", "anonymous", sfp, buf, sizeof(buf)) == 230) break;
      cmsLog_error("auth-pass error");
      errCode = Error_LoginFailed;
      goto errOut2;
   default:
      cmsLog_error("auth-user error");
      errCode = Error_PasswordRequestFailed;
      goto errOut2;
   }

   if (ftpcmd("TYPE I", NULL, sfp, buf, sizeof(buf)) != 200)
   {
      cmsLog_error("transfer mode error");
      errCode = Error_NoTransferMode;
      goto errOut2;
   }


   if (ftpcmd("PASV", NULL, sfp, buf, sizeof(buf)) !=  227)
   {
      cmsLog_error("PASV error");
      errCode = Error_NoPASV;
      goto errOut2;
   }
   s = strrchr(buf, ',');
   *s = 0;
   port = atoi(s+1);
   s = strrchr(buf, ',');
   port += atoi(s+1) * 256;
   memset(&s_in, 0, sizeof(struct sockaddr_in));
   s_in.sin_family = AF_INET;
   inet_pton(AF_INET, server_addr, &s_in.sin_addr);
   s_in.sin_port = htons(port);
   dfd = open_conn_socket(&s_in);
   if (dfd <= 0) 
   {
      cmsLog_error("open data socket error");
      errCode = Error_NoResponse;
      goto errOut2;
   }

   if ((ftpcmd("SIZE ", filename, sfp, buf, sizeof(buf)) != 213) || (safe_strtoul(buf+4, &filesize)))
   {
      cmsLog_error("SIZE error");
      errCode = Error_IncorrectSize;
      goto errOut1;
   }

   get_if_stats(&rxbytes, NULL);
   cmsTms_getXSIDateTimeMicroseconds(0, diagstats.ROMTime, sizeof(diagstats.ROMTime));

   if (ftpcmd("RETR ", filename, sfp, buf, sizeof(buf)) > 150)
   {
      cmsLog_error("RETR error");
      errCode = Error_Timeout;
      goto errOut1;
   }

   cmsLog_notice("RETR sent");

   set_sockopt_nocopy(dfd);

   cmsLog_notice("start data transfer");

   while (filesize > 0) 
   {
      //cmsLog_debug("retrieve file size:%ld\n", filesize);
      n =read(dfd, buf, (filesize > sizeof(buf))?sizeof(buf):filesize);
      if (n<=0) 
      {
         cmsLog_error("file read error, %d", n);
         errCode = n<0 ? Error_Timeout : Error_TransferFailed;
         goto errOut1;
      }

      if (firstRead) 
      {
         cmsTms_getXSIDateTimeMicroseconds(0, diagstats.BOMTime, sizeof(diagstats.BOMTime));
         firstRead = 0;
      }

      diagstats.TestBytesReceived += n;
      filesize -= n;
   }		

   cmsTms_getXSIDateTimeMicroseconds(0, diagstats.EOMTime, sizeof(diagstats.EOMTime));

   cmsLog_notice("end data transfer");

   get_if_stats(&diagstats.TotalBytesReceived, NULL);
   diagstats.TotalBytesReceived -= rxbytes;

   close(dfd);

   if (ftpcmd(NULL, NULL, sfp, buf, sizeof(buf)) != 226)
   {
      cmsLog_error("read file send ok error");
   }

   ftpcmd("QUIT", NULL, sfp, buf, sizeof(buf));
   goto errOut2;

errOut1:
   close(dfd);
errOut2:
   close(sfd);
   fclose(sfp);
   cmsLog_debug("return errCode:%d", errCode);
   return errCode;	
}

