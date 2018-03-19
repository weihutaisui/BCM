/***********************************************************************
 *
 *  Copyright (c) 2007-2010  Broadcom Corporation
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
#include "cms_obj.h"
#include "cms_dal.h"

#ifdef DMP_X_ITU_ORG_GPON_1
CmsRet dalPMirror_configGponPortMirrors(char *lst);
#endif

CmsRet dalPMirror_configPortMirrors(char *lst)
{
    char *pToken = NULL, *pLast = NULL;
    char *cp1, *cp2, *cp3;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    WanDebugPortMirroringCfgObject *obj = NULL;
    CmsRet ret = CMSRET_SUCCESS;

#ifdef DMP_X_ITU_ORG_GPON_1
    return dalPMirror_configGponPortMirrors(lst);
#endif

    INIT_INSTANCE_ID_STACK(&iidStack);
    /* Remove all current port mirroring entries. */
    while( cmsObj_getNext(MDMOID_WAN_DEBUG_PORT_MIRRORING_CFG, &iidStack,
        (void **) &obj) == CMSRET_SUCCESS )
    {
        cmsObj_deleteInstance(MDMOID_WAN_DEBUG_PORT_MIRRORING_CFG, &iidStack);
        INIT_INSTANCE_ID_STACK(&iidStack);
        cmsObj_free((void **)&obj);
    }

    /* Add new port mirroring entries in lst.  lst has following format:
     * monitor_interface|direction(1[out],0[in])|mirror_interface|
     * status(1[enabled],0[disabled])
     */
    pToken = strtok_r(lst, ", ", &pLast);
    while ( pToken != NULL )
    {
        cp1 = cp2 = cp3 = NULL;
        if( (cp1 = strstr(pToken, "|")) != NULL )
            if( (cp2 = strstr(cp1 + 1, "|")) != NULL )
                cp3 = strstr(cp2 + 1, "|");
            
        if( cp1 && cp2 && cp3 )
        {
            *cp1 = *cp2 = *cp3 = '\0';

            INIT_INSTANCE_ID_STACK(&iidStack);
            if( (ret = cmsObj_addInstance(MDMOID_WAN_DEBUG_PORT_MIRRORING_CFG,
                &iidStack)) == CMSRET_SUCCESS)
            {
                /* Get the instance of object in the newly created subtree,
                 * InternetGatewayDevice.X_BROADCOM_COM_DebugPortMirroringCfg.
                 * {i}
                 */
                if( (ret = cmsObj_get(MDMOID_WAN_DEBUG_PORT_MIRRORING_CFG,
                    &iidStack, 0, (void **) &obj)) == CMSRET_SUCCESS )
                {
                    /* Fill the object. */
                    CMSMEM_REPLACE_STRING(obj->monitorInterface, &pToken[0]);
                    CMSMEM_REPLACE_STRING(obj->mirrorInterface, &cp2[1]);
                    obj->direction = (!cmsUtl_strcmp(&cp1[1], "IN"))
                        ? PMIRROR_DIR_IN : PMIRROR_DIR_OUT;
                    obj->status = (cp3[1] == '1')
                        ? PMIRROR_ENABLED : PMIRROR_DISABLED;

                    cmsLog_debug("Adding new port mirroring entry: monitor "
                        "intf=%s, mirror intf=%s, direction=%d, status=%d",
                        obj->monitorInterface, obj->mirrorInterface,
                        obj->direction, obj->status);

                    /* Set X_BROADCOM_COM_DebugPortMirroringCfg. */
                    ret = cmsObj_set(obj, &iidStack);

                    /* Free X_BROADCOM_COM_DebugPortMirroringCfg object. */
                    cmsObj_free((void **) &obj);

                    if( ret != CMSRET_SUCCESS )
                    {
                        CmsRet r2;
                        cmsLog_error("Failed to set X_BROADCOM_COM_DebugPort"
                            "MirroringCfg, ret = %d", ret);
           
                        r2 = cmsObj_deleteInstance(
                            MDMOID_WAN_DEBUG_PORT_MIRRORING_CFG, &iidStack);
                        if( r2 != CMSRET_SUCCESS )
                            cmsLog_error("failed to delete created X_BROADCOM_"
                                "COM_DebugPortMirroringCfg, r2=%d", r2);

                        cmsLog_debug("Failed to set created X_BROADCOM_COM_"
                            "DebugPortMirroringCfg");
                    }
                }
                else
                {
                    cmsLog_error("Failed to get WanDebugPortMirroringCfgObject, "
                        "ret=%d", ret);
                    break;
                }       
            }
            else
            {
                cmsLog_error("could not create new port mirroring, ret=%d",ret);
                break;
            }
        }

        /* Get next port mirror config object information. */
        pToken = strtok_r(NULL, ", ", &pLast);
    }

    return (ret);
}


