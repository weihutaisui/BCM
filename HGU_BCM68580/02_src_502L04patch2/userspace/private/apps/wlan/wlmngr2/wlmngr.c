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

#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <security_ipc.h>
#include "board.h"
#include <shutils.h>

#ifdef DSLCPE_1905
#include <wps_1905.h>
#endif
#include "odl.h"
#include <pthread.h>
#include <bcmnvram.h>
#include <bcmconfig.h>

#include "wlioctl.h"
#include "wlutils.h"
#include "wlmngr.h"
#include "wllist.h"

#include "wlsyscall.h"

#include <sys/socket.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <linux/if_packet.h>

#include <wlcsm_lib_dm.h>
//#include "wlmngr_http.h"

#ifdef SUPPORT_WSC
#include <time.h>
#endif
#include <wlcsm_lib_api.h>
#include <wlcsm_lib_dm.h>
#include <wlcsm_linux.h>
// #define WL_WLMNGR_DBG

#if defined(HSPOT_SUPPORT)
#include <wlcsm_lib_hspot.h>
#endif
#define IFC_BRIDGE_NAME	 "br0"

extern int g_dm_loaded;
extern int dhd_probe(char *name);
extern bool wl_wlif_is_psta(char *ifname);

#define WL_PREFIX "_wl"
pthread_mutex_t g_WLMNGR_THREAD_MUTEX= PTHREAD_MUTEX_INITIALIZER; /**< mutex for Synchronization between Thread */
unsigned int g_radio_idx=0;
pthread_mutex_t g_WLMNGR_REBOOT_MUTEX= PTHREAD_MUTEX_INITIALIZER; /**< mutex for sync reboot */
static int _wlmngr_handle_wlcsm_cmd_get_dmvar(t_WLCSM_MNGR_VARHDR  *hdr,char *varName,char *varValue);

void wlmngr_get_thread_lock(void)
{
    pthread_mutex_lock(&g_WLMNGR_THREAD_MUTEX);
}

void wlmngr_release_thread_lock(void)
{
    pthread_mutex_unlock(&g_WLMNGR_THREAD_MUTEX);
}

bool cur_mbss_on = FALSE; /* current mbss status, off by default at init */
int act_wl_cnt = 0; /* total enabled wireless adapters */
int is_smp_system = 0;  /* set to 1 if we are on SMP system */

#ifdef SUPPORT_WSC
int wps_config_command;
int wps_action;
char wps_uuid[36];
char wps_unit[32];
int wps_method;
char wps_autho_sta_mac[sizeof("00:00:00:00:00:00")];
int wps_enr_auto = 0;
#define MAX_BR_NUM	8
#endif

#define WLHSPOT_NOT_SUPPORTED 2
/* These functions are to be used only within this file */
static void wlmngr_getVarFromNvram(void *var, const char *name, const char *type);
static bool wlmngr_detectAcsd(void);

int get_wps_env();
int set_wps_env(char *uibuf);


#ifdef BCM_WBD
static int _wlmngr_nv_wbd_need_adjust(const unsigned int idx) {

    char nvram_name[WL_SIZE_256_MAX]= {0};
    /*when wbd_ifnames is not defined and wbd_mode is 2(slave)  */

    if ((nvram_get("wbd_ifnames")==NULL)   && !strncmp(nvram_safe_get("wbd_mode"),"2",1)) {
        snprintf(nvram_name,WL_SIZE_256_MAX,"wl%d_dwds",idx);
        if(!strncmp(nvram_safe_get(nvram_name),"1",1))  {
            snprintf(nvram_name,WL_SIZE_256_MAX,"wl%d_mode",idx);
            if(!strncmp(nvram_safe_get(nvram_name),"sta",3))  {
                return 1;
            }
        }
    }
    return 0;
}

static void _wlmngr_nv_adjust_wbd(const unsigned int idx,int direction) {
    /*when dwds and sta setting on primary interface, use wlx.1 as the
     *wbd serving BSS */
    if(_wlmngr_nv_wbd_need_adjust(idx))
        wlcsm_dm_mngr_set_all_value(idx,1,"wlEnblSsid","1");
}
#endif

void wlmngr_enum_ifnames(unsigned int idx)
{
    char ifbuf[WL_SIZE_512_MAX] = {0};
    char brlist[WL_SIZE_256_MAX];
    char name[32],nv_name[32],nv_valu[64];
    char *next=NULL,*lnext=NULL;
    int index=0,bridgeIndex=0;
    char br_ifnames[128];
    wlcsm_dm_get_bridge_info(ifbuf);
    if(ifbuf[0]!='\0') {
        /*rest lanx_ifnames */
        for (index = 1; index<MAX_BR_NUM; ++index) {
            sprintf(brlist,"lan%d_ifnames",index);
            if(nvram_get(brlist))
                wlcsm_nvram_unset(brlist);
        }

        /* reset wlx_vifs */
        sprintf(nv_name,"wl%d_vifs",idx);
        wlcsm_nvram_unset(nv_name);
        /* reset wlx.y_ifname, but leave wlx_ifname unchanged regardless */

        for (index=1; index<WL_RADIO_WLNUMBSS(idx); index++) {
            sprintf(brlist,"wl%d.%d_ifname",idx,index);
            if(wlcsm_nvram_get(brlist))
                wlcsm_nvram_unset(brlist);
        }

        for (index = 0; index  <WL_WIFI_RADIO_NUMBER; ++index) {
            sprintf(brlist,"wl%d",index);
            if(strstr(ifbuf,brlist)==NULL && strlen(ifbuf)+strlen(brlist)+1<WL_SIZE_512_MAX) {
                strcat(ifbuf,":");
                strcat(ifbuf,brlist);
            }
        }
        for_each(brlist,ifbuf,lnext) {
            index=0;
            bridgeIndex=0;
            memset(br_ifnames,0,sizeof(br_ifnames));
            foreachcolon(name,brlist,next) {
                if(index++ == 0) {

                    /* here it is the bridge name,and we get index from it */
                    if(sscanf(name,"br%d",&bridgeIndex)) {
                        if (bridgeIndex == 0)
                            snprintf(nv_name, sizeof(nv_name), "lan_hwaddr");
                        else
                            snprintf(nv_name, sizeof(nv_name), "lan%d_hwaddr", bridgeIndex);

                        wlmngr_getHwAddr(0, name, nv_valu);
                        nvram_set(nv_name, nv_valu);

                        if (bridgeIndex == 0)
                            snprintf(nv_name, sizeof(nv_name), "lan_ifname");
                        else
                            snprintf(nv_name, sizeof(nv_name), "lan%d_ifname", bridgeIndex);
                        nvram_set(nv_name,name);

                    } else {
                        WLCSM_TRACE(WLCSM_TRACE_LOG," BRIDGE name is in different format??? \r\n" );
                    }

                } else if(strncmp(name,"usb",3)!=0)  {
                    if(strlen(br_ifnames))
                        snprintf(br_ifnames+strlen(br_ifnames), sizeof(br_ifnames)-strlen(br_ifnames), " %s",name);
                    else
                        snprintf(br_ifnames, sizeof(br_ifnames), "%s",name);
                }
            }
#ifdef BCM_WBD
            if(_wlmngr_nv_wbd_need_adjust(idx)) {
                snprintf(nv_name, sizeof(nv_name), "wl%d.1",idx);
                if(!strstr(br_ifnames,nv_name)) {
                    snprintf(br_ifnames+strlen(br_ifnames), sizeof(br_ifnames)-strlen(br_ifnames), " %s",nv_name);
                }
            }

            snprintf(nv_name, sizeof(nv_name), "br%d_ifnames", bridgeIndex);
            nvram_set(nv_name,br_ifnames);
#endif

            if (bridgeIndex == 0)
                snprintf(nv_name, sizeof(nv_name), "lan_ifnames");
            else
                snprintf(nv_name, sizeof(nv_name), "lan%d_ifnames", bridgeIndex);

            WLCSM_TRACE(WLCSM_TRACE_DBG,"br_ifnames:%s\r\n",br_ifnames);
            nvram_set(nv_name,br_ifnames);
            bridgeIndex++;
        }

        /* construct wlx_vifs */
        brlist[0]='\0';
        for (index=1; index<WL_RADIO_WLNUMBSS(idx); index++) {
            if(WL_BSSID_WLENBLSSID(idx,index)) {
                snprintf(nv_name,sizeof(nv_name)," wl%d.%d",idx,index);
                strcat(brlist,nv_name);
            }
        }
        if(strlen(brlist)>1) {
            snprintf(nv_name,sizeof(nv_name),"wl%d_vifs",idx);
            wlcsm_nvram_set(nv_name,brlist);
        }
    }
}

/* hook function for DM layer to get runtime object pointer */
void *wlmngr_get_runtime_obj_pointer(unsigned int radio_idx,unsigned int sub_idx,unsigned int oid,WLCSM_DM_MNGR_CMD cmd,void *par)
{
    switch ( oid ) {
    case WLMNGR_DATA_POS_WIFI_ASSOCIATED_DEVICE:
        WLCSM_TRACE(WLCSM_TRACE_LOG," TODO: return the Assocaited device pointer, currently we are using different structure,to consolidate \r\n" );
        break;
    case WLMNGR_DATA_POS_WIFI_SSID_STATS:
        WLCSM_TRACE(WLCSM_TRACE_LOG," TODO:get WIFI SSID STATISTICS \r\n" );
        break;
    case WLMNGR_DATA_POS_WIFI_RADIO_STATS:
        WLCSM_TRACE(WLCSM_TRACE_LOG," TODOL get RADIO SATISTICS \r\n" );
        break;
    default:
        WLCSM_TRACE(WLCSM_TRACE_LOG,"ERROR: NO SUCH RUNTIME OBJECT!!!!!!!!!!!!!!!!!!!!\n" );
        break;
    }
    return NULL;
}


static int __wlmngr_handle_wlcsm_var_validate(t_WLCSM_MNGR_VARHDR  *hdr, char *varName,char *varValue)
{
    int ret=0;
    ret=wlmngr_handle_special_var(hdr,varName,varValue,WLMNGR_VAR_OPER_VALIDATE);
    WLCSM_TRACE(WLCSM_TRACE_DBG, "ret:%d",ret);
    return ret== WLMNGR_VAR_HANDLE_FAILURE?WLMNGR_VAR_HANDLE_FAILURE:0;
}

static int _wlmngr_handle_wlcsm_cmd_reg_dm_event(t_WLCSM_MNGR_VARHDR  *hdr, char *varName,char *varValue)
{
    /* sub_idx is the event number */
    WLCSM_TRACE(WLCSM_TRACE_LOG," REGISTER event:%d \r\n",hdr->sub_idx );
    return  wlcsm_dm_reg_event(hdr->sub_idx,WLCSM_MNGR_CMD_GET_SAVEDM(hdr->radio_idx));
}
static int _wlmngr_handle_wlcsm_cmd_pwrreset_event(t_WLCSM_MNGR_VARHDR  *hdr, char *varName,char *varValue)
{
#ifdef IDLE_PWRSAVE
    wlmngr_togglePowerSave();
#endif
    return 0;
}
static int _wlmngr_handle_wlcsm_cmd_setdmdbglevel_event(t_WLCSM_MNGR_VARHDR  *hdr, char *varName,char *varValue)
{
    return wlcsm_dm_set_dbglevel(varValue);
}

static int
wl_send_dif_event(const char *ifname, uint32 event)
{
    static int s = -1;
    int len, n;
    struct sockaddr_in to;
    char data[IFNAMSIZ + sizeof(uint32)];

    /* create a socket to receive dynamic i/f events */
    if (s < 0) {
        s = socket(AF_INET, SOCK_DGRAM, 0);
        if (s < 0) {
            perror("socket");
            return -1;
        }
    }

    /* Init the message contents to send to eapd. Specify the interface
     * and the event that occured on the interface.
     */
    strncpy(data, ifname, IFNAMSIZ);
    *(uint32 *)(data + IFNAMSIZ) = event;
    len = IFNAMSIZ + sizeof(uint32);

    /* send to eapd */
    to.sin_addr.s_addr = inet_addr(EAPD_WKSP_UDP_ADDR);
    to.sin_family = AF_INET;
    to.sin_port = htons(EAPD_WKSP_DIF_UDP_PORT);

    n = sendto(s, data, len, 0, (struct sockaddr *)&to,
               sizeof(struct sockaddr_in));

    if (n != len) {
        perror("udp send failed\n");
        return -1;
    }

    dprintf("hotplug_net(): sent event %d\n", event);

    return n;
}

