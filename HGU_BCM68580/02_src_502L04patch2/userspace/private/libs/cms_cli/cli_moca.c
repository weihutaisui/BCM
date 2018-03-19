/***********************************************************************
 *
 *  Copyright (c) 2009  Broadcom Corporation
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

/** command driven CLI code goes into this file */

#ifdef SUPPORT_CLI_CMD
#ifdef SUPPORT_MOCA

#include "cms_log.h"
#include "cli.h"
//#include "mocalib-cli.h"
#include "cms_util.h"
#include "prctl.h"

struct moca_ifname_map
{
   char  *input;
   char  *ifname;
};

#define DEFAULT_MOCA_IF_NAME "moca0"

static char * g_szIfName = NULL;


/*
 * This mapping is defined in cli_moca.c, mocactl.c, and mocap.c.  Must keep
 * in sync!
 */
#define MAX_IFNAME_INPUTS 7
static struct moca_ifname_map g_mocaIfNames[MAX_IFNAME_INPUTS] =
{
   {"0",     "moca0"},
   {"moca0", "moca0"},
   {"lan",   "moca1"},
   {"1",     "moca1"},
   {"moca1", "moca1"},
   {"wan",   "moca0"},
   {NULL,    NULL}, /* Keep at end of array for easy searching */
};

static char * getMoCAIfName( char * argv )
{
   UINT32 i;
   char * pRet = NULL;

   /* Search the g_mocaIfNames array for a match */
   for ( i = 0; g_mocaIfNames[i].input != NULL; i++ )
   {
      if (strcmp(argv, g_mocaIfNames[i].input) == 0)
      {
         pRet = g_mocaIfNames[i].ifname;
         break;
      }
   }

   return (pRet);
}

#if 0
static void copyMocaInitParms ( char * parms, LanMocaIntfObject * lanMocaObj, UINT64 * initMask )
{
   char * p_option = NULL;
   char password[MoCA_MAX_PASSWORD_LEN + 1];
   char * last;
   
   /* Parse the string for the 4 legacy init parameters */
   p_option = strstr(parms, "--lof ");
   if (p_option != NULL)
   {
      *initMask |= MoCA_INIT_PARAM_NV_PARAMS_LOF_MASK;
      p_option += strlen("--lof ");
      lanMocaObj->lastOperationalFrequency = strtoul(p_option, &last, 0);
   }

   p_option = strstr(parms, "--autoScan ");
   if (p_option != NULL)
   {
      p_option += strlen("--autoScan ");
      if (strncmp(p_option, "on", strlen("on")) == 0)
      {
         *initMask |= MoCA_INIT_PARAM_AUTO_NETWORK_SEARCH_EN_MASK;
         lanMocaObj->autoNwSearch = MoCA_AUTO_NW_SCAN_ENABLED;
      }
      else if (strncmp(p_option, "off", strlen("off")) == 0)
      {
         *initMask |= MoCA_INIT_PARAM_AUTO_NETWORK_SEARCH_EN_MASK;
         lanMocaObj->autoNwSearch = MoCA_AUTO_NW_SCAN_DISABLED;
      }         
   }

   p_option = strstr(parms, "--privacy ");
   if (p_option != NULL)
   {
      p_option += strlen("--privacy ");
      if (strncmp(p_option, "on", strlen("on")) == 0)
      {
         *initMask |= MoCA_INIT_PARAM_PRIVACY_MASK;
         lanMocaObj->privacy = MoCA_PRIVACY_ENABLED;
      }
      else if (strncmp(p_option, "off", strlen("off")) == 0)
      {
         *initMask |= MoCA_INIT_PARAM_PRIVACY_MASK;
         lanMocaObj->privacy = MoCA_PRIVACY_DISABLED;
      }
   }

   p_option = strstr(parms, "--password ");
   if (p_option != NULL)
   {
      p_option += strlen("--password ");

      memcpy(password, p_option, MoCA_MAX_PASSWORD_LEN );
      p_option = strtok_r(password, " ", &last);
      lanMocaObj->password = cmsMem_strdup(password);
      *initMask |= MoCA_INIT_PARAM_PASSWORD_MASK;
   }

   lanMocaObj->initParmsString = cmsMem_strdup(parms);
}


static int StartHandler( char * parms )
{
   CmsRet                    nRet = CMSRET_SUCCESS;
   UINT64                    initMask = 0x0 ;
   LanMocaIntfObject         lanMocaObj;


   copyMocaInitParms( parms, &lanMocaObj, &initMask );

   nRet = dalMoca_start( g_szIfName, &lanMocaObj, initMask);

   if ( nRet == CMSRET_SUCCESS ) {
      cliCmdSaveNeeded = TRUE;
   }

   return 0;
}
static int StopHandler( char * parms )
{
   CmsRet                    nRet = CMSRET_SUCCESS;

   nRet = dalMoca_stop( g_szIfName );

   if (nRet == CMSRET_SUCCESS)
      return 0;
   else
      return -1;
}
static int ReStartHandler( char * parms )
{
   CmsRet                    nRet = CMSRET_SUCCESS;
   UINT64                    reInitMask = 0x0 ;
   LanMocaIntfObject         lanMocaObj;

   copyMocaInitParms( parms, &lanMocaObj, &reInitMask );

   nRet = dalMoca_restart( g_szIfName, &lanMocaObj, reInitMask);

   if ( nRet == CMSRET_SUCCESS ) {
      cliCmdSaveNeeded = TRUE;
      return 0;
   }
   else {
      return -1;
   }
}

