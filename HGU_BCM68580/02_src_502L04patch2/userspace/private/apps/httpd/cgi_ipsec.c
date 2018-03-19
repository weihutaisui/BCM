/***********************************************************************
 *
 *  Copyright (c) 2005-2007  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2011:proprietary:standard

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "cms.h"
#include "httpd.h"
#include "cgi_cmd.h"
#include "cgi_ipsec.h"
#include "cgi_main.h"
#include "secapi.h"
#include "syscall.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_dal.h"

#ifdef SUPPORT_IPSEC
#define CERT_PATH "/var/cert"

extern WEB_NTWK_VAR glbWebVar;

void writeIPSecScript(FILE *fs);

void writeIPSecHeader(FILE *fs)
{
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "<script language=\"javascript\" src=\"portName.js\"></script>\n");
}

void writeIPSecBody(FILE *fs)
{
   fprintf(fs, "</head>\n<body>\n<blockquote>\n<form>\n");
}

int bcmIPSecNumEntries();
int bcmIPSecNumEntries_igd();
int bcmIPSecNumEntries_dev2();
#if defined(SUPPORT_DM_LEGACY98)
#define bcmIPSecNumEntries()  bcmIPSecNumEntries_igd()
#elif defined(SUPPORT_DM_HYBRID)
#define bcmIPSecNumEntries()  bcmIPSecNumEntries_igd()
#elif defined(SUPPORT_DM_PURE181)
#define bcmIPSecNumEntries()  bcmIPSecNumEntries_dev2()
#elif defined(SUPPORT_DM_DETECT)
#define bcmIPSecNumEntries()   (cmsMdm_isDataModelDevice2() ? \
                              bcmIPSecNumEntries_dev2()   : \
                              bcmIPSecNumEntries_igd())
#endif

void fillIPSecInfo(FILE *fs);
void fillIPSecInfo_igd(FILE *fs);
void fillIPSecInfo_dev2(FILE *fs);
#if defined(SUPPORT_DM_LEGACY98)
#define fillIPSecInfo(f)  fillIPSecInfo_igd((f))
#elif defined(SUPPORT_DM_HYBRID)
#define fillIPSecInfo(f)  fillIPSecInfo_igd((f))
#elif defined(SUPPORT_DM_PURE181)
#define fillIPSecInfo(f)  fillIPSecInfo_dev2((f))
#elif defined(SUPPORT_DM_DETECT)
#define fillIPSecInfo(f)   (cmsMdm_isDataModelDevice2() ? \
                         fillIPSecInfo_dev2((f))   : \
                         fillIPSecInfo_igd((f)))
#endif


#if !defined(SUPPORT_DM_PURE181)
void fillIPSecInfo_igd(FILE *fs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   IPSecCfgObject *ipsecObj = NULL;
   CmsRet ret;

   while ( (ret = cmsObj_getNext
         (MDMOID_IP_SEC_CFG, &iidStack, (void **) &ipsecObj)) == CMSRET_SUCCESS) 
   {
      fprintf(fs, "   <tr>\n");
      fprintf(fs, "      <td>%s</td>\n", ipsecObj->connName);
      fprintf(fs, "      <td>%s</td>\n", ipsecObj->remoteGWAddress);
      fprintf(fs, "      <td>%s</td>\n", ipsecObj->localIPAddress);
      fprintf(fs, "      <td>%s</td>\n", ipsecObj->remoteIPAddress);
      fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n", ipsecObj->connName);
      fprintf(fs, "   </tr>\n");
      // Free the mem allocated this object by the get API.
      cmsObj_free((void **) &ipsecObj);
   }
}

int bcmIPSecNumEntries_igd()
{
   CmsRet ret = CMSRET_SUCCESS;
   int numTunCfg = 0;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   IPSecCfgObject *ipsecObj = NULL;

   while ( (ret = cmsObj_getNext
         (MDMOID_IP_SEC_CFG, &iidStack, (void **) &ipsecObj)) == CMSRET_SUCCESS) 
   {
      numTunCfg++;
      // Free the mem allocated this object by the get API.
      cmsObj_free((void **) &ipsecObj);
   }
   return numTunCfg;
}
#endif

// Print the content of drop down list for certificate selection in edit page
void cgiPrintCertList(char *print)
{
   char *prtloc = print;

#ifdef SUPPORT_CERT
   CertificateCfgObject *certObj;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    int wsize;
   CmsRet ret = CMSRET_SUCCESS;

   while ( (ret = cmsObj_getNext
         (MDMOID_CERTIFICATE_CFG, &iidStack, (void **) &certObj)) == CMSRET_SUCCESS)
   {
      UINT32 inst = iidStack.instance[iidStack.currentDepth-1];
      if (strcmp(certObj->type, CERT_TYPE_SIGNED) == 0) {
         sprintf(prtloc, "certificateName[%d] = new Option('%s', '%s');\n%n", 
                 inst, certObj->name, certObj->name, &wsize); prtloc +=wsize;
      }
      cmsObj_free((void **) &certObj);
    }
#endif
    *prtloc = 0;
}

/*
 * cgiPrintEnetDiag is replacing BcmLan_printEnetDiag. in utils.c.
 * This routine print ENET diagnostics page.
 */
