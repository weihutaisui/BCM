/***********************************************************************
 *
 * <:copyright-BRCM:2014:proprietary:standard
 * 
 *    Copyright (c) 2014 Broadcom 
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
 *
 ************************************************************************/

#include "cms_util.h"
#include "cms_msg.h"
#include "cms_cli.h"
#include "cli.h"
#include "cms_qdm.h"

#include "devctl_xtm.h"    // for PORTID_TO_PORTMASK

#ifdef DMP_DEVICE2_BASELINE_1

#ifdef SUPPORT_CLI_CMD


/***************************************************************************
// Function Name: cli_wanShowServices_dev2
// Description  : show specific or all wan connections for TR181 data model.
// Returns      : status.
****************************************************************************/
CmsRet cli_wanShowServicesSub_dev2(const char *specificVcc)
{
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   Dev2IpInterfaceObject *ipIntf=NULL;
   char linkType[BUFLEN_32] = {0};
   char vcc[BUFLEN_32] = {0};   
   char currL2IntfNameBuf[CMS_IFNAME_LENGTH] = {0};
   UBOOL8 found = FALSE;

   while (cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE,
                              &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **)&ipIntf) == CMSRET_SUCCESS)
   {
      cmsLog_debug(" ipIntf->name %s, ipIntf->X_BROADCOM_COM_Upstream %d",
                   ipIntf->name, ipIntf->X_BROADCOM_COM_Upstream);

      if (ipIntf->X_BROADCOM_COM_Upstream == FALSE)
      {
         /* skip non wan ip interfaces */
         cmsObj_free((void **)&ipIntf);  
         continue;
      }

      found = FALSE;

      if (qdmIpIntf_getLayer2IntfNameByLayer3IntfNameLocked_dev2
             (ipIntf->name, currL2IntfNameBuf) == CMSRET_SUCCESS)
      {
#ifdef DMP_DEVICE2_ETHERNETINTERFACE_1      
{
         Dev2EthernetInterfaceObject *ethIntfObj=NULL;
         InstanceIdStack ethIntfIidStack = EMPTY_INSTANCE_ID_STACK;
         while (found == FALSE &&
                cmsObj_getNextFlags(MDMOID_DEV2_ETHERNET_INTERFACE,
                                    &ethIntfIidStack,
                                    OGF_NO_VALUE_UPDATE,
                                    (void **) &ethIntfObj) == CMSRET_SUCCESS)
         {
            if (ethIntfObj->upstream == TRUE &&
                cmsUtl_strcmp(currL2IntfNameBuf, ethIntfObj->name) == 0)
            {
               // ETHWAN does not support MDMVS_IPOA nor MDMVS_PPPOA
               snprintf(linkType, BUFLEN_32-1, "%s", MDMVS_EOA);
               found = TRUE;
            }

            cmsObj_free((void **)&ethIntfObj);
         }
}
#endif /* DMP_DEVICE2_ETHERNETINTERFACE_1 */

#ifdef DMP_DEVICE2_DSL_1       
         /* Need to find out if the layer 2 interface is AMT which has vcc string */
         SINT32 vpi =0;
         SINT32 vci = 0;
         UINT32 portId = 0;  
         InstanceIdStack atmIidStack = EMPTY_INSTANCE_ID_STACK;
         Dev2AtmLinkObject *atmLinkObj = NULL;

         while (found == FALSE &&
                cmsObj_getNextFlags(MDMOID_DEV2_ATM_LINK,
                                    &atmIidStack,
                                    OGF_NO_VALUE_UPDATE,
                                    (void **)&atmLinkObj) == CMSRET_SUCCESS)
         {
            memset(vcc, 0, BUFLEN_32);
               
            if (cmsUtl_strcmp(currL2IntfNameBuf, atmLinkObj->name) == 0)
            {
               /* get vpi/vci string */
               cmsUtl_atmVpiVciStrToNum_dev2(atmLinkObj->destinationAddress, &vpi, &vci);
               portId = atmLinkObj->X_BROADCOM_COM_ATMInterfaceId;
               snprintf(vcc, BUFLEN_32-1, "%d.%d.%d", portId, vpi, vci);
               snprintf(linkType, BUFLEN_32-1, "%s", atmLinkObj->linkType);

               if (specificVcc != NULL)
               {
                  if (cmsUtl_strcmp(vcc, specificVcc) == 0)
                     found = TRUE;
               }
               else
               {
                  found = TRUE;
               }
            }

            cmsObj_free((void **)&atmLinkObj);
         }
#endif /* DMP_DEVICE2_DSL_1  */  

#ifdef DMP_DEVICE2_PTMLINK_1
         InstanceIdStack ptmIidStack = EMPTY_INSTANCE_ID_STACK;
         Dev2PtmLinkObject *ptmLinkObj = NULL;

         while (found == FALSE &&
                cmsObj_getNextFlags(MDMOID_DEV2_PTM_LINK,
                                    &ptmIidStack,
                                    OGF_NO_VALUE_UPDATE,
                                    (void **)&ptmLinkObj) == CMSRET_SUCCESS)
         {
            if (cmsUtl_strcmp(currL2IntfNameBuf, ptmLinkObj->name) == 0)
            {
               // PTM does not support MDMVS_IPOA nor MDMVS_PPPOA
               snprintf(linkType, BUFLEN_32-1, "%s", MDMVS_EOA);
               found = TRUE;
            }

            cmsObj_free((void **)&ptmLinkObj);
         }
#endif   /* DMP_DEVICE2_PTMLINK_1 */

#ifdef DMP_DEVICE2_OPTICAL_1
         InstanceIdStack optIidStack = EMPTY_INSTANCE_ID_STACK;
         OpticalInterfaceObject* optIntfObj = NULL;

         while (found == FALSE &&
                cmsObj_getNextFlags(MDMOID_OPTICAL_INTERFACE,
                                    &optIidStack,
                                    OGF_NO_VALUE_UPDATE,
                                    (void **)&optIntfObj) == CMSRET_SUCCESS)
         {
            if (cmsUtl_strcmp(currL2IntfNameBuf, optIntfObj->name) == 0)
            {
               snprintf(linkType, BUFLEN_32-1, "%s", MDMVS_EOA);
               found = TRUE;
            }

            cmsObj_free((void **)&optIntfObj);
         }
#endif   /* DMP_DEVICE2_OPTICAL_1*/
      }

      /* skip wan ip interfaces that does not match */
      if (found == FALSE)
      {
         cmsObj_free((void **)&ipIntf);  
         continue;
      }

      // VCC address
      if (vcc[0] != '\0')
      {
         printf("%s\t", vcc);
      }
      else
      {
         printf("N/A\t");         
      }

      // service name
      if (strlen(ipIntf->alias) < 8 )
         printf("%s\t\t", ipIntf->alias);
      else
         printf("%s\t", ipIntf->alias);

      // interface name
      if (strlen(ipIntf->name) < 8 )
         printf("%s\t\t", ipIntf->name);
      else
         printf("%s\t", ipIntf->name);

      // protocol
      if (cmsUtl_strcmp(linkType, MDMVS_EOA) == 0)
      {
         if (!ipIntf->X_BROADCOM_COM_BridgeService)
         {
            if (strstr(ipIntf->lowerLayers,"Device.PPP."))  // a bit of a hack, but it works
            {
               printf("PPPoE\t");
            }
            else
            {
               printf("IPoE\t");
            }
         }
         else
         {
            printf("Bridged\t");
         }
      }
      else if (cmsUtl_strcmp(linkType, MDMVS_IPOA) == 0)
      {
         printf("IPoA\t");
      }
      else if (cmsUtl_strcmp(linkType, MDMVS_PPPOA) == 0)
      {
         printf("PPPoA\t");
      }
      
      // IGMP
#ifdef DMP_X_BROADCOM_COM_IGMP_1      
      printf("%s\t", ipIntf->X_BROADCOM_COM_IGMPEnabled ? "Enable" : "Disable");
      printf("%s\t", ipIntf->X_BROADCOM_COM_IGMP_SOURCEEnabled? "Enable" : "Disable");
#else
      printf("%s\t", "Disable");
      printf("%s\t", "Disable");
#endif /* DMP_X_BROADCOM_COM_IGMP_1 */

#ifdef DMP_X_BROADCOM_COM_MLD_1
      printf("%s\t", ipIntf->X_BROADCOM_COM_MLDEnabled? "Enable" : "Disable");
      printf("%s\t", ipIntf->X_BROADCOM_COM_MLD_SOURCEEnabled? "Enable" : "Disable");
#endif
      printf("%s\t", ipIntf->X_BROADCOM_COM_IPv4ServiceStatus? ipIntf->X_BROADCOM_COM_IPv4ServiceStatus : "\t");

      // wan IP address
      if (!ipIntf->X_BROADCOM_COM_BridgeService);      
      {
         char ipAddress[CMS_IPADDR_LENGTH] = {0};
         
         if (qdmIpIntf_getIpvxAddressByNameLocked_dev2(CMS_AF_SELECT_IPV4, ipIntf->name, ipAddress) == CMSRET_SUCCESS)
         {
            printf("%s", ipAddress);
         }
      }

      printf("\n");

      cmsObj_free((void **)&ipIntf);
   }  
 
   cmsLog_debug(" Exit");

   return CMSRET_SUCCESS;
   
}


