/*
* <:copyright-BRCM:2011:proprietary:standard
* 
*    Copyright (c) 2011 Broadcom 
*    All Rights Reserved
* 
*  This program is the proprietary software of Broadcom and/or its
*  licensors, and may only be used, duplicated, modified or distributed pursuant
*  to the terms and conditions of a separate, written license agreement executed
*  between you and Broadcom (an "Authorized License").  Except as set forth in
*  an Authorized License, Broadcom grants no license (express or implied), right
*  to use, or waiver of any kind with respect to the Software, and Broadcom
*  expressly reserves all rights in and to the Software and all intellectual
*  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
*  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
*  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
* 
*  Except as expressly set forth in the Authorized License,
* 
*  1. This program, including its structure, sequence and organization,
*     constitutes the valuable trade secrets of Broadcom, and you shall use
*     all reasonable efforts to protect the confidentiality thereof, and to
*     use this information only in connection with your use of Broadcom
*     integrated circuit products.
* 
*  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
*     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
*     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
*     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
*     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
*     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
*     PERFORMANCE OF THE SOFTWARE.
* 
*  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
*     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
*     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
*     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
*     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
*     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
*     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
*     LIMITED REMEDY.
:>
*/


#ifdef BRCM_WLAN

//#ifdef DMP_DEVICE2_WIFIRADIO_1


#include <stdio.h>

#include "cms.h"
#include "cms_mdm.h"
#include "cms_phl.h"
#include "cms_obj.h"
#include "cms_util.h"
#include "cgi_wl_dmx.h"
#include "cgi2_wl_mdm.h"


extern int wldmx_radioIdx;
extern int wldmx_ssidIdx;

static UINT32 convertRadioIndexToIid(int radioIdx);
static UINT32 convertSsidIndexToIid(int radioIdx, int ssidIdx);


/*
 * This structure and table maps the wl variables used in WebUI
 * to the TR181 data model objects and params.
 */
typedef struct wldmx_param_entry {
   const char *cgiName;
   const char *trName;
   MdmParamTypes trType;
   UBOOL8 readOnly;
   MdmObjectId oid;
   SINT32 radioIdx;  /**< if -1, then use wldmx_radioIdx */
   SINT32 ssidIdx;   /**< if -1, then use wldmx_ssidIdx.  If -2, use radioIdx only. */
} WLDMX_PARAM_ENTRY;


WLDMX_PARAM_ENTRY wldmx_param_array[] = {
   {
      .cgiName = "wlEnbl",
      .trName = "Enable",
      .trType = MPT_BOOLEAN,
      .oid = MDMOID_DEV2_WIFI_SSID,
      .radioIdx = -1,
      .ssidIdx = -1,
   },
   {
      .cgiName = "wlSsid",
      .trName = "SSID",
      .trType = MPT_STRING,
      .oid = MDMOID_DEV2_WIFI_SSID,
      .radioIdx = -1,
      .ssidIdx = -1,
   },
   {
      .cgiName = "wlGlobalMaxAssoc",
      .trName = "X_BROADCOM_COM_WlGlobalMaxAssoc",
      .trType = MPT_INTEGER,
      .oid = MDMOID_DEV2_WIFI_RADIO,
      .radioIdx = -1,
      .ssidIdx = -2,
   },
   {
      .cgiName = "wlMaxAssoc",
      .trName = "MaxAssociatedDevices",
      .trType = MPT_UNSIGNED_INTEGER,
      .oid = MDMOID_DEV2_WIFI_ACCESS_POINT,
      .radioIdx = -1,
      .ssidIdx = -1,
   },
   {
      .cgiName = "wlEnbl_2",  // does this always refer to second ssid of this radio?
      .trName = "Enable",
      .trType = MPT_BOOLEAN,
      .oid = MDMOID_DEV2_WIFI_SSID,
      .radioIdx = -1,
      .ssidIdx = 1,
   },
   {
      .cgiName = "wlSsid_2",
      .trName = "SSID",
      .trType = MPT_STRING,
      .oid = MDMOID_DEV2_WIFI_SSID,
      .radioIdx = -1,
      .ssidIdx = 1,
   },
};

#define NUM_AUTOSET_PARAMS (sizeof(wldmx_param_array)/sizeof(WLDMX_PARAM_ENTRY))

#define MAX_AUTOSET_ALLOC_COUNT  30
UINT32 currAutoSetIdx=0;
PhlSetParamValue_t *currAutoSetArray=NULL;


void BcmWl_initAutoSet_dev2()
{
   if (currAutoSetArray != NULL)
   {
      cmsLog_error("there is already an autoset in progress, currAutosetIdx=%d",
                   currAutoSetIdx);
      return;
   }

   currAutoSetArray = cmsMem_alloc(MAX_AUTOSET_ALLOC_COUNT * sizeof(PhlSetParamValue_t), ALLOC_ZEROIZE);
   if (currAutoSetArray == NULL)
   {
      cmsLog_error("mem allocation failed");
      return;
   }

   // should be 0, but set it again to be safe
   currAutoSetIdx = 0;
}


