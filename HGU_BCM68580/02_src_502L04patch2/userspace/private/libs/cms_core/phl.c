/***********************************************************************
 *
 *  Copyright (c) 2006-2008  Broadcom Corporation
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

#include "cms.h"
#include "cms_phl.h"
#include "cms_mdm.h"
#include "cms_mem.h"
#include "cms_log.h"
#include "cms_util.h"
#include "mdm.h"
#include "odl.h"

/** Local functions **/
static UBOOL8 phl_sameObjDescs(MdmPathDescriptor *pPathDesc1,
                             MdmPathDescriptor *pPathDesc2);
static CmsRet phl_getNextPath(UBOOL8             paramOnly,
                              UBOOL8             nextLevelOnly,
                              MdmPathDescriptor  *pRootPath,
                              MdmPathDescriptor  *pNextPath);




/* start of function implementations */

static UBOOL8 phl_sameObjDescs(MdmPathDescriptor *pPathDesc1,
                               MdmPathDescriptor *pPathDesc2)
{
   return ((pPathDesc1->oid == pPathDesc2->oid) &&
           (cmsMdm_compareIidStacks(&(pPathDesc1->iidStack), &(pPathDesc2->iidStack)) == 0));

}


/** call mdm_getNextObjPathDesc() with additional check for objects
 *  that should be hidden from tr69c.
 */
static CmsRet phl_wrappedGetNextObjPathDesc(const MdmPathDescriptor *pRootPath,
                                            MdmPathDescriptor *pNextPath)
{
   MdmPathDescriptor lastHiddenPath;
   UBOOL8 found=FALSE;
   CmsRet ret=CMSRET_SUCCESS;

   INIT_PATH_DESCRIPTOR(&lastHiddenPath);


   while (!found && ret == CMSRET_SUCCESS)
   {
      ret = mdm_getNextObjPathDesc(pRootPath, pNextPath);

      if (ret == CMSRET_SUCCESS)
      {
         found = TRUE;

         /*
          * Normally, we would be done at this point.  But if the caller is tr69c,
          * we have to make sure this object is not hidden from tr69c.
          */
         if (mdmLibCtx.eid == EID_TR69C ||
             mdmLibCtx.eid == EID_CWMPD ||
             mdmLibCtx.eid == EID_USPD)
         {
            UBOOL8 hidden=FALSE;


            mdm_getPathDescHiddenFromAcs(pNextPath, &hidden);

            if (hidden)
            {
               /* this object is hidden, so go around again and get the next object */
               found = FALSE;

               /* remember the top of a sub-tree which is hidden from ACS */
               if ((lastHiddenPath.oid == 0) ||
                   (FALSE == mdm_isPathDescContainedInSubTree(&lastHiddenPath, pNextPath)))
               {
                  lastHiddenPath = *pNextPath;
               }
            }


            /*
             * We have to do this in case the user marks a top level object as
             * hideObjectFromAcs="true", but does not mark the objects below that
             * object with hideObjectFromAcs.  mdm_getNextObjPathDesc() will keep
             * traversing the tree, so we have to detect those objects and skip them.
             */
            if (lastHiddenPath.oid != 0 && found)
            {
               if (mdm_isPathDescContainedInSubTree(&lastHiddenPath, pNextPath))
               {
                  found = FALSE;
               }
            }
         }
      }
   }

   return ret;
}


/** call mdm_getNextChildObjectPathDesc() with additional check for objects
 *  that should be hidden from tr69c.
 */
static CmsRet phl_wrappedGetNextChildObjPathDesc(const MdmPathDescriptor *pRootPath,
                                                 MdmPathDescriptor *pNextPath)
{
   UBOOL8 found=FALSE;
   CmsRet ret=CMSRET_SUCCESS;

   while (!found && ret == CMSRET_SUCCESS)
   {
      ret = mdm_getNextChildObjPathDesc(pRootPath, pNextPath);

      if (ret == CMSRET_SUCCESS)
      {
         found = TRUE;

         /*
          * Normally, we would be done at this point.  But if the caller is tr69c,
          * we have to make sure this object is not hidden from tr69c.
          */
         if (mdmLibCtx.eid == EID_TR69C ||
             mdmLibCtx.eid == EID_CWMPD ||
             mdmLibCtx.eid == EID_USPD)
         {
            UBOOL8 hidden=FALSE;
            mdm_getPathDescHiddenFromAcs(pNextPath, &hidden);
            if (hidden)
            {
               /* this object is hidden, so go around again and get the next object */
               found = FALSE;
            }
         }
      }
   }

   return ret;
}


/** call mdm_getNextParamName() with additional check for parameters taht should be
 * hidden from tr69c.
 */
static CmsRet phl_wrappedGetNextParamName(MdmPathDescriptor *path)
{
   UBOOL8 found=FALSE;
   CmsRet ret=CMSRET_SUCCESS;

   while (!found && ret == CMSRET_SUCCESS)
   {
      ret = mdm_getNextParamName(path);

      if (ret == CMSRET_SUCCESS)
      {
         found = TRUE;

         /*
          * Normally, we would be done at this point.  But if the caller is tr69c,
          * we have to make sure this parameter is not hidden from tr69c.
          */
         if (mdmLibCtx.eid == EID_TR69C ||
             mdmLibCtx.eid == EID_CWMPD ||
             mdmLibCtx.eid == EID_USPD)
         {
            UBOOL8 hidden=FALSE;
            mdm_getPathDescHiddenFromAcs(path, &hidden);
            if (hidden)
            {
               /* this parameter is hidden, so go around again and get the next parameter */
               found = FALSE;
            }
         }
      }
   }

   return ret;
}


