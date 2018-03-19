/*
* <:copyright-BRCM:2013:proprietary:epon
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
* :> 
*/


/*
* \file TkSdkInitApi.c
* \brief the TK sdk init 
*
*/

#include "TkSdkInitApi.h"
#include "TkOamCommon.h"
#include "TkOsAlDataInOut.h"
#include "OamProcessInit.h"
#include "TkOamMem.h"
#include "rdpactl_api.h"
#include "rdpa_types.h"

#include "ethswctl_api.h"
#include <bcmnet.h>

#include "cms_util.h"
#ifdef BRCM_CMS_BUILD
#include "cms_mdm.h"
#include "cms_core.h"
#include "cms_msg.h"
#include "cms_qos.h"
#include "cms_qdm.h"
#include "rut_tmctl_wrap.h"
#endif

LinkConfig gTagLinkCfg;
BOOL stand_alone_mode = FALSE;
#if defined(BCM_PON_XRDP)
char uniRealIfname[MAX_UNI_PORTS][IFNAMESIZ]={"eth0", "eth1", "eth2", "eth3", "eth4", "eth5", "eth6", "eth7"};
#else
char uniRealIfname[MAX_UNI_PORTS][IFNAMESIZ]={"eth0", "eth1", "eth2", "eth3"};
#endif
char eponRealIfname[IFNAMESIZ]={"epon0"};
char eponVlanIfname[IFNAMESIZ]={"epon0.1"};
char eponVeipIfname[MAX_VEIP_If_NUM][IFNAMESIZ];
char eponSfuVirtualIfname[IFNAMESIZ]={"veip0.1"};
char eponVeipCmsIfIdx = 0;

/* the system task exit, delete the tasks, message queue, semphore etc. */
void TkOamStackExit ()
{
    if (SEM_VALID(gTagLinkCfg.semId)) 
    {
        TkOsSemDelete (&gTagLinkCfg.semId);
    }

    if (gTagLinkCfg.qReqId)
    {
        TkOsMsgQDestroy (gTagLinkCfg.qReqId);
    }

    if (!gTagLinkCfg.ThreadId)
    {
        TkOsThreadDestroy(gTagLinkCfg.ThreadId);
    }
    
    TkOamMemsFree ( );

    return ;
}

#ifdef EPON_HGU
static void eponVeipVportNameInit(void)
{
    U8 loop = 0;
    char tempName[IFNAMESIZ] = {0};
   
    do
    {
        memset(tempName, 0, IFNAMESIZ);
        snprintf(tempName, IFNAMESIZ, "%s%d", EPON_IFC_STR, loop);

        memcpy(eponVeipIfname[loop], tempName, IFNAMESIZ);
        cmsLog_debug("eponVeipIfname[%d] %s\n", loop, eponVeipIfname[loop]);

        loop++;
    }while((loop < UniCfgDbGetOamManagedPortCount()) && (loop < MAX_VEIP_If_NUM));
}
#endif

static void eponVportNameInit(void)
{
#ifdef EPON_HGU
    eponVeipVportNameInit();
#endif
}

#ifdef BRCM_CMS_BUILD
static void rdpaDataPathInit(void)
{
    U8 port_id;
    
    /* Set UNI port default action */
    for (port_id = 0; port_id < UniCfgDbGetActivePortCount(); port_id++)
    {
        (void)rdpaCtl_set_port_sal_miss_action(bcm_enet_map_oam_idx_to_phys_port(port_id), FALSE, rdpa_forward_action_host);
        (void)rdpaCtl_set_port_dal_miss_action(bcm_enet_map_oam_idx_to_phys_port(port_id), FALSE, rdpa_forward_action_host);
        (void)bcm_port_learning_ind_set(0, bcm_enet_map_oam_idx_to_phys_port(port_id), TRUE);
#ifdef EPON_SFU
        (void)bcm_port_vlan_isolation_set(0, bcm_enet_map_oam_idx_to_phys_port(port_id), FALSE, FALSE);
#endif
    }

    if (rut_tmctl_getQueueOwner() == TMCTL_OWNER_BH)
    {
        tmctl_if_t tm_if;
        UINT32 qidPrioMap;
        int tmctl_flags = 0;

        memset(&tm_if, 0, sizeof(tm_if));
        for (port_id = 0; port_id < UniCfgDbGetActivePortCount(); port_id++)
        {
            tm_if.ethIf.ifname = uniRealIfname[port_id];

            qidPrioMap = rut_tmctl_getQueueMap();
            tmctl_flags = TMCTL_SCHED_TYPE_SP | TMCTL_INIT_DEFAULT_QUEUES;
            tmctl_flags |= (qidPrioMap == QID_PRIO_MAP_Q7P7)? \
              TMCTL_QIDPRIO_MAP_Q7P7 : TMCTL_QIDPRIO_MAP_Q0P7;

            if (rutoam_tmctl_portTmInit(TMCTL_DEV_ETH, &tm_if,
              tmctl_flags) != TMCTL_SUCCESS)
            {
                cmsLog_error("Failed to setup tm for port %d\n", port_id);
            }
        }
    }
}
#endif

