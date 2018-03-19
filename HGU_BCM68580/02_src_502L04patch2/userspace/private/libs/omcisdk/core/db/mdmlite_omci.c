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
*      OMCI application related mdm-lite helper functions.
*
*****************************************************************************/


/* ---- Include Files ----------------------------------------------------- */

#include "owsvc_api.h"

#include "mdmlite_api.h"
#include "mdmlite_glue.h"
#include "mdmlite_local.h"
#include "omciobj_defs.h"


/* ---- Private Constants and Types --------------------------------------- */

#define OMCIMIB_PRINTBUF_MAX 512000

typedef struct
{
    UBOOL8 initialized;
    UBOOL8 locked;
    pthread_t self;
    int count;
    pthread_mutex_t m;
} omciMibMutex_t;

typedef struct
{
    UINT32 entryNum;
    OmciObjectInfo_t *curPtr;
} omciMibDumpInfo;


/* ---- Macros ------------------------------------------------------------ */

#define omcimib_assert(expr) \
  omcimib_assertFunc(__FILE__, __LINE__, __STRING(expr), (expr))

/* #define OMCIMIB_DEBUG 1 */
#ifdef OMCIMIB_DEBUG
#define omcimib_print printf
#else
#define omcimib_print(argu, arg1...)
#endif  /* OMCIMIB_DEBUG */


/* ---- Private Function Prototypes --------------------------------------- */

static void omcimib_assertFunc(const char *filename, UINT32 lineNumber,
  const char *exprString, SINT32 expr);
static void omcimib_initLock(void);
static void omcimib_deleteLock(void);
static void omcimib_lock(void);
static void omcimib_unlock(void);
static void omcimib_oidInfoInit(void);
static CmsRet omcimib_mdmInit(void);
static void omcimib_supportedObjectNodeCbFunc(_MdmObjectNode *objNodeP,
  InstanceIdStack *iidStack __attribute__((unused)), void *cbContext);
static void omcimib_objNumCbFunc(_MdmObjectNode *objNodeP, InstanceIdStack
  *iidStack __attribute__((unused)), void *cbContext);
static void omcimib_allObjectCbFunc(_MdmObjectNode *objNodeP,
  InstanceIdStack *iidStack, void *cbContext);


/* ---- Public Variables -------------------------------------------------- */

extern int cnt_oidInfoArray_omci;
/* OID info flat array from the schema. */
extern _MdmOidInfoEntry oidInfoArray_omci[];
/* Adjusted OID info array with the OID as index. */
extern _MdmOidInfoEntry oidInfoArray_omciFull[];
/* OMCI data model root object node. */
extern _MdmObjectNode ituTOrgChildObjArray;


/* ---- Private Variables ------------------------------------------------- */

_MdmObjectNode *OidNodePtrTable[OMCI_MAX_OID + 1];
static omciMibMutex_t OmciMibMutex;


/* ---- Functions --------------------------------------------------------- */

