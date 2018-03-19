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
* :>
*/

#ifdef DMP_DEVICE2_BASELINE_1

#include "cms.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_dal.h"
#include "qdm_intf.h"
#include "ssk.h"


/*!\file ssk2_linkstatus.c
 *
 * This file attempts to generalize LAN and WAN link status detection
 * functions in TR181.
 */

#ifdef DMP_DEVICE2_ETHERNETINTERFACE_1
static void checkEthLinkStatusLocked_dev2(const char *intfName);
#endif

#ifdef DMP_DEVICE2_WIFIRADIO_1
static void checkWifiLinkStatusLocked_dev2(const char *intfName);
#endif

#ifdef DMP_DEVICE2_MOCA_1
static void checkMocaLinkStatusLocked_dev2(const char *intfName);
#endif

#ifdef DMP_X_BROADCOM_COM_EPONWAN_1
static void checkEponLinkStatusLocked_dev2(const char *intfName);
#endif

#ifdef DMP_DEVICE2_USBINTERFACE_1
static void checkUsbLinkStatusLocked_dev2(void);
#endif

static void updateBridgeMgmtPortLocked_dev2(void);


/* see comment in linkstatus_wan.c */
extern UBOOL8 isRdpaGBEAEsysLockedIn;


void updateLinkStatus_dev2(const char *intfName)
{
   UBOOL8 gotLock=FALSE;
   CmsRet ret;

   cmsLog_debug("Entered: intfName=%s", intfName);

   /*
    * Very bad if we lose a link status change event.  Keep looping until
    * we get the lock.
    */
   while (!gotLock)
   {
      if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not get lock, ret=%d", ret);
         cmsLck_dumpInfo();
      }
      else
      {
         gotLock = TRUE;
      }
   }

   if ((intfName == NULL) ||
       (!strncmp(intfName, ETH_IFC_STR, strlen(ETH_IFC_STR))))
   {
#ifdef DMP_DEVICE2_ETHERNETINTERFACE_1
      checkEthLinkStatusLocked_dev2(intfName);
#endif
      /*
       * We were given a specific eth intf name, so after checking its link
       * status, we can return.
       */
      if (intfName)
      {
         cmsLck_releaseLock();
         return;
      }
   }

   if ((intfName == NULL) ||
       (!strncmp(intfName, WLAN_IFC_STR, strlen(WLAN_IFC_STR))))
   {
#ifdef DMP_DEVICE2_WIFIRADIO_1
      checkWifiLinkStatusLocked_dev2(intfName);
#endif
      /*
       * We were given a specific wl intf name, so after checking its link
       * status, we can return.
       */
      if (intfName)
      {
         cmsLck_releaseLock();
         return;
      }
   }

   if ((intfName == NULL) ||
       (!strncmp(intfName, HOMEPLUG_IFC_STR, strlen(HOMEPLUG_IFC_STR))))
   {
#ifdef DMP_DEVICE2_HOMEPLUG_1
      checkHomePlugLinkStatusLocked(intfName);
#endif
      /*
       * We were given a specific homeplug intf name, so after checking its link
       * status, we can return.
       */
      if (intfName)
      {
         cmsLck_releaseLock();
         return;
      }
   }

   if ((intfName == NULL) ||
       (!strncmp(intfName, MOCA_IFC_STR, strlen(MOCA_IFC_STR))))
   {
#ifdef DMP_DEVICE2_MOCA_1
      checkMocaLinkStatusLocked_dev2(intfName);
#endif
      /*
       * We were given a specific moca intf name, so after checking its link
       * status, we can return.
       */
      if (intfName)
      {
         cmsLck_releaseLock();
         return;
      }
   }

   if ((intfName == NULL) ||
       (!strncmp(intfName, EPON_IFC_STR, strlen(EPON_IFC_STR))))
   {
#ifdef DMP_X_BROADCOM_COM_EPONWAN_1
      checkEponLinkStatusLocked_dev2(intfName);
#endif
      /*
       * We were given a specific epon intf name, so after checking its link
       * status, we can return.
       */
      if (intfName)
      {
         cmsLck_releaseLock();
         return;
      }
   }

   /* GPON reports link status by CMS msg, so don't need to handle here */

   /*
    * We never get specific intfName for DSL and USB, so if we get here,
    * always check.
    */
#ifdef DMP_DEVICE2_USBINTERFACE_1
   checkUsbLinkStatusLocked_dev2();
#endif


#ifdef DMP_DEVICE2_DSL_1
   if (!isRdpaGBEAEsysLockedIn)
   {
      checkDslLinkStatusLocked_dev2();
   }
#endif

   {
      static UBOOL8 firstLinkUpdate=TRUE;
      if (firstLinkUpdate)
      {
         updateBridgeMgmtPortLocked_dev2();
         firstLinkUpdate = FALSE;
      }
   }

   cmsLck_releaseLock();

   return;
}


