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


#include "cms.h"
#include "cms_util.h"
#include "prctl.h"
#include "cms_core.h"
#include "cms_dal.h"
#include "cms_msg.h"
#include "cms_qos.h"
#include "ssk.h"
#include "devctl_xtm.h"
#include "bcmxtmcfg.h"
#include "adslctlapi.h"
#include "devctl_adsl.h"

/*!\file linkstatus_wan.c
 * \brief This file detects changes in the WAN (layer 2) link status.
 *
 */


/**wanLinkInfoHead is the head of a linked list of WanLinkInfo structs. 
 * This is used by TR98 WAN side code only.
 */
DLIST_HEAD(wanLinkInfoHead);

/** In bonding mode, need to keep track of the previous link status for each xDsl link
 *  to compare with the current xDsl links to make a determination on weather the wan
 * link is up or need to be tear down.
 */
typedef struct
{
   UBOOL8 isCurrLine0Up;     /**< current  primary line link state */
   UBOOL8 isNewLine0Up;      /**< new primary line link state  */
   UBOOL8 isCurrLine1Up;     /**< current secondary line link state */
   UBOOL8 isNewLine1Up;      /**< new secondary line link state */
} DslBondingLinkInfo;

DslBondingLinkInfo atmDslBondingLinksStateTable={FALSE, FALSE, FALSE, FALSE};
DslBondingLinkInfo ptmDslBondingLinksStateTable={FALSE, FALSE, FALSE, FALSE};

extern UBOOL8 dslDiagInProgress;
#ifdef DMP_DSLDIAGNOSTICS_1
dslLoopDiag dslLoopDiagInfo = {FALSE, EMPTY_INSTANCE_ID_STACK,0};
#endif

#ifdef DMP_X_BROADCOM_COM_SELT_1
dslDiag dslDiagInfo = {FALSE, 0, 0, 0};
#endif

#ifdef DMP_X_BROADCOM_COM_ATMWAN_1
static void fixATMPvcStatusLocked(const InstanceIdStack *parentIidStack, UBOOL8 wanLinkUp);
#ifdef DMP_X_BROADCOM_COM_DSLBONDING_1
static UBOOL8 isBondedAtmLinkUp(void);
#endif
#endif

#ifdef DMP_PTMWAN_1
static void fixPtmChannelStatusLocked(const InstanceIdStack *parentIidStack, UBOOL8 wanLinkUp);
#ifdef DMP_X_BROADCOM_COM_DSLBONDING_1
static UBOOL8 isBondedPtmLinkUp(void);
#endif
static void informErrorSampleStatusChangeLocked(const InstanceIdStack *parentIidStack);
#endif

#ifdef DMP_ETHERNETWAN_1
static UBOOL8 checkWanEthLinkStatusLocked_igd(const char *intfName);
#endif

#ifdef DMP_X_BROADCOM_COM_WIFIWAN_1
static UBOOL8 checkWanWifiLinkStatusLocked_igd(const char *intfName);
#endif

#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
static CmsRet updateGponLinkOpState(UBOOL8 opState);
#endif /* DMP_X_BROADCOM_COM_GPONWAN_1 */
#ifdef DMP_X_BROADCOM_COM_EPONWAN_1
static CmsRet updateEponLinkOpState(UBOOL8 opState);
#endif /* DMP_X_BROADCOM_COM_EPONWAN_1 */

extern void *msgHandle;

/* tmp hack for RDPA init issue: once we have confirmed that RDPA is
 * initialized for GBE, and we detected WAN GBE link up, "Lock In" that
 * config and ignore DSL.
 */
UBOOL8 isRdpaGBEAEsysLockedIn=FALSE;
UINT8 strEthIdInx=7;
UINT8 strWanoeMacInx=4;


void processGetWanLinkStatus_igd(CmsMsgHeader *msg)
{
   CmsMsgHeader replyMsg = EMPTY_MSG_HEADER;
   UINT32 status = WAN_LINK_DISABLED;
   WanLinkInfo *wanLinkInfo __attribute__ ((unused));
   CmsRet ret;
   
   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d", ret);
      return;
   }

   if (msg->dataLength == 0)
   {
      /*
       * If the message does not contain an interface name, then return WAN_LINK_UP
       * if any WAN link is up.
       */
      cmsLog_debug("no ifName specified by caller");
      dlist_for_each_entry(wanLinkInfo, &wanLinkInfoHead, dlist)
      {
         if (wanLinkInfo->isUp)
         {
            status = WAN_LINK_UP;
            break;
         }
      }
   }
   else
   {
      /*
       * Caller has asked about a specific layer 2 WAN link. This is the more common case.
       */
      char *queryIfName __attribute__ ((unused)) = NULL;
      UBOOL8 found=FALSE;


      queryIfName = (char *) (msg + 1);
      cmsLog_debug("queryIfName=%s", queryIfName);

      dlist_for_each_entry(wanLinkInfo, &wanLinkInfoHead, dlist)
      {
#ifdef DMP_X_BROADCOM_COM_ATMWAN_1
         if (wanLinkInfo->isATM)
         {
            WanDslLinkCfgObject *dslLinkCfg=NULL;
            InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
            UINT32 flags=OGF_NO_VALUE_UPDATE;

            /*
             * for atm WAN device, look for the ifName in the WANDSLLinkCfg object.
             * I use the OGF_NO_VALUE_UPDATE flag here because I don't want to force
             * MDM to update the status.  I just want to know the current status.
             * Any changes to the status should be detected via the checkAllWanLinkStatusLocked()
             * function.
             */
            while (!found &&
                   ((ret = cmsObj_getNextInSubTreeFlags(MDMOID_WAN_DSL_LINK_CFG, &(wanLinkInfo->iidStack), &iidStack, flags, (void **) &dslLinkCfg)) == CMSRET_SUCCESS))
            {
               char VpiVciStr[BUFLEN_32];
               SINT32 vpi;
               SINT32 vci;
               UINT32 ifNameLen;

               /* VpiVCiStr is for PPPoA queryIFname, looks like "0.0.36" */
               if ((ret = cmsUtl_atmVpiVciStrToNum(dslLinkCfg->destinationAddress, &vpi, &vci)) != CMSRET_SUCCESS)
               {
                  cmsLog_error("could not convert destinationAddress %s", dslLinkCfg->destinationAddress);
               }
               snprintf(VpiVciStr, sizeof(VpiVciStr), "%u.%d.%d", dslLinkCfg->X_BROADCOM_COM_ATMInterfaceId, vpi, vci);

               /* Only compare the base layer 2 interface names like atm0 with cmsUtl_strncmp
               * since for VlanMux and MSC, the queryIfName will look like atm0.100 or atm0_1
               */
               ifNameLen = cmsUtl_strlen(dslLinkCfg->X_BROADCOM_COM_IfName);
               if ((ifNameLen > strlen(ATM_IFC_STR) && !cmsUtl_strncmp(dslLinkCfg->X_BROADCOM_COM_IfName, queryIfName, ifNameLen)) || 
                   !cmsUtl_strcmp(VpiVciStr, queryIfName))
               {
                  found = TRUE;
                  if (!cmsUtl_strcmp(dslLinkCfg->linkStatus, MDMVS_UP))
                  {
                     status = WAN_LINK_UP;
                  }
               }
               cmsObj_free((void **) &dslLinkCfg);
            }
         }
#endif /* DMP_X_BROADCOM_COM_ATMWAN_1 */


#ifdef DMP_PTMWAN_1
         if (wanLinkInfo->isPTM)
         {
            WanPtmLinkCfgObject *ptmLinkCfg=NULL;
            InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
            UINT32 flags=OGF_NO_VALUE_UPDATE;
            UINT32 ifNameLen;

            /*
             * for ptm WAN device, look for the ifName in the WANPTMLinkCfg object.
             * I use the OGF_NO_VALUE_UPDATE flag here because I don't want to force
             * MDM to update the status.  I just want to know the current status.
             * Any changes to the status should be detected via the checkAllWanLinkStatusLocked()
             * function.
             */
            while (!found &&
                   ((ret = cmsObj_getNextInSubTreeFlags(MDMOID_WAN_PTM_LINK_CFG, &(wanLinkInfo->iidStack), &iidStack, flags, (void **) &ptmLinkCfg)) == CMSRET_SUCCESS))
            {

              /* Only compare the base layer 2 interface names like ptm0 with cmsUtl_strncmp
               * since for VlanMux and MSC, the queryIfName will look like ptm0.100 or ptm0_1
               */
               ifNameLen = cmsUtl_strlen(ptmLinkCfg->X_BROADCOM_COM_IfName);
               if (ifNameLen > strlen(PTM_IFC_STR) && !cmsUtl_strncmp(ptmLinkCfg->X_BROADCOM_COM_IfName, queryIfName, ifNameLen))
               {
                  found = TRUE;
                  if (!cmsUtl_strcmp(ptmLinkCfg->linkStatus, MDMVS_UP))
                  {
                     status = WAN_LINK_UP;
                  }
               }
               cmsObj_free((void **) &ptmLinkCfg);
            }
         }
#endif /* DMP_PTMWAN_1 */
               

#ifdef DMP_ETHERNETWAN_1
         if (wanLinkInfo->isEth)
         {
            WanEthIntfObject *wanEth=NULL;
            UINT32 flags=OGF_NO_VALUE_UPDATE;
            UINT32 ifNameLen;

            /*
             * for Ethernet WAN device, look for the ifName in the WANEthIntf object.
             * I use the OGF_NO_VALUE_UPDATE flag here because I don't want to force
             * MDM to update the status.  I just want to know the current status.
             * Any changes to the status should be detected via the checkAllWanLinkStatusLocked()
             * function.
             */
            if ((ret = cmsObj_get(MDMOID_WAN_ETH_INTF, &(wanLinkInfo->iidStack), flags, (void **) &wanEth)) == CMSRET_SUCCESS)
            {

               /* Only compare the base layer 2 interface names like eth3 with cmsUtl_strncmp
               * since for VlanMux and MSC, the queryIfName will look like eth3.100 or eth3_1
               */
               ifNameLen = cmsUtl_strlen(wanEth->X_BROADCOM_COM_IfName);
               if (ifNameLen > strlen(ETH_IFC_STR) && !cmsUtl_strncmp(wanEth->X_BROADCOM_COM_IfName, queryIfName, ifNameLen))
               {
                  found = TRUE;
                  if (!cmsUtl_strcmp(wanEth->status, MDMVS_UP))
                  {
                     status = WAN_LINK_UP;
                  }
               }
               cmsObj_free((void **) &wanEth);
            }
         }
#endif /* DMP_ETHERNET_1 */
               

#ifdef DMP_X_BROADCOM_COM_MOCAWAN_1
         if (wanLinkInfo->isMoca)
         {
            WanMocaIntfObject *wanMoca=NULL;
            UINT32 flags=OGF_NO_VALUE_UPDATE;
            UINT32 ifNameLen;

            /*
             * for Moca WAN device, look for the ifName in the WANMocaIntf object.
             * I use the OGF_NO_VALUE_UPDATE flag here because I don't want to force
             * MDM to update the status.  I just want to know the current status.
             * Any changes to the status should be detected via the checkAllWanLinkStatusLocked()
             * function.
             */
            if ((ret = cmsObj_get(MDMOID_WAN_MOCA_INTF, &(wanLinkInfo->iidStack), flags, (void **) &wanMoca)) == CMSRET_SUCCESS)
            {

               /* Only compare the base layer 2 interface names like moca0 with cmsUtl_strncmp
               * since for VlanMux and MSC, the queryIfName will look like moca0.100 or moca0_1
               */
               ifNameLen = cmsUtl_strlen(wanMoca->ifName);
               if (ifNameLen > strlen(MOCA_IFC_STR) && !cmsUtl_strncmp(wanMoca->ifName, queryIfName, ifNameLen))
               {
                  found = TRUE;
                  if (!cmsUtl_strcmp(wanMoca->status, MDMVS_UP))
                  {
                     status = WAN_LINK_UP;
                  }
               }
               cmsObj_free((void **) &wanMoca);
            }
         }
#endif /* DMP_X_BROADCOM_COM_MOCAWAN_1 */


#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
         if (wanLinkInfo->isGpon)
         {
            WanGponLinkCfgObject *gponLinkCfg=NULL;
            InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
            UINT32 flags=OGF_NO_VALUE_UPDATE;
            UINT32 ifNameLen;

            /*
             * for Gpon WAN device, look for the ifName in the WANGponLinkCfg object.
             * I use the OGF_NO_VALUE_UPDATE flag here because I don't want to force
             * MDM to update the status.  I just want to know the current status.
             * Any changes to the status should be detected via the checkAllWanLinkStatusLocked()
             * function.
             */
            while (!found &&
                   ((ret = cmsObj_getNextInSubTreeFlags(MDMOID_WAN_GPON_LINK_CFG, &(wanLinkInfo->iidStack), &iidStack, flags, (void **) &gponLinkCfg)) == CMSRET_SUCCESS))
            {
              /* Only compare the base layer 2 interface names like gpon with cmsUtl_strncmp
               * since for VlanMux and MSC, the queryIfName will look like gpon0.100 or gpon1.200
               */
               ifNameLen = cmsUtl_strlen(gponLinkCfg->ifName);

               cmsLog_debug("gponLinkCfg->ifName=%s, queryIfName=%s, ifNameLen=%d",  gponLinkCfg->ifName, queryIfName, ifNameLen);
               
               if (ifNameLen > strlen(GPON_IFC_STR) && !cmsUtl_strncmp(gponLinkCfg->ifName, queryIfName, ifNameLen)
                                                    && gponLinkCfg->enable)
               {
                  found = TRUE;
                  if (!cmsUtl_strcmp(gponLinkCfg->linkStatus, MDMVS_UP))
                  {
                     status = WAN_LINK_UP;
                  }
               }
               cmsObj_free((void **) &gponLinkCfg);
            }
         }
#endif /* DMP_X_BROADCOM_COM_GPONWAN_1 */

#ifdef DMP_X_BROADCOM_COM_EPONWAN_1
         if (wanLinkInfo->isEpon)
         {
#ifdef EPON_SFU
            WanEponIntfObject *eponCfg=NULL;
            InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
            UINT32 flags=OGF_NO_VALUE_UPDATE;
            UINT32 ifNameLen;

            /*
             * for Epon WAN device, look for the ifName in the WANEponLinkCfg object.
             * I use the OGF_NO_VALUE_UPDATE flag here because I don't want to force
             * MDM to update the status.  I just want to know the current status.
             * Any changes to the status should be detected via the checkAllWanLinkStatusLocked()
             * function.
             */
            while (!found &&
                   ((ret = cmsObj_getNextInSubTreeFlags(MDMOID_WAN_EPON_INTF, &(wanLinkInfo->iidStack), &iidStack, flags, (void **) &eponCfg)) == CMSRET_SUCCESS))
            {
              /* Only compare the base layer 2 interface names like epon with cmsUtl_strncmp
               * since for VlanMux and MSC, the queryIfName will look like epon0.100 
               */
               ifNameLen = cmsUtl_strlen(eponCfg->ifName);

               cmsLog_debug("eponCfg->ifName=%s, queryIfName=%s, ifNameLen=%d",  eponCfg->ifName, queryIfName, ifNameLen);
               
               if (ifNameLen > strlen(EPON_IFC_STR) && !cmsUtl_strncmp(eponCfg->ifName, queryIfName, ifNameLen)
                                                    && eponCfg->enable)
               {
                  found = TRUE;
                  if (!cmsUtl_strcmp(eponCfg->status, MDMVS_UP))
                  {
                     status = WAN_LINK_UP;
                  }
               }
               cmsObj_free((void **) &eponCfg);
            }
#else
            WanEponLinkCfgObject *eponLinkCfg=NULL;
            InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
            UINT32 flags=OGF_NO_VALUE_UPDATE;
            UINT32 ifNameLen;

            /*
             * for Epon HGU WAN device, look for the ifName in the WANEponLinkCfg object.
             * I use the OGF_NO_VALUE_UPDATE flag here because I don't want to force
             * MDM to update the status.  I just want to know the current status.
             * Any changes to the status should be detected via the checkAllWanLinkStatusLocked()
             * function.
             */
            while (!found &&
                   ((ret = cmsObj_getNextInSubTreeFlags(MDMOID_WAN_EPON_LINK_CFG, &(wanLinkInfo->iidStack), &iidStack, flags, (void **) &eponLinkCfg)) == CMSRET_SUCCESS))
            {
              /* Only compare the base layer 2 interface names like epon with cmsUtl_strncmp
               * since for VlanMux and MSC, the queryIfName will look like gpon0.100 or gpon1.200
               */
               ifNameLen = cmsUtl_strlen(eponLinkCfg->ifName);

               cmsLog_debug("eponLinkCfg->ifName=%s, queryIfName=%s, ifNameLen=%d",  eponLinkCfg->ifName, queryIfName, ifNameLen);
               
               if (ifNameLen > strlen(EPON_IFC_STR) && !cmsUtl_strncmp(eponLinkCfg->ifName, queryIfName, ifNameLen)
                                                    && eponLinkCfg->enable)
               {
                  found = TRUE;
                  if (!cmsUtl_strcmp(eponLinkCfg->linkStatus, MDMVS_UP))
                  {
                     status = WAN_LINK_UP;
                  }
               }
               cmsObj_free((void **) &eponLinkCfg);
            }
#endif
         }
#endif /* DMP_X_BROADCOM_COM_EPONWAN_1 */

#ifdef DMP_X_BROADCOM_COM_WIFIWAN_1
         if (wanLinkInfo->isWifi)
         {
            WanWifiIntfObject *wanWifi=NULL;
            UINT32 flags=OGF_NO_VALUE_UPDATE;
            UINT32 ifNameLen;

            /*
             * for WIFI WAN device, look for the ifName in the WANWifiIntf object.
             * I use the OGF_NO_VALUE_UPDATE flag here because I don't want to force
             * MDM to update the status.  I just want to know the current status.
             * Any changes to the status should be detected via the checkAllWanLinkStatusLocked()
             * function.
             */
            if ((ret = cmsObj_get(MDMOID_WAN_WIFI_INTF, &(wanLinkInfo->iidStack), flags, (void **) &wanWifi)) == CMSRET_SUCCESS)
            {

               /* Only compare the base layer 2 interface names like wl0 with cmsUtl_strncmp
                * to update the status.
                */
               ifNameLen = cmsUtl_strlen(wanWifi->ifName);
               if (ifNameLen > strlen(WLAN_IFC_STR) && !cmsUtl_strncmp(wanWifi->ifName, queryIfName, ifNameLen))
               {
                  found = TRUE;
                  if (!cmsUtl_strcmp(wanWifi->status, MDMVS_UP))
                  {
                     status = WAN_LINK_UP;
                  }
               }
               cmsObj_free((void **) &wanWifi);
            }
         }
#endif /* DMP_X_BROADCOM_COM_WIFIWAN_1 */

      } /* dlist_for_each_entry over all WanLinkInfo entries */

      if (!found)
      {
         cmsLog_error("queried ifName %s not found (srcEid=0x%x)", queryIfName, msg->src);
      }
   }

   cmsLck_releaseLock();


   cmsLog_debug("WAN Link status=%d", status);

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