UBOOL8 BcmWl_autoSet_dev2(const char *varName, const char *varValue)
{
   UINT32 i=0;
   PhlSetParamValue_t *setParam;
   UBOOL8 found=FALSE;

   if (currAutoSetIdx >= MAX_AUTOSET_ALLOC_COUNT)
   {
      cmsLog_error("Too many params to set (%d), increase MAX_AUTOSET_ALLOC_COUNT",
                   currAutoSetIdx);
      return FALSE;
   }

   setParam = &(currAutoSetArray[currAutoSetIdx]);

   while (!found && i < NUM_AUTOSET_PARAMS)
   {
      if (!cmsUtl_strcmp(varName, wldmx_param_array[i].cgiName))
      {
         UINT32 r, s, oid;

         /* fill in a setParam entry for this variable */
         setParam->pValue = cmsMem_strdup(varValue);
         sprintf(setParam->pathDesc.paramName, "%s", wldmx_param_array[i].trName);
         setParam->pParamType = (char *) cmsMdm_paramTypeToString(wldmx_param_array[i].trType);
         setParam->pathDesc.oid = wldmx_param_array[i].oid;

         if (wldmx_param_array[i].ssidIdx == -2)
         {
            /* this is a radio only param, only need to use radio index */
            r = (wldmx_param_array[i].radioIdx == -1) ? wldmx_radioIdx :
                                              wldmx_param_array[i].radioIdx;
            oid = convertRadioIndexToIid(r);
         }
         else
         {
            r = (wldmx_param_array[i].radioIdx == -1) ? wldmx_radioIdx :
                                           wldmx_param_array[i].radioIdx;

            s = (wldmx_param_array[i].ssidIdx == -1) ? wldmx_ssidIdx :
                                           wldmx_param_array[i].ssidIdx;
            oid = convertSsidIndexToIid(r, s);
         }

         PUSH_INSTANCE_ID(&(setParam->pathDesc.iidStack), oid);

         cmsLog_error("XXX [%d] %s iidStack=%s value=%s",
                      currAutoSetIdx, varName,
                      cmsMdm_dumpIidStack(&(setParam->pathDesc.iidStack)),
                      setParam->pValue);

         currAutoSetIdx++;
         found = TRUE;
      }

      i++;
   }

   return found;
}


void BcmWl_flushAutoSet_dev2()
{
   CmsRet ret;
   UINT32 i=0;

   cmsLog_debug("Setting %d entries to MDM", currAutoSetIdx);
   ret = cmsPhl_setParameterValues(currAutoSetArray, (SINT32) currAutoSetIdx);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("setParamValues failed, ret=%d", ret);
   }

   /* free all the individual value bufs before freeing the entire array */
   for (i=0; i < currAutoSetIdx; i++)
   {
      cmsMem_free(currAutoSetArray[i].pValue);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(currAutoSetArray);
   currAutoSetIdx = 0;

   /* XXX In TR181, if we enable a SSID, we must also enable all the
    * QoS queues associated with this SSID and the default classifiers
    * which channel the various packet types into those queues.
    * In TR98, that was done from the wlmngr calling into CMS_DAL, but
    * in TR181, wlmngr knows nothing about CMS, CMS DAL, or MDM.  So this
    * file is responsible for enabling the associated queues and classifiers.
    */
   return;
}
#if 0

UBOOL8 BcmWl_autoGet_dev2(const char *varName, char *varValue)
{
   UINT32 i=0;
   MdmPathDescriptor pathDesc;
   PhlGetParamValue_t *getParamValueList=NULL;
   SINT32 numGetParamValues;
   UBOOL8 found=FALSE;
   CmsRet ret;

   INIT_PATH_DESCRIPTOR(&pathDesc);

   while (!found && i < NUM_AUTOSET_PARAMS)
   {
      if (!cmsUtl_strcmp(varName, wldmx_param_array[i].cgiName))
      {
         UINT32 r, s, oid;

         found = TRUE;

         /* fill in a setParam entry for this variable */
         pathDesc.oid = wldmx_param_array[i].oid;
         sprintf(pathDesc.paramName, "%s", wldmx_param_array[i].trName);

         if (wldmx_param_array[i].ssidIdx == -2)
         {
            /* this is a radio only param, only need to use radio index */
            r = (wldmx_param_array[i].radioIdx == -1) ? wldmx_radioIdx :
                                              wldmx_param_array[i].radioIdx;
            oid = convertRadioIndexToIid(r);
         }
         else
         {
            r = (wldmx_param_array[i].radioIdx == -1) ? wldmx_radioIdx :
                                           wldmx_param_array[i].radioIdx;

            s = (wldmx_param_array[i].ssidIdx == -1) ? wldmx_ssidIdx :
                                           wldmx_param_array[i].ssidIdx;
            oid = convertSsidIndexToIid(r, s);
         }

         PUSH_INSTANCE_ID(&(pathDesc.iidStack), oid);

         ret = cmsPhl_getParameterValues(&pathDesc, 1, TRUE,
                     &getParamValueList, &numGetParamValues);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("getParamValues failed, ret=%d", ret);
            sprintf(varValue, "0");
         }
         else
         {
            sprintf(varValue, "%s", getParamValueList->pValue);
            cmsLog_error("XXX returning %s (%s) value %s",
                         varName, cmsMdm_dumpIidStack(&(pathDesc.iidStack)),
                         varValue);
            cmsPhl_freeGetParamValueBuf(getParamValueList, numGetParamValues);
         }
      }

      i++;
   }

   return found;
}