static int _wlmngr_handle_wlcsm_cmd_nethotplug_event(t_WLCSM_MNGR_VARHDR  *hdr, char *interface,char *action)
{
    bool psta_if, dyn_if,wds_if, add_event, remove_event, monitor_if;
    char temp_s[256] = {0};
    char wdsap_ifname[32]= {0};
    char *lan_ifname=NULL;

    add_event = !WLCSM_STRCMP(action, "add");
    remove_event = !WLCSM_STRCMP(action, "remove");
    psta_if = wl_wlif_is_psta(interface);
    monitor_if = !strncmp(interface, "radiotap", 8);
    wds_if = !strncmp(interface, "wds", 3);
    dyn_if = wds_if || psta_if || monitor_if;

    WLCSM_TRACE(WLCSM_TRACE_DBG, "interface:%s and action:%s",interface,action);

    if (!dyn_if && !remove_event)
        return 0;

    if(wds_if && !wl_wlif_wds_ap_ifname(interface, wdsap_ifname))
        get_bridge_by_ifname(wdsap_ifname, &lan_ifname);
    /* default bridge to "br0" if no lan_ifname nvram specified */
    if(!lan_ifname && !(lan_ifname=nvram_get("lan_ifname")))
        lan_ifname="br0";

    if (add_event) {
        /* only need to do bridge opernation for wds dynamic interface */
        if(wds_if) {
            snprintf(temp_s,255,"brctl addif %s %s",lan_ifname, interface);
            system(temp_s);
        }
        snprintf(temp_s,255,"ifconfig %s up",interface);
        system(temp_s);
        if(psta_if)
            wl_send_dif_event(interface, 0);
        return 0;
    }

    if (remove_event) {
        /* Indicate interface delete event to eapd */
        snprintf(temp_s,255,"brctl delif %s %s",lan_ifname, interface);
        system(temp_s);
        if(psta_if)
            wl_send_dif_event(interface, 1);
    }

    return 0;
}
static int _wlmngr_handle_wlcsm_cmd_set_dmvar(t_WLCSM_MNGR_VARHDR  *hdr, char *varName,char *varValue)
{
    unsigned int pos=0;
    int ret=0;
    WLCSM_NAME_OFFSET *name_offset= wlcsm_dm_get_mngr_entry(hdr,varValue,&pos);
    if(!name_offset) {
        WLCSM_TRACE(WLCSM_TRACE_LOG," reurn not successull \r\n" );
        ret=-1;
    } else {
        WLCSM_TRACE(WLCSM_TRACE_LOG,"dm oid is matching to wlmngr %s and offset:%d\r\n",name_offset->name,name_offset->offset);
        ret=__wlmngr_handle_wlcsm_var_validate(hdr,name_offset->name,varValue);
        if(!ret) {
            WLCSM_TRACE(WLCSM_TRACE_LOG," set wlmngr var by DM OID \r\n" );
            WLCSM_MNGR_CMD_SET_CMD(hdr->radio_idx,0);
            ret=wlmngr_set_var(hdr,name_offset->name,varValue);
        }
    }
    return ret;
}

static int _wlmngr_handle_wlcsm_cmd_validate_dmvar(t_WLCSM_MNGR_VARHDR  *hdr, char *varName,char *varValue)
{

    unsigned int pos=0;
    int ret=0;
    WLCSM_NAME_OFFSET *name_offset= wlcsm_dm_get_mngr_entry(hdr,varValue,&pos);

    if(!name_offset) {
        WLCSM_TRACE(WLCSM_TRACE_LOG," reurn not successull \r\n" );
        ret=-1;
    } else {
        WLCSM_TRACE(WLCSM_TRACE_LOG,"dm oid is matching to wlmngr %s and offset:%d\r\n",name_offset->name,name_offset->offset);
        ret=__wlmngr_handle_wlcsm_var_validate(hdr,name_offset->name,varValue);
    }
    return ret;
}

//
//**************************************************************************
// Function Name: getGPIOverlays
// Description  : get the value of GPIO overlays
// Parameters   : interface idx.
// Returns      : none.
//**************************************************************************
unsigned long wlmngr_adjust_GPIOOverlays(const unsigned int idx )
{
    int f = open( "/dev/brcmboard", O_RDWR );
    unsigned long  GPIOOverlays = 0;
    if( f > 0 ) {
        BOARD_IOCTL_PARMS IoctlParms;
        memset( &IoctlParms, 0x00, sizeof(IoctlParms) );
        IoctlParms.result = -1;
        IoctlParms.string = (char *)&GPIOOverlays;
        IoctlParms.strLen = sizeof(GPIOOverlays);
        ioctl(f, BOARD_IOCTL_GET_GPIOVERLAYS, &IoctlParms);
        WL_RADIO(idx).GPIOOverlays=GPIOOverlays;
        WLCSM_TRACE(WLCSM_TRACE_LOG," ---SETTING GPIOOverlay---:%u \r\n",GPIOOverlays );
        close(f);
    }
    return GPIOOverlays;
}

//**************************************************************************
// Function Name: setASPM
// Description  : set aspm according to GPIOOverlays
// Parameters   : interface idx.
// Returns      : none.
//**************************************************************************
void wlmngr_setASPM(const unsigned int idx )
{

#ifdef IDLE_PWRSAVE
    {
        char cmd[WL_SIZE_132_MAX];
        int is_dhd=0;
        snprintf(cmd,sizeof(cmd),"wl%d",idx);
        is_dhd=!dhd_probe(cmd);
        /* Only enable L1 mode, because L0s creates EVM issues. The power savings are the same */
        if (WL_RADIO(idx).GPIOOverlays & BP_OVERLAY_PCIE_CLKREQ) {
            if(is_dhd)
                snprintf(cmd, sizeof(cmd), "dhd -i wl%d aspm 0x102", idx);
            else
                snprintf(cmd, sizeof(cmd), "wl -i wl%d aspm 0x102", idx);
        } else {
            if(is_dhd)
                snprintf(cmd, sizeof(cmd), "dhd -i wl%d aspm 0x2", idx);
            else
                snprintf(cmd, sizeof(cmd), "wl -i wl%d aspm 0x2", idx);
        }

        bcmSystem(cmd);
    }
#endif
}


static  void _wlmngr_nv_adjust_security(const unsigned int idx,SYNC_DIRECTION direction)
{
    char buf[WL_SIZE_512_MAX];

    nvram_set("wl_key1",WL_APSEC_WLKEY1(idx,0));
    nvram_set("wl_key2",WL_APSEC_WLKEY2(idx,0));
    nvram_set("wl_key3",WL_APSEC_WLKEY3(idx,0));
    nvram_set("wl_key4",WL_APSEC_WLKEY4(idx,0));
    snprintf(buf, sizeof(buf), "%d",WL_APSEC_WLKEYINDEX(idx,0));
    nvram_set("wl_key",buf);
    nvram_set("wl_wep",WL_APSEC_WLWEP(idx,0));
}

static  void _wlmngr_nv_adjust_wps(const unsigned int idx ,SYNC_DIRECTION direction)
{
    int br,i;
    char cmd[WL_SIZE_256_MAX]= {0};

    if ( direction== WL_SYNC_FROM_DM ) {
        for(i=0; i<WL_RADIO_WLNUMBSS(idx); i++) {
            if(WL_BSSID_BRIDGE_NAME(idx,i)) {
                br = WL_BSSID_BRIDGE_NAME(idx,i)[2] - 0x30;
                if ( br == 0 )
                    snprintf(cmd, sizeof(cmd), "lan_wps_oob");
                else
                    snprintf(cmd, sizeof(cmd), "lan%d_wps_oob", br);

                if(WLCSM_STRCMP(WL_APWPS_WLWSCAPMODE(idx,i),"0"))
                    nvram_set(cmd,"disabled");
                else
                    nvram_set(cmd,"enabled");
            } else
                WLCSM_TRACE(WLCSM_TRACE_ERR,"ERROR: NO bridge interface name???? \r\n" );
        }
    }
}

static  void _wlmngr_nvram_adjust(const unsigned int idx,int direction)
{

    _wlmngr_nv_adjust_wps(idx,direction);
    _wlmngr_nv_adjust_security(idx,direction);
#ifndef NO_CMS
    wlmngr_nv_adjust_chanspec(idx,direction);
#endif
#ifdef BCM_WBD
    _wlmngr_nv_adjust_wbd(idx,direction);
#endif

}

static  void _wlmngr_adjust_country_rev(const unsigned int idx , SYNC_DIRECTION  direction)
{
    char ccode[10]= {0}; /* make it bigger to tolerate the wrong counryrev */
    snprintf(ccode, sizeof(ccode), "%s/%d",WL_RADIO_COUNTRY(idx),WL_RADIO_REGREV(idx));
    wlcsm_strcpy(&WL_RADIO_COUNTRYREV(idx),ccode);
    WLCSM_TRACE(WLCSM_TRACE_LOG," countryrev:%s \r\n",WL_RADIO_COUNTRYREV(idx));
}

void wlmngr_adjust_radio_runtime(int idx)
{
    int i=0;
    char wlver[WL_VERSION_STR_LEN]= {0};
    char cmd[WL_SIZE_132_MAX]= {0};
    char buf[WL_CAP_STR_LEN]= {0};

    WL_RADIO(idx).wlCoreRev=wlmngr_getCoreRev(idx);
    WL_RADIO(idx).wlRate=wlmngr_getValidRate(idx, WL_RADIO(idx).wlRate);
    WL_RADIO(idx).wlMCastRate=  wlmngr_getValidRate(idx, WL_RADIO(idx).wlMCastRate);
    WL_RADIO(idx).wlBand= wlmngr_getValidBand(idx,WL_RADIO(idx).wlBand);

    wlcsm_strcpy(&WL_PHYTYPE(idx),wlmngr_getPhyType(idx));

    if(wlmngr_getVer(idx,wlver)) {
        wlcsm_strcpy(&(WL_RADIO(idx).wlVersion),wlver);
        WLCSM_TRACE(WLCSM_TRACE_LOG," version is:%s \r\n",WL_RADIO(idx).wlVersion );
    } else
        WLCSM_TRACE(WLCSM_TRACE_ERR," ERROR:could not get adapter version \r\n" );

    snprintf(cmd, sizeof(cmd), "wl%d", idx);
    wl_iovar_get(cmd, "cap", (void *)buf, WL_CAP_STR_LEN);


    if(strstr(buf, "afterburner")) {
        WL_RADIO(idx).wlHasAfterburner=TRUE;
    }
    if(strstr(buf, "ampdu")) {
        WL_RADIO(idx).wlAmpduSupported=TRUE;
    } else {
        /* A-MSDU doesn't work with AMPDU */
        if(strstr(buf, "amsdu")) {
            WL_RADIO(idx).wlAmsduSupported=TRUE;
        }
    }

    if(strstr(buf, "wme")) {
        WL_RADIO_WLHASWME(idx)=TRUE;
        for(i=0; i<WL_RADIO_WLNUMBSS(idx); i++) {
            WL_AP_WLWME(idx,i)=TRUE;
        }
    }

    if(strstr(buf, "mbss"))  {
        WL_RADIO_WLSUPPORTMBSS(idx)=TRUE;
    }

    if(strstr(buf, "sta")) {
        WL_RADIO_WLHASAPSTA(idx)=1;
    }

    if(strstr(buf, "vec")) {
        WL_RADIO_WLHASVEC(idx)=1;
    }

#ifdef WMF
    /* for WMF-- we enable then unconditionally for now because of 43602 dongle  */
    WL_RADIO_WLHASWMF(idx)=TRUE;
#endif

    for(i=0; i<WL_RADIO_WLNUMBSS(idx); i++) {
        if ( strncmp(WL_RADIO(idx).wlNmode, WL_OFF,strlen(WL_OFF)) &&
                ( strncmp(WL_PHYTYPE(idx), WL_PHY_TYPE_N,strlen(WL_PHY_TYPE_N)) ||
                  strncmp(WL_PHYTYPE(idx), WL_PHY_TYPE_AC,strlen(WL_PHY_TYPE_AC))) &&
                !strncmp(WL_APSEC(idx,i).wlWpa, "tkip",4))
            wlcsm_strcpy(&(WL_APSEC(idx,i).wlWpa), "tkip+aes");

    }
}


