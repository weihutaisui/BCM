/*************************************************
* <:copyright-BRCM:2013:proprietary:standard
*
*    Copyright (c) 2013 Broadcom
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
/**
 *	@file	 wlcsm_dm_tr181.c
 *	@brief	 wlcsm data model tr181 APIs
 *
 * 	this file will handle wlmngr data structure and mapping to tr-181 data mapping
 * 	this file will be mostly like wlmdm in previous implementation.
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <asm/types.h>
#include <sys/types.h>
//#include <bcmnvram.h>
#include <errno.h>
#include "wlcsm_linux.h"
#include "cms.h"

#include "cms_util.h"
#include "mdm.h"
#include "mdm_private.h"
#include "odl.h"

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_cli.h"
#include "cms_log.h"
#include "cms.h"
#include "cms_obj.h"
#include "cms_mdm.h"
#include "cms_mem.h"
#include "cms_mgm.h"
#include "cms_util.h"
#include "cms_data_model_selector.h"
#include "cms_msg.h"
#include "mdm.h"     // for mdmLibCxt, but this file should not access this struct.  Delete later.
#include <board.h>
#include "bcmpwrmngtcfg.h"
#include "devctl_pwrmngt.h"
#include <fcntl.h>
#include "wlcsm_dm_tr181.h"
#define LOCK_WAIT     (60000)

static void *g_MsgHandle=NULL;

static int boardIoctl(int board_ioctl, BOARD_IOCTL_ACTION action, char *string, int strLen, int offset, char *buf)
{
    BOARD_IOCTL_PARMS IoctlParms;
    int boardFd = 0;

    boardFd = open("/dev/brcmboard", O_RDWR);
    if ( boardFd != -1 ) {
        IoctlParms.string = string;
        IoctlParms.strLen = strLen;
        IoctlParms.offset = offset;
        IoctlParms.action = action;
        IoctlParms.buf    = buf;
        ioctl(boardFd, board_ioctl, &IoctlParms);
        close(boardFd);
        boardFd = IoctlParms.result;
    } else
        printf("Unable to open device /dev/brcmboard.\n");

    return boardFd;
}

int _wlcsm_dm_tr181_obj_change_handler(unsigned int radio_idx,unsigned int sub_idx ,void *buf)
{
#ifdef WLCSM_DEBUG
    WLCSM_DM_OBJ_VALUE_SET *data= (WLCSM_DM_OBJ_VALUE_SET *)buf;
    WLCSM_TRACE(WLCSM_TRACE_DBG," wlmngr:oid:%u,offset:%u and change to value:%s \r\n",data->oid,data->offset,data->value );
#endif
    return 0;
}

int _wlcsm_dm_tr181_sta_change_handler(unsigned int radio_idx,unsigned int sub_idx ,void *buf)
{
    size_t ret_size = 0;
    WL_STALIST_SUMMARIES *sta_summaries = NULL;
    if (buf) {
        sta_summaries = (WL_STALIST_SUMMARIES *)buf;
        ret_size = (sta_summaries->num_of_stas) * sizeof(WL_STATION_LIST_ENTRY);
#ifdef WL_DM_TR181_DEBUG
        WL_STATION_LIST_ENTRY *pStationEntry;
        int i;
        pStationEntry = (WL_STATION_LIST_ENTRY *) sta_summaries->stalist_summary;
        for (i = 0 ; i < sta_summaries->num_of_stas && pStationEntry != NULL ; i++) {
            cmsLog_debug("[%s/%s][%s](%d/%d)\n", pStationEntry->ssid, pStationEntry->ifcName,
                         pStationEntry->macAddress,pStationEntry->associated,pStationEntry->authorized);
            pStationEntry++;
        }
#endif
        ret_size += sizeof(WL_STALIST_SUMMARIES);
    }

    CmsMsgHeader *msg = (CmsMsgHeader *) cmsMem_alloc((sizeof(CmsMsgHeader)+ ret_size), mdmLibCtx.allocFlags);
    if (!msg) {
        cmsLog_error("ERROR: Fail to allocate message buffer");
        return 1;
    }
    char *msgData = (char *)(msg+1);

    msg->src = EID_WLMNGR;
    msg->dst = EID_SSK;
    msg->type = CMS_MSG_WIFI_UPDATE_ASSOCIATEDDEVICE;
    msg->flags_request = 1;
    msg->dataLength = ret_size;
    if (buf)
        memcpy(msgData, buf, ret_size);

    cmsMsg_send(g_MsgHandle, msg);
    CMSMEM_FREE_BUF_AND_NULL_PTR(msg);
    return 0;
}

/*********************************************************************************//*****
 *  TR181 CMS related functions starts
 **************************************************************************************/
#define MDM_STRCPY(x, y)    if ( (y) != NULL ) \
                    CMSMEM_REPLACE_STRING_FLAGS( (x), (y), mdmLibCtx.allocFlags )

CmsRet wlWriteMdmToFlash (int requestlock )
{
    CmsRet ret;

    if(requestlock) {
        ret = cmsLck_acquireLockWithTimeout( LOCK_WAIT ); //3 //3s delay
        if ( ret != CMSRET_SUCCESS ) {
            printf("Could not get lock!\n");
            return ret;
        }
    }
    ret = cmsMgm_saveConfigToFlash();
    if(requestlock)
        cmsLck_releaseLock();

    return ret;
}

void cmsMdm_cleanup_wrapper()
{
#if defined(SUPPORT_DM_LEGACY98) || defined(SUPPORT_DM_HYBRID)

    cmsMdm_cleanup();

#elif defined(SUPPORT_DM_PURE181)

    /*  In Pure TR181 mode, we never initialized the MDM, so no cleanup */

#elif defined(SUPPORT_DM_DETECT)

    if (cmsUtil_isDataModelDevice2() == 0) {
        cmsMdm_cleanup();
    }

#endif
}