/***************************************************************************
// Function Name: cli_wanShowInterfaces_dev2
// Description  : show all wan connections for TR181 data model.
// Returns      : status.
****************************************************************************/
void cli_wanShowInterfaces_dev2(void)
{
   // print header
#ifdef DMP_X_BROADCOM_COM_MLD_1
   printf("VCC\tService\t\tInterface\t" \
          "Proto.\tIGMP\tSrc?\tMLD\tSrc?\tStatus\t\tIP\n");
   printf("\tName\t\tName\t\t\t\t\t\t\t\t\taddress\n");
#else
   printf("VCC\tService\t\tInterface\t" \
          "Proto.\tIGMP\tSrc?\tStatus\t\tIP\n");
   printf("\tName\t\tName\t\t\t\t\t\t\t\taddress\n");
#endif

   cli_wanShowServicesSub_dev2(NULL);
}


UBOOL8 cli_isValidL2IfName_dev2(const char *l2IfName)
{
   UBOOL8 found = FALSE;
   char wanL2IfName[CMS_IFNAME_LENGTH];
   char *p = NULL;

   cmsUtl_strncpy(wanL2IfName, l2IfName, CMS_IFNAME_LENGTH-1);
   if ((p = strchr(wanL2IfName, '/')) != NULL)
   {
      /* now wanL2IfName is "ptm0", "atm0" etc. */
      *p = '\0';
   }

#ifdef DMP_ETHERNETWAN_1
   InstanceIdStack iidStack;
#endif   /* DMP_ETHERNETWAN_1 */

#ifdef DMP_DEVICE2_ETHERNETINTERFACE_1
   InstanceIdStack ethIntfIidStack;
   Dev2EthernetInterfaceObject *ethIntfObj = NULL;
#endif   /* DMP_DEVICE2_ETHERNETINTERFACE_1 */

#ifdef DMP_DEVICE2_DSL_1
   InstanceIdStack atmIidStack;
   Dev2AtmLinkObject *atmLinkObj = NULL;
#endif   /* DMP_DEVICE2_DSL_1 */

#ifdef DMP_DEVICE2_PTMLINK_1
   InstanceIdStack ptmIidStack;
   Dev2PtmLinkObject *ptmLinkObj = NULL;
#endif   /* DMP_DEVICE2_PTMLINK_1 */

#ifdef DMP_DEVICE2_OPTICAL_1
   InstanceIdStack optIidStack;
   OpticalInterfaceObject* optIntfObj = NULL;
#endif   /* DMP_DEVICE2_OPTICAL_1 */

#ifdef DMP_ETHERNETWAN_1
   if (cmsUtl_strstr(wanL2IfName, ETH_IFC_STR) != NULL)
   {
      found = dalEth_getEthIntfByIfName(wanL2IfName, &iidStack, NULL);
   }
#endif   /* DMP_ETHERNETWAN_1 */

#ifdef DMP_DEVICE2_ETHERNETINTERFACE_1
   INIT_INSTANCE_ID_STACK(&ethIntfIidStack);

   while (found == FALSE &&
          cmsObj_getNextFlags(MDMOID_DEV2_ETHERNET_INTERFACE,
                              &ethIntfIidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **) &ethIntfObj) == CMSRET_SUCCESS)
   {
      if (ethIntfObj->upstream == TRUE &&
          cmsUtl_strcmp(ethIntfObj->name, wanL2IfName) == 0)
      {
         found = TRUE;
      }

      cmsObj_free((void **)&ethIntfObj);
   }