/** Count the number of parameters that are not hidden from tr69c.
 */
static CmsRet phl_wrappedGetParamNameCount(const MdmPathDescriptor *path, UINT32 *numParams)
{
   MdmPathDescriptor paramPath;
   CmsRet ret=CMSRET_SUCCESS;

   if (IS_PARAM_NAME_PRESENT(path))
   {
      cmsLog_error("pathDesc must not have param name when calling this function");
      return CMSRET_INVALID_ARGUMENTS;
   }

   *numParams = 0;
   paramPath = *path;

   while ((ret = phl_wrappedGetNextParamName(&paramPath)) == CMSRET_SUCCESS)
   {
      (*numParams)++;
   }

   return CMSRET_SUCCESS;
}




/** Get the next param or object name in the walk.
 *  Must be called with lock held.
 *
 * @param paramOnly (IN) Report parameter names only.  Do not report object names.
 *                       GetParameterValues and GetParameterAttributes calls this
 *                       function with paramOnly=TRUE because they don't want the 
 *                       object names.  But GetParameterNames calls this function with
 *                       paramOnly=FALSE because it wants the object names as well as
 *                       the paramNames.
 */
static CmsRet phl_getNextPath(UBOOL8             paramOnly,
                              UBOOL8             nextLevelOnly,
                              MdmPathDescriptor  *pRootPath,
                              MdmPathDescriptor  *pNextPath)
{
   MdmPathDescriptor path;
   CmsRet            rc = CMSRET_SUCCESS;

   if (pRootPath->oid == 0 || pRootPath->paramName[0] != 0)
   {
      cmsLog_error("invalid root object path");
      return CMSRET_INTERNAL_ERROR;
   }

   if (pNextPath->oid == 0)
   {
      /* This is the first call to tree traversal.
       * Return the root object path.
       */
      *pNextPath = *pRootPath;

      if (!paramOnly)
      {
         return rc;
      }
   }

   /* Set path to pNextPath and do the traversing */
   path = *pNextPath;

   /* If this getnext is not for paramOnly and is nextLevelOnly and
    * pNextPath and pRootPath are not the same object, pNextPath must be the
    * direct child object of pRootPath.  We want to return the next
    * direct child object of pRootPath.
    */
   if (!paramOnly && nextLevelOnly && !phl_sameObjDescs(pRootPath, &path))
   {
      /* Return the next direct child object of pRootPath. */
      rc = phl_wrappedGetNextChildObjPathDesc(pRootPath, &path);
      if (rc != CMSRET_SUCCESS && rc != CMSRET_NO_MORE_INSTANCES)
      {
         cmsLog_error("mdm_getNextChildObjPathDesc error %d", rc);
      }
   }
   else
   {
      /* In this case, we want to return either the next parameter or
       * the next hierarchical object after pNextPath object.
       */
      while (rc == CMSRET_SUCCESS)
      {
         /* Get the next parameter of the path object */
         rc = phl_wrappedGetNextParamName(&path);
         if (rc == CMSRET_SUCCESS)
         {
            /* done */
            break;   /* out of while (rc == CMSRET_SUCCESS) */
         }
         else if (rc == CMSRET_NO_MORE_INSTANCES)
         {
            /* There are no more parameter in the path object.
             * If this getnext is for paramOnly and is nextLevelOnly,
             * then done with finding nextParam.
             * Otherwise, find the next hierarchical object after
             * path and continue traversing.
             */
            if (!paramOnly || !nextLevelOnly)
            {
               /* find the next hierarchical object path */

               /* if pRootPath and path have the same object path,
                * we know that this will be the first call to
                * mdm_getNextObjPathDesc. path must be set to 0
                * prior to the first call to mdm_getNextObjPathDesc.
                */
               if (phl_sameObjDescs(pRootPath, &path))
               {
                  memset(&path, 0, sizeof(MdmPathDescriptor));
               }

	       if(nextLevelOnly)
                  rc = phl_wrappedGetNextChildObjPathDesc(pRootPath, &path);
	       else 
               rc = phl_wrappedGetNextObjPathDesc(pRootPath, &path);
               if (rc == CMSRET_SUCCESS)
               {
                  /* If not paramOnly, just return path object */
                  if (!paramOnly)
                  {
                     /* done */
                     break;   /* out of while (rc == CMSRET_SUCCESS) */
                  }
               }
               else if (rc != CMSRET_NO_MORE_INSTANCES)
               {
                  cmsLog_error("mdm_getNextObjPathDesc error %d", rc);
               }
            }
         }
         else
         {
            cmsLog_error("mdm_getNextParamName error %d", rc);
         }
      }  /* while (rc == CMSRET_SUCCESS) */
   }

   if (rc == CMSRET_SUCCESS)
   {
      /* Return the next path */
      *pNextPath = path;
   }

   return rc;

}  /* End of phl_getNextPath() */


CmsRet cmsPhl_getNextPath(UBOOL8             paramOnly,
                          UBOOL8             nextLevelOnly,
                          MdmPathDescriptor  *pRootPath,
                          MdmPathDescriptor  *pNextPath)
{
   UINT32 maxEntries=1;
   CmsRet rc;

   if ((rc = lck_checkBeforeEntry(__FUNCTION__, maxEntries))!= CMSRET_SUCCESS)
   {
      return rc;
   }

   rc = phl_getNextPath(paramOnly, nextLevelOnly, pRootPath, pNextPath);

   lck_trackExit(__FUNCTION__);

   return rc;

}   /* End of cmsPhl_getNextPath() */