int getShmIdByMsg(void *g_MsgHandle)
{
    int shmId;
    UINT32 timeoutMs=5000;
    CmsRet r2;
    CmsMsgHeader msg = EMPTY_MSG_HEADER;
    msg.src = EID_WLMNGR;
    msg.dst = EID_SMD;
    msg.type = CMS_MSG_GET_SHMID;
    msg.flags_request = 1;

    r2 = cmsMsg_sendAndGetReplyWithTimeout(g_MsgHandle, &msg, timeoutMs);
    if (r2 == CMSRET_TIMED_OUT) { /*  assumes shmId is never 9809, which is value of CMSRET_TIMED_OUT */
        cmsLog_error("could not get shmId from smd (r2=%d)", r2);
        return UNINITIALIZED_SHM_ID;
    }

    shmId = (SINT32) r2;
    cmsLog_debug("got smdId=%d", shmId);

    return shmId;
}

CmsRet cmsMdm_init_wrapper(int *shmId  __attribute__((unused)),
                           void *g_MsgHandle  __attribute__((unused)))
{
    CmsRet ret=CMSRET_SUCCESS;

    if (*shmId == UNINITIALIZED_SHM_ID) {
        *shmId = getShmIdByMsg(g_MsgHandle);
        if (*shmId == UNINITIALIZED_SHM_ID) {
            return CMSRET_INTERNAL_ERROR;
        }
    }

    if ((ret = cmsMdm_initWithAcc(EID_WLMNGR, 0, g_MsgHandle, shmId)) != CMSRET_SUCCESS) {
        cmsLog_error("Could not initialize mdm, ret=%d", ret);
    }
    return ret;
}

#define WLCSM_STRCPY(to,from)  do {\
            char *dst_value=NULL;\
            if(from) {\
                dst_value=malloc(strlen(from)+1); \
                if(dst_value) { \
                strncpy(dst_value,from,strlen(from)+1); \
                            if(to) free(to); \
                to=dst_value; \
                } else \
                return 1;\
           }} while(0)

/*********************************************************************************//*****
 *  TR181 CMS related functions end
 **************************************************************************************/

static int _wlcsm_exchange_mapping_value(char b_loading,char *src, char *dst, WLCSM_DM_WLMNGR_MAPPING* mapping,int entry_size)
{
    int i=0,ret=0;
    WLCSM_NAME_OFFSET  *src_set,*dst_set;
    char **srcpptr,**dstpptr;
    for ( i=0; i<entry_size; i++ ) {

        if(b_loading==B_DM_LOADING) {
            src_set = &(mapping[i].dm_set);
            dst_set = &(mapping[i].wlmngr_set);
        } else {
            src_set=&(mapping[i].wlmngr_set);
            dst_set=&(mapping[i].dm_set);
            WLCSM_TRACE(WLCSM_TRACE_LOG," name:%s:%s,type:%d \r\n",src_set->name,dst_set->name,mapping[i].type );
        }

        /*when it reach's end,break out because the real size maybe smaller than the entry_size */
        if(!src_set->name) break;
        srcpptr=(char **)(src+src_set->offset);
        dstpptr=(char **)(dst+dst_set->offset);

        switch ( mapping[i].type ) {

        case WLCSM_DT_UINT:
            NUM_SYNC(src,src_set->offset,dst,dst_set->offset,UINT32);
            break;
        case WLCSM_DT_SINT32:
            NUM_SYNC(src,src_set->offset,dst,dst_set->offset,SINT32);
            break;
        case WLCSM_DT_UINT64:
            NUM_SYNC(src,src_set->offset,dst,dst_set->offset,UINT64);
            break;
        case WLCSM_DT_SINT64:
            NUM_SYNC(src,src_set->offset,dst,dst_set->offset,SINT64);
            break;
        case WLCSM_DT_BOOL:
            NUM_SYNC(src,src_set->offset,dst,dst_set->offset,UBOOL8);
            break;
        case WLCSM_DT_BOOLREV:
            BOOL_REV(src,src_set->offset,dst,dst_set->offset,UBOOL8);
            break;
        case WLCSM_DT_STRING:
        case WLCSM_DT_BASE64:
        case WLCSM_DT_HEXBINARY:
        case WLCSM_DT_DATETIME:
            if(b_loading==B_DM_LOADING) {
                if (*srcpptr && WLCSM_STRCMP(*srcpptr,"(null)")) {
                    WLCSM_STRCPY(*dstpptr,*srcpptr);
                } else {
                    if(*dstpptr) free(*dstpptr);
                    *dstpptr=NULL;
                }
            } else  {
                if (*srcpptr) {
                    MDM_STRCPY(*dstpptr, *srcpptr);
                } else if(*dstpptr)
                    CMSMEM_FREE_BUF_AND_NULL_PTR(*dstpptr);
            }
            break;
        case WLCSM_DT_STR2INT:
            if(b_loading==B_DM_LOADING) {
                SINT32 value=0;
                SINT32  *dstptr=(SINT32 *)(dst+dst_set->offset);
                if (*srcpptr) {
                    value=wlcsm_dm_get_mapper_int(mapping[i].mapper,*srcpptr,0,&ret);
                    WLCSM_TRACE(WLCSM_TRACE_LOG," get mapper value is:%d \r\n",value );
                }
                *dstptr=value;
            } else {
                char *tmp_str_ptr;
                tmp_str_ptr= wlcsm_dm_get_mapper_str(mapping[i].mapper, NUMVAR_VALUE(src, src_set->offset,SINT32),0,&ret);
                if (tmp_str_ptr && !ret) {
                    MDM_STRCPY(*dstpptr, tmp_str_ptr);
                }
            }
            break;
        case WLCSM_DT_INT2STR:
            if(b_loading==B_DM_LOADING) {
                char *tmp_str_ptr;
                tmp_str_ptr= wlcsm_dm_get_mapper_str(mapping[i].mapper, NUMVAR_VALUE(src, src_set->offset,SINT32),0,&ret);
                if (tmp_str_ptr && !ret) {
                    WLCSM_STRCPY(*dstpptr,tmp_str_ptr);
                }
            } else {
                SINT32 value=0;
                SINT32  *dstptr=(SINT32 *)(dst+dst_set->offset);
                if (*srcpptr) {
                    value=wlcsm_dm_get_mapper_int(mapping[i].mapper,*srcpptr,0,&ret);
                    WLCSM_TRACE(WLCSM_TRACE_LOG," get mapper value is:%d \r\n",value );
                }
                *dstptr=value;
            }
        default:
            WLCSM_TRACE(WLCSM_TRACE_ERR," DO NOT UNDERSTAND THE TYPE:%d and name is:%s:%s    ",src_set->name,dst_set->name ,mapping[i].type );
            break;
        }
    }
    return 0;
}
#define  __wlcsm_exchange_mapping_value(b_loading,src,dst,mapping,entry_size)  _wlcsm_exchange_mapping_value((b_loading),(char *)(src), (char *)(dst), (WLCSM_DM_WLMNGR_MAPPING*)(mapping),(entry_size))