#ifdef DMP_BASELINE_1
static void initWanLinkInfo_igd(void)
{
   WanCommonIntfCfgObject *wanCommonIntf;
   WanDslIntfCfgObject *dslIntfCfg=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UINT32 flags=OGF_NO_VALUE_UPDATE;
   WanLinkInfo *wanLinkInfo;
   UBOOL8 addToList;
   CmsRet ret;


   cmsLog_notice("initializing list of WANDevices (links)");

   while ((ret = cmsObj_getNextFlags(MDMOID_WAN_COMMON_INTF_CFG, &iidStack, flags, (void **)&wanCommonIntf)) == CMSRET_SUCCESS)
   {
      addToList = FALSE;

      /* allocate the struct to be added to the list */
      if ((wanLinkInfo = cmsMem_alloc(sizeof(WanLinkInfo), ALLOC_ZEROIZE)) == NULL)
      {
         cmsLog_error("wanLinkInfo allocation failed");
         cmsObj_free((void **) &wanCommonIntf);
         return;
      }


      if (!cmsUtl_strcmp(wanCommonIntf->WANAccessType, MDMVS_DSL))
      {
         ret = cmsObj_get(MDMOID_WAN_DSL_INTF_CFG, &iidStack, flags, (void **) &dslIntfCfg);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("could not get DSL intf cfg, ret=%d", ret);
            cmsObj_free((void **) &wanCommonIntf);
            CMSMEM_FREE_BUF_AND_NULL_PTR(wanLinkInfo);
            continue;
         }

         if (!cmsUtl_strcmp(dslIntfCfg->linkEncapsulationUsed, MDMVS_G_992_3_ANNEX_K_ATM))
         {
            /*
             * only add the primary line to our list.  In the monitor loop, I will
             * actually check both lines when I check the primary line.
             */
            if (dslIntfCfg->X_BROADCOM_COM_BondingLineNumber == 0)
            {
               wanLinkInfo->isATM = TRUE;
               addToList = TRUE;
            }
         }
         else if (!cmsUtl_strcmp(dslIntfCfg->linkEncapsulationUsed, MDMVS_G_993_2_ANNEX_K_PTM))
         {
            /*
             * only add the primary line to our list.  In the monitor loop, I will
             * actually check both lines when I check the primary line.
             */
            if (dslIntfCfg->X_BROADCOM_COM_BondingLineNumber == 0)
            {
               wanLinkInfo->isPTM = TRUE;
               addToList = TRUE;
            }
         }
         else
         {
            cmsLog_error("unrecognized linkEncapsulation type %s", dslIntfCfg->linkEncapsulationUsed);
            cmsObj_free((void **) &wanCommonIntf);
            cmsObj_free((void **) &dslIntfCfg);
            CMSMEM_FREE_BUF_AND_NULL_PTR(wanLinkInfo);
            continue;
         }
         
         cmsObj_free((void **) &dslIntfCfg);
      }
      else if (!cmsUtl_strcmp(wanCommonIntf->WANAccessType, MDMVS_ETHERNET))
      {
         wanLinkInfo->isEth = TRUE;
         addToList = TRUE;
      }
      else if (!cmsUtl_strcmp(wanCommonIntf->WANAccessType, MDMVS_X_BROADCOM_COM_MOCA))
      {
         wanLinkInfo->isMoca = TRUE;
         addToList = TRUE;
      }
      else if (!cmsUtl_strcmp(wanCommonIntf->WANAccessType, MDMVS_X_BROADCOM_COM_PON))
      {
         WanPonIntfObject *ponObj = NULL;

         if ((ret = cmsObj_get(MDMOID_WAN_PON_INTF, &iidStack, flags, (void **) &ponObj)) != CMSRET_SUCCESS)
         {
            cmsLog_error("could not get Pon intf cfg, ret=%d", ret);
            cmsObj_free((void **) &wanCommonIntf);
            CMSMEM_FREE_BUF_AND_NULL_PTR(wanLinkInfo);
            continue;
         }

         if (!cmsUtl_strcmp(ponObj->ponType, MDMVS_GPON))
         {
            wanLinkInfo->isGpon = TRUE;
            addToList = TRUE;
         }
         else if (!cmsUtl_strcmp(ponObj->ponType, MDMVS_EPON))
         {
            wanLinkInfo->isEpon = TRUE;
            addToList = TRUE;
         }
                  
         cmsObj_free((void **) &ponObj);

      }
      else if (!cmsUtl_strcmp(wanCommonIntf->WANAccessType, MDMVS_X_BROADCOM_COM_WIFI))
      {
         wanLinkInfo->isWifi= TRUE;
         addToList = TRUE;
      }
      
      cmsObj_free((void **) &wanCommonIntf);
      
      if (addToList)
      {
         cmsLog_debug("adding wanLinkInfo at %s, atm=%d ptm=%d eth=%d moca=%d gpon=%d epon=%d",
                      cmsMdm_dumpIidStack(&iidStack),
                      wanLinkInfo->isATM, wanLinkInfo->isPTM, wanLinkInfo->isEth, wanLinkInfo->isMoca, wanLinkInfo->isGpon, wanLinkInfo->isEpon);

         wanLinkInfo->iidStack = iidStack;

         /* add this to the end of the list */
         dlist_prepend((DlistNode *) wanLinkInfo, &wanLinkInfoHead);
      }
      else
      {
         cmsLog_debug("ignoring WanDevice %s", cmsMdm_dumpIidStack(&iidStack));
         CMSMEM_FREE_BUF_AND_NULL_PTR(wanLinkInfo);
      }
   }

   return;
}


