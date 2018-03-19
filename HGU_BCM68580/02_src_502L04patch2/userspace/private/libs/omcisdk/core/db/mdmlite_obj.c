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
*      "MDM-lite" database object functions.
*
*****************************************************************************/


/* ---- Include Files ----------------------------------------------------- */

#include "mdmlite_api.h"
#include "mdmlite_glue.h"
#include "mdmlite_local.h"


/* ---- Private Constants and Types --------------------------------------- */


/* ---- Private Function Prototypes --------------------------------------- */

/* CMS object operation. */
CmsRet mdmlite_addObjectCallBack(_MdmObjectNode *objNode, InstanceIdStack
  *iidStack, void *mdmObj, void *context);
CmsRet mdmlite_delObjectCallBack(_MdmObjectNode *objNode, InstanceIdStack
  *iidStack, void *mdmObj, void *context);
CmsRet mdmlite_setObjectInternal(const void *newMdmObj, const void *currMdmObj,
  const InstanceIdStack *iidStack, UBOOL8 rclCB);
CmsRet _cmsObj_getNthParam(const void *mdmObj, const UINT32 paramNbr,
  _MdmObjParamInfo *paramInfo);
CmsRet _cmsObj_setNthParam(void *mdmObj, const UINT32 paramNbr,
  const void *paramVal);
CmsRet mdmlite_procNewObject(const void *newMdmObj, const InstanceIdStack
  *iidStack, UINT32 actionBits);


/* ---- Public Variables -------------------------------------------------- */


/* ---- Private Variables ------------------------------------------------- */


/* ---- Functions --------------------------------------------------------- */


/*****************************************************************************
*  FUNCTION:  mdmlite_getNextObject
*  PURPOSE:   Get next MdmObject.
*  PARAMETERS:
*      oid - object id.
*      iidStack - instance stack (instance information).
*      mdmObj - mdm object pointer.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet mdmlite_getNextObject(MdmObjectId oid, InstanceIdStack *iidStack,
  void **mdmObj)
{
    InstanceIdStack parentIidStack = EMPTY_INSTANCE_ID_STACK;

    return (_mdm_getNextObjectInSubTree(oid, &parentIidStack, iidStack, mdmObj));
}

/*****************************************************************************
*  FUNCTION:  _cmsObj_free
*  PURPOSE:   Free a MdmObject returned by cmsObj_get and cmsObj_getnext.
*  PARAMETERS:
*      mdmObj - mdm object pointer.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void _cmsObj_free(void **mdmObj)
{
    mdmlite_freeObject(mdmObj);
}

/*****************************************************************************
*  FUNCTION:  _cmsObj_get()
*  PURPOSE:   Get a MdmObject.
*  PARAMETERS:
*      oid - object id.
*      iidStack - instance stack (instance information).
*      getFlags - Additional flags for the get.
*      mdmObj - mdm object pointer.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      The caller must free this object by calling cmsObj_free().
*****************************************************************************/
CmsRet _cmsObj_get(MdmObjectId oid, const InstanceIdStack *iidStack,
  UINT32 getFlags __attribute__((unused)), void **mdmObj)
{
    CmsRet ret;
    void *internalMdmObj = NULL;
    const _MdmOidInfoEntry *oidInfo = NULL;

    if ((oidInfo = mdmlite_getOidInfo(oid)) == NULL)
    {
       mdmlite_error("Could not find OID info for oid %d", oid);
       ret = CMSRET_INTERNAL_ERROR;
    }
    else
    {
        ret = mdmlite_getObject(oid, iidStack, &internalMdmObj);
    }

   if ((ret == CMSRET_SUCCESS) && ((getFlags & OGF_NO_VALUE_UPDATE) == 0))
   {
      {
         if (oidInfo->stlHandlerFunc == NULL)
         {
            mdmlite_error("oid %d has NULL stl handler func", oid);
            ret = CMSRET_INTERNAL_ERROR;
         }
         else
         {
            _MdmObjectNode *objNode=NULL;

            /*
             * Before calling the STL handler, push this mdmObj into the setq.
             * This will handle scenario where STL handler func calls RUT
             * func, which calls cmsObj_get or cmsPhl_get on this object.
             * We want to return the values from the new mdmObj, which has
             * not yet been pushed into the MDM.
             */
            if ((objNode = mdmlite_getObjectNode(oid)) == NULL)
            {
                mdmlite_error("Invalid oid=%d", oid);
               ret =  CMSRET_RESOURCE_EXCEEDED;
            }
            else
            {
               ret = oidInfo->stlHandlerFunc(internalMdmObj, iidStack);
            }
         }
      }

      if (ret == CMSRET_SUCCESS_OBJECT_UNCHANGED)
      {
         /*
          * stl handler function did not change the data, so we don't
          * need to push the mdmObj back into the MDM.  Just return our
          * current copy of mdmObj and change the return value to a
          * regular CMSRET_SUCCESS so the caller will not get confused.
          */
          ret = CMSRET_SUCCESS;
      }
      else if (ret == CMSRET_SUCCESS)
      {
         /*
          * stl handler function has changed the data.  Push the updated
          * mdmObj into the MDM.
          */
         ret = mdmlite_setObject(&internalMdmObj, iidStack, TRUE);
         if (ret != CMSRET_SUCCESS)
         {
            mdmlite_freeObject(internalMdmObj);
         }
         else
         {
            /* mdm_setObject stole our mdmObj from us, need to get it again. */
            ret = mdmlite_getObject(oid, iidStack, &internalMdmObj);
         }
      }
      else
      {
         /* error in the stl handler function. */
         mdmlite_freeObject(internalMdmObj);
      }
   }

   if ((CMSRET_SUCCESS == ret) && (internalMdmObj != NULL))
   {
         /* return the internal object directly to the internal caller */
         *mdmObj = internalMdmObj;
   }
   else
   {
         *mdmObj = NULL;
         mdmlite_error("Could not find OID %d", oid);
   }

   return ret;
}

