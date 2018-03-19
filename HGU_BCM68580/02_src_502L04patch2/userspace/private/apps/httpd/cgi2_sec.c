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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "cms.h"
#include "httpd.h"
#include "cgi_cmd.h"
#include "cgi_sec.h"
#include "cgi_main.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_dal.h"
#include "cms_qdm.h"

UBOOL8 matchFilterName(const char *filterName);

CmsRet addIpFilterInAll_dev2(const char *name, const char *ipver, const char *protocol,
                            const char *srcAddr, const char *srcMask, const char *srcPort, 
                            const char *dstAddr, const char *dstMask, const char *dstPort)
{
   /* go through all  IP.Device with firewall enable and add Firewall Exception and rule ... */
   Dev2IpInterfaceObject *ipIntfObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;


   INIT_INSTANCE_ID_STACK(&iidStack);
   while ( (cmsObj_getNext
   	  (MDMOID_DEV2_IP_INTERFACE, &iidStack, (void **) &ipIntfObj)) == CMSRET_SUCCESS) 
   {
      if(qdmIpIntf_isFirewallEnabledOnIntfnameLocked(ipIntfObj->name))
      {
   	   cmsLog_debug("adding ifName=%s", ipIntfObj->name);
   	   if ((ret = dalSec_addIpFilterIn(name, ipver, protocol, srcAddr, srcMask, srcPort, dstAddr, dstMask, dstPort, ipIntfObj->name) != CMSRET_SUCCESS))
   	   {
   		   cmsLog_error("adding fw rule on ifName=%s fail.", ipIntfObj->name);
   	   }
      }
      cmsObj_free((void **) &ipIntfObj);	   
   }

   return ret;
}


/** Create a concatenated list of ifNames which contain the specified
 *  filter name.
 *
 * @param filtername (IN) The filtername to search for.
 * @param ifNames    (OUT) The concatenated list of all ifNames
 */
void getAllIfNamesContainingFilterName_dev2(const char *filtername, char *ifNames)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack parentIidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2FirewallExceptionRuleObject *fwRuleObj=NULL;
   Dev2FirewallExceptionObject *fwExObj=NULL;   
   char ifname[BUFLEN_32];
   CmsRet ret = CMSRET_SUCCESS;



   INIT_INSTANCE_ID_STACK(&iidStack);
   while ( (ret = cmsObj_getNext
         (MDMOID_DEV2_FIREWALL_EXCEPTION_RULE, &iidStack, (void **) &fwRuleObj)) == CMSRET_SUCCESS) 
   {
      if (cmsUtl_strcmp(fwRuleObj->filterName, filtername) == 0 )
      {

		  parentIidStack=iidStack;
		  if ((ret = cmsObj_getAncestor(MDMOID_DEV2_FIREWALL_EXCEPTION, MDMOID_DEV2_FIREWALL_EXCEPTION_RULE, 
								 &parentIidStack, (void **) &fwExObj)) == CMSRET_SUCCESS)
		  {
			 if(qdmIpIntf_isFirewallEnabledOnIpIntfFullPathLocked(fwExObj->IPInterface))
			 {
                if ((ret = qdmIntf_fullPathToIntfnameLocked(fwExObj->IPInterface,ifname)) == CMSRET_SUCCESS)
                {
                 strcat(ifNames, ifname);
                 strcat(ifNames, ",");
                }
		 	 }
		     cmsObj_free((void **) &fwExObj);
	  	  }
      }

      cmsObj_free((void **) &fwRuleObj);
   }

   /* remove the last comma */
   ifNames[strlen(ifNames)-1] = '\0';

   return;
}