static int ConfigHandler( char * parms )
{
   CmsRet nRet = CMSRET_SUCCESS;
   LanMocaIntfObject lanMocaObj;

   memset (&lanMocaObj, 0x00, sizeof(lanMocaObj));

   lanMocaObj.configParmsString = cmsMem_strdup(parms);
   
   nRet = dalMoca_setConfig( g_szIfName, &lanMocaObj );

   if ( nRet == CMSRET_SUCCESS ) {
      cliCmdSaveNeeded = TRUE;
      return 0;
   }
   else {
      return -1;
   }
}
static int TraceHandler( char * parms )
{
   CmsRet nRet = CMSRET_SUCCESS ;
   LanMocaIntfObject lanMocaObj;

   memset (&lanMocaObj, 0x00, sizeof(lanMocaObj));

   lanMocaObj.traceParmsString = cmsMem_strdup(parms);

   nRet = dalMoca_setTrace(g_szIfName, &lanMocaObj);

   if( nRet == CMSRET_SUCCESS ) {
      cliCmdSaveNeeded = TRUE;
      return 0;
   }
   else {
      return -1;
   }
}



/** Process MoCA command line string.
 *
 * This function parses "moca" CLI commands and executes the appropriate 
 * MoCA functions as a result. 
 *
 * @param cmdLine (IN) String pointer to the command line arguments.
 * @return None.
 */
void  processMocaCmd(char *cmdLine)
{
   char *argv[2]={NULL};
   int   argvIndex = 0;
   char *last = NULL;
   char *pMoCAIfName;
   int retVal = 0;
   char cli_buf[CLI_MAX_BUF_SZ];

   cliCmdSaveNeeded = FALSE;

   cmsLog_debug("Processing string \"%s\"\n", cmdLine);

   /* Get the main command or interface number */
   argv[argvIndex] = strtok_r(cmdLine, " ", &last);

   /* the first parameter may be a moca interface number. If not present,
    * assume interface 0 */
   if ( argv[argvIndex] != NULL )
   {
      pMoCAIfName = getMoCAIfName( argv[argvIndex] );
      if (pMoCAIfName != NULL) {
         argvIndex++;
         g_szIfName = pMoCAIfName;
         cmsLog_debug("found ifName '%s'", g_szIfName);

         /* get the main command */
         argv[argvIndex] = strtok_r(NULL, " ", &last);
      }
      else {
         g_szIfName = DEFAULT_MOCA_IF_NAME;
         cmsLog_debug("using default ifName '%s'", g_szIfName);
      }
   }

   if (argv[argvIndex] == NULL)
   {
      retVal = -1;
   }
   else if (strcmp(argv[argvIndex], "start") == 0)
   {
      retVal = StartHandler(last);
   }
   else if (strcmp(argv[argvIndex], "restart") == 0)
   {
      retVal = ReStartHandler(last);
   }
   else if (strcmp(argv[argvIndex], "stop") == 0)
   {
      retVal = StopHandler(last);
   }
   else if (strcmp(argv[argvIndex], "config") == 0)
   {
      retVal = ConfigHandler(last);
   }
   else if (strcmp(argv[argvIndex], "trace") == 0)
   {
      retVal = TraceHandler(last);
   }
   else
   {
      sprintf(cli_buf, "mocactl %s %s %s", g_szIfName, argv[argvIndex], last);
      prctl_runCommandInShellBlocking(cli_buf);
   }

   if (retVal != 0)
   {
      prctl_runCommandInShellBlocking("mocactl --help");
   }
   return;
}
#else
void  processMocaCmd(char *cmdLine)
{
   char *argv[2]={NULL};
   int   argvIndex = 0;
   char *last = NULL;
   char *pMoCAIfName;
   int retVal = 0;
   char cli_buf[CLI_MAX_BUF_SZ];

   cliCmdSaveNeeded = FALSE;

   cmsLog_debug("Processing string \"%s\"\n", cmdLine);

   /* Get the main command or interface number */
   argv[argvIndex] = strtok_r(cmdLine, " ", &last);

   /* the first parameter may be a moca interface number. If not present,
    * assume interface 0 */
   if ( argv[argvIndex] != NULL )
   {
      pMoCAIfName = getMoCAIfName( argv[argvIndex] );
      if (pMoCAIfName != NULL) {
         argvIndex++;
         g_szIfName = pMoCAIfName;
         cmsLog_debug("found ifName '%s'", g_szIfName);

         /* get the main command */
         argv[argvIndex] = strtok_r(NULL, " ", &last);
      }
      else {
         g_szIfName = DEFAULT_MOCA_IF_NAME;
         cmsLog_debug("using default ifName '%s'", g_szIfName);
      }
   }

   sprintf(cli_buf, "mocactl %s %s %s", g_szIfName, argv[argvIndex], last);
   prctl_runCommandInShellBlocking(cli_buf);

   if (retVal != 0)
   {
      prctl_runCommandInShellBlocking("mocactl --help");
   }
   return;
}

#endif

#endif
#endif