/*****************************************************************************
*  FUNCTION:  _cmsObj_getNext
*  PURPOSE:   Get next instance of the MdmObject.
*  PARAMETERS:
*      oid - object id.
*      iidStack - instance stack (instance information).
*      mdmObj - mdm object pointer.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet _cmsObj_getNext(MdmObjectId oid, InstanceIdStack *iidStack,
  void **mdmObj)
{
   InstanceIdStack parentIidStack = EMPTY_INSTANCE_ID_STACK;
   return (_cmsObj_getNextInSubTreeFlags(oid, &parentIidStack, iidStack, 0,
     mdmObj));
}

/** Same as cmsObj_getNext() but this one accepts a flags argument. */
CmsRet _cmsObj_getNextFlags(MdmObjectId oid, InstanceIdStack *iidStack,
  UINT32 getFlags, void **mdmObj)
{
   InstanceIdStack parentIidStack = EMPTY_INSTANCE_ID_STACK;

   return (_cmsObj_getNextInSubTreeFlags(oid, &parentIidStack, iidStack,
     getFlags, mdmObj));
}

/*****************************************************************************
*  FUNCTION:  _cmsObj_getNextInSubTree
*  PURPOSE:   Get next instance of the MdmObject under the specified parent
*             instance.
*  PARAMETERS:
*      oid - object id.
*      parentIidStack - parent instance stack (instance information).
*      iidStack - instance stack (instance information).
*      mdmObj - mdm object pointer.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      The obtained instance is duped inside mdm_getNextObjectInSubTree().
*****************************************************************************/
CmsRet _cmsObj_getNextInSubTree(MdmObjectId oid, const InstanceIdStack
  *parentIidStack, InstanceIdStack *iidStack, void **mdmObj)
{
    return (_cmsObj_getNextInSubTreeFlags(oid, parentIidStack, iidStack, 0,
      mdmObj));
}

