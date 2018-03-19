/*****************************************************************************
 *
 *  Copyright (c) 2006-2008  Broadcom Corporation
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "cgi_main.h"
#include "httpd.h"
#include "devctl_xtm.h"
#include "bcmxtmcfg.h"
#ifdef BRCM_WLAN
#ifndef SUPPORT_UNIFIED_WLMNGR 
#include "wlapi.h"
#include "wlmdm.h"
#endif
#include "cgi_wl_dmx.h"
#endif

#ifdef BRCM_VOICE_SUPPORT
#include <cgi_voice.h>
#include <dal_voice.h>
#endif

#ifdef BRCM_WLAN 
#ifdef WL_IMPL_PLUS 
extern struct ej_handler __start_ej_handlers;
extern struct ej_handler __stop_ej_handlers;
#endif

int wl_handle_ej(FILE *stream,char *func,int argc,char **argv) {
	/*   Call handler */
	struct ej_handler *handler; 

#ifdef WL_IMPL_PLUS 
	for (handler = &__start_ej_handlers; handler<&__stop_ej_handlers; handler++) {
#else
			for (handler = &ej_handlers[0]; handler->pattern; handler++) {
#endif

		if (strcmp(handler->pattern, func) == 0) {
			handler->output(0, stream, argc-1, (argv+1));
			return 0;
		}   
	}   
	return -1;
}   
#endif

static void
ejGet(int argc, char **argv, FILE *stream)
{
	char value[WEB_BIG_BUF_SIZE_MAX]; // big ipsec table

	if (argc < 2)
		return;

   value[0] = '\0';
   cgiGetVar(argv[1], value, stream);

	fputs(value, stream);
}


/** based on the specified page name, refresh the relevant sections of
 * glbWebVar so that we serve up a html page with the latest values from
 * the MDM.
 */
