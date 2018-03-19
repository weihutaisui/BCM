/***********************************************************************
 *
 *  Copyright (c) 2012  Broadcom Corporation
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
 
#ifdef DMP_DEVICE2_BASELINE_1

#ifdef DMP_DEVICE2_DSL_1

#include "cms.h"
#include "cms_mdm.h"
#include "cms_obj.h"
#include "cms_qdm.h"
#include "cms_util.h"
#include "AdslMibDef.h"
#include "devctl_adsl.h"


UBOOL8 qdmDsl_isAtmConnectionLocked(const char *lowerLayers)
{
   adslMibInfo adslMib;
   MdmPathDescriptor pathDesc;
   Dev2DslLineObject *dslLineObj;
   CmsRet ret = CMSRET_SUCCESS;
   long adslMibSize=sizeof(adslMib);
   UBOOL8 isAtm=TRUE;  /* assume an ATM connection by default */

   /* first get the line object underneath this channel */
   INIT_PATH_DESCRIPTOR(&pathDesc);
   cmsMdm_fullPathToPathDescriptor(lowerLayers, &pathDesc);
   if ((ret = cmsObj_get(MDMOID_DEV2_DSL_LINE,&pathDesc.iidStack,0,(void **)&dslLineObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Cannot get line object for this channel to determine encapsulation type, ret=%d", ret);
      return FALSE;
   }
   if (CMSRET_SUCCESS != xdslCtl_GetObjectValue(dslLineObj->X_BROADCOM_COM_BondingLineNumber, NULL, 0, (char *) &adslMib, &adslMibSize))
   {
      cmsLog_error("could not get MIB for line %d", dslLineObj->X_BROADCOM_COM_BondingLineNumber);
      cmsObj_free((void **) &dslLineObj);
      return FALSE;
   }
   cmsObj_free((void **) &dslLineObj);

   /* the following logic is copied from rut_dsl.c to figure out if the channel is trained for ATM or PTM mode */
#if defined(DMP_VDSL2WAN_1) || defined(DMP_X_BROADCOM_COM_VDSL2WAN_1)
   UBOOL8 isVdsl=(adslMib.adslConnection.modType == kVdslModVdsl2);
   cmsLog_debug("isVdsl=%d", isVdsl);
   cmsLog_debug("MIB rcv2Info %d %d", adslMib.vdslInfo[0].rcv2Info.tmType[0], adslMib.vdslInfo[0].rcv2Info.tmType[1]);
   cmsLog_debug("MIB xmt2Info %d %d", adslMib.vdslInfo[0].xmt2Info.tmType[0], adslMib.vdslInfo[0].xmt2Info.tmType[1]);

   /* a 1 in tmType[0] means DPAPI_DATA_ATM */
   /* a 4 in tmType[0] means DPAPI_DATA_NITRO - ATM with header compression, possible with Broadcom CO */
   if (isVdsl)
   {
      if ((adslMib.vdslInfo[0].xmt2Info.tmType[0] != 1) &&
          (adslMib.vdslInfo[0].xmt2Info.tmType[0] != 4))
      {
         /* this is a PTM connection */
         isAtm = FALSE;
      }
   }
   else
#endif /* defined(DMP_VDSL2WAN_1) || defined(DMP_X_BROADCOM_COM_VDSL2WAN_1) */
   {
      UINT8 connType=adslMib.adsl2Info2lp[0].xmtChanInfo.connectionType;

      cmsLog_debug("adsl connType=%d", connType);
      if ((connType != 1) && (connType != 4))
      {
         /* this is a PTM connection */
         isAtm = FALSE;
      }
   }
   cmsLog_debug("isAtm=%d", isAtm);
   return isAtm;
}

/* given a dsl line full path, find the active channel above it */
UBOOL8 qdmDsl_getChannelFullPathFromLineFullPathLocked(const char *lineFullPath, char **channelFullPath)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2DslChannelObject *dslChannelObj = NULL;
   MdmPathDescriptor channelPathDesc;
   UBOOL8 found = FALSE;

   while ((!found) &&
          (cmsObj_getNext(MDMOID_DEV2_DSL_CHANNEL, &iidStack, (void **) &dslChannelObj) == CMSRET_SUCCESS))
   {
      if (!cmsUtl_strcmp(dslChannelObj->lowerLayers, lineFullPath))
      {
         if ((dslChannelObj->enable) &&
             (!cmsUtl_strcmp(dslChannelObj->status, MDMVS_UP)))
         {
            found= TRUE;
            INIT_PATH_DESCRIPTOR(&channelPathDesc);
            channelPathDesc.oid = MDMOID_DEV2_DSL_CHANNEL;
            channelPathDesc.iidStack = iidStack;
            cmsMdm_pathDescriptorToFullPathNoEndDot(&channelPathDesc,channelFullPath);
         }
      }
      cmsObj_free((void **) &dslChannelObj);
   }
   return (found);
}

