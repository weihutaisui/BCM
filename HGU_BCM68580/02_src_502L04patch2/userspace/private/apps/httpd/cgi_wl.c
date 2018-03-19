/*
* <:copyright-BRCM:2011:proprietary:standard
* 
*    Copyright (c) 2011 Broadcom 
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
:>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>

#include "httpd.h"
#include "cgi_main.h"
#include "cgi_wl.h"
#include "cgi_wl_dmx.h"

#include <bcmnvram.h>
#ifndef SUPPORT_UNIFIED_WLMNGR 
#include "wlapi.h"
#include "wldefs.h"
#endif


#include "cms.h"
#include "cms_log.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"
#include "cms_dal.h"

#ifndef SUPPORT_UNIFIED_WLMNGR 
#include "wlmdm.h"
#endif

#ifdef SUPPORT_WLAN_VISUALIZATION
#include <vis_gui.h>
#endif

#define IFC_LARGE_LEN 264

//#define SUPPORT_WDS_WEP

int index_wl =0;

static  char wltargetPage[WL_MID_SIZE_MAX<<1]= {0};
static     char wlrefreshPage[WL_MID_SIZE_MAX<<1]= {0};

void wlSetTargetPage(char *page)
{
    if(strlen(page) >= (WL_MID_SIZE_MAX<<1)) {
        wltargetPage[0]='\0';
        return;
    }
    strcpy(wltargetPage, page);
}

char *wlGetTargetPage(void)
{
    return wltargetPage;
}

void wlSetRefreshPage(char *page)
{
    if(strlen(page) >= (WL_MID_SIZE_MAX<<1)) {
        wltargetPage[0]='\0';
        return;
    }
    strcpy(wlrefreshPage, page);
}

char *wlGetRefreshPage(void)
{
    return wlrefreshPage;
}

void cgiRefreshPage(char *page, char *refreshPage, FILE *fs)
{
    if(*page == '\0' || *refreshPage == '\0') {
        printf("cgiRefreshPage: parameters not found\n");
        return;
    }

    wlSetTargetPage(page);
    wlSetRefreshPage(refreshPage);
    BcmWl_SetVar_dmx("wlRefresh", "1");
    do_ej(page, fs);

}

void wlParseSetUrl(char *path)
{
    char *query = strchr(path, '?');
    char *name, *value, *next;

    if (query) {
        for (value = ++query; value; value = next) {
            name = strsep(&value, "=");
            if (name) {
                next = value;
                value = strsep(&next, "&");
                if (!value) {
                    value = next;
                    next = NULL;
                }
                cgiUrlDecode(value);
                BcmWl_SetVar_dmx(name, value);
            } else
                next = NULL;
        }
    }
}


#ifdef SUPPORT_WLAN_VISUALIZATION


void do_wl_json_set(char *path, FILE *fs,int len,const char *boundry)
{

    vis_do_json_set(path,fs,len,boundry);

}


void do_wl_json_get(char *path, FILE *fs)
{
    vis_do_json_get(path,fs);
}



void do_wl_vis_dbdownload(char *path, FILE *fs)
{
  vis_do_visdbdwnld_cgi(path,fs);
}

#endif

void do_wl_status_get(char *path, FILE *fs)
{
    if(wlcsm_nvram_get("_wlrestart_"))
        fputs("0",fs);
    else
        fputs("1",fs);
}
void do_wl_cgi(char *path, FILE *fs)
{
    char filename[WEB_BUF_SIZE_MAX];
    char* query = NULL;
    char* ext = NULL;
    char *beginPtr=NULL;
    int offset=0;
    int doSetup = FALSE;
    char tmpBuf[BUFLEN_64];
#ifdef SUPPORT_QUICKSETUP
    int doQuickSetup = FALSE;
#endif

    cmsLog_debug("Enter: path=%s", path);

    query = strchr(path, '?');

    cgiGetValueByName(query, "sessionKey", tmpBuf);
    cgiUrlDecode(tmpBuf);
    cgiSetVar("sessionKey", tmpBuf);

    if ( query != NULL ) {
        /*
         * Parse the path, which contains the values set by the user on
         * the WebUI.  Set them into the MDM.
         */
        wlUnlockReadMdm_dmx();
        wlParseSetUrl(path);
    }

    filename[0] = '\0';

    /*
     * On real system, path begins with /webs/, but on DESKTOP_LINUX, path
     * has some linux fs path then /webs/.  Find the offset.  On real system,
     * offset will be 0.
     */
    beginPtr = strstr(path, "/webs/");
    if (beginPtr == NULL) {
        cmsLog_error("Could not find /webs/ prefix in path %s", path);
        /* we could not find the expected /webs prefix, complain but still
         * use it as is.
         */
        beginPtr = path;
    }
    offset = beginPtr - path;

    ext = strchr(&(path[offset]), '.');
    if ( ext != NULL ) {
        *ext = '\0';
        strcpy(filename, &(path[offset]));
        cmsLog_debug("filename=%s", filename);

        // setup and store configurations
        if ( strcmp(filename, "/webs/wlcfgadv") == 0 ) {
            wlSetRefreshPage("wlcfgadvrefresh.wl?wlRefresh=0");
            doSetup = TRUE;
        } else if ( strcmp(filename, "/webs/wlcfg") == 0 ) {
            wlSetRefreshPage("wlcfgrefresh.wl?wlRefresh=0");
            doSetup = TRUE;

        } else if ( strcmp(filename, "/webs/wlsecurity") == 0 ) {
            wlSetRefreshPage("wlsecrefresh.wl?wlRefresh=0");
            doSetup = TRUE;

        } else if ( strcmp(filename, "/webs/wlmacflt" ) == 0 ) {
            wlSetRefreshPage("wlcfgadvrefresh.wl?wlRefresh=0");
            doSetup = TRUE;

#ifdef SUPPORT_SES
        } else if ( strcmp(filename, "/webs/wlses") == 0 ) {
            wlSetRefreshPage("wlsesrefresh.wl?wlRefresh=0");
            doSetup = TRUE;
#endif
        } else if ( strcmp(filename, "/webs/wlsecrefresh") == 0 ) {
            // refresh wlsecurity.html page
            strcpy(filename, "/webs/wlsecurity");
        } else if ( strcmp(filename, "/webs/wlcfgrefresh") == 0 ) {
            // refresh wlcfg.html page

            strcpy(filename, "/webs/wlcfg");

        } else if ( strcmp(filename, "/webs/wlcfgadvrefresh") == 0 ) {
            // refresh wlcfgadv.html page
            strcpy(filename, "/webs/wlcfgadv");
#ifdef SUPPORT_SES
        } else if ( strcmp(filename, "/webs/wlsesrefresh") == 0 ) {
            // refresh wlses.html page
            strcpy(filename, "/webs/wlses");
#endif

#ifdef SUPPORT_QUICKSETUP
        } else if ( strcmp(filename, "/webs/quicksetup") == 0 ) {
            doQuickSetup = TRUE;
#endif
        }


        if( strstr(filename, "/webs/wlswitchinterface") != NULL ) {

            sscanf(filename, "/webs/wlswitchinterface%d", &index_wl);
            BcmWl_Switch_instance_dmx(index_wl);
            strcpy(filename, "/webs/wlcfg");
        }

        if( strstr(filename, "/webs/wlshow") != NULL ) {
            extern int wlshow;
            wlshow = 1;
            strcpy(filename, "/webs/wlcfgadv");
        }

        strcat(filename, ".html");

        if(doSetup) {

            if(cgiValidateSessionKey(fs) != CMSRET_SUCCESS) {
                /*sessionkey validation failed*/
                return;
            }


            wlSetTargetPage(filename);

            /* replace output page with wlrefresh.html */
            strcpy(filename, "/webs/wlrefresh.html");
            glbSaveConfigNeeded = GLBCONFIGNEEDED_DMX;
            BcmWl_SetVar_dmx("wlRefresh", "1");

            /* write back to mdm */
            wlUnlockWriteMdmOne_dmx(index_wl);
        }
#ifdef SUPPORT_QUICKSETUP
        else if(doQuickSetup == TRUE) {
            /* write back to mdm */
            if(wlUnlockWriteMdmOne_dmx(index_wl) == CMSRET_SUCCESS) {
                //do_ej("/webs/quicksetuptestsucc.html", fs);
            } else {
                glbWebVar.quicksetupErrCode = QS_WLAN_ADD_ERROR;
            }
            return;
        }
#endif

        do_ej(filename, fs);
    }
}

void cgiWlMacFlt(char *query, FILE *fs)
{
    char action[IFC_LARGE_LEN];
    char idx[WL_MID_SIZE_MAX];

    idx[0]='\0';
    cgiGetValueByName(query, "action", action);

    if ( strcmp(action, "add") == 0 )
        cgiWlMacFltAdd(query, fs);
    else if ( strcmp(action, "remove") == 0 )
        cgiWlMacFltRemove(query, fs);
    else if ( strcmp(action, "save") == 0 )
        cgiWlMacFltSave(query, fs);
    else if ( strcmp(action, "refresh") == 0 ) {
        //read datastructure
        wlUnlockReadMdm_dmx();
        if(cgiGetValueByName(query, "wlSsidIdx", idx) == CGI_STS_OK && idx[0]) {
            BcmWl_SetVar_dmx("wlSsidIdx", idx);
        }
        BcmWl_SetVar_dmx("wlRefresh", "0");
        cgiWlMacFltView(fs);
    } else {
        //read datastructure
        wlUnlockReadMdm_dmx();
        cgiWlMacFltView(fs);
    }
}