static void
ejRefreshGlbWebVar(const char *path)
{
   char *last;

   last = strrchr(path, '/');
   if (last == NULL)
   {
      cmsLog_error("could not extract last element in %s", path);
      return;
   }
   else
   {
      if (*last == '/')
      {
         last++;
      }
      cmsLog_debug("Entered--> %s", last);
   }

   if (!strcasecmp(last, "logconfig.html"))
   {
      cmsDal_getCurrentSyslogCfg(&glbWebVar);
   }
   else if (!strcasecmp(last, "lancfg2.html"))
   {
      dalLan_getLanPageInfo(&glbWebVar);
   }
   else if (!strcasecmp(last, "diag.html"))
   {
      SINT32 i;
      if (glbDiagInfoClear != 0)
      {
         for (i = 0; i < WEB_DIAG_MAX; i++)
         {
            glbDiagInfo[i][0] = '\0';
         }
      }
      glbDiagInfoClear = 1;
   }   
#ifdef SUPPORT_TR69C
   else if (!strcasecmp(last, "tr69cfg.html"))
   {
      cmsDal_getTr69cCfg(&glbWebVar);
   }
#endif
#ifdef DMP_X_ITU_ORG_GPON_1 /* aka SUPPORT_OMCI */
   else if (!strcasecmp(last, "omcisystem.html"))
   {
      cmsDal_getOmciSystem(&glbWebVar);
   }
#endif
#if defined(DMP_TIME_1) || defined(DMP_DEVICE2_TIME_1) 
   else if (!strcasecmp(last, "sntpcfg.html"))
   {
      cmsDal_getNtpCfg(&glbWebVar);
   }
#endif
#ifdef DMP_ADSLWAN_1
   else if (!strcasecmp(last, "adslcfg.html") ||
            !strcasecmp(last, "xdslcfg.html"))
   {
      dalWan_getAdslFlags(&(glbWebVar.adslFlag));
   }
#endif
#ifdef SUPPORT_DSL_BONDING
   else if (!strcasecmp(last, "dslbondingcfg.html"))
   {
      CmsRet nRet ;
	    XTM_BOND_INFO bondInfo ;
      dalDsl_getDslBonding(&(glbWebVar.dslBonding));
	    nRet = devCtl_xtmGetBondingInfo ( &bondInfo );
	    if( nRet == CMSRET_SUCCESS )
				 glbWebVar.dslBondingStatus = 0x1 ;
			else
				 glbWebVar.dslBondingStatus = 0x0 ;
   }
#endif
#ifdef DMP_X_BROADCOM_COM_SNMP_1
   else if (!strcasecmp(last, "snmpconfig.html"))
   {
      cmsDal_getCurrentSnmpCfg(&glbWebVar);
   }
#endif /* DMP_X_BROADCOM_COM_SNMP_1 */
#ifdef SUPPORT_MOCA
   else if (!strcasecmp(last, "mocacfg.html"))
   {
      dalMoca_getCurrentCfg(&glbWebVar);
   }
#endif
#ifdef SUPPORT_LANVLAN
   else if (!strcasecmp(last, "lanvlancfg.html"))
   {
      char buf[BUFLEN_64], *p;
      if (glbWebVar.lanName[0] == '\0')	  
      {
         cgiGetAllL2EthIntf(0, NULL, buf);
         if ((p = strchr(buf, '|')) != NULL) *p = '\0';
         strncpy(glbWebVar.lanName, buf, sizeof(glbWebVar.lanName));
      }
      dalLan_getLanVlan(&glbWebVar);
   }
#endif
#ifdef DMP_X_BROADCOM_COM_ETHERNETOAM_1
   else if (!strcasecmp(last, "diagethoam.html"))
   {
      dalEthOam_getAllCfg(&glbWebVar);
   }
#endif
#ifdef SUPPORT_HOMEPLUG
   else if (!strcasecmp(last, "homeplug.html"))
   {
      dalHomeplug_getCurrentCfg(&glbWebVar);
   }
#endif
#ifdef SUPPORT_IEEE1905
    else if (!strcasecmp(last, "ieee1905cfg.html"))
    {
       dalIeee1905_getCurrentCfg(&glbWebVar);
    }
#endif
#ifdef SUPPORT_STUN
   else if (!strcasecmp(last, "stuncfg.html"))
   {
      dalStun_getStunCfg(&glbWebVar);
   }
#endif
   else if (!strcasecmp(last, "scacccntr.html") ||
            !strcasecmp(last, "upload.html") ||
            !strcasecmp(last, "uploadinfo.html") ||
            !strcasecmp(last, "wancfg.html") ||
            !strcasecmp(last, "enblbridge.html") ||
            !strcasecmp(last, "ipoacfg.html") ||
            !strcasecmp(last, "enblservice.html") ||
            !strcasecmp(last, "ifcgateway.html") ||
            !strcasecmp(last, "ifcdns.html") ||
            !strcasecmp(last, "ntwksum2.html") ||
            !strcasecmp(last, "updatesettings.html") ||
            !strcasecmp(last, "menu.html") ||
            !strcasecmp(last, "footer.html") ||
            !strcasecmp(last, "natcfg2.html") ||
            !strcasecmp(last, "pppoe.html") ||
            !strcasecmp(last, "multicast.html") ||
            !strcasecmp(last, "upnpcfg.html") ||
            !strcasecmp(last, "ddnsadd.html") ||
            !strcasecmp(last, "diagbr.html") ||
            !strcasecmp(last, "diaglan.html") ||
            !strcasecmp(last, "resetrouter.html") ||
            !strcasecmp(last, "defaultsettings.html") ||
            !strcasecmp(last, "rebootinfo.html") ||
            !strcasecmp(last, "backupsettings.html") ||
            !strcasecmp(last, "logintro.html") ||
            !strcasecmp(last, "seclogintro.html") ||
            !strcasecmp(last, "diagipow.html") ||
            !strcasecmp(last, "diagmer.html") ||
            !strcasecmp(last, "diagpppoa.html") ||
            !strcasecmp(last, "diagpppoe.html") ||
            !strcasecmp(last, "StaticIpAdd.html") ||
            !strcasecmp(last, "StaticIpErr.html") ||
            !strcasecmp(last, "berrun.html") || 
            !strcasecmp(last, "berstart.html") || 
            !strcasecmp(last, "berstop.html") || 
            !strcasecmp(last, "berstart.html") || 
            !strcasecmp(last, "statsifc.html") || 
            !strcasecmp(last, "statsadsl.html") || 
            !strcasecmp(last, "statsadslreset.html") || 
            !strcasecmp(last, "statsxtmreset.html") || 
            !strcasecmp(last, "statsmocareset.html") || 
            !strcasecmp(last, "statsmocalanreset.html") || 
            !strcasecmp(last, "statsmocawanreset.html") || 
            !strcasecmp(last, "cfgatm.html") || 
            !strcasecmp(last, "cfgptm.html") || 
            !strcasecmp(last, "logo.html") ||
            !strcasecmp(last, "containertable.html") ||
            !strcasecmp(last, "containerdetail.html") ||
            !strcasecmp(last, "qrcodesample.html"))
   {
      /* don't need to do any refreshes for these pages */
   }
#ifdef BRCM_WLAN
   /*WLAN page starts with wl*/
   else if ( !strncasecmp(last, "wl", 2) )
   {
       wlUnlockReadMdm_dmx();
   }
#endif
   else if (!strcasecmp(last,"info.html") )
   {
#ifdef BRCM_WLAN
      wlUnlockReadMdm_dmx();
#endif
      cmsDal_getAllVersionInfo(&glbWebVar);
   }
   else 
   {
      cmsLog_notice("refresh for page %s not implemented yet!", last );
   }


   cmsLog_debug("Exit--> %s", last);

   return;
}





 
#ifdef BRCM_WLAN
int wlshow = 0;
#ifdef BCMWAPI_WAI
   extern int BcmWapi_GetCertList(int argc, char **argv, FILE *wp);