static int _wlcsm_dm_save_wifi()
{
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret;
    _Dev2WifiObject *wifiObj=NULL;
    WLCSM_WLAN_WIFI_STRUCT *wifi=&g_wifi_obj;

    ret = cmsLck_acquireLockWithTimeout( LOCK_WAIT ); //3
    if ( ret != CMSRET_SUCCESS ) {
        printf("Could not get lock!\n");
        return ret;
    }

    if ((ret = cmsObj_get(MDMOID_DEV2_WIFI, &iidStack, 0, (void *) &wifiObj)) != CMSRET_SUCCESS) {

        if (wifiObj != NULL)
            cmsObj_free((void **) &wifiObj);
        cmsLck_releaseLock();
        return ret;
    }


    ret=__wlcsm_exchange_mapping_value(B_DM_POPULATING,wifi,wifiObj,g_dm_tr181_mapping_DEV2_WIFI,MAPPING_ENTRY_SIZE(g_dm_tr181_mapping_DEV2_WIFI));

    if ((ret = cmsObj_set(wifiObj,&iidStack)) != CMSRET_SUCCESS)
        WLCSM_TRACE(WLCSM_TRACE_ERR," ERROR to write to wifiobj \r\n" );

    cmsLck_releaseLock();

    if (wifiObj != NULL) {
        cmsObj_free((void **) &wifiObj);
    }

    WLCSM_TRACE(WLCSM_TRACE_LOG," ====================POPUTLATING WIFI done=================== \r\n" );
    return 0;
}


static int _wlcsm_dm_tr181_save_radio(int idx,int withlock)
{
    _Dev2WifiRadioObject *wlRadioCfgObj=NULL;
    int ret=0;
    WLCSM_WLAN_ADAPTER_STRUCT *adapter = (WLCSM_WLAN_ADAPTER_STRUCT *)(&gp_adapter_objs[idx-1]);
    WLCSM_WLAN_RADIO_STRUCT *radio = &(adapter->radio);
    WLCSM_TRACE(WLCSM_TRACE_LOG," ============== starting to save radio================= \r\n" );
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    if(withlock) {
        ret = cmsLck_acquireLockWithTimeout( LOCK_WAIT); //3 //3s delay
        if ( ret != CMSRET_SUCCESS ) {
            return ret;
        }
    }
    iidStack.instance[0] = idx;
    iidStack.currentDepth=1;
    if ((ret = cmsObj_get(MDMOID_DEV2_WIFI_RADIO, &iidStack, 0, (void **)&wlRadioCfgObj)) != CMSRET_SUCCESS)
        goto release;
    ret=__wlcsm_exchange_mapping_value(B_DM_POPULATING,radio,wlRadioCfgObj,g_dm_tr181_mapping_DEV2_WIFI_RADIO,MAPPING_ENTRY_SIZE(g_dm_tr181_mapping_DEV2_WIFI_RADIO));
    if ((ret = cmsObj_set(wlRadioCfgObj,&iidStack)) != CMSRET_SUCCESS)
        WLCSM_TRACE(WLCSM_TRACE_ERR," ERROR to write to radio \r\n" );
release:
    if (wlRadioCfgObj != NULL)
        cmsObj_free((void **) &wlRadioCfgObj);
    if(withlock)
        cmsLck_releaseLock();
    WLCSM_TRACE(WLCSM_TRACE_LOG," ====================saveinging radio done=================== \r\n" );

    return ret;
}

