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

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include <time.h>

#include "cgi_util.h"
#include "cgi_main.h"
#include "syscall.h"
#include "sysdiag.h"
#include "cms_util.h"
#include "cms_log.h"
#include "cms_msg.h"
#include "cms_dal.h"
#include "cms_qdm.h"
#include "httpd.h"
#include "cgi_ntwk.h"
#include "cgi_route.h"
#include "cms_boardcmds.h"
#include "cms_seclog.h"
#include "oam_util.h"


#ifdef BRCM_WLAN
int wlgetintfNo( void );
#endif



/* Needed by cgiGetGponWanType and cgiGetEponWanType.  httpd should not
 * call rutWan_xxx directly.  This function should get moved to cmsUtil lib.
 */
extern CmsRet rutWan_getOpticalWanType(char * optIfWanType);
static void cgiGetGponWanType(char *varValue);
static void cgiGetEponWanType(char *varValue);
#ifdef DMP_X_BROADCOM_COM_EPON_1
static void cgiIsEponLoidSupported(char *varValue);
#endif


int glbIsOutOfVcc = 0; // 1 if number of PVCs hits its limitation
extern int glbIsVccInUsed;        // this global var is defined in cgimain.c



void do_test_cgi(char *path, FILE *fs) {
   char filename[WEB_BUF_SIZE_MAX];
   char* query = NULL;
   char* ext = NULL;

   query = strchr(path, '?');
   if ( query != NULL )
      cgiTestParseSet(path);

   filename[0] = '\0';
   ext = strchr(path, '.');
   if ( ext != NULL ) {
      *ext = '\0';
      strcpy(filename, path);
      strcat(filename, ".html");
      do_ej(filename, fs);
   }
}



void cgiGetTestVar_igd(char *varName, char *varValue) 
{
   InstanceIdStack   wanDevIid=EMPTY_INSTANCE_ID_STACK;
   WanDslIntfCfgObject *dslIntfCfg = NULL;
   WanBertTestObject *bertTestObj = NULL;
   int found = 0;
   double ratio;

   cmsLog_debug("enter: varName=%s", varName);
   while (!found && (cmsObj_getNext(MDMOID_WAN_DSL_INTF_CFG, &wanDevIid, (void **)&dslIntfCfg) == CMSRET_SUCCESS))
   {
      /*
       * We are looking for a DSLIntfCfg which is really a DSL line and has some kind of signal
       */
      if (dslIntfCfg->enable &&
          cmsUtl_strcmp(dslIntfCfg->status, MDMVS_DISABLED))
      {
         found = TRUE;
      }
      cmsObj_free((void **)&dslIntfCfg);  /* no longer needed */

         if( found )
         {
            if ((cmsObj_get(MDMOID_WAN_BERT_TEST, &wanDevIid, 0, (void **) &bertTestObj)) == CMSRET_SUCCESS)
            {
               if ( cmsUtl_strcmp(varName, "berTime") == 0 )
               {
                  sprintf(varValue, "%u", bertTestObj->totalTime);
               }
               else if ( cmsUtl_strcmp(varName, "berState") == 0 )
               {
                  if (cmsUtl_strcmp(bertTestObj->bertTestStatus,MDMVS_NOT_RUNNING) == 0)
                  {
                     strcpy(varValue,"0");
                  }
                  else
                  {
                     strcpy(varValue,"1");
                  }
               }
               else if ( cmsUtl_strcmp(varName, "berTotalBits") == 0 )
               {
                  sprintf(varValue, "0x%08X%08X", bertTestObj->bitsTestedCntHigh,
                          bertTestObj->bitsTestedCntLow);
               }
               else if ( cmsUtl_strcmp(varName, "berErrorBits") == 0 )
               {
                  sprintf(varValue, "0x%08X%08X", bertTestObj->errBitsCntHigh,bertTestObj->errBitsCntLow);
               }
               else if ( cmsUtl_strcmp(varName, "berErrorRatio") == 0 ) 
               {
                  unsigned long long bertTotalBits64 = (unsigned long long)bertTestObj->bitsTestedCntHigh;
                  unsigned long long bertErrBits64 = (unsigned long long)bertTestObj->errBitsCntHigh;
                  unsigned long long *err64, *tot64;

                  bertTotalBits64 = (bertTotalBits64 << 32) | bertTestObj->bitsTestedCntLow;
                  bertErrBits64 = (bertErrBits64 << 32) | bertTestObj->errBitsCntLow;
                  err64 = (unsigned long long *) &bertErrBits64;
                  tot64 = (unsigned long long *) &bertTotalBits64;
                  if ( *tot64 != 0 ) 
                  {
                     ratio = (double)(*err64) / (double)(*tot64);
                     sprintf(varValue, "%4.2e", ratio);
                  } 
                  else
                  {
                     strcpy(varValue, "Not Applicable");
                  }
               }
               else if ( cmsUtl_strcmp(varName, "adslTestMode") == 0 )
               {
                  sprintf(varValue, "%s", bertTestObj->bertTestMode);
               }
               else
               {
                  strcpy(varValue, "");
               }
               cmsObj_free((void **) &bertTestObj);
            } /* bertObj */
         } /* if (found) */
      } /* while (!found) */
   return;
}

void cgiSetTestVar_igd(char *varName, char *varValue) 
{
   InstanceIdStack   wanDevIid=EMPTY_INSTANCE_ID_STACK;
   WanDslIntfCfgObject *dslIntfCfg = NULL;
   WanBertTestObject *bertTestObj = NULL;
   int found = 0;
   CmsRet ret;
   int time;

   cmsLog_debug("enter: varName=%s", varName);

   while (!found && (cmsObj_getNext(MDMOID_WAN_DSL_INTF_CFG, &wanDevIid, (void **)&dslIntfCfg) == CMSRET_SUCCESS))
   {
      /*
       * We are looking for a DSL line that is UP.
       */
      found = !cmsUtl_strcmp(dslIntfCfg->status, MDMVS_UP);
      cmsObj_free((void **)&dslIntfCfg);  /* no longer needed */

         if (found)
         {
            if ((cmsObj_get(MDMOID_WAN_BERT_TEST, &wanDevIid, 0, (void **) &bertTestObj)) == CMSRET_SUCCESS)
            {
               /* it's already running, don't allow parameter change except user wants to stop the test */
               if ( ( (cmsUtl_strcmp(bertTestObj->bertTestStatus,MDMVS_RUNNING) == 0) &&
                      ((cmsUtl_strcmp(varName,"berState") == 0) && (cmsUtl_strcmp(varValue,"0") != 0)) 
                      ) ||
                    ( (cmsUtl_strcmp(bertTestObj->bertTestStatus,MDMVS_RUNNING) == 0) &&
                      (cmsUtl_strcmp(varName,"berState") != 0) 
                      ) )

               {
                  cmsObj_free((void **) &bertTestObj);
                  return;
               }

               if ( cmsUtl_strcmp(varName, "berTime") == 0 )
               {
                  time = atol(varValue);
                  bertTestObj->bertTestDuration = time;
               }
               else if ( cmsUtl_strcmp(varName, "berState") == 0 )
               {         
                  if (cmsUtl_strcmp(varValue,"0") == 0 )
                  {
                     /* if it's already stopped, just return */
                     if (cmsUtl_strcmp(bertTestObj->bertTestMode,MDMVS_STOP) == 0)
                     {
                        cmsObj_free((void **) &bertTestObj);
                        return;
                     }
                     cmsMem_free(bertTestObj->bertTestMode);
                     bertTestObj->bertTestMode = cmsMem_strdup(MDMVS_STOP);
                  }
                  else
                  {
                     /* starting, reset bertTime timer for user to set it */
                     cmsMem_free(bertTestObj->bertTestMode);
                     bertTestObj->bertTestMode = cmsMem_strdup(MDMVS_START);
                     bertTestObj->bertTestDuration = 0;
                  }
               }

               if ((ret = cmsObj_set(bertTestObj, &wanDevIid)) != CMSRET_SUCCESS)
               {
                  cmsLog_error("cmsObj_set returns error ret %d",ret);
               }
               cmsObj_free((void **) &bertTestObj);
            } /* bertObj */
         } /* if (found) */
   } /* while (!found) */

   return;
}