void freeWanLinkInfoList_igd(void)
{
   WanLinkInfo *tmp = NULL;

   while (dlist_empty(&wanLinkInfoHead) == 0)
   {
      tmp = (WanLinkInfo *) wanLinkInfoHead.next;
      
      cmsLog_debug("Free wanLinkInfo iidstack: %s", 
         cmsMdm_dumpIidStack(&(tmp->iidStack)));

      dlist_unlink((DlistNode *) tmp);
      cmsMem_free(tmp);
   }

    cmsLog_debug("Done free wanLinkInfo list.");
}

#ifdef DMP_X_BROADCOM_COM_DSLBONDING_1
void displayBondingStateTable(const char *heading, DslBondingLinkInfo *xDslBondingLinkStatetbl )
{
   cmsLog_notice("%s  xDsl link state table:", heading);
   cmsLog_notice("line0_curr %d, line0_new %d", xDslBondingLinkStatetbl->isCurrLine0Up, xDslBondingLinkStatetbl->isNewLine0Up);
   cmsLog_notice("line1_curr %d, line1_new %d", xDslBondingLinkStatetbl->isCurrLine1Up, xDslBondingLinkStatetbl->isNewLine1Up);
}   
#endif

UBOOL8 checkWanLinkStatusLocked_igd(const char *intfName)
{
   WanLinkInfo *wanLinkInfo;
   static UBOOL8 wanLinkInfoInitialized=FALSE;
   
   cmsLog_debug("Enter: intfName=%s", intfName);

   if (!wanLinkInfoInitialized)
   {
      initWanLinkInfo_igd();
      wanLinkInfoInitialized = TRUE;
   }


   if ((intfName == NULL) ||
       (!strncmp(intfName, ETH_IFC_STR, strlen(ETH_IFC_STR))))
   {
      UBOOL8 found=FALSE;

#ifdef DMP_ETHERNETWAN_1
      found = checkWanEthLinkStatusLocked_igd(intfName);
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

#ifdef DMP_X_BROADCOM_COM_WIFIWAN_1
         found = checkWanWifiLinkStatusLocked_igd(intfName);
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


   /* XXX TODO: testing shows epon will generate generic netlink msg, so
    * we should definitely check for it here.
    */


   if ((intfName != NULL) &&
       (!strncmp(intfName, HOMEPLUG_IFC_STR, strlen(HOMEPLUG_IFC_STR))))
   {
      /*
       * homeplug intf is never on WAN side, so there is no point in
       * doing any more in this WAN side function.
       */
      return FALSE;
   }

   if (intfName)
   {
      cmsLog_debug("CMS: add specific WAN link status support for %s", intfName);
   }


   /*
    * If we get there, there was no intfName given or an intfName was given
    * but we don't know how to handle it.  So search through all interfaces
    * and return found=FALSE to maintain old behavior of searching through
    * all WAN side and LAN side interfaces.
    */
   dlist_for_each_entry(wanLinkInfo, &wanLinkInfoHead, dlist)
   {
      cmsLog_debug("iidStack=%s (isATM=%d isPTM=%d isEth=%d isMoca=%d isGpon=%d, isEpon=%d  isWifi=%d)",
         cmsMdm_dumpIidStack(&(wanLinkInfo->iidStack)),
         wanLinkInfo->isATM, wanLinkInfo->isPTM, wanLinkInfo->isEth, wanLinkInfo->isMoca, wanLinkInfo->isGpon, wanLinkInfo->isEpon, wanLinkInfo->isWifi);

#ifdef DMP_X_BROADCOM_COM_ATMWAN_1
      if (wanLinkInfo->isATM && !isRdpaGBEAEsysLockedIn)
      {
         WanDslIntfCfgObject *dslIntfCfg=NULL;
         UBOOL8 changed=FALSE;
         UBOOL8 atmWanLinkUp=FALSE;
         CmsRet ret;

         if ((ret = cmsObj_get(MDMOID_WAN_DSL_INTF_CFG, &(wanLinkInfo->iidStack), 0, (void **) &dslIntfCfg))
                              == CMSRET_SUCCESS)
         {
#ifndef DMP_X_BROADCOM_COM_DSLBONDING_1
            /* This is for no bonding atm wan link status */
            atmWanLinkUp =  (cmsUtl_strcmp(dslIntfCfg->status, MDMVS_UP)==0) ? TRUE : FALSE;  
            
#else  /* BONDING section */

            UBOOL8 bothLinkStateChange=FALSE;

            displayBondingStateTable("Before read the xDsl driver", &atmDslBondingLinksStateTable);

            /* First get the new line 0 link state */
            atmDslBondingLinksStateTable.isNewLine0Up = (cmsUtl_strcmp(dslIntfCfg->status, MDMVS_UP)==0) ? TRUE : FALSE;  

            /* get new secondary line (1) */
            atmDslBondingLinksStateTable.isNewLine1Up = isBondedAtmLinkUp();

            /* Now, compare with the previous (curr) link stats to decide
            * if the both links is changed at the same time or not.  If both changed, and only one of the 2 lines is up, 
            * atmWanLinkUp should not be up since there could be  a time that both are down and wan link need 
            * to be tear down.  If both lines change and both lines are up, the wan link should be up.
            */
            if ((atmDslBondingLinksStateTable.isNewLine0Up != atmDslBondingLinksStateTable.isCurrLine0Up) &&
                (atmDslBondingLinksStateTable.isNewLine1Up != atmDslBondingLinksStateTable.isCurrLine1Up))
            {
               bothLinkStateChange = TRUE;
            }

            displayBondingStateTable("after read  the xDsl driver", &atmDslBondingLinksStateTable);
            cmsLog_notice("bothLinkStateChange %d", bothLinkStateChange);

            if (!bothLinkStateChange &&
                (atmDslBondingLinksStateTable.isNewLine0Up || atmDslBondingLinksStateTable.isNewLine1Up))
            {               
               atmWanLinkUp = TRUE;
            }

            if (bothLinkStateChange &&
                (atmDslBondingLinksStateTable.isNewLine0Up && atmDslBondingLinksStateTable.isNewLine1Up))
            {               
               atmWanLinkUp = TRUE;
            }
            
            /* save the new in curr link stats */
            atmDslBondingLinksStateTable.isCurrLine0Up = atmDslBondingLinksStateTable.isNewLine0Up;
            atmDslBondingLinksStateTable.isCurrLine1Up = atmDslBondingLinksStateTable.isNewLine1Up;
            
#endif /* DMP_X_BROADCOM_COM_DSLBONDING_1 */

            if (!wanLinkInfo->isUp && atmWanLinkUp)
            {
               /* ATM wan link went from down to up */

               wanLinkInfo->isUp = TRUE;
               changed = TRUE;
               printf("(ssk) xDSL link up, Connection Type: ATM\n");
               matchRdpaWanType("DSL");
            }
            else if (wanLinkInfo->isUp && !atmWanLinkUp)
            {
               /* ATM wan link went from up to down */

               wanLinkInfo->isUp = FALSE;
               changed = TRUE;
               printf("(ssk) xDSL ATM link down.\n");
            }
            else
            {
               cmsLog_debug("no change in ATM link status, %d", wanLinkInfo->isUp);
            }

            cmsObj_free((void **) &dslIntfCfg);

            if (changed)
            {
               /* For layer 2 link state: down -> up, need to work on layer 2 object first in fixATMPvcStatusLocked
               * then layer 3 object in updateWanConnStatusInSubtreeLocked
               * For layer 2 link state: up -> down, work on layer 3 first then layer 2 
               */
               if (wanLinkInfo->isUp)
               {
                  fixATMPvcStatusLocked(&(wanLinkInfo->iidStack), wanLinkInfo->isUp);
                  updateWanConnStatusInSubtreeLocked(&(wanLinkInfo->iidStack), wanLinkInfo->isUp);
               }
               else
               {
                  updateWanConnStatusInSubtreeLocked(&(wanLinkInfo->iidStack), wanLinkInfo->isUp);
                  fixATMPvcStatusLocked(&(wanLinkInfo->iidStack), wanLinkInfo->isUp);
               }            
            }
         }
         else
         {
            cmsLog_error("cmsObj_get MDMOID_WAN_DSL_INTF_CFG returns error %d", ret);
         }
      }
#endif /* DMP_X_BROADCOM_COM_ATMWAN_1 */

#ifdef DMP_PTMWAN_1
      if (wanLinkInfo->isPTM && !isRdpaGBEAEsysLockedIn)
      {
         WanDslIntfCfgObject *dslIntfCfg=NULL;
         UBOOL8 changed=FALSE;
         UBOOL8 ptmWanLinkUp=FALSE;
         CmsRet ret;

         if ((ret = cmsObj_get(MDMOID_WAN_DSL_INTF_CFG, &(wanLinkInfo->iidStack), 0, (void **) &dslIntfCfg))
                              == CMSRET_SUCCESS)
         {

#ifndef DMP_X_BROADCOM_COM_DSLBONDING_1
            /* This is for no bonding ptm wan link status */
            ptmWanLinkUp =  (cmsUtl_strcmp(dslIntfCfg->status, MDMVS_UP)==0) ? TRUE : FALSE;  
            
#else   /* BONDING section */

            UBOOL8 bothLinkStateChange=FALSE;

            displayBondingStateTable("Before read the xDsl driver", &ptmDslBondingLinksStateTable);

            /* First get the new line 0 link state */
            ptmDslBondingLinksStateTable.isNewLine0Up = (cmsUtl_strcmp(dslIntfCfg->status, MDMVS_UP)==0) ? TRUE : FALSE;

            /* get new secondary line (1) */
            ptmDslBondingLinksStateTable.isNewLine1Up = isBondedPtmLinkUp();

            /* Now, compare with the previous (curr) link stats to decide
            * if the both links is changed at the same time or not.  If both changed, and only one of the 2 lines is up, 
            * ptmWanLinkUp should not be up since there could be  a time that both are down and wan link need 
            * to be tear down.  If both lines change and both lines are up, the wan link should be up.
            */
            if ((ptmDslBondingLinksStateTable.isNewLine0Up != ptmDslBondingLinksStateTable.isCurrLine0Up) &&
                (ptmDslBondingLinksStateTable.isNewLine1Up != ptmDslBondingLinksStateTable.isCurrLine1Up))
            {
               bothLinkStateChange = TRUE;
            }

            displayBondingStateTable("after read  the xDsl driver", &ptmDslBondingLinksStateTable);
            cmsLog_notice("bothLinkStateChange %d", bothLinkStateChange);

            if (!bothLinkStateChange &&
                (ptmDslBondingLinksStateTable.isNewLine0Up || ptmDslBondingLinksStateTable.isNewLine1Up))
            {               
               ptmWanLinkUp = TRUE;
            }

            if (bothLinkStateChange &&
                (ptmDslBondingLinksStateTable.isNewLine0Up && ptmDslBondingLinksStateTable.isNewLine1Up))
            {               
               ptmWanLinkUp = TRUE;
            }
            
            /* save the new in curr link stats */
            ptmDslBondingLinksStateTable.isCurrLine0Up = ptmDslBondingLinksStateTable.isNewLine0Up;
            ptmDslBondingLinksStateTable.isCurrLine1Up = ptmDslBondingLinksStateTable.isNewLine1Up;
            
#endif /* DMP_X_BROADCOM_COM_DSLBONDING_1 */

            if (!wanLinkInfo->isUp && ptmWanLinkUp)
            {
               /* PTM wan link went from down to up */

               wanLinkInfo->isUp = TRUE;
               changed = TRUE;
               printf("(ssk) xDSL link up, Connection Type: PTM\n");
               matchRdpaWanType("DSL");
            }
            else if (wanLinkInfo->isUp && !ptmWanLinkUp)
            {
               /* PTM wan link went from up to down */

               wanLinkInfo->isUp = FALSE;
               changed = TRUE;
               printf("(ssk) xDSL PTM link down.\n");
            }
            else
            {
               cmsLog_debug("no change in PTM link status, %d", wanLinkInfo->isUp);
            }

            if (dslIntfCfg->errorSamplesAvailable)
            {
              informErrorSampleStatusChangeLocked(&(wanLinkInfo->iidStack));
            }
            
            cmsObj_free((void **) &dslIntfCfg);

            if (changed)
            {
               /* For layer 2 link state: down -> up, need to work on layer 2 object first in fixPtmChannelStatusLocked
               * then layer 3 object in updateWanConnStatusInSubtreeLocked
               * For layer 2 link state: up -> down, work on layer 3 first then layer 2 
               */
               if (wanLinkInfo->isUp)
               {
                  fixPtmChannelStatusLocked(&(wanLinkInfo->iidStack), wanLinkInfo->isUp);
                  updateWanConnStatusInSubtreeLocked(&(wanLinkInfo->iidStack), wanLinkInfo->isUp);
               }
               else
               {
                  updateWanConnStatusInSubtreeLocked(&(wanLinkInfo->iidStack), wanLinkInfo->isUp);
                  fixPtmChannelStatusLocked(&(wanLinkInfo->iidStack), wanLinkInfo->isUp);
               }
            }
         }
         else
         {
            cmsLog_error("cmsObj_get MDMOID_WAN_DSL_INTF_CFG returns error %d", ret);
         }
      }
#endif /* DMP_PTMWAN_1 */
            

#ifdef DMP_X_BROADCOM_COM_MOCAWAN_1 
      if (wanLinkInfo->isMoca)
      {
         WanMocaIntfObject *wanMocaIntf=NULL;
         UBOOL8 changed=FALSE;
         CmsRet ret;

         if ((ret = cmsObj_get(MDMOID_WAN_MOCA_INTF, &(wanLinkInfo->iidStack), 0, (void **)&wanMocaIntf))
                              == CMSRET_SUCCESS)
         {
            if (!wanLinkInfo->isUp && !cmsUtl_strcmp(wanMocaIntf->status, MDMVS_UP))
            {
               /* Moca wan link went from down to up */

               wanLinkInfo->isUp = TRUE;
               changed = TRUE;
               printf("(ssk) MocaWan %s link up\n", wanMocaIntf->ifName);
               matchRdpaWanType("GBE");
            }
            else if (wanLinkInfo->isUp && cmsUtl_strcmp(wanMocaIntf->status, MDMVS_UP))
            {
               /* Moca wan link went from up to down */

               wanLinkInfo->isUp = FALSE;
               changed = TRUE;

               printf("(ssk) MocaWan %s link down\n", wanMocaIntf->ifName);
            }
            else
            {
               cmsLog_debug("no change in moca link status, %s", wanMocaIntf->status);
            }

            if (changed)
            {
               cmsLog_debug("send link up message 1");
               sendStatusMsgToSmd((wanLinkInfo->isUp ? CMS_MSG_WAN_LINK_UP : CMS_MSG_WAN_LINK_DOWN), 
                                  wanMocaIntf->ifName);
               updateWanConnStatusInSubtreeLocked(&(wanLinkInfo->iidStack), wanLinkInfo->isUp);
            }

            cmsObj_free((void **)&wanMocaIntf);
         }
         else
         {
            cmsLog_error("cmsObj_get MDMOID_WAN_MOCA_INTF returns error %d", ret);
         }
      }
#endif /* DMP_X_BROADCOM_COM_MOCAWAN_1 */

#ifdef DMP_X_BROADCOM_COM_GPONWAN_1 
      /* Since gpon interface link is virtual (via cms message instead of physical (from drivers), the layer 3
      * actions are done in the processGponWanLinkChange function
      */
#endif /* DMP_X_BROADCOM_COM_GPONWAN_1 */


#ifdef DMP_X_BROADCOM_COM_EPONWAN_1 
      if (wanLinkInfo->isEpon)
      {
#ifdef EPON_SFU
         WanEponIntfObject *wanEponIntf=NULL;
         UBOOL8 changed=FALSE;
         CmsRet ret;

         if ((ret = cmsObj_get(MDMOID_WAN_EPON_INTF, &(wanLinkInfo->iidStack), 0, (void **)&wanEponIntf))
                              == CMSRET_SUCCESS)
         {
            if (!wanLinkInfo->isUp && !cmsUtl_strcmp(wanEponIntf->status, MDMVS_UP))
            {
               /* Eth wan link went from down to up */

               wanLinkInfo->isUp = TRUE;
               changed = TRUE;
               printf("(ssk) EponWan %s link up\n", wanEponIntf->ifName);
            }
            else if (wanLinkInfo->isUp && cmsUtl_strcmp(wanEponIntf->status, MDMVS_UP))
            {
               /* Epon wan link went from up to down */

               wanLinkInfo->isUp = FALSE;
               changed = TRUE;

               printf("(ssk) EponWan %s link down\n", wanEponIntf->ifName);
            }
            else
            {
               cmsLog_debug("no change in Epon WAN link status, %s", wanEponIntf->status);
            }

            if (changed)
            {
               cmsLog_debug("send link up message");
               sendStatusMsgToSmd((wanLinkInfo->isUp ? CMS_MSG_WAN_LINK_UP : CMS_MSG_WAN_LINK_DOWN), 
                                  wanEponIntf->ifName);
               updateWanConnStatusInSubtreeLocked(&(wanLinkInfo->iidStack), wanLinkInfo->isUp);
            }

            cmsObj_free((void **)&wanEponIntf);
         }
         else
         {
            cmsLog_error("cmsObj_get MDMOID_WAN_EPON_INTF returns error %d", ret);
         }
#else
         InstanceIdStack eponLinkIid = EMPTY_INSTANCE_ID_STACK;   
         WanEponLinkCfgObject *eponLinkCfg = NULL;
         UBOOL8 changed=FALSE;
         CmsRet ret;

         while ((ret = cmsObj_getNextInSubTreeFlags
            (MDMOID_WAN_EPON_LINK_CFG, &(wanLinkInfo->iidStack), &eponLinkIid, OGF_NO_VALUE_UPDATE, (void **)&eponLinkCfg)) == CMSRET_SUCCESS)
         {
            if (!wanLinkInfo->isUp && !cmsUtl_strcmp(eponLinkCfg->linkStatus, MDMVS_UP))
            {
               /* Eth wan link went from down to up */

               wanLinkInfo->isUp = TRUE;
               changed = TRUE;
               printf("(ssk) EponWan %s link up\n", eponLinkCfg->ifName);
            }
            else if (wanLinkInfo->isUp && cmsUtl_strcmp(eponLinkCfg->linkStatus, MDMVS_UP))
            {
               /* Epon wan link went from up to down */

               wanLinkInfo->isUp = FALSE;
               changed = TRUE;

               printf("(ssk) EponWan %s link down\n", eponLinkCfg->ifName);
            }
            else
            {
               cmsLog_debug("no change in Epon WAN link status, %s", eponLinkCfg->linkStatus);
            }

            if (changed)
            {
               cmsLog_debug("send link up message");
               sendStatusMsgToSmd((wanLinkInfo->isUp ? CMS_MSG_WAN_LINK_UP : CMS_MSG_WAN_LINK_DOWN), 
                                  eponLinkCfg->ifName);
               updateWanConnStatusInSubtreeLocked(&(wanLinkInfo->iidStack), wanLinkInfo->isUp);
            }

            cmsObj_free((void **)&eponLinkCfg);
         }
#endif
      }

#endif /* DMP_X_BROADCOM_COM_EPONWAN_1 */

   }  /* end of iteration through all the wanLinkInfo's */

   return FALSE;
}
#endif  /* DMP_BASELINE_1 */


#ifdef DMP_X_BROADCOM_COM_ATMWAN_1
void fixATMPvcStatusLocked(const InstanceIdStack *parentIidStack, UBOOL8 wanLinkUp)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanDslLinkCfgObject *dslLinkCfg = NULL;

   /*
    * mwang: hmm, we have to start the dslLinkCfg, which is perVCC, which goes into the
    * rcl handler function.  THis could be done better.
    */
   while (cmsObj_getNextInSubTree(MDMOID_WAN_DSL_LINK_CFG, parentIidStack, &iidStack, (void **)&dslLinkCfg) == CMSRET_SUCCESS)
   {
      cmsLog_debug("wanLinkUp=%d, dslLinkCfg->enable=%d, dslLinkCfg->linkStatus=%s", 
                   wanLinkUp, dslLinkCfg->enable, dslLinkCfg->linkStatus);

      if (dslLinkCfg->enable)
      {
         /* if dslLinkCfg is enabled and dsl link is up and dslLinkCfg->linkStatus is not "UP", start lay2 interface */
         if (wanLinkUp && cmsUtl_strcmp(dslLinkCfg->linkStatus, MDMVS_UP))
         {
            CMSMEM_REPLACE_STRING(dslLinkCfg->linkStatus, MDMVS_UP);
            cmsLog_debug("Activate PVC on %s", dslLinkCfg->X_BROADCOM_COM_IfName);
            ret = cmsObj_set(dslLinkCfg, &iidStack);

            cmsLog_debug("send link up message2");
            sendStatusMsgToSmd(CMS_MSG_WAN_LINK_UP, dslLinkCfg->X_BROADCOM_COM_IfName);
         }
         else if (!wanLinkUp && !cmsUtl_strcmp(dslLinkCfg->linkStatus, MDMVS_UP))
         {
            CMSMEM_REPLACE_STRING(dslLinkCfg->linkStatus, MDMVS_DOWN);
            cmsLog_debug("Deactivate PVC on %s", dslLinkCfg->X_BROADCOM_COM_IfName);
            ret = cmsObj_set(dslLinkCfg, &iidStack);

            cmsLog_debug("send link up message3");
            sendStatusMsgToSmd(CMS_MSG_WAN_LINK_DOWN, dslLinkCfg->X_BROADCOM_COM_IfName);
         }
         if (ret != CMSRET_SUCCESS)
         {
            cmsObj_free((void **) &dslLinkCfg);
            cmsLog_error("Failed to set PVC status, error=%d", ret);
            return;
         }            
      }
      else
      {
         cmsLog_debug("dslLinkCfg is not enabled");
      }
         
      cmsObj_free((void **) &dslLinkCfg);
   }
}