/*************************************************************//* *
  * @brief  internal API to write nvram by parameter values
  *
  * 	adjust wlmngr variables to solve the relationship between
  * 	variables and run_time configurationjust for one adapter,
  *
  * @return void
  ****************************************************************/
static  void _wlmmgr_vars_adjust(const unsigned int idx ,SYNC_DIRECTION direction)
{

    _wlmngr_adjust_country_rev(idx, direction);

    if ( direction==WL_SYNC_FROM_DM ) {

        wlmngr_adjust_GPIOOverlays(idx); /* GPIOOverlay is run time, only for this FROM DM direction */
        wlmngr_adjust_radio_runtime(idx);

    } else {

        WLCSM_TRACE(WLCSM_TRACE_LOG," !!!!TODO:--- Adjust VAR befor write to DM \r\n" );

    }
    wlmngr_update_possibble_channels(idx,direction);
}

#ifdef NO_CMS
void _wlmngr_handle_bridge_setup(void) {
    char cmd[WL_SIZE_132_MAX];
    int i,j;
    for (i = 0; i < WL_WIFI_RADIO_NUMBER; i++) {
        for (j = 0 ; j<WL_RADIO_WLMAXMBSS(i); j++) {
            snprintf(cmd,sizeof(cmd),"brctl addif %s %s",WL_BSSID_WLBRNAME(i,j),WL_BSSID_IFNAME(i,j));
            bcmSystem(cmd);
        }
    }
}
#endif

#ifdef DSLCPE_1905
WLMNGR_1905_CREDENTIAL g_wlmngr_wl_credentials[MAX_WLAN_ADAPTER][WL_MAX_NUM_SSID];
typedef struct struct_value {
    int offset;
    char *name;
    char isInt;
} STRUCT_VALUE;


static int _wlmngr_check_config_status_change (const unsigned int idx , int *bssid,int *credential_changed, int *change_status)
{

    STRUCT_VALUE items[]= {
        {CREDENTIAL_VAROFF(wlSsid),	 	"ssid",		0},
        {CREDENTIAL_VAROFF(wlWpaPsk), 	        "wpa_psk",	0},
        {CREDENTIAL_VAROFF(wlAuthMode), 	"akm",		0},
        {CREDENTIAL_VAROFF(wlAuth),		"auth",		1},
        {CREDENTIAL_VAROFF(wlWep), 		"wep",		0},
        {CREDENTIAL_VAROFF(wlWpa), 		"crypto", 	0},
        {-1,NULL}
    };
    STRUCT_VALUE *item=items;
    WLMNGR_1905_CREDENTIAL    *pmssid;
    int i=0,ret=0;
    char buf[32],name[32],tmp[100],*str;
    int br=0;
    int oob=0;

    *bssid = 0;

    for (i = 0 ; i<WL_RADIO_WLMAXMBSS(idx); i++) {
        br = WL_BSSID_WLBRNAME(idx,i)[2] - 0x30;

        if ( br == 0 )
            snprintf(name, sizeof(name), "lan_wps_oob");
        else
            snprintf(name, sizeof(name), "lan%d_wps_oob", br);
        strncpy(buf, nvram_safe_get(name), sizeof(buf));
        if ( WLCSM_STRCMP(buf,"enabled"))
            oob=2;
        else
            oob=1;

        if(((g_wlmngr_wl_credentials[idx][i].lan_wps_oob)&0xf)!=oob) {
            if(oob==1)  *change_status= WPS_1905_CONF_TO_UNCONF;
            else *change_status= WPS_1905_UNCONF_TO_CONF;
            *bssid=i;
            *credential_changed=0;
            g_wlmngr_wl_credentials[idx][i].lan_wps_oob=oob;
            ret=1;
        }

        if(ret==0)	 {
            if(oob==1)  *change_status= WPS_1905_CONF_NOCHANGE_UNCONFIGURED;
            else *change_status= WPS_1905_CONF_NOCHANGE_CONFIGURED;
        }

        snprintf(name, sizeof(name), "%s_", WL_BSSID_IFNAME(idx,i));
        pmssid=(WLMNGR_1905_CREDENTIAL *)(&(g_wlmngr_wl_credentials[idx][i]));
        for(item=items; item->name!=NULL; item++) {
            str=nvram_get(strcat_r(name, item->name, tmp));
            /* for akm, it is kind of special*/
            if(!WLCSM_STRCMP(item->name,"akm") && (str==NULL||strlen(str)==0)) {
                str="open";
            }
            if(!item->isInt) {
                char *value=CREDENTIAL_STRVARVALUE(pmssid,item->offset);
                if((str==NULL && strlen(value)!=0)|| (str!=NULL && WLCSM_STRCMP(str,value))) {
                    *bssid=i;
                    *credential_changed=1;
                    ret= 1;
                }
                if(ret) {
                    if(!str)  value[0]='\0';
                    else strcpy(value,str);
                }
            } else {
                int *value=CREDENTIAL_INTVARPTR(pmssid,item->offset);
                if(str!=NULL && atoi(str)!=*value) {
                    *bssid=i;
                    ret= 1;
                    *credential_changed=1;
                    *value=atoi(str);
                }

            }
        }
    }
    /* use that bit to indicat if this wlmngr boot time, if not boot time, use return value */
    if(!(g_wlmngr_wl_credentials[idx][0].lan_wps_oob&0x10)) {
        g_wlmngr_wl_credentials[idx][0].lan_wps_oob|=0x10;
        return 0;
    }
    return ret;
}

/** @brief  checking if wlan credential changed
*
*   check if wireless lan credential changed when some application change it from TR69 etc APPs
*/

static int open_udp_socket(char *addr, uint16 port)
{
    int reuse = 1;
    int sock_fd;
    struct sockaddr_in sockaddr;

    /*  open loopback socket to communicate with EAPD */
    memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = inet_addr(addr);
    sockaddr.sin_port = htons(port);

    if ((sock_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        WLCSM_TRACE(WLCSM_TRACE_LOG, "Unable to create loopback socket\n");
        goto exit0;
    }

    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse)) < 0) {
        WLCSM_TRACE(WLCSM_TRACE_LOG, "Unable to setsockopt to loopback socket %d.\n", sock_fd);
        goto exit1;
    }

    if (bind(sock_fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0) {
        WLCSM_TRACE(WLCSM_TRACE_LOG, "Unable to bind to loopback socket %d\n", sock_fd);
        goto exit1;
    }
    WLCSM_TRACE(WLCSM_TRACE_LOG, "opened loopback socket %d in port %d\n", sock_fd, port);
    return sock_fd;

    /*  error handling */
exit1:
    close(sock_fd);

exit0:
    WLCSM_TRACE(WLCSM_TRACE_LOG, "failed to open loopback socket\n");
    return -1;
}


/*
* ===  FUNCTION  ======================================================================
*         Name:  _wlmngr_send_notification_1905
*  Description:
* =====================================================================================
*/

static int _wlmngr_send_notification_1905 (const unsigned int idx ,int bssid,int credential_changed, int conf_status)
{
    struct sockaddr_in sockAddr;
    int rc;
    int port;
    char *portnum=nvram_safe_get("1905_com_socket");
    char nvramVar[32];
    rc = sscanf(portnum,"%d",&port);
    if ( rc == 1 ) {
        int sock_fd;

        sock_fd=open_udp_socket(WPS_1905_ADDR,WPS_1905_PORT);
        if(sock_fd>=0) {
            int buflen = sizeof(WPS_1905_MESSAGE) + sizeof(WPS_1905_NOTIFY_MESSAGE);
            WPS_1905_NOTIFY_MESSAGE notify_msg;
            WPS_1905_MESSAGE *pmsg = (WPS_1905_MESSAGE *)malloc(buflen);
            if(pmsg) {
                notify_msg.confStatus=conf_status;
                notify_msg.credentialChanged=credential_changed;
                snprintf(notify_msg.ifName, sizeof(notify_msg.ifName), "%s",WL_BSSID_IFNAME(idx,bssid));

                memset(pmsg,'\0',buflen);
                pmsg->cmd=WPS_1905_NOTIFY_CLIENT_RESTART;
                pmsg->len=sizeof(WPS_1905_NOTIFY_MESSAGE);
                pmsg->status=1;
                memcpy((char *)(pmsg+1),&notify_msg,sizeof(WPS_1905_NOTIFY_MESSAGE));

                /*  kernel address */
                memset(&sockAddr, 0, sizeof(sockAddr));
                sockAddr.sin_family      = AF_INET;
                sockAddr.sin_addr.s_addr = htonl(0x7f000001); /*  127.0.0.1 */
                sockAddr.sin_port        = htons(port);

                rc = sendto(sock_fd, pmsg, buflen, 0, (struct sockaddr *)&sockAddr,sizeof(sockAddr));
                free(pmsg);
                close(sock_fd);
                if (buflen != rc) {
                    printf("%s: sendto failed", __FUNCTION__);
                    return 1;
                } else {
                    return 0;
                }
            }
        }
    }
    return 1;
}


#endif
/*************************************************************//* *
  * @brief  internal API to write nvram by parameter values
  *
  *	some parameters mapping to nvram entries in the system where
  *	wlconf depends on to configure wl interfaces. This API will read
  *	the mapping and write each single Nvram entry.
  *
  * @return void
  ****************************************************************/
void _wlmngr_write_wl_nvram(const unsigned int idx)
{
    char name[128];
    int i=0,j=0,entries_num=0;

    WLCSM_NVRAM_MNGR_MAPPING *mapping;
    WLCSM_WLAN_ADAPTER_STRUCT *adapter;

    adapter=&(gp_adapter_objs[idx]);
    char *value,tmp_str[1024];
    entries_num=sizeof(g_wlcsm_nvram_mngr_mapping)/sizeof(WLCSM_NVRAM_MNGR_MAPPING);

    for(i=0; i<adapter->radio.wlNumBss; i++) {

        for ( j=0; j<entries_num; j++ ) {

            mapping= &(g_wlcsm_nvram_mngr_mapping[j]);
            if(mapping->type==MNGR_GENERIC_VAR) continue;
            else if((i==0 || ((i>0) && mapping->type==MNGR_SSID_SPECIFIC_VAR))) {
                if(!i)
                    snprintf(name, sizeof(name), "wl%d_%s",idx,mapping->nvram_var);
                else
                    snprintf(name, sizeof(name), "wl%d.%d_%s",idx,i,mapping->nvram_var);

                WLCSM_TRACE(WLCSM_TRACE_LOG," j:%d,name of nvram is:%s \r\n",j,name);

                value=wlcsm_mapper_get_mngr_value(idx,i, mapping,tmp_str);
                if(value)
                    wlcsm_nvram_set(name,value);
                WLCSM_TRACE(WLCSM_TRACE_LOG," j:%d,name of nvram is:%s and value:%s \r\n",j,name,value?value:"NULL");
            }
        }
    }
}


#if defined(HSPOT_SUPPORT)

static void wlmngr_HspotCtrl(void)
{

    /* First to kill all hspotap process if already start*/
    bcmSystem("killall -q -15 hspotap 2>/dev/null");
    bcmSystem("hspotap&");
}
#endif

