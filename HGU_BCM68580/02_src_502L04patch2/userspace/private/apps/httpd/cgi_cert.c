/***********************************************************************
 *
 *  Copyright (c) 2006-2010  Broadcom Corporation
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

#ifdef SUPPORT_CERT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>

#include "cms.h"
#include "httpd.h"
#include "cgi_main.h"
#include "cgi_cert.h"
#include "syscall.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_dal.h"

//#define CERT_DEBUG

extern WEB_NTWK_VAR glbWebVar; // this global var is defined in cgimain.c

// Main entry for local certificate web pages
void cgiCertLocal(char *query, FILE *fs) {
   char action[BUFLEN_264];

   cgiGetValueByName(query, "action", action);

#ifdef CERT_DEBUG
   printf("cgiCertLocal: action %s\n", action);
#endif

   if ( cmsUtl_strcmp(action, "preadd") == 0 )
      cgiCertPreAdd(fs, CERT_LOCAL);
   else if ( cmsUtl_strcmp(action, "preimport") == 0 )
      cgiCertPreImport(fs, CERT_LOCAL);
   else if ( cmsUtl_strcmp(action, "preload") == 0 )
      cgiCertPreLoad(query, fs, CERT_LOCAL);
   else if ( cmsUtl_strcmp(action, "add") == 0 )
      cgiCertAdd(query, fs, CERT_LOCAL);
   else if ( cmsUtl_strcmp(action, "import") == 0 )
      cgiCertImport(query, fs, CERT_LOCAL);
   else if ( cmsUtl_strcmp(action, "remove") == 0 )
      cgiCertRemove(query, fs, CERT_LOCAL);
   else if ( cmsUtl_strcmp(action, "show") == 0 )
      cgiCertShow(query, fs, CERT_LOCAL);
   else if ( cmsUtl_strcmp(action, "load") == 0 )
      cgiCertLoadSigned(query, fs, CERT_LOCAL);
   else
      cgiCertView(fs, CERT_LOCAL);
}

// Main entry for CA certificate web pages
void cgiCertCA(char *query, FILE *fs) {
   char action[BUFLEN_264];

   cgiGetValueByName(query, "action", action);

#ifdef CERT_DEBUG
   printf("cgiCertCA: action %s\n", action);
#endif

   if ( cmsUtl_strcmp(action, "preadd") == 0 )
      cgiCertPreAdd(fs, CERT_CA);
   else if ( cmsUtl_strcmp(action, "preimport") == 0 )
      cgiCertPreImport(fs, CERT_CA);
   else if ( cmsUtl_strcmp(action, "preload") == 0 )
      cgiCertPreLoad(query, fs, CERT_CA);
   else if ( cmsUtl_strcmp(action, "add") == 0 )
      cgiCertAdd(query, fs, CERT_CA);
   else if ( cmsUtl_strcmp(action, "import") == 0 )
      cgiCertImport(query, fs, CERT_CA);
   else if ( cmsUtl_strcmp(action, "remove") == 0 )
      cgiCertRemove(query, fs, CERT_CA);
   else if ( cmsUtl_strcmp(action, "show") == 0 )
      cgiCertShow(query, fs, CERT_CA);
   else if ( cmsUtl_strcmp(action, "load") == 0 )
      cgiCertLoadSigned(query, fs, CERT_CA);
   else
      cgiCertView(fs, CERT_CA);
}

// Called before "add" page
void cgiCertPreAdd(FILE *fs, int type)
{
    char filename[HTTPD_BUFLEN_10K];

    if (type == CERT_LOCAL) {
        if (dalCert_getNumberOfExistedCert(type) >= CERT_LOCAL_MAX_ENTRY) {
            cgiCertErr(fs, "Cannot add more local certificate entries", type);
        }
        else {
            strcpy(glbWebVar.certCategory, "local");
            makePathToWebPage(filename, HTTPD_BUFLEN_10K, "certadd.html");
            do_ej(filename, fs);
        }
    }
    else {
        if (dalCert_getNumberOfExistedCert(type) >= CERT_CA_MAX_ENTRY) {
            cgiCertErr(fs, "Cannot add more CA certificate entries", type);
        }
        else {
            strcpy(glbWebVar.certCategory, "ca");
            makePathToWebPage(filename, HTTPD_BUFLEN_10K, "certadd.html");
            do_ej(filename, fs);
        }
    }
}

// Called before "import" page
void cgiCertPreImport(FILE *fs, int type) 
{
    char filename[HTTPD_BUFLEN_10K];

    if (type == CERT_LOCAL) {
        if (dalCert_getNumberOfExistedCert(type) >= CERT_LOCAL_MAX_ENTRY) {
            cgiCertErr(fs, "Cannot add more local certificate entries", type);
        }
        else {
            strcpy(glbWebVar.certCategory, "local");
            makePathToWebPage(filename, HTTPD_BUFLEN_10K, "certimport.html");
            do_ej(filename, fs);
        }
    }
    else {
        if (dalCert_getNumberOfExistedCert(type) >= CERT_CA_MAX_ENTRY) {
            cgiCertErr(fs, "Cannot add more CA certificate entries", type);
        }
        else {
            strcpy(glbWebVar.certCategory, "ca");
            makePathToWebPage(filename, HTTPD_BUFLEN_10K, "certcaimport.html");
            do_ej(filename, fs);
        }
    }
}

// Called before "load signed" page
void cgiCertPreLoad(char *query, FILE *fs, int type) 
{
    char name[CERT_NAME_LEN];
    char filename[HTTPD_BUFLEN_10K];

    if (type == CERT_LOCAL) {
        strcpy(glbWebVar.certCategory, "local");
    }
    else {
        strcpy(glbWebVar.certCategory, "ca");
    }
        
    cgiGetValueByName(query, "name", name);
    strcpy(glbWebVar.certName, name);
    
    makePathToWebPage(filename, HTTPD_BUFLEN_10K, "certloadsigned.html");
    do_ej(filename, fs);
}

// Add new certificate request
void cgiCertAdd(char *query, FILE *fs, int type) {
   char cmd[BUFLEN_264], name[CERT_NAME_LEN];
   CmsRet ret = CMSRET_SUCCESS;
   CERT_ADD_INFO addinfo;
   CertificateCfgObject *certCfg = NULL;

   if ((ret = cmsObj_get(MDMOID_CERTIFICATE_CFG, NULL,
        OGF_DEFAULT_VALUES, (void **) &certCfg)) != CMSRET_SUCCESS)
   {
      cgiCertErr(fs, " Certificate Add Error: Cannot allocate memory.", type);
      return;
   }       

#ifdef CERT_DEBUG
   printf("cgiCertAdd: query %s\n", query);
#endif

   cgiGetValueByName(query, "certName", name);
   CMSMEM_FREE_BUF_AND_NULL_PTR(certCfg->name);
   certCfg->name = cmsMem_strdup(name);
   CMSMEM_FREE_BUF_AND_NULL_PTR(certCfg->type);
   certCfg->type = cmsMem_strdup(CERT_TYPE_SIGNING_REQ);
   cgiGetValueByName(query, "certCommonName", addinfo.commonName);
   cgiGetValueByName(query, "certCountry", addinfo.country);
   cgiGetValueByName(query, "certState", addinfo.state);
   cgiGetValueByName(query, "certOrg", addinfo.organization);

   if (dalCert_findCert(name, type) == TRUE) 
   {
       cgiCertErr(fs, " A certificate with the same name already exists", type);
   }
   else 
   {
      ret = rutCert_createCertReq(&addinfo, certCfg);
      if (ret == CMSRET_SUCCESS)
      {
         ret = rutCert_retrieveSubject(certCfg);
         if (ret == CMSRET_SUCCESS)
         {
            ret = dalCert_addCert(certCfg);
         }
      }

      if (ret != CMSRET_SUCCESS)
      {
          sprintf(cmd, "Cannot add certificate named %s.<br> Status: %d.", name, ret);
          cgiCertErr(fs, cmd, type);
      } 
      else 
      {
         cgiCertReqShow(fs, certCfg);
         glbSaveConfigNeeded = TRUE;
      }
   }

   /* free certCfg object */
   cmsObj_free((void **) &certCfg);
}