static void printFirewallExceptionEntry_dev2(const Dev2FirewallExceptionRuleObject *exceptionObj, FILE *fs)
{
   char ifNames[BUFLEN_256]={0};

   fprintf(fs, "   <tr>\n");
   fprintf(fs, "      <td>%s</td>\n", exceptionObj->filterName);
   getAllIfNamesContainingFilterName_dev2(exceptionObj->filterName, ifNames);
   fprintf(fs, "      <td>%s</td>\n", ifNames); 
   fprintf(fs, "      <td>%s</td>\n", exceptionObj->IPVersion);
   fprintf(fs, "      <td>%s</td>\n", exceptionObj->protocol);
   
   if ( cmsUtl_strcmp(exceptionObj->sourceIPAddress, "\0") != 0 ) 
   {
      if (strchr(exceptionObj->sourceIPAddress, ':') == NULL)
      {
         /* IPv4 address */
         if ( cmsUtl_strcmp(exceptionObj->sourceNetMask, "\0") != 0 )
         {
            fprintf(fs, "      <td>%s/%d</td>\n", exceptionObj->sourceIPAddress,
                    cmsNet_getLeftMostOneBitsInMask(exceptionObj->sourceNetMask));
         }
         else
         {
            fprintf(fs, "      <td>%s</td>\n", exceptionObj->sourceIPAddress);
         } 
      }
      else
      {
         /* IPv6 address */
         fprintf(fs, "      <td>%s</td>\n", exceptionObj->sourceIPAddress);
      }
   } 
   else
   {
      fprintf(fs, "      <td>&nbsp</td>\n");
   }

   if ( exceptionObj->sourcePortStart != 0)
   {
      if ( exceptionObj->sourcePortEnd != 0)
      {
         fprintf(fs, "      <td>%d:%d</td>\n", exceptionObj->sourcePortStart, exceptionObj->sourcePortEnd);
      }
      else
      {
         fprintf(fs, "      <td>%d</td>\n", exceptionObj->sourcePortStart);
      }
   }
   else
   {
      fprintf(fs, "      <td>&nbsp</td>\n");
   }

   if ( cmsUtl_strcmp(exceptionObj->destinationIPAddress, "\0") != 0 ) 
   {
      if (strchr(exceptionObj->destinationIPAddress, ':') == NULL)
      {
         /* IPv4 address */
         if ( cmsUtl_strcmp(exceptionObj->destinationNetMask, "\0") != 0 )
         {
            fprintf(fs, "      <td>%s/%d</td>\n", exceptionObj->destinationIPAddress,
                    cmsNet_getLeftMostOneBitsInMask(exceptionObj->destinationNetMask));
         }
         else
         {
            fprintf(fs, "      <td>%s</td>\n", exceptionObj->destinationIPAddress);
         } 
      }
      else
      {
         /* IPv6 address */
         fprintf(fs, "      <td>%s</td>\n", exceptionObj->destinationIPAddress);
      }
   } 
   else
   {
      fprintf(fs, "      <td>&nbsp</td>\n");
   }
   
   if ( exceptionObj->destinationPortStart != 0)
   {
      if ( exceptionObj->destinationPortEnd != 0)
      {
         fprintf(fs, "      <td>%d:%d</td>\n", exceptionObj->destinationPortStart, exceptionObj->destinationPortEnd);
      }
      else
      {
         fprintf(fs, "      <td>%d</td>\n", exceptionObj->destinationPortStart);
      }
   }
   else
   {
      fprintf(fs, "      <td>&nbsp</td>\n");
   }
   	  
   fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n", exceptionObj->filterName);
   fprintf(fs, "   </tr>\n");


}


