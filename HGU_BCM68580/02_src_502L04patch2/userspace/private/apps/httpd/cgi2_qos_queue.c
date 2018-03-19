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


/* this file contains TR181 specific QoS code.  Common QoS code is in
 * cgi_qos_queue.c
 */


#include "cms.h"
#include "cms_qos.h"
#include "cms_qdm.h"
#include "httpd.h"
#include "cgi_cmd.h"
#include "cgi_main.h"
#include "cgi_qos.h"


void cgiQosQueueViewBody_dev2(FILE *fs)
{
   char intfname[CMS_IFNAME_LENGTH];
   InstanceIdStack iidStack;
   Dev2QosQueueObject *qObj = NULL;
   CmsRet ret;

   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_DEV2_QOS_QUEUE, &iidStack, (void **)&qObj)) == CMSRET_SUCCESS)
   {
      if (strstr(qObj->interface, TR181_WIFI_INTF_PATH) != NULL)
      {
         /* skip Wifi queues */
         cmsObj_free((void **)&qObj);
         continue;
      }

      if ((ret = qdmIntf_fullPathToIntfnameLocked(qObj->interface, intfname)) != CMSRET_SUCCESS)
      {
         cmsLog_error("qdmIntf_fullPathToIntfnameLocked on %s returns %d",
                      qObj->interface, ret);
         cmsObj_free((void **)&qObj);
         continue;
      }

      fprintf(fs, "    <tr align='center'>\n");

      /* queue name */
      fprintf(fs, "      <td>%s</td>\n", qObj->X_BROADCOM_COM_QueueName);
      
      /* queue key */
      fprintf(fs, "      <td>%d</td>\n", PEEK_INSTANCE_ID(&iidStack));
      
      /* queue interface */
      fprintf(fs, "      <td>%s</td>\n", intfname);

      /* queue id */
      fprintf(fs, "      <td>%d</td>\n", qObj->X_BROADCOM_COM_QueueId);

      /* queue precedence/algorithm/weight */
      if (strcmp(qObj->schedulerAlgorithm, MDMVS_SP) == 0)
      {
         fprintf(fs, "      <td>%d/%s</td>\n",
                 qObj->precedence, qObj->schedulerAlgorithm);
      }
      else
      {
         fprintf(fs, "      <td>%d/%s/%d</td>\n",
                 qObj->precedence, qObj->schedulerAlgorithm, qObj->weight);
      }

#if defined(DMP_ADSLWAN_1)
      /* queue latency path */
      if (qObj->X_BROADCOM_COM_DslLatency >= 0)
      {
         fprintf(fs, "      <td>%s</td>\n", qObj->X_BROADCOM_COM_DslLatency? "Path1" : "Path0");
      }
      else
      {
         fprintf(fs, "      <td>&nbsp;</td>\n");
      }
#endif
             
#if defined(SUPPORT_PTM)
      /* queue PTM priority */
      if (qObj->X_BROADCOM_COM_PtmPriority >= 0)
      {
         fprintf(fs, "      <td>%s</td>\n", qObj->X_BROADCOM_COM_PtmPriority? "High" : "Low");
      }
      else
      {
         fprintf(fs, "      <td>&nbsp;</td>\n");
      }
#endif

#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_63158) || defined(CHIP_4908) || defined(CHIP_63268)
      /* drop algorithm */
      if (cmsUtl_strcmp(qObj->dropAlgorithm, MDMVS_DT) == 0)
      {
         fprintf(fs, "      <td>%s</td>\n", MDMVS_DT);
      }
      else if (cmsUtl_strcmp(qObj->dropAlgorithm, MDMVS_RED) == 0)
      {
         fprintf(fs, "      <td>%s/%d/%d</td>\n", MDMVS_RED,
                 qObj->REDThreshold, qObj->X_BROADCOM_COM_LowClassMaxThreshold);
      }
      else if (cmsUtl_strcmp(qObj->dropAlgorithm, MDMVS_WRED) == 0)
      {
         fprintf(fs, "      <td>%s/%d/%d/%d/%d</td>\n", MDMVS_WRED,
                 qObj->REDThreshold, qObj->X_BROADCOM_COM_LowClassMaxThreshold,
                 qObj->X_BROADCOM_COM_HighClassMinThreshold, qObj->X_BROADCOM_COM_HighClassMaxThreshold);
      }
      else
      {
         fprintf(fs, "      <td>&nbsp;</td>\n");
      }
#endif

#if defined(CHIP_63268) || defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_63158) || defined(CHIP_4908) || (defined(DMP_ADSLWAN_1) && defined(SUPPORT_FAPCTL)) || defined(SUPPORT_RDPA) || defined(SUPPORT_BCMTM)
      if (qObj->shapingRate != QOS_QUEUE_NO_SHAPING)
      {
         fprintf(fs, "      <td>%d</td>\n", qObj->shapingRate);
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }
#endif      
#if defined(CHIP_63268) || defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_63158) || defined(CHIP_4908) || (defined(DMP_ADSLWAN_1) && defined(SUPPORT_FAPCTL)) || defined(SUPPORT_BCMTM)
      if (qObj->X_BROADCOM_COM_MinBitRate != QOS_QUEUE_NO_SHAPING)
      {
         fprintf(fs, "      <td>%d</td>\n", qObj->X_BROADCOM_COM_MinBitRate);
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }
      if ((qObj->shapingRate               != QOS_QUEUE_NO_SHAPING ||
           qObj->X_BROADCOM_COM_MinBitRate != QOS_QUEUE_NO_SHAPING) &&
          qObj->shapingBurstSize > 0)
      {
         fprintf(fs, "      <td>%d</td>\n", qObj->shapingBurstSize);
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }
#endif             

      /* queue enable */
      if (qObj->X_BROADCOM_COM_DslLatency >= 0 && qObj->X_BROADCOM_COM_QueueId == 1)
      {
         /* this is the default queue */
         fprintf(fs, "      <td align='center'><input type='checkbox' name='enbl' %s></td>\n", qObj->enable? "checked" : "");
         fprintf(fs, "      <td align='center'>&nbsp</td>\n");
      }
      else
      {
         fprintf(fs, "      <td align='center'><input type='checkbox' name='enbl' %s></td>\n", qObj->enable? "checked" : "");
         fprintf(fs, "      <td align='center'><input type='checkbox' name='rml'></td>\n");
      }

      fprintf(fs, "  </tr>\n");
      fflush(fs);
      cmsObj_free((void **)&qObj);
      glbEntryCt++;
   }
}

