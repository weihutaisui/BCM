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
*      MDM-lite definition, API and CMS-OBJ/MDM shim layer header file.
*
*****************************************************************************/

#ifndef _MDMLITE_API_H_
#define _MDMLITE_API_H_

/* ---- Include Files ----------------------------------------------------- */

#include "mdmlite_defs.h"
#include "mdmlite_shim.h"


/* ---- Constants and Types ----------------------------------------------- */

#define MDM_FULLPATH_CONVERSION 1


/* ---- Macro API definitions --------------------------------------------- */


/* ---- Variable Externs -------------------------------------------------- */


/* ---- Function Prototypes ----------------------------------------------- */

/* MDM */
void mdmlite_initPathDescriptor(_MdmPathDescriptor *pathDesc);
CmsRet mdmlite_addObjectInstance(_MdmPathDescriptor *pathDesc, void *cbFunc,
  void *cbContext);
void mdmlite_freeObject(void **mdmObj);
CmsRet mdmlite_getObject(MdmObjectId oid, const InstanceIdStack *iidStack,
  void **mdmObj);
CmsRet mdmlite_getNextObject(MdmObjectId oid, InstanceIdStack *iidStack,
  void **mdmObj);
CmsRet mdmlite_setObject(void **mdmObj, const InstanceIdStack *iidStack,
  UBOOL8 doValueChanged);
void mdmlite_traverseOidNodes(_MdmNodeTraverseCallback cbFunc, void *cbContext);
void mdmlite_traverseAllNodes(_MdmObjectNode *objNode, InstanceIdStack
  *iidStack, _MdmNodeTraverseCallback cbFunc, void *cbContext);
#ifdef MDM_FULLPATH_CONVERSION
CmsRet mdmlite_fullPathToPathDescriptor(const char *fullpath,
  _MdmPathDescriptor *pathDesc);
CmsRet mdmlite_pathDescriptorToFullPath(const _MdmPathDescriptor *pathDesc,
  char **fullpath);
#endif /* MDM_FULLPATH_CONVERSION */

#endif /* _MDMLITE_API_H_ */