/** Find all Bridge Management Ports and propagate UP status from them.
 *  This will cause brx interfaces to be UP and have IP addresses, same as TR98.
 */
void updateBridgeMgmtPortLocked_dev2()
{
   Dev2BridgePortObject *brPortObj=NULL;
   InstanceIdStack brPortIidStack=EMPTY_INSTANCE_ID_STACK;


   while (cmsObj_getNext(MDMOID_DEV2_BRIDGE_PORT,
                         &brPortIidStack,
                         (void **) &brPortObj) == CMSRET_SUCCESS)
   {
      if (brPortObj->managementPort)
      {
         cmsLog_debug("propagate UP status on %s",
                      cmsMdm_dumpIidStack(&brPortIidStack));
         intfStack_propagateStatusByIidLocked(MDMOID_DEV2_BRIDGE_PORT,
                                              &brPortIidStack, MDMVS_UP);
      }

      cmsObj_free((void **) &brPortObj);
   }
}


#if defined(DMP_DEVICE2_ETHERNETINTERFACE_1) || defined(DMP_DEVICE2_WIFIRADIO_1) || defined(DMP_DEVICE2_USBINTERFACE_1) || defined(DMP_DEVICE2_MOCA_1)
static void updateLanHostEntryActiveStatus_dev2(const char *ifName, UBOOL8 linkUp)
{
   char *fullPath=NULL;
   Dev2HostObject *hostObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("Entered: ifName=%s, linkUp=%d", ifName, linkUp);

   ret = qdmIntf_intfnameToFullPathLocked(ifName, TRUE, &fullPath);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get fullpath for %s, ret=%d", ifName, ret);
      return;
   }
   else
   {
      cmsLog_debug("ifName %s ==> %s", ifName, fullPath);
   }

   /*
    * Go through all LAN_HOST_ENTRYs and update the status for the specified
    * ifName.
    */
   while (cmsObj_getNextFlags(MDMOID_DEV2_HOST, &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **)&hostObj) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(hostObj->layer1Interface, fullPath) == 0)
      {
         cmsLog_debug("found Dev2HostObject with layer1Interface=%s, ifName=%s",
                      hostObj->layer1Interface, ifName);

         hostObj->active = linkUp;

         if ((ret = cmsObj_set(hostObj, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("set of Dev2HostObject failed, ret=%d (active=%d)",
                         ret, hostObj->active);
         }
      }

      cmsObj_free((void **)&hostObj);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(fullPath);
 
   return;
}
#endif  /* DMP_DEVICE2_ETHERNETINTERFACE_1 || DMP_DEVICE2_WIFIRADIO_1 || DMP_DEVICE2_USBINTERFACE_1 || DEVICE2_MOCA */


#ifdef DMP_DEVICE2_ETHERNETINTERFACE_1
void checkEthLinkStatusLocked_dev2(const char *intfName)
{
   UBOOL8 found = FALSE;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2EthernetInterfaceObject *ethIntfObj=NULL;
   CmsMsgType msgType;
   UBOOL8 isLinkStatusChanged;
//   LanLinkStatusRecord *linkStatusRec=NULL;
//   char bridgeIfName[CMS_IFNAME_LENGTH]={0};

   cmsLog_debug("Enter: intfName=%s", intfName);


   /*
    * Now that we are using the standard Linux netlink mechanism, this
    * function gets called a lot, and reading the enet switch
    * registers can be expensive.  So use OGF_NO_VALUE_UPDATE first to find
    * the exact right ethObj without calling the STL handler function which
    * reads from the enet switch.
    */
   while (!found &&
          cmsObj_getNextFlags(MDMOID_DEV2_ETHERNET_INTERFACE, &iidStack,
                OGF_NO_VALUE_UPDATE, (void **) &ethIntfObj) == CMSRET_SUCCESS)
   {
      if (intfName)
      {
         if (cmsUtl_strcmp(ethIntfObj->name, intfName))
         {
            /* not the intf that has changed status, go to next */
            cmsObj_free((void **) &ethIntfObj);
            continue;
         }
         else
         {
            found = TRUE;
         }
      }

      /*
       * Now do a real read all the way down to the STL handler function and
       * the enet switch.
       */
      cmsLog_debug("doing actual read of %s (upstream=%d)",
                    ethIntfObj->name, ethIntfObj->upstream);
      cmsObj_free((void **) &ethIntfObj);
      if (cmsObj_get(MDMOID_DEV2_ETHERNET_INTERFACE, &iidStack, 0,
                                   (void **) &ethIntfObj) != CMSRET_SUCCESS)
      {
         cmsLog_error("actual get of ethObj failed");
         continue;
      }

      isLinkStatusChanged = comparePreviousLinkStatus(ethIntfObj->name,
                                                      ethIntfObj->upstream,
                                                      ethIntfObj->status);

      /* For cms lockup in updateLinkStatus_dev2()
         Only only add new link state record 
         and don't propagate link status for checkEthLinkStatusLocked_dev2(NULL) */
      if (!intfName && isLinkStatusChanged)
         isLinkStatusChanged = FALSE;

      if (isLinkStatusChanged)
      {
         cmsLog_debug("link change detected on %s (upstream=%d) status=%s",
                       ethIntfObj->name, ethIntfObj->upstream,
                       ethIntfObj->status);

         if (ethIntfObj->upstream)
         {
            if (!cmsUtl_strcmp(ethIntfObj->status, MDMVS_UP))
            {
               printf("(ssk) EthernetWan %s link up\n", ethIntfObj->name);
               msgType = CMS_MSG_WAN_LINK_UP;
            }
            else
            {
               printf("(ssk) EthernetWan %s link down\n", ethIntfObj->name);
               msgType = CMS_MSG_WAN_LINK_DOWN;
            }
         }
         else
         {
            if (!cmsUtl_strcmp(ethIntfObj->status, MDMVS_UP))
            {
               /* this is a LAN side eth link up msg */
               msgType = CMS_MSG_ETH_LINK_UP;
            }
            else
            {
               msgType = CMS_MSG_ETH_LINK_DOWN;
            }
         }

         intfStack_propagateStatusByIidLocked(MDMOID_DEV2_ETHERNET_INTERFACE,
                                              &iidStack, ethIntfObj->status);

         sendStatusMsgToSmd(msgType, ethIntfObj->name);


#ifdef DHCP_AUTO_PORT_ASSIGNMENT_LATER
         if (msgType == CMS_MSG_ETH_LINK_DOWN)
         {
            linkStatusRec = getLanLinkStatusRecord(ethObj->X_BROADCOM_COM_IfName);
            if (linkStatusRec == NULL)
            {
               cmsLog_error("cannot find linkStatusRec for %s", ethObj->X_BROADCOM_COM_IfName);
            }
            else
            {
               if (linkStatusRec->tmpMoved == tmpMoveDone)
               {
                  /*
                   * If we moved this ethernet port to another bridge because
                   * of DHCP vendor id, then if the link goes down, move the
                   * ethernet port back to its original bridge.  But we can't
                   * do it inside this cmsObj_getNext while loop because moving
                   * the LanEthernetInterface object while we are iterating through
                   * them will confuse the MDM.  So just record that we need to do it.
                   */
                  linkStatusRec->moveBack = TRUE;
               }
            }
         }
         else if (msgType == CMS_MSG_ETH_LINK_UP)
         {
            linkStatusRec = getLanLinkStatusRecord(ethObj->X_BROADCOM_COM_IfName);
            if (linkStatusRec == NULL)
            {
               cmsLog_error("cannot find linkStatusRec for %s", ethObj->X_BROADCOM_COM_IfName);
            }
            else
            {
               if (linkStatusRec->tmpMoved == tmpMoveDoing)
               {
                  linkStatusRec->tmpMoved = tmpMoveDone;
               }
            }
         }
#endif  /* DHCP_AUTO_PORT_ASSIGNMENT_LATER */


         if (msgType == CMS_MSG_ETH_LINK_UP)
         {
            updateLanHostEntryActiveStatus_dev2(ethIntfObj->name, TRUE);
         }
         else if (msgType == CMS_MSG_ETH_LINK_DOWN)
         {
            updateLanHostEntryActiveStatus_dev2(ethIntfObj->name, FALSE);
         }
      } /* if (isLinkStatusChanged) */

      cmsObj_free((void **) &ethIntfObj);
   } /* while loop over all ethernet interfaces */


#ifdef DHCP_AUTO_PORT_ASSIGNMENT_LATER
   linkStatusRec = NULL;
   dlist_for_each_entry(linkStatusRec, &lanLinkStatusRecordHead, dlist)
   {
      if (linkStatusRec->moveBack)
      {
         char destBridgeIfName[CMS_IFNAME_LENGTH]={0};

         getStandardBridgeIfNameLocked(linkStatusRec->ifName, destBridgeIfName);

         cmsLog_debug("moving %s from %s back to %s", linkStatusRec->ifName, linkStatusRec->tmpBridgeIfName, destBridgeIfName);
         moveIfNameToBridgeLocked(linkStatusRec->ifName, linkStatusRec->tmpBridgeIfName, destBridgeIfName);

         /* clear all state, everything is back to normal */
         linkStatusRec->tmpMoved = tmpMoveNo;
         linkStatusRec->moveBack = FALSE;
         memset(linkStatusRec->tmpBridgeIfName, 0, sizeof(linkStatusRec->tmpBridgeIfName));
      }
   }
#endif /* DHCP_AUTO_PORT_ASSIGNMENT_LATER */


   return;
}
#endif  /* DMP_DEVICE2_ETHERNETINTERFACE_1 */


#ifdef DMP_DEVICE2_WIFIRADIO_1

/* The lowerLayer pointer of an SSID object points to a Wifi Radio object.
 * Return TRUE if the Wifi Radio object is Upstream (WAN).
 */
static UBOOL8 isSsidLowerLayerRadioUpstream(const char *lowerLayer)
{
   UBOOL8 isUpstream=FALSE;
   MdmPathDescriptor pathDesc = EMPTY_PATH_DESCRIPTOR;
   Dev2WifiRadioObject *wifiRadioObj=NULL;
   CmsRet ret;

   /* I assume SSID lowerLayer will only have one fullpath */
   ret = cmsMdm_fullPathToPathDescriptor(lowerLayer, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not convert lowerLayer %s, ret=%d", lowerLayer, ret);
      return FALSE;
   }

   if (pathDesc.oid != MDMOID_DEV2_WIFI_RADIO)
   {
      cmsLog_error("Expected LowerLayers to point to Wifi Radio (%d) got %d",
                   MDMOID_DEV2_WIFI_RADIO, pathDesc.oid);
      return FALSE;
   }

   ret = cmsObj_get(pathDesc.oid, &pathDesc.iidStack, 0, (void **)&wifiRadioObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get Wifi Radio object, ret=%d", ret);
      return FALSE;
   }

   isUpstream = wifiRadioObj->upstream;

   cmsObj_free((void **)&wifiRadioObj);

   return isUpstream;
}

void checkWifiLinkStatusLocked_dev2(const char *intfName)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2WifiSsidObject *ssidObj = NULL;
   CmsMsgType msgType;
   UBOOL8 isLinkStatusChanged;
   UBOOL8 isUpstream;

   cmsLog_debug("Enter: intfName=%s", intfName);

   while (cmsObj_getNext(MDMOID_DEV2_WIFI_SSID, &iidStack, (void **) &ssidObj) == CMSRET_SUCCESS)
   {

      cmsLog_debug("IfName=%s status=%s", ssidObj->name, ssidObj->status);

      if (intfName &&
          cmsUtl_strcmp(ssidObj->name, intfName))
      {
         /* not the intf that has changed status, go to next */
         cmsObj_free((void **) &ssidObj);
         continue;
      }

      isUpstream = isSsidLowerLayerRadioUpstream(ssidObj->lowerLayers);
      cmsLog_debug("ssid %s LowerLayer %s, isUpstream=%d",
                    ssidObj->name, ssidObj->lowerLayers, isUpstream);

      isLinkStatusChanged = comparePreviousLinkStatus(ssidObj->name,
                                                      isUpstream,
                                                      ssidObj->status);

      if (isLinkStatusChanged)
      {
         if (isUpstream)
         {
            if (!cmsUtl_strcmp(ssidObj->status, MDMVS_UP))
            {
               printf("(ssk) Wifi Wan %s link up\n", ssidObj->name);
               msgType = CMS_MSG_WAN_LINK_UP;
            }
            else
            {
               printf("(ssk) Wifi Wan %s link down\n", ssidObj->name);
               msgType = CMS_MSG_WAN_LINK_DOWN;
            }
         }
         else
         {
            if (!cmsUtl_strcmp(ssidObj->status, MDMVS_UP))
            {
               /* this is a LAN side SSID link up msg */
               msgType = CMS_MSG_WIFI_LINK_UP;
            }
            else
            {
               msgType = CMS_MSG_WIFI_LINK_DOWN;
            }
         }

         intfStack_propagateStatusByIidLocked(MDMOID_DEV2_WIFI_SSID, &iidStack, ssidObj->status);

         sendStatusMsgToSmd(msgType, ssidObj->name);

         if (msgType == CMS_MSG_WIFI_LINK_UP)
         {
            updateLanHostEntryActiveStatus_dev2(ssidObj->name, TRUE);
         }
         else if (msgType == CMS_MSG_WIFI_LINK_DOWN)
         {
            updateLanHostEntryActiveStatus_dev2(ssidObj->name, FALSE);
         }
      }  /* if (isLinkStatusChanged) */

      cmsObj_free((void **) &ssidObj);
   }

   return;
}
#endif /* DMP_DEVICE2_WIFIRADIO_1 */