void cgiPrintIPSecTable_igd(char *varValue)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   IPSecCfgObject *ipsecObj=NULL;
   char *prtloc = varValue;
   char chkstr[10];
   int  wsize;

   cmsLog_debug("Enter");

   while (cmsObj_getNext(MDMOID_IP_SEC_CFG, &iidStack,
                         (void **) &ipsecObj) == CMSRET_SUCCESS)
   {
      UINT32 inst = iidStack.instance[iidStack.currentDepth-1];
      if (ipsecObj->enable) {
         strcpy(chkstr, "checked");
      } else {
         strcpy(chkstr, "");
      }
      sprintf(prtloc, "<tr>\n%n", &wsize); prtloc +=wsize;
      sprintf(prtloc, "<td align='center'><input type=checkbox name='connEn%d' %s onClick='enClick(%d, document.forms[0].connEn%d);'></td>\n%n", 
      inst, chkstr, inst, inst, &wsize); prtloc +=wsize;
      sprintf(prtloc, "<td align='center'>%s</td>\n%n", ipsecObj->connName, &wsize); prtloc +=wsize;
      sprintf(prtloc, "<td align='center'>%s</td>\n%n", ipsecObj->remoteGWAddress, &wsize); prtloc +=wsize;
      sprintf(prtloc, "<td align='center'>%s</td>\n%n", ipsecObj->localIPAddress, &wsize); prtloc +=wsize;
      sprintf(prtloc, "<td align='center'>%s</td>\n%n", ipsecObj->remoteIPAddress, &wsize); prtloc +=wsize;
      sprintf(prtloc, "</tr>\n%n", &wsize); prtloc +=wsize;

      cmsObj_free((void **) &ipsecObj);
    }
    *prtloc = 0;

}

void cgiIPSec(char *query, FILE *fs) 
{
   char action[BUFLEN_264];

   cgiGetValueByName(query, "action", action);

   if ( strcmp(action, "add") == 0 )
   {
      cgiIPSecAdd(query, fs);
   }
   else if ( strcmp(action, "remove") == 0 )
   {
      cgiIPSecRemove(query, fs);
   }
   else if ( strcmp(action, "save") == 0 )
   {
      cgiIPSecSave(fs);
   }
   else
   {
      cgiIPSecView(fs);
   }
}


