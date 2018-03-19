/***********************************************************************
 *
 *  Copyright (c) 2011  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2011:proprietary:standard

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

/** command driven CLI code goes into this file */

#ifdef SUPPORT_CLI_CMD

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>
#include <sys/wait.h>

#include "cms_util.h"
#include "cms_core.h"
#include "cms_cli.h"
#include "cli.h"


#define MAX_OPTS  21
 
/** Globals. **/
const char wanusage[] = "\nUsage:\n";

const char wanaddintf[] =
   "       wan add interface <atm|ptm|eth>\n";

const char wanaddservice[] =
   "       wan add service <interfacename> --protocol <bridge|ipoe|pppoe|ipoa|pppoa>\n";

const char wanaddatmintf[] = 
   "       wan add interface atm <port.vpi.vci>\n"\
   "       --linktype [EoA|PPPoA|IPoA] [--encap <llc|vcmux>]\n"\
/* vlanmux and qos not supported   "       [--vlanMux <enable|disable>] [--qos <enable|disable>]\n"\ */
   "       [--atmcat UBR | --atmcat UBRwPCR <pcr> | --atmcat CBR <pcr> \n"\
   "       [--atmcat nrtVBR <pcr> <scr> <mbs> | --atmcat rtVBR <pcr> <scr> <mbs>]\n";

const char wanaddptmintf[] = 
   "       wan add interface ptm <port> [--priority <normal|high|both>] [--vlanMux <enable|disable>] [--qos <enable|disable>]\n";
/* vlanmux and qos not supported   \"       [--vlanMux <enable|disable>] [--qos <enable|disable>]\n"; */

const char wanaddethintf[] = 
   "       wan add interface eth <ethx>\n";

const char wandelete[] =
   "       wan delete interface atm <port.vpi.vci>\n"\
   "       wan delete interface ptm <port> --priority <normal|high|both>\n"\
   "       wan delete interface eth <ethx>\n"\
   "       wan delete service L3IfName\n";

const char wanshow[] = 
   "       wan show interface\n"\
   "       wan show [<port.vpi.vci>]\n";

const char wanhelp[] =
   "       wan --help <bridge|pppoe|pppoa|ipoe|ipoa>\n";

const char wanbridge[] =
   "       wan add service <L2interfacename> --protocol bridge\n"\
/* vlan not supported    "       [--vlan <vlan id>] [--service <servicename>]\n"; */
   "       [--service <servicename>]\n";
const char wanipoe[] =
   "       wan add service <L2interfacename> --protocol ipoe\n"\
/* vlan not supported    "       [--vlan <vlan id>] [--service <servicename>]\n"\ */
   "       [--firewall <enable|disable>] [--nat <enable|disable>]\n"\
   "       [--igmp <enable|disable>]\n"\
   "       [--ipaddr <wanipaddress> <wansubnetmask]\n"\
   "       [--dhcpclient <enable|disable>]\n"\
   "       [--gatewayifname <L2interfacename>] [--dnsifname <L2interfacename>]\n";

const char wanpppoe[] =
   "       wan add service <L2interfacename> --protocol pppoe\n"\
/* msc not supported      "       [--connid <connection id>]\n"\ */
/*  vlan not supported  "       [--vlan <vlan id>] [--service <servicename>]\n"\ */
   "       [--firewall <enable|disable>] [--nat <enable|disable>]\n"\
   "       [--igmp <enable|disable>]\n"\
   "       [--username <username> --password <password>]\n"\
   "       [--pppidletimeout <timeout>] [--pppipextension <disable|enable>]\n"\
   "       [--gatewayifname <pppinterfacename>] [--dnsifname <pppinterfacename>]\n";

const char wanpppoa[] =
   "       wan add service <L2interfacename> --protocol pppoa\n"\
   "       [--service <servicename>]\n"\
   "       [--firewall <enable|disable>] [--nat <enable|disable>]\n"\
   "       [--igmp <enable|disable>]\n"\
   "       [--username <username> --password <password>]\n"\
   "       [--pppidletimeout <timeout>] [--pppipextension <disable|enable>]\n";

const char wanipoa[] =
   "       wan add service <L2interfacename> --protocol ipoa\n"\
   "       --ipaddr <wanipaddress> <wansubnetmask\n" \
   "       [--service <servicename>]\n"\
   "       [--firewall <enable|disable>] [--nat <enable|disable>]\n"\
   "       [--igmp <enable|disable>]\n";


/** Prototypes. **/

static CmsRet cmdWanAdd(SINT32 argc, char **argv);
#ifdef SUPPORT_DSL
static CmsRet cmdWanAddAtmIntf(SINT32 argc, char **argv);
#ifdef SUPPORT_PTM
static CmsRet cmdWanAddPtmIntf(SINT32 argc, char **argv);
#endif
#endif /* SUPPORT_DSL */
#ifdef SUPPORT_ETHWAN
static CmsRet cmdWanAddEthIntf(SINT32 argc, char **argv);
#endif
static CmsRet cmdWanAddService(SINT32 argc, char **argv);
static CmsRet cmdWanDelete(SINT32 argc, char **argv);
static CmsRet cmdWanDeleteInterface(SINT32 argc, char **argv);
static CmsRet cmdWanDeleteService(SINT32 argc, char **argv);
static CmsRet cmdWanShow(SINT32 argc, char **argv);
static void   cmdWanHelp(char *help);
// todo: static CmsRet validateWanConn(PWEB_NTWK_VAR pInfo);
#ifdef SUPPORT_DSL
static CmsRet validateVccAddr(const char *addr, PWEB_NTWK_VAR pInfo);
static CmsRet validateLinkType(char *input, char *linkType);
static CmsRet validateEncap(char *encap);
static CmsRet validateAtmServiceCategory(const char *cat, char *servcat, UINT32 *numParams);
#endif
static CmsRet validateProtocol(char *protocol, SINT32 *ntwkPrtcl);
//static CmsRet validateVlan(char *vlan, SINT32 *vlanId);
static CmsRet validateTimeout(char *timeout, SINT32 *pppTimeOut);
static CmsRet validateConnId(char *connId, SINT32 *serviceId);
static CmsRet validateService(char *service, char *serviceName);
static CmsRet validateUsername(char *username, char *pppUserName);
static CmsRet validatePassword(char *password, char *pppPassword);
static CmsRet validateState(char *state, SINT32 *enable);



/***************************************************************************
 * Function Name: processWanCmd
 * Description  : Main program function.
 ***************************************************************************/
