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
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>

#include <wlcsm_lib_dm.h>
#include <wlcsm_lib_api.h>
t_WLCSM_MSG_HDR	*wlcsm_mngr_recv_mesg(char *buf);

#include <bcmnvram.h>
//#include "wlapi.h"
#include "wlmngr.h"
#include "wlsyscall.h"

#include <sys/socket.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include <pthread.h>
#ifdef HSPOT_SUPPORT
#include <wlcsm_lib_hspot.h>
#endif

#define MNGR_VAR_RADIO_IDX(header)(((t_WLCSM_MNGR_VAR *)(header))->hdr.radio_idx)
#define MNGR_VAR_SSID_IDX(header) (((t_WLCSM_MNGR_VAR *)(header))->hdr.sub_idx)
#define MNGR_VAR_VARNAME(header) (((t_WLCSM_MNGR_VAR *)(header))->pair.value)
#define MNGR_VAR_VARVALUE(header) VALUEPAIR_VALUE(&(((t_WLCSM_MNGR_VAR *)(header))->pair))

int g_wlmngr_restart=0;
int g_wlmngr_restart_all=0;
int g_dm_loaded=0;
int g_wlmngr_ready_for_event=0;

void wlan_restart_all(int wait,SYNC_DIRECTION direction)
{
    t_WLCSM_MNGR_VARHDR	hdr= {0};
    g_wlmngr_restart_all=1;
    WLCSM_MNGR_CMD_SET_CMD( hdr.radio_idx, WLCSM_MNGR_CMD_RESTART);
    if(wait)
        WLCSM_MNGR_CMD_SET_WAIT(hdr.radio_idx);
    WLCSM_MNGR_CMD_SET_SOURCE(hdr.radio_idx,direction);
    wlmngr_handle_wlcsm_cmd_restart(&hdr,NULL,NULL);
}


void wlcsm_event_handler(t_WLCSM_EVENT_TYPE type,...)
{

    switch ( type ) {
    case WLCSM_EVT_NVRAM_CHANGED: {
        va_list arglist;
        va_start(arglist,type);
        char * name=va_arg(arglist,char *);
        char * value=va_arg(arglist,char *);
#ifdef WLCSM_DEBUG
        char * oldvalue=va_arg(arglist,char *);
#endif
        WLMNGR_RESTART_LOCK();
        wlmngr_get_thread_lock();
        if(value)
            value=wlcsm_trim_str(value);
        WLCSM_TRACE(WLCSM_TRACE_DBG,"   nvram chane received name:%s,value:%s,oldvalue:%s\r\n",name,value,oldvalue);
        wlcsm_nvram_update_runtime_mngr(name,value);
        wlmngr_special_nvram_handler(name,value);
        wlmngr_release_thread_lock();
        WLMNGR_RESTART_UNLOCK();
        va_end(arglist);
    }
    break;
    case WLCSM_EVT_NVRAM_COMMITTED:
        WLCSM_TRACE(WLCSM_TRACE_DBG," NVRAM COMMIT TO SAVE DM AND NVRAM  \r\n" );
        WLMNGR_RESTART_LOCK();
        wlmngr_get_thread_lock();
        wlmngr_save_nvram();
        wlcsm_dm_save_config(0,0);
        wlmngr_release_thread_lock();
        WLMNGR_RESTART_UNLOCK();
        break;
    default:
        break;
    }

}


int g_wlmngr_is_running=1;

void *wlmngr_dm_runtime_hook(unsigned int radio_idx,unsigned int sub_idx,unsigned int oid,WLCSM_DM_MNGR_CMD cmd,void *par)
{

    switch ( cmd ) {
    case WLCSM_DM_MNGR_CMD_GET_SOURCE:
        return wlmngr_get_runtime_obj_pointer(radio_idx,sub_idx,oid,cmd,par);
    default:
        break;
    }
    return NULL;
}

void wlmngr_terminater_handler(int signal)
{
    g_wlmngr_is_running=0;
    wlcsm_shutdown();
}

int main(int argc, char **argv)
{
    char *tempptr,*varName;
    int len=0,recv_len=0,cmd=0;
    t_WLCSM_MSG_HDR	*header;
    unsigned int radio_idx=0 ;
    static  char temp_buf[MAX_NLRCV_BUF_SIZE]= {0};
    static unsigned int varValue[MAX_NLRCV_BUF_SIZE]= {0}; /* 4 times  */
#ifdef WLCSM_DEBUG
    SINT32 c;
    unsigned int  sub_idx=0;
    while ((c = getopt(argc, argv, "m:")) != -1) {
        switch(c) {
        case 'm':
            break;
        default:
            printf("WLMNGR: unsupported option %d, ignored\n", c);
            break;
        }
    }
    WLCSM_SET_TRACE("wlmngr");
    bcmSystem("wlcsmdbg&");
#endif

    WL_WIFI_RADIO_NUMBER=wlcsm_wl_get_adapter_num();
    if(!WL_WIFI_RADIO_NUMBER || (wlcsm_init() !=WLCSM_SUCCESS)) {
        /* first to deinit DM since DM was initalized successfully from here */
        wlcsm_dm_cleanup();
        fprintf(stderr,"%s !!!!!\n",WL_WIFI_RADIO_NUMBER?"WLCSM_INIT Error":"NO WL ADAPTER");
        return -1;
    }

#ifdef CPU_MIPS
    /* on Mips, since we are use linux threads instead of NPTL, it has
     * a bug to shutdown mutliplethread in blocking*/
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM,wlmngr_terminater_handler);
#endif

    WLCSM_DM_REGISTER(SYS_DM_NAME);

