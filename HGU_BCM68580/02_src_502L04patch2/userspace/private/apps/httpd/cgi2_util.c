/*
* <:copyright-BRCM:2011:proprietary:standard
* 
*    Copyright (c) 2011 Broadcom 
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
:>
*/

#ifdef DMP_DEVICE2_BASELINE_1

#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "cms_core.h"
#include "cms_util.h"
#include "cgi_main.h"
#include "cgi_util.h"
#include "dal_lan.h"


void cgiPrintGroupList_dev2(char *print)
{
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   int  wsize = 0; 
   char *p = print;

   sprintf(p, "<select size=\"1\" name=\"brName\" onChange='changeBrName()'>\n%n", &wsize);
   p += wsize;

   while (cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, &iidStack, OGF_NO_VALUE_UPDATE,
                              (void **)&ipIntfObj)== CMSRET_SUCCESS)
   {
      if ((!cmsUtl_strncmp(ipIntfObj->name, "br", strlen("br"))) && (ipIntfObj->X_BROADCOM_COM_GroupName))
      {
          sprintf(p, "<option value=\"%s\"> %s\n%n",
                  ipIntfObj->X_BROADCOM_COM_GroupName,
                  ipIntfObj->X_BROADCOM_COM_GroupName,
                  &wsize);
          p += wsize;
      }
      cmsObj_free((void **) &ipIntfObj);
   }

   sprintf(p, " </option>\n%n", &wsize);
   p += wsize;
   sprintf(p, "  </select>\n%n", &wsize);
   p += wsize;
}

void getUptimeString_dev2(char *value)
{
   char uptimeString[BUFLEN_512];
   UINT32 s = 0;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2DeviceInfoObject *deviceInfo = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   if ((ret = cmsObj_get(MDMOID_DEV2_DEVICE_INFO, &iidStack, 0, (void **) &deviceInfo)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get DEV2_DEVICE_INFO, ret=%d", ret);
   }
   else
   {
      s = deviceInfo->upTime;
      cmsObj_free((void **)&deviceInfo);
   }

   cmsTms_getDaysHoursMinutesSeconds(s, uptimeString, sizeof(uptimeString));

   sprintf(value, "%s", uptimeString);
}


#ifdef SUPPORT_TOD
void getPcMacAddr_dev2(char *macAddr)
{
   UBOOL8 found = FALSE;
   Dev2HostObject *hostObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   strcpy(macAddr, "");

   while (found == FALSE && 
          cmsObj_getNextFlags(MDMOID_DEV2_HOST,
                              &iidStack, 
                              OGF_NO_VALUE_UPDATE,
                              (void **)&hostObj) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(glbWebVar.pcIpAddr, hostObj->IPAddress) == 0)
      {
         found = TRUE;
         strcpy(macAddr, hostObj->physAddress);
      }

      cmsObj_free((void **) &hostObj);
   }
}
#endif   /* SUPPORT_TOD */


UBOOL8 cgiWillLanIpChange_dev2(const char *newIpAddr, const char *intfGroupName)
{
   UBOOL8 changed = FALSE, firewallEnabled = FALSE;
   char brIfName[CMS_IFNAME_LENGTH];
   char ipAddr[CMS_IPADDR_LENGTH], subnetMask[CMS_IPADDR_LENGTH];

   memset(brIfName, 0, CMS_IFNAME_LENGTH);
   memset(ipAddr, 0, CMS_IPADDR_LENGTH);
   memset(subnetMask, 0, CMS_IPADDR_LENGTH);

   /* intfGroupName is the Interface Group name, e.g. Default or IPTV */
   /* brIfName is the Linux interface name for the bridge, e.g. br0 */
   dalLan_getBridgeIfNameFromBridgeName_dev2(intfGroupName, brIfName);

   dalLan_getIpIntfInfo_dev2(FALSE, brIfName, ipAddr, subnetMask, &firewallEnabled);

   if (cmsUtl_strcmp(ipAddr, newIpAddr) != 0)
   {
      changed = TRUE;
   }

   return changed;
}