void cgiWlMacFltAdd(char *query, FILE *fs)
{
    char cmd[IFC_LARGE_LEN];
    WL_STATUS sts = WL_STS_OK;
    char mac[WL_MID_SIZE_MAX];
    char ssid[WL_SSID_SIZE_MAX];
    char ifcName[WL_SM_SIZE_MAX];
    int i = 0;

    cmd[0] = '\0';
    mac[0] = '\0';
    ssid[0] = '\0';
    ifcName[0]='\0';

    cgiGetValueByName(query, "wlFltMacAddr", mac);
    // convert lower case to upper case to treat
    // 00:11:22:33:44:aa the same as 00:11:22:33:44:AA
    for ( i = 0; mac[i] != '\0'; i++ )
        mac[i] = toupper(mac[i]);

    BcmWl_GetVar_dmx("wlSsid", ssid);
    BcmWl_GetVar_dmx("wlCurIdxIfcName", ifcName);


    sts = (WL_STATUS) BcmWl_addFilterMac_dmx(mac, ssid, ifcName);
    if ( sts != WL_STS_OK ) {
        sprintf(cmd, "Add wireless MAC filter for %s failed. " \
                "Status: %d.", mac, sts);
        cgiWriteMessagePage(fs, "Wireless MAC Filter Add Error", cmd,
                            "wlmacflt.cmd?action=view");
    } else {
        wlUnlockWriteMdmOne_dmx(MARK_IDX(index_wl));
        glbSaveConfigNeeded = GLBCONFIGNEEDED_DMX;
        cgiRefreshPage("/webs/wlrefresh.html","wlmacflt.cmd?action=view", fs);
    }

}

void cgiWlMacFltRemove(char *query, FILE *fs)
{
    char *pToken = NULL, *pLast = NULL;
    char lst[WEB_BUF_SIZE_MAX], cmd[IFC_LARGE_LEN];
    WL_STATUS sts = WL_STS_OK;

    char curIfcName[WL_SM_SIZE_MAX];
    cgiGetValueByName(query, "rmLst", lst);
    pToken = strtok_r(lst, ", ", &pLast);
    BcmWl_GetVar_dmx("wlCurIdxIfcName", curIfcName);
    while ( pToken != NULL ) {
        sts = (WL_STATUS) BcmWl_removeFilterMac_dmx(pToken, curIfcName);
        if ( sts != WL_STS_OK ) break;
        pToken = strtok_r(NULL, ", ", &pLast);
    }

    if ( sts == WL_STS_OK ) {
        wlUnlockWriteMdmOne_dmx(index_wl);
        glbSaveConfigNeeded = GLBCONFIGNEEDED_DMX;
        cgiRefreshPage("/webs/wlrefresh.html","wlmacflt.cmd?action=view", fs);
    } else {
        sprintf(cmd, "Cannot remove wireless mac filter for %s.<br>" \
                "Status: %d.", pToken, sts);
        cgiWriteMessagePage(fs, "Wireless MAC Filter Remove Error", cmd,
                            "wlmacflt.cmd?action=view");
    }
}

void cgiWlMacFltSave(char *query, FILE *fs)
{
    char mode[IFC_LARGE_LEN];
    char tmp[WL_SM_SIZE_MAX];

    cgiGetValueByName(query, "wlFltMacMode", mode);
    BcmWl_SetVar_dmx("wlFltMacMode", mode);

    /* WPS_V2: when 'allow' && MacFilterList empty, to force wps disabled */
    BcmWl_GetVar_dmx("wlWscVer2", tmp);
    if ((strcmp(tmp, "enabled") == 0) && (strcmp(mode, "allow") == 0) && BcmWl_isMacFltEmpty_dmx()) {
        sprintf(tmp, "disabled");
        BcmWl_SetVar_dmx("wlWscMode", tmp);
    }

    wlUnlockWriteMdmOne_dmx(MARK_IDX(index_wl));
    glbSaveConfigNeeded = GLBCONFIGNEEDED_DMX;

    cgiRefreshPage("/webs/wlrefresh.html","wlmacflt.cmd?action=view", fs);
}

#if !defined(SUPPORT_TR181_WLMNGR)
/* this is the orginal way of dumping the filter macs table */
void wlPrintFilterMacs(FILE *fs, char *supportMbss, char *idx, char *curIfcName)
{
    char ifcName[WL_SM_SIZE_MAX];
    char mac[WL_MID_SIZE_MAX];
    void *node=NULL;

    ifcName[0]='\0';

    node = BcmWl_getFilterMac(node, mac, NULL, ifcName);

    while ( node != NULL) {
        if(!atoi(supportMbss)|| (atoi(idx)== MAIN_BSS_IDX && *ifcName=='\0') || (strcmp(curIfcName,ifcName)==0)) {
            fprintf(fs, "   <tr>\n");
            fprintf(fs, "      <td>%s</td>\n", mac);
            fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n", mac);
            fprintf(fs, "   </tr>\n");
        }
        node = BcmWl_getFilterMac(node, mac, NULL, ifcName);
    }
}
#endif  /* DMP_WIFILAN_1 */

#ifdef DMP_DEVICE2_WIFIACCESSPOINT_1
void wlPrintFilterMacs_dev2(FILE *fs, char *supportMbss, char *idx, char *curIfcName);
#endif  /* DMP_DEVICE2_WIFIACCESSPOINT_1 */

void cgiWlMacFltView(FILE *fs)
{
    char value[WEB_BUF_SIZE_MAX];
    char curIfcName[WL_SM_SIZE_MAX];
    char idx[WL_MID_SIZE_MAX];
    char supportMbss[WL_SM_SIZE_MAX];
    char tmp[WL_SM_SIZE_MAX];

    curIfcName[0]='\0', supportMbss[0]='\0';

    writeWlMacFltScript(fs);

    fprintf(fs, "<b>Wireless -- MAC Filter</b><br><br>\n");
    fprintf(fs, "<table border='0' cellpadding='4' cellspacing='0'>\n");
    fprintf(fs, "   <tr>\n");

    BcmWl_GetVar_dmx("wlSupportMbss", supportMbss);

    if(atoi(supportMbss)) {
        fprintf(fs, "   <td> Select SSID: </td>\n");
        fprintf(fs, "   <td><select name='wlSsidIdx' onChange='pageRefresh()' > \n");
        BcmWl_GetVar_dmx("wlSsidList", value);
        fprintf(fs, "   %s\n", value);

        fprintf(fs, "   </select></td>\n");
        fprintf(fs, "   </tr>\n");
        fprintf(fs, "</table><br><br>\n");
        fprintf(fs, "<table border='0' cellpadding='4' cellspacing='0'>\n");
        fprintf(fs, "   <tr>\n");
    }

    fprintf(fs, "      <td>MAC Restrict Mode:</td>\n");
    fprintf(fs, "      <td><input type='radio' name='wlFltMacMode' onClick='modeClick()'>&nbsp;&nbsp;Disabled</td>\n");
    fprintf(fs, "      <td><input type='radio' name='wlFltMacMode' onClick='modeClick()'>&nbsp;&nbsp;Allow</td>\n");
    fprintf(fs, "      <td><input type='radio' name='wlFltMacMode' onClick='modeClick()'>&nbsp;&nbsp;Deny</td>\n");
    BcmWl_GetVar_dmx("wlWscVer2", tmp);
    if (strcmp(tmp, "enabled") == 0)
        fprintf(fs, "      <td>&nbsp;&nbsp Note: If 'allow' is choosed and mac filter is empty, WPS will be disabled</td>\n");
    fprintf(fs, "   </tr>\n");
    fprintf(fs, "</table><br><br>\n");

    fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
    // write table header
    fprintf(fs, "   <tr>\n");
    fprintf(fs, "      <td class='hd'>MAC Address</td>\n");
    fprintf(fs, "      <td class='hd'>Remove</td>\n");
    fprintf(fs, "   </tr>\n");

    if(atoi(supportMbss)) {
        // write table body
        BcmWl_GetVar_dmx("wlCurIdxIfcName", curIfcName);
        BcmWl_GetVar_dmx("wlSsidIdx", idx);
    }

#if defined(SUPPORT_TR181_WLMNGR)
    wlPrintFilterMacs_dev2(fs, supportMbss, idx, curIfcName);
#else
    wlPrintFilterMacs(fs, supportMbss, idx, curIfcName);
#endif

    fprintf(fs, "</table><br><br>\n");
    fprintf(fs, "<input type='button' onClick='addClick()' value='Add'>\n");
    fprintf(fs, "<input type='button' onClick='removeClick(this.form.rml)' value='Remove'>\n");
    fprintf(fs, "</center>\n");
    fprintf(fs, "</form>\n");
    fprintf(fs, "</blockquote>\n");
    fprintf(fs, "</body>\n");
    fprintf(fs, "</html>\n");
    fflush(fs);
}