CmsRet cgiConfigPassword(void)
{
   CmsRet ret;
   NetworkAccessMode accessMode;
   UBOOL8 bPwdValid;
   UBOOL8 bUpdatePwd = FALSE;
   char  *varName = NULL;
   char  *pPassWd;
   CmsSecurityLogData logData = EMPTY_CMS_SECURITY_LOG_DATA;
   HttpLoginType authLevel = LOGIN_INVALID;
   
   /* already have lock */
   if ((ret = cmsDal_getCurrentLoginCfg(&glbWebVar)) != CMSRET_SUCCESS)
   {
      printf("Could not get current password info, ret=%d\n", ret);
      return ret;
   }

   CMSLOG_SEC_SET_PORT(&logData, HTTPD_PORT);
   CMSLOG_SEC_SET_APP_NAME(&logData, "HTTP");
   CMSLOG_SEC_SET_USER(&logData, &glbWebVar.curUserName[0]);
   CMSLOG_SEC_SET_SRC_IP(&logData, &glbWebVar.pcIpAddr[0]);

   accessMode = NETWORK_ACCESS_DISABLED;
   if (0 == strcmp(glbWebVar.inUserName, glbWebVar.adminUserName))
   {
      if ( 0 == strcmp(glbWebVar.curUserName, glbWebVar.adminUserName) )
      {
         /* for verification we need to specify the network access
            based on the user who's password is being changed and not
            the user who is changing the password */
         accessMode = NETWORK_ACCESS_LAN_SIDE;
         bUpdatePwd = TRUE;
         varName    = "adminPassword";
      }
      else
      {
         cmsLog_security(LOG_SECURITY_PWD_CHANGE_FAIL, &logData, "Account %s", glbWebVar.inUserName);
      }
   }
   else if (0 == strcmp(glbWebVar.inUserName, glbWebVar.sptUserName))
   {
      if ( (0 == strcmp(glbWebVar.curUserName, glbWebVar.adminUserName)) ||
           (0 == strcmp(glbWebVar.curUserName, glbWebVar.sptUserName)) )
      {
         accessMode = NETWORK_ACCESS_WAN_SIDE;
         bUpdatePwd = TRUE;
         varName    = "sptPassword";
      }
      else
      {
         cmsLog_security(LOG_SECURITY_PWD_CHANGE_FAIL, &logData, "Account %s", glbWebVar.inUserName);
      }
   }
   else if (0 == strcmp(glbWebVar.inUserName, glbWebVar.usrUserName))
   {
      if ( (0 == strcmp(glbWebVar.curUserName, glbWebVar.adminUserName)) ||
           (0 == strcmp(glbWebVar.curUserName, glbWebVar.usrUserName)) )
      {
         accessMode = NETWORK_ACCESS_LAN_SIDE;
         bUpdatePwd = TRUE;
         varName    = "usrPassword";
      }
      else
      {
         cmsLog_security(LOG_SECURITY_PWD_CHANGE_FAIL, &logData, "Account %s", glbWebVar.inUserName);
      }
   }
   else
   {
      cmsLog_security(LOG_SECURITY_PWD_CHANGE_FAIL, &logData, "Invalid user");
   }

   ret = CMSRET_INVALID_PARAM_VALUE;
   if ( bUpdatePwd )
   {
      /* verify information passed in 
         already have lock */
      bPwdValid  = cmsDal_authenticate(&authLevel, accessMode, &glbWebVar.inUserName[0], &glbWebVar.inOrgPassword[0]);
      if ( TRUE == bPwdValid )
      {
#ifdef SUPPORT_HASHED_PASSWORDS
         pPassWd = cmsUtil_pwEncrypt(&glbWebVar.inPassword[0], cmsUtil_cryptMakeSalt());
#else
         pPassWd = &glbWebVar.inPassword[0];
#endif
         cgiSetVar(varName, pPassWd);
         if ((ret = cmsDal_setLoginCfg(&glbWebVar)) != CMSRET_SUCCESS)
         {
            cmsLog_error("setLoginCfg failed, ret=%d", ret);
         }
         cmsLog_security(LOG_SECURITY_PWD_CHANGE_SUCCESS, &logData, "Account %s", glbWebVar.inUserName);
      }
      else
      {
         cmsLog_security(LOG_SECURITY_PWD_CHANGE_FAIL, &logData, "Invalid password");
      }
   }

   return ret;
}


CmsRet cgiConfigSysLog(void)
{
   CmsRet ret;

   if ((ret = cmsDal_setSyslogCfg(&glbWebVar)) != CMSRET_SUCCESS)
   {
      cmsLog_error("set syslogCfg failed, ret=%d", ret);
   }

   return ret;
}

#if defined(DMP_TIME_1) || defined(DMP_DEVICE2_TIME_1)
CmsRet cgiConfigNtp(void)
{
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_notice("setting NTP config");

   if ((ret = cmsDal_setNtpCfg(&glbWebVar)) != CMSRET_SUCCESS)
   {
      cmsLog_error("set of NTP config failed, ret=%d", ret);
      return ret;
   }

   return ret;
}
#endif


#ifdef DMP_X_BROADCOM_COM_SNMP_1

CmsRet cgiConfigSnmp(void) 
{
   CmsRet ret = CMSRET_SUCCESS;
   
   cmsLog_notice("setting SNMP config");

   if ((ret = cmsDal_setSnmpCfg(&glbWebVar)) != CMSRET_SUCCESS)
   {
      cmsLog_error("set of SNMP config failed, ret=%d", ret);
      return ret;
   }

   return ret;
}

#endif /* DMP_X_BROADCOM_COM_SNMP_1 */



#ifdef DMP_X_BROADCOM_COM_DBUSREMOTE_1
CmsRet cgiConfigDbusremote(void)
{
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_notice("setting Dbusremote config");

   if ((ret = cmsDal_setDbusremoteCfg(&glbWebVar)) != CMSRET_SUCCESS)
   {
      cmsLog_error("set of Dbusremote config failed, ret=%d", ret);
      return ret;
   }

   return ret;
}
#endif /* DMP_X_BROADCOM_COM_DBUSREMOTE_1 */


#ifdef SUPPORT_TR69C

CmsRet cgiTr69cConfig(void)
{
   CmsRet ret;

   cmsLog_notice("setting tr69c config");

   if ((ret = cmsDal_setTr69cCfg(&glbWebVar)) != CMSRET_SUCCESS)
   {
      cmsLog_error("set of tr69c config failed, ret=%d", ret);
   }

   return ret;
}


/** This is some diagnostic functionality required by the TR69 spec.
 */
void cgiTr69cGetRpcMethods(void)
{
   CmsMsgHeader msg=EMPTY_MSG_HEADER;
   CmsRet ret;

   cmsLog_notice("Sending message to tr69c so it can send GetRpcMethods to ACS");

   msg.type = CMS_MSG_TR69_GETRPCMETHODS_DIAG;
   msg.src = EID_HTTPD;
   msg.dst = EID_SMD;
   msg.flags_event = 1;

   ret = cmsMsg_send(msgHandle, &msg);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMsg_send returned error %d", ret);
   }
}
#endif  /* SUPPORT_TR69C */

#ifdef DMP_X_ITU_ORG_GPON_1 /* aka SUPPORT_OMCI */
CmsRet cgiOmciSystem(void)
{
   CmsRet ret;

   cmsLog_notice("setting OMCI system");

   if ((ret = cmsDal_setOmciSystem(&glbWebVar)) != CMSRET_SUCCESS)
   {
      cmsLog_error("set of OMCI system failed, ret=%d", ret);
   }

   return ret;
}
#endif  /* DMP_X_ITU_ORG_GPON_1 */


void cgiWriteMessagePage(FILE *fs, char *title,
                         char *msg, char *location) {
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "<title></title>\n");

   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n");
   fprintf(fs, "function btnBack() {\n");
   fprintf(fs, "   var code = 'location=\"%s\"';\n", location);
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n");
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");

   fprintf(fs, "</head>\n");
   fprintf(fs, "<body>\n<blockquote>\n<form>\n");
   fprintf(fs, "<b>%s</b><br><br>\n", title);
   fprintf(fs, "%s<br><br>\n", msg);

   if ( location != NULL ) {
      fprintf(fs, "<center>\n");
      fprintf(fs, "<input type='button' " \
                    "value='&nbsp;Back&nbsp;' " \
                    "onClick='btnBack()'>\n");
      fprintf(fs, "</center>\n");
   }

   fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
   fflush(fs);
}

void do_auth(char *userid __attribute__((unused)),
             char *passwd __attribute__((unused)),
             char *realm __attribute__((unused)),
             char *sptPasswd __attribute__((unused)),
             char *usrPasswd __attribute__((unused)))
{
   /*
    * No need to load the username and passwords.
    * The cmsDal_authenticate() function does it all now.
    * Copying the realm has been moved into httpd.c
    */
#ifdef obsolete_in_cms
//   strcpy(userid, glbWebVar.adminUserName);
//   strcpy(passwd, glbWebVar.adminPassword);
//   strcpy(sptPasswd, glbWebVar.sptPassword);
//   strcpy(usrPasswd, glbWebVar.usrPassword);
//   strcpy(realm, "DSL Router");
#endif
}

/* Converts hexadecimal to decimal (character): */

