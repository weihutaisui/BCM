/***********************************************************************
 *
 *  Copyright (c) 2017 Broadcom
 *  All Rights Reserved
 *
<:label-BRCM:2017:proprietary:standard

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
*      Header file for MDM-lite local definitions and APIs.
*
*****************************************************************************/

#ifndef MDMLITE_LOCAL_H
#define MDMLITE_LOCAL_H

/* ---- Include Files ----------------------------------------------------- */

#include "mdmlite_glue.h"
#include "mdmlite_defs.h"


/* ---- Constants and Types ----------------------------------------------- */

#define OBN_INSTANCE_NODE            0x01

#define ACTION_CHECK_ACCESS          0x01
#define ACTION_VALIDATE              0x02

/* Structure to track multiple instances of sub-trees.
 * iidStack - instance id's needed to reach this instance node.
 * next - next _InstanceHeadNode
 * objData - linked list of instances.
 */
typedef struct _InstanceHeadNode
{
   InstanceIdStack iidStack;
   struct _InstanceHeadNode *next;
   UINT32 nextInstanceIdToAssign;
   void *objData;
} _InstanceHeadNode;

/* Structure to track multiple instances of an object.
 * These structures are organized in a singly linked list. The head
 * of the list is a InstanceHeadNode.
 *
 * instanceId - the instance id of this node.
 * next - next _InstanceDescNode, if any.
 * objData - object instance.
 */
typedef struct _InstanceDescNode
{
   UINT32 instanceId;
   struct _InstanceDescNode *next;
   void *objData;
} _InstanceDescNode;

/* Context object for mdm_traverseParamNodes */
typedef struct paramsContext
{
   char *buf;
   char *indentBuf;
   UINT32 idx;
   UINT32 max;
   UINT32 paramIndex;
} _paramsContext;

/* Callback function used in addObjectInstance and deleteObjectInstance */
typedef CmsRet (*_MdmOperationCallback)(_MdmObjectNode *objNode,
  InstanceIdStack *iidStack, void *mdmObj, void *cbContext);

/* Callback function used in traverseParamNodes. */
typedef void (*_MdmParamTraverseCallback)(_MdmParamNode *paramNode,
   void *currParamVal, void *cbContext);


/* ---- Macro API definitions --------------------------------------------- */

#ifdef CC_DUMPIIDSTACK_DUMMY
#define mdmlite_dumpIidStack "na"
#else
#define mdmlite_dumpIidStack mdmlite_dumpIidStackDetail
#endif

//#define CC_OMCIDB_DEBUG 1

#define mdmlite_error(fmt, arg...) \
  printf("OMCIDB ERR[%s.%u]: " fmt "\n", __FUNCTION__, __LINE__, ##arg)

#if defined(CC_OMCIDB_DEBUG)
#define mdmlite_debug(fmt, arg...) \
  printf("OMCIDB DBG[%s.%u]: " fmt "\n", __FUNCTION__, __LINE__, ##arg)
#else
#define mdmlite_debug(fmt, arg...)
#endif

#define IS_INSTANCE_NODE(n) ((n)->flags & OBN_INSTANCE_NODE)
#define IS_INDIRECT0(n) ((n)->instanceDepth == 0)
#define IS_INDIRECT2(n) (((n)->instanceDepth > 0) && (IS_INSTANCE_NODE(n)))
#define SET_IIDSTACK_DEPTH(s, d) ((s)->currentDepth = (d))

#define IS_IIDSTACK_EMPTY(s) ((s)->currentDepth == 0)
#define DEPTH_OF_IIDSTACK(s) ((s)->currentDepth)
#define GET_MDM_OBJECT_ID(obj)  (*((MdmObjectId *) (obj)))

#define IS_PARAM_WRITABLE(n) ((n)->flags & PRN_WRITABLE)

/** Test whether there is a paramName in MdmPathDescriptor */
#define IS_PARAM_NAME_PRESENT(p) ((p)->paramName[0] != 0)

#define _INIT_PATH_DESCRIPTOR(p) (memset((void *)(p), 0, sizeof(_MdmPathDescriptor)))


/* ---- Variable Externs -------------------------------------------------- */


/* ---- Function Prototypes ----------------------------------------------- */

/* From mdmlite_core.c. */
_MdmObjectNode *mdmlite_copyNodePointer(_MdmObjectNode *objNodeP);
CmsRet mdmlite_getObjectPtr(MdmObjectId oid, const InstanceIdStack *iidStack,
  void **mdmObj);
CmsRet mdmlite_getParamVal(const void *mdmObj, _MdmParamNode *paramNode,
  void **val);
UINT32 mdmlite_getNextInstanceId(_InstanceHeadNode *instHead);
CmsRet _deleteMdmObjects(_MdmObjectNode *objNode, UINT32 depth,
  InstanceIdStack *iidStack, _MdmOperationCallback cbFunc,
  void *cbContext);
CmsRet _mdm_getNextObjectInSubTree(MdmObjectId oid,
  const InstanceIdStack *parentIidStack, InstanceIdStack *iidStack,
  void **mdmObj);
CmsRet mdmlite_setParamNode(void *mdmObj, _MdmParamNode *paramNode, const void
  *paramVal);
_InstanceHeadNode *mdmlite_getInstanceHead(const _MdmObjectNode *objNode,
  const InstanceIdStack *iidStack);
_InstanceDescNode *mdmlite_getInstDescById(const _InstanceHeadNode *instHead,
  UINT32 id);
const char *mdmlite_oidToGenericPath(MdmObjectId oid);
char *mdmlite_dumpIidStackDetail(const InstanceIdStack *iidStack);
void *mdmlite_dupObject(const void *mdmObj);
_InstanceDescNode *mdmlite_getInstanceDesc(const _InstanceHeadNode *instHead,
  UINT32 instanceId);
_InstanceDescNode *mdmlite_getInstanceDescFromObjNode(const _MdmObjectNode *objNode,
  const InstanceIdStack *iidStack);
CmsRet mdmlite_createSubTree(_MdmObjectNode *objNode, UINT32 depth,
  InstanceIdStack *iidStack, _MdmOperationCallback cbFunc, void *cbContext);
CmsRet mdmlite_deleteObjectInstance(_MdmPathDescriptor *pathDesc,
  _MdmOperationCallback cbFunc, void *cbContext);

/* From mdmlite_print.c. */
CmsRet mdmlite_serializeObjectToBuf(const MdmObjectId oid, char *buf,
  UINT32 *len);
void mdmlite_dumpObject(MdmObjectId oid);

/* From mdmlite_omci.c. */
_MdmObjectNode *mdmlite_getObjectNode(MdmObjectId oid);
void mdmlite_setObjectNode(_MdmObjectNode *objNodeP);
UINT32 mdmlite_getMaxOid(void);
_MdmOidInfoEntry *mdmlite_getOidInfo(MdmObjectId oid __attribute__((unused)));
void mdmlite_getOidInfoPtrs(const _MdmOidInfoEntry **begin,
  const _MdmOidInfoEntry **end);

/* From omci_oidInfoArray.c. */
void mdm_getPtrs_oidInfoArray_omci(const _MdmOidInfoEntry **begin,
  const _MdmOidInfoEntry **end);


#endif /* MDMLITE_LOCAL_H */