#ifdef SUPPORT_DSL
void dalPMirror_fillPMirrorEntry(const char *l2IfName, char *pMirrorList, SINT32 pMListSize)
{
   
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   
   /* Entry format: monitor_interface|direction(IN, OUT)|
    * mirror_interface|status(1[enabled],0[disabled])
    */
   if (l2IfName && pMirrorList)
   {
      /* See if the connection has a port mirror entry. */
      char tempStr2[BUFLEN_64];
      WanDebugPortMirroringCfgObject *pmObj = NULL;

      tempStr2[0] = '\0';
      INIT_INSTANCE_ID_STACK(&iidStack);
      while( cmsObj_getNextFlags(MDMOID_WAN_DEBUG_PORT_MIRRORING_CFG,
           &iidStack, OGF_NO_VALUE_UPDATE, (void **) &pmObj) == CMSRET_SUCCESS )
      {
         if (!cmsUtl_strcmp(pmObj->monitorInterface, l2IfName) &&
             pmObj->direction == PMIRROR_DIR_IN )
         {
            /* Found port mirror for connection 'in' direction. */
            snprintf(tempStr2, sizeof(tempStr2), "/IN/%s/%c|",
               pmObj->mirrorInterface,
               (pmObj->status == PMIRROR_ENABLED) ? '1' : '0');
            strncat(pMirrorList, l2IfName, pMListSize-1);
            strncat(pMirrorList, tempStr2, pMListSize-1);
            cmsObj_free((void **)&pmObj);
            break;
         }
         cmsObj_free((void **)&pmObj);
      }

      if( tempStr2[0] == '\0' )
      {
         /* Did not find port mirror for connection 'in' direction.
          * Make up default values.
          */
         strncat(pMirrorList, l2IfName, pMListSize-1);
         strncat(pMirrorList, "/IN/ /0|", pMListSize-1);
      }

      tempStr2[0] = '\0';
      INIT_INSTANCE_ID_STACK(&iidStack);
      while( cmsObj_getNextFlags(MDMOID_WAN_DEBUG_PORT_MIRRORING_CFG,
           &iidStack, OGF_NO_VALUE_UPDATE, (void **) &pmObj) == CMSRET_SUCCESS )
      {
         if (!cmsUtl_strcmp(pmObj->monitorInterface, l2IfName) &&
             pmObj->direction == PMIRROR_DIR_OUT )
         {
            /* Found port mirror for connection 'out' direction. */
            snprintf(tempStr2, sizeof(tempStr2), "/OUT/%s/%c|",
               pmObj->mirrorInterface,
               (pmObj->status == PMIRROR_ENABLED) ? '1' : '0');
            strncat(pMirrorList, l2IfName, pMListSize-1);
            strncat(pMirrorList, tempStr2, pMListSize-1);
            cmsObj_free((void **)&pmObj);
            break;
         }
         cmsObj_free((void **)&pmObj);
      }

      if( tempStr2[0] == '\0' )
      {
         /* Did not find port mirror for connection 'out' direction.
          * Make up default values.
          */
         strncat(pMirrorList, l2IfName, pMListSize-1);
         strncat(pMirrorList, "/OUT/ /0|", pMListSize-1);
      }
   }

}
#endif