char hexToDec(char *what) {
   char digit;

   digit = (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A')+10 : (what[0] - '0'));
   digit *= 16;
   digit += (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A')+10 : (what[1] - '0'));

   return (digit);
}

/* Unescapes "%"-escaped characters in a query: */

void unescapeUrl(char *url) {
   int x,y,len;

   len = strlen(url);

   for ( x = 0, y = 0; url[y]; x++, y++) {
      if ( (url[x] = url[y]) == '%' &&
           y < (len - 2) ) {
         url[x] = hexToDec(&url[y+1]);
         y += 2;
      }
   }
   url[x] = '\0';
}

void cgiUrlDecode(char *s) {
   char *pstr = s;

   /* convert plus (+) to space (' ') */
   for ( pstr = s;
          pstr != NULL && *pstr != '\0';
          pstr++ )
      if ( *pstr == '+' ) *pstr = ' ';

   unescapeUrl(s);
}

void cgiParseSet(char *path) {
   char *query = strchr(path, '?');
   char *name, *value, *next;

   /* Parse name=value&name=value& ... &name=value */
   if (query) {
      for (value = ++query; value; value = next) {
         name = strsep(&value, "=");
         if (name) {
            next = value;
            value = strsep(&next, "&");
            if (!value) {
               value = next;
               next = NULL;
            }
            cgiUrlDecode(value);
            cgiSetVar(name, value);
         } else
            next = NULL;
      }
   }
}

void cgiTestParseSet(char *path) {
   char *query = strchr(path, '?');
   char *name, *value, *next;

   /* Parse name=value&name=value& ... &name=value */
   if (query) {
      for (value = ++query; value; value = next) {
         name = strsep(&value, "=");
         if (name) {
            next = value;
            value = strsep(&next, "&");
            if (!value) {
               value = next;
               next = NULL;
            }
            cgiUrlDecode(value);
            cgiSetTestVar(name, value);
         } else
            next = NULL;
      }
   }
}


/* 
 * cgiGetValueByNameSafe(): Get parameter value by identification from httpd input.
 * (see the usage in cgi_main.h)
 */
CGI_STATUS  cgiGetValueByNameSafe( const char *query, const char *id, char ** buf, char *val, int len) 
{
   int ret = CGI_STS_ERR_GENERAL;
   int idlen, querylen;
   int index = 0;
   int buflen = 0;
   char *name = NULL, *value = NULL, *pc = NULL, *temp = NULL, *nextId = NULL;

   if (val != NULL)
   {
      val[0] = '\0';
   }

   /* validate & initialize return value */
   if ( ((val == NULL) && (buf == NULL)) || ((val != NULL) && (buf != NULL)) ||
        ((val != NULL) && (len == 0)) || ((buf != NULL) && (len > 0)) ) 
   {
      return ret;
   }

   if ( query == NULL || id == NULL) return ret;

   if ( *query =='\0' || *id == '\0') return ret;

   /* search for the given id */
   /* Parse name=value&name=value& ... &name=value */
   name = strstr(query, id);
   idlen = strlen(id);
   while ( name != NULL ) {
	 if (name[idlen] != '=')  
             name = strstr(name+idlen, id);
         else
	     break;
   }	 
   
   if ( name == NULL ) return CGI_STS_ERR_FIND;

   /* calculate the amount of memory */
   nextId = strchr(name, '&');
   if ( nextId != NULL )
   {
      buflen = nextId - name - idlen;
   }
   else
   {
      querylen = strlen(query);
      buflen = querylen - ((name - query) + idlen );
   }

   if ( (len > 0) && (len < buflen) )
   {
      cmsLog_error("memory overflow %d", len);
      *val = '\0';
      return CGI_STS_ERR_GENERAL;
   }

   temp = (char *)cmsMem_alloc(buflen, ALLOC_ZEROIZE);

   if (temp != NULL) {
      value = name+idlen;
      for ( pc = temp, value++;
                value != NULL && *value != '&' && *value != '\0';
                pc++, value++, index++ )
      {
         *pc = *value;
      }
      *pc = '\0';
      cgiUrlDecode(temp);

      if ( len != 0 )
      {
         memcpy(val, temp, buflen);
      }
      else
      {
         *buf = (char *)cmsMem_alloc(buflen, ALLOC_ZEROIZE);
         memcpy(*buf, temp, buflen);
      }

      cmsMem_free(temp);
   }

   return CGI_STS_OK;
}

#include "boardparms.h"
extern int devCtl_getGponOpticsType(void);
static int isPmdExist(void)
{
   unsigned short gpon_optics_type;

   gpon_optics_type = devCtl_getGponOpticsType();
   return (BP_GPON_OPTICS_TYPE_PMD == gpon_optics_type);
}

void cgiGetSysInfo(int argc __attribute__((unused)),
                   char **argv, char *varValue) {

   if ( cmsUtl_strcmp(argv[2], "noBrPrtcl") == 0 )
   {
      dalWan_allBridgePrtcl(glbWebVar.brName, varValue);
   }
   else if ( cmsUtl_strcmp(argv[2], "brPrtcl") == 0 )
   {
      sprintf(varValue, "%d", qdmIpIntf_isBridgedWanExistedLocked());
   }
   else if ( cmsUtl_strcmp(argv[2], "uptimeString") == 0 )
   {
      getUptimeString(varValue);
   }
   else if ( cmsUtl_strcmp(argv[2], "memSize") == 0 )
      sprintf(varValue, "%d", devCtl_getSdramSize());
   else if ( cmsUtl_strcmp(argv[2], "numCpuThreads") == 0 )
      sprintf(varValue, "%d", glbWebVar.numCpuThreads);
   else if ( cmsUtl_strcmp(argv[2], "dns1") == 0 )
      cgiGetActiveDnsIp(varValue, NULL);
   else if ( cmsUtl_strcmp(argv[2], "dns2") == 0 )
      cgiGetActiveDnsIp(NULL, varValue);
   else if ( cmsUtl_strcmp(argv[2], "dnsIfcs") == 0 )
      cgiGetDnsInterfacesList(varValue);
   else if ( cmsUtl_strcmp(argv[2], "dnsPrimary") == 0 )
      cgiGetDnsStaticIp(varValue, NULL, TRUE);
   else if ( cmsUtl_strcmp(argv[2], "dnsSecondary") == 0 )
      cgiGetDnsStaticIp(NULL, varValue, TRUE);      
   else if ( cmsUtl_strcmp(argv[2], "lanAddr") == 0 )
      dalLan_getIpIntfInfo(FALSE, "br0", varValue, NULL, NULL);
#ifdef SUPPORT_WAN_HTML
   else if ( cmsUtl_strcmp(argv[2], "dfltGwIfc") == 0 )         /* get active default gateway */
      dalRt_getActiveDefaultGateway(varValue);
   else if ( cmsUtl_strcmp(argv[2], "gtwyIfcs") == 0 )         /* get default gateway list */
      cgiGetDefaultGatewList(varValue);
#endif
   else if ( cmsUtl_strcmp(argv[2], "enblNat") == 0 )
      cmsDal_getEnblNatForWeb(varValue);
   else if ( cmsUtl_strcmp(argv[2], "enblFullcone") == 0 )
      cmsDal_getEnblFullconeForWeb(varValue);
   else if ( cmsUtl_strcmp(argv[2], "enblFirewall") == 0 )
      cgiGetEnblFirewallForWeb(varValue);
   else if ( cmsUtl_strcmp(argv[2], "pppIpExtension") == 0 )
      cgiGetPppIpExtensionForWeb(varValue);
#if defined(DMP_X_BROADCOM_COM_IGMPSNOOP_1)
   else if ( cmsUtl_strcmp(argv[2], "igmpSnpSup") == 0 )
      sprintf(varValue, "1");
   else if ( cmsUtl_strcmp(argv[2], "enblIgmpSnp") == 0 )
      dalLan_getIgmpSnoopingEnabled(glbWebVar.brName, varValue);
   else if ( cmsUtl_strcmp(argv[2], "enblIgmpMode") == 0 )
      dalLan_getIgmpSnoopingMode(glbWebVar.brName, varValue);
   else if ( cmsUtl_strcmp(argv[2], "enblIgmpLanToLanMcast") == 0 )
      dalLan_getIgmpLanToLanMcastEnable (glbWebVar.brName, varValue);
#else
   else if ( cmsUtl_strcmp(argv[2], "igmpSnpSup") == 0 )
      sprintf(varValue, "0");
#endif
#ifdef SUPPORT_IPV6
   else if ( cmsUtl_strcmp(argv[2], "lanULAAddr6") == 0 )
      qdmIpv6_getLanULAAddr6("br0", varValue);
   else if ( cmsUtl_strcmp(argv[2], "dns6Type") == 0 )
      qdmIpv6_getDns6Info(varValue, NULL, NULL, NULL);
   else if ( cmsUtl_strcmp(argv[2], "dns6Ifc") == 0 )
      qdmIpv6_getDns6Info(NULL, varValue, NULL, NULL);
   else if ( cmsUtl_strcmp(argv[2], "dns6Pri") == 0 )
      cgiGetDnsStaticIp(varValue, NULL, FALSE);
   else if ( cmsUtl_strcmp(argv[2], "dns6Sec") == 0 )
      cgiGetDnsStaticIp(NULL, varValue, FALSE);
   else if ( cmsUtl_strcmp(argv[2], "sitePrefixType") == 0 )
      qdmIpv6_getSitePrefixInfo(varValue, NULL, NULL);
   else if ( cmsUtl_strcmp(argv[2], "pdWanIfc") == 0 )
      qdmIpv6_getSitePrefixInfo(NULL, varValue, NULL);
   else if ( cmsUtl_strcmp(argv[2], "sitePrefix") == 0 )
      qdmIpv6_getSitePrefixInfo(NULL, NULL, varValue);
   else if ( cmsUtl_strcmp(argv[2], "dfltGw6Ifc") == 0 )
      qdmRt_getSysDfltGw6(varValue, NULL);
   else if ( cmsUtl_strcmp(argv[2], "dfltGw6") == 0 )
      qdmRt_getSysDfltGw6(NULL, varValue);
#if defined (DMP_X_BROADCOM_COM_MLDSNOOP_1)
   else if ( cmsUtl_strcmp(argv[2], "mldSnpSup") == 0 )
      sprintf(varValue, "1");
   else if ( cmsUtl_strcmp(argv[2], "enblMldSnp") == 0 ) 
      dalLan_getMldSnoopingEnabled(glbWebVar.brName, varValue);
   else if ( cmsUtl_strcmp(argv[2], "enblMldMode") == 0 ) 
      dalLan_getMldSnoopingMode(glbWebVar.brName, varValue);
   else if ( cmsUtl_strcmp(argv[2], "enblMldLanToLanMcast") == 0 )
      dalLan_getMldLanToLanMulticastEnable (glbWebVar.brName,varValue);
#else
   else if ( cmsUtl_strcmp(argv[2], "mldSnpSup") == 0 )
      sprintf(varValue, "0");
#endif
#endif
#ifdef SUPPORT_IPP
   else if ( cmsUtl_strcmp(argv[2], "ippEnabled") == 0 )
      cmsDal_getPrintServerInfo(varValue, NULL, NULL);
   else if ( cmsUtl_strcmp(argv[2], "ippMake") == 0 )
      cmsDal_getPrintServerInfo(NULL, varValue, NULL);
   else if ( cmsUtl_strcmp(argv[2], "ippName") == 0 )
      cmsDal_getPrintServerInfo(NULL, NULL, varValue);
#endif
#ifdef DMP_X_BROADCOM_COM_DLNA_1
   else if ( cmsUtl_strcmp(argv[2], "dmsEnabled") == 0 )
      cmsDal_getDigitalMediaServerInfo(varValue, NULL, NULL);
   else if ( cmsUtl_strcmp(argv[2], "dmsMediaPath") == 0 )
      cmsDal_getDigitalMediaServerInfo(NULL, varValue, NULL);
   else if ( cmsUtl_strcmp(argv[2], "dmsBrName") == 0 )
      cmsDal_getDigitalMediaServerInfo(NULL, NULL, varValue);
#endif	  
   else if ( cmsUtl_strcmp(argv[2], "upldSts") == 0 )
   {
      /*
       * These numbers get mapped to specific messages to the user in
       * uploadinfo.html.  Default all unrecognized codes to 4, which 
       * is fatal error.
       */
      switch (glbUploadStatus)
      {
         case CMSRET_SUCCESS:
            sprintf(varValue, "0");
            break;
            
         case CMSRET_RESOURCE_EXCEEDED:
            sprintf(varValue, "1");
            break;
            
         case CMSRET_INVALID_FILENAME:
            sprintf(varValue, "2");
            break;
            
         case CMSRET_INVALID_IMAGE:
            sprintf(varValue, "3");
            break;

         case CMSRET_PMD_CALIBRATION_FILE_SUCCESS:
            sprintf(varValue, "4");
            break;
            
         default:
            sprintf(varValue, "5");
            break;
      }
   }
   else if ( cmsUtl_strcmp(argv[2], "chipId") == 0 )
   {
      UINT32 chipId=0;
      devCtl_getChipId(&chipId);
      sprintf(varValue, "%x", chipId);
   }
   else if ( cmsUtl_strcmp(argv[2], "dateTime") == 0 ) {
#if defined(DMP_TIME_1) || defined(DMP_DEVICE2_TIME_1)
      time_t cur_time = time(NULL);
      ctime_r(&cur_time, varValue);
      // remove the last '\n' character
      varValue[strlen(varValue)-1] = '\0';
#else
      strcpy(varValue, "&nbsp");
#endif
   } else if ( cmsUtl_strcmp(argv[2], "adslStd") == 0 )
#ifdef ANNEX_C
      strcpy(varValue, "annex_c");
#else
      strcpy(varValue, "annex_a");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildSnmp") == 0 )
#ifdef SUPPORT_SNMP
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildDbusRemote") == 0 )
#ifdef SUPPORT_DBUSREMOTE
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildMapt") == 0 )
#ifdef SUPPORT_MAPT
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildUpnp") == 0 )
#ifdef SUPPORT_UPNP
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildDdnsd") == 0 )
#ifdef SUPPORT_DDNSD
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildSntp") == 0 )
#if defined(DMP_TIME_1) || defined(DMP_DEVICE2_TIME_1)
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildIpTables") == 0 )
#ifdef SUPPORT_IPTABLES
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildEbTables") == 0 )
#ifdef SUPPORT_EBTABLES
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildToD") == 0 )
#ifdef SUPPORT_TOD
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildUrlFilter") == 0 )
#ifdef SUPPORT_URLFILTER
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildDnsProxy") == 0 )
#ifdef DMP_X_BROADCOM_COM_DNSPROXY_1 /* aka SUPPORT_DNSPROXY */
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildPR") == 0 )
#ifdef SUPPORT_POLICYROUTING
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildPortmap") == 0)
#ifdef SUPPORT_PORT_MAP
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildIpp") == 0 )
#ifdef SUPPORT_IPP
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildDLNA") == 0 )
#ifdef DMP_X_BROADCOM_COM_DLNA_1
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif	  
   else if ( cmsUtl_strcmp(argv[2], "buildRip") == 0 )
#ifdef SUPPORT_RIP
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildIPSec") == 0 )
#ifdef SUPPORT_IPSEC
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildCert") == 0 )
#ifdef SUPPORT_CERT
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildTr69c") == 0 )
#ifdef SUPPORT_TR69C
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildXmpp") == 0 )
#ifdef SUPPORT_XMPP
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildJqplot") == 0 )
#ifdef SUPPORT_JQPLOT
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildWebSockets") == 0 )
#ifdef SUPPORT_WEB_SOCKETS
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildDpi") == 0 )
#ifdef SUPPORT_DPI
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildWanVlanMux") == 0 )
#ifdef SUPPORT_WANVLANMUX
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildIPv6") == 0 )
#ifdef SUPPORT_IPV6
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "enblIPv6") == 0 )
#ifdef SUPPORT_IPV6
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildPcp") == 0 )
#ifdef SUPPORT_PCP
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "enblPolicer") == 0 )
#ifdef SUPPORT_POLICING
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "enblRtLmt") == 0 )
#ifdef SUPPORT_RATE_LIMIT
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildOmci") == 0 )
#ifdef DMP_X_ITU_ORG_GPON_1 /* aka SUPPORT_OMCI */
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "numWlAdaptor") == 0 )
#ifdef BRCM_WLAN
      sprintf(varValue, "%d", wlgetintfNo() );