void processWanCmd(char *cmdLine)
{
   SINT32 argc = 0;
   char *argv[MAX_OPTS]={NULL};
   char *last = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   cliCmdSaveNeeded = FALSE;

   /* parse the command line and build the argument vector */
   argv[0] = strtok_r(cmdLine, " ", &last);
   if (argv[0] != NULL)
   {
      for (argc = 1; argc < MAX_OPTS; argc++)
      {
         argv[argc] = strtok_r(NULL, " ", &last);
         if (argv[argc] == NULL)
            break;
         cmsLog_debug("arg[%d]=%s", argc, argv[argc]);
      }
   }

   if (ret == CMSRET_SUCCESS)
   {
      if (argv[0] == NULL)
      {
         cmdWanHelp(NULL);
      }
      else if (strcasecmp(argv[0], "add") == 0)
      {
         if (currPerm & PERM2)
         {
            ret = cmdWanAdd(argc-1, &argv[1]);
         }
         else
         {
            ret = CMSRET_REQUEST_DENIED;
         }
      }
      else if (strcasecmp(argv[0], "delete") == 0)
      {
         if (currPerm & PERM2)
         {
            ret = cmdWanDelete(argc-1, &argv[1]);
         }
         else
         {
            ret = CMSRET_REQUEST_DENIED;
         }
      }
      else if (strcasecmp(argv[0], "show") == 0)
      {
         ret = cmdWanShow(argc-1, &argv[1]);
      }
      else if (strcasecmp(argv[0], "--help") == 0)
      {
         cmdWanHelp(argv[1]);
      }
      else {
         printf("\nInvalid option '%s'\n", argv[0]);
         ret = CMSRET_INVALID_ARGUMENTS;
      }
   }

   /*
    * If command was not successful, then we should not save to flash.
    * Otherwise, cli_processCliCmd will save the config for us if
    * cliCmdSaveNeeded is TRUE.
    */
   if (ret != CMSRET_SUCCESS)
   {
      cliCmdSaveNeeded = FALSE;
   }

   if (ret == CMSRET_REQUEST_DENIED)
   {
      printf("\nYou do not have permission to execute this sub-option\n\n");
   }
   
   return;
}


/***************************************************************************
 * Function Name: cmdWanCfg
 * Description  : Processes the wan add command.
 ***************************************************************************/
CmsRet cmdWanAdd(SINT32 argc, char **argv)
{
   CmsRet ret = CMSRET_SUCCESS;

   if (argc == 0)
   {
      cmdWanHelp("wanadd");
      return ret;
   }

   
   memset(&glbWebVar, 0, sizeof(WEB_NTWK_VAR));
   /* just load glbWebVar with default info for the user */
   cmsDal_getAllInfo(&glbWebVar);

   cmsLog_debug("argc[0] = %s", argv[0]);
   if (!cmsUtl_strcasecmp(argv[0], "interface"))
   {
#ifdef SUPPORT_DSL
      /*
       * First level parsing, decide if user wants to add atm or ptm interface
       */
      if (!cmsUtl_strcmp(argv[1], "atm"))
      {
         ret = cmdWanAddAtmIntf(argc-2, &argv[2]);
      }
      else
#ifdef SUPPORT_PTM
      if (!cmsUtl_strcmp(argv[1], "ptm"))
      {
         ret = cmdWanAddPtmIntf(argc-2, &argv[2]);
      }
      else
#endif
#endif /* SUPPORT_DSL */
#ifdef SUPPORT_ETHWAN
      if (!cmsUtl_strcmp(argv[1], "eth"))
      {
         ret = cmdWanAddEthIntf(argc-2, &argv[2]);
      }
      else
#endif
      {
         cmdWanHelp("wanaddintf");
      }
   }
   else if (!cmsUtl_strcasecmp(argv[0], "service"))
   {
      /*
       * User wants to add a [bridge|ipoe|pppoe|ipoa|pppoa] service.
       */
      ret = cmdWanAddService(argc-1, &argv[1]);
   }
   else
   {
      cmdWanHelp("wanadd");
      return ret;
   }


   return ret;
}


#ifdef SUPPORT_DSL

CmsRet cmdWanAddAtmIntf(SINT32 argc, char **argv)
{
   SINT32 i=0;
   char encap[BUFLEN_8];
   CmsRet ret = CMSRET_SUCCESS;

   if (argc == 0)
   {
      cmdWanHelp("atmIntf");
      return ret;
   }

   while (argv[i] != NULL)
   {
      cmsLog_debug("arg[%d] = %s", i, argv[i]);
      i++;
   }

   /* vcc is mandatory */
   ret = validateVccAddr(argv[0], &glbWebVar);
   if (ret != CMSRET_SUCCESS)
   {
      return ret;
   }

   if (cli_isVccAddrExist(&glbWebVar))
   {
      printf("An ATM interface with that address already exists\n");
      return CMSRET_REQUEST_DENIED;
   }

   /* now parse the optional arguments */
   for (i = 1; (i < argc) && (ret == CMSRET_SUCCESS); ++i)
   {
      if (strcasecmp(argv[i], "--linktype") == 0)
      {
         i++;
         cmsLog_debug("got linktype %s", argv[i]);

         if ((ret = validateLinkType(argv[i], glbWebVar.linkType)) != CMSRET_SUCCESS)
         {
            cmdWanHelp("atmintf");
            return ret;
         }
      }
      else if (strcasecmp(argv[i], "--encap") == 0)
      {
         if ((ret = validateEncap(argv[++i])) == CMSRET_SUCCESS)
         {
            strcpy(encap, argv[i]);
         }
      }
      else if (strcasecmp(argv[i], "--atmcat") == 0)
      {
         UINT32 atmCatParamCount=0;

         if ((ret = validateAtmServiceCategory(argv[++i], glbWebVar.atmServiceCategory, &atmCatParamCount)) == CMSRET_SUCCESS)
         {
            /* UBR without PCR (the default) has no additional params */

            if (atmCatParamCount >= 1)
            {
               /* all other atm service categories have at least this peak cell rate */
               glbWebVar.atmPeakCellRate = atoi(argv[++i]);
            }

            if (atmCatParamCount == 3)
            {
               /* the VBR categories also need sustained cell rate and max burst size */
               glbWebVar.atmSustainedCellRate = atoi(argv[++i]);
               glbWebVar.atmMaxBurstSize = atoi(argv[++i]);
            }
         }
      }
      else if (strcasecmp(argv[i], "--vlanMux") == 0)
      {
         ret = validateState(argv[++i], &(glbWebVar.enVlanMux));
      }
      else if (strcasecmp(argv[i], "--qos") == 0)
      {
         ret = validateState(argv[++i], &(glbWebVar.enblQos));
         if (ret == CMSRET_SUCCESS && glbWebVar.enblQos)
         {
            if ((ret = cli_checkQosQueueResources(&glbWebVar)) != CMSRET_SUCCESS)
            {
               return ret;
            }
         }
      }
      else
      {
         printf("\nunrecognized option %s\n\n", argv[i]);
         ret = CMSRET_INVALID_ARGUMENTS;
      }
   }

   if (ret != CMSRET_SUCCESS)
   {
      cmdWanHelp("atmintf");
      return ret;
   }


   /* set encapsulation mode */
   if (!cmsUtl_strcmp(glbWebVar.linkType, MDMVS_PPPOA))
   {
      if (strcasecmp(encap, "llc") == 0)
      {
         glbWebVar.encapMode = 1;
      }
   }
   else
   {
      if (strcasecmp(encap, "vcmux") == 0)
      {
         glbWebVar.encapMode = 1;
      }
   }


   cmsLog_debug("adding ATM interface");
   glbWebVar.connMode = 1; // vlanmux mode is always true.
   
   if ((ret = dalDsl_addAtmInterface(&glbWebVar)) == CMSRET_SUCCESS)
   {
      printf("ATM interface added.\n");
      cliCmdSaveNeeded = TRUE;
   }

   return ret;
}

