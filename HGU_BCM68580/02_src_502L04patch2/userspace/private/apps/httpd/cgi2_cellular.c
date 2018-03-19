/***********************************************************************
 *
 *  Copyright (c) 2010  Broadcom Corporation
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

#ifdef SUPPORT_CELLULAR
#ifdef DMP_CELLULARINTERFACEBASE_1

#include "cms.h"
#include "cms_obj.h"
#include "cms_dal.h"
#include "cms_msg_cellular.h"
#include "httpd.h"
#include "cgi_cmd.h"
#include "cgi_main.h"
#include "rut2_cellular.h"
#include "ril.h"


extern WEB_NTWK_VAR glbWebVar; // this global var is defined in cgimain.c

static CmsRet cellularApnAdd(Dev2CellularApnInfo *cellularApnInfo)
{
   Dev2CellularAccessPointObject *cellularApnObj = NULL;
   InstanceIdStack iidStack;
   char ifname[BUFLEN_8];
   CmsRet ret;

   cmsLog_debug("Enable=%d, Name=%s, Type=%s, Mcc=%s, Mnc=%s, enblIpVer=%d, Carrier=%s, \
               Mmsproxy=%s, Mmsport=%s, Mmsc=%s, Authtype=%d, Username=%s, \
               Password=%s, Proxy=%s, ProxyPort=%s, id=%d", 
               cellularApnInfo->enable, cellularApnInfo->APN, cellularApnInfo->type,
               cellularApnInfo->MCC, cellularApnInfo->MNC, cellularApnInfo->enblIpVer, cellularApnInfo->carrier,
               cellularApnInfo->mmsproxy, cellularApnInfo->mmsport, cellularApnInfo->mmsc,
               cellularApnInfo->authtype, cellularApnInfo->username, cellularApnInfo->password,
               cellularApnInfo->proxy, cellularApnInfo->proxyPort, cellularApnInfo->id);
	  
   /* add a new cellular APN instance */
   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_addInstance(MDMOID_DEV2_CELLULAR_ACCESS_POINT, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_addInstance returns error, ret=%d", ret);
      return CMSRET_INTERNAL_ERROR;
   }

#ifdef DMP_X_BROADCOM_COM_CELLULARWAN_1
   if (cellularApnInfo->enable)
   {
      /*Create WAN Service automatically*/
      /*WAN Service Object should be created before Cellular APN obj as it need an interface name*/
      if ((ret = dalCellular_addCellularWanService(cellularApnInfo->APN, cellularApnInfo->enblIpVer, 
           iidStack.instance[0], ifname)) != CMSRET_SUCCESS)
      {
         cmsLog_error("dalCellular_addCellularWanService returns error. ret=%d", ret);
         return ret;	  
      }
   }   
