/***********************************************************************
 *
 *  Copyright (c) 2014  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2014:proprietary:standard

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

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "dal.h"

#if defined(SUPPORT_IEEE1905)
#ifdef BRCM_WLAN
int wlgetintfNo( void );
#endif

// Called with CMS lock
CmsRet dalIeee1905_getCurrentCfg(WEB_NTWK_VAR* webVar)
{
    CmsRet ret = CMSRET_SUCCESS;
    Dev2Ieee1905AlObject *ieee1905AlObj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

    ret = cmsObj_get(MDMOID_DEV2_IEEE1905_AL, &iidStack, 0, (void **) &ieee1905AlObj);
    if ((ret == CMSRET_SUCCESS) && ieee1905AlObj)
    {
        /* set ieee1905 webVar parameters */      
        webVar->ieee1905IsEnabled = ieee1905AlObj->enable;
        if (ieee1905AlObj->deviceFriendlyName) {
            strncpy(webVar->ieee1905Dfname, ieee1905AlObj->deviceFriendlyName, sizeof(webVar->ieee1905Dfname)-1);
            webVar->ieee1905Dfname[sizeof(webVar->ieee1905Dfname)-1] = '\0';
        }
        else {
            strcpy(webVar->ieee1905Dfname, "");
        }
        webVar->ieee1905IsRegistrar  = ieee1905AlObj->isRegistrar;
        webVar->ieee1905ApFreq24En   = ieee1905AlObj->APFreqBand24Enable;
        webVar->ieee1905ApFreq5En    = ieee1905AlObj->APFreqBand5Enable;
        webVar->ieee1905ShowApFreq24 = 0;
        webVar->ieee1905ShowApFreq5  = 0;
#ifdef BRCM_WLAN
        if ( ieee1905AlObj->isRegistrar ) {
            webVar->ieee1905ShowApFreq24 = 1;
            webVar->ieee1905ShowApFreq5 = 1;
        }
        else
        {
            FILE *fp;
            char buf[BUFLEN_64];
            int rt;
            int wlBand;
            int wlcnt;
            int i;
            char phyType;

            wlcnt = wlgetintfNo();
            for (i = 0; i < wlcnt; i++) {
                sprintf(buf, "nvram get %s%d_phytype", WLAN_IFC_STR, i);
                fp = popen(buf, "r");
                if (fp == NULL) {
                    continue;
                }

                rt = fscanf(fp, "%c", &phyType);
                if ( 1 != rt ) {
                    pclose(fp);
                    continue;
                }
                pclose(fp);

                switch (phyType)
                {
                    case 'a':
                    case 'v':
                        webVar->ieee1905ShowApFreq5 = 1;
                        break;
                    case 'b':
                    case 'g':
                        webVar->ieee1905ShowApFreq24 = 1;
                        break;
                    case 'n':
                    case 'h':
                    case 'l':
                        snprintf(buf, sizeof(buf), "nvram get %s%d_nband", WLAN_IFC_STR, i);
                        fp = popen(buf, "r");
                        if (fp == NULL) {
                            continue;
                        }
                        rt = fscanf(fp, "%u", &wlBand);
                        if ( 1 == rt ) {
                            switch( wlBand ) {
                                case 2:
                                    webVar->ieee1905ShowApFreq24 = 1;
                                    break;
                                case 1:
                                    webVar->ieee1905ShowApFreq5 = 1;
                                    break;
                            }
                        }
                        pclose(fp);
                        break;
                }
            }
        }
#endif
    }
    return ret;
}
#endif