void cgiIPSecAdd(char *query, FILE *fs) 
{
   char cmdPR[BUFLEN_128];
   char ph1KeyTime[BUFLEN_16];
   char ph2KeyTime[BUFLEN_16];
   CmsRet ret = CMSRET_SUCCESS;

   cmdPR[0] = ph1KeyTime[0] = ph2KeyTime[0] = '\0';
   cgiGetValueByName(query, "ipsConnName", glbWebVar.ipsConnName);
   cgiGetValueByName(query, "ipsIpver", glbWebVar.ipsIpver);
   cgiGetValueByName(query, "ipsLocalGwIf", glbWebVar.ipsLocalGwIf);
   cgiGetValueByName(query, "ipsTunMode", glbWebVar.ipsTunMode);
   cgiGetValueByName(query, "ipsRemoteGWAddr", glbWebVar.ipsRemoteGWAddr);
   cgiGetValueByName(query, "ipsLocalIPMode", glbWebVar.ipsLocalIPMode);
   cgiGetValueByName(query, "ipsLocalIP", glbWebVar.ipsLocalIP);
   cgiGetValueByName(query, "ipsLocalMask", glbWebVar.ipsLocalMask);
   cgiGetValueByName(query, "ipsRemoteIPMode", glbWebVar.ipsRemoteIPMode);
   cgiGetValueByName(query, "ipsRemoteIP", glbWebVar.ipsRemoteIP);
   cgiGetValueByName(query, "ipsRemoteMask", glbWebVar.ipsRemoteMask);
   cgiGetValueByName(query, "ipsKeyExM", glbWebVar.ipsKeyExM);
   cgiGetValueByName(query, "ipsAuthM", glbWebVar.ipsAuthM);
   cgiGetValueByName(query, "ipsPSK", glbWebVar.ipsPSK);
   cgiGetValueByName(query, "ipsCertificateName", glbWebVar.ipsCertificateName);
   cgiGetValueByName(query, "ipsPerfectFSEn", glbWebVar.ipsPerfectFSEn);
   cgiGetValueByName(query, "ipsManualEncryptionAlgo", 
                     glbWebVar.ipsManualEncryptionAlgo);
   cgiGetValueByName(query, "ipsManualEncryptionKey", 
                     glbWebVar.ipsManualEncryptionKey);
   cgiGetValueByName(query, "ipsManualAuthAlgo", glbWebVar.ipsManualAuthAlgo);
   cgiGetValueByName(query, "ipsManualAuthKey", glbWebVar.ipsManualAuthKey);
   cgiGetValueByName(query, "ipsSPI", glbWebVar.ipsSPI);
   cgiGetValueByName(query, "ipsPh1Mode", glbWebVar.ipsPh1Mode);
   cgiGetValueByName(query, "ipsPh1EncryptionAlgo", glbWebVar.ipsPh1EncryptionAlgo);
   cgiGetValueByName(query, "ipsPh1IntegrityAlgo", glbWebVar.ipsPh1IntegrityAlgo);
   cgiGetValueByName(query, "ipsPh1DHGroup", glbWebVar.ipsPh1DHGroup);
   cgiGetValueByName(query, "ipsPh1KeyTime", ph1KeyTime);
   glbWebVar.ipsPh1KeyTime = atoi(ph1KeyTime);
   cgiGetValueByName(query, "ipsPh2EncryptionAlgo", glbWebVar.ipsPh2EncryptionAlgo);
   cgiGetValueByName(query, "ipsPh2IntegrityAlgo", glbWebVar.ipsPh2IntegrityAlgo);
   cgiGetValueByName(query, "ipsPh2DHGroup", glbWebVar.ipsPh2DHGroup);
   cgiGetValueByName(query, "ipsPh2KeyTime", ph2KeyTime);
   glbWebVar.ipsPh2KeyTime = atoi(ph2KeyTime);

   if (bcmIPSecNumEntries() < MAX_IPSEC_TUNNELS) {
      /*
       * Create a new object instance and add it to the
       * tunnel configuration.
       */
      ret = dalIPSec_addTunnel(&glbWebVar);
   } else {
      cmsLog_error("IPSec Error: Tunnel table full");
      sprintf(cmdPR, "Configure IPSec Tunnel error because max tunnels exceeded");
      return;
   }
   if ( ret != CMSRET_SUCCESS ) 
   {
      sprintf(cmdPR, "Add IPSec Tunnel named %s failed. Status: %d.", 
              glbWebVar.ipsConnName, ret);
      cgiWriteMessagePage(fs, "IPSec Tunnel Add Error", cmdPR, 
                          "ipsec.cmd?action=view");
   } 
   else 
   {
      glbSaveConfigNeeded = TRUE;
      cgiIPSecView(fs);
   }

}