#endif


   /* get the object, it will be initially filled in with default values */
   if ((ret = cmsObj_get(MDMOID_DEV2_CELLULAR_ACCESS_POINT, &iidStack, 0, (void **) &cellularApnObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get newly created object, ret=%d", ret);
      return CMSRET_INTERNAL_ERROR;
   }

   cellularApnObj->X_BROADCOM_COM_Id = cellularApnInfo->id;

   cellularApnObj->enable = cellularApnInfo->enable;
   
   CMSMEM_REPLACE_STRING(cellularApnObj->APN, cellularApnInfo->APN);
      
   /*Sarah: hard code interface here as we support only 1 cellular interface */   
   CMSMEM_REPLACE_STRING(cellularApnObj->interface, cmsMdm_isDataModelDevice2() ? 
	  	                 "Device.Cellular.Interface.1" : "InternetGatewayDevice.Device.Cellular.Interface.1");
   
   CMSMEM_REPLACE_STRING(cellularApnObj->X_BROADCOM_COM_Type, cellularApnInfo->type);
   
   CMSMEM_REPLACE_STRING(cellularApnObj->X_BROADCOM_COM_MCC, cellularApnInfo->MCC);
   
   CMSMEM_REPLACE_STRING(cellularApnObj->X_BROADCOM_COM_MNC, cellularApnInfo->MNC);
   
   cellularApnObj->X_BROADCOM_COM_IpVer = cellularApnInfo->enblIpVer;
   
   CMSMEM_REPLACE_STRING(cellularApnObj->X_BROADCOM_COM_Carrier, cellularApnInfo->carrier);
   
   CMSMEM_REPLACE_STRING(cellularApnObj->X_BROADCOM_COM_Mmsproxy, cellularApnInfo->mmsproxy);
   
   cellularApnObj->X_BROADCOM_COM_Mmsport = cellularApnInfo->mmsport;
   
   CMSMEM_REPLACE_STRING(cellularApnObj->X_BROADCOM_COM_Mmsc, cellularApnInfo->mmsc);
   
   cellularApnObj->X_BROADCOM_COM_Authtype = cellularApnInfo->authtype;
   
   CMSMEM_REPLACE_STRING(cellularApnObj->username, cellularApnInfo->username);
   
   CMSMEM_REPLACE_STRING(cellularApnObj->password, cellularApnInfo->password);
   
   CMSMEM_REPLACE_STRING(cellularApnObj->proxy, cellularApnInfo->proxy);
   
   cellularApnObj->proxyPort = cellularApnInfo->proxyPort;
	  
   /* set the Cellular APN Object instance */
   if ((ret = cmsObj_set(cellularApnObj, &iidStack)) != CMSRET_SUCCESS)
   {
      CmsRet r2;
      cmsLog_error("cmsObj_set returns error, ret = %d", ret);

      /* since set failed, we have to delete the instance that we just added */
      if ((r2 = cmsObj_deleteInstance(MDMOID_DEV2_CELLULAR_ACCESS_POINT, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsObj_deleteInstance returns error, r2=%d", r2);
      }
   }

    cmsObj_free((void **)&cellularApnObj);

    return CMSRET_SUCCESS;
}  /* End of cgiCellularApnAdd() */

static void cgiCellularApnSelectViewBody(FILE *fs)
{
   Dev2CellularApnInfo cellularApnInfo;
   UINT32 count = 0, i;
   char buf[sizeof(CmsMsgHeader) + BUFLEN_8]={0};
   CmsMsgHeader *replyBuf = (CmsMsgHeader *) buf;
   CmsMsgHeader msg = EMPTY_MSG_HEADER;
   char mcc[BUFLEN_4];
   char mnc[BUFLEN_4];
   CmsRet ret;
   
   cmsLog_debug("Enter");

   msg.type = CMS_MSG_CELLULARAPP_GET_REQUEST;
   msg.src = cmsMsg_getHandleEid(msgHandle);
   msg.flags_request = 1;
   msg.dataLength = 0;
   msg.dst = EID_CELLULAR_APP;
   msg.wordData = DEVCELL_INT_USIMMCCMNC;

   if ((ret = cmsMsg_sendAndGetReplyBufWithTimeout(msgHandle, &msg, &replyBuf, MSECS_IN_SEC)) != CMSRET_SUCCESS )
   {
      cmsLog_error("send cms msg Get DEVCELL_INT_USIMMCCMNC error, ret=%d", ret);
      return;
   }

   cmsLog_debug("sim mcc_mnc =%s", (char *)(replyBuf + 1));
   
   cmsUtl_strncpy(mcc, (char *)(replyBuf + 1), sizeof(mcc));
   cmsUtl_strncpy(mnc, (char *)(replyBuf + 1) + 3, sizeof(mnc));

   memset(&cellularApnInfo, 0, sizeof(cellularApnInfo));

   if ((ret = rutCellularDB_getApnCount(mcc, mnc, &count)) != CMSRET_SUCCESS)
   {
      cmsLog_error("rutCellularDB_getApnCount returns error:%d", ret);
      return;
   }

   for (i = 0; i < count; i++)
   {
      if ((ret = rutCellularDB_getApnByOffset(mcc, mnc, i, &cellularApnInfo)) != CMSRET_SUCCESS)
      {
         cmsLog_error("rutCellularDB_getApnByOffset returns error:%d", ret);
         return;
      }

      /* skip user added APN */	  
      if (cellularApnInfo.id >= CELLULAR_APN_USER_BASE_ID)
      {
         break;
      }

      fprintf(fs, "    <tr align='center'>\n");
      /* Not allow empty APN name */	  
      fprintf(fs, "      <td>%s</td>\n", cellularApnInfo.APN);
      fprintf(fs, "      <td>%s</td>\n", IS_EMPTY_STRING(cellularApnInfo.type) ? "&nbsp" : cellularApnInfo.type);
      fprintf(fs, "      <td>%s</td>\n", cellularApnInfo.MCC);
      fprintf(fs, "      <td>%s</td>\n", cellularApnInfo.MNC);
      fprintf(fs, "      <td>%s</td>\n", IS_EMPTY_STRING(cellularApnInfo.carrier) ? "&nbsp" : cellularApnInfo.carrier);
      fprintf(fs, "      <td>%s</td>\n", IS_EMPTY_STRING(cellularApnInfo.mmsproxy) ? "&nbsp" : cellularApnInfo.mmsproxy);
      fprintf(fs, "      <td>%d</td>\n", cellularApnInfo.mmsport);
      fprintf(fs, "      <td>%s</td>\n", IS_EMPTY_STRING(cellularApnInfo.mmsc) ? "&nbsp" : cellularApnInfo.mmsc);
      fprintf(fs, "      <td>%s</td>\n", cellularApnInfo.authtype == -1 ? "N/A" :
                                         cellularApnInfo.authtype == 0 ? "Disable" :	  
                                         cellularApnInfo.authtype == 1 ? "PAP" :	  
                                         cellularApnInfo.authtype == 2 ? "CHAP" : "PAP/CHAP");	  
      fprintf(fs, "      <td>%s</td>\n", IS_EMPTY_STRING(cellularApnInfo.username) ? "&nbsp" : cellularApnInfo.username);
      fprintf(fs, "      <td>%s</td>\n", IS_EMPTY_STRING(cellularApnInfo.password) ? "&nbsp" : cellularApnInfo.password);
      fprintf(fs, "      <td>%s</td>\n", IS_EMPTY_STRING(cellularApnInfo.proxy) ? "&nbsp" : cellularApnInfo.proxy);
      fprintf(fs, "      <td>%d</td>\n", cellularApnInfo.proxyPort);
      fprintf(fs, "      <td align='center'><input type='checkbox' name='sel' value='%d'></td>\n", cellularApnInfo.id);
      fprintf(fs, "  </tr>\n");
   }
}

static void cgiCellularApnViewBody(FILE *fs)
{
   InstanceIdStack iidStack;
   Dev2CellularAccessPointObject *cellularApnObj = NULL;
   CmsRet ret;

   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_DEV2_CELLULAR_ACCESS_POINT, &iidStack, (void **)&cellularApnObj)) == CMSRET_SUCCESS)
   {

      fprintf(fs, "    <tr align='center'>\n");

      if (!cmsUtl_strcmp(cellularApnObj->APN, CELLULAR_AUTO_SEL_APN_NAME))	  
      {
         Dev2CellularApnInfo cellularApnInfo;
		 
         memset(&cellularApnInfo, 0, sizeof(cellularApnInfo));
         cellularApnInfo.enable = TRUE;
		 if ((ret = rutCellular_getPrefdApn(&cellularApnInfo.id)) != CMSRET_SUCCESS)
         {
            cmsLog_notice("rutCellular_getPrefdApn returns error:%d", ret);
         }
         else if ((ret = rutCellularDB_getApnById(&cellularApnInfo)) != CMSRET_SUCCESS)
         {
            cmsLog_notice("rutCellularDB_getApnById %d returns error:%d", cellularApnInfo.id, ret);
         }

         fprintf(fs, "      <td>%s(AUTO)</td>\n", cellularApnInfo.APN);
         fprintf(fs, "      <td>%s</td>\n", IS_EMPTY_STRING(cellularApnInfo.type) ? "&nbsp" : cellularApnInfo.type);
         fprintf(fs, "      <td>%s</td>\n", IS_EMPTY_STRING(cellularApnInfo.MCC) ? "&nbsp" : cellularApnInfo.MCC);
         fprintf(fs, "      <td>%s</td>\n", IS_EMPTY_STRING(cellularApnInfo.MNC) ? "&nbsp" : cellularApnInfo.MNC);
         fprintf(fs, "      <td>%s</td>\n", cellularApnInfo.enblIpVer == ENBL_IPV4_ONLY ? "IPV4" :
                                            cellularApnInfo.enblIpVer == ENBL_IPV6_ONLY ? "IPV6" : "IPv4|IPv6");
         fprintf(fs, "      <td>%s</td>\n", IS_EMPTY_STRING(cellularApnInfo.carrier) ? "&nbsp" : cellularApnInfo.carrier);
         fprintf(fs, "      <td>%s</td>\n", IS_EMPTY_STRING(cellularApnInfo.mmsproxy) ? "&nbsp" : cellularApnInfo.mmsproxy);
         fprintf(fs, "      <td>%d</td>\n", cellularApnInfo.mmsport);
         fprintf(fs, "      <td>%s</td>\n", IS_EMPTY_STRING(cellularApnInfo.mmsc) ? "&nbsp" : cellularApnInfo.mmsc);
         fprintf(fs, "      <td>%s</td>\n", cellularApnInfo.authtype == -1 ? "N/A" :
                                            cellularApnInfo.authtype == 0 ? "Disable" :	  
                                            cellularApnInfo.authtype == 1 ? "PAP" :	  
                                            cellularApnInfo.authtype == 2 ? "CHAP" : "PAP/CHAP");	  
         fprintf(fs, "      <td>%s</td>\n", IS_EMPTY_STRING(cellularApnInfo.username) ? "&nbsp" : cellularApnInfo.username);
         fprintf(fs, "      <td>%s</td>\n", IS_EMPTY_STRING(cellularApnInfo.password) ? "&nbsp" : cellularApnInfo.password);
         fprintf(fs, "      <td>%s</td>\n", IS_EMPTY_STRING(cellularApnInfo.proxy) ? "&nbsp" : cellularApnInfo.proxy);
         fprintf(fs, "      <td>%d</td>\n", cellularApnInfo.proxyPort);		 
      }
	  else
      {
         /* Not allow empty APN name */	  
         fprintf(fs, "      <td>%s</td>\n", cellularApnObj->APN);
         fprintf(fs, "      <td>%s</td>\n", IS_EMPTY_STRING(cellularApnObj->X_BROADCOM_COM_Type) ? "&nbsp" : cellularApnObj->X_BROADCOM_COM_Type);
         fprintf(fs, "      <td>%s</td>\n", cellularApnObj->X_BROADCOM_COM_MCC);
         fprintf(fs, "      <td>%s</td>\n", cellularApnObj->X_BROADCOM_COM_MNC);
         fprintf(fs, "      <td>%s</td>\n", cellularApnObj->X_BROADCOM_COM_IpVer == ENBL_IPV4_ONLY ? "IPV4" :
                                            cellularApnObj->X_BROADCOM_COM_IpVer == ENBL_IPV6_ONLY ? "IPV6" : "IPv4|IPv6");
         fprintf(fs, "      <td>%s</td>\n", IS_EMPTY_STRING(cellularApnObj->X_BROADCOM_COM_Carrier) ? "&nbsp" : cellularApnObj->X_BROADCOM_COM_Carrier);
         fprintf(fs, "      <td>%s</td>\n", IS_EMPTY_STRING(cellularApnObj->X_BROADCOM_COM_Mmsproxy) ? "&nbsp" : cellularApnObj->X_BROADCOM_COM_Mmsproxy);
         fprintf(fs, "      <td>%d</td>\n", cellularApnObj->X_BROADCOM_COM_Mmsport);
         fprintf(fs, "      <td>%s</td>\n", IS_EMPTY_STRING(cellularApnObj->X_BROADCOM_COM_Mmsc) ? "&nbsp" : cellularApnObj->X_BROADCOM_COM_Mmsc);
         fprintf(fs, "      <td>%s</td>\n", cellularApnObj->X_BROADCOM_COM_Authtype ==  -1 ? "N/A" :
                                            cellularApnObj->X_BROADCOM_COM_Authtype == 0 ? "Disable" :	  
                                            cellularApnObj->X_BROADCOM_COM_Authtype == 1 ? "PAP" :	  
                                            cellularApnObj->X_BROADCOM_COM_Authtype == 2 ? "CHAP" : "PAP/CHAP");	  
         fprintf(fs, "      <td>%s</td>\n", IS_EMPTY_STRING(cellularApnObj->username) ? "&nbsp" : cellularApnObj->username);
         fprintf(fs, "      <td>%s</td>\n", IS_EMPTY_STRING(cellularApnObj->password) ? "&nbsp" : cellularApnObj->password);
         fprintf(fs, "      <td>%s</td>\n", IS_EMPTY_STRING(cellularApnObj->proxy) ? "&nbsp" : cellularApnObj->proxy);
         fprintf(fs, "      <td>%d</td>\n", cellularApnObj->proxyPort);
      }
	  
      fprintf(fs, "      <td align='center'><input type='checkbox' name='enbl' %s></td>\n", 
                                         cellularApnObj->enable? "checked" : "");
      fprintf(fs, "      <td align='center'><input type='checkbox' name='rml'></td>\n");
      fprintf(fs, "  </tr>\n");
	  
      cmsObj_free((void **)&cellularApnObj);
   }
}