void qdmDsl_getPath1LineRateLocked_dev2(int *lineRate)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2DslChannelObject *dslChannelObj = NULL;
   UBOOL8 found = FALSE;
   int rate = 0;

   while ((!found) &&
          (cmsObj_getNext(MDMOID_DEV2_DSL_CHANNEL, &iidStack, (void **) &dslChannelObj) == CMSRET_SUCCESS))
   {
      if ((dslChannelObj->enable) &&
          (!cmsUtl_strcmp(dslChannelObj->status, MDMVS_UP)))
      {
         /* Path/Bearer 1: Up Stream Rate */
         rate = dslChannelObj->X_BROADCOM_COM_UpstreamCurrRate_2;
         found = TRUE;
      }
      cmsObj_free((void **) &dslChannelObj);
   }
   *lineRate = rate;
}

/* isVdsl is TRUE if VDSL, else ADSL of some sort. isAtm is TRUE if ATM mode, else PTM mode */
void qdmDsl_getDSLTrainedModeLocked_dev2(UBOOL8 *isVdsl, UBOOL8 *isAtm)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE;
   Dev2DslLineObject *dslLineObj;
   *isVdsl = FALSE;
   *isAtm = TRUE;
   adslMibInfo adslMib;
   long adslMibSize=sizeof(adslMib);

   /* first check to see if the Line is trained for VDSL */
   while (!found &&
          cmsObj_getNext(MDMOID_DEV2_DSL_LINE, &iidStack, (void **)&dslLineObj) == CMSRET_SUCCESS)

   {
      if (dslLineObj->enable && !cmsUtl_strcmp(dslLineObj->status, MDMVS_UP))
      {
         found = TRUE;
         if (!cmsUtl_strncmp(dslLineObj->standardUsed, "G.993.2",strlen("G.993.2")))
         {
            *isVdsl = TRUE;
         }
         break;
      }
      cmsObj_free((void **) &dslLineObj);
   }
   if (found)
   {
      if (CMSRET_SUCCESS != xdslCtl_GetObjectValue(dslLineObj->X_BROADCOM_COM_BondingLineNumber, NULL, 0, (char *) &adslMib, &adslMibSize))
      {
         cmsLog_error("could not get MIB for line %d", dslLineObj->X_BROADCOM_COM_BondingLineNumber);
      }
      else
      {

         /* the following logic is copied from rut_dsl.c to figure out if the channel is trained for ATM or PTM mode */
#if defined(DMP_VDSL2WAN_1) || defined(DMP_X_BROADCOM_COM_VDSL2WAN_1)
         cmsLog_debug("MIB rcv2Info %d %d", adslMib.vdslInfo[0].rcv2Info.tmType[0], adslMib.vdslInfo[0].rcv2Info.tmType[1]);
         cmsLog_debug("MIB xmt2Info %d %d", adslMib.vdslInfo[0].xmt2Info.tmType[0], adslMib.vdslInfo[0].xmt2Info.tmType[1]);
         
         /* a 1 in tmType[0] means DPAPI_DATA_ATM */
         /* a 4 in tmType[0] means DPAPI_DATA_NITRO - ATM with header compression, possible with Broadcom CO */
         if (*isVdsl)
         {
            if ((adslMib.vdslInfo[0].xmt2Info.tmType[0] != 1) &&
                (adslMib.vdslInfo[0].xmt2Info.tmType[0] != 4))
            {
               /* this is a PTM connection */
               *isAtm = FALSE;
            }
         }
         else
#endif /* defined(DMP_VDSL2WAN_1) || defined(DMP_X_BROADCOM_COM_VDSL2WAN_1) */
         {
            UINT8 connType=adslMib.adsl2Info2lp[0].xmtChanInfo.connectionType;

            cmsLog_debug("adsl connType=%d", connType);
            if ((connType != 1) && (connType != 4))
            {
               /* this is a PTM connection */
               *isAtm = FALSE;
            }
         }            
      } /* get adslMib ok */
      cmsObj_free((void **) &dslLineObj);
   } /*found */   
   cmsLog_debug("isAtm=%d, isVdsl=%d", *isAtm,*isVdsl);
}