/* Same as cmsObj_getNextInSubTree() but this one accepts a flags argument. */
CmsRet _cmsObj_getNextInSubTreeFlags(MdmObjectId oid, const InstanceIdStack
  *parentIidStack, InstanceIdStack *iidStack, UINT32 getFlags
  __attribute__((unused)), void **mdmObj)
{
   UBOOL8 keepLooking=TRUE;
   CmsRet ret = CMSRET_SUCCESS;
   void *internalMdmObj=NULL;
   const _MdmOidInfoEntry *oidInfo;

   while (keepLooking)
   {
      ret = _mdm_getNextObjectInSubTree(oid, parentIidStack, iidStack, &internalMdmObj);
      if (ret != CMSRET_SUCCESS)
      {
         return ret;
      }
      else
      {
         keepLooking = FALSE;
      }
   }

   if ((getFlags & OGF_NO_VALUE_UPDATE) == 0)
   {
      if ((oidInfo = mdmlite_getOidInfo(oid)) == NULL)
      {
         mdmlite_error("Could not find OID info for oid %d", oid);
         ret = CMSRET_INTERNAL_ERROR;
      }
      else
      {
         if (oidInfo->stlHandlerFunc == NULL)
         {
            mdmlite_error("oid %d has NULL stl handler func", oid);
            ret = CMSRET_INTERNAL_ERROR;
         }
         else
         {
            _MdmObjectNode *objNode=NULL;

            /*
             * Before calling the STL handler, push this mdmObj into the setq.
             * This will handle scenario where STL handler func calls RUT
             * func, which calls cmsObj_get or cmsPhl_get on this object.
             * We want to return the values from the new mdmObj, which has
             * not yet been pushed into the MDM.
             */
            if ((objNode = mdmlite_getObjectNode(oid)) == NULL)
            {
                mdmlite_error("Invalid oid=%d", oid);
                ret =  CMSRET_RESOURCE_EXCEEDED;
            }
            else
            {
                ret = oidInfo->stlHandlerFunc(internalMdmObj, iidStack);
            }
         }
      }

      if (ret == CMSRET_SUCCESS_OBJECT_UNCHANGED)
      {
         ret = CMSRET_SUCCESS;
      }
      else if (ret == CMSRET_SUCCESS)
      {
         ret = mdmlite_setObject(&internalMdmObj, iidStack, TRUE);
         if (ret != CMSRET_SUCCESS)
         {
            mdmlite_freeObject(&internalMdmObj);
         }
         else
         {
            ret = mdmlite_getObject(oid, iidStack, &internalMdmObj);
         }
      }
      else
      {
         mdmlite_freeObject(&internalMdmObj);
      }
   }

   if (CMSRET_SUCCESS == ret)
   {
       /* return the internal object directly to the internal caller */
       *mdmObj = internalMdmObj;
   }
   else if (ret == CMSRET_NO_MORE_INSTANCES)
   {
       *mdmObj = NULL;
   }
   else
   {
      *mdmObj = NULL;
      mdmlite_error("Could not find OID %d, ret=%d", oid, ret);
   }

    return CMSRET_SUCCESS;
}