#endif
static void ejGetWireless(int argc, char **argv, FILE *stream)
{
	char value[WEB_BIG_BUF_SIZE_MAX_WLAN]; // country list is big

	if (argc < 2)
		return;

        value[0] = '\0';

        if(argc > 2) {
   		BcmWl_GetVarEx_dmx(argc, argv, value);
        }
        else  {
#ifdef BCMWAPI_WAI
	      if (strcmp(argv[1], "wlWapiCertList") == 0) {
             /* Write large list directly */
             BcmWapi_GetCertList(argc, argv, stream);
             return;
          }
#endif
          if (strcmp(argv[1],"wlshow"))
              BcmWl_GetVar_dmx(argv[1], value);
           else
              sprintf(value,"%d", wlshow);
        }

	fputs(value, stream);
}


extern char *wlGetTargetPage(void);
extern char *wlGetRefreshPage(void);

static void
ejGetWirelessPage(int argc  __attribute__((unused)),
                  char **argv  __attribute__((unused)),
                  FILE *stream)
{
        fputs(wlGetTargetPage(), stream);
}
static void
ejGetWirelessRefresh(int argc  __attribute__((unused)),
                     char **argv  __attribute__((unused)),
                     FILE *stream)
{
       fputs(wlGetRefreshPage(), stream);
}

#endif  /* BRCM_WLAN */


#ifdef BRCM_VOICE_SUPPORT
static void
ejGetVoice(int argc, char **argv, FILE *stream)
{
	char value[WEB_BUF_SIZE_MAX];

   cmsLog_debug("\n     ***** %s %d argv[1] = %s \n", __FUNCTION__, __LINE__, argv[1]);

   if (argc < 2) return;

   value[0] = '\0';
   cgiGetVarVoice(argv[1], value);
	fputs(value, stream);
}
#endif

