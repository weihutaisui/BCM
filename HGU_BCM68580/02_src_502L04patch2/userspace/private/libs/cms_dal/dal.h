/***********************************************************************
 *
 *  Copyright (c) 2006  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:standard

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

#ifndef __DAL_H__
#define __DAL_H__


/*!\file dal.h
 * \brief private header file for the Data Aggregation Layer API.
 *
 */

#include "cms_core.h"


/** If a given buffer pointer is not NULL, insert a 0 byte so it becomes a 
 *  null terminated string.
 */
#define IF_PTR_NOT_NULL_TERMINATE(p) if ((p) != NULL) {*(p) = 0;}


/** If the source string pointer is not NULL, copy into destination buffer,
 *  otherwise, insert a 0 byte into the destination buffer so it becomes a
 *  null terminated string.
 */
#define STR_COPY_OR_NULL_TERMINATE(dst, src)                      \
   if ((dst) != NULL) {if ((src) != NULL) strcpy(dst, src); else *(dst) = 0;};




/** Read the DeviceInfo object and fill in the WEB_NTWK_VAR struct for display on WebUI.
 *
 *@param (IN) pointer to WEB_NTWK_VAR
 */
void getDeviceInfo(WEB_NTWK_VAR *webVar);

void getDeviceInfo_igd(WEB_NTWK_VAR *webVar);

void getDeviceInfo_dev2(WEB_NTWK_VAR *webVar);

#if defined(SUPPORT_DM_LEGACY98)
#define getDeviceInfo(w)          getDeviceInfo_igd((w))
#elif defined(SUPPORT_DM_HYBRID)
#define getDeviceInfo(w)          getDeviceInfo_igd((w))
#elif defined(SUPPORT_DM_PURE181)
#define getDeviceInfo(w)          getDeviceInfo_dev2((w))
#elif defined(SUPPORT_DM_DETECT)
#define getDeviceInfo(w)          (cmsMdm_isDataModelDevice2() ? \
                                   getDeviceInfo_dev2((w)) : \
                                   getDeviceInfo_igd((w)))
#endif




#endif /*__DAL_H__ */