/*****************************************************************************
*  FUNCTION:  _callSetHandlerFunc
*  PURPOSE:   Invoke the RCL handler.
*  PARAMETERS:
*      objNode - object node pointer.
*      iidStack - instance stack (instance information).
*      newMdmObj - updated object.
*      currMdmObj - current object.
*      errorParam - error parameter.
*      errorCode - error code.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet _callSetHandlerFunc(_MdmObjectNode *objNode, const InstanceIdStack
  *iidStack, void *newMdmObj, const void *currMdmObj, char **errorParam,
  CmsRet *errorCode)
{
    MdmObjectId oid = objNode->oid;
    const _MdmOidInfoEntry *oidInfo;
    CmsRet ret;

    if ((oidInfo = mdmlite_getOidInfo(oid)) == NULL)
    {
        mdmlite_error("Could not find OID info for oid %d", oid);
        ret = CMSRET_INTERNAL_ERROR;
    }
    else
    {
        if (oidInfo->rclHandlerFunc == NULL)
        {
            mdmlite_error("oid %d has NULL rcl handler func", oid);
            ret = CMSRET_INTERNAL_ERROR;
        }
        else
        {
            mdmlite_debug("callback on %s", oidInfo->fullPath);

            ret = oidInfo->rclHandlerFunc(newMdmObj, currMdmObj, iidStack,
              errorParam, errorCode);
        }
    }
    return ret;
}

/*****************************************************************************
*  FUNCTION:  _cmsObj_setNoRclCallback
*  PURPOSE:   Set an object without RCL callback.
*  PARAMETERS:
*      mdmObj - mdm object pointer.
*      iidStack - instance stack (instance information).
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet _cmsObj_setNoRclCallback(const void *mdmObj, const InstanceIdStack *iidStack)
{
    MdmObjectId oid;
    CmsRet ret = CMSRET_SUCCESS;
    void *currMdmObj = NULL;

    if ((mdmObj == NULL) || (iidStack == NULL))
    {
       return CMSRET_INVALID_ARGUMENTS;
    }

    oid = *((MdmObjectId*)mdmObj);

    ret = mdmlite_getObject(oid, iidStack, &currMdmObj);
    if (ret == CMSRET_SUCCESS)
    {
        ret = mdmlite_setObjectInternal(mdmObj, currMdmObj, iidStack, FALSE);
        mdmlite_freeObject((void**)&currMdmObj);
    }
    else
    {
       mdmlite_error("Could not find OID %d", oid);
    }

    return CMSRET_SUCCESS;
}

/*****************************************************************************
*  FUNCTION:  _cmsObj_set
*  PURPOSE:   Set the object.
*  PARAMETERS:
*      mdmObj - mdm object pointer.
*      iidStack - instance stack (instance information).
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet _cmsObj_set(const void *mdmObj, const InstanceIdStack *iidStack)
{
    MdmObjectId oid;
    CmsRet ret = CMSRET_SUCCESS;
    void *currMdmObj = NULL;

    if ((mdmObj == NULL) || (iidStack == NULL))
    {
       return CMSRET_INVALID_ARGUMENTS;
    }

    oid = *((MdmObjectId*)mdmObj);

    ret = mdmlite_getObject(oid, iidStack, &currMdmObj);
    if (ret == CMSRET_SUCCESS)
    {
        ret = mdmlite_setObjectInternal(mdmObj, currMdmObj, iidStack, TRUE);
        mdmlite_freeObject((void**)&currMdmObj);
    }
    else
    {
       mdmlite_error("Could not find OID %d. ret=%d", oid, ret);
    }

    return CMSRET_SUCCESS;
}

/*****************************************************************************
*  FUNCTION:  _cmsObj_addInstance
*  PURPOSE:   Create an instance of an object.
*  PARAMETERS:
*      oid - object id.
*      iidStack - instance stack (instance information).
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet _cmsObj_addInstance(MdmObjectId oid, InstanceIdStack *iidStack)
{
    _MdmPathDescriptor pathDesc;
    _MdmObjectNode *objNode;
    CmsRet ret = CMSRET_SUCCESS;
    CmsRet ret2 = CMSRET_SUCCESS;
    UINT32 objectsAdded = 0;

    _INIT_PATH_DESCRIPTOR(&pathDesc);
    pathDesc.oid = oid;
    pathDesc.iidStack = *iidStack;

    if ((objNode = mdmlite_getObjectNode(oid)) == NULL)
    {
        mdmlite_error("Invalid oid=%d", oid);
        return CMSRET_INVALID_PARAM_NAME;
    }

    ret = mdmlite_addObjectInstance(&pathDesc, mdmlite_addObjectCallBack,
      &objectsAdded);
    if (ret != CMSRET_SUCCESS)
    {
       if (objectsAdded > 0)
       {
           mdmlite_error("%d objects already added when addObject failed, undo", objectsAdded);
           ret2 = mdmlite_deleteObjectInstance(&pathDesc, mdmlite_delObjectCallBack, NULL);
           if (ret2 != CMSRET_SUCCESS)
           {
              mdmlite_error("undo failed, reboot required.");
              ret = CMSRET_FAIL_REBOOT_REQUIRED;
           }
        }
    }

    if (IS_CMSRET_A_SUCCESS_VARIANT(ret))
    {
       *iidStack = pathDesc.iidStack;
    }

    return CMSRET_SUCCESS;
}

/*****************************************************************************
*  FUNCTION:  _cmsObj_getNthParam
*  PURPOSE:   Get attribute based on offset.
*  PARAMETERS:
*      mdmObj - mdm object pointer.
*      paramNbr - attribute offset.
*      paramInfo - obtained parameter information.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet _cmsObj_getNthParam(const void *mdmObj, const UINT32 paramNbr,
  _MdmObjParamInfo *paramInfo)
{
    MdmObjectId oid;
    _MdmObjectNode *objNode;
    _MdmParamNode *paramNode;
    CmsRet ret = CMSRET_SUCCESS;

    if ((mdmObj == NULL) || (paramInfo == NULL))
    {
        mdmlite_error("Invalid parameter");
        return CMSRET_INVALID_ARGUMENTS;
    }

    oid = *((MdmObjectId*)mdmObj);
    objNode = mdmlite_getObjectNode(oid);
    if (objNode == NULL)
    {
        mdmlite_error("Invalid oid=%d", oid);
        ret = CMSRET_INVALID_ARGUMENTS;
    }
    else if (paramNbr >= objNode->numParamNodes)
    {
        mdmlite_error("Invalid parameter number (%d >= %d)",
          paramNbr, objNode->numParamNodes);
        ret = CMSRET_INVALID_ARGUMENTS;
    }

    if (ret != CMSRET_SUCCESS)
    {
        return ret;
    }

    paramNode = &(objNode->params[paramNbr]);

    paramInfo->totalParams = objNode->numParamNodes;
    strcpy(paramInfo->name, paramNode->name);
    paramInfo->type = paramNode->type;
    paramInfo->minVal = (UINT32)paramNode->vData.min;
    paramInfo->maxVal = (UINT32)paramNode->vData.max;

    switch (paramNode->type)
    {
        case _MPT_STRING:
        case _MPT_DATE_TIME:
        case _MPT_BASE64:
        case _MPT_HEX_BINARY:
            {
                char **pMdmObj = (char**)mdmObj;
                paramInfo->val = pMdmObj[paramNode->offsetInObject/sizeof(char *)];
                paramInfo->minVal = 0;
                break;
            }

        case _MPT_INTEGER:
        case _MPT_LONG64:
            {
                SINT32 *pMdmObj = (SINT32*)mdmObj;
                paramInfo->val = &pMdmObj[paramNode->offsetInObject/sizeof(SINT32)];
                break;
            }

        case _MPT_UNSIGNED_INTEGER:
        case _MPT_UNSIGNED_LONG64:
            {
                UINT32 *pMdmObj = (UINT32*)mdmObj;
                paramInfo->val = &pMdmObj[paramNode->offsetInObject/sizeof(UINT32)];
                break;
            }
        case _MPT_BOOLEAN:
            {
                UBOOL8 *pMdmObj = (UBOOL8*)mdmObj;
                paramInfo->val = &pMdmObj[paramNode->offsetInObject/sizeof(UBOOL8)];
                break;
            }

        default:
            {
                cmsLog_error("invalid type of paramNode, %s %d",
                             paramNode->name, paramNode->type);
                ret = CMSRET_INTERNAL_ERROR;
                break;
            }

    } /* end of switch on paramNode->type */

    return ret;
}

