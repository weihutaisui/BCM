/*****************************************************************************
 *
 *  Copyright (c) 2005-2012  Broadcom Corporation
 *  All Rights Reserved
 *
 * <:label-BRCM:2012:proprietary:standard
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
 * :>
 *
 ************************************************************************/

#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "igd.h"
#include "deviceconfigparams.h"

#include "session.h"
#include "os_defs.h"
#include "uuid.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

extern pSessionInfo pCurrentSession;
extern void *msgHandle;
extern void delayed_call(uint seconds, voidfp_t f);

int DeviceConfig_Init(PService psvc, service_state_t state)
{
   switch (state) 
   {
   case SERVICE_CREATE:
      psvc->opaque = NULL;
      break;

   case SERVICE_DESTROY:
      break;
   }
   
   return TRUE;
}

int DeviceConfig_GetVar(struct Service *psvc, int varindex)
{
   struct StateVar *var;
   var = &(psvc->vars[varindex]);

   switch (varindex) 
   {
   case VAR_PersistenData:
      break;

   case VAR_ConfigFile:
      break;

   case VAR_AStatus:
      strcpy(var->value,"ChangesApplied");
      break;

   case VAR_AUUID:
      strcpy(var->value,"");
      break;
   }
   return TRUE;
}

int ConfigurationStarted(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   struct Param *pParams;

#ifdef UUID_TYPE
   /* assuming that it comes in as uuid_t record as defined in the UUID spec */
   uuid_t sessionId;
#else
   /* assuming it comes in as a 16 bytes- hex octects, with or without -; ignore - */
   char sessionId[TR64_SSID_LEN];
#endif

   UPNP_TRACE(("ConfigurationStarted(entry)\n"));

   /* The only input parameter for this action is SessionID 
    */
   pParams = findActionParamByRelatedVar(ac,VAR_AUUID);
   
   if (pParams == FALSE)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

#ifdef UUID_TYPE
#else
   /* call routine to parse and ignore -; put it array of 16 bytes octets */
   if (sidStrToUuid(pParams->value,sessionId) == -1)
   {
      soap_error(uclient,SOAP_INVALIDARGS);
   }
   else
   {
      /**Start of a session with the specified UUID as sessionID.  Lock.
       * when we get to here, it means this is the first session; otherwise, event is queued up
       * for later processing
       **/
      createSession(sessionId);
   }
#endif
   return TRUE;

} /* ConfigurationStarted */

int ConfigurationFinished(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   if ((pCurrentSession == NULL) ||
       ((pCurrentSession != NULL) && (pCurrentSession->state == SESSION_EXPIRED)))
   {
      /* we should not get here because doAuthentication has taken care of this */
      soap_error(uclient,SOAP_SESSIONIDEXPIRED);
      return FALSE;
   }
   else 
   {
      /* new state will signal reaper to process waitList if any */
      pCurrentSession->state = SESSION_EXPIRED;
      if (pCurrentSession->configStatus == REBOOT_REQUIRED)
      {
         OutputCharValueToAC(ac,VAR_AStatus,"RebootRequired");
      }
      else
      {
         OutputCharValueToAC(ac,VAR_AStatus,"ChangesApplied");
      }
   }

   return TRUE;
   
} /* ConfigurationFinished */