void cgiScOutFltViewbody_dev2(FILE *fs) 
{
	InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
	InstanceIdStack parentIid = EMPTY_INSTANCE_ID_STACK;
    char *ipIntfFullPath=NULL;
    Dev2FirewallExceptionObject *fwExObj=NULL;	
	Dev2FirewallExceptionRuleObject *obj=NULL;
	CmsRet ret = CMSRET_SUCCESS;
	UBOOL8 found=FALSE;

    /* Outgoing rule just support on br0 on webGUI now , may extend later */
	if(qdmIntf_intfnameToFullPathLocked_dev2("br0",FALSE,&ipIntfFullPath) == CMSRET_SUCCESS)
	{
		cmsLog_debug("entered, ipIntfFullPath=%s", ipIntfFullPath);
		 
		INIT_INSTANCE_ID_STACK(&parentIid);
		while (!found && cmsObj_getNext(MDMOID_DEV2_FIREWALL_EXCEPTION, &parentIid, (void **) &fwExObj) == CMSRET_SUCCESS)
		{
		   if (cmsUtl_strcmp(fwExObj->IPInterface, ipIntfFullPath) == 0 )
		   {
			   found=TRUE;
			   ret = CMSRET_SUCCESS;		   
			   break;
		   }
		   cmsObj_free((void **)&fwExObj);
		}
		
		CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);
	}

	if(found)
	{
	   INIT_INSTANCE_ID_STACK(&iidStack);
       while  ((ret = cmsObj_getNextInSubTreeFlags
       (MDMOID_DEV2_FIREWALL_EXCEPTION_RULE, &parentIid ,&iidStack, OGF_NO_VALUE_UPDATE, (void **)&obj)) == CMSRET_SUCCESS)
       {
          if (obj->enable && (cmsUtl_strcmp(obj->target, MDMVS_DROP) == 0 ))	   
          {
             fprintf(fs, "   <tr>\n");
             fprintf(fs, "      <td>%s</td>\n", obj->filterName);
             fprintf(fs, "      <td>%s</td>\n", obj->IPVersion);
             fprintf(fs, "      <td>%s</td>\n", obj->protocol);
             
             if ( cmsUtl_strcmp(obj->sourceIPAddress, "\0") != 0 ) 
             {
             if (strchr(obj->sourceIPAddress, ':') == NULL)
             {
                /* IPv4 address */
                if ( cmsUtl_strcmp(obj->sourceNetMask, "\0") != 0 )
                {
                   fprintf(fs, "      <td>%s/%d</td>\n", obj->sourceIPAddress,
                           cmsNet_getLeftMostOneBitsInMask(obj->sourceNetMask));
                }
                else
                {
                   fprintf(fs, "      <td>%s</td>\n", obj->sourceIPAddress);
                } 
             }
             else
             {
                /* IPv6 address */
                fprintf(fs, "      <td>%s</td>\n", obj->sourceIPAddress);
             }
             } 
             else
             {
             fprintf(fs, "      <td>&nbsp</td>\n");
             }
             
             if ( obj->sourcePortStart != 0)
             {
             if ( obj->sourcePortEnd != 0)
             {
                fprintf(fs, "      <td>%d:%d</td>\n", obj->sourcePortStart, obj->sourcePortEnd);
             }
             else
             {
                fprintf(fs, "      <td>%d</td>\n", obj->sourcePortStart);
             }
             }
             else
             {
             fprintf(fs, "      <td>&nbsp</td>\n");
             }
             
             if ( cmsUtl_strcmp(obj->destinationIPAddress, "\0") != 0 ) 
             {
             if (strchr(obj->destinationIPAddress, ':') == NULL)
             {
                /* IPv4 address */
                if ( cmsUtl_strcmp(obj->destinationNetMask, "\0") != 0 )
                {
                   fprintf(fs, "      <td>%s/%d</td>\n", obj->destinationIPAddress,
                           cmsNet_getLeftMostOneBitsInMask(obj->destinationNetMask));
                }
                else
                {
                   fprintf(fs, "      <td>%s</td>\n", obj->destinationIPAddress);
                } 
             }
             else
             {
                /* IPv6 address */
                fprintf(fs, "      <td>%s</td>\n", obj->destinationIPAddress);
             }
             } 
             else
             {
             fprintf(fs, "      <td>&nbsp</td>\n");
             }
             
             if ( obj->destinationPortStart != 0)
             {
             if ( obj->destinationPortEnd != 0)
             {
                fprintf(fs, "      <td>%d:%d</td>\n", obj->destinationPortStart, obj->destinationPortEnd);
             }
             else
             {
                fprintf(fs, "      <td>%d</td>\n", obj->destinationPortStart);
             }
             }
             else
             {
             fprintf(fs, "      <td>&nbsp</td>\n");
             }
             
             fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n", obj->filterName);
             fprintf(fs, "   </tr>\n");
          }
          cmsObj_free((void **)&obj);
        }
        cmsObj_free((void **)&fwExObj);	   
   }

   return;
}



void cgiScInFltViewbody_dev2(FILE *fs) 
{
	InstanceIdStack iidStack;
	Dev2FirewallExceptionRuleObject *fwRuleObj=NULL;
	CmsRet ret = CMSRET_SUCCESS;

    // write table body
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ( (ret = cmsObj_getNext
   	  (MDMOID_DEV2_FIREWALL_EXCEPTION_RULE, &iidStack, (void **) &fwRuleObj)) == CMSRET_SUCCESS) 
   {
      if (fwRuleObj->enable && !matchFilterName(fwRuleObj->filterName) && (cmsUtl_strcmp(fwRuleObj->target, MDMVS_ACCEPT) == 0 ))
      {
   	  printFirewallExceptionEntry_dev2((Dev2FirewallExceptionRuleObject *) fwRuleObj, fs);
      }
   
      cmsObj_free((void **) &fwRuleObj);
   }


}


