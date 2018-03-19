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



/*!\file cgi2_wl_dmx.c
 * \brief This file provides the new wlcsm based implementation for the
 * API described in cgi_wl_dmx.h.  This file should be compiled into
 * a shared library, just like the old implementation of the wlmngr.so.
 *
 */



#include <stdio.h>

#include "httpd.h"
#include "cms_dal.h"
#include "cms_msg.h"
#include "cms.h"
#include "cms_obj.h"
#include "cms_util.h"
#include "cgi_wl_dmx.h"
#include "cgi2_wl_mdm.h"
#include <wlcsm_linux.h>
#include <wlcsm_lib_api.h>



/* web pages set global state about which radio/adapter they are currently working on */
unsigned int  wldmx_radioIdx=0;  // starts from 0
unsigned int  wldmx_ssidIdx=0;   // starts from 0

void BcmWl_Switch_instance_dev2(int index)
{
    wldmx_radioIdx = index;
}


UBOOL8 parseEmbeddedIndex(const char *varName,
                          char *genericNameBuf, UINT32 bufLen,unsigned int *out_ssidIdx)
{
    char *idxPtr;
    UBOOL8 hit=FALSE;

    idxPtr = strstr(varName, "_wl");
    if (idxPtr && idxPtr[4] == 'v') {
        hit = TRUE;

        /* copy the generic/base part of the variable name */
        if (idxPtr - varName < (SINT32) bufLen-1) {
            memcpy(genericNameBuf, varName, idxPtr - varName);
        } else {
            cmsLog_error("varName %s is too long for genericNameBuf (%d)",
                         varName, bufLen);
        }

        /* get the radio index */
        idxPtr += 3;
        if (*idxPtr >= '0' && *idxPtr <= '9') {
            int parsed_radioIdx = *idxPtr - '0';

            if (parsed_radioIdx != wldmx_radioIdx) {
                /* weird, why mis-match between variable name radio idx and
                 * current set radio idx?
                 */
                // it is ok as wlcfg.html wil have static v0 there,use current radioIdx is right
                // cmsLog_error("XXX varName=%s parsed_radioIdx=%d curr_radioIdx=%d, use current value",
                //             varName, parsed_radioIdx, wldmx_radioIdx);
            }
            //wldmx_radioIdx = parsed_radioIdx;
        } else {
            cmsLog_error("Unexpected radio index character %c (varName=%s)",
                         *idxPtr, varName);
        }

        /* get the ssid index */
        idxPtr += 2;
        *out_ssidIdx = atoi(idxPtr);
    }
    return hit;
}


void BcmWl_SetVar_dev2(char *varName, char *varValue)
{

    /*
     * First check for any hardcoded, manual handling of varName.
     */
    if (!cmsUtl_strcasecmp(varName, "wlSsidIdx")) {
        wldmx_ssidIdx = atoi(varValue);
        wlcsm_mngr_set(wldmx_radioIdx,0,varName,varValue);
        cmsLog_error("XXX setting SSID index to %d (curr radio=%d)", wldmx_ssidIdx, wldmx_radioIdx);
    } else {
        /*
         * Second, try to use auto-parsing to set the variable.
         * Auto-parsing expect the varName to be like: aabbcc_wl0v1
         */
        char genericNameBuf[BUFLEN_64]= {0};
        char *var_real_name=NULL;
        unsigned int tmp_ssidIdx=0,use_ssidIdx;
        if (parseEmbeddedIndex(varName, genericNameBuf, sizeof(genericNameBuf),&tmp_ssidIdx)) {
            var_real_name=genericNameBuf;
            use_ssidIdx=tmp_ssidIdx;
        } else {
            var_real_name=varName;
            use_ssidIdx=wldmx_ssidIdx;
        }

        /* handl web page name mismatching here in web code other than in wlmngr */
        /* 1. 	wlEnbl -- wlEnbl is really for Radio enable/disable, but historically it is used for SSID enble disable at
         * 	least in web page when try to read/write enable variables
        *
         * 2.   for wlWpa, the varValue's + symbol was repalced with space, we need to restore it back.
         * 	*/
        if(strcmp(var_real_name,"wlEnbl")==0 ) {
            if(use_ssidIdx==0)
                wlcsm_mngr_set(wldmx_radioIdx,use_ssidIdx,"wlEnblSsid",varValue);
            else
                var_real_name="wlEnblSsid";
        } else if((strcmp(var_real_name,"wlWpa")==0) && (strncmp(varValue, "tkip aes",8)) == 0) {
            WLCSM_TRACE(WLCSM_TRACE_DBG," wlWpa value is :%s \r\n",varValue );
            varValue[4] = '+';
            WLCSM_TRACE(WLCSM_TRACE_DBG," wlWpa value is :%s \r\n",varValue );
        }

        wlcsm_mngr_set(wldmx_radioIdx,use_ssidIdx,var_real_name,varValue);
    }
}
void BcmWl_print_stalist(FILE *fs)
{
    char *temp_str=malloc(WL_MAX_ASSOC_STA *sizeof(WL_STATION_LIST_ENTRY)+sizeof(WL_STALIST_SUMMARIES)+1);
    if(temp_str) {

        char *value=wlcsm_mngr_get(wldmx_radioIdx,0,"wl_stalist_summaries",temp_str);
        if(value) {
            int i=0;
            WL_STALIST_SUMMARIES *stalist_summaries= (WL_STALIST_SUMMARIES *)value;
            WL_STATION_LIST_ENTRY *entry;
            for (i = 0; i < stalist_summaries->num_of_stas; i++) {
                entry=&( stalist_summaries->stalist_summary[i]);
                fprintf(fs, "<tr> <td><p align=center> %s&nbsp </td> <td><p align=center> %s </p></td> <td><p align=center> %s </p></td> <td><p align=center> %s&nbsp </td>  <td><p align=center> %s&nbsp </td>  </tr>\n",
                        entry->macAddress, entry->associated?"Yes":"NO", entry->authorized?"Yes":"NO", entry->ssid,entry->ifcName );
            }
        }
        free(temp_str);
    }
}