CmsRet cmsPhl_setParameterValues(PhlSetParamValue_t *pSetParamValueList,
                                 SINT32             numEntries)
{
   UINT32 maxEntries=1;
   SINT32               i = 0, j = 0;
   PhlSetParamValue_t   *pSetParamValue;
   UBOOL8               writable;
   MdmNodeAttributes    nodeAttr;
   CmsRet               rc = CMSRET_SUCCESS;
   CmsRet               ret = CMSRET_SUCCESS;

   if ((rc = lck_checkBeforeEntry(__FUNCTION__, maxEntries))!= CMSRET_SUCCESS)
   {
      return rc;
   }

   cmsLog_debug("--->entered, numEntries=%d", numEntries);

   for (i = 0, pSetParamValue = pSetParamValueList;
        i < numEntries;
        i++, pSetParamValue++)
   {
      /*
       * An early fault may have been detected in doSetParameterValues.
       */
      if (pSetParamValue->status != CMSRET_SUCCESS)
      {
         rc = CMSRET_INVALID_ARGUMENTS;
         continue;
      }

      /*
       * make sure the caller is in the access list.
       * A good side effect of getting the attributes is that we verify
       * the pathDesc points to a valid object.
       */
      ret = mdm_getParamAttributes(&(pSetParamValue->pathDesc), &nodeAttr);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("invalid param %s%s",
                      mdm_oidToGenericPath(pSetParamValue->pathDesc.oid),
                      pSetParamValue->pathDesc.paramName);
         pSetParamValue->status = CMSRET_INVALID_PARAM_NAME;
         rc = CMSRET_INVALID_ARGUMENTS;
         continue;
      }

      if (!mdm_isInAccessList(nodeAttr.accessBitMask) &&
          !mdm_isFullWriteAccessEid(mdmLibCtx.eid))
      {
         const CmsEntityInfo *eInfo __attribute__ ((unused));

         eInfo = cmsEid_getEntityInfo(mdmLibCtx.eid);
         cmsLog_error("caller %s (eid=%d) is not in access list 0x%x",
                      eInfo->name, mdmLibCtx.eid, nodeAttr.accessBitMask);
         pSetParamValue->status = CMSRET_REQUEST_DENIED;
         rc = CMSRET_INVALID_ARGUMENTS;
         continue;
      }

      /* make sure pathDesc points to a writable param */
      mdm_getPathDescWritable(&(pSetParamValue->pathDesc), &writable);
      if (!writable && !mdm_isFullWriteAccessEid(mdmLibCtx.eid))
      {
         cmsLog_error("param %s%s is NOT writable",
                      mdm_oidToGenericPath(pSetParamValue->pathDesc.oid),
                      pSetParamValue->pathDesc.paramName);
         /*
          * Plugfest 1/21/08: should be 9008 (SET_NON_WRITABLE_PARAM)
          * not 9001 (REQUEST_DENIED).  Also, do not break out of the
          * loop.  Keep iterating through the params in the array to
          * gather more error codes on the other parameters.
          */
         pSetParamValue->status = CMSRET_SET_NON_WRITABLE_PARAM;
         rc = CMSRET_INVALID_ARGUMENTS;
         continue;
      }

      /* make sure the type given by the ACS matches what we think it is */
      if (cmsUtl_strcmp(pSetParamValue->pParamType,
                        mdm_getParamType(&pSetParamValue->pathDesc)))
      {
         cmsLog_error("invalid param type detected for %s%s, got=%s exp=%s",
                      mdm_oidToGenericPath(pSetParamValue->pathDesc.oid),
                      pSetParamValue->pathDesc.paramName,
                      pSetParamValue->pParamType,
                      mdm_getParamType(&pSetParamValue->pathDesc));
         pSetParamValue->status = CMSRET_INVALID_PARAM_TYPE;
         rc = CMSRET_INVALID_ARGUMENTS;
         continue;
      }

      /* make sure the new string value is valid */
      pSetParamValue->status = mdm_validateString(&(pSetParamValue->pathDesc), pSetParamValue->pValue);
      if (pSetParamValue->status != CMSRET_SUCCESS)
      {
         cmsLog_error("invalid param value detected for %s%s, value=%s",
                      mdm_oidToGenericPath(pSetParamValue->pathDesc.oid),
                      pSetParamValue->pathDesc.paramName,
                      pSetParamValue->pValue);
         rc = CMSRET_INVALID_ARGUMENTS;
         continue;
      }

      /* make sure parameter name is not duplicated
       * to fix issue in UNH TR-069 Certification test case 5.59
       * SetParameterValues Same Parameter Multiple Times
       */
      for (j = (i + 1); j < numEntries; j++)
      {
         if ((pSetParamValue->pathDesc.oid == pSetParamValueList[j].pathDesc.oid) &&
             (cmsUtl_strcmp(pSetParamValue->pathDesc.paramName, pSetParamValueList[j].pathDesc.paramName) == 0) &&
             (cmsMdm_compareIidStacks(&(pSetParamValue->pathDesc.iidStack), &(pSetParamValueList[j].pathDesc.iidStack)) == 0))
         {
            pSetParamValue->status = CMSRET_INVALID_ARGUMENTS;
            pSetParamValueList[j].status = CMSRET_INVALID_ARGUMENTS;
         }
      }
      if (pSetParamValue->status != CMSRET_SUCCESS)
      {
         cmsLog_error("duplicate param name detected for %s%s, value=%s",
                      mdm_oidToGenericPath(pSetParamValue->pathDesc.oid),
                      pSetParamValue->pathDesc.paramName,
                      pSetParamValue->pValue);
         rc = CMSRET_INVALID_ARGUMENTS;
         continue;
      }
   }  /* for (i = 0; ....) */

   if (rc == CMSRET_SUCCESS)
   {
      /* call the object dispatch layer api */   
      rc = odl_set(pSetParamValueList, numEntries);
      if (rc != CMSRET_SUCCESS && rc != CMSRET_SUCCESS_REBOOT_REQUIRED)
      {
         cmsLog_error("odl_set error %d", rc);
      }
   }

   lck_trackExit(__FUNCTION__);

   return rc;

}  /* End of cmsPhl_setParameterValues() */