#if defined(DMP_BASELINE_1)
void dalPMirror_getPMirrorList_igd(char *lst)
{
   CmsRet ret;
   WanDevObject *wanDev=NULL;
   WanCommonIntfCfgObject *comIntf = NULL;
   InstanceIdStack wanDevIid=EMPTY_INSTANCE_ID_STACK;
   WanDslIntfCfgObject *dslIntf=NULL;
   char pMirrorList[BUFLEN_1024]={0};
   SINT32 lstLen=0;
   
#ifdef DMP_X_BROADCOM_COM_ATMWAN_1
   UBOOL8 isATM;
#endif
#ifdef DMP_PTMWAN_1
   UBOOL8 isPTM;
#endif

   while ((ret = cmsObj_getNextFlags(MDMOID_WAN_DEV, &wanDevIid, OGF_NO_VALUE_UPDATE, (void **)&wanDev)) == CMSRET_SUCCESS)
   {

      cmsObj_free((void **)&wanDev);  /* no longer needed */

      if ((ret = cmsObj_get(MDMOID_WAN_COMMON_INTF_CFG, &wanDevIid, OGF_NO_VALUE_UPDATE, (void **)&comIntf)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsObj_getNext <MDMOID_WAN_COMMON_INTF_CFG> returns error. ret=%d", ret);
         return;
      }

      if (!cmsUtl_strcmp(comIntf->WANAccessType, MDMVS_DSL))
      {
         if ((ret = cmsObj_get(MDMOID_WAN_DSL_INTF_CFG, &wanDevIid, OGF_NO_VALUE_UPDATE, (void **)&dslIntf)) != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_getNextInSubTree  <MDMOID_WAN_DSL_INTF_CFG> error. ret=%d", ret);
            cmsObj_free((void **) &comIntf);
            return;
         }
      }
      else
      {
         cmsLog_debug("Not a DSL WandDev.");
      }
      
#ifdef DMP_X_BROADCOM_COM_ATMWAN_1
      isATM = (!cmsUtl_strcmp(comIntf->WANAccessType, MDMVS_DSL) &&
               !cmsUtl_strcmp(dslIntf->linkEncapsulationUsed, MDMVS_G_992_3_ANNEX_K_ATM));
#endif

#ifdef DMP_PTMWAN_1
      isPTM = (!cmsUtl_strcmp(comIntf->WANAccessType, MDMVS_DSL) &&
               !cmsUtl_strcmp(dslIntf->linkEncapsulationUsed, MDMVS_G_993_2_ANNEX_K_PTM));
#endif

      cmsObj_free((void **)&dslIntf); // If already NULL, will return gracefully
      cmsObj_free((void **)&comIntf);

#ifdef DMP_X_BROADCOM_COM_ATMWAN_1
      if (isATM)
      {
         WanDslLinkCfgObject *dslLink=NULL;
         InstanceIdStack conDevIid=EMPTY_INSTANCE_ID_STACK;   
         WanConnDeviceObject *wanCon=NULL;

         while ((ret = cmsObj_getNextInSubTreeFlags(MDMOID_WAN_CONN_DEVICE, &wanDevIid, &conDevIid, OGF_NO_VALUE_UPDATE,
                                               (void **)&wanCon)) == CMSRET_SUCCESS)
         {
            if ((ret = cmsObj_get(MDMOID_WAN_DSL_LINK_CFG, &conDevIid, OGF_NO_VALUE_UPDATE, (void **)&dslLink)) != CMSRET_SUCCESS)
            {
               cmsLog_error("cmsObj_get<MDMOID_DSL_LINK_CFG> returns error, ret=%d", ret);
               cmsObj_free((void **)&wanCon);
               return;
            }

            /* Found an ATM interface and try to found it match in the port mirror list */
            dalPMirror_fillPMirrorEntry(dslLink->X_BROADCOM_COM_IfName, pMirrorList, sizeof(pMirrorList));
            
            cmsObj_free((void **)&wanCon);
            cmsObj_free((void **)&dslLink);
         }
      }
#endif /* DMP_X_BROADCOM_COM_ATMWAN_1 */

#ifdef DMP_PTMWAN_1
      if (isPTM)
      {
         WanPtmLinkCfgObject *ptmLink = NULL;
         InstanceIdStack conDevIid=EMPTY_INSTANCE_ID_STACK;   
         WanConnDeviceObject *wanCon=NULL;

         while ((ret = cmsObj_getNextInSubTreeFlags(MDMOID_WAN_CONN_DEVICE, &wanDevIid, &conDevIid, OGF_NO_VALUE_UPDATE,
                                               (void **)&wanCon)) == CMSRET_SUCCESS)
         {
            if ((ret = cmsObj_get(MDMOID_WAN_PTM_LINK_CFG, &conDevIid, OGF_NO_VALUE_UPDATE, (void **)&ptmLink)) != CMSRET_SUCCESS)
            {
               cmsLog_error("cmsObj_get<MDMOID_PTM_LINK_CFG> returns error, ret=%d", ret);
               cmsObj_free((void **)&wanCon);
               return;
            }

            /* Found a PTM interface and try to found it match in the port mirror list */
            dalPMirror_fillPMirrorEntry(ptmLink->X_BROADCOM_COM_IfName, pMirrorList, sizeof(pMirrorList));            
            cmsObj_free((void **)&wanCon);
            cmsObj_free((void **)&ptmLink);
         }            
      }
#endif /* DMP_PTMWAN_1 */

   }

   if (pMirrorList[0] != '\0')
   {
      strcpy(lst, pMirrorList);
   }  

   lstLen = strlen(lst);
   
   if (lstLen > 0)
   {
      lst[lstLen-1] = '\0';   /* remove the last '|' character   */
   }
   
   cmsLog_debug("Port Mirror list: %s", lst);
   
}
#endif


#ifdef DMP_X_ITU_ORG_GPON_1
void dalPMirror_getGponPMirrorList(char *lst)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   char pMirrorList[BUFLEN_1024]={0};
   UINT32 maskSize;
   UINT8 *bin = NULL;
   UINT32 binSize;
   
   maskSize = CONFIG_BCM_MAX_GEM_PORTS / 8;
   if (CONFIG_BCM_MAX_GEM_PORTS % 8) 
      maskSize += 1;
   /* Entry format: gemPortMask-gemPortMask-...-gemPortMask|
    * direction(IN, OUT)| mirror_interface|status(1[enabled],0[disabled])
    */
   if (lst)
   {
      /* See if the connection has a port mirror entry. */
      char tempStr2[BUFLEN_1024];
      char maskStr[BUFLEN_1024];
      char maskTemp[BUFLEN_16];
      UINT32 i;
      WanDebugPortMirroringCfgObject *pmObj = NULL;

      tempStr2[0] = '\0';
      maskStr[0] = '\0';
      INIT_INSTANCE_ID_STACK(&iidStack);
      while( cmsObj_getNextFlags(MDMOID_WAN_DEBUG_PORT_MIRRORING_CFG,
           &iidStack, OGF_NO_VALUE_UPDATE, (void **) &pmObj) == CMSRET_SUCCESS )
      {
         if (pmObj->direction == PMIRROR_DIR_IN && pmObj->gemPortMaskArray)
         {
            /* Found port mirror for connection 'in' direction. */
            cmsUtl_hexStringToBinaryBuf(pmObj->gemPortMaskArray, &bin, &binSize);
            for (i = 0; i < maskSize && bin; i+=4)
            {
               snprintf(maskTemp, sizeof(maskTemp), "%02X%02X%02X%02X-",
                  ((i+3) < binSize) ? bin[i+3] : 0, 
                  ((i+2) < binSize) ? bin[i+2] : 0, 
                  ((i+1) < binSize) ? bin[i+1] : 0, 
                  ( i    < binSize) ? bin[i]   : 0);                   
               strncat(maskStr, maskTemp, sizeof(maskStr)-1);
            }
            if (bin)
                cmsMem_free(bin);
            /* remove last - */
            maskStr[strlen(maskStr)-1] = '\0';
            snprintf(tempStr2, sizeof(tempStr2), "%s/IN/%s/%c|",
               maskStr,
               pmObj->mirrorInterface,
               (pmObj->status == PMIRROR_ENABLED) ? '1' : '0');
            strncat(pMirrorList, tempStr2, sizeof(pMirrorList)-1);
            cmsObj_free((void **)&pmObj);
            break;
         }
         cmsObj_free((void **)&pmObj);
      }

      if( tempStr2[0] == '\0' )
      {
         /* Did not find port mirror for connection 'in' direction.
          * Make up default values.
          */
         for (i = 0; i < maskSize; i+=4)
            strncat(pMirrorList, "00000000-", sizeof(pMirrorList)-1);
         /* replace last - to / */
         pMirrorList[strlen(pMirrorList)-1] = '/';
         strncat(pMirrorList, "IN/ /0|", sizeof(pMirrorList)-1);
      }

      tempStr2[0] = '\0';
      maskStr[0] = '\0';
      INIT_INSTANCE_ID_STACK(&iidStack);
      while( cmsObj_getNextFlags(MDMOID_WAN_DEBUG_PORT_MIRRORING_CFG,
           &iidStack, OGF_NO_VALUE_UPDATE, (void **) &pmObj) == CMSRET_SUCCESS )
      {
         if (pmObj->direction == PMIRROR_DIR_OUT)
         {
            /* Found port mirror for connection 'out' direction. */
            cmsUtl_hexStringToBinaryBuf(pmObj->gemPortMaskArray, &bin, &binSize);
            for (i = 0; i < maskSize && bin; i+=4)
            {
               snprintf(maskTemp, sizeof(maskTemp), "%02X%02X%02X%02X-",
                  ((i+3) < binSize) ? bin[i+3] : 0, 
                  ((i+2) < binSize) ? bin[i+2] : 0, 
                  ((i+1) < binSize) ? bin[i+1] : 0, 
                  ( i    < binSize) ? bin[i]   : 0);                   
               strncat(maskStr, maskTemp, sizeof(maskStr)-1);
            }
            if (bin)
                cmsMem_free(bin);

            /* remove last - */
            maskStr[strlen(maskStr)-1] = '\0';

            snprintf(tempStr2, sizeof(tempStr2), "%s/OUT/%s/%c",
               maskStr,
               pmObj->mirrorInterface,
               (pmObj->status == PMIRROR_ENABLED) ? '1' : '0');
            strncat(pMirrorList, tempStr2, sizeof(pMirrorList)-1);
            cmsObj_free((void **)&pmObj);
            break;
         }
         cmsObj_free((void **)&pmObj);
      }

      if( tempStr2[0] == '\0' )
      {
         /* Did not find port mirror for connection 'out' direction.
          * Make up default values.
          */
         for (i = 0; i < maskSize; i+=4)
            strncat(pMirrorList, "00000000-", sizeof(pMirrorList)-1);
         /* replace last - to / */
         pMirrorList[strlen(pMirrorList)-1] = '/';

         strncat(pMirrorList, "OUT/ /0", sizeof(pMirrorList)-1);
      }

      if (pMirrorList[0] != '\0')
      {
          strcpy(lst, pMirrorList);
      }  
   }
}

