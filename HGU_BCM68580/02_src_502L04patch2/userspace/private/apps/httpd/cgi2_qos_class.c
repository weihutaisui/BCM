/***********************************************************************
 *
 *  Copyright (c) 2010  Broadcom Corporation
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


#ifdef DMP_DEVICE2_QOS_1


/*!\file cgi2_qos_class.c
 * \brief this file contains TR181 specific QoS code.  Common QoS code is in
 *        cgi_qos_class.c
 */


#include <unistd.h>
#include <linux/if_ether.h>

#include "cms.h"
#include "cms_qos.h"
#include "cms_qdm.h"
#include "httpd.h"
#include "cgi_cmd.h"
#include "cgi_main.h"
#include "cgi_qos.h"


extern void cgiQosClsView(FILE *fs);


void cgiQosClsViewBody_dev2(FILE *fs)
{
   const char *excludeStr = "(E)";
   InstanceIdStack iidStack;
   Dev2QosClassificationObject *cObj = NULL;
   CmsRet ret;

   /* display each entry in the classification table. */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_DEV2_QOS_CLASSIFICATION, &iidStack, (void **)&cObj)) == CMSRET_SUCCESS)
   {
      fprintf(fs, "   <tr>\n");

      /* display class name */
      fprintf(fs, "      <td>%s</td>\n", cObj->X_BROADCOM_COM_ClassName);

      /* display ClassificationOrder */
      fprintf(fs, "      <td>%d</td>\n", cObj->order);

      /* display classification criterion- ClassInterface */
      if (!IS_EMPTY_STRING(cObj->interface))
      {
         char ingressIntfNameBuf[CMS_IFNAME_LENGTH]={0};

         /* find intfname from class interface full path */
         if (cmsUtl_strcmp(cObj->interface, MDMVS_LOCAL) == 0 ||
             cmsUtl_strcmp(cObj->interface, MDMVS_LAN)   == 0 ||
             cmsUtl_strcmp(cObj->interface, MDMVS_WAN)   == 0)
         {
            fprintf(fs, "      <td>%s</td>\n", cObj->interface);
         }
         else if ((ret = qdmIntf_fullPathToIntfnameLocked(cObj->interface, ingressIntfNameBuf)) == CMSRET_SUCCESS)
         {
            fprintf(fs, "      <td>%s</td>\n", ingressIntfNameBuf);
         }
         else
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }

      /* display classification criterion- Ethertype */
      if (cObj->ethertype == ETH_P_IP)
      {
         fprintf(fs, "      <td>IP%s</td>\n", cObj->protocolExclude? excludeStr : "");
      }
      else if (cObj->ethertype == ETH_P_ARP)
      {
         fprintf(fs, "      <td>ARP%s</td>\n", cObj->protocolExclude? excludeStr : "");
      }
      else if (cObj->ethertype == ETH_P_IPV6)
      {
         fprintf(fs, "      <td>IPv6%s</td>\n", cObj->protocolExclude? excludeStr : "");
      }
      else if (cObj->ethertype == ETH_P_PPP_DISC)
      {
         fprintf(fs, "      <td>PPPoE_DISC%s</td>\n", cObj->protocolExclude? excludeStr : "");
      }
      else if (cObj->ethertype == ETH_P_PPP_SES)
      {
         fprintf(fs, "      <td>PPPoE_SES%s</td>\n", cObj->protocolExclude? excludeStr : "");
      }
      else if (cObj->ethertype == 0x8865)
      {
         fprintf(fs, "      <td>0x8865%s</td>\n", cObj->protocolExclude? excludeStr : "");
      }
      else if (cObj->ethertype == 0x8866)
      {
         fprintf(fs, "      <td>0x8866%s</td>\n", cObj->protocolExclude? excludeStr : "");
      }
      else if (cObj->ethertype == ETH_P_8021Q)
      {
         fprintf(fs, "      <td>8021Q%s</td>\n", cObj->protocolExclude? excludeStr : "");
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }

      /* display classification criterion- SourceMACAddress */
      if (!IS_EMPTY_STRING(cObj->sourceMACAddress))
      {
         if (!IS_EMPTY_STRING(cObj->sourceMACMask))
         {
            fprintf(fs, "      <td>%s/ %s%s</td>\n", cObj->sourceMACAddress, cObj->sourceMACMask, cObj->sourceMACExclude? excludeStr : "");
         }
         else
         {
            fprintf(fs, "      <td>%s%s</td>\n", cObj->sourceMACAddress, cObj->sourceMACExclude? excludeStr : "");
         }
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }

      /* display classification criterion- DestMACAddress */
      if (!IS_EMPTY_STRING(cObj->destMACAddress))
      {
         if (!IS_EMPTY_STRING(cObj->destMACMask))
         {
            fprintf(fs, "      <td>%s/ %s%s</td>\n", cObj->destMACAddress, cObj->destMACMask, cObj->destMACExclude? excludeStr : "");
         }
         else
         {
            fprintf(fs, "      <td>%s%s</td>\n", cObj->destMACAddress, cObj->destMACExclude? excludeStr : "");
         }
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }

      /* display classification criterion- either SourceIP or SourceVendorClassID or SourceUserClassID */
      if (!IS_EMPTY_STRING(cObj->sourceIP))
      {
         if (strchr(cObj->sourceIP, ':') != NULL)
         {
            /* IPv6 address */
            fprintf(fs, "      <td>%s%s</td>\n", cObj->sourceIP, cObj->sourceIPExclude? excludeStr : "");
         }
         else
         {
            /* IPv4 address */
            if (!IS_EMPTY_STRING(cObj->sourceMask))
            {
               fprintf(fs, "      <td>%s/%d%s</td>\n", cObj->sourceIP, cmsNet_getLeftMostOneBitsInMask(cObj->sourceMask), cObj->sourceIPExclude? excludeStr : "");
            }
            else
            {
               fprintf(fs, "      <td>%s%s</td>\n", cObj->sourceIP, cObj->sourceIPExclude? excludeStr : "");
            }
         }
      }
      else if (!IS_EMPTY_STRING(cObj->sourceVendorClassID))
      {
         fprintf(fs, "      <td>DHCP OP%d:%s%s</td>\n", DHCP_VENDOR_CLASS_OPTION, cObj->sourceVendorClassID, cObj->sourceVendorClassIDExclude? excludeStr : "");
      }
      else if (!IS_EMPTY_STRING(cObj->sourceUserClassID))
      {
         fprintf(fs, "      <td>DHCP OP%d:%s%s</td>\n", DHCP_USER_CLASS_OPTION, cObj->sourceUserClassID, cObj->sourceUserClassIDExclude? excludeStr : "");
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }

      /* display classification criterion- DestIP */
      if (!IS_EMPTY_STRING(cObj->destIP))
      {
         if (strchr(cObj->destIP, ':') != NULL)
         {
            /* IPv6 address */
            fprintf(fs, "      <td>%s%s</td>\n", cObj->destIP, cObj->destIPExclude? excludeStr : "");
         }
         else
         {
            /* IPv4 address */
            if (!IS_EMPTY_STRING(cObj->destMask))
            {
               fprintf(fs, "      <td>%s/%d%s</td>\n", cObj->destIP, cmsNet_getLeftMostOneBitsInMask(cObj->destMask), cObj->destIPExclude? excludeStr : "");
            }
            else
            {
               fprintf(fs, "      <td>%s%s</td>\n", cObj->destIP, cObj->destIPExclude? excludeStr : "");
            }
         }
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }

      /* display classification criterion- Protocol */
      if (cObj->protocol == IPPROTO_TCP)
      {
         fprintf(fs, "      <td>TCP%s</td>\n", cObj->protocolExclude? excludeStr : "");
      }
      else if (cObj->protocol == IPPROTO_UDP)
      {
         fprintf(fs, "      <td>UDP%s</td>\n", cObj->protocolExclude? excludeStr : "");
      }
      else if (cObj->protocol == IPPROTO_ICMP)
      {
         fprintf(fs, "      <td>ICMP%s</td>\n", cObj->protocolExclude? excludeStr : "");
      }
      else if (cObj->protocol == IPPROTO_IGMP)
      {
         fprintf(fs, "      <td>IGMP%s</td>\n", cObj->protocolExclude? excludeStr : "");
      }
      else if (cObj->protocol == IPPROTO_ICMPV6)
      {
         fprintf(fs, "      <td>ICMPV6%s</td>\n", cObj->protocolExclude? excludeStr : "");
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }

      /* display classification criterion- SourcePort, SourcePortRangeMax */
      if (cObj->sourcePort != QOS_CRITERION_UNUSED)
      {
         if (cObj->sourcePortRangeMax != QOS_CRITERION_UNUSED)
         {
            fprintf(fs, "      <td>%d:%d%s</td>\n", cObj->sourcePort, cObj->sourcePortRangeMax, cObj->sourcePortExclude? excludeStr : "");
         }
         else
         {
            fprintf(fs, "      <td>%d%s</td>\n", cObj->sourcePort, cObj->sourcePortExclude? excludeStr : "");
         }
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }

      /* display classification criterion- DestPort, DestPortRangeMax */
      if (cObj->destPort != QOS_CRITERION_UNUSED)
      {
         if (cObj->destPortRangeMax != QOS_CRITERION_UNUSED)
         {
            fprintf(fs, "      <td>%d:%d%s</td>\n", cObj->destPort, cObj->destPortRangeMax, cObj->destPortExclude? excludeStr : "");
         }
         else
         {
            fprintf(fs, "      <td>%d%s</td>\n", cObj->destPort, cObj->destPortExclude? excludeStr : "");
         }
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }

      /* display classification criterion- DSCPCheck */
      fprintf(fs, "      <td>%s%s</td>\n", cgiQosDscpMarkToName(cObj->DSCPCheck), cObj->DSCPExclude ? excludeStr : "");

      /* display classification criterion- EthernetPriorityCheck */
      if (cObj->ethernetPriorityCheck != QOS_CRITERION_UNUSED)
      {
         fprintf(fs, "      <td>%d%s</td>\n", cObj->ethernetPriorityCheck, cObj->ethernetPriorityExclude ? excludeStr : "");
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }

      /* display classification result- ClassQueue */
      fprintf(fs, "      <td>%d</td>\n", cObj->X_BROADCOM_COM_ClassQueue);

#ifdef SUPPORT_POLICING
      /* display classification result- ClassPolicer */
      if (cObj->X_BROADCOM_COM_ClassPolicer != QOS_RESULT_NO_CHANGE)
      {
         fprintf(fs, "      <td>%d</td>\n", cObj->X_BROADCOM_COM_ClassPolicer);
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }
#endif

      /* display classification result- DSCPMark */
      fprintf(fs, "      <td>%s</td>\n", cgiQosDscpMarkToName(cObj->DSCPMark));

      /* display classification result- EthernetPriorityMark */
      if (cObj->ethernetPriorityMark != QOS_RESULT_NO_CHANGE)
      {
         fprintf(fs, "      <td>%d</td>\n", cObj->ethernetPriorityMark);
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }

#ifdef SUPPORT_RATE_LIMIT
      /* display classification result- ClassRate */
      if (cObj->X_BROADCOM_COM_ClassRate != QOS_RESULT_NO_CHANGE)
      {
         fprintf(fs, "      <td>%d</td>\n", cObj->X_BROADCOM_COM_ClassRate);
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }
#endif

      fprintf(fs, "      <td align='center'><input type='checkbox' name='enbl' %s></td>\n", cObj->enable? "checked" : "");
      fprintf(fs, "      <td align='center'><input type='checkbox' name='rml'></td>\n");

      fprintf(fs, "   </tr>\n");

      cmsObj_free((void **)&cObj);
   }
}