#endif /* DMP_X_BROADCOM_COM_ATMWAN_1 */

#ifdef DMP_PTMWAN_1

void informErrorSampleStatusChangeLocked(const InstanceIdStack *parentIidStack)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanPtmLinkCfgObject *ptmLinkCfg = NULL;
 
   cmsLog_debug("Inform Error sample status");
   while (cmsObj_getNextInSubTree(MDMOID_WAN_PTM_LINK_CFG, parentIidStack, &iidStack, (void **)&ptmLinkCfg) == CMSRET_SUCCESS)
   {
      if (ptmLinkCfg->enable)
      {
        sendStatusMsgToSmd(CMS_MSG_WAN_ERRORSAMPLES_AVAILABLE, ptmLinkCfg->X_BROADCOM_COM_IfName);
        cmsLog_debug("ErrorSamplesAvailable event received");
      }
      cmsObj_free((void **) &ptmLinkCfg);
      if (ret != CMSRET_SUCCESS)
      {
        cmsLog_error("Failed to set ptmLinkCfg. ret=%d", ret);
        return;
      }
   }
}

void fixPtmChannelStatusLocked(const InstanceIdStack *parentIidStack, UBOOL8 wanLinkUp)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanPtmLinkCfgObject *ptmLinkCfg = NULL;
 
   while (cmsObj_getNextInSubTree(MDMOID_WAN_PTM_LINK_CFG, parentIidStack, &iidStack, (void **)&ptmLinkCfg) == CMSRET_SUCCESS)
   {
      cmsLog_debug("wanLinkUp=%d, ptmLinkCfg->enable=%d, ptmLinkCfg->linkStatus=%s",
                   wanLinkUp, ptmLinkCfg->enable, ptmLinkCfg->linkStatus);

      if (ptmLinkCfg->enable)
      {
         /* if ptmLinkCfg is enabled and dsl link is up and ptmLinkCfg->linkStatus is not "UP", start xtm operation */
         if (ptmLinkCfg->enable && wanLinkUp && cmsUtl_strcmp(ptmLinkCfg->linkStatus, MDMVS_UP))
         {
            CMSMEM_REPLACE_STRING(ptmLinkCfg->linkStatus, MDMVS_UP);
            cmsLog_debug("Activate PTM channel %s", ptmLinkCfg->X_BROADCOM_COM_IfName);
            ret = cmsObj_set(ptmLinkCfg, &iidStack);

            cmsLog_debug("send link up message4");
            sendStatusMsgToSmd(CMS_MSG_WAN_LINK_UP, ptmLinkCfg->X_BROADCOM_COM_IfName);
         }
         else if (!wanLinkUp && !cmsUtl_strcmp(ptmLinkCfg->linkStatus, MDMVS_UP))
         {
            CMSMEM_REPLACE_STRING(ptmLinkCfg->linkStatus, MDMVS_DOWN);
            cmsLog_debug("Deactivate PTM channel %s", ptmLinkCfg->X_BROADCOM_COM_IfName);
            ret = cmsObj_set(ptmLinkCfg, &iidStack);

            cmsLog_debug("send link up message5");
            sendStatusMsgToSmd(CMS_MSG_WAN_LINK_DOWN, ptmLinkCfg->X_BROADCOM_COM_IfName);
         }

         if (ret != CMSRET_SUCCESS)
         {
            cmsObj_free((void **) &ptmLinkCfg);
            cmsLog_error("Failed to set ptmLinkCfg. ret=%d", ret);
            return;
         }
      }

      cmsObj_free((void **) &ptmLinkCfg);
   }
}
#endif /* DMP_PTMWAN_1 */