CmsRet cmsPhl_getPathCount(MdmPathDescriptor *pPathList,
                           SINT32            numEntries,
                           UBOOL8            paramOnly,
                           UBOOL8            nextLevelOnly,
                           SINT32            *pPathCnt)
{
   UINT32 maxEntries=1;
   SINT32            i;
   MdmPathDescriptor *pPath;
   CmsRet            rc = CMSRET_SUCCESS;

   *pPathCnt = 0;

   if ((rc = lck_checkBeforeEntry(__FUNCTION__, maxEntries))!= CMSRET_SUCCESS)
   {
      return rc;
   }

   /* loop through the requested name list */
   for (i = 0, pPath = pPathList;
        i < numEntries && rc == CMSRET_SUCCESS;
        i++, pPath++)
   {
      if (pPath->paramName[0] != 0)
      {
         /* pPath is a parameter path */
         *pPathCnt += 1;
      }
      else
      {
         /* pPath is an object path */
         /* traverse the subtree below pPath to find the
          * total number of parameters in the subtree.
          */
         MdmPathDescriptor nextObj = *pPath;

         while (rc == CMSRET_SUCCESS)
         {
            UINT32   numParams = 0;

            if (!paramOnly)
            {
               /* count this object */
               *pPathCnt += 1;
            }

            /* Find the number of parameters contained in this object */
            rc = phl_wrappedGetParamNameCount(&nextObj, &numParams);
            if (rc == CMSRET_SUCCESS)
            {
               /* count the parameters */
               *pPathCnt += numParams;

               if (nextLevelOnly)
               {
                  if (paramOnly)
                  {
                     /* done counting all the parameters */
                     break;   /* out of while (rc == CMSRET_SUCCESS) */
                  }
                  else
                  {
                     /* nextLevelOnly && !paramOnly */
                     /* for this case, we need to count the direct child
                      * objects of pPath.
                      */
                     /* nextObj must be set to 0 prior to the first call
                      * to mdm_getNextChildObjPathDesc.
                      */
                     memset(&nextObj, 0, sizeof(MdmPathDescriptor));

                     while (rc == CMSRET_SUCCESS)
                     {
                        /* get the next child object of pPath */
                        rc = phl_wrappedGetNextChildObjPathDesc(pPath, &nextObj);
                        if (rc == CMSRET_SUCCESS)
                        {
                           *pPathCnt += 1;
                        }
                        else if (rc != CMSRET_NO_MORE_INSTANCES)
                        {
                           cmsLog_error("mdm_getNextChildObjPathDesc error %d", rc);
                        }
                     }
                  }
               }
               else
               {
                  /* get the next hierarchical object after nextObj */

                  /* if pPath and nextObj have the same object path,
                   * we know that this will be the first call to
                   * mdm_getNextObjPathDesc. nextObj must be set to 0
                   * prior to the first call to mdm_getNextObjPathDesc.
                   */
                  if (phl_sameObjDescs(pPath, &nextObj))
                  {
                     memset(&nextObj, 0, sizeof(MdmPathDescriptor));
                  }

                  rc = phl_wrappedGetNextObjPathDesc(pPath, &nextObj);
                  if (rc != CMSRET_SUCCESS && rc != CMSRET_NO_MORE_INSTANCES)
                  {
                     cmsLog_error("mdm_getNextObjPathDesc error %d", rc);
                  }
               }
            }
            else
            {
               cmsLog_error("mdm_getParamNameCount error %d", rc);
            }
         }  /* while (rc == CMSRET_SUCCESS) */
         
         if (rc == CMSRET_NO_MORE_INSTANCES)
         {
            rc = CMSRET_SUCCESS;
         }
      }
   }  /* for (i = 0; ...) */

   if (rc != CMSRET_SUCCESS)
   {
      *pPathCnt = 0;
   }
   
   lck_trackExit(__FUNCTION__);

   return rc;

}  /* End of cmsPhl_getPathCount() */