UBOOL8 isUsedByOtherBridge_dev2(const char *intfGroupName, const char *ipAddr, const char *subnetMask)
{
   CmsRet ret = CMSRET_OBJECT_NOT_FOUND;
   UBOOL8 isInUse = FALSE;
   struct in_addr ip, mask, subnet;
   struct in_addr tmp_ip, tmp_mask, tmp_subnet;
   /* this object is Device.IP.Interface.{i}.IPv4Address.{i}. */
   Dev2Ipv4AddressObject *ipv4Addr = NULL;
   /* this object is Device.IP.Interface.{i}. */
   Dev2IpInterfaceObject *ipIntfObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStackParent = EMPTY_INSTANCE_ID_STACK;

   if (intfGroupName == NULL)
   {
      cmsLog_error("Interface group name is NULL");
      return isInUse;
   }

   if (!inet_aton(ipAddr, &ip) || !inet_aton(subnetMask, &mask))
   {
      cmsLog_error("ip address conversion failed on %s or %s", ipAddr, subnetMask);
      return isInUse;
   }

   subnet.s_addr = ip.s_addr & mask.s_addr;
   
   while (isInUse == FALSE &&
          (ret = cmsObj_getNext(MDMOID_DEV2_IPV4_ADDRESS,
                                &iidStack, (void **) &ipv4Addr)) == CMSRET_SUCCESS)
   {
      memcpy(&iidStackParent, &iidStack, sizeof(InstanceIdStack));

      if ((ret = cmsObj_getAncestor(MDMOID_DEV2_IP_INTERFACE, MDMOID_DEV2_IPV4_ADDRESS,
                                    &iidStackParent, (void **) &ipIntfObj)) == CMSRET_SUCCESS)
      {
         if ((!cmsUtl_strncmp(ipIntfObj->name, "br", strlen("br"))) && (cmsUtl_strcmp(ipIntfObj->X_BROADCOM_COM_GroupName, intfGroupName) != 0))
         {
            if (!inet_aton(ipv4Addr->IPAddress, &tmp_ip) || !inet_aton(ipv4Addr->subnetMask, &tmp_mask))
            {
               cmsLog_error("ip address conversion failed on %s or %s", ipv4Addr->IPAddress, ipv4Addr->subnetMask);
            }
            else
            {
               tmp_subnet.s_addr = tmp_ip.s_addr & tmp_mask.s_addr;

               if (tmp_subnet.s_addr == subnet.s_addr)
               {
                  isInUse = TRUE;
               }
            }
         }            

         cmsObj_free((void **) &ipIntfObj);
      }            

      cmsObj_free((void **) &ipv4Addr);
   }

   return isInUse;
}