#if defined(DMP_X_BROADCOM_COM_ATMWAN_1) && defined(DMP_X_BROADCOM_COM_DSLBONDING_1)
UBOOL8 isBondedAtmLinkUp(void)
{
   UBOOL8 isUp=FALSE;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   WanDslIntfCfgObject *bondedDslIntfObj=NULL;
   CmsRet ret;

   ret = dalDsl_getBondingAtmDslIntfObject(&iidStack, &bondedDslIntfObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_debug("could not get Bonded WanDslIntfObject, ret=%d", ret);
      return isUp;
   }

   isUp = !cmsUtl_strcmp(bondedDslIntfObj->status, MDMVS_UP);

   cmsObj_free((void **) &bondedDslIntfObj);

   return isUp;
}
#endif  /* DMP_X_BROADCOM_COM_ATMWAN_1 && DMP_X_BROADCOM_COM_DSLBONDING_1 */


#if defined(DMP_PTMWAN_1) && defined(DMP_X_BROADCOM_COM_DSLBONDING_1)

UBOOL8 isBondedPtmLinkUp(void)
{
   UBOOL8 isUp=FALSE;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   WanDslIntfCfgObject *bondedDslIntfObj=NULL;
   CmsRet ret;

   ret = dalDsl_getBondingPtmDslIntfObject(&iidStack, &bondedDslIntfObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_debug("could not get Bonded WanDslIntfObject, ret=%d", ret);
      return isUp;
   }

   isUp = !cmsUtl_strcmp(bondedDslIntfObj->status, MDMVS_UP);

   cmsObj_free((void **) &bondedDslIntfObj);

   return isUp;
}
#endif  /* DMP_PTMWAN_1 && DMP_X_BROADCOM_COM_DSLBONDING_1 */

#ifdef SUPPORT_DSL_BONDING
void setWanDslTrafficType ()
{
   int status ;
   CmsRet ret ;
   XTM_BOND_INFO bondInfo ;

   ret = dalDsl_getDslBonding (&status) ;
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_debug("could not get Bonded WanDslIntfObject, ret=%d", ret) ;
      return ;
   }

	ret = devCtl_xtmGetBondingInfo ( &bondInfo );

   status =  (ret == CMSRET_SUCCESS) ? 0x1 : 0x0 ;

   sleep (1) ;

	if (status == 0x1) {

		ret = dalDsl_setDslBonding (status) ;
		if (ret != CMSRET_SUCCESS)
		{
			cmsLog_debug("could not set Bonded WanDslIntfObject, ret=%d", ret) ;
			return ;
		}

		cmsMgm_saveConfigToFlash() ;
	}

	/* else for non-bonded traffic, we will still keep the system mode as dual
	 * interface mode, as it is a super set configuration, which  will work for
	 * single line mode & will facilitate future dual interface configuration
	 * switching */
}
#endif /* SUPPORT_DSL_BONDING */


void getDslDiagResults(void)
{
#ifdef DMP_DSLDIAGNOSTICS_1
   if (dslLoopDiagInfo.dslLoopDiagInProgress == TRUE)
   {
      getDslLoopDiagResults();
   }
#endif   
#ifdef DMP_X_BROADCOM_COM_SELT_1
   if (dslDiagInfo.dslDiagInProgress == TRUE)
   {
      getDslSeltDiagResults();
   }
#endif
}

#ifdef DMP_DSLDIAGNOSTICS_1
void processWatchDslLoopDiag_igd(CmsMsgHeader *msg)
{
   WanDslDiagObject *dslDiagObj;
   dslDiagMsgBody *info = (dslDiagMsgBody*) (msg+1);
   InstanceIdStack iidStack = info->iidStack;
   CmsRet ret;

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) == CMSRET_SUCCESS)
   {
      if (cmsObj_get(MDMOID_WAN_DSL_DIAG, &iidStack, 0, (void **) &dslDiagObj) == CMSRET_SUCCESS)
      {
         CMSMEM_REPLACE_STRING(dslDiagObj->loopDiagnosticsState,MDMVS_REQUESTED);
         if ((ret = cmsObj_set(dslDiagObj, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_set returns error. ret=%d", ret);
         }
         else
         {
            dslDiagInProgress = TRUE;
            dslLoopDiagInfo.dslLoopDiagInProgress = TRUE;
            dslLoopDiagInfo.iidStack = info->iidStack;
            dslLoopDiagInfo.pollRetries = 0;
            dslLoopDiagInfo.src = msg->src;
         }
         cmsObj_free((void **) &dslDiagObj);
      }
      cmsLck_releaseLock();
   } /* aquire lock ok */
}

void getDslLoopDiagResults_igd(void)
{
   WanDslDiagObject *dslDiagObj;
   InstanceIdStack iidStack = dslLoopDiagInfo.iidStack;
   WanDslIntfCfgObject *dslCfgObj=NULL;
   UINT32 lineId=0;
   
   cmsLog_debug("Enter: dslLoopDiagInfo.pollRetries %d, inProgress %d",dslLoopDiagInfo.pollRetries,
                dslLoopDiagInfo.dslLoopDiagInProgress);

   if ((cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) == CMSRET_SUCCESS)
   {   
      if (cmsObj_get(MDMOID_WAN_DSL_DIAG, &iidStack, 0, (void **) &dslDiagObj) == CMSRET_SUCCESS)
      {
         if ((cmsUtl_strcmp(dslDiagObj->loopDiagnosticsState, MDMVS_REQUESTED) == 0) &&
             (dslLoopDiagInfo.pollRetries < DIAG_DSL_LOOPBACK_TIMEOUT_PERIOD))
         {
            dslLoopDiagInfo.pollRetries++;
         }
         else
         {
            if (dslLoopDiagInfo.pollRetries >= DIAG_DSL_LOOPBACK_TIMEOUT_PERIOD)
            {
               CMSMEM_REPLACE_STRING(dslDiagObj->loopDiagnosticsState,MDMVS_ERROR_INTERNAL);
               cmsObj_set(dslDiagObj, &iidStack);
            }
            dslDiagInProgress=FALSE;
            CmsMsgHeader msg = EMPTY_MSG_HEADER;
            dslLoopDiagInfo.dslLoopDiagInProgress = FALSE;
            dslLoopDiagInfo.pollRetries = 0;

            if (dslLoopDiagInfo.src == EID_TR69C)
            {
               msg.type = CMS_MSG_DSL_LOOP_DIAG_COMPLETE;
               msg.src =  EID_SSK;
               msg.dst = EID_TR69C;
               msg.flags_event = 1;
               if (cmsObj_get(MDMOID_WAN_DSL_INTF_CFG, &iidStack, 0,(void **)&dslCfgObj) == CMSRET_SUCCESS)
               {
                  lineId = dslCfgObj->X_BROADCOM_COM_BondingLineNumber;
                  cmsObj_free((void **) &dslCfgObj);
               }
               msg.wordData = lineId;               
               if (cmsMsg_send(msgHandle, &msg) != CMSRET_SUCCESS)
               {
                  cmsLog_error("could not send out CMS_MSG_DSL_LOOP_DIAG_COMPLETE event msg");
               }
               else
               {
                  cmsLog_debug("Send out CMS_MSG_DSL_LOOP_DIAG_COMPLETE event msg.");
               }
            }
         }
         cmsObj_free((void **) &dslDiagObj);

      } /* get obj ok */
      cmsLck_releaseLock();
   } /* lock requested ok */
}

#endif /* DMP_DSLDIAGNOSTICS_1 */

#ifdef DMP_ETHERNETWAN_1
UBOOL8 checkWanEthLinkStatusLocked_igd(const char *intfName)
{
   WanLinkInfo *wanLinkInfo;
   UBOOL8 found=FALSE;
#if !defined(CHIP_63138) && !defined(CHIP_63148) && !defined(CHIP_63158) && !defined(CHIP_4908)
   char wan_sp_str[10];
#endif
   cmsLog_debug("Entered: intfName=%s", intfName);

   dlist_for_each_entry(wanLinkInfo, &wanLinkInfoHead, dlist)
   {
      WanEthIntfObject *wanEthIntf=NULL;
      UBOOL8 changed=FALSE;
      CmsRet ret;

      if (wanLinkInfo->isEth)
      {
         ret = cmsObj_get(MDMOID_WAN_ETH_INTF, &(wanLinkInfo->iidStack), 0, (void **)&wanEthIntf);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_get MDMOID_WAN_ETH_INTF returns error %d", ret);
            return found;
         }

         cmsLog_debug("wanEthIntf name=%s status=%s (prev isUp=%d)",
                       wanEthIntf->X_BROADCOM_COM_IfName, wanEthIntf->status,
                       wanLinkInfo->isUp);

         if (intfName &&
             !cmsUtl_strcmp(wanEthIntf->X_BROADCOM_COM_IfName, intfName))
         {
            found = TRUE;
         }

         if (!wanLinkInfo->isUp && !cmsUtl_strcmp(wanEthIntf->status, MDMVS_UP))
         {
            /* Eth wan link went from down to up */
            wanLinkInfo->isUp = TRUE;
            changed = TRUE;

            printf("(ssk) EthernetWan %s link up\n", wanEthIntf->X_BROADCOM_COM_IfName);
#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_63158) || defined(CHIP_4908)
            matchRdpaWanType("GBE");
#else
            strcpy (wan_sp_str,"GBE ");
            strcat(wan_sp_str, wanEthIntf->X_BROADCOM_COM_IfName);
            matchRdpaWanType(wan_sp_str);
#endif
         }
         else if (wanLinkInfo->isUp && cmsUtl_strcmp(wanEthIntf->status, MDMVS_UP))
         {
            /* Eth wan link went from up to down */
            wanLinkInfo->isUp = FALSE;
            changed = TRUE;

            printf("(ssk) EthernetWan %s link down\n", wanEthIntf->X_BROADCOM_COM_IfName);
         }
         else
         {
            cmsLog_debug("no change in eth link status, %s", wanEthIntf->status);
         }

         if (changed)
         {
            cmsLog_debug("send link up message");
            sendStatusMsgToSmd((wanLinkInfo->isUp ? CMS_MSG_WAN_LINK_UP : CMS_MSG_WAN_LINK_DOWN),
                               wanEthIntf->X_BROADCOM_COM_IfName);
            updateWanConnStatusInSubtreeLocked(&(wanLinkInfo->iidStack), wanLinkInfo->isUp);
         }

         cmsObj_free((void **)&wanEthIntf);
      }
   }

   return found;
}
#endif /* DMP_ETHERNETWAN_1 */




#ifdef DMP_X_BROADCOM_COM_WIFIWAN_1

/** Note the reason we come into this function is a lucky coincidence.
 *  Usually wl0 is configured as the WAN interface and wl0.1 must be enabled
 *  and is on the LAN side.  When wl0 is successfully associated with the AP,
 *  the wl0.1 intf generates a standard Linux netlink msg.  This is detected
 *  by processStdNetlinkMonitor, which calls updateLinkStatus(wl0.1).
 *  UpdateLinkStatus checks the WAN side links first, so we get here and
 *  detect that wl0 is up.  Previous code relied on getting a
 *  CMS_MSG_WLAN_LINK_STATUS_CHANGED from wlevt, but wlevt never sent it.
 *
 */
