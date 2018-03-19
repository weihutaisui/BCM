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
#include "prctl.h"

/* forward declarations */
CmsRet menuAdslShow(void);
CmsRet menuLanCfg(void);
CmsRet menuLanShow(void);
CmsRet menuDnsCfg(void);
CmsRet menuDnsShow(void);
CmsRet menuDftGwCfg(void);
CmsRet menuDftGwShow(void);

#ifdef SUPPORT_RIP
CmsRet menuRouteRipGlobalCfg(void);
CmsRet menuRouteRipIntfCfg(void);
CmsRet menuRouteRipShowCfg(void);
#endif

CmsRet menuRouteAddStaticCfg(void);
CmsRet menuRouteDeleteStaticCfg(void);
CmsRet menuRouteShow(void);
CmsRet menuVrtSrvAdd(void);
CmsRet menuVrtSrvRemove(void);
CmsRet menuVrtSrvShow(void);
CmsRet menuDmzCfg(void);
CmsRet menuDmzRemove(void);
CmsRet menuDmzShow(void);
CmsRet menuOutFltAdd(void);
CmsRet menuOutFltRemove(void);
CmsRet menuOutFltShow(void);
CmsRet menuInFltAdd(void);
CmsRet menuInFltRemove(void);
CmsRet menuInFltShow(void);
CmsRet menuMacFltChange(void);
CmsRet menuMacFltAdd(void);
CmsRet menuMacFltRemove(void);
CmsRet menuMacFltShow(void);
CmsRet menuQosAdd(void);
CmsRet menuQosRemove(void);
CmsRet menuQosShow(void);
CmsRet menuBackupSettings(void);
CmsRet menuUpdateSettings(void);
CmsRet menuDumpSettings(void);
CmsRet menuSysLogCfg(void);
CmsRet menuSysLogShow(void);

#ifdef SUPPORT_SNMP
CmsRet menuSnmpCfg(void);
CmsRet menuSnmpShow(void);
#ifdef BUILD_SNMP_DEBUG
CmsRet menuSnmpDebug(void);
#endif
#endif /* SUPPORT_SNMP */


CmsRet menuUpdateSoftware(void);
CmsRet menuAdminPasswdCfg(void);
CmsRet menuUserPasswdCfg(void);
CmsRet menuSupportPasswdCfg(void);
CmsRet menuDiagOam(void);
CmsRet menuRestoreReboot(void);
CmsRet menuReboot(void);


typedef CmsRet (*CLI_MENU_FUNC) (void);
typedef CmsRet (*CLI_MENU_PRE_FUNC)(WEB_NTWK_VAR *webVar);
typedef CmsRet (*CLI_MENU_POST_FUNC)(const WEB_NTWK_VAR *webVar);


typedef struct cli_menu_item {
   int   index;
   UINT8 perm;  /**< permission bits required to execute this item */
   char* name;
   struct cli_menu_item *subMenu;
   CLI_MENU_FUNC         menuFunc;
   CLI_MENU_PRE_FUNC     preMenuFunc;
   CLI_MENU_POST_FUNC    postMenuFunc;
} CLI_MENU_ITEM;


CLI_MENU_ITEM lanMenu[] = {
   { 0, PERM2, "Configure", NULL, menuLanCfg,   NULL, NULL },
   { 0, PERM3, "Show",      NULL, menuLanShow,  NULL, NULL },
   { 0, PERM3, "Exit",      NULL, NULL,         NULL, NULL }
};


CLI_MENU_ITEM wanMenu[] = {
   { 0, PERM2, "Configure", NULL, menuWanCfg,    NULL, NULL },
   { 0, PERM2, "Delete",    NULL, menuWanDelete, NULL, NULL },
   { 0, PERM3, "Show",      NULL, menuWanShow,   NULL, NULL },
   { 0, PERM3, "Exit",      NULL, NULL,          NULL, NULL }
};

CLI_MENU_ITEM dnsMenu[] = {
   { 0, PERM2, "Configure", NULL, menuDnsCfg,  NULL, NULL },
   { 0, PERM3, "Show",      NULL, menuDnsShow, NULL, NULL },
   { 0, PERM3, "Exit",      NULL, NULL, NULL, NULL }
};


/* second level sub-menu of routeMenu */
CLI_MENU_ITEM defaultGatewayMenu[] = {
   { 0, PERM2, "Configure", NULL, menuDftGwCfg,   NULL, NULL },
   { 0, PERM3, "Show",      NULL, menuDftGwShow,  NULL, NULL },
   { 0, PERM3, "Exit",      NULL, NULL, NULL, NULL }
};



#ifdef SUPPORT_RIP
/* second level sub-menu of routeMenu */
CLI_MENU_ITEM ripMenu[] = {
   { 0, PERM2, "Global",    NULL, menuRouteRipGlobalCfg,  NULL, NULL },
   { 0, PERM2, "Interface", NULL, menuRouteRipIntfCfg,    NULL, NULL },
   { 0, PERM3, "Show",      NULL, menuRouteRipShowCfg,    NULL, NULL },
   { 0, PERM3, "Exit",      NULL, NULL, NULL, NULL }
};
#endif /* SUPPORT_RIP */


CLI_MENU_ITEM routeMenu[] = {
   { 0, PERM2, "Default Gateway", defaultGatewayMenu, NULL ,  NULL, NULL },
   { 0, PERM2, "Add Route",       NULL,               menuRouteAddStaticCfg ,  NULL, NULL },
   { 0, PERM2, "Delete Route",    NULL,               menuRouteDeleteStaticCfg ,  NULL, NULL },
   { 0, PERM3, "Show Route",      NULL,               menuRouteShow ,  NULL, NULL },
#ifdef SUPPORT_RIP
   { 0, PERM3, "RIP",             ripMenu,            NULL ,  NULL, NULL },
#endif
   { 0, PERM3, "Exit", NULL, NULL, NULL, NULL }
};


/* second level sub-menu of nat menu */
CLI_MENU_ITEM virtualServerMenu[] = {
   { 0, PERM2, "Add",    NULL, menuVrtSrvAdd ,  NULL, NULL },
   { 0, PERM2, "Remove", NULL, menuVrtSrvRemove ,  NULL, NULL },
   { 0, PERM3, "Show",   NULL, menuVrtSrvShow ,  NULL, NULL },
   { 0, PERM3, "Exit",   NULL, NULL, NULL, NULL }
};

/* second level sub-menu of nat menu */
CLI_MENU_ITEM dmzMenu[] = {
   { 0, PERM2, "Configure",  NULL, menuDmzCfg ,  NULL, NULL },
   { 0, PERM2, "Remove",     NULL, menuDmzRemove ,  NULL, NULL },
   { 0, PERM3, "Show",       NULL, menuDmzShow ,  NULL, NULL },
   { 0, PERM3, "Exit",       NULL, NULL, NULL, NULL }
};


CLI_MENU_ITEM natMenu[] = {
   { 0, PERM3, "Virtual Server",   virtualServerMenu, NULL ,  NULL, NULL },
   { 0, PERM3, "DMZ",              dmzMenu,           NULL ,  NULL, NULL },
   { 0, PERM3, "Exit",             NULL,              NULL, NULL, NULL }
};


/* third level sub-menu of ip filtering menu */
CLI_MENU_ITEM outIpFilteringMenu[] = {
   { 0, PERM2, "Add",     NULL, menuOutFltAdd ,  NULL, NULL },
   { 0, PERM2, "Remove",  NULL, menuOutFltRemove ,  NULL, NULL },
   { 0, PERM3, "Show",    NULL, menuOutFltShow ,  NULL, NULL },
   { 0, PERM3, "Exit",    NULL, NULL, NULL, NULL }
};