static void cgiCellularSmsViewBody(FILE *fs)
{
#if 0
   fprintf(fs, "  <tr align='center'>\n");
   fprintf(fs, "      <td>%s</td>\n", "2015-1-1 01:01");
   fprintf(fs, "      <td>%s</td>\n", "13712341234");
   fprintf(fs, "      <td>%s</td>\n", "test message 1");
   fprintf(fs, "      <td align='center'><input type='checkbox' name='rml'></td>\n");
   fprintf(fs, "  </tr>\n");

   fprintf(fs, "  <tr align='center'>\n");
   fprintf(fs, "      <td>%s</td>\n", "2015-2-1 23:59");
   fprintf(fs, "      <td>%s</td>\n", "12345678");
   fprintf(fs, "      <td>%s</td>\n", "test message 2");
   fprintf(fs, "      <td align='center'><input type='checkbox' name='rml'></td>\n");
   fprintf(fs, "  </tr>\n");

   fprintf(fs, "  <tr align='center'>\n");
   fprintf(fs, "      <td>%s</td>\n", "2015-3-1 12:30");
   fprintf(fs, "      <td>%s</td>\n", "8612345678");
   fprintf(fs, "      <td>%s</td>\n", "test message 3");
   fprintf(fs, "      <td align='center'><input type='checkbox' name='rml'></td>\n");
   fprintf(fs, "  </tr>\n");
#endif   
}


static void writeCellularApnSelectViewScript(FILE *fs)
{
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "function cancelClick() {\n");
   fprintf(fs, "   var loc = 'cellularapn.cmd?action=viewcfg';\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function selClick(sel) {\n");
   fprintf(fs, "   var lst = '';\n");
   fprintf(fs, "   if (sel.length > 0)\n");
   fprintf(fs, "      for (i = 0; i < sel.length; i++) {\n");
   fprintf(fs, "         if ( sel[i].checked == true )\n");
   fprintf(fs, "            lst += sel[i].value + ',';\n");
   fprintf(fs, "         else\n");
   fprintf(fs, "            lst += '0,';\n");
   fprintf(fs, "      }\n");
   fprintf(fs, "   else if ( sel.checked == true )\n");
   fprintf(fs, "      lst = sel[i].value + ',';\n");
   fprintf(fs, "   else\n");
   fprintf(fs, "      lst = '0';\n");
   fprintf(fs, "   var loc = 'cellularapn.cmd?action=select&selLst=' + lst;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");

}  /* End of writeQosClsScript() */


static void writeCellularApnViewScript(FILE *fs)
{
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "function selClick() {\n");
   fprintf(fs, "   var loc = 'cellularapn.cmd?action=selectView';\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function addClick() {\n");
   fprintf(fs, "   var loc = 'cellularapnadd.html';\n");
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function enableClick(ebl) {\n");
   fprintf(fs, "   var lst = '';\n");
   fprintf(fs, "   if (ebl.length > 0)\n");
   fprintf(fs, "      for (i = 0; i < ebl.length; i++) {\n");
   fprintf(fs, "         if ( ebl[i].checked == true )\n");
   fprintf(fs, "            lst += '1,';\n");
   fprintf(fs, "         else\n");
   fprintf(fs, "            lst += '0,';\n");
   fprintf(fs, "      }\n");
   fprintf(fs, "   else if ( ebl.checked == true )\n");
   fprintf(fs, "      lst = '1';\n");
   fprintf(fs, "   else\n");
   fprintf(fs, "      lst = '0';\n");
   fprintf(fs, "   var loc = 'cellularapn.cmd?action=enable&eblLst=' + lst;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function removeClick(rml) {\n");
   fprintf(fs, "   var lst = '';\n");
   fprintf(fs, "   if (rml.length > 0)\n");
   fprintf(fs, "      for (i = 0; i < rml.length; i++) {\n");
   fprintf(fs, "         if ( rml[i].checked == true )\n");
   fprintf(fs, "            lst += '1,';\n");
   fprintf(fs, "         else\n");
   fprintf(fs, "            lst += '0,';\n");
   fprintf(fs, "      }\n");
   fprintf(fs, "   else if ( rml.checked == true )\n");
   fprintf(fs, "      lst = '1';\n");
   fprintf(fs, "   else\n");
   fprintf(fs, "      lst = '0';\n");
   fprintf(fs, "   var loc = 'cellularapn.cmd?action=remove&rmLst=' + lst;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");

}  /* End of writeQosClsScript() */