UBOOL8 checkWanWifiLinkStatusLocked_igd(const char *intfName)
{
   WanLinkInfo *wanLinkInfo;
   UBOOL8 found=FALSE;

   cmsLog_debug("Entered: intfName=%s", intfName);

   dlist_for_each_entry(wanLinkInfo, &wanLinkInfoHead, dlist)
   {
      if (wanLinkInfo->isWifi)
      {
         WanWifiIntfObject *wanWifiIntf=NULL;
         UBOOL8 changed=FALSE;
         CmsRet ret;

         if ((ret = cmsObj_get(MDMOID_WAN_WIFI_INTF, &(wanLinkInfo->iidStack),
                                0, (void **)&wanWifiIntf)) == CMSRET_SUCCESS)
         {
            cmsLog_debug("wanWifiIntf name=%s status=%s (prev isUp=%d)",
                          wanWifiIntf->ifName, wanWifiIntf->status,
                          wanLinkInfo->isUp);

            if (intfName &&
                !cmsUtl_strcmp(wanWifiIntf->ifName, intfName))
            {
               found = TRUE;
            }

            if (!wanLinkInfo->isUp && !cmsUtl_strcmp(wanWifiIntf->status, MDMVS_UP))
            {
               /* Wifi wan link went from down to up */

               wanLinkInfo->isUp = TRUE;
               changed = TRUE;
               printf("(ssk) WifiWan %s link up\n", wanWifiIntf->ifName);
            }
            else if (wanLinkInfo->isUp && cmsUtl_strcmp(wanWifiIntf->status, MDMVS_UP))
            {
               /* Wifi wan link went from up to down */

               wanLinkInfo->isUp = FALSE;
               changed = TRUE;

               printf("(ssk) WifiWan %s link down\n", wanWifiIntf->ifName);
            }
            else
            {
               cmsLog_debug("no change in wl link status, %s", wanWifiIntf->status);
            }

            if (changed)
            {
               cmsLog_debug("send link up message");
               sendStatusMsgToSmd((wanLinkInfo->isUp ? CMS_MSG_WAN_LINK_UP : CMS_MSG_WAN_LINK_DOWN),
                                  wanWifiIntf->ifName);
               updateWanConnStatusInSubtreeLocked(&(wanLinkInfo->iidStack), wanLinkInfo->isUp);
            }

            cmsObj_free((void **)&wanWifiIntf);
         }
         else
         {
            cmsLog_error("cmsObj_get MDMOID_WAN_WIFI_INTF returns error %d", ret);
         }
      }
   }

   return found;
}
#endif /* DMP_X_BROADCOM_COM_WIFIWAN_1 */


/* This function does not need any ifdefs.  It works on RDPA and non-RDPA
 * systems, works for TR98 or TR181.
 */
#if defined(BCM_PON)
void matchRdpaWanType(const char *wanTypeUp)
{
   CmsRet ret;
   SINT32 count;
   UINT32 length;
   char   *token;
   char buf[BUFLEN_16]={0};
   char buf2[BUFLEN_16]={0};
   char wanemac[]= "EMACX";

   if (isRdpaGBEAEsysLockedIn)
   {
      /*
       * Once we have determined that this is a Gigabit Ethernet WAN system,
       * don't bother checking anymore.
       */
      return;
   }

   count = cmsPsp_get(RDPA_WAN_TYPE_PSP_KEY, buf, sizeof(buf));
   if (count == 0)
   {
      /* key not found or error, this is not a RDPA system, do nothing. */
      return;
   }

   if (count < 0)
   {
      cmsLog_error("unexpected data in %s, len=%d", RDPA_WAN_TYPE_PSP_KEY, count);
      return;
   }

   /* Multiple Wan Emac Support Scenario */
   token = cmsUtl_strstr(wanTypeUp,"eth");
   if (token)
   {
	   wanemac[strWanoeMacInx]=*(token+strlen("eth"));
	   /* Get the OEMAC SP value */
	   count = cmsPsp_get(RDPA_WAN_OEMAC_PSP_KEY, buf2, sizeof(buf2));
	   if (count == 0)
	   {
		   printf("\n*** SP WanOEMac not defined !!! ");
		   return;
	   }
	   token = cmsUtl_strstr(wanTypeUp,"5");
	   /* for case of AE str in SP which coherent to GBE with EMAC5 */
	   if (token && !cmsUtl_strncasecmp((const char *)buf, "AE",strlen("AE")))
	   {
		   cmsLog_debug("wan type %s matched RDPA init.  No action.", (const char *)buf);
		   isRdpaGBEAEsysLockedIn=TRUE;
		   return;
	   }
	   else if (cmsUtl_strncasecmp((const char *)buf2, wanemac,strlen(wanemac)))
	   {
           if(!cmsUtl_strncasecmp((const char *)buf2, "EPONMAC",strlen("EPONMAC")))
           {
               isRdpaGBEAEsysLockedIn=TRUE;
               return;
           }
           printf("\n*** WanOEMac can not be changed, back to previous WanOEMac configuration !!!");
		   /* Prepare the wanoe str */
		   wanemac[strWanoeMacInx] = wanTypeUp[strEthIdInx];
		   ret = cmsPsp_set(RDPA_WAN_OEMAC_PSP_KEY, wanemac, strlen(wanemac));
		   if (ret != CMSRET_SUCCESS)
		   {
			   cmsLog_error("Could not set PSP %s to %s, ret=%d (reboot canceled)",
					   RDPA_WAN_OEMAC_PSP_KEY, wanemac, ret);
			   return;
		   }
		   printf("\n*** please remove all previous wan services prior to SP change");
		   goto end_func;
	   }
   }


   token = cmsUtl_strcasestr(wanTypeUp,(const char *)buf);
   if (token)
   {
      cmsLog_debug("wan type %s matched RDPA init.  No action.", wanTypeUp);
      if (!cmsUtl_strncmp(token, "GBE",strlen("GBE")))
      {
         isRdpaGBEAEsysLockedIn=TRUE;
      }
      return;
   }

   /*
    * If we get here, we have a mismatch.  Record the actual wan type that
    * came up and reboot.
    */
   printf("\n*** (ssk) actual WAN link type %s does not match RDPA "
          "initialized type %s  -- reboot!\n\n", wanTypeUp, buf);

   length = strlen(wanTypeUp);
   if (length > strlen("GBE"))
   {
	   ret = cmsPsp_set(RDPA_WAN_TYPE_PSP_KEY, "GBE", strlen("GBE"));
	   if (ret != CMSRET_SUCCESS)
	   {
		   cmsLog_error("Could not set PSP %s to GBE, ret=%d (reboot canceled)",
				   RDPA_WAN_TYPE_PSP_KEY, ret);
		   return;
	   }
       /* Prepare the wanoe str */
       wanemac[strWanoeMacInx] = wanTypeUp[strEthIdInx]; 
       ret = cmsPsp_set(RDPA_WAN_OEMAC_PSP_KEY, wanemac, strlen(wanemac));
       if (ret != CMSRET_SUCCESS)
       {
           cmsLog_error("Could not set PSP %s to %s, ret=%d (reboot canceled)",
                   RDPA_WAN_OEMAC_PSP_KEY, wanemac, ret);
           return;
       }
       goto end_func;
   }

   ret = cmsPsp_set(RDPA_WAN_TYPE_PSP_KEY, wanTypeUp, strlen(wanTypeUp));
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not set PSP %s to %s, ret=%d (reboot canceled)",
                   RDPA_WAN_TYPE_PSP_KEY, wanTypeUp, ret);
      return;
   }

end_func:
   cmsUtil_sendRequestRebootMsg(msgHandle);
   /* after requesting reboot, we do not want to continue execution.
    * so just sleep here until we reboot.
    */
   sleep(120);
   cmsLog_error("System still not rebooted after 120 seconds??");
   exit(0);
}
#else
void matchRdpaWanType(const char *wanTypeUp __attribute__((unused)))
{
}
#endif


