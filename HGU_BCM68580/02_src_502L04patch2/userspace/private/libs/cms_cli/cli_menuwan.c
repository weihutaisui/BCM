/***********************************************************************
 *
 *  Copyright (c) 2007  Broadcom Corporation
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


/** menu driven CLI code goes into this file */

#ifdef SUPPORT_CLI_MENU

#include "cms_util.h"
#include "cms_core.h"
#include "cms_cli.h"
#include "cli.h"

static CmsRet getPvcInfo(PWEB_NTWK_VAR pInfo);
static CmsRet getWanInfo(PWEB_NTWK_VAR pInfo);
static CmsRet getIpoaInfo(PWEB_NTWK_VAR pInfo);
static CmsRet getPppInfo(PWEB_NTWK_VAR pInfo);
static CmsRet getIPoEInfo(PWEB_NTWK_VAR pInfo);
static CmsRet getBridgeInfo(PWEB_NTWK_VAR pInfo);
static CmsRet getFullconeInfo(PWEB_NTWK_VAR pInfo);
static UBOOL8 isValidCategory(char *category);
static UBOOL8 isValidProtocol(char *protocol);
static UBOOL8 isValidEncapsulation(char *encap);
static UBOOL8 isValidAuthMethod(char *method);


CmsRet menuWanCfg(void)
{
   CmsRet ret;
   WEB_NTWK_VAR wanInfo;

   memset(&wanInfo, 0, sizeof(WEB_NTWK_VAR));

   printf("\r\n");

   /* retrieve PVC info from user */
   if ((ret = getPvcInfo(&wanInfo)) == CMSRET_SUCCESS)
   {
      /* retrieve WAN info from user */
      if ((ret = getWanInfo(&wanInfo)) == CMSRET_SUCCESS)
      {
         switch (wanInfo.ntwkPrtcl)
         {
         case PROTO_PPPOE:
         case PROTO_PPPOA:
            ret = getPppInfo(&wanInfo);
            break;
         case PROTO_MER:
#if SUPPORT_ETHWAN
         case PROTO_IPOWAN:
#endif
            ret = getIPoEInfo(&wanInfo);
            break;
         case PROTO_IPOA:
            ret = getIpoaInfo(&wanInfo);
            break;
         case PROTO_BRIDGE:
            ret = getBridgeInfo(&wanInfo);
            break;
         }
         if (ret == CMSRET_SUCCESS)
         {
            if ((ret = cmsLck_acquireLockWithTimeout(CLI_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
            {
               cmsLog_error("failed to get lock, ret=%d", ret);
               return ret;
            }

            if (wanInfo.serviceId == 0)
            {
               if ((ret = dalWan_addInterface(&wanInfo)) != CMSRET_SUCCESS)
               {
                  cmsLog_error("dalWan_addInterface failed, ret=%d", ret);
               }
            }
            else 
            {
               if ((ret = dalWan_editInterface(&wanInfo)) != CMSRET_SUCCESS)
               {
                  cmsLog_error("dalWan_editInterface failed, ret=%d", ret);
               }
            }
            
            cmsLck_releaseLock();
#if 0
            switch ( ret )
            {
               case DB_WAN_ADD_OUT_OF_PVC:
                  printf("\nCannot add new WAN connection since system is run out of PVC.\n");
                  break;
               case DB_WAN_ADD_OUT_OF_QUEUES:
                  printf("\nCannot add new WAN connection since system is run out of PVC queue.\n");
                  break;
               case DB_WAN_ADD_OUT_OF_CONNECTION:
                  printf("\nCannot add new WAN connection since PVC is run out of WAN connection.\n");
                  break;
               case DB_WAN_ADD_OUT_OF_MEMORY:
                  printf("\nCannot add new WAN connection since system is run out of memory.\n");
                  break;
            }
#endif
         }
      }
   }

   if (ret == CMSRET_SUCCESS)
   {
      printf("\nPVC (%d/%d) is configured successfully.\n", wanInfo.atmVpi, wanInfo.atmVci);
      
      /* Set flag to save the config. */
      cliMenuSaveNeeded = TRUE;
   }
   
   return ret;

}  /* End of menuWanCfg() */

/***************************************************************************
// Function Name: menuWanDelete
// Description  : remove the specific wan connection.
// Returns      : status.
****************************************************************************/
CmsRet menuWanDelete(void)
{
   PARAMS_SETTING pvcCfgParams[] =
   {
      // prompt name      Default   validation function
      {"VPI [0-255]",     "0",      cli_isValidVpi},
      {"VCI [32-65535]",  "35",     cli_isValidVci},
      {"WAN ID [1-8]",    "1",      cli_isValidWanId},
      {NULL, "", NULL}
   };
   InstanceIdStack iidStack;
   WEB_NTWK_VAR wanInfo;
   CmsRet ret;

   memset(&wanInfo, 0, sizeof(WEB_NTWK_VAR));

   printf("\r\n");

   /* get input from user */
   if ((ret = cli_getMenuParameters(pvcCfgParams, TRUE)) != CMSRET_SUCCESS)
   {
      printf("\nError in getting information for deleting WAN configuration.\n");
   }
   else
   {
      /* copy all the new values from pvcCfgParams to wanInfo */
      wanInfo.atmVpi    = atoi(pvcCfgParams[0].param);
      wanInfo.atmVci    = atoi(pvcCfgParams[1].param);
      wanInfo.serviceId = atoi(pvcCfgParams[2].param);

      if ((ret = cmsLck_acquireLockWithTimeout(CLI_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get lock, ret=%d", ret);
         printf("Could not run command due to lock failure.\r\n");
         return ret;
      }

      if (dalWan_getIpConn(&wanInfo, &iidStack, NULL)  == TRUE ||
          dalWan_getPppConn(&wanInfo, &iidStack, NULL) == TRUE)
      {
         if ((ret = dalWan_deleteService(&wanInfo)) != CMSRET_SUCCESS)
         {
            printf("\nFailed to delete WAN connection (%d/%d/%d).\n",
                   wanInfo.atmVpi, wanInfo.atmVci, wanInfo.serviceId);
            cmsLog_error("dalWan_deleteService failed for %d/%d/%d (ret=%d)",
                         wanInfo.atmVpi, wanInfo.atmVci, wanInfo.serviceId, ret);
         }      
      }
      else
      {
         printf("\nCannot delete WAN connection. WAN (%d/%d/%d) does not exist.\n",
                wanInfo.atmVpi, wanInfo.atmVci, wanInfo.serviceId);
         ret = CMSRET_INVALID_ARGUMENTS;
      }
      
      cmsLck_releaseLock();
   }

   if (ret == CMSRET_SUCCESS)
   {
      printf("\nPVC (%d/%d/%d) is deleted successfully.\n", wanInfo.atmVpi, wanInfo.atmVci, wanInfo.serviceId);

      /* Set flag to save the config. */
      cliMenuSaveNeeded = TRUE;
   }

   return CMSRET_SUCCESS;

}  /* End of menuWanDelete() */

/***************************************************************************
// Function Name: menuWanShow
// Description  : show all wan connection.
// Returns      : status.
****************************************************************************/
CmsRet menuWanShow(void)
{
   return cli_wanShowServices(NULL);
}

/***************************************************************************
// Function Name: getPvcInfo
// Description  : retrieve PVC configuration from user.
// Parameters   : pInfo -- pointer to wan information.
// Returns      : status.
****************************************************************************/
CmsRet getPvcInfo(PWEB_NTWK_VAR pInfo)
{
   PARAMS_SETTING pvcCfgParams[] =
   {
      /* prompt name                                     Default   validation function */
      {"VPI [0-255]",                                       "0",  cli_isValidVpi},
      {"VCI [32-65535]",                                    "35", cli_isValidVci},
      {"WAN ID [0-8]",                                      "0",  cli_isValidWanId},
      {"Category [1-ubr,2-ubrpcr,3-cbr,4-rtvbr,5-nrtvbr]",  "1",  isValidCategory},
      {NULL, "", NULL}
   };
   PARAMS_SETTING cbrCfgParams[] =
   {
      {"Peek Cell Rate [cells/s]", "", cli_isNumber},
      {NULL, "", NULL}
   };
   PARAMS_SETTING vbrCfgParams[] =
   {
      {"Peek Cell Rate [cells/s]",        "", cli_isNumber},
      {"Sustainable Cell Rate [cells/s]", "", cli_isNumber},
      {"Maximum Burst Size [cells/s]",    "", cli_isNumber},
      {NULL, "", NULL}
   };
   CmsRet ret;
   int pcrMax = 0;
   InstanceIdStack iidStack;

   /* get input from user */
   if ((ret = cli_getMenuParameters(pvcCfgParams, TRUE)) != CMSRET_SUCCESS)
   {
      printf("\nError getting PVC config info.\n");
      return ret;
   }

   /* copy all the new values from pvcCfgParams to pInfo */
   pInfo->atmVpi    = atoi(pvcCfgParams[0].param);
   pInfo->atmVci    = atoi(pvcCfgParams[1].param);
   pInfo->serviceId = atoi(pvcCfgParams[2].param);

   if (pInfo->serviceId != 0)
   {
      /* want to edit the existing wan connection on this pvc */

      if ((ret = cmsLck_acquireLockWithTimeout(CLI_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get lock, ret=%d", ret);
         printf("Could not run command due to lock failure.\r\n");
         return ret;
      }

      if (dalWan_getDslLinkCfg(pInfo, &iidStack, NULL) == TRUE)
      {
         if (dalWan_getIpConn(pInfo, &iidStack, NULL)  == FALSE &&
             dalWan_getPppConn(pInfo, &iidStack, NULL) == FALSE)
         {
            printf("\nConnection id not found on the existing PVC.\n");
            ret = CMSRET_INVALID_ARGUMENTS;
         }
      }
      else
      {
         printf("\nThe PVC does not exist. connection id not found.\n");
         ret = CMSRET_INVALID_ARGUMENTS;
      }

      cmsLck_releaseLock();
   }
   if (ret != CMSRET_SUCCESS)
   {
      return ret;
   }

#if 0 /* TODO: get ADSL line rate */
   ADSL_CONNECTION_INFO adslInfo;
   BcmAdslCtl_GetConnectionInfo(&adslInfo);
   if ( adslInfo.LinkState == BCM_ADSL_LINK_UP) {
      if ( adslInfo.ulInterleavedUpStreamRate != 0 )
         pcrMax = adslInfo.ulInterleavedUpStreamRate  / (53 * 8);
      else
         pcrMax = adslInfo.ulFastUpStreamRate  / (53 * 8);
   }
   else
#endif
      pcrMax = 1887;   // 1887 * 53 * 8 = 800000

   if (strcasecmp(pvcCfgParams[3].param, "2") == 0 ||
       strcasecmp(pvcCfgParams[3].param, "3") == 0)
   {
      if ((ret = cli_getMenuParameters(cbrCfgParams, FALSE)) != CMSRET_SUCCESS)
      {
         printf("\nError getting CBR config info.\n");
         return ret;
      }

      if (strcasecmp(pvcCfgParams[3].param, "2") == 0)
      {
         strcpy(pInfo->atmServiceCategory, MDMVS_UBR_PLUS);
      }
      else
      {
         strcpy(pInfo->atmServiceCategory, MDMVS_CBR);
      }

      pInfo->atmPeakCellRate = atoi(cbrCfgParams[0].param);
      if (pInfo->atmPeakCellRate <= 0 || pInfo->atmPeakCellRate > pcrMax)
      {
         printf("\nPeak Cell Rate \"%d\" is out of range [1-%d].\n",
                pInfo->atmPeakCellRate, pcrMax);
         return CMSRET_INVALID_ARGUMENTS;
      }
   }
   else if (strcasecmp(pvcCfgParams[3].param, "4") == 0 ||
            strcasecmp(pvcCfgParams[3].param, "5") == 0)
   {
      if ((ret = cli_getMenuParameters(vbrCfgParams, FALSE)) != CMSRET_SUCCESS)
      {
         printf("\nError getting VBR config info.\n");
         return ret;
      }

      if (strcasecmp(pvcCfgParams[3].param, "4") == 0)
      {
         strcpy(pInfo->atmServiceCategory, MDMVS_VBR_RT);
      }
      else
      {
         strcpy(pInfo->atmServiceCategory, MDMVS_VBR_NRT);
      }

      pInfo->atmPeakCellRate      = atoi(vbrCfgParams[0].param);
      pInfo->atmSustainedCellRate = atoi(vbrCfgParams[1].param);
      pInfo->atmMaxBurstSize      = atoi(vbrCfgParams[2].param);
      if (pInfo->atmPeakCellRate <= 0 || pInfo->atmPeakCellRate > pcrMax)
      {
         printf("\nPeak Cell Rate \"%d\" is out of range [1-%d].\n",
                pInfo->atmPeakCellRate, pcrMax);
         return CMSRET_INVALID_ARGUMENTS;
      }
      if (pInfo->atmSustainedCellRate <= 0 || pInfo->atmSustainedCellRate > pcrMax)
      {
         printf("\nSustained Cell Rate \"%d\" is out of range [1-%d].\n",
                pInfo->atmSustainedCellRate, pcrMax);
         return CMSRET_INVALID_ARGUMENTS;
      }
      if (pInfo->atmSustainedCellRate > pInfo->atmPeakCellRate)
      {
         printf("\nSustained Cell Rate has to be smaller than Peak Cell Rate.\n");
         return CMSRET_INVALID_ARGUMENTS;
      }
      if (pInfo->atmMaxBurstSize <= 0 || pInfo->atmMaxBurstSize > 1000000)
      {
         printf("\nMax Burst Size \"%d\" is out of range [1-1000000].\n",
                pInfo->atmMaxBurstSize);
         return CMSRET_INVALID_ARGUMENTS;
      }
   }
   else
   {
      strcpy(pInfo->atmServiceCategory, MDMVS_UBR);
   }

   return CMSRET_SUCCESS;

}  /* End of getPvcInfo() */

/***************************************************************************
// Function Name: getWanInfo
// Description  : retrieve WAN configuration from user.
// Parameters   : pInfo -- pointer to wan information.
// Returns      : status.
****************************************************************************/
CmsRet getWanInfo(PWEB_NTWK_VAR pInfo)
{
   PARAMS_SETTING wanCfgParams[] =
   {
      // prompt name                                     Default   validation function
      {"Protocol [1-pppoa,2-pppoe,3-ipoe,4-ipoa,5-bridge]", "1",   isValidProtocol},
      {"Encapsulation [1-llc,2-vcmux]",                    "1",   isValidEncapsulation},
      {"State [1-enable,2-disable]",                       "1",   cli_isValidState},
      {"Service name",                                     "",    NULL},
      {NULL, "", NULL}
   };
   CmsRet ret;
   InstanceIdStack iidStack;
   WanDslLinkCfgObject *dslLinkCfg;

   /* get input from user */
   if ((ret = cli_getMenuParameters(wanCfgParams, FALSE)) != CMSRET_SUCCESS)
   {
      printf("\nError in getting information for WAN configuration.\n");
      return ret;
   }

   /* protocol */
   if (strcasecmp(wanCfgParams[0].param, "1") == 0)
   {
      pInfo->ntwkPrtcl = PROTO_PPPOA;
   }
   else if (strcasecmp(wanCfgParams[0].param, "2") == 0)
   {
      pInfo->ntwkPrtcl = PROTO_PPPOE;
   }
   else if (strcasecmp(wanCfgParams[0].param, "3") == 0)
   {
      pInfo->ntwkPrtcl = PROTO_MER;
   }
   else if (strcasecmp(wanCfgParams[0].param, "4") == 0)
   {
      pInfo->ntwkPrtcl = PROTO_IPOA;
   }
   else
   {
      pInfo->ntwkPrtcl = PROTO_BRIDGE;
   }

   if ((ret = cmsLck_acquireLockWithTimeout(CLI_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      printf("Could not run command due to lock failure.\r\n");
      return ret;
   }

   /* if PVC exists, then get its info */
   if (dalWan_getDslLinkCfg(pInfo, &iidStack, &dslLinkCfg) == TRUE)
   {
      if (pInfo->serviceId == 0)
      {
         /* want to add a new wan connection on this pvc */
         if (strcmp(dslLinkCfg->linkType, MDMVS_EOA) == 0)
         {
            /* For EoA linktype, only PPPoE or IPoE or Bridge connections are allowed. */
            if (pInfo->ntwkPrtcl != PROTO_PPPOE &&
                pInfo->ntwkPrtcl != PROTO_MER &&
                pInfo->ntwkPrtcl != PROTO_BRIDGE)
            {
               printf("\nOnly allow PPPoE or IPoE or Bridge on this existing PVC linktype.\n");
               ret = CMSRET_INVALID_ARGUMENTS;
            }
         }
         else if (strcmp(dslLinkCfg->linkType, MDMVS_PPPOA) == 0)
         {
            /* For PPPoA linktype, only PPPoA connection can be configured. */
            if (pInfo->ntwkPrtcl != PROTO_PPPOA)
            {
               printf("\nOnly allow PPPoA on this existing PVC linktype.\n");
               ret = CMSRET_INVALID_ARGUMENTS;
            }
         }
         else if (strcmp(dslLinkCfg->linkType, MDMVS_IPOA) == 0)
         {
            /* For IPoA linktype, only IPoA connection can be configured. */
            if (pInfo->ntwkPrtcl != PROTO_IPOA)
            {
               printf("\nOnly allow IPoA on this existing PVC linktype.\n");
               ret = CMSRET_INVALID_ARGUMENTS;
            }
         }
      }
      else
      {
         /* want to edit the existing wan connection on this pvc */
         InstanceIdStack iidStackConn;

         if (strcmp(dslLinkCfg->linkType, MDMVS_EOA) == 0)
         {
            if (delWan_getAnotherIpConn(pInfo->serviceId, &iidStack, &iidStackConn, NULL)  == TRUE ||
                delWan_getAnotherPppConn(pInfo->serviceId, &iidStack, &iidStackConn, NULL) == TRUE)
            {
               /* For EoA linktype, only PPPoE or IPoE or Bridge connections are allowed. */
               if (pInfo->ntwkPrtcl != PROTO_PPPOE &&
                   pInfo->ntwkPrtcl != PROTO_MER &&
                   pInfo->ntwkPrtcl != PROTO_BRIDGE)
               {
                  printf("\nOnly allow PPPoE or IPoE or Bridge on this existing PVC linktype.\n");
                  ret = CMSRET_INVALID_ARGUMENTS;
               }
            }
         }
         else if (strcmp(dslLinkCfg->linkType, MDMVS_PPPOA) == 0)
         {
            /* For PPPoA linktype, only PPPoA connection can be configured. */
            if (delWan_getAnotherPppConn(pInfo->serviceId, &iidStack, &iidStackConn, NULL) == TRUE)
            {
               if (pInfo->ntwkPrtcl != PROTO_PPPOA)
               {
                  printf("\nOnly allow PPPoA on this existing PVC linktype.\n");
                  ret = CMSRET_INVALID_ARGUMENTS;
               }
            }
         }
         else if (strcmp(dslLinkCfg->linkType, MDMVS_IPOA) == 0)
         {
            /* For IPoA linktype, only IPoA connection can be configured. */
            if (delWan_getAnotherIpConn(pInfo->serviceId, &iidStack, &iidStackConn, NULL) == TRUE)
            {
               if (pInfo->ntwkPrtcl != PROTO_IPOA)
               {
                  printf("\nOnly allow IPoA on this existing PVC linktype.\n");
                  ret = CMSRET_INVALID_ARGUMENTS;
               }
            }
         }
      }

      cmsObj_free((void **)&dslLinkCfg);
   }

   cmsLck_releaseLock();
   
   if (ret != CMSRET_SUCCESS)
   {
      return ret;
   }
      
   /* encapsulation mode */
   if (pInfo->ntwkPrtcl == PROTO_PPPOA)
   {
      if (strcasecmp(wanCfgParams[1].param, "1") == 0)
      {
         pInfo->encapMode = 1;
      }
      else  /* 2- vcmux */
      {
         /* for pppoa, the default encap mode is vcmux.
          * therefore set it to zero.
          */
         pInfo->encapMode = 0;
      }
   }
   else
   {
      if (strcasecmp(wanCfgParams[1].param, "2") == 0)
      {
         pInfo->encapMode = 1;
      }
      else  /* 1- llc */
      {
         /* for other type of connections, the default encap mode is llc.
          * therefore set it to zero.
          */
         pInfo->encapMode = 0;
      }
   }

   /* state */
   if (strcasecmp(wanCfgParams[2].param, "1") == 0)
   {
      pInfo->enblService = TRUE;
   }
   else
   {
      pInfo->enblService = FALSE;
   }

   /* service name */
   if (strlen(wanCfgParams[3].param) > 0)
   {
      strcpy(pInfo->serviceName, wanCfgParams[3].param);
   }

   return CMSRET_SUCCESS;

}  /* End of getWanInfo() */

/***************************************************************************
// Function Name: getPppInfo
// Description  : retrieve PPP configuration from user.
// Parameters   : pInfo -- pointer to wan information.
// Returns      : status.
****************************************************************************/
CmsRet getPppInfo(PWEB_NTWK_VAR pInfo)
{
   PARAMS_SETTING pppCfgParams[] =
   {
      {"User name",                                            "", cli_isValidPppUserName},
      {"Password",                                             "", cli_isValidPppPassword},
      {"Idle timeout (minutes) [0-1090]",                      "0", cli_isValidIdleTimeout},
      {"Authentication method [1-auto,2-pap,3-chap,4-mschap]", "1", isValidAuthMethod},
      {"IGMP [1-enable,2-disable]",                            "2", cli_isValidState},
      {"QoS [1-enable,2-disable]",                             "2", cli_isValidState},
      {"PPP IP address [1-enable,2-disable]",                  "2", cli_isValidState},
      {NULL, "", NULL}
   };
   PARAMS_SETTING pppoeCfgParams[] =
   {
      {"PPPoE service name", "", cli_isValidPppPassword},
      {NULL, "",   NULL}
   };
   PARAMS_SETTING pppIpAddrCfgParams[] =
   {
      {"IP address",  "", cli_isIpAddress},
      {NULL, "",   NULL}
   };
   CmsRet ret;

   if ((ret = cli_getMenuParameters(pppCfgParams, FALSE)) != CMSRET_SUCCESS)
   {
      printf("\nError in getting information for WAN configuration.\n");
      return ret;
   }

   /* ppp local IP address */
   if (strcasecmp(pppCfgParams[6].param, "1") == 0)
   {
      pInfo->useStaticIpAddress = TRUE;
      if ((ret = cli_getMenuParameters(pppIpAddrCfgParams, FALSE)) != CMSRET_SUCCESS)
      {
         printf("\nError in getting information for WAN configuration.\n");
         return ret;
      }
      strcpy(pInfo->pppLocalIpAddress, pppIpAddrCfgParams[0].param);
   }
   else
   {
      pInfo->useStaticIpAddress = FALSE;
   }

   /* ppp user name */
   strcpy(pInfo->pppUserName, pppCfgParams[0].param);
   /* ppp password */
   strcpy(pInfo->pppPassword, pppCfgParams[1].param);
   /* ppp idle timeout */
   pInfo->pppTimeOut = atoi(pppCfgParams[2].param);
   if (pInfo->pppTimeOut > 0)
   {
      pInfo->enblOnDemand = TRUE;
   }
   else
   {
      pInfo->enblOnDemand = FALSE;
   }
   /* ppp authentication method */
   if (strcasecmp(pppCfgParams[3].param, "2") == 0)
   {
      pInfo->pppAuthMethod = PPP_AUTH_METHOD_PAP;
   }
   else if (strcasecmp(pppCfgParams[3].param, "3") == 0)
   {
      pInfo->pppAuthMethod = PPP_AUTH_METHOD_CHAP;
   }
   else if (strcasecmp(pppCfgParams[3].param, "4") == 0)
   {
      pInfo->pppAuthMethod = PPP_AUTH_METHOD_MSCHAP;
   }
   else
   {
      pInfo->pppAuthMethod = PPP_AUTH_METHOD_AUTO;
   }
   /* initialize wan IP to 0.0.0.0 */
   strcpy(pInfo->wanIpAddress, "0.0.0.0");
   /* enable Firewall */
   pInfo->enblFirewall = TRUE;
   /* enable NAT */
   pInfo->enblNat = TRUE;
   /* disable Full Cone NAT */
   pInfo->enblFullcone = FALSE;
   /* enable IGMP */
   if (strcasecmp(pppCfgParams[4].param, "1") == 0)
   {
      pInfo->enblIgmp = TRUE;
   }
   else
   {
      pInfo->enblIgmp = FALSE;
   }
   /* enable QoS */
   if (strcasecmp(pppCfgParams[5].param, "1") == 0)
   {
      if ((ret = cli_checkQosQueueResources(pInfo)) == CMSRET_SUCCESS)
      {
         pInfo->enblQos = TRUE;
      }
      else
      {
         return ret;
      }
   }
   else
   {
      pInfo->enblQos = FALSE;
   }

   if (pInfo->ntwkPrtcl == PROTO_PPPOE)
   {
      if ((ret = cli_getMenuParameters(pppoeCfgParams, FALSE)) != CMSRET_SUCCESS)
      {
         printf("\nError in getting information for PPPoE service name.\n");
         return ret;
      }
      /* pppoe service name */
      strcpy(pInfo->pppServerName, pppoeCfgParams[0].param);
   }
   
   return CMSRET_SUCCESS;

}  /* End of getPppInfo() */

/***************************************************************************
// Function Name: getIPoEInfo
// Description  : retrieve  IPoE  configuration from user.
// Parameters   : pInfo -- pointer to wan information.
// Returns      : status.
****************************************************************************/
CmsRet getIPoEInfo(PWEB_NTWK_VAR pInfo)
{
   PARAMS_SETTING dhcpCfgParams[] =
   {
      {"DHCP Client [1-enable,2-disable]", "1", cli_isValidState},
      {NULL, "", NULL}
   };
   PARAMS_SETTING ipCfgParams[] =
   {
      {"WAN IP address",  "", cli_isIpAddress},
      {"WAN subnet mask", "", cli_isIpAddress},
      {NULL, "", NULL}
   };
   PARAMS_SETTING wanCfgParams[] =
   {
      {"Firewall [1-enable,2-disable]", "1", cli_isValidState},
      {"NAT [1-enable,2-disable]",      "1", cli_isValidState},
      {"IGMP [1-enable,2-disable]",     "2", cli_isValidState},
      {"QoS [1-enable,2-disable]",      "2", cli_isValidState},
      {NULL, "", NULL}
   };
   CmsRet ret;

   /* DHCP Client */
   if ((ret = cli_getMenuParameters(dhcpCfgParams, FALSE)) != CMSRET_SUCCESS)
   {
      printf("\nError in getting information for WAN configuration.\n");
      return ret;
   }
   if ( strcasecmp(dhcpCfgParams[0].param, "1") == 0 )
   {
      pInfo->enblDhcpClnt = TRUE;
      /* initialize wan IP and subnet mask to 0.0.0.0 */
      strcpy(pInfo->wanIpAddress, "0.0.0.0");
      strcpy(pInfo->wanSubnetMask, "0.0.0.0");
   }
   else
   {
      pInfo->enblDhcpClnt = FALSE;
      if ((ret = cli_getMenuParameters(ipCfgParams, FALSE)) != CMSRET_SUCCESS)
      {
         printf("\nError in getting information for WAN configuration.\n");
         return ret;
      }
      /* WAN IP address */
      strcpy(pInfo->wanIpAddress, ipCfgParams[0].param);
      /* WAN subnet mask */
      strcpy(pInfo->wanSubnetMask, ipCfgParams[1].param);
   }

   /* initialize default gateway to 0.0.0.0 */
   strcpy(pInfo->defaultGatewayList, "0.0.0.0");
   
   if ((ret = cli_getMenuParameters(wanCfgParams, FALSE)) != CMSRET_SUCCESS)
   {
      printf("\nError in getting information for WAN configuration.\n");
      return ret;
   }
   /* enable Firewall */
   if ( strcasecmp(wanCfgParams[0].param, "1") == 0 )
   {
      pInfo->enblFirewall = TRUE;
   }
   else
   {
      pInfo->enblFirewall = FALSE;
   }
   /* enable NAT */
   if ( strcasecmp(wanCfgParams[1].param, "1") == 0 )
   {
      pInfo->enblNat = TRUE;
      if ((ret = getFullconeInfo(pInfo)) != CMSRET_SUCCESS) {
	      printf("\nError in getting information for Full Cone NAT "
		     "configuration.\n");
	      return ret;
      }
   }
   else
   {
      pInfo->enblNat = FALSE;
   }
   /* enable IGMP */
   if ( strcasecmp(wanCfgParams[2].param, "1") == 0 )
   {
      pInfo->enblIgmp = TRUE;
   }
   else
   {
      pInfo->enblIgmp = FALSE;
   }
   /* enable QoS */
   if ( strcasecmp(wanCfgParams[3].param, "1") == 0 )
   {
      if ((ret = cli_checkQosQueueResources(pInfo)) == CMSRET_SUCCESS)
      {
         pInfo->enblQos = TRUE;
      }
   }
   else
   {
      pInfo->enblQos = FALSE;
   }

   return ret;

}  /* End of getIPoEInfo() */

/***************************************************************************
// Function Name: getIpoaInfo
// Description  : retrieve IPoA configuration from user.
// Parameters   : pInfo -- pointer to wan information.
// Returns      : status.
****************************************************************************/
CmsRet getIpoaInfo(PWEB_NTWK_VAR pInfo)
{
   PARAMS_SETTING ipoaCfgParams[] =
   {
      {"WAN IP address",                "",  cli_isIpAddress},
      {"WAN subnet mask",               "",  cli_isIpAddress},
      {"Firewall [1-enable,2-disable]", "1", cli_isValidState},
      {"NAT [1-enable,2-disable]",      "1", cli_isValidState},
      {"IGMP [1-enable,2-disable]",     "2", cli_isValidState},
      {"QoS [1-enable,2-disable]",      "2", cli_isValidState},
      {NULL, "", NULL}
   };
   CmsRet ret;

   if ((ret = cli_getMenuParameters(ipoaCfgParams, FALSE)) != CMSRET_SUCCESS)
   {
      printf("\nError in getting information for WAN configuration.\n");
      return ret;
   }
   /* WAN IP address */
   strcpy(pInfo->wanIpAddress, ipoaCfgParams[0].param);
   /* WAN subnet mask */
   strcpy(pInfo->wanSubnetMask, ipoaCfgParams[1].param);
   /* initialize default gateway to 0.0.0.0 */
   strcpy(pInfo->defaultGatewayList, "0.0.0.0");
   /* enable Firewall */
   if ( strcasecmp(ipoaCfgParams[2].param, "1") == 0 )
   {
      pInfo->enblFirewall = TRUE;
   }
   else
   {
      pInfo->enblFirewall = FALSE;
   }
   /* enable NAT */
   if ( strcasecmp(ipoaCfgParams[3].param, "1") == 0 )
   {
      pInfo->enblNat = TRUE;
      if ((ret = getFullconeInfo(pInfo)) != CMSRET_SUCCESS) {
	      printf("\nError in getting information for Full Cone NAT "
		     "configuration.\n");
	      return ret;
      }
   }
   else
   {
      pInfo->enblNat = FALSE;
   }
   /* enable IGMP */
   if ( strcasecmp(ipoaCfgParams[4].param, "1") == 0 )
   {
      pInfo->enblIgmp = TRUE;
   }
   else
   {
      pInfo->enblIgmp = FALSE;
   }
   /* enable QoS */
   if ( strcasecmp(ipoaCfgParams[5].param, "1") == 0 )
   {
      if ((ret = cli_checkQosQueueResources(pInfo)) == CMSRET_SUCCESS)
      {
         pInfo->enblQos = TRUE;
      }
   }
   else
   {
      pInfo->enblQos = FALSE;
   }

   return ret;

}  /* End of getIpoaInfo() */

/***************************************************************************
// Function Name: getBridgeInfo
// Description  : retrieve bridge configuration from user.
// Parameters   : pInfo -- pointer to wan information.
// Returns      : status.
****************************************************************************/
CmsRet getBridgeInfo(PWEB_NTWK_VAR pInfo)
{
   PARAMS_SETTING brCfgParams[] =
   {
      {"QoS [1-enable,2-disable]", "2", cli_isValidState},
      {NULL, "", NULL}
   };
   CmsRet ret;

   if ((ret = cli_getMenuParameters(brCfgParams, FALSE)) != CMSRET_SUCCESS)
   {
      printf("\nError in getting information for WAN configuration.\n");
      return ret;
   }

   /* enable QoS */
   if ( strcasecmp(brCfgParams[0].param, "1") == 0 )
   {
      if ((ret = cli_checkQosQueueResources(pInfo)) == CMSRET_SUCCESS)
      {
         pInfo->enblQos = TRUE;
      }
   }
   else
   {
      pInfo->enblQos = FALSE;
   }

   pInfo->enblFirewall = FALSE;
   pInfo->enblNat = FALSE;
   pInfo->enblIgmp = FALSE;

   return ret;

}  /* End of getBridgeInfo() */

/***************************************************************************
// Function Name: getFullconeInfo
// Description  : retrieve Full Cone NAT configuration from user.
// Parameters   : pInfo -- pointer to wan information.
// Returns      : status.
****************************************************************************/
CmsRet getFullconeInfo(PWEB_NTWK_VAR pInfo)
{
   PARAMS_SETTING fcCfgParams[] =
   {
      {"Full Cone NAT [1-enable,2-disable]", "2", cli_isValidState},
      {NULL, "", NULL}
   };
   CmsRet ret;

   if ((ret = cli_getMenuParameters(fcCfgParams, FALSE)) != CMSRET_SUCCESS)
   {
      printf("\nError in getting information for Full Cone configuration.\n");
      return ret;
   }

   /* enable Full Cone NAT */
   if ( strcasecmp(fcCfgParams[0].param, "1") == 0 )
   {
      pInfo->enblFullcone = TRUE;
   }
   else
   {
      pInfo->enblFullcone = FALSE;
   }

   return ret;

}  /* End of getFullconeInfo() */

/***************************************************************************
// Function Name: isValidCategory
// Description  : validate vcc category.
// Parameters   : category - vcc service category.
// Returns      : CLI_FALSE - invalid format.
//                CLI_TRUE - valid format.
****************************************************************************/
UBOOL8 isValidCategory(char *category)
{
   int select = atoi(category);

   // Category [1-ubr,2-ubrpcr,3-cbr,4-rtvbr,5-nrtvbr]
   return ( select <= 5 && select >= 1 );
}

/***************************************************************************
// Function Name: isValidProtocol
// Description  : validate protocol.
// Parameters   : protocol - WAN protocol.
// Returns      : FALSE - invalid format.
//                TRUE - valid format.
****************************************************************************/
UBOOL8 isValidProtocol(char *protocol)
{
   int select = atoi(protocol);

   // Protocol [1-pppoa,2-pppoe,3-ipoe,4-ipoa,5-bridge]
   return ( select <= 5 && select >= 1 );
}

/***************************************************************************
// Function Name: isValidEncapsulation
// Description  : validate protocol encapsulation.
// Parameters   : encap - protocol encapsulation.
// Returns      : FALSE - invalid format.
//                TRUE - valid format.
****************************************************************************/
UBOOL8 isValidEncapsulation(char *encap)
{
   return ( strcasecmp(encap, "1") == 0 || strcasecmp(encap, "2") == 0 );
}

/***************************************************************************
// Function Name: isValidAuthMethod
// Description  : validate PPP authentication method.
// Parameters   : method - PPP authentication method.
// Returns      : FALSE - invalid format.
//                TRUE - valid format.
****************************************************************************/
UBOOL8 isValidAuthMethod(char *method)
{
   return ( strcasecmp(method, "1") == 0 ||
            strcasecmp(method, "2") == 0 ||
            strcasecmp(method, "3") == 0 ||
            strcasecmp(method, "4") == 0 );
}


#endif  /* SUPPORT_CLI_MENU */