#else
      sprintf(varValue, "%d", 0 );
#endif 
   else if ( cmsUtl_strcmp(argv[2], "buildP8021AG") == 0 )
#ifdef SUPPORT_P8021AG
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildEthWan") == 0 )
#ifdef SUPPORT_ETHWAN
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildPtm") == 0 )
#ifdef SUPPORT_PTM
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildVdsl") == 0 )
#ifdef DMP_VDSL2WAN_1
#ifndef SUPPORT_DSL_GFAST
      strcpy(varValue, "1"); // this indicates vdsl option was compiled in
#else
      strcpy(varValue, "2"); // this indicates vdsl option + Gfast was compiled in
#endif
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildDhcpRelay") == 0 )
#ifdef SUPPORT_DHCP_RELAY
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildPwrMngt") == 0 )
#ifdef DMP_X_BROADCOM_COM_PWRMNGT_1
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildHostMipsPwrsave") == 0 )
#ifdef SUPPORT_HOSTMIPS_PWRSAVE
      strcpy(varValue, "");
#else
      strcpy(varValue, " style=\"display:none\"");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildDdrSelfRefreshPwrsave") == 0 )
#ifdef SUPPORT_DDR_SELF_REFRESH_PWRSAVE
      strcpy(varValue, "");
#else
      strcpy(varValue, " style=\"display:none\"");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildEnergyEfficientEthernet") == 0 )
#ifdef SUPPORT_ENERGY_EFFICIENT_ETHERNET
      strcpy(varValue, "");
#else
      strcpy(varValue, " style=\"display:none\"");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildEthernetPwrsave") == 0 )
#ifdef SUPPORT_ETH_PWRSAVE
      strcpy(varValue, "");
#else
      strcpy(varValue, " style=\"display:none\"");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildAvs") == 0 )
#if defined(SUPPORT_AVS_PWRSAVE)
      strcpy(varValue, "");
#else
      strcpy(varValue, " style=\"display:none\"");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildAtm") == 0 )
#ifdef SUPPORT_DSL
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildMocaWan") == 0 )
#if defined(DMP_X_BROADCOM_COM_MOCAWAN_1) || defined(DMP_DEVICE2_MOCA_1)
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( strcmp(argv[2], "buildDslBonding") == 0 )
#ifdef SUPPORT_DSL_BONDING
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildMulticast") == 0 )
#if defined(DMP_X_BROADCOM_COM_IGMP_1) || defined(DMP_X_BROADCOM_COM_MLD_1) || defined(DMP_X_BROADCOM_COM_MCAST_1)
   {
      strcpy(varValue, "1");
   }
#else
   {
      strcpy(varValue, "0");
   }
#endif /* aka SUPPORT_IGMP*/
   else if ( cmsUtl_strcmp(argv[2], "buildL2tpAc") == 0 )
#ifdef DMP_X_BROADCOM_COM_L2TPAC_1
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildPptpAc") == 0 )
#ifdef DMP_X_BROADCOM_COM_PPTPAC_1
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildStorageService") == 0 )
#ifdef DMP_STORAGESERVICE_1
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildSambaService") == 0 )
#ifdef SUPPORT_SAMBA
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildSupportMoca") == 0 )
#ifdef SUPPORT_MOCA
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
#ifdef SUPPORT_MOCA
   else if ( cmsUtl_strcmp(argv[2], "enblMocaPrivacy") == 0 )
      sprintf(varValue, "%d", glbWebVar.enblMocaPrivacy);
   else if ( cmsUtl_strcmp(argv[2], "mocaPassword") == 0 )
      strcpy(varValue, glbWebVar.mocaPassword);
   else if ( cmsUtl_strcmp(argv[2], "enblMocaAutoScan") == 0 )
      sprintf(varValue, "%d", glbWebVar.enblMocaAutoScan);
   else if ( cmsUtl_strcmp(argv[2], "mocaFrequency") == 0 )
      sprintf(varValue, "%u", glbWebVar.mocaFrequency);
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildSupportPlc") == 0 )
#ifdef SUPPORT_HOMEPLUG
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildStandby") == 0 )
#ifdef DMP_X_BROADCOM_COM_STANDBY_1
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildAutoDetection") == 0 )
#ifdef DMP_X_BROADCOM_COM_AUTODETECTION_1
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildGpon") == 0 )
#ifdef DMP_X_ITU_ORG_GPON_1
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildGponWan") == 0 )
      cgiGetGponWanType(varValue);
   else if ( cmsUtl_strcmp(argv[2], "maxGemPort") == 0 )