#ifdef BUILD_EAPD
    /* Start Event handling Daemon */
    bcmSystem("/bin/wlevt2");
#endif

    wlan_restart_all(0,WLCSM_MNGR_RESTART_MDM);
    while(!g_dm_loaded)
        sleep(1);

    if(g_dm_loaded<0) {
        wlcsm_dm_cleanup();
        wlcsm_shutdown();
        fprintf(stderr,"!!!!! DM LOAD FAILURE!!!! QUIT \n");
        return -1;
    }

    wlcsm_register_process ("wlmngr");
    wlcsm_register_event_generic_hook(wlcsm_event_handler);
    wlcsm_nvram_set("hide_hnd_header","y");
    wlcsm_nvram_set("support_hnd_pages","y");
    g_wlcsm_dm_mngr_runtime_hook=wlmngr_dm_runtime_hook;

    while(g_wlmngr_is_running) {
        memset(temp_buf,0,MAX_NLRCV_BUF_SIZE);
        g_wlmngr_ready_for_event=1;
        header=wlcsm_mngr_recv_mesg((char *)temp_buf);
        if(header) {
            radio_idx=MNGR_VAR_RADIO_IDX(header+1);
            cmd=WLCSM_MNGR_CMD_GET_CMD(radio_idx);
            switch(header->type) {
            case WLCSM_MSG_GETWL_VAR: {
                tempptr=(char *)varValue;
                varName=MNGR_VAR_VARNAME(header+1);
                if( (cmd == WLCSM_MNGR_CMD_RESTART) && (g_wlmngr_restart||g_wlmngr_restart_all)) {
                    WLCSM_TRACE(WLCSM_TRACE_DBG," restart request coming in again and get ignored  \r\n" );
                    wlcsm_mngr_response(WLCSM_MSG_GETWL_VAR_RESP_DONE,varName,"",1,header->pid);

                } else {

                    wlmngr_get_thread_lock();
                    recv_len= wlmngr_get_var((t_WLCSM_MNGR_VARHDR *)(header+1),varName,(char *)varValue);
                    len=recv_len;
                    do {

                        if(len<=NL_PACKET_SIZE) {

                            wlcsm_mngr_response(WLCSM_MSG_GETWL_VAR_RESP_DONE,MNGR_VAR_VARNAME(header+1),tempptr,len,header->pid);

                        } else  {

                            wlcsm_mngr_response(WLCSM_MSG_GETWL_VAR_RESP,MNGR_VAR_VARNAME(header+1),tempptr,NL_PACKET_SIZE,header->pid);
                            tempptr+=NL_PACKET_SIZE;
                        }
                        len-=NL_PACKET_SIZE;

                    } while(len>0);

                    wlmngr_release_thread_lock();
                    if(recv_len>0) memset(varValue,0,recv_len);
                    WLCSM_TRACE(WLCSM_TRACE_LOG,"radio:%d, ssid:%d getwl name:%s,value:%s \r\n",radio_idx,sub_idx,varName,(char *)varValue );

                }
            }
            break;
            case WLCSM_MSG_SETWL_VAR: {
                char *name=MNGR_VAR_VARNAME(header+1);
                char *value=MNGR_VAR_VARVALUE(header+1);
                if(cmd != WLCSM_MNGR_CMD_NETHOTPLUG)
                    wlmngr_get_thread_lock();
                if(!wlmngr_set_var((t_WLCSM_MNGR_VARHDR *)(header+1),name,value))
                    wlcsm_mngr_response(WLCSM_MSG_SETWL_VAR_RESP,name,"",1,header->pid);
                else
                    wlcsm_mngr_response(WLCSM_MSG_SETWL_VAR_RESP,name," ",2,header->pid);
                if(cmd != WLCSM_MNGR_CMD_NETHOTPLUG)
                    wlmngr_release_thread_lock();
                break;
            }


            }
        } else {
            if(g_wlmngr_is_running)
                printf("%s:%d  WLMNGR ERROR and QUIT \r\n",__FUNCTION__,__LINE__ );
            g_wlmngr_is_running=0;
        }
    }

//    wlcsm_shutdown();
//    wlcsm_dm_cleanup();
    return 0;
}