#ifdef DMP_DEVICE2_USBINTERFACE_1
void checkUsbLinkStatusLocked_dev2()
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2UsbInterfaceObject *usbIntfObj = NULL;
   CmsMsgType msgType;
   UBOOL8 isLinkStatusChanged;

   cmsLog_debug("Enter:");

   while (cmsObj_getNext(MDMOID_DEV2_USB_INTERFACE, &iidStack, (void **) &usbIntfObj) == CMSRET_SUCCESS)
   {
      cmsLog_debug("IfName=%s status=%s upstream=%d",
                   usbIntfObj->name, usbIntfObj->status, usbIntfObj->upstream);

      isLinkStatusChanged = comparePreviousLinkStatus(usbIntfObj->name,
                                                      usbIntfObj->upstream,
                                                      usbIntfObj->status);

      if (isLinkStatusChanged)
      {
         if (usbIntfObj->upstream)
         {
            if (!cmsUtl_strcmp(usbIntfObj->status, MDMVS_UP))
            {
               printf("(ssk) USB Wan %s link up\n", usbIntfObj->name);
               msgType = CMS_MSG_WAN_LINK_UP;
            }
            else
            {
               printf("(ssk) USB Wan %s link down\n", usbIntfObj->name);
               msgType = CMS_MSG_WAN_LINK_DOWN;
            }
         }
         else
         {
            if (!cmsUtl_strcmp(usbIntfObj->status, MDMVS_UP))
            {
               /* this is a LAN side USB link up msg */
               msgType = CMS_MSG_USB_LINK_UP;
            }
            else
            {
               msgType = CMS_MSG_USB_LINK_DOWN;
            }
         }

         intfStack_propagateStatusByIidLocked(MDMOID_DEV2_USB_INTERFACE, &iidStack, usbIntfObj->status);

         sendStatusMsgToSmd(msgType, usbIntfObj->name);

         if (msgType == CMS_MSG_USB_LINK_UP)
         {
            updateLanHostEntryActiveStatus_dev2(usbIntfObj->name, TRUE);
         }
         else if (msgType == CMS_MSG_USB_LINK_DOWN)
         {
            updateLanHostEntryActiveStatus_dev2(usbIntfObj->name, FALSE);
         }
      }  /* if (isLinkStatusChanged) */

      cmsObj_free((void **) &usbIntfObj);
   }

   return;
}
#endif /* DMP_DEVICE2_USBINTERFACE_1*/