CmsRet cmsPhl_getParamValue(MdmPathDescriptor   *pPath,
                            PhlGetParamValue_t  **pParamValue)
{
   UINT32 maxEntries=MDM_MAX_ENTRY_COUNT;
   PhlGetParamValue_t   *pResp;
   CmsRet               rc = CMSRET_SUCCESS;

   *pParamValue = NULL;

   if (pPath->paramName[0] == 0)
   {
      cmsLog_error("invalid parameter name");
      return CMSRET_INVALID_PARAM_NAME;
   }

   /* allocate memory for parameter value response */
   pResp = cmsMem_alloc(sizeof(PhlGetParamValue_t), ALLOC_ZEROIZE);
   if (pResp == NULL)
   {
      cmsLog_error("cmsMem_alloc failed");
      return CMSRET_INTERNAL_ERROR;
   }

   if ((rc = lck_checkBeforeEntry(__FUNCTION__, maxEntries))!= CMSRET_SUCCESS)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(pResp);
      return rc;
   }

   /* get parameter value from MDM */
   rc = odl_get(pPath, &(pResp->pValue));
   if (rc == CMSRET_SUCCESS)
   {
      pResp->pathDesc   = *pPath;
      pResp->pParamType = mdm_getParamType(pPath);
      *pParamValue      = pResp;
   }
   else
   {
      cmsLog_debug("odl_get error %d,  pPath->paramName %s", rc, pPath->paramName);
      cmsPhl_freeGetParamValueBuf(pResp, 1);
   }

   lck_trackExit(__FUNCTION__);

   return rc;

}  /* End of cmsPhl_getParamValue() */


CmsRet cmsPhl_getParameterValues(MdmPathDescriptor  *pPathList,
                                 SINT32             numEntries,
                                 UBOOL8             nextLevelOnly,
                                 PhlGetParamValue_t **pParamValueList,
                                 SINT32             *pNumParamValueEntries)
{
   UINT32 maxEntries=1;
   SINT32               i, numParams, numRespEntries;
   MdmPathDescriptor    *pPath;
   PhlGetParamValue_t   *pRespBuf, *pResp;
   CmsRet               rc = CMSRET_SUCCESS;

   *pParamValueList       = NULL;
   *pNumParamValueEntries = 0;

   /* find out how many parameter-value pairs will be in the response */
   numParams = 0;
   rc = cmsPhl_getPathCount(pPathList, numEntries, TRUE, nextLevelOnly, &numParams);
   if (rc != CMSRET_SUCCESS || numParams == 0)
   {
      return rc;
   }

   /* allocate memory for parameter value response */
   pRespBuf = cmsMem_alloc(numParams * sizeof(PhlGetParamValue_t), ALLOC_ZEROIZE);
   if (pRespBuf == NULL)
   {
      cmsLog_error("cmsMem_alloc failed");
      return CMSRET_INTERNAL_ERROR;
   }

   if ((rc = lck_checkBeforeEntry(__FUNCTION__, maxEntries))!= CMSRET_SUCCESS)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(pRespBuf);
      return rc;
   }


   /* loop through the requested name list */
   
   pResp          = pRespBuf;
   numRespEntries = 0;

   for (i = 0, pPath = pPathList;
        i < numEntries && rc == CMSRET_SUCCESS;
        i++, pPath++)
   {
      if (pPath->paramName[0] != 0)
      {
         /* this is a parameter path */
         rc = odl_get(pPath, &(pResp->pValue));
         if (rc == CMSRET_SUCCESS)
         {
            pResp->pathDesc   = *pPath;
            pResp->pParamType = mdm_getParamType(pPath);
            pResp++;
            numRespEntries++;
         }
         else
         {
            cmsLog_error("odl_get error %d", rc);
         }
      }
      else
      {
         /* this is an object path */
         /* traverse the sub-tree below the object path */
         MdmPathDescriptor nextPath;

         /* set nextPath to 0 to start traversing */
         memset(&nextPath, 0, sizeof(MdmPathDescriptor));

         while (rc == CMSRET_SUCCESS)
         {
            rc = phl_getNextPath(TRUE, nextLevelOnly, pPath, &nextPath);
            if (rc == CMSRET_SUCCESS)
            {
               rc = odl_get(&nextPath, &(pResp->pValue));
               if (rc == CMSRET_SUCCESS)
               {
                  pResp->pathDesc   = nextPath;
                  pResp->pParamType = mdm_getParamType(&(pResp->pathDesc));
                  pResp++;
                  numRespEntries++;
               }
               else
               {
                  cmsLog_error("odl_get error %d", rc);
               }
            }
            else if (rc == CMSRET_NO_MORE_INSTANCES)
            {
               rc = CMSRET_SUCCESS;
               break;   /* out of while (rc == CMSRET_SUCCESS) */
            }
            else
            {
               cmsLog_error("phl_getNextPath error %d", rc);
            }
         }  /* while (rc == CMSRET_SUCCESS) */
      }
   }  /* for (i = 0; ....) */

   if (rc == CMSRET_SUCCESS)
   {
      *pParamValueList       = pRespBuf;
      *pNumParamValueEntries = numRespEntries;
   }
   else
   {
      cmsPhl_freeGetParamValueBuf(pRespBuf, numParams);
   }

   lck_trackExit(__FUNCTION__);

   return rc;

}  /* End of cmsPhl_getParameterValues() */