static int _wlcsm_dm_tr181_save_bssid(int idx,int sub_idx,int requestlock)
{
    int ret=0;
    int num_of_bssids= wlcsm_dm_get_bssid_num(idx);
    WLCSM_WLAN_ADAPTER_STRUCT *adapter = (WLCSM_WLAN_ADAPTER_STRUCT *)(&gp_adapter_objs[idx-1]);
    WLCSM_WLAN_BSSID_STRUCT *bssids;

    WLCSM_TRACE(WLCSM_TRACE_LOG," ====== loading adapter:%d === bssid:%d ==== \r\n",idx,sub_idx);

    bssids = adapter->bssids;

    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    _Dev2WifiSsidObject *ssidObj=NULL;

    iidStack.instance[0] = sub_idx+(idx-1)*num_of_bssids;
    iidStack.currentDepth=1;

    if(requestlock) {
        ret = cmsLck_acquireLockWithTimeout( LOCK_WAIT); //3 //3s delay
        if ( ret != CMSRET_SUCCESS ) {
            return ret;
        }
    }
    if ((ret = cmsObj_get(MDMOID_DEV2_WIFI_SSID, &iidStack, 0, (void **) &ssidObj)) != CMSRET_SUCCESS) {
        if (ssidObj != NULL) {
            cmsObj_free((void **) &ssidObj);
        }
        if(requestlock)
            cmsLck_releaseLock();
        return ret;
    }
    WLCSM_TRACE(WLCSM_TRACE_LOG," DM's ssid is:%s, ifname is:%s \r\n",ssidObj->SSID,ssidObj->X_BROADCOM_COM_WlIfname);
    ret= __wlcsm_exchange_mapping_value(B_DM_POPULATING,(char *)&bssids[sub_idx-1],(char *)ssidObj,g_dm_tr181_mapping_DEV2_WIFI_SSID,MAPPING_ENTRY_SIZE(g_dm_tr181_mapping_DEV2_WIFI_SSID));

    if ((ret = cmsObj_set(ssidObj,&iidStack)) != CMSRET_SUCCESS)
        WLCSM_TRACE(WLCSM_TRACE_ERR," ERROR to write to bssid \r\n" );

    if (ssidObj != NULL) {
        cmsObj_free((void **) &ssidObj);
    }
    if(requestlock)
        cmsLck_releaseLock();

    return ret;
}
static int _wlcsm_dm_tr181_save_access_point(int idx,int sub_idx,int requestlock)
{
    int ret=0;
    int num_of_bssids= wlcsm_dm_get_bssid_num(idx);
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

    WLCSM_TRACE(WLCSM_TRACE_LOG," ACCESSPOINT: num of bssids:%d \r\n",num_of_bssids );
    _Dev2WifiAccessPointObject *wlAccessPointCfgObj=NULL;
    _Dev2WifiAccessPointSecurityObject *accessPointSecurityObj=NULL;
    _Dev2WifiAccessPointWpsObject *wpsObj =NULL;
    WLCSM_WLAN_AP_STRUCT *accesspoint;
    WLCSM_WLAN_AP_SECURITY_STRUCT *security;
    WLCSM_WLAN_AP_WPS_STRUCT   *wps;


    WLCSM_WLAN_ADAPTER_STRUCT *adapter = (WLCSM_WLAN_ADAPTER_STRUCT *)(&gp_adapter_objs[idx-1]);
    WLCSM_WLAN_ACCESSPOINT_STRUCT *ssids;


    ssids=adapter->ssids;
    accesspoint = &(ssids[sub_idx-1].accesspoint);
    security = &(ssids[sub_idx-1].security);
    wps = &(ssids[sub_idx-1].wps);


    iidStack.instance[0] = sub_idx+(idx-1)*num_of_bssids;
    iidStack.currentDepth=1;

    if(requestlock) {
        ret = cmsLck_acquireLockWithTimeout( LOCK_WAIT); //3 //3s delay
        if ( ret != CMSRET_SUCCESS ) {
            return ret;
        }
    }
    if ((ret = cmsObj_get(MDMOID_DEV2_WIFI_ACCESS_POINT, &iidStack, 0, (void **)&wlAccessPointCfgObj)) != CMSRET_SUCCESS) {

        if (wlAccessPointCfgObj != NULL) {
            cmsObj_free((void **) &wlAccessPointCfgObj);
        }
        if(requestlock)
            cmsLck_releaseLock();
        return ret;
    }

    ret= __wlcsm_exchange_mapping_value(B_DM_POPULATING,accesspoint,wlAccessPointCfgObj,g_dm_tr181_mapping_DEV2_WIFI_ACCESS_POINT,MAPPING_ENTRY_SIZE(g_dm_tr181_mapping_DEV2_WIFI_ACCESS_POINT));

    if ((ret = cmsObj_set(wlAccessPointCfgObj,&iidStack)) != CMSRET_SUCCESS)
        WLCSM_TRACE(WLCSM_TRACE_ERR," ERROR to write to bssid \r\n" );

    if (wlAccessPointCfgObj != NULL) {
        cmsObj_free((void **) &wlAccessPointCfgObj);
    }

    if ((ret = cmsObj_get(MDMOID_DEV2_WIFI_ACCESS_POINT_SECURITY, &iidStack, 0, (void **) &accessPointSecurityObj)) != CMSRET_SUCCESS) {


        if (accessPointSecurityObj!=NULL) {
            cmsObj_free((void **) &accessPointSecurityObj);
        }
        if(requestlock)
            cmsLck_releaseLock();
        return ret;
    }

    ret= __wlcsm_exchange_mapping_value(B_DM_POPULATING,security,accessPointSecurityObj,g_dm_tr181_mapping_DEV2_WIFI_ACCESS_POINT_SECURITY,MAPPING_ENTRY_SIZE(g_dm_tr181_mapping_DEV2_WIFI_ACCESS_POINT_SECURITY));


    if ((ret = cmsObj_set(accessPointSecurityObj,&iidStack)) != CMSRET_SUCCESS)
        WLCSM_TRACE(WLCSM_TRACE_ERR," ERROR to write to security \r\n" );

    if (accessPointSecurityObj!=NULL) {
        cmsObj_free((void **) &accessPointSecurityObj);
    }


    if ((ret = cmsObj_get(MDMOID_DEV2_WIFI_ACCESS_POINT_WPS, &iidStack, 0, (void **) &wpsObj)) != CMSRET_SUCCESS) {
        if (wpsObj!=NULL) {
            cmsObj_free((void **) &wpsObj);
        }
        if(requestlock)
            cmsLck_releaseLock();
        return ret;
    }

    ret= __wlcsm_exchange_mapping_value(B_DM_POPULATING,wps,wpsObj,g_dm_tr181_mapping_DEV2_WIFI_ACCESS_POINT_WPS,MAPPING_ENTRY_SIZE(g_dm_tr181_mapping_DEV2_WIFI_ACCESS_POINT_WPS));

    if ((ret = cmsObj_set(wpsObj,&iidStack)) != CMSRET_SUCCESS)
        WLCSM_TRACE(WLCSM_TRACE_ERR," ERROR to write to security \r\n" );

    if (wpsObj!=NULL) {
        cmsObj_free((void **) &wpsObj);
    }
    if(requestlock)
        cmsLck_releaseLock();
    return ret;
}