#ifdef DMP_DEVICE2_MOCA_1
void checkMocaLinkStatusLocked_dev2(const char *intfName)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2MocaInterfaceObject *mocaIntfObj = NULL;
   CmsMsgType msgType;
   UBOOL8 isLinkStatusChanged;

   cmsLog_debug("Enter: intfName=%s", intfName);

   while (cmsObj_getNext(MDMOID_DEV2_MOCA_INTERFACE, &iidStack, (void **) &mocaIntfObj) == CMSRET_SUCCESS)
   {
      cmsLog_debug("IfName=%s status=%s upstream=%d",
                   mocaIntfObj->name, mocaIntfObj->status, mocaIntfObj->upstream);

      if (intfName &&
          cmsUtl_strcmp(mocaIntfObj->name, intfName))
      {
         /* not the intf that has changed status, go to next */
         cmsObj_free((void **) &mocaIntfObj);
         continue;
      }

      isLinkStatusChanged = comparePreviousLinkStatus(mocaIntfObj->name,
                                                      mocaIntfObj->upstream,
                                                      mocaIntfObj->status);

      if (isLinkStatusChanged)
      {
         if (mocaIntfObj->upstream)
         {
            if (!cmsUtl_strcmp(mocaIntfObj->status, MDMVS_UP))
            {
               printf("(ssk) MoCA Wan %s link up\n", mocaIntfObj->name);
               msgType = CMS_MSG_WAN_LINK_UP;
            }
            else
            {
               printf("(ssk) MoCA Wan %s link down\n", mocaIntfObj->name);
               msgType = CMS_MSG_WAN_LINK_DOWN;
            }
         }
         else
         {
            if (!cmsUtl_strcmp(mocaIntfObj->status, MDMVS_UP))
            {
               /* this is a LAN side Moca link up msg */
               msgType = CMS_MSG_MOCA_LAN_LINK_UP;
            }
            else
            {
               msgType = CMS_MSG_MOCA_LAN_LINK_DOWN;
            }
         }

         intfStack_propagateStatusByIidLocked(MDMOID_DEV2_MOCA_INTERFACE, &iidStack, mocaIntfObj->status);

         sendStatusMsgToSmd(msgType, mocaIntfObj->name);

         if (msgType == CMS_MSG_MOCA_LAN_LINK_UP)
         {
            updateLanHostEntryActiveStatus_dev2(mocaIntfObj->name, TRUE);
         }
         else if (msgType == CMS_MSG_MOCA_LAN_LINK_DOWN)
         {
            updateLanHostEntryActiveStatus_dev2(mocaIntfObj->name, FALSE);
         }
      }  /* if (isLinkStatusChanged) */

      cmsObj_free((void **) &mocaIntfObj);
   }

   return;
}
#endif /* DMP_DEVICE2_MOCA_INTERFACE_1*/