CmsRet dalPMirror_configGponPortMirrors(char *lst)
{
    char *pToken = NULL, *pLast = NULL;
    char *cp1, *cp2, *cp3, *cp4;
    UINT32 gemMask;
    UINT32 maskSize;
    UINT32 index, i;
    UINT8  bin[CONFIG_BCM_MAX_GEM_PORTS / 8 + 1];
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    WanDebugPortMirroringCfgObject *obj = NULL;
    CmsRet ret = CMSRET_SUCCESS;
   
    maskSize = CONFIG_BCM_MAX_GEM_PORTS / 8;
    if (CONFIG_BCM_MAX_GEM_PORTS % 8) 
       maskSize += 1;

    INIT_INSTANCE_ID_STACK(&iidStack);
    /* Remove all current port mirroring entries. */
    while( cmsObj_getNext(MDMOID_WAN_DEBUG_PORT_MIRRORING_CFG, &iidStack,
        (void **) &obj) == CMSRET_SUCCESS )
    {
        cmsObj_deleteInstance(MDMOID_WAN_DEBUG_PORT_MIRRORING_CFG, &iidStack);
        INIT_INSTANCE_ID_STACK(&iidStack);
        cmsObj_free((void **)&obj);
    }

    /* Add new port mirroring entries in lst.  lst has following format:
     * gemPortMask-gemPortMask-...-gemPortMask-|
     * direction(IN, OUT)| mirror_interface|status(1[enabled],0[disabled])     
     */
    pToken = strtok_r(lst, ", ", &pLast);
    while ( pToken != NULL )
    {
        cp1 = cp2 = cp3 = cp4 = NULL;
        if( (cp1 = strstr(pToken, "|")) != NULL )
            if( (cp2 = strstr(cp1 + 1, "|")) != NULL )
                if( (cp3 = strstr(cp2 + 1, "|")) != NULL )
                    cp4 = strstr(cp3 + 1, "|");
            
        if( cp1 && cp2 && cp3 && cp4)
        {
            *cp1 = *cp2 = *cp3 = *cp4 = '\0';
            INIT_INSTANCE_ID_STACK(&iidStack);
            if( (ret = cmsObj_addInstance(MDMOID_WAN_DEBUG_PORT_MIRRORING_CFG,
                &iidStack)) == CMSRET_SUCCESS)
            {
                /* Get the instance of object in the newly created subtree,
                 * InternetGatewayDevice.X_BROADCOM_COM_DebugPortMirroringCfg.
                 * {i}
                 */
                if( (ret = cmsObj_get(MDMOID_WAN_DEBUG_PORT_MIRRORING_CFG,
                    &iidStack, 0, (void **) &obj)) == CMSRET_SUCCESS )
                {
                    /* Fill the object. */
                    {
                       char *p1 = pToken, *p2;
                       index = 0;
                       while(p1 < cp1)
                       {
                          if ( (p2 = strstr(p1, "-")) != NULL )
                          {
                             *p2 = '\0';
                             sscanf(p1, "%X", &gemMask);
                             for (i = 0; i < 4 && index < maskSize; i++)
                             {
                                bin[index++] = (unsigned char)((gemMask >> (i*8)) & 0xff);
                             }
                             p1 = p2 + 1;
                          } 
                          else
                              break;
                       }
                    }
                    cmsMem_free(obj->gemPortMaskArray);
                    cmsUtl_binaryBufToHexString(bin, maskSize, &obj->gemPortMaskArray);
                    CMSMEM_REPLACE_STRING(obj->mirrorInterface, &cp3[1]);
                    obj->direction = (!cmsUtl_strcmp(&cp2[1], "IN"))
                        ? PMIRROR_DIR_IN : PMIRROR_DIR_OUT;
                    obj->status = (cp4[1] == '1')
                        ? PMIRROR_ENABLED : PMIRROR_DISABLED;

                    cmsLog_debug("Adding new port mirroring entry: monitor "
                        "mirror intf=%s, direction=%d, status=%d",
                        obj->mirrorInterface, obj->direction, obj->status);
                    cmsLog_debug("gem port mask array:");
                    for (i = 0, index = 0; i < CONFIG_BCM_MAX_GEM_PORTS; i += 32, index += 8)
                    {
                        char str[BUFLEN_16];
                        memset(str, 0, sizeof(str));
                        memcpy(str, &obj->gemPortMaskArray[index], 
                            (index + 8) <= strlen(obj->gemPortMaskArray) ?
                            8 : strlen(obj->gemPortMaskArray) - index);
                        cmsLog_debug("%4d - %4d : %s", index, index+7, str);                       
                    }
                    /* Set X_BROADCOM_COM_DebugPortMirroringCfg. */
                    ret = cmsObj_set(obj, &iidStack);

                    /* Free X_BROADCOM_COM_DebugPortMirroringCfg object. */
                    cmsObj_free((void **) &obj);

                    if( ret != CMSRET_SUCCESS )
                    {
                        CmsRet r2;
                        cmsLog_error("Failed to set X_BROADCOM_COM_DebugPort"
                            "MirroringCfg, ret = %d", ret);
           
                        r2 = cmsObj_deleteInstance(
                            MDMOID_WAN_DEBUG_PORT_MIRRORING_CFG, &iidStack);
                        if( r2 != CMSRET_SUCCESS )
                            cmsLog_error("failed to delete created X_BROADCOM_"
                                "COM_DebugPortMirroringCfg, r2=%d", r2);

                        cmsLog_debug("Failed to set created X_BROADCOM_COM_"
                            "DebugPortMirroringCfg");
                    }
                }
                else
                {
                    cmsLog_error("Failed to get WanDebugPortMirroringCfgObject, "
                        "ret=%d", ret);
                    break;
                }       
            }
            else
            {
                cmsLog_error("could not create new port mirroring, ret=%d",ret);
                break;
            }
        }

        /* Get next port mirror config object information. */
        pToken = strtok_r(NULL, ", ", &pLast);
    }
    return (ret);
}
#endif
   