/*****************************************************************************
*  FUNCTION:  _cmsObj_setNthParam
*  PURPOSE:   Set attribute based on offset.
*  PARAMETERS:
*      mdmObj - mdm object pointer.
*      paramNbr - attribute offset.
*      paramVal - parameter value.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet _cmsObj_setNthParam(void *mdmObj, const UINT32 paramNbr, const void
  *paramVal)
{
    MdmObjectId oid;
    _MdmObjectNode *objNode;
    _MdmParamNode *paramNode;
    CmsRet ret = CMSRET_SUCCESS;

    if ((mdmObj == NULL) || (paramVal == NULL))
    {
        return CMSRET_INVALID_ARGUMENTS;
    }

    oid = *((MdmObjectId*)mdmObj);
    objNode = mdmlite_getObjectNode(oid);
    if (objNode == NULL)
    {
        mdmlite_error("Invalid oid=%d", oid);
        ret = CMSRET_INVALID_ARGUMENTS;
    }
    else if (paramNbr >= objNode->numParamNodes)
    {
        mdmlite_error("Invalid parameter number (%d >= %d)",
          paramNbr, objNode->numParamNodes);
        ret = CMSRET_INVALID_ARGUMENTS;
    }

    if (ret != CMSRET_SUCCESS)
    {
        mdmlite_error("Invalid parameter, oid=%d", oid);
        return ret;
    }

    paramNode = &(objNode->params[paramNbr]);

    return mdmlite_setParamNode(mdmObj, paramNode, paramVal);
}

/*****************************************************************************
*  FUNCTION:  _cmsObj_deleteInstance
*  PURPOSE:   Delete an instance of an object.
*  PARAMETERS:
*      oid - object id.
*      iidStack - instance stack (instance information).
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet _cmsObj_deleteInstance(MdmObjectId oid, const InstanceIdStack *iidStack)
{
   _MdmPathDescriptor pathDesc;
   CmsRet ret = CMSRET_SUCCESS;
   UINT32 deleteError = 0;

   _INIT_PATH_DESCRIPTOR(&pathDesc);
   pathDesc.oid = oid;
   pathDesc.iidStack = *iidStack;

   ret = mdmlite_deleteObjectInstance(&pathDesc, mdmlite_delObjectCallBack,
     &deleteError);
   if ((ret != CMSRET_SUCCESS) && (ret != CMSRET_SUCCESS_REBOOT_REQUIRED))
   {
      if (deleteError > 0)
      {
          mdmlite_error("%d rcl handler error detected, reboot required", deleteError);
          ret = CMSRET_SUCCESS_REBOOT_REQUIRED;
      }
   }

    return CMSRET_SUCCESS;
}

/*****************************************************************************
*  FUNCTION:  mdmlite_addObjectCallBack
*  PURPOSE:   Callback function when adding an object.
*  PARAMETERS:
*      objNode - object node.
*      iidStack - instance stack (instance information).
*      mdmObj - mdm object pointer.
*      context - context of the callback.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet mdmlite_addObjectCallBack(_MdmObjectNode *objNode __attribute__((unused)),
  InstanceIdStack *iidStack, void *mdmObj, void *context)
{
   UINT32 *objectsAdded = (UINT32 *) context;
   CmsRet ret;

   mdmlite_debug("got callback on %s", mdmlite_oidToGenericPath(objNode->oid));

   ret = mdmlite_setObjectInternal(mdmObj, NULL, iidStack, TRUE);
   if (objectsAdded != NULL) (*objectsAdded)++;

   return ret;
}

/*****************************************************************************
*  FUNCTION:  mdmlite_delObjectCallBack
*  PURPOSE:   Callback function when deleting an object.
*  PARAMETERS:
*      objNode - object node.
*      iidStack - instance stack (instance information).
*      mdmObj - mdm object pointer.
*      context - context of the callback.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet mdmlite_delObjectCallBack(_MdmObjectNode *objNode, InstanceIdStack
  *iidStack, void *mdmObj, void *context)
{
   MdmObjectId oid = objNode->oid;
   const _MdmOidInfoEntry *oidInfo;
   UINT32 *deleteError = (UINT32 *) context;
   char *errorParam=NULL;
   CmsRet errorCode = CMSRET_SUCCESS;
   CmsRet ret = CMSRET_SUCCESS;

   if ((oidInfo = mdmlite_getOidInfo(oid)) == NULL)
   {
       mdmlite_error("Could not find OID info for oid %d", oid);
       ret = CMSRET_INTERNAL_ERROR;
   }
   else
   {
       if (oidInfo->rclHandlerFunc == NULL)
       {
           mdmlite_error("oid %d has NULL rcl handler func", oid);
           ret = CMSRET_INTERNAL_ERROR;
       }
       else
       {
           mdmlite_debug("callback on %s", oidInfo->fullPath);

           /* call the rcl handler func because this object now has gone out of existence */
           ret = oidInfo->rclHandlerFunc(NULL, mdmObj, iidStack,
                                         &errorParam, &errorCode);
       }
   }

   if (!IS_CMSRET_A_SUCCESS_VARIANT(ret))
   {
      if (deleteError != NULL) (*deleteError)++;
   }

   return ret;
}