CmsRet cmsPhl_getParamInfo(MdmPathDescriptor *pPath,
                           PhlGetParamInfo_t **pParamInfo)
{
   UINT32 maxEntries=1;
   PhlGetParamInfo_t *pResp;
   CmsRet            rc = CMSRET_SUCCESS;
   CmsRet ret;

   *pParamInfo = NULL;
                   
   /* allocate memory for parameter name info */
   pResp = cmsMem_alloc(sizeof(PhlGetParamInfo_t), ALLOC_ZEROIZE);
   if (pResp == NULL)
   {
      cmsLog_error("cmsMem_alloc failed");
      return CMSRET_INTERNAL_ERROR;
   }

   if ((rc = lck_checkBeforeEntry(__FUNCTION__, maxEntries))!= CMSRET_SUCCESS)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(pResp);
      return rc;
   }

   /* get info from MDM */
   if ((ret = mdm_getPathDescWritable(pPath, &(pResp->writable))) == CMSRET_SUCCESS)
   {
      if (IS_PARAM_NAME_PRESENT(pPath))
      {
         /*
          * The MdmPathDescriptor can point to an object or a parameter.
          * Only query about tr69c password if the MdmPathDescriptor points
          * to a parameter name.
          */
         ret = mdm_getParamIsTr69Password(pPath, &(pResp->isTr69Password));
         if (ret == CMSRET_SUCCESS)
         {
            MdmParamNode *paramNode;

            /* get the profile name of this parameter */
            if ((paramNode = mdm_getParamNode(pPath->oid, pPath->paramName)))
            {
               pResp->profile = paramNode->profile;
            }
         }
      }
      else
      {
         MdmObjectNode *objectNode;

         /* get the profile name of this object */
         if ((objectNode = mdm_getObjectNode(pPath->oid)))
         {
            pResp->profile = objectNode->profile;
         }
      }
   }

   if (ret == CMSRET_SUCCESS)
   {
      pResp->pathDesc = *pPath;
      *pParamInfo     = pResp;
   }
   else
   {
      rc = ret;
      cmsLog_error("error %d", rc);
      cmsMem_free(pResp);
   }

   lck_trackExit(__FUNCTION__);

   return rc;

}  /* End of cmsPhl_getParamInfo() */


CmsRet cmsPhl_getParameterNames(MdmPathDescriptor *pPath,
                                UBOOL8            nextLevelOnly,
                                PhlGetParamInfo_t **pParamInfoList,
                                SINT32            *pNumEntries)
{
   UINT32 maxEntries=1;
   SINT32            numParams, numRespEntries;
   PhlGetParamInfo_t *pRespBuf, *pResp;
   CmsRet            rc = CMSRET_SUCCESS;
                
   *pParamInfoList = NULL;
   *pNumEntries    = 0;

   /* find out how many name-infor pairs will be in the response */
   numParams = 0;
   rc = cmsPhl_getPathCount(pPath, 1, FALSE, nextLevelOnly, &numParams);
   if (rc != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsPhl_getPathCount error %d", rc);
      return rc;
   }

   if (numParams == 0)
   {
      cmsLog_error("numParams = 0");
      return CMSRET_INTERNAL_ERROR;
   }

   /* allocate memory for parameter name response */
   pRespBuf = cmsMem_alloc(numParams * sizeof(PhlGetParamInfo_t), ALLOC_ZEROIZE);
   if (pRespBuf == NULL)
   {
      cmsLog_error("cmsMem_alloc failed");
      return CMSRET_INTERNAL_ERROR;
   }

   if ((rc = lck_checkBeforeEntry(__FUNCTION__, maxEntries))!= CMSRET_SUCCESS)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(pRespBuf);
      return rc;
   }

   pResp          = pRespBuf;
   numRespEntries = 0;

   if (pPath->paramName[0] != 0)
   {
      /* this is a parameter name */
      pResp->pathDesc = *pPath;

      rc = mdm_getPathDescWritable(&(pResp->pathDesc), &(pResp->writable));
      if (rc == CMSRET_SUCCESS)
      {
         pResp++;
         numRespEntries++;
      }
      else
      {
         cmsLog_error("mdm_getPathDescWritable error %d", rc);
      }
   }
   else
   {
      /* this is an object name */
      /* traverse the sub-tree below the object node */

      MdmPathDescriptor nextPath;

      /* set nextPath to 0 to start traversing */
      memset(&nextPath, 0, sizeof(MdmPathDescriptor));

      while (rc == CMSRET_SUCCESS)
      {
         rc = phl_getNextPath(FALSE, nextLevelOnly, pPath, &nextPath);
         if (rc == CMSRET_SUCCESS)
         {
            rc = mdm_getPathDescWritable(&nextPath, &(pResp->writable));
            if (rc == CMSRET_SUCCESS)
            {
               pResp->pathDesc = nextPath;
               pResp++;
               numRespEntries++;
            }
            else
            {
               cmsLog_error("mdm_getPathDescWritable error %d", rc);
            }
         }
         else if (rc == CMSRET_NO_MORE_INSTANCES)
         {
            rc = CMSRET_SUCCESS;
            break;   /* out of while (rc == CMSRET_SUCCESS) */
         }
         else
         {
            cmsLog_error("phl_getNextPath error %d", rc);
         }
      }  /* while (rc == CMSRET_SUCCESS) */
   }

   if (rc == CMSRET_SUCCESS)
   {
      *pParamInfoList = pRespBuf;
      *pNumEntries    = numRespEntries;
   }
   else
   {
      cmsMem_free(pRespBuf);
   }

   lck_trackExit(__FUNCTION__);

   return rc;

}  /* End of cmsPhl_getParameterNames() */