static bool enableBSD(void)
{
    int i=0,ret=FALSE;
    char buf[WL_MID_SIZE_MAX];
    nvram_set("bsd_role","0");
    if( act_wl_cnt == 0 ) return FALSE;
    for (i=0; i<WL_WIFI_RADIO_NUMBER; i++) {
        if( WL_RADIO_WLENBL(i) == TRUE && WL_RADIO_BSDROLE(i) > 0 ) {
            snprintf(buf, sizeof(buf), "%d", WL_RADIO_BSDROLE(i));
            nvram_set("bsd_role",buf);
            snprintf(buf, sizeof(buf), "%d", WL_RADIO_BSDPPORT(i));
            nvram_set("bsd_pport",buf);
            snprintf(buf, sizeof(buf), "%d", WL_RADIO_BSDHPORT(i));
            nvram_set("bsd_hpport",buf);
            snprintf(buf, sizeof(buf), "%s", WL_RADIO_BSDHELPER(i));
            nvram_set("bsd_helper",buf);
            snprintf(buf, sizeof(buf), "%s", WL_RADIO_BSDPRIMARY(i));
            nvram_set("bsd_primary",buf);
            ret=TRUE;
            break;
        }
    }

#ifdef BCM_WBD
    ret=TRUE;
#endif
    return ret;
}

static void wlmngr_BSDCtrl(void )
{
    /* First to kill all bsd process if already start*/
    bcmSystem("killall -q -15 bsd 2>/dev/null");
    if ( enableBSD() == TRUE )
        bcmSystem("bsd&");
}

static bool enableSSD()
{
    int i=0;
    char buf[WL_MID_SIZE_MAX];
    nvram_set("ssd_enable","0");
    if( act_wl_cnt == 0 ) return FALSE;
    for (i=0; i<WL_WIFI_RADIO_NUMBER; i++)
        if( WL_RADIO_WLENBL(i) == TRUE && WL_RADIO_SSDENABLE(i) > 0 ) {
            snprintf(buf, sizeof(buf), "%d", WL_RADIO_SSDENABLE(i));
            nvram_set("ssd_enable",buf);
            return TRUE;
        }
    return FALSE;
}

static void wlmngr_SSDCtrl(void)
{
    /* First to kill all ssd process if already start*/
    bcmSystem("killall -q -15 ssd 2>/dev/null");
    if ( enableSSD() == TRUE )
        bcmSystem("ssd&");
}

static unsigned int _bits_count(unsigned int  n)
{
    unsigned int count = 0;
    while ( n > 0 ) {
        if ( n & 1 )
            count++;
        n >>= 1;
    }
    return count;
}

void wlmngr_startAcsd(void)
{
    int timeout = 0;
    char cmd[WL_SIZE_132_MAX];
    int i=0,enabled=0;
    for(i=0; i<WL_WIFI_RADIO_NUMBER; i++)
        if(WL_BSSID_WLENBLSSID(i,0) && WL_RADIO_WLENBL(i)) enabled=1;
    if(enabled) { /* not background, due to acs init scan should be before wl up */
        bcmSystem("acsd");
        for(i=0; i<WL_WIFI_RADIO_NUMBER; i++) {
            timeout = WL_RADIO(i).wlCsScanTimer*60;
            if(timeout) {
                snprintf(cmd, sizeof(cmd), "acs_cli -i %s acs_cs_scan_timer %d", WL_BSSID_IFNAME(i,0), timeout);
                bcmSystem(cmd);
            }
        }
    }
}

#if defined(BCM_APPEVENTD)
int wlmngr_start_appeventd(void)
{
    if (nvram_match("appeventd_enable", "1"))
    {
        bcmSystem("appeventd&");
    }

    return 1;
}

int wlmngr_stop_appeventd(void)
{
    bcmSystem("killall -q -9 appeventd 2>/dev/null");
    return 1;
}
#endif

void wlmngr_postStart_Service(void)
{
#if defined(HSPOT_SUPPORT)
    wlmngr_HspotCtrl();
#endif
#ifdef __CONFIG_TOAD__
    bcmSystem("toad");
#endif
#ifdef EXT_ACS
    /* Usermode autochannel */
    if (wlmngr_detectAcsd())
        wlmngr_startAcsd();
#endif
#ifdef __CONFIG_VISUALIZATION__
    bcmSystem("vis-dcon");
    bcmSystem("vis-datacollector");
#endif
#if defined(BCM_APPEVENTD)
    wlmngr_start_appeventd();
#endif
#ifdef BCM_WBD
    {
        char tmp[64];
        char *lan_ifname;
        lan_ifname=wlcsm_nvram_get("wbd_mode");
        if(lan_ifname) {
            if(!strncmp(lan_ifname,"2",1)) {
                lan_ifname=wlcsm_nvram_get("wbd_default_gateway");
                if(lan_ifname) {
                    bcmSystem("ip route flush 0/0");
                    snprintf(tmp,sizeof(tmp),"route add default gw %s",lan_ifname);
                    bcmSystem(tmp);
                }
            } else
                bcmSystem("wbd_master");
            bcmSystem("wbd_slave");
        }
    }
#endif

}


extern char *wlcsm_prefix_match(char *name);

static int _wlmngr_tobe_saved_nvram(char *name)
{

    char *prefix=wlcsm_prefix_match(name);
    int index=0;
    if(prefix) {
        int entries_num=sizeof(g_wlcsm_nvram_mngr_mapping)/sizeof(WLCSM_NVRAM_MNGR_MAPPING);
        char *var_name=name+strlen(prefix);
        char *nvram_var;
        for ( index=0; index<entries_num; index++) {
            nvram_var= g_wlcsm_nvram_mngr_mapping[index].nvram_var;
            if(!strncmp(var_name,nvram_var,strlen(nvram_var))) return 0;
        }
    }
    return 1;
}


/**
 * wlmngr write run time nvram to data module, only update nvram which is not in regular entries
 */
int  wlmngr_save_nvram(void)
{
    char *name;
    char *pair,*buf = malloc(MAX_NVRAM_SPACE);
    int pair_len_max=WL_LG_SIZE_MAX+WL_SIZE_132_MAX;
    if(!buf) {
        fprintf(stderr,"could not allocate memory for buf\n");
        return -1;
    } else if(!(pair=malloc(WL_LG_SIZE_MAX+WL_SIZE_132_MAX))) {
        fprintf(stderr,"could not allocate memory for pair\n");
        free(buf);
        return -1;
    }

    if(g_wifi_obj.nvram) {
        free(g_wifi_obj.nvram);
        g_wifi_obj.nvram=NULL;
    }

    if(!(g_wifi_obj.nvram=malloc(MAX_NVRAM_SPACE))) {
        fprintf(stderr,"could not allocate memory for buf\n");
        free(buf);
        free(pair);
        return -1;
    }

    nvram_getall(buf, MAX_NVRAM_SPACE*sizeof(char));
    memset(g_wifi_obj.nvram,0,MAX_NVRAM_SPACE);
    strcat(g_wifi_obj.nvram,"FFFF");
    for (name = buf; *name && (strlen(g_wifi_obj.nvram)<MAX_NVRAM_SPACE); name += strlen(name) + 1) {
        if(!strncmp(name,"wps_force_restart", strlen("wps_force_restart"))
                || !strncmp(name, "pair_len_byte=", strlen("pair_len_byte="))
                || !strncmp(name, "acs_ifnames", strlen("acs_ifnames"))
                || !strncmp(name, "wl_unit", strlen("wl_unit")))
            continue;
        if(_wlmngr_tobe_saved_nvram(name)) {

#if defined(HSPOT_SUPPORT)
            if(! wlcsm_hspot_var_isdefault(name)) {
#endif
                snprintf(pair, pair_len_max, "%03X%s", strlen(name), name);
                strcat(g_wifi_obj.nvram, pair);
#if defined(HSPOT_SUPPORT)
            }
#endif
        }
    }
    wlcsm_dm_save_nvram();
    free(g_wifi_obj.nvram);
    g_wifi_obj.nvram=NULL;
    free(buf);
    free(pair);
    return 0;
}


#if 0 //XXXX temp comment out
extern struct nvram_tuple router_defaults_override_type1[];
void wlmngr_devicemode_overwrite(const unsigned int radio_idx)
{
    char name[100];
    int i = 0;
    struct nvram_tuple *t;
    if(!strncmp(nvram_safe_get("devicemode"), "1", 1)) {
        for (t = router_defaults_override_type1; t->name; t++) {
            if(!strncmp(t->name,"router_disable",14))
                continue;
            if (!strncmp(t->name, "wl_", 3)) {
                for (i = 0; i < WL_RADIO_WLNUMBSS(radio_idx) && i < WL_MAX_NUM_SSID; i++) {
                    WLCSM_TRACE(WLCSM_TRACE_ERR, " set %s= %s\r\n", t->name, t->value);
                    if(!i)
                        sprintf(name, "wl%d_%s", radio_idx, t->name + 3);
                    else
                        sprintf(name, "wl%d.%d_%s", radio_idx, i, t->name + 3);
                    WLCSM_TRACE(WLCSM_TRACE_ERR, " set %s= %s\r\n", name, t->value);
                    nvram_set(name, t->value);
                    wlcsm_nvram_update_runtime_mngr(name, t->value);
                    wlmngr_special_nvram_handler(name, t->value);
                }
            } else {
                nvram_set(t->name, t->value);
                WLCSM_TRACE(WLCSM_TRACE_ERR, " set %s= %s\r\n", t->name, t->value);
            }
        }
        nvram_set("pre_devicemode","1");
    }  else {
        char *premode= nvram_get("pre_devicemode");
        if(premode && (!strncmp(premode, "1", 1))) {
            fprintf(stderr," DEVICE MODE CHANGED!!! SUGGEST TO RESTORE DEFAULT AND RECONFIGURE DEVICE!!!! \r\n");
        }
    }
}
#endif

void wlmngr_pre_setup(const unsigned int idx )
{

    char *restart;
    restart = nvram_get("wps_force_restart");
    if ( restart!=NULL && (strncmp(restart, "y", 1)!= 0) &&  (strncmp(restart, "Y", 1)!= 0)  ) {
        nvram_set("wps_force_restart", "Y");
    } else {
        wlmngr_wlIfcDown(idx);
        wlmngr_WlConfDown(idx);
    }
}


static int wlmngr_stop_wps(void)
{
    int ret = 0;
    FILE *fp = NULL;
    char saved_pid[32],cmd[64];
    int i, wait_time = 3;
    pid_t pid;

    if (((fp = fopen("/tmp/wps_monitor.pid", "r")) != NULL) &&
            (fgets(saved_pid, sizeof(saved_pid), fp) != NULL)) {
        /* remove new line first */
        for (i = 0; i < sizeof(saved_pid); i++) {
            if (saved_pid[i] == '\n')
                saved_pid[i] = '\0';
        }
        saved_pid[sizeof(saved_pid) - 1] = '\0';
        snprintf(cmd,64,"kill %s",saved_pid);
        bcmSystem(cmd);

        do {
            if ((pid = get_pid_by_name("/bin/wps_monitor")) <= 0)
                break;
            wait_time--;
            sleep(1);
        } while (wait_time);

        if (wait_time == 0) {
            printf("Unable to kill wps_monitor!\n");
            ret=1;
        }
    }
    if (fp) {
        fclose(fp);
        if(!ret)
            bcmSystem("rm -rf /tmp/wps_monitor.pid");
    }

    return ret;
}

void wlmngr_setup(unsigned int idx )
{

    char cmd[WL_SIZE_132_MAX];
#ifdef SUPPORT_WSC
    char *restart;
#endif
    //  wlmngr_devicemode_overwrite(idx);
    snprintf(cmd, sizeof(cmd), "wl -i wl%d phy_watchdog 0", idx);
    WLCSM_TRACE(WLCSM_TRACE_LOG," cmd:%s \r\n",cmd );
    BCMWL_WLCTL_CMD(cmd);
#ifdef SUPPORT_WSC
    restart = nvram_get("wps_force_restart");
    if ( restart!=NULL && (strncmp(restart, "y", 1)!= 0) &&  (strncmp(restart, "Y", 1)!= 0)  ) {
        nvram_set("wps_force_restart", "Y");
    }
#endif
    wlmngr_setupMbssMacAddr(idx);
    wlmngr_enum_ifnames(idx);
    if ( WL_RADIO_WLENBL(idx) == TRUE ) {
        if (!wlmngr_detectAcsd())
            wlmngr_autoChannel(idx);
        while(!g_wlmngr_ready_for_event)
            sleep(1);
        wlmngr_doWlConf(idx);
        wlmngr_setup_if_mac(idx);
        wlmngr_doSecurity(idx);
        wlmngr_doWdsSec(idx);
    }
}