static void
ejGetTest(int argc, char **argv, FILE *stream)
{
	char value[WEB_BUF_SIZE_MAX];

	if (argc < 2)
		return;

   value[0] = '\0';
   cgiGetTestVar(argv[1], value);

	fputs(value, stream);
}

#ifdef DMP_X_BROADCOM_COM_SELT_1
static void
ejGetSelt(int argc, char **argv, FILE *stream)
{
	char value[WEB_BUF_SIZE_MAX];

	if (argc < 2)
		return;

   value[0] = '\0';
   cgiGetSeltVar(argv[1], value);

	fputs(value, stream);
}

static void
ejGetSeltResult(int argc __attribute__((unused)), char **argv __attribute__((unused)), FILE *stream)
{
   cgiPrintSeltResult(NULL,stream);
}
#endif /* #ifdef DMP_X_BROADCOM_COM_SELT_1 */

static void
ejGetOther(int argc, char **argv, FILE *stream)
{
	char value[WEB_BUF_SIZE_MAX];

	if (argc < 2) return;

   /*
    * mwang: this is a very dangerous interface.  We are passing
    * in a value buf of length WEB_BUF_SIZE_MAX, but the callee does
    * not know the length, so the callee could easily overwrite
    * this buffer.  Need to make this interface safer, or better yet,
    * get rid of all these special ejGetXXX functions.
    */
   value[0] = '\0';
   value[WEB_BUF_SIZE_MAX-1]='z';
   value[WEB_BUF_SIZE_MAX-2]='z';
   value[WEB_BUF_SIZE_MAX-3]='z';
   value[WEB_BUF_SIZE_MAX-4]='z';

   cgiGetVarOther(argc, argv, value);

   if (value[WEB_BUF_SIZE_MAX-1]!='z' ||
       value[WEB_BUF_SIZE_MAX-2]!='z' ||
       value[WEB_BUF_SIZE_MAX-3]!='z' ||
       value[WEB_BUF_SIZE_MAX-4]!='z')
   {
      cmsLog_error("Buffer overflow detected.  Increase WEB_BUF_SIZE_MAX "
                   "(currently at %d)", WEB_BUF_SIZE_MAX);
   }


	fputs(value, stream);
}

