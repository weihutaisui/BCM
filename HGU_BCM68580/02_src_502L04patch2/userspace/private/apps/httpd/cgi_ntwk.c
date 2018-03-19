/***********************************************************************
 *
 *  Copyright (c) 2000-2010  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2011:proprietary:standard

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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/syslog.h>
#include <net/if_arp.h>
#include <net/route.h>
#include "cgi_cmd.h"
#include "cgi_ntwk.h"
#include "cgi_main.h"
#include "syscall.h"
#include "sysdiag.h"
#include "httpd.h"
//mwang #include "bcmadsl.h"
#include "cgi_util.h"
#include "cms_dal.h"
#include "cms_seclog.h"
#include "cms_qdm.h"
#include "prctl.h"
#include "cms_image.h"
#include "pmd.h"

extern void cgiFormLayer3IfName(char *L3IfName);

#ifdef SUPPORT_TR69C_AUTONOMOUS_TRANSFER_COMPLETE
extern CmsImageTransferStats imageTransferStats;
#endif

void cgiArpAdd(char *query, FILE *fs) {
   int sts = 0, i = 0;
   char ipAddr[BUFLEN_40];
   char macAddr[BUFLEN_40];
   char cmd[WEB_BUF_SIZE_MAX];
   char msg[WEB_BUF_SIZE_MAX];
   struct arpreq req;
   struct sockaddr sa;
   struct sockaddr_in *addr = (struct sockaddr_in*)&sa;
   unsigned char *ptr = NULL, *mac = NULL, *mac1 = NULL;
   int sockfd = 0;

   ipAddr[0] = '\0'; macAddr[0] = '\0'; cmd[0] = '\0'; msg[0] = '\0';

   cgiGetValueByName(query, "arpAddr", ipAddr);
   cgiGetValueByName(query, "arpMac", macAddr);

   addr->sin_family = AF_INET;
   addr->sin_port = 0;
   if ( inet_aton(ipAddr, &addr->sin_addr) != 0 ) {
      memcpy((char *)&req.arp_pa, (char *)&sa, sizeof(struct sockaddr));
      bzero((char*)&sa,sizeof(struct sockaddr));
      ptr = (unsigned char *)sa.sa_data;
      mac = (unsigned char *)macAddr;
      for ( i = 0; i < 6; i++ ) {
         *ptr = (unsigned char)(strtol((char*)mac,(char**)&mac1,16));
         mac1++;
         ptr++;
         mac = mac1;
      }
      sa.sa_family = ARPHRD_ETHER;
      memcpy((char *)&req.arp_ha, (char *)&sa, sizeof(struct sockaddr));
      req.arp_flags = ATF_PERM;
      req.arp_dev[0] = '\0';

      if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
         strcpy(msg, "Error in create socket");
         sts = -1;
      } else {
         if (ioctl(sockfd, SIOCSARP, &req) < 0) {
            strcpy(msg, "Error in SIOCSARP");
            sts = -1;
         }
      }
      if ( sockfd > 0 )
         close(sockfd);
   } else {
      strcpy(msg, "Invalid IP address");
      sts = -1;
   }

   if ( sts != 0 ) {
      sprintf(cmd, "Configure ARP for %s failed. " \
                   "Status: %s.", ipAddr, msg);
      cgiWriteMessagePage(fs, "ARP Add Error", cmd, NULL);
   } else
      cgiArpView(query, fs);
}

void cgiArpRemove(char *query, FILE *fs) {
   int sts = 0;
   char ipAddr[BUFLEN_40];
   char cmd[WEB_BUF_SIZE_MAX];
   char msg[WEB_BUF_SIZE_MAX];
   struct arpreq req;
   struct sockaddr sa;
   struct sockaddr_in *addr = (struct sockaddr_in*)&sa;
   int sockfd = 0;

   ipAddr[0] = '\0'; cmd[0] = '\0'; msg[0] = '\0';

   cgiGetValueByName(query, "arpAddr", ipAddr);

   addr->sin_family = AF_INET;
   addr->sin_port = 0;
   if ( inet_aton(ipAddr, &addr->sin_addr) != 0 ) {
      memcpy((char *)&req.arp_pa, (char *)&sa, sizeof(struct sockaddr));
      bzero((char*)&sa, sizeof(struct sockaddr));
      sa.sa_family = ARPHRD_ETHER;
      memcpy((char *)&req.arp_ha, (char *)&sa, sizeof(struct sockaddr));
      req.arp_flags = ATF_PERM;
      req.arp_dev[0] = '\0';

      if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
         strcpy(msg, "Error in create socket");
         sts = -1;
      } else {
         if (ioctl(sockfd, SIOCDARP, &req) < 0) {
            strcpy(msg, "Error in SIOCDARP");
            sts = -1;
         }
      }
      if ( sockfd > 0 )
         close(sockfd);
   } else {
      strcpy(msg, "Invalid IP address");
      sts = -1;
   }

   if ( sts != 0 ) {
      sprintf(cmd, "Remove ARP for %s failed. " \
                   "Status: %s.", ipAddr, msg);
      cgiWriteMessagePage(fs, "ARP Remove Error", cmd, NULL);
   } else
      cgiArpView(query, fs);
}

void cgiArpView(char *query __attribute__((unused)), FILE *fs)
{
   char col[6][32];
   char line[512];
   int count = 0;

   writeNtwkHeader(fs);
   fprintf(fs, "<b>Device Info -- ARP</b><br><br>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
   // write headers
   fprintf(fs, "   <tr>\n");
   fprintf(fs, "      <td class='hd'>IP address</td>\n");
   fprintf(fs, "      <td class='hd'>Flags</td>\n");
   fprintf(fs, "      <td class='hd'>HW Address</td>\n");
   fprintf(fs, "      <td class='hd'>Device</td>\n");
   fprintf(fs, "   </tr>\n");
   // write body
   FILE* fsArp = fopen("/proc/net/arp", "r");
   if ( fsArp != NULL ) {
      while ( fgets(line, sizeof(line), fsArp) ) {
         // read pass header line
         if ( count++ < 1 ) continue;

         sscanf(line, "%s %s %s %s %s %s",
            col[0], col[1], col[2], col[3], col[4], col[5]);

         fprintf(fs, "   <tr>\n");
         fprintf(fs, "      <td>%s</td>\n", col[0]);

         if ( cmsUtl_strcmp(col[2], "0x0") == 0 )
            fprintf(fs, "      <td>Incomplete</td>\n");
         else if ( cmsUtl_strcmp(col[2], "0x6") == 0 )
            fprintf(fs, "      <td>Permanent</td>\n");
         else if ( cmsUtl_strcmp(col[2], "0xa") == 0 ||
                   cmsUtl_strcmp(col[2], "0xA") == 0 )
            fprintf(fs, "      <td>Publish</td>\n");
         else
            fprintf(fs, "      <td>Complete</td>\n");

         fprintf(fs, "      <td>%s</td>\n", col[3]);
         fprintf(fs, "      <td>%s</td>\n", col[5]);
         fprintf(fs, "   </tr>\n");
      }
      fclose(fsArp);
   }
   fprintf(fs, "</table><br>\n");
   fprintf(fs, "</form>\n");
   fprintf(fs, "</blockquote>\n");
   fprintf(fs, "</body>\n");
   fprintf(fs, "</html>\n");

   fflush(fs);
}

void writeNtwkHeader(FILE *fs) {
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "<title></title>\n</head>\n<body>\n<blockquote>\n<form>\n");
}

void cgiLogView(char *query __attribute__((unused)), FILE *fs) {
   char data[WEB_BUF_SIZE_MAX], line[WEB_BUF_SIZE_MAX];
   char severity[WEB_BUF_SIZE_MAX], levelDisplay[WEB_BUF_SIZE_MAX];
   int levelSev = 0, levelDsp = 0;
   int readPtr = BCM_SYSLOG_FIRST_READ;
   int entries=0;

   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "<title></title>\n");
   fprintf(fs, "</head>\n");
   fprintf(fs, "<center>\n<form>\n");
   fprintf(fs, "<b>System Log</b><br><br>\n");
   fprintf(fs, "<table border='1' cellpadding='1' cellspacing='0'>\n");
   // write headers
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd'>Date/Time</td>\n");
   fprintf(fs, "      <td class='hd'>Facility</td>\n");
   fprintf(fs, "      <td class='hd'>Severity</td>\n");
   fprintf(fs, "      <td class='hd'>Message</td>\n");
   fprintf(fs, "   </tr>\n");

   // get log display level
   cgiGetVar("logDisplay", levelDisplay, fs);
   levelDsp = atoi(levelDisplay);

   // write body
   while ((readPtr != BCM_SYSLOG_READ_BUFFER_ERROR) &&
           (readPtr != BCM_SYSLOG_READ_BUFFER_END)) {

    if ( entries++ > 500) {
        break;
    }
      readPtr = cmsLog_readPartial(readPtr, line);
      if ( readPtr == BCM_SYSLOG_READ_BUFFER_ERROR )
           break;

      // do not parse when line is empty
      if ( line[0] == '\0' ) continue;

      // get severity level of the log entry
      cgiGetLogData(line, severity, CGI_LOG_SEVERITY);
      levelSev = cgiGetLogSeverityLevel(severity);
      // do not display entry that has severity level higher than display level
      if ( levelSev > levelDsp ) continue;

      switch ( levelSev ) {
         case LOG_EMERG:
            fprintf(fs, "   <tr style='background-color: #ff3300'>\n");
            break;
         case LOG_ALERT:
            fprintf(fs, "   <tr style='background-color: #ff0066'>\n");
            break;
         case LOG_CRIT:
            fprintf(fs, "   <tr style='background-color: #ff6600'>\n");
            break;
         case LOG_ERR:
            fprintf(fs, "   <tr style='background-color: yellow'>\n");
            break;
         case LOG_WARNING:
            fprintf(fs, "   <tr style='background-color: #ffff66'>\n");
            break;
         case LOG_NOTICE:
            fprintf(fs, "   <tr>\n");
            break;
         case LOG_INFO:
            fprintf(fs, "   <tr>\n");
            break;
         case LOG_DEBUG:
            fprintf(fs, "   <tr style='background-color: silver'>\n");
            break;
         default:
            fprintf(fs, "   <tr style='background-color: gray'>\n");
            break;
      }

      cgiGetLogData(line, data, CGI_LOG_DATE_TIME);
      fprintf(fs, "      <td>%s</td>\n", data);
      cgiGetLogData(line, data, CGI_LOG_FACILITY);
      fprintf(fs, "      <td>%s</td>\n", data);
      fprintf(fs, "      <td>%s</td>\n", severity);
      cgiGetLogData(line, data, CGI_LOG_MESSAGE);
      fprintf(fs, "      <td>%s</td>\n", data);
      fprintf(fs, "   </tr>\n");
   }

   fprintf(fs, "</table>\n");
   fprintf(fs, "<p align='center'>\n");
   fprintf(fs, "<input type='button' onClick='location.href=\"logview.cmd\"' value='Refresh'>\n");
   fprintf(fs, "<input type='button' onClick='window.close()' value='Close'>\n");
   fprintf(fs, "</form>\n</center>\n</body>\n</html>\n");
   fflush(fs);
}

void cgiGetLogData(char *line, char *data, int field){
   char date[4], times[9];
   char *dot = NULL, *cp = NULL;
#if defined(DMP_TIME_1) || defined(DMP_DEVICE2_TIME_1) 
   char month[4];
#else
   static char months[12][4] =
      { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
   static int daysOfMonth[12] =
      { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
   int days = 0, i = 0, j = 0;
#endif

   if ( line == NULL ) return;
   data[0] = '\0';

   switch (field) {
      case CGI_LOG_DATE_TIME:
#if defined(DMP_TIME_1) || defined(DMP_DEVICE2_TIME_1) 
        strncpy(month,line,3);
        month[3] = '\0';
            strncpy(date,&line[4],2);
        date[2] = '\0';
        strncpy(times, &line[7],8);
            times[8] = '\0';
        sprintf(data, "%s %s %s", month,date,times);
#else
            // format of date/time as follow: "Jan  1 00:00:00"
            // need to convert to "1st day 00:00:00"
            strncpy(date, line, 3);
            date[3] = '\0';
            for ( i = 0; i < 12; i++ ) {
                if ( cmsUtl_strcmp(months[i], date) == 0 )
                   break;
            }
            if ( i < 12 ) {
                for ( j = 0; j < i; j++ )
                   days += daysOfMonth[j];
            }
            strncpy(date, &line[4], 2);
            date[2] = '\0';
            days += atoi(date);
            strncpy(times, &line[7], 8);
            times[8] = '\0';
            switch (days) {
               case 1:
                  sprintf(data, "%dst day %s", days, times);
                  break;
               case 2:
                  sprintf(data, "%dnd day %s", days, times);
                  break;
               case 3:
                  sprintf(data, "%drd day %s", days, times);
                  break;
               default:
                  sprintf(data, "%dth day %s", days, times);
                  break;
            }
#endif
         break;
      case CGI_LOG_FACILITY:
         dot = strchr(&line[16], '.');
         if ( dot != NULL ) {
            for ( cp = (dot - 1); cp != NULL && *cp !=  ' ' ; cp-- )
               ;
            if ( ++cp != NULL ) {
               strncpy(data, cp, dot - cp);
               data[dot - cp] = '\0';
            }
         }
         break;
      case CGI_LOG_SEVERITY:
         dot = strchr(&line[16], '.');
         if ( dot != NULL ) {
            for ( cp = (dot + 1); cp != NULL && *cp !=  ' ' ; cp++ )
               ;
            if ( cp != NULL ) {
               dot++;
               strncpy(data, dot, cp - dot);
               data[cp - dot] = '\0';
            }
         }
         break;
      case CGI_LOG_MESSAGE:
         dot = strchr(&line[16], '.');
         if ( dot != NULL ) {
            for ( cp = (dot + 1); cp != NULL && *cp !=  ' ' ; cp++ )
               ;
            if ( ++cp != NULL )
               strcpy(data, cp);
         }
         break;
      default:
         data[0] = '\0';
         break;
   }
}


int cgiGetLogSeverityLevel(char *severity) {
   int i = 0;
   char severityTbl[8][32] = {
      "emerg",
      "alert",
      "crit",
      "err",
      "warn",
      "notice",
      "info",
      "debug"
                             };

   for ( i = 0; i < 8; i++ )
      if ( strstr(severityTbl[i], severity)  != NULL )
         break;

   return i;
}

void cgiAdslCfgTestMode(char *query, FILE *fs) {
   char mode[BUFLEN_40];
   extern int glbAdslTestMode;

   cgiGetValueByName(query, "adslTestMode", mode);
   glbAdslTestMode = atoi(mode);

   switch ( glbAdslTestMode ) {
      case 4:
         prctl_runCommandInShellBlocking("adslctl connection --L3");
         break;
      case 3:
         prctl_runCommandInShellBlocking("adslctl connection --noretrain");
         break;
      case 2:
         prctl_runCommandInShellBlocking("adslctl connection --medley");
         break;
      case 1:
         prctl_runCommandInShellBlocking("adslctl connection --reverb");
         break;
      default:
         prctl_runCommandInShellBlocking("adslctl connection --up");
         break;
   }

   do_ej("/webs/adslcfgadv.html", fs);
}

void cgiAdslCfgTones(char *query, FILE *fs) {
   extern char glbAdslTestTones[257];
   unsigned char toneMap[32];
   char cmd[BUFLEN_256], map[BUFLEN_256];
   char xmtMap[BUFLEN_256], rcvMap[BUFLEN_256];
   int i = 0, j = 0, k = 0;

   cgiGetValueByName(query, "adslTestTones", glbAdslTestTones);

   for ( i = 0, k = 0; i < 256; i++, k++ ) {
      if ( (i != 0) && (i % 8) == 0 ) {
         j++;
         k = 0;
      }
      if ( glbAdslTestTones[i] == '1' )
         switch ( k ) {
         case 0:
            toneMap[j] |= 0x01;
            break;
         case 1:
            toneMap[j] |= 0x02;
            break;
         case 2:
            toneMap[j] |= 0x04;
            break;
         case 3:
            toneMap[j] |= 0x08;
            break;
         case 4:
            toneMap[j] |= 0x10;
            break;
         case 5:
            toneMap[j] |= 0x20;
            break;
         case 6:
            toneMap[j] |= 0x40;
            break;
         case 7:
            toneMap[j] |= 0x80;
            break;
         }
      else
         switch ( k ) {
         case 0:
            toneMap[j] &= 0xFE;
            break;
         case 1:
            toneMap[j] &= 0xFD;
            break;
         case 2:
            toneMap[j] &= 0xFB;
            break;
         case 3:
            toneMap[j] &= 0xF7;
            break;
         case 4:
            toneMap[j] &= 0xEF;
            break;
         case 5:
            toneMap[j] &= 0xDF;
            break;
         case 6:
            toneMap[j] &= 0xBF;
            break;
         case 7:
            toneMap[j] &= 0x7F;
            break;
         }
   }

   // make xmtMap
   strcpy(xmtMap, "0x");
   for ( i = 0; i < 4; i++ ) {
      sprintf(map, "%02x", toneMap[i]);
      strcat(xmtMap, map);
   }

   //make rcvMap
   strcpy(rcvMap, "0x");
   for ( i = 4; i < 32; i++ ) {
      sprintf(map, "%02x", toneMap[i]);
      strcat(rcvMap, map);
   }

   // execute "adslctl connection --tones" command
   sprintf(cmd, "adslctl connection --tones 0 32 %s 32 224 %s",
           xmtMap, rcvMap);
   prctl_runCommandInShellBlocking(cmd);

   do_ej("/webs/adslcfgtone.html", fs);
}

void cgiGetEnblFirewallForWeb(char *varValue)
{
   /* Currently, always return firewall enabled for IP filter. */
   
   UBOOL8 b = TRUE;

