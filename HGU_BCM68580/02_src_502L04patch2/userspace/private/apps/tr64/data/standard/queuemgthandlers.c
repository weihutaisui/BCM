/*****************************************************************************
 *
 *  Copyright (c) 2005-2012  Broadcom Corporation
 *  All Rights Reserved
 *
 * <:label-BRCM:2012:proprietary:standard
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
 *
 ************************************************************************/

#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "igd.h"
#include "queuemgtparams.h"
#include "tr64defs.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"
#include "cms_dal.h"
#include "cms_qdm.h"
#include "cms_qos.h"

int GetQueueMgt_Info(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)       
{
   InstanceIdStack iidStack;
   QMgmtObject *qMgmtObj = NULL;
   CmsRet ret;

   int errorinfo = 0;

   /* get the current queue management config */
   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_get(MDMOID_Q_MGMT, &iidStack, 0, (void **)&qMgmtObj)) == CMSRET_SUCCESS)
   {
      if(qMgmtObj->enable)
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_Enable, "1");  
      }
      else
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_Enable, "0");  
      }
      errorinfo |= OutputNumValueToAC(ac, VAR_MaxQueues, qMgmtObj->maxQueues);
      errorinfo |= OutputNumValueToAC(ac, VAR_MaxClassificationEntries, qMgmtObj->maxClassificationEntries);
      errorinfo |= OutputNumValueToAC(ac, VAR_MaxAppEntries, qMgmtObj->maxAppEntries);
      errorinfo |= OutputNumValueToAC(ac, VAR_MaxFlowEntries, qMgmtObj->maxFlowEntries);
      errorinfo |= OutputNumValueToAC(ac, VAR_MaxPolicerEntries, qMgmtObj->maxPolicerEntries);
      errorinfo |= OutputNumValueToAC(ac, VAR_MaxQueueEntries, qMgmtObj->maxQueueEntries);
      errorinfo |= OutputNumValueToAC(ac, VAR_DefaultForwardingPolicy, qMgmtObj->defaultForwardingPolicy);
      errorinfo |= OutputNumValueToAC(ac, VAR_DefaultPolicer, qMgmtObj->defaultPolicer);
      errorinfo |= OutputNumValueToAC(ac, VAR_DefaultQueue, qMgmtObj->defaultQueue);
      errorinfo |= OutputNumValueToAC(ac, VAR_DefaultDSCPMark, qMgmtObj->defaultDSCPMark);
      errorinfo |= OutputNumValueToAC(ac, VAR_DefaultEthernetPriorityMark, qMgmtObj->defaultEthernetPriorityMark);
      errorinfo |= OutputCharValueToAC(ac, VAR_AvailableAppList, qMgmtObj->availableAppList);

      cmsObj_free((void **) &qMgmtObj);;

   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }   
   
   if(errorinfo)
   {
      soap_error( uclient, errorinfo );
      return FALSE;
   } 
   return TRUE;
}
 
int SetQueueMGTEnable(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)       
{
   InstanceIdStack iidStack;
   QMgmtObject *qMgmtObj = NULL;
   CmsRet ret;

   int errorinfo = 0;
   struct Param *pParams;
   char QueueMgtEnable[4];

   pParams = findActionParamByRelatedVar(ac,VAR_Enable);
   if (pParams != NULL)
   {
      strcpy(QueueMgtEnable, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }     

   /* get the current queue management config */
   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_get(MDMOID_Q_MGMT, &iidStack, 0, (void **)&qMgmtObj)) == CMSRET_SUCCESS)
   {
      if(strcmp(QueueMgtEnable , "1") == 0)
      {
         qMgmtObj->enable = 1;
      }
      else
      {
         qMgmtObj->enable = 0;
      }

      ret = cmsObj_set(qMgmtObj, &iidStack);
      cmsObj_free((void **) &qMgmtObj);;

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("set of MDMOID_Q_MGMT failed, ret=%d", ret);
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }
   }
   else
   {
      cmsLog_error("cmsObj_get <MDMOID_Q_MGMT> returns error. ret=%d", ret);
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }   
   
   if(errorinfo)
   {
      soap_error( uclient, errorinfo );
      return FALSE;
   } 
   cmsMgm_saveConfigToFlash();
   return TRUE;
}

int SetDefaultBehavior(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)       
{
   InstanceIdStack iidStack;
   QMgmtObject *qMgmtObj = NULL;
   CmsRet ret;

   int errorinfo = 0;
   struct Param *pParams;
   
   char DefaultQueue[4];
   char DefaultDSCPMark[4];

   pParams = findActionParamByRelatedVar(ac,VAR_DefaultDSCPMark);
   if (pParams != NULL)
   {
      strcpy(DefaultDSCPMark, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }     

   pParams = findActionParamByRelatedVar(ac,VAR_DefaultQueue);
   if (pParams != NULL)
   {
      strcpy(DefaultQueue, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }   

   /* get the current queue management config */
   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_get(MDMOID_Q_MGMT, &iidStack, 0, (void **)&qMgmtObj)) == CMSRET_SUCCESS)
   {
      qMgmtObj->defaultQueue = atoi(DefaultQueue);
      qMgmtObj->defaultDSCPMark= atoi(DefaultDSCPMark);

      ret = cmsObj_set(qMgmtObj, &iidStack);
      cmsObj_free((void **) &qMgmtObj);;

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("set of MDMOID_Q_MGMT failed, ret=%d", ret);
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }
   }
   else
   {
      cmsLog_error("cmsObj_get <MDMOID_Q_MGMT> returns error. ret=%d", ret);
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }   

   if(errorinfo)
   {
      soap_error( uclient, errorinfo );
      return FALSE;
   } 
   cmsMgm_saveConfigToFlash();
   return TRUE;
}