static void
ejFncCmd(int argc, char **argv, FILE *stream  __attribute__((unused)))
{
	if (argc < 2) return;

   cgiFncCmd(argc, argv);
}
#ifdef BRCM_WLAN 
struct cpe_ej_handler cpe_ej_handlers[] = {
#else
struct ej_handler ej_handlers[] = {
#endif
	{ "ejGet", ejGet },
#if BRCM_WLAN
	{ "ejGetWl", ejGetWireless },
	{ "ejGetWirelessPage", ejGetWirelessPage },
	{ "ejGetWirelessRefresh", ejGetWirelessRefresh },	
#endif
#ifdef BRCM_VOICE_SUPPORT
	{ "ejGetVoice", ejGetVoice },
#endif
	{ "ejGetTest", ejGetTest },
	{ "ejGetOther", ejGetOther },
#ifdef DMP_X_BROADCOM_COM_SELT_1
	{ "ejGetSelt", ejGetSelt },
	{ "ejGetSeltResult", ejGetSeltResult },
#endif
	{ "ejFncCmd", ejFncCmd },
	{ NULL, NULL }
};

static char * get_arg(char *args, char **next);
static void call(char *func, FILE *stream);

static char *
get_arg(char *args, char **next)
{
	char *arg, *end;

	/* Parse out arg, ... */
	if (!(end = strchr(args, ','))) {
		end = args + strlen(args);
		*next = NULL;
	} else
		*next = end + 1;

	/* Skip whitespace and quotation marks on either end of arg */
	for (arg = args; isspace(*arg) || *arg == '"'; arg++);
	for (*end-- = '\0'; isspace(*end) || *end == '"'; end--)
		*end = '\0';

	return arg;
}

static void
call(char *func, FILE *stream)
{
	char *args, *end, *next;
	int argc;
	char * argv[16];
#ifdef BRCM_WLAN 
	struct cpe_ej_handler *handler;
	int handled=0;
#else
	struct ej_handler *handler;
#endif
	/* Parse out ( args ) */
	if (!(args = strchr(func, '(')))
		return;
	if (!(end = strchr(func, ')')))
		return;
	*args++ = *end = '\0';

	/* Set up argv list */
	argv[0] = func;
	for (argc = 1; argc < 16 && args; argc++, args = next) {
		if (!(argv[argc] = get_arg(args, &next)))
			break;
	}

	/* Call handler */
#ifdef BRCM_WLAN 
	for (handler = &cpe_ej_handlers[0]; handler->pattern; handler++) {
#else
	for (handler = &ej_handlers[0]; handler->pattern; handler++) {
#endif
		if (strcmp(handler->pattern, func) == 0) {
			handler->output(argc, argv, stream);
#ifdef BRCM_WLAN 
			handled=1;
#endif
			break;
		}
	}
#ifdef BRCM_WLAN 
	if(!handled) 	wl_handle_ej(stream,func,argc,argv);
#endif

}

void
do_ej(char *path, FILE *stream)
{
   FILE *fp;
   int c;
   char pattern[256], *func = NULL, *end = NULL;
   int len = 0;

   char fullPath[CMS_MAX_FULLPATH_LENGTH]={0};
#ifdef BRCM_WLAN 
   char fullPath_t[CMS_MAX_FULLPATH_LENGTH]={0};
   
   if(strncmp(path,"/web",4)) {
     makePathToWebPage(fullPath_t, sizeof(fullPath_t),path );
     path=fullPath_t;
   }
#endif


   /*
    * Some call sequences already prepended the correct DESKTOP prefix
    * and some call sequences don't.  So check for it here.
    */
   if (path[0] == '/' && path[1] == 'w' && path[2] == 'e' && path[3] == 'b')
   {
      CmsRet ret;
      if ((ret = cmsUtl_getRunTimePath(path, fullPath, sizeof(fullPath))) != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not find root dir, ret=%d", ret);
         return;
      }
   }
   else
   {
      cmsUtl_strncpy(fullPath, path, sizeof(fullPath));
   }

   cmsLog_debug("opening file %s", fullPath);

   if (!(fp = fopen(fullPath, "r")))
   {
      /*
       * Some httpd code will set the path to empty when an error message
       * is sent back to the web browser for display.  So don't treat that
       * as an unexpected error here.
       */
      if (path[0] != '\0')
      {
         cmsLog_error("Could not open %s", fullPath);
      }
      return;
   }

   ejRefreshGlbWebVar(path);

   /*
    * do_ej basically scans the file for special symbols, does look-ups
    * in the MDM and varous other tables to replace the special symbols
    * with their values, and sends the variable-substituted page back
    * to the user.
    */
	while ((c = getc(fp)) != EOF) {

		/* Add to pattern space */
		pattern[len++] = c;
		pattern[len] = '\0';
		if (len == (sizeof(pattern) - 1))
			goto release;

		/* Look for <% ... */
		if (!func && strncmp(pattern, "<%", strlen(pattern)) == 0) {
			if (strlen(pattern) == 2)
				func = pattern + 2;
			continue;
		}

		/* Look for the function call */
		if (func) {
			if ((end = strstr(func, "%>"))) {
				/* Skip whitespace on either end of function call */
				for (; isspace(*func); func++);
				for (*end-- = '\0'; isspace(*end); end--)
					*end = '\0';

				/* Call function */
				call(func, stream);
				func = NULL;
				len = 0;
			}
			continue;
		}

	release:
		/* Release pattern space */
		fputs(pattern, stream);
		func = NULL;
		len = 0;
	} /* end of while */

	fclose(fp);

   return;
}