// Import certificate
void cgiCertImport(char *query, FILE *fs, int type) 
{
   char name[CERT_NAME_LEN], content[CERT_KEY_MAX_LEN];
   char cmd[BUFLEN_264], privKey[CERT_KEY_MAX_LEN];
   CertificateCfgObject *certCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   if (cmsObj_get(MDMOID_CERTIFICATE_CFG, NULL,
        OGF_DEFAULT_VALUES, (void **) &certCfg) != CMSRET_SUCCESS)
   {
      cgiCertErr(fs, " Certificate Add Error: Cannot allocate memory.", type);
      return;
   }       

#ifdef CERT_DEBUG
    printf("cgiCertImport: query %s\n", query);
#endif

   cgiGetValueByName(query, "certName", name);
   CMSMEM_FREE_BUF_AND_NULL_PTR(certCfg->name);
   certCfg->name = cmsMem_strdup(name);
   cgiGetValueByName(query, "certPublic", content);
   CMSMEM_FREE_BUF_AND_NULL_PTR(certCfg->content);
   certCfg->content = cmsMem_strdup(content);

   if (type == CERT_CA) 
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(certCfg->type);
      certCfg->type = cmsMem_strdup(CERT_TYPE_CA);
   }
   else
   {
      cgiGetValueByName(query, "certPrivate", privKey);
      CMSMEM_FREE_BUF_AND_NULL_PTR(certCfg->privKey);
      certCfg->privKey = cmsMem_strdup(privKey);
      CMSMEM_FREE_BUF_AND_NULL_PTR(certCfg->type);
      certCfg->type = cmsMem_strdup(CERT_TYPE_SIGNED);
   }

   if (dalCert_findCert(name, type) == TRUE) 
   {
      cgiCertErr(fs, " A certificate with the same name already exists", type);
   }
   else
   {
      if (rutCert_processImportedCert(certCfg) != CMSRET_SUCCESS) 
      {
         cgiCertErr(fs, "Error in imported certificate", type);
      }
      else 
      {
         ret = dalCert_addCert(certCfg);
         if (ret == CMSRET_SUCCESS)
         {
            cgiCertView(fs, type);
            glbSaveConfigNeeded = TRUE;
         }
         else
         {
             sprintf(cmd, "Cannot add certificate named %s.<br> Status: %d.", name, ret);
             cgiCertErr(fs, cmd, type);
         }
      }
   }

   /* free certCfg object */
   cmsObj_free((void **) &certCfg);
}


