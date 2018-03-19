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
*      "MDM-lite" database print functions.
*
*****************************************************************************/


/* ---- Include Files ----------------------------------------------------- */

#include "mdmlite_api.h"
#include "mdmlite_glue.h"
#include "mdmlite_local.h"


/* ---- Private Constants and Types --------------------------------------- */

/* Switch to simple output format to reduce the output buffer size. */
#define OUTPUT_FOMRAT_XML 1

#define DATABASE_TOP_NODE            "database"
#define DATABASE_ATTR_INSTANCE       "instance"
#define DATABASE_ATTR_NEXT_INSTANCE  "nextInstance"
#define DATABASE_DISPLAY_INDENT      2
#define OBJECT_PRINTBUF_MAX          0x80000


/* ---- Private Function Prototypes --------------------------------------- */

static void mdmlite_paramPrintCbFunc(_MdmParamNode *paramNode, void *currVal,
  void *cbContext);
static CmsRet mdmlite_traverseParamNodes(_MdmObjectNode *objNodeP,
  InstanceIdStack *iidStack, _MdmParamTraverseCallback cbFunc,
  void *cbContext);
CmsRet mdmlite_dumpObjectNode(_MdmObjectNode *objNode, InstanceIdStack
  *iidStack, char *indentBuf, char *buf, UINT32 *idx, UINT32 max);
void mdmlite_dumpParams(_MdmObjectNode *objNode, InstanceIdStack *iidStack,
  char *indentBuf, char *buf, UINT32 *idx, UINT32 max);


/* ---- Public Variables -------------------------------------------------- */


/* ---- Private Variables ------------------------------------------------- */


/* ---- Functions --------------------------------------------------------- */

/*****************************************************************************
*  FUNCTION:  mdmlite_paramPrintCbFunc
*  PURPOSE:   Convert the parameter value into string.
*  PARAMETERS:
*      paramNode - parameter node.
*      currVal - parameter value.
*      cbContext - callback context.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
static void mdmlite_paramPrintCbFunc(_MdmParamNode *paramNode, void *currVal,
  void *cbContext)
{
    _paramsContext *ctx = (_paramsContext*)cbContext;
    char valBuf[BUFLEN_32];
    char *currStr;

    currStr = valBuf;

    switch (paramNode->type)
    {
        case _MPT_STRING:
        case _MPT_DATE_TIME:
        case _MPT_BASE64:
        case _MPT_HEX_BINARY:
        {
            char **currStrPtr = (char**)currVal;
            currStr = (*currStrPtr);
            break;
        }

        case _MPT_INTEGER:
        {
            SINT32 *ivalPtr = (SINT32*)currVal;
            SINT32 ival = (*ivalPtr);
            sprintf(valBuf, "%d", ival);
            break;
        }

        case _MPT_UNSIGNED_INTEGER:
        {
            UINT32 *uvalPtr = (UINT32*)currVal;
            UINT32 uval = (*uvalPtr);
            sprintf(valBuf, "%u", uval);
            break;
        }

        case _MPT_LONG64:
        {
            SINT64 *ivalPtr = (SINT64*)currVal;
            SINT64 ival = (*ivalPtr);
            sprintf(valBuf, "%lld", ival);
            break;
        }

        case _MPT_UNSIGNED_LONG64:
        {
            UINT64 *uvalPtr = (UINT64*)currVal;
            UINT64 uval = (*uvalPtr);
            sprintf(valBuf, "%llu", uval);
            break;
        }

        case _MPT_BOOLEAN:
        {
           UBOOL8 *bvalPtr = (UBOOL8*)currVal;
           UBOOL8 bval = (*bvalPtr);

           sprintf(valBuf, "%s", (bval ? "TRUE" : "FALSE"));
           break;
        }
    } /* End of switch(paramNode->type) */

    if (ctx->buf != NULL)
    {
        UINT32 wrote;

#if defined (OUTPUT_FOMRAT_XML)
        wrote = snprintf(&(ctx->buf[ctx->idx]), ctx->max - ctx->idx, "%s<%s",
          ctx->indentBuf, paramNode->name);
#else
        wrote = snprintf(&(ctx->buf[ctx->idx]), ctx->max - ctx->idx, "%s%s: ",
          ctx->indentBuf, paramNode->name);
#endif /* OUTPUT_FOMRAT_XML */
        ctx->idx += (wrote < ctx->max - ctx->idx) ? wrote : ctx->max - ctx->idx;

#if defined (OUTPUT_FOMRAT_XML)
        wrote = snprintf(&(ctx->buf[ctx->idx]), ctx->max - ctx->idx, ">%s</%s>\n",
          currStr, paramNode->name);
#else
        wrote = snprintf(&(ctx->buf[ctx->idx]), ctx->max - ctx->idx, "%s\n",
          currStr);
#endif /* OUTPUT_FOMRAT_XML */

        ctx->idx += (wrote < ctx->max - ctx->idx) ? wrote : ctx->max - ctx->idx;
    }

    ctx->paramIndex++;
}

