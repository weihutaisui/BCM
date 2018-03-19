/***********************************************************************
 *
 *  Copyright (c) 2011  Broadcom Corporation
 *  All Rights Reserved
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


#include "cms.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_dal.h"
#include "cms_msg.h"
#include "cms_qos.h"
#include "ssk.h"
#include "oal.h"


/** head of the doubly linked list of SskLinkStatusRecords */
DLIST_HEAD(sskLinkStatusRecordHead);



#ifdef DMP_ETHERNETLAN_1
static UBOOL8 checkLanEthLinkStatusLocked_igd(const char *intfName);
#endif

#ifdef DMP_WIFILAN_1
static UBOOL8 checkLanWifiLinkStatusLocked_igd(const char *intfName);
#endif

#ifdef DMP_USBLAN_1
static void checkUsbLinkStatusLocked_igd(void);
#endif

#ifdef DMP_X_BROADCOM_COM_MOCALAN_1
static UBOOL8 checkMocaLanLinkStatusLocked_igd(const char *intfName);
#endif

#ifdef DMP_BASELINE_1
static CmsRet getBridgeIfName(const InstanceIdStack *iidStack, char *ifName, UINT32 ifNameLen);
static void updateLanDhcpcConnectionStatus(const char *bridgeIfName, UBOOL8 linkup);
#endif

#ifdef DMP_BRIDGING_1
static void getVendorIdBridgeIfNameLocked(const char *vendor_id, char *bridgeIfName);
static void getStandardBridgeIfNameLocked(const char *ifName, char *bridgeIfName);
static void moveIfNameToBridgeLocked(const char *ifName, const char *srcBridgeIfName, const char *destBridgeIfName);
#endif


CmsRet rutLan_moveEthInterface(const char *ethIfName, const char *fromBridgeIfName, const char *toBridgeIfName);
CmsRet rutLan_moveUsbInterface(const char *usbIfName, const char *fromBridgeIfName, const char *toBridgeIfName);
CmsRet rutLan_moveWlanInterface(const char *ifName, const char *fromBridgeIfName, const char *toBridgeIfName);


#ifdef DMP_BASELINE_1
UBOOL8 checkLanLinkStatusLocked_igd(const char *intfName)
{
   cmsLog_debug("Enter: intfName=%s", intfName);

   if ((intfName == NULL) ||
       (!strncmp(intfName, ETH_IFC_STR, strlen(ETH_IFC_STR))))
   {
      UBOOL8 found=FALSE;

#ifdef DMP_ETHERNETLAN_1
      found = checkLanEthLinkStatusLocked_igd(intfName);
#endif
      /*
       * We were given a specific eth intf name, so after checking its link
       * status, we can return.
       */
      if (intfName)
      {
         return found;
      }
   }


   if ((intfName == NULL) ||
       (!strncmp(intfName, WLAN_IFC_STR, strlen(WLAN_IFC_STR))))
   {
      UBOOL8 found=FALSE;

#ifdef DMP_WIFILAN_1
      found = checkLanWifiLinkStatusLocked_igd(intfName);
#endif
      /*
       * We were given a specific wl intf name, so after checking its link
       * status, we can return.
       */
      if (intfName)
      {
         return found;
      }
   }


   if ((intfName == NULL) ||
       (!strncmp(intfName, MOCA_IFC_STR, strlen(MOCA_IFC_STR))))
   {
      UBOOL8 found=FALSE;

#ifdef DMP_X_BROADCOM_COM_MOCALAN_1
      found = checkMocaLanLinkStatusLocked_igd(intfName);
#endif
      /*
       * We were given a specific moca intf name, so after checking its link
       * status, we can return.
       */
      if (intfName)
      {
         return found;
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
         return TRUE;
      }
   }

   if (intfName)
   {
      cmsLog_debug("CMS: add specific LAN link status support for %s", intfName);
   }


#ifdef DMP_USBLAN_1
   checkUsbLinkStatusLocked_igd();
#endif

   return FALSE;
}


static void updateLanHostEntryActiveStatus_igd(const char *ifName, UBOOL8 linkUp)
{
   LanHostEntryObject *hostEntry=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("Entered: ifName=%s, linkUp=%d", ifName, linkUp);

   /*
    * Go through all LAN_HOST_ENTRYs and update the status for the specified
    * ifName.
    */
   while (cmsObj_getNextFlags(MDMOID_LAN_HOST_ENTRY, &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **)&hostEntry) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(hostEntry->X_BROADCOM_COM_IfName, ifName) == 0)
      {
         cmsLog_debug("found host entry with ifName: %s", hostEntry->X_BROADCOM_COM_IfName);

         hostEntry->active = linkUp;

         if ((ret = cmsObj_set(hostEntry, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("set of host entry failed, ret=%d (active=%d)",
                          ret, hostEntry->active);
         }
      }

      cmsObj_free((void **)&hostEntry);
   }

   return;
}
#endif  /* DMP_BASELINE_1 */