#ifdef SUPPORT_PTM

CmsRet cmdWanAddPtmIntf(SINT32 argc, char **argv)
{
   SINT32 i=0;
   CmsRet ret = CMSRET_SUCCESS;

   if (argc == 0)
   {
      cmdWanHelp("ptmIntf");
      return ret;
   }

   while (argv[i] != NULL)
   {
      cmsLog_debug("arg[%d] = %s", i, argv[i]);
      i++;
   }

   cmsLog_debug("adding PTM interface");

   /* first arg must be port. catch people writing ATM VCC 0.0.42 here */
   if (cmsUtl_strstr(argv[0], "."))
   {
      cmdWanHelp("ptmintf");
      ret = CMSRET_INVALID_ARGUMENTS;
   }
   
   glbWebVar.portId = atoi(argv[0]);
   cmsLog_debug("portId=%d", glbWebVar.portId);

   /* to make the defaults of a ptm add to be the same as WEBUI.... */
   glbWebVar.ptmPriorityHigh = 1;
   glbWebVar.ptmPriorityNorm = 1;
   glbWebVar.connMode = 1; // vlanmux mode is always true.
   glbWebVar.enblQos = 1;

   for (i = 1; (i < argc) && (ret == CMSRET_SUCCESS); ++i)
   {
      
      if (cmsUtl_strcasecmp(argv[i], "--priority") == 0)
      {
         i++;
         cmsLog_debug("got priority %s", argv[i]);
         if (!cmsUtl_strcasecmp(argv[i], "normal"))
         {
            glbWebVar.ptmPriorityNorm = 1;
         }
         else if (!cmsUtl_strcasecmp(argv[i], "high"))
         {
            glbWebVar.ptmPriorityHigh = 1;
         }
         else if (!cmsUtl_strcasecmp(argv[i], "both"))
         {
            glbWebVar.ptmPriorityHigh = 1;
            glbWebVar.ptmPriorityNorm = 1;
         }
         else
         {
            printf("\nunrecognized option %s\n\n", argv[i]);
            cmdWanHelp("ptmintf");
            ret = CMSRET_INVALID_ARGUMENTS;
         }
      }
      else if (strcasecmp(argv[i], "--vlanMux") == 0)
      {
         ret = validateState(argv[++i], &(glbWebVar.enVlanMux));
      }
      else if (strcasecmp(argv[i], "--qos") == 0)
      {
         ret = validateState(argv[++i], &(glbWebVar.enblQos));
         if (ret == CMSRET_SUCCESS && glbWebVar.enblQos)
         {
            if ((ret = cli_checkQosQueueResources(&glbWebVar)) != CMSRET_SUCCESS)
            {
               return ret;
            }
         }
      }
      else
      {
         printf("\nunrecognized option %s\n\n", argv[i]);
         cmdWanHelp("ptmintf");
         ret = CMSRET_INVALID_ARGUMENTS;
      }
   }

   if (ret == CMSRET_SUCCESS)
   {
      if ((ret = dalDsl_addPtmInterface(&glbWebVar)) == CMSRET_SUCCESS)
      {
         printf("PTM interface added.\n");
         cliCmdSaveNeeded = TRUE;
      }
      else
      {
         printf("Failed to add PTM interface.\n");
      }
   }

   return ret;
}
#endif  /* SUPPORT_PTM */

#endif  /* SUPPORT_DSL */

#ifdef SUPPORT_ETHWAN
CmsRet cmdWanAddEthIntf(SINT32 argc, char **argv)
{
   SINT32 i=0;
   CmsRet ret = CMSRET_SUCCESS;
   if (argc == 0)
   {
      cmdWanHelp("ethintf");
      return ret;
   }

   while (argv[i] != NULL)
   {
      cmsLog_debug("arg[%d] = %s", i, argv[i]);
      i++;
   }

   cmsLog_debug("adding EthWAN interface");

   strcpy(glbWebVar.wanL2IfName, argv[0] );
   glbWebVar.connMode = 1; // vlanmux mode is always true.

   if (ret == CMSRET_SUCCESS)
   {
      if ((ret = dalEth_addEthInterface(&glbWebVar)) == CMSRET_SUCCESS)
      {
         printf("EthWAN interface added.\n");
         cliCmdSaveNeeded = TRUE;
      }
      else
      {
         printf("Failed to add EthWAN interface.\n");
      }
   }
   return ret;
}
#endif