void Tr64ResetDelay(void)
{
   CmsRet ret;

   UPNP_TRACE(("Tr64ResetDelay():\n"));

   if ((pCurrentSession != NULL) && (pCurrentSession->eventList.count > 0))
   {
      /* get ready for reboot; let session reaper start processing the event in evenList */
      pCurrentSession->state = SESSION_EXPIRED;
      delayed_call(2,Tr64ResetDelay);
   }
   else
   {
      if ((ret = cmsLck_acquireLockWithTimeout(TR64C_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get lock, ret=%d", ret);
         return ;
      }

      cmsMgm_invalidateConfigFlash();

      cmsLck_releaseLock(); 

      cmsUtil_sendRequestRebootMsg(msgHandle);
   }
} /* Tr64ResetDelay */

int Tr64FactoryReset(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   UPNP_TRACE(("Tr64FactoryReset()\n"));

   delayed_call(2,Tr64ResetDelay);

   return TRUE;
}  /* Tr64FactoryReset */

void Tr64RebootDelay(void)
{
   UPNP_TRACE(("Tr64RebootDelay():\n"));

   if ((pCurrentSession != NULL) && (pCurrentSession->eventList.count > 0))
   {
      /* get ready for reboot; let session reaper start processing the event in evenList */
      pCurrentSession->state = SESSION_EXPIRED;
      delayed_call(2,Tr64RebootDelay);
   }
   else
   {
      cmsUtil_sendRequestRebootMsg(msgHandle);
   }
} /* Tr64RebootDelay */

int Tr64Reboot(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   /* reboot needs check to see if any event is left to be processed after a session
      locking; if there is, put it in a delay timer */

   UPNP_TRACE(("Tr64Reboot(entry)\n"));

   delayed_call(1,Tr64RebootDelay);

   return TRUE;
}  /* Tr64Reboot */

int GetConfigFile(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   UINT32 cfgBufLen;
   char *cfgBuf;
   CmsRet ret;
   cfgBufLen = cmsImg_getConfigFlashSize();
   
   if ((cfgBuf = cmsMem_alloc(cfgBufLen, ALLOC_ZEROIZE)) == NULL)
   {
      cmsLog_error("could not allocate %u bytes", cfgBufLen);
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;    
   }
   
   if ((ret = cmsMgm_readConfigFlashToBuf(cfgBuf, &cfgBufLen)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to read config flash into buf, ret=%d", ret);
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE; 
   }

   if(OutputCharValueToAC(ac, VAR_ConfigFile, cfgBuf))
   {
      cmsMem_free(cfgBuf);
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;       
   } 

   cmsMem_free(cfgBuf);
   return TRUE;
}

static void xmlDecodeBuffer(char *ps, char *input, int len)
{
   char *pIn = input; /* traverse through input string */
   int left = len;

   if (pIn) 
   {
      while (left > 0)
      {
         if (strncmp(pIn,"&amp;",5) == 0) 
         {
            *ps++ = '&';
            left -= 5;
            pIn += 5;
         }
         else if (strncmp(pIn,"&lt;",4) == 0) 
         {
            *ps++ = '<';
            left -= 4;
            pIn += 4;
         }
         else if (strncmp(pIn,"&gt;",4) == 0) 
         {
            *ps++ = '>';
            left -= 4;
            pIn += 4;
         }
         else if (strncmp(pIn,"&quot;",6) == 0) 
         {
            *ps++ = '"';
            left -= 6;
            pIn += 6;
         }
         else if (strncmp(pIn,"&apos;",6) == 0) 
         {
            *ps++ = '\\';
            left -= 6;
            pIn += 6;
         }
         else if (strncmp(pIn,"&#9;",4) == 0) 
         {
            *ps++ = '\t'; /* tab */
            left -= 4;
            pIn += 4;
         }
         else if (strncmp(pIn,"&#10;",5) == 0) 
         {
            *ps++ = '\r'; /* lf */
            left -= 5;
            pIn += 5;
         }
         else if (strncmp(pIn,"&#13;",5) == 0) 
         {
            *ps++ = '\n'; /* cr */
            left -= 5;
            pIn += 5;
         }
         else
         {
            *ps++ = *pIn;
            left -= 1;
            pIn += 1;
         }
      } /* while */
   } /* if pIn != NULL */
} /* xmlDecodeBuffer */


int SetConfigFile(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   CmsImageFormat format;
   struct Param *pParams;   
   unsigned int configLen = 32 * 1024;
   CmsRet ret;
   
   pParams = findActionParamByRelatedVar(ac,VAR_ConfigFile);
   if (pParams != NULL)
   {
      char *buffer;

      buffer = (char*) malloc(configLen);
      if (buffer == NULL)
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }

      memset(buffer,0,configLen);

      xmlDecodeBuffer(buffer, pParams->value, strlen(pParams->value));

      cmsLog_debug("image uploaded, now determine format and validate");

      cmsLck_releaseLock();

      format = cmsImg_validateImage(buffer, strlen(buffer), msgHandle);    
   
      if (format == CMS_IMAGE_FORMAT_INVALID)
      {
         cmsLog_error("invalid image format");
         free(buffer);
         soap_error( uclient, SOAP_ACTIONFAILED );
         cmsLck_acquireLockWithTimeout(TR64C_LOCK_TIMEOUT);
         return FALSE;
      }
      else
      {
         cmsLog_debug("got recognized and valid format %d", format);
      }

      ret = cmsImg_writeValidatedImage(buffer, strlen(buffer), format, msgHandle);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("write of validated image failed, ret=%d", ret);
      }
      else
      {
         /*
          * In the modem, cmsImg_writeValidatedImage() will trigger a 
          * reboot.  On the desktop, we will still be here.
          */
         cmsLog_debug("image written");
      }

   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      cmsLck_acquireLockWithTimeout(TR64C_LOCK_TIMEOUT);
      return FALSE;
   }

   cmsLck_acquireLockWithTimeout(TR64C_LOCK_TIMEOUT);
   return TRUE;
}