#ifdef notUsed
   if ((b = dalWan_isAnyFirewallEnabled()) == FALSE)
   {
      b = dalLan_isAnyFirewallEnabled();
   }
#endif /* notUsed */

   if (b)
      strcpy(varValue, "1");
   else
      strcpy(varValue, "0");
}

void cgiGetPppIpExtensionForWeb(char *varValue)
{
   if (dalWan_isPPPIpExtension())
      strcpy(varValue, "1");
   else
      strcpy(varValue, "0");
}


void cgiConfigDnsInterfacesList(char *filename __attribute__((unused)))
{

   cmsLog_debug("Enter. glbWebVar->defaultGatewayList=%s", glbWebVar.dnsIfcsList);

   dalRt_setDefaultGatewayList(glbWebVar.defaultGatewayList);

   glbSaveConfigNeeded = TRUE;
}


void cgiGetDnsInterfacesList(char *varValue)
{
   char savedDnsIfcsList[CMS_MAX_DNSIFNAME * CMS_IFNAME_LENGTH]={0};

   dalDns_getIpvxDnsIfNameList(CMS_AF_SELECT_IPV4, savedDnsIfcsList);

   /* If go back button is pressed and list is modified, need to use
   * the one in glbWebVar->dnsIfcsList
   */
   if (savedDnsIfcsList[0] != '\0')
   {
      if (cmsUtl_strcmp(glbWebVar.dnsIfcsList, savedDnsIfcsList))
      {
         strcpy(varValue, glbWebVar.dnsIfcsList);
      }
      else
      {
         strcpy(varValue, savedDnsIfcsList);
      }
   }
   else
   {
      strcpy(varValue, "&nbsp");
   }

}