/* I hate to copy this giant function, but if I try to generalize it,
 * I would have to touch too much code and too many variables.  So making
 * a copy seems like the least bad approach.
 */
void cgiQosClsAdd_dev2(char *query, FILE *fs)
{
   char cmd[WEB_MD_BUF_SIZE_MAX];
   Dev2QosClassificationObject *cObj;
   InstanceIdStack iidStack;
   UBOOL8 isDuplicate;
   CmsRet ret;

   /* get default values for the classification object instance */
   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_get(MDMOID_DEV2_QOS_CLASSIFICATION, &iidStack, OGF_DEFAULT_VALUES, (void **)&cObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get returns error. ret=%d", ret);
      sprintf(cmd, "Add qos class failed. ret=%d", ret);
      cgiWriteMessagePage(fs, "QoS Classification Config Error", cmd, "qoscls.cmd?action=view");
      return;
   }

   /* get an unused classification key */
   if ((ret = dalQos_getAvailableClsKey(&(cObj->X_BROADCOM_COM_ClassKey))) != CMSRET_SUCCESS)
   {
      cmsLog_error("dalQos_getAvailableClsKey returns error. ret=%d", ret);
      sprintf(cmd, "Add qos class failed. ret=%d", ret);
      cgiWriteMessagePage(fs, "QoS Classification Config Error", cmd, "qoscls.cmd?action=view");
      cmsObj_free((void **)&cObj);
      return;
   }

   cmd[0] = '\0';

   /* className */
   if (cgiGetValueByName(query, "clsName", cmd) == CGI_STS_OK)
   {
      cmsMem_free(cObj->X_BROADCOM_COM_ClassName);
      cObj->X_BROADCOM_COM_ClassName = cmsMem_strdup(cmd);
   }

   /* ClassificationEnable */
   if (cgiGetValueByName(query, "rulsEnbl", cmd) == CGI_STS_OK)
   {
      cObj->enable = atoi(cmd);
   }

   /* ClassificationOrder */
   if (cgiGetValueByName(query, "rulsOrder", cmd) == CGI_STS_OK)
   {
      cObj->order = atoi(cmd);
   }

   /* Ether type */
   if (cgiGetValueByName(query, "ethertype", cmd) == CGI_STS_OK)
   {
      cObj->ethertype = strtol(cmd, (char **)NULL, 16);
   }

   /* Protocol */
   if (cgiGetValueByName(query, "protocol", cmd) == CGI_STS_OK)
   {
      cObj->protocol = atoi(cmd);    /* TCP or UDP or ICMP or IGMP */
   }

   /* either SourceIP,SourceMask or SourceVendorClassID or SourceUserClassID */
   if (cgiGetValueByName(query, "ipoptionlist", cmd) == CGI_STS_OK)
   {
      SINT32 ipoptionlist = atoi(cmd);

      if (cgiGetValueByName(query, "srcAddr", cmd) == CGI_STS_OK)
      {
         if (ipoptionlist == 0)
         {
            if (strchr(cmd, ':') != NULL)
            {
               /* IPv6 address */
               cmsMem_free(cObj->sourceIP);
               cObj->sourceIP = cmsMem_strdup(cmd);
            }
            else
            {
               /* IPv4 address */
               struct in_addr addr, mask;

               cmsNet_inet_cidrton(cmd, &addr, &mask);
               if (addr.s_addr != 0)
               {
                  cmsMem_free(cObj->sourceIP);
                  cObj->sourceIP = cmsMem_strdup(inet_ntoa(addr));

                  if (mask.s_addr != 0)
                  {
                     cmsMem_free(cObj->sourceMask);
                     cObj->sourceMask = cmsMem_strdup(inet_ntoa(mask));
                  }
               }
            }
         }
         else if (ipoptionlist == 1)
         {
            cmsMem_free(cObj->sourceVendorClassID);
            cObj->sourceVendorClassID = cmsMem_strdup(cmd);
         }
         else if (ipoptionlist == 2)
         {
            cmsMem_free(cObj->sourceUserClassID);
            cObj->sourceUserClassID = cmsMem_strdup(cmd);
         }
      }
   }

   /* EthernetPriorityCheck */
   if (cgiGetValueByName(query, "vlan8021p", cmd) == CGI_STS_OK)
   {
      cObj->ethernetPriorityCheck = atoi(cmd);
   }

   /* EthernetPriorityMark */
   if (cgiGetValueByName(query, "wanVlan8021p", cmd) == CGI_STS_OK)
   {
      cObj->ethernetPriorityMark = atoi(cmd);
   }

   /* DSCPCheck */
   if ((cgiGetValueByName(query, "dscpCheck", cmd)) == CGI_STS_OK)
   {
      cObj->DSCPCheck = atoi(cmd);
   }

   /* DSCPMark */
   if ((cgiGetValueByName(query, "dscpMark", cmd)) == CGI_STS_OK)
   {
      cObj->DSCPMark = atoi(cmd);
   }

   /* SourcePort and SourcePortRangeMax */
   if (cgiGetValueByName(query, "srcPort", cmd) == CGI_STS_OK)
   {
      char *portStr = NULL, *last = NULL;

      if ((portStr = strtok_r(cmd, ":", &last)) != NULL)
      {
         cObj->sourcePort = atoi(portStr);
      }
      if (cObj->sourcePort != QOS_CRITERION_UNUSED)
      {
         if ((portStr = strtok_r(NULL, ":", &last)) != NULL)
         {
            cObj->sourcePortRangeMax = atoi(portStr);
         }
      }
   }

   /* DestPort and DestPortRangeMax */
   if (cgiGetValueByName(query, "dstPort", cmd) == CGI_STS_OK)
   {
      char *portStr = NULL, *last = NULL;

      if ((portStr = strtok_r(cmd, ":", &last)) != NULL)
      {
         cObj->destPort = atoi(portStr);
      }
      if (cObj->destPort != QOS_CRITERION_UNUSED)
      {
         if ((portStr = strtok_r(NULL, ":", &last)) != NULL)
         {
            cObj->destPortRangeMax = atoi(portStr);
         }
      }
   }

   /* DestIP and DestMask */
   if ((cgiGetValueByName(query, "dstAddr", cmd)) == CGI_STS_OK)
   {
      if (strchr(cmd, ':') != NULL)
      {
         /* IPv6 address */
         cmsMem_free(cObj->destIP);
         cObj->destIP = cmsMem_strdup(cmd);
      }
      else
      {
         /* IPv4 address */
         struct in_addr addr, mask;

         cmsNet_inet_cidrton(cmd, &addr, &mask);
         if (addr.s_addr != 0)
         {
            cmsMem_free(cObj->destIP);
            cObj->destIP = cmsMem_strdup(inet_ntoa(addr));

            if (mask.s_addr != 0)
            {
               cmsMem_free(cObj->destMask);
               cObj->destMask = cmsMem_strdup(inet_ntoa(mask));
            }
         }
      }
   }

   /* ProtocolExclude */
   if (cgiGetValueByName(query, "protExclude", cmd) == CGI_STS_OK)
   {
      cObj->protocolExclude = atoi(cmd);
   }

   /* SourceIPExclude */
   if (cgiGetValueByName(query, "srcIpExclude", cmd) == CGI_STS_OK)
   {
      cObj->sourceIPExclude = atoi(cmd);
   }

   /* DestIPExclude */
   if (cgiGetValueByName(query, "destIpExclude", cmd) == CGI_STS_OK)
   {
      cObj->destIPExclude = atoi(cmd);
   }

   /* SourcePortExclude */
   if (cgiGetValueByName(query, "srcPortExclude", cmd) == CGI_STS_OK)
   {
      cObj->sourcePortExclude = atoi(cmd);
   }

   /* DestPortExclude */
   if (cgiGetValueByName(query, "destportExclude", cmd) == CGI_STS_OK)
   {
      cObj->destPortExclude = atoi(cmd);
   }

   /* EthernetPriorityExclude */
   if (cgiGetValueByName(query, "etherPrioExclude", cmd) == CGI_STS_OK)
   {
      cObj->ethernetPriorityExclude = atoi(cmd);
   }

   /* DSCPExclude */
   if (cgiGetValueByName(query, "dscpExclude", cmd) == CGI_STS_OK)
   {
      cObj->DSCPExclude = atoi(cmd);
   }

   /* ClassQueue */
   /* This tells us exactly which QoS queue the packet is supposed to
    * go out on.  Makes the egressInterface param seem redundant.
    */
   if (cgiGetValueByName(query, "queuekey", cmd) == CGI_STS_OK)
   {
      cObj->X_BROADCOM_COM_ClassQueue = atoi(cmd);
   }

#ifdef SUPPORT_POLICING
   /* ClassPolicer */
   if (cgiGetValueByName(query, "policerkey", cmd) == CGI_STS_OK)
   {
      cObj->X_BROADCOM_COM_ClassPolicer = atoi(cmd);
   }
#endif

#ifdef SUPPORT_RATE_LIMIT
    /* rate control */
   if (cgiGetValueByName(query, "rate", cmd) == CGI_STS_OK)
   {
      cObj->X_BROADCOM_COM_ClassRate = atoi(cmd);
   }
#endif

   /* egress interface */
   if (cgiGetValueByName(query, "egress", cmd) == CGI_STS_OK)
   {
      cmsMem_free(cObj->X_BROADCOM_COM_egressInterface);
      cObj->X_BROADCOM_COM_egressInterface = cmsMem_strdup(cmd);
   }

   /* ClassInterface (ingress interface) */
   if (cgiGetValueByName(query, "classIf", cmd) == CGI_STS_OK)
   {
      if (cmd[0] != '\0')
      {
         char *classInterface=NULL;

         if (cmsUtl_strcmp(cmd, MDMVS_LOCAL) == 0 ||
             cmsUtl_strcmp(cmd, MDMVS_LAN)   == 0 ||
             cmsUtl_strcmp(cmd, MDMVS_WAN)   == 0)
         {
            cmsMem_free(cObj->interface);
            cObj->interface = cmsMem_strdup(cmd);
         }
         else
         {
            /* convert L2 or L3 intfName to fullPath */
            ret = qdmIntf_intfnameToFullPathLocked(cmd, FALSE, &classInterface);
            if (ret != CMSRET_SUCCESS)
            {
               ret = qdmIntf_intfnameToFullPathLocked(cmd, TRUE, &classInterface);
            }

            if (ret != CMSRET_SUCCESS)
            {
               cmsLog_error("Could not convert %s to L2 or L3 fullPath", cmd);
            }
            else
            {
               cmsMem_free(cObj->interface);
               cObj->interface = classInterface;
            }
         }
      }
   }

   /* SourceMACAddress and SourceMACMask */
   if ((cgiGetValueByName(query, "srcMacAddr", cmd)) == CGI_STS_OK)
   {
      cmsMem_free(cObj->sourceMACAddress);
      cObj->sourceMACAddress = cmsMem_strdup(cmd);
      if ((cgiGetValueByName(query, "srcMacMask", cmd)) == CGI_STS_OK)
      {
         cmsMem_free(cObj->sourceMACMask);
         cObj->sourceMACMask = cmsMem_strdup(cmd);
      }
   }

   /* DestMACAddress and DestMACMask */
   if ((cgiGetValueByName(query, "dstMacAddr", cmd)) == CGI_STS_OK)
   {
      cmsMem_free(cObj->destMACAddress);
      cObj->destMACAddress = cmsMem_strdup(cmd);
      if ((cgiGetValueByName(query, "dstMacMask", cmd)) == CGI_STS_OK)
      {
         cmsMem_free(cObj->destMACMask);
         cObj->destMACMask = cmsMem_strdup(cmd);
      }
   }

   /* check to see if the classification already exists */
   if ((ret = dalQos_duplicateClassCheck(cObj, &isDuplicate)) != CMSRET_SUCCESS)
   {
      cmsLog_error("dalQos_duplicateClassCheck returns error. ret=%d", ret);
      sprintf(cmd, "Duplicate check failed. ret=%d", ret);
      cgiWriteMessagePage(fs, "QoS Classification Config Error", cmd, "qoscls.cmd?action=viewcfg");
      cmsObj_free((void **)&cObj);
      return;
   }
   if (isDuplicate)
   {
      sprintf(cmd, "Classification already exists.");
      cmsLog_error(cmd);
      cgiWriteMessagePage(fs, "QoS Classification Config Error", cmd, "qoscls.cmd?action=viewcfg");
      cmsObj_free((void **)&cObj);
      return;
   }

   if (cObj->X_BROADCOM_COM_ClassQueue > 0)
   {
      UBOOL8 enabled=FALSE;

      ret = qdmQos_getQueueInfoByClassQueueLocked(cObj->X_BROADCOM_COM_ClassQueue,
                                                  &enabled, NULL, NULL);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("qdmQos_getQueueInfoByClassQueueLocked returns error. ret=%d", ret);
         sprintf(cmd, "Get class queue info failed. ret=%d", ret);
         cgiWriteMessagePage(fs, "QoS Classification Config Error", cmd, "qoscls.cmd?action=viewcfg");
         cmsObj_free((void **)&cObj);
         return;
      }

      if (cObj->enable && !enabled)
      {
         sprintf(cmd, "Class queue is not enabled.");
         cmsLog_error(cmd);
         cgiWriteMessagePage(fs, "QoS Classification Config Error", cmd, "qoscls.cmd?action=viewcfg");
         cmsObj_free((void **)&cObj);
         return;
      }
   }