int AddClassificationEntry( UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   QMgmtClassificationObject *cObj;
   InstanceIdStack iidStack;
   CmsRet ret;


   struct Param *pParams;

   unsigned int ClsEnable;
   int  ClsOrder;
   char ClassInterface[256];
   char DestIP[24];
   char DestMask[24];
   int  DestIPExclude;
   char SourceIP[24];
   char SourceMask[24];
   int  SourceIPExclude;
   int  Protocol;
   int  ProtocolExclude;
   int  DestPort;
   int  DestPortRangeMax;
   int  DestPortExclude;
   int  SourcePort;
   int  SourcePortRangeMax;
   int  SourcePortExclude;

   char SourceMACAddress[32];
   char SourceMACMask[32];
   int  SourceMACExclude;
   char DestMACAddress[32];
   char DestMACMask[32];
   int  DestMACExclude;
   int  Ethertype;
   int  EthertypeExclude;

   char SourceVendorClassID[256];
   int  SourceVendorClassIDExclude=0;
   char SourceUserClassID[256];
   int  SourceUserClassIDExclude=0;

   int  DSCPCheck;
   int  DSCPExclude;
   int  DSCPMark;
   int  EthernetPriorityCheck;
   int  EthernetPriorityExclude;
   int  EthernetPriorityMark;

   int  VLANIDCheck;
   int  VLANIDExclude;
   int  ForwardingPolicy;
   int  ClassPolicer;

   int  ClassQueue;
   int  ClassApp;

   pParams = findActionParamByRelatedVar(ac,VAR_ClassificationEnable);
   if (pParams != NULL)
   {
      ClsEnable = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }  

   pParams = findActionParamByRelatedVar(ac,VAR_ClassificationOrder);
   if (pParams != NULL)
   {
      ClsOrder = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 

   pParams = findActionParamByRelatedVar(ac,VAR_ClassInterface);
   if (pParams != NULL)
   {
      strcpy(ClassInterface, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 

   pParams = findActionParamByRelatedVar(ac,VAR_DestIP);
   if (pParams != NULL)
   {
      strcpy(DestIP, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 

   pParams = findActionParamByRelatedVar(ac,VAR_DestMask);
   if (pParams != NULL)
   {
      strcpy(DestMask, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 
   
   pParams = findActionParamByRelatedVar(ac,VAR_DestIPExclude);
   if (pParams != NULL)
   {
      DestIPExclude = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }    

   pParams = findActionParamByRelatedVar(ac,VAR_SourceIP);
   if (pParams != NULL)
   {
      strcpy(SourceIP, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 

   pParams = findActionParamByRelatedVar(ac,VAR_SourceMask);
   if (pParams != NULL)
   {
      strcpy(SourceMask, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 
   
   pParams = findActionParamByRelatedVar(ac,VAR_SourceIPExclude);
   if (pParams != NULL)
   {
      SourceIPExclude = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }    

   pParams = findActionParamByRelatedVar(ac,VAR_Protocol);
   if (pParams != NULL)
   {
      Protocol = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 

   pParams = findActionParamByRelatedVar(ac,VAR_ProtocolExclude);
   if (pParams != NULL)
   {
      ProtocolExclude = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 
   
   pParams = findActionParamByRelatedVar(ac,VAR_DestPort);
   if (pParams != NULL)
   {
      DestPort = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }    

   pParams = findActionParamByRelatedVar(ac,VAR_DestPortRangeMax);
   if (pParams != NULL)
   {
      DestPortRangeMax = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }    

   pParams = findActionParamByRelatedVar(ac,VAR_DestPortExclude);
   if (pParams != NULL)
   {
      DestPortExclude = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }  

   pParams = findActionParamByRelatedVar(ac,VAR_SourcePort);
   if (pParams != NULL)
   {
      SourcePort = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }    

   pParams = findActionParamByRelatedVar(ac,VAR_SourcePortRangeMax);
   if (pParams != NULL)
   {
      SourcePortRangeMax = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }    

   pParams = findActionParamByRelatedVar(ac,VAR_SourcePortExclude);
   if (pParams != NULL)
   {
      SourcePortExclude = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }  

   pParams = findActionParamByRelatedVar(ac,VAR_SourceMACAddress);
   if (pParams != NULL)
   {
      strcpy(SourceMACAddress, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 

   pParams = findActionParamByRelatedVar(ac,VAR_SourceMACMask);
   if (pParams != NULL)
   {
      strcpy(SourceMACMask, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 

   pParams = findActionParamByRelatedVar(ac,VAR_SourceMACExclude);
   if (pParams != NULL)
   {
      SourceMACExclude = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 

   pParams = findActionParamByRelatedVar(ac,VAR_DestMACAddress);
   if (pParams != NULL)
   {
      strcpy(DestMACAddress, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 

   pParams = findActionParamByRelatedVar(ac,VAR_DestMACMask);
   if (pParams != NULL)
   {
      strcpy(DestMACMask, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 

   pParams = findActionParamByRelatedVar(ac,VAR_DestMACExclude);
   if (pParams != NULL)
   {
      DestMACExclude = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 

   pParams = findActionParamByRelatedVar(ac,VAR_Ethertype);
   if (pParams != NULL)
   {
      Ethertype = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 

   pParams = findActionParamByRelatedVar(ac,VAR_EthertypeExclude);
   if (pParams != NULL)
   {
      EthertypeExclude = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 

   pParams = findActionParamByRelatedVar(ac,VAR_DSCPCheck);
   if (pParams != NULL)
   {
      DSCPCheck = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   pParams = findActionParamByRelatedVar(ac,VAR_DSCPExclude);
   if (pParams != NULL)
   {
      DSCPExclude = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 
   
   pParams = findActionParamByRelatedVar(ac,VAR_DSCPMark);
   if (pParams != NULL)
   {
      DSCPMark = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 

   pParams = findActionParamByRelatedVar(ac,VAR_EthernetPriorityCheck);
   if (pParams != NULL)
   {
      EthernetPriorityCheck = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 

   pParams = findActionParamByRelatedVar(ac,VAR_EthernetPriorityExclude);
   if (pParams != NULL)
   {
      EthernetPriorityExclude = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 
   
   pParams = findActionParamByRelatedVar(ac,VAR_EthernetPriorityMark);
   if (pParams != NULL)
   {
      EthernetPriorityMark = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }    

   pParams = findActionParamByRelatedVar(ac,VAR_VLANIDCheck);
   if (pParams != NULL)
   {
      VLANIDCheck = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 
   
   pParams = findActionParamByRelatedVar(ac,VAR_VLANIDExclude);
   if (pParams != NULL)
   {
      VLANIDExclude = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }   
 
   pParams = findActionParamByRelatedVar(ac,VAR_ForwardingPolicy);
   if (pParams != NULL)
   {
      ForwardingPolicy = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }    

   pParams = findActionParamByRelatedVar(ac,VAR_ClassPolicer);
   if (pParams != NULL)
   {
      ClassPolicer = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }    

   pParams = findActionParamByRelatedVar(ac,VAR_ClassQueue);
   if (pParams != NULL)
   {
      ClassQueue = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 

   pParams = findActionParamByRelatedVar(ac,VAR_ClassApp);
   if (pParams != NULL)
   {
      ClassApp= atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 

   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_get(MDMOID_Q_MGMT_CLASSIFICATION, &iidStack, OGF_DEFAULT_VALUES, (void **)&cObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get returns error. ret=%d", ret);
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   
   cObj->classificationEnable = ClsEnable;
   cObj->classificationOrder = ClsOrder;
   CMSMEM_REPLACE_STRING(cObj->classInterface , ClassInterface);
   CMSMEM_REPLACE_STRING(cObj->destIP, DestIP);
   CMSMEM_REPLACE_STRING(cObj->destMask, DestMask);
   cObj->destIPExclude = DestIPExclude;
   CMSMEM_REPLACE_STRING(cObj->sourceIP, SourceIP);  
   CMSMEM_REPLACE_STRING(cObj->sourceMask, SourceMask);  
   cObj->sourceIPExclude = SourceIPExclude;
   cObj->protocol = Protocol;
   cObj->protocolExclude = ProtocolExclude;
   cObj->destPort = DestPort;
   cObj->destPortRangeMax = DestPortRangeMax;
   cObj->destPortExclude = DestPortExclude;
   cObj->sourcePort = SourcePort;
   cObj->sourcePortRangeMax = SourcePortRangeMax;
   cObj->sourcePortExclude = SourcePortExclude;
   CMSMEM_REPLACE_STRING(cObj->sourceMACAddress, SourceMACAddress);  
   CMSMEM_REPLACE_STRING(cObj->sourceMACMask, SourceMACMask);  
   cObj->sourceMACExclude = SourceMACExclude;

   CMSMEM_REPLACE_STRING(cObj->destMACAddress, DestMACAddress);  
   CMSMEM_REPLACE_STRING(cObj->destMACMask, DestMACMask);  
   cObj->destMACExclude = DestMACExclude;

   cObj->ethertype = Ethertype;
   cObj->ethertypeExclude = EthertypeExclude;

   CMSMEM_REPLACE_STRING(cObj->sourceVendorClassID, SourceVendorClassID);  
   cObj->sourceVendorClassIDExclude = SourceVendorClassIDExclude;

   CMSMEM_REPLACE_STRING(cObj->sourceUserClassID, SourceUserClassID);  
   cObj->sourceUserClassIDExclude = SourceUserClassIDExclude;

   cObj->DSCPCheck = DSCPCheck;
   cObj->DSCPExclude = DSCPExclude;
   cObj->DSCPMark = DSCPMark;

   cObj->ethernetPriorityCheck = EthernetPriorityCheck;
   cObj->ethernetPriorityExclude = EthernetPriorityExclude;
   cObj->ethernetPriorityMark = EthernetPriorityMark;

   cObj->VLANIDCheck = VLANIDCheck;
   cObj->VLANIDExclude = VLANIDExclude;

   cObj->forwardingPolicy = ForwardingPolicy;
   cObj->classPolicer = ClassPolicer;

   cObj->classQueue = ClassQueue;
   cObj->classApp = ClassApp;

   /* Create new classification instance, and insert this object in there */
   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_addInstance(MDMOID_Q_MGMT_CLASSIFICATION, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_addInstance <MDMOID_Q_MGMT_CLASSIFICATION> returns error, ret=%d", ret);
   }
   else
   {
      if ((ret = cmsObj_set((void *)cObj, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsObj_set returns error, ret = %d", ret);
         cmsObj_deleteInstance(MDMOID_Q_MGMT_CLASSIFICATION, &iidStack);
      }
   }

   if (ret != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   cmsMgm_saveConfigToFlash();

   return TRUE;    
}

int DeleteClassificationEntry( UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   QMgmtClassificationObject *cObj;
   InstanceIdStack iidStack;
   CmsRet ret;

   struct Param *pParams;

   char ClassificationKey[32]; 

   pParams = findActionParamByRelatedVar(ac,VAR_ClassificationKey);
   if (pParams != NULL)
   {
      strcpy(ClassificationKey, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }    
   
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_Q_MGMT_CLASSIFICATION, &iidStack, (void **)&cObj)) == CMSRET_SUCCESS)
   {

      if ( atoi(ClassificationKey) == cObj->classificationKey )
      {
         /* delete this instance */
         if ((ret = cmsObj_deleteInstance(MDMOID_Q_MGMT_CLASSIFICATION, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsObj_free((void **)&cObj);
            break;
         }
      }
      cmsObj_free((void **)&cObj);
   }

   if(ret != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_NOSUCHENTRYINARRAY );
      return FALSE;   
   }

   cmsMgm_saveConfigToFlash();

   return TRUE;
}
      
int GetSpecificClassificationEntry(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)       
{
   QMgmtClassificationObject *cObj;
   InstanceIdStack iidStack;
   CmsRet ret;

   int errorinfo = 0;
   int found = 0;
   struct Param *pParams;

   char ClassificationKey[32]; 


   pParams = findActionParamByRelatedVar(ac,VAR_ClassificationKey);
   if (pParams != NULL)
   {
      strcpy(ClassificationKey, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }     

   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_Q_MGMT_CLASSIFICATION, &iidStack, (void **)&cObj)) == CMSRET_SUCCESS)
   {
      if ( atoi(ClassificationKey) == cObj->classificationKey )
      {
         found = 1;
         errorinfo |= OutputNumValueToAC(ac, VAR_ClassificationEnable, cObj->classificationEnable);
         errorinfo |= OutputNumValueToAC(ac, VAR_ClassificationOrder, cObj->classificationOrder);
         errorinfo |= OutputCharValueToAC(ac, VAR_ClassInterface, cObj->classInterface);
         errorinfo |= OutputCharValueToAC(ac, VAR_DestIP, cObj->destIP);
         errorinfo |= OutputCharValueToAC(ac, VAR_DestMask, cObj->destMask);
         errorinfo |= OutputNumValueToAC(ac, VAR_DestIPExclude, cObj->destIPExclude);
         errorinfo |= OutputCharValueToAC(ac, VAR_SourceIP, cObj->sourceIP);
         errorinfo |= OutputCharValueToAC(ac, VAR_SourceMask, cObj->sourceMask);
         errorinfo |= OutputNumValueToAC(ac, VAR_SourceIPExclude, cObj->sourceIPExclude);
         errorinfo |= OutputNumValueToAC(ac, VAR_Protocol, cObj->protocol);
         errorinfo |= OutputNumValueToAC(ac, VAR_ProtocolExclude, cObj->protocolExclude);
         errorinfo |= OutputNumValueToAC(ac, VAR_DestPort, cObj->destPort);   
         errorinfo |= OutputNumValueToAC(ac, VAR_DestPortRangeMax, cObj->destPortRangeMax);   
         errorinfo |= OutputNumValueToAC(ac, VAR_DestPortExclude, cObj->destPortExclude);   
         errorinfo |= OutputNumValueToAC(ac, VAR_SourcePort, cObj->sourcePort);   
         errorinfo |= OutputNumValueToAC(ac, VAR_SourcePortRangeMax, cObj->sourcePortRangeMax);   
         errorinfo |= OutputNumValueToAC(ac, VAR_SourcePortExclude, cObj->sourcePortExclude); 
         errorinfo |= OutputCharValueToAC(ac, VAR_SourceMACAddress, cObj->sourceMACAddress); 
         errorinfo |= OutputCharValueToAC(ac, VAR_SourceMACMask, cObj->sourceMACMask); 
         errorinfo |= OutputNumValueToAC(ac, VAR_SourceMACExclude, cObj->sourceMACExclude); 
         errorinfo |= OutputCharValueToAC(ac, VAR_DestMACAddress, cObj->destMACAddress); 
         errorinfo |= OutputCharValueToAC(ac, VAR_DestMACMask, cObj->destMACMask); 
         errorinfo |= OutputNumValueToAC(ac, VAR_DestMACExclude, cObj->destMACExclude); 
         errorinfo |= OutputNumValueToAC(ac, VAR_Ethertype, cObj->ethertype); 
         errorinfo |= OutputNumValueToAC(ac, VAR_EthertypeExclude, cObj->ethertypeExclude); 
         errorinfo |= OutputCharValueToAC(ac, VAR_SourceVendorClassID, cObj->sourceVendorClassID); 
         errorinfo |= OutputNumValueToAC(ac, VAR_SourceVendorClassIDExclude, cObj->sourceVendorClassIDExclude); 
         errorinfo |= OutputCharValueToAC(ac, VAR_SourceUserClassID, cObj->sourceUserClassID); 
         errorinfo |= OutputNumValueToAC(ac, VAR_SourceUserClassIDExclude, cObj->sourceUserClassIDExclude); 
         errorinfo |= OutputNumValueToAC(ac, VAR_DSCPCheck, cObj->DSCPCheck);      
         errorinfo |= OutputNumValueToAC(ac, VAR_DSCPExclude, cObj->DSCPExclude);      
         errorinfo |= OutputNumValueToAC(ac, VAR_DSCPMark, cObj->DSCPMark);      
         errorinfo |= OutputNumValueToAC(ac, VAR_EthernetPriorityCheck, cObj->ethernetPriorityCheck);      
         errorinfo |= OutputNumValueToAC(ac, VAR_EthernetPriorityExclude, cObj->ethernetPriorityExclude);      
         errorinfo |= OutputNumValueToAC(ac, VAR_EthernetPriorityMark, cObj->ethernetPriorityMark);      
         errorinfo |= OutputNumValueToAC(ac, VAR_ClassQueue, cObj->classQueue);      
         errorinfo |= OutputNumValueToAC(ac, VAR_VLANIDCheck, cObj->VLANIDCheck); 
         errorinfo |= OutputNumValueToAC(ac, VAR_VLANIDExclude, cObj->VLANIDExclude); 
         errorinfo |= OutputNumValueToAC(ac, VAR_ForwardingPolicy, cObj->forwardingPolicy); 
         errorinfo |= OutputNumValueToAC(ac, VAR_ClassPolicer, cObj->classPolicer); 
         errorinfo |= OutputNumValueToAC(ac, VAR_ClassApp, cObj->classApp); 

         cmsObj_free((void **)&cObj);
         break;
      }
      cmsObj_free((void **)&cObj);

   }
   
   if(found ==0)
   {
      soap_error( uclient, SOAP_NOSUCHENTRYINARRAY );
      return FALSE;
   } 

   if(errorinfo)
   {
      soap_error( uclient, errorinfo );
      return FALSE;
   } 
   return TRUE;
}
     
int GetGenericClassificationEntry(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)       
{
   QMgmtClassificationObject *cObj;
   InstanceIdStack iidStack;
   CmsRet ret;

   uint32 index = 0;
   int found = 0;
   int errorinfo = 0;
   struct Param *pParams;

   pParams = findActionParamByRelatedVar(ac,VAR_ClassificationNumberOfEntries);
   if (pParams != NULL)
   {
      index = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }     

   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_Q_MGMT_CLASSIFICATION, &iidStack, (void **)&cObj)) == CMSRET_SUCCESS)
   {                  
      if ( index == iidStack.instance[iidStack.currentDepth-1])
      {
         found = 1;

         errorinfo |= OutputNumValueToAC(ac, VAR_ClassificationKey, cObj->classificationKey);
         errorinfo |= OutputNumValueToAC(ac, VAR_ClassificationEnable, cObj->classificationEnable);
         errorinfo |= OutputNumValueToAC(ac, VAR_ClassificationOrder, cObj->classificationOrder);
         errorinfo |= OutputCharValueToAC(ac, VAR_ClassInterface, cObj->classInterface);
         errorinfo |= OutputCharValueToAC(ac, VAR_DestIP, cObj->destIP);
         errorinfo |= OutputCharValueToAC(ac, VAR_DestMask, cObj->destMask);
         errorinfo |= OutputNumValueToAC(ac, VAR_DestIPExclude, cObj->destIPExclude);
         errorinfo |= OutputCharValueToAC(ac, VAR_SourceIP, cObj->sourceIP);
         errorinfo |= OutputCharValueToAC(ac, VAR_SourceMask, cObj->sourceMask);
         errorinfo |= OutputNumValueToAC(ac, VAR_SourceIPExclude, cObj->sourceIPExclude);
         errorinfo |= OutputNumValueToAC(ac, VAR_Protocol, cObj->protocol);
         errorinfo |= OutputNumValueToAC(ac, VAR_ProtocolExclude, cObj->protocolExclude);
         errorinfo |= OutputNumValueToAC(ac, VAR_DestPort, cObj->destPort);   
         errorinfo |= OutputNumValueToAC(ac, VAR_DestPortRangeMax, cObj->destPortRangeMax);   
         errorinfo |= OutputNumValueToAC(ac, VAR_DestPortExclude, cObj->destPortExclude);   
         errorinfo |= OutputNumValueToAC(ac, VAR_SourcePort, cObj->sourcePort);   
         errorinfo |= OutputNumValueToAC(ac, VAR_SourcePortRangeMax, cObj->sourcePortRangeMax);   
         errorinfo |= OutputNumValueToAC(ac, VAR_SourcePortExclude, cObj->sourcePortExclude); 
         errorinfo |= OutputCharValueToAC(ac, VAR_SourceMACAddress, cObj->sourceMACAddress); 
         errorinfo |= OutputCharValueToAC(ac, VAR_SourceMACMask, cObj->sourceMACMask); 
         errorinfo |= OutputNumValueToAC(ac, VAR_SourceMACExclude, cObj->sourceMACExclude); 
         errorinfo |= OutputCharValueToAC(ac, VAR_DestMACAddress, cObj->destMACAddress); 
         errorinfo |= OutputCharValueToAC(ac, VAR_DestMACMask, cObj->destMACMask); 
         errorinfo |= OutputNumValueToAC(ac, VAR_DestMACExclude, cObj->destMACExclude); 
         errorinfo |= OutputNumValueToAC(ac, VAR_Ethertype, cObj->ethertype); 
         errorinfo |= OutputNumValueToAC(ac, VAR_EthertypeExclude, cObj->ethertypeExclude); 
         errorinfo |= OutputCharValueToAC(ac, VAR_SourceVendorClassID, cObj->sourceVendorClassID); 
         errorinfo |= OutputNumValueToAC(ac, VAR_SourceVendorClassIDExclude, cObj->sourceVendorClassIDExclude); 
         errorinfo |= OutputCharValueToAC(ac, VAR_SourceUserClassID, cObj->sourceUserClassID); 
         errorinfo |= OutputNumValueToAC(ac, VAR_SourceUserClassIDExclude, cObj->sourceUserClassIDExclude); 
         errorinfo |= OutputNumValueToAC(ac, VAR_DSCPCheck, cObj->DSCPCheck);      
         errorinfo |= OutputNumValueToAC(ac, VAR_DSCPExclude, cObj->DSCPExclude);      
         errorinfo |= OutputNumValueToAC(ac, VAR_DSCPMark, cObj->DSCPMark);      
         errorinfo |= OutputNumValueToAC(ac, VAR_EthernetPriorityCheck, cObj->ethernetPriorityCheck);      
         errorinfo |= OutputNumValueToAC(ac, VAR_EthernetPriorityExclude, cObj->ethernetPriorityExclude);      
         errorinfo |= OutputNumValueToAC(ac, VAR_EthernetPriorityMark, cObj->ethernetPriorityMark);      
         errorinfo |= OutputNumValueToAC(ac, VAR_ClassQueue, cObj->classQueue);      
         errorinfo |= OutputNumValueToAC(ac, VAR_VLANIDCheck, cObj->VLANIDCheck); 
         errorinfo |= OutputNumValueToAC(ac, VAR_VLANIDExclude, cObj->VLANIDExclude); 
         errorinfo |= OutputNumValueToAC(ac, VAR_ForwardingPolicy, cObj->forwardingPolicy); 
         errorinfo |= OutputNumValueToAC(ac, VAR_ClassPolicer, cObj->classPolicer); 
         errorinfo |= OutputNumValueToAC(ac, VAR_ClassApp, cObj->classApp); 

         cmsObj_free((void **)&cObj);
         break;
      }
      cmsObj_free((void **)&cObj);

   }
   
   if(found ==0)
   {
      soap_error( uclient, SOAP_NOSUCHENTRYINARRAY );
      return FALSE;
   } 


   
   if(errorinfo)
   {
      soap_error( uclient, errorinfo );
      return FALSE;
   } 
   return TRUE;
}

int SetClassificationEntryEnable(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)       
{
   QMgmtClassificationObject *cObj;
   InstanceIdStack iidStack;
   CmsRet ret;

   struct Param *pParams;

   char ClassificationKey[32];
   char ClassificationEnable[4];

   pParams = findActionParamByRelatedVar(ac,VAR_ClassificationKey);
   if (pParams != NULL)
   {
      strcpy(ClassificationKey, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }     

   pParams = findActionParamByRelatedVar(ac,VAR_ClassificationEnable);
   if (pParams != NULL)
   {
      strcpy(ClassificationEnable, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }     
   
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_Q_MGMT_CLASSIFICATION, &iidStack, (void **)&cObj)) == CMSRET_SUCCESS)
   {
      if(cObj->classificationKey == atoi(ClassificationKey))
      {
         if(0==strcmp(ClassificationEnable, "0"))
         {
            cObj->classificationEnable = 0;
         }
         else
         {
            cObj->classificationEnable = 1;
         }
         ret = cmsObj_set(cObj, &iidStack);         
      }
      cmsObj_free((void **)&cObj);
   }  

   if( ret!= CMSRET_SUCCESS )
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 
   cmsMgm_saveConfigToFlash();
   return TRUE;
}
 
      
int SetClassificationEntryOrder(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)       
{
   QMgmtClassificationObject *cObj;
   InstanceIdStack iidStack;
   CmsRet ret;

   struct Param *pParams;

   char ClassificationKey[32];
   char ClassificationOrder[32];

   pParams = findActionParamByRelatedVar(ac,VAR_ClassificationKey);
   if (pParams != NULL)
   {
      strcpy(ClassificationKey, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }     

   pParams = findActionParamByRelatedVar(ac,VAR_ClassificationOrder);
   if (pParams != NULL)
   {
      strcpy(ClassificationOrder, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }     
   
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_Q_MGMT_CLASSIFICATION, &iidStack, (void **)&cObj)) == CMSRET_SUCCESS)
   {
      if(cObj->classificationKey == atoi(ClassificationKey))
      {
         cObj->classificationOrder = atoi(ClassificationOrder);
         ret = cmsObj_set(cObj, &iidStack);         
      }
      cmsObj_free((void **)&cObj);
   }  

   if( ret!= CMSRET_SUCCESS )
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 
   cmsMgm_saveConfigToFlash();
   return TRUE;
}
      

#define CMS_TR64_ADDED_QUEUE_NAME  "tr64-added-queue"


int AddQueueEntry( UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   CmsRet ret;

   struct Param *pParams;

   UBOOL8 enable=FALSE;
   UINT32 queueId=1;  // cgiQosQueueAdd has complex logic to figure out queueId, for now hardcode here
   const char *algorithm="SP";
   char intfNameBuf[CMS_IFNAME_LENGTH]={0};
   char queueKeyNameBuf[BUFLEN_128]={0};
   UINT32 queueKey;
   UINT32 precedence=1;

   pParams = findActionParamByRelatedVar(ac,VAR_QueueEnable);
   if (pParams != NULL)
   {
      if (!cmsUtl_strcasecmp(pParams->value, "true") ||
          !cmsUtl_strcasecmp(pParams->value, "1"))
      {
         enable = TRUE;
      }
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }  

   pParams = findActionParamByRelatedVar(ac,VAR_QueueInterface);
   if (pParams != NULL)
   {
      ret = qdmIntf_fullPathToIntfnameLocked(pParams->value, intfNameBuf);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("qdmIntf_fullPathToIntfname on %s ret=%d",
                      pParams->value, ret);
      }
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 

   pParams = findActionParamByRelatedVar(ac,VAR_QueuePrecedence);
   if (pParams != NULL)
   {
      precedence = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 

   /*
    * Cannot set queueKey directly, so embedded it inside of queueName.
    */
   pParams = findActionParamByRelatedVar(ac,VAR_QueueKey);
   if (pParams != NULL)
   {
      queueKey = atoi(pParams->value);
      sprintf(queueKeyNameBuf, "%s-%d", CMS_TR64_ADDED_QUEUE_NAME, queueKey);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 
   
   /* dalQos_queueAdd provides very little value here.  Maybe we should
    * just create an instance, get the object, and set the params that we
    * received.  Will need separate _igd and _dev2 versions of this function
    * though.
    */
   ret = dalQos_queueAdd(intfNameBuf, algorithm,
                         enable, queueKeyNameBuf, queueId,
                         1, precedence,
                         -1, -1, 0,
                         -1, -1, "DT", 0, 0, 0, 0);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("dalQos_queueAdd returns error. ret=%d", ret);
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   

   if (ret!=CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   cmsMgm_saveConfigToFlash();
   return TRUE;    
}

int DeleteQueueEntry( UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   QMgmtQueueObject *queueObj;
   InstanceIdStack iidStack;
   char queueKeyNameBuf[BUFLEN_128]={0};
   UINT32 queueKey;
   CmsRet ret;
   struct Param *pParams;


   pParams = findActionParamByRelatedVar(ac,VAR_QueueKey);
   if (pParams != NULL)
   {
      queueKey = atoi(pParams->value);
      sprintf(queueKeyNameBuf, "%s-%d", CMS_TR64_ADDED_QUEUE_NAME, queueKey);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }    
   
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_Q_MGMT_QUEUE, &iidStack, (void **)&queueObj)) == CMSRET_SUCCESS)
   {

      if ( !cmsUtl_strcmp(queueKeyNameBuf, queueObj->X_BROADCOM_COM_QueueName ) )
      {
         if ((ret = cmsObj_deleteInstance(MDMOID_Q_MGMT_QUEUE, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("dalQos_queueDelete returns error, ret=%d", ret);
         }
         cmsObj_free((void **)&queueObj);

         /* queueKey is unique, so once we found it, we can break out of loop */
         break;
      }

      cmsObj_free((void **)&queueObj);
   }

   if(ret != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_NOSUCHENTRYINARRAY );
      return FALSE;   
   }
   cmsMgm_saveConfigToFlash();
   return TRUE; 
}

int SetQueueEntryEnable(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)       
{
   QMgmtQueueObject *queueObj;
   InstanceIdStack iidStack;
   CmsRet ret;

   struct Param *pParams;

   char QueueKey[32];
   char QueueEnable[4];

   pParams = findActionParamByRelatedVar(ac,VAR_QueueKey);
   if (pParams != NULL)
   {
      strcpy(QueueKey, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }     

   pParams = findActionParamByRelatedVar(ac,VAR_QueueEnable);
   if (pParams != NULL)
   {
      strcpy(QueueEnable, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }     
   
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_Q_MGMT_QUEUE, &iidStack, (void **)&queueObj)) == CMSRET_SUCCESS)
   {
      if(queueObj->queueKey == atoi(QueueKey))
      {
         if(0==strcmp(QueueEnable, "0"))
         {
            queueObj->queueEnable = 0;
         }
         else
         {
            queueObj->queueEnable = 1;
         }
         ret = cmsObj_set(queueObj, &iidStack);         
      }
      cmsObj_free((void **)&queueObj);
   }  

   if( ret!= CMSRET_SUCCESS )
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 
   cmsMgm_saveConfigToFlash();
   return TRUE;
}

int GetSpecificQueueEntry(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)       
{
   QMgmtQueueObject *queueObj;
   InstanceIdStack iidStack;
   CmsRet ret;

   int found = 0;
   int errorinfo = 0;
   struct Param *pParams;

   char QueueKey[8];

   pParams = findActionParamByRelatedVar(ac,VAR_QueueKey);
   if (pParams != NULL)
   {
      strcpy(QueueKey, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }     

   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_Q_MGMT_QUEUE, &iidStack, (void **)&queueObj)) == CMSRET_SUCCESS)
   {

      if ( atoi(QueueKey) == queueObj->queueKey )
      {
         found = 1;
         errorinfo |= OutputNumValueToAC(ac, VAR_QueueEnable, queueObj->queueEnable);
         errorinfo |= OutputCharValueToAC(ac, VAR_QueueStatus, queueObj->queueStatus);
         errorinfo |= OutputCharValueToAC(ac, VAR_QueueInterface, queueObj->queueInterface);
         errorinfo |= OutputNumValueToAC(ac, VAR_QueuePrecedence, queueObj->queuePrecedence);
         cmsObj_free((void **)&queueObj);
         break;
      }
      cmsObj_free((void **)&queueObj);   
  }

   
   if(found == 0)
   {
      soap_error( uclient, SOAP_NOSUCHENTRYINARRAY );
      return FALSE;
   }
   
   if(errorinfo)
   {
      soap_error( uclient, errorinfo );
      return FALSE;
   } 
   return TRUE;
}

int GetGenericQueueEntry(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)       
{
   QMgmtQueueObject *queueObj;
   InstanceIdStack iidStack;
   CmsRet ret;
   
   int index = 0;
   int found = 0;
   int errorinfo = 0;
   struct Param *pParams;

   pParams = findActionParamByRelatedVar(ac,VAR_QueueNumberOfEntries);
   if (pParams != NULL)
   {
      index = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }     
  

   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_Q_MGMT_QUEUE, &iidStack, (void **)&queueObj)) == CMSRET_SUCCESS)
   {
      if ( index == iidStack.instance[iidStack.currentDepth-1] )
      {
         found = 1;
         errorinfo |= OutputNumValueToAC(ac, VAR_QueueKey, queueObj->queueKey);
         errorinfo |= OutputNumValueToAC(ac, VAR_QueueEnable, queueObj->queueEnable);
         errorinfo |= OutputCharValueToAC(ac, VAR_QueueStatus, queueObj->queueStatus);
         errorinfo |= OutputCharValueToAC(ac, VAR_QueueInterface, queueObj->queueInterface);
         errorinfo |= OutputNumValueToAC(ac, VAR_QueuePrecedence, queueObj->queuePrecedence);

         cmsObj_free((void **)&queueObj);
         break;
      }
      cmsObj_free((void **)&queueObj);   
   }

   
   if(found == 0)
   {
      soap_error( uclient, SOAP_NOSUCHENTRYINARRAY );
      return FALSE;
   }
   
   if(errorinfo)
   {
      soap_error( uclient, errorinfo );
      return FALSE;
   } 
   return TRUE;
}