/* third level sub-menu of ip filtering menu */
CLI_MENU_ITEM inIpFilteringMenu[] = {
   { 0, PERM2, "Add",     NULL, menuInFltAdd ,  NULL, NULL },
   { 0, PERM2, "Remove",  NULL, menuInFltRemove ,  NULL, NULL },
   { 0, PERM3, "Show",    NULL, menuInFltShow ,  NULL, NULL },
   { 0, PERM3, "Exit",    NULL, NULL, NULL, NULL }
};

/* second level sub-menu of firewall menu */
CLI_MENU_ITEM ipFilteringMenu[] = {
   { 0, PERM3, "Outgoing IP Filtering",  outIpFilteringMenu, NULL ,  NULL, NULL },
   { 0, PERM3, "Incoming IP Filtering",  inIpFilteringMenu,  NULL ,  NULL, NULL },
   { 0, PERM3, "Exit",                   NULL,               NULL, NULL, NULL }
};

/* second level sub-menu of firewall menu */
CLI_MENU_ITEM macFilteringMenu[] = {
   { 0, PERM2, "Change Policy", NULL, menuMacFltChange ,  NULL, NULL },
   { 0, PERM2, "Add",           NULL, menuMacFltAdd ,  NULL, NULL },
   { 0, PERM2, "Remove",        NULL, menuMacFltRemove ,  NULL, NULL },
   { 0, PERM3, "Show",          NULL, menuMacFltShow ,  NULL, NULL },
   { 0, PERM3, "Exit",          NULL, NULL, NULL, NULL }
};


CLI_MENU_ITEM firewallMenu[] = {
   { 0, PERM3, "IP Filtering",     ipFilteringMenu,  NULL ,  NULL, NULL },
   { 0, PERM3, "MAC Filtering",    macFilteringMenu, NULL ,  NULL, NULL },
   { 0, PERM3, "Exit",             NULL,             NULL, NULL, NULL }
};

CLI_MENU_ITEM qosMenu[] = {
   { 0, PERM2, "Add",    NULL, menuQosAdd ,  NULL, NULL },
   { 0, PERM2, "Remove", NULL, menuQosRemove ,  NULL, NULL },
   { 0, PERM3, "Show",   NULL, menuQosShow ,  NULL, NULL },
   { 0, PERM3, "Exit",   NULL, NULL, NULL, NULL }
};


/* second level sub-menu of management menu */
CLI_MENU_ITEM mgmtSettingsMenu[] = {
   { 0, PERM3, "Backup",      NULL, menuBackupSettings ,  NULL, NULL },
   { 0, PERM3, "Update",      NULL, menuUpdateSettings ,  NULL, NULL },
   { 0, PERM3, "Dump",        NULL, menuDumpSettings ,  NULL, NULL },
   { 0, PERM3, "Exit",        NULL, NULL, NULL, NULL }
};

/* second level sub-menu of management menu */
CLI_MENU_ITEM mgmtSyslogMenu[] = {
   { 0, PERM3, "Configure",    NULL, menuSysLogCfg,  cmsDal_getCurrentSyslogCfg, cmsDal_setSyslogCfg },
   { 0, PERM3, "Show",         NULL, menuSysLogShow, cmsDal_getCurrentSyslogCfg, NULL },
   { 0, PERM3, "Exit",         NULL, NULL, NULL, NULL}
};

#ifdef SUPPORT_SNMP
/* second level sub-menu of management menu */
CLI_MENU_ITEM mgmtSnmpAgentMenu[] = {
   { 0, PERM2, "Configure",   NULL, menuSnmpCfg ,  NULL, NULL },
   { 0, PERM3, "Show",        NULL, menuSnmpShow ,  NULL, NULL },
#ifdef BUILD_SNMP_DEBUG
   { 0, PERM2, "Debug",       NULL, menuSnmpDebug ,  NULL, NULL },
#endif /*  BUILD_SNMP_DEBUG */
   { 0, PERM3, "Exit",        NULL, NULL, NULL, NULL }
};
#endif /* SUPPORT_SNMP */

/* second level sub-menu of management menu */
CLI_MENU_ITEM mgmtUpdateSoftwareMenu[] = {
   { 0, PERM3, "Update Software",   NULL, menuUpdateSoftware ,  NULL, NULL },
   { 0, PERM3, "Exit",              NULL, NULL, NULL, NULL }
};

CLI_MENU_ITEM managementMenu[] = {
   { 0, PERM3, "Settings",          mgmtSettingsMenu,       NULL ,  NULL, NULL },
   { 0, PERM3, "System Log",        mgmtSyslogMenu,         NULL ,  NULL, NULL },
#ifdef SUPPORT_SNMP
   { 0, PERM3, "SNMP Agent",        mgmtSnmpAgentMenu,      NULL ,  NULL, NULL },
#endif
   { 0, PERM3, "Update Software",   mgmtUpdateSoftwareMenu, NULL ,  NULL, NULL },
   { 0, PERM3, "Exit",              NULL,                   NULL, NULL, NULL }
};

CLI_MENU_ITEM passwordsMenu[] = {
   { 0, PERM1, "Admin",    NULL, menuAdminPasswdCfg ,  NULL, NULL },
   { 0, (PERM_ADMIN|PERM_USER), "User",     NULL, menuUserPasswdCfg ,  NULL, NULL },
   { 0, PERM2, "Support",  NULL, menuSupportPasswdCfg ,  NULL, NULL },
   { 0, PERM3, "Exit",     NULL, NULL, NULL, NULL }
};

CLI_MENU_ITEM oamDiagMenu[] = {
   { 0, PERM3, "Test ATM OAM Loopback",    NULL, menuDiagOam ,  NULL, NULL },
   { 0, PERM3, "Exit",                     NULL, NULL, NULL, NULL }
};

CLI_MENU_ITEM mainMenu[] = {
   { 0, PERM3, "ADSL Link State",    NULL,           menuAdslShow ,  NULL, NULL },
   { 0, PERM3, "LAN",                lanMenu,        NULL ,  NULL, NULL },
   { 0, PERM3, "WAN",                wanMenu,        NULL ,  NULL, NULL },
   { 0, PERM3, "DNS Server",         dnsMenu,        NULL ,  NULL, NULL },
   { 0, PERM3, "Route Setup",        routeMenu,      NULL ,  NULL, NULL },
   { 0, PERM3, "NAT",                natMenu,        NULL ,  NULL, NULL },
   { 0, PERM3, "Firewall",           firewallMenu,   NULL ,  NULL, NULL },
   { 0, PERM3, "Quality Of Service", qosMenu,        NULL ,  NULL, NULL },
   { 0, PERM3, "Management",         managementMenu, NULL ,  NULL, NULL },
   { 0, PERM3, "Passwords",          passwordsMenu,  NULL ,  NULL, NULL },
   { 0, PERM3, "OAM Diagnostics",    oamDiagMenu,    NULL ,  NULL, NULL },
   { 0, PERM3, "Reset to Default",   NULL,           menuRestoreReboot ,  NULL, NULL },
   { 0, PERM3, "Reboot",             NULL,           menuReboot ,  NULL, NULL },
   { 0, PERM3, "Exit",               NULL,           NULL, NULL, NULL}
};



/* this one does not actually get displayed, it is used to reference the main menu */
CLI_MENU_ITEM rootMenu[] = {
   { 1, PERM3, "Main", mainMenu, NULL,  NULL, NULL },
   { 2, PERM3, "Exit", NULL, NULL, NULL, NULL}
};