static void writeCellularSmsViewScript(FILE *fs)
{
   fprintf(fs, "<script language='javascript' src='util.js'></script>\n");
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "function removeClick(rml) {\n");
   fprintf(fs, "   var lst = '';\n");
   fprintf(fs, "   if (rml.length > 0)\n");
   fprintf(fs, "      for (i = 0; i < rml.length; i++) {\n");
   fprintf(fs, "         if ( rml[i].checked == true )\n");
   fprintf(fs, "            lst += '1,';\n");
   fprintf(fs, "         else\n");
   fprintf(fs, "            lst += '0,';\n");
   fprintf(fs, "      }\n");
   fprintf(fs, "   else if ( rml.checked == true )\n");
   fprintf(fs, "      lst = '1';\n");
   fprintf(fs, "   else\n");
   fprintf(fs, "      lst = '0';\n");
   fprintf(fs, "   var loc = 'cellularapn.cmd?action=remove&rmLst=' + lst;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function applyClick(rml) {\n");
   fprintf(fs, "   var loc;\n");
   fprintf(fs, "   var msg;\n");
   fprintf(fs, "   with ( document.forms[0] ) {\n");
   fprintf(fs, "      if (isValidDecNum(cellularSMSC.value) == false){\n");
   fprintf(fs, "         msg = 'SMSC ' + cellularSMSC.value + ' is invalid number';\n");
   fprintf(fs, "         alert(msg);\n");
   fprintf(fs, "         return;\n");
   fprintf(fs, "      }\n");
   fprintf(fs, "      loc = 'cellularapn.cmd?action=apply&cellularSMSC=' + smsc.value;\n\n");
   fprintf(fs, "   }\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");

}  /* End of writeCellularSmsViewScript() */


static void cgiCellularApnSelectView( FILE *fs )
{
   // write header
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
                                                                                
   // write body
   fprintf(fs, "</head>\n<body>\n<blockquote>\n");
                                                                                
   fprintf(fs, "<form>\n");                                                                             
   fprintf(fs, "<b>Cellular APN Setup</b><br><br>\n");
   fprintf(fs, "To select an APN, check select-checkbox, then click the <b>Select</b> button.<br>\n");
   fprintf(fs, "<br>\n");
      
   fprintf(fs, "<table border='1' cellpadding='6' cellspacing='0'>\n");
   // write table header
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd'>Name</td>\n");
   fprintf(fs, "      <td class='hd'>Type</td>\n");
   fprintf(fs, "      <td class='hd'>MCC</td>\n");
   fprintf(fs, "      <td class='hd'>MNC</td>\n");
   fprintf(fs, "      <td class='hd'>Carrier</td>\n");
   fprintf(fs, "      <td class='hd'>Mmsproxy</td>\n");
   fprintf(fs, "      <td class='hd'>Mmsport</td>\n");
   fprintf(fs, "      <td class='hd'>Mmsc</td>\n");
   fprintf(fs, "      <td class='hd'>Authtype</td>\n");   
   fprintf(fs, "      <td class='hd'>Username</td>\n");
   fprintf(fs, "      <td class='hd'>Password</td>\n");
   fprintf(fs, "      <td class='hd'>Proxy</td>\n");
   fprintf(fs, "      <td class='hd'>ProxyPort</td>\n");
   fprintf(fs, "      <td class='hd'>Select</td>\n");
   
   fprintf(fs, "   </tr>\n");

   /*
    * Traverse the MDM to dump out each APN entry.  This code is data model
    * dependent, so has been moved to a separate function.
    */
   cgiCellularApnSelectViewBody(fs);


   fprintf(fs,"</table><br><br>\n");
   fprintf(fs, "<input type='button' onClick='cancelClick()' value='Cancel'>\n");
   fprintf(fs, "<input type='button' onClick='selClick(this.form.sel)' value='Select'>\n");
   fprintf(fs, "</form>\n");

   // write Java Script
   writeCellularApnSelectViewScript(fs);

   fprintf(fs, "</blockquote>\n");
   fprintf(fs, "</body>\n");
   fprintf(fs, "</html>\n");
   fflush(fs);

}  /* End of cgiCellularApnView() */


static void cgiCellularApnView( FILE *fs )
{
   // write header
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
                                                                                
   // write body
   fprintf(fs, "</head>\n<body>\n<blockquote>\n");
                                                                                
   fprintf(fs, "<form>\n");                                                                             
   fprintf(fs, "<b>Cellular APN Setup</b><br><br>\n");
   fprintf(fs, "To add an APN, click the <b>Add</b> button.<br>\n");
   fprintf(fs, "To remove APNs, check their remove-checkboxes, then click the <b>Remove</b> button.<br>\n");
   fprintf(fs, "To enable an APN, check enable-checkbox, then click the <b>Enable</b> button.<br>\n");
   fprintf(fs, "The enable-checkbox also shows status of the APN after page reload.<br>\n");
   fprintf(fs, "<br>\n");
      
   fprintf(fs, "<table border='1' cellpadding='6' cellspacing='0'>\n");
   // write table header
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd'>Name</td>\n");
   fprintf(fs, "      <td class='hd'>Type</td>\n");
   fprintf(fs, "      <td class='hd'>MCC</td>\n");
   fprintf(fs, "      <td class='hd'>MNC</td>\n");
   fprintf(fs, "      <td class='hd'>IpProtocal</td>\n");
   fprintf(fs, "      <td class='hd'>Carrier</td>\n");
   fprintf(fs, "      <td class='hd'>Mmsproxy</td>\n");
   fprintf(fs, "      <td class='hd'>Mmsport</td>\n");
   fprintf(fs, "      <td class='hd'>Mmsc</td>\n");
   fprintf(fs, "      <td class='hd'>Authtype</td>\n");   
   fprintf(fs, "      <td class='hd'>Username</td>\n");
   fprintf(fs, "      <td class='hd'>Password</td>\n");
   fprintf(fs, "      <td class='hd'>Proxy</td>\n");
   fprintf(fs, "      <td class='hd'>ProxyPort</td>\n");
   fprintf(fs, "      <td class='hd'>Enable</td>\n");
   fprintf(fs, "      <td class='hd'>Remove</td>\n");
   
   fprintf(fs, "   </tr>\n");

   /*
    * Traverse the MDM to dump out each APN entry.  This code is data model
    * dependent, so has been moved to a separate function.
    */
   cgiCellularApnViewBody(fs);


   fprintf(fs,"</table><br><br>\n");
   fprintf(fs, "<input type='button' onClick='selClick()' value='Select from Database'>\n");
   fprintf(fs, "<input type='button' onClick='addClick()' value='Add New'>\n");
   fprintf(fs, "<input type='button' onClick='enableClick(this.form.enbl)' value='Enable'>\n");
   fprintf(fs, "<input type='button' onClick='removeClick(this.form.rml)' value='Remove'>\n");
   fprintf(fs, "</form>\n");

   // write Java Script
   writeCellularApnViewScript(fs);

   fprintf(fs, "</blockquote>\n");
   fprintf(fs, "</body>\n");
   fprintf(fs, "</html>\n");
   fflush(fs);

}  /* End of cgiCellularApnView() */