void BcmWl_print_wdsscanlist(FILE *fs)
{
    char configured_wds[WL_WDS_NUM*WL_MACADDR_SIZE];
    WL_WDSAPLIST_SUMMARIES *wdsaplist_summaries;
    WL_WDSAP_LIST_ENTRY *entry;
    char *temp_str;
    int i=0;
    if((temp_str=malloc(WL_MAX_ASSOC_STA *sizeof(WL_WDSAP_LIST_ENTRY)+sizeof(WL_WDSAPLIST_SUMMARIES)+1))) {
        char *configuredwds=  wlcsm_mngr_get(wldmx_radioIdx,0,"wlWds",configured_wds);
        if((wdsaplist_summaries=(WL_WDSAPLIST_SUMMARIES *)wlcsm_mngr_get(wldmx_radioIdx,0,"wdsscan_summaries",temp_str))) {
            for (i = 0; i < wdsaplist_summaries->num_of_aps; i++) {
                entry=&( wdsaplist_summaries->wdsaplist_summary[i]);
                fprintf(fs, "         <tr>\n");
                if (configuredwds && strstr(configuredwds,entry->mac))
                    fprintf(fs, "            <td align=center><input type='checkbox' checked name='scanwds' value='%s'></td>\n",entry->mac);
                else
                    fprintf(fs, "            <td align=center><input type='checkbox' name='scanwds' value='%s'></td>\n", entry->mac);
                fprintf(fs, "            <td> %s &nbsp</td>\n", entry->ssid);
                fprintf(fs, "            <td>%s </td>\n", entry->mac);
                fprintf(fs, "         </tr>\n");
            }
        }
        free(temp_str);
    }
}