void cgiIPSecRemove(char *query, FILE *fs) 
{
   char *pToken = NULL, *pLast = NULL;
   char lst[BUFLEN_1024], cmd[BUFLEN_264];
   CmsRet ret = CMSRET_SUCCESS;

   cgiGetValueByName(query, "rmLst", lst);
   pToken = strtok_r(lst, ",", &pLast);
   while ( pToken != NULL )
   {
      ret = dalIPSec_deleteTunnel(pToken);
      if ( ret != CMSRET_SUCCESS )
         break;

      pToken = strtok_r(NULL, ",", &pLast);
   }

   if ( ret == CMSRET_SUCCESS )
   {
      glbSaveConfigNeeded = TRUE;
      cgiIPSecView(fs);
   }
   else
   {
      sprintf(cmd, "Cannot remove IPSec configuration entry.<br>" 
                   "Status: %d.", ret);
      cgiWriteMessagePage(fs, "IPSec Config Remove Error", 
                          cmd, "ipsec.cmd?action=view");
   }
}



void cgiIPSecSave(FILE *fs)
{
   glbSaveConfigNeeded = TRUE;
   cgiIPSecView(fs);
}


void cgiIPSecView(FILE *fs) 
{
   writeIPSecHeader(fs);
   writeIPSecScript(fs);
   writeIPSecBody(fs);
   fprintf(fs, "<b>IPSec Tunnel Mode Connections</b><br><br>\n");
   fprintf(fs, "Add, remove or enable/disable IPSec tunnel connections from this page.<br><br>\n");
   fprintf(fs, "<center>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
   /* write table header */
   fprintf(fs, "   <tr>\n");
   fprintf(fs, "      <td class='hd'>Connection Name</td>\n");
   fprintf(fs, "      <td class='hd'>Remote Gateway</td>\n");
   fprintf(fs, "      <td class='hd'>Local Addresses</td>\n");
   fprintf(fs, "      <td class='hd'>Remote Addresses</td>\n");
   fprintf(fs, "      <td class='hd'>Remove</td>\n");
   fprintf(fs, "   </tr>\n");

   /* write table body */
   fillIPSecInfo(fs);
   
   fprintf(fs, "</table><br>\n");
   fprintf(fs, "<input type='button' onClick='addClick()' value='Add New Connection'>\n");
   fprintf(fs, "<input type='button' onClick='removeClick(this.form.rml)' value='Remove'><br><br>\n");
   
   fprintf(fs, "</center>\n");
   fprintf(fs, "</form>\n");
   fprintf(fs, "</blockquote>\n");
   fprintf(fs, "</body>\n");
   fprintf(fs, "</html>\n");
   fflush(fs);
}

void writeIPSecScript(FILE *fs) 
{
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "function addClick() {\n");
   fprintf(fs, "   var loc = 'ipsconfig.html';\n\n");
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function removeClick(rml) {\n");
   fprintf(fs, "   var lst = '';\n");
   fprintf(fs, "   if (rml.length > 0)\n");
   fprintf(fs, "      for (i = 0; i < rml.length; i++) {\n");
   fprintf(fs, "         if ( rml[i].checked == true )\n");
   fprintf(fs, "            lst += rml[i].value + ',';\n");
   fprintf(fs, "      }\n");
   fprintf(fs, "   else if ( rml.checked == true )\n");
   fprintf(fs, "      lst = rml.value;\n");
   fprintf(fs, "   var loc = 'ipsec.cmd?action=remove&rmLst=' + lst;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
}

#endif /* SUPPORT_IPSEC */