CmsRet cmsPhl_setParameterAttributes(PhlSetParamAttr_t *pSetParamAttrList,
                                     SINT32            numEntries)
{
   UINT32 maxEntries=1;
   SINT32            i;
   PhlSetParamAttr_t *pSetParamAttr;
   UBOOL8            testOnly = TRUE;
   CmsRet            rc = CMSRET_SUCCESS;


   if ((rc = lck_checkBeforeEntry(__FUNCTION__, maxEntries))!= CMSRET_SUCCESS)
   {
      return rc;
   }

   /*
    * We need to loop through the setParamAttrList twice,
    * first with testOnly = TRUE, and if there are no errors, loop
    * through a second time with testOnly = FALSE.  This is to satisfy
    * TR69 atomic set requirements.  See A.3.2.4.
    */

   /* Perhaps we should only allow EID_TR69C to do this?  i.e. check mdmLibCtx.eid? */
    
   /* loop through the set parameter attribute list */
   pSetParamAttr = pSetParamAttrList;
   for (i = 0; i < numEntries && rc == CMSRET_SUCCESS; i++, pSetParamAttr++)
   {
      if ((pSetParamAttr->attributes.accessBitMaskChange == 0) &&
          (pSetParamAttr->attributes.notificationChange  == 0))
      {
         continue;   /* does not change anything */
      }

      rc = mdm_setParamAttributes(&(pSetParamAttr->pathDesc), &(pSetParamAttr->attributes), testOnly);
      if (rc != CMSRET_SUCCESS)
      {
         lck_trackExit(__FUNCTION__);
         return rc;
      }
   }  /* for (i = 0; ....) */

   /* OK, the test run succeeded, now do the real set */
   testOnly = FALSE;
   pSetParamAttr = pSetParamAttrList;
   for (i = 0; i < numEntries && rc == CMSRET_SUCCESS; i++, pSetParamAttr++)
   {
      if ((pSetParamAttr->attributes.accessBitMaskChange == 0) &&
          (pSetParamAttr->attributes.notificationChange  == 0))
      {
         continue;   /* does not change anything */
      }

      rc = mdm_setParamAttributes(&(pSetParamAttr->pathDesc), &(pSetParamAttr->attributes), FALSE);
      if (rc != CMSRET_SUCCESS)
      {
         cmsLog_error("mdm_setParamAttributes failure %d", rc);
      }
   }  /* for (i = 0; ....) */

   lck_trackExit(__FUNCTION__);

   return rc;

}  /* End of cmsPhl_setParameterAttributes() */


CmsRet cmsPhl_getParamAttr(MdmPathDescriptor *pPath,
                           PhlGetParamAttr_t **pParamAttr)
{
   UINT32 maxEntries=1;
   PhlGetParamAttr_t    *pResp;
   CmsRet               rc = CMSRET_SUCCESS;

   *pParamAttr = NULL;

   if (pPath->paramName[0] == 0)
   {
      cmsLog_error("invalid parameter name");
      return CMSRET_INVALID_PARAM_NAME;
   }

   /* allocate memory for parameter value response */
   pResp = cmsMem_alloc(sizeof(PhlGetParamAttr_t), ALLOC_ZEROIZE);
   if (pResp == NULL)
   {
      cmsLog_error("cmsMem_alloc failed");
      return CMSRET_INTERNAL_ERROR;
   }

   if ((rc = lck_checkBeforeEntry(__FUNCTION__, maxEntries))!= CMSRET_SUCCESS)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(pResp);
      return rc;
   }

   /* get parameter attributes from MDM */
   rc = mdm_getParamAttributes(pPath, &(pResp->attributes));
   if (rc == CMSRET_SUCCESS)
   {
      pResp->pathDesc = *pPath;
      *pParamAttr     = pResp;
   }
   else
   {
      cmsLog_error("mdm_getParamAttributes error %d", rc);
      cmsMem_free(pResp);
   }

   lck_trackExit(__FUNCTION__);

   return rc;

}  /* End of cmsPhl_getParamAttr() */