#ifdef SUPPORT_POLICING
   if (cObj->X_BROADCOM_COM_ClassPolicer > 0)
   {
      CmsQosPolicerInfo policerInfo;

      if ((ret = qdmQos_getClassPolicerInfoLocked(cObj->X_BROADCOM_COM_ClassPolicer, &policerInfo)) != CMSRET_SUCCESS)
      {
         cmsLog_error("qdmQos_getClassPolicerInfoLocked returns error. ret=%d", ret);
         sprintf(cmd, "Get class policer info failed. ret=%d", ret);
         cgiWriteMessagePage(fs, "QoS Classification Config Error", cmd, "qoscls.cmd?action=viewcfg");
         cmsObj_free((void **)&cObj);
         return;
      }

      if (cObj->enable && !policerInfo.enable)
      {
         sprintf(cmd, "Class policer is not enabled.");
         cmsLog_error(cmd);
         cgiWriteMessagePage(fs, "QoS Classification Config Error", cmd, "qoscls.cmd?action=viewcfg");
         cmsObj_free((void **)&cObj);
         return;
      }
   }
#endif

   /* Set this to the configuration */
   cmsLog_debug("Adding new qos classification: name=%s order=%d classQueue=%d egressIntf=%s",
            cObj->X_BROADCOM_COM_ClassName, cObj->order,
            cObj->X_BROADCOM_COM_ClassQueue, cObj->X_BROADCOM_COM_egressInterface);

   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_addInstance(MDMOID_DEV2_QOS_CLASSIFICATION, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_addInstance MDMOID_DEV2_QOS_CLASSIFICATION returns error, ret=%d", ret);
   }
   else
   {
      if ((ret = cmsObj_set((void *)cObj, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsObj_set returns error, ret = %d", ret);
         cmsObj_deleteInstance(MDMOID_DEV2_QOS_CLASSIFICATION, &iidStack);
      }
   }

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not add Classification obj, ret=%d", ret);
      sprintf(cmd, "Add qos class failed. ret=%d", ret);
      cgiWriteMessagePage(fs, "QoS Classification Config Error", cmd, "qoscls.cmd?action=view");
   }
   else
   {
      /* set flag to save mdm to flash */
      glbSaveConfigNeeded = TRUE;
      /* refresh page */
      cgiQosClsView(fs);
   }

   cmsObj_free((void **)&cObj);
}