#if defined(DMP_X_BROADCOM_COM_GPONWAN_1)
static CmsRet updateGponLinkCfg(const OmciServiceMsgBody *info, InstanceIdStack *pGponLinkIid)
{
   CmsRet ret = CMSRET_INTERNAL_ERROR;
   UBOOL8 changed = FALSE;
   WanGponLinkCfgObject *gponLinkCfg = NULL;

   /* Find the GPON link config object for the given interface name */
   if (dalGpon_getGponLinkByIfName((char *)info->l2Ifname, pGponLinkIid, &gponLinkCfg) == FALSE)
   {
      goto out;
   }

   /* Even if gpon interface is not created yet ie. not enabled by userapps like WebUI or tr69,
   * just save the gpon interface link info for later use.  This is for the case  when the interface 
   * is created after the interface is already up.
   */
   if (info->serviceParams.serviceStatus == TRUE &&
       cmsUtl_strcmp(gponLinkCfg->linkStatus, MDMVS_UP))
   {
      CMSMEM_REPLACE_STRING(gponLinkCfg->linkStatus, MDMVS_UP);
      changed = TRUE;
   }
   else if (info->serviceParams.serviceStatus == FALSE &&
       cmsUtl_strcmp(gponLinkCfg->linkStatus, MDMVS_DOWN))
   {
      CMSMEM_REPLACE_STRING(gponLinkCfg->linkStatus, MDMVS_DOWN);
      changed = TRUE;
   }

   if (changed == TRUE)
   {
      /* save the gpon interface */
      if ((ret = cmsObj_set(gponLinkCfg, pGponLinkIid)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to set GponLinkCfgObject. ret=%d", ret);
         cmsObj_free((void **)&gponLinkCfg);
         goto out;
      }   
   }

   cmsObj_free((void **)&gponLinkCfg);

   ret = CMSRET_SUCCESS;

out:
   return ret;
}
static CmsRet updateGponServiceStatus(const OmciServiceMsgBody *info, InstanceIdStack *parentIidStack)
{
   CmsRet ret = CMSRET_INTERNAL_ERROR;
   SINT32 pbits = -1, vlanId = -1;
   UBOOL8 found = FALSE;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   WanConnDeviceObject *wanConn = NULL;
   WanIpConnObject *ipConn=NULL;
   WanPppConnObject *pppConn=NULL;

   /* Only work on the this WanConn and need this WanConn IidStack (gponLinkIid becomes wanConnIid after the call)
   * since each  gpon later 2 interface link status has one message from omicd
   */
   if ((ret = cmsObj_getAncestorFlags(MDMOID_WAN_CONN_DEVICE, 
      MDMOID_WAN_GPON_LINK_CFG, parentIidStack, OGF_NO_VALUE_UPDATE, (void **)&wanConn)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Fail to get cmsObj_getAncestor(MDMOID_WAN_CONN_DEVICE). ret=%d", ret);
      goto out;
   }
   cmsObj_free((void **) &wanConn);

   pbits = info->serviceParams.pbits;
   vlanId = info->serviceParams.vlanId;

   while (cmsObj_getNextInSubTreeFlags(MDMOID_WAN_IP_CONN, parentIidStack, &iidStack, OGF_NO_VALUE_UPDATE, (void **) &ipConn) == CMSRET_SUCCESS)
   {
      /* take care of don't care pbits and vlanId filter */
      if (ipConn->X_BROADCOM_COM_VlanMux8021p == -1 && ipConn->X_BROADCOM_COM_VlanMuxID == -1)
         found = TRUE;
      else if (pbits != -1 && vlanId != -1)
         found = (ipConn->X_BROADCOM_COM_VlanMux8021p == pbits &&
	          ipConn->X_BROADCOM_COM_VlanMuxID == vlanId);
      else if (pbits == -1 && vlanId == -1)
         found = TRUE;
      else if (pbits == -1)
         found = (ipConn->X_BROADCOM_COM_VlanMuxID == vlanId);
      else if (vlanId == -1)
         found = (ipConn->X_BROADCOM_COM_VlanMux8021p == pbits);

      if (found == TRUE)
      {
         cmsLog_debug("WanIpConnObject, ifName=%s, strStatus=%s, status=%d, pbits=%d, vlanId=%d",
            ipConn->X_BROADCOM_COM_IfName, ipConn->connectionStatus, info->serviceParams.serviceStatus, pbits, vlanId);
         if (info->serviceParams.serviceStatus == TRUE)
         {
            if (!cmsUtl_strcmp(ipConn->connectionStatus, MDMVS_UNCONFIGURED) ||
                !cmsUtl_strcmp(ipConn->connectionStatus, MDMVS_DISCONNECTED))
            {
               cmsLog_debug("Activate  gpon interface %s and send link up message", ipConn->X_BROADCOM_COM_IfName);
               sendStatusMsgToSmd(CMS_MSG_WAN_LINK_UP, ipConn->X_BROADCOM_COM_IfName);
               updateSingleWanConnStatusLocked(&iidStack, (void *)ipConn, info->serviceParams.serviceStatus);
            }
         }
         else
         {
            if (!cmsUtl_strcmp(ipConn->connectionStatus, MDMVS_CONNECTING) ||
                !cmsUtl_strcmp(ipConn->connectionStatus, MDMVS_CONNECTED))
            {
               cmsLog_debug("Deactivate gpon interface  %s and send link down message", ipConn->X_BROADCOM_COM_IfName);
               sendStatusMsgToSmd(CMS_MSG_WAN_LINK_DOWN, ipConn->X_BROADCOM_COM_IfName);
               updateSingleWanConnStatusLocked(&iidStack, (void *)ipConn, info->serviceParams.serviceStatus);
            }
         }
      }

      cmsObj_free((void **) &ipConn);
   }

   INIT_INSTANCE_ID_STACK(&iidStack);
   while (cmsObj_getNextInSubTreeFlags(MDMOID_WAN_PPP_CONN, parentIidStack, &iidStack, OGF_NO_VALUE_UPDATE, (void **) &pppConn) == CMSRET_SUCCESS)
   {
      /* take care of don't care pbits and vlanId filter */
      if (pppConn->X_BROADCOM_COM_VlanMux8021p == -1 && pppConn->X_BROADCOM_COM_VlanMuxID == -1)
         found = TRUE;
      else if (pbits != -1 && vlanId != -1)
         found = (pppConn->X_BROADCOM_COM_VlanMux8021p == pbits &&
	          pppConn->X_BROADCOM_COM_VlanMuxID == vlanId);
      else if (pbits == -1 && vlanId == -1)
         found = TRUE;
      else if (pbits == -1)
         found = (pppConn->X_BROADCOM_COM_VlanMuxID == vlanId);
      else if (vlanId == -1)
         found = (pppConn->X_BROADCOM_COM_VlanMux8021p == pbits);

      if (found == TRUE)
      {
         cmsLog_debug("WanPppConnObject, ifName=%s, strStatus=%s, status=%d, pbits=%d, vlanId=%d\n",
            pppConn->X_BROADCOM_COM_IfName, pppConn->connectionStatus, info->serviceParams.serviceStatus, pbits, vlanId);
         if (info->serviceParams.serviceStatus == TRUE)
         {
            if (!cmsUtl_strcmp(pppConn->connectionStatus, MDMVS_UNCONFIGURED) ||
                !cmsUtl_strcmp(pppConn->connectionStatus, MDMVS_DISCONNECTED))
            {
               cmsLog_debug("Activate  gpon interface %s and send link up message", pppConn->X_BROADCOM_COM_IfName);
               sendStatusMsgToSmd(CMS_MSG_WAN_LINK_UP, pppConn->X_BROADCOM_COM_IfName);
               updateSingleWanConnStatusLocked(&iidStack, (void *)pppConn, info->serviceParams.serviceStatus);
            }
         }
         else
         {
            if (!cmsUtl_strcmp(pppConn->connectionStatus, MDMVS_CONNECTING) ||
                !cmsUtl_strcmp(pppConn->connectionStatus, MDMVS_CONNECTED))
            {
               cmsLog_debug("Deactivate gpon interface  %s and send link down message", pppConn->X_BROADCOM_COM_IfName);
               sendStatusMsgToSmd(CMS_MSG_WAN_LINK_DOWN, pppConn->X_BROADCOM_COM_IfName);
               updateSingleWanConnStatusLocked(&iidStack, (void *)pppConn, info->serviceParams.serviceStatus);
            }
         }
      }

      cmsObj_free((void **) &pppConn);
   }

   ret = CMSRET_SUCCESS;

out:
   return ret;
}

static CmsRet updatePonIntfStatus(void)
{
   CmsRet ret = CMSRET_INTERNAL_ERROR;
   UBOOL8 changed = FALSE;
   UBOOL8 ponLinkUp = FALSE;
   WanGponLinkCfgObject *gponLinkCfg = NULL;
   WanPonIntfObject *ponObj=NULL;
   InstanceIdStack gponLinkIid = EMPTY_INSTANCE_ID_STACK;   
   InstanceIdStack gponWanIid = EMPTY_INSTANCE_ID_STACK;

   /* Get Gpon WanDevice iidStack first */      
   if ((ret = dalGpon_getGponWanIidStatck(&gponWanIid)) != CMSRET_SUCCESS)
   {
      goto out;
   }
         
   /* Update ponCfg object status if there is any changes in the ponObj->status
   *  If there is one gponLinkCfg linkStatus "UP", the ponObj->status is considered "UP"
   */
   while (!ponLinkUp && (ret = cmsObj_getNextInSubTreeFlags
      (MDMOID_WAN_GPON_LINK_CFG, &gponWanIid, &gponLinkIid, OGF_NO_VALUE_UPDATE, (void **)&gponLinkCfg)) == CMSRET_SUCCESS)
   {
      ponLinkUp = !cmsUtl_strcmp(gponLinkCfg->linkStatus, MDMVS_UP);
      cmsLog_debug("%s status %s", gponLinkCfg->ifName, gponLinkCfg->linkStatus);
      cmsObj_free((void **) &gponLinkCfg);
   }

   if ((ret = cmsObj_get(MDMOID_WAN_PON_INTF, &gponWanIid, 0, (void **)&ponObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get PonIntfObj. ret=%d", ret);
      goto out;
   }   

   changed = FALSE;
   if (ponLinkUp && cmsUtl_strcmp(ponObj->status, MDMVS_UP))
   {
      CMSMEM_REPLACE_STRING(ponObj->status, MDMVS_UP);
      changed = TRUE;
   }
   else if (!ponLinkUp && !cmsUtl_strcmp(ponObj->status, MDMVS_UP))
   {
      CMSMEM_REPLACE_STRING(ponObj->status, MDMVS_DISABLED);
      changed = TRUE;
   }

   if (changed)
   {
      ret = cmsObj_set(ponObj, &gponWanIid);
   }
   cmsObj_free((void **)&ponObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set ponObj. ret=%d", ret);
      goto out;
   }

   ret = CMSRET_SUCCESS;

out:
   return ret;
}

void processGponWanServiceStatusChange_igd(CmsMsgHeader *msg)
{
   CmsRet ret = CMSRET_SUCCESS;
   OmciServiceMsgBody *info = (OmciServiceMsgBody *) (msg + 1);
   InstanceIdStack gponLinkIid = EMPTY_INSTANCE_ID_STACK;   
   CmsMsgHeader replyMsg = EMPTY_MSG_HEADER;

   /* Need to change error to debug later on */
   cmsLog_debug("gemIndex=%d, gemPortId=%d, pbits=%d, vlanid=%d, serviceType=%d, serviceStatus=%d", 
                info->linkParams.gemPortIndex,  info->linkParams.portID, info->serviceParams.pbits, 
                info->serviceParams.vlanId, info->linkParams.serviceType, info->serviceParams.serviceStatus);

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d", ret);
      return;
   }

   if (info->linkParams.serviceType == OMCI_SERVICE_MULTICAST)
   {
      /* do nothing since multicast is taken care by
         hardware multicast accelerator through vlanctl */
      goto out;
   }

   if (info->serviceParams.serviceStatus == TRUE)
   {
      if ((ret = updateGponLinkCfg(info, &gponLinkIid)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Fail to update GponLinkCfgObject, ret=%d", ret);
         goto out;
      }

      updateGponServiceStatus(info, &gponLinkIid);

      updatePonIntfStatus();
   }
   else
   {
      WanGponLinkCfgObject *gponLinkCfg = NULL;
      UINT16 connNum;
      extern UINT16 rutWan_getNumberOfActiveConnections(const InstanceIdStack *parentIidStack);

      if (dalGpon_getGponLinkByIfName((char *)info->l2Ifname, &gponLinkIid, &gponLinkCfg) == FALSE)
      {
         cmsLog_error("Fail to get GponLink, ifname=%s", info->l2Ifname);
         goto out;
      }

      /* L3 */
      updateGponServiceStatus(info, &gponLinkIid);
      connNum = rutWan_getNumberOfActiveConnections(&gponLinkIid);
      /* L2 */
      if (connNum == 0)
      {
          INIT_INSTANCE_ID_STACK(&gponLinkIid);
          if ((ret = updateGponLinkCfg(info, &gponLinkIid)) != CMSRET_SUCCESS)
          {
             cmsLog_error("Fail to update GponLinkCfgObject, ret=%d", ret);
             goto out;
          }
          updatePonIntfStatus();
      }
   }

out:   
   /* need to response the msg to omcid after all is done */
   replyMsg.type = msg->type;
   replyMsg.src = EID_SSK;
   replyMsg.dst = msg->src;
   replyMsg.flags_request = 0;
   replyMsg.flags_response = 1;
   replyMsg.wordData = CMSRET_SUCCESS;

   if ((ret = cmsMsg_send(msgHandle, &replyMsg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("msg send failed, ret=%d",ret);
   }

   cmsLck_releaseLock();
}

#endif /* DMP_X_BROADCOM_COM_GPONWAN_1 */

#if defined(DMP_X_BROADCOM_COM_EPONWAN_1)
static CmsRet updateEponInterfaceCfg(const EponLinkStatusMsgBody *info, InstanceIdStack *pEponIntfIid, UBOOL8 *isChanged)
{
   CmsRet ret = CMSRET_SUCCESS;
   *isChanged = FALSE;

#ifdef EPON_SFU
   WanEponIntfObject *eponIntfCfg = NULL;

   /* Find the EPON interface config object for the given interface name */
   if (dalEpon_getEponIntfByIfName((char *)info->l2Ifname, pEponIntfIid, &eponIntfCfg) == FALSE)
   {
      return CMSRET_INTERNAL_ERROR;
   }

   /* epon0 has been created and up in kernel space. MDM_init will add this interface to data model
    * Any link change, eponapp will notify to ssk then update L2 data model then trigger L3 WAN service status change.
    * 1 meaning link UP; 2 meaning link DOWN which will be translated to NoLink in eponapp then send to ssk.
    */
   if (info->linkStatus == 1 &&
         cmsUtl_strcmp(eponIntfCfg->status, MDMVS_UP))
   {
      CMSMEM_REPLACE_STRING(eponIntfCfg->status, MDMVS_UP);
      *isChanged = TRUE;
   }
   else if (info->linkStatus == 2 &&
         cmsUtl_strcmp(eponIntfCfg->status, MDMVS_DOWN))
   {
      CMSMEM_REPLACE_STRING(eponIntfCfg->status, MDMVS_NOLINK);
      *isChanged = TRUE;
   }
   
   /* save the epon interface */
   if ((ret = cmsObj_set(eponIntfCfg, pEponIntfIid)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set WanEponIntfObject. ret=%d", ret);
   }   

   cmsObj_free((void **)&eponIntfCfg);
#else
   WanEponLinkCfgObject *eponLinkCfg = NULL;

   /* Find the EPON interface config object for the given interface name */
   if (dalEpon_getEponLinkByIfName((char *)info->l2Ifname, pEponIntfIid, &eponLinkCfg) == FALSE)
   {
      return CMSRET_INTERNAL_ERROR;
   }

   /* epon0 has been created and up in kernel space. MDM_init will add this interface to data model
    * Any link change, eponapp will notify to ssk then update L2 data model then trigger L3 WAN service status change.
    * 1 meaning link UP; 2 meaning link DOWN which will be translated to NoLink in eponapp then send to ssk.
    */
   if (info->linkStatus == 1 &&
         cmsUtl_strcmp(eponLinkCfg->linkStatus, MDMVS_UP))
   {
      CMSMEM_REPLACE_STRING(eponLinkCfg->linkStatus, MDMVS_UP);
      *isChanged = TRUE;
   }
   else if (info->linkStatus == 2 &&
         cmsUtl_strcmp(eponLinkCfg->linkStatus, MDMVS_DOWN))
   {
      CMSMEM_REPLACE_STRING(eponLinkCfg->linkStatus, MDMVS_DOWN);
      *isChanged = TRUE;
   }
   
   /* save the epon interface */
   if ((ret = cmsObj_set(eponLinkCfg, pEponIntfIid)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set WanEponLinkObject. ret=%d", ret);
   }   

   cmsObj_free((void **)&eponLinkCfg);
#endif
   cmsLog_debug("isChanged=%d", *isChanged);
   return ret;
}

static CmsRet updateEponServiceStatus(InstanceIdStack *parentIidStack, const EponLinkStatusMsgBody *info)
{
   CmsRet ret = CMSRET_SUCCESS;

#ifdef EPON_SFU
   WanDevObject *wanDev = NULL;

   if ((ret = cmsObj_getAncestorFlags(MDMOID_WAN_DEV, 
               MDMOID_WAN_EPON_INTF, parentIidStack, OGF_NO_VALUE_UPDATE, (void **)&wanDev)) != CMSRET_SUCCESS)

   {
      cmsLog_error("Fail to get cmsObj_getAncestor(MDMOID_WAN_CONN_DEVICE). ret=%d", ret);
      return ret;
   }
   cmsObj_free((void **) &wanDev);

   /* perform the action on IGD.WANDevice.{i}.X_BROADCOM_COM_EponInterfaceConfig. */
   updateWanConnStatusInSubtreeLocked(parentIidStack, (info->linkStatus == 1)?TRUE:FALSE);
#else
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   WanConnDeviceObject *wanConn = NULL;
   WanIpConnObject *ipConn=NULL;
   WanPppConnObject *pppConn=NULL;
   UBOOL8 found = FALSE;
   SINT32 vlanId = -1;

   if ((ret = cmsObj_getAncestorFlags(MDMOID_WAN_CONN_DEVICE, 
      MDMOID_WAN_EPON_LINK_CFG, parentIidStack, OGF_NO_VALUE_UPDATE, (void **)&wanConn)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Fail to get cmsObj_getAncestor(MDMOID_WAN_CONN_DEVICE). ret=%d", ret);
      return ret;
   }
   cmsObj_free((void **) &wanConn);

   vlanId = info->vlanId;

   while (cmsObj_getNextInSubTreeFlags(MDMOID_WAN_IP_CONN, parentIidStack, &iidStack, OGF_NO_VALUE_UPDATE, (void **) &ipConn) == CMSRET_SUCCESS)
   {
      /* take care of don't care vlanId filter */
      if (ipConn->X_BROADCOM_COM_VlanMuxID == -1)
         found = TRUE;
      else if (vlanId == -1)
         found = TRUE;
      else if (vlanId != -1)
         found = ipConn->X_BROADCOM_COM_VlanMuxID == vlanId;
      cmsLog_debug("WanIpConnObject, ifName=%s, strStatus=%s, status=%d",
         ipConn->X_BROADCOM_COM_IfName, ipConn->connectionStatus, info->linkStatus);
      if (found)
      {
         if (info->linkStatus == 1)
         {
            if (!cmsUtl_strcmp(ipConn->connectionStatus, MDMVS_UNCONFIGURED) ||
                !cmsUtl_strcmp(ipConn->connectionStatus, MDMVS_DISCONNECTED))
            {
               cmsLog_debug("Activate  epon interface %s and send link up message", ipConn->X_BROADCOM_COM_IfName);
               sendStatusMsgToSmd(CMS_MSG_WAN_LINK_UP, ipConn->X_BROADCOM_COM_IfName);
               updateSingleWanConnStatusLocked(&iidStack, (void *)ipConn, TRUE);
            }
         }
         else
         {
            if (!cmsUtl_strcmp(ipConn->connectionStatus, MDMVS_CONNECTING) ||
                !cmsUtl_strcmp(ipConn->connectionStatus, MDMVS_CONNECTED))
            {
               cmsLog_debug("Deactivate epon interface  %s and send link down message", ipConn->X_BROADCOM_COM_IfName);
               sendStatusMsgToSmd(CMS_MSG_WAN_LINK_DOWN, ipConn->X_BROADCOM_COM_IfName);
               updateSingleWanConnStatusLocked(&iidStack, (void *)ipConn, FALSE);
            }
         }
      }
      cmsObj_free((void **) &ipConn);
   }

   INIT_INSTANCE_ID_STACK(&iidStack);
   while (cmsObj_getNextInSubTreeFlags(MDMOID_WAN_PPP_CONN, parentIidStack, &iidStack, OGF_NO_VALUE_UPDATE, (void **) &pppConn) == CMSRET_SUCCESS)
   {
      /* take care of don't care vlanId filter */
      if (pppConn->X_BROADCOM_COM_VlanMuxID == -1)
         found = TRUE;
      else if (vlanId == -1)
         found = TRUE;
      else if (vlanId != -1)
         found = pppConn->X_BROADCOM_COM_VlanMuxID == vlanId;
      cmsLog_debug("WanPppConnObject, ifName=%s, strStatus=%s, status=%d",
         pppConn->X_BROADCOM_COM_IfName, pppConn->connectionStatus, info->linkStatus);
      if (found)
      {
         if (info->linkStatus == TRUE)
         {
            if (!cmsUtl_strcmp(pppConn->connectionStatus, MDMVS_UNCONFIGURED) ||
                !cmsUtl_strcmp(pppConn->connectionStatus, MDMVS_DISCONNECTED))
            {
               cmsLog_debug("Activate  epon interface %s and send link up message", pppConn->X_BROADCOM_COM_IfName);
               sendStatusMsgToSmd(CMS_MSG_WAN_LINK_UP, pppConn->X_BROADCOM_COM_IfName);
               updateSingleWanConnStatusLocked(&iidStack, (void *)pppConn, info->linkStatus);
            }
         }
         else
         {
            if (!cmsUtl_strcmp(pppConn->connectionStatus, MDMVS_CONNECTING) ||
                !cmsUtl_strcmp(pppConn->connectionStatus, MDMVS_CONNECTED))
            {
               cmsLog_debug("Deactivate epon interface  %s and send link down message", pppConn->X_BROADCOM_COM_IfName);
               sendStatusMsgToSmd(CMS_MSG_WAN_LINK_DOWN, pppConn->X_BROADCOM_COM_IfName);
               updateSingleWanConnStatusLocked(&iidStack, (void *)pppConn, info->linkStatus);
            }
         }
      }
      cmsObj_free((void **) &pppConn);
   }

#endif
   return CMSRET_SUCCESS;
}

void processEponWanLinkChange_igd(CmsMsgHeader *msg)
{
   CmsRet ret = CMSRET_SUCCESS;
   EponLinkStatusMsgBody *info = (EponLinkStatusMsgBody *) (msg + 1);
   InstanceIdStack eponIntfIid = EMPTY_INSTANCE_ID_STACK;
   CmsMsgHeader replyMsg = EMPTY_MSG_HEADER;
   UBOOL8 isChanged = FALSE;

   cmsLog_debug("l2IfName=%s, linkStatus=%d", info->l2Ifname, info->linkStatus);

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d", ret);
      cmsLck_dumpInfo();
      return;
   }

   if ((ret = updateEponInterfaceCfg(info, &eponIntfIid, &isChanged)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Fail to update WanEponIntfObject, ret=%d", ret);
      goto out;
   }

   cmsLog_debug("l2IfName=%s, linkStatus=%d, isChagned=%d", info->l2Ifname, info->linkStatus, isChanged);

   if (TRUE == isChanged)
   {
      // Only L2 is UP or Down (actually other status all can think is down) then trigger Layer 3 WAN services UP or Down
      ret = updateEponServiceStatus(&eponIntfIid, info);
   }
   //updatePonIntfStatus(); if EPON case need update InternetGatewayDevice.WANDevice.{i}.X_BROADCOM_COM_WANPonInterfaceConfig.status ?? 

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

#if defined(DMP_X_BROADCOM_COM_EPONWAN_1) || defined (DMP_X_BROADCOM_COM_GPONWAN_1)
void processWanLinkOpState(CmsMsgHeader *msg)
{
   CmsRet ret = CMSRET_SUCCESS;
   WanConfPhyOpStateMsgBody *info = (WanConfPhyOpStateMsgBody*)(msg + 1);
   CmsMsgHeader replyMsg = EMPTY_MSG_HEADER;

   cmsLog_debug("phyType=%d, opState=%d", info->phyType, info->opState);

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d", ret);
      cmsLck_dumpInfo();
      goto out;
   }

#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
   if (info->phyType == WANCONF_PHY_TYPE_GPON)
   {
       updateGponLinkOpState(info->opState);
   }
#endif /* DMP_X_BROADCOM_COM_GPONWAN_1 */

#ifdef DMP_X_BROADCOM_COM_EPONWAN_1
   if (info->phyType == WANCONF_PHY_TYPE_EPON)
   {
       updateEponLinkOpState(info->opState);
   }
#endif /* DMP_X_BROADCOM_COM_EPONWAN_1 */

   cmsLck_releaseLock();

out:   
   replyMsg.type = msg->type;
   replyMsg.src = EID_SSK;
   replyMsg.dst = msg->src;
   replyMsg.wordData = ret;
   replyMsg.flags_request = 0;
   replyMsg.flags_response = 1;

   if ((ret = cmsMsg_send(msgHandle, &replyMsg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("msg send failed, ret=%d", ret);
   }
}

#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
#ifndef DMP_DEVICE2_OPTICAL_1
static CmsRet updateGponLinkOpState(UBOOL8 opState)
{
   CmsRet ret = CMSRET_INTERNAL_ERROR;
   InstanceIdStack gponWanIid = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack gponLinkIid = EMPTY_INSTANCE_ID_STACK;
   WanGponLinkCfgObject *gponLinkCfg = NULL;
   UBOOL8 found = FALSE;

   if (dalGpon_getGponWanIidStatck(&gponWanIid) != CMSRET_SUCCESS)
   {
      cmsLog_error("dalGpon_getGponWanIidStatck() failed.");
      return ret;
   }

   while (found == FALSE &&
     cmsObj_getNextInSubTree(MDMOID_WAN_GPON_LINK_CFG, &gponWanIid,
     &gponLinkIid, (void**)&gponLinkCfg) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strstr(gponLinkCfg->ifName, GPON_IFC_STR) != NULL)
      {
         found = TRUE;
         gponLinkCfg->phyDetected = opState;
         ret = cmsObj_set(gponLinkCfg, &gponLinkIid);
         cmsObj_free((void **)&gponLinkCfg);
         if (ret == CMSRET_SUCCESS)
         {
             break;
         }
         else
         {
            cmsLog_error("Failed to set GponLinkCfgObject. ret=%d", ret);
         }
      }
      cmsObj_free((void **)&gponLinkCfg);
   }

   return ret;
}
#else
static CmsRet updateGponLinkOpState(UBOOL8 opState  __attribute__((unused)))
{
   CmsRet ret = CMSRET_INTERNAL_ERROR;
   return ret;
}
#endif
#endif /* DMP_X_BROADCOM_COM_GPONWAN_1 */


#ifdef DMP_X_BROADCOM_COM_EPONWAN_1
#ifndef DMP_DEVICE2_OPTICAL_1
static CmsRet updateEponLinkOpState(UBOOL8 opState)
{
   CmsRet ret = CMSRET_INTERNAL_ERROR;
   InstanceIdStack eponWanIid = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack eponLinkIid = EMPTY_INSTANCE_ID_STACK;
   WanEponLinkCfgObject *eponLinkCfg = NULL;
   UBOOL8 found = FALSE;

   if (dalEpon_getEponWanIidStatck(&eponWanIid) != CMSRET_SUCCESS)
   {
      cmsLog_error("dalEpon_getEponWanIidStatck() failed.");
      return ret;
   }

   while (found == FALSE &&
     cmsObj_getNextInSubTree(MDMOID_WAN_EPON_LINK_CFG, &eponWanIid,
     &eponLinkIid, (void**)&eponLinkCfg) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strstr(eponLinkCfg->ifName, EPON_IFC_STR) != NULL)
      {
         found = TRUE;
         eponLinkCfg->phyDetected = opState;
         ret = cmsObj_set(eponLinkCfg, &eponLinkIid);
         cmsObj_free((void **)&eponLinkCfg);
         if (ret == CMSRET_SUCCESS)
         {
             break;
         }
         else
         {
            cmsLog_error("Failed to set EponLinkCfgObject. ret=%d", ret);
         }
      }
      cmsObj_free((void **)&eponLinkCfg);
   }

   return ret;
}
#else
static CmsRet updateEponLinkOpState(UBOOL8 opState __attribute__((unused)))
{
   CmsRet ret = CMSRET_INTERNAL_ERROR;
   return ret;
}
#endif /* DMP_DEVICE2_OPTICAL_1 */
#endif /* DMP_X_BROADCOM_COM_EPONWAN_1 */

#endif /* defined(DMP_X_BROADCOM_COM_EPONWAN_1) || defined (DMP_X_BROADCOM_COM_GPONWAN_1) */