#ifdef DMP_X_BROADCOM_COM_EPONWAN_1
void checkEponLinkStatusLocked_dev2(const char *intfName)
{
   cmsLog_debug("Enter: intfName=%s", intfName);
}
#endif /* DMP_X_BROADCOM_COM_EPONWAN_1 */


/** This function is used for both TR181 WAN and LAN side.  Note that for
 *  the TR181 WAN side, the only WAN_LINK_UP and WAN_LINK_DISABLED
 *  are returned (unlike TR98 WAN side, no intermediate values, e.g.
 *  WAN_LINK_NO_SIGNAL, WAN_LINK_INITIALIZING, are returned)
 */
static void processGetWanLanLinkStatus_dev2(CmsMsgHeader *msg, UBOOL8 isWan)
{
   UINT32 status;
   SskLinkStatusRecord *linkStatusRec;
   char intfNameBuf[CMS_IFNAME_LENGTH]={0};
   UBOOL8 isUp=FALSE;
   CmsRet ret;

   cmsLog_debug("Enter");


   /* set initial value of status to disabled (down) */
   status = isWan ? WAN_LINK_DISABLED : LAN_LINK_DISABLED;


   /* see if caller is asking about a specific interface */
   if (msg->dataLength > sizeof(intfNameBuf))
   {
      cmsLog_error("dataLength (ifName) too long %d", msg->dataLength);
      return;
   }

   if (msg->dataLength > 0)
   {
      strcpy(intfNameBuf, (char *) (msg+1));
   }


   if( (ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS )
   {
      cmsLog_error("could not get lock, ret=%d", ret);
      return;
   }

   dlist_for_each_entry(linkStatusRec, &sskLinkStatusRecordHead, dlist)
   {
      if (linkStatusRec->isWan == isWan)
      {
         if (IS_EMPTY_STRING(intfNameBuf) && linkStatusRec->isLinkUp)
         {
            /* caller did not ask for specific intf, so only break if we find
             * a link up.  Otherwise, keep looking.
             */
            status = (isWan) ? WAN_LINK_UP : LAN_LINK_UP;
            isUp = TRUE;
            break;
         }

         if (!IS_EMPTY_STRING(intfNameBuf) &&
             !strcmp(intfNameBuf, linkStatusRec->ifName))
         {
            /* caller asked for a specific intf and we found it.  So we
             * should stop looking at this point regardless of the answer.
             */
            if (linkStatusRec->isLinkUp)
            {
               status = (isWan) ? WAN_LINK_UP : LAN_LINK_UP;
               isUp = TRUE;
            }
            break;
         }
      }
   }

   /*
    * Sometimes, a caller is asking about a specific intfName that is
    * actually a higher level intf name than what is actually tracked by
    * the link status code.  So if we do not have a link up answer at this
    * point, look at some higher level objects.
    */
   if (!isUp && !IS_EMPTY_STRING(intfNameBuf))
   {
#ifdef DMP_DEVICE2_VLANTERMINATION_1
      if (strstr(intfNameBuf, "."))
      {
         /* intf name is something like ptm0.1, check Ethernet.VlanTermination objs */
         Dev2VlanTerminationObject *ethVlanTermObj=NULL;
         InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
         while ((!isUp) &&
                (ret = cmsObj_getNext(MDMOID_DEV2_VLAN_TERMINATION, &iidStack,
                                (void **) &ethVlanTermObj)) == CMSRET_SUCCESS)
         {
            if (!cmsUtl_strcmp(intfNameBuf, ethVlanTermObj->name))
            {
               status = (isWan) ? WAN_LINK_UP : LAN_LINK_UP;
               isUp = TRUE;
            }
            cmsObj_free((void **)&ethVlanTermObj);
         }
      }
#endif  /* DMP_DEVICE2_VLANTERMINATION_1 */

      if (!strstr(intfNameBuf, "."))
      {
         /* intf name is something like ptm0, check Ethernet.Link objs */
         Dev2EthernetLinkObject *ethLinkObj=NULL;
         InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
         while ((!isUp) &&
                (ret = cmsObj_getNext(MDMOID_DEV2_ETHERNET_LINK, &iidStack,
                                (void **) &ethLinkObj)) == CMSRET_SUCCESS)
         {
            if (!cmsUtl_strcmp(intfNameBuf, ethLinkObj->name))
            {
               status = (isWan) ? WAN_LINK_UP : LAN_LINK_UP;
               isUp = TRUE;
            }
            cmsObj_free((void **)&ethLinkObj);
         }
      }

#ifdef DMP_DEVICE2_ATMLINK_1
      if (strstr(intfNameBuf, "atm") && !strstr(intfNameBuf, "."))
      {
         /* intf name is something like atm0, check Atm.Link objs */
         Dev2AtmLinkObject *atmLinkObj=NULL;
         InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
         while ((!isUp) &&
                (ret = cmsObj_getNext(MDMOID_DEV2_ATM_LINK, &iidStack,
                                (void **) &atmLinkObj)) == CMSRET_SUCCESS)
         {
            if (!cmsUtl_strcmp(intfNameBuf, atmLinkObj->name))
            {
               status = (isWan) ? WAN_LINK_UP : LAN_LINK_UP;
               isUp = TRUE;
            }
            cmsObj_free((void **)&atmLinkObj);
         }
      }
#endif /* #ifdef DMP_DEVICE2_ATMLINK_1 */      

#if defined(DMP_X_BROADCOM_COM_GPONWAN_1)|| defined(DMP_X_BROADCOM_COM_EPONWAN_1)
      if ((strstr(intfNameBuf, GPON_IFC_STR) && !strstr(intfNameBuf, ".")) ||
          (strstr(intfNameBuf, EPON_IFC_STR) && !strstr(intfNameBuf, ".")))
      {
          InstanceIdStack optIntfIid = EMPTY_INSTANCE_ID_STACK;
          OpticalInterfaceObject *optIntfObj = NULL;

          /* Find the ptical interface config object for the given interface name */
          if (dalOptical_getIntfByIfName((char *)intfNameBuf, &optIntfIid, &optIntfObj) == FALSE)
          {
              cmsLog_error("Failed to get dalOptical_getIntfByIfName for %s", intfNameBuf);
          }
          else
          {
               status = (isWan) ? WAN_LINK_UP : LAN_LINK_UP;
               isUp = (cmsUtl_strcmp(optIntfObj->status, MDMVS_UP) == 0);
          }
          cmsObj_free((void **)&optIntfObj);
      }
#endif /* DMP_X_BROADCOM_COM_GPONWAN_1 || DMP_X_BROADCOM_COM_EPONWAN_1 */
   }

   cmsLck_releaseLock();

   cmsLog_debug("replying to 0x%x ifName=%s isWan=%d status=%d",
                msg->src, intfNameBuf, isWan, status);

   /* reuse the existing msg, just reverse some of the fields */
   msg->dst = msg->src;
   msg->src = EID_SSK;
   msg->flags_request = 0;
   msg->flags_response = 1;
   msg->wordData = status;

   if ((ret = cmsMsg_send(msgHandle, msg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("msg send failed, ret=%d",ret);
   }

   return;
}

void processGetWanLinkStatus_dev2(CmsMsgHeader *msg)
{
   UBOOL8 isWan=TRUE;
   processGetWanLanLinkStatus_dev2(msg, isWan);
}


void processGetLanLinkStatus_dev2(CmsMsgHeader *msg)
{
   UBOOL8 isWan=FALSE;
   processGetWanLanLinkStatus_dev2(msg, isWan);
}



#if defined(DMP_X_BROADCOM_COM_EPONWAN_1)
static CmsRet updateEponInterfaceCfg(const EponLinkStatusMsgBody *info, InstanceIdStack *optIntfIid)
{
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 isChanged = FALSE;
   OpticalInterfaceObject *optIntfObj = NULL;

   /* Find the EPON optical interface config object for the given interface name */
   if (dalOptical_getIntfByIfName((char *)info->l2Ifname, optIntfIid, &optIntfObj) == FALSE)
   {
       cmsLog_error("Failed to get dalOptical_getIntfByIfName for %s", info->l2Ifname);
       return CMSRET_INTERNAL_ERROR;
   }

   /* epon0 has been created and up in kernel space. MDM_init will add this interface to data model
    * Any link change, eponapp will notify to ssk then update L2 data model then trigger L3 WAN service status change.
    * 1 meaning link UP; 2 meaning link DOWN which will be translated to NoLink in eponapp then send to ssk.
    */
   if (info->linkStatus == 1 &&
       cmsUtl_strcmp(optIntfObj->status, MDMVS_UP))
   {
       CMSMEM_REPLACE_STRING(optIntfObj->status, MDMVS_UP);
       isChanged = TRUE;
   }
   else if (info->linkStatus == 2 &&
       cmsUtl_strcmp(optIntfObj->status, MDMVS_DOWN))
   {
       CMSMEM_REPLACE_STRING(optIntfObj->status, MDMVS_DOWN);
       isChanged = TRUE;
   }

   if (isChanged == FALSE)
       goto Exit;

   /* save the epon optical interface */
   if ((ret = cmsObj_set(optIntfObj, optIntfIid)) != CMSRET_SUCCESS)
   {
       cmsLog_error("Failed to set OpticalInterfaceObject. ret=%d", ret);
       goto Exit;
   }   

   cmsLog_debug("updating status to: %s", optIntfObj->status);
   intfStack_propagateStatusByIidLocked(MDMOID_OPTICAL_INTERFACE, optIntfIid, optIntfObj->status);
   sendStatusMsgToSmd(info->linkStatus == 1 ? CMS_MSG_WAN_LINK_UP : CMS_MSG_WAN_LINK_DOWN, info->l2Ifname);

Exit:
   cmsObj_free((void **)&optIntfObj);

   return ret;
}

void processEponWanLinkChange_dev2(CmsMsgHeader *msg)
{
   CmsRet ret = CMSRET_SUCCESS;
   EponLinkStatusMsgBody *info = (EponLinkStatusMsgBody *) (msg + 1);
   InstanceIdStack optIntfIid = EMPTY_INSTANCE_ID_STACK;
   CmsMsgHeader replyMsg = EMPTY_MSG_HEADER;

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d", ret);
      cmsLck_dumpInfo();
      return;
   }

   if ((ret = updateEponInterfaceCfg(info, &optIntfIid)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to update OpticalInterfaceObject, ret=%d", ret);
      goto out;
   }

out:   
   /* need to response the msg to eponapp after all is done */
   replyMsg.type = msg->type;
   replyMsg.src = EID_SSK;
   replyMsg.dst = msg->src;
   replyMsg.wordData = ret;
   replyMsg.flags_request = 0;
   replyMsg.flags_response = 1;

   if ((ret = cmsMsg_send(msgHandle, &replyMsg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("msg send failed, ret=%d",ret);
   }

   cmsLck_releaseLock();
}
#endif /* DMP_X_BROADCOM_COM_EPONWAN_1 */

#if defined(DMP_X_BROADCOM_COM_GPONWAN_1)
static CmsRet updateGponInterfaceCfg(const OmciServiceMsgBody *info, InstanceIdStack *optIntfIid)
{
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 isChanged = FALSE;
   OpticalInterfaceObject *optIntfObj = NULL;

   if (info->serviceParams.pbits != -1 && info->serviceParams.vlanId != -1)
   {
       cmsLog_debug("Skipping OMCI Service pbit=%d vlan=%d", info->serviceParams.pbits, info->serviceParams.vlanId);
       goto Exit;
   }

   /* Find the GPON optical interface config object for the given interface name */
   if (dalOptical_getIntfByIfName((char *)info->l2Ifname, optIntfIid, &optIntfObj) == FALSE)
   {
       cmsLog_error("Failed to get dalOptical_getIntfByIfName for %s", info->l2Ifname);
       return CMSRET_INTERNAL_ERROR;
   }

   /* Even if gpon interface is not created yet ie. not enabled by userapps like WebUI or tr69,
   * just save the gpon interface link info for later use.  This is for the case  when the interface 
   * is created after the interface is already up.
   */
   if (info->serviceParams.serviceStatus == TRUE &&
       cmsUtl_strcmp(optIntfObj->status, MDMVS_UP))
   {
       CMSMEM_REPLACE_STRING(optIntfObj->status, MDMVS_UP);
       isChanged = TRUE;
   }
   else if (info->serviceParams.serviceStatus == FALSE &&
       cmsUtl_strcmp(optIntfObj->status, MDMVS_DOWN))
   {
       CMSMEM_REPLACE_STRING(optIntfObj->status, MDMVS_DOWN);
       isChanged = TRUE;
   }

   if (isChanged == FALSE)
       goto Exit;

   /* save the epon optical interface */
   if ((ret = cmsObj_set(optIntfObj, optIntfIid)) != CMSRET_SUCCESS)
   {
       cmsLog_error("Failed to set OpticalInterfaceObject. ret=%d", ret);
       goto Exit;
   }   

   cmsLog_debug("Updating status to: %s", optIntfObj->status);
   intfStack_propagateStatusByIidLocked(MDMOID_OPTICAL_INTERFACE, optIntfIid, optIntfObj->status);
   sendStatusMsgToSmd(info->serviceParams.serviceStatus == TRUE ? CMS_MSG_WAN_LINK_UP : CMS_MSG_WAN_LINK_DOWN, info->l2Ifname);

Exit:
   cmsObj_free((void **)&optIntfObj);

   return ret;
}

void processGponWanServiceStatusChange_dev2(CmsMsgHeader *msg)
{
   CmsRet ret = CMSRET_SUCCESS;
   OmciServiceMsgBody *info = (OmciServiceMsgBody *) (msg + 1);
   InstanceIdStack optIntfIid = EMPTY_INSTANCE_ID_STACK;
   CmsMsgHeader replyMsg = EMPTY_MSG_HEADER;

   cmsLog_debug("\nifname=%s gemIndex=%d, gemPortId=%d, pbits=%d, vlanid=%d, serviceType=%d, serviceStatus=%d", 
                info->l2Ifname, info->linkParams.gemPortIndex,  info->linkParams.portID, info->serviceParams.pbits, 
                info->serviceParams.vlanId, info->linkParams.serviceType, info->serviceParams.serviceStatus);

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d", ret);
      cmsLck_dumpInfo();
      return;
   }

   if ((ret = updateGponInterfaceCfg(info, &optIntfIid)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to update OpticalInterfaceObject, ret=%d", ret);
      goto out;
   }

out:   
   /* need to response the msg to eponapp after all is done */
   replyMsg.type = msg->type;
   replyMsg.src = EID_SSK;
   replyMsg.dst = msg->src;
   replyMsg.wordData = ret;
   replyMsg.flags_request = 0;
   replyMsg.flags_response = 1;

   if ((ret = cmsMsg_send(msgHandle, &replyMsg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("msg send failed, ret=%d",ret);
   }

   cmsLck_releaseLock();
}
#endif /* DMP_X_BROADCOM_COM_GPONWAN_1 */

#endif  /* DMP_DEVICE2_BASELINE_1 */