static void cgiCellularApnAdd( char *query, FILE *fs )
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2CellularAccessPointObject *cellularApnObj = NULL;
   Dev2CellularApnInfo cellularApnInfo;
   char buf[BUFLEN_64] = "\0";   
   CmsRet ret;

   cgiGetValueByName(query, "cellularApnType", cellularApnInfo.type);
   
   if (cmsUtl_strstr(cellularApnInfo.type, "default") &&
      (cmsObj_getNext(MDMOID_DEV2_CELLULAR_ACCESS_POINT, &iidStack, (void **)&cellularApnObj) == CMSRET_SUCCESS) &&
      (!cmsUtl_strcmp(cellularApnObj->APN, CELLULAR_AUTO_SEL_APN_NAME)))
   {
      cmsLog_error("exist Auto default APN");
      sprintf(buf, "Need to remove Auto default APN before add a manual default APN");
      cgiWriteMessagePage(fs, "Cellular APN Config Error", buf, "cellularapn.cmd?action=viewcfg");
      cmsObj_free((void **)&cellularApnObj);
      return;	  
   }

   cgiGetValueByName(query, "cellularApnEnable", buf);
   cellularApnInfo.enable = atoi(buf);

   cgiGetValueByName(query, "cellularApnName", cellularApnInfo.APN);
   
   cgiGetValueByName(query, "cellularApnMcc", cellularApnInfo.MCC);
   
   cgiGetValueByName(query, "cellularApnMnc", cellularApnInfo.MNC);
   
   cgiGetValueByName(query, "cellularEnblIpVer", buf);
   cellularApnInfo.enblIpVer = atoi(buf);
   
   cgiGetValueByName(query, "cellularApnCarrier", cellularApnInfo.carrier);
   
   cgiGetValueByName(query, "cellularApnMmsproxy", cellularApnInfo.mmsproxy);
   
   cgiGetValueByName(query, "cellularApnMmsport", buf);
   cellularApnInfo.mmsport= atoi(buf);
   
   cgiGetValueByName(query, "cellularApnMmsc", cellularApnInfo.mmsc);
   
   cgiGetValueByName(query, "cellularApnAuthtype", buf);
   cellularApnInfo.authtype = atoi(buf);
   
   cgiGetValueByName(query, "cellularApnUsername", cellularApnInfo.username);
   
   cgiGetValueByName(query, "cellularApnPassword", cellularApnInfo.password);
   
   cgiGetValueByName(query, "cellularApnProxy", cellularApnInfo.proxy);
   
   cgiGetValueByName(query, "cellularApnProxyPort", buf);
   cellularApnInfo.proxyPort= atoi(buf);

   cellularApnInfo.id = 0; //auto generate an Id

   if ((ret = cellularApnAdd(&cellularApnInfo)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cellularApnAdd returns error. ret=%d", ret);
      sprintf(buf, "Add Cellular APN failed. ret=%d", ret);
      cgiWriteMessagePage(fs, "Cellular APN Config Error", buf, "cellularapn.cmd?action=viewcfg");
      return;	  
   }
	
    glbSaveConfigNeeded = TRUE;
    cgiCellularApnView(fs);
}  /* End of cgiCellularApnAdd() */