#ifdef BRCM_WLAN
void cgiQosWlQueueViewBody_dev2(FILE *fs)
{
   char intfname[CMS_IFNAME_LENGTH];
   InstanceIdStack iidStack;
   Dev2QosQueueObject *qObj = NULL;
   CmsRet ret;

   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_DEV2_QOS_QUEUE, &iidStack, (void **)&qObj)) == CMSRET_SUCCESS)
   {
      if ((strstr(qObj->interface, TR181_WIFI_INTF_PATH) == NULL) || !qObj->enable)
      {
         /* skip non-Wifi queues or disabled Wifi queues. */
         cmsObj_free((void **)&qObj);
         continue;
      }

      if ((ret = qdmIntf_fullPathToIntfnameLocked(qObj->interface, intfname)) != CMSRET_SUCCESS)
      {
         cmsLog_error("qdmIntf_fullPathToIntfnameLocked on %s returns %d",
                      qObj->interface, ret);
         cmsObj_free((void **)&qObj);
         continue;
      }

      fprintf(fs, "    <tr align='center'>\n");

      /* queue name */
      fprintf(fs, "      <td>%s</td>\n", qObj->X_BROADCOM_COM_QueueName);
      
      /* queue key */
      fprintf(fs, "      <td>%d</td>\n", PEEK_INSTANCE_ID(&iidStack));
      
      /* queue interface */
      fprintf(fs, "      <td>%s</td>\n", intfname);

      /* queue id */
      fprintf(fs, "      <td>%d</td>\n", qObj->X_BROADCOM_COM_QueueId);

      /* queue precedence/algorithm/weight */
      if (strcmp(qObj->schedulerAlgorithm, MDMVS_SP) == 0)
      {
         fprintf(fs, "      <td>%d/%s</td>\n",
                 qObj->precedence, qObj->schedulerAlgorithm);
      }
      else
      {
         fprintf(fs, "      <td>%d/%s/%d</td>\n",
                 qObj->precedence, qObj->schedulerAlgorithm, qObj->weight);
      }

      /* queue enable */
      fprintf(fs, "      <td align='center'>%s</td>\n", qObj->enable? "Enabled" : "Disabled");

      fprintf(fs, "  </tr>\n");
      fflush(fs);
      cmsObj_free((void **)&qObj);
   }
}
#endif