void cgiGetDnsStaticIp(char *staticDns1, char *staticDns2, UBOOL8 isIPv4)
{
   char savedDnsStaticIps[CMS_MAX_ACTIVE_DNS_IP * CMS_IPADDR_LENGTH]={0};
   char dns1[CMS_IPADDR_LENGTH]={0};
   char dns2[CMS_IPADDR_LENGTH]={0};
   SINT32 dnsLen=0;

   if (staticDns1)
   {
      strcpy(staticDns1, "&nbsp");
   }
   if (staticDns2)
   {
      strcpy(staticDns2, "&nbsp");
   }

   qdmDns_getStaticIpvxDnsServersLocked(isIPv4 ? CMS_AF_SELECT_IPV4 : CMS_AF_SELECT_IPV6, savedDnsStaticIps);

   /* If go back button is pressed and list is modified, need to use
   * the one in glbWebVar->dnsIfcsList
   */
   if (savedDnsStaticIps[0] != '\0')
   {
      cmsUtl_parseDNS(savedDnsStaticIps, dns1, dns2, isIPv4);
         
      if (staticDns1 && dns1[0] != '\0')
      {
         dnsLen =  sizeof(dns1) - 1;
         strncpy(staticDns1, dns1, dnsLen);
         staticDns1[dnsLen] = '\0';
      }
      else if (staticDns2 && dns2[0] != '\0')
      {
        dnsLen =  sizeof(dns2) - 1;
        strncpy(staticDns2, dns2, dnsLen);
        staticDns2[dnsLen] = '\0';
      }
   }

}