CmsRet cgiScMacFltViewBrWanbody_dev2(FILE *fs)
{

    Dev2IpInterfaceObject *ipIntfObj = NULL;
    Dev2MacFilterObject *macFilterObj=NULL;    
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_SUCCESS;
    MdmPathDescriptor pathDesc;

    INIT_INSTANCE_ID_STACK(&iidStack);      
    while ((cmsObj_getNext(MDMOID_DEV2_IP_INTERFACE, &iidStack, (void **)&ipIntfObj)) == CMSRET_SUCCESS)
    {
      if (ipIntfObj->X_BROADCOM_COM_BridgeService)
      {

         char *ipIntfFullPath=NULL;

         INIT_PATH_DESCRIPTOR(&pathDesc);
         pathDesc.oid = MDMOID_DEV2_IP_INTERFACE;
         pathDesc.iidStack = iidStack;

         if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &ipIntfFullPath)) != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsMdm_pathDescriptorToFullPath returns error. ret=%d", ret);
            cmsObj_free((void **) &ipIntfObj);
            continue; //get next one
         }

         INIT_PATH_DESCRIPTOR(&pathDesc);
         if(qdmIpIntf_getMacFilterByFullPathLocked_dev2(ipIntfFullPath, &pathDesc)!= CMSRET_SUCCESS)
         {
            cmsLog_error("Cannot get MacFilterCfgObject!!");  
            CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);
            cmsObj_free((void **) &ipIntfObj);
            continue; //get next one            
         }

         CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);

         if(( cmsObj_get(pathDesc.oid, &(pathDesc.iidStack), OGF_NORMAL_UPDATE,(void **) &macFilterObj) != CMSRET_SUCCESS))
         {
             cmsLog_error("Cannot get MacFilterCfgObject!!");
             cmsObj_free((void **) &ipIntfObj);
             continue; //get next one
         }

         fprintf(fs, "   <tr>\n");
         fprintf(fs, "      <td>%s</td>\n", ipIntfObj->name);
         if (cmsUtl_strcmp(macFilterObj->policy, MDMVS_FORWARD) == 0)
            fprintf(fs, "      <td><b><font color='green'>%s</font></b></td>\n", macFilterObj->policy);
         else
            fprintf(fs, "      <td><b><font color='red'>%s</font></b></td>\n", macFilterObj->policy);

         fprintf(fs, "      <td align='center'><input type='checkbox' name='ChangePolicy' value='%s'></td>\n",
                        ipIntfObj->name);
         fprintf(fs, "   </tr>\n");

         cmsObj_free((void **) &macFilterObj);
      }        
      cmsObj_free((void **) &ipIntfObj);
    }

   return ret;
}


CmsRet cgiScMacFltViewCfgbody_dev2(FILE *fs)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack parentIidStack = EMPTY_INSTANCE_ID_STACK;   
   Dev2MacFilterCfgObject*macFilterCfg=NULL;    
   Dev2MacFilterObject *macFilterObj=NULL;    
   char ifName[BUFLEN_32]={0};

   INIT_INSTANCE_ID_STACK(&iidStack);      
   while ((cmsObj_getNext(MDMOID_DEV2_MAC_FILTER_CFG, &iidStack, (void **)&macFilterCfg)) == CMSRET_SUCCESS)
   {
       if ( macFilterCfg->enable == TRUE ) 
       {
          fprintf(fs, "   <tr>\n");

          parentIidStack = iidStack;
          if ((ret = cmsObj_getAncestor(MDMOID_DEV2_MAC_FILTER, MDMOID_DEV2_MAC_FILTER_CFG, 
                                                            &parentIidStack, (void **) &macFilterObj)) != CMSRET_SUCCESS)
          {
             cmsLog_error("Failed to get Dev2MacFilterObject. ret=%d", ret);
             cmsObj_free((void **) &macFilterCfg);
             return ret;
          }
          
          ifName[0]='\0';

          qdmIntf_getIntfnameFromFullPathLocked_dev2(macFilterObj->IPInterface, ifName, sizeof(ifName));
          cmsObj_free((void **) &macFilterObj);          
          
          fprintf(fs, "      <td>%s</td>\n", ifName);
          fprintf(fs, "      <td>%s</td>\n", macFilterCfg->protocol);
    
          if ( cmsUtl_strcmp(macFilterCfg->destinationMAC, "\0") != 0 ) 
          {
             fprintf(fs, "      <td>%s</td>\n", macFilterCfg->destinationMAC);
          } 
          else
          {
             fprintf(fs, "      <td>&nbsp</td>\n");
          }
    
          if ( cmsUtl_strcmp(macFilterCfg->sourceMAC, "\0") != 0 ) 
          {
             fprintf(fs, "      <td>%s</td>\n", macFilterCfg->sourceMAC);
          } 
          else
          {
             fprintf(fs, "      <td>&nbsp</td>\n");
          }
          fprintf(fs, "      <td>%s</td>\n", macFilterCfg->direction);
    
          fprintf(fs, 
             "      <td align='center'><input type='checkbox' name='rml' value='%s|%s|%s|%s|%s'></td>\n",
             macFilterCfg->protocol, macFilterCfg->destinationMAC, macFilterCfg->sourceMAC, ifName, macFilterCfg->direction);
    
          fprintf(fs, "   </tr>\n");
       }
       cmsObj_free((void **)&macFilterCfg);
    
   }

   return ret;    
}