/*****************************************************************************
*  FUNCTION:  mdmlite_setObjectInternal
*  PURPOSE:   Internal operations when setting an object.
*  PARAMETERS:
*      newMdmObj - updated object.
*      currMdmObj - current object.
*      iidStack - instance stack (instance information).
*      rclCB - whether the RCL callback should be invoked.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet mdmlite_setObjectInternal(const void *newMdmObj, const void *currMdmObj,
  const InstanceIdStack *iidStack, UBOOL8 rclCB)
{
   MdmObjectId oid;
   _MdmObjectNode *objNode;
   void *dupedNewMdmObj;
   char *errorParam = NULL;
   CmsRet ret, errorCode = CMSRET_SUCCESS;

   if ((newMdmObj == NULL) || (iidStack == NULL))
   {
       mdmlite_error("bad input args");
       return CMSRET_INVALID_ARGUMENTS;
   }

   oid = *((MdmObjectId*)newMdmObj);
   if ((objNode = mdmlite_getObjectNode(oid)) == NULL)
   {
       mdmlite_error("Invalid oid=%d", oid);
       return CMSRET_INVALID_ARGUMENTS;
   }

   if ((ret = mdmlite_procNewObject(newMdmObj, iidStack, ACTION_VALIDATE))
     != CMSRET_SUCCESS)
   {
       mdmlite_error("mdmlite_procNewObject() failed, oid=%d", oid);
       return ret;
   }

   mdmlite_debug("starting new internal set of %s iidStack=%s",
     objNode->name, mdmlite_dumpIidStack(iidStack));

   if ((dupedNewMdmObj = mdmlite_dupObject(newMdmObj)) == NULL)
   {
       mdmlite_error("mdmlite_dupObject() failed, oid=%d", oid);
       return CMSRET_RESOURCE_EXCEEDED;
   }

   if (rclCB == TRUE)
   {
       ret = _callSetHandlerFunc(objNode, iidStack, dupedNewMdmObj, currMdmObj,
         &errorParam, &errorCode);

       CMSMEM_FREE_BUF_AND_NULL_PTR(errorParam);
   }

   if (IS_CMSRET_A_SUCCESS_VARIANT(ret))
   {
       CmsRet r2;
       r2 = mdmlite_setObject(&dupedNewMdmObj, iidStack, TRUE);
       if (r2 != CMSRET_SUCCESS)
       {
           ret = r2;
           mdmlite_freeObject(&dupedNewMdmObj);
       }
   }
   else
   {
       mdmlite_freeObject(&dupedNewMdmObj);
   }

   mdmlite_debug("finished internal set of %s iidStack=%s, ret=%d",
     objNode->name, mdmlite_dumpIidStack(iidStack), ret);

   return ret;
}

char *_cmsMdm_dumpIidStack(const InstanceIdStack *iidStack)
{
    return mdmlite_dumpIidStack(iidStack);
}

const char *_mdm_oidToGenericPath(MdmObjectId oid)
{
    return mdmlite_oidToGenericPath(oid);
}
