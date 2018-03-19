/* 
* <:copyright-BRCM:2007:proprietary:gpon
* 
*    Copyright (c) 2007 Broadcom 
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

#include "bcm_OS_Deps.h"
#include <linux/bcm_log.h>
#include "bcm_gponBasicDefs.h"
#include "bcm_omci.h"
#include "bcm_ploamPortCtrl.h"
#include "bcm_ploam.h"
#include "bdmf_dev.h"

int skip_drv_init = 0;
module_param(skip_drv_init, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

extern int rdpa_gpon_declare_and_register_to_bdmf(void);
extern void rdpa_gpon_bdmf_unregister(void);

extern int rdpa_gpon_init_stack(char *wan_type, char *wan_rate);
extern void rdpa_gpon_exit_stack(void);


int __init gponModuleCreate(void) 
{   
    int  ret;
    int  count;
    char wan_type_buf[PSP_BUFLEN_16] = {};
    char wan_rate_buf[PSP_BUFLEN_16] = {};

    count = kerSysScratchPadGet((char*)RDPA_WAN_TYPE_PSP_KEY, (char*)wan_type_buf, (int)sizeof(wan_type_buf));
    if (count == 0)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_GPON, "ScratchPad does not contain RdpaWanType");
        return -1;
    }
    count = kerSysScratchPadGet((char*)RDPA_WAN_RATE_PSP_KEY, (char*)wan_rate_buf, (int)sizeof(wan_rate_buf));
    if (count == 0)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_GPON, "ScratchPad does not contain RdpaWanRate");
        return -1;
    }
    if (strcasecmp(wan_type_buf, "GPON") && strcasecmp(wan_type_buf, "XGPON1") && 
        strcasecmp(wan_type_buf, "NGPON2") && strcasecmp(wan_type_buf, "XGS"))
    {
        BCM_LOG_NOTICE(BCM_LOG_ID_GPON, "RdpaWanType is not GPON/XGPON/NGPON2 ( %s ), Remove module", wan_type_buf);
        return -1;
    }

    ret = rdpa_gpon_declare_and_register_to_bdmf();
    if (ret < 0) 
    {
        BCM_LOG_ERROR(BCM_LOG_ID_GPON, "GPON registration to BDMF failed: ret=%d", ret);
        return -1;
    }

    ret = rdpa_gpon_init_stack(wan_type_buf, wan_rate_buf);
    if (ret < 0) 
    {
        BCM_LOG_ERROR(BCM_LOG_ID_GPON, "Init stack failed: ret=%d", ret);
        return -1;
    }

    if (!skip_drv_init) 
    {
        ret = bcm_ploamCreate();
        if (ret) 
        {
            BCM_LOG_ERROR(BCM_LOG_ID_GPON, "Ploam create failed.");
            return -1;
        }
        
        ret = bcm_omciCreate();
        if (ret) 
        {
            BCM_LOG_ERROR(BCM_LOG_ID_GPON, "Omci create failed.");
            bcm_ploamDelete();
            return -1;
        }  
        
        bcm_ploamInit(1);
    }

    BCM_LOG_INFO(BCM_LOG_ID_GPON, "GPON drivers initialization done");
    
    return 0;
}

void __exit gponModuleDelete(void) 
{
    if (!skip_drv_init) 
        bcm_ploamDelete();
    rdpa_gpon_exit_stack();
    rdpa_gpon_bdmf_unregister();

    BCM_LOG_NOTICE(BCM_LOG_ID_GPON, "gponModule deleted");
}

MODULE_DESCRIPTION("Broadcom GPON Driver");
MODULE_LICENSE("Proprietary");
module_init(gponModuleCreate);
module_exit(gponModuleDelete);