//**************************************************************************
// Function Name: doQoS
// Description  : setup ebtables marking for wireless interface.
// Parameters   : none.
// Returns      : None.
//**************************************************************************
static void wlmngr_doQoS(unsigned int idx)
{
    WLCSM_TRACE(WLCSM_TRACE_DBG," TODO: wlmngr_doQOS, how to .... \r\n" );
    if (!WL_AP_WLWME(idx,0))
        return;

#if 0
#ifdef DMP_QOS_1
    /* all of this code assumes the old TR98 way of doing things.
     * In TR181, the interface between wlan and CMS will be cleaner.
     */
    if (m_instance_wl[idx].m_wlVar.wlWme) {
        for (i=0; i<WL_RADIO(idx).wlNumBss; i++) {
            if (WL_BSSID_WLENBLSSID(idx,i) && m_instance_wl[idx].m_wlVar.wlEnbl) {
                /* enable all the default queues associated with this ssid. */
                if ((ret = dalQos_setDefaultWlQueuesLock(WL_BSSID_IFNAME(idx,i), TRUE)) != CMSRET_SUCCESS) {
                    printf("dalQos_setDefaultWlQueuesLock() returns error. ret=%d\n", ret);
                    return;
                }
            } else {
                /* disable all the default queues associated with this ssid. */
                if ((ret = dalQos_setDefaultWlQueuesLock(WL_BSSID_IFNAME(idx,i), FALSE)) != CMSRET_SUCCESS) {
                    printf("dalQos_setDefaultWlQueuesLock() returns error. ret=%d\n", ret);
                    return;
                }
            }
        }
    } else {
        /* disable all the default wireless queues */
        for (i=0; i<WL_RADIO(idx).wlNumBss; i++) {
            if ((ret = dalQos_setDefaultWlQueuesLock(WL_BSSID_IFNAME(idx,i), FALSE)) != CMSRET_SUCCESS) {
                printf("dalQos_setDefaultWlQueuesLock() returns error. ret=%d\n", ret);
                return;
            }
        }
    }

#endif /* DMP_QOS_1 */
#endif /* DMP_QOS_1 */
}


void wlmngr_post_setup(unsigned int idx ) {

    char cmd[64];
    if ( WL_RADIO_WLENBL(idx) == TRUE ) {
        wlmngr_WlConfStart(idx);
        wlmngr_wlIfcUp(idx);
    }

    wlmngr_doQoS(idx);
    snprintf(cmd, sizeof(cmd), "wl -i wl%d phy_watchdog 1", idx);
    BCMWL_WLCTL_CMD(cmd);

    snprintf(cmd, sizeof(cmd), "wl -i wl%d fcache 1", idx);
    BCMWL_WLCTL_CMD(cmd);

    // send ARP packet with bridge IP and hardware address to device
    // this piece of code is -required- to make br0's mac work properly
    // in all cases
    snprintf(cmd, sizeof(cmd), "/opt/usr/sbin/sendarp -s %s -d %s", WL_BSSID(idx,0).wlBrName,WL_BSSID(idx,0).wlBrName);
    bcmSystem(cmd);

    if( wlmngr_getCoreRev(idx) >= 40 ) {
        char tmp[100], prefix[] = "wlXXXXXXXXXX_";
        int txchain;
        snprintf(prefix, sizeof(prefix), "%s_", WL_BSSID_IFNAME(idx,0));
        wlmngr_getVarFromNvram(&txchain, strcat_r(prefix, "txchain", tmp), "int");

        if( _bits_count((unsigned int) txchain) > 1)
            WL_RADIO_WLTXBFCAPABLE(idx)=1;
    }
    wlmngr_setASPM(idx);
}



//**************************************************************************
// Function Name: clearSesLed
// Description  : clear SES LED.
// Parameters   : none.
// Returns      : none.
//**************************************************************************
void wlmngr_clearSesLed(void )
{
    int f = open( "/dev/brcmboard", O_RDWR );
    /* set led off */
    if( f > 0 ) {
        int  led = 0;
        BOARD_IOCTL_PARMS IoctlParms;
        memset( &IoctlParms, 0x00, sizeof(IoctlParms) );
        IoctlParms.result = -1;
        IoctlParms.string = (char *)&led;
        IoctlParms.strLen = sizeof(led);
        ioctl(f, BOARD_IOCTL_SET_SES_LED, &IoctlParms);
        close(f);
    }
}


#if defined(SUPPORT_WSC)
void wlmngr_startWsc(void)
{
    int i=0, j=0;
    char buf[64];
    char *buf1;

    int br;
    char ifnames[128];

    strncpy(buf, nvram_safe_get("wl_unit"), sizeof(buf));

    if (buf[0] == '\0') {
        nvram_set("wl_unit", "0");
        i = 0;
        j = 0;
    } else {
        if ((buf[1] != '\0') && (buf[2] != '\0')) {
            buf[3] = '\0';
            j = isdigit(buf[2])? atoi(&buf[2]):0;
        } else {
            j = 0;
        }

        buf[1] = '\0';
        i = isdigit(buf[0]) ? atoi(&buf[0]):0;
    }

    WLCSM_TRACE(WLCSM_TRACE_LOG," .............. \r\n" );
    nvram_set("wps_mode", WL_APWPS_WLWSCMODE(i,j)); //enabled/disabled
    nvram_set("wl_wps_config_state", WL_APWPS_WLWSCAPMODE(i,j)); // 1/0
    nvram_set("wl_wps_reg",         "enabled");
    if (strlen(nvram_safe_get("wps_version2")) == 0)
#ifdef WPS_V2
        nvram_set("wps_version2", "enabled");
#else
        nvram_set("wps_version2", "disabled");
#endif
    /* Since 5.22.76 release, WPS IR is changed to per Bridge. Previous IR enabled/disabled is
    Per Wlan Intf */

    for ( br=0; br<MAX_BR_NUM; br++ ) {
        if ( br == 0 )
            snprintf(buf, sizeof(buf), "lan_ifnames");
        else
            snprintf(buf, sizeof(buf), "lan%d_ifnames", br);
        buf1=nvram_get(buf);
        if(!buf1) continue;
        else  strncpy(ifnames, buf1, sizeof(ifnames));

        if (ifnames[0] =='\0')
            continue;
        if ( br == 0 )
            snprintf(buf, sizeof(buf), "lan_wps_reg");
        else
            snprintf(buf, sizeof(buf), "lan%d_wps_reg", br);
        nvram_set(buf, "enabled");
    }

    if (nvram_get("wps_config_method") == NULL) { /* initialization */
        set_wps_config_method_default(); /* for DTM 1.1 test */
        if (nvram_match("wps_version2", "enabled"))
            nvram_set("_wps_config_method", "sta-pin"); /* This var is only for WebUI use, default to sta-pin */
        else
            nvram_set("_wps_config_method", "pbc"); /* This var is only for WebUI use, default to PBC */
    }

    nvram_set("wps_uuid",           "0x000102030405060708090a0b0c0d0ebb");
    nvram_set("wps_device_name",    "BroadcomAP");
    nvram_set("wps_mfstring",       "Broadcom");
    nvram_set("wps_modelname",      "Broadcom");
    nvram_set("wps_modelnum",       "123456");
    nvram_set("boardnum",           "1234");
    nvram_set("wps_timeout_enable",	"0");

    nvram_set("wps_config_command", "0");
    nvram_set("wps_status", "0");
    nvram_set("wps_method", "1");
    nvram_set("wps_config_command", "0");
    nvram_set("wps_proc_mac", "");
    nvram_set("wps_sta_pin", "00000000");
    nvram_set("wps_currentband", "");
    nvram_set("wps_autho_sta_mac", "00:00:00:00:00:00");
    nvram_set("router_disable", "0");


    if (strlen(nvram_safe_get("wps_device_pin")) != 8)
        wl_wscPinGen();

    if (nvram_match("wps_restart", "1")) {
        nvram_set("wps_restart", "0");
    } else {
        nvram_set("wps_restart", "0");
        nvram_set("wps_proc_status", "0");
    }
    bcmSystem("wps_monitor&");
}

#endif //end of SUPPORT_WSC


#if defined(SUPPORT_WSC)
void set_wps_config_method_default(void)
{
    if (nvram_match("wps_version2", "enabled")) {
        nvram_set("wps_config_method", "0x228c");
        /* WPS_UI_MEHTOD_PBC */
        nvram_set("wps_method", "2");

    } else
        nvram_set("wps_config_method", "0x84");
}
#endif /* SUPPORT_WSC */

#ifdef  __CONFIG_RPCAPD__
int
start_rpcapd(void)
{
    int i=0,ret=0,rpcapd_enabled=0;
    char temp[64], *nv_value;
    char to_restart=0;
    for (i=0; i<WL_WIFI_RADIO_NUMBER; i++) {
        if (!WLCSM_STRCMP(WL_BSSID_WLMODE(i,0), "monitor")) {
            rpcapd_enabled=1;
            snprintf(temp,64,"dhd%d_rnr_rxoffl",i);
            nv_value=wlcsm_nvram_get(temp);
            if(!nv_value || !strncmp(nv_value,"1",1)) {
                /*need to use system shell to set nvram in order to
                 *available to kernel,need to adjust  */
                snprintf(temp,64,"nvram set dhd%d_rnr_rxoffl=0",i);
                bcmSystem(temp);
                /*mark it changed and restore back if change back
                 *to other mode */
                snprintf(temp,64,"nvram set dhd%d_rnr_rxoffl_changed=1",i);
                bcmSystem(temp);
                to_restart=1;
            }
        } else {
            snprintf(temp,64,"dhd%d_rnr_rxoffl_changed",i);
            /*restore it back to rxoffl enable mode*/
            if(wlcsm_nvram_get(temp)) {
                snprintf(temp,64,"nvram unset dhd%d_rnr_rxoffl",i);
                bcmSystem(temp);
                snprintf(temp,64,"nvram unset dhd%d_rnr_rxoffl_changed",i);
                bcmSystem(temp);
                to_restart=1;
            }
        }
        }
    if(to_restart) {
        bcmSystem("nvram kcommit");
        wlmngr_save_nvram();
        wlcsm_dm_save_config(0,0);
        wlmngr_release_thread_lock();
        WLMNGR_RESTART_UNLOCK();
		fprintf(stderr, "Board reboots to enable runner offload...\n");
        bcmSystem("reboot");
    } else if(rpcapd_enabled) {

        ret=eval("rpcapd", "-d", "-n");
    }
    return ret;
}

int
stop_rpcapd(void)
{
    int ret = eval("killall", "rpcapd");
    return ret;
}
#endif /* __CONFIG_RPCAPD__ */

//**************************************************************************
// Function Name: stopServices
// Description  : stop deamon services
//                which is required/common for each reconfiguration
// Parameters   : None
// Returns      : None
//**************************************************************************
void wlmngr_stopServices(void)
{

#ifdef SUPPORT_WSC
    bcmSystem("killall -q -9 lld2d 2>/dev/null");
    bcmSystem("rm -rf /var/run/lld2d* 2>/dev/null");
    bcmSystem("killall -q -9 wps_ap 2>/dev/null");
    bcmSystem("killall -q -9 wps_enr 2>/dev/null");
    wlmngr_stop_wps();
    wlmngr_clearSesLed();
#endif
    bcmSystem("killall -q -9 nas 2>/dev/null");
    bcmSystem("killall -q -9 eapd 2>/dev/null");
    bcmSystem("killall -q -9 acsd 2>/dev/null");
#ifdef BCMWAPI_WAI
    bcmSystem("killall -q -15 wapid");
#endif
#if defined(HSPOT_SUPPORT)
    bcmSystem("killall -q -15 hspotap");
#endif

    bcmSystem("killall -q -15 bsd");
    bcmSystem("killall -q -15 ssd");
#ifdef __CONFIG_TOAD__
    bcmSystem("killall -q -15 toad");
#endif
#ifdef __CONFIG_VISUALIZATION__
    bcmSystem("killall -q -9 vis-datacollector");
    bcmSystem("killall -q -9 vis-dcon");
#endif
#ifdef BCM_WBD
    bcmSystem("killall wbd_master");
    bcmSystem("killall wbd_slave");
#endif
#if defined(BCM_APPEVENTD)
    wlmngr_stop_appeventd();
#endif
#ifdef  __CONFIG_RPCAPD__
    stop_rpcapd();
#endif /* __CONFIG_RPCAPD__ */

    bcmSystem("killall -q -9 dhd_monitor 2>/dev/null");
    usleep(300000);

}