#ifdef SUPPORT_DSL
void cgiGetDslLineRate_dev2(int argc, char **argv, char *varValue) 
{
   Dev2DslChannelObject *dslChannelObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found = FALSE;
   UINT32 totalRate=0;
   UBOOL8 bondingStats=FALSE;
#ifdef DMP_DEVICE2_BONDEDDSL_1
   Dev2DslBondingGroupObject *dslBondingGroupObj = NULL;
   Dev2DslBondingGroupStatsTotalObject *dslBondingGroupStatsTotalObj = NULL;
#endif

   cmsLog_debug("argc=%d", argc);

#ifdef DMP_DEVICE2_BONDEDDSL_1
   /* the rate is the aggregated rate of all the bondedChannels */
   while (!bondingStats &&
          cmsObj_getNext(MDMOID_DEV2_DSL_BONDING_GROUP, &iidStack, (void **) &dslBondingGroupObj) == CMSRET_SUCCESS)
   {
      if ((dslBondingGroupObj->enable == TRUE) && 
          ((!cmsUtl_strcmp(dslBondingGroupObj->status, MDMVS_UP)) &&
           (!cmsUtl_strcmp(dslBondingGroupObj->groupStatus, MDMVS_NONE))))
      {
         if (cmsObj_get(MDMOID_DEV2_DSL_BONDING_GROUP_STATS_TOTAL, &iidStack, 0, (void **) &dslBondingGroupStatsTotalObj) == CMSRET_SUCCESS)
         {
            /* argument[2] indicates upstream/downstream, argv[3] bearer for vdsl2,adsl2,adsl2+ */
            if (!cmsUtl_strcmp(argv[2], "0") && !cmsUtl_strcmp(argv[3], "0"))
            {
               totalRate = dslBondingGroupStatsTotalObj->upstreamRate;
            }
            else if (!cmsUtl_strcmp(argv[2], "1") && !cmsUtl_strcmp(argv[3], "0"))
            {
               totalRate = dslBondingGroupStatsTotalObj->downstreamRate;
            }
            sprintf(varValue,"%d",(totalRate/1000));
            bondingStats = TRUE;
            
            cmsObj_free((void **) &dslBondingGroupStatsTotalObj);
         }
      }
      cmsObj_free((void **) &dslBondingGroupObj);
   } /* while */
#endif

#ifdef DMP_DEVICE2_FAST_1
   {
      Dev2FastLineObject *fastLineObj = NULL;
      Dev2FastLineTestParamsObject *testParamObj = NULL;
      InstanceIdStack testIidStack = EMPTY_INSTANCE_ID_STACK;

      if (!bondingStats)
      {
         INIT_INSTANCE_ID_STACK(&iidStack);
         while (!found && 
                cmsObj_getNext(MDMOID_DEV2_FAST_LINE, &iidStack, (void **) &fastLineObj) == CMSRET_SUCCESS)
         {
            if (!cmsUtl_strcmp(fastLineObj->status, MDMVS_UP))
            {
               testIidStack = iidStack;
               if (cmsObj_get(MDMOID_DEV2_FAST_LINE_TEST_PARAMS, &testIidStack, 0, (void **) &testParamObj) == CMSRET_SUCCESS)
               {
                  if (!cmsUtl_strcmp(argv[2], "0") && !cmsUtl_strcmp(argv[3], "0"))
                  {
                     totalRate += (testParamObj->upstreamCurrRate);
                  }
                  else /* (!cmsUtl_strcmp(argv[2], "1") && !cmsUtl_strcmp(argv[3], "0")) */
                  {
                     totalRate += (testParamObj->downstreamCurrRate);
                  }
                  found = TRUE;
                  sprintf(varValue,"%d",(totalRate));
                  cmsObj_free((void **) &testParamObj);
               } /* test parm */
            } /* up */
            cmsObj_free((void **) &fastLineObj);
         } /* fast */
      } /* !bonding */
   }
#endif

   while (!found &&
          cmsObj_getNext(MDMOID_DEV2_DSL_CHANNEL, &iidStack, (void **) &dslChannelObj) == CMSRET_SUCCESS)
   {
      /* argument[2] indicates upstream/downstream, argv[3] bearer for vdsl2,adsl2,adsl2+ */
      if (!cmsUtl_strcmp(argv[2], "0") && !cmsUtl_strcmp(argv[3], "0"))
      {
         if (!bondingStats)
         {
            /* Path/Bearer 0: Up Stream Rate */
            if (!cmsUtl_strcmp(dslChannelObj->status, MDMVS_UP))
            {
               totalRate += (dslChannelObj->upstreamCurrRate * 1000);
               found = TRUE;
            }
            sprintf(varValue,"%d",(totalRate/1000));
         } /* do not have aggregate bonding stats */
      }
      else if (!cmsUtl_strcmp(argv[2], "1") && !cmsUtl_strcmp(argv[3], "0"))
      {
         if (!bondingStats)
         {
            /* Path/Bearer 0: Down Stream Rate */
            if (!cmsUtl_strcmp(dslChannelObj->status, MDMVS_UP))
            {
               totalRate += (dslChannelObj->downstreamCurrRate * 1000);
               found = TRUE;
            }
            sprintf(varValue, "%d",(totalRate/1000));
         } /* do not have aggregate bonding stats */
      }
#ifdef DMP_X_BROADCOM_COM_DSL_1
      else if (!cmsUtl_strcmp(argv[2], "0") && !cmsUtl_strcmp(argv[3], "1"))
      {
         /* Path/Bearer 1: Up Stream Rate */
         if (!cmsUtl_strcmp(dslChannelObj->status, MDMVS_UP))
         {
            sprintf(varValue,"%d",(dslChannelObj->X_BROADCOM_COM_UpstreamCurrRate_2/1000));
            found = TRUE;
         }
      }
      else if (!cmsUtl_strcmp(argv[2], "1") && !cmsUtl_strcmp(argv[3], "1"))
      { 
         /* Path/Bearer 1: Down Stream Rate */
         if (!cmsUtl_strcmp(dslChannelObj->status, MDMVS_UP))
         {
            sprintf(varValue, "%d",(dslChannelObj->X_BROADCOM_COM_DownstreamCurrRate_2/1000));
            found = TRUE;
         }
      }
#endif /* DMP_X_BROADCOM_COM_DSL_1 */
      else
      {
         cmsLog_error("unrecognized request %s/%s", argv[2], argv[3]);
         strcpy(varValue, "&nbsp");
      }
      cmsObj_free((void **) &dslChannelObj);
   
   } /* while */
   
   return;
}
#endif /* SUPPORT_DSL */