int create_epon_vport(void)
{
    struct ifreq ifr;
    int err, skfd;
    struct ethctl_data ethctl = {};

    if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("socket open error\n");
        return -1;
    }

    strcpy(ifr.ifr_name, "bcmsw");
    if ((err = ioctl(skfd, SIOCGIFINDEX, &ifr)) < 0 )
    {
        printf("bcmsw interface does not exist");
        goto exit;
    }

    ethctl.op = ETHCREATEEPONVPORT;
    ifr.ifr_data = (void *)&ethctl;
    err = ioctl(skfd, SIOCETHCTLOPS, &ifr);
    if (err < 0 )
        printf("Error %d bcm init epon vport\n", err);

exit:
    close(skfd);
    return err;
}

#ifdef BRCM_CMS_BUILD
static const UINT32 udefDefaultVals[] = {0x888e, 0x8809, 0x8808};

/* This function returns index if found mach entry and if not returns the next avaliable index  */
static int checkEntry(const char *type, UINT32 val, int *index )
{
    CmsRet ret=CMSRET_SUCCESS;
	InstanceIdStack iidStack;
    IngressFiltersDataObject *filterData = NULL;
	INIT_INSTANCE_ID_STACK(&iidStack);
	PUSH_INSTANCE_ID(&iidStack,1); 
	cmsLck_acquireLock();

    /* Check if filters alredy configured or the first filter equal to type */
	if ((ret = cmsObj_get(MDMOID_INGRESS_FILTERS_DATA, &iidStack, OGF_NO_VALUE_UPDATE, (void **) &filterData)) != CMSRET_SUCCESS)
	{
		cmsLck_releaseLock();
		return 0;
	}
    if ((!cmsUtl_strcmp(filterData->type, type) && val == 0) ||
        (!cmsUtl_strcmp(filterData->type, type) && filterData->val == val))
    {
        *index = 1;
        cmsObj_free((void **) &filterData);
		cmsLck_releaseLock();
        return 1;
    }

    *index = *index+1;

    /* Search the filter type */
    while ((ret = cmsObj_getNext(MDMOID_INGRESS_FILTERS_DATA, &iidStack, (void **) &filterData)) == CMSRET_SUCCESS)
    {
        if((!cmsUtl_strcmp(filterData->type, type) && val == 0) ||
           (!cmsUtl_strcmp(filterData->type, type) && filterData->val == val))
        {
            cmsObj_free((void **) &filterData);
			cmsLck_releaseLock();
            return 1;
        }
		cmsObj_free((void **) &filterData);
        *index = *index+1;
    }

    cmsObj_free((void **) &filterData);
	cmsLck_releaseLock();
    return 0;
}