CmsRet cmdWanAddService(SINT32 argc, char **argv)
{
   UBOOL8 isPtmInterface = FALSE;
   SINT32 i;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("argc=%d argv[0]=%s argv[1]=%s", argc, argv[0], argv[1]);

   if (argc < 3)
   {
      /* we need at least the interface-name, --protocol <bridge|ipoe|pppoe|pppoa|ipoa> part */
      cmdWanHelp("wanaddservice");
      return CMSRET_INVALID_ARGUMENTS;
   }
   
   if (cmsUtl_strcasecmp(argv[1], "--protocol"))
   {
      cmdWanHelp("wanaddservice");
      return CMSRET_INVALID_ARGUMENTS;
   }

   cmsLog_debug("interface-name=%s", argv[0]);
   cmsLog_debug("add service %s", argv[2]);


   if (!cli_isValidL2IfName(argv[0]))
   {
      cmsLog_error("%s is not valid (already in use).", argv[0]);
      return CMSRET_INVALID_ARGUMENTS;
   }
   
   if (cmsUtl_strstr(argv[0], "ptm") != NULL)
   {
      isPtmInterface = TRUE;
   }
   
   strcpy(glbWebVar.wanL2IfName, argv[0]);
   glbWebVar.connMode = 1; // vlanmux mode is always true.
   
   /* now parse the optional arguments */
   for (i = 1; (i < argc) && (ret == CMSRET_SUCCESS); ++i)
   {
      if (strcasecmp(argv[i], "--connid") == 0)
      {
         ret = validateConnId(argv[++i], &glbWebVar.serviceId);
      }
      else if (strcasecmp(argv[i], "--protocol") == 0)
      {
         if ((ret = validateProtocol(argv[++i], &glbWebVar.ntwkPrtcl)) == CMSRET_SUCCESS)
         {
            if (glbWebVar.ntwkPrtcl == PROTO_BRIDGE)
            {
               /* the dal should really treat empty string as 0.0.0.0 */
               strcpy(glbWebVar.dnsPrimary, "0.0.0.0");
               strcpy(glbWebVar.dnsSecondary, "0.0.0.0");
            }
            if (isPtmInterface == TRUE)
            {
               if (glbWebVar.ntwkPrtcl == PROTO_IPOA)
               {
                  printf("IPoA service cannot be configureed over PTM interface\n\n");
                  return CMSRET_INVALID_ARGUMENTS;
               }
               if (glbWebVar.ntwkPrtcl == PROTO_PPPOA)
               {
                  printf("PPPoA service cannot be configured over PTM interface\n\n");
                  return CMSRET_INVALID_ARGUMENTS;
               }
            }
         }
         else
         {
            cmdWanHelp("wannaddservice");
            return ret;
         }
      }
//#ifdef SUPPORT_VLAN
//      else if (strcasecmp(argv[i], "--vlan") == 0)
//      {
//         ret = validateVlan(argv[++i], &glbWebVar.vlanId);
//      }
//#endif
      else if (strcasecmp(argv[i], "--service") == 0)
      {
         ret = validateService(argv[++i], &glbWebVar.serviceName[0]);
      }
      else if (strcasecmp(argv[i], "--username") == 0)
      {
         ret = validateUsername(argv[++i], &glbWebVar.pppUserName[0]);
      }
      else if (strcasecmp(argv[i], "--password") == 0)
      {
         ret = validatePassword(argv[++i], &glbWebVar.pppPassword[0]);
      }
      else if (strcasecmp(argv[i], "--pppidletimeout") == 0)
      {
         ret = validateTimeout(argv[++i], &glbWebVar.pppTimeOut);
      }
      else if (strcasecmp(argv[i], "--ipaddr") == 0)
      {
         if (cli_isIpAddress(argv[++i]))
         {
            strcpy(&glbWebVar.wanIpAddress[0], argv[i]);
            if (cli_isIpAddress(argv[++i]))
            {
               strcpy(&glbWebVar.wanSubnetMask[0], argv[i]);
            }
            else
            {
               printf("\nInvalid wan subnet mask\n");
               ret = CMSRET_INVALID_ARGUMENTS;
            }
         }
         else
         {
            printf("\nInvalid wan ip address\n");
            ret = CMSRET_INVALID_ARGUMENTS;
         }
      }
      else if (strcasecmp(argv[i], "--state") == 0)
      {
         ret = validateState(argv[++i], &glbWebVar.enblService);
      }
      else if (strcasecmp(argv[i], "--firewall") == 0)
      {
         ret = validateState(argv[++i], &glbWebVar.enblFirewall);
      }
      else if (strcasecmp(argv[i], "--nat") == 0)
      {
         ret = validateState(argv[++i], &glbWebVar.enblNat);
      }
      else if (strcasecmp(argv[i], "--fullcone") == 0)
      {
         ret = validateState(argv[++i], &glbWebVar.enblFullcone);
      }
#ifdef DMP_X_BROADCOM_COM_IGMP_1
      else if (strcasecmp(argv[i], "--igmp") == 0)
      {
         ret = validateState(argv[++i], &glbWebVar.enblIgmp);
         ret = validateState(argv[i], &glbWebVar.enblIgmpMcastSource);
      }
#endif
#if defined(DMP_X_BROADCOM_COM_GPONWAN_1)
      else if (strcasecmp(argv[i], "--nomcastvlanfilter") == 0)
      {
         ret = validateState(argv[++i], &glbWebVar.noMcastVlanFilter);
      }
#endif /* DMP_X_BROADCOM_COM_GPONWAN_1 */
      else if (strcasecmp(argv[i], "--pppipextension") == 0)
      {
         ret = validateState(argv[++i], &glbWebVar.pppIpExtension);
      }
      else if (strcasecmp(argv[i], "--dhcpclient") == 0)
      {
         ret = validateState(argv[++i], &glbWebVar.enblDhcpClnt);
         /*
          * For IPoE, the DAL addWanIpConnection expects to see 0.0.0.0
          * in glbWebVar.wanIpAddress to indicate this is a dynamic IPoE.
          * It does not look at glbWebVar.enblDhcpClnt.
          */
         if (glbWebVar.enblDhcpClnt)
         {
            sprintf(glbWebVar.wanIpAddress, "0.0.0.0");
         }
      }
      else if (strcasecmp(argv[i], "--debug") == 0)
      {
         ret = validateState(argv[++i], &glbWebVar.enblPppDebug);
      }
      else if (strcasecmp(argv[i], "--ppptobridge") ==0)
      {
         ret = validateState(argv[++i], &glbWebVar.pppToBridge);
      }
      else if (strcasecmp(argv[i], "--dnsIfName") == 0)
      {
         i++;
         strcpy(glbWebVar.dnsIfcsList, argv[i]);

         /*
          * Since user has specified an interface to get DNS from, make sure
          * we put zeros in the static DNS server entries.  The DAL checks
          * for these zeros.
          */
         strcpy(glbWebVar.dnsPrimary, "0.0.0.0");
         strcpy(glbWebVar.dnsSecondary, "0.0.0.0");
      }
      else if (strcasecmp(argv[i], "--gatewayIfName") == 0)
      {
         i++;
         strcpy(glbWebVar.wanIfName, argv[i]);
      }

      else
      {
         printf("\nInvalid argument '%s'\n", argv[i]);
         ret = CMSRET_INVALID_ARGUMENTS;
      }
   }  /* for */

   if (ret != CMSRET_SUCCESS)
   {
      return ret;
   }
   

   if (glbWebVar.ntwkPrtcl == PROTO_PPPOA || glbWebVar.ntwkPrtcl == PROTO_PPPOE)
   {
      glbWebVar.enblOnDemand = (glbWebVar.pppTimeOut > 0)? TRUE : FALSE;
   }

   /*
    * Now verify the consistency of the config.
    * validateWannConn needs to be updated for PTM.
    * For now, don't validate.  Wait until Sean has finished his interface renameing work.
    */
   cmsLog_debug("skip validate WANConn for now");
   //   ret = validateWanConn(&glbWebVar);
   ret = CMSRET_SUCCESS;

   if (ret == CMSRET_SUCCESS)
   {
      if (glbWebVar.serviceId == 0)
      {
         cmsLog_debug("calling dalWan_addInterface now");
         if ((ret = dalWan_addService(&glbWebVar)) != CMSRET_SUCCESS)
         {
            cmsLog_error("dalWan_addService failed, ret=%d", ret);
         }
      }
      else 
      {
         if ((ret = dalWan_editInterface(&glbWebVar)) != CMSRET_SUCCESS)
         {
            cmsLog_error("dalWan_editInterface failed, ret=%d", ret);
         }
      }
      
      if (ret == CMSRET_SUCCESS)
      {
         printf("\n%s WAN service on %s was configured successfully.\n",
                argv[2], glbWebVar.wanL2IfName);
         /* Set flag to save the config. */
         cliCmdSaveNeeded = TRUE;
      }
      else
      {
         printf("\nFailed to configure %s WAN service on %s.\n",
                argv[2], glbWebVar.wanL2IfName);
      }
   }

   return ret;
}