//**************************************************************************
// Function Name: startService
// Description  : start deamon services
//                which is required/common for each reconfiguration
// Parameters   : None
// Returns      : None
//**************************************************************************
void wlmngr_startServices(void)
{
    char tmp[64];
    char *lan_ifname;

    if (act_wl_cnt == 0)
        return;  /* all adapters are disabled */

    lan_ifname = nvram_safe_get("lan_ifname");
#ifdef SUPPORT_WSC
    snprintf(tmp, sizeof(tmp), "lld2d %s", lan_ifname);
    bcmSystem(tmp);
#endif
    bcmSystem("eapd");
    bcmSystem("nas");
    bcmSystem("dhd_monitor");


#ifdef SUPPORT_WSC
    wlmngr_startWsc();
#endif
#ifdef BCMWAPI_WAI
    bcmSystem("wapid");
#endif
    wlmngr_BSDCtrl();
    wlmngr_SSDCtrl();

#ifdef  __CONFIG_RPCAPD__
    start_rpcapd();
#endif
}

//**************************************************************************
// Function Name: getVar
// Description  : get value by variable name.
// Parameters   : var - variable name.
// Returns      : value - variable value.
//**************************************************************************
void wlmngr_getVar(const unsigned int idx , char *varName, char *varValue)

{
    WLCSM_TRACE(WLCSM_TRACE_LOG," TODO:get var \r\n" );
}
//**************************************************************************
// Function Name: getVar
// Description  : get value with full input params.
//**************************************************************************
#define MAXVAR 10

char *wlmngr_getVarEx(const unsigned int idx , int argc, char **argv, char *varValue)
{

    char *var = argv[1];
    char *ret_str=NULL;
    if (!WLCSM_STRCMP(var, "wlNPhyRates")) {
        ret_str= wlmngr_getNPhyRates(idx, argc, argv, varValue);
    } else if (!WLCSM_STRCMP(var, "wlChannelList"))
        ret_str= wlmngr_getChannelList(idx, argc, argv, varValue);
    return ret_str;
}



//**************************************************************************
// Function Name: wlmngr_getVarFromNvram
// Description  : retrieve var from nvram by name
// Parameters   : var, name, and type
// Returns      : none
//**************************************************************************
void wlmngr_getVarFromNvram(void *var, const char *name, const char *type)
{
    char temp_s[256] = {0};
    int len;

    strncpy(temp_s, nvram_safe_get(name), sizeof(temp_s));

    if(!WLCSM_STRCMP(type,"int")) {
        if(*temp_s)
            *(int*)var = atoi(temp_s);
        else
            *(int*)var = 0;
    } else if(!WLCSM_STRCMP(type,"string")) {
        if(*temp_s) {
            len = strlen(temp_s);

            /* Don't truncate tail-space if existed for SSID string */
            if (strstr(name, "ssid") == NULL) {
                if ((strstr(name, "wpa_psk") == NULL) && (len > 0) && (temp_s[len - 1] == ' '))
                    temp_s[len - 1] = '\0';
            }
            strcpy((char*)var,temp_s);
        } else {
            *(char*)var = 0;
        }
    } else {
        printf("wlmngr_getVarFromNvram:type not found\n");
    }
}


static bool wlmngr_detectAcsd(void)
{
    FILE *fp;
    char *files[]= {"/bin/acsd","/usr/sbin/acsd" };
    int i=0;
    for(; i<2; i++) {
        fp = fopen(files[i], "r");
        if ( fp != NULL ) {
            /* ACSD found */
            fclose(fp);
            return TRUE;
        }
    }
    /* ACSD not found */
    return FALSE;
}



static int _wlmngr_restart_interface(unsigned int radio_idx,SYNC_DIRECTION direction,bool in_thread)
{
    unsigned int i=0,start_idx,end_idx;
#ifdef DSLCPE_1905
    int credential_changed=0;
    int config_status=0;
    unsigned int bss_id=0;
#endif
    unsigned int idx;
    brcm_get_lock("wps",200);
    if(in_thread)
        wlmngr_get_thread_lock();
    idx=WLCSM_MNGR_CMD_GET_IDX(radio_idx);

    if(WLCSM_MNGR_CMD_GET_SOURCE(radio_idx)==WLCSM_MNGR_RESTART_TR69C && idx>0) {
        /*tr69 index starts at 1 */
        idx--;
    }
    act_wl_cnt = 0;
    for (i=0; i<WL_WIFI_RADIO_NUMBER; i++)
        act_wl_cnt += (WL_RADIO_WLENBL(i) == TRUE) ? 1 : 0;
    if(!g_wlmngr_restart_all) {
        start_idx=idx;
        end_idx=idx+1;
    } else {
        start_idx=0;
        end_idx=WL_WIFI_RADIO_NUMBER;
    }
    /* shutdown all the services(apps) first */
    wlmngr_stopServices();

    for(i=start_idx; i<end_idx; i++ ) {
        wlmngr_pre_setup(i); /* shutdown all the services(apps) first */
        _wlmmgr_vars_adjust(i,direction);
        _wlmngr_write_wl_nvram(i);
        _wlmngr_nvram_adjust(i,direction);
        wlmngr_setup(i);
    }
    if(in_thread)
        wlmngr_release_thread_lock();

    wlmngr_startServices();

    for(i=start_idx; i<end_idx; i++ ) {
        wlmngr_post_setup(i);

#ifdef DSLCPE_1905
        if(_wlmngr_check_config_status_change(i,&bss_id,&credential_changed,&config_status))
            _wlmngr_send_notification_1905(i,bss_id,credential_changed,config_status);
#endif
        /*The only reason here to save configurat again is to trigger CMS's RTL
         *to add wl interface to bridge */
        wlcsm_dm_save_config(i+1,WLCSM_MNGR_CMD_GET_SOURCE(radio_idx));
    }
#ifdef NO_CMS
    /* when NO_CMS build, bridge configur is not set and wireless interface may be not
     * in forward mode lead to acsd failure */
    _wlmngr_handle_bridge_setup();
#endif
    wlmngr_postStart_Service();

    wlmngr_save_nvram();
    brcm_release_lock("wps");
    g_wlmngr_restart_all=0;
    g_wlmngr_restart=0;
    WLCSM_TRACE(WLCSM_TRACE_DBG, " WLMNGR RESTART INTERFACE DONE\n");
    return 0;
}
#ifdef HSPOT_SUPPORT
static void _wlmngr_hspot_default(void) {
    int i=0, j;
    char temp_buf[32]= {0};
    wlcsm_hspot_nvram_default("wl_",0);
    for (i = 0; i < WL_WIFI_RADIO_NUMBER; i++) {
        snprintf(temp_buf, sizeof(temp_buf), "wl%d_",i);
        wlcsm_hspot_nvram_default(temp_buf,0);
        for (j = 1; j <WL_RADIO_WLNUMBSS(i); j++) {
            snprintf(temp_buf, sizeof(temp_buf), "wl%d.%d_",i,j);
            wlcsm_hspot_nvram_default(temp_buf,0);
        }
    }
}
#endif

/*************************************************************//**
 * @brief API for restart and/or save dm in seperated thread.
 *
 * 	In order to make wlmngr keep serving request for var set/get
 * 	when receving restart, it will be execute in a sepearted
 * 	thread, this will make sure whichever process depending
 * 	on the reply for variables to unlock DM be able to keep going
 * 	until release DM lock. HTTPD is such a case.
 *
 * @return void
 ***************************************************************/
static void _wlmngr_restart_handler(void *arg)
{
    unsigned int radio_idx= g_radio_idx,idx=0;
    int from=WLCSM_MNGR_CMD_GET_SOURCE(radio_idx);
    int dm_direction= WL_SYNC_FROM_DM;
    int ret=from;


    if(WLCSM_MNGR_CMD_GET_IDX(radio_idx)>=WL_WIFI_RADIO_NUMBER) {
        WLCSM_TRACE(WLCSM_TRACE_ERR, " Coming in IDX is too big\n");
        WLCSM_MNGR_CMD_SET_IDX(radio_idx,0);
    }

    wlcsm_nvram_set("_wlrestart_","1");
    if(from>WLCSM_MNGR_RESTART_FROM_MDM)  {
        /**Wait for following setting. A little tricky for multiple object set and restart. @JC*/
        sleep(3);
        if(arg)
            wlmngr_get_thread_lock(); /* if restart from a seperate thread, then lock */

        idx= WLCSM_MNGR_CMD_GET_IDX(radio_idx);
        if(WLCSM_MNGR_CMD_GET_SOURCE(radio_idx)!=WLCSM_MNGR_RESTART_TR69C)
            idx= WLCSM_MNGR_CMD_GET_IDX(radio_idx)+1;

        if(g_wlmngr_restart_all)
            WLCSM_DM_SELECT(SYS_DM_NAME,0,ret);
        else
            WLCSM_DM_SELECT(SYS_DM_NAME,idx,ret);

        if(arg) wlmngr_release_thread_lock();
        if(ret) {
            g_dm_loaded=ret;
            fprintf(stderr," DM init error, thus,we will return from here, do not need to continue as DM has problem   \r\n" );
            WLMNGR_RESTART_UNLOCK();
            return;
        } else if(!g_dm_loaded) {
            _wlmngr_hspot_default();
            g_dm_loaded=1;
        }
        dm_direction= WL_SYNC_FROM_DM;
    } else
        dm_direction=WL_SYNC_TO_DM;

    if(from==WLCSM_MNGR_RESTART_NVRAM)
        g_wlmngr_restart_all=1;

    _wlmngr_restart_interface(radio_idx,dm_direction,arg?1:0);

    WLMNGR_RESTART_UNLOCK();
    wlcsm_nvram_unset("_wlrestart_");
    fprintf(stderr, "--WL RESTART DONE--\n");
    if(arg) pthread_exit(0);
}

static int _wlmngr_start_wl_restart(t_WLCSM_MNGR_VARHDR *hdr)
{
    pthread_t save_dm_thread=0;
    unsigned int radio_idx=hdr->radio_idx;
    int from=WLCSM_MNGR_CMD_GET_SOURCE(radio_idx);
    WLCSM_SET_TRACE("wlmngr");
    g_wlmngr_restart=1;
    g_radio_idx=radio_idx;
    if(WLCSM_MNGR_CMD_GET_WAIT(radio_idx) && from <WLCSM_MNGR_RESTART_FROM_MDM)
        _wlmngr_restart_handler(NULL);
    else {
        if(pthread_create(&save_dm_thread,NULL,(void *)&_wlmngr_restart_handler,(void *)&g_radio_idx)) {
            fprintf(stderr, "%s:%d:	could not restart wireless from separated thread\n",__FUNCTION__,__LINE__);
            WLMNGR_RESTART_UNLOCK();
            return 1 ;
        }
        pthread_detach(save_dm_thread);
    }
    WLCSM_TRACE(WLCSM_TRACE_LOG," restart handled  \r\n" );
    return 0;
}