void cgiQosQueueRemove_dev2(char *query, FILE *fs)
{
   char cmd[WEB_MD_BUF_SIZE_MAX];
   char lst[WEB_BUF_SIZE_MAX];
   char *last = NULL;
   char *check;
   InstanceIdStack iidStack, iidStackPrev;
   Dev2QosQueueObject *qObj = NULL;
   CmsRet ret;

   lst[0] = '\0';
   if (cgiGetValueByName(query, "rmLst", lst) != CGI_STS_OK)
   {
      return;
   }

   /* loop through the queue table */
   INIT_INSTANCE_ID_STACK(&iidStack);
   iidStackPrev = iidStack;
   while ((ret = cmsObj_getNext(MDMOID_DEV2_QOS_QUEUE, &iidStack, (void **)&qObj)) == CMSRET_SUCCESS)
   {
      if (strstr(qObj->interface, TR181_WIFI_INTF_PATH) != NULL ||
          (qObj->X_BROADCOM_COM_DslLatency >= 0 && qObj->X_BROADCOM_COM_QueueId == 1))
      {
         /* wlan and dsl default queues are static. Can not be removed. */
         cmsObj_free((void **)&qObj);
         continue;
      }

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
         cmsObj_free((void **)&qObj);
         break;   /* done */
      }
      if (atoi(check) == 1)
      {
         cmsLog_debug("Deleting instance=%d QID=%d intf=%s",
                      PEEK_INSTANCE_ID(&iidStack),
                      qObj->X_BROADCOM_COM_QueueId,
                      qObj->interface);

         if ((ret = cmsObj_deleteInstance(MDMOID_DEV2_QOS_QUEUE, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("deleteInstance returns error, ret = %d", ret);
            sprintf(cmd, "Delete queue failed. ret=%d", ret);
            cgiWriteMessagePage(fs, "QoS Queue Config Error", cmd, "qosqueue.cmd?action=viewcfg");
            cmsObj_free((void **)&qObj);
            break;
         }
         glbSaveConfigNeeded = TRUE;

         /* since this instance has been deleted, we want to set the iidStack to
          * the previous instance, so that we can continue to do getNext.
          */
         iidStack = iidStackPrev;
      }
      /* save this iidStack in case we want to do a getNext from this instance */
      iidStackPrev = iidStack;
      
      cmsObj_free((void **)&qObj);
   }
}


void cgiQosQueueEnable_dev2(char *query, FILE *fs)
{
   char cmd[WEB_MD_BUF_SIZE_MAX];
   char lst[WEB_BUF_SIZE_MAX];
   char *last = NULL;
   char *check;
   InstanceIdStack iidStack;
   Dev2QosQueueObject *qObj = NULL;
   CmsRet ret;

   lst[0] = '\0';
   if (cgiGetValueByName(query, "eblLst", lst) != CGI_STS_OK)
   {
      return;
   }

   /* loop through the queue table */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_DEV2_QOS_QUEUE, &iidStack, (void **)&qObj)) == CMSRET_SUCCESS)
   {
      if (strstr(qObj->interface, TR181_WIFI_INTF_PATH) != NULL)
      {
         /* wlan are static. Can not be enabled or disabled. */
         cmsObj_free((void **)&qObj);
         continue;
      }

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
         cmsObj_free((void **)&qObj);
         break;   /* done */
      }
   
      if (atoi(check) == 1)
      {
         if (!qObj->enable)
         {
            qObj->enable = TRUE;
            if ((ret = cmsObj_set(qObj, &iidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("cmsObj_set returns error. ret=%d", ret);
               sprintf(cmd, "Enable queue failed. ret=%d", ret);
               cgiWriteMessagePage(fs, "QoS Queue Config Error", cmd, "qosqueue.cmd?action=viewcfg");
               cmsObj_free((void **)&qObj);
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
         if (qObj->enable)
         {
            qObj->enable = FALSE;
            if ((ret = cmsObj_set(qObj, &iidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("cmsObj_set returns error. ret=%d", ret);
               sprintf(cmd, "Disable queue failed. ret=%d", ret);
               cgiWriteMessagePage(fs, "QoS Queue Config Error", cmd, "qosqueue.cmd?action=viewcfg");
               cmsObj_free((void **)&qObj);
               break;
            }
            else
            {
               glbSaveConfigNeeded = TRUE;
            }
         }
      }
      cmsObj_free((void **)&qObj);
   }
}


void cgiGetQosQueueInfo_dev2(int argc __attribute__((unused)),
                        char **argv,
                        char *varValue)
{
   InstanceIdStack iidStack;
   Dev2QosQueueObject *qObj = NULL;
   char varValue1[BUFLEN_8];
   char varValue2[BUFLEN_64];
   char info[BUFLEN_32];
   char l2IntfNameBuf[CMS_IFNAME_LENGTH]={0};
   CmsRet ret;

   varValue[0]  = '\0';

   /* loop through the queue table */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_DEV2_QOS_QUEUE, &iidStack, (void **)&qObj)) == CMSRET_SUCCESS)
   {
      if (!qObj->enable)
      {
         cmsObj_free((void **)&qObj);
         continue;
      }

      /* convert mdm full path string to queue interface name */
      if ((ret = qdmIntf_fullPathToIntfnameLocked(qObj->interface, l2IntfNameBuf)) != CMSRET_SUCCESS)
      {
         cmsLog_error("qdmIntf_fullPathToIntfnameLocked on %s ret=%d",
                       qObj->interface, ret);
         cmsObj_free((void **)&qObj);
         continue;
      }

      /* varValue 1 contains only the instance id of this queue */
      sprintf(varValue1, "%d:", PEEK_INSTANCE_ID(&iidStack));

      /* Now fill in varValue2 with a bunch of info */
      varValue2[0] = '\0';
      if (qObj->X_BROADCOM_COM_DslLatency != -1)
      {
         sprintf(info, "&Path%d", qObj->X_BROADCOM_COM_DslLatency);
         strcat(varValue2, info);
      }
      if (qObj->X_BROADCOM_COM_PtmPriority != -1)
      {
         sprintf(info, "&%s", qObj->X_BROADCOM_COM_PtmPriority? "Hi" : "Lo");
         strcat(varValue2, info);
      }
      sprintf(info, "&Key%d", PEEK_INSTANCE_ID(&iidStack));
      strcat(varValue2, info);
      sprintf(info, "&Pre%d", qObj->precedence);
      strcat(varValue2, info);
      if (strcmp(qObj->schedulerAlgorithm, MDMVS_SP))
      {
         sprintf(info, "&Wt%d", qObj->weight);
         strcat(varValue2, info);
      }

      if (dalWan_isInterfaceVlanEnabled(l2IntfNameBuf))
      {
         strcat(varValue2, ":1"); /* vlanmux enabled */
      }
      else
      {
         strcat(varValue2, ":0"); /* vlanmux disabled */
      }
      strcat(varValue2, "|");

      if (qdmIntf_isLayer2IntfNameUpstreamLocked(l2IntfNameBuf))
      {
         Dev2IpInterfaceObject *ipIntfObj = NULL;
         InstanceIdStack iidStack2 = EMPTY_INSTANCE_ID_STACK;

         /*
          * This queue intf is WAN side egress.  Find all the layer 3
          * interfaces that are configured on top of this layer 2 intf,
          * e.g. ptm0 ==> ppp0.1
          */
         while (cmsObj_getNext(MDMOID_DEV2_IP_INTERFACE, &iidStack2,
                               (void **)&ipIntfObj) == CMSRET_SUCCESS)
         {
            if (ipIntfObj->X_BROADCOM_COM_Upstream)
            {
               char thisL2IntfNameBuf[CMS_IFNAME_LENGTH]={0};
               CmsRet r2;

               r2 = qdmIpIntf_getLayer2IntfNameByLayer3IntfNameLocked(ipIntfObj->name, thisL2IntfNameBuf);
               if ((r2 == CMSRET_SUCCESS) &&
                   (cmsUtl_strcmp(l2IntfNameBuf, thisL2IntfNameBuf) == 0))
               {
                  sprintf(info, "%s", ipIntfObj->name);
                  
                  if ((strcmp(argv[2], "all") == 0) || (strstr(info, argv[2]) != NULL))
                  {
                     strcat(info, "(wan)");
                     /* varValue1 contains "instanceId:" */
                     strcat(varValue, varValue1);
                     strcat(varValue, info);
                     /* varValue2 contains queue scheduling/priority info */
                     strcat(varValue, varValue2);
                  }
               }
            }

            cmsObj_free((void **)&ipIntfObj);
         }
      }
      else if ((strcmp(argv[2], "all") == 0) || (strstr(l2IntfNameBuf, argv[2]) != NULL))
      {
         /* This queue is LAN side egress: just use layer 2 intfName */
         strcat(varValue, varValue1);
         strcat(varValue, l2IntfNameBuf);
         strcat(varValue, varValue2);
      }

      cmsObj_free((void **)&qObj);
   }
}


void cgiGetQueueIntf_dev2(int argc __attribute__((unused)),
                     char **argv __attribute__((unused)),
                     char *varValue)
{
   InstanceIdStack iidStack;
   void *mdmObj  = NULL;
   char desc[BUFLEN_32];
   CmsRet ret;

   varValue[0] = '\0';
   desc[0]     = '\0';

   /* get LAN Ethernet interfaces */
#ifdef DMP_DEVICE2_ETHERNETINTERFACE_1
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_DEV2_ETHERNET_INTERFACE, &iidStack, &mdmObj)) == CMSRET_SUCCESS)
   {
      if (((Dev2EthernetInterfaceObject *)mdmObj)->enable &&
          ((Dev2EthernetInterfaceObject *)mdmObj)->upstream == FALSE)
      {
         strcat(varValue, ((Dev2EthernetInterfaceObject *)mdmObj)->name);
         strcat(varValue, "&");
         cgiGetIntfQueuePrec(((Dev2EthernetInterfaceObject *)mdmObj)->name, FALSE, desc);
         strcat(varValue, desc);
         strcat(varValue, "|");
      }
      cmsObj_free(&mdmObj);
   }
   if (ret != CMSRET_NO_MORE_INSTANCES && ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_getNext DEV2_ETHERNET_INTERFACE returns error. ret=%d", ret);
      return;
   }