#ifdef DMP_X_ITU_ORG_GPON_1
      sprintf(varValue, "%d", CONFIG_BCM_MAX_GEM_PORTS);
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildModSwWebUi") == 0 )
#ifdef SUPPORT_MODSW_WEBUI
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildModSwWebUiAdmin") == 0 )
#ifdef SUPPORT_MODSW_WEBUI_ADMIN
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildModSwWebUiSupport") == 0 )
#ifdef SUPPORT_MODSW_WEBUI_SUPPORT
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildModSwBaseline") == 0 )
#ifdef DMP_DEVICE2_SM_BASELINE_1
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildModSwOsgiEE") == 0 )
#ifdef DMP_DEVICE2_X_BROADCOM_COM_MODSW_OSGIEE_1
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildModSwLinuxEE") == 0 )
#ifdef DMP_DEVICE2_X_BROADCOM_COM_MODSW_LINUXEE_1
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildModSwLinuxPFP") == 0 )
#ifdef DMP_DEVICE2_X_BROADCOM_COM_MODSW_LINUXPFP_1
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildEponWan") == 0 )
      cgiGetEponWanType(varValue);
#ifdef DMP_X_BROADCOM_COM_EPON_1
   else if ( cmsUtl_strcmp(argv[2], "buildEponLoid") == 0 )
      cgiIsEponLoidSupported(varValue);
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildBmu") == 0 )
#ifdef SUPPORT_BMU
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "supportLanvlan") == 0 )
#ifdef SUPPORT_LANVLAN
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildfapctl") == 0 )
#ifdef SUPPORT_FAPCTL
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "supportfaptm") == 0 )
#if defined(DMP_ADSLWAN_1) && defined(SUPPORT_FAPCTL)
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "supportbcmtm") == 0 )
#if defined(SUPPORT_BCMTM)
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
#ifdef DMP_DEVICE2_OPTICAL_1

   else if ( cmsUtl_strcmp(argv[2], "buildOptical") == 0 )
#ifdef DMP_X_ITU_ORG_GPON_1
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif    // DMP_X_ITU_ORG_GPON_1

#endif    // DMP_DEVICE2_OPTICAL_1
   else if ( cmsUtl_strcmp(argv[2], "buildWifiWan") == 0 )
#ifdef SUPPORT_WIFIWAN
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "supportrddtm") == 0 )
#if defined(SUPPORT_RDPA)
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "supportEthPortShaping") == 0 )
#if (defined(SUPPORT_DSL) && defined(SUPPORT_FAPCTL)) || (defined(SUPPORT_RDPA) && (defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_63158) || defined(CHIP_4908))) || defined(SUPPORT_BCMTM)
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildTms") == 0 )
#ifdef DMP_X_BROADCOM_COM_ETHERNETOAM_1
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif    // DMP_X_BROADCOM_COM_ETHERNETOAM_1
   else if ( cmsUtl_strcmp(argv[2], "buildSpdsvc") == 0 )
#if defined(DMP_X_BROADCOM_COM_SPDSVC_1)
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "support1905") == 0 )
#if defined(SUPPORT_IEEE1905)
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif    // SUPPORT_IEEE1905
   else if ( cmsUtl_strcmp(argv[2], "buildNfc") == 0 )
#if defined(DMP_X_BROADCOM_COM_NFC_1)
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "support1905TopologyWeb") == 0 )
#if defined(SUPPORT_1905_TOPOLOGY_WEB_PAGE)
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif    // SUPPORT_IEEE1905
   else if ( cmsUtl_strcmp(argv[2], "buildUsbHosts") == 0 )
#ifdef DMP_DEVICE2_USBHOSTSBASIC_1
      strcpy(varValue, cmsMdm_isDataModelDevice2() ? "1" : "0");
#else
      strcpy(varValue, "0");
#endif    // DMP_DEVICE2_USBHOSTSBASIC_1
   else if ( cmsUtl_strcmp(argv[2], "buildQrcode") == 0 )
#if defined(SUPPORT_QRCODE)
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif    // SUPPORT_QRCODE
   else if ( cmsUtl_strcmp(argv[2], "buildIngressFilters") == 0 )
#if defined(DMP_X_BROADCOM_COM_RDPA_1)
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildStatsQueue") == 0 )
#if defined(DMP_X_BROADCOM_COM_RDPA_1)
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildCellular") == 0 )
#if defined(SUPPORT_CELLULAR)
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif    // SUPPORT_CELLULAR
   else if ( cmsUtl_strcmp(argv[2], "buildOpenvswitch") == 0 )
#if defined(DMP_X_BROADCOM_COM_OPENVSWITCH_1)
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildStun") == 0 )
#if defined(SUPPORT_STUN)
      strcpy(varValue, "1");
#else
      strcpy(varValue, "0");
#endif
   else if ( cmsUtl_strcmp(argv[2], "buildPMD") == 0 )
   {
      if (isPmdExist())
      {
         strcpy(varValue, "1");
      }
      else
      {
         strcpy(varValue, "0");
      }
   }
   else
      strcpy(varValue, "&nbsp");
}


#ifdef SUPPORT_TOD
void getPcMacAddr_igd(char *macAddr)
{
   LanDevObject *lanDevObj=NULL;
   LanIpIntfObject *ipIntfObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack2;
   InstanceIdStack iidStack3 = EMPTY_INSTANCE_ID_STACK;
   LanHostEntryObject *entryObj = NULL;
   CmsRet ret;
   UBOOL8 found = FALSE;
   
   strcpy(macAddr, "");

   while (!found && (ret = cmsObj_getNext(MDMOID_LAN_DEV, &iidStack, (void **) &lanDevObj)) == CMSRET_SUCCESS)
   {
      cmsObj_free((void **) &lanDevObj);

      /* just use the first LAN IP INTF from each LAN Device */
      INIT_INSTANCE_ID_STACK(&iidStack2);
      if ((ret = cmsObj_getNextInSubTree(MDMOID_LAN_IP_INTF, &iidStack, &iidStack2, (void **) &ipIntfObj)) != CMSRET_SUCCESS)
      {
         /* weird, each LANDevice should have at least 1 IP Intf. */
         cmsLog_error("could not find ip intf under LANDevice %s", cmsMdm_dumpIidStack(&iidStack));
      }
      else
      {
         cmsLog_debug("Bridge %s", ipIntfObj->X_BROADCOM_COM_IfName);
         cmsObj_free((void **) &ipIntfObj);
         
         INIT_INSTANCE_ID_STACK(&iidStack3);
         while (!found && (ret = cmsObj_getNextInSubTree(MDMOID_LAN_HOST_ENTRY, &iidStack, &iidStack3, (void **) &entryObj)) == CMSRET_SUCCESS)
         {
            if( !cmsUtl_strcmp(glbWebVar.pcIpAddr, entryObj->IPAddress) )
            {
               found = TRUE;
               strcpy( macAddr, entryObj->MACAddress );
            }

            cmsObj_free((void **) &entryObj);
         }
      }
   }

}
#endif

/* TODO: need to be removed or rewritten later on ? */
UBOOL8 cgi_isQosEnabled(void) {
    UBOOL8 enabled = FALSE;
#if 0 //~
    UINT16 vccId = 0;   // set to 0 to search vcc from beginning
    IFC_ATM_VCC_INFO vccInfo;
   
    while ( BcmDb_getVccInfoNext(&vccId, &vccInfo) == DB_VCC_GET_OK ) {
        if ( vccInfo.enblQos == TRUE ) {
            enabled = TRUE;
            break;
        }
    }
#endif //~
    return enabled;
}