void cgiGetActiveDnsIp(char *activeDns1, char *activeDns2)
{
   char savedDnsStaticIps[CMS_MAX_ACTIVE_DNS_IP * CMS_IPADDR_LENGTH]={0};
   char dns1[CMS_IPADDR_LENGTH]={0};
   char dns2[CMS_IPADDR_LENGTH]={0};
      
   if (activeDns1)
   {
     strcpy(activeDns1, "&nbsp");
   }
   else if (activeDns2)
   {
     strcpy(activeDns2, "&nbsp");
   }  

   if (qdmDns_getStaticIpvxDnsServersLocked(CMS_AF_SELECT_IPVX, savedDnsStaticIps))
   {
      if (savedDnsStaticIps[0] != '\0')
      {
         cmsUtl_parseDNS(savedDnsStaticIps, dns1, dns2, FALSE);

         if (dns1[0] == '\0' && dns2[0] == '\0')
         {
            cmsUtl_parseDNS(savedDnsStaticIps, dns1, dns2, TRUE);
         }
      }
   }
   else
   {
      qdmDns_getActiveIpvxDnsIpLocked(CMS_AF_SELECT_IPVX, dns1, dns2);
   }

   if (activeDns1 && dns1[0] != '\0')
   {
     /* hopefully caller has passed in a big enough buffer */
      strcpy(activeDns1, dns1);
   }
   else if (activeDns2 && dns2[0] != '\0')
   {
     strcpy(activeDns2, dns2);
   }

}


