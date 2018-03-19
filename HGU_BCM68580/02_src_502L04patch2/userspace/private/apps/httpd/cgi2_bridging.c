/***********************************************************************
 *
 *  Copyright (c) 2013  Broadcom Corporation
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


#ifdef DMP_DEVICE2_BASELINE_1

#ifdef DMP_DEVICE2_BRIDGE_1

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "qdm_intf.h"

#include "httpd.h"
#include "cgi_main.h"

void cgiGetBridgeInterfaceInfo_dev2(char **argv, char *varValue)
{
   InstanceIdStack parentIidStack=EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   Dev2BridgePortObject *brPortObj=NULL;
   char info[BUFLEN_64]={0};


   varValue[0] = '\0';

   if (strcmp(argv[2], "all") == 0)
   {
      /*
       * List all the LAN interfaces that are available for adding to a new bridge,
       * i.e. only list filters that belong to the default bridge group.
       */
      /* Take a small shortcut and assume Bridge.1 is br0, the default bridge group */
      PUSH_INSTANCE_ID(&parentIidStack, 1);

      while (cmsObj_getNextInSubTreeFlags(MDMOID_DEV2_BRIDGE_PORT,
                                     &parentIidStack,
                                     &iidStack,
                                     OGF_NO_VALUE_UPDATE,
                                     (void **) &brPortObj) == CMSRET_SUCCESS)
      {
         if (!brPortObj->managementPort && !qdmIpIntf_isIntfNameUpstreamLocked_dev2(brPortObj->name))
         {
            sprintf(info, "%s|", brPortObj->name);
            strcat(varValue, info);
         }

         cmsObj_free((void **) &brPortObj);
      }
   }
   else if (strcmp(argv[2], "group") == 0)
   {
      /*
       * Get all the interfaces on the bridge group in glbWebVar.groupName.
       * Is this used for edit?  We don't support edit anymore, so delete this code.
       */
      cmsLog_error("get group %s not supported", glbWebVar.groupName);
   }

   if (strlen(varValue) > 0)
   {
      // Remove the trailing | token seperator symbol
      *(varValue + strlen(varValue) - 1) = '\0';
   }

   cmsLog_debug("returning varValue=%s", varValue);
}


void cgiGetDhcpVendorId_dev2(char *varValue)
{
   varValue[0] = '\0';

   cmsLog_error("Not implemented yet");
}


UINT32 cgiGetNumberOfIntfGroups_dev2()
{
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;
   UINT32 numOfBr = 0;

   while ((ret = cmsObj_getNext(MDMOID_DEV2_IP_INTERFACE, &iidStack,
                               (void **)&ipIntfObj)) == CMSRET_SUCCESS)
   {
      if (!ipIntfObj->X_BROADCOM_COM_Upstream)
      {
         numOfBr++;
      }

      cmsObj_free((void **)&ipIntfObj);
   }

   return numOfBr;
}

#ifdef DMP_X_BROADCOM_COM_OPENVSWITCH_1
extern UBOOL8 rutOpenVS_isEnabled(void);
extern UBOOL8 rutOpenVS_isOpenVSPorts(const char *ifName);
#endif