// Load signed certificate
void cgiCertLoadSigned(char *query, FILE *fs, int type)
{
   char name[CERT_NAME_LEN], content[CERT_KEY_MAX_LEN];
   CmsRet ret = CMSRET_SUCCESS;
   CertificateCfgObject *certCfg = NULL;

   if ((ret = cmsObj_get(MDMOID_CERTIFICATE_CFG, NULL,
        OGF_DEFAULT_VALUES, (void **) &certCfg)) != CMSRET_SUCCESS)
   {
      cgiCertErr(fs, " Certificate Add Error: Cannot allocate memory.", type);
      return;
   }

#ifdef CERT_DEBUG
   printf("cgiCertLoadSignedvoi: query %s\n", query);
#endif

   cgiGetValueByName(query, "certName", name);

   ret = dalCert_getCert(name, type, certCfg);
   if (ret == CMSRET_SUCCESS) 
   {
      if (cmsUtl_strcmp(certCfg->type, CERT_TYPE_SIGNING_REQ) == 0) 
      {
         cgiGetValueByName(query, "certPublic", content);
         CMSMEM_FREE_BUF_AND_NULL_PTR(certCfg->content);
         certCfg->content = cmsMem_strdup(content);
         CMSMEM_FREE_BUF_AND_NULL_PTR(certCfg->type);
         certCfg->type = cmsMem_strdup(CERT_TYPE_SIGNED);
         if (rutCert_verifyCertReq(certCfg) == CMSRET_SUCCESS) 
         {
            ret = dalCert_setCert(name, type, certCfg);
            cgiCertView(fs, type);
            glbSaveConfigNeeded = TRUE;
         }
         else 
         {
            cgiCertErr(fs, "Certificate doesn't match request", type);
         }
      }
      else 
      {
         cgiCertErr(fs, "Certificate type error", type);
      }
   }
   else 
   {
      cgiCertErr(fs, "Certificate name not found", type);
   }

   /* free certCfg object */
   cmsObj_free((void **) &certCfg);
}