UBOOL8 qdmDsl_isXdslLinkUpLocked_dev2()
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2DslLineObject *dslLineObj;

   while (cmsObj_getNext(MDMOID_DEV2_DSL_LINE, &iidStack, (void **)&dslLineObj) == CMSRET_SUCCESS)
   {
      if (dslLineObj->enable && !cmsUtl_strcmp(dslLineObj->status, MDMVS_UP))
      {
         cmsObj_free((void **) &dslLineObj);
         return TRUE;
      }
      cmsObj_free((void **) &dslLineObj);
   }
   return FALSE;
}

#ifdef DMP_DEVICE2_BONDEDDSL_1
/* Returns Admin status enabled/not enabled */
void qdmDsl_isDslBondingEnabled_dev2(UBOOL8 *enabled)
{
   UBOOL8 found = FALSE;
   Dev2DslBondingGroupObject *dslBondingGroupObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   while (!found && cmsObj_getNext(MDMOID_DEV2_DSL_BONDING_GROUP, &iidStack, (void **) &dslBondingGroupObj) == CMSRET_SUCCESS)
   {
      if (dslBondingGroupObj->enable == TRUE) 
      {
         found = TRUE;
      }
      cmsObj_free((void **) &dslBondingGroupObj);      
   }
   if (found)
   {
      *enabled = TRUE;
   }
   else
   {
      *enabled = FALSE;
   }
}

/* The bonding scheme of the group: ATM or Ethernet. 
 * Operational in this routine means the bonding group is active even though it may be down now.
 *     1. if bonding is enabled, bonding status is UP but bonding group status is not NONE, this is not used for lowerLayer
 *     2. if (bonding is enabled, bonding status is UP, bonding group status is NONE) OR
 *           (bonding is enabled, bonding status is DOWN, this is used for lowerLayer since DSL Channel is down too)
 */
void qdmDsl_isDslBondingGroupStatusOperational_dev2(char *scheme,UBOOL8 *operational)
{
   UBOOL8 found = FALSE;
   Dev2DslBondingGroupObject *dslBondingGroupObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;


   while (!found && cmsObj_getNext(MDMOID_DEV2_DSL_BONDING_GROUP, &iidStack, (void **) &dslBondingGroupObj) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(dslBondingGroupObj->bondScheme, scheme))
      {
         found = TRUE;
         if (dslBondingGroupObj->enable)
         {
            if (((!cmsUtl_strcmp(dslBondingGroupObj->groupStatus, MDMVS_NONE)) &&
                 (!cmsUtl_strcmp(dslBondingGroupObj->status, MDMVS_UP))) ||
                (!cmsUtl_strcmp(dslBondingGroupObj->status, MDMVS_LOWERLAYERDOWN)))
            {
               *operational = TRUE;
            }
            else
            {
               *operational = FALSE;
            }
         }
         else
         {
            *operational = FALSE;
         }
      }
      cmsObj_free((void **) &dslBondingGroupObj);      
   }
}
#endif /* DMP_DEVICE2_BONDEDDSL_1 */