void cgiGetIntfGroupInfo_dev2(UINT32 brIdx,
             char *groupName, UINT32 groupNameLen,
             char *intfList, UINT32 intfListLen, UINT32 *numIntf,
             char *wanIntfList, UINT32 wanIntfListLen, UINT32 *numWanIntf, char *wanIfName,
             char *vendorIdList, UINT32 vendorIdListLen, UINT32 *numVendorIds)
{
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;
   UINT32 i=0;
   UBOOL8 found=FALSE;

   while (!found &&
          (ret = cmsObj_getNext(MDMOID_DEV2_IP_INTERFACE, &iidStack,
                               (void **)&ipIntfObj)) == CMSRET_SUCCESS)
   {
      if (ipIntfObj->X_BROADCOM_COM_Upstream)
      {
         /* only count the Downstream IP interfaces, since that is what
          * our numOfBr was doing.
          */
         cmsObj_free((void **) &ipIntfObj);
         continue;
      }

      if (i != brIdx)
      {
         cmsObj_free((void **) &ipIntfObj);
         i++;
         continue;
      }

      /* this is the (LAN) bridge entry we want, gather info */
      found = TRUE;

      if (ipIntfObj->X_BROADCOM_COM_GroupName)
      {
         cmsUtl_strncpy(groupName, ipIntfObj->X_BROADCOM_COM_GroupName, groupNameLen);
      }

      /* Get the LAN side interfaces associated with this intf group */
      {
         Dev2BridgeObject *brObj=NULL;
         Dev2BridgePortObject *brPortObj=NULL;
         InstanceIdStack brIidStack = EMPTY_INSTANCE_ID_STACK;
         InstanceIdStack brPortIidStack = EMPTY_INSTANCE_ID_STACK;
         UBOOL8 found2=FALSE;

         while (!found2 &&
                (cmsObj_getNextFlags(MDMOID_DEV2_BRIDGE, &brIidStack,
                                           OGF_NO_VALUE_UPDATE,
                                           (void **) &brObj) == CMSRET_SUCCESS))
         {
            if (!cmsUtl_strcmp(brObj->X_BROADCOM_COM_IfName, ipIntfObj->name))
            {
               found2 = TRUE;

               /* loop over all the non-mgmt ports in this bridge */
               while (cmsObj_getNextInSubTree(MDMOID_DEV2_BRIDGE_PORT,
                                           &brIidStack, &brPortIidStack,
                                           (void **) &brPortObj) == CMSRET_SUCCESS)
               {
                  if (!brPortObj->managementPort)
                  {
                     /*
                      * For now, assume no VLAN on LAN side, so every LAN
                      * side bridge obj should point directly down to the
                      * Layer 2 intf.
                      */
#ifndef SUPPORT_LANVLAN
                     if (qdmIntf_isFullPathLayer2Locked_dev2(brPortObj->lowerLayers) &&
                         !qdmIntf_isLayer2FullPathUpstreamLocked_dev2(brPortObj->lowerLayers))
#endif
                     {
                        if (intfList[0] != '\0')
                        {
                           cmsUtl_strncat(intfList, intfListLen, "|");
                        }
                        cmsUtl_strncat(intfList, intfListLen, brPortObj->name);
                        (*numIntf)++;
                     }
                  }

                  cmsObj_free((void **) &brPortObj);
               }
            }

            cmsObj_free((void **) &brObj);
         }
      }

      /*
       * Find the layer 2 or layer 3 WAN interface associated with this
       * intf group.  Special case for "Default" group: list all WAN
       * interfaces not associated with any other intf group.
       */
      {
         Dev2IpInterfaceObject *wanIpIntfObj = NULL;
         InstanceIdStack wanIidStack = EMPTY_INSTANCE_ID_STACK;

         while (cmsObj_getNext(MDMOID_DEV2_IP_INTERFACE, &wanIidStack,
                               (void **)&wanIpIntfObj) == CMSRET_SUCCESS)
         {
            if (wanIpIntfObj->X_BROADCOM_COM_Upstream)
            {
               if (!cmsUtl_strcmp(wanIpIntfObj->X_BROADCOM_COM_BridgeName, ipIntfObj->name) ||
                   (IS_EMPTY_STRING(wanIpIntfObj->X_BROADCOM_COM_BridgeName) &&
                    !cmsUtl_strcmp(ipIntfObj->X_BROADCOM_COM_GroupName, "Default")))
               {
#ifdef DMP_X_BROADCOM_COM_OPENVSWITCH_1
                  if( (rutOpenVS_isEnabled()) && (rutOpenVS_isOpenVSPorts(wanIpIntfObj->name)))
                       ;
                  else
                  {
                     if (wanIntfList[0] != '\0')
                     {
                        strcat(wanIntfList, "|");
                     }
                     cmsUtl_strncat(wanIntfList, wanIntfListLen, wanIpIntfObj->name);
                     sprintf(wanIfName, "%s", wanIpIntfObj->name);
                     (*numWanIntf)++;
                  }
#else
                  if (wanIntfList[0] != '\0')
                  {
                     strcat(wanIntfList, "|");
                  }
                  cmsUtl_strncat(wanIntfList, wanIntfListLen, wanIpIntfObj->name);
                  sprintf(wanIfName, "%s", wanIpIntfObj->name);
                  (*numWanIntf)++;
#endif
               }
            }

            cmsObj_free((void **) &wanIpIntfObj);
         }
      }


      /* XXX TODO: also need to list any DHCP vendor id's associated with
       * this interface group.  For now, just touch the vars to suppress
       * compiler warnings.
       */
      if (vendorIdListLen > 0)
      {
         vendorIdList[0] = '\0';
         (*numVendorIds) = 0;
      }

      cmsObj_free((void **) &ipIntfObj);
   }

   return;
}

#endif  /* DMP_DEVICE2_BRIDGE_1 */

#endif  /* DMP_DEVICE2_BASELINE_1 */