#ifdef SUPPORT_ETHWAN
   /* get WAN Ethernet interfaces */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_DEV2_ETHERNET_INTERFACE, &iidStack, &mdmObj)) == CMSRET_SUCCESS)
   {
      if (((Dev2EthernetInterfaceObject *)mdmObj)->enable &&
          ((Dev2EthernetInterfaceObject *)mdmObj)->upstream == TRUE)
      {
         strcat(varValue, ((Dev2EthernetInterfaceObject *)mdmObj)->name);
         strcat(varValue, "(wan)");
         strcat(varValue, "&");
         cgiGetIntfQueuePrec(((Dev2EthernetInterfaceObject *)mdmObj)->name, TRUE, desc);
         strcat(varValue, desc);
         strcat(varValue, "|");
      }
      cmsObj_free(&mdmObj);
   }
   if (ret != CMSRET_NO_MORE_INSTANCES && ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_getNext DEV2_ETHERNET_INTERFACE returns error. ret=%d", ret);
      return;
   }
#endif  /* SUPPORT_ETHWAN */

#endif  /* DMP_DEVICE2_ETHERNETINTERFACE_1 */

#ifdef todo_later
#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
   /* get enabled gpon wan interface */
   INIT_INSTANCE_ID_STACK(&iidStack);
   /* Get Gpon WanDevice iidStack first */      
   if (dalGpon_getGponWanIidStatck(&iidStack) == CMSRET_SUCCESS)
   {
      InstanceIdStack gponLinkIid = EMPTY_INSTANCE_ID_STACK;   
      WanGponLinkCfgObject *gponLinkCfg = NULL;
      
      /* go over all the wan gpon link config objects to find the enabled gpon interface and add it to the list  */
      while  ((ret = cmsObj_getNextInSubTreeFlags
         (MDMOID_WAN_GPON_LINK_CFG, &iidStack, &gponLinkIid, OGF_NO_VALUE_UPDATE, (void **)&gponLinkCfg)) == CMSRET_SUCCESS)
      {
         if (gponLinkCfg->enable)
         {
            strcat(varValue, gponLinkCfg->ifName);
            strcat(varValue, "(wan)");
            strcat(varValue, "&");
            cgiGetIntfQueuePrec(gponLinkCfg->ifName, TRUE, desc);
            strcat(varValue, desc);
            strcat(varValue, "|");
         }
         cmsObj_free((void **)&gponLinkCfg);
      }
   }