static int __wlcsm_dm_tr181_save_config(int idx,int requestlock)
{

    int ret=0,i=0;
    int num_of_bssids= wlcsm_dm_get_bssid_num(idx);

    ret=_wlcsm_dm_tr181_save_radio(idx,requestlock);
    WLCSM_TRACE(WLCSM_TRACE_LOG," ==>>>>>bssid_num:%d \r\n",gp_adapter_objs[idx-1].radio.wlNumBss );
    if(!ret) {

        for ( i=1; i<=num_of_bssids; i++ ) {
            ret=_wlcsm_dm_tr181_save_bssid(idx,i,requestlock);
            if(ret || (ret=_wlcsm_dm_tr181_save_access_point(idx,i,requestlock)))
                return ret;
        }
    }
    return ret;

}

static int _wlcsm_dm_tr181_save_config(int idx,int from)
{
    int start_idx,end_idx,ret=0;

    if(idx) {
        start_idx=idx;
        end_idx=idx;
    } else {
        start_idx=1;
        end_idx= WL_WIFI_RADIO_NUMBER;
    }

    ret = cmsLck_acquireLockWithTimeout( LOCK_WAIT ); //3 //3s delay
    if ( ret != CMSRET_SUCCESS ) {
        printf("Could not get lock!\n");
        return ret;
    }

    for(idx=start_idx; idx<=end_idx; idx++) {

        ret=__wlcsm_dm_tr181_save_config(idx,0); /* save without request lock */
        if(ret) break; //if not success, break;
    }

    ret=wlWriteMdmToFlash(0);

    cmsLck_releaseLock();
    return ret;

}



static int  _wlcsm_dm_load_wifi()
{
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret;
    _Dev2WifiObject *wifiObj=NULL;
    WLCSM_WLAN_WIFI_STRUCT *wifi=&g_wifi_obj;

    ret = cmsLck_acquireLockWithTimeout( LOCK_WAIT ); //3
    if ( ret != CMSRET_SUCCESS ) {
        printf("Could not get lock!\n");
        return ret;
    }

    if ((ret = cmsObj_get(MDMOID_DEV2_WIFI, &iidStack, 0, (void *) &wifiObj)) != CMSRET_SUCCESS) {

        cmsLck_releaseLock();
        return ret;
    }

    cmsLck_releaseLock();

    ret=__wlcsm_exchange_mapping_value(B_DM_LOADING,wifiObj,wifi,g_dm_tr181_mapping_DEV2_WIFI,MAPPING_ENTRY_SIZE(g_dm_tr181_mapping_DEV2_WIFI));

    if (wifiObj != NULL) {
        cmsObj_free((void **) &wifiObj);
    }
    WLCSM_TRACE(WLCSM_TRACE_LOG," ====================Loading WIFI done=================== \r\n" );
    return 0;
}

static int _wlcsm_dm_tr181_load_radio(int idx)
{
    _Dev2WifiRadioObject *wlRadioCfgObj=NULL;
    int ret=0;
    WLCSM_WLAN_ADAPTER_STRUCT *adapter = (WLCSM_WLAN_ADAPTER_STRUCT *)(&gp_adapter_objs[idx-1]);
    WLCSM_WLAN_RADIO_STRUCT *radio = &(adapter->radio);
    WLCSM_TRACE(WLCSM_TRACE_LOG," ============== starting to load radio================= \r\n" );


    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    ret = cmsLck_acquireLockWithTimeout( LOCK_WAIT); //3 //3s delay
    if ( ret != CMSRET_SUCCESS ) {
        return ret;
    }

    iidStack.instance[0] = idx;
    iidStack.currentDepth=1;
    if ((ret = cmsObj_get(MDMOID_DEV2_WIFI_RADIO, &iidStack, 0, (void **)&wlRadioCfgObj)) != CMSRET_SUCCESS) {

        cmsLck_releaseLock();
        return ret;
    }

    ret=__wlcsm_exchange_mapping_value(B_DM_LOADING,wlRadioCfgObj,radio,g_dm_tr181_mapping_DEV2_WIFI_RADIO,MAPPING_ENTRY_SIZE(g_dm_tr181_mapping_DEV2_WIFI_RADIO));
    if (wlRadioCfgObj != NULL) {
        cmsObj_free((void **) &wlRadioCfgObj);
    }

    cmsLck_releaseLock();

    WLCSM_TRACE(WLCSM_TRACE_LOG," after mapping: radiowlnumbss:%d,eanbled:%d  \r\n",radio->wlNumBss,radio->wlEnbl );
    WLCSM_TRACE(WLCSM_TRACE_LOG," after mapping: wlBand:%d,phytype:%s  \r\n",radio->wlBand,radio->wlPhyType );
    WLCSM_TRACE(WLCSM_TRACE_LOG," ====================Loading radio done=================== \r\n" );

    return ret;
}

