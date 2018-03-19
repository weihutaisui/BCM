/***********************************************************************
 *
 *  Copyright (c) 2009-2013  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2013:proprietary:standard

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


#ifdef DMP_DEVICE2_BASELINE_1

#ifdef DMP_CELLULARINTERFACEBASE_1

#include <sys/types.h>

#include "cms_core.h"
#include "cms_qdm.h"
#include "cms_util.h"
#include "rcl.h"
#include "rut_util.h"
#include "rut2_cellular.h"
#include "sqlite3.h"


CmsRet rutCellularDB_getApnCount(char *mcc, char* mnc, UINT32 *count)
{
   sqlite3 *db;
   int rc;
   char sqlcmd[BUFLEN_256];
   char** result;
   int nRow;
   int nCol;
   
   if((rc = sqlite3_open_v2(RILAPN_TELEPHONY_DB_FILE, &db, SQLITE_OPEN_READONLY, NULL)) != SQLITE_OK)
   {
      cmsLog_error( "sqlite3_open_v2 %s error: %s\n", RILAPN_TELEPHONY_DB_FILE, sqlite3_errmsg(db));
      sqlite3_close(db);
      return CMSRET_INTERNAL_ERROR;
   }
   
   snprintf(sqlcmd, sizeof(sqlcmd), "select count(*) from carriers where mcc=\'%s\' and mnc=\'%s\';", mcc, mnc);  
   if((rc = sqlite3_get_table(db, sqlcmd, &result, &nRow, &nCol, NULL)) != SQLITE_OK)
   {
      cmsLog_error( "sqlite3_get_table error: %s\n", sqlite3_errmsg(db));
      sqlite3_close(db);
      return CMSRET_INTERNAL_ERROR;
   }
   
   *count = atoi(result[1]);

   sqlite3_free_table(result);
   sqlite3_close(db);
   
   cmsLog_debug("Count =%d\n", *count);
   return CMSRET_SUCCESS;
}



CmsRet rutCellularDB_getApnByOffset(char *mcc, char* mnc, UINT32 offset, Dev2CellularApnInfo *cellularApnInfo)
{
   sqlite3 *db;
   sqlite3_stmt *stmt;
   int rc;
   char sqlcmd[BUFLEN_256];
   char *p;
   
   if((rc = sqlite3_open_v2(RILAPN_TELEPHONY_DB_FILE, &db, SQLITE_OPEN_READONLY, NULL)) != SQLITE_OK)
   {
      cmsLog_error( "sqlite3_open_v2 %s error: %s\n", RILAPN_TELEPHONY_DB_FILE, sqlite3_errmsg(db));
      sqlite3_close(db);
      return CMSRET_INTERNAL_ERROR;
   }

   snprintf(sqlcmd, sizeof(sqlcmd), "select * from carriers where mcc=\'%s\' and mnc=\'%s\' limit 1 offset %d;", mcc, mnc, offset);  
   if ((rc = sqlite3_prepare_v2(db, sqlcmd, -1, &stmt, NULL)) != SQLITE_OK)
   {
     cmsLog_error("sqlite3_prepare_v2 \"%s\" error: %s", sqlcmd, sqlite3_errmsg(db));
     sqlite3_close(db);
     return CMSRET_INTERNAL_ERROR;
   }
  
   if ((rc = sqlite3_step(stmt)) != SQLITE_ROW)
   {
     cmsLog_error("sqlite3_step error: %s", sqlite3_errmsg(db));
     sqlite3_finalize(stmt);
     sqlite3_close(db);
     return CMSRET_INTERNAL_ERROR;
   }

   cellularApnInfo->id = sqlite3_column_int(stmt, 0);

   p = (char*) sqlite3_column_text( stmt, 1);
   cmsUtl_strncpy(cellularApnInfo->carrier, p, sizeof(cellularApnInfo->carrier));
   
   p = (char*) sqlite3_column_text( stmt, 3);
   cmsUtl_strncpy(cellularApnInfo->MCC, p, sizeof(cellularApnInfo->MCC));
   
   p = (char*) sqlite3_column_text( stmt, 4);
   cmsUtl_strncpy(cellularApnInfo->MNC, p, sizeof(cellularApnInfo->MNC));
   
   p = (char*) sqlite3_column_text( stmt, 5);
   cmsUtl_strncpy(cellularApnInfo->APN, p, sizeof(cellularApnInfo->APN));
   
   p = (char*) sqlite3_column_text( stmt, 6);
   cmsUtl_strncpy(cellularApnInfo->username, p, sizeof(cellularApnInfo->username));
      
   p = (char*) sqlite3_column_text( stmt, 8);
   cmsUtl_strncpy(cellularApnInfo->password, p, sizeof(cellularApnInfo->password));
   
   p = (char*) sqlite3_column_text( stmt, 9);
   cmsUtl_strncpy(cellularApnInfo->proxy, p, sizeof(cellularApnInfo->proxy));
   
   p = (char*) sqlite3_column_text( stmt, 10);
   cellularApnInfo->proxyPort = p ? atoi(p) : 0;
   
   p = (char*) sqlite3_column_text( stmt, 11);
   cmsUtl_strncpy(cellularApnInfo->mmsproxy, p, sizeof(cellularApnInfo->mmsproxy));
   
   p = (char*) sqlite3_column_text( stmt, 12);
   cellularApnInfo->mmsport = p ? atoi(p) : 0;
   
   p = (char*) sqlite3_column_text( stmt, 13);
   cmsUtl_strncpy(cellularApnInfo->mmsc, p, sizeof(cellularApnInfo->mmsc));
   
   p = (char*) sqlite3_column_text( stmt, 14);
   cellularApnInfo->authtype = p ? atoi(p) : -1;
  
   p = (char*) sqlite3_column_text( stmt, 15);
   cmsUtl_strncpy(cellularApnInfo->type, p, sizeof(cellularApnInfo->type));

   p = (char*) sqlite3_column_text( stmt, 17);
   cellularApnInfo->enblIpVer = p ? (cmsUtl_strstr(p, "IPV4V6") ? ENBL_IPV4_IPV6 :
                               (cmsUtl_strstr(p, "IPV6") ? ENBL_IPV6_ONLY : ENBL_IPV4_ONLY)): 
                                ENBL_IPV4_ONLY;

   sqlite3_finalize(stmt);
   sqlite3_close(db);

   return CMSRET_SUCCESS;
}


/* PARAM IN:  cellularApnInfo->id */
CmsRet rutCellularDB_getApnById(Dev2CellularApnInfo *cellularApnInfo)
{
   sqlite3 *db;
   sqlite3_stmt *stmt;
   int rc;
   char sqlcmd[BUFLEN_256];
   char *p;
   
   if((rc = sqlite3_open_v2(RILAPN_TELEPHONY_DB_FILE, &db, SQLITE_OPEN_READONLY, NULL)) != SQLITE_OK)
   {
      cmsLog_error( "sqlite3_open_v2 %s error: %s\n", RILAPN_TELEPHONY_DB_FILE, sqlite3_errmsg(db));
      sqlite3_close(db);
      return CMSRET_INTERNAL_ERROR;
   }

   snprintf(sqlcmd, sizeof(sqlcmd), "select * from carriers where _id='%d';", cellularApnInfo->id);  
   if ((rc = sqlite3_prepare_v2(db, sqlcmd, -1, &stmt, NULL)) != SQLITE_OK)
   {
     cmsLog_error("sqlite3_prepare_v2 \"%s\" error: %s", sqlcmd, sqlite3_errmsg(db));
     sqlite3_close(db);
     return CMSRET_INTERNAL_ERROR;
   }
  
   if ((rc = sqlite3_step(stmt)) != SQLITE_ROW)
   {
     cmsLog_error("sqlite3_step error: %s", sqlite3_errmsg(db));
     sqlite3_finalize(stmt);
     sqlite3_close(db);
     return CMSRET_INTERNAL_ERROR;
   }
 
   p = (char*) sqlite3_column_text( stmt, 1);
   cmsUtl_strncpy(cellularApnInfo->carrier, p, sizeof(cellularApnInfo->carrier));
   
   p = (char*) sqlite3_column_text( stmt, 3);
   cmsUtl_strncpy(cellularApnInfo->MCC, p, sizeof(cellularApnInfo->MCC));
   
   p = (char*) sqlite3_column_text( stmt, 4);
   cmsUtl_strncpy(cellularApnInfo->MNC, p, sizeof(cellularApnInfo->MNC));
   
   p = (char*) sqlite3_column_text( stmt, 5);
   cmsUtl_strncpy(cellularApnInfo->APN, p, sizeof(cellularApnInfo->APN));
   
   p = (char*) sqlite3_column_text( stmt, 6);
   cmsUtl_strncpy(cellularApnInfo->username, p, sizeof(cellularApnInfo->username));
      
   p = (char*) sqlite3_column_text( stmt, 8);
   cmsUtl_strncpy(cellularApnInfo->password, p, sizeof(cellularApnInfo->password));
   
   p = (char*) sqlite3_column_text( stmt, 9);
   cmsUtl_strncpy(cellularApnInfo->proxy, p, sizeof(cellularApnInfo->proxy));
   
   p = (char*) sqlite3_column_text( stmt, 10);
   cellularApnInfo->proxyPort = p ? atoi(p) : 0;
   
   p = (char*) sqlite3_column_text( stmt, 11);
   cmsUtl_strncpy(cellularApnInfo->mmsproxy, p, sizeof(cellularApnInfo->mmsproxy));
   
   p = (char*) sqlite3_column_text( stmt, 12);
   cellularApnInfo->mmsport = p ? atoi(p) : 0;
   
   p = (char*) sqlite3_column_text( stmt, 13);
   cmsUtl_strncpy(cellularApnInfo->mmsc, p, sizeof(cellularApnInfo->mmsc));
   
   p = (char*) sqlite3_column_text( stmt, 14);
   cellularApnInfo->authtype = p ? atoi(p) : -1;
   
   p = (char*) sqlite3_column_text( stmt, 15);
   cmsUtl_strncpy(cellularApnInfo->type, p, sizeof(cellularApnInfo->type));

   sqlite3_finalize(stmt);
   sqlite3_close(db);

   return CMSRET_SUCCESS;
}