void cgiGetMemInfo(int argc __attribute__((unused)),
                   char **argv, char *varValue)
{
   // get info directly from the temporary memory

   if ( cmsUtl_strcmp(argv[2], "portVpiVci") == 0 ) {
#if SUPPORT_ETHWAN
      if ( glbWebVar.enableEthWan == 1 ) {
         sprintf(varValue, "N / A");
      } else {
#endif
         sprintf(varValue, "%d / %d / %d", glbWebVar.portId, glbWebVar.atmVpi, glbWebVar.atmVci);
#if SUPPORT_ETHWAN
      }
#endif
   } else if ( cmsUtl_strcmp(argv[2], "ntwkPrtcl") == 0 )
      cmsDal_getWanProtocolName(glbWebVar.ntwkPrtcl, varValue);
   else if ( cmsUtl_strcmp(argv[2], "wanAddr") == 0 )
      switch ( glbWebVar.ntwkPrtcl ) {
      case PROTO_PPPOE:
      case PROTO_PPPOA:
         if (glbWebVar.useStaticIpAddress == TRUE)
            strcpy(varValue, glbWebVar.pppLocalIpAddress);
         else
            strcpy(varValue, "Automatically Assigned");
         break;
      case PROTO_MER:
#if SUPPORT_ETHWAN
      case PROTO_IPOWAN:
#endif
         if ( glbWebVar.enblDhcpClnt == 1 )
            strcpy(varValue, "Automatically Assigned");
         else
            strcpy(varValue, glbWebVar.wanIpAddress);
         break;
      case PROTO_IPOA:
         strcpy(varValue, glbWebVar.wanIpAddress);
         break;
      default:
         strcpy(varValue, "Not Applicable");
         break;
      }
   else if ( cmsUtl_strcmp(argv[2], "dfltGw") == 0 )
      switch ( glbWebVar.ntwkPrtcl ) {
      case PROTO_PPPOE:
      case PROTO_PPPOA:
         strcpy(varValue, "Automatically Assigned");
         break;
      case PROTO_MER:
#if SUPPORT_ETHWAN
      case PROTO_IPOWAN:
#endif
         if ( glbWebVar.enblDhcpClnt == 1 )
            strcpy(varValue, "Automatically Assigned");
         else
            strcpy(varValue, glbWebVar.defaultGatewayList);
         break;
      case PROTO_IPOA:
         strcpy(varValue, glbWebVar.defaultGatewayList);
         break;
      default:
         strcpy(varValue, "Not Applicable");
         break;
      }
#ifdef SUPPORT_IPV6
   else if ( cmsUtl_strcmp(argv[2], "wanGtwy6") == 0 )
      switch ( glbWebVar.ntwkPrtcl ) {
      case PROTO_PPPOE:
      case PROTO_PPPOA:
      case PROTO_MER:
#if SUPPORT_ETHWAN
      case PROTO_IPOWAN:
#endif
      case PROTO_IPOA:
         strcpy(varValue, glbWebVar.wanGtwy6);
         break;
      default:
         strcpy(varValue, "Not Applicable");
         break;
      }
#endif
   else if ( cmsUtl_strcmp(argv[2], "enblIgmp") == 0 ) {
      if ( glbWebVar.ntwkPrtcl != PROTO_BRIDGE ) {
#ifdef DMP_X_BROADCOM_COM_IGMP_1
         if (glbWebVar.enblIgmp == 1)
            strcpy(varValue, "Enabled");
         else
#endif
            strcpy(varValue, "Disabled");
      } else
         strcpy(varValue, "Not Applicable");
   }
   else if ( cmsUtl_strcmp(argv[2], "enblIgmpMcastSource") == 0 ) {
#ifdef DMP_X_BROADCOM_COM_IGMP_1
      if (glbWebVar.enblIgmpMcastSource == 1)
         strcpy(varValue, "Enabled");
      else
#endif
         strcpy(varValue, "Disabled");
   }
#if defined(DMP_X_BROADCOM_COM_GPONWAN_1)
   else if ( cmsUtl_strcmp(argv[2], "noMcastVlanFilter") == 0 ) {
#ifdef DMP_X_BROADCOM_COM_IGMP_1
      if ( glbWebVar.enblIgmp == 1 ) {
         if (glbWebVar.noMcastVlanFilter == 1)
            strcpy(varValue, "Enabled");
         else
            strcpy(varValue, "Disabled");
      } else
#endif
         strcpy(varValue, "Not Applicable");
   }
#endif /* DMP_X_BROADCOM_COM_GPONWAN_1 */
   else if ( cmsUtl_strcmp(argv[2], "enblMld") == 0 ) {
      if ( glbWebVar.ntwkPrtcl != PROTO_BRIDGE ) {
#ifdef DMP_X_BROADCOM_COM_MLD_1
         if (glbWebVar.enblMld == 1)
            strcpy(varValue, "Enabled");
         else
#endif
            strcpy(varValue, "Disabled");
      } else
         strcpy(varValue, "Not Applicable");
   }
   else if ( cmsUtl_strcmp(argv[2], "enblMldMcastSource") == 0 ) {
#ifdef DMP_X_BROADCOM_COM_MLD_1
      if (glbWebVar.enblMldMcastSource== 1)
         strcpy(varValue, "Enabled");
      else
#endif
         strcpy(varValue, "Disabled");
   }
   else if ( cmsUtl_strcmp(argv[2], "enblService") == 0 ) {
      if (glbWebVar.enblService == 1)
         strcpy(varValue, "Enabled");
      else
         strcpy(varValue, "Disabled");
   }
   else if ( cmsUtl_strcmp(argv[2], "enblQos") == 0 ) {
      if (glbWebVar.enblQos == 1)
         strcpy(varValue, "Enabled");
      else
         strcpy(varValue, "Disabled");
   }
   else if ( cmsUtl_strcmp(argv[2], "enblDiffServ") == 0 ) {
      if (glbWebVar.enblDiffServ == 1)
         strcpy(varValue, "Enabled");
      else
         strcpy(varValue, "Disabled");
   }
   else if ( cmsUtl_strcmp(argv[2], "isQosEnabled") == 0 ) {
      if (cgi_isQosEnabled() == TRUE)
         strcpy(varValue, "true");
      else
         strcpy(varValue, "false");
   }
   else if ( cmsUtl_strcmp(argv[2], "queueNum") == 0 ) {
      UINT32 usedQueues;
      cmsDal_getNumUsedQueues(ATM, &usedQueues);
      sprintf(varValue, "%d", usedQueues);
   }
   else if ( cmsUtl_strcmp(argv[2], "enblNat") == 0 ) {
      if (glbWebVar.enblNat == 1)
         strcpy(varValue, "Enabled");
      else
         strcpy(varValue, "Disabled");
   }
   else if ( cmsUtl_strcmp(argv[2], "enblFullcone") == 0 ) {
      if (glbWebVar.enblFullcone == 1)
         strcpy(varValue, "Enabled");
      else
         strcpy(varValue, "Disabled");
   }
   else if ( cmsUtl_strcmp(argv[2], "enblFirewall") == 0 ) {
      if (glbWebVar.enblFirewall == 1)
         strcpy(varValue, "Enabled");
      else
         strcpy(varValue, "Disabled");
   }
   else if ( cmsUtl_strcmp(argv[2], "pppoeServiceName") == 0 ) {
      char ifName[BUFLEN_256];

      ifName[0] = '\0';
      sprintf(ifName, "ppp_0_0_1");
      //      getPppoeServiceName(pppoeService, ifName);
      strcpy(varValue, "&nbsp");
   }
   else if ( cmsUtl_strcmp(argv[2], "errMsg") == 0 ) {
      if ( glbErrMsg[0] != '\0' )
         strcpy(varValue, glbErrMsg);
      else
         strcpy(varValue, "&nbsp");
   }
   else if ( cmsUtl_strcmp(argv[2], "isOutOfVcc") == 0 )
      sprintf(varValue, "%d", glbIsOutOfVcc);
   else if ( cmsUtl_strcmp(argv[2], "isVccInUsed") == 0 )
      sprintf(varValue, "0");
   else if ( cmsUtl_strcmp(argv[2], "isDataEmpty") == 0 ) {
#if 0 //~   varValue == "1" is for quicksetup
      WAN_CON_ID wanId;
      WAN_CON_INFO wanInfo;
      wanId.vpi = wanId.vci = wanId.conId = 0;
      if ( BcmDb_getWanInfoNext(&wanId, &wanInfo) == DB_WAN_GET_NOT_FOUND )
         strcpy(varValue, "1");
      else
#endif //~
         strcpy(varValue, "0");
   }
#ifdef SUPPORT_TOD
   else if ( cmsUtl_strcmp(argv[2], "pcMacAddr") == 0 )
   {
      getPcMacAddr(varValue);
   }
#endif
   else
   {
      cmsLog_error("unrecognized get of variable %s", argv[2]);
      strcpy(varValue, "&nbsp");
   }
}

#ifdef SUPPORT_DSL
void cgiGetLineRate(int argc, char **argv, char *varValue) 
{
   cgiGetDslLineRate(argc,argv,varValue);
}

