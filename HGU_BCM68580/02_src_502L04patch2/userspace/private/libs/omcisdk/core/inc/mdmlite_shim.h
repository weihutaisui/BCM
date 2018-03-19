/***********************************************************************
 *
 *  Copyright (c) 2017 Broadcom
 *  All Rights Reserved
 *
<:label-BRCM:2017:proprietary:omcid

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

/*****************************************************************************
*    Description:
*
*      Header file for CMS-OBJ and MDM shim APIs on top of MDM-lite.
*
*****************************************************************************/

#ifndef _MDMLITE_SHIM_H_
#define _MDMLITE_SHIM_H_

/* ---- Include Files ----------------------------------------------------- */

#include "mdmlite_defs.h"


/* ---- Constants and Types ----------------------------------------------- */

#define _mdm_initPathDescriptor mdmlite_initPathDescriptor
#define _mdm_addObjectInstance  mdmlite_addObjectInstance
#define _mdm_freeObject         mdmlite_freeObject
#define _mdm_getObject          mdmlite_getObject
#define _mdm_getNextObject      mdmlite_getNextObject
#define _mdm_setObject          mdmlite_setObject


/* ---- Variable Externs -------------------------------------------------- */


/* ---- Function Prototypes ----------------------------------------------- */

/* CMS OBJ */
CmsRet _cmsObj_get(MdmObjectId oid, const InstanceIdStack *iidStack,
  UINT32 getFlags, void **mdmObj);
CmsRet _cmsObj_getNext(MdmObjectId oid, InstanceIdStack *iidStack,
  void **mdmObj);
CmsRet _cmsObj_getNextFlags(MdmObjectId oid, InstanceIdStack *iidStack,
  UINT32 getFlags, void **mdmObj);
CmsRet _cmsObj_getNextInSubTree(MdmObjectId oid, const InstanceIdStack
  *parentIidStack, InstanceIdStack *iidStack, void **mdmObj);
CmsRet _cmsObj_getNextInSubTreeFlags(MdmObjectId oid,
  const InstanceIdStack *parentIidStack, InstanceIdStack *iidStack,
  UINT32 getFlags, void **mdmObj);
CmsRet _cmsObj_getNthParam(const void *mdmObj, const UINT32 paramNbr,
  _MdmObjParamInfo *paramInfo);
void _cmsObj_free(void **mdmObj);
CmsRet _cmsObj_set(const void *mdmObj, const InstanceIdStack *iidStack);
CmsRet _cmsObj_setNoRclCallback(const void *mdmObj, const InstanceIdStack *iidStack);

CmsRet _cmsObj_setNthParam(void *mdmObj, const UINT32 paramNbr,
  const void *paramVal);
CmsRet _cmsObj_addInstance(MdmObjectId oid, InstanceIdStack *iidStack);
CmsRet _cmsObj_deleteInstance(MdmObjectId oid, const InstanceIdStack *iidStack);

/* MDM */
const char *_mdm_oidToGenericPath(MdmObjectId oid);
_MdmOidInfoEntry *mdmlite_getOidInfo(MdmObjectId oid);
char *_cmsMdm_dumpIidStack(const InstanceIdStack *iidStack);

/* Lock */
CmsRet _cmsLck_acquireLockWithTimeout(UINT32 timeoutMilliSeconds);
CmsRet _cmsLck_releaseLock(void);

/* OMCI */
CmsRet omcimib_init(void);
void omcmib_cleanup(void);
void omcimib_dumpAll(void);
void omcimib_dumpObject(const void *mdmObj);
void omcimib_dumpObjectId(MdmObjectId oid);
CmsRet omcimib_getSupportedObjectInfo(OmciObjectInfo_t **objectInfoListPP,
  SINT32 *entryNumP);
CmsRet omcimib_getObjectInfo(OmciObjectInfo_t **objectInfoListPP,
  SINT32 *entryNumP);
CmsRet omcimib_getParamInfo(MdmObjectId oid, OmciParamInfo_t **paramInfoListPP,
  SINT32 *entryNumP);


#endif /* _MDMLITE_SHIM_H_ */
