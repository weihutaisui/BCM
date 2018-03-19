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
*      MDM-lite related definitions from MDM.
*
*****************************************************************************/

#ifndef _MDMLITE_DEFS_H_
#define _MDMLITE_DEFS_H_

/* ---- Include Files ----------------------------------------------------- */

#include "os_defs.h"
#include "omcimib_dts.h"


/* ---- Constants and Types ----------------------------------------------- */

/* Schema definitions. */

#define PRN_WRITABLE                    0x0001
#define PRN_ALWAYS_WRITE_TO_CONFIG_FILE 0x0008

#define OGF_NORMAL_UPDATE               0x0000
#define OGF_DEFAULT_VALUES              0x0001
#define OGF_NO_VALUE_UPDATE             0x0002

#define OBN_INSTANCE_NODE               0x01
#define OBN_DYNAMIC_INSTANCES           0x02
#define OBN_PRUNE_WRITE_TO_CONFIG_FILE  0x04

#define MAX_MDM_PARAM_NAME_LENGTH       55

#define EMPTY_INSTANCE_ID_STACK         {0, {0,0,0,0}}

/* Same as ValidatorData. */
typedef struct
{
   void *min;
   void *max;
} _ValidatorData;

/* Same as MdmPathDescriptor. */
typedef struct
{
    MdmObjectId oid;
    InstanceIdStack iidStack;
    char paramName[MAX_MDM_PARAM_NAME_LENGTH + 1];
} _MdmPathDescriptor;

/* Same as MdmParamNode. Used by the schema file. Do not change. */
typedef struct
{
    const char *name;
    struct _MdmObjectNode *parent;
    _MdmParamTypes type;
    UINT16 flags;
    UINT16 offsetInObject;
    char *defaultValue;
    char *suggestedValue;
    _ValidatorData vData;
} _MdmParamNode;

/* Same as MdmNodeAttributes. Used by the schema file. Do not change. */
typedef struct
{
    UINT16 accessBitMaskChange:1;
    UINT16 accessBitMask:15;
    UINT8 notificationChange:1;
    UINT8 notification:7;
    UINT8 valueChanged:1;
    UINT8 reserved:7;
} _MdmNodeAttributes;

/* Same as AttributesDescNode. */
typedef struct _AttributesDescNode
{
    UINT32 paramNum;
    struct _AttributesDescNode *next;
    _MdmNodeAttributes nodeAttr;
} _AttributesDescNode;

/* Same as MdmObjectNode. Used by the schema file. Do not change. */
typedef struct _MdmObjectNode
{
    MdmObjectId oid;
    char *name;
    UINT8 flags;
    UINT8 instanceDepth;
    struct _MdmObjectNode *parent;
    _MdmNodeAttributes nodeAttr;
    _AttributesDescNode *attrDesc;
    UINT16 numParamNodes;
    UINT16 numChildObjNodes;
    _MdmParamNode *params;
    struct _MdmObjectNode *childObjNodes;
    void *objData;
} _MdmObjectNode;

/* Same as MdmObjParamInfo. Used by the schema file. Do not change. */
typedef struct
{
    UINT32 totalParams;
    char name[MAX_MDM_PARAM_NAME_LENGTH + 1];
    _MdmParamTypes type;
    UINT32 minVal;
    UINT32 maxVal;
    void *val;
} _MdmObjParamInfo;

/* Same as MdmOidInfoEntry. */
typedef struct
{
    MdmObjectId oid;
    const char *fullPath;
    CmsRet (*rclHandlerFunc)();
    CmsRet (*stlHandlerFunc)();
} _MdmOidInfoEntry;

typedef struct
{
    UINT32 eid;
    UBOOL8 hideObjectsPendingDelete;
    void *msgHandle;
}_MdmLibraryContext;

typedef struct
{
   char           paramName[MAX_MDM_PARAM_NAME_LENGTH + 1];
   _MdmParamTypes type;
   UINT16         offsetInObject; /* size */
   UBOOL8         writable;
} _MdmParamInfo;

/* Callback function to be used in mdmlite_traverseOidNodes(). */
typedef void (*_MdmNodeTraverseCallback)(_MdmObjectNode *objNode,
  InstanceIdStack *iidStack, void *cbContext);


/* ---- Macro API definitions --------------------------------------------- */

#define PUSH_INSTANCE_ID(s, id) \
   do {if ((s)->currentDepth < MAX_MDM_INSTANCE_DEPTH ) { \
         (s)->instance[(s)->currentDepth] = (id);    \
         (s)->currentDepth++; }                      \
   } while(0)

#define POP_INSTANCE_ID(s) \
   ((s)->currentDepth > 0) ? (s)->instance[(((s)->currentDepth)--) - 1] : 0
#define INIT_INSTANCE_ID_STACK(s)  (memset((void *)(s), 0, sizeof(InstanceIdStack)))
#define PEEK_INSTANCE_ID(s) \
   ((s)->currentDepth > 0) ? (s)->instance[(s)->currentDepth - 1] : 0


/* ---- Variable Externs -------------------------------------------------- */


/* ---- Function Prototypes ----------------------------------------------- */


#endif /* _MDMLITE_DEFS_H_ */