/***************************************************************************
 * Function Name: cmdWanDelete
 * Description  : Processes the wan delete command.
 ***************************************************************************/
CmsRet cmdWanDelete(SINT32 argc, char **argv)
{
   CmsRet ret = CMSRET_SUCCESS;

   if (argc == 0)
   {
      cmdWanHelp("delete");
      return ret;
   }

   if (!cmsUtl_strcmp(argv[0], "interface"))
   {
      return (cmdWanDeleteInterface(argc-1, &(argv[1])));
   }
   else if (!cmsUtl_strcmp(argv[0], "service"))
   {
      return (cmdWanDeleteService(argc-1, &(argv[1])));
   }
   else
   {
      cmdWanHelp("delete");
      return ret;
   }

}

CmsRet cmdWanDeleteInterface(SINT32 argc, char **argv)
{
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("%s", argv[0]);

#ifdef SUPPORT_DSL
   if (!cmsUtl_strcmp(argv[0], "atm"))
   {
      if ((ret = validateVccAddr(argv[1], &glbWebVar)) != CMSRET_SUCCESS)
      {
         return ret;
      }

      ret = dalDsl_deleteAtmInterfaceWithoutIfName(&glbWebVar);
      if (ret != CMSRET_SUCCESS)
      {
         if (ret == CMSRET_INVALID_ARGUMENTS)
         {
            printf("Could not delete non-existent ATM interface\n");
         }
         else if (ret == CMSRET_REQUEST_DENIED)
         {
            printf("Cannot delete this interface because there are still services defined on it.\n");
         }
         else
         {
            printf("Unspecified error.\n");
         }
      }
      else
      {
         /* successful delete */
         printf("ATM interface deleted.\n");
         cliCmdSaveNeeded = TRUE;
      }
   }
#ifdef SUPPORT_PTM
   else if (!cmsUtl_strcmp(argv[0], "ptm"))
   {
      /* first arg must be port. catch people writing ATM VCC 0.0.42 here */
      if (cmsUtl_strstr(argv[1], "."))
      {
         cmdWanHelp("delete");
         return CMSRET_INVALID_ARGUMENTS;
      }
   
      glbWebVar.portId = atoi(argv[1]);
      cmsLog_debug("portId=%d", glbWebVar.portId);

      /* priority must be specified */
      if (argc != 4)
      {
         cmdWanHelp("delete");
         return CMSRET_INVALID_ARGUMENTS;
      }

      if (cmsUtl_strcasecmp(argv[2], "--priority"))
      {
         cmdWanHelp("delete");
         return CMSRET_INVALID_ARGUMENTS;
      }

      if (!cmsUtl_strcasecmp(argv[3], "normal"))
      {
         glbWebVar.ptmPriorityNorm = 1;
      }
      else if (!cmsUtl_strcasecmp(argv[3], "high"))
      {
         glbWebVar.ptmPriorityHigh = 1;
      }
      else if (!cmsUtl_strcasecmp(argv[3], "both"))
      {
         glbWebVar.ptmPriorityHigh = 1;
         glbWebVar.ptmPriorityNorm = 1;
      }
      else
      {
         cmdWanHelp("delete");
         return CMSRET_INVALID_ARGUMENTS;
      }

      ret = dalDsl_deletePtmInterfaceWithoutIfName(&glbWebVar);
      if (ret != CMSRET_SUCCESS)
      {
         if (ret == CMSRET_INVALID_ARGUMENTS)
         {
            printf("Could not delete non-existent PTM interface\n");
         }
         else if (ret == CMSRET_REQUEST_DENIED)
         {
            printf("Cannot delete this interface because there are still services defined on it.\n");
         }
         else
         {
            printf("Unspecified error.\n");
         }
      }
      else
      {
         /* successful delete */
         printf("PTM interface deleted.\n");
         cliCmdSaveNeeded = TRUE;
      }
   }
#endif  /* SUPPORT_PTM */
   else
#endif /* SUPPORT_DSL */
#ifdef SUPPORT_ETHWAN
   if (!cmsUtl_strcmp(argv[0], "eth"))
   {
      if (argc < 2)
      {
         cmdWanHelp("delete");
         return CMSRET_INVALID_ARGUMENTS;
      }
   
      strcpy(glbWebVar.wanL2IfName, argv[1]);
      cmsLog_debug("wanL2IfName=%s", glbWebVar.wanL2IfName);
      ret = dalEth_deleteEthInterface(&glbWebVar);
      if (ret != CMSRET_SUCCESS)
      {
         if (ret == CMSRET_INVALID_ARGUMENTS)
         {
            printf("Could not delete non-existent EthWAN interface\n");
         }
         else if (ret == CMSRET_REQUEST_DENIED)
         {
            printf("Cannot delete this interface because there are still services defined on it.\n");
         }
         else
         {
            printf("Unspecified error.\n");
         }
      }
      else
      {
         /* successful delete */
         printf("EthWAN interface deleted.\n");
         cliCmdSaveNeeded = TRUE;
      }
   }
   else
#endif
   {
      cmdWanHelp("delete");
      return ret;
   }

   return ret;
}


CmsRet cmdWanDeleteService(SINT32 argc, char **argv)
{
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("argc=%d", argc);

   if (argc != 1)
   {
      cmdWanHelp("delete");
      return CMSRET_INVALID_ARGUMENTS;
   }

   cmsLog_debug("L3 ifname=%s", argv[0]);

#ifdef SUPPORT_CELLULAR
   /* Cellular wan serive is configured with APN */
   if (cmsUtl_strstr(argv[0], CELLULAR_IFC_STR))
   {
      printf("\nCannot delete Cellular WAN service %s. Try delete APN instead.\n", argv[0]);
      return CMSRET_INVALID_ARGUMENTS;
   }  
#endif

   strcpy(glbWebVar.wanIfName, argv[0]);

   ret = dalWan_deleteService(&glbWebVar);
   if (ret != CMSRET_SUCCESS)
   {
      if (ret == CMSRET_NO_MORE_INSTANCES)
      {
         printf("\nCannot delete non-existent WAN service %s.\n", glbWebVar.wanIfName);
      }
      else
      {
         printf("\nFailed to delete WAN service %s\n", glbWebVar.wanIfName);
      }
   }      
   else
   {
      printf("\nWAN service %s was deleted successfully.\n", glbWebVar.wanIfName);
      cliCmdSaveNeeded = TRUE;
   }

   return ret;
}


/***************************************************************************
 * Function Name: cmdWanShow
 * Description  : Processes the netctl wan --show command.
 * Returns      : 0 - success, non-0 - error
 ***************************************************************************/