void BcmWl_printMbssTbl(int idx, char *text)
{
    int wsize;
    char *prtloc = text;
    char temp_str[MAX_NLRCV_BUF_SIZE];
    char *value=wlcsm_mngr_get(idx,0,"wl_mbss_summaries",temp_str);
    if(value) {

        int i=0;
        WL_BSSID_SUMMARIES *bss_summaries= (WL_BSSID_SUMMARIES *)value;
        for (i = 0; i < bss_summaries->num_of_bssid; i++) {
            sprintf(prtloc, "<tr><td><input type='checkbox' name='wlEnbl_wl%dv%d' value='ON' %s></td> \n%n",
                    idx,i+1,
                    (bss_summaries->bssid_summary[i].wlEnbl?"CHECKED":"") , &wsize);
            prtloc +=wsize;

            sprintf(prtloc, "<td><input type='text' name='wlSsid_wl%dv%d' maxlength='32' size='32' value='%s'></td> \n%n",
                    idx,i+1,
                    bss_summaries->bssid_summary[i].wlSsid, &wsize);
            prtloc +=wsize;

            /* Note our WebUI "hide" is opposite meaning of TR181 AdvertisementEnabled */
            sprintf(prtloc, "<td><input type='checkbox' valign='middle' align='center' name='wlHide_wl%dv%d' value='ON' %s></td> \n%n",
                    idx,i+1,
                    (bss_summaries->bssid_summary[i].wlHide?"CHECKED":"") , &wsize);
            prtloc +=wsize;

            sprintf(prtloc, "<td><input type='checkbox' valign='middle' align='center' name='wlAPIsolation_wl%dv%d' value='ON' %s></td> \n%n",
                    idx,i+1,
                    (bss_summaries->bssid_summary[i].wlIsolation?"CHECKED":"") , &wsize);
            prtloc +=wsize;

            sprintf(prtloc, "<td><input type='checkbox' valign='middle' align='center' name='wlDisableWme_wl%dv%d' value='ON' %s %s></td> \n%n",
                    idx, i+1,
                    (bss_summaries->bssid_summary[i].wlDisableWme?"CHECKED":""),
                    (bss_summaries->bssid_summary[i].wlWme?"":"DISABLED") , &wsize);
            prtloc +=wsize;
            sprintf(prtloc, "<td><input type='checkbox' valign='middle' align='center' name='wlEnableWmf_wl%dv%d' value='ON' %s %s></td> \n%n",
                    idx, i+1,
                    (bss_summaries->bssid_summary[i].wlEnableWmf?"CHECKED":""),
                    (bss_summaries->bssid_summary[i].wmfSupported?"":"DISABLED") , &wsize);
            prtloc +=wsize;

#if 0
            if(bss_summaries->wlSupportHspot) {
                if(bss_summaries->bssid_summary[i].wlEnableHspot!=2) {
                    snprintf(varname, sizeof(varname),"wl%d.%d_akm", idx,i);
                    ptr = nvram_get(varname);
                    if(ptr && (!strcmp(ptr,"wpa2")))
                        sprintf(prtloc, "<td valign='middle' align='center'><input type='checkbox' name='wlEnableHspot_wl%dv%d' value='ON' %s ></td> \n%n",
                                idx, i, ((m_instance_wl[idx].m_wlMssidVar[i].wlEnableHspot==ON)?"CHECKED":""), &wsize);
                    else
                        sprintf(prtloc, "<td valign='middle' align='center' width='60'><input type='checkbox' disabled=true name='wlEnableHspot_wl%dv%d' value='ON' %s >[wpa2!]</td> \n%n",
                                idx, i, ((m_instance_wl[idx].m_wlMssidVar[i].wlEnableHspot==ON)?"CHECKED":""), &wsize);
                    prtloc +=wsize;
                }
            }
#endif
            sprintf(prtloc, "<td><input type='text' valign='middle' align='center' name='wlMaxAssoc_wl%dv%d' maxlength='3' size='3' value='%d'></td> \n%n",
                    idx,i+1,
                    (bss_summaries->bssid_summary[i].max_clients) , &wsize);
            prtloc +=wsize;

            if (bss_summaries->bssid_summary[i].wlEnbl)
                sprintf(prtloc, "<td>%s</td></tr> \n%n",bss_summaries->bssid_summary[i].bssid, &wsize);
            else
                sprintf(prtloc, "<td>N/A</td></tr> \n%n", &wsize);

            prtloc +=wsize;
        }
    }
    *prtloc = 0;
}

/** Return the requested var value.
 *
 * See also wlmngr_getVar() in wlmngr.c
 */