static int _wlcsm_dm_tr181_load_bssid(int idx,int sub_idx)
{
    int len=0,ret=0;
    int num_of_bssids= wlcsm_dm_get_bssid_num(idx);
    WLCSM_WLAN_ADAPTER_STRUCT *adapter = (WLCSM_WLAN_ADAPTER_STRUCT *)(&gp_adapter_objs[idx-1]);
    WLCSM_WLAN_BSSID_STRUCT *bssids;
    len= sizeof(WLCSM_WLAN_BSSID_STRUCT) * num_of_bssids;

    WLCSM_TRACE(WLCSM_TRACE_LOG," bssids: num of bssids:%d \r\n",num_of_bssids );
    WLCSM_TRACE(WLCSM_TRACE_LOG," ====== loading adapter:%d === bssid:%d ==== \r\n",idx,sub_idx);

    if(!adapter->bssids) {
        WLCSM_TRACE(WLCSM_TRACE_LOG," try to allocate for idx:%d,sub_idx:%d \r\n",idx,sub_idx );
        if(!(adapter->bssids=malloc(len))) {
            WLCSM_TRACE(WLCSM_TRACE_ERR," could not allocate memory for bssids \r\n");
        } else
            memset(adapter->bssids,0,len);
    } else
        WLCSM_TRACE(WLCSM_TRACE_LOG," bssids is not null, allocated already???:sub_idx:%d \r\n",sub_idx );

    bssids = adapter->bssids;


    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    _Dev2WifiSsidObject *ssidObj=NULL;

    iidStack.instance[0] = sub_idx+(idx-1)*num_of_bssids;
    iidStack.currentDepth=1;

    ret = cmsLck_acquireLockWithTimeout( LOCK_WAIT); //3 //3s delay
    if ( ret != CMSRET_SUCCESS ) {
        return ret;
    }
    if ((ret = cmsObj_get(MDMOID_DEV2_WIFI_SSID, &iidStack, 0, (void **) &ssidObj)) != CMSRET_SUCCESS) {
        if (ssidObj != NULL) {
            cmsObj_free((void **) &ssidObj);
        }
        cmsLck_releaseLock();
        return ret;
    }
    cmsLck_releaseLock();
    WLCSM_TRACE(WLCSM_TRACE_LOG," DM's ssid is:%s, ifname is:%s \r\n",ssidObj->SSID,ssidObj->X_BROADCOM_COM_WlIfname);
    ret= __wlcsm_exchange_mapping_value(B_DM_LOADING,(char *)ssidObj,(char *)&bssids[sub_idx-1],g_dm_tr181_mapping_DEV2_WIFI_SSID,MAPPING_ENTRY_SIZE(g_dm_tr181_mapping_DEV2_WIFI_SSID));
    WLCSM_TRACE(WLCSM_TRACE_LOG," sub_idx:%d,WLMNG's ssid is:%s, ifname is :%s \r\n",sub_idx,bssids[sub_idx-1].wlSsid,bssids[sub_idx-1].wlIfname );
    WLCSM_TRACE(WLCSM_TRACE_LOG," ifname:%s \r\n",WL_BSSID_IFNAME(idx-1,sub_idx-1));
    WLCSM_TRACE(WLCSM_TRACE_LOG," =================================================++++++++++++++++++++++++++++++++++++++++ \r\n" );

    if (ssidObj != NULL) {
        cmsObj_free((void **) &ssidObj);
    }

    return ret;
}

static int _wlcsm_dm_tr181_load_access_point(int idx,int sub_idx)
{
    int ret=0;
    int num_of_bssids= wlcsm_dm_get_bssid_num(idx);
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

    WLCSM_TRACE(WLCSM_TRACE_LOG," ACCESSPOINT: num of bssids:%d \r\n",num_of_bssids );
    _Dev2WifiAccessPointObject *wlAccessPointCfgObj=NULL;
    _Dev2WifiAccessPointSecurityObject *accessPointSecurityObj=NULL;
    _Dev2WifiAccessPointWpsObject *wpsObj =NULL;
    WLCSM_WLAN_AP_STRUCT *accesspoint;
    WLCSM_WLAN_AP_SECURITY_STRUCT *security;
    WLCSM_WLAN_AP_WPS_STRUCT   *wps;


    WLCSM_WLAN_ADAPTER_STRUCT *adapter = (WLCSM_WLAN_ADAPTER_STRUCT *)(&gp_adapter_objs[idx-1]);
    WLCSM_WLAN_ACCESSPOINT_STRUCT *ssids;

    if(!adapter->ssids) {
        adapter->ssids = malloc( sizeof(WLCSM_WLAN_ACCESSPOINT_STRUCT) * num_of_bssids);
        if(!adapter->ssids) {
            WLCSM_TRACE(WLCSM_TRACE_ERR," allocate mem for AP failed \r\n");
            return -1;
        } else {
            memset(adapter->ssids,0,sizeof(WLCSM_WLAN_ACCESSPOINT_STRUCT)*num_of_bssids);
        }
    }

    ssids=adapter->ssids;
    accesspoint = &(ssids[sub_idx-1].accesspoint);
    security = &(ssids[sub_idx-1].security);
    wps = &(ssids[sub_idx-1].wps);



    iidStack.instance[0] = sub_idx+(idx-1)*num_of_bssids;
    iidStack.currentDepth=1;

    ret = cmsLck_acquireLockWithTimeout( LOCK_WAIT); //3 //3s delay
    if ( ret != CMSRET_SUCCESS ) {
        return ret;
    }
    if ((ret = cmsObj_get(MDMOID_DEV2_WIFI_ACCESS_POINT, &iidStack, 0, (void **)&wlAccessPointCfgObj)) != CMSRET_SUCCESS) {

        if (wlAccessPointCfgObj != NULL) {
            cmsObj_free((void **) &wlAccessPointCfgObj);
        }
        cmsLck_releaseLock();
        return ret;
    }

    ret= __wlcsm_exchange_mapping_value(B_DM_LOADING,(char *)wlAccessPointCfgObj,accesspoint,g_dm_tr181_mapping_DEV2_WIFI_ACCESS_POINT,MAPPING_ENTRY_SIZE(g_dm_tr181_mapping_DEV2_WIFI_ACCESS_POINT));

    if (wlAccessPointCfgObj != NULL) {
        cmsObj_free((void **) &wlAccessPointCfgObj);
    }

    if ((ret = cmsObj_get(MDMOID_DEV2_WIFI_ACCESS_POINT_SECURITY, &iidStack, 0, (void **) &accessPointSecurityObj)) != CMSRET_SUCCESS) {


        if (accessPointSecurityObj!=NULL) {
            cmsObj_free((void **) &accessPointSecurityObj);
        }
        cmsLck_releaseLock();
        return ret;
    }

    ret= __wlcsm_exchange_mapping_value(B_DM_LOADING,(char *)accessPointSecurityObj,security,g_dm_tr181_mapping_DEV2_WIFI_ACCESS_POINT_SECURITY,MAPPING_ENTRY_SIZE(g_dm_tr181_mapping_DEV2_WIFI_ACCESS_POINT_SECURITY));


    if (accessPointSecurityObj!=NULL) {
        cmsObj_free((void **) &accessPointSecurityObj);
    }

    iidStack.instance[0] = sub_idx+(idx-1)*num_of_bssids;
    iidStack.currentDepth=1;
    if ((ret = cmsObj_get(MDMOID_DEV2_WIFI_ACCESS_POINT, &iidStack, 0, (void **)&wlAccessPointCfgObj)) != CMSRET_SUCCESS) {

        if (wlAccessPointCfgObj != NULL) {
            cmsObj_free((void **) &wlAccessPointCfgObj);
        }
        cmsLck_releaseLock();
        return ret;
    }
    if (wlAccessPointCfgObj != NULL) {
        cmsObj_free((void **) &wlAccessPointCfgObj);
    }


    if ((ret = cmsObj_get(MDMOID_DEV2_WIFI_ACCESS_POINT_WPS, &iidStack, 0, (void **) &wpsObj)) != CMSRET_SUCCESS) {
        if (wpsObj!=NULL) {
            cmsObj_free((void **) &wpsObj);
        }
        cmsLck_releaseLock();
        return ret;
    }

    cmsLck_releaseLock();
    ret= __wlcsm_exchange_mapping_value(B_DM_LOADING,(char *)wpsObj,wps,g_dm_tr181_mapping_DEV2_WIFI_ACCESS_POINT_WPS,MAPPING_ENTRY_SIZE(g_dm_tr181_mapping_DEV2_WIFI_ACCESS_POINT_WPS));

    if (wpsObj!=NULL) {
        cmsObj_free((void **) &wpsObj);
    }
    return ret;
}