CmsRet cmdWanShow(SINT32 argc, char **argv)
{
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("argc=%d", argc);

   memset(&glbWebVar, 0, sizeof(WEB_NTWK_VAR));

   if (argc == 0)
   {
      /* no args given, default behavior is to show all services */
      ret = cli_wanShowServices(NULL);
   }
   else if (argc == 1 && cmsUtl_strcasecmp(argv[0], "interface"))
   {
#ifdef SUPPORT_DSL
      ret = validateVccAddr(argv[0], &glbWebVar);
      if (ret == CMSRET_SUCCESS)
      {
         ret = cli_wanShowServices(argv[0]);
      }
      else
      {
         printf("%s is invalid VCC address\n\n", argv[0]);
      }
#endif  /* SUPPORT_DSL */
   }
   else if (argc == 1 && !cmsUtl_strcasecmp(argv[0], "interface"))
   {
      /* list all ATM/PTM and EthWAN interfaces configured */
      cli_wanShowInterfaces();
   }
   else
   {
      cmdWanHelp("show");
   }
   
   return ret;
}

/***************************************************************************
 * Function Name: cmdWanHelp
 * Description  : Processes the netctl wan --help command.
 * Returns      : void
 ***************************************************************************/
void cmdWanHelp(char *help)
{
   if (help == NULL || strcasecmp(help, "--help") == 0)
   {
      printf("%s%s%s%s%s%s", wanusage, wanaddintf, wanaddservice, wandelete, wanshow, wanhelp);
   }
   else if (strcasecmp(help, "wanadd") == 0)
   {
      printf("%s%s%s", wanusage, wanaddintf, wanaddservice);
   }
   else if (strcasecmp(help, "wanaddintf") == 0)
   {
      printf("%s%s%s%s", wanusage, wanaddintf, wanaddatmintf, wanaddptmintf);
   }
   else if (strcasecmp(help, "wanaddservice") == 0)
   {
      printf("%s%s%s%s%s%s%s", wanusage, wanaddservice, wanbridge, wanipoe, wanpppoe, wanipoa, wanpppoa );
   }
   else if (strcasecmp(help, "delete") == 0)
   {
      printf("%s%s", wanusage, wandelete);
   }
   else if (strcasecmp(help, "show") == 0)
   {
      printf("%s%s", wanusage, wanshow);
   }
   else if (strcasecmp(help, "atmintf") == 0)
   {
      printf("%s%s", wanusage, wanaddatmintf);
   }
   else if (strcasecmp(help, "ptmintf") == 0)
   {
      printf("%s%s", wanusage, wanaddptmintf);
   }
   else if (strcasecmp(help, "ethintf") == 0)
   {
      printf("%s%s", wanusage, wanaddethintf);
   }
   else if (strcasecmp(help, "bridge") == 0)
   {
      printf("%s%s%s%s%s", wanusage, wanbridge, wandelete, wanshow, wanhelp);
   }
   else if (strcasecmp(help, "pppoe") == 0)
   {
      printf("%s%s%s%s%s", wanusage, wanpppoe, wandelete, wanshow, wanhelp);
   }
   else if (strcasecmp(help, "pppoa") == 0)
   {
      printf("%s%s%s%s%s", wanusage, wanpppoa, wandelete, wanshow, wanhelp);
   }
   else if ((strcasecmp(help, "mer") == 0) || (strcasecmp(help, "ipoe") == 0))
   {
      printf("%s%s%s%s%s", wanusage, wanipoe, wandelete, wanshow, wanhelp);
   }
   else if (strcasecmp(help, "ipoa") == 0)
   {
      printf("%s%s%s%s%s", wanusage, wanipoa, wandelete, wanshow, wanhelp);
   }
}


#ifdef not_used
/***************************************************************************
 * Function Name: validateWanConn
 * Description  : validate a VCC address string in the form of
 *                port.vpi.vci.
 * Returns      : 0 - success, non-0 - error
 ***************************************************************************/
CmsRet validateWanConn(PWEB_NTWK_VAR pInfo)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack;
   WanDslLinkCfgObject *dslLinkCfg;

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

   return ret;
   
}  /* End of validateWanConn() */

#endif /* not_used */

#ifdef SUPPORT_DSL

/***************************************************************************
 * Function Name: ValidateVccAddr
 * Description  : validate a VCC address string in the form of
 *                port.vpi.vci.
 * Returns      : 0 - success, non-0 - error
 ***************************************************************************/
CmsRet validateVccAddr(const char *addr, PWEB_NTWK_VAR pInfo)
{
   CmsRet nRet = CMSRET_SUCCESS;
   char buf[BUFLEN_256];

   if ( addr == NULL || (strlen(addr) >= sizeof(buf) - 1) )
   {
      nRet = CMSRET_INVALID_ARGUMENTS;
   }
   else
   {
      char *pToken = NULL, *pLast = NULL;
      
      nRet = CMSRET_INVALID_ARGUMENTS;
   
      // need to copy since strtok_r updates string
      strcpy(buf, addr);

      // VCC port
      if ((pToken = strtok_r(buf, ".", &pLast)) != NULL)
      {
         pInfo->portId = atoi(pToken);
         if (cli_isNumber(pToken))
         {
            // VCC vpi
            if ((pToken = strtok_r(NULL, ".", &pLast)) != NULL)
            {
               if (cli_isValidVpi(pToken))
               {
                  if (pInfo != NULL)
                     pInfo->atmVpi = atoi(pToken);

                  // VCC vci
                  if ((pToken = strtok_r(NULL, ".", &pLast)) != NULL)
                  {
                     if (cli_isValidVci(pToken))
                     {
                        if (pInfo != NULL)
                           pInfo->atmVci = atoi(pToken);

                        nRet = CMSRET_SUCCESS;
                     }
                  }
               }
            }
         }
      }

      cmsLog_debug("extracted portId=%d vpi=%d vci=%d nRet=%d",
                   pInfo->portId, pInfo->atmVpi, pInfo->atmVci, nRet);
   }


   return nRet;
         }

#endif  /* SUPPORT_DSL */


/***************************************************************************
 * Function Name: validateProtocol
 * Description  : validate WAN protocol
 * Returns      : 0 - success, non-0 - error
 ***************************************************************************/
CmsRet validateProtocol(char *protocol, SINT32 *ntwkPrtcl)
{
   CmsRet nRet = CMSRET_SUCCESS;

   if ( protocol == NULL )
   {
      nRet = CMSRET_INVALID_ARGUMENTS;
   }
   else
   {
      if (strcasecmp(protocol, "bridge") == 0)
      {
         if (ntwkPrtcl != NULL)
            *ntwkPrtcl = PROTO_BRIDGE;
      }
      else if (strcasecmp(protocol, "pppoe") == 0)
      {
         if (ntwkPrtcl != NULL)
            *ntwkPrtcl = PROTO_PPPOE;
      }
      else if (strcasecmp(protocol, "pppoa") == 0)
      {
         if (ntwkPrtcl != NULL)
            *ntwkPrtcl = PROTO_PPPOA;
      }
      else if ((strcasecmp(protocol, "mer") == 0) || (strcasecmp(protocol, "ipoe") == 0))
      {
         if (ntwkPrtcl != NULL)
            *ntwkPrtcl = PROTO_MER;
      }
      else if (strcasecmp(protocol, "ipoa") == 0)
      {
         if (ntwkPrtcl != NULL)
            *ntwkPrtcl = PROTO_IPOA;
      }
      else
      {
         nRet = CMSRET_INVALID_ARGUMENTS;
      }
   }
   if (nRet != CMSRET_SUCCESS )
   {
      printf("\nInvalid protocol '%s'\n", protocol? protocol : "");
   }
   return (nRet);
}