void BcmWl_GetVar_dev2(char *varName, char *varValue)
{

    /*
     * First check for any hardcoded, manual handling of varName.
     */
    if (!cmsUtl_strcasecmp(varName, "wlInstance_id")) {
        sprintf(varValue, "%d", wldmx_radioIdx);
    } else if (!cmsUtl_strcasecmp(varName, "wlMbssTbl")) {
        BcmWl_printMbssTbl(wldmx_radioIdx, varValue);
    } else if (!cmsUtl_strcasecmp(varName, "wlScanResult")) {
        wlcsm_mngr_get(wldmx_radioIdx,wldmx_ssidIdx,varName,varValue);
        WLCSM_TRACE(WLCSM_TRACE_DBG," wlscanresult:%s \r\n",varValue );
    } else if (!cmsUtl_strcasecmp(varName, "wlSsidIdx")) {
        char *value= NULL;
        value=wlcsm_mngr_get(wldmx_radioIdx,0,varName,varValue);
        if(value) {
            wldmx_ssidIdx=atoi(value);
        }
    } else {
        /*
         * Now try to use auto-parsing to set the variable.
         * Auto-parsing expect the varName to be like: aabbcc_wl0v1
         */
        char genericNameBuf[BUFLEN_64]= {0};
        char *value= NULL;
        char *var_real_name=NULL;
        unsigned int tmp_ssidIdx=0,use_ssidIdx;
        if (parseEmbeddedIndex(varName, genericNameBuf, sizeof(genericNameBuf),&tmp_ssidIdx)) {
            var_real_name=genericNameBuf;
            use_ssidIdx=tmp_ssidIdx;
        } else {
            var_real_name=varName;
            use_ssidIdx=wldmx_ssidIdx;
        }
        if(strcmp(var_real_name,"wlEnbl")==0 && wldmx_ssidIdx)
            var_real_name="wlEnblSsid";

        WLCSM_TRACE(WLCSM_TRACE_LOG," get:%s \r\n",var_real_name );

        value=wlcsm_mngr_get(wldmx_radioIdx,use_ssidIdx,var_real_name,varValue);
        if(!value) varValue[0]='\0';
    }
}

void BcmWl_GetVarEx_dev2(int argc, char **argv, char *varValue)
{
    /* argc is bigger than 2, guranteed by caller */
    *varValue = '\0';
    if (!cmsUtl_strcmp(argv[1], "wlCountryList")) {
        cgiWlMdm_getCountryList(wldmx_radioIdx, argc, argv, varValue);
    } else {
        int i;
        char *varName=NULL;
        varName=malloc(512);
        if(varName==NULL) return;
        sprintf(varName,"%s",argv[1]);
        for (i=2; i < argc; i++) {
            sprintf(varName,"%s %s",varName,argv[i]);
        }
        WLCSM_TRACE(WLCSM_TRACE_LOG," -----------EXT TRYING to get:%s------- \r\n",varName );
        if(!wlcsm_mngr_get(wldmx_radioIdx,wldmx_ssidIdx,varName,varValue)) {
            WLCSM_TRACE(WLCSM_TRACE_LOG,"**** varName:%s NOT FOUND or NULL \r\n",varName);
            sprintf(varValue, "0");
        }
        free(varName);
    }
}


/* this is new way of dumping the filter macs table (and using TR181) */
void wlPrintFilterMacs_dev2(FILE *fs, char *supportMbss, char *idx, char *curIfcName)
{
    char mac[32],*next,temp_str[MAX_NLRCV_BUF_SIZE];
    char *macs= wlcsm_mngr_get(wldmx_radioIdx,wldmx_ssidIdx,"wlFltMacList",temp_str);
    if(macs) {
        for_each(mac,macs,next) {
            fprintf(fs, "   <tr>\n");
            fprintf(fs, "      <td>%s</td>\n", mac);
            fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n", mac);
            fprintf(fs, "   </tr>\n");
        }
    }
}

int BcmWl_addFilterMac_dev2(char *mac, char *ssid, char *ifcName)
{

    char temp_str[MAX_NLRCV_BUF_SIZE];
    char *l_wlFltMacList= wlcsm_mngr_get(wldmx_radioIdx,wldmx_ssidIdx,"wlFltMacList",temp_str);

    if((!l_wlFltMacList) || !strstr(l_wlFltMacList,mac)) {
        char *tmp=malloc((l_wlFltMacList?strlen(l_wlFltMacList)+1:0)+1+strlen(mac));
        if(tmp) {
            if(l_wlFltMacList && strlen(l_wlFltMacList)>16) {
                sprintf(tmp,"%s %s",l_wlFltMacList,mac);
            } else
                strcpy(tmp,mac);
            wlcsm_mngr_set(wldmx_radioIdx,wldmx_ssidIdx,"wlFltMacList",tmp);
            free(tmp);
            return 0;
        } else {
            cmsLog_error(" Failed to add %s ifcName %s", mac, ifcName);
            return -1;
        }
    }
    return 0;
}