void cgiQosClsRemove_dev2(char *query, FILE *fs)
{
   char cmd[WEB_MD_BUF_SIZE_MAX];
   char lst[WEB_BUF_SIZE_MAX];
   char *last = NULL;
   InstanceIdStack iidStack, iidStackPrev;
   Dev2QosClassificationObject *cObj = NULL;
   CmsRet ret;

   lst[0] = '\0';
   if (cgiGetValueByName(query, "rmLst", lst) != CGI_STS_OK)
   {
      return;
   }

   /* loop through the classification table to find the instances to delete */
   INIT_INSTANCE_ID_STACK(&iidStack);
   iidStackPrev = iidStack;
   while ((ret = cmsObj_getNext(MDMOID_DEV2_QOS_CLASSIFICATION, &iidStack, (void **)&cObj)) == CMSRET_SUCCESS)
   {
      char *check;

      if (last == NULL)
      {
         check = strtok_r(lst, ",", &last);
      }
      else
      {
         check = strtok_r(NULL, ",", &last);
      }
      if (check == NULL)
      {
         cmsObj_free((void **)&cObj);
         break;   /* done */
      }

      if (atoi(check) == 1)
      {
         /* delete this instance */
         cmsLog_debug("deleting classification: name=%s order=%d classQueue=%d egressIntf=%s",
                  cObj->X_BROADCOM_COM_ClassName, cObj->order,
                  cObj->X_BROADCOM_COM_ClassQueue, cObj->X_BROADCOM_COM_egressInterface);
         if ((ret = cmsObj_deleteInstance(MDMOID_DEV2_QOS_CLASSIFICATION, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_deleteInstance returns error, ret=%d", ret);
            sprintf(cmd, "Delete qos class failed. ret=%d", ret);
            cgiWriteMessagePage(fs, "QoS Classification Config Error", cmd, "qoscls.cmd?action=view");
            cmsObj_free((void **)&cObj);
            break;
         }
         glbSaveConfigNeeded = TRUE;

         /* since this instance has been deleted, we want to set the iidStack to
          * the previous instance, so that we can continue to do getNext.
          */
         iidStack = iidStackPrev;
      }
      cmsObj_free((void **)&cObj);

      /* save this iidStack in case we want to do a getNext from this instance */
      iidStackPrev = iidStack;
   }
}


void cgiQosClsEnable_dev2(char *query, FILE *fs)
{
   char cmd[WEB_MD_BUF_SIZE_MAX];
   char lst[WEB_BUF_SIZE_MAX];
   char *last = NULL;
   char *check;
   InstanceIdStack iidStack;
   Dev2QosClassificationObject *cObj = NULL;
   CmsRet ret;

   lst[0] = '\0';
   if (cgiGetValueByName( query, "eblLst", lst ) != CGI_STS_OK)
   {
      return;
   }
   /* loop through the classification table */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_DEV2_QOS_CLASSIFICATION, &iidStack, (void **)&cObj)) == CMSRET_SUCCESS)
   {
      if (last == NULL)
      {
         check = strtok_r(lst, ",", &last);
      }
      else
      {
         check = strtok_r(NULL, ",", &last);
      }

      if (check == NULL)
      {
         cmsObj_free((void **)&cObj);
         break;   /* done */
      }

      if (atoi(check) == 1)
      {
         if (!cObj->enable)
         {
            cObj->enable = TRUE;
            if ((ret = cmsObj_set(cObj, &iidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("cmsObj_set returns error. ret=%d", ret);
               sprintf(cmd, "Enable qos class failed. ret=%d", ret);
               cgiWriteMessagePage(fs, "QoS Classification Config Error", cmd, "qoscls.cmd?action=view");
               cmsObj_free((void **)&cObj);
               break;
            }
            else
            {
               glbSaveConfigNeeded = TRUE;
            }
         }
      }
      else
      {
         if (cObj->enable)
         {
            cObj->enable = FALSE;
            if ((ret = cmsObj_set(cObj, &iidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("cmsObj_set returns error. ret=%d", ret);
               sprintf(cmd, "Enable qos class failed. ret=%d", ret);
               cgiWriteMessagePage(fs, "QoS Classification Config Error", cmd, "qoscls.cmd?action=view");
               cmsObj_free((void **)&cObj);
               break;
            }
            else
            {
               glbSaveConfigNeeded = TRUE;
            }
         }
      }

      cmsObj_free((void **)&cObj);
   }
}


void cgiGetQosClsRulsOrder_dev2(int argc __attribute__((unused)),
                           char **argv __attribute__((unused)),
                           char *varValue)
{
   InstanceIdStack iidStack;
   Dev2QosClassificationObject *cObj = NULL;
   CmsRet ret;

   varValue[0] = '\0';

   /* loop through the queue table */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_DEV2_QOS_CLASSIFICATION, &iidStack, (void **)&cObj)) == CMSRET_SUCCESS)
   {
      char order[BUFLEN_16];

      sprintf(order, "%d|", cObj->order);
      strcat(varValue, order);
      cmsObj_free((void **)&cObj);
   }

   strcat(varValue, "Last");

}


#endif  /* DMP_DEVICE2_QOS_1 */