CmsRet rutCellularDB_addApn(Dev2CellularAccessPointObject *cellularApnObj)
{
   sqlite3 *db;
   int rc;
   char sqlcmd[BUFLEN_256];
   char *ipVer;

   cmsLog_debug("id = %d, apn = %s", cellularApnObj->X_BROADCOM_COM_Id, cellularApnObj->APN);
   
   if((rc = sqlite3_open_v2(RILAPN_TELEPHONY_DB_FILE, &db, SQLITE_OPEN_READWRITE, NULL)) != SQLITE_OK)
   {
      cmsLog_error( "sqlite3_open_v2 %s error: %s\n", RILAPN_TELEPHONY_DB_FILE, sqlite3_errmsg(db));
      return CMSRET_INTERNAL_ERROR;
   }

   ipVer = cellularApnObj->X_BROADCOM_COM_IpVer == ENBL_IPV4_ONLY ? "IP" :
           cellularApnObj->X_BROADCOM_COM_IpVer == ENBL_IPV6_ONLY ? "IPV6" : "IPV4V6";
										 	
   snprintf(sqlcmd, sizeof(sqlcmd), "insert into carriers values(%d, '%s','%s%s','%s','%s','%s','%s','','%s','%s',%d,'%s', %d, '%s', %d,'%s',1,'%s','%s',1,0,'','',NULL,-1)",
            cellularApnObj->X_BROADCOM_COM_Id, cellularApnObj->X_BROADCOM_COM_Carrier, cellularApnObj->X_BROADCOM_COM_MCC, 
            cellularApnObj->X_BROADCOM_COM_MNC, cellularApnObj->X_BROADCOM_COM_MCC, cellularApnObj->X_BROADCOM_COM_MNC,
            cellularApnObj->APN, cellularApnObj->username, cellularApnObj->password,
            cellularApnObj->proxy, cellularApnObj->proxyPort, cellularApnObj->X_BROADCOM_COM_Mmsproxy,
            cellularApnObj->X_BROADCOM_COM_Mmsport, cellularApnObj->X_BROADCOM_COM_Mmsc, cellularApnObj->X_BROADCOM_COM_Authtype, 
            cellularApnObj->X_BROADCOM_COM_Type, ipVer, ipVer);
   
   cmsLog_debug( "sqlcmd: %s\n", sqlcmd);
	  
   if ((rc = sqlite3_exec(db, sqlcmd, NULL, NULL, NULL)) != SQLITE_OK)
   {
      cmsLog_error( "sqlite3_exec %s error: %s\n", sqlcmd, sqlite3_errmsg(db));
      sqlite3_close(db);
      return CMSRET_INTERNAL_ERROR;
   }
   
   sqlite3_close(db);
   return CMSRET_SUCCESS;
}