/* given a dsl line full path, return the ATM channel above it */
UBOOL8 qdmDsl_getATMChannelFullPathFromLineFullPathLocked(const char *lineFullPath, char **channelFullPath)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2DslChannelObject *dslChannelObj = NULL;
   MdmPathDescriptor channelPathDesc;
   UBOOL8 found = FALSE;

   while ((!found) &&
          (cmsObj_getNext(MDMOID_DEV2_DSL_CHANNEL, &iidStack, (void **) &dslChannelObj) == CMSRET_SUCCESS))
   {
      if (!cmsUtl_strcmp(dslChannelObj->lowerLayers, lineFullPath))
      {
         if ((dslChannelObj->enable) &&
             (!cmsUtl_strcmp(dslChannelObj->status, MDMVS_UP)))
         {
            found= TRUE;
            INIT_PATH_DESCRIPTOR(&channelPathDesc);
            channelPathDesc.oid = MDMOID_DEV2_DSL_CHANNEL;
            channelPathDesc.iidStack = iidStack;
            cmsMdm_pathDescriptorToFullPathNoEndDot(&channelPathDesc,channelFullPath);
         }
      }
      cmsObj_free((void **) &dslChannelObj);
   }
   return (found);
}

/* given a dsl line full path, return the PTM channel above it */
UBOOL8 qdmDsl_getPTMChannelFullPathFromLineFullPathLocked(const char *lineFullPath, char **channelFullPath)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2DslChannelObject *dslChannelObj = NULL;
   MdmPathDescriptor channelPathDesc;
   UBOOL8 found = FALSE;

   while ((!found) &&
          (cmsObj_getNext(MDMOID_DEV2_DSL_CHANNEL, &iidStack, (void **) &dslChannelObj) == CMSRET_SUCCESS))
   {
      if (!cmsUtl_strcmp(dslChannelObj->lowerLayers, lineFullPath))
      {
         if ((dslChannelObj->enable) &&
             (!cmsUtl_strcmp(dslChannelObj->status, MDMVS_UP)))
         {
            found= TRUE;
            INIT_PATH_DESCRIPTOR(&channelPathDesc);
            channelPathDesc.oid = MDMOID_DEV2_DSL_CHANNEL;
            channelPathDesc.iidStack = iidStack;
            cmsMdm_pathDescriptorToFullPathNoEndDot(&channelPathDesc,channelFullPath);
         }
      }
      cmsObj_free((void **) &dslChannelObj);
   }
   return (found);
}

/* lowerLayers is a CSL of lower layer.
 * This routine will look at the current status and not read from driver(???)
 */
UBOOL8 qdmDsl_isAnyLowerLayerChannelUpLocked_dev2(char *lowerLayers)
{
   char *pLowerLayers;
   char *pNext=NULL;
   char *pToken;
   Dev2DslChannelObject *dslChannelObj;
   int found = FALSE;
   MdmPathDescriptor lowerLayerPathDesc;
   
   pLowerLayers = cmsMem_strdup(lowerLayers);
   pToken = strtok_r(pLowerLayers,",",&pNext);
   while (pToken && !found)
   {
      INIT_PATH_DESCRIPTOR(&lowerLayerPathDesc);
      if (cmsMdm_fullPathToPathDescriptor(pToken, &lowerLayerPathDesc) == CMSRET_SUCCESS)
      {
#if 0
         if (cmsObj_get(lowerLayerPathDesc.oid,&lowerLayerPathDesc.iidStack,
                        OGF_NO_VALUE_UPDATE,(void **)&dslChannelObj) == CMSRET_SUCCESS)
#else
         if (cmsObj_get(lowerLayerPathDesc.oid,&lowerLayerPathDesc.iidStack,
                        0,(void **)&dslChannelObj) == CMSRET_SUCCESS)
#endif
         {
            if (!cmsUtl_strcmp(dslChannelObj->status,MDMVS_UP))
            {
               found = TRUE;
            }
            else
            {
               pToken = strtok_r(NULL,",",&pNext);
            }
            cmsObj_free((void **) &dslChannelObj);
         }
      } 
      else
      {
         /* something is wrong */
         break;
      }
   } /* while */
   CMSMEM_FREE_BUF_AND_NULL_PTR(pLowerLayers);
   return (found);
}