static void _wlmngr_updateStationList(const unsigned int idx,unsigned int sub_idx)
{
    char *buf;
    struct stat statbuf;
    int i,j;
    static char cmd[WL_SIZE_132_MAX];
    static char wl_authefile[80];
    static char wl_assocfile[80];
    static char wl_authofile[80];
    int num_of_stas=0;
    FILE *fp = NULL;
    int buflen= 0;

    if (!WL_BSSID_WLENBLSSID(idx,sub_idx)) {
        WLCSM_TRACE(WLCSM_TRACE_ERR,"WHAT? WHY THIS AP IS NOT ENABLED?  \r\n" );
        return;
    }

    /* first to clear up existing sta informations and reconstruct latter */
    if (WL_AP_STAS(idx,sub_idx)!= NULL) {
        /*release sta mac memory*/
        for (i = 0; i < WL_AP(idx,sub_idx).numStations; i++) {
            if(WL_AP_STAS(idx,sub_idx)[i].macAddress) {
                free(WL_AP_STAS(idx,sub_idx)[i].macAddress);
                WL_AP_STAS(idx,sub_idx)[i].macAddress=NULL;
            }
        }
        free(WL_AP_STAS(idx,sub_idx));
        WL_AP_STAS(idx,sub_idx)=NULL;
    } else
        WLCSM_TRACE(WLCSM_TRACE_LOG," cusBssStaList is null \r\n" );
    WL_AP(idx,sub_idx).numStations = 0;
    /* for now stalist should be cleared */

    /* using wl to get current association list  */
    snprintf(wl_authefile, sizeof(wl_authefile), "/var/wl%d_authe", idx);
    snprintf(wl_assocfile, sizeof(wl_assocfile), "/var/wl%d_assoc", idx);
    snprintf(wl_authofile, sizeof(wl_authofile), "/var/wl%d_autho", idx);
    snprintf(cmd, sizeof(cmd), "wl -i %s authe_sta_list > /var/wl%d_authe",WL_BSSID_NAME(idx,sub_idx), idx);
    BCMWL_WLCTL_CMD(cmd);
    snprintf(cmd, sizeof(cmd), "wl -i %s assoclist > /var/wl%d_assoc",WL_BSSID_NAME(idx,sub_idx), idx);
    BCMWL_WLCTL_CMD(cmd);
    snprintf(cmd, sizeof(cmd), "wl -i %s autho_sta_list > /var/wl%d_autho",WL_BSSID_NAME(idx,sub_idx), idx);
    BCMWL_WLCTL_CMD(cmd);

    if(!stat(wl_authefile, &statbuf)) {

        fp = fopen(wl_authefile, "r");
        if(fp) {
            buflen= statbuf.st_size;
            buf = (char*)malloc(buflen);
            if (buf ) {
                for (num_of_stas=0;; num_of_stas++) {
                    if (!fgets(buf, buflen, fp) || buf[0]=='\n' || buf[0]=='\r') {
                        break;
                    }
                }
                if (num_of_stas > 0) {
                    int asize;
                    char *pa;
                    WL_AP_STAS(idx,sub_idx)= malloc (sizeof(WLCSM_WLAN_AP_STA_STRUCT) *num_of_stas );
                    if ( WL_AP_STAS(idx,sub_idx) == NULL ) {
                        printf("%s::%s@%d WL_AP_STAS(idx,sub_idx) malloc Error\n", __FILE__, __FUNCTION__, __LINE__ );
                        free(buf);
                        fclose(fp);
                        return;
                    }

                    rewind(fp);

                    for (j=0; j<num_of_stas; j++) {
                        if (fgets(buf, buflen, fp)) {
                            if (wlmngr_scanForAddr(buf, buflen, &pa, &asize)) {
                                asize= (asize > WL_MID_SIZE_MAX -1)?  ( WL_MID_SIZE_MAX -1):asize;
                                WL_AP_STAS(idx,sub_idx)[j].macAddress=malloc(asize);
                                if( !WL_AP_STAS(idx,sub_idx)[j].macAddress) {
                                    free(buf);
                                    fclose(fp);
                                    return;
                                }
                                strncpy(WL_AP_STAS(idx,sub_idx)[j].macAddress, pa, asize);
                                WL_AP_STAS(idx,sub_idx)[j].macAddress[asize] = '\0';
                                pa = &(WL_AP_STAS(idx,sub_idx)[j].macAddress[asize-1]);
                                if (*pa == '\n' || *pa == '\r') *pa='\0';
                                WL_AP_STAS(idx,sub_idx)[j].associated = wlmngr_scanFileForMAC(wl_assocfile, WL_AP_STAS(idx,sub_idx)[j].macAddress);
                                WL_AP_STAS(idx,sub_idx)[j].authorized = wlmngr_scanFileForMAC(wl_authofile, WL_AP_STAS(idx,sub_idx)[j].macAddress);
                                WL_AP(idx,sub_idx).numStations++;
                            }
                        }
                    }
                }
                free(buf);
            }
            fclose(fp);
        }
    }
    unlink(wl_authefile);
    unlink(wl_assocfile);
    unlink(wl_authofile);

}
static unsigned int  _wlmngr_sta_list_fillup(const unsigned int idx ,char *varValue)
{
    WL_STALIST_SUMMARIES *sta_summaries=(WL_STALIST_SUMMARIES *)varValue;
    unsigned int j,i=0,ret_size,numStations=0;
    WLCSM_WLAN_AP_STA_STRUCT *curBssStaList=NULL;
    WL_STATION_LIST_ENTRY *sta=sta_summaries->stalist_summary;

    for (i=0; i<WL_RADIO_WLNUMBSS(idx) && i < WL_MAX_NUM_SSID; i++) {
        WLCSM_TRACE(WLCSM_TRACE_LOG," sta for i:%d number:%d \r\n",i,WL_AP(idx,i).numStations);
        numStations+=WL_AP(idx,i).numStations;
        curBssStaList =WL_AP_STAS(idx,i);
        if(curBssStaList!=NULL) {
            WLCSM_TRACE(WLCSM_TRACE_LOG," not empty \r\n" );
            for (j = 0; j < WL_AP(idx,i).numStations; j++) {
                WLCSM_TRACE(WLCSM_TRACE_LOG," macaddress:%s \r\n",curBssStaList->macAddress );
                memcpy(sta->macAddress,curBssStaList->macAddress,18);
                sta->associated=curBssStaList->associated;
                sta->authorized=curBssStaList->authorized;
                sta->radioIndex=idx;
                sta->ssidIndex =i;
                memcpy(sta->ssid,WL_BSSID_WLSSID(idx,i),strlen(WL_BSSID_WLSSID(idx,i))+1);
                memcpy(sta->ifcName,WL_BSSID_NAME(idx,i),strlen(WL_BSSID_NAME(idx,i))+1);
                sta++;
                curBssStaList++;
            }
        } else WLCSM_TRACE(WLCSM_TRACE_LOG," Bss is emtpy? \r\n" );

    }
    WLCSM_TRACE(WLCSM_TRACE_LOG," total:%d \r\n",numStations );

    sta_summaries->num_of_stas=numStations;
    ret_size=(sta_summaries->num_of_stas)*sizeof(WL_STATION_LIST_ENTRY)+sizeof(WL_STALIST_SUMMARIES);
    varValue[ret_size]='\0';
    return ret_size+1;
}


static int _wlmngr_handle_wlcsm_wl_sta_event (t_WLCSM_MNGR_VARHDR *hdr,char *varName,char *varValue)
{
    _wlmngr_updateStationList( WLCSM_MNGR_CMD_GET_IDX(hdr->radio_idx),hdr->sub_idx);
    if(g_wlcsm_dm_mngr_handlers[WLCSM_DM_MNGR_EVENT_STAS_CHANGED].func) {
        int numStations=0;
        char *staBuffer;
        int stalist_size, i=0, idx=WLCSM_MNGR_CMD_GET_IDX(hdr->radio_idx);
        for (i=0; i<WL_RADIO_WLNUMBSS(idx) && i < WL_MAX_NUM_SSID; i++)
            numStations+=WL_AP(idx,i).numStations;
        stalist_size=numStations*sizeof(WL_STATION_LIST_ENTRY)+sizeof(WL_STALIST_SUMMARIES)+1;
        staBuffer=malloc(stalist_size);
        if(staBuffer) {
            _wlmngr_sta_list_fillup( WLCSM_MNGR_CMD_GET_IDX(hdr->radio_idx), staBuffer);
            g_wlcsm_dm_mngr_handlers[WLCSM_DM_MNGR_EVENT_STAS_CHANGED].func(WLCSM_MNGR_CMD_GET_IDX(hdr->radio_idx),hdr->sub_idx, staBuffer);
            free(staBuffer);
        }
    }

#ifdef IDLE_PWRSAVE
    wlmngr_togglePowerSave();
#endif
    return 0;
}

int  wlmngr_handle_wlcsm_cmd_restart(t_WLCSM_MNGR_VARHDR *hdr,char *varName,char *varValue)
{
    /* lock setting g_radio_idx */
    int ret=0;
    WLMNGR_RESTART_LOCK();
    if(!g_wlmngr_restart) {
        switch(WLCSM_MNGR_CMD_GET_SOURCE(hdr->radio_idx)) {

        case WLCSM_MNGR_RESTART_HTTPD:
        case WLCSM_MNGR_RESTART_MDM:
        case WLCSM_MNGR_RESTART_TR69C:
        case WLCSM_MNGR_RESTART_NVRAM:
            ret=_wlmngr_start_wl_restart(hdr);
            if(ret)
                fprintf(stderr, "%s:%d:	restart failed\n",__FUNCTION__,__LINE__);
            return ret;
        default:
            fprintf(stderr, "Restart from unknow source:%d	\n",WLCSM_MNGR_CMD_GET_SOURCE(hdr->radio_idx));
            break;
        }
    }
    WLMNGR_RESTART_UNLOCK();
    return ret;
}




typedef  int (*MNGR_CMD_HANDLER)(t_WLCSM_MNGR_VARHDR  *hdr,char *name,char *value);

static MNGR_CMD_HANDLER g_mngr_cmd_handlers[]= {
    wlmngr_handle_wlcsm_cmd_restart,
    _wlmngr_handle_wlcsm_wl_sta_event,
    _wlmngr_handle_wlcsm_cmd_validate_dmvar,
    _wlmngr_handle_wlcsm_cmd_get_dmvar,
    _wlmngr_handle_wlcsm_cmd_set_dmvar,
    _wlmngr_handle_wlcsm_cmd_reg_dm_event,
    _wlmngr_handle_wlcsm_cmd_pwrreset_event,
    _wlmngr_handle_wlcsm_cmd_setdmdbglevel_event,
    _wlmngr_handle_wlcsm_cmd_nethotplug_event,
};