CmsRet rutCellularDB_DelApn(UINT32 id)
{
   sqlite3 *db;
   int rc;
   char sqlcmd[BUFLEN_256];
   
   cmsLog_debug("id = %d", id);
   
   if((rc = sqlite3_open_v2(RILAPN_TELEPHONY_DB_FILE, &db, SQLITE_OPEN_READWRITE, NULL)) != SQLITE_OK)
   {
      cmsLog_error( "sqlite3_open_v2 %s error: %s\n", RILAPN_TELEPHONY_DB_FILE, sqlite3_errmsg(db));
      return CMSRET_INTERNAL_ERROR;
   }

   snprintf(sqlcmd, sizeof(sqlcmd), "delete from carriers where _id = '%d'", id);
   
   if ((rc = sqlite3_exec(db, sqlcmd, NULL, NULL, NULL)) != SQLITE_OK)
   {
      cmsLog_error( "sqlite3_exec %s error: %s\n", sqlcmd, sqlite3_errmsg(db));
      sqlite3_close(db);
      return CMSRET_INTERNAL_ERROR;
   }
   
   sqlite3_close(db);
   return CMSRET_SUCCESS;
}


CmsRet rutCellular_setPrefdApn(UINT32 id, const InstanceIdStack *skipIidStack)
{
   FILE *fp = NULL;
   char buf[BUFLEN_64];
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2CellularAccessPointObject *cellularApnObj = NULL;
   CmsRet ret;
   
   cmsLog_debug("id = %d", id);

   if (id == 0)   
   {
      while (!id && (ret = cmsObj_getNext(MDMOID_DEV2_CELLULAR_ACCESS_POINT, &iidStack, (void **)&cellularApnObj)) == CMSRET_SUCCESS)
      {
         if (iidStack.instance[0] == skipIidStack->instance[0])   	  
         {
           continue;
         }
		 
         if (cellularApnObj->enable && !cmsUtl_strcmp(cellularApnObj->X_BROADCOM_COM_Type, CELLULAR_APN_TYPE_DEFAULT))		 
         {
           id = cellularApnObj->X_BROADCOM_COM_Id;
         }
		 
         cmsObj_free((void **)&cellularApnObj);
      }
	  
      cmsLog_debug("auto select APN id = %d", id, iidStack.instance[0]);

      /* Use factory default value if no proper APN */
      if (!id)
      {
         snprintf(buf, sizeof(buf), "cp /etc/preferred-apn.xml %s", RILAPN_PREF_APN_FILE);   
         rut_doSystemAction("rutCellular_setPrefdApn", buf);
         return CMSRET_SUCCESS;
      }
   }

   if ((fp = fopen(RILAPN_PREF_APN_FILE, "w+")) == NULL)
   {
      cmsLog_error("could not open file %s", RILAPN_PREF_APN_FILE);
      return CMSRET_INTERNAL_ERROR;
   }
   
   fputs("<?xml version='1.0' encoding='utf-8' standalone='yes' ?>\n", fp);
   fputs("<map>\n", fp);
   snprintf(buf, sizeof(buf), "    <long name=\"apn_id\" value=\"%d\" />\n", id);   
   fputs(buf, fp);
   fputs("</map>\n", fp);
   fclose(fp);

   return CMSRET_SUCCESS;
}


