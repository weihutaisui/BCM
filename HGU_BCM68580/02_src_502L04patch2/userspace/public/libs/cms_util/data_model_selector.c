/***********************************************************************
 *
 *  Copyright (c) 2006-2007  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:DUAL/GPL:standard

Unless you and Broadcom execute a separate written software license
agreement governing use of this software, this software is licensed
to you under the terms of the GNU General Public License version 2
(the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
with the following added to such license:

   As a special exception, the copyright holders of this software give
   you permission to link this software with independent modules, and
   to copy and distribute the resulting executable under terms of your
   choice, provided that you also meet, for each linked independent
   module, the terms and conditions of the license of that module.
   An independent module is a module which is not derived from this
   software.  The special exception does not apply to any modifications
   of the software.

Not withstanding the above, under no circumstances may you combine
this software in any way with any other Broadcom software provided
under a license other than the GPL, without Broadcom's express prior
written consent.

:>
 *
 ************************************************************************/

#include "cms.h"
#include "cms_util.h"


void cmsUtil_toggleDataModel(void)
{
#if defined(SUPPORT_DM_LEGACY98) || defined(SUPPORT_DM_HYBRID) ||  defined(SUPPORT_DM_PURE181)
   cmsLog_error("this function is only valid in DM Detect mode");
#elif defined(SUPPORT_DM_DETECT)
   SINT32 rv;
   CmsRet ret;
   UINT8 dmc[CMS_DATA_MODEL_PSP_VALUE_LEN]={0};

   rv = cmsPsp_get(CMS_DATA_MODEL_PSP_KEY, dmc, sizeof(dmc));
   if (rv != CMS_DATA_MODEL_PSP_VALUE_LEN)
   {
      /* No PSP file, create one with default entry of 1 (meaning Pure181) */
      dmc[0] = 1;
   }
   else
   {
      /* toggle to the other value */
      if (dmc[0] == 0)
      {
         dmc[0] = 1;
      }
      else if (dmc[0] == 1)
      {
         dmc[0] = 0;
      }
      else
      {
         cmsLog_error("Unexpected value in CMS Data Model PSP key=%d", dmc[0]);
         /* not a fatal error, just set to 1 */
         dmc[0] = 1;
      }
   }

   if ((ret = cmsPsp_set(CMS_DATA_MODEL_PSP_KEY, dmc, sizeof(dmc))) != CMSRET_SUCCESS)
   {
      cmsLog_error("set of CMS Data Model PSP key failed, ret=%d", ret);
   }
#endif
}


UBOOL8 cmsUtil_isDataModelDevice2(void)
{
   UBOOL8 b = FALSE;

#if defined(SUPPORT_DM_LEGACY98) || defined(SUPPORT_DM_HYBRID)

   b = FALSE;

#elif defined(SUPPORT_DM_PURE181)

   b = TRUE;

#elif defined(SUPPORT_DM_DETECT)
   SINT32 rv;
   UINT8 dmc[CMS_DATA_MODEL_PSP_VALUE_LEN]={0};

   rv = cmsPsp_get(CMS_DATA_MODEL_PSP_KEY, dmc, sizeof(dmc));
   if (rv != CMS_DATA_MODEL_PSP_VALUE_LEN)
   {
      /* No PSP file, assume default of TRUE (meaning Pure181) */
      b = TRUE;
   }
   else
   {
      if (dmc[0] == 0)
      {
         b = FALSE;
      }
      else if (dmc[0] == 1)
      {
         b = TRUE;
      }
      else
      {
         cmsLog_error("Unexpected value in CMS Data Model PSP key=%d", dmc[0]);
         /* not a fatal error, just assume TRUE */
         b = TRUE;
      }
   }
#endif

   return b;
}


void cmsUtil_setDataModelDevice2(void)
{
#if defined(SUPPORT_DM_LEGACY98) || defined(SUPPORT_DM_HYBRID) ||  defined(SUPPORT_DM_PURE181)
   cmsLog_error("this function is only valid in DM Detect mode");
#elif defined(SUPPORT_DM_DETECT)
   CmsRet ret;
   UINT8 dmc[CMS_DATA_MODEL_PSP_VALUE_LEN]={0};

   dmc[0] = 1;
   if ((ret = cmsPsp_set(CMS_DATA_MODEL_PSP_KEY, dmc, sizeof(dmc))) != CMSRET_SUCCESS)
   {
      cmsLog_error("set of CMS Data Model PSP key failed, ret=%d", ret);
   }
#endif
}


void cmsUtil_clearDataModelDevice2(void)
{
#if defined(SUPPORT_DM_LEGACY98) || defined(SUPPORT_DM_HYBRID) ||  defined(SUPPORT_DM_PURE181)
   cmsLog_error("this function is only valid in DM Detect mode");
#elif defined(SUPPORT_DM_DETECT)
   CmsRet ret;
   UINT8 dmc[CMS_DATA_MODEL_PSP_VALUE_LEN]={0};

   if ((ret = cmsPsp_set(CMS_DATA_MODEL_PSP_KEY, dmc, sizeof(dmc))) != CMSRET_SUCCESS)
   {
      cmsLog_error("clear of CMS Data Model PSP key failed, ret=%d", ret);
   }
#endif
}