CmsRet cmsPhl_getParameterAttributes(MdmPathDescriptor *pPathList,
                                     SINT32            numEntries,
                                     UBOOL8            nextLevelOnly,
                                     PhlGetParamAttr_t **pParamAttrList,
                                     SINT32            *pNumParamAttrEntries)
{
   UINT32 maxEntries=1;
   SINT32            i, numParams, numRespEntries;
   MdmPathDescriptor *pPath;
   PhlGetParamAttr_t *pRespBuf, *pResp;
   CmsRet            rc = CMSRET_SUCCESS;

   *pParamAttrList       = NULL;
   *pNumParamAttrEntries = 0;

   /* find out how many parameter-value pairs will be in the response */
   numParams = 0;
   rc = cmsPhl_getPathCount(pPathList, numEntries, TRUE, nextLevelOnly, &numParams);
   if (rc != CMSRET_SUCCESS || numParams == 0)
   {
      return rc;
   }

   /* allocate memory for parameter attribute response */
   pRespBuf = cmsMem_alloc(numParams * sizeof(PhlGetParamAttr_t), ALLOC_ZEROIZE);
   if (pRespBuf == NULL)
   {
      cmsLog_error("cmsMem_alloc failed");
      return CMSRET_INTERNAL_ERROR;
   }


   if ((rc = lck_checkBeforeEntry(__FUNCTION__, maxEntries))!= CMSRET_SUCCESS)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(pRespBuf);
      return rc;
   }


   /* loop through the requested name list */
   
   pResp          = pRespBuf;
   numRespEntries = 0;

   for (i = 0, pPath = pPathList;
        i < numEntries && rc == CMSRET_SUCCESS;
        i++, pPath++)
   {
      if (pPath->paramName[0] != 0)
      {
         /* this is a parameter path */
         rc = mdm_getParamAttributes(pPath, &(pResp->attributes));
         if (rc == CMSRET_SUCCESS)
         {
            pResp->pathDesc = *pPath;
            pResp++;
            numRespEntries++;
         }
         else
         {
            cmsLog_error("mdm_getParamAttributes error %d", rc);
         }
      }
      else
      {
         /* this is an object path */
         /* traverse the sub-tree below the object node */
         MdmPathDescriptor nextPath;

         /* set nextPath to 0 to start traversing */
         memset(&nextPath, 0, sizeof(MdmPathDescriptor));

         while (rc == CMSRET_SUCCESS)
         {
            rc = phl_getNextPath(TRUE, nextLevelOnly, pPath, &nextPath);
            if (rc == CMSRET_SUCCESS)
            {
               rc = mdm_getParamAttributes(&nextPath, &(pResp->attributes));
               if (rc == CMSRET_SUCCESS)
               {
                  pResp->pathDesc = nextPath;
                  pResp++;
                  numRespEntries++;
               }
               else
               {
                  cmsLog_error("mdm_getParamAttributes error %d", rc);
               }
            }
            else if (rc == CMSRET_NO_MORE_INSTANCES)
            {
               rc = CMSRET_SUCCESS;
               break;
            }
            else
            {
               cmsLog_error("phl_getNextPath error %d", rc);
            }
         }  /* while (rc == CMSRET_SUCCESS) */
      }
   }  /* for (i = 0; ....) */

   if (rc == CMSRET_SUCCESS)
   {
      *pParamAttrList       = pRespBuf;
      *pNumParamAttrEntries = numRespEntries;
   }
   else
   {
      cmsMem_free(pRespBuf);
   }

   lck_trackExit(__FUNCTION__);

   return rc;

}  /* End of cmsPhl_getParameterAttributes() */


CmsRet cmsPhl_addObjInstance(MdmPathDescriptor *pPath)
{
   UINT32 maxEntries=1;
   CmsRet   rc = CMSRET_SUCCESS;

   if (pPath->paramName[0] != 0)
   {
      cmsLog_error("invalid object path");
      return CMSRET_INVALID_PARAM_NAME;
   }

   if ((rc = lck_checkBeforeEntry(__FUNCTION__, maxEntries))!= CMSRET_SUCCESS)
   {
      return rc;
   }

   /* do we have to check access list for add operation? */

   rc = odl_addObjectInstance(pPath);
   if (rc != CMSRET_SUCCESS)
   {
      cmsLog_error("odl_addObjInstance failure %d", rc);
   }

   lck_trackExit(__FUNCTION__);

   return rc;
      
}  /* End of cmsPhl_addObjInstance() */


CmsRet cmsPhl_delObjInstance(MdmPathDescriptor *pPath)
{
   UINT32 maxEntries=1;
   CmsRet   rc = CMSRET_SUCCESS;

   if (pPath->paramName[0] != 0)
   {
      cmsLog_error("Invalid object path");
      return CMSRET_INVALID_ARGUMENTS;
   }

   if ((rc = lck_checkBeforeEntry(__FUNCTION__, maxEntries))!= CMSRET_SUCCESS)
   {
      return rc;
   }

   /* do we have to check access list for delete operation? */

   rc = odl_deleteObjectInstance(pPath);
   if (rc != CMSRET_SUCCESS)
   {
      cmsLog_error("odl_delObjInstance failure %d", rc);
   }

   lck_trackExit(__FUNCTION__);

   return rc;

}  /* End of cmsPhl_delObjInstance() */


void cmsPhl_freeGetParamValueBuf(PhlGetParamValue_t *pBuf,
                                 SINT32             numEntries)
{
   SINT32               i;
   PhlGetParamValue_t   *ptr = pBuf;

   for (i = 0; (i < numEntries) && (ptr != NULL); i++, ptr++)
   {
      cmsMem_free(ptr->pValue);
   }
   cmsMem_free(pBuf);

}  /* End of cmsPhl_freeGetParamValueBuf() */


UINT32 cmsPhl_getNumberOfParamValueChanges(void)
{
   UINT32 maxEntries=1;
   UINT32 n = 0;

   if (lck_checkBeforeEntry(__FUNCTION__, maxEntries) == CMSRET_SUCCESS)
   {
      n = mdm_getNumberOfParamValueChanges();
      lck_trackExit(__FUNCTION__);
   }

   return n;

}  /* End of cmsPhl_getNumberOfParamValueChanges() */

UBOOL8 cmsPhl_isParamValueChanged(const MdmPathDescriptor *pathDesc)
{
   UINT32 maxEntries=1;
   UBOOL8 ret = FALSE;

   if (lck_checkBeforeEntry(__FUNCTION__, maxEntries) == CMSRET_SUCCESS)
   {
      ret = mdm_isParamValueChanged(pathDesc);
      lck_trackExit(__FUNCTION__);
   }

   return ret;

}  /* End of cmsPhl_isParamValueChanged() */

void cmsPhl_clearAllParamValueChanges(void)
{
   UINT32 maxEntries=1;

   if (lck_checkBeforeEntry(__FUNCTION__, maxEntries) == CMSRET_SUCCESS)
   {
      mdm_clearAllParamValueChanges();
      lck_trackExit(__FUNCTION__);      
   }

   return;

}  /* End of cmsPhl_clearAllParamValueChanges() */