#ifdef SUPPORT_DSL
void cgiGetTestVar_dev2(char *varName, char *varValue) 
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2DslLineObject *dslLineObj;
   Dev2DslLineBertTestObject *bertTestObj = NULL;
   UBOOL8 found = FALSE;
   double ratio;

   cmsLog_debug("enter: varName=%s", varName);

   while (!found && (cmsObj_getNext(MDMOID_DEV2_DSL_LINE, &iidStack, (void **)&dslLineObj) == CMSRET_SUCCESS))
   {
      if (dslLineObj->enable && !cmsUtl_strcmp(dslLineObj->status, MDMVS_UP))
      {
         found = TRUE;
      }
      cmsObj_free((void **) &dslLineObj);
   }
   if (found)
   {
      if ((cmsObj_get(MDMOID_DEV2_DSL_LINE_BERT_TEST, &iidStack, 0, (void **) &bertTestObj)) == CMSRET_SUCCESS)
      {
         if ( cmsUtl_strcmp(varName, "berTime") == 0 )
         {
            sprintf(varValue, "%u", bertTestObj->totalTime);
         }
         else if ( cmsUtl_strcmp(varName, "berState") == 0 )
         {
            if (cmsUtl_strcmp(bertTestObj->bertTestStatus,MDMVS_NOT_RUNNING) == 0)
            {
               strcpy(varValue,"0");
            }
            else
            {
               strcpy(varValue,"1");
            }
         }
         else if ( cmsUtl_strcmp(varName, "berTotalBits") == 0 )
         {
            sprintf(varValue, "0x%08X%08X", bertTestObj->bitsTestedCntHigh,
                    bertTestObj->bitsTestedCntLow);
         }
         else if ( cmsUtl_strcmp(varName, "berErrorBits") == 0 )
         {
            sprintf(varValue, "0x%08X%08X", bertTestObj->errBitsCntHigh,bertTestObj->errBitsCntLow);
         }
         else if ( cmsUtl_strcmp(varName, "berErrorRatio") == 0 ) 
         {
            unsigned long long bertTotalBits64 = (unsigned long long)bertTestObj->bitsTestedCntHigh;
            unsigned long long bertErrBits64 = (unsigned long long)bertTestObj->errBitsCntHigh;
            unsigned long long *err64, *tot64;
            
            bertTotalBits64 = (bertTotalBits64 << 32) | bertTestObj->bitsTestedCntLow;
            bertErrBits64 = (bertErrBits64 << 32) | bertTestObj->errBitsCntLow;
            err64 = (unsigned long long *) &bertErrBits64;
            tot64 = (unsigned long long *) &bertTotalBits64;
            if ( *tot64 != 0 ) 
            {
               ratio = (double)(*err64) / (double)(*tot64);
               sprintf(varValue, "%4.2e", ratio);
            } 
            else
            {
               strcpy(varValue, "Not Applicable");
            }
         }
         else if ( cmsUtl_strcmp(varName, "adslTestMode") == 0 )
         {
            sprintf(varValue, "%s", bertTestObj->bertTestMode);
         }
         else
         {
            strcpy(varValue, "");
         }
         cmsObj_free((void **) &bertTestObj);
      } /* bertObj */
   } /* if (found) */
   return;
}
#else
void cgiGetTestVar_dev2(char *varName __attribute__((unused)), char *varValue __attribute__((unused))) 
{
   return;
}
#endif /* SUPPORT_DSL */


