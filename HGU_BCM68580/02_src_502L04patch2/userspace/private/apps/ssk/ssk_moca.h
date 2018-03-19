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

#ifndef __SSK_MOCA_H__
#define __SSK_MOCA_H__


/*!\file ssk_moca.h
 * \brief private header file for Moca utility functions that are used by ssk.
 *
 */

#include "cms_core.h"
#include "cms_msg.h"


/** Write LOF value into MDM object
 *
 * @param obj  (IN/OUT) : This can be a TR98 LanMocaInterface, TR98
 *                        WanMocaInterface (which is exactly the same as
 *                        LANMocaInterface), or a TR181 MocaInterface obj.
 * @param lof  (IN) :     The lof value to write into the object.
 */
void sskMoca_writeLof(void *obj, UINT32 lof);
void sskMoca_writeLof_igd(void *obj, UINT32 lof);
void sskMoca_writeLof_dev2(void *obj, UINT32 lof);

#if defined(SUPPORT_DM_LEGACY98)
#define sskMoca_writeLof(o, v)           sskMoca_writeLof_igd((o), (v))
#elif defined(SUPPORT_DM_HYBRID)
#define sskMoca_writeLof(o, v)           sskMoca_writeLof_igd((o), (v))
#elif defined(SUPPORT_DM_PURE181)
#define sskMoca_writeLof(o, v)           sskMoca_writeLof_dev2((o), (v))
#elif defined(SUPPORT_DM_DETECT)
#define sskMoca_writeLof(o, v)  (cmsMdm_isDataModelDevice2() ? \
                                      sskMoca_writeLof_dev2((o), (v)) : \
                                      sskMoca_writeLof_igd((o), (v)))
#endif




/** Return the LOF value from the given MDM object.
 *
 * @param obj  (IN/OUT) : This can be a TR98 LanMocaInterface, TR98
 *                        WanMocaInterface (which is exactly the same as
 *                        LANMocaInterface), or a TR181 MocaInterface obj.
 *
 * @return The LOF value.
 */
UINT32 sskMoca_readLof(const void *obj);
UINT32 sskMoca_readLof_igd(const void *obj);
UINT32 sskMoca_readLof_dev2(const void *obj);

#if defined(SUPPORT_DM_LEGACY98)
#define sskMoca_readLof(o)           sskMoca_readLof_igd((o))
#elif defined(SUPPORT_DM_HYBRID)
#define sskMoca_readLof(o)           sskMoca_readLof_igd((o))
#elif defined(SUPPORT_DM_PURE181)
#define sskMoca_readLof(o)           sskMoca_readLof_dev2((o))
#elif defined(SUPPORT_DM_DETECT)
#define sskMoca_readLof(o)  (cmsMdm_isDataModelDevice2() ? \
                                      sskMoca_readLof_dev2((o)) : \
                                      sskMoca_readLof_igd((o)))
#endif




/** Write NonDefSeqNum value into MDM object
 *
 * @param obj  (IN/OUT) : This can be a TR98 LanMocaInterface, TR98
 *                        WanMocaInterface (which is exactly the same as
 *                        LANMocaInterface), or a TR181 MocaInterface obj.
 * @param num  (IN) :     The nonDefSeqNum value to write into the object.
 */
void sskMoca_writeNonDefSeqNum(void *obj, UINT32 num);
void sskMoca_writeNonDefSeqNum_igd(void *obj, UINT32 num);
void sskMoca_writeNonDefSeqNum_dev2(void *obj, UINT32 num);

#if defined(SUPPORT_DM_LEGACY98)
#define sskMoca_writeNonDefSeqNum(o, v)  sskMoca_writeNonDefSeqNum_igd((o), (v))
#elif defined(SUPPORT_DM_HYBRID)
#define sskMoca_writeNonDefSeqNum(o, v)  sskMoca_writeNonDefSeqNum_igd((o), (v))
#elif defined(SUPPORT_DM_PURE181)
#define sskMoca_writeNonDefSeqNum(o, v)  sskMoca_writeNonDefSeqNum_dev2((o), (v))
#elif defined(SUPPORT_DM_DETECT)
#define sskMoca_writeNonDefSeqNum(o, v)  (cmsMdm_isDataModelDevice2() ? \
                               sskMoca_writeNonDefSeqNum_dev2((o), (v)) : \
                               sskMoca_writeNonDefSeqNum_igd((o), (v)))
#endif




/** Return the NonDefSeqNum value from the given MDM object.
 *
 * @param obj  (IN/OUT) : This can be a TR98 LanMocaInterface, TR98
 *                        WanMocaInterface (which is exactly the same as
 *                        LANMocaInterface), or a TR181 MocaInterface obj.
 *
 * @return The NonDefSeqNum value.
 */
UINT32 sskMoca_readNonDefSeqNum(const void *obj);
UINT32 sskMoca_readNonDefSeqNum_igd(const void *obj);
UINT32 sskMoca_readNonDefSeqNum_dev2(const void *obj);

#if defined(SUPPORT_DM_LEGACY98)
#define sskMoca_readNonDefSeqNum(o)       sskMoca_readNonDefSeqNum_igd((o))
#elif defined(SUPPORT_DM_HYBRID)
#define sskMoca_readNonDefSeqNum(o)       sskMoca_readNonDefSeqNum_igd((o))
#elif defined(SUPPORT_DM_PURE181)
#define sskMoca_readNonDefSeqNum(o)       sskMoca_readNonDefSeqNum_dev2((o))
#elif defined(SUPPORT_DM_DETECT)
#define sskMoca_readNonDefSeqNum(o)  (cmsMdm_isDataModelDevice2() ? \
                                      sskMoca_readNonDefSeqNum_dev2((o)) : \
                                      sskMoca_readNonDefSeqNum_igd((o)))
#endif




/** Write MocaControl value into MDM object
 *
 * @param obj  (IN/OUT) : This can be a TR98 LanMocaInterface, TR98
 *                        WanMocaInterface (which is exactly the same as
 *                        LANMocaInterface), or a TR181 MocaInterface obj.
 * @param controlStr (IN) :  The MocaControl string to write into the object.
 */
void sskMoca_writeMocaControl(void *obj, const char *controlStr);
void sskMoca_writeMocaControl_igd(void *obj, const char *controlStr);
void sskMoca_writeMocaControl_dev2(void *obj, const char *controlStr);

#if defined(SUPPORT_DM_LEGACY98)
#define sskMoca_writeMocaControl(o, v)  sskMoca_writeMocaControl_igd((o), (v))
#elif defined(SUPPORT_DM_HYBRID)
#define sskMoca_writeMocaControl(o, v)  sskMoca_writeMocaControl_igd((o), (v))
#elif defined(SUPPORT_DM_PURE181)
#define sskMoca_writeMocaControl(o, v)  sskMoca_writeMocaControl_dev2((o), (v))
#elif defined(SUPPORT_DM_DETECT)
#define sskMoca_writeMocaControl(o, v)  (cmsMdm_isDataModelDevice2() ? \
                                 sskMoca_writeMocaControl_dev2((o), (v)) : \
                                 sskMoca_writeMocaControl_igd((o), (v)))
#endif


#endif /*__SSK_MOCA_H__ */