static void cgiCellularApnSelect(char *query, FILE *fs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2CellularAccessPointObject *cellularApnObj = NULL;
   char lst[WEB_BUF_SIZE_MAX];
   char *last = NULL;
   char *check;
   char buf[BUFLEN_64];
   CmsRet ret;
   UINT32 id;
   UBOOL8 hasDefault = FALSE;   
   Dev2CellularApnInfo cellularApnInfo;

   cgiGetValueByName(query, "cellularApnType", cellularApnInfo.type);
   
   if ((cmsObj_getNext(MDMOID_DEV2_CELLULAR_ACCESS_POINT, &iidStack, (void **)&cellularApnObj) == CMSRET_SUCCESS) &&
      (!cmsUtl_strcmp(cellularApnObj->APN, CELLULAR_AUTO_SEL_APN_NAME)))
   {
      hasDefault = TRUE;	  
   }

   lst[0] = '\0';
   if (cgiGetValueByName(query, "selLst", lst) != CGI_STS_OK)
   {
      return;
   }

   /* loop through the queue table */
   for (check = strtok_r(lst, ",", &last); check != NULL; check = strtok_r(NULL, ",", &last))
   {   
      id = atoi(check);   
      if ( id == 0)
      {
         continue; /*skip not selected*/
      }

      memset(&cellularApnInfo, 0, sizeof(cellularApnInfo));
      cellularApnInfo.id = id;
      cellularApnInfo.enable = TRUE;
      if ((ret = rutCellularDB_getApnById(&cellularApnInfo)) != CMSRET_SUCCESS)
      {
         cmsLog_error("rutCellularDB_getApnById %d returns error:%d", id, ret);
         continue;
      }

      if (hasDefault && strstr(cellularApnInfo.type, "default"))
      {
         cmsLog_error("Need to remove Auto default APN before add a manual default APN, skip this selection");
         continue;
      }

      if ((ret = cellularApnAdd(&cellularApnInfo)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cellularApnAdd returns error. ret=%d", ret);
         sprintf(buf, "Add Cellular APN failed. ret=%d", ret);
         cgiWriteMessagePage(fs, "Cellular APN Config Error", buf, "cellularapn.cmd?action=viewcfg");
         return;	  
      }
	  
      glbSaveConfigNeeded = TRUE;
   }
}


static void cgiCellularApnEnable(char *query, FILE *fs)
{
   char cmd[WEB_MD_BUF_SIZE_MAX];
   char lst[WEB_BUF_SIZE_MAX];
   char ifname[BUFLEN_8];   
   char *last = NULL;
   char *check;
   InstanceIdStack iidStack;
   Dev2CellularAccessPointObject *cellularApnObj = NULL;
   CmsRet ret;
   UBOOL8 enbl;
   
   lst[0] = '\0';
   if (cgiGetValueByName(query, "eblLst", lst) != CGI_STS_OK)
   {
      return;
   }

   /* loop through the queue table */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_DEV2_CELLULAR_ACCESS_POINT, &iidStack, (void **)&cellularApnObj)) == CMSRET_SUCCESS)
   {
      if (last == NULL)
      {
         check = strtok_r(lst, ",", &last);
      }
      else
      {
         check = strtok_r(NULL, ",", &last);
      }
   
      if (check == NULL)
      {
         cmsObj_free((void **)&cellularApnObj);
         break;   /* done */
      }

      if ((enbl = (atoi(check) == 1)) == cellularApnObj->enable)
      {
         cmsObj_free((void **)&cellularApnObj);
         continue;   /* no change, continue */
      }
	  
#ifdef DMP_X_BROADCOM_COM_CELLULARWAN_1
      if (enbl)
      {
	     if ((ret = dalCellular_addCellularWanService(cellularApnObj->APN, cellularApnObj->X_BROADCOM_COM_IpVer, 
              iidStack.instance[0],ifname)) != CMSRET_SUCCESS)
         {
            cmsLog_error("dalCellular_addCellularWanService returns error. ret=%d", ret);
            sprintf(cmd, "Add Cellular Wan service failed. ret=%d", ret);
            cgiWriteMessagePage(fs, "Cellular APN Config Error", cmd, "cellularapn.cmd?action=viewcfg");
            cmsObj_free((void **)&cellularApnObj);
            break;
         }   
      }   
#endif    

      cellularApnObj->enable = enbl;
      if ((ret = cmsObj_set(cellularApnObj, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsObj_set returns error. ret=%d", ret);
         sprintf(cmd, "set cellularApnObj to %d failed. ret=%d", enbl, ret);
         cgiWriteMessagePage(fs, "cellularApnObj Config Error", cmd, "cellularapn.cmd?action=viewcfg");
         cmsObj_free((void **)&cellularApnObj);
         break;
      }

      cmsObj_free((void **)&cellularApnObj);
	  
      if (!enbl)
      {      
#ifdef DMP_X_BROADCOM_COM_CELLULARWAN_1
         if ((ret = dalCellular_delCellularWanService(iidStack.instance[0], &glbWebVar)) != CMSRET_SUCCESS)      
         {
            cmsLog_error("dalCellular_delCellularWanService returns error, ret = %d", ret);
            sprintf(cmd, "Delete Cellular Wan service failed. ret=%d", ret);
            cgiWriteMessagePage(fs, "Cellular APN Config Error", cmd, "cellularapn.cmd?action=viewcfg");
            cmsObj_free((void **)&cellularApnObj);
            break;
         }
#endif  
      }
 
      glbSaveConfigNeeded = TRUE;
   }
}


static void cgiCellularApnRemove(char *query, FILE *fs)
{
   char cmd[WEB_MD_BUF_SIZE_MAX];
   char lst[WEB_BUF_SIZE_MAX];
   char *last = NULL;
   char *check;
   InstanceIdStack iidStack, iidStackPrev;
   Dev2CellularAccessPointObject *cellularApnObj = NULL;
   CmsRet ret;

   lst[0] = '\0';
   if (cgiGetValueByName(query, "rmLst", lst) != CGI_STS_OK)
   {
      return;
   }

   /* loop through the queue table */
   INIT_INSTANCE_ID_STACK(&iidStack);
   iidStackPrev = iidStack;
   while ((ret = cmsObj_getNext(MDMOID_DEV2_CELLULAR_ACCESS_POINT, &iidStack, (void **)&cellularApnObj)) == CMSRET_SUCCESS)
   {
      if (last == NULL)
      {
         check = strtok_r(lst, ",", &last);
      }
      else
      {
         check = strtok_r(NULL, ",", &last);
      }
      if (check == NULL)
      {
         cmsObj_free((void **)&cellularApnObj);
         break;   /* done */
      }
      if (atoi(check) == 1)
      {
         cmsLog_debug("Deleting instance=%d APN=%s", PEEK_INSTANCE_ID(&iidStack), cellularApnObj->APN);

         if ((ret = cmsObj_deleteInstance(MDMOID_DEV2_CELLULAR_ACCESS_POINT, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("deleteInstance MDMOID_DEV2_CELLULAR_ACCESS_POINT returns error, ret = %d", ret);
            sprintf(cmd, "Delete Cellular APN failed. ret=%d", ret);
            cgiWriteMessagePage(fs, "Cellular APN Delete Error", cmd, "cellularapn.cmd?action=viewcfg");
            cmsObj_free((void **)&cellularApnObj);
            break;
         }

#ifdef DMP_X_BROADCOM_COM_CELLULARWAN_1
         if ((ret = dalCellular_delCellularWanService(iidStack.instance[0], &glbWebVar) != CMSRET_SUCCESS))
         {
            cmsLog_error("dalCellular_delCellularWanService returns error, ret = %d", ret);
            sprintf(cmd, "Delete Cellular APN failed. ret=%d", ret);
            cgiWriteMessagePage(fs, "Cellular APN Delete Error", cmd, "cellularapn.cmd?action=viewcfg");
            cmsObj_free((void **)&cellularApnObj);
            break;
         }
#endif

         glbSaveConfigNeeded = TRUE;

         /* since this instance has been deleted, we want to set the iidStack to
          * the previous instance, so that we can continue to do getNext.
          */
         iidStack = iidStackPrev;
      }
      /* save this iidStack in case we want to do a getNext from this instance */
      iidStackPrev = iidStack;
      
      cmsObj_free((void **)&cellularApnObj);
   }
}


static void cgiCellularSmsView( FILE *fs )
{
   // write header
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
                                                                                
   // write body
   fprintf(fs, "</head>\n<body>\n<blockquote>\n");
                                                                                
   fprintf(fs, "<form>\n");                                                                             
   fprintf(fs, "<b>Cellular SMS Configuration</b><br><br>\n");
   fprintf(fs, "Config desired values, then click the <b>Apply/Save</b> button.<br>\n");
   fprintf(fs, "To remove SMSs, check their remove-checkboxes, then click the <b>Remove</b> button.<br>\n");
   fprintf(fs, "<br><br>\n");

   fprintf(fs, "<table cellSpacing='0' cellPadding='0' border='0'>\n");
   fprintf(fs, "   <tr>\n");
   fprintf(fs, "      <td width='50'>SMSC:</td>\n");
   fprintf(fs, "      <td><input type='text' size='32' name='cellularSMSC' maxlength='32'></td>\n");
   fprintf(fs, "   </tr>\n");
   fprintf(fs, "</table>\n");
   fprintf(fs, "<br><br>\n");
            
   fprintf(fs, "<table border='1' cellpadding='6' cellspacing='0'>\n");
   // write table header
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd'>Date</td>\n");
   fprintf(fs, "      <td class='hd'>From</td>\n");
   fprintf(fs, "      <td class='hd' width='150'>Content</td>\n");
   fprintf(fs, "      <td class='hd'>Remove</td>\n");
   fprintf(fs, "   </tr>\n");

   /*
    * Traverse the MDM to dump out each APN entry.  This code is data model
    * dependent, so has been moved to a separate function.
    */
   cgiCellularSmsViewBody(fs);


   fprintf(fs,"</table><br><br>\n");
   fprintf(fs, "<input type='button' onClick='removeClick(this.form.rml)' value='Remove'>\n");
   fprintf(fs, "<input type='button' onClick='applyClick()' value='Apply/Save'>\n");
   fprintf(fs, "</form>\n");

   // write Java Script
   writeCellularSmsViewScript(fs);

   fprintf(fs, "</blockquote>\n");
   fprintf(fs, "</body>\n");
   fprintf(fs, "</html>\n");
   fflush(fs);

}  /* End of cgiCellularApnView() */

static void cgiCellularSmsRemove(char *query, FILE *fs)
{
   char cmd[WEB_MD_BUF_SIZE_MAX];
   char lst[WEB_BUF_SIZE_MAX];
   char *last = NULL;
   char *check;
   InstanceIdStack iidStack, iidStackPrev;
   Dev2CellularAccessPointObject *cellularApnObj = NULL;
   CmsRet ret;

   lst[0] = '\0';
   if (cgiGetValueByName(query, "rmLst", lst) != CGI_STS_OK)
   {
      return;
   }

   /* loop through the queue table */
   INIT_INSTANCE_ID_STACK(&iidStack);
   iidStackPrev = iidStack;
   while ((ret = cmsObj_getNext(MDMOID_DEV2_CELLULAR_ACCESS_POINT, &iidStack, (void **)&cellularApnObj)) == CMSRET_SUCCESS)
   {
      if (last == NULL)
      {
         check = strtok_r(lst, ",", &last);
      }
      else
      {
         check = strtok_r(NULL, ",", &last);
      }
      if (check == NULL)
      {
         cmsObj_free((void **)&cellularApnObj);
         break;   /* done */
      }
      if (atoi(check) == 1)
      {
         cmsLog_debug("Deleting instance=%d APN=%s", PEEK_INSTANCE_ID(&iidStack), cellularApnObj->APN);

         if ((ret = cmsObj_deleteInstance(MDMOID_DEV2_CELLULAR_ACCESS_POINT, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("deleteInstance MDMOID_DEV2_CELLULAR_ACCESS_POINT returns error, ret = %d", ret);
            sprintf(cmd, "Delete Cellular APN failed. ret=%d", ret);
            cgiWriteMessagePage(fs, "QoS Cellular APN Delete Error", cmd, "cellularapn.cmd?action=viewcfg");
            cmsObj_free((void **)&cellularApnObj);
            break;
         }

#ifdef DMP_X_BROADCOM_COM_CELLULARWAN_1
         if ((ret = dalCellular_delCellularWanService(iidStack.instance[0], &glbWebVar) != CMSRET_SUCCESS))
         {
            cmsLog_error("dalCellular_delCellularWanService returns error, ret = %d", ret);
            sprintf(cmd, "Delete Cellular APN failed. ret=%d", ret);
            cgiWriteMessagePage(fs, "QoS Cellular APN Delete Error", cmd, "cellularapn.cmd?action=viewcfg");
            cmsObj_free((void **)&cellularApnObj);
         }
#endif
         glbSaveConfigNeeded = TRUE;

         /* since this instance has been deleted, we want to set the iidStack to
          * the previous instance, so that we can continue to do getNext.
          */
         iidStack = iidStackPrev;
      }
      /* save this iidStack in case we want to do a getNext from this instance */
      iidStackPrev = iidStack;
      
      cmsObj_free((void **)&cellularApnObj);
   }
}


void cgiGetCellularInfo(int argc __attribute((unused)), char **argv __attribute((unused)), char *varValue)
{
   Dev2CellularInterfaceObject *cellularIntfObj = NULL;
   Dev2CellularInterfaceUsimObject *cellularIntfSimObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   cmsLog_debug("argv[2] = %s", argv[2]);
   
   *varValue = '\0';

   if ( cmsUtl_strcmp(argv[2], "availNtwk") == 0 )
   {
//Sarah: todo   
      sprintf(varValue, "%s", "aaa|11111,bbb|22222,ccc|33333");
      return;	  
   }

   if ( cmsUtl_strcmp(argv[2], "voiceRegState") == 0 )
   {
      char buf[sizeof(CmsMsgHeader) + BUFLEN_512]={0};
      CmsMsgHeader *replyBuf = (CmsMsgHeader *) buf;
      GetRilReqVoiceRegStateBody *regVoiceRegState;
      CmsMsgHeader msg = EMPTY_MSG_HEADER;

      msg.type = CMS_MSG_CELLULARAPP_GET_RIL_REQUEST;
      msg.src = cmsMsg_getHandleEid(msgHandle);
      msg.flags_request = 1;
      msg.dataLength = 0;
      msg.dst = EID_CELLULAR_APP;
      msg.wordData = RIL_REQUEST_VOICE_REGISTRATION_STATE;

      memset(buf, 0, sizeof(buf));	  
      if ((ret = cmsMsg_sendAndGetReplyBufWithTimeout(msgHandle, &msg, &replyBuf, MSECS_IN_SEC)) != CMSRET_SUCCESS )
      {
         cmsLog_error("send cms msg Get RIL_REQUEST_VOICE_REGISTRATION_STATE error, ret=%d", ret);
         return;
      }

      regVoiceRegState = (GetRilReqVoiceRegStateBody *)(replyBuf + 1); 
      cmsLog_debug("regState =%s, regDenyReason = %s", regVoiceRegState->regState, regVoiceRegState->regDenyReason);
      
      sprintf(varValue, "%s", regVoiceRegState->regState);
	  
      if (!IS_EMPTY_STRING(regVoiceRegState->regDenyReason))
      {
         sprintf(varValue, "(%s)", regVoiceRegState->regDenyReason);
      }
	  
      return;	  
   }

   if ( cmsUtl_strcmp(argv[2], "dataRegState") == 0 )
   {
      char buf[sizeof(CmsMsgHeader) + BUFLEN_512]={0};
      CmsMsgHeader *replyBuf = (CmsMsgHeader *) buf;
      GetRilReqDataRegStateBody *regDataRegState;
      CmsMsgHeader msg = EMPTY_MSG_HEADER;

      msg.type = CMS_MSG_CELLULARAPP_GET_RIL_REQUEST;
      msg.src = cmsMsg_getHandleEid(msgHandle);
      msg.flags_request = 1;
      msg.dataLength = 0;
      msg.dst = EID_CELLULAR_APP;
      msg.wordData = RIL_REQUEST_DATA_REGISTRATION_STATE;

      memset(buf, 0, sizeof(buf));	  
      if ((ret = cmsMsg_sendAndGetReplyBufWithTimeout(msgHandle, &msg, &replyBuf, MSECS_IN_SEC)) != CMSRET_SUCCESS )
      {
         cmsLog_error("send cms msg Get RIL_REQUEST_DATA_REGISTRATION_STATE error, ret=%d", ret);
         return;
      }

      regDataRegState = (GetRilReqDataRegStateBody *)(replyBuf + 1); 
      cmsLog_debug("regState =%s, regDenyReason = %s, availDataRadioTech = %s", 
                   regDataRegState->regState, regDataRegState->regDenyReason, regDataRegState->availDataRadioTech);
      
      sprintf(varValue, "%s", regDataRegState->regState);
	  
      if (!IS_EMPTY_STRING(regDataRegState->regDenyReason))
      {
         sprintf(varValue, "(%s)", regDataRegState->regDenyReason);
      }
	  
      return;	  
   }

   if ( cmsUtl_strcmp(argv[2], "imsRegState") == 0 )
   {
      char buf[sizeof(CmsMsgHeader) + BUFLEN_512]={0};
      CmsMsgHeader *replyBuf = (CmsMsgHeader *) buf;
      GetRilReqIMSRegStateBody *regImsRegState;
      CmsMsgHeader msg = EMPTY_MSG_HEADER;

      msg.type = CMS_MSG_CELLULARAPP_GET_RIL_REQUEST;
      msg.src = cmsMsg_getHandleEid(msgHandle);
      msg.flags_request = 1;
      msg.dataLength = 0;
      msg.dst = EID_CELLULAR_APP;
      msg.wordData = RIL_REQUEST_IMS_REGISTRATION_STATE;

      memset(buf, 0, sizeof(buf));	  
      if ((ret = cmsMsg_sendAndGetReplyBufWithTimeout(msgHandle, &msg, &replyBuf, MSECS_IN_SEC)) != CMSRET_SUCCESS )
      {
         cmsLog_error("send cms msg Get RIL_REQUEST_IMS_REGISTRATION_STATE error, ret=%d", ret);
         return;
      }

      regImsRegState = (GetRilReqIMSRegStateBody *)(replyBuf + 1); 
      cmsLog_debug("regState =%s, radioTechnology = %s", 
                   regImsRegState->regState, regImsRegState->radioTechnology);
      
      sprintf(varValue, "%s", regImsRegState->regState);
	  	  
      return;	  
   }
   
   /*Get parameters under Device.Cellular.*/      
   if ( cmsUtl_strcmp(argv[2], "cellular") == 0 )
   {
      Dev2CellularObject *cellularObj = NULL;
	  
      if ((ret = cmsObj_get(MDMOID_DEV2_CELLULAR, &iidStack, 0, (void **)&cellularObj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsObj_get MDMOID_DEV2_CELLULAR returns error. ret=%d", ret);
         return;	  
      }
	  
      sprintf(varValue, "%s", cellularObj->roamingStatus?:"unknown");
	  
      cmsObj_free((void **)&cellularObj);
      return;	  
   }

   
   /*Get parameters under Device.Cellular.Interface.1.*/     
   if ( cmsUtl_strcmp(argv[2], "cellularInterface") == 0 )
   {
      if ((ret = cmsObj_getNext(MDMOID_DEV2_CELLULAR_INTERFACE, &iidStack, (void **)&cellularIntfObj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsObj_get MDMOID_DEV2_CELLULAR_INTERFACE returns error. ret=%d", ret);
         return;	  
      }
      
      sprintf(varValue, "%s|%s|%s|%s|%d|%d|%d", 
                        cellularIntfObj->status?:"unknown",
                        cellularIntfObj->IMEI?:"unknown",
                        cellularIntfObj->currentAccessTechnology?:"unknown",
                        cellularIntfObj->networkInUse?:"unknown",
                        cellularIntfObj->RSSI,
                        cellularIntfObj->upstreamMaxBitRate,
                        cellularIntfObj->downstreamMaxBitRate);
   
      cmsObj_free((void **)&cellularIntfObj);
   }

   /*Get parameters under Device.Cellular.Interface.1.USIM.*/   
   if ( cmsUtl_strcmp(argv[2], "cellularSIM") == 0 )
   {
      INIT_INSTANCE_ID_STACK(&iidStack);
      if ((ret = cmsObj_getNext(MDMOID_DEV2_CELLULAR_INTERFACE_USIM, &iidStack, (void **)&cellularIntfSimObj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsObj_get MDMOID_DEV2_CELLULAR_INTERFACE_USIM returns error. ret=%d", ret);
         return;	  
      }
      
      sprintf(varValue, "%s|%s|%s|%s", 
                        cellularIntfSimObj->status?:"unknown",
                        cellularIntfSimObj->IMSI?:"unknown",
                        cellularIntfSimObj->ICCID?:"unknown",
                        cellularIntfSimObj->MSISDN?:"unknown");
	  	  
      cmsObj_free((void **)&cellularIntfSimObj);
   }
   
}



CmsRet cgiCellularCfg(void)
{
   CmsRet ret;

   cmsLog_notice("setting Cellular config");

   if ((ret = dalCellular_configCellular(glbWebVar.cellularRoamingEnbl)) != CMSRET_SUCCESS)
   {
      cmsLog_error("set of Cellular config failed, ret=%d", ret);
   }

   return ret;
}


CmsRet cgiCellularIntfCfg(void)
{
   CmsRet ret;

   cmsLog_notice("setting Cellular Interface config");

   if ((ret = dalCellular_configCellularInterface(glbWebVar.cellularIntfEnable, glbWebVar.cellularPrefdAccTech, 
                                                  glbWebVar.cellularNtwkReq, glbWebVar.cellularNtwkReq_MCCMNC, 
                                                  glbWebVar.cellularPinChk, glbWebVar.cellularPin)) != CMSRET_SUCCESS)
   {
      cmsLog_error("set of Cellular Interface Config failed, ret=%d", ret);
   }

   return ret;
} 


void cgiCellularApnCfg(char *query, FILE *fs)
{
   char action[BUFLEN_264];

   cgiGetValueByName(query, "action", action);
   if (strcmp(action, "add") == 0)
   {
      cgiCellularApnAdd( query, fs );
   }
   else if (strcmp(action, "enable") == 0)
   {
      cgiCellularApnEnable(query, fs);

      /* refresh the page */
      cgiCellularApnView(fs);
   }
   else if (strcmp(action, "remove") == 0)
   {
      cgiCellularApnRemove(query, fs);

      /* refresh the page */
      cgiCellularApnView(fs);
   }
   else if (strcmp(action, "selectView") == 0)
   {
      cgiCellularApnSelectView(fs);
   } 
   else if (strcmp(action, "select") == 0)
   {
      cgiCellularApnSelect(query, fs);

      /* refresh the page */
      cgiCellularApnView(fs);
   }    
   else
   {
      cgiCellularApnView(fs);
   }
   
}  /* End of cgiCellularApnCfg() */


void cgiCellularSmsCfg(char *query, FILE *fs)
{
   char action[BUFLEN_264];

   cgiGetValueByName(query, "action", action);
   if (strcmp(action, "remove") == 0)
   {
      cgiCellularSmsRemove(query, fs);

      /* refresh the page */
      cgiCellularSmsView(fs);
   }
   else
   {
      cgiCellularSmsView(fs);
   }
   
}  /* End of cgiCellularApnCfg() */

#else /* DMP_CELLULARINTERFACEBASE_1 is not defined */
#error "Cellular objects incompatible with current Data Model mode, go to make menuconfig to fix"
#endif
#endif  /* SUPPORT_CELLULAR */

