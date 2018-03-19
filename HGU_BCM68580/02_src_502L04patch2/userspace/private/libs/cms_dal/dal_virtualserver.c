/***********************************************************************
 *
 *  Copyright (c) 2006-2009  Broadcom Corporation
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

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"

/* local functions */
static CmsRet fillVrtSrvCfg(const UBOOL8 ppp_found, const char *srvName, const char *srvAddr, const char *protocol, const UINT16 EPS, const UINT16 EPE, 
                                             const UINT16 IPS, const UINT16 IPE, void * obj);

CmsRet dalVirtualServer_addEntry_igd(const char *dstWanIf, const char *srvName, const char *srvAddr, const char *protocol, 
	                                                         const UINT16 EPS, const UINT16 EPE, const UINT16 IPS, const UINT16 IPE)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack pppIidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack ipIidStack = EMPTY_INSTANCE_ID_STACK;
   WanPppConnObject *pppConn = NULL;   
   WanIpConnObject  *ipConn = NULL;
   UBOOL8 ppp_found = FALSE;
   UBOOL8 ip_found = FALSE;
   WanPppConnPortmappingObject *pppConnPM = NULL;
   WanIpConnPortmappingObject *ipConnPM = NULL;

   /* adding a new virtual server entry */
   cmsLog_debug("Adding new virtual server with %s/%s/%s/%s/%u/%u/%u/%u", dstWanIf, srvName, srvAddr, protocol, EPS, EPE, IPS, IPE);

   INIT_INSTANCE_ID_STACK(&pppIidStack);
   while (!ppp_found && 
      (cmsObj_getNextFlags(MDMOID_WAN_PPP_CONN, &pppIidStack, OGF_NO_VALUE_UPDATE, (void **) &pppConn)) == CMSRET_SUCCESS)
   {
      if(!cmsUtl_strcmp(pppConn->X_BROADCOM_COM_IfName, dstWanIf))
      {
         ppp_found = TRUE;
      }
      cmsObj_free((void **) &pppConn);
   }

   INIT_INSTANCE_ID_STACK(&ipIidStack);
   while (!ppp_found && !ip_found &&
      (cmsObj_getNextFlags(MDMOID_WAN_IP_CONN, &ipIidStack, OGF_NO_VALUE_UPDATE, (void **) &ipConn)) == CMSRET_SUCCESS)
   {
      if(!cmsUtl_strcmp(ipConn->X_BROADCOM_COM_IfName, dstWanIf))
      {
         ip_found = TRUE;
      }
      cmsObj_free((void **) &ipConn);
   }
   

   /* add new instance */
   if (ppp_found == TRUE)
   {
      if ((ret = cmsObj_addInstance(MDMOID_WAN_PPP_CONN_PORTMAPPING, &pppIidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not create new virtual server entry, ret=%d", ret);
         return ret;
      }

      if ((ret = cmsObj_get(MDMOID_WAN_PPP_CONN_PORTMAPPING, &pppIidStack, 0, (void **) &pppConnPM)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to get VirtualServerCfgObject, ret=%d", ret);
         cmsObj_deleteInstance(MDMOID_WAN_PPP_CONN_PORTMAPPING, &pppIidStack);
         return ret;
      }       

      if ((ret = fillVrtSrvCfg(ppp_found, srvName, srvAddr, protocol, EPS, EPE, IPS, IPE, pppConnPM)) != CMSRET_SUCCESS)
      {
         cmsObj_free((void **) &pppConnPM);
         cmsObj_deleteInstance(MDMOID_WAN_PPP_CONN_PORTMAPPING, &pppIidStack);
         return ret;
      }
   
      cmsLog_debug("in pppConnPM, %s/%s/%s/%u/%u/%u/%u", 
         pppConnPM->portMappingDescription, pppConnPM->internalClient, pppConnPM->portMappingProtocol, pppConnPM->externalPort, 
         pppConnPM->X_BROADCOM_COM_ExternalPortEnd, pppConnPM->internalPort, pppConnPM->X_BROADCOM_COM_InternalPortEnd);
      
      /* set and activate WanPppConnPortmappingObject */
      ret = cmsObj_set(pppConnPM, &pppIidStack);
      cmsObj_free((void **) &pppConnPM);
   
      if (ret != CMSRET_SUCCESS)
      {
         CmsRet r2;
         cmsLog_error("Failed to set WanPppConnPortmappingObject, ret = %d", ret);
          
         r2 = cmsObj_deleteInstance(MDMOID_WAN_PPP_CONN_PORTMAPPING, &pppIidStack);
         if (r2 != CMSRET_SUCCESS)
         {
            cmsLog_error("failed to delete created WanPppConnPortmappingObject, r2=%d", r2);
         }
   
         cmsLog_debug("Failed to set virtual server and successfully delete created WanPppConnPortmappingObject");
   
      }   
   }
   else if (ip_found == TRUE)
   {
      if ((ret = cmsObj_addInstance(MDMOID_WAN_IP_CONN_PORTMAPPING, &ipIidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not create new virtual server entry, ret=%d", ret);
         return ret;
      }
   
      if ((ret = cmsObj_get(MDMOID_WAN_IP_CONN_PORTMAPPING, &ipIidStack, 0, (void **) &ipConnPM)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to get WanIpConnPortmappingObject, ret=%d", ret);
         cmsObj_deleteInstance(MDMOID_WAN_IP_CONN_PORTMAPPING, &ipIidStack);
         return ret;
      }       
   
      if ((ret = fillVrtSrvCfg(ppp_found, srvName, srvAddr, protocol, EPS, EPE, IPS, IPE, ipConnPM)) != CMSRET_SUCCESS)
      {
         cmsObj_free((void **) &ipConnPM);
         cmsObj_deleteInstance(MDMOID_WAN_IP_CONN_PORTMAPPING, &ipIidStack);
         return ret;
      }
   
      cmsLog_debug("in ipConnPM, %s/%s/%s/%u/%u/%u/%u", 
         ipConnPM->portMappingDescription, ipConnPM->internalClient, ipConnPM->portMappingProtocol, ipConnPM->externalPort,
         ipConnPM->X_BROADCOM_COM_ExternalPortEnd, ipConnPM->internalPort, ipConnPM->X_BROADCOM_COM_InternalPortEnd);
      
      /* set and activate WanIpConnPortmappingObject */
      ret = cmsObj_set(ipConnPM, &ipIidStack);
      cmsObj_free((void **) &ipConnPM);
   
      if (ret != CMSRET_SUCCESS)
      {
         CmsRet r2;
         cmsLog_error("Failed to set WanIpConnPortmappingObject, ret = %d", ret);
          
         r2 = cmsObj_deleteInstance(MDMOID_WAN_IP_CONN_PORTMAPPING, &ipIidStack);
         if (r2 != CMSRET_SUCCESS)
         {
            cmsLog_error("failed to delete created WanIpConnPortmappingObject, r2=%d", r2);
         }
   
         cmsLog_debug("Failed to set virtual server and successfully delete created WanIpConnPortmappingObject");
   
      }
   }
   else
   {
      return CMSRET_INVALID_ARGUMENTS;
   }

   
   return ret;
}


CmsRet dalVirtualServer_deleteEntry_igd(const char * srvAddr, const char * protocol, const UINT16 EPS, const UINT16 EPE, const UINT16 IPS, const UINT16 IPE)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 ppp_found = FALSE;
   UBOOL8 ip_found = FALSE;
   WanPppConnPortmappingObject *pppConnPM = NULL;
   WanIpConnPortmappingObject *ipConnPM = NULL;

   /* deleting a new virtual server entry */
   cmsLog_debug("Deleting virtual server with %s/%s/%u/%u/%u/%u", srvAddr, protocol, EPS, EPE, IPS, IPE);

   while ((ret = cmsObj_getNext(MDMOID_WAN_PPP_CONN_PORTMAPPING, &iidStack, (void **) &pppConnPM)) == CMSRET_SUCCESS)
   {
      if(!cmsUtl_strcmp(srvAddr, pppConnPM->internalClient) && !cmsUtl_strcmp(protocol, pppConnPM->portMappingProtocol) && 
          EPS == pppConnPM->externalPort && EPE == pppConnPM->X_BROADCOM_COM_ExternalPortEnd && 
          IPS == pppConnPM->internalPort && IPE == pppConnPM->X_BROADCOM_COM_InternalPortEnd) 
      {
         ppp_found = TRUE;
         break;
      }
      else
      {
         cmsObj_free((void **) &pppConnPM);
      }
   }

   if (ppp_found == FALSE)
   {
      INIT_INSTANCE_ID_STACK(&iidStack);
      while ((ret = cmsObj_getNext(MDMOID_WAN_IP_CONN_PORTMAPPING, &iidStack, (void **) &ipConnPM)) == CMSRET_SUCCESS)
      {
         if(!cmsUtl_strcmp(srvAddr, ipConnPM->internalClient) && !cmsUtl_strcmp(protocol, ipConnPM->portMappingProtocol) && 
             EPS == ipConnPM->externalPort && EPE == ipConnPM->X_BROADCOM_COM_ExternalPortEnd && 
             IPS == ipConnPM->internalPort && IPE == ipConnPM->X_BROADCOM_COM_InternalPortEnd) 
         {
            ip_found = TRUE;
            break;
         }
         else
         {
            cmsObj_free((void **) &ipConnPM);
         }
      }
   }

   if(ppp_found == TRUE)
   {
      ret = cmsObj_deleteInstance(MDMOID_WAN_PPP_CONN_PORTMAPPING, &iidStack);
      cmsObj_free((void **) &pppConnPM);

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to delete WanPppConnPortmappingObject, ret = %d", ret);
         return ret;
      }
   }
   else if(ip_found == TRUE)
   {
      ret = cmsObj_deleteInstance(MDMOID_WAN_IP_CONN_PORTMAPPING, &iidStack);
      cmsObj_free((void **) &ipConnPM);

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to delete WanIpConnPortmappingObject, ret = %d", ret);
         return ret;
      }
   }
   else
   {
      cmsLog_debug("bad %s/%s/%u/%u/%u/%u, no virtual server entry found", srvAddr, protocol, EPS, EPE, IPS, IPE);
      return CMSRET_INVALID_ARGUMENTS;
   }

   return ret;

}


CmsRet dalDmzHost_addEntry_igd(const char *srvAddr)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   SecDmzHostCfgObject *dmzCfg = NULL;   

   /* adding a Dmz host entry */
   cmsLog_debug("Setting Dmz host with IP %s", srvAddr);

   if ((ret = cmsObj_get(MDMOID_SEC_DMZ_HOST_CFG, &iidStack, 0, (void **) &dmzCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get SecDmzHostCfgObject, ret=%d", ret);
      return ret;
   }       
   
   cmsMem_free(dmzCfg->IPAddress);
   dmzCfg->IPAddress = cmsMem_strdup(srvAddr);

   cmsLog_debug("in dmzCfg, %s", dmzCfg->IPAddress);
   
   /* set and activate SecDmzHostCfgObject */
   ret = cmsObj_set(dmzCfg, &iidStack);
   cmsObj_free((void **) &dmzCfg);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set SecDmzHostCfgObject, ret = %d", ret);
   }   

   return ret;

}


#if 0
CmsRet dalDmzHostRule(const UBOOL8 turnOn)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack1 = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack2 = EMPTY_INSTANCE_ID_STACK;   
   SecDmzHostCfgObject *dmzCfg = NULL;   
   WanPppConnObject *wan_ppp_con = NULL;
   WanIpConnObject *wan_ip_con = NULL;
   char dmzHost[BUFLEN_16], cmd[BUFLEN_128];
   char action = 'D';   // default remove dmz rule...
   char insert = 'D';
   char one = ' ';    

   /* getting a Dmz host entry */
   cmsLog_debug("Getting Dmz host info");

   if ((ret = cmsObj_get(MDMOID_SEC_DMZ_HOST_CFG, &iidStack, 0, (void **) &dmzCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get SecDmzHostCfgObject, ret=%d", ret);
      return ret;
   }  

   if (dmzCfg->IPAddress == NULL)
   {
      dmzHost[0] = '\0';   
   }
   else
   {
      strcpy(dmzHost, dmzCfg->IPAddress);
   }

   cmsObj_free((void **) &dmzCfg);  

    /* if DMZ exis */
   if ( dmzHost[0] != '\0' ) 
   {  
      /* add DMZ rule */
      if (turnOn) 
      {  
         action = 'A';
         insert = 'I';
         one = '1';
      }

      while ( (ret = cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStack1, (void **) &wan_ip_con)) == CMSRET_SUCCESS) 
      {
         if ( wan_ip_con->NATEnabled == FALSE ) 
         {
            cmsObj_free((void **) &wan_ip_con);
            continue;
         }

         if ( wan_ip_con->X_BROADCOM_COM_IfName[0] != '\0' ) 
         {
            sprintf(cmd, "iptables -t nat -%c PREROUTING -i %s -j DNAT --to-destination %s", 
               action, wan_ip_con->X_BROADCOM_COM_IfName, dmzHost);
            bcmSystem("dalDmzHostRule", cmd);
            sprintf(cmd, "iptables -%c FORWARD %c -i %s -d %s -j ACCEPT", insert, one, wan_ip_con->X_BROADCOM_COM_IfName, dmzHost);
            bcmSystem("dalDmzHostRule", cmd);	
         }
         cmsObj_free((void **) &wan_ip_con);
      }

      while ((ret = cmsObj_getNext(MDMOID_WAN_PPP_CONN, &iidStack2, (void **) &wan_ppp_con)) == CMSRET_SUCCESS)
      {
         if ( wan_ppp_con->NATEnabled == FALSE ) 
         {
            cmsObj_free((void **) &wan_ppp_con);
            continue;
         }

         if ( wan_ppp_con->X_BROADCOM_COM_IfName[0] != '\0' ) 
         {
            sprintf(cmd, "iptables -t nat -%c PREROUTING -i %s -j DNAT --to-destination %s", 
               action, wan_ppp_con->X_BROADCOM_COM_IfName, dmzHost);
            bcmSystem("dalDmzHostRule", cmd);
            sprintf(cmd, "iptables -%c FORWARD %c -i %s -d %s -j ACCEPT", insert, one, wan_ppp_con->X_BROADCOM_COM_IfName, dmzHost);
            bcmSystem("dalDmzHostRule", cmd);	
         }
         cmsObj_free((void **) &wan_ppp_con);
      }

   }

   return ret;

}
#endif


CmsRet dalGetDmzHost_igd(char *address)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   SecDmzHostCfgObject *dmzCfg = NULL; 

   if(address == NULL)
   {
      return CMSRET_INVALID_PARAM_TYPE;
   }

   if ((ret = cmsObj_get(MDMOID_SEC_DMZ_HOST_CFG, &iidStack, 0, (void **) &dmzCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get SecDmzHostCfgObject, ret=%d", ret);
      return ret;
   }  

   if(dmzCfg->IPAddress != NULL)
   {
      strcpy(address, dmzCfg->IPAddress);
   }
   else
   {
      address[0] = '\0';
   }

   cmsObj_free((void **) &dmzCfg);  

   return ret;

}


CmsRet fillVrtSrvCfg(const UBOOL8 ppp_found, const char *srvName, const char *srvAddr, const char *protocol, const UINT16 EPS, 
	                              const UINT16 EPE, const UINT16 IPS, const UINT16 IPE, void * obj)
{
   WanPppConnPortmappingObject *pppConnPM = NULL;
   WanIpConnPortmappingObject *ipConnPM = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("%u/%s/%s/%s/%u/%u/%u/%u", ppp_found, srvName, srvAddr, protocol, EPS, EPE, IPS, IPE);
   if (ppp_found == TRUE)
   {
      pppConnPM = (WanPppConnPortmappingObject *) obj;
      pppConnPM->portMappingEnabled= TRUE;

      CMSMEM_REPLACE_STRING(pppConnPM->portMappingDescription, srvName);
      CMSMEM_REPLACE_STRING(pppConnPM->internalClient, srvAddr);
      if (cmsUtl_strcmp(protocol, "1") == 0)
      {
         CMSMEM_REPLACE_STRING(pppConnPM->portMappingProtocol, MDMVS_TCP);
      }
      else if (cmsUtl_strcmp(protocol, "2") == 0)
      {
         CMSMEM_REPLACE_STRING(pppConnPM->portMappingProtocol, MDMVS_UDP);
      }
      else
      {
         CMSMEM_REPLACE_STRING(pppConnPM->portMappingProtocol, MDMVS_TCP_OR_UDP);
      }
   
      pppConnPM->externalPort = EPS;
      pppConnPM->X_BROADCOM_COM_ExternalPortEnd = EPE;
      pppConnPM->internalPort = IPS;
      pppConnPM->X_BROADCOM_COM_InternalPortEnd = IPE;   
         
      if ((pppConnPM->portMappingDescription == NULL) ||(pppConnPM->internalClient == NULL) ||(pppConnPM->portMappingProtocol == NULL))
      {
         cmsLog_error("malloc failed.");
         cmsObj_free((void **) &pppConnPM);
         return CMSRET_RESOURCE_EXCEEDED;
      }
   }
   else
   {
      ipConnPM = (WanIpConnPortmappingObject *) obj;
      ipConnPM->portMappingEnabled = TRUE;

      CMSMEM_REPLACE_STRING(ipConnPM->portMappingDescription, srvName);
      CMSMEM_REPLACE_STRING(ipConnPM->internalClient, srvAddr);
      if (cmsUtl_strcmp(protocol, "1") == 0)
      {
         CMSMEM_REPLACE_STRING(ipConnPM->portMappingProtocol, MDMVS_TCP);
      }
      else if (cmsUtl_strcmp(protocol, "2") == 0)
      {
         CMSMEM_REPLACE_STRING(ipConnPM->portMappingProtocol, MDMVS_UDP);
      }
      else
      {
         CMSMEM_REPLACE_STRING(ipConnPM->portMappingProtocol, MDMVS_TCP_OR_UDP);
      }

      ipConnPM->externalPort = EPS;
      ipConnPM->X_BROADCOM_COM_ExternalPortEnd = EPE;
      ipConnPM->internalPort = IPS;
      ipConnPM->X_BROADCOM_COM_InternalPortEnd = IPE;   

      if ((ipConnPM->portMappingDescription == NULL) ||(ipConnPM->internalClient == NULL) ||(ipConnPM->portMappingProtocol == NULL))
      {
         cmsLog_error("malloc failed.");
         cmsObj_free((void **) &ipConnPM);
         return CMSRET_RESOURCE_EXCEEDED;
      }
   }
   
   return ret;
}