#endif /* DMP_X_BROADCOM_COM_GPONWAN_1 */

#ifdef DMP_X_BROADCOM_COM_EPONWAN_1
   /* get enabled epon wan interface */
   INIT_INSTANCE_ID_STACK(&iidStack);
   /* Get Epon WanDevice iidStack first */      
   if (dalEpon_getEponWanIidStatck(&iidStack) == CMSRET_SUCCESS)
   {
      InstanceIdStack eponLinkIid = EMPTY_INSTANCE_ID_STACK;   
      WanEponLinkCfgObject *eponLinkCfg = NULL;

      /* go over all the wan epon link config objects to find the enabled epon interface and add it to the list  */
      while  ((ret = cmsObj_getNextInSubTreeFlags
         (MDMOID_WAN_EPON_LINK_CFG, &iidStack, &eponLinkIid, OGF_NO_VALUE_UPDATE, (void **)&eponLinkCfg)) == CMSRET_SUCCESS)
      {
         if (eponLinkCfg->enable)
         {
            strcat(varValue, eponLinkCfg->ifName);
            strcat(varValue, "(wan)");
            strcat(varValue, "&");
            cgiGetIntfQueuePrec(eponLinkCfg->ifName, TRUE, desc);
            strcat(varValue, desc);			
            strcat(varValue, "|");
            cmsObj_free((void **)&eponLinkCfg);
            break;			
         }
         cmsObj_free((void **)&eponLinkCfg);
      }
   }