#ifdef DMP_ETHERNETLAN_1
UBOOL8 checkLanEthLinkStatusLocked_igd(const char *intfName)
{
   UBOOL8 found = FALSE;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanEthIntfObject *ethObj=NULL;
   CmsMsgType msgType=CMS_MSG_ETH_LINK_DOWN;
   UBOOL8 isLanLinkStatusChanged;
   SskLinkStatusRecord *linkStatusRec=NULL;
   char bridgeIfName[CMS_IFNAME_LENGTH]={0};

   cmsLog_debug("Enter: intfName=%s", intfName);


   /*
    * Now that we are using the standard Linux netlink mechanism, this
    * function gets called a lot, and reading the enet switch
    * registers can be expensive.  So use OGF_NO_VALUE_UPDATE first to find
    * the exact right ethObj without calling the STL handler function which
    * reads from the enet switch.
    */
   while (!found &&
          cmsObj_getNextFlags(MDMOID_LAN_ETH_INTF, &iidStack,
                     OGF_NO_VALUE_UPDATE, (void **) &ethObj) == CMSRET_SUCCESS)
   {
      if (intfName)
      {
         if (cmsUtl_strcmp(ethObj->X_BROADCOM_COM_IfName, intfName))
         {
            /* not the one that has changed status, go to next */
            cmsObj_free((void **) &ethObj);
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
      cmsLog_debug("doing actual read of %s", ethObj->X_BROADCOM_COM_IfName);
      cmsObj_free((void **) &ethObj);
      if (cmsObj_get(MDMOID_LAN_ETH_INTF, &iidStack, 0, (void **) &ethObj) != CMSRET_SUCCESS)
      {
         cmsLog_error("actual get of ethObj failed");
         continue;
      }

      isLanLinkStatusChanged = comparePreviousLinkStatus(ethObj->X_BROADCOM_COM_IfName,
                                                 FALSE, ethObj->status);

      if (isLanLinkStatusChanged)
      {
         cmsLog_debug("link change detected on %s, new status=%s",
                      ethObj->X_BROADCOM_COM_IfName,  ethObj->status);
         msgType = (cmsUtl_strcmp(ethObj->status, MDMVS_UP) == 0) ? 
                              CMS_MSG_ETH_LINK_UP: CMS_MSG_ETH_LINK_DOWN;

         sendStatusMsgToSmd(msgType, ethObj->X_BROADCOM_COM_IfName);

         if (msgType == CMS_MSG_ETH_LINK_DOWN)
         {
            linkStatusRec = getLinkStatusRecord(ethObj->X_BROADCOM_COM_IfName);
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
            linkStatusRec = getLinkStatusRecord(ethObj->X_BROADCOM_COM_IfName);
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

         if (CMSRET_SUCCESS == getBridgeIfName(&iidStack, bridgeIfName, sizeof(bridgeIfName)))
         {
            updateLanDhcpcConnectionStatus(bridgeIfName, (msgType == CMS_MSG_ETH_LINK_UP));
         }

         updateLanHostEntryActiveStatus_igd(ethObj->X_BROADCOM_COM_IfName, (msgType == CMS_MSG_ETH_LINK_UP));
      }

#ifdef BRCM_VOICE_SUPPORT
      /* special check for voice. */
      if (isVoiceOnLanSide && msgType == CMS_MSG_ETH_LINK_UP)
      {
         initVoiceOnLanSideLocked(ethObj->X_BROADCOM_COM_IfName);
      }
#endif

      cmsObj_free((void **) &ethObj);
   }


#ifdef DMP_BRIDGING_1
   linkStatusRec = NULL;
   dlist_for_each_entry(linkStatusRec, &sskLinkStatusRecordHead, dlist)
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
#endif

   return found;
}
#endif  /* DMP_ETHERNETLAN_1 */


#ifdef DMP_USBLAN_1
void checkUsbLinkStatusLocked_igd(void)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanUsbIntfObject *usbObj=NULL;
   CmsMsgType msgType=CMS_MSG_USB_LINK_DOWN;
   UBOOL8 isLanLinkStatusChanged;

   while (cmsObj_getNext(MDMOID_LAN_USB_INTF, &iidStack, (void **) &usbObj) == CMSRET_SUCCESS)
   {
      cmsLog_debug("IfName=%s, status=%s", usbObj->X_BROADCOM_COM_IfName, usbObj->status);

      isLanLinkStatusChanged = comparePreviousLinkStatus(usbObj->X_BROADCOM_COM_IfName,
                                                FALSE, usbObj->status);

      if (isLanLinkStatusChanged)
      {
         msgType = (cmsUtl_strcmp(usbObj->status, MDMVS_UP) == 0) ? 
                                CMS_MSG_USB_LINK_UP: CMS_MSG_USB_LINK_DOWN;

         sendStatusMsgToSmd(msgType, usbObj->X_BROADCOM_COM_IfName);
      }

      cmsObj_free((void **) &usbObj);
   }

   return;
}

#endif  /* DMP_USBLAN_1 */



#ifdef DMP_WIFILAN_1
UBOOL8 checkLanWifiLinkStatusLocked_igd(const char *intfName)
{
   WlVirtIntfCfgObject *ssidObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE;
   UBOOL8 isLanLinkStatusChanged;
   CmsMsgType msgType;

   cmsLog_debug("Entered: intfName=%s", intfName);

   while (!found &&
          cmsObj_getNext(MDMOID_WL_VIRT_INTF_CFG, &iidStack,
                                        (void **) &ssidObj) == CMSRET_SUCCESS)
   {
      if (intfName && !cmsUtl_strcmp(intfName, ssidObj->wlIfcname))
      {
         found = TRUE;
      }

      isLanLinkStatusChanged = comparePreviousLinkStatus(ssidObj->wlIfcname,
                                                 FALSE, ssidObj->status);

      if (isLanLinkStatusChanged)
      {
         msgType = (cmsUtl_strcmp(ssidObj->status, MDMVS_UP) == 0) ?
                              CMS_MSG_WIFI_LINK_UP: CMS_MSG_WIFI_LINK_DOWN;

         sendStatusMsgToSmd(msgType, ssidObj->wlIfcname);
      }

      cmsObj_free((void **)&ssidObj);
   }

   return found;
}
#endif


#ifdef DMP_X_BROADCOM_COM_MOCALAN_1
UBOOL8 checkMocaLanLinkStatusLocked_igd(const char *intfName)
{
   LanMocaIntfObject *mocaLanObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE;
   UBOOL8 isLanLinkStatusChanged;
   CmsMsgType msgType;

   cmsLog_debug("Entered: intfName=%s", intfName);

   while (!found &&
          cmsObj_getNext(MDMOID_LAN_MOCA_INTF, &iidStack,
                                (void **) &mocaLanObj) == CMSRET_SUCCESS)
   {
      if (intfName && !cmsUtl_strcmp(intfName, mocaLanObj->ifName))
      {
         found = TRUE;
      }

      isLanLinkStatusChanged = comparePreviousLinkStatus(mocaLanObj->ifName,
                                                 FALSE, mocaLanObj->status);

      if (isLanLinkStatusChanged)
      {
         msgType = (cmsUtl_strcmp(mocaLanObj->status, MDMVS_UP) == 0) ?
                                  CMS_MSG_MOCA_LAN_LINK_UP:
                                  CMS_MSG_MOCA_LAN_LINK_DOWN;

         sendStatusMsgToSmd(msgType, mocaLanObj->ifName);

         updateLanHostEntryActiveStatus_igd(mocaLanObj->ifName,
                                       (msgType == CMS_MSG_MOCA_LAN_LINK_UP));
      }

      cmsObj_free((void **)&mocaLanObj);
   }

   return found;
}
#endif


#ifdef DMP_DEVICE2_HOMEPLUG_1
/* this function can bed used in TR98 and TR181 */
void checkHomePlugLinkStatusLocked(const char *intfName)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   HomePlugInterfaceObject *hplugIntfObj=NULL;
   CmsMsgType msgType;   
   char *xLinkStatus;

   cmsLog_debug("Entered: intfName=%s", intfName);

   while (cmsObj_getNext(MDMOID_HOME_PLUG_INTERFACE, &iidStack, (void **) &hplugIntfObj) == CMSRET_SUCCESS)
   {
      cmsLog_debug("IfName=%s, status=%s, numAssoc=%d",
                  hplugIntfObj->name, hplugIntfObj->status, hplugIntfObj->associatedDeviceNumberOfEntries);

      if (intfName &&
          cmsUtl_strcmp(hplugIntfObj->name, intfName))
      {
         cmsLog_error("intfName mismatch, expected %s got %s",
                       hplugIntfObj->name, intfName);
         /* print the warning, but keep going */
      }

      /*
       * For homeplug, an indication of "link" status is based on "Enable" 
       * parameter, which is "false" if PLC signal is mute and "true" otherwise.
       */
      if (hplugIntfObj->enable)
      {
         xLinkStatus = MDMVS_UP;
         msgType = CMS_MSG_HOMEPLUG_LINK_UP;
      }
      else
      {
         xLinkStatus = MDMVS_DOWN;
         msgType = CMS_MSG_HOMEPLUG_LINK_DOWN;
      }

#ifdef DMP_BASELINE_1
      if (!cmsMdm_isDataModelDevice2())
      {
         UBOOL8 isLinkStatusChanged;
         isLinkStatusChanged = comparePreviousLinkStatus(hplugIntfObj->name,
                                                         hplugIntfObj->upstream,
                                                         xLinkStatus);
         if (isLinkStatusChanged)
         {
            /* Hybrid TR98+TR181 */
            /* for now, homeplug is always on br0 */
            updateLanDhcpcConnectionStatus("br0", (msgType == CMS_MSG_HOMEPLUG_LINK_UP));
         }
         cmsLog_debug("sending msg 0x%x (name=%s)", msgType, hplugIntfObj->name);		 
         sendStatusMsgToSmd(msgType, hplugIntfObj->name);      
      }         
#endif

#ifdef DMP_DEVICE2_BASELINE_1
      if (cmsMdm_isDataModelDevice2())
      {
         cmsLog_debug("xLinkStatus %s", xLinkStatus);
         /* PURE TR181 */
         /*
          * intfStack uses "status" as indication of link status, but
          * for homeplug, link status is really indicated by
          * associatedNumberOfDevices.  Here, we pass in the xLinkStatus
          * which is based on associatedNumberOfDevices even though the
          * hplugIntfObj->status is always(?) UP.  Hopefully will not cause
          * problems or confusion later on....
          */

         intfStack_propagateStatusByIidLocked(MDMOID_HOME_PLUG_INTERFACE, &iidStack, xLinkStatus);
         cmsLog_debug("sending msg 0x%x (name=%s)", msgType, hplugIntfObj->name);
         sendStatusMsgToSmd(msgType, hplugIntfObj->name);             
      }
#endif /* DMP_DEVICE2_BASELINE_1 */

      cmsObj_free((void **) &hplugIntfObj);
      
   }

   return;
}

void processGetIFLinkStatus(CmsMsgHeader *msg)
{
   CmsMsgHeader replyMsg = EMPTY_MSG_HEADER;
   CmsRet ret;
   UINT32 status = LAN_LINK_DISABLED;
   char *ifname = (char *) (msg + 1);
   UBOOL8 found = FALSE;

   cmsLog_debug("Enter");

   if( (ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS )
   {
      cmsLog_error("could not get lock, ret=%d", ret);
   }
   else
   {
      InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
#ifndef MDMOID_DEV2_ETHERNET_INTERFACE
      LanEthIntfObject *ethObj=NULL;

      while (!found && 
             cmsObj_getNext(MDMOID_LAN_ETH_INTF, &iidStack, (void **) &ethObj) == CMSRET_SUCCESS)
      {
         cmsLog_debug("IfName=%s, status=%s", ethObj->X_BROADCOM_COM_IfName, ethObj->status);

         if ( (0 == cmsUtl_strcmp(ethObj->X_BROADCOM_COM_IfName, ifname)) )
         {
            if ( (0 == cmsUtl_strcmp(ethObj->status, MDMVS_UP)) )
            {
               status = LAN_LINK_UP;
            }
            found = TRUE;
         }

         cmsObj_free((void **) &ethObj);
      }
#else
      Dev2EthernetInterfaceObject *ethObj=NULL;
      while (!found &&
              cmsObj_getNext(MDMOID_DEV2_ETHERNET_INTERFACE, &iidStack, (void **) &ethObj) == CMSRET_SUCCESS)
      {
         cmsLog_debug("IfName %s, status %s", ethObj->name,  ethObj->status);
         if ( (0 == cmsUtl_strcmp(ethObj->name, ifname)) )
         {
            if ( (0 == cmsUtl_strcmp(ethObj->status, MDMVS_UP)) )
            {
               status = LAN_LINK_UP;
            }
            found = TRUE;
         }
      }
      cmsObj_free((void **) &ethObj);
#endif

      cmsLck_releaseLock();
   }

   cmsLog_debug("%s link status=%d", ifname, status);

   replyMsg.type = msg->type;
   replyMsg.src = EID_SSK;
   replyMsg.dst = msg->src;
   replyMsg.flags_request = 0;
   replyMsg.flags_response = 1;
   replyMsg.wordData = status;

   if ((ret = cmsMsg_send(msgHandle, &replyMsg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("msg send failed, ret=%d",ret);
   }

   return;
}
#endif  /* DMP_DEVICE2_HOMEPLUG_1 */


/** Compare new ifName link status state with previous state.  
 * Create a link state record for the specified ifName if it does not
 * exist.  The new link state record is initially in the "link down" state.
 *
 *@return TRUE if link status has changed
 */
UBOOL8 comparePreviousLinkStatus(const char *ifName, UBOOL8 isWan, const char *status)
{
   SskLinkStatusRecord *linkStatusRec;
   UBOOL8 changed=FALSE;

   if ((linkStatusRec = getLinkStatusRecord(ifName)) == NULL)
   {
      if (cmsUtl_strlen(ifName) >= (SINT32) sizeof(linkStatusRec->ifName))
      {
         cmsLog_error("ifName %s too long, max=%d", ifName, CMS_IFNAME_LENGTH);
         return FALSE;
      }

      /* need to create a new record for this lan ifName */
      linkStatusRec = (SskLinkStatusRecord *) cmsMem_alloc(sizeof(SskLinkStatusRecord), ALLOC_ZEROIZE);
      if (linkStatusRec == NULL)
      {
         cmsLog_error("Could not allocate LinkStatusRecord");
         return FALSE;
      }
      else
      {
         cmsLog_debug("added new LinkStatusRecord for %s", ifName);
         sprintf(linkStatusRec->ifName,"%s", ifName);
         linkStatusRec->isWan = isWan;
      }

      dlist_append((DlistNode *) linkStatusRec, &sskLinkStatusRecordHead);
   }


   /* at this point, we have a linkStatusRec */
   if (linkStatusRec->isWan != isWan)
   {
      /*
       * Moving an interface and its associated linkStatusRec to/from LAN
       * and WAN is allowed as long as the current linkstate is not up;
       * meaning you have properly shut down (disabled) the interface
       * before moving it.
       */
      cmsLog_debug("moving linkStatusRecord for %s from %s to %s", ifName,
                   (linkStatusRec->isWan ? "WAN" : "LAN"),
                   (isWan ? "WAN" : "LAN"));

      if (linkStatusRec->isLinkUp)
      {
         cmsLog_error("You must disable %s and allow ssk to get updated before moving", ifName);
         /* oh, well, fix it up anyways */
         linkStatusRec->isLinkUp = FALSE;
      }

      linkStatusRec->isWan = isWan;
   }

   if (linkStatusRec->isLinkUp && cmsUtl_strcmp(status, MDMVS_UP))
   {
      cmsLog_debug("%s went from link up to link down", ifName);
      linkStatusRec->isLinkUp = FALSE;
      changed = TRUE;
   }
   else if (!linkStatusRec->isLinkUp && !cmsUtl_strcmp(status, MDMVS_UP))
   {
      cmsLog_debug("%s went from link down to link up", ifName);
      linkStatusRec->isLinkUp = TRUE;
      changed = TRUE;
   }
   else
   {
      cmsLog_debug("No status change on %s (still at %s)", ifName, status);
   }

   return changed;
}


SskLinkStatusRecord *getLinkStatusRecord(const char *ifName)
{
   SskLinkStatusRecord *linkStatusRec=NULL;
   UBOOL8 found=FALSE;


   dlist_for_each_entry(linkStatusRec, &sskLinkStatusRecordHead, dlist)
   {
#ifdef SUPPORT_LANVLAN
      /* For VLAN LAN, only compare the part without vlan extension; ie. only the "eth1" part since ifName 
      * is "eth1.0" here
      */
#ifdef BRCM_WLAN
    /* For WIFI interface, need to compare the entire string  */
      if(cmsUtl_strstr(ifName, "wl"))
      {
        if (cmsUtl_strcmp(ifName, linkStatusRec->ifName) == 0)
        {
            found=TRUE;
            break;
        }
      }
      else
#endif
      if (cmsUtl_strncmp(ifName, linkStatusRec->ifName, cmsUtl_strlen(linkStatusRec->ifName)) == 0)
#else   
      if (cmsUtl_strcmp(ifName, linkStatusRec->ifName) == 0)
#endif
      
      {
         found=TRUE;
         break;
      }
   }

   return (found) ? linkStatusRec : NULL;
}


void cleanupLinkStatusRecords(void)
{
   SskLinkStatusRecord *rec;

   cmsLog_debug("free all sskLinkStatusRecords");

   while (!dlist_empty(&sskLinkStatusRecordHead))
   {
      rec = (SskLinkStatusRecord *) sskLinkStatusRecordHead.next;
      dlist_unlink((DlistNode *) rec);
      cmsMem_free(rec);
   }

   return;
}


#ifdef DMP_BASELINE_1
void processGetLanLinkStatus_igd(CmsMsgHeader *msg)
{
   CmsMsgHeader replyMsg = EMPTY_MSG_HEADER;
   UINT32 status = LAN_LINK_DISABLED;
   UBOOL8 anyLinkOn = FALSE;
   CmsRet ret;

   cmsLog_debug("Enter");

   if( (ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS )
   {
      cmsLog_error("could not get lock, ret=%d", ret);
   }
   else
   {
      InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
      LanEthIntfObject *ethObj=NULL;
#ifdef DMP_USBLAN_1
      LanUsbIntfObject *usbObj=NULL;
      InstanceIdStack iidStack1 = EMPTY_INSTANCE_ID_STACK;
#endif
      /* check LAN Eth */
      while( (!anyLinkOn) &&
             cmsObj_getNext(MDMOID_LAN_ETH_INTF, &iidStack, (void **) &ethObj) == CMSRET_SUCCESS )
      {
         cmsLog_debug("IfName=%s, status=%s", ethObj->X_BROADCOM_COM_IfName, ethObj->status);

         anyLinkOn = (0 == cmsUtl_strcmp(ethObj->status, MDMVS_UP));
         if ( anyLinkOn )
         {
            status = LAN_LINK_UP;
         }

         cmsObj_free((void **) &ethObj);
      }

#ifdef DMP_USBLAN_1
      while( (!anyLinkOn) &&
             cmsObj_getNext(MDMOID_LAN_USB_INTF, &iidStack1, (void **) &usbObj) == CMSRET_SUCCESS )
      {
         cmsLog_debug("IfName=%s, status=%s", usbObj->X_BROADCOM_COM_IfName, usbObj->status);

         anyLinkOn = (0 == cmsUtl_strcmp(usbObj->status, MDMVS_UP));
         if ( anyLinkOn )
         {
            status = LAN_LINK_UP;
         }
         cmsObj_free((void **) &usbObj);
      }
#endif  /* DMP_USBLAN_1 */

      cmsLck_releaseLock();
   }

   cmsLog_debug("LAN Link status=%d", status);

   replyMsg.type = msg->type;
   replyMsg.src = EID_SSK;
   replyMsg.dst = msg->src;
   replyMsg.flags_request = 0;
   replyMsg.flags_response = 1;
   replyMsg.wordData = status;

   if ((ret = cmsMsg_send(msgHandle, &replyMsg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("msg send failed, ret=%d",ret);
   }

   return;
}


/** Given the iidStack of a LANEthernetInterface object, get the
 * name of the IpInterface (bridge) that this object belongs to.
 */
CmsRet getBridgeIfName(const InstanceIdStack *iidStack, char *ifName, UINT32 ifNameLen)
{
   InstanceIdStack parentIidStack=*iidStack;
   InstanceIdStack ipIntfIidStack=EMPTY_INSTANCE_ID_STACK;
   LanIpIntfObject *ipIntfObj=NULL;
   LanHostCfgObject *hostCfgObj=NULL;
   CmsRet ret;

   ret = cmsObj_getAncestor(MDMOID_LAN_HOST_CFG, MDMOID_LAN_ETH_INTF, &parentIidStack, (void **)&hostCfgObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not find ancestor obj, ret=%d", ret);
      return ret;
   }
   cmsObj_free((void **) &hostCfgObj);

   ret = cmsObj_getNextInSubTree(MDMOID_LAN_IP_INTF, &parentIidStack, &ipIntfIidStack, (void **)&ipIntfObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not find ip intf obj, ret=%d", ret);
      return ret;
   }

   if (cmsUtl_strlen(ipIntfObj->X_BROADCOM_COM_IfName) + 1 >= (SINT32) ifNameLen)
   {
      cmsLog_error("ifname (%s) is too long for provided buffer (%d)",
            ipIntfObj->X_BROADCOM_COM_IfName, ifNameLen);
   }
   else
   {
      cmsUtl_strncpy(ifName, ipIntfObj->X_BROADCOM_COM_IfName, ifNameLen);
   }

   cmsObj_free((void **)&ipIntfObj);

   return ret;
}


/** If the parent IpInterface is in DHCP mode, then update the
 *  dhcpcConnectionStatus field of the object.  Otherwise, do nothing.
 */
void updateLanDhcpcConnectionStatus(const char *bridgeIfName __attribute((unused)),
                                    UBOOL8 linkup __attribute((unused)))
{
   static UINT32 linkUpCount=0;
   InstanceIdStack ipIntfIidStack=EMPTY_INSTANCE_ID_STACK;
   LanIpIntfObject *ipIntfObj=NULL;
   CmsRet ret;
   UBOOL8 found = FALSE;

   cmsLog_debug("%s linkup=%d", bridgeIfName, linkup);

   if (cmsUtl_strcmp(bridgeIfName, "br0"))
   {
      // This feature only works on br0
      return;
   }

   while (!found &&
         cmsObj_getNext(MDMOID_LAN_IP_INTF, &ipIntfIidStack, (void **) &ipIntfObj) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(ipIntfObj->X_BROADCOM_COM_IfName, bridgeIfName))
      {
         found = TRUE;
      }
      else
      {
         cmsObj_free((void **) &ipIntfObj);
      }
   }

   if (!found)
   {
      cmsLog_error("could not find IpIntf object of %s", bridgeIfName);
      return;
   }

   /*
    * We have to manually keep track of interface link statuses on br0
    * so we know if br0 is UP or not.
    */
   if (linkup)
   {
      linkUpCount++;
   }
   else
   {
      if (linkUpCount == 0)
      {
         cmsLog_error("linkUpCount is already 0, cannot decrement anymore!");
      }
      else
      {
         linkUpCount--;
      }
   }
   cmsLog_debug("br0 linkCount is now %d", linkUpCount);


   if (cmsUtl_strcmp(ipIntfObj->IPInterfaceAddressingType, MDMVS_DHCP))
   {
      // this interface is not in DHCP mode, do nothing and return
      cmsObj_free((void **)&ipIntfObj);
      return;
   }


   /*
    * Only start dhcpc if it is not already started and linkUpCount > 0.
    * Only stop dhcpc if linkUpCount == 0.
    * Ignore all other transitions.
    * XXX This means dhcp client action is triggered on link status transitions.
    * If you just enable dhcp client, but there is no link status change,
    * This code will not run and so dhcp client will still not get started.
    * Maybe fix in rcl_lanIpIntfObject?
    */
   if (linkUpCount > 0 &&
       !cmsUtl_strcmp(ipIntfObj->X_BROADCOM_COM_DhcpConnectionStatus, MDMVS_UNCONFIGURED))
   {
      cmsLog_debug("linkUp and dhcpc not started yet");
      /* do work below */
   }
   else if (linkUpCount == 0)
   {
      cmsLog_debug("All links are down!");
      /* do work below */
   }
   else
   {
      cmsLog_debug("linkUpCount=%d dhcpc status=%s, no further processing",
                    linkUpCount,
                    ipIntfObj->X_BROADCOM_COM_DhcpConnectionStatus);
      cmsObj_free((void **) &ipIntfObj);
      return;
   }

   /*
    * The mini-state machine for the DhcpConnectionStatus field is similar
    * to the one for WanIpConnection.
    * Unconfigured: means no link is detected
    * Connecting: link is detected, tell the RCL to start dhcpc to get IP addr
    * Connected: dhcpc got IP addr.
    * This algorhithm does not expect more than one cable to be plugged in
    * while in DHCP mode (it doesn't make sense).  But if more than one cable
    * is plugged in or removed, it will probably still work OK.  (not tested
    * though.)
    */
   CMSMEM_REPLACE_STRING(ipIntfObj->X_BROADCOM_COM_DhcpConnectionStatus,
                 ((linkUpCount > 0) ? MDMVS_CONNECTING : MDMVS_UNCONFIGURED));
   cmsLog_debug("set state to %s", ipIntfObj->X_BROADCOM_COM_DhcpConnectionStatus);
   ret = cmsObj_set(ipIntfObj, &ipIntfIidStack);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("set of lanIpIntfObj failed, ret=%d", ret);
   }
   cmsObj_free((void **) &ipIntfObj);

   return;
}
#endif  /* DMP_BASELINE_1 */


#ifdef DMP_BRIDGING_1

void processDhcpdDenyVendorId(CmsMsgHeader *msg)
{
   DHCPDenyVendorID *vid = (DHCPDenyVendorID *) (msg + 1);
   char portName[CMS_IFNAME_LENGTH]={0};
   char destBridgeIfName[CMS_IFNAME_LENGTH]={0};
   SskLinkStatusRecord *linkStatusRec;
   CmsRet ret;

   cmsLog_debug("dhcpd dataLength=%d vendorID=%s MAC=%02x:%02x:%02x:%02x:%02x:%02x interface=%s\n", 
           msg->dataLength, vid->vendor_id,
           vid->chaddr[0], vid->chaddr[1], vid->chaddr[2],
           vid->chaddr[3], vid->chaddr[4], vid->chaddr[5],
           vid->ifName);

   ret = oal_getPortNameFromMacAddr(vid->ifName, vid->chaddr, portName);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("oal_getPortnameFromMacAddr failed, cannot process denyVendorId, ret=%d");
   }
   else
   {
      cmsLog_debug("need to move %s because of vendorId match", portName);

      if( (ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS )
      {
         cmsLog_error("could not get lock, ret=%d", ret);
      }
      else
      {
         getVendorIdBridgeIfNameLocked(vid->vendor_id, destBridgeIfName);
             
         if ((linkStatusRec = getLinkStatusRecord(portName)) == NULL)
         {
            cmsLog_error("cannot find linkstatusRec for %s", portName);
         }
         else
         {
            /*
             * Remember that we moved this interface so that we can move it back
             * if the link goes down.
             * But there is a down/up action in the process of ETH/USB LAN moving.
             * avoid to misjudge in the first link down, give three states to tmpMoved.  
             * when the link is up,if tmpMoved == tmpMoveDoing, set tmpMoved = tmpMoveDone.
             * when the link is down,if tmpMoved ==tmpMoveDone,means 
             * the port had been Tmpmoved, we should move it back
             */
            if (strncmp(portName, "eth", 3) == 0)
            {         
               linkStatusRec->tmpMoved = tmpMoveDoing;
            }
#ifdef DMP_USBLAN_1
            else if (strncmp(portName, "usb", 3) == 0)
            {         
               linkStatusRec->tmpMoved = tmpMoveDoing;
            }
#endif
#ifdef BRCM_WLAN
            else if (strncmp(portName, "wl", 2) == 0)
            {    
                /* there is no down/up action in the process of WL interface moving.
                * should set tmpMoved = tmpMoveDone beforehand.
                * see the function rutLan_moveWlanInterface()
                */
			   linkStatusRec->tmpMoved = tmpMoveDone;
            }
#endif
            strcpy(linkStatusRec->tmpBridgeIfName, destBridgeIfName);
        }
             
        moveIfNameToBridgeLocked(portName, vid->ifName, destBridgeIfName);

        cmsLck_releaseLock();
      }
   }

   return;
}


/** Given a vendor ID, look up Layer2Bridging objects and return the bridge
 * this vendor id should be attached to.
 *
 * @param vendor_id (IN) the vendor_id to query
 * @param bridgeIfName (OUT) the bridge name this vendor id should be assigned to.
 *                           The variable must be at least CMS_IFNAME_LENGTH long.
 *
 */
void getVendorIdBridgeIfNameLocked(const char *vendor_id, char *bridgeIfName)
{
   UBOOL8 found=FALSE;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   L2BridgingFilterObject *filterObj=NULL;
   char * vendorIdString;


   while (!found &&
          cmsObj_getNextFlags(MDMOID_L2_BRIDGING_FILTER, &iidStack, OGF_NO_VALUE_UPDATE, (void **) &filterObj) == CMSRET_SUCCESS)
   {
         if ((vendorIdString = cmsUtl_getDhcpVendorIdsFromAggregateString(filterObj->sourceMACFromVendorClassIDFilter)) != NULL)
         {
            if (!cmsUtl_strcmp(filterObj->filterInterface, MDMVS_LANINTERFACES))
            {
               char *vptr;
               UINT32 j;
            
               for (j=0; j < MAX_PORTMAPPING_DHCP_VENDOR_IDS; j++)
               {
                  vptr = &(vendorIdString[j * (DHCP_VENDOR_ID_LEN+1)]);
                  if (*vptr != '\0' && (!cmsUtl_strcmp(vptr, vendor_id)))
                  {
                     sprintf(bridgeIfName, "br%d", filterObj->filterBridgeReference);
                     found = TRUE;
                  }
               }
                   
            }
            CMSMEM_FREE_BUF_AND_NULL_PTR(vendorIdString);
         }

      cmsObj_free((void **) &filterObj);
   }
                         
   return;
}


/** Given a layer 2 port name, look up Layer2Bridging objects and return the bridge
 * this port is normally attached to (without vendor id).
 *
 * @param ifName       (IN) the layer 2 port name, e.g. eth0, eth1, etc.
 * @param bridgeIfName (OUT) the bridge name this vendor id should be assigned to.
 *                           The variable must be at least CMS_IFNAME_LENGTH long.
 *
 */
void getStandardBridgeIfNameLocked(const char *ifName, char *bridgeIfName)
{
   char availIntfPathName[BUFLEN_256]={0};
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   L2BridgingFilterObject *filterObj=NULL;
   UINT32 availIntfKey;
   UBOOL8 found=FALSE;
   CmsRet ret;

   ret = dalPMap_lanIfNameToAvailableInterfaceReference(ifName, availIntfPathName);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cannot find availInterfaceRef for ifName=%s", ifName);
      return;
   }

   /*
    * The availIntfPathName ends with a '.', but the entry in the filter
    * object does not end in a '.', so remove the last '.' before the compare.
    */
   availIntfPathName[(strlen(availIntfPathName))-1] = '\0';


   /*
    * Look through all the filters, get the availableInterfaceKey,
    * translate the availableInterfaceKey to path, and compare to path.
    */
   while (!found &&
          cmsObj_getNextFlags(MDMOID_L2_BRIDGING_FILTER, &iidStack, OGF_NO_VALUE_UPDATE, (void **) &filterObj) == CMSRET_SUCCESS)
   {
      if ((filterObj->sourceMACFromVendorClassIDFilter == NULL) &&
          (cmsUtl_strcmp(filterObj->filterInterface, MDMVS_LANINTERFACES)) &&
          (CMSRET_SUCCESS == cmsUtl_strtoul(filterObj->filterInterface, NULL, 0, &availIntfKey)))
      {
         InstanceIdStack availIntfIidStack;
         L2BridgingIntfObject *availIntfObj=NULL;

         ret = dalPMap_getAvailableInterfaceByKey(availIntfKey, &availIntfIidStack, &availIntfObj);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("could not find availIntf for key %d", availIntfKey);
         }
         else
         {
            if (!cmsUtl_strcmp(availIntfPathName, availIntfObj->interfaceReference))
            {
               sprintf(bridgeIfName, "br%d", filterObj->filterBridgeReference);
               cmsLog_debug("set bridgeIfName to %s", bridgeIfName);
               found = TRUE;
            }

            cmsObj_free((void **) &availIntfObj);
         }
      }

      cmsObj_free((void **) &filterObj);
   }

   return;
}


/** Move the specified layer 2 interface to the specified bridge.
 *
 * @param ifName           (IN) ifName to move.
 * @param srcBridgeIfName  (IN) bridge to move the ifName from
 * @param destBridgeIfName (IN) bridge to move the ifName to.
 *
 */
void moveIfNameToBridgeLocked(const char *ifName,
                              const char *srcBridgeIfName,
                              const char *destBridgeIfName)
{
   char pathName[BUFLEN_256]={0};
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   L2BridgingIntfObject *availIntfObj=NULL;
   CmsRet ret;

   /*
    * mwang_todo: ssk should not call rutLan_xxx directly. Fix later.
    * RUT functions should only be called by RCL/STL/RUT functions inside
    * cms_core.  The right way to do this is for ssk to set a special object
    * that moves the interface.  But it is much faster to do it this way for now.  
    */


   /*
    * Get the availableInterface object that points to this interface.
    * We need to update this object after the interface has been moved.
    */
   ret = dalPMap_lanIfNameToAvailableInterfaceReference(ifName, pathName);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("dalPMap_lanIfNameToAvailableInterfaceReference failed for %s. ret %d", ifName, ret);
      return;
   }    
   pathName[strlen(pathName)-1] = '\0';

   ret = dalPMap_getAvailableInterfaceByRef(pathName, &iidStack, &availIntfObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not find available interface object for %s", ifName);
      return;
   }


   if (strncmp(ifName, "eth", 3) == 0)
   {
      ret = rutLan_moveEthInterface(ifName, srcBridgeIfName, destBridgeIfName);
   }
#ifdef DMP_USBLAN_1
   else if (strncmp(ifName, "usb", 3) == 0)
   {
      ret = rutLan_moveUsbInterface(ifName, srcBridgeIfName, destBridgeIfName);
   }
#endif
#ifdef BRCM_WLAN
   else if (strncmp(ifName, "wl", 2) == 0)
   {
      ret = rutLan_moveWlanInterface(ifName, srcBridgeIfName, destBridgeIfName);
   }
#endif
   else
   {
      cmsLog_error("moving of interface %s not supported (yet?)", ifName);
      ret = CMSRET_INTERNAL_ERROR;
   }

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("move of %s from %s to %s failed, ret=%d", ifName, srcBridgeIfName, destBridgeIfName);
   }
   else
   {

      /* ifname has moved, so pathName will change */
      memset(pathName, 0, sizeof(pathName));
      dalPMap_lanIfNameToAvailableInterfaceReference(ifName, pathName);
      pathName[strlen(pathName)-1] = '\0';

      cmsLog_debug("update availIntfObj with new interface pathname=%s", pathName);

      CMSMEM_REPLACE_STRING(availIntfObj->interfaceReference, pathName);

      if ((ret = cmsObj_set(availIntfObj, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("update of availintf object failed, ret=%d", ret);
      }
   }

   cmsObj_free((void **) &availIntfObj);

   return;
}


#endif /* DMP_BRIDGING_1 */