void cgiGetDslLineRate_igd(int argc, char **argv, char *varValue) 
{
   WanDslIntfCfgObject *dslIntfObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found = FALSE;
   UINT32 totalRate=0;

#ifdef DMP_X_BROADCOM_COM_DSLBONDING_1
   UBOOL8 bondingStatsAdded=FALSE;
   WanDslIntfCfgObject *pPtmBondingDslIntfObj = NULL;
   WanDslIntfCfgObject *pAtmBondingDslIntfObj = NULL;
   InstanceIdStack ptmBondingIidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack atmBondingIidStack = EMPTY_INSTANCE_ID_STACK;

   dalDsl_getBondingPtmDslIntfObject(&ptmBondingIidStack, &pPtmBondingDslIntfObj);
   dalDsl_getBondingAtmDslIntfObject(&atmBondingIidStack, &pAtmBondingDslIntfObj);
#endif


   cmsLog_debug("argc=%d", argc);

   while (!found &&
          cmsObj_getNext(MDMOID_WAN_DSL_INTF_CFG, &iidStack, (void **) &dslIntfObj) == CMSRET_SUCCESS)
   {
      if ((dslIntfObj->enable) &&
          (dslIntfObj->X_BROADCOM_COM_BondingLineNumber == 0))
      {
         if (!cmsUtl_strcmp(argv[2], "0") && !cmsUtl_strcmp(argv[3], "0"))
         {
            /* Path/Bearer 0: Up Stream Rate */
            if (!cmsUtl_strcmp(dslIntfObj->status, MDMVS_UP))
            {
               // [JIRA SWBCACPE-10306]: TR-098 requires
               // upstreamCurrRate, downstreamCurrRate, upstreamMaxRate, downstreamMaxRate
               // in Kbps instead of Bps
               totalRate += (dslIntfObj->upstreamCurrRate * 1000);
               found = TRUE;
            }

#ifdef DMP_X_BROADCOM_COM_DSLBONDING_1
				if (!bondingStatsAdded) {
               if (pPtmBondingDslIntfObj->X_BROADCOM_COM_EnableBonding &&
					    !cmsUtl_strcmp(pPtmBondingDslIntfObj->status, MDMVS_UP)) {

						totalRate += (pPtmBondingDslIntfObj->upstreamCurrRate * 1000);
						bondingStatsAdded=TRUE;
					}
					else if (pAtmBondingDslIntfObj->X_BROADCOM_COM_EnableBonding &&
					    !cmsUtl_strcmp(pAtmBondingDslIntfObj->status, MDMVS_UP)) {

						totalRate += (pAtmBondingDslIntfObj->upstreamCurrRate * 1000);
						bondingStatsAdded=TRUE;
					}
				}
#endif

            sprintf(varValue,"%d",(totalRate/1000));
         }
         else if (!cmsUtl_strcmp(argv[2], "1") && !cmsUtl_strcmp(argv[3], "0"))
         {
            /* Path/Bearer 0: Down Stream Rate */
            if (!cmsUtl_strcmp(dslIntfObj->status, MDMVS_UP))
            {
               // [JIRA SWBCACPE-10306]: TR-098 requires
               // upstreamCurrRate, downstreamCurrRate, upstreamMaxRate, downstreamMaxRate
               // in Kbps instead of Bps
               totalRate += (dslIntfObj->downstreamCurrRate * 1000);
               found = TRUE;
            }

#ifdef DMP_X_BROADCOM_COM_DSLBONDING_1
				if (!bondingStatsAdded) {
               if (pPtmBondingDslIntfObj->X_BROADCOM_COM_EnableBonding &&
					    !cmsUtl_strcmp(pPtmBondingDslIntfObj->status, MDMVS_UP)) {

                  totalRate += (pPtmBondingDslIntfObj->downstreamCurrRate * 1000);
						bondingStatsAdded=TRUE;
					}
					else if (pAtmBondingDslIntfObj->X_BROADCOM_COM_EnableBonding &&
					    !cmsUtl_strcmp(pAtmBondingDslIntfObj->status, MDMVS_UP)) {

                  totalRate += (pAtmBondingDslIntfObj->downstreamCurrRate * 1000);
						bondingStatsAdded=TRUE;
					}
				}
#endif

            sprintf(varValue, "%d",(totalRate/1000));
         }
         else if (!cmsUtl_strcmp(argv[2], "0") && !cmsUtl_strcmp(argv[3], "1"))
         {
            /* Path/Bearer 1: Up Stream Rate */
            if (!cmsUtl_strcmp(dslIntfObj->status, MDMVS_UP))
            {
               sprintf(varValue,"%d",(dslIntfObj->X_BROADCOM_COM_UpstreamCurrRate_2/1000));
               found = TRUE;
            }
         }
         else if (!cmsUtl_strcmp(argv[2], "1") && !cmsUtl_strcmp(argv[3], "1"))
         { 
            /* Path/Bearer 1: Down Stream Rate */
            if (!cmsUtl_strcmp(dslIntfObj->status, MDMVS_UP))
            {
               sprintf(varValue, "%d",(dslIntfObj->X_BROADCOM_COM_DownstreamCurrRate_2/1000));
               found = TRUE;
            }
         }
         else
         {
            cmsLog_error("unrecognized request %s/%s", argv[2], argv[3]);
            strcpy(varValue, "&nbsp");
         }
      }

      cmsObj_free((void **) &dslIntfObj);
   }

#ifdef DMP_X_BROADCOM_COM_DSLBONDING_1
   cmsObj_free((void **) &pPtmBondingDslIntfObj);
   cmsObj_free((void **) &pAtmBondingDslIntfObj);
#endif

   return;
}
#endif /* SUPPORT_DSL */

void cgiGetDmzHost(int argc __attribute__((unused)),
                   char **argv __attribute__((unused)),
                   char *varValue)
{
   cmsLog_debug("enter");
   dalGetDmzHost(varValue);
}


UBOOL8 cgiWillLanIpChange_igd(const char *newIpAddr, const char *intfGroupName)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanIpIntfObject *lanIpObj = NULL;
   UBOOL8 changed = FALSE;
   UBOOL8 found = FALSE;
   char brIfName[CMS_IFNAME_LENGTH]={0};

#ifdef DMP_BRIDGING_1
   L2BridgingEntryObject *pBridgeObj = NULL;

   while ((!found) &&
          (CMSRET_SUCCESS == cmsObj_getNextFlags(MDMOID_L2_BRIDGING_ENTRY, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&pBridgeObj)))
   {
      if (0 == cmsUtl_strcmp(intfGroupName, pBridgeObj->bridgeName))
      {
         sprintf(brIfName, "br%d", pBridgeObj->bridgeKey);
         found = TRUE;
      }
      cmsObj_free((void **)&pBridgeObj);
   }

   if (!found)
   {
      cmsLog_error("Can't find the BridgeName=[%s] from the DataBase ", intfGroupName);
      return changed;
   }
#else
   /*
    * If TR-098 Layer 2 Bridging functionality is not enabled, then the
    * bridge ifname will be passed into intfGroupName.
    */
   sprintf(brIfName, "%s", intfGroupName);
#endif

   INIT_INSTANCE_ID_STACK(&iidStack);
   found = FALSE;

   while ((!found) &&
          (CMSRET_SUCCESS == cmsObj_getNextFlags(MDMOID_LAN_IP_INTF, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&lanIpObj)))
   {
      if (!cmsUtl_strcmp(brIfName, lanIpObj->X_BROADCOM_COM_IfName))
      {
         found = TRUE;
         changed = cmsUtl_strcmp(lanIpObj->IPInterfaceIPAddress, newIpAddr);
      }

      cmsObj_free((void**) &lanIpObj);
   }

   cmsLog_debug("intfGroupName=%s newIPAddr=%s changed=%d", intfGroupName, newIpAddr, changed);

   return changed;
}


UBOOL8 isUsedByOtherBridge_igd(const char *intfGroupName, const char *ipAddr, const char *subnetMask)
{
   char                  skipIfName[CMS_IFNAME_LENGTH]={0};
   UBOOL8                isInUse = FALSE;
   LanIpIntfObject       *lanIpObj = NULL;
   InstanceIdStack       iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet                ret;
   struct in_addr ip, mask, subnet;

   cmsLog_debug("checking on intfGroupName=%s ipAddr=%s subnetMask=%s", intfGroupName, ipAddr, subnetMask);

   if (!inet_aton(ipAddr, &ip) || !inet_aton(subnetMask, &mask))
   {
      cmsLog_error("ip address conversion failed on %s or %s", ipAddr, subnetMask);
      return isInUse;
   }

   subnet.s_addr = ip.s_addr & mask.s_addr;
   
#ifdef DMP_BRIDGING_1
   /*
    * First we need to find the bridge name (e.g. br0) that is associated
    * with the Interface Group Name.  Once we have that bridge name, we
    * will know not to check that bridge for subnet conflict.
    */
   {
      UBOOL8                found = FALSE;
      L2BridgingEntryObject *bridgeObj = NULL;

      while((!found) &&
            (CMSRET_SUCCESS == cmsObj_getNextFlags(MDMOID_L2_BRIDGING_ENTRY, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&bridgeObj)))
      {
         if (!cmsUtl_strcmp(bridgeObj->bridgeName, intfGroupName))
         {
            sprintf(skipIfName, "br%d", bridgeObj->bridgeKey);
            cmsLog_debug("intf group %s is on bridge %s", intfGroupName, skipIfName);
            found = TRUE;
         }

         cmsObj_free((void **) &bridgeObj);
      }
   }
#else
   /*
    * If TR-098 Layer 2 Bridging functionality is not enabled, then the
    * bridge ifname will be passed into intfGroupName.
    */
   sprintf(skipIfName, "%s", intfGroupName);

#endif


   INIT_INSTANCE_ID_STACK(&iidStack);

   while((ret = cmsObj_getNextFlags(MDMOID_LAN_IP_INTF, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&lanIpObj)) == CMSRET_SUCCESS)
      {
         struct in_addr tmp_ip, tmp_mask, tmp_subnet;
          
      /*
       * Only check for subnet conflict if our getNext has a bridge that
       * is different than the once the caller wants to change.
       */
      if (cmsUtl_strcmp(lanIpObj->X_BROADCOM_COM_IfName, skipIfName))
      {
         if (!inet_aton(lanIpObj->IPInterfaceIPAddress, &tmp_ip) || !inet_aton(lanIpObj->IPInterfaceSubnetMask, &tmp_mask))
         {
            cmsLog_error("ip address conversion failed on %s or %s", lanIpObj->IPInterfaceIPAddress, lanIpObj->IPInterfaceSubnetMask);
         }
         else
         {
         tmp_subnet.s_addr = tmp_ip.s_addr & tmp_mask.s_addr;

         if(tmp_subnet.s_addr == subnet.s_addr)
         {
               isInUse = TRUE;
            }
         }
         }
         
         cmsObj_free((void **)&lanIpObj);
   }

   return isInUse;
}