#endif   /* DMP_DEVICE2_ETHERNETINTERFACE_1 */

#ifdef DMP_DEVICE2_DSL_1
   INIT_INSTANCE_ID_STACK(&atmIidStack);

   while (found == FALSE &&
          cmsObj_getNextFlags(MDMOID_DEV2_ATM_LINK,
                              &atmIidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **)&atmLinkObj) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(atmLinkObj->name, wanL2IfName) == 0)
      {
         found = TRUE;
      }

      cmsObj_free((void **)&atmLinkObj);
   }
#endif   /* DMP_DEVICE2_DSL_1 */

#ifdef DMP_DEVICE2_PTMLINK_1
   INIT_INSTANCE_ID_STACK(&ptmIidStack);

   while (found == FALSE &&
          cmsObj_getNextFlags(MDMOID_DEV2_PTM_LINK,
                              &ptmIidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **)&ptmLinkObj) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(ptmLinkObj->name, wanL2IfName) == 0)
      {
         found = TRUE;
      }

      cmsObj_free((void **)&ptmLinkObj);
   }
#endif   /* DMP_DEVICE2_PTMLINK_1 */

#ifdef DMP_DEVICE2_OPTICAL_1
   INIT_INSTANCE_ID_STACK(&optIidStack);

   while (found == FALSE &&
          cmsObj_getNextFlags(MDMOID_OPTICAL_INTERFACE,
                              &optIidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **)&optIntfObj) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(optIntfObj->name, wanL2IfName) == 0)
      {
         found = TRUE;
      }

      cmsObj_free((void **)&optIntfObj);
   }