#ifdef SUPPORT_DSL
CmsRet validateLinkType(char *input, char *linkType)
{
   CmsRet nRet = CMSRET_SUCCESS;

   if ( input == NULL )
   {
      nRet = CMSRET_INVALID_ARGUMENTS;
   }
   else
   {
      if (strcasecmp(input, "ipoa") == 0)
      {
         strcpy(linkType, MDMVS_IPOA);
      }
      else if (strcasecmp(input, "pppoa") == 0)
      {
         strcpy(linkType, MDMVS_PPPOA);
      }
      else if (strcasecmp(input, "EOA") == 0)
      {
         strcpy(linkType, MDMVS_EOA);
      }
      else
      {
         nRet = CMSRET_INVALID_ARGUMENTS;
      }
   }

   if (nRet != CMSRET_SUCCESS )
   {
      printf("\nInvalid linktype %s\n", input);
   }
   return (nRet);
}

/***************************************************************************
 * Function Name: validateEncap
 * Description  : validate WAN protocol encapsulation mode
 * Returns      : 0 - success, non-0 - error
 ***************************************************************************/
CmsRet validateEncap(char *encap)
{
   CmsRet nRet = CMSRET_SUCCESS;

   if ( encap == NULL )
   {
      nRet = CMSRET_INVALID_ARGUMENTS;
   }
   else
   {
      if (strcasecmp(encap, "llc")   != 0 &&
          strcasecmp(encap, "vcmux") != 0)
      {
         nRet = CMSRET_INVALID_ARGUMENTS;
      }
   }
   if (nRet != CMSRET_SUCCESS )
   {
      printf("\nInvalid encapsulation '%s'\n", encap? encap : "");
   }
   return (nRet);
}

CmsRet validateAtmServiceCategory(const char *cat, char *servcat, UINT32 *numParams)
{
   CmsRet nRet = CMSRET_SUCCESS;

   if (!strcasecmp(cat, "UBR"))
   {
      sprintf(servcat, MDMVS_UBR);
      *numParams = 0;
   }
   else if (!strcasecmp(cat, "UBRwPCR"))
   {
      sprintf(servcat, MDMVS_UBRWPCR);
      *numParams = 1;
   }
   else if (!strcasecmp(cat, "CBR"))
   {
      sprintf(servcat, MDMVS_CBR);
      *numParams = 1;
   }
   else if (!strcasecmp(cat, "nrtVBR"))
   {
      sprintf(servcat, MDMVS_VBR_NRT);
      *numParams = 3;
   }
   else if (!strcasecmp(cat, "rtVBR"))
   {
      sprintf(servcat, MDMVS_VBR_RT);
      *numParams = 3;
   }
   else
   {
      nRet = CMSRET_INVALID_ARGUMENTS;
   }


   if (nRet != CMSRET_SUCCESS )
   {
      printf("\nInvalid ATM Service Category '%s'\n", cat);
   }

   return (nRet);
}
#endif

#if 0
/***************************************************************************
 * Function Name: validateVlan
 * Description  : validate PPP timeout
 * Returns      : 0 - success, non-0 - error
 ***************************************************************************/
CmsRet validateVlan(char *vlan, SINT32 *vlanId)
{
   CmsRet ret = CMSRET_SUCCESS;

   if (cli_isValidVlanId(vlan))
   {
      if (vlanId != NULL)
         *vlanId = atoi(vlan);
   }
   else
   {
      printf("\nInvalid vlan id '%s'\n", vlan? vlan : "");
      ret = CMSRET_INVALID_ARGUMENTS;
   }
   return (ret);
}
#endif

/***************************************************************************
 * Function Name: validateTimeout
 * Description  : validate PPP timeout
 * Returns      : 0 - success, non-0 - error
 ***************************************************************************/
CmsRet validateTimeout(char *timeout, SINT32 *pppTimeOut)
{
   CmsRet ret = CMSRET_SUCCESS;

   if (cli_isValidIdleTimeout(timeout))
   {
      if (pppTimeOut != NULL)
         *pppTimeOut = atoi(timeout);
   }
   else
   {
      printf("\nInvalid timeout '%s'\n", timeout? timeout : "");
      ret = CMSRET_INVALID_ARGUMENTS;
   }
   return (ret);
}

/***************************************************************************
 * Function Name: validateConnId
 * Description  : validate PPP timeout
 * Returns      : 0 - success, non-0 - error
 ***************************************************************************/
CmsRet validateConnId(char *connId, SINT32 *serviceId)
{
   CmsRet ret = CMSRET_SUCCESS;

   if (cli_isValidWanId(connId))
   {
      if (serviceId != NULL)
         *serviceId = atoi(connId);
   }
   else
   {
      printf("\nInvalid connId '%s'\n", connId? connId : "");
      ret = CMSRET_INVALID_ARGUMENTS;
   }
   return (ret);
}

/***************************************************************************
 * Function Name: validateService
 * Description  : validate WAN service name
 * Returns      : 0 - success, non-0 - error
 ***************************************************************************/
CmsRet validateService(char *service, char *serviceName)
{
   CmsRet ret = CMSRET_SUCCESS;

   if (cli_isValidWanServiceName(service))
   {
      if (serviceName != NULL)
         strcpy(serviceName, service);
   }
   else
   {
      printf("\nInvalid service name '%s'\n", service? service : "");
      ret = CMSRET_INVALID_ARGUMENTS;
   }
   return (ret);
}

/***************************************************************************
 * Function Name: validateUsername
 * Description  : validate PPP username
 * Returns      : 0 - success, non-0 - error
 ***************************************************************************/
CmsRet validateUsername(char *username, char *pppUserName)
{
   CmsRet ret = CMSRET_SUCCESS;

   if (cli_isValidPppUserName(username))
   {
      if (pppUserName != NULL)
         strcpy(pppUserName, username);
   }
   else
   {
      printf("\nInvalid username '%s'\n", username? username : "");
      ret = CMSRET_INVALID_ARGUMENTS;
   }
   return (ret);
}

/***************************************************************************
 * Function Name: validatePassword
 * Description  : validate PPP password
 * Returns      : 0 - success, non-0 - error
 ***************************************************************************/
CmsRet validatePassword(char *password, char *pppPassword)
{
   CmsRet ret = CMSRET_SUCCESS;

   if (cli_isValidPppPassword(password))
   {
      if (pppPassword != NULL)
         strcpy(pppPassword, password);
   }
   else
   {
      printf("\nInvalid password '%s'\n", password? password : "");
      ret = CMSRET_INVALID_ARGUMENTS;
   }
   return (ret);
}