void writeWlMacFltScript(FILE *fs)
{
    char mode[WL_MID_SIZE_MAX], enbl[WL_MID_SIZE_MAX];
    char refresh[WL_SM_SIZE_MAX];
    char idx[WL_MID_SIZE_MAX];
    char supportMbss[WL_SM_SIZE_MAX];
    char syncNvram[WL_SM_SIZE_MAX];

    BcmWl_GetVar_dmx("wlSyncNvram", syncNvram);
    BcmWl_GetVar_dmx("wlRefresh", refresh);
    BcmWl_GetVar_dmx("wlSupportMbss", supportMbss);

    fprintf(fs, "<html><head>\n");
    fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
    fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
    fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");

    fprintf(fs, "<script language='javascript' src='util.js'></script>\n");
    fprintf(fs, "<script language='javascript'>\n");
    fprintf(fs, "<!-- hide\n\n");

    /*generate new session key in glbCurrSessionKey*/
    cgiGetCurrSessionKey(0,NULL,NULL);


    fprintf(fs, "var wlRefresh = '%d'; \n\n", atoi(refresh));
    fprintf(fs, "if ( wlRefresh == '1' ) { \n");
    fprintf(fs, "   var code = 'location=\"wlmacflt.cmd?action=refresh&sessionKey=%d\"';\n",glbCurrSessionKey);
    fprintf(fs, "   eval(code);\n");
    fprintf(fs, "}\n\n");

    fprintf(fs, "function pageRefresh() {\n");
    fprintf(fs, "   with ( document.forms[0] ) {\n");
    fprintf(fs, "   var loc = 'wlmacflt.cmd?action=refresh' \n");
    fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
    fprintf(fs, "      var code = 'location=\"' + loc \n");
    if(atoi(supportMbss)) {
        fprintf(fs, "                    + submitSelect(wlSsidIdx) + '\"';\n");
    }
    fprintf(fs, "                    + '\"';\n");
    fprintf(fs, "      eval(code);\n");
    fprintf(fs, "   }\n");
    fprintf(fs, "}\n\n");

    BcmWl_GetVar_dmx("wlSsidIdx", idx);
    BcmWl_GetVar_dmx(WL_IFC_ENABLED, enbl);
    BcmWl_GetVar_dmx("wlFltMacMode", mode);
    fprintf(fs, "function frmLoad() {\n");
    fprintf(fs, "   with ( document.forms[0] ) {\n");
    if(atoi(supportMbss)) {
        fprintf(fs, "      setSelect(wlSsidIdx, '%s');\n", idx);
    }
    fprintf(fs, "      if ( '%s' == 'disabled' )\n", mode);
    fprintf(fs, "         wlFltMacMode[0].checked = true;\n");
    fprintf(fs, "      else if ( '%s' == 'allow' )\n", mode);
    fprintf(fs, "         wlFltMacMode[1].checked = true;\n");
    fprintf(fs, "      else\n");
    fprintf(fs, "         wlFltMacMode[2].checked = true;\n");
    fprintf(fs, "   }\n");
    fprintf(fs, "}\n\n");

    fprintf(fs, "function addClick() {\n");
    fprintf(fs, "   if ( '%s' == '0' ) {\n", enbl);
    fprintf(fs, "      alert('Cannot add MAC filter address since wireless is currently disabled.');\n");
    fprintf(fs, "      return;\n");
    fprintf(fs, "   }\n\n");
    fprintf(fs, "   var loc = 'wlmacflt.html';\n\n");
    fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
    fprintf(fs, "   eval(code);\n");
    fprintf(fs, "}\n\n");

    fprintf(fs, "function removeClick(rml) {\n");
    fprintf(fs, "   if ( '%s' == '0' ) {\n", enbl);
    fprintf(fs, "      alert('Cannot remove MAC filter address since wireless is currently disabled.');\n");
    fprintf(fs, "      return;\n");
    fprintf(fs, "   }\n\n");
    fprintf(fs, "   var lst = '';\n");
    fprintf(fs, "   if (rml.length > 0)\n");
    fprintf(fs, "      for (i = 0; i < rml.length; i++) {\n");
    fprintf(fs, "         if ( rml[i].checked == true )\n");
    fprintf(fs, "            lst += rml[i].value + ', ';\n");
    fprintf(fs, "      }\n");
    fprintf(fs, "   else if ( rml.checked == true )\n");
    fprintf(fs, "      lst = rml.value;\n");

    fprintf(fs, "   var loc = 'wlmacflt.cmd?action=remove&rmLst=' + lst;\n\n");
    fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
    fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
    fprintf(fs, "   eval(code);\n");
    fprintf(fs, "}\n\n");

    fprintf(fs, "function modeClick() {\n");
    fprintf(fs, "   var loc = 'wlmacflt.cmd?action=save';\n\n");
    fprintf(fs, "   if ( '%s' == '0' ) {\n", enbl);
    fprintf(fs, "      alert('Cannot change MAC filter mode since wireless is currently disabled.');\n");
    fprintf(fs, "      return;\n");
    fprintf(fs, "   }\n\n");
    fprintf(fs, "   with ( document.forms[0] ) {\n");
    fprintf(fs, "      if ( wlFltMacMode[0].checked == true )\n");
    fprintf(fs, "         loc += '&wlFltMacMode=disabled';\n");
    fprintf(fs, "      else if ( wlFltMacMode[1].checked == true )\n");
    fprintf(fs, "         loc += '&wlFltMacMode=allow';\n");
    fprintf(fs, "      else\n");
    fprintf(fs, "         loc += '&wlFltMacMode=deny';\n");
    fprintf(fs, "   }\n\n");
    fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
    fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
    fprintf(fs, "   eval(code);\n");
    fprintf(fs, "}\n\n");
    fprintf(fs, "// done hiding -->\n");
    fprintf(fs, "</script>\n");

    fprintf(fs, "</head>\n<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
}

void cgiWlWds(char *query, FILE *fs)
{
    char action[IFC_LARGE_LEN];

    cgiGetValueByName(query, "action", action);

    if ( strcmp(action, "save") == 0 )
        cgiWlWdsSave(query, fs);
    else if ( strcmp(action, "refresh") == 0 ) {
        //read datastructure
        wlUnlockReadMdm_dmx();
        BcmWl_SetVar_dmx("wlRefresh", "0");
        cgiWlWdsView(query, fs);
    }  else  {
        //read datastructure
        wlUnlockReadMdm_dmx();
        cgiWlWdsView(query, fs);
    }
}

#if defined(DMP_WIFILAN_1) && !defined(SUPPORT_UNIFIED_WLMNGR)
/* This is the original way of dumping WDS macs */
void cgiWlPrintWdsMacs(FILE *fs)
{
    char mac[WL_MID_SIZE_MAX];
    char ssid[WL_SSID_SIZE_MAX];
    void *node = NULL;

    node = BcmWl_getScanWdsMacSSID(node, mac, ssid);

    while ( node != NULL ) {
        fprintf(fs, "         <tr>\n");
        if ( BcmWl_IsWdsMacConfigured(mac))
            fprintf(fs, "            <td align=center><input type='checkbox' checked name='scanwds' value='%s'></td>\n", mac);
        else
            fprintf(fs, "            <td align=center><input type='checkbox' name='scanwds' value='%s'></td>\n", mac);
        fprintf(fs, "            <td> %s &nbsp</td>\n", ssid);
        fprintf(fs, "            <td>%s </td>\n", mac);
        fprintf(fs, "         </tr>\n");
        node = BcmWl_getScanWdsMacSSID(node, mac, ssid);
    }
}
#endif

#ifdef DMP_DEVICE2_WIFIACCESSPOINT_1
/* this is new way of dumping WDS macs */
void cgiWlPrintWdsMacs_dev2(FILE *fs)
{
    BcmWl_print_wdsscanlist(fs);
}
#endif  /* DMP_DEVICE2_WIFIACCESSPOINT_1 */

#define WDS_MAC_ADDR    "wlWdsMacAddr"
#define WDS_MODE        "wlMode"
#define WDS_LAZYWDS     "wlLazyWds"

void cgiWlWdsView(char *query, FILE *fs)
{
    char mode[WL_SM_SIZE_MAX];
    char lazywds[WL_SM_SIZE_MAX];
    char *pToken = NULL;

    writeWlWdsScript(query, fs);
    fprintf(fs, "<b>Wireless -- Bridge</b><br><br>\n");
    fprintf(fs, "This page allows you to configure wireless bridge features of the wireless LAN\n");
    fprintf(fs, "interface. Select Disabled in Bridge Restrict which disables wireless bridge\n");
    fprintf(fs, "restriction. Any wireless bridge will be granted access. Selecting Enabled or\n");
    fprintf(fs, "Enabled(Scan) enables wireless bridge restriction. Only those bridges selected in\n");
    fprintf(fs, "Remote Bridges will be granted access.<br>\n");
    fprintf(fs, "Click \"Refresh\" to update the remote bridges.  Wait for few seconds to update.<br>\n");
    fprintf(fs, "Click \"Apply/Save\" to configure the wireless bridge options.<br><br>\n");
    fprintf(fs, "<div id='apMode'>\n");
    fprintf(fs, "<table border='0' cellpadding='4' cellspacing='0'>\n");
    fprintf(fs, "   <tr>\n");
    fprintf(fs, "      <td width='200'>AP Mode:</td>\n");
    fprintf(fs, "      <td><select name='wlMode' size=\"1\" onChange=\"wl_recalc(0);\">\n");
    fprintf(fs, "            <option value=\"ap\">Access Point</option>\n");
    fprintf(fs, "            <option value=\"wds\">Wireless Bridge</option>\n");
    fprintf(fs, "      </select></td>\n");
    fprintf(fs, "   </tr>\n");
    fprintf(fs, "</table>\n");
    fprintf(fs, "</div>\n");
    fprintf(fs, "<table border='0' cellpadding='4' cellspacing='0'>\n");
    fprintf(fs, "   <tr>\n");
    fprintf(fs, "      <td width='200'>Bridge Restrict:</td>\n");
    fprintf(fs, "      <td><select name='wllazywds' size='1' onChange='BridgeRestrictChange(0);'>\n");
    fprintf(fs, "            <option value=\"0\">Enabled</option>\n");
    fprintf(fs, "            <option value=\"2\">Enabled(Scan)</option>\n");
    fprintf(fs, "            <option value=\"1\">Disabled</option>\n");
    fprintf(fs, "      </select></td>\n");
    fprintf(fs, "   </tr>\n");

#ifdef SUPPORT_WDS_WEP
    fprintf(fs, "   <tr>\n");
    fprintf(fs, "      <td rowspan=4 valign=top>Bridge Security:</td>\n");
    fprintf(fs, "      <td><input type= 'radio' name='wlwdssec_enable' value='0'> Use the Main Setting in Security Page</td>\n");
    fprintf(fs, "   </tr>\n");
    fprintf(fs, "   <tr>\n");
    fprintf(fs, "     <td><input type= 'radio' name='wlwdssec_enable' value='1' checked> Use a Seperate Setting (Enter a key below)</td>\n");
    fprintf(fs, "   </tr>\n");
    fprintf(fs, "   <tr>\n");
    fprintf(fs, "     <td>(Enter key 1 with 13(or 5) ASCII characters or 26(or10)hexadecimal digits for WEP encryption; leave blank for no encryption)</td>\n");
    fprintf(fs, "   </tr>\n");
    fprintf(fs, "   <tr>\n");
    fprintf(fs, "     <td><input type='text' name='wlwdskey' size='30' maxlength=26></td>\n");
    fprintf(fs, "   </tr>\n");
#endif

    fprintf(fs, "</table>\n");
    fprintf(fs, "<div id='staticWds'>\n");
    fprintf(fs, "   <table border='0' cellpadding='4' cellspacing='0'>\n");
    fprintf(fs, "      <tr>\n");
    fprintf(fs, "         <td width='200'>Remote Bridges MAC Address:</td> \n");
    fprintf(fs, "         <td><input type='text' name='wlwds' onChange=\"wl_recalc(0);\"></td>\n");
    fprintf(fs, "         <td><input type='text' name='wlwds' onChange=\"wl_recalc(0);\"></td>\n");
    fprintf(fs, "      </tr>\n");
    fprintf(fs, "      <tr>\n");
    fprintf(fs, "         <td width='200'></td>\n");
    fprintf(fs, "         <td><input type='text' name='wlwds' onChange=\"wl_recalc(0);\"></td>\n");
    fprintf(fs, "         <td><input type='text' name='wlwds' onChange=\"wl_recalc(0);\"></td>\n");
    fprintf(fs, "      </tr>\n");
    fprintf(fs, "   </table>\n");
    fprintf(fs, "</div>\n");
    fprintf(fs, "<div id='dynamicWds'>\n");
    fprintf(fs, "<table border='0' cellpadding='4' cellspacing='0'>\n");
    fprintf(fs, "   <tr>\n");
    fprintf(fs, "      <td valign=top width='200'>Remote Bridges MAC Address:</td>\n");
    fprintf(fs, "      <td>\n");
    // write table header
    fprintf(fs, "         <table border='1' cellpadding='4' cellspacing='0'>\n");
    fprintf(fs, "            <tr>\n");
    fprintf(fs, "               <td class='hd'> &nbsp </td>\n");
    fprintf(fs, "               <td class='hd'> SSID </td>\n");
    fprintf(fs, "               <td class='hd'> BSSID </td>\n");
    fprintf(fs, "            </tr>\n");

    pToken = strstr(query, "&wlMode");
    if (pToken == NULL) {
        BcmWl_GetVar_dmx(WDS_LAZYWDS, lazywds);
    } else {
        cgiGetValueByName(pToken, WDS_MODE, mode);
        cgiGetValueByName(pToken, WDS_LAZYWDS, lazywds);
    }
    if (strcmp(lazywds, "2") == 0) {
        // scan wds AP
        BcmWl_ScanWdsMacResult_dmx();
        // write table body
#if defined(SUPPORT_DM_LEGACY98)
        cgiWlPrintWdsMacs(fs);
#elif defined(SUPPORT_DM_HYBRID)
        cgiWlPrintWdsMacs(fs);
#elif defined(SUPPORT_DM_PURE181)
        cgiWlPrintWdsMacs_dev2(fs);
#elif defined(SUPPORT_DM_DETECT)
        if (cmsMdm_isDataModelDevice2()) {
            cgiWlPrintWdsMacs_dev2(fs);
        } else {
            cgiWlPrintWdsMacs(fs);
        }
#endif
    }

    fprintf(fs, "         </table>\n");
    fprintf(fs, "      </td>\n");
    fprintf(fs, "   </tr>\n");
    fprintf(fs, "</table>\n");
    fprintf(fs, "</div>\n");

    fprintf(fs, "<br><br><center>\n");
    fprintf(fs, "<input type='button' onClick='refreshClick()' value='Refresh'>\n");
    fprintf(fs, "<input type='button' onClick='btnApply()' value='Apply/Save'>\n");
    fprintf(fs, "</center>\n");
    fprintf(fs, "</form>\n");
    fprintf(fs, "</blockquote>\n");
    fprintf(fs, "</body>\n");
    fprintf(fs, "</html>\n");
    fflush(fs);
}

void writeWlWdsScript(char *query, FILE *fs)
{
    char mode[WL_SM_SIZE_MAX], enbl[WL_SM_SIZE_MAX], lazywds[WL_SM_SIZE_MAX];
    char wlWds0[WL_MID_SIZE_MAX];
    char wlWds1[WL_MID_SIZE_MAX];
    char wlWds2[WL_MID_SIZE_MAX];
    char wlWds3[WL_MID_SIZE_MAX];
#ifdef SUPPORT_WDS_WEP
    char wlWdsKey[WL_MID_SIZE_MAX];
    char wlWdsSecEnable[WL_SM_SIZE_MAX];
#endif
    char *pToken = NULL;
    char wlAfterBurnerEn[WL_SIZE_8_MAX];
    char refresh[WL_SM_SIZE_MAX];
    char syncNvram[WL_SM_SIZE_MAX];

    BcmWl_GetVar_dmx("wlSyncNvram", syncNvram);
    BcmWl_GetVar_dmx(WL_IFC_ENABLED, enbl);
    BcmWl_GetVar_dmx("wlRefresh", refresh);
    pToken = strstr(query, "&wlMode");
    if (pToken == NULL) {
        BcmWl_GetVar_dmx(WDS_MODE, mode);
        BcmWl_GetVar_dmx(WDS_LAZYWDS, lazywds);
    } else {
        cgiGetValueByName(pToken, WDS_MODE, mode);
        cgiGetValueByName(pToken, WDS_LAZYWDS, lazywds);
    }
    if (strcmp(lazywds, "2") == 0)
        BcmWl_ScanWdsMacStart_dmx();
    fprintf(fs, "<html><head>\n");
    fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
    fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
    fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");

    fprintf(fs, "<script language='javascript' src='util.js'></script>\n");
    fprintf(fs, "<script language='javascript'>\n");
    fprintf(fs, "<!-- hide\n\n");

    /*generate new session key in glbCurrSessionKey*/
    cgiGetCurrSessionKey(0,NULL,NULL);


    BcmWl_GetVar_dmx("wlWds0", wlWds0);
    BcmWl_GetVar_dmx("wlWds1", wlWds1);
    BcmWl_GetVar_dmx("wlWds2", wlWds2);
    BcmWl_GetVar_dmx("wlWds3", wlWds3);
    BcmWl_GetVar_dmx("wlAfterBurnerEn", wlAfterBurnerEn);
#ifdef SUPPORT_WDS_WEP
    BcmWl_GetVar_dmx("wlWdsKey", wlWdsKey);
    BcmWl_GetVar_dmx("wlWdsSecEnable", wlWdsSecEnable);
#endif
    fprintf(fs, "var wds = new Array( '%s', '%s', '%s', '%s' );\n\n", wlWds0, wlWds1, wlWds2, wlWds3);
    fprintf(fs, "var afterburneren = '%s' ; \n\n", wlAfterBurnerEn);
    fprintf(fs, "function hideStaticWds(hide) {\n");
    fprintf(fs, "   var status = 'visible';\n");
    fprintf(fs, "   if ( hide == 1 )\n");
    fprintf(fs, "      status = 'hidden';\n");
    fprintf(fs, "   if (document.getElementById)\n");
    fprintf(fs, "      document.getElementById('staticWds').style.visibility = status;\n");
    fprintf(fs, "   else {\n");
    fprintf(fs, "      if (document.layers) {\n");
    fprintf(fs, "         with ( document.forms[0] ) {\n");
    fprintf(fs, "            if ( hide == 1 ) {\n");
    fprintf(fs, "               for ( i = 0; i < 4; i++ )\n");
    fprintf(fs, "                  wlwds[i].value = '';\n");
    fprintf(fs, "            } else {\n");
    fprintf(fs, "               for ( i = 0; i < 4; i++ )\n");
    fprintf(fs, "                  wlwds[i].value = wds[i];\n");
    fprintf(fs, "            }\n");
    fprintf(fs, "         }\n");
    fprintf(fs, "      } else\n");
    fprintf(fs, "         document.all.staticWds.style.visibility = status;\n");
    fprintf(fs, "   }\n");
    fprintf(fs, "}\n\n");

    fprintf(fs, "function hideDynamicWds(hide) {\n");
    fprintf(fs, "   var status = 'visible';\n");
    fprintf(fs, "   if ( hide == 1 )\n");
    fprintf(fs, "      status = 'hidden';\n");
    fprintf(fs, "   if (document.getElementById)\n");
    fprintf(fs, "      document.getElementById('dynamicWds').style.visibility = status;\n");
    fprintf(fs, "   else {\n");
    fprintf(fs, "      if (document.layers) {\n");
    fprintf(fs, "      } else\n");
    fprintf(fs, "         document.all.dynamicWds.style.visibility = status;\n");
    fprintf(fs, "   }\n");
    fprintf(fs, "}\n\n");

    fprintf(fs, "function refreshClick() {\n");
    fprintf(fs, "   var lst = '';\n");
    fprintf(fs, "   with ( document.forms[0] ) {\n");
    fprintf(fs, "      var idx = wllazywds.selectedIndex\n");
    fprintf(fs, "      var restrict = wllazywds[idx].value;\n");
    fprintf(fs, "      if (restrict != '2') {\n");
    fprintf(fs, "         alert('Refresh only allowed when Bridge Restrict has \"Enabled(Scan)\" selected.');\n");
    fprintf(fs, "         return;\n");
    fprintf(fs, "      }\n");
    fprintf(fs, "      lst += '&wlMode=' + wlMode.options[wlMode.selectedIndex].value;\n");
    fprintf(fs, "      lst += '&wlLazyWds=' + restrict;\n");
    fprintf(fs, "   }\n");
    fprintf(fs, "   var loc = 'wlwds.cmd?action=scan' + lst;\n\n");
    fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
    fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
    fprintf(fs, "   eval(code);\n");
    fprintf(fs, "}\n\n");


    fprintf(fs, "function wl_recalc(OnPageLoad) {\n");
    fprintf(fs, "   var ret=true;\n");
    fprintf(fs, "   with ( document.forms[0] ) {\n");
    fprintf(fs, "       if(OnPageLoad==0) {\n");
    fprintf(fs, "       }\n");
    fprintf(fs, "   }\n");
    fprintf(fs, "   return ret;\n");
    fprintf(fs, "}\n\n");

    fprintf(fs, "function BridgeRestrictChange(OnPageLoad) {\n");
    fprintf(fs, "   with ( document.forms[0] ) {\n");
    fprintf(fs, "      var restrict = wllazywds[wllazywds.selectedIndex].value;\n");
    fprintf(fs, "      if ( wl_recalc(OnPageLoad) == false) return; \n\n");
    fprintf(fs, "      switch ( restrict ) {\n");
    fprintf(fs, "      case '0':\n");
    fprintf(fs, "         hideDynamicWds(1);\n");
    fprintf(fs, "         hideStaticWds(0);\n");
    fprintf(fs, "         break;\n");
    fprintf(fs, "      case '1':\n");
    fprintf(fs, "         hideDynamicWds(1);\n");
    fprintf(fs, "         hideStaticWds(1);\n");
    fprintf(fs, "         break;\n");
    fprintf(fs, "      case '2':\n");
    fprintf(fs, "         refreshClick();\n");
    fprintf(fs, "         break;\n");
    fprintf(fs, "      }\n");
    fprintf(fs, "      document.getElementById('apMode').style.visibility = 'hidden';\n");
    fprintf(fs, "   }\n");
    fprintf(fs, "}\n\n");

    fprintf(fs, "function parseRestrictIndex(restrict) {\n");
    fprintf(fs, "   var ret;\n");
    fprintf(fs, "      switch(restrict) {\n");
    fprintf(fs, "      case '0':\n");
    fprintf(fs, "         ret = 0;\n");
    fprintf(fs, "         break;\n");
    fprintf(fs, "      case '2':\n");
    fprintf(fs, "         ret = 1;\n");
    fprintf(fs, "         break;\n");
    fprintf(fs, "      default:\n");
    fprintf(fs, "         ret = 2;\n");
    fprintf(fs, "         break;\n");
    fprintf(fs, "   }\n");
    fprintf(fs, "   return ret;\n");
    fprintf(fs, "}\n\n");

    fprintf(fs, "function frmLoad() {\n");
    fprintf(fs, "   with ( document.forms[0] ) {\n");
    fprintf(fs, "      if ( '%s' == 'ap' )\n", mode);
    fprintf(fs, "         wlMode.selectedIndex = 0;\n");
    fprintf(fs, "      else\n");
    fprintf(fs, "         wlMode.selectedIndex = 1;\n\n");
#ifdef SUPPORT_WDS_WEP
    fprintf(fs, "      wlwdssec_enable[%d].checked=true;\n", atoi(wlWdsSecEnable));
    fprintf(fs, "      wlwdskey.value = '%s';\n", wlWdsKey);
#endif
    fprintf(fs, "      wllazywds.selectedIndex = parseRestrictIndex('%s');\n\n", lazywds);

    fprintf(fs, "      for ( i = 0; i < 4; i++ )\n");
    fprintf(fs, "         wlwds[i].value = wds[i];\n\n");

    fprintf(fs, "      if (wllazywds.selectedIndex == 1) {\n");
    fprintf(fs, "         hideDynamicWds(0);\n");
    fprintf(fs, "         hideStaticWds(1);\n");
    fprintf(fs, "      } else\n");
    fprintf(fs, "         BridgeRestrictChange(1);\n");
    fprintf(fs, "      document.getElementById('apMode').style.visibility = 'hidden';\n");
    fprintf(fs, "   }\n");
    fprintf(fs, "}\n\n");

    fprintf(fs, "function btnApply() {\n");
    fprintf(fs, "   var loc = 'wlwds.cmd?action=save';\n\n");
    fprintf(fs, "   var idx;\n\n");
    fprintf(fs, "   if ( '%s' == '0' ) {\n", enbl);
    fprintf(fs, "      alert('Cannot apply the change since wireless is currently disabled.');\n");
    fprintf(fs, "      return;\n");
    fprintf(fs, "   }\n\n");
    fprintf(fs, "   with ( document.forms[0] ) {\n");
    fprintf(fs, "      var idx;\n");
    fprintf(fs, "      var restrict;\n");
#ifdef SUPPORT_WDS_WEP
    fprintf(fs, "   if (wlwdskey.value !='') { ;\n");
    fprintf(fs, "      if ( (isValidKey(wlwdskey.value, 13)) == false && (isValidKey(wlwdskey.value, 5) == false)) {  ;\n");
    fprintf(fs, "         alert('WEP Key' + wlwdskey.value + 'is invalid. Please enter 13 ASCII characters or 26 hexadecimal digits for a 128-bit WEP encryption key.'); \n");
    fprintf(fs, "         return;\n");
    fprintf(fs, "      };\n");
    fprintf(fs, "   };\n");
#endif
    fprintf(fs, "      idx = wlMode.selectedIndex;\n");
    fprintf(fs, "      loc += '&wlMode=' + wlMode.options[idx].value;\n");
    fprintf(fs, "      idx = wllazywds.selectedIndex;\n");
    fprintf(fs, "      loc += '&wlLazyWds=' + wllazywds.options[idx].value;\n");
#ifdef SUPPORT_WDS_WEP
    fprintf(fs, "      loc += '&wlWdsKey=' + wlwdskey.value;\n");
//   fprintf(fs, "      alert('wlwdssec_enable[0].checked='+ wlwdssec_enable[0].checked + ',' + 'wlwdssec_enable[1].checked='+ wlwdssec_enable[1].checked);\n");
    fprintf(fs, "      if (wlwdssec_enable[0].checked)\n");
    fprintf(fs, "          loc += '&wlWdsSecEnable=0';\n");
    fprintf(fs, "      else\n");
    fprintf(fs, "          loc += '&wlWdsSecEnable=1';\n");
//   fprintf(fs, "      alert('wlWdsKey='+ wlwdskey.value);\n");
#endif
    fprintf(fs, "      restrict = wllazywds.options[idx].value;\n");
    fprintf(fs, "      switch ( restrict ) {\n");
    fprintf(fs, "      case '0':\n");
    fprintf(fs, "         for ( i = 0; i < 4; i++ ) {\n");
    fprintf(fs, "            if ( wlwds[i].value != '' && isValidMacAddress(wlwds[i].value) == false ) {\n");
    fprintf(fs, "               msg = 'Remote Bridge MAC address \"' + wlwds[i].value + '\" is invalid MAC address.';\n");
    fprintf(fs, "               alert(msg);\n");
    fprintf(fs, "               return;\n");
    fprintf(fs, "            }\n");
    fprintf(fs, "            loc += '&wlWds' + i + '=';\n");
    fprintf(fs, "            loc += wlwds[i].value;\n");
    fprintf(fs, "         }\n");
    fprintf(fs, "         break;\n");

    fprintf(fs, "      case '2':\n");
    fprintf(fs, "         var lst = '';\n");
    fprintf(fs, "         if (scanwds.length > 0)\n");
    fprintf(fs, "            for (i = 0; i < scanwds.length; i++) {\n");
    fprintf(fs, "               if ( scanwds[i].checked == true )\n");
    fprintf(fs, "                  lst += scanwds[i].value + ', ';\n");
    fprintf(fs, "            }\n");
    fprintf(fs, "         else if ( scanwds.checked == true )\n");
    fprintf(fs, "            lst = scanwds.value;\n");
    fprintf(fs, "         loc += '&wdsLst=' + lst;\n\n");
    fprintf(fs, "         break;\n");

    fprintf(fs, "      case '1':\n");
    fprintf(fs, "         break;\n");
    fprintf(fs, "      }\n");
    fprintf(fs, "   }\n\n");

    fprintf(fs, "   loc += '&wlSyncNvram=1';\n");
    fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
    fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
//   fprintf(fs, "   alert('command='+code);\n");
    fprintf(fs, "   eval(code);\n");
    fprintf(fs, "}\n\n");

    fprintf(fs, "<!-- hide\n\n");
    fprintf(fs, "// done hiding -->\n");
    fprintf(fs, "</script>\n");

    fprintf(fs, "</head>\n<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
}


#ifndef SUPPORT_DM_PURE181
WL_STATUS BcmWl_addWdsMac_tr98(char *lst)
{

    char *pToken,*pLast;
    pToken = strtok_r(lst, ", ", &pLast);
    WL_STATUS sts = WL_STS_OK;
    while ( pToken != NULL ) {
        sts = (WL_STATUS)BcmWl_addWdsMac(pToken);
        if ( sts != WL_STS_OK ) {
            return sts;
        }
        pToken = strtok_r(NULL, ", ", &pLast);
    }
    return WL_STS_OK;
}
#endif

void cgiWlWdsSave(char *query, FILE *fs)
{
    char data[IFC_LARGE_LEN]= {0};
    WL_STATUS sts = WL_STS_OK;
    char *name;
    char lst[WEB_BUF_SIZE_MAX] = {0}; /* lst size? using Pointer for cgiGetValueByName could be better*/
    char wdsstr[WL_MACADDR_SIZE*WL_WDS_NUM]= {0};
    int i;


    data[0] = '\0';

    name = query;
#ifdef SUPPORT_WDS_WEP
    cgiGetValueByName(name, "wlWdsKey", data);
    BcmWl_SetVar_dmx("wlWdsKey", data);

    cgiGetValueByName(name, "wlWdsSecEnable", data);
    BcmWl_SetVar_dmx("wlWdsSecEnable", data);
#endif
    cgiGetValueByName(name, WDS_MODE, data);
    BcmWl_SetVar_dmx(WDS_MODE, data);
    cgiGetValueByName(name, WDS_LAZYWDS, data);
    BcmWl_SetVar_dmx(WDS_LAZYWDS, data);

    BcmWl_removeAllWdsMac_dmx();
    switch(atoi(data)) {
    case WL_BRIDGE_RESTRICT_ENABLE:
        for (i = 0; i < WL_WDS_NUM; i++) {
            sprintf(data, "wlWds%d", i);
            cgiGetValueByName(query, data, lst);
            BcmWl_setWds_dmx(i,lst,wdsstr);
            BcmWl_SetVar_dmx(data, lst);
        }
        if(strlen(wdsstr)>0)
            BcmWl_SetVar_dmx("wlWds",wdsstr);
        else
            BcmWl_SetVar_dmx("wlWds",NULL);
        break;
    case WL_BRIDGE_RESTRICT_ENABLE_SCAN:
        cgiGetValueByName(query, "wdsLst", lst);
        sts = (WL_STATUS) BcmWl_addWdsMac_dmx(lst);
        if ( sts != WL_STS_OK ) {
            sprintf(data, "Add wireless WDS MAC for failed. " \
                    "Status: %d.",  sts);
            cgiWriteMessagePage(fs, "Wireless WDS MAC Add Error", "",
                                "wlwds.cmd?action=view");
        }
        break;
    }
    wlUnlockWriteMdmOne_dmx(MARK_IDX(index_wl));
    glbSaveConfigNeeded = GLBCONFIGNEEDED_DMX;
    cgiRefreshPage("/webs/wlrefresh.html","wlwds.cmd?action=view", fs);
}


#if defined(DMP_WIFILAN_1) && !defined(SUPPORT_UNIFIED_WLMNGR)
/* This is the original way of dumping station list */
void cgiWlPrintStationList(FILE *fs)
{
    //refersh data structure
    wlUnlockReadMdm_dmx();
    BcmWl_aquireStationList();
    int nums = BcmWl_getNumStations();
    char addr[WL_MID_SIZE_MAX];
    char associated, authorized;
    char sign[][8] = {"&nbsp", "Yes"};
    char ssid[WL_SSID_SIZE_MAX];
    char ifcName[WL_SM_SIZE_MAX];
    int i;
    for(i=0; i<nums; i++) {
        BcmWl_getStation(i, addr, &associated, &authorized, ssid, ifcName);
        fprintf(fs, "               <tr> <td><p align=center> %s&nbsp </td> <td><p align=center> %s </p></td> <td><p align=center> %s </p></td> <td><p align=center> %s&nbsp </td>  <td><p align=center> %s&nbsp </td>  </tr>\n", addr, sign[(int)associated], sign[(int)authorized], ssid, ifcName);
    }
}
#endif  /* DMP_WIFILAN_1 */

#ifdef DMP_DEVICE2_WIFIACCESSPOINT_1
/* this is new way of dumping station list (and using TR181) */
void cgiWlPrintStationList_dev2(FILE *fs)
{
    /* XXX TODO: go through MDM to dump station list.  The list should
     * already be up to date via other notification mechanisms.  For now,
     * just print out some fake data
     */
    BcmWl_print_stalist(fs);
}
#endif  /* DMP_DEVICE2_WIFIACCESSPOINT_1 */

void cgiWlStationList(char *query __attribute__((unused)), FILE *fs)
{

    fprintf(fs, "<html>\n");
    fprintf(fs, "<head>\n");
    fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
    fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
    fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
    fprintf(fs, "</head>\n");
    fprintf(fs, "<body>\n");
    fprintf(fs, "<blockquote>\n");
    fprintf(fs, "<b>Wireless -- Authenticated Stations</b><br><br>\n");
    fprintf(fs, "This page shows authenticated wireless stations and their status.<br><br>\n");
    fprintf(fs, "         <table border='1' cellpadding='4' cellspacing='0'>\n");
    fprintf(fs, "            <tr>\n");
    fprintf(fs, "               <td class='hd'><p align=center> MAC </td>\n");
    fprintf(fs, "               <td class='hd'><p align=center> Associated </td>\n");
    fprintf(fs, "               <td class='hd'><p align=center> Authorized </td>\n");
    fprintf(fs, "               <td class='hd'><p align=center> SSID </td>\n");
    fprintf(fs, "               <td class='hd'><p align=center> Interface </td>\n");
    fprintf(fs, "            </tr>\n");


#if defined(SUPPORT_DM_LEGACY98)
    cgiWlPrintStationList(fs);
#elif defined(SUPPORT_DM_HYBRID)
    cgiWlPrintStationList(fs);
#elif defined(SUPPORT_DM_PURE181)
    cgiWlPrintStationList_dev2(fs);
#elif defined(SUPPORT_DM_DETECT)
    if (cmsMdm_isDataModelDevice2()) {
        cgiWlPrintStationList_dev2(fs);
    } else {
        cgiWlPrintStationList(fs);
    }
#endif


    fprintf(fs, "</table>\n");
    fprintf(fs, "<br>\n");
    fprintf(fs, "<form><p align=center><input type=button value='Refresh' onClick='document.location.reload()'></p></form>\n");
    fprintf(fs, "</blockquote>\n");
    fprintf(fs, "</body>\n");
    fprintf(fs, "</html>\n");
    fflush(fs);

}

#ifdef BCMWAPI_WAI

#define  FN_TOKEN          "filename="
#define  FN_TOKEN_LEN      strlen(FN_TOKEN)
#define  IMAGE_BOUND_SIZE  256
#define  IMAGE_BUF_SIZE    2048

static CmsRet wlSaveWapiCert(char *bound, int boundSize, FILE *stream, char *output_file_name, int max_read_size)
{
    char begin_cert_str[] = "-----BEGIN CERTIFICATE-----";
    char buf[IMAGE_BUF_SIZE]= {0};
    char *pdest = NULL;
    int byteRd = 0;
    FILE *fp = NULL;

    // get Content-Disposition: form-data; name="filename"; filename="test"
    // check filename, if "", user click no filename and report

    httpd_gets(buf, sizeof(buf));


    if ((pdest = strstr(buf, FN_TOKEN)) == NULL) {
        cmsLog_error("could not find filename within the first %d bytes", sizeof(buf));
        return CMSRET_DOWNLOAD_FAILURE;
    }

    pdest += (FN_TOKEN_LEN);

    if (*pdest == '"' && *(pdest + 1) == '"') {
        cmsLog_error("No filename selected");
        return CMSRET_INVALID_FILENAME;
    }

    while (httpd_gets(buf, sizeof(buf)) == TRUE)
    {

        if (strncmp(buf, begin_cert_str, strlen(begin_cert_str)) == 0) {
            if (fp != NULL) {
                fclose(fp);
                return CMSRET_DOWNLOAD_FAILURE;
            } else {
                fp = fopen(output_file_name, "w");

                if (fp == NULL) {
                    return CMSRET_DOWNLOAD_FAILURE;
                }
            }
        }

        if (strncmp(buf, bound, boundSize) == 0) {
            if (fp != NULL) {
                fclose(fp);
                return CMSRET_SUCCESS;
            } else {
                return CMSRET_DOWNLOAD_FAILURE;
            }
        }

        pdest = memchr(buf, 0xa, sizeof(buf));

        if (pdest == NULL)
            byteRd = sizeof(buf) - 1;   // last byte not count!
        else
            byteRd = pdest - buf + 1;   // include 0xa

        max_read_size -= byteRd;

        if (max_read_size < 0) {
            if (fp != NULL)
                fclose(fp);
            return CMSRET_DOWNLOAD_FAILURE;
        }

        if (fp != NULL)
            fwrite(buf, 1, byteRd, fp);
    }

    if (fp != NULL) {
        fclose(fp);
        return CMSRET_DOWNLOAD_FAILURE;
    }
}

#define AS_CERFILE        "as_cerfile" /* ASU Certificate File name in security.asp */
#define USER_CERFILE        "user_cerfile" /* User Certificate File name in security.asp */
#define AS_CERFILE_PATH        "/var/as_cerfile.cer" /* Temporary file to save ASU certificate */
#define USER_CERFILE_PATH    "/var/user_cerfile.cer" /* Temporary file to save User certificate */

int cgiWlWapiApCertUpload(FILE *stream, int upload_len)
{
    char bound[IMAGE_BOUND_SIZE]= {0};
    int boundSize = 0;
    UBOOL8 isTerminated=FALSE;
    CmsRet result = CMSRET_SUCCESS;
    int ret = WEB_STS_ERROR;

    cmsLog_debug("upload_len=%d", upload_len);

    /*
     * Read the first UPLOAD_BOUND_SIZE(256) bytes of the image.
     */
    httpd_gets(bound, sizeof(bound));
    boundSize = cmsMem_strnlen(bound, sizeof(bound), &isTerminated) - 2;

    if (!isTerminated || boundSize <= 0) {
        cmsLog_error("Failed to find bound within the first %d bytes", sizeof(bound));
        return WEB_STS_ERROR;
    }

    bound[boundSize] = '\0';

    result = wlSaveWapiCert(bound, boundSize, stream, AS_CERFILE_PATH, upload_len);

    if (result != CMSRET_SUCCESS)
        goto cgiWlWapiApCertUpload_Error;

    result = wlSaveWapiCert(bound, boundSize, stream, USER_CERFILE_PATH, upload_len);

    if (result != CMSRET_SUCCESS)
        goto cgiWlWapiApCertUpload_Error;

    if (BcmWapi_InstallApCert(AS_CERFILE_PATH, USER_CERFILE_PATH) != 0)
        goto cgiWlWapiApCertUpload_Error;

    ret = WEB_STS_OK;

cgiWlWapiApCertUpload_Error:

    while (httpd_gets(bound, sizeof(bound)) == TRUE)
        ;
    wlLockWriteMdmOne(index_wl);

    glbSaveConfigNeeded = GLBCONFIGNEEDED_DMX;

    if (ret == WEB_STS_OK) {
        cgiWriteMessagePage(stream, "Message", "Certificates installed!",
                            "wlsecurity.html");
    } else {
        cgiWriteMessagePage(stream, "Message", "Invalid certificates!",
                            "wlsecurity.html");
    }

    return ret;
}

void cgiWlWapiAs(char *query, FILE *fs)
{
    char user_cert_file_name[256];
    char action[WL_SM_SIZE_MAX];
    char need[WL_SM_SIZE_MAX];
    char owner[WL_SM_SIZE_MAX];
    char period_str[WL_SM_SIZE_MAX];
    char unit_str[WL_SM_SIZE_MAX];
    char cert_sn[WL_SM_SIZE_MAX];
    unsigned int period;

    cgiGetValueByName(query, "action", action);
    cgiGetValueByName(query, "need", need);
    cgiGetValueByName(query, "owner", owner);
    cgiGetValueByName(query, "period", period_str);
    cgiGetValueByName(query, "period_unit", unit_str);
    cgiGetValueByName(query, "cert_sn", cert_sn);

    period = atoi(period_str) * atoi(unit_str) * 86400;

    /* Send Msg to Wlmngr is not a good idea here!*/
    if (strcmp(action, "start") == 0) {
        BcmWapi_SetAsPending(1);
        wlSendCmsMsgToWlmngrByHandle(msgHandle, "Start");
        do_ej("/webs/wlwapias.html", fs);
    } else if (strcmp(action, "stop") == 0) {
        BcmWapi_SetAsPending(1);
        wlSendCmsMsgToWlmngrByHandle(msgHandle, "Stop");
        do_ej("/webs/wlwapias.html", fs);
    } else if (strcmp(action, "revoke") == 0) {
        BcmWapi_SetAsPending(1);
        BcmWapi_CertRevoke(cert_sn);
        wlSendCmsMsgToWlmngrByHandle(msgHandle, "Record");
        do_ej("/webs/wlwapias.html", fs);
    } else if (strcmp(need, "ascert") == 0) {
        char *buf = malloc(WAPI_CERT_BUFF_SIZE);
        FILE *f_as;
        int count;

        if (buf == NULL)
            return;

        memset(buf, 0, WAPI_CERT_BUFF_SIZE);

        f_as = fopen("/var/as.cer", "r");

        if (f_as == NULL) {
            free(buf);
            return;
        }

        count = fread(buf, 1, WAPI_CERT_BUFF_SIZE, f_as);
        fwrite(buf, 1, count, fs);
        free(buf);
        fclose(f_as);
        wlSendCmsMsgToWlmngrByHandle(msgHandle, "Record");
    } else if (strcmp(need, "usrcert") == 0) {
        char buf[WAPI_CERT_BUFF_SIZE] = {0};
        FILE *f_as;
        int count;
        char rcv_msg[255] = {0};

        if (BcmWapi_CertAsk(owner, period, rcv_msg))
            printf("%s@%d Error\n\n", __FUNCTION__, __LINE__ );
        else {



            f_as = fopen(rcv_msg, "r");

            if (f_as == NULL) {
                return;
            }

            count = fread(buf, 1, WAPI_CERT_BUFF_SIZE, f_as);
            fwrite(buf, 1, count, fs);
            fclose(f_as);
            unlink(user_cert_file_name);
            wlSendCmsMsgToWlmngr(EID_WLWAPID, "Record");
        }

    }
}

#endif /* BCMWAPI_WAI */

#ifdef SUPPORT_WIFIWAN

/* XXX TODO: most of the code in this section is data model independent.
 * However, there is a little bit which is TR98 specific.  Need to break that out.
 */
static void writeWifiWanCfgScript(FILE *fs, char *addLoc, char *removeLoc)
{

    /* write html header */
    fprintf(fs, "<html><head>\n");
    fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
    fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
    fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");

    /* show wl label of front/rear panel */
    fprintf(fs, "<script language='javascript' src='portName.js'></script>\n");
    fprintf(fs, "<script language='javascript'>\n");
    fprintf(fs, "<!-- hide\n\n");

    /*generate new session key in glbCurrSessionKey*/
    cgiGetCurrSessionKey(0,NULL,NULL);

    /* show wl label of front/rear panel */
    fprintf(fs, "var brdId = '%s';\n", glbWebVar.boardID);
    fprintf(fs, "var intfDisp = '';\n");
    fprintf(fs, "var brdIntf = '';\n");

    fprintf(fs, "function addClick() {\n");
    fprintf(fs, "   var code = 'location=\"' + '%s.html' + '\"';\n", addLoc);    /* for atm, cfgatm, ptm, cfgptm */
    fprintf(fs, "   eval(code);\n");
    fprintf(fs, "}\n\n");

    fprintf(fs, "function removeClick(rml) {\n");
    fprintf(fs, "   var lst = '';\n");
    fprintf(fs, "   if (rml.length > 0)\n");
    fprintf(fs, "      for (i = 0; i < rml.length; i++) {\n");
    fprintf(fs, "         if ( rml[i].checked == true )\n");
    fprintf(fs, "            lst += rml[i].value + ', ';\n");
    fprintf(fs, "      }\n");
    fprintf(fs, "   else if ( rml.checked == true )\n");
    fprintf(fs, "      lst = rml.value;\n");

    fprintf(fs, "   var loc = '%s.cmd?action=remove&rmLst=' + lst;\n\n", removeLoc); /* for atm, dslatm, ptm, dslptm */
    fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
    fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
    fprintf(fs, "   eval(code);\n");
    fprintf(fs, "}\n\n");
    fprintf(fs, "// done hiding -->\n");
    fprintf(fs, "</script>\n");

    /* write body title */
    fprintf(fs, "<title></title>\n</head>\n<body>\n<blockquote>\n<form>\n");
    fprintf(fs, "<center>\n");
}

static void cgiWifiWanCfgView(FILE *fs)
{
    WanDevObject *wanDev=NULL;
    WanCommonIntfCfgObject *wanCommon=NULL;
    WanWifiIntfObject *wlIntf = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    NameList *ifList = NULL;
    UBOOL8 isSet = FALSE;
    CmsRet ret;

    /* write Java Script */
    writeWifiWanCfgScript(fs, "cfgwlwan", "wifiwan");

    /* write table */
    fprintf(fs, "<b>WiFi WAN Interface Configuration</b><br><br>\n");
    fprintf(fs, "Choose Add, or Remove to configure WiFi WAN interfaces.<br>\n");
    fprintf(fs, "Allow one WiFi as layer 2 wan interface.<br><br>\n");
    fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
    /* write table header */
    fprintf(fs, "   <tr align='center'>\n");
    fprintf(fs, "      <td class='hd'>Interface/(Name)</td>\n");
    fprintf(fs, "      <td class='hd'>Connection Mode</td>\n");
    fprintf(fs, "      <td class='hd'>Remove</td>\n");
    fprintf(fs, "   </tr>\n");


    while (cmsObj_getNext(MDMOID_WAN_DEV, &iidStack, (void **)&wanDev) == CMSRET_SUCCESS) {
        if( (ret = cmsObj_get(MDMOID_WAN_COMMON_INTF_CFG, &iidStack, 0, (void **)&wanCommon)) != CMSRET_SUCCESS ) {
            cmsLog_error("Cannot get WanCommonIntfCfgObject, ret = %d", ret);
            cmsObj_free((void **)&wanDev);
            break;
        } else {
            cmsObj_free((void **)&wanDev);
            if (cmsUtl_strcmp(wanCommon->WANAccessType, MDMVS_X_BROADCOM_COM_WIFI)) {
                /* it is a not an wifi wan device, so skip it. */
                cmsObj_free((void **) &wanCommon);
                continue;
            } else {
                cmsObj_free((void **) &wanCommon);
                if( (ret = cmsObj_get(MDMOID_WAN_WIFI_INTF, &iidStack, 0, (void **)&wlIntf)) != CMSRET_SUCCESS ) {
                    cmsLog_error("Cannot get WanWlIntfObject, ret = %d", ret);
                    cmsObj_free((void **)&wanDev);
                    break; /* break out of while loop so we send a response to caller */
                } else {
                    if (wlIntf->ifName != NULL) {
                        fprintf(fs, "   <tr align='center'>\n");

                        /* show wl label of front/rear panel */
                        /* fprintf(fs, "      <td>%s</td>\n", wlIntf->ifName); */
                        fprintf(fs, "<script language='javascript'>\n");
                        fprintf(fs, "<!-- hide\n");
                        fprintf(fs, "brdIntf = brdId + '|' + '%s';\n", wlIntf->ifName);
                        fprintf(fs, "intfDisp = getUNameByLName(brdIntf);\n");
                        fprintf(fs, "document.write('<td>%s/' + intfDisp + '</td>');\n", wlIntf->ifName);
                        fprintf(fs, "// done hiding -->\n");
                        fprintf(fs, "</script>\n");

                        fprintf(fs, "      <td>%s</td>\n", wlIntf->connectionMode);

                        /* remove check box */
                        fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n",
                                wlIntf->ifName);
                        fprintf(fs, "   </tr>\n");

                        isSet = TRUE; /* set Wl layer2 wan interface already */
                    }
                    /* free wlIntf */
                    cmsObj_free((void **) &wlIntf);
                }
            }
        }
    } /* while loop over WANDevice. */

    fprintf(fs, "</table><br>\n");

    /* check available Wl layer2 interface, if there is no iface, don't show add button */
    /* if (cmsDal_getAvailableL2WlIntf(&ifList) == CMSRET_SUCCESS && ifList != NULL)*/
    /* Only support one Wl layer2 interface */
    if (!isSet) {
        fprintf(fs, "<input type='button' onClick='addClick()' value='Add'>\n");
        cmsDal_freeNameList(ifList);
    }

    fprintf(fs, "<input type='button' onClick='removeClick(this.form.rml)' value='Remove'>\n");

    fprintf(fs, "</center>\n</form>\n</blockquote>\n</body>\n</html>\n");
    fflush(fs);
}

static CmsRet cgiWifiWanAdd(char *query, FILE *fs)
{
    char connectionMode[BUFLEN_8];

    cgiGetValueByName(query, "ifname",  glbWebVar.wanL2IfName);
    cgiGetValueByName(query, "connMode",  connectionMode);
    glbWebVar.connMode = atoi(connectionMode);

    if (dalWifiWan_addWlInterface(&glbWebVar) != CMSRET_SUCCESS) {
        do_ej("/webs/wifiwanadderr.html", fs);
        return CMSRET_INTERNAL_ERROR;
    } else {
        cmsLog_debug("dalWifiWan_addWlWanInterface ok.");
        /*
         * Wl add was successful, tell handle_request to save the config
         * before releasing the lock.
         */
        glbSaveConfigNeeded = GLBCONFIGNEEDED_DMX;
    }

    return CMSRET_SUCCESS;

}

static CmsRet cgiWifiWanRemove(char *query, FILE *fs)
{
    char *pToken = NULL, *pLast = NULL;
    char lst[BUFLEN_1024];
    CmsRet ret=CMSRET_SUCCESS;

    cgiGetValueByName(query, "rmLst", lst);
    pToken = strtok_r(lst, ", ", &pLast);

    while (pToken != NULL) {
        strcpy(glbWebVar.wanL2IfName, pToken);

        if ((ret = dalWifiWan_deleteWlInterface(&glbWebVar)) == CMSRET_REQUEST_DENIED) {
            do_ej("/webs/wifiwandelerr.html", fs);
            return ret;
        } else if (ret != CMSRET_SUCCESS) {
            cmsLog_error("dalWifiWan_deleteWlInterface failed for  failed for %s (ret=%d)", glbWebVar.wanL2IfName, ret);
            return ret;
        }

        pToken = strtok_r(NULL, ", ", &pLast);

    } /* end of while loop over list of connections to delete */

    /*
     * Whether or not there were errors during the delete,
     * save our config.
     */
    glbSaveConfigNeeded = GLBCONFIGNEEDED_DMX;

    return ret;
}

void cgiWifiWanCfg(char *query, FILE *fs)
{
    char action[BUFLEN_256];

    cgiGetValueByName(query, "action", action);
    if (cmsUtl_strcmp(action, "add") == 0) {
        if (cgiWifiWanAdd(query, fs) != CMSRET_SUCCESS) {
            return;
        }
    } else if (cmsUtl_strcmp(action, "remove") == 0) {
        if (cgiWifiWanRemove(query, fs) != CMSRET_SUCCESS) {
            return;
        }
    }

    /* for WiFi WAN Interface display */
    cgiWifiWanCfgView(fs);
}

void cgiGetAvailableL2WlIntf(int argc __attribute__((unused)), char **argv __attribute__((unused)), char *varValue)
{
    NameList *nl, *ifList = NULL;
    int first = 1;

    varValue[0] = '\0';
    if (dalWifiWan_getAvailableL2WlIntf(&ifList) != CMSRET_SUCCESS || ifList == NULL) {
        return;
    }

    nl = ifList;
    while (nl != NULL) {
        if (!first)
            strcat(varValue,"|");
        else
            first = 0;
        strcat(varValue, nl->name);
        nl = nl->next;
    }
    cmsDal_freeNameList(ifList);
}
#endif /* SUPPORT_WIFIWAN */