#endif /* DMP_X_BROADCOM_COM_EPONWAN_1 */

#ifdef SUPPORT_MOCA
#ifdef DMP_X_BROADCOM_COM_MOCALAN_1
   /* get LAN Ethernet interfaces */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_LAN_MOCA_INTF, &iidStack, &mdmObj)) == CMSRET_SUCCESS)
   {
      if (((LanMocaIntfObject *)mdmObj)->enable)
      {
         strcat(varValue, ((LanMocaIntfObject *)mdmObj)->ifName);
         strcat(varValue, "&");
         cgiGetIntfQueuePrec(((LanMocaIntfObject *)mdmObj)->ifName, FALSE, desc);
         strcat(varValue, desc);
         strcat(varValue, "|");
      }
      cmsObj_free(&mdmObj);
   }
   if (ret != CMSRET_NO_MORE_INSTANCES && ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_getNext <MDMOID_LAN_MOCA_INTF> returns error. ret=%d", ret);
      return;
   }
#endif
#ifdef DMP_X_BROADCOM_COM_MOCAWAN_1
   /* get WAN Moca interfaces */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_WAN_MOCA_INTF, &iidStack, &mdmObj)) == CMSRET_SUCCESS)
   {
      if (((WanMocaIntfObject *)mdmObj)->enable)
      {
         strcat(varValue, ((WanMocaIntfObject *)mdmObj)->ifName);
         strcat(varValue, "(wan)");
         strcat(varValue, "&");
         cgiGetIntfQueuePrec(((WanMocaIntfObject *)mdmObj)->ifName, TRUE, desc);
         strcat(varValue, desc);
         strcat(varValue, "|");
      }
      cmsObj_free(&mdmObj);
   }
   if (ret != CMSRET_NO_MORE_INSTANCES && ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_getNext <MDMOID_WAN_MOCA_INTF> returns error. ret=%d", ret);
      return;
   }
#endif
#endif
#endif  /* todo_later */