void cgiConfigDns(char *filename __attribute__((unused)))
{

#ifdef DMP_X_BROADCOM_COM_IPV6_1
   /* deprecated proprietary Broadcom IPv6 */
   cmsLog_debug("dns6Type=%s dns6Ifc=%s, dns6Pri=%s, dns6Sec=%s",
                glbWebVar.dns6Type, glbWebVar.dns6Ifc, glbWebVar.dns6Pri, glbWebVar.dns6Sec);

   cmsDal_setDns6Info_igd(glbWebVar.dns6Type, glbWebVar.dns6Ifc, glbWebVar.dns6Pri, glbWebVar.dns6Sec);
#endif

#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   /* Hybrid IPv6 using TR181 */
   if (!cmsUtl_strcmp(glbWebVar.dns6Type, MDMVS_DHCP))
   {
      /* current WebUI inputs only a single intf for IPv6 IfNameList */
      dalDns_deleteAllStaticIpvxDnsServers(CMS_AF_SELECT_IPV6);
      dalDns_setIpvxDnsIfNameList(CMS_AF_SELECT_IPV6, glbWebVar.dns6Ifc);
   }
   else if (!cmsUtl_strcmp(glbWebVar.dns6Type, MDMVS_STATIC))
   {
      dalDns_setStaticIpvxDnsServers(CMS_AF_SELECT_IPV6, glbWebVar.dns6Pri, glbWebVar.dns6Sec);
   }
   else
   {
      cmsLog_error("Invalid Ipv6 dns6Type %s", glbWebVar.dns6Type);
   }
#endif

   cmsLog_debug("Enter. glbWebVar->dnsIfcsList=%s, dns1=%s, dns2=%s",
                 glbWebVar.dnsIfcsList,
                 glbWebVar.dnsPrimary, glbWebVar.dnsSecondary);

   if (cmsUtl_strcmp(glbWebVar.dnsIfcsList, ""))
   {
      /*
       * If we want to use DnsIfNameList, we must delete any static DNS
       * servers because static DNS servers have higher precedence.
       */
      dalDns_deleteAllStaticIpvxDnsServers(CMS_AF_SELECT_IPV4);
      dalDns_setIpvxDnsIfNameList(CMS_AF_SELECT_IPV4, glbWebVar.dnsIfcsList);
   }
   else if (cmsUtl_strcmp(glbWebVar.dnsPrimary, "0.0.0.0")) /* primary has to be non "0.0.0.0" , but secondary can be "0.0.0.0" */
   {
      dalDns_setStaticIpvxDnsServers(CMS_AF_SELECT_IPV4, glbWebVar.dnsPrimary, glbWebVar.dnsSecondary);
   }
      
   glbSaveConfigNeeded = TRUE;
}


/** Send the config file out through this file handle.
 */
