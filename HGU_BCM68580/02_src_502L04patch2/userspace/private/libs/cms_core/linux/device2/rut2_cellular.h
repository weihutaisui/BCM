/***********************************************************************
 *
 *  Copyright (c) 2009  Broadcom Corporation
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

#ifndef __RUT_CELLULAR_H__
#define __RUT_CELLULAR_H__

/*!\file rut_cellular.h
 * \brief System level interface functions for cellular functionality.
 *
 * In most cases, the functions in this file should only be called by
 * RCL, STL, and other RUT functions.
 */


#include "cms.h"
#include "cms_core.h"

#define CELLULAR_APN_USER_BASE_ID 1000 //Base _id in telephony.db to append user added APN
#define CELLULAR_AUTO_SEL_APN_NAME "AUTO"
#define CELLULAR_APN_TYPE_DEFAULT "default"
#define RILAPN_TELEPHONY_DB_FILE "/data/radio/telephony.db"
#define RILAPN_PREF_APN_FILE     "/data/radio/preferred-apn.xml"

typedef struct
{
    UINT32    id;	/**< Id */
    char      APN[BUFLEN_64+1];	/**< APN */
    UBOOL8    enable;	/**< Id */
    char      type[BUFLEN_128];	/**< Type */
    char      MCC[BUFLEN_4];	/**< MCC */
    char      MNC[BUFLEN_4];	/**< MNC */
    UINT8     enblIpVer;
    char      carrier[BUFLEN_128+1];	/**< Carrier */
    char      mmsproxy[BUFLEN_16+1];	/**< Mmsproxy */
    UINT32    mmsport;	/**< Mmsport */
    char      mmsc[BUFLEN_128+1];	/**< Mmsc */
    SINT32    authtype;	/**< Authtype */
    char      username[BUFLEN_256+1];	/**< Username */
    char      password[BUFLEN_256+1];	/**< Password */
    char      proxy[BUFLEN_16+1];	/**< Proxy */
    UINT32    proxyPort;	/**< ProxyPort */
    UBOOL8    bearer;
} Dev2CellularApnInfo;


CmsRet rutCellularDB_getApnCount(char *mcc, char* mnc, UINT32 *count);

CmsRet rutCellularDB_getApnByOffset(char *mcc, char* mnc, UINT32 offset, Dev2CellularApnInfo *cellularApnInfo);

CmsRet rutCellularDB_getApnById(Dev2CellularApnInfo *cellularApnInfo);

CmsRet rutCellularDB_addApn(Dev2CellularAccessPointObject *cellularApnObj);

CmsRet rutCellularDB_DelApn(UINT32 id);

CmsRet rutCellular_setPrefdApn(UINT32 id, const InstanceIdStack *skipIidStack);

CmsRet rutCellular_getPrefdApn(UINT32 *id);

UBOOL8 rutCellular_getIfnameByApnInstId(const UINT8 apnInstanceID, char *ifname);

#endif  /* __RUT_CELLULAR_H__ */