#define MAX_MENU_STACK_DEPTH  8

typedef struct {
   CLI_MENU_ITEM *parentMenu;
   CLI_MENU_ITEM *subMenu;
} CLI_MENU_STACK_ITEM;

CLI_MENU_STACK_ITEM menuStack[MAX_MENU_STACK_DEPTH] = {{mainMenu, &(rootMenu[0])},
                                                       {0, 0}
                                                      };
                                                        
static UINT32 currMenuStackIndex=0;

static CLI_MENU_ITEM *currMenu = mainMenu;

UBOOL8 cliMenuSaveNeeded=FALSE;

/* some helper funcs for processMenuItem */
static CmsRet cli_callPreFunc(CLI_MENU_PRE_FUNC func);
static CmsRet cli_callPostFunc(CLI_MENU_POST_FUNC func);
static void cli_setPathBuf(void);


UBOOL8 cli_processMenuItem(const char *cmdLine)
{
   SINT32 itemId=0;
   SINT32 index=0;
   UBOOL8 found=FALSE;
   CLI_MENU_ITEM *tmpMenuItem = &(currMenu[0]);
   CmsRet ret;

   /*
    * Find the entry with the specified itemId.  Permissions may mask out
    * some of the entries, so we cannot use itemId to index directly in the array.
    */
   ret = cmsUtl_strtol(cmdLine, NULL, 10, &itemId);
   if (ret != CMSRET_SUCCESS || itemId <= 0)
   {
      /* this is not a valid menu index */
      return FALSE;
   }

   while (!found)
   {
      if (itemId == tmpMenuItem->index)
      {
         found = TRUE;
      }
      else
      {
         index++;
      }

      /* this is the last entry.  Must break even if we have not found a match yet */
      if (!strcasecmp(tmpMenuItem->name, "Exit"))
      {
         break;
      }

      tmpMenuItem++;
   }

   cmsLog_debug("found=%d itemId=%d index=%d", found, itemId, index);

   if (!found)
   {
      return FALSE;
   }


   cliMenuSaveNeeded = FALSE;

   /*
    * OK, we got a valid menu index.
    * At this point, there are 3 choices:
    * 1. call a handler function
    * 2. go down to another sub-menu.
    * 3. go up to previous sub-menu.
    */
   if (currMenu[index].menuFunc != NULL)
   {
      /* if there is a pre-func, call it */
      if (currMenu[index].preMenuFunc != NULL)
      {
         cmsLog_debug("calling pre-menu func");
         ret = cli_callPreFunc(currMenu[index].preMenuFunc);
      }

      if (ret == CMSRET_SUCCESS)
      {
         cmsLog_debug("calling menu func");
         ret = (*(currMenu[index].menuFunc))();
      }

      /* if there is a post-func, call it */
      if ((ret == CMSRET_SUCCESS) &&
          (currMenu[index].postMenuFunc != NULL))
      {
         cmsLog_debug("calling post-menu func");
         ret = cli_callPostFunc(currMenu[index].postMenuFunc);
      }

      if (ret == CMSRET_SUCCESS)
      {
         /* no error, continue to see if we need to flush config */
         if (cliMenuSaveNeeded)
         {
            /* regardless of success or failure, clear cliMenuSaveNeeded */
            cliMenuSaveNeeded = FALSE;

            if ((ret = cmsLck_acquireLockWithTimeout(CLI_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
            {
               cmsLog_error("failed to get lock, ret=%d", ret);
               printf("Could not run command due to lock failure.\r\n");
               return ret;
            }

            if ((ret = cmsMgm_saveConfigToFlash()) != CMSRET_SUCCESS)
            {
               cmsLog_error("saveConfigToFlash failed, ret=%d", ret);
            }
            
            cmsLck_releaseLock();
         }
      }

      cli_waitForContinue();
   }
   else if (currMenu[index].subMenu != NULL)
   {
      cmsLog_debug("push one level down");
      currMenuStackIndex++;
      menuStack[currMenuStackIndex].parentMenu = currMenu;
      menuStack[currMenuStackIndex].subMenu = &(currMenu[index]);
      currMenu = currMenu[index].subMenu;
      cli_setPathBuf();
   }
   else if (!strcasecmp(currMenu[index].name, "Exit"))
   {
      cmsLog_debug("pop one level up");
      if (currMenuStackIndex == 0)
      {
         cmsLog_debug("last exit menu item, logout");
         cli_keepLooping = FALSE;
      }
      else
      {
         currMenu = menuStack[currMenuStackIndex].parentMenu;

         menuStack[currMenuStackIndex].parentMenu = NULL;
         menuStack[currMenuStackIndex].subMenu = NULL;
         currMenuStackIndex--;

         cli_setPathBuf();
      }
   }

   return TRUE;
}


void cli_displayMenu(void)
{
   SINT32 i, j;

   //clear screen from start to end
   printf("\E[2J");
   // move cursor to 0, 0 position
   printf("\E[H");
   // display text to make user configure backspace
   printf("Note: If you have problem with Backspace key, please make sure \
you configure your terminal emulator settings. For instance, from HyperTerminal \
you would need to use File->Properties->Setting->Back Space key sends.\n");

   // display menu
   printf("\n\n   %s Menu\n\n", menuStack[currMenuStackIndex].subMenu->name);


   /*
    * Initialze the indexes of the menu items.  The indices may change depending
    * on the permission of the currently logged in user.  If we do not set an
    * index on the entry, that means the user does not have permission to execute
    * that command.
    */
   i = 0;
   j = 0;
   do
   {
      j++;
      if (currPerm & currMenu[j-1].perm)
      {
         i++;
         currMenu[j-1].index = i;
      }
   } while (strcasecmp(currMenu[j-1].name, "Exit"));


   i = 0;
   do
   {
      if (currMenu[i].index != 0)
      {
         printf("%2d. %s\r\n", currMenu[i].index, currMenu[i].name);
      }

   } while (strcasecmp(currMenu[i++].name, "Exit"));

}


void cli_setPathBuf(void)
{
   UINT32 i=1;
   UINT32 len;

   if (currMenuStackIndex == 0)
   {
      /* we are at Main Menu, just print standard prompt and return */
      sprintf(menuPathBuf, " > ");
      return;
   }

   menuPathBuf[0] = 0;
   while (i <= currMenuStackIndex)
   {
      len = strlen(menuPathBuf);
      snprintf(&(menuPathBuf[len]), MAX_MENU_PATH_LENGTH, "/%s", menuStack[i].subMenu->name);
      i++;
   }

   len = strlen(menuPathBuf);
   snprintf(&(menuPathBuf[len]), MAX_MENU_PATH_LENGTH, " > ");
}


CmsRet cli_callPreFunc(CLI_MENU_PRE_FUNC func)
{
   CmsRet ret;

   if ((ret = cmsLck_acquireLockWithTimeout(CLI_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      printf("Could not run command due to lock failure.\r\n");
      return ret;
   }

   if ((ret = (*func)(&glbWebVar)) != CMSRET_SUCCESS)
   {
      cmsLog_error("prefunc returned %d", ret);
   }

   cmsLck_releaseLock();

   return ret;
}


CmsRet cli_callPostFunc(CLI_MENU_POST_FUNC func)
{
   CmsRet ret;

   if ((ret = cmsLck_acquireLockWithTimeout(CLI_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      printf("Could not run command due to lock failure.\r\n");
      return ret;
   }

   if ((ret = (*func)(&glbWebVar)) != CMSRET_SUCCESS)
   {
      cmsLog_error("postfunc returned %d", ret);
   }

   cmsLck_releaseLock();

   return ret;
}


/***************************************************************************
// Function Name: getParameter
// Description  : Press <enter> to use default, '-' to go to previous
//                parameter.
// 
// Ported from cliutil.cpp::getParameter
//
// Note: (1) Parameter always has a current string.  Pressing <enter> to
//           keep the default string.
//       (2) ALWAYS set last promptName to NULL so that getparameter and
//           showParameter knows the end of array
//       (3) bValidateFunction is a validation function and can be NULL if
//           no parameter validation check is required.
//       (4) Parameter is set to current value in the menu.
// Returns    CmsRet;
****************************************************************************/

CmsRet cli_getMenuParameters(PARAMS_SETTING *inParam, UBOOL8 showInstruction)
{
   UINT32 len = 0, i = 0;
   char tmpBuf[CLI_MAX_BUF_SZ];
   char *input = NULL;
   PARAMS_SETTING *tmpPtr = inParam;
   UINT32 positionColon = 0;
   CmsRet ret=CMSRET_SUCCESS;

   if ( showInstruction  ) {
      printf("Press <enter> to use current value\r\n");
      //CliUtil::cliPrintf("Press '-' to go previous parameter\r\n");
      printf("Press <esc> and <enter> to cancel\r\n\r\n");
   }

   while (tmpPtr->prompt != NULL)
   {
      sprintf(tmpBuf, "%s (%s)", tmpPtr->prompt, tmpPtr->param);
      len = strlen(tmpBuf);
      if (len > positionColon)
      {
         positionColon = len;
      }
      tmpPtr++;
   }
   if (positionColon > 0)
   {
      positionColon += 1;
   }

   tmpPtr = inParam;
   while ( tmpPtr->prompt != NULL ) {
      sprintf(tmpBuf, "%s (%s)", tmpPtr->prompt, tmpPtr->param);
      len = strlen(tmpBuf);
      // append blank (' ') characters to indent colon (:)
      if ( positionColon < CLI_MAX_BUF_SZ ) {
         for ( i = len; i < positionColon; i++ )
            strcat(tmpBuf, " ");
      }
      // if prompt is NOT Password then use cliRdString()
      if ( strncasecmp("Password", tmpPtr->prompt, sizeof("Password")) != 0 ) {
         printf("%s: ", tmpBuf);
         fflush(stdout);

         if ((ret = cli_readString(tmpBuf, CLI_MAX_BUF_SZ)) != CMSRET_SUCCESS) {
            return ret;
         }

      } else { // if prompt is Password then use getpass()
         strcat(tmpBuf, ": ");
         /* here is another place where exit-on-idle is not honored */
         input = getpass(tmpBuf);
         memset(tmpBuf, 0, CLI_MAX_BUF_SZ);
         if ( input != NULL ) {
            len = strlen(input);
            if ( len < CLI_MAX_BUF_SZ )
               strncpy(tmpBuf, input, len);
            else {
               strncpy(tmpBuf, input, CLI_MAX_BUF_SZ - 1);
               tmpBuf[CLI_MAX_BUF_SZ - 1] = '\0';
            }
         }
      }
      switch (tmpBuf[0]) {
         case 27:          // escape to cancel
            printf("\r\nThe getting parameter progress is cancel.\r\n");
            return CMSRET_OP_ABORTED_BY_USER;
         case '-':
            // if tmpBuf = "-" only
            if ( tmpBuf[1] == '\0' ) {
               // go back one parameter
               if ( tmpPtr != inParam ) tmpPtr--;
            } else {
               // validate function is supplied, do a check
               if (tmpPtr->validateFnc != NULL) {
                  // default value is tmpPtr->param
                  if ( tmpPtr->validateFnc(tmpPtr->param) != TRUE ) {
                     printf("Invalid Parameter.  Try again!\r\n");
                     break;
                  }
               }
               tmpPtr++;
            }
            break;
         case 0:              // user selected default value
            // validate function is supplied, do a check
            if (tmpPtr->validateFnc != NULL) {
               // default value is tmpPtr->param
               if ( tmpPtr->validateFnc(tmpPtr->param) != TRUE ) {
                  printf("Invalid Parameter.  Try again!\r\n");
                  break;
               }
            }
            tmpPtr++;
            break;
         default:            // new parameter
            // validate function is supplied, do a check
            if (tmpPtr->validateFnc != NULL) {
               if ( tmpPtr->validateFnc(tmpBuf) != TRUE ) {
                  printf("Invalid Parameter.  Try again!\r\n");
                  break;
               }
            }
            strncpy(tmpPtr->param, tmpBuf, CLI_MAX_BUF_SZ);
            tmpPtr++;
            break;
      }
   }

   return CMSRET_SUCCESS;
}



CmsRet menuAdslShow(void)
{
   return CMSRET_SUCCESS;
}
CmsRet menuLanCfg(void)
{
   return CMSRET_SUCCESS;
}

CmsRet menuLanShow(void)
{
   return CMSRET_SUCCESS;
}
CmsRet menuDnsCfg(void)
{
   return CMSRET_SUCCESS;
}

CmsRet menuDnsShow(void)
{
   cmsLog_debug("entered");
   return CMSRET_SUCCESS;
}

CmsRet menuDftGwCfg(void)
{
   return CMSRET_SUCCESS;
}

CmsRet menuDftGwShow(void)
{
   return CMSRET_SUCCESS;
}

#ifdef SUPPORT_RIP
CmsRet menuRouteRipGlobalCfg(void)
{
   return CMSRET_SUCCESS;
}

CmsRet menuRouteRipIntfCfg(void)
{
   return CMSRET_SUCCESS;
}

CmsRet menuRouteRipShowCfg(void)
{
   return CMSRET_SUCCESS;
}

#endif

CmsRet menuRouteAddStaticCfg(void)
{

   CmsRet ret;
   UBOOL8 showInstruction=TRUE;
   char addr[BUFLEN_16], mask[BUFLEN_16];
   char gw[BUFLEN_16], ifc[BUFLEN_16], metric[BUFLEN_16];
   unsigned int len, parmEnter=0;

   PARAMS_SETTING routeCfgParams[] = {
    // prompt name     Default   validation function
    {"Destination network IP address",  "", cmsUtl_isValidIpv4Address},
    {"Destination network Subnet mask", "", cmsUtl_isValidIpv4Address},
    {"Gateway IP Address ", "", NULL},
    {"Gateway interface name [max 31 characters]", "", NULL},
    {"Metric [press enter for default value]", "", NULL},
    {NULL,          "", NULL}
   };

   if ((ret = cmsLck_acquireLockWithTimeout(CLI_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      printf("Could not run command due to lock failure.\r\n");
      return ret;
   } 

   printf("\r\n");
   printf("\tStatic Route Add Menu\n\n");
   printf("Note: Gateway can be IP Address and/or Interface Name.\n");
   printf("      Hit return at prompt if parameter is not used.\n\n");

   if ((ret = cli_getMenuParameters(routeCfgParams, showInstruction)) != CMSRET_SUCCESS) 
   {
      printf("\nError getting route info.\n");
      cmsLck_releaseLock();
      return ret;
   }

   cmsLog_debug("Add static route with addr/mask/gtwy/ifc:   %s/%s/%s/%s/%s", routeCfgParams[0].param, routeCfgParams[1].param, routeCfgParams[2].param, routeCfgParams[3].param, routeCfgParams[4].param);
  
   strcpy(addr, routeCfgParams[0].param);
   strcpy(mask, routeCfgParams[1].param);

   /* gateway IP, let's see if it's entered */
   len = strlen(routeCfgParams[2].param);

   if ( len > 1) 
   {
      if (cmsUtl_isValidIpv4Address(routeCfgParams[2].param) == TRUE) 
      {
         strcpy(gw,routeCfgParams[2].param);
         parmEnter = 1;
      }
      else 
      {
         printf("\n Error: Invalid gateway IP address");
         cmsLck_releaseLock();
         return CMSRET_INVALID_ARGUMENTS;
      }
   }
   else 
   {
      gw[0]='\0';
   }
   len = strlen(routeCfgParams[3].param);

   if (len >= BUFLEN_16) 
   {
      printf("\nError: invalid interface name.  Maximum length is 15 characters long\n");
      cmsLck_releaseLock();
      return CMSRET_INVALID_ARGUMENTS;
   }
   if (len > 1) 
   {
      strcpy(ifc, routeCfgParams[3].param);
      parmEnter = 1;
   }
   else
   {
      ifc[0] = '\0';
   }

   len = strlen(routeCfgParams[4].param);

   if ( len > 1) 
   {
      strcpy(metric,routeCfgParams[4].param);
   }
   else 
   {
      metric[0]='\0';
   }

   if (!parmEnter) 
   {
      printf("\nError: gateway address or interface name must be entered.\n");
      cmsLck_releaseLock();
      return CMSRET_INVALID_ARGUMENTS;
   }

   if ((ret = dalStaticRoute_addEntry(addr, mask, gw, ifc, metric)) != CMSRET_SUCCESS) 
   {
      cmsLog_error("dalStaticRoute_addEntry failed, ret=%d", ret);
      printf("\nError happens when add the route.\n");
      cmsLck_releaseLock();
      return ret;
   }
   else
   {
      cliMenuSaveNeeded = TRUE;
   }

   cmsLck_releaseLock();
  
   return CMSRET_SUCCESS;
  
}

CmsRet menuRouteDeleteStaticCfg(void)
{
   CmsRet ret;
   UBOOL8 showInstruction = TRUE;
   char addr[BUFLEN_16], mask[BUFLEN_16];
   unsigned int len=0;

   PARAMS_SETTING routeCfgParams[] = {
    // prompt name     Default   validation function
    {"Destination network IP address",  "", cmsUtl_isValidIpv4Address},
    {"Destination network Subnet mask", "", cmsUtl_isValidIpv4Address},
    {NULL,          "", NULL}
   };

   if ((ret = cmsLck_acquireLockWithTimeout(CLI_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      printf("Could not run command due to lock failure.\r\n");
      return ret;
   }   

   printf("\r\n");
   printf("\tStatic Route Delete Menu\n\n");

   if ((ret = cli_getMenuParameters(routeCfgParams, showInstruction)) != CMSRET_SUCCESS) 
   {
      printf("\nError getting route info.\n");
      cmsLck_releaseLock();
      return ret;
   }

   len = strlen(routeCfgParams[0].param);
   strncpy(addr,routeCfgParams[0].param,len);
   addr[len]='\0';

   len = strlen(routeCfgParams[1].param);
   strncpy(mask,routeCfgParams[1].param,len);
   mask[len]='\0';

  if ((ret = dalStaticRoute_deleteEntry(addr, mask)) != CMSRET_SUCCESS) 
  {
     cmsLog_error("dalStaticRoute_deleteEntry failed, ret=%d", ret);
     printf("\nError happens when delete the route.\n");
     cmsLck_releaseLock();
     return ret;
   }
   else
   {
      cliMenuSaveNeeded = TRUE;
   }

   cmsLck_releaseLock();  

   return CMSRET_SUCCESS;
}

CmsRet menuRouteShow(void)
{

   char cmd[BUFLEN_64];
   FILE* fs = NULL;

   printf("\r\n");
   printf("\tRouting Table Show Menu\n\n");

   printf("Flags: U - up, ! - reject, G - gateway, H - host, R - Reinstate \n");
   printf("       D - dynamic (redirect), M - modified (redirect)\n\n");

   cmd[0] = '\0';

   /* execute command with err output to rterr */
   sprintf(cmd, "route 2> /var/rterr");
   prctl_runCommandInShellBlocking(cmd);

   /* check to see if there's any error */
   fs = fopen("/var/rterr", "r");
   if (fs != NULL) 
   {
      prctl_runCommandInShellBlocking("cat /var/rterr");
      fclose(fs);
      prctl_runCommandInShellBlocking("rm /var/rterr");
   }

   return CMSRET_SUCCESS;

}

CmsRet menuVrtSrvAdd(void)
{
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 showInstruction=TRUE;
   UINT32 positionColon;
   char srvName[BUFLEN_64], srvAddr[BUFLEN_16], protocol[BUFLEN_4], dstWanIf[BUFLEN_18]; 
   UINT16 EPS, EPE, IPS, IPE;
   int select;

   PARAMS_SETTING vrtSrvParam[] = {
      // prompt name     Default   validation function
      {"Service name", "", NULL},
      {"Protocol [0-tcp&udp,1-tcp,2-udp]",  "1", NULL},
      {"External start port", "", cmsUtl_isValidPortNumber},
      {"External end port", "", cmsUtl_isValidPortNumber},
      {"Internal start port", "", cmsUtl_isValidPortNumber},
      {"Internal end port", "", cmsUtl_isValidPortNumber},
      {"Internal server IP address", "", cmsUtl_isValidIpv4Address},
      {"Service interface", "", NULL},
      {NULL,          "", NULL}
   };

   positionColon = strlen(vrtSrvParam[1].prompt) + 5;

   if ((ret = cmsLck_acquireLockWithTimeout(CLI_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      printf("Could not run command due to lock failure.\r\n");
      return ret;
   } 

   printf("\r\n");
   printf("\tVirtual Server Add Menu\n\n");
   printf("Note: Hit return at prompt if parameter is not used.\n\n");

//   if ((ret = cli_getMenuParameters(vrtSrvParam, showInstruction, positionColon)) != CMSRET_SUCCESS) 
   if ((ret = cli_getMenuParameters(vrtSrvParam, showInstruction)) != CMSRET_SUCCESS)    
   {
      printf("\nError getting virtual server info.\n");
      cmsLck_releaseLock();
      return ret;
   }

   cmsLog_debug("Add virtual server with name/proto/eps/epe/ips/ipe/addr:   %s/%s/%s/%s/%s/%s/%s", 
   	vrtSrvParam[0].param, vrtSrvParam[1].param, vrtSrvParam[2].param, vrtSrvParam[3].param, 
   	vrtSrvParam[4].param, vrtSrvParam[5].param, vrtSrvParam[6].param, vrtSrvParam[7].param);
  
   strcpy(srvName, vrtSrvParam[0].param);

   if (srvName[0] == '\0' || strlen(srvName) > 64)
   {
      printf("Invalid service name: service name must be less than 64 characters");
      cmsLck_releaseLock();
      return CMSRET_INVALID_PARAM_VALUE;
   }
   
   strcpy(protocol, vrtSrvParam[1].param);

   select = atoi(protocol);

   if ( (select > 2 || select < 0) && (select != -1) )
   {
      printf("Invalid protocol: 0-tcp&udp, 1-tcp, 2-udp");
      cmsLck_releaseLock();
      return CMSRET_INVALID_PARAM_VALUE;
   }
   
   EPS = atoi(vrtSrvParam[2].param);
   EPE = atoi(vrtSrvParam[3].param);
   IPS = atoi(vrtSrvParam[4].param);
   IPE = atoi(vrtSrvParam[5].param);
   
   if (cmsUtl_isValidPortNumber(vrtSrvParam[2].param) == FALSE || cmsUtl_isValidPortNumber(vrtSrvParam[3].param) == FALSE || cmsUtl_isValidPortNumber(vrtSrvParam[4].param) == FALSE || cmsUtl_isValidPortNumber(vrtSrvParam[5].param) == FALSE || EPS == 0 || EPE == 0 || IPS == 0 || IPE == 0)
   {
      printf("Invalid port number");
      cmsLck_releaseLock();
      return CMSRET_INVALID_PARAM_VALUE;
   }

   strcpy(srvAddr, vrtSrvParam[6].param);
   strcpy(dstWanIf, vrtSrvParam[7].param);

   if ((ret = dalVirtualServer_addEntry(dstWanIf, srvName, srvAddr, protocol, EPS, EPE, IPS, IPE)) != CMSRET_SUCCESS) 
   {
      cmsLog_error("dalVirtualServer_addEntry failed, ret=%d", ret);
      printf("\nError happens when add the virtual server.\n");
      cmsLck_releaseLock();
      return ret;
   }
   else
   {
      cliMenuSaveNeeded = TRUE;
   }

   cmsLck_releaseLock();
  
   return ret;
  
}

CmsRet menuVrtSrvRemove(void)
{
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 showInstruction=TRUE;
   UINT32 positionColon;
   char srvAddr[BUFLEN_16], protocol[BUFLEN_8]; 
   UINT16 EPS, EPE, IPS, IPE;
   int select;

   PARAMS_SETTING vrtSrvParam[] = {
      // prompt name     Default   validation function
      {"Service name", "", NULL},
      {"Protocol [0-tcp&udp,1-tcp,2-udp]",  "1", NULL},
      {"External start port", "", cmsUtl_isValidPortNumber},
      {"External end port", "", cmsUtl_isValidPortNumber},
      {"Internal start port", "", cmsUtl_isValidPortNumber},
      {"Internal end port", "", cmsUtl_isValidPortNumber},
      {"Internal server IP address", "", cmsUtl_isValidIpv4Address},
      {NULL,          "", NULL}
   };
   
   printf("\n\tVirtual Server Removal\n\n");
   printf("Note: Hit return at prompt if parameter is not used or to accept default value.\n");

   positionColon = strlen(vrtSrvParam[1].prompt) + 5;

   if ((ret = cmsLck_acquireLockWithTimeout(CLI_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      printf("Could not run command due to lock failure.\r\n");
      return ret;
   } 

   printf("\r\n");
   printf("\tVirtual Server Removal\n\n");
   printf("Note: Hit return at prompt if parameter is not used.\n\n");

//   if ((ret = cli_getMenuParameters(vrtSrvParam, showInstruction, positionColon)) != CMSRET_SUCCESS) 
   if ((ret = cli_getMenuParameters(vrtSrvParam, showInstruction)) != CMSRET_SUCCESS) 
   {
      printf("\nError getting virtual server info.\n");
      cmsLck_releaseLock();
      return ret;
   }

   cmsLog_debug("Delete virtual server with proto/eps/epe/ips/ipe/addr:   %s/%s/%s/%s/%s/%s", 
   	vrtSrvParam[1].param, vrtSrvParam[2].param, vrtSrvParam[3].param, vrtSrvParam[4].param, vrtSrvParam[5].param, vrtSrvParam[6].param);

   strcpy(protocol, vrtSrvParam[1].param);
   select = atoi(protocol);

   if ( (select > 2 || select < 0) && (select != -1) )
   {
      printf("Invalid protocol: 0-tcp&udp, 1-tcp, 2-udp, 3-icmp");
      cmsLck_releaseLock();
      return CMSRET_INVALID_PARAM_VALUE;
   }

   if (select == 1)
   {
      strcpy(protocol, "TCP");
   }
   else if (select == 2)
   {
      strcpy(protocol, "UDP");
   }
   else
   {
      strcpy(protocol, "\0");
   }
   
   EPS = atoi(vrtSrvParam[2].param);
   EPE = atoi(vrtSrvParam[3].param);
   IPS = atoi(vrtSrvParam[4].param);
   IPE = atoi(vrtSrvParam[5].param);

   strcpy(srvAddr, vrtSrvParam[6].param);

   if ((ret = dalVirtualServer_deleteEntry(srvAddr, protocol, EPS, EPE, IPS, IPE)) != CMSRET_SUCCESS) 
   {
      cmsLog_error("dalVirtualServer_deleteEntry failed, ret=%d", ret);
      printf("\nError happens when delete the virtual server.\n");
      cmsLck_releaseLock();
      return ret;
   }
   else
   {
      cliMenuSaveNeeded = TRUE;
   }
   
   cmsLck_releaseLock();
  
   return ret;   
}

CmsRet menuVrtSrvShow(void)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanPppConnPortmappingObject *pppObj = NULL;
   WanIpConnPortmappingObject *ipObj = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   printf("\n\tVirtual Server Show\n\n");
   printf("Server Name\tProto.\tExternal Start\tExternal End\tInternal Start\tInternal End\tServer\n");
   printf("\t\t\tPort\t\tPort\t\tPort\t\tPort\t\tIP Address\n");

   if ((ret = cmsLck_acquireLockWithTimeout(CLI_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      printf("Could not run command due to lock failure.\r\n");
      return ret;
   } 
   
   while ( (ret = cmsObj_getNext
         (MDMOID_WAN_PPP_CONN_PORTMAPPING, &iidStack, (void **) &pppObj)) == CMSRET_SUCCESS) 
   {
      if ( strlen(pppObj->portMappingDescription) < 8 )
      {
         printf("%s\t\t", pppObj->portMappingDescription);
      }
      else
      {
         printf("%s\t", pppObj->portMappingDescription);
      }

      if ( !strcmp(pppObj->portMappingProtocol, "TCP") ) 
      {
         printf("TCP\t");
      }
      else if ( !strcmp(pppObj->portMappingProtocol, "UDP") )
      {
         printf("UDP\t");
      }
      else
      {
         printf("TCP&UDP\t");
      }

      printf("%u\t\t", pppObj->externalPort);
      printf("%u\t\t", pppObj->X_BROADCOM_COM_ExternalPortEnd);
      printf("%u\t\t", pppObj->internalPort);
      printf("%u\t\t", pppObj->X_BROADCOM_COM_InternalPortEnd);
      printf("%s\n", pppObj->internalClient);

      cmsObj_free((void **) &pppObj);
   }

   while ( (ret = cmsObj_getNext
         (MDMOID_WAN_IP_CONN_PORTMAPPING, &iidStack, (void **) &ipObj)) == CMSRET_SUCCESS) 
   {
      if ( strlen(ipObj->portMappingDescription) < 8 )
      {
         printf("%s\t\t", ipObj->portMappingDescription);
      }
      else
      {
         printf("%s\t", ipObj->portMappingDescription);
      }
	  
      if ( !strcmp(ipObj->portMappingProtocol, "TCP") ) 
      {
         printf("TCP\t");
      }
      else if ( !strcmp(ipObj->portMappingProtocol, "UDP") )
      {
         printf("UDP\t");
      }
      else
      {
         printf("TCP&UDP\t");
      }

      printf("%u\t\t", ipObj->externalPort);
      printf("%u\t\t", ipObj->X_BROADCOM_COM_ExternalPortEnd);
      printf("%u\t\t", ipObj->internalPort);
      printf("%u\t\t", ipObj->X_BROADCOM_COM_InternalPortEnd);
      printf("%s\n", ipObj->internalClient);

      cmsObj_free((void **) &ipObj);
   }
   
   cmsLck_releaseLock();

   return CMSRET_SUCCESS;
}

CmsRet menuDmzCfg(void)
{
   char addr[BUFLEN_16];
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 showInstruction=TRUE;
   UINT32 positionColon;
   
   PARAMS_SETTING dmzParam[] = {
      // prompt name     Default   validation function
      {"DMZ IP address",  "", cmsUtl_isValidIpv4Address},
      {NULL,          "", NULL}
   };

   if ((ret = cmsLck_acquireLockWithTimeout(CLI_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      printf("Could not run command due to lock failure.\r\n");
      return ret;
   } 
   
   printf("\n\tDMZ Configuration\n\n");

//   if ((ret = cli_getMenuParameters(dmzParam, showInstruction, positionColon)) != CMSRET_SUCCESS) 
   if ((ret = cli_getMenuParameters(dmzParam, showInstruction)) != CMSRET_SUCCESS) 
   {
      printf("\nError getting Dmz host info.\n");
      cmsLck_releaseLock();
      return ret;
   }   

   positionColon = strlen(dmzParam[0].prompt) + 4;

   strcpy(addr, dmzParam[0].param);

   if ((ret = dalDmzHost_addEntry(addr)) != CMSRET_SUCCESS) 
   {
      cmsLog_error("dalDmzHost_addEntry failed, ret=%d", ret);
      cmsLck_releaseLock();	  
      return ret;
   }
   else
   {
      cliMenuSaveNeeded = TRUE;
   }
   
   cmsLck_releaseLock();

   return ret;
}

CmsRet menuDmzRemove(void)
{
   char addr[BUFLEN_16];
   CmsRet ret = CMSRET_SUCCESS;   
   UBOOL8 showInstruction=TRUE;
   PARAMS_SETTING dmzParam[] = {
   // prompt name     Default   validation function
   {"Do you really want to remove DMZ Host? [1-yes,2-no] ==>",  "", NULL},
   {NULL,          "", NULL}
};

   printf("\n\tDMZ Removal\n\n");
//   printf("Do you really want to remove DMZ Host? [1-yes,2-no] ==> ");
   
//   if ((ret = cli_readString(confirm, 4)) != CMSRET_SUCCESS) {
//      return ret;
//   }


   if ((ret = cmsLck_acquireLockWithTimeout(CLI_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      printf("Could not run command due to lock failure.\r\n");
      return ret;
   }    

//   if ((ret = cli_getMenuParameters(dmzParam, showInstruction, positionColon)) != CMSRET_SUCCESS) 
   if ((ret = cli_getMenuParameters(dmzParam, showInstruction)) != CMSRET_SUCCESS) 
   {
      printf("\nError getting Dmz host info.\n");
      cmsLck_releaseLock();
      return ret;
   }   
   
   if ( strcasecmp(dmzParam[0].param, "1") == 0 ) 
   {
      addr[0] = '\0';
      if ((ret = dalDmzHost_addEntry(addr)) != CMSRET_SUCCESS) 
      {
         cmsLog_error("dalDmzHost_addEntry failed, ret=%d", ret);
         cmsLck_releaseLock();	  
         return ret;
      }
      else
      {
         cliMenuSaveNeeded = TRUE;
      }
   }
   cmsLck_releaseLock();

   return ret;
}

CmsRet menuDmzShow(void)
{
   CmsRet ret = CMSRET_SUCCESS;
   char addr[BUFLEN_16];

   if ((ret = cmsLck_acquireLockWithTimeout(CLI_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      printf("Could not run command due to lock failure.\r\n");
      return ret;
   }    
   printf("\n\tDMZ Show\n\n");

   // get current DMZ Host IP address
   dalGetDmzHost(addr);
   printf("DMZ Host IP address: %s\n", addr);   
   
   cmsLck_releaseLock();
   return ret;
}


CmsRet menuOutFltAdd(void)
{
   return CMSRET_SUCCESS;
}
CmsRet menuOutFltRemove(void)
{
   return CMSRET_SUCCESS;
}

CmsRet menuOutFltShow(void)
{
   return CMSRET_SUCCESS;
}

CmsRet menuInFltAdd(void)
{
   return CMSRET_SUCCESS;
}

CmsRet menuInFltRemove(void)
{
   return CMSRET_SUCCESS;
}

CmsRet menuInFltShow(void)
{
   return CMSRET_SUCCESS;
}

CmsRet menuMacFltChange(void)
{
   return CMSRET_SUCCESS;
}

CmsRet menuMacFltAdd(void)
{
   return CMSRET_SUCCESS;
}

CmsRet menuMacFltRemove(void)
{
   return CMSRET_SUCCESS;
}

CmsRet menuMacFltShow(void)
{
   return CMSRET_SUCCESS;
}

CmsRet menuQosAdd(void)
{
   return CMSRET_SUCCESS;
}

CmsRet menuQosRemove(void)
{
   return CMSRET_SUCCESS;
}

CmsRet menuQosShow(void)
{
   return CMSRET_SUCCESS;
}

CmsRet menuBackupSettings(void)
{
#ifdef orig_cfm_code
     char cmd[IFC_LARGE_LEN];
   PARAMS_SETTING backupSettingParam[] = {
      // prompt name     Default   validation function
      {"Tftp Server IP address",  "", BcmCli_isIpAddress},
      {"Remote File Name", "backupsettings.cfg", NULL},
      {NULL,          "", NULL}
   };

   printf("\n\tBackup Configuration Setting\n\n");

   CliUtil::showInstruction = CLI_TRUE;
   CliUtil::positionColon = 25;

   if ( BcmCli_getParameter(backupSettingParam) == CLI_FALSE ) {
      BcmCli_cliPrintf("\nError getting Tftp IP address info.\n");
      CliUtil::promptContinue();
      return CLI_STS_ERR_GENERAL;
   }
   sprintf(cmd, "tftp -p -f %s -t c %s", backupSettingParam[1].param, backupSettingParam[0].param);
   prctl_runCommandInShellBlocking(cmd);
   CliUtil::promptContinue();
   return CLI_STS_OK;
#endif
   
   return CMSRET_SUCCESS;
}

CmsRet menuUpdateSettings(void)
{
#ifdef orig_cfm_code
     char cmd[IFC_LARGE_LEN];
   PARAMS_SETTING updateSettingParam[] = {
      // prompt name     Default   validation function
      {"Tftp Server IP address",  "", BcmCli_isIpAddress},
      {"Update Setting File Name", "backupsettings.cfg", NULL},
      {NULL,          "", NULL}
   };

   printf("\n\tUpdate Configuration Setting\n\n");

   CliUtil::showInstruction = CLI_TRUE;
   CliUtil::positionColon = 25;

   if ( BcmCli_getParameter(updateSettingParam) == CLI_FALSE ) {
      BcmCli_cliPrintf("\nError getting Tftp IP address info.\n");
      CliUtil::promptContinue();
      return CLI_STS_ERR_GENERAL;
   }
   sprintf(cmd, "tftp -g -t c -f %s %s", updateSettingParam[1].param, updateSettingParam[0].param);
   prctl_runCommandInShellBlocking(cmd);
   CliUtil::promptContinue();
   return CLI_STS_OK;;
#endif
   return CMSRET_SUCCESS;
}

CmsRet menuDumpSettings(void)
{
   /* dump current config to terminal */
   
#ifdef orig_cfm_code
     char cmd[IFC_LARGE_LEN];

   cmd[0] = '\0';
   if (BcmPsi_writeFlashToFile() == PSI_STS_OK) {
     sprintf(cmd, "cat %s", PSI_XML_PATH_FLASHCONFIG);
     prctl_runCommandInShellBlockingMute(cmd);
   }
   else
     printf("Configuration database is empty.\n");
   CliUtil::promptContinue();
   return CLI_STS_OK;
#endif
   return CMSRET_SUCCESS;
}

CmsRet menuSysLogCfg(void)
{
   CmsRet ret;
   UBOOL8 showInstruction=TRUE;
   PARAMS_SETTING sysLogCfgParams[] = {
      // prompt name     Default   validation function
      {"State [1-enable,2-disable]",   "", cli_isValidState},
      {"Log level [0-7]", "",     cmsUtl_isValidSyslogLevel},
      {"Display level [0-7]", "", cmsUtl_isValidSyslogLevel},
      {"Mode [1-local,2-remote,3-both]", "", cmsUtl_isValidSyslogMode},
      {"Server IP address", "",   cmsUtl_isValidIpv4Address},
      {"Server UDP port", "",     cmsUtl_isValidPortNumber},
      {NULL,            "", NULL}
   };

   /* first copy the current values from glbWebVar to syslogCfgParams */
   strcpy(sysLogCfgParams[0].param, (glbWebVar.logStatus ? "1" : "2"));
   sprintf(sysLogCfgParams[1].param, "%d", glbWebVar.logLevel);
   sprintf(sysLogCfgParams[2].param, "%d", glbWebVar.logDisplay);
   sprintf(sysLogCfgParams[3].param, "%d", glbWebVar.logMode);
   strcpy(sysLogCfgParams[4].param, glbWebVar.logIpAddress);
   sprintf(sysLogCfgParams[5].param, "%d", glbWebVar.logPort);


   /* now get input from user */
   printf("\n\tSystem Log Configuration\n\n");

   if ((ret = cli_getMenuParameters(sysLogCfgParams, showInstruction)) != CMSRET_SUCCESS) {
      printf("\nError getting system log info.\n");
      return ret;
   }


   /* now copy all the new values from syslogcfgParams back to glbWebVar */
   glbWebVar.logStatus = ( strncmp(sysLogCfgParams[0].param, "1", 1) == 0 );
   glbWebVar.logLevel = atoi(sysLogCfgParams[1].param);
   glbWebVar.logDisplay = atoi(sysLogCfgParams[2].param);
   glbWebVar.logMode = atoi(sysLogCfgParams[3].param);
   strcpy(glbWebVar.logIpAddress, sysLogCfgParams[4].param);
   glbWebVar.logPort = atoi(sysLogCfgParams[5].param);


   /*
    * the post-func will send the new values to the DAL, so we don't need
    * to do any writes here.  Just tell the post-func that we need to
    * save the config.
    */
   cliMenuSaveNeeded = TRUE;

   return CMSRET_SUCCESS;
}


/** Ported from climenumngt: this menu item does not show the contents of the
 *  log, just the config settings.
 */
CmsRet menuSysLogShow(void)
{
   printf("\n\tSystem Log Information\n\n");
   if ( glbWebVar.logStatus == TRUE )
      printf("Log               : enable\n");
   else
      printf("Log               : disable\n");

   printf("Display level     : %d\n", glbWebVar.logDisplay);
   printf("Log level         : %d\n", glbWebVar.logLevel);

   /* see cmsUtl_syslogModeToNum(): these are hard coded values in logconfig.html */
   if ( glbWebVar.logMode == 3 )
      printf("Option            : local & remote\n");
   else if ( glbWebVar.logMode == 2 )
      printf("Option            : remote\n");
   else
      printf("Option            : local\n");

   printf("Server IP address : %s\n", glbWebVar.logIpAddress);
   printf("Server UDP port   : %d\n", glbWebVar.logPort);

   return CMSRET_SUCCESS;
}

#ifdef SUPPORT_SNMP
CmsRet menuSnmpCfg(void)
{
   return CMSRET_SUCCESS;
}

CmsRet menuSnmpShow(void)
{
   return CMSRET_SUCCESS;
}

#ifdef BUILD_SNMP_DEBUG
CmsRet menuSnmpDebug(void)
{
   return CMSRET_SUCCESS;
}
#endif
#endif /* SUPPORT_SNMP */


CmsRet menuUpdateSoftware(void)
{
#ifdef orig_cfm_code
      char cmd[IFC_LARGE_LEN];
   PARAMS_SETTING updateSoftwareParam[] = {
      // prompt name     Default   validation function
      {"Tftp Server IP address",  "", BcmCli_isIpAddress},
      {"Update Software File Name", "bcm963xx_fs_kernel", NULL},
      {NULL,          "", NULL}
   };

   printf("\n\tUpdate Software\n\n");

   CliUtil::showInstruction = CLI_TRUE;
   CliUtil::positionColon = 25;

   if ( BcmCli_getParameter(updateSoftwareParam) == CLI_FALSE ) {
      BcmCli_cliPrintf("\nError getting Tftp IP address info.\n");
      CliUtil::promptContinue();
      return CLI_STS_ERR_GENERAL;
   }
   sprintf(cmd, "tftp -g -t i -f %s %s", updateSoftwareParam[1].param, updateSoftwareParam[0].param);
   prctl_runCommandInShellBlocking(cmd);
   CliUtil::promptContinue();
   return CLI_STS_OK;;
#endif   
   return CMSRET_SUCCESS;
}

CmsRet menuAdminPasswdCfg(void)
{
   return CMSRET_SUCCESS;
}

CmsRet menuUserPasswdCfg(void)
{
   return CMSRET_SUCCESS;
}

CmsRet menuSupportPasswdCfg(void)
{
   return CMSRET_SUCCESS;
}

CmsRet menuDiagOam(void)
{
   return CMSRET_SUCCESS;
}

CmsRet menuRestoreReboot(void)
{
   CmsRet ret;
   UBOOL8 showInstruction = FALSE;
   PARAMS_SETTING rebootParam[] = {
      // prompt name     Default   validation function
      {"Restore default configuration and reboot?  Confirm [1-yes,2-no]",  "", cli_isValidState},
      {NULL,          "", NULL}
   };
   
   if ((ret = cli_getMenuParameters(rebootParam, showInstruction)) != CMSRET_SUCCESS)
   {
      printf("\nError getting confirmation.\n");
      return CMSRET_INVALID_ARGUMENTS;
   }
   
   if (!strcmp(rebootParam[0].param, "1"))
   {
      if ((ret = cmsLck_acquireLockWithTimeout(CLI_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get lock, ret=%d", ret);
         printf("Could not run command due to lock failure.\r\n");
         return ret;
      }
      
      cmsMgm_invalidateConfigFlash();
      
      cmsLck_releaseLock();
      
      cmsUtil_sendRequestRebootMsg(cliPrvtMsgHandle);
   }

   return CMSRET_SUCCESS;
}

CmsRet menuReboot(void)
{
   CmsRet ret;
   UBOOL8 showInstruction = FALSE;
   PARAMS_SETTING rebootParam[] = {
      // prompt name     Default   validation function
      {"Reboot?  Confirm [1-yes,2-no]",  "", cli_isValidState},
      {NULL,          "", NULL}
   };
   
   if ((ret = cli_getMenuParameters(rebootParam, showInstruction)) != CMSRET_SUCCESS)
   {
      printf("\nError getting confirmation.\n");
      return CMSRET_INVALID_ARGUMENTS;
   }
   
   if (!strcmp(rebootParam[0].param, "1"))
   {
      cmsUtil_sendRequestRebootMsg(cliPrvtMsgHandle);
   }
   
   return CMSRET_SUCCESS;
}




#endif  /* SUPPORT_CLI_MENU */