CmsRet rutCellular_getPrefdApn(UINT32 *id)
{
   FILE *fp = NULL;
   char buf[BUFLEN_64];
   
   cmsLog_debug("enter", id);

   if ((fp = fopen(RILAPN_PREF_APN_FILE, "r")) == NULL)
   {
      cmsLog_error("could not open file %s", RILAPN_PREF_APN_FILE);
      return CMSRET_INTERNAL_ERROR;
   }

   /*skip "<?xml version='1.0' encoding='utf-8' standalone='yes' ?>" */
   fgets(buf, sizeof(buf), fp);   
   /*skip "<map>" */
   fgets(buf, sizeof(buf), fp);   
   /*get "    <long name="apn_id" value="xxx" />" */  
   fgets(buf, sizeof(buf), fp);   
   fclose(fp);

   if (sscanf(buf, "    <long name=\"apn_id\" value=\"%d\" />\n", id) == -1)
   {
      cmsLog_error( "%s read error", RILAPN_PREF_APN_FILE);
      return CMSRET_INTERNAL_ERROR;
   }
   
   cmsLog_debug("id = %d", *id);

   return CMSRET_SUCCESS;
}


UBOOL8 rutCellular_getIfnameByApnInstId(const UINT8 apnInstanceID, char *ifname)
{
   WanIpConnObject *ipConn=NULL;
   WanCellularLinkCfgObject *cellularLinkCfgObj = NULL;
   InstanceIdStack ipconnIidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack linkIidStack = EMPTY_INSTANCE_ID_STACK;
   char apnInstanceIDStr[BUFLEN_8] = {0};
   UBOOL8 found = FALSE;   
   CmsRet ret;
 
   sprintf(apnInstanceIDStr, "%d", apnInstanceID); 
    
   while (!found && (ret = cmsObj_getNext(MDMOID_WAN_CELLULAR_LINK_CFG, &linkIidStack, (void **)&cellularLinkCfgObj)) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strstr(cellularLinkCfgObj->cellularAPN, apnInstanceIDStr))
      {
         if ((ret = cmsObj_getNextInSubTreeFlags(MDMOID_WAN_IP_CONN, &linkIidStack, &ipconnIidStack, OGF_NO_VALUE_UPDATE, (void **)&ipConn)) == CMSRET_SUCCESS)
         {
            cmsUtl_strcpy(ifname, ipConn->X_BROADCOM_COM_IfName);
            cmsObj_free((void **) &ipConn);
         }

         found = TRUE;		 
      }
	  
      cmsObj_free((void **) &cellularLinkCfgObj);
   }
   
   cmsLog_debug("ifname = %s", ifname);

   return found;
}

#endif  /* DMP_CELLULARINTERFACEBASE_1 */

#endif /* DMP_DEVICE2_BASELINE_1 */