/*****************************************************************************
*  FUNCTION:  omcimib_assertFunc
*  PURPOSE:   Assert function.
*  PARAMETERS:
*      filename - file name of assertion location.
*      lineNumber - line number of assertion location.
*      exprString - expression string.
*      expr - expression.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
static void omcimib_assertFunc(const char *filename, UINT32 lineNumber,
  const char *exprString, SINT32 expr)
{
    pid_t pid;
    int rc;

    if (expr == 0)
    {
        mdmlite_error("Assertion \"%s\" failed at %s:%d",
          exprString, filename, lineNumber);

        pid = getpid();
        if ((rc = kill(pid, SIGABRT)) < 0)
        {
            cmsLog_error("invalid pid(%d)", pid);
        }
    }
}

/*****************************************************************************
*  FUNCTION:  omcimib_initLock
*  PURPOSE:   Create OMCI MIB lock.
*  PARAMETERS:
*      None.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
static void omcimib_initLock(void)
{
    OmciMibMutex.self = 0;
    OmciMibMutex.count = 0;
    pthread_mutex_init(&OmciMibMutex.m, NULL);
    OmciMibMutex.initialized = 1;
    OmciMibMutex.locked = TRUE;
}

/*****************************************************************************
*  FUNCTION:  omcimib_deleteLock
*  PURPOSE:   Delete OMCI MIB lock.
*  PARAMETERS:
*      None.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
static void omcimib_deleteLock(void)
{
    pthread_mutex_destroy(&OmciMibMutex.m);
}

/*****************************************************************************
*  FUNCTION:  omcimib_lock
*  PURPOSE:   Get the OMCI MIB lock for access.
*  PARAMETERS:
*      None.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
static void omcimib_lock(void)
{
    pthread_mutex_lock(&OmciMibMutex.m);
    OmciMibMutex.self = pthread_self();
    OmciMibMutex.count = 1;
    OmciMibMutex.locked = TRUE;
}

/*****************************************************************************
*  FUNCTION:  omcimib_unlock
*  PURPOSE:   Release the OMCI MIB lock for access.
*  PARAMETERS:
*      None.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
static void omcimib_unlock(void)
{
    omcimib_assert(OmciMibMutex.self == pthread_self());
    omcimib_assert(OmciMibMutex.locked == TRUE);
    OmciMibMutex.self = 0;
    pthread_mutex_unlock(&OmciMibMutex.m);
    OmciMibMutex.locked = FALSE;
}

/*****************************************************************************
*  FUNCTION:  omcimib_oidInfoInit
*  PURPOSE:   OMCI MIB schema (not the actual instances) initialization.
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
static void omcimib_oidInfoInit(void)
{
    UINT32 i;

    for (i = 0; i < (UINT32)cnt_oidInfoArray_omci; i++)
    {
        mdmlite_debug("oid=%d\n", oidInfoArray_omci[i].oid);
        memcpy(&oidInfoArray_omciFull[oidInfoArray_omci[i].oid],
          &oidInfoArray_omci[i], sizeof(_MdmOidInfoEntry));
    }
}

/*****************************************************************************
*  FUNCTION:  omcimib_mdmInit
*  PURPOSE:   OMCI MIB schema (not the actual instances) initialization.
*  PARAMETERS:
*      None.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
static CmsRet omcimib_mdmInit(void)
{
    CmsRet ret=CMSRET_SUCCESS;
    InstanceIdStack iidStack;

    omcimib_initLock();

    omcimib_oidInfoInit();
    mdmlite_copyNodePointer((_MdmObjectNode*)&ituTOrgChildObjArray);

    INIT_INSTANCE_ID_STACK(&iidStack);
    ret = mdmlite_createSubTree(&ituTOrgChildObjArray, 0, &iidStack,
      NULL, NULL);

    return ret;
}

/*****************************************************************************
*  FUNCTION:  _cmsLck_acquireLockWithTimeout
*  PURPOSE:   Shim layer function for the legacy code.
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet _cmsLck_acquireLockWithTimeout(UINT32 timeoutMilliSeconds
  __attribute__((unused)))
{
    omcimib_lock();
    return CMSRET_SUCCESS;
}

/*****************************************************************************
*  FUNCTION:  _cmsLck_releaseLock
*  PURPOSE:   Shim layer function for the legacy code.
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet _cmsLck_releaseLock(void)
{
    omcimib_unlock();
    return CMSRET_SUCCESS;
}

/*****************************************************************************
*  FUNCTION:  omcimib_dumpAll
*  PURPOSE:   Dump the entire OMCI MIB.
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void omcimib_dumpAll(void)
{
    char *printBuf;
    UINT32 len = OMCIMIB_PRINTBUF_MAX;

    printBuf = mdmlite_alloc(OMCIMIB_PRINTBUF_MAX);
    if (printBuf == NULL)
    {
        mdmlite_error("mdmlite_alloc(%d) failed", OMCIMIB_PRINTBUF_MAX);
        return;
    }

    mdmlite_serializeObjectToBuf(MDMOID_G_988, printBuf, &len);
    printf("%s\n", printBuf);
    mdmlite_free(printBuf);
}

/*****************************************************************************
*  FUNCTION:  omcimib_dumpObjectId
*  PURPOSE:   Dump a mdmObj based on the specified oid.
*  PARAMETERS:
*      oid - object id.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void omcimib_dumpObjectId(MdmObjectId oid)
{
    mdmlite_dumpObject(oid);
}

/*****************************************************************************
*  FUNCTION:  omcimib_dumpObject
*  PURPOSE:   Dump a mdmObj.
*  PARAMETERS:
*      mdmObj - mdm object pointer.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void omcimib_dumpObject(const void *mdmObj)
{
    MdmObjectId oid;

    oid =(unsigned int)(*((MdmObjectId*)mdmObj));
    mdmlite_dumpObject(oid);
}

/*****************************************************************************
*  FUNCTION:  omcimib_init
*  PURPOSE:   OMCI MIB init function.
*  PARAMETERS:
*      None.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet omcimib_init(void)
{
    omcimib_print("%s()\n", __FUNCTION__);
    return omcimib_mdmInit();
}

/*****************************************************************************
*  FUNCTION:  omcmib_cleanup
*  PURPOSE:   OMCI MIB exit function.
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void omcmib_cleanup(void)
{
    omcimib_print("%s()\n", __FUNCTION__);
    omcimib_deleteLock();
}

/*****************************************************************************
*  FUNCTION:  mdmlite_getMaxOid
*  PURPOSE:   Get the maximum oid value of OMCI MIB.
*  PARAMETERS:
*      None.
*  RETURNS:
*      The maximum oid value.
*  NOTES:
*      None.
*****************************************************************************/
UINT32 mdmlite_getMaxOid(void)
{
    return OMCI_MAX_OID;
}