// Remove certificate
void cgiCertRemove(char *query, FILE *fs, int type) {
   char cmd[WEB_MD_BUF_SIZE_MAX], name[CERT_NAME_LEN];
   CmsRet ret = CMSRET_SUCCESS;

#ifdef CERT_DEBUG
   printf("cgiCertRemove: query %s\n", query);
#endif

   cgiGetValueByName(query, "certName", name);
   ret = dalCert_delCert(name, type);

   if (ret == CMSRET_SUCCESS) 
   {
      cgiCertView(fs, type);
      glbSaveConfigNeeded = TRUE;
   } 
   else 
   {
      sprintf(cmd, "Cannot remove certificate named %s.<br> Status: %d.", name, ret);
      cgiCertErr(fs, cmd, type);
   }
}


// Certificate main page
void cgiCertView(FILE *fs, int type) {
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CertificateCfgObject *certCfg = NULL;

#ifdef CERT_DEBUG
   printf("cgiCertView:\n");
#endif

   if (type == CERT_LOCAL) {
       // reset reference counts for all local certificates
       dalCert_resetRefCount();
   }
   
   writeCertHeader(fs);
   writeCertScript(fs, type);
   writeCertBody(fs);
   if (type == CERT_LOCAL) {
       fprintf(fs, "<b>Local Certificates</b><br><br>\n");
       fprintf(fs, "Add, View or Remove certificates from this page.\n");
       fprintf(fs, "Local certificates are used by peers to verify your identity.<br>\n");
       fprintf(fs, "Maximum %d certificates can be stored.<br>\n", CERT_LOCAL_MAX_ENTRY);
   }
   else {
       fprintf(fs, "<b>Trusted CA (Certificate Authority) Certificates</b><br><br>\n");
       fprintf(fs, "Add, View or Remove certificates from this page.\n");
       fprintf(fs, "CA certificates are used by you to verify peers' certificates.<br>\n");
       fprintf(fs, "Maximum %d certificates can be stored.<br>\n", CERT_CA_MAX_ENTRY);
   }
   fprintf(fs, "<br><br><center>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
   // write table header
   fprintf(fs, "   <tr>\n");
   fprintf(fs, "      <td class='hd'>Name</td>\n");
   if (type == CERT_LOCAL) {
       fprintf(fs, "      <td class='hd'>In Use</td>\n");
   }
   fprintf(fs, "      <td class='hd'>Subject</td>\n");
   fprintf(fs, "      <td class='hd'>Type</td>\n");
   fprintf(fs, "      <td class='hd'>Action</td>\n");
   fprintf(fs, "   </tr>\n");

   // write table body
   while (cmsObj_getNext(MDMOID_CERTIFICATE_CFG, &iidStack, (void **) &certCfg) == CMSRET_SUCCESS) 
   {
      // skip certificates that do not match with type
      if ((type == CERT_LOCAL && cmsUtl_strcmp(certCfg->type, CERT_TYPE_CA) == 0) ||
          (type == CERT_CA && cmsUtl_strcmp(certCfg->type, CERT_TYPE_CA) != 0))
      {
         cmsObj_free((void **) &certCfg);
         continue;
      }
      
      fprintf(fs, "   <tr>\n");
      fprintf(fs, "      <td>%s</td>\n", certCfg->name);
      if (type == CERT_LOCAL) {
          if (certCfg->refCount > 0) {
             fprintf(fs, "      <td>&nbsp Yes (%d)</td>\n", certCfg->refCount);
          }
          else
          {
             fprintf(fs, "      <td>&nbsp</td>\n");
          }
      }
      fprintf(fs, "      <td>%s</td>\n", certCfg->subject);
      fprintf(fs, "      <td>%s</td>\n", certCfg->type);
      fprintf(fs, "      <td align='center'><input value='View' type='button' onclick=\"viewClick('%s');\"> &nbsp ", certCfg->name);
      if (cmsUtl_strcmp(certCfg->type, CERT_TYPE_SIGNING_REQ) == 0) {
        fprintf(fs, "      <input value='Load Signed' type='button' onclick=\"loadClick('%s');\"> &nbsp ", certCfg->name);
      }
      if (type == CERT_LOCAL) {
         fprintf(fs, "      <input value='Remove' type='button' onclick=\"removeClick('%s', '%d');\"></td>\n", certCfg->name, certCfg->refCount);
      }
      else {
         fprintf(fs, "      <input value='Remove' type='button' onclick=\"removeClick('%s', '0');\"></td>\n", certCfg->name);
      }
      fprintf(fs, "   </tr>\n");
      /* free certCfg object */
      cmsObj_free((void **) &certCfg);
   }

   fprintf(fs, "</table><br>\n");
   if (type == CERT_LOCAL) {
      fprintf(fs, "<input type='button' onClick='addClick()' value='Create Certificate Request'> &nbsp &nbsp ");
   }
   fprintf(fs, "<input type='button' onClick='importClick()' value='Import Certificate'>\n");
   fprintf(fs, "</center>\n");
   fprintf(fs, "</form>\n");
   fprintf(fs, "</blockquote>\n");
   fprintf(fs, "</body>\n");
   fprintf(fs, "</html>\n");
   fflush(fs);
}


// Detailed information about a single certificate
void cgiCertShow(char *query, FILE *fs, int type) {
   char name[CERT_NAME_LEN];
   CertificateCfgObject *certCfg = NULL;

   if (cmsObj_get(MDMOID_CERTIFICATE_CFG, NULL,
        OGF_DEFAULT_VALUES, (void **) &certCfg) != CMSRET_SUCCESS)
   {
      cgiCertErr(fs, " Certificate Show Error: Cannot allocate memory.", type);
      return;
   }

   cgiGetValueByName(query, "certName", name);
   dalCert_getCert(name, type, certCfg);

   writeCertHeader(fs);
   writeCertBody(fs);

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "<b>Certificate Details</b><br><br>\n");
   fprintf(fs, "<center>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
   fprintf(fs, "<tr>\n");
   fprintf(fs, "<td>Name</td> <td>%s</td>\n", certCfg->name);
   fprintf(fs, "</tr>\n");
   fprintf(fs, "<tr>\n");
   fprintf(fs, "<td>Type</td> <td>%s</td>\n", certCfg->type);
   fprintf(fs, "</tr>\n");
   fprintf(fs, "<tr>\n");
   fprintf(fs, "<td>Subject</td> <td>%s</td>\n", certCfg->subject);
   fprintf(fs, "</tr>\n");
   fprintf(fs, "<tr>\n");
   fprintf(fs, "<td>Certificate</td> <td><textarea cols = '80' rows='20'>\n%s</textarea></td>\n", certCfg->content);
   fprintf(fs, "</tr>\n");
   if (type != CERT_CA) {
       fprintf(fs, "<tr>\n");
       fprintf(fs, "<td>Private Key</td> <td><textarea cols = '80' rows='20'>\n%s</textarea></td>\n", certCfg->privKey);
       fprintf(fs, "</tr>\n");
       fprintf(fs, "<tr>\n");
       fprintf(fs, "<td>Signing Request</td> <td><textarea cols = '80' rows='20'>\n%s</textarea></td>\n", certCfg->reqPub);
       fprintf(fs, "</tr>\n");
   }
   fprintf(fs, "</table><br>\n");

   if (type == CERT_CA) {
       fprintf(fs, "<input type='button' value='&nbsp Back &nbsp' onClick=\"history.back(-1)\">\n");
   }
   else {
       fprintf(fs, "<input type='button' value='&nbsp Back &nbsp' onClick=\"history.back(-1)\">\n");
       if (cmsUtl_strcmp(certCfg->type, CERT_TYPE_SIGNING_REQ) == 0) {
           fprintf(fs, "<input type='button' value='Load Signed Certificate' onClick=\"eval('location=\"'certlocal.cmd?action=preload&name=%s&sessionKey=%d'\"')\">\n", certCfg->name,glbCurrSessionKey);
       }
   }
   
   fprintf(fs, "</center>\n");
   fprintf(fs, "</blockquote>\n</body>\n");

   /* free certCfg object */
   cmsObj_free((void **) &certCfg);
}


// Show signing request
void cgiCertReqShow(FILE *fs, CertificateCfgObject *certCfg) {

   writeCertHeader(fs);
   writeCertBody(fs);

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "<b>Certificate signing request</b><br>\n");
   fprintf(fs, "Certificate signing request successfully created.  Note a request is not yet functional -\n");
   fprintf(fs, "have it signed by a Certificate Authority and load the signed certificate to this device.<br><br>\n");
   fprintf(fs, "<center>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
   fprintf(fs, "<tr>\n");
   fprintf(fs, "<td>Name</td> <td>%s</td>\n", certCfg->name);
   fprintf(fs, "</tr>\n");
   fprintf(fs, "<tr>\n");
   fprintf(fs, "<td>Type</td> <td>%s</td>\n", certCfg->type);
   fprintf(fs, "</tr>\n");
   fprintf(fs, "<tr>\n");
   fprintf(fs, "<td>Subject</td> <td>%s</td>\n", certCfg->subject);
   fprintf(fs, "</tr>\n");
   fprintf(fs, "<tr>\n");
   fprintf(fs, "<td>Signing Request</td> <td><textarea cols = '80' rows='20'>\n%s</textarea></td>\n", certCfg->reqPub);
   fprintf(fs, "</tr>\n");
   fprintf(fs, "</table><br>\n");
   fprintf(fs, "<input type='button' value='&nbsp Back &nbsp' onClick=\"history.back(-1)\">\n");
   fprintf(fs, "<input type='button' value='Load Signed Certificate' onClick=\"eval('location=\"'certlocal.cmd?action=preload&name=%s&sessionKey=%d'\"')\">\n", certCfg->name,glbCurrSessionKey);
   fprintf(fs, "</center>\n");
   fprintf(fs, "</blockquote>\n</body>\n");
}

// Scripts
void writeCertScript(FILE *fs, int type) {
   char dcmd[BUFLEN_24];

   if (type == CERT_LOCAL) {
       strcpy(dcmd, "certlocal.cmd");
   }
   else {
      strcpy(dcmd, "certca.cmd");
   }
    
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "function addClick() {\n");
   fprintf(fs, "   var loc = '%s?action=preadd';\n", dcmd);
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");
   
   fprintf(fs, "function importClick() {\n");
   fprintf(fs, "   var loc = '%s?action=preimport';\n", dcmd);
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function viewClick(name) {\n");
   fprintf(fs, "   var loc = '%s?action=show&certName=' + name;\n\n", dcmd);
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");
   
   fprintf(fs, "function loadClick(name) {\n");
   fprintf(fs, "   var loc = '%s?action=preload&name=' + name;\n", dcmd);
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function removeClick(name, refCount) {\n");
   fprintf(fs, "   var rem = true;\n");
   fprintf(fs, "   if (refCount > 0) {\n");
   fprintf(fs, "        if (!confirm('This certificate is in use. Other functions may stop working if it is deleted. Are you sure want to force the deletion?')) {\n");
   fprintf(fs, "            rem = false;\n");
   fprintf(fs, "        }\n");
   fprintf(fs, "   }\n");
   fprintf(fs, "   if (rem) {   \n");
   fprintf(fs, "       var loc = '%s?action=remove&certName=' + name;\n\n", dcmd);
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "       var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "       eval(code);\n");
   fprintf(fs, "   }\n");
   fprintf(fs, "}\n\n");
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
}


// Header
void writeCertHeader(FILE *fs) {
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "<script language=\"javascript\" src=\"util.js\"></script>\n");
}


// body
void writeCertBody(FILE *fs) {
   fprintf(fs, "</head>\n<body>\n<blockquote>\n<form>\n");
}

// Error page
void cgiCertErr(FILE *fs, char *message, int type)
{
    char fn[BUFLEN_32];
    
    if (type == CERT_CA) {
        strcpy(fn, "certcal.cmd?action=view");
    }
    else {
        strcpy(fn, "certlocal.cmd?action=view");
    }

    cgiWriteMessagePage(fs, " Certificate Error", message, fn);
}

#endif   //SUPPORT_CERT