/*************************************************************//**
 * @brief  load all configuration to manager's structure
 *
 * @return  int
 ***************************************************************/
static int _wlcsm_dm_tr181_load_config(int idx,int from)
{
    int i, start_idx,end_idx,ret=0;

    if(idx) {
        start_idx=idx;
        end_idx=idx;
    } else {
        start_idx=1;
        end_idx= WL_WIFI_RADIO_NUMBER;
    }
    for(idx=start_idx; idx<=end_idx; idx++) {
        ret=_wlcsm_dm_tr181_load_radio(idx);
        WLCSM_TRACE(WLCSM_TRACE_LOG," ==>>>>>bssid_num:%d \r\n",WL_RADIO_WLNUMBSS(idx-1));
        if(!ret) {
            for ( i=1; i<=WL_RADIO_WLNUMBSS(idx-1); i++ ) {
                ret=_wlcsm_dm_tr181_load_bssid(idx,i);
                if(ret || (ret=_wlcsm_dm_tr181_load_access_point(idx,i)))
                    return ret;
            }
        } else
            break;
    }
    return ret;
}

int _wlcsm_dm_tr181_save_nvram(void)
{
    int ret=_wlcsm_dm_save_wifi();
    if(ret==CMSRET_SUCCESS)
        wlWriteMdmToFlash (1);
    return ret;
}


/*************************************************************//**
 * @brief  default setting to data model
 *
 * 	init data model with default value, in mdm case,
 * 	it should init mdm if the configuration is not there
 * 	,something like hardwareAdjusting. after this function
 * 	,data model should have at least the default configruation
 * 	 or loaded saved configuration.
 *
 * @return  int
 ***************************************************************/
static int _wlcsm_dm_tr181_init(void)
{
    int ret=0;

    SINT32 shmId=UNINITIALIZED_SHM_ID;
    CmsLogLevel logLevel=DEFAULT_LOG_LEVEL;
    /*
     * Do CMS initialization after wlevt is spawned so that wlevt does
     * not inherit any file descriptors opened by CMS init code.
     */
    cmsLog_initWithName(EID_WLMNGR, "wlmngr");
    cmsLog_setLevel(logLevel);

    if ((ret = cmsMsg_initWithFlags(EID_WLMNGR, 0, &g_MsgHandle)) != CMSRET_SUCCESS) {
        cmsLog_error("could not initialize msg, ret=%d", ret);
        cmsLog_cleanup();
        return -1;
    }

    /* The wrapper has logic to request the shmId from smd if wlmngr was
     * started on the command line.  It also contains logic to skip MDM
     * initialization if we are in Pure181 mode.
     */
    if ((ret = cmsMdm_init_wrapper(&shmId, g_MsgHandle)) != CMSRET_SUCCESS) {
        cmsMsg_cleanup(&g_MsgHandle);
        cmsLog_cleanup();
        return -1;
    }
    /* this function to init the global variable  g_wifi_obj */
    ret=_wlcsm_dm_load_wifi();
    return ret;
}


static int _wlcsm_dm_tr181_deinit(void)
{
    cmsMdm_cleanup_wrapper();
    cmsMsg_cleanup(&g_MsgHandle);
    cmsLog_cleanup();
    return 0;
}

CmsRet _wlcsm_dm_tr181_oid_mngr_name(char *buf)
{
    unsigned int *nums=(unsigned int *)buf;
    unsigned int oid=nums[0];
    unsigned int offset=nums[1];
    int size=WLCSM_DM_WLMNGR_OID_MAPPING_ENTRY_SIZE(g_wlcsm_tr181_oid_mapping);
    int i=0;
    WLCSM_DM_WLMNGR_OID_MAPPING *oidmapping=NULL;
    WLCSM_DM_WLMNGR_MAPPING *mapping=NULL;
    WLCSM_NAME_OFFSET **mngr_set_pointer=NULL;
    for ( i=0; i<size; i++ ) {
        if(g_wlcsm_tr181_oid_mapping[i].oid==oid) {
            oidmapping=&(g_wlcsm_tr181_oid_mapping[i]);
            mapping=(g_wlcsm_tr181_oid_mapping[i].mapper);
            size=g_wlcsm_tr181_oid_mapping[i].size;
            for ( i=0; i<size; i++ ) {
                WLCSM_TRACE(WLCSM_TRACE_DBG,"i:%d  offset:%d,name:%s \r\n",i,mapping[i].dm_set.offset, mapping[i].dm_set.name );
                if(!mapping[i].dm_set.name) return -1;
                if(mapping[i].dm_set.offset==offset) {
                    if(strlen(mapping[i].wlmngr_set.name)< WLCSM_MNGR_VARNAME_MAX) {
                        sprintf(buf,"%s",mapping[i].wlmngr_set.name);
                        mngr_set_pointer=(WLCSM_NAME_OFFSET **)buf;
                        *mngr_set_pointer= &(mapping[i].wlmngr_set);
                        return oidmapping->mngr_oid;
                    }
                }
            }
        }
    }
    return -1;
}