#endif

/*
 * The WebUI and WL code thinks in terms of radio index and ssid index,
 * which is 0 based.
 * But in the MDM, all SSIDs are mapped to a single linear list of
 * SSIDs which covers multiple adapters.  AccessPoint info is in a
 * separate linear list.  So we need a function that maps index to instance id
 */
static UINT32 convertSsidIndexToIid(int radioIdx, int ssidIdx)
{
   /* for now, assume 4 SSID per radio */
   UINT32 ssidPerRadio = 4;

   /* SSID index 0 => oid 1 */
   return (radioIdx * ssidPerRadio) + (ssidIdx + 1);
}

static UINT32 convertRadioIndexToIid(int radioIdx)
{
   /* Radio index 0 => oid 1 */
   return radioIdx + 1;
}



#include "prctl.h"   // for runCommandInShell


void cgiWlMdm_getCountryList(int idx, int argc, char **argv, char *list)
{
   char *band = 0;

   if(argc >= 3) band = argv[2];

   if (!(*band == 'a' || *band == 'b')) {
      strcpy(list, "");
      return;
   }

#ifdef DESKTOP_LINUX
   if (idx > 10)
   {
      cmsLog_error("invalid idx %d", idx);
      strcpy(list, "");
            return;
   }
   sprintf(list, "%s\n%s\n%s\n%s\n",
         "document.forms[0].wlCountry[1] = new Option(\"#a\", \"#a\");",
         "document.forms[0].wlCountry[1] = new Option(\"#e\", \"#e\");",
         "document.forms[0].wlCountry[1] = new Option(\"#r\", \"#r\");",
         "document.forms[0].wlCountry[1] = new Option(\"UNITED STATES\", \"US\");");
#else
   char cmd[256];
   char name[64];
   char abbrv[64];
   char line[256];
   char *iloc, *eloc;
   int i, wsize;
   int cn = 0;
   char *tmpWlCountry="US";
   int matchedCount;

   /*
    * XXX The country list output should be stored in MDM.  This function
    * should just reformat the data to CGI/HTML.
    */
   sprintf(cmd, "wlctl -i wl%d country list %c > /var/wl%dclist", idx, *band, idx);
   prctl_runCommandInShellBlocking(cmd);
   sprintf(cmd, "/var/wl%dclist", idx );

   FILE *fp = fopen(cmd, "r");
   if ( fp != NULL ) {
      iloc = list;
      eloc = list + (2048-100);

      matchedCount = fscanf(fp, "%*[^\n]");   // skip first line
      if (matchedCount > 100)
      {
         // don't care about matched count, just make compiler happy
         cmsLog_debug("wow");
      }
      matchedCount = fscanf(fp, "%*1[\n]");
      if (matchedCount > 100)
      {
         // don't care about matched count, just make compiler happy
         cmsLog_debug("wow");
      }

      for (i=0;iloc<eloc;i++) {
         line[0]=0; abbrv[0]=0; name[0]=0;
         if (!fgets(line, sizeof(line), fp))
            break;
         if (sscanf(line, "%s %[^\n]s", abbrv, name) == -1) {
            break;
         }
         sprintf(iloc, "document.forms[0].wlCountry[%d] = new Option(\"%s\", \"%s\");\n%n", i, name[0]?name:abbrv, abbrv, &wsize);
         iloc += wsize;

         /* was !strcmp(m_instance_wl[idx].m_wlVar.wlCountry, abbrv)) */
         if (!strcmp("US", abbrv)) {
            cn =1;
         }
      }

      /* Case for country abbrv that is not in countrylist. e.g US/13, put it to last */
      /* hardcoded US instead of m_instance_wl[idx].m_wlVar.wlCountry */
      if (!cn && (tmpWlCountry != NULL) ) {
         sprintf(iloc, "document.forms[0].wlCountry[%d] = new Option(\"%s\", \"%s\");\n%n",
               i, tmpWlCountry, tmpWlCountry, &wsize);
         iloc += wsize;
      }

      *iloc = 0;
      fclose(fp);
      unlink(cmd);
   }
#endif  /* DESKTOP_LINUX */
}







//#endif  /* DMP_DEVICE2_WIFIRADIO_1 */

#endif  /* BRCM_WLAN */