#ifdef SUPPORT_DSL
void cgiSetTestVar_dev2(char *varName, char *varValue) 
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2DslLineObject *dslLineObj;
   Dev2DslLineBertTestObject *bertTestObj = NULL;
   UBOOL8 found = FALSE;
   CmsRet ret;
   int time;

   cmsLog_debug("enter: varName=%s, varValue %s", varName,varValue);

   while (!found && (cmsObj_getNext(MDMOID_DEV2_DSL_LINE, &iidStack, (void **)&dslLineObj) == CMSRET_SUCCESS))
   {
      if (dslLineObj->enable && !cmsUtl_strcmp(dslLineObj->status, MDMVS_UP))
      {
         found = TRUE;
      }
      cmsObj_free((void **) &dslLineObj);
   }
   if (found)
   {
      if ((cmsObj_get(MDMOID_DEV2_DSL_LINE_BERT_TEST, &iidStack, 0, (void **) &bertTestObj)) == CMSRET_SUCCESS)
      {
         /* it's already running, don't allow parameter change except user wants to stop the test */
         if ( ( (cmsUtl_strcmp(bertTestObj->bertTestStatus,MDMVS_RUNNING) == 0) &&
                ((cmsUtl_strcmp(varName,"berState") == 0) && (cmsUtl_strcmp(varValue,"0") != 0)) 
                ) ||
              ( (cmsUtl_strcmp(bertTestObj->bertTestStatus,MDMVS_RUNNING) == 0) &&
                (cmsUtl_strcmp(varName,"berState") != 0) 
                ) )
         {
            cmsObj_free((void **) &bertTestObj);
            return;
         }
         if ( cmsUtl_strcmp(varName, "berTime") == 0 )
         {
            time = atol(varValue);
            bertTestObj->bertTestDuration = time;
         }
         else if ( cmsUtl_strcmp(varName, "berState") == 0 )
         {         
            if (cmsUtl_strcmp(varValue,"0") == 0 )
            {
               /* if it's already stopped, just return */
               if (cmsUtl_strcmp(bertTestObj->bertTestMode,MDMVS_STOP) == 0)
               {
                  cmsObj_free((void **) &bertTestObj);
                  return;
               }
               cmsMem_free(bertTestObj->bertTestMode);
               bertTestObj->bertTestMode = cmsMem_strdup(MDMVS_STOP);
            }
            else
            {
               /* starting, reset bertTime timer for user to set it */
               cmsMem_free(bertTestObj->bertTestMode);
               bertTestObj->bertTestMode = cmsMem_strdup(MDMVS_START);
               bertTestObj->bertTestDuration = 0;
            }
         }
         if ((ret = cmsObj_set(bertTestObj, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_set returns error ret %d",ret);
         }
         cmsObj_free((void **) &bertTestObj);
      } /* bertObj */
   } /* if (found) */
   return;
}
#else
void cgiSetTestVar_dev2(char *varName __attribute__((unused)), char *varValue __attribute__((unused))) 
{
   return;
}
#endif /* SUPPORT_DSL */

#endif  /* DMP_DEVICE2_BASELINE_1 */