/*****************************************************************************
*  FUNCTION:  mdmlite_getObjectNode
*  PURPOSE:   Get the OidNodePtrTable[] entry of the specified oid.
*  PARAMETERS:
*      oid - object id.
*  RETURNS:
*      _MdmObjectNode.
*  NOTES:
*      None.
*****************************************************************************/
_MdmObjectNode *mdmlite_getObjectNode(MdmObjectId oid)
{
    if (oid > mdmlite_getMaxOid())
    {
        mdmlite_error("Invalid oid %d (max=%d)", oid, mdmlite_getMaxOid());
        return NULL;
    }

    return (OidNodePtrTable[oid]);
}

/*****************************************************************************
*  FUNCTION:  mdmlite_setObjectNode
*  PURPOSE:   Initialize OidNodePtrTable[] with objNode pointer.
*  PARAMETERS:
*      objNodeP - pointer to objNode from the schema table.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void mdmlite_setObjectNode(_MdmObjectNode *objNodeP)
{
    if (objNodeP->oid > mdmlite_getMaxOid())
    {
        mdmlite_error("Invalid oid %d (max=%d)", objNodeP->oid,
          mdmlite_getMaxOid());
        return;
    }

    OidNodePtrTable[objNodeP->oid] = objNodeP;
}

/*****************************************************************************
*  FUNCTION:  mdmlite_traverseOidNodes
*  PURPOSE:   Traverse the object node table OidNodePtrTable[].
*  PARAMETERS:
*      cbFunc - callback function.
*      cbContext - callback context.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void mdmlite_traverseOidNodes(_MdmNodeTraverseCallback cbFunc, void *cbContext)
{
    _MdmObjectNode *objNodeP;
    UINT32 i;

    for (i = 0; i <= mdmlite_getMaxOid(); i++)
    {
        if (OidNodePtrTable[i] != NULL)
        {
            objNodeP = OidNodePtrTable[i];
            if (cbFunc != NULL)
            {
                cbFunc(objNodeP, NULL, cbContext);
            }
        }
    }
}

/*****************************************************************************
*  FUNCTION:  mdmlite_getOidInfo
*  PURPOSE:   Get the oid info entry of the specificed oid.
*  PARAMETERS:
*      oid - object id.
*  RETURNS:
*      _MdmOidInfoEntry.
*  NOTES:
*      None.
*****************************************************************************/
_MdmOidInfoEntry *mdmlite_getOidInfo(MdmObjectId oid)
{
    if (oid <= OMCI_MAX_OID)
    {
        return &oidInfoArray_omciFull[oid];
    }
    else
    {
        return (_MdmOidInfoEntry*)NULL;
    }
}