int BcmWl_removeFilterMac_dev2(char *remove_macs, char *ifcName)
{
    char temp_str[MAX_NLRCV_BUF_SIZE];
    char *l_wlFltMacList= wlcsm_mngr_get(wldmx_radioIdx,wldmx_ssidIdx,"wlFltMacList",temp_str);
    if(l_wlFltMacList && remove_macs) {

        char name[32], *next;
        char *fltmaclist=malloc(strlen(l_wlFltMacList)+1);
        int first=1;
        if(fltmaclist) {
            fltmaclist[0]='\0';
            for_each(name,l_wlFltMacList,next) {
                /* go through each mac in the list and see if it is in the remove string */
                if(!strstr(name,remove_macs)) {
                    if(!first) strcat(fltmaclist," ");
                    else first=0;
                    strcat(fltmaclist,name);
                }
            }
            wlcsm_mngr_set(wldmx_radioIdx,wldmx_ssidIdx,"wlFltMacList",fltmaclist);
            free(fltmaclist);
        } else
            return -1;
    }
    return 0;
}

int BcmWl_isMacFltEmpty_dev2()
{
    cmsLog_error("TODO: count actual number");
    return 0;
}

void BcmWl_ScanWdsMacStart_dev2()
{
    WLCSM_TRACE(WLCSM_TRACE_DBG," Start to scan,actually only clean the list  \r\n" );
    wlcsm_mngr_set(wldmx_radioIdx,wldmx_ssidIdx,"wdsStSc","");
}

void BcmWl_ScanWdsMacResult_dev2()
{
//   cmsLog_error("TODO: collect result");
}
int BcmWl_addWdsMac_dev2(char *lst)
{

    char *pLast,*pToken;
    char wdsstr[WL_MACADDR_SIZE*WL_WDS_NUM]= {0};
    int idx=0;
    if(lst && strlen(lst)>0) {
        pToken = strtok_r(lst, ", ", &pLast);
        while ( pToken != NULL && idx<WL_WDS_NUM) {
            BcmWl_setWds_dev2(idx++, pToken,wdsstr);
            pToken = strtok_r(NULL, ", ", &pLast);
        }
        if(idx>0) {
            wlcsm_mngr_set(wldmx_radioIdx,wldmx_ssidIdx,"wlLazyWds","0");
            if(wlcsm_mngr_set(wldmx_radioIdx,wldmx_ssidIdx,"wlWds",wdsstr))
                return !WL_STS_OK;
        }
    } else  if(wlcsm_mngr_set(wldmx_radioIdx,wldmx_ssidIdx,"wlWds",NULL))
        return !WL_STS_OK;
    return WL_STS_OK;
}

int BcmWl_removeAllWdsMac_dev2()
{
    /* in TR181- we don't need to clear it while we can set it NULL */
    return 0;
}


int wlmngr_alloc_dev2(int adapter_cnt)
{
    cmsLog_debug("NO-OP (adapter_cnt=%d)", adapter_cnt);
    return 0;
}

void wlmngr_free_dev2(void )
{
    cmsLog_debug("NO-OP");
}

int wldsltr_alloc_dev2(int adapter_cnt)
{
    cmsLog_debug("NO-OP (adapter_cnt=%d)", adapter_cnt);
    return 0;
}

void wldsltr_free_dev2(void )
{
    cmsLog_debug("NO-OP");
}

CmsRet wlUnlockReadMdm_dev2(void )
{
    cmsLog_debug("NO-OP");
    return CMSRET_SUCCESS;
}
void BcmWl_setWds_dev2(int i, char *mac,char *buf)
{
    if(i==0)
        strcpy(buf,mac);
    else if( buf && mac) {
        strcat(buf," ");
        strcat(buf,mac);
    }
}

CmsRet wlUnlockWriteMdmOne_dev2(int idx)
{

    int wait_restart_finished= !(IS_IDX_MARKED(idx));
    idx=REAL_IDX(idx);

    if(wait_restart_finished) {
        WLCSM_TRACE(WLCSM_TRACE_DBG," release cmsLock!!! \r\n" );

        cmsLck_releaseLock();
    }
    wlcsm_mngr_restart(idx,WLCSM_MNGR_RESTART_HTTPD,WLCSM_MNGR_RESTART_SAVEDM,wait_restart_finished);
    if (wait_restart_finished && (cmsLck_acquireLockWithTimeout(HTTPD_LOCK_TIMEOUT) != CMSRET_SUCCESS))
        cmsLog_error("restore back MDM lock failed");
    WLCSM_TRACE(WLCSM_TRACE_DBG,"  keeping pging after restart \r\n" );


    glbSaveConfigNeeded = FALSE;
    return CMSRET_SUCCESS;
}