void cgiScPrtTrgViewBody_dev2(FILE *fs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack NAT_iidStack=EMPTY_INSTANCE_ID_STACK;
   Dev2NatPortTriggeringObject *portrigObj = NULL;
   Dev2NatIntfSettingObject *natIntfObj=NULL;   
   CmsRet ret = CMSRET_SUCCESS;
   char ifName[BUFLEN_32]={0};
   
   glbEntryCt = 0;

   /* Get the objects one after another till we fail. */
   while ( (ret = cmsObj_getNext
         (MDMOID_DEV2_NAT_PORT_TRIGGERING, &iidStack, (void **) &portrigObj)) == CMSRET_SUCCESS) 
   {
      if ( portrigObj->enable == TRUE) 
      {
         memcpy(&NAT_iidStack, &iidStack, sizeof(iidStack));
         cmsObj_getAncestor(MDMOID_DEV2_NAT_INTF_SETTING, MDMOID_DEV2_NAT_PORT_TRIGGERING, 
                                            &NAT_iidStack, (void **) &natIntfObj);
         qdmIntf_getIntfnameFromFullPathLocked_dev2(natIntfObj->interface, ifName, sizeof(ifName));
         cmsObj_free((void **) &natIntfObj);
         
         fprintf(fs, "   <tr>\n");
         fprintf(fs, "      <td>%s</td>\n", portrigObj->name);
         if ( !strcmp(portrigObj->triggerProtocol, "TCP") ) 
         {
            fprintf(fs, "      <td>TCP</td>\n");
         }
         else if ( !strcmp(portrigObj->triggerProtocol, "UDP") )
         {
            fprintf(fs, "      <td>UDP</td>\n");
         }
         else
         {
            fprintf(fs, "      <td>TCP/UDP</td>\n");
         }
         fprintf(fs, "      <td>%d</td>\n", portrigObj->triggerPortStart);
         fprintf(fs, "      <td>%d</td>\n", portrigObj->triggerPortEnd);
         if ( !strcmp(portrigObj->openProtocol, "TCP") ) 
         {
            fprintf(fs, "      <td>TCP</td>\n");
         }
         else if ( !strcmp(portrigObj->openProtocol, "UDP") )
         {
            fprintf(fs, "      <td>UDP</td>\n");
         }
         else
         {
            fprintf(fs, "      <td>TCP/UDP</td>\n");
         }
         fprintf(fs, "      <td>%d</td>\n", portrigObj->openPortStart);
         fprintf(fs, "      <td>%d</td>\n", portrigObj->openPortEnd);
         fprintf(fs, "      <td>%s</td>\n", ifName);
         fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s|%s|%u|%u|%u|%u'></td>\n",
            ifName, portrigObj->triggerProtocol,
            portrigObj->triggerPortStart, portrigObj->triggerPortEnd,
            portrigObj->openPortStart, portrigObj->openPortEnd);
         fprintf(fs, "   </tr>\n");
      }
      // Free the mem allocated this object by the get API.
      cmsObj_free((void **) &portrigObj);

      glbEntryCt++;
   }
   
   fprintf(fs, "</table><br>\n");
   if (glbEntryCt >= SEC_ADD_REMOVE_ROW) {      // only display the following button 
      fprintf(fs, "<input type='button' onClick='addClick()' value='Add'>\n");
      fprintf(fs, "<input type='button' onClick='removeClick(this.form.rml)' value='Remove'>\n");
   }
   glbEntryCt = SEC_PTR_TRG_MAX - glbEntryCt;
   
}

#endif /* DMP_DEVICE2_BASELINE_1 */