static CmsRet eponWanFilterEntryConfig(const char *type, UINT32 val)
{
    CmsRet ret = CMSRET_SUCCESS;
    int index=1;
    int i=0;
    IngressFiltersDataObject *filterData = NULL;
    char strPortMask[BUFLEN_64+1]={};
    memset(strPortMask, '0', BUFLEN_64 + 1);
    strPortMask[BUFLEN_64] = '\0';

    /* Get the valid index for this wan filter */
    i = checkEntry(type, val, &index);
	InstanceIdStack iidStack;
	INIT_INSTANCE_ID_STACK(&iidStack);
	cmsLck_acquireLock();

	if (i)
	{
		PUSH_INSTANCE_ID(&iidStack, index); 
		if ((ret = cmsObj_get(MDMOID_INGRESS_FILTERS_DATA, &iidStack, OGF_NORMAL_UPDATE, (void **) &filterData)) != CMSRET_SUCCESS)
		{
			cmsLck_releaseLock();
			return 0;
		}
        cmsUtl_strncpy(strPortMask, filterData->ports, sizeof(strPortMask));

        /* set the last char in the arry to 1 means wan0 port will be marked in the port bit mask */
        strPortMask[BUFLEN_64-1] = '1';
        filterData->ports = cmsMem_strdup(strPortMask);
        filterData->val = val;

        if ((ret = cmsObj_set((void *)filterData, &iidStack)) != CMSRET_SUCCESS)
            cmsLog_error("Could not set filter data object, ret=%d", ret);
    }
    else
	{
		if ((ret = cmsObj_addInstance(MDMOID_INGRESS_FILTERS_DATA, &iidStack)) != CMSRET_SUCCESS)
		{
			printf("%s@%d Add Obj Error=%d\n", __FUNCTION__, __LINE__, ret );
			goto exit;
		}
        strPortMask[BUFLEN_64-1] = '1';
		if ((ret = cmsObj_get(MDMOID_INGRESS_FILTERS_DATA, &iidStack, OGF_NORMAL_UPDATE, (void **) &filterData)) != CMSRET_SUCCESS)
		{
            cmsLog_error("Could not get filter data object, ret=%d", ret);
			cmsLck_releaseLock();
			return 0;
		}

        /* Update the instance port map */
        filterData->ports = cmsMem_strdup(strPortMask);
        filterData->type = cmsMem_strdup(type);
        filterData->action = cmsMem_strdup(MDMVS_FILTER_CPU);
        filterData->val = val;
        if ((ret = cmsObj_set((void *)filterData, &iidStack)) != CMSRET_SUCCESS)
            cmsLog_error("Could not set filter data object, ret=%d", ret);
    }
exit:
	if(filterData)
		cmsObj_free((void **) &filterData);
	cmsLck_releaseLock();
    return ret;
}
#endif

/* the system task init */
STATUS TkOamStackInit (BOOL stand_alone, U8 vendorMask)
{
	int i;
    char cmdStr[64];
    BOOL MpcpRegisterEn = FALSE;

	stand_alone_mode = stand_alone;
    
#ifdef BRCM_CMS_BUILD
    CmsRet ret = CMSRET_SUCCESS;
#endif

    /* stop Mpcp Register process*/
    eponStack_CtlMpcpRegister(EponSetOpe, &MpcpRegisterEn);

#ifndef DESKTOP_LINUX
    create_epon_vport();
    memset(cmdStr, 0,sizeof(cmdStr));
    sprintf(cmdStr, "ifconfig %s up", eponRealIfname);
    system(cmdStr);
#endif
    eponVportNameInit();
#ifdef BRCM_CMS_BUILD
	for (i = 0; i < sizeof(udefDefaultVals)/sizeof(UINT32); i++)
	{
        ret = eponWanFilterEntryConfig(MDMVS_FILTER_ETYPE_UDEF,udefDefaultVals[i]);
		if (ret)
			cmsLog_error("Could not set udefDefaultVals, ret=%d", ret);
    }

    rdpaDataPathInit();
#endif
    OamProcessInit(vendorMask);

    TkOamMemInit(24, 2048);

    TkOsDataRxTxInit();

    TkOsDataRxThreadInit(TkDataProcessHandle);

#ifdef DPOE_OAM_BUILD
    if (vendorMask & OAM_DPOE_SUPPORT)
    {
        TkOsOnuBusyMonitorThreadInit();
    }
#endif

    /* start Mpcp Register process*/
    MpcpRegisterEn = TRUE;
    eponStack_CtlMpcpRegister(EponSetOpe, &MpcpRegisterEn);

    return OK;
}