#ifdef DMP_DEVICE2_ATMLINK_1
   /* get ATM (link) interfaces */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_DEV2_ATM_LINK, &iidStack, (void **)&mdmObj)) == CMSRET_SUCCESS)
   {
      if (((Dev2AtmLinkObject *)mdmObj)->enable &&
          ((Dev2AtmLinkObject *)mdmObj)->X_BROADCOM_COM_ATMEnbQos)
      {
         SINT32 vpi, vci;

         cmsUtl_atmVpiVciStrToNum_dev2(((Dev2AtmLinkObject *)mdmObj)->destinationAddress, &vpi, &vci);
         
         sprintf(desc, "%s[%d_%d_%d]",
                        ((Dev2AtmLinkObject *)mdmObj)->name,
                        ((Dev2AtmLinkObject *)mdmObj)->X_BROADCOM_COM_ATMInterfaceId,
                        vpi,
                        vci);
         strcat(varValue, desc);
         strcat(varValue, "&");
         cgiGetIntfQueuePrec(((Dev2AtmLinkObject *)mdmObj)->name, TRUE, desc);
         strcat(varValue, desc);
         strcat(varValue, "|");
      }
      cmsObj_free(&mdmObj);
   }
#endif /* DMP_DEVICE2_ATMLINK_1 */

#ifdef DMP_DEVICE2_PTMLINK_1
   /* get PTM (link) interfaces */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_DEV2_PTM_LINK, &iidStack, (void **)&mdmObj)) == CMSRET_SUCCESS)
   {
      if (((Dev2PtmLinkObject *)mdmObj)->enable &&
          ((Dev2PtmLinkObject *)mdmObj)->X_BROADCOM_COM_PTMEnbQos)
      {
         sprintf(desc, "%s[%d_%d_%d]",
                        ((Dev2PtmLinkObject *)mdmObj)->name,
                        ((Dev2PtmLinkObject *)mdmObj)->X_BROADCOM_COM_PTMPortId,
                        ((Dev2PtmLinkObject *)mdmObj)->X_BROADCOM_COM_PTMPriorityHigh,
                        ((Dev2PtmLinkObject *)mdmObj)->X_BROADCOM_COM_PTMPriorityLow);
         strcat(varValue, desc);
         strcat(varValue, "&");
         cgiGetIntfQueuePrec(((Dev2PtmLinkObject *)mdmObj)->name, TRUE, desc);
         strcat(varValue, desc);
         strcat(varValue, "|");
      }
      cmsObj_free(&mdmObj);
   }
#endif /* DMP_DEVICE2_PTMLINK_1 */

   if (strlen(varValue) > 0)
   {
      /* get rid of the last '|' */
      varValue[strlen(varValue)-1] = '\0';
   }
   cmsLog_debug("varValue=%s", varValue);

}


CmsRet cgiGetQosQueueSchedulerInfo_dev2(const char *l2Ifname,
                                       char *precArray, SINT32 *numSP,
                                       SINT32 *numWFQ, SINT32 *numWRR)
{
   InstanceIdStack iidStack;
   Dev2QosQueueObject *qObj = NULL;
   char ifname[CMS_IFNAME_LENGTH]={0};
   CmsRet ret;

   *numSP = *numWFQ = *numWRR = 0;
   
   /* loop through the queue table */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_DEV2_QOS_QUEUE, &iidStack, (void **)&qObj)) == CMSRET_SUCCESS)
   {
      if (strstr(qObj->interface, TR181_WIFI_INTF_PATH) != NULL)
      {
         /* skip Wifi queues */
         cmsObj_free((void **)&qObj);
         continue;
      }

      /* convert mdm full path string to queue interface name */
      if ((ret = qdmIntf_fullPathToIntfnameLocked(qObj->interface, ifname)) != CMSRET_SUCCESS)
      {
         cmsLog_error("qdmIntf_fullPathToIntfnameLocked returns error. ret=%d", ret);
         cmsObj_free((void **)&qObj);
         continue;
      }

      if (cmsUtl_strcmp(l2Ifname, ifname) == 0)
      {
         if (cmsUtl_strcmp(qObj->schedulerAlgorithm, MDMVS_WFQ) == 0)
         {
            (*numWFQ)++;
            precArray[qObj->precedence] = CMS_QUEUE_SCHED_WFQ;
         }
         else if (cmsUtl_strcmp(qObj->schedulerAlgorithm, MDMVS_WRR) == 0)
         {
            (*numWRR)++;

            precArray[qObj->precedence] = CMS_QUEUE_SCHED_WRR;
         }
         else
         {
            (*numSP)++;
            precArray[qObj->precedence] = CMS_QUEUE_SCHED_SP;
         }
      }
      cmsObj_free((void **)&qObj);
   }

   return ret;
}




#endif  /* DMP_DEVICE2_QOS_1 */