/*****************************************************************************
*  FUNCTION:  mdmlite_getOidInfoPtrs
*  PURPOSE:   Get the starting and ending points of _MdmOidInfoEntry.
*  PARAMETERS:
*      begin - starting pointer.
*      end - ending pointer.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void mdmlite_getOidInfoPtrs(const _MdmOidInfoEntry **begin,
  const _MdmOidInfoEntry **end)
{
    return (mdm_getPtrs_oidInfoArray_omci(begin, end));
}

/*****************************************************************************
*  FUNCTION:  omcimib_supportedObjectNodeCbFunc
*  PURPOSE:   Callback function to get supported OMCI objects.
*  PARAMETERS:
*      objNodeP - pointer to objNode from the schema table.
*      iidStack - instance stack.
*      cbContext - callback context.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
static void omcimib_supportedObjectNodeCbFunc(_MdmObjectNode *objNodeP,
  InstanceIdStack *iidStack __attribute__((unused)), void *cbContext)
{
    OmciObjectInfo_t *objInfoP;
    omciMibDumpInfo *infoP = (omciMibDumpInfo*)cbContext;

    objInfoP = infoP->curPtr;
    objInfoP->oid = objNodeP->oid;
    objInfoP->iidStack.currentDepth = 0;
    objInfoP->iidStack.instance[0] = 0;
    infoP->entryNum++;
    infoP->curPtr++;
}

/*****************************************************************************
*  FUNCTION:  omcimib_objNumCbFunc
*  PURPOSE:   Callback function to get total number of OMCI objects.
*             Place holders (equipment management, ani management, etc.) are
*             excluded.
*  PARAMETERS:
*      objNodeP - pointer to objNode from the schema table.
*      iidStack - instance stack.
*      cbContext - callback context.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
static void omcimib_objNumCbFunc(_MdmObjectNode *objNodeP, InstanceIdStack
  *iidStack __attribute__((unused)), void *cbContext)
{
    omciMibDumpInfo *infoP = (omciMibDumpInfo*)cbContext;

    /* Skip place holder objects. */
    if (objNodeP->numParamNodes != 0)
    {
        infoP->entryNum++;
    }
}

/*****************************************************************************
*  FUNCTION:  omcimib_allObjectCbFunc
*  PURPOSE:   Callback function to get an OMCI object (place holder excluded).
*  PARAMETERS:
*      objNodeP - pointer to objNode from the schema table.
*      iidStack - instance stack.
*      cbContext - callback context.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
static void omcimib_allObjectCbFunc(_MdmObjectNode *objNodeP,
  InstanceIdStack *iidStack, void *cbContext)
{
    OmciObjectInfo_t *objInfoP;
    omciMibDumpInfo *infoP = (omciMibDumpInfo*)cbContext;

    if (objNodeP->numParamNodes != 0)
    {
        objInfoP = infoP->curPtr;
        objInfoP->oid = objNodeP->oid;
        memcpy(&objInfoP->iidStack, iidStack, sizeof(InstanceIdStack));
        omcimib_print("Name:%s, NP:%d, d:%d, "
          "iidstack[curd:%d, %d:%d:%d:%d:%d:%d]\n",
          objNodeP->name, objNodeP->numParamNodes,
          objNodeP->instanceDepth, iidStack->currentDepth,
          iidStack->instance[0], iidStack->instance[1],
          iidStack->instance[2], iidStack->instance[3],
          iidStack->instance[4], iidStack->instance[5]);
        infoP->entryNum++;
        infoP->curPtr++;
    }
}

/*****************************************************************************
*  FUNCTION:  omcimib_getSupportedObjectInfo
*  PURPOSE:   Get supported OMCI objects (not actual ME instances). The
*             object may or may not have any instances.
*  PARAMETERS:
*      objectInfoListPP - pointer to the returned object list.
*      entryNumP - pointer to the number of entries in the object list.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      Caller is responsible for freeing objectInfoListPP.
*****************************************************************************/
CmsRet omcimib_getSupportedObjectInfo(OmciObjectInfo_t **objectInfoListPP,
  SINT32 *entryNumP)
{
    CmsRet ret = CMSRET_SUCCESS;
    OmciObjectInfo_t *objInfoListP;
    UINT32 entryNumMax = OMCI_MAX_OID - MDMOID_G_988;
    omciMibDumpInfo dumpInfo;

    objInfoListP = mdmlite_alloc(sizeof(OmciObjectInfo_t) * entryNumMax);
    if (objInfoListP == NULL)
    {
        mdmlite_error("mdmlite_alloc() failed, entryNum=%d", entryNumMax);
        return CMSRET_INTERNAL_ERROR;
    }

    /* Traverse to obtain the total number of supported objects. */
    dumpInfo.entryNum = 0;
    dumpInfo.curPtr = objInfoListP;
    mdmlite_traverseOidNodes(omcimib_supportedObjectNodeCbFunc, &dumpInfo);

    *objectInfoListPP = objInfoListP;
    *entryNumP = dumpInfo.entryNum;

    return ret;
}