void qdmDsl_getModulationTypeStrLocked(int lineNumber, char *modeStr)
{
   adslMibInfo adslMib;
   long adslMibSize=sizeof(adslMib);

   if (CMSRET_SUCCESS != xdslCtl_GetObjectValue(lineNumber, NULL, 0, (char *) &adslMib, &adslMibSize))
   {
      cmsLog_error("could not get MIB for line %d", lineNumber);
   }
   else
   {
      switch (adslMib.adslConnection.modType)
      {
      case kAdslModGdmt:
         strcpy(modeStr,"ADSL_G.dmt");
         break;
      case kAdslModT1413:
         strcpy(modeStr,"ADSL_ANSI_T1.413");
         break;
      case kAdslModGlite:
         strcpy(modeStr,"ADSL_G.lite");
         break;
      case kAdslModAdsl2:
         strcpy(modeStr,"ADSL_G.dmt.bis");
         break;
      case kAdslModAdsl2p:
         strcpy(modeStr,"ADSL_2plus");
         break;
      case kAdslModReAdsl2:
         strcpy(modeStr,"ADSL_re-adsl");
         break;
      case kVdslModVdsl2:
         strcpy(modeStr,"VDSL2");
         break;
#ifdef DMP_DEVICE2_FAST_1
      case kXdslModGfast:
         strcpy(modeStr,"G.fast");
         break;
#endif
      default:
         strcpy(modeStr,"unknown");
         break;
      } /* modulationType */
   }
}

CmsRet qdmDsl_getLineIdFromChannelFullPathLocked_dev2(const char *fullPath,
                                                      UINT32 *lineId)
{
   MdmPathDescriptor pathDesc;
   CmsRet ret = CMSRET_SUCCESS;
   Dev2DslChannelObject *dslChannelObj = NULL;
   Dev2DslLineObject *dslLineObj = NULL;

   *lineId = 0;

   INIT_PATH_DESCRIPTOR(&pathDesc);
   ret = cmsMdm_fullPathToPathDescriptor(fullPath, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s, ret=%d", fullPath, ret);
      return ret;
   }
   if ((cmsObj_get(pathDesc.oid, &(pathDesc.iidStack), 0, (void **) &dslChannelObj)) == CMSRET_SUCCESS)
   {
      /* the lower layer is the dsl line full path */
      INIT_PATH_DESCRIPTOR(&pathDesc);
      ret = cmsMdm_fullPathToPathDescriptor(fullPath, &pathDesc);
      if (ret == CMSRET_SUCCESS)
      {
         if ((cmsObj_get(pathDesc.oid, &(pathDesc.iidStack), 0, (void **) &dslLineObj)) == CMSRET_SUCCESS)
         {
            *lineId = dslLineObj->X_BROADCOM_COM_BondingLineNumber;
            cmsObj_free((void **) &dslLineObj);
         }
      }
      cmsObj_free((void **) &dslChannelObj);
   }
   return (ret);
}

#ifdef DMP_DEVICE2_FAST_1
/* Given a fast line full path, find the ptm channel above it.
 * In FAST data model, there is no concept of channel; there are a lot of shared parameters
 * in in FAST and DSL data model.   We need to get the channel object and get those parameters.
 * The channel won't be up.
 */
UBOOL8 qdmDsl_getChannelFullPathFromFastLineFullPathLocked(const char *lineFullPath, char **channelFullPath)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2DslChannelObject *dslChannelObj = NULL;
   MdmPathDescriptor channelPathDesc;
   UBOOL8 found = FALSE;

   while ((!found) &&
          (cmsObj_getNext(MDMOID_DEV2_DSL_CHANNEL, &iidStack, (void **) &dslChannelObj) == CMSRET_SUCCESS))
   {
      if (!cmsUtl_strcmp(dslChannelObj->lowerLayers, lineFullPath))
      {
         if (!cmsUtl_strcmp(dslChannelObj->linkEncapsulationUsed, MDMVS_G_993_2_ANNEX_K_PTM))
         {
            found= TRUE;
            INIT_PATH_DESCRIPTOR(&channelPathDesc);
            channelPathDesc.oid = MDMOID_DEV2_DSL_CHANNEL;
            channelPathDesc.iidStack = iidStack;
            cmsMdm_pathDescriptorToFullPathNoEndDot(&channelPathDesc,channelFullPath);
         }
      }
      cmsObj_free((void **) &dslChannelObj);
   }
   return (found);
}
#endif /* DMP_DEVICE2_FAST_1 */

#endif /* DMP_DEVICE2_DSL_1 */

#endif /* DMP_DEVICE2_BASELINE_1 */