static  int  _wlmngr_get_var(t_WLCSM_MNGR_VARHDR *hdr,char *varName,char *varValue,int oid)
{
    char *idxStr;
    char *inputVarName=varName;
    char varNameBuf[WL_MID_SIZE_MAX];
    char ifcName[WL_MID_SIZE_MAX];
    char *next;
    int ret=0;
    int i=0;
    int num=0;
    char mac[32];

    unsigned int idx=WLCSM_MNGR_CMD_GET_IDX(hdr->radio_idx);
    char *tmp = NULL;
    unsigned int wlcsm_mngr_cmd=WLCSM_MNGR_CMD_GET_CMD(hdr->radio_idx);
    if(wlcsm_mngr_cmd) {
        if(wlcsm_mngr_cmd>WLCSM_MNGR_CMD_LAST) {
            fprintf(stderr," WLCSM CMD IS TOO BIG, NO SUCH COMMDN \r\n" );
            return 0;
        } else
            return g_mngr_cmd_handlers[wlcsm_mngr_cmd-1](hdr,varName,varValue);
    } else {

        if((varName==NULL || *varName=='\0')||
                ((hdr->sub_idx) >= WL_RADIO_WLMAXMBSS(WLCSM_MNGR_CMD_GET_IDX(hdr->radio_idx)))) {
            fprintf(stderr," Invalid name!! or idx is too big :%d\r\n",hdr->radio_idx );
            return 0;
        }

        strncpy(varNameBuf,varName,sizeof(varNameBuf));
        varName = varNameBuf;
        if((next=strstr(varName," "))) {
            /* the varname is a special string that require extend care
             * used in old GUI, not used in latest  */
            char *argv[20];
            char *prev=varName;
            int argc=1;
            do {
                varName[next-varName]='\0';
                argv[argc++]=prev;
                prev=next+1;
            } while((next=strstr(prev," ")));
            argv[argc++]=prev;
            if(wlmngr_getVarEx(idx, argc, argv, varValue))
                return strlen(varValue)+1;
            else return 0;

        } else {
            /* regulare varName handling here */
            int sub_idx=hdr->sub_idx;
            idxStr=strstr(varName, WL_PREFIX);
            if(idxStr) {
                sub_idx = atoi(idxStr+strlen(WL_PREFIX)+2); /* wlXx */
                *idxStr = '\0';
            }

            ret=wlmngr_handle_special_var(hdr,varName,varValue,WLMNGR_VAR_OPER_GET);
            if(ret) return ret;
            else {
                /* most of the special vars followings are for old GUI,will remove
                 * when old GUI are completely phased out */
                if ( WLCSM_STRCMP(varName, "wlCurIdxIfcName") == 0 ) {
                    snprintf(varValue, WL_MID_SIZE_MAX, "%s", WL_BSSID_IFNAME(idx,0));
                } else if (WLCSM_STRCMP(varName, "wlBands") == 0 ) {
                    snprintf(varValue, WL_MID_SIZE_MAX, "%d", wlmngr_getBands(idx));
                } else if (WLCSM_STRCMP(varName, "wlChanImState") == 0) {
                    snprintf(varValue, WL_MID_SIZE_MAX, "%d", wlmngr_getChannelImState(idx));
                } else if (WLCSM_STRCMP(varName, "wlCurrentBw") == 0 ) {
                    snprintf(varValue, WL_MID_SIZE_MAX, "%d", ((wlmngr_getCurrentChSpec(idx) & WL_CHANSPEC_BW_MASK) >> WL_CHANSPEC_BW_SHIFT));
                } else if (WLCSM_STRCMP(varName, "wlpwrsave") == 0) {
                    wlmngr_getPwrSaveStatus(idx, varValue);
                } else if (WLCSM_STRCMP(varName,"wl_stalist_summaries") == 0) {
                    return _wlmngr_sta_list_fillup(idx,varValue);
#if defined(__CONFIG_HSPOT__)
                }  else if ( WLCSM_STRCMP(varName, "wlEnableHspot") == 0 ) {
                    snprintf(varValue, WL_MID_SIZE_MAX, "%s", "2");
                }  else if ( WLCSM_STRCMP(varName, "wlPassPoint") == 0 ) {
                    snprintf(varValue, WL_MID_SIZE_MAX, "%s", "1");
#endif
                } else if (WLCSM_STRCMP(varName,"wdsscan_summaries") == 0) {
                    WL_WDSAPLIST_SUMMARIES *wdsap_summaries=(WL_WDSAPLIST_SUMMARIES *)varValue;
                    int ret_size=0;
                    wlmngr_scanWdsResult(idx);
                    if (WL_RADIO(idx).m_tblScanWdsMac != NULL) {
                        WL_FLT_MAC_ENTRY *entry = NULL;
                        int ap_count=0;
                        list_for_each(entry, (WL_RADIO(idx).m_tblScanWdsMac) ) {
                            WLCSM_TRACE(WLCSM_TRACE_LOG," mac:%s,ssid:%s \r\n",entry->macAddress,entry->ssid );
                            memcpy(wdsap_summaries->wdsaplist_summary[ap_count].mac,entry->macAddress,WL_MID_SIZE_MAX);
                            memcpy(wdsap_summaries->wdsaplist_summary[ap_count].ssid,entry->ssid,WL_SSID_SIZE_MAX);
                            WLCSM_TRACE(WLCSM_TRACE_LOG," mac:%s,ssid:%s \r\n",wdsap_summaries->wdsaplist_summary[ap_count].mac,
                                        wdsap_summaries->wdsaplist_summary[ap_count].ssid);
                            ap_count++;
                        }
                        wdsap_summaries->num_of_aps=ap_count;
                        WLCSM_TRACE(WLCSM_TRACE_LOG," there are :%d ap scanned \r\n",wdsap_summaries->num_of_aps );
                        ret_size=(wdsap_summaries->num_of_aps)*sizeof(WL_WDSAP_LIST_ENTRY);
                    } else {
                        wdsap_summaries->num_of_aps=0;
                        WLCSM_TRACE(WLCSM_TRACE_LOG," there is no scan ap find \r\n" );
                    }
                    ret_size+=sizeof(WL_WDSAPLIST_SUMMARIES);
                    varValue[ret_size]='\0';
                    return ret_size+1;
                } else if (WLCSM_STRCMP(varName,"wl_mbss_summaries") == 0) {
                    int num_of_mbss= WL_RADIO_WLNUMBSS(idx);
                    WL_BSSID_SUMMARIES *bss_summaries=(WL_BSSID_SUMMARIES *)varValue;
                    WL_BSSID_SUMMARY  *summary=bss_summaries->bssid_summary;
                    bss_summaries->num_of_bssid=num_of_mbss-1;
#if !defined(HSPOT_SUPPORT)
                    bss_summaries->wlSupportHspot=WLHSPOT_NOT_SUPPORTED;
#else
                    bss_summaries->wlSupportHspot=1;
#endif
                    for (i = 0; i < num_of_mbss-1; i++) {
                        summary[i].wlEnbl=WL_BSSID_WLENBLSSID(idx,i+1);
                        summary[i].wlHide=WL_AP_WLHIDE(idx,i+1);
                        summary[i].wlIsolation=WL_AP_WLAPISOLATION(idx,i+1);
                        summary[i].wlWme=WL_AP_WLWME(idx,i+1);
                        summary[i].wlDisableWme=WL_AP_WLDISABLEWME(idx,i+1);
                        summary[i].wlEnableWmf=WL_BSSID_WLENABLEWMF(idx,i+1);
                        summary[i].wmfSupported=WL_RADIO_WLHASWMF(idx);
#if !defined(HSPOT_SUPPORT)
                        summary[i].wlEnableHspot=WLHSPOT_NOT_SUPPORTED;
#else
                        summary[i].wlEnableHspot=WL_AP_WLENABLEHSPOT(idx,i+1);
#endif
                        summary[i].max_clients=WL_AP_WLMAXASSOC(idx,i+1);

                        if(WL_BSSID_WLBSSID(idx,i+1))
                            strncpy(summary[i].bssid,WL_BSSID_WLBSSID(idx,i+1), sizeof(summary[i].bssid));
                        else
                            summary[i].bssid[0]='\0';

                        if(WL_BSSID_WLSSID(idx,i+1))
                            strncpy(summary[i].wlSsid,WL_BSSID_WLSSID(idx,i+1), sizeof(summary[i].wlSsid));
                        else
                            summary[i].wlSsid[0]='\0';

                    }

                    /* we only have Virual BSS filled up,so minus one */
                    varValue[sizeof(WL_BSSID_SUMMARIES)+(num_of_mbss-1)*sizeof(WL_BSSID_SUMMARY)]='\0';
                    return sizeof(WL_BSSID_SUMMARIES)+(num_of_mbss-1)*sizeof(WL_BSSID_SUMMARY)+1;

                }

                else if (!strncmp(varName, "wlWds",5) && sscanf(varName,"wlWds%d",&num)) {

                    if( WL_RADIO_WLWDS(idx)) {
                        WLCSM_TRACE(WLCSM_TRACE_LOG," WLWDS str:%s \r\n",WL_RADIO_WLWDS(idx));
                        for_each(mac,WL_RADIO_WLWDS(idx),next) {
                            if(i++==num)
                                snprintf(varValue, WL_MID_SIZE_MAX, "%s", mac);
                        }
                    } else {
                        varValue[0]='\0';
                        WLCSM_TRACE(WLCSM_TRACE_LOG," wlWDS is NULL, the varName is:%s \r\n",varName );
                    }
                }

                else if (nvram_match("wps_version2", "enabled") && WLCSM_STRCMP(varName, "wlWscAuthoStaMac") == 0) {
                    tmp = nvram_safe_get("wps_autho_sta_mac");
                    snprintf(varValue, WL_MID_SIZE_MAX, "%s", tmp);

                } else if (WLCSM_STRCMP(varName, "wlSsidList") == 0 ) {
                    wlmngr_printSsidList(idx, varValue);
                } else if(WLCSM_STRCMP(varName, "wlCurrentChannel") == 0 ) {
                    wlmngr_getCurrentChannel(idx);
                    snprintf(varValue, WL_MID_SIZE_MAX, "%d", WL_RADIO_WLCURRENTCHANNEL(idx));
                } else if ( WLCSM_STRCMP(varName, "wlInterface") == 0 ) {
                    bcmGetWlName(0, 0, ifcName);
                    if ( bcmIsValidWlName(ifcName) == TRUE )
                        strcpy(varValue, "1");
                    else
                        strcpy(varValue, "0");
                } else if ( WLCSM_STRCMP(varName, "lan_hwaddr") == 0 ) {
                    wlmngr_getHwAddr(idx, IFC_BRIDGE_NAME, varValue);
                    bcmProcessMarkStrChars(varValue);
                } else if(WLCSM_STRCMP(varName, "wlInfo") == 0 ) {
                    wlmngr_getWlInfo(idx, varValue, "wlcap");
#ifndef SUPPORT_SES
                } else if(WLCSM_STRCMP(varName, "wlSesAvail") == 0 ) {
                    strcpy(varValue, "0");
#endif
                } else {
                    char *temp=NULL;
                    if (WLCSM_STRCMP(varName, "wlSsid_2") == 0 )
                        varName="wlSsid";
                    else if (WLCSM_STRCMP(varName, "wlEnbl_2") == 0 )
                        varName="wlEnblSsid";
                    if(!oid)
                        temp = wlcsm_dm_mngr_get_all_value(idx,sub_idx,varName,varValue);
                    else
                        temp=wlcsm_dm_mngr_get_value(idx,sub_idx,varName,varValue,oid);
                    if(!temp)
                        return 0;
                }

            }
        }
    }
    varName = inputVarName;
    ret=strlen(varValue)+1;
    return ret;
}

static int _wlmngr_handle_wlcsm_cmd_get_dmvar(t_WLCSM_MNGR_VARHDR  *hdr,char *varName,char *varValue)
{
    unsigned int mngr_oid=0;
    WLCSM_NAME_OFFSET *name_offset= wlcsm_dm_get_mngr_entry(hdr,varValue,&mngr_oid);

    if(!name_offset) {
        WLCSM_TRACE(WLCSM_TRACE_LOG," reurn not successull \r\n" );
        return 0;
    } else {
        WLCSM_TRACE(WLCSM_TRACE_LOG," mngr_oid:%u \r\n",mngr_oid);
        WLCSM_MNGR_CMD_SET_CMD(hdr->radio_idx,0);
        return _wlmngr_get_var(hdr,name_offset->name,varValue,mngr_oid);
    }
}


int wlmngr_get_var(t_WLCSM_MNGR_VARHDR *hdr,char *varName,char *varValue)
{
    return _wlmngr_get_var(hdr,varName,varValue,0);
}


int wlmngr_set_var(t_WLCSM_MNGR_VARHDR *hdr,char *varName,char *varValue)
{
    unsigned int wlcsm_mngr_cmd=WLCSM_MNGR_CMD_GET_CMD(hdr->radio_idx);
    int ret=0;
    if(wlcsm_mngr_cmd) {
        if(wlcsm_mngr_cmd>WLCSM_MNGR_CMD_LAST) {
            fprintf(stderr," WLCSM CMD IS TOO BIG, NO SUCH COMMAND \r\n" );
            ret = -1;
        } else
            ret = g_mngr_cmd_handlers[wlcsm_mngr_cmd-1](hdr,varName,varValue);
    } else {
        ret=wlmngr_handle_special_var(hdr,varName,varValue,WLMNGR_VAR_OPER_SET);
        if(ret) ret = 0;
        else
            ret = wlcsm_dm_mngr_set_all_value(WLCSM_MNGR_CMD_GET_IDX(hdr->radio_idx),
                                              hdr->sub_idx,varName,varValue);
    }
    return ret;
}
/* End of file */