void cgiBackupSettings(char *query __attribute__((unused)), FILE *fs)
{
   UINT32 cfgBufLen;
   char *cfgBuf;
   CmsRet ret;
   CmsSecurityLogData logData = EMPTY_CMS_SECURITY_LOG_DATA;
   
   cfgBufLen = cmsImg_getConfigFlashSize();
   
   if ((cfgBuf = cmsMem_alloc(cfgBufLen, ALLOC_ZEROIZE)) == NULL)
   {
      cmsLog_error("could not allocate %u bytes", cfgBufLen);
      return;
   }

#ifdef SUPPORT_TR69C_AUTONOMOUS_TRANSFER_COMPLETE
    memset(&imageTransferStats,0,sizeof(CmsImageTransferStats));
    imageTransferStats.startTime = time(NULL);
    imageTransferStats.isDownload = FALSE;
#endif

   if ((ret = cmsMgm_readConfigFlashToBuf(cfgBuf, &cfgBufLen)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to read config flash into buf, ret=%d", ret);
   }
   else
   {
      UINT32 count;
      
      count = fwrite(cfgBuf, 1, cfgBufLen, fs);

#ifdef SUPPORT_TR69C_AUTONOMOUS_TRANSFER_COMPLETE
      imageTransferStats.fileSize = count;
      imageTransferStats.fileType = CMS_IMAGE_FORMAT_XML_CFG;
#endif

      if (count != cfgBufLen)
      {
#ifdef SUPPORT_TR69C_AUTONOMOUS_TRANSFER_COMPLETE
         imageTransferStats.faultCode = CMSRET_INTERNAL_ERROR;
         strcpy(imageTransferStats.faultStr,"Write of config file failed.");
#endif
         cmsLog_error("write of config file failed, count=%u cfgBufLen=%u", count, cfgBufLen);
      }
#ifdef SUPPORT_TR69C_AUTONOMOUS_TRANSFER_COMPLETE
      else
      {
         imageTransferStats.faultCode = CMSRET_SUCCESS;
      }
#endif
   }

#ifdef SUPPORT_TR69C_AUTONOMOUS_TRANSFER_COMPLETE
   imageTransferStats.completeTime = time(NULL);
   cmsImg_sendAutonomousTransferCompleteMsg(msgHandle,&imageTransferStats);
#endif

   CMSLOG_SEC_SET_PORT(&logData, HTTPD_PORT);
   CMSLOG_SEC_SET_APP_NAME(&logData, "HTTP");
   CMSLOG_SEC_SET_USER(&logData, &glbWebVar.curUserName[0]);
   CMSLOG_SEC_SET_SRC_IP(&logData, &glbWebVar.pcIpAddr[0]);
   cmsLog_security(LOG_SECURITY_AUTH_RESOURCES, &logData, "Back up configuration settings");
   
   cmsMem_free(cfgBuf);
   return;
}


#define PRINT_TABLE_LINE(parameter, format_specifier, field, value) if (!ret) \
{                                                                             \
   fprintf(fs, "   <tr>\n");                                                  \
   fprintf(fs, "      <td>%d</td>\n", parameter);                             \
   fprintf(fs, "      <td>%d</td>\n", param.len);                             \
   fprintf(fs, "      <td>%s</td>\n", #parameter " " field);                  \
   if (val == CAL_FILE_INVALID_ENTRANCE)                                      \
   {                                                                          \
      fprintf(fs, "      <td>Parameter is invalid</td>\n");                   \
   }                                                                          \
   else                                                                       \
   {                                                                          \
      fprintf(fs, "      <td>" #format_specifier "</td>\n", value);           \
   }                                                                          \
   fprintf(fs, "   </tr>\n");                                                 \
}

#define PRINT_TABLE_PARAMETER(parameter, format_specifier, value) do          \
{                                                                             \
   PRINT_TABLE_LINE(parameter, format_specifier, "", value)                   \
} while(0)

#define PRINT_TABLE_FIELD(parameter, format_specifier, field) do              \
{                                                                             \
   PRINT_TABLE_LINE(parameter, format_specifier, #field, field)               \
} while(0)

#define PARAMETER_READ_AND_PRINT(parameter) do                                \
{                                                                             \
   param.offset =  parameter;                                                 \
   ret = ioctl(LaserDevFd, PMD_IOCTL_CAL_FILE_READ, &param);                  \
   PRINT_TABLE_PARAMETER(parameter, 0x%X, val);                               \
} while(0)

void cgiPmdSettings(char *query __attribute__((unused)), FILE *fs)
{
   int32_t val = 0;
   pmd_params param = {.offset =  0, .len = 0, .buf = (unsigned char *)&val};
   int ret = -1;
   int LaserDevFd;
   uint16_t type, voltage;
   uint16_t dir, tia, vga;
   float bias_gain, mod_gain;
   uint16_t assert, desassert;
   uint16_t high, low;
   uint16_t rate, dload;

   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "<script language=\"javascript\">\n");
   fprintf(fs, "<!-- hide\n\n");
   fprintf(fs, "function backupClick() {\n");
   fprintf(fs, "   var loc = 'pmdbackupsettings.conf';\n");
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n");
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
   fprintf(fs, "<title></title>\n</head>\n<body>\n<blockquote>\n");
   fprintf(fs, "<form method='post' ENCTYPE='multipart/form-data' action='pmdupload.cgi' onSubmit='return isInProgress();'>\n");
   fprintf(fs, "<b>Settings - PMD</b><br>\n");
   fprintf(fs, "<br>\n");
   fprintf(fs, "<b>Step 1:</b> Obtain PMD calibration file from your provider.<br>\n");
   fprintf(fs, "<b>Step 2:</b> Enter the path to the image file location in the box below or<br>\n");
   fprintf(fs, "click the &quot;Choose File&quot; button to locate the file. You may Drag&Drop as well.<br>\n");
   fprintf(fs, "<b>Step 3:</b> Click the \"Update PMD Calibration\" button once to upload the new file.<br>\n");
   fprintf(fs, "NOTE: The update process takes about 10 Seconds to complete, and your Broadband Router\n");
   fprintf(fs, "will reboot.<br>\n");
   fprintf(fs, "<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
   fprintf(fs, "   <tr>\n");
   fprintf(fs, "      <td>PMD Calibration File Name:&nbsp;\n");
   fprintf(fs, "      </td>\n");
   fprintf(fs, "      <td><input type='file' name='filename' size='15'></td>\n");
   fprintf(fs, "   </tr>\n");
   fprintf(fs, "</table>\n");
   fprintf(fs, "<p align=\"center\"><input type='submit' value='Update PMD Calibration'></p>\n");
   fprintf(fs, "</form>\n");

   fprintf(fs, "<html><head>\n");
   fprintf(fs, "Backup PMD Calibration. You may save the PMD Calibration to a file on your PC.<br>\n");
   fprintf(fs, "<p align=\"center\"><input type='button' value='Backup PMD Calibration' onClick='backupClick()'></p>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");

   // write headers
   fprintf(fs, "   <tr>\n");
   fprintf(fs, "      <td class='hd'>Index</td>\n");
   fprintf(fs, "      <td class='hd'>Sub-Index</td>\n");
   fprintf(fs, "      <td class='hd'>Parameter</td>\n");
   fprintf(fs, "      <td class='hd'>Value</td>\n");
   fprintf(fs, "   </tr>\n");

   // write body
   LaserDevFd = open(LASER_DEV, O_RDWR);
   if( LaserDevFd < 0)
   {
      goto close_calibration_table;
   }

   PARAMETER_READ_AND_PRINT(pmd_file_watermark);
   PARAMETER_READ_AND_PRINT(pmd_file_version);
   PARAMETER_READ_AND_PRINT(pmd_level_0_dac);
   PARAMETER_READ_AND_PRINT(pmd_level_1_dac);
   PARAMETER_READ_AND_PRINT(pmd_bias);
   PARAMETER_READ_AND_PRINT(pmd_mod);

   param.offset = pmd_apd;
   ret = ioctl(LaserDevFd, PMD_IOCTL_CAL_FILE_READ, &param);
   type = (val & 400) >> 10;
   voltage = val & 0x3ff;
   PRINT_TABLE_FIELD(pmd_apd, 0x%X, type);
   PRINT_TABLE_FIELD(pmd_apd, 0x%X, voltage);

   param.offset = pmd_mpd_config;
   ret = ioctl(LaserDevFd, PMD_IOCTL_CAL_FILE_READ, &param);
   dir = (val & 0x200) >> 9;
   vga = (val & 0xc00) >> 10;
   tia = (val & 0xF000) >> 12;
   PRINT_TABLE_FIELD(pmd_mpd_config, 0x%X, dir);
   PRINT_TABLE_FIELD(pmd_mpd_config, 0x%X, vga);
   PRINT_TABLE_FIELD(pmd_mpd_config, 0x%X, tia);

   param.offset = pmd_mpd_gains;
   ret = ioctl(LaserDevFd, PMD_IOCTL_CAL_FILE_READ, &param);
   bias_gain = (float)(val & 0xff)/ 256;
   mod_gain  = (float)(val >> 8 )/ 256;
   PRINT_TABLE_FIELD(pmd_mpd_gains, %f, bias_gain);
   PRINT_TABLE_FIELD(pmd_mpd_gains, %f, mod_gain);

   PARAMETER_READ_AND_PRINT(pmd_apdoi_ctrl);

   param.offset = pmd_rssi_a;
   ret = ioctl(LaserDevFd, PMD_IOCTL_CAL_FILE_READ, &param);
   PRINT_TABLE_PARAMETER(pmd_rssi_a, %f, (float)val / 256);

   param.offset = pmd_rssi_b;
   ret = ioctl(LaserDevFd, PMD_IOCTL_CAL_FILE_READ, &param);
   PRINT_TABLE_PARAMETER(pmd_rssi_b, %f, (float)val / 256);

   param.offset = pmd_rssi_c;
   ret = ioctl(LaserDevFd, PMD_IOCTL_CAL_FILE_READ, &param);
   PRINT_TABLE_PARAMETER(pmd_rssi_c, %f, (float)val / 256);

   PARAMETER_READ_AND_PRINT(pmd_temp_0);

   param.offset = pmd_temp_coff_h;
   ret = ioctl(LaserDevFd, PMD_IOCTL_CAL_FILE_READ, &param);
   PRINT_TABLE_PARAMETER(pmd_temp_coff_h, %f, (float)val / 4096);

   param.offset = pmd_temp_coff_l;
   ret = ioctl(LaserDevFd, PMD_IOCTL_CAL_FILE_READ, &param);
   PRINT_TABLE_PARAMETER(pmd_temp_coff_l, %f, (float)val / 4096);

   PARAMETER_READ_AND_PRINT(pmd_esc_thr);
   PARAMETER_READ_AND_PRINT(pmd_rogue_thr);
   PARAMETER_READ_AND_PRINT(pmd_avg_level_0_dac);
   PARAMETER_READ_AND_PRINT(pmd_avg_level_1_dac);

   param.offset = pmd_dacrange;
   ret = ioctl(LaserDevFd, PMD_IOCTL_CAL_FILE_READ, &param);
   PRINT_TABLE_PARAMETER(pmd_dacrange, 0x%X, val & 0x3);

   param.offset = pmd_los_thr;
   ret = ioctl(LaserDevFd, PMD_IOCTL_CAL_FILE_READ, &param);
   assert = val & 0xff;
   desassert = (val & 0xff00) >> 8;
   PRINT_TABLE_FIELD(pmd_los_thr, 0x%X, assert);
   PRINT_TABLE_FIELD(pmd_los_thr, 0x%X, desassert);

   param.offset = pmd_sat_pos;
   ret = ioctl(LaserDevFd, PMD_IOCTL_CAL_FILE_READ, &param);
   high = val & 0xff;
   low = (val & 0xff00) >> 8;
   PRINT_TABLE_FIELD(pmd_sat_pos, 0x%X, high);
   PRINT_TABLE_FIELD(pmd_sat_pos, 0x%X, low);

   param.offset = pmd_sat_neg;
   ret = ioctl(LaserDevFd, PMD_IOCTL_CAL_FILE_READ, &param);
   high = val & 0xff;
   low = (val & 0xff00) >> 8;
   PRINT_TABLE_FIELD(pmd_sat_neg, 0x%X, high);
   PRINT_TABLE_FIELD(pmd_sat_neg, 0x%X, low);

   param.offset = pmd_edge_rate;
   ret = ioctl(LaserDevFd, PMD_IOCTL_CAL_FILE_READ, &param);
   rate = val & 0xff;
   dload = (val & 0xff00) >> 8;
   PRINT_TABLE_FIELD(pmd_edge_rate, 0x%X, rate);
   PRINT_TABLE_FIELD(pmd_edge_rate, 0x%X, dload);
   param.len = 1;
   ret = ioctl(LaserDevFd, PMD_IOCTL_CAL_FILE_READ, &param);
   rate = val & 0xff;
   dload = (val & 0xff00) >> 8;
   PRINT_TABLE_FIELD(pmd_edge_rate, 0x%X, rate);
   PRINT_TABLE_FIELD(pmd_edge_rate, 0x%X, dload);
   param.len = 0;

   PARAMETER_READ_AND_PRINT(pmd_preemphasis_weight);
   param.len = 1;
   PARAMETER_READ_AND_PRINT(pmd_preemphasis_weight);
   param.len = 0;
   PARAMETER_READ_AND_PRINT(pmd_preemphasis_delay);
   param.len = 1;
   PARAMETER_READ_AND_PRINT(pmd_preemphasis_delay);
   param.len = 0;

   PARAMETER_READ_AND_PRINT(pmd_duty_cycle);
   PARAMETER_READ_AND_PRINT(pmd_mpd_calibctrl);
   PARAMETER_READ_AND_PRINT(pmd_tx_power);
   PARAMETER_READ_AND_PRINT(pmd_bias0);
   PARAMETER_READ_AND_PRINT(pmd_temp_offset);
   PARAMETER_READ_AND_PRINT(pmd_bias_delta_i);
   PARAMETER_READ_AND_PRINT(pmd_slope_efficiency);

   close(LaserDevFd);

close_calibration_table:
   fprintf(fs, "</table><br>\n");
   fprintf(fs, "</form>\n");
   fprintf(fs, "</blockquote>\n");
   fprintf(fs, "</body>\n");
   fprintf(fs, "</html>\n");

   fflush(fs);
}

/** Send the PMD calibration file out through this file handle.
 */
#define READ_BUF_SIZE (256)

void cgiBackupPmdSettings(char *query __attribute__((unused)), FILE *fs)
{
   FILE *pmd_calibration_file;

   CmsSecurityLogData logData = EMPTY_CMS_SECURITY_LOG_DATA;

#ifdef SUPPORT_TR69C_AUTONOMOUS_TRANSFER_COMPLETE
    memset(&imageTransferStats,0,sizeof(CmsImageTransferStats));
    imageTransferStats.startTime = time(NULL);
    imageTransferStats.isDownload = FALSE;
#endif

   pmd_calibration_file = fopen(PMD_CALIBRATION_FILE_PATH, "r");
   if (NULL == pmd_calibration_file)
   {
      cmsLog_error("failed to open the PMD calibration file, errno=%d, %s", errno, strerror(errno));
   }
   else
   {
      UINT32 write_count, read_count, bytes_read, bytes_written;
      char calibration_data[READ_BUF_SIZE];

      read_count = write_count = 0;
      do
      {
         bytes_read = fread(calibration_data, 1, READ_BUF_SIZE, pmd_calibration_file);
         read_count += bytes_read;
         if (0 < bytes_read)
         {
            bytes_written = fwrite(calibration_data, 1, bytes_read, fs);
            write_count += bytes_written;
         }
         else
         {
            bytes_written = 0;
         }
      } while (0 < bytes_written);

#ifdef SUPPORT_TR69C_AUTONOMOUS_TRANSFER_COMPLETE
      imageTransferStats.fileSize = write_count;
      imageTransferStats.fileType = CMS_IMAGE_FORMAT_FLASH;
#endif

      if (read_count != write_count)
      {
#ifdef SUPPORT_TR69C_AUTONOMOUS_TRANSFER_COMPLETE
         imageTransferStats.faultCode = CMSRET_INTERNAL_ERROR;
         strcpy(imageTransferStats.faultStr,"Write of PMD calibration backup file failed.");
#endif
         cmsLog_error("write of PMD calibration backup file failed, write_count=%u read_count=%u", write_count, read_count);
      }
#ifdef SUPPORT_TR69C_AUTONOMOUS_TRANSFER_COMPLETE
      else
      {
         imageTransferStats.faultCode = CMSRET_SUCCESS;
      }
#endif
   }

#ifdef SUPPORT_TR69C_AUTONOMOUS_TRANSFER_COMPLETE
   imageTransferStats.completeTime = time(NULL);
   cmsImg_sendAutonomousTransferCompleteMsg(msgHandle,&imageTransferStats);
#endif

   CMSLOG_SEC_SET_PORT(&logData, HTTPD_PORT);
   CMSLOG_SEC_SET_APP_NAME(&logData, "HTTP");
   CMSLOG_SEC_SET_USER(&logData, &glbWebVar.curUserName[0]);
   CMSLOG_SEC_SET_SRC_IP(&logData, &glbWebVar.pcIpAddr[0]);
   cmsLog_security(LOG_SECURITY_AUTH_RESOURCES, &logData, "Backup PMD calibration");

   return;
}