void cgiGetStaticIpLease(int argc __attribute__((unused)),
                         char **argv __attribute__((unused)),
                         char *varValue)
{
   dalLan_GetStaticIpLease(glbWebVar.brName, varValue);
}


UBOOL8 isStaticPVC(void)
{
   UBOOL8 isStatic = FALSE;
   
   if  (glbWebVar.ntwkPrtcl == CMS_WAN_TYPE_IPOA ||
      (glbWebVar.ntwkPrtcl == CMS_WAN_TYPE_STATIC_IPOE && cmsUtl_strcmp(glbWebVar.wanIpAddress, "0.0.0.0")))
   {
      isStatic = TRUE;
   }

   return isStatic;

}

#ifdef SUPPORT_DSL
UBOOL8 isXdslLinkUp(void)
{
   return (qdmDsl_isXdslLinkUpLocked());
}

/* this function really wants to know if DSL line is trained PTM mode or not */
UBOOL8 isVdslPtmMode(void)
{
   UBOOL8 isVdsl, isAtm;
   
   qdmDsl_getDSLTrainedModeLocked(&isVdsl,&isAtm);
   if (isAtm)
   {
      return FALSE;
   }
   return TRUE;
}

void cgiGetIsDslBondingEnabled (int argc __attribute__((unused)), char **argv  __attribute__((unused)), char *varValue)
{

   sprintf(varValue, "0");

#ifdef SUPPORT_DSL_BONDING
   if (dalDsl_isDslBondingEnabled())
   {
      sprintf(varValue, "1");
   }
#endif
   cmsLog_debug("return varValue=%s", varValue);
   return;
}


/*
 * this function is used to fill in the "isVdsl" variable in webUI.
 * But the name is misleading.  This variable actually indicates if the
 * DSL line is up and in VDSL mode.  See also buildVdsl
 */
void cgiIsVdslSupported (int argc __attribute__((unused)), char **argv  __attribute__((unused)), char *varValue)
{
   UBOOL8 isAtm, isVdsl;

   qdmDsl_getDSLTrainedModeLocked(&isVdsl,&isAtm);
   if (isVdsl)
   {
      sprintf(varValue, "1");
   }
   else
   {
      sprintf(varValue, "0");
   }
}
#endif /* SUPPORT_DSL */

char *cgiRelTime(UINT32 seconds)
{
   static char s[] = "XXXXX days, XX hours, XX minutes, XX seconds";
   char *c = s;

   if (seconds > 60*60*24) 
   {
      c += sprintf(c, "%d days, ", seconds / (60*60*24));
      seconds %= 60*60*24;
   }
   if (seconds > 60*60) 
   {
      c += sprintf(c, "%d hours, ", seconds / (60*60));
      seconds %= 60*60;
   }
   if (seconds > 60) 
   {
      c += sprintf(c, "%d minutes, ", seconds / 60);
      seconds %= 60;
   }
   c += sprintf(c, "%d seconds", seconds);
   return s;
   
}

void cgiPrintDHCPLeases(FILE *fs)
{
   FILE *fp;
   int i;
   unsigned long now;
   unsigned long expires;
   char file[] = "/etc/udhcpd.leases";
   char strExpires[BUFLEN_64], strMacAddr[BUFLEN_32], *ptr;
   struct lease_t lease;
   struct in_addr addr;

   if (!(fp = fopen(file, "r")))
   {
      cmsLog_error("could not open DHCP lease file");
      return;
   }
   now = time(0);
   while (fread(&lease, sizeof(lease), 1, fp))
   {
      memset(strExpires, 0, sizeof(strExpires));
      memset(strMacAddr, 0, sizeof(strMacAddr));

      addr.s_addr = lease.yiaddr;
      expires = ntohl(lease.expires);
      expires = (now < expires) ? expires - now : 0;

      ptr = strMacAddr;
      for (i = 0; i < 6; i++) {
         ptr += sprintf(ptr, "%02x", lease.chaddr[i]);
         if (i != 5) ptr += sprintf(ptr, ":");
      }
      if (!expires) sprintf(strExpires, "Expired");
      else {
         int day, hour, minute, second;
         day = expires / (60*60*24);
         expires %=  (60*60*24);
         hour = expires / (60*60);
         expires %= (60*60);
         minute = expires / 60;
         second = expires %= 60;
         if (day)
            sprintf(strExpires, "%d days, %d hours, %d minutes, %d seconds", day, hour, minute, second);
         else if (hour)
            sprintf(strExpires, "%d hours, %d minutes, %d seconds", hour, minute, second);
         else if (minute)
            sprintf(strExpires, "%d minutes, %d seconds", minute, second);
         else
            sprintf(strExpires, "%d seconds", second);
      }
      fprintf(fs,  " <tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",
            lease.hostname, strMacAddr, inet_ntoa(addr), strExpires);
   }
   fclose(fp);
}

void cgiPrintGroupList_igd(char *print)
{
   InstanceIdStack           iidStack = EMPTY_INSTANCE_ID_STACK;
   int  wsize; 
   char *p=print;

   sprintf(p, "<select size=\"1\" name=\"brName\" onChange='changeBrName()'>\n%n", &wsize); p +=wsize;

#ifdef DMP_BRIDGING_1
   {
      L2BridgingEntryObject    *pBridgeObj = NULL;
      while (CMSRET_SUCCESS == cmsObj_getNextFlags(MDMOID_L2_BRIDGING_ENTRY, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&pBridgeObj))
   {
      sprintf(p,"<option value=\"%s\"> %s\n%n", pBridgeObj->bridgeName, pBridgeObj->bridgeName, &wsize); p +=wsize;
      cmsObj_free((void **)&pBridgeObj);
   }
   }

#else
   {
      LanIpIntfObject          *lanIpObj = NULL;
      /*
       * If TR-098 Layer 2 Bridging is not supported, then just give the 
       * name of the bridge interface.
       */
      while(CMSRET_SUCCESS == cmsObj_getNextFlags(MDMOID_LAN_IP_INTF, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&lanIpObj))
      {
         sprintf(p,"<option value=\"%s\"> %s\n%n", lanIpObj->X_BROADCOM_COM_IfName, lanIpObj->X_BROADCOM_COM_IfName, &wsize); p +=wsize;
         cmsObj_free((void **)&lanIpObj);
      }
   }
#endif

   sprintf(p, " </option>\n%n", &wsize); p +=wsize;
   sprintf(p, "  </select>\n%n", &wsize); p +=wsize;
}

void getUptimeString_igd(char *value)
{
   char uptimeString[BUFLEN_512];
   UINT32 s=0;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   IGDDeviceInfoObject *deviceInfo=NULL;
   CmsRet ret;

   if ((ret = cmsObj_get(MDMOID_IGD_DEVICE_INFO, &iidStack, 0, (void **) &deviceInfo)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get DEVICE_INFO, ret=%d", ret);
   }
   else
   {
      s = deviceInfo->upTime;
      cmsObj_free((void **)&deviceInfo);
   }

   cmsTms_getDaysHoursMinutesSeconds(s, uptimeString, sizeof(uptimeString));

   sprintf(value, "%s", uptimeString);
}


void cgiGetGponWanType(char *varValue)
{

   strcpy(varValue,"0");
   
#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
   char wanTypeBuf[BUFLEN_32]={0};
   CmsRet ret;

   ret = rutWan_getOpticalWanType(wanTypeBuf);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get optical wan type, return 0");
      strcpy(varValue,"0");
      return;
   }

   //TBD think about uniPon(gpon_epon)?
   if (strcmp(wanTypeBuf, MDMVS_GPON) == 0)
      strcpy(varValue,"1");
#endif /* DMP_X_BROADCOM_COM_GPONWAN_1 */

}

void cgiGetEponWanType(char *varValue)
{
   strcpy(varValue,"0");

#ifdef DMP_X_BROADCOM_COM_EPON_1
   char wanTypeBuf[BUFLEN_32]={0};
   CmsRet ret;

   ret = rutWan_getOpticalWanType(wanTypeBuf);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get optical wan type, return 0");
      strcpy(varValue,"0");
      return;
   }

   //TBD think about uniPon(gpon_epon)?
   if (strcmp(wanTypeBuf, MDMVS_EPON) == 0)
      strcpy(varValue,"1");
#endif /* DMP_X_BROADCOM_COM_EPONWAN_1 */

}

#ifdef DMP_X_BROADCOM_COM_EPON_1

enum
{
   OuiOamStart,
   OuiTeknovus = OuiOamStart,
   OuiCtc,
   OuiNtt,
   OuiDasan,
   OuiDpoe,
   OuiKt,
   OuiPmc,
   OuiCuc,
   OuiOamEnd,
   Oui802Dot1 = OuiOamEnd,
   OuiKnownCount
};

enum
{
   OAM_BCM_SUPPORT = 1<<OuiTeknovus,
   OAM_CTC_SUPPORT = 1<<OuiCtc,
   OAM_DPOE_SUPPORT = 1<<OuiDpoe,
   OAM_CUC_SUPPORT = 1<<OuiCuc,
};

void cgiIsEponLoidSupported(char *varValue)
{
   strcpy(varValue,"0");

   UINT8 oamSel = OAM_CTC_SUPPORT;
   XponObject *obj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   if ((ret = cmsObj_get(MDMOID_XPON, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get xpon, ret=%d", ret);
   }
   else
   {
      oamSel = obj->oamSelection;
      cmsObj_free((void **) &obj);
   }

   if (oamSel & OAM_CUC_SUPPORT)
      strcpy(varValue,"1");
}
#endif /* DMP_X_BROADCOM_COM_EPON_1 */