/***************************************************************************
 * Function Name: validateState
 * Description  : validate WAN service state (enable or disable)
 * Returns      : 0 - success, non-0 - error
 ***************************************************************************/
CmsRet validateState(char *state, SINT32 *enable)
{
   CmsRet ret = CMSRET_SUCCESS;

   if (state == NULL)
   {
      ret = CMSRET_INVALID_ARGUMENTS;
   }
   else
   {
      if (strcasecmp(state, "enable") == 0)
      {
         if (enable != NULL)
            *enable = TRUE;
      }
      else if (strcasecmp(state, "disable") == 0)
      {
         if (enable != NULL)
            *enable = FALSE;
      }
      else
      {
         ret = CMSRET_INVALID_ARGUMENTS;
      }
   }
   if (ret != CMSRET_SUCCESS)
   {
      printf("\nInvalid state '%s'\n", state? state : "");
   }
   return (ret);
}


static void virtualServerCmdUsage(void)
{
      printf("usage: virtualserver show\n");
      printf("       virtualserver enable|disable num\n");
}


void virtualServerShow_igd(void)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanIpConnPortmappingObject *ipPortmappingObj=NULL;
   WanPppConnPortmappingObject *pppPortmappingObj=NULL;
   CmsRet ret;
   UINT32 index=0;

   while ((ret = cmsObj_getNext(MDMOID_WAN_IP_CONN_PORTMAPPING, &iidStack, (void **) &ipPortmappingObj)) == CMSRET_SUCCESS)
   {
      printf("[%2d] enabled=%d %s %d-%d\n", index, ipPortmappingObj->portMappingEnabled,
             ipPortmappingObj->portMappingDescription,
             ipPortmappingObj->externalPort,
             ipPortmappingObj->X_BROADCOM_COM_ExternalPortEnd);
      cmsObj_free((void **) &ipPortmappingObj);
      index++;
   }

   INIT_INSTANCE_ID_STACK(&iidStack);

   while ((ret = cmsObj_getNext(MDMOID_WAN_PPP_CONN_PORTMAPPING, &iidStack, (void **) &pppPortmappingObj)) == CMSRET_SUCCESS)
   {
      printf("[%2d] enabled=%d %s %d-%d\n", index, pppPortmappingObj->portMappingEnabled,
             pppPortmappingObj->portMappingDescription,
             pppPortmappingObj->externalPort,
             pppPortmappingObj->X_BROADCOM_COM_ExternalPortEnd);
      cmsObj_free((void **) &pppPortmappingObj);
      index++;
   }

   if (index == 0)
   {
      printf("no virtual server entries at this time.\n");
   }

   return;
}


void virtualServerEnableDisable_igd(UBOOL8 doEnable, UINT32 entryNum)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanIpConnPortmappingObject *ipPortmappingObj=NULL;
   WanPppConnPortmappingObject *pppPortmappingObj=NULL;
   CmsRet ret, r2;
   UINT32 index=0;
   UBOOL8 found=FALSE;

   while (!found &&
          (ret = cmsObj_getNext(MDMOID_WAN_IP_CONN_PORTMAPPING, &iidStack, (void **) &ipPortmappingObj)) == CMSRET_SUCCESS)
   {
      if (index == entryNum)
      {
         ipPortmappingObj->portMappingEnabled = doEnable;
         if ((r2 = cmsObj_set(ipPortmappingObj, &iidStack)) != CMSRET_SUCCESS)
         {
            printf("set of entry %d to enable=%d failed, ret=%d", entryNum, doEnable, r2);
         }

         found = TRUE;
      }

      cmsObj_free((void **) &ipPortmappingObj);
      index++;
   }

   INIT_INSTANCE_ID_STACK(&iidStack);

   while ((ret = cmsObj_getNext(MDMOID_WAN_PPP_CONN_PORTMAPPING, &iidStack, (void **) &pppPortmappingObj)) == CMSRET_SUCCESS)
   {
      if (index == entryNum)
      {
         pppPortmappingObj->portMappingEnabled = doEnable;
         if ((r2 = cmsObj_set(pppPortmappingObj, &iidStack)) != CMSRET_SUCCESS)
         {
            printf("set of entry %d to enable=%d failed, ret=%d", entryNum, doEnable, r2);
         }

         found = TRUE;
      }

      cmsObj_free((void **) &pppPortmappingObj);
      index++;
   }


   if (!found)
   {
      printf("Could not find virtualserver entry %d.\n", entryNum);
   }

   return;
}


void virtualServerShow(void)
{
#if defined(SUPPORT_DM_LEGACY98)
   virtualServerShow_igd();
#elif defined(SUPPORT_DM_HYBRID)
   virtualServerShow_igd();
#elif defined(SUPPORT_DM_PURE181)
   virtualServerShow_dev2();
#elif defined(SUPPORT_DM_DETECT)
   if (cmsMdm_isDataModelDevice2() == TRUE)
      virtualServerShow_dev2();
   else
      virtualServerShow_igd();
#endif
}


void virtualServerEnableDisable(UBOOL8 doEnable, UINT32 entryNum)
{
#if defined(SUPPORT_DM_LEGACY98)
   virtualServerEnableDisable_igd(doEnable, entryNum);
#elif defined(SUPPORT_DM_HYBRID)
   virtualServerEnableDisable_igd(doEnable, entryNum);
#elif defined(SUPPORT_DM_PURE181)
   virtualServerEnableDisable_dev2(doEnable, entryNum);
#elif defined(SUPPORT_DM_DETECT)
   if (cmsMdm_isDataModelDevice2() == TRUE)
      virtualServerEnableDisable_dev2(doEnable, entryNum);
   else
      virtualServerEnableDisable_igd(doEnable, entryNum);
#endif
}


void processVirtualServerCmd(char *cmdLine)
{
   UBOOL8 doShow=FALSE;
   UBOOL8 doEnable=FALSE;
   UINT32 entryNum;

   if (!strncasecmp(cmdLine, "show", 4))
   {
      doShow = TRUE;
   }
   else if (!strncasecmp(cmdLine, "enable", 6))
   {
      if (strlen(cmdLine) < 8)
      {
         virtualServerCmdUsage();
         return;
      }

      entryNum = atoi(&(cmdLine[7]));
      doEnable = TRUE;
   }
   else if (!strncasecmp(cmdLine, "disable", 7))
   {
      if (strlen(cmdLine) < 9)
      {
         virtualServerCmdUsage();
         return;
      }

      entryNum = atoi(&(cmdLine[8]));
   }
   else
   {
      virtualServerCmdUsage();
      return;
   }

   if (doShow)
   {
      virtualServerShow();
   }
   else
   {
      virtualServerEnableDisable(doEnable, entryNum);
   }

   return;
}

#endif   /* SUPPORT_CLI_CMD */