CmsRet _wlcsm_dm_tr181_getbridge_info(char *buf)
{
    Dev2BridgeObject *brObj=NULL;
    Dev2BridgePortObject *brPortObj=NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack sub_iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret;
    int first=1;
    buf[0]='\0';

    ret = cmsLck_acquireLockWithTimeout( LOCK_WAIT ); //3 //3s delay
    if ( ret != CMSRET_SUCCESS ) {
        printf("Could not get lock!\n");
        return ret;
    }

    while ((ret = cmsObj_getNextFlags(MDMOID_DEV2_BRIDGE, &iidStack,
                                      OGF_NO_VALUE_UPDATE,
                                      (void **) &brObj)) == CMSRET_SUCCESS) {
        UINT32 bridgeNumber;
        bridgeNumber = atoi(&(brObj->X_BROADCOM_COM_IfName[2]));
        cmsAst_assert(bridgeNumber < MAX_LAYER2_BRIDGES);
        INIT_INSTANCE_ID_STACK(&sub_iidStack);
        if(!first) strcat(buf," ");
        strcat(buf,brObj->X_BROADCOM_COM_IfName);
        first=0;
        while (cmsObj_getNextInSubTree(MDMOID_DEV2_BRIDGE_PORT, &iidStack,
                                       &sub_iidStack,(void **) &brPortObj) == CMSRET_SUCCESS) {
            if (!brPortObj->managementPort) {
                strcat(buf,":");
                strcat(buf,brPortObj->name);
            }
            cmsObj_free((void **)&brPortObj);
        }
        cmsObj_free((void **) &brObj);
    }

    cmsLck_releaseLock();
    return ret;
}

static CmsRet wlReadPowerManagement(PwrMngtObject *pPwrMngt)
{
    CmsRet ret = CMSRET_INTERNAL_ERROR;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    PwrMngtObject *pwrMngtObj = NULL;

    ret = cmsLck_acquireLockWithTimeout( LOCK_WAIT );
    if ( ret != CMSRET_SUCCESS ) {
        return ret;
    }

    if ((ret = cmsObj_get(MDMOID_PWR_MNGT, &iidStack, 0, (void **) &pwrMngtObj)) != CMSRET_SUCCESS) {
        cmsLog_error("Failed to get pwrMngtObj, ret=%d", ret);
        cmsLck_releaseLock();
        return ret;
    }

    memcpy(pPwrMngt, pwrMngtObj, sizeof(PwrMngtObject));

    cmsObj_free((void **) &pwrMngtObj);

    cmsLck_releaseLock();
    return ret;
}


static void wlcsm_dm_tr181_togglePowerSave(char *anyassoc)
{
    CmsRet ret = CMSRET_SUCCESS;
    PwrMngtObject pwrMngtObj;
    PWRMNGT_CONFIG_PARAMS configParms;
    int curAVSEn=0;
    UINT32 configMask;
    int chipId;
    char anyAssociated = anyassoc?1:0;
    memset(&pwrMngtObj, 0x00, sizeof(pwrMngtObj)) ;
    memset(&configParms, 0x00, sizeof(configParms)) ;
    if ((ret=wlReadPowerManagement(&pwrMngtObj)) != CMSRET_SUCCESS) {
        printf("Failed to get pwrMngtObj, ret=%d", ret);
        return;
    }
    curAVSEn = pwrMngtObj.avsEn;

    chipId = boardIoctl(BOARD_IOCTL_GET_CHIP_ID, 0, "", 0, 0, "");
    WLCSM_TRACE(WLCSM_TRACE_DBG,"chipid is:%d\r\n",chipId);
    if (chipId == 0x6362) { /* only enabled for 6362 */
        if (anyAssociated) {
            /* disable Adaptive Voltage Scaling */
            configParms.avs = PWRMNGT_DISABLE;
            configMask = PWRMNGT_CFG_PARAM_MEM_AVS_MASK;
            PwrMngtCtl_SetConfig( &configParms, configMask, NULL ) ;
        } else {
            /* restore back to the user setting */
            configParms.avs = curAVSEn;
            configMask = PWRMNGT_CFG_PARAM_MEM_AVS_MASK;
            PwrMngtCtl_SetConfig( &configParms, configMask, NULL ) ;
        }
    }
}

int _wlcsm_dm_tr181_query_info(WLCSM_DM_QUERY_CMD cmd,void *buf)
{
    CmsRet ret;
    switch ( cmd ) {
    case WLCSM_DM_QUERY_BRIDGE_INFO:
        ret=_wlcsm_dm_tr181_getbridge_info((char *)buf);
        break;
    case WLCSM_DM_QUERY_MNGR_ENTRY:
        WLCSM_TRACE(WLCSM_TRACE_DBG," tr181 get mngr entry \r\n" );
        ret=_wlcsm_dm_tr181_oid_mngr_name((char *)buf);
        break;
    case WLCSM_DM_QUERY_SETDBGLEVEL: {
        int dbglevel=0;
        sscanf(buf,"%d",&dbglevel);
        cmsLog_setLevel(dbglevel);
        ret=CMSRET_SUCCESS;
    }
    break;
    case WLCSM_DM_QUERY_PWRCTL: {
        wlcsm_dm_tr181_togglePowerSave(buf);
        ret=CMSRET_SUCCESS;
    }
    break;
    default:
        cmsLog_error("unknow cmd:%d\n",cmd);
        ret=-1;
        break;
    }
    return ret;

}

DECLARE_WLCSM_DM_ITEM(tr181);