/*****************************************************************************
*  FUNCTION:  omcimib_getObjectInfo
*  PURPOSE:   Get all OMCI object instances (excluding place holders),
*             including the iidStack information.
*  PARAMETERS:
*      objectInfoListPP - pointer to the returned object instance list.
*      entryNumP - pointer to the number of entries in the object instance list.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      Caller is responsible for freeing objectInfoListPP.
*****************************************************************************/
CmsRet omcimib_getObjectInfo(OmciObjectInfo_t **objectInfoListPP,
  SINT32 *entryNumP)
{
    _MdmObjectNode *objNodeP = NULL;
    omciMibDumpInfo dumpInfo;
    OmciObjectInfo_t *objInfoListP;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_SUCCESS;

    objNodeP = mdmlite_getObjectNode(MDMOID_G_988);
    memset(&dumpInfo, 0x0, sizeof(omciMibDumpInfo));

    /* Traverse to obtain the total number of object instances. */
    mdmlite_traverseAllNodes(objNodeP, &iidStack, omcimib_objNumCbFunc,
      &dumpInfo);

    objInfoListP = mdmlite_alloc(sizeof(OmciObjectInfo_t) * dumpInfo.entryNum);
    if (objInfoListP == NULL)
    {
        mdmlite_error("mdmlite_alloc() failed, entryNum=%d", dumpInfo.entryNum);
        return CMSRET_INTERNAL_ERROR;
    }

    /* Traverse again to obtain every object instance. */
    dumpInfo.entryNum = 0;
    dumpInfo.curPtr = objInfoListP;
    mdmlite_traverseAllNodes(objNodeP, &iidStack, omcimib_allObjectCbFunc,
      &dumpInfo);
    omcimib_print("Total object instances = %d\n", dumpInfo.entryNum);

    *objectInfoListPP = objInfoListP;
    *entryNumP = dumpInfo.entryNum;

    return ret;
}

/*****************************************************************************
*  FUNCTION:  omcimib_getParamInfo
*  PURPOSE:   Get parametere information (not actual values) of the specified
*             oid.
*  PARAMETERS:
*      oid - object id.
*      paramInfoListPP - pointer to the returned parameter list.
*      entryNumP - pointer to the number of entries in the parameter list.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      Caller is responsible for freeing paramInfoListPP.
*****************************************************************************/
CmsRet omcimib_getParamInfo(MdmObjectId oid, OmciParamInfo_t
  **paramInfoListPP, SINT32 *entryNumP)
{
    _MdmObjectNode *objNodeP = NULL;
    OmciParamInfo_t *omciPramInfoListP;
    OmciParamInfo_t *omciParamEntryP;
    SINT32 i;

    objNodeP = mdmlite_getObjectNode(oid);
    if (objNodeP == NULL)
    {
        mdmlite_error("Invalid oid %d (max=%d)", oid,
          mdmlite_getMaxOid());
        return CMSRET_INTERNAL_ERROR;
    }

    if (objNodeP->numParamNodes == 0)
    {
        *paramInfoListPP = NULL;
        *entryNumP = 0;
        return CMSRET_SUCCESS;
    }

    omciPramInfoListP = mdmlite_alloc(sizeof(OmciParamInfo_t) *
      objNodeP->numParamNodes);
    if (omciPramInfoListP == NULL)
    {
        mdmlite_error("mdmlite_alloc() failed, entryNum=%d",
          objNodeP->numParamNodes);
        return CMSRET_INTERNAL_ERROR;
    }

    omciParamEntryP = omciPramInfoListP;
    for (i = 0; i < objNodeP->numParamNodes; i++)
    {
        _MdmParamNode *paramNodeP = &(objNodeP->params[i]);

        strcpy(omciParamEntryP->paramName, paramNodeP->name);
        omciParamEntryP->type = paramNodeP->type;
        omciParamEntryP->offsetInObject = paramNodeP->offsetInObject;
        omciParamEntryP->writable = paramNodeP->flags & PRN_WRITABLE;
        omciParamEntryP->minVal = (UINT32)paramNodeP->vData.min;
        omciParamEntryP->maxVal = (UINT32)paramNodeP->vData.max;
        omcimib_print("param {name=%s, type=%d, offset=%d, w=%d, min=%d, max=%d}\n",
          omciParamEntryP->paramName, omciParamEntryP->type,
          omciParamEntryP->offsetInObject, omciParamEntryP->writable,
          omciParamEntryP->minVal, omciParamEntryP->maxVal);
        omciParamEntryP++;
    }

    omcimib_print("Total parameters = %d\n", objNodeP->numParamNodes);

    *paramInfoListPP = omciPramInfoListP;
    *entryNumP = objNodeP->numParamNodes;

    return CMSRET_SUCCESS;
}