/*****************************************************************************
*  FUNCTION:  mdmlite_traverseParamNodes
*  PURPOSE:   Traverse and print parameter nodes.
*  PARAMETERS:
*      objNode - parameter node.
*      iidStack - node instance.
*      cbFunc - callback function.
*      cbContext - callback context.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
static CmsRet mdmlite_traverseParamNodes(_MdmObjectNode *objNodeP,
  InstanceIdStack *iidStack, _MdmParamTraverseCallback cbFunc,
  void *cbContext)
{
    UINT32 i;
    void *mdmObj = NULL;
    _MdmParamNode *paramNode;
    CmsRet ret = CMSRET_SUCCESS;
    void *val;

    ret = mdmlite_getObjectPtr(objNodeP->oid, iidStack, &mdmObj);
    if (ret != CMSRET_SUCCESS)
    {
        mdmlite_error("mdmlite_getObjectNode() failed, oid=%d", objNodeP->oid);
        return ret;
    }

    for (i = 0; i < objNodeP->numParamNodes; i++)
    {
        paramNode = &(objNodeP->params[i]);
        mdmlite_debug("i=%d, type=%d, offset=%d\n",
          i, paramNode->type, paramNode->offsetInObject);

        ret = mdmlite_getParamVal(mdmObj, paramNode, &val);
        if ((ret == CMSRET_SUCCESS) && (cbFunc != NULL))
        {
            cbFunc(paramNode, val, cbContext);
        }
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION: mdmlite_dumpObjectNode
*  PURPOSE:  Dump the contents of the specified objNode to buf, and also
*            recurse to child object nodes.
*  PARAMETERS:
*      objNode - object node.
*      iidStack - node instance.
*      indentBuf - indentation buffer.
*      buf - print buffer.
*      idx - print buffer offset.
*      max - print buffer maximum size.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet mdmlite_dumpObjectNode(_MdmObjectNode *objNode, InstanceIdStack
  *iidStack, char *indentBuf, char *buf, UINT32 *idx, UINT32 max)
{
    CmsRet ret = CMSRET_SUCCESS;

    if ((IS_INDIRECT0(objNode)) ||
      (IS_INDIRECT2(objNode) &&
      (DEPTH_OF_IIDSTACK(iidStack) == objNode->instanceDepth)))
    {
        UINT32 i, wrote;
        char *indentBuf2;
        UINT32 indentBuf2Len;

        indentBuf2Len = strlen(indentBuf) + DATABASE_DISPLAY_INDENT + 1;
        indentBuf2 = mdmlite_alloc(indentBuf2Len);

        if (indentBuf2 != NULL)
        {
           memset(indentBuf2, 0x20, indentBuf2Len - 1); /* fill with spaces */

           if (IS_INDIRECT2(objNode))
           {
               UINT32 topInstanceId = PEEK_INSTANCE_ID(iidStack);
#if defined (OUTPUT_FOMRAT_XML)
               wrote = snprintf(&buf[*idx], max - (*idx), "%s<%s %s=\"%u\"",
#else
               wrote = snprintf(&buf[*idx], max - (*idx), "%s%s %s=\"%u\"",
#endif /* OUTPUT_FOMRAT_XML */
                 indentBuf2, objNode->name, DATABASE_ATTR_INSTANCE,
                 topInstanceId);
           }
           else
           {
#if defined (OUTPUT_FOMRAT_XML)
               wrote = snprintf(&buf[*idx], max - (*idx), "%s<%s",
                 indentBuf2, objNode->name);
#else
               wrote = snprintf(&buf[*idx], max - (*idx), "%s%s",
                 indentBuf2, objNode->name);
#endif /* OUTPUT_FOMRAT_XML */
           }

           *idx += (wrote < max - (*idx)) ? wrote : max - (*idx);

#if defined (OUTPUT_FOMRAT_XML)
           wrote = snprintf(&buf[*idx], max - (*idx), ">\n");
#else
           wrote = snprintf(&buf[*idx], max - (*idx), " {\n");
#endif /* OUTPUT_FOMRAT_XML */
           *idx += (wrote < max - (*idx)) ? wrote : max - (*idx);

           /* Go write out the params. */
           mdmlite_dumpParams(objNode, iidStack, indentBuf2, buf, idx, max);

           for (i = 0; i < objNode->numChildObjNodes && ((*idx) < max); i++)
           {
               mdmlite_dumpObjectNode(&(objNode->childObjNodes[i]), iidStack,
                 indentBuf2, buf, idx, max);
           }

#if defined (OUTPUT_FOMRAT_XML)
           wrote = snprintf(&buf[*idx], max - (*idx), "%s</%s>\n",
             indentBuf2, objNode->name);
#else
           wrote = snprintf(&buf[*idx], max - (*idx), "%s}\n", indentBuf2);
#endif /* OUTPUT_FOMRAT_XML */
           *idx += (wrote < max - (*idx)) ? wrote : max - (*idx);

           CMSMEM_FREE_BUF_AND_NULL_PTR(indentBuf2);
        }
    }
    else if (IS_INDIRECT2(objNode) &&
      (DEPTH_OF_IIDSTACK(iidStack) + 1 == objNode->instanceDepth))
    {
        InstanceIdStack childIidStack;
        _InstanceHeadNode *instHead = NULL;
        _InstanceDescNode *instDesc = NULL;
        UINT32 wrote;

        instHead = mdmlite_getInstanceHead(objNode, iidStack);
        if (instHead != NULL)
        {
            instDesc = (_InstanceDescNode *) instHead->objData;
            while ((instDesc != NULL) && ((*idx) < max))
            {
                /* Recursively dump each instance of this object. */
                childIidStack = *iidStack;
                PUSH_INSTANCE_ID(&childIidStack, instDesc->instanceId);
                mdmlite_dumpObjectNode(objNode, &childIidStack, indentBuf,
                  buf, idx, max);

                instDesc = instDesc->next;
            }

            if (instHead->nextInstanceIdToAssign != 1)
            {
#if defined (OUTPUT_FOMRAT_XML)
                wrote = snprintf(&buf[*idx], max - (*idx),
                  "  %s<%s %s=\"%u\" ></%s>\n",
                  indentBuf, objNode->name, DATABASE_ATTR_NEXT_INSTANCE,
                  instHead->nextInstanceIdToAssign, objNode->name);
#else
                wrote = snprintf(&buf[*idx], max - (*idx),
                  "  %s%s %s=\"%u\"\n",
                  indentBuf, objNode->name, DATABASE_ATTR_NEXT_INSTANCE,
                  instHead->nextInstanceIdToAssign);
#endif /* OUTPUT_FOMRAT_XML */
                *idx += (wrote < max - (*idx)) ? wrote : max - (*idx);
            }
        }
    }
    else
    {
        mdmlite_error("could not determine action, objNode %s "
          "depth=%d iidStack=%s",
          mdmlite_oidToGenericPath(objNode->oid),
          objNode->instanceDepth,
          mdmlite_dumpIidStack(iidStack));
        ret = CMSRET_INTERNAL_ERROR;
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  mdmlite_serializeObjectToBuf
*  PURPOSE:   Traverse and print object nodes starting from the root oid
*             into a buffer.
*  PARAMETERS:
       oid - root oid.
       buf - user allocated print buffer.
       len - print buffer size.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet mdmlite_serializeObjectToBuf(const MdmObjectId oid, char *buf,
  UINT32 *len)
{
    UINT32 wrote;
    UINT32 idx = 0;
    UINT32 max = *len;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    _MdmObjectNode *objNode = NULL;
    char indentBuf[1] = {0};  /* initial indent is no spaces/empty string */
    CmsRet ret=CMSRET_SUCCESS;

    objNode = mdmlite_getObjectNode(oid);
    if (objNode == NULL)
    {
        mdmlite_error("Invalid oid=%d", oid);
        return CMSRET_RESOURCE_EXCEEDED;
    }

    wrote = snprintf(buf, max, "Dump %s in database\n", objNode->name);

    if (wrote < max)
    {
        max -= wrote;
        idx += wrote;

        ret = mdmlite_dumpObjectNode(objNode, &iidStack, indentBuf, buf,
          &idx, max);

        if (idx < max)
        {
#if defined (OUTPUT_FOMRAT_XML)
            wrote = snprintf(&(buf[idx]), max - idx, "</%s>\n",
              DATABASE_TOP_NODE);
#else
            wrote = snprintf(&(buf[idx]), max - idx, "%s\n",
              DATABASE_TOP_NODE);
#endif /* OUTPUT_FOMRAT_XML */
            idx += (wrote < max - idx) ? wrote : max - idx;
        }
    }

    if (ret != CMSRET_SUCCESS)
    {
        *len = 0;
        return ret;
    }
    else if (idx >= max)
    {
        mdmlite_error("object information could not fit in %d bytes", (*len));
        return CMSRET_RESOURCE_EXCEEDED;
    }
    else
    {
        *len = idx + 1; /* plus 1 to include last NULL char */
        return CMSRET_SUCCESS;
    }
}

/*****************************************************************************
*  FUNCTION:  mdmlite_dumpParams
*  PURPOSE:   Write out the object parameters.
*  PARAMETERS:
*      objNode - object node.
*      iidStack - node instance.
*      indentBuf - indentation buffer.
*      buf - print buffer.
*      idx - print buffer offset.
*      max - print buffer maximum size.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void mdmlite_dumpParams(_MdmObjectNode *objNode, InstanceIdStack *iidStack,
  char *indentBuf, char *buf, UINT32 *idx, UINT32 max)
{
    _paramsContext ctx;
    char *indentBuf2;
    UINT32 indentBuf2Len;
    CmsRet ret = CMSRET_SUCCESS;

    indentBuf2Len = strlen(indentBuf) + DATABASE_DISPLAY_INDENT + 1;
    indentBuf2 = mdmlite_alloc(indentBuf2Len);

    if (indentBuf2 != NULL)
    {
        /* fill with spaces */
        memset(indentBuf2, 0x20, indentBuf2Len - 1);

        ctx.buf = buf;
        ctx.indentBuf = indentBuf2;
        ctx.idx = *idx;
        ctx.max = max;
        ctx.paramIndex = 0;

        ret = mdmlite_traverseParamNodes(objNode, iidStack,
          mdmlite_paramPrintCbFunc, &ctx);
        if (ret != CMSRET_SUCCESS)
        {
            mdmlite_error("mdm_traverseParamNodes failed, ret=%d", ret);
        }

        *idx = ctx.idx;

        CMSMEM_FREE_BUF_AND_NULL_PTR(indentBuf2);
    }
}

/*****************************************************************************
*  FUNCTION:  mdmlite_dumpObject
*  PURPOSE:   Print object node based on oid.
*  PARAMETERS:
       oid - root oid.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void mdmlite_dumpObject(MdmObjectId oid)
{
    char *printBuf;
    UINT32 len = OBJECT_PRINTBUF_MAX;

    printBuf = mdmlite_alloc(OBJECT_PRINTBUF_MAX);
    if (printBuf == NULL)
    {
        mdmlite_error("mdmlite_alloc(%d) failed", OBJECT_PRINTBUF_MAX);
        return;
    }

    mdmlite_serializeObjectToBuf(oid, printBuf, &len);
    printf("%s\n", printBuf);
    mdmlite_free(printBuf);
}
