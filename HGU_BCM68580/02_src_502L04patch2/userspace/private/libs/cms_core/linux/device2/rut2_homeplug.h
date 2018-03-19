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

#ifndef __RUT_HOMEPLUG_H__
#define __RUT_HOMEPLUG_H__

/*!\file rut_homeplug.h
 * \brief System level interface functions for Homeplug functionality.
 *
 * In most cases, the functions in this file should only be called by
 * RCL, STL, and other RUT functions.
 */


#include "cms.h"
#include "devctl_homeplug.h"

/** Change Alias parameter if it is necessary.
 *
 * @param newObj  (IN) The new object.
 * @param currObj (IN) The current object.
 *
 * @return CMSRET_SUCCESS on success otherwiser return CMSRET_INTERNAL_ERROR;
 */
CmsRet rutHomeplug_setInterfaceAlias(const _HomePlugInterfaceObject *newObj,
                                     const _HomePlugInterfaceObject *currObj);

/** Change LogicalNetwork parameter if it is necessary.
 *
 * @param newObj  (IN) The new object.
 * @param currObj (IN) The current object.
 *
 * @return CMSRET_SUCCESS on success otherwiser return CMSRET_INTERNAL_ERROR;
 */
CmsRet rutHomeplug_setInterfaceLogicalNetwork(const _HomePlugInterfaceObject *newObj,
                                              const _HomePlugInterfaceObject *currObj);

/** Change NetworkPassword parameter if it is necessary.
 *
 * @param newObj  (IN) The new object.
 * @param currObj (IN) The current object.
 *
 * @return CMSRET_SUCCESS on success otherwiser return CMSRET_INTERNAL_ERROR;
 */
CmsRet rutHomeplug_setInterfaceNetworkPassword(_HomePlugInterfaceObject *newObj,
                                               const _HomePlugInterfaceObject *currObj);

/** Change X_BROADCOM_COM_DiagPeriodicInterval parameter if it is necessary.
 *
 * @param newObj  (IN) The new object.
 * @param currObj (IN) The current object.
 *
 * @return CMSRET_SUCCESS on success otherwiser return CMSRET_INTERNAL_ERROR;
 */
CmsRet rutHomeplug_setInterfaceDiagPeriodicInterval(const _HomePlugInterfaceObject *newObj,
                                                    const _HomePlugInterfaceObject *currObj);

/** Change Status parameter if it is necessary.
 *
 * @param status  (IN)  The new value.
 *
 * @return CMSRET_SUCCESS on success otherwiser return CMSRET_INTERNAL_ERROR;
 */
CmsRet rutHomeplug_setInterfaceStatus(BCMHPAV_STATUS status);


/** Detect changes of any fields between newObj and currObj.
 *
 * @param newObj  (IN) The new object.
 * @param currObj (IN) The current object.
 *
 * @return TRUE if any of the fields (except for the enabled field) has changed.
 */
UBOOL8 rutHomeplug_isValuesChanged(const _HomePlugInterfaceObject *newObj, const _HomePlugInterfaceObject *currObj);


/** Update the number of associated devices in the homeplug interface object
 *
 * @param iidStack (IN) where the new homeplug device entry is being added/deleted.
 * @param delta (IN) the number to change by.
 */
void rutHomeplug_modifyNumAssocHomeplugDevices(const InstanceIdStack *iidStack, SINT32 delta);

#endif  /* __RUT_HOMEPLUG_H__ */