#endif   /* DMP_DEVICE2_OPTICAL_1*/

   return found;
}


#ifdef DMP_DEVICE2_DSL_1

UBOOL8 cli_isVccAddrExist_dev2(const WEB_NTWK_VAR *webVar)
{
   SINT32 vpi = 0, vci = 0;
   UBOOL8 atmLinkFound = FALSE;
   InstanceIdStack atmIidStack;
   Dev2AtmLinkObject *atmLinkObj = NULL;

   cmsLog_debug("looking for port %d vpi %d vci %d",
                webVar->portId, webVar->atmVpi, webVar->atmVci);

   INIT_INSTANCE_ID_STACK(&atmIidStack);

   while (atmLinkFound == FALSE &&
          cmsObj_getNextFlags(MDMOID_DEV2_ATM_LINK,
                              &atmIidStack,
                              OGF_NO_VALUE_UPDATE,
                         (void **)&atmLinkObj) == CMSRET_SUCCESS)
   {
      cmsUtl_atmVpiVciStrToNum_dev2(atmLinkObj->destinationAddress, &vpi, &vci);

      if (webVar->portId == atmLinkObj->X_BROADCOM_COM_ATMInterfaceId &&
          webVar->atmVpi == vpi &&
          webVar->atmVci == vci)
      {
         atmLinkFound = TRUE;
      }

      cmsObj_free((void **)&atmLinkObj);
   }

   return atmLinkFound;
}


#endif   /* DMP_DEVICE2_DSL_1 */

#endif   /* SUPPORT_CLI_CMD */

#endif   /* DMP_DEVICE2_BASELINE_1 */


