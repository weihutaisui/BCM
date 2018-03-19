/**********************************************************************************
** <:copyright-BRCM:2017:proprietary:standard
** 
**    Copyright (c) 2017 Broadcom 
**    All Rights Reserved
** 
**  This program is the proprietary software of Broadcom and/or its
**  licensors, and may only be used, duplicated, modified or distributed pursuant
**  to the terms and conditions of a separate, written license agreement executed
**  between you and Broadcom (an "Authorized License").  Except as set forth in
**  an Authorized License, Broadcom grants no license (express or implied), right
**  to use, or waiver of any kind with respect to the Software, and Broadcom
**  expressly reserves all rights in and to the Software and all intellectual
**  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
**  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
**  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
** 
**  Except as expressly set forth in the Authorized License,
** 
**  1. This program, including its structure, sequence and organization,
**     constitutes the valuable trade secrets of Broadcom, and you shall use
**     all reasonable efforts to protect the confidentiality thereof, and to
**     use this information only in connection with your use of Broadcom
**     integrated circuit products.
** 
**  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
**     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
**     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
**     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
**     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
**     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
**     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
**     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
**     PERFORMANCE OF THE SOFTWARE.
** 
**  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
**     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
**     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
**     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
**     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
**     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
**     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
**     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
**     LIMITED REMEDY.
** :>
***********************************************************************************/
#ifdef BUILD_SNMP_BRIDGE_MIB
/* General includes */
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <fcntl.h>
#include <net/if.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include <linux/if_arp.h>


/* SNMP includes */
#include "asn1.h"
#include "snmp.h"
#include "agtMib.h"
#include "agtEngine.h"


#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"
#include "cms_qos.h"
#include "cms_qdm.h"
#include "qBridge.h"

pDOT1Q_TP_FDB_TABLE_LIST pDot1qTpFdbList;
pDOT1Q_VLAN_CURRENT_TABLE_LIST pDot1qVlanCurrentList;
pDOT1Q_VLAN_STATIC_TABLE_LIST pDot1qVlanStaticList;
pDOT1Q_PORT_VLAN_TABLE_LIST pDot1qPortVlanList;

void add_dot1qTpFdbInfoToList(pDOT1Q_TP_FDB_ENTRY_INFO p)
{
   if (pDot1qTpFdbList->head == NULL) 
   {
      pDot1qTpFdbList->head = p;
      pDot1qTpFdbList->tail = p;
   }
   else 
   {
      pDot1qTpFdbList->tail->next = p;
      pDot1qTpFdbList->tail = p;
   }
   pDot1qTpFdbList->count++;
}

void free_dot1qTpFdbInfo(void) 
{
   pDOT1Q_TP_FDB_ENTRY_INFO ptr;

   while (pDot1qTpFdbList->head) 
   {
      ptr = pDot1qTpFdbList->head;
      pDot1qTpFdbList->head = pDot1qTpFdbList->head->next;
      free(ptr);
   }
   pDot1qTpFdbList->count = 0;
}


//**************************************************************************
// Function Name: bcmMacStrToNum
// Description  : convert MAC address from string to array of 6 bytes.
//                Ex: 0a:0b:0c:0d:0e:0f -> 0a0b0c0d0e0f
// Returns      : status.
//**************************************************************************
int bcmMacStrToNum(char *macAddr, char *str) 
{
   char *pToken = NULL, *pLast = NULL;
   char *buf;
   UINT16 i = 1;
   int len;
   
   if ( macAddr == NULL ) return 0;
   if ( (str == NULL) || (strlen(str) == 0) ) return 0;

   len = strlen(str) + 1;
   if (len > 20)
     len = 20;
   buf = (char*)malloc(len);
   memset(buf,0,len);

   if ( buf == NULL ) return 0;

   /* need to copy since strtok_r updates string */  
   strncpy(buf, str,len-1);

   /* Mac address has the following format
       xx:xx:xx:xx:xx:xx where x is hex number */
   pToken = strtok_r(buf, ":", &pLast);
   if (pToken != NULL) 
   {
      macAddr[0] = (char) strtol(pToken, (char **)NULL, 16);
      for ( i = 1; i < 6; i++ ) 
      {
         pToken = strtok_r(NULL, ":", &pLast);
            if (pToken == NULL)
               break;
         macAddr[i] = (char) strtol(pToken, (char **)NULL, 16);
      }
   }

   free(buf);

   return 1;
}

/*
 > brctl showmacs br0
port no mac addr                is local?       ageing timer
  1     00:00:b4:20:1e:f0       no                44.45
  1     02:10:18:01:00:01       yes                0.00
  2     02:10:18:01:00:02       yes                0.00
  3     02:10:18:01:00:03       yes                0.00
*/
int scan_dot1qTpFdbTable(void)
{
   pDOT1Q_TP_FDB_ENTRY_INFO pEntry;

   char cmd[BUFLEN_1024];
   char brName[BUFLEN_64];
   char line[BUFLEN_264];
   FILE *fd = NULL;

   int  IndexOfBr   = 0;
   int  IndexOfMac = 0;
   L2BridgingEntryObject          *pBridgeObj = NULL;
   InstanceIdStack iidStack    = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;  

   free_dot1qTpFdbInfo();

   while ((ret = cmsObj_getNext(MDMOID_L2_BRIDGING_ENTRY, &iidStack, (void **)&pBridgeObj)) == CMSRET_SUCCESS) 
   {
      memset(brName, 0 , BUFLEN_64);
      sprintf(brName, "br%d", IndexOfBr);

      if (1) 
      {
         sprintf(cmd, "brctl showmacs %s>/var/qbrfdtbl", brName);
         prctl_runCommandInShellWithTimeout(cmd);
         //bcmSystemMute("snmpd_executeQBridgeCommand", cmd);
         line[0] = '\0';

         fd = fopen("/var/qbrfdtbl", "r");

         if (fd != NULL ) 
         {
            fgets(line, BUFLEN_264, fd);

            while (fgets(line, BUFLEN_264, fd)) 
            {
               int port;
               char macNum[MAC_ADDR_LEN];
               char macStr[MAC_STR_LEN];
               char local[BUFLEN_16];
               char agetime[BUFLEN_16];

               memset(macNum, 0, MAC_ADDR_LEN);
               memset(macStr, 0, MAC_STR_LEN);

               pEntry = (pDOT1Q_TP_FDB_ENTRY_INFO)malloc(sizeof(DOT1Q_TP_FDB_ENTRY_INFO));
               if (pEntry == NULL) 
               {
                  fclose(fd);
                  return -1;
               }
               memset(pEntry,0,sizeof(DOT1Q_TP_FDB_ENTRY_INFO));
               sscanf(line, "%ld %s %s %s", &port, macStr, local, agetime);
               
               bcmMacStrToNum(pEntry->dot1qTpFdbAddress, macStr);
 
               pEntry->dot1qTpFdbPort = port;
               if( 0 == strcmp(local,"yes") )
               {
                  pEntry->dot1qTpFdbStatus = DOT1QTPFDBSTATUS_VALUE_SELF;
               }
               else
               {
                  pEntry->dot1qTpFdbStatus = DOT1QTPFDBSTATUS_VALUE_LEARNED;
               }
               
               IndexOfMac++;
               pEntry->index = IndexOfMac;
               add_dot1qTpFdbInfoToList(pEntry);
            }
            fclose(fd);
            prctl_runCommandInShellWithTimeout("rm /var/qbrfdtbl");
            //            bcmSystemMute("snmpd_executeQBridgeCommand", "rm /var/qbrfdtbl");
         }
      }

      IndexOfBr++;

      cmsObj_free((void **)&pBridgeObj);
   }

   return 1;
}

unsigned char *
var_dot1qTpFdbEntry(int *var_len, Oid *newoid, Oid *reqoid, int searchType, snmp_info_t *mesg, WRITE_METHOD *write_method)
{
   int column = newoid->name[(newoid->namelen - 1)];
   int result;
   pDOT1Q_TP_FDB_ENTRY_INFO pEntry;
   int index;

   index = newoid->namelen;
   newoid->namelen++;
   if( mesg->pdutype != SNMP_SET_REQ_PDU ) 
   {
      scan_dot1qTpFdbTable();

      pEntry = pDot1qTpFdbList->head;
      while (pEntry) 
      {
         newoid->name[index] = pEntry->index;

         result = compare(reqoid, newoid);
         if (((searchType == EXACT) && (result == 0)) ||((searchType == NEXT) && (result < 0)))
         {
            break; /* found */
         }
         pEntry = pEntry->next;
      } 
      if (pEntry == NULL)
      {
         return NO_MIBINSTANCE;
      }
      *var_len = sizeof(long);

      switch (column) 
      {
         case I_dot1qTpFdbAddress:

            if ((pEntry->dot1qTpFdbAddress[0] == 0) && (pEntry->dot1qTpFdbAddress[1] == 0) &&
                (pEntry->dot1qTpFdbAddress[2] == 0) && (pEntry->dot1qTpFdbAddress[3] == 0) &&
                (pEntry->dot1qTpFdbAddress[4] == 0) && (pEntry->dot1qTpFdbAddress[5] == 0))
            {
               *var_len = 0;
            }
            else
            {
               *var_len = sizeof(pEntry->dot1qTpFdbAddress);
            }
            return (unsigned char *)pEntry->dot1qTpFdbAddress;
         case I_dot1qTpFdbPort:
            return (unsigned char *)&pEntry->dot1qTpFdbPort;
         case I_dot1qTpFdbStatus:
            return (unsigned char *)&pEntry->dot1qTpFdbStatus;
         default:
            return NO_MIBINSTANCE;
      } /* switch */
   } /* != SNMP_SET_REQ_PDU */
   else 
   {
      newoid->name[index] = reqoid->name[index];
      /* return whatever as long as it's non-zero */
      return (unsigned char*)&long_return;
   }
}

void add_dot1qVlanCurrentInfoToList(pDOT1Q_VLAN_CURRENT_ENTRY_INFO p)
{
   if (pDot1qVlanCurrentList->head == NULL) 
   {
      pDot1qVlanCurrentList->head = p;
      pDot1qVlanCurrentList->tail = p;
   }
   else 
   {
      pDot1qVlanCurrentList->tail->next = p;
      pDot1qVlanCurrentList->tail = p;
   }
   pDot1qVlanCurrentList->count++;
}

void free_dot1qVlanCurrentInfo(void) 
{
   pDOT1Q_VLAN_CURRENT_ENTRY_INFO ptr;

   while (pDot1qVlanCurrentList->head) 
   {
      ptr = pDot1qVlanCurrentList->head;

      pDot1qVlanCurrentList->head = pDot1qVlanCurrentList->head->next;
      if(ptr)free(ptr);

   }
   pDot1qVlanCurrentList->count = 0;
}

int scan_dot1qVlanCurrentTable(void)
{

   int index = 1;
   pDOT1Q_VLAN_CURRENT_ENTRY_INFO pEntry;
   free_dot1qVlanCurrentInfo();

#ifdef LGD_TO_DO

#else
   //add_dot1qVlanCurrentInfoToList(pEntry);
#endif
   return 1;
}

unsigned char *
var_dot1qVlanCurrentEntry(int *var_len, Oid *newoid, Oid *reqoid, int searchType, snmp_info_t *mesg, WRITE_METHOD *write_method)
{
   int column = newoid->name[(newoid->namelen - 1)];
   int result;
   pDOT1Q_VLAN_CURRENT_ENTRY_INFO pEntry;
   int index;

   *write_method = 0;

   index = newoid->namelen;
   newoid->namelen++;
   if( mesg->pdutype != SNMP_SET_REQ_PDU ) 
   {
      scan_dot1qVlanCurrentTable();

      pEntry = pDot1qVlanCurrentList->head;

      while (pEntry) 
      {
         newoid->name[index] = pEntry->index;
         /* Determine whether it is the requested OID	*/
         result = compare(reqoid, newoid);
         if (((searchType == EXACT) && (result == 0)) ||((searchType == NEXT) && (result < 0)))
         {
            break; /* found */
         }
         pEntry = pEntry->next;
      } 

      if (pEntry == NULL)
      {
         return NO_MIBINSTANCE;
      }

      *var_len = sizeof(long);
      
      switch (column) 
      {
         case I_dot1qVlanTimeMark:
            return NO_MIBINSTANCE;
         case I_dot1qVlanIndex:
            return NO_MIBINSTANCE;
         case I_dot1qVlanFdbId:
            return NO_MIBINSTANCE;       
         default:
            return NO_MIBINSTANCE;
      } /* switch */
   } /* != SNMP_SET_REQ_PDU */
   else 
   {
      newoid->name[index] = reqoid->name[index];
      /* return whatever as long as it's non-zero */
      return (unsigned char*)&long_return;
   }
}

void add_dot1qVlanStaticInfoToList(pDOT1Q_VLAN_STATIC_ENTRY_INFO p)
{
   if (pDot1qVlanStaticList->head == NULL) 
   {
      pDot1qVlanStaticList->head = p;
      pDot1qVlanStaticList->tail = p;
   }
   else 
   {
      pDot1qVlanStaticList->tail->next = p;
      pDot1qVlanStaticList->tail = p;
   }
   pDot1qVlanStaticList->count++;
}

void free_dot1qVlanStaticInfo(void) 
{
   pDOT1Q_VLAN_STATIC_ENTRY_INFO ptr;

   while (pDot1qVlanStaticList->head) 
   {
      ptr = pDot1qVlanStaticList->head;

      pDot1qVlanStaticList->head = pDot1qVlanStaticList->head->next;
      if(ptr->dot1qVlanStaticName)free(ptr->dot1qVlanStaticName);
      if(ptr->dot1qVlanStaticEgressPorts)free(ptr->dot1qVlanStaticEgressPorts);
      if(ptr->dot1qVlanForbiddenEgressPorts)free(ptr->dot1qVlanForbiddenEgressPorts);
      if(ptr->dot1qVlanStaticUntaggedPorts)free(ptr->dot1qVlanStaticUntaggedPorts);
      if(ptr)free(ptr);

   }
   pDot1qVlanStaticList->count = 0;
}

int scan_dot1qVlanStaticTable(void)
{
   pDOT1Q_VLAN_STATIC_ENTRY_INFO pEntry;
   pDOT1Q_VLAN_STATIC_ENTRY_INFO pTmpEntry;

   InstanceIdStack iidStack;
   QMgmtClassificationObject *cObj = NULL;
   CmsRet ret;
   int PortNum;
   char tmpBuf[64];

   free_dot1qVlanStaticInfo();

   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_Q_MGMT_CLASSIFICATION, &iidStack, (void **)&cObj)) == CMSRET_SUCCESS)
   {
      PortNum = 0;
      if((cObj->X_BROADCOM_COM_VLANIDTag > 0)
         &&((strstr(cObj->classInterface, "InternetGatewayDevice.LANDevice.1.LANEthernetInterfaceConfig.1"))
            ||(strstr(cObj->classInterface, "InternetGatewayDevice.LANDevice.1.LANEthernetInterfaceConfig.2"))
            ||(strstr(cObj->classInterface, "InternetGatewayDevice.LANDevice.1.LANEthernetInterfaceConfig.3"))
            ||(strstr(cObj->classInterface, "InternetGatewayDevice.LANDevice.1.LANEthernetInterfaceConfig.4"))
            ||(strstr(cObj->classInterface, "InternetGatewayDevice.LANDevice.1.LANEthernetInterfaceConfig.5"))))
      {
         pEntry = (pDOT1Q_VLAN_STATIC_ENTRY_INFO)malloc(sizeof(DOT1Q_VLAN_STATIC_ENTRY_INFO));
         if (pEntry == NULL) 
         {
            return -1;
         }
         memset(pEntry,0,sizeof(DOT1Q_VLAN_STATIC_ENTRY_INFO));

         pEntry->vlanId = cObj->X_BROADCOM_COM_VLANIDTag;
         pEntry->dot1qVlanStaticName = strdup(cObj->X_BROADCOM_COM_ClassName);

         if(NULL!=strstr(cObj->classInterface, "InternetGatewayDevice.LANDevice.1.LANEthernetInterfaceConfig.1"))
         {
            PortNum = PortNum|ETH_PORTLIST_PORT_1_VALUE;
         }
         if(NULL!=strstr(cObj->classInterface, "InternetGatewayDevice.LANDevice.1.LANEthernetInterfaceConfig.2"))
         {
            PortNum = PortNum|ETH_PORTLIST_PORT_2_VALUE;
         }
         if(NULL!=strstr(cObj->classInterface, "InternetGatewayDevice.LANDevice.1.LANEthernetInterfaceConfig.3"))
         {
            PortNum = PortNum|ETH_PORTLIST_PORT_3_VALUE;
         }
         if(NULL!=strstr(cObj->classInterface, "InternetGatewayDevice.LANDevice.1.LANEthernetInterfaceConfig.4"))
         {
            PortNum = PortNum|ETH_PORTLIST_PORT_4_VALUE;
         }

         memset(tmpBuf, 0 , sizeof(tmpBuf));
         sprintf(tmpBuf, "0x%2x:00:00:00", PortNum);

         pEntry->dot1qVlanStaticEgressPorts = strdup(tmpBuf);
         pEntry->dot1qVlanForbiddenEgressPorts = strdup("0x00:00:00:00");
         pEntry->dot1qVlanStaticUntaggedPorts = strdup("0x00:00:00:00");
         pEntry->dot1qVlanStaticRowStatus = ROW_STATUS_ACTIVE;
   
         pTmpEntry = pDot1qVlanStaticList->head;
      
         while (pTmpEntry) 
         {
            cmsLog_debug("pTmpEntry->vlanId[%d]pEntry->vlanId[%d]", pTmpEntry->vlanId, pEntry->vlanId);
         
            if (pTmpEntry->vlanId == pEntry->vlanId )
            {
               break; /* found */
            }
            pTmpEntry = pTmpEntry->next;
         } 

         if (pTmpEntry == NULL)
         {
            add_dot1qVlanStaticInfoToList(pEntry);
         }
         else
         {
            unsigned char PortListNum[4];
            if(1==util_convertPortListToNumber(PortListNum, pTmpEntry->dot1qVlanStaticEgressPorts))
            {
               cmsLog_debug("PortNum[%2x]PortListNum[%2x]", PortNum, PortListNum[0]);
               PortNum = PortNum|PortListNum[0];
               sprintf(tmpBuf, "0x%2x:00:00:00", PortNum);
               strcpy(pTmpEntry->dot1qVlanStaticEgressPorts, tmpBuf); 
            }
         }
      }
      cmsObj_free((void **)&cObj);
   }   

   return 1;
}
/*
Port Number   6358GW(eth0+swith(eth1))     PortList String                     MAP
    1          eth0                         0x80:00:00:00      1000 0000 0000 0000 0000 0000 0000 0000 
    2          eth1                         0x40:00:00:00      0100 0000 0000 0000 0000 0000 0000 0000
    3          eth1.2                       0x20:00:00:00      0010 0000 0000 0000 0000 0000 0000 0000 
    4          eth1.3                       0x10:00:00:00      0001 0000 0000 0000 0000 0000 0000 0000
    5          eth1.4                       0x08:00:00:00      0000 1000 0000 0000 0000 0000 0000 0000
*/
int util_convertPortListToNumber(char *PortListNum, char *PortListStr)
{
   char *pToken = NULL, *pLast = NULL;
   char *buf;
   UINT16 i = 1;
   int len;
   
   if ( PortListNum == NULL ) 
   {
      return 0;
   }

   if ( (PortListStr == NULL) || (strlen(PortListStr) == 0) ) 
   {
      return 0;
   }

   len = strlen(PortListStr) + 1;

   if (len > 20)
   {
      len = 20;
   }
 
   buf = (char*)malloc(len);
   memset(buf,0,len);

   if ( buf == NULL ) 
   {  
      return 0;
   }

   /* need to copy since strtok_r updates string */
   cmsUtl_strncpy(buf, PortListStr ,len-1);

   /* PortList has the following format
       0xXX:XX:XX:XX where X is hex number */

   cmsLog_debug("buf[%s]PortListStr[%s]", buf, PortListStr);

   pToken = strtok_r(buf, ":", &pLast);

   cmsLog_debug("pToken[%s]pLast[%s]", pToken, pLast);

   if (pToken != NULL) 
   {
      PortListNum[0] = (unsigned char) strtol(pToken, (char **)NULL, 16);
      for ( i = 1; i < 4; i++ ) 
      {
         pToken = strtok_r(NULL, ":", &pLast);
         if (pToken == NULL)
         {  
            free(buf);
            return 0;
         }
         PortListNum[i] = (unsigned char) strtol(pToken, (char **)NULL, 16);
      }
   }
   else
   {
      free(buf);
      return 0;
   }

   free(buf);

   return 1;

}

int util_getNewClassificationOrder(void)
{
   InstanceIdStack iidStack;
   QMgmtClassificationObject *cObj = NULL;
   CmsRet ret;
   int ClsOrder = 0;

   /* display each entry in the classification table. */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_Q_MGMT_CLASSIFICATION, &iidStack, (void **)&cObj)) == CMSRET_SUCCESS)
   {
      if(cObj->classificationOrder>ClsOrder)
      {
         ClsOrder = cObj->classificationOrder;
      }
      cmsObj_free((void **)&cObj);
   }
   ClsOrder ++;
   return ClsOrder;
}

int util_getDefaultQueueKey(void)
{
   int queueKey = -1;
   char intfname[BUFLEN_32];
   InstanceIdStack iidStack;
   QMgmtQueueObject *qObj = NULL;
   CmsRet ret;

   intfname[0] = '\0';


   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_Q_MGMT_QUEUE, &iidStack, (void **)&qObj)) == CMSRET_SUCCESS)
   {
      //      if ((ret = cmsDal_fullPathToIntfname(qObj->queueInterface, intfname)) != CMSRET_SUCCESS)
      if ((ret = qdmIntf_fullPathToIntfnameLocked(qObj->queueInterface, intfname)) != CMSRET_SUCCESS)
      {
         cmsLog_error("qdmIntf_fullPathToIntfname returns error. ret=%d", ret);
         cmsObj_free((void **)&qObj);
         continue;
      }

      if(NULL!=strstr(qObj->queueInterface, "WANConnectionDevice"))
      {
         queueKey = iidStack.instance[iidStack.currentDepth-1];
      }       
           
      cmsObj_free((void **)&qObj);

      if(queueKey!=-1)
      {
         return queueKey;
      }
   }
   return queueKey;
}


int add_dot1qVlanStaticTable(pDOT1Q_VLAN_STATIC_ENTRY_INFO pEntry)
{
   CmsRet ret;
   unsigned char PortListNum[4];
   int queueKey = -1;
   int ClsOrder = 0;

   queueKey = util_getDefaultQueueKey();
   if (queueKey==-1)
   {
      cmsLog_error("There is not a WANConnectionDevice Type Queue in the System.");
      return -1;
   }

   if(1==util_convertPortListToNumber(PortListNum, pEntry->dot1qVlanStaticEgressPorts))
   {
      cmsLog_debug("PortListNum [%d][%d][%d][%d]",PortListNum[0],PortListNum[1],PortListNum[2],PortListNum[3] );
      int iPortIndex = 0;
      int MAX_OCTVALUE = 8;
      int dot1dPortDefaultUserPriority=0;

      for(iPortIndex=1; iPortIndex<=8;iPortIndex++)
      {
         int PortIndexOffset = 0;
         PortIndexOffset = MAX_OCTVALUE - iPortIndex;
         if(PortListNum[0]&(1<<PortIndexOffset))
         {
            LanEthIntfObject *ethObj = NULL;
            QMgmtClassificationObject *cObj;
            InstanceIdStack iidStack;
            char tmpBuf[64];

            ClsOrder = util_getNewClassificationOrder();
            cmsLog_debug("The new create VLAN group include the PORT[%d]", iPortIndex);

            INIT_INSTANCE_ID_STACK(&iidStack);                      
            while (cmsObj_getNext(MDMOID_LAN_ETH_INTF, &iidStack, (void **) &ethObj) == CMSRET_SUCCESS)
            {
               if((iidStack.instance[iidStack.currentDepth-1])==iPortIndex)
               {
                  dot1dPortDefaultUserPriority = ethObj->X_BROADCOM_COM_EthernetPriorityMark;
               }   
               cmsObj_free((void **) &ethObj);   
            }

            
            INIT_INSTANCE_ID_STACK(&iidStack);
            if ((ret = cmsObj_get(MDMOID_Q_MGMT_CLASSIFICATION, &iidStack, OGF_DEFAULT_VALUES, (void **)&cObj)) != CMSRET_SUCCESS)
            {
               cmsLog_error("cmsObj_get returns error. ret=%d", ret);
               return -1;
            }
         
            cObj->classificationEnable = 1;
            cmsMem_free(cObj->X_BROADCOM_COM_ClassName);
            cObj->X_BROADCOM_COM_ClassName = cmsMem_strdup(pEntry->dot1qVlanStaticName);
            cObj->X_BROADCOM_COM_VLANIDTag = pEntry->vlanId;
            cObj->ethernetPriorityMark = dot1dPortDefaultUserPriority;
            memset(tmpBuf, 0, sizeof(tmpBuf));
            sprintf(tmpBuf, "InternetGatewayDevice.LANDevice.1.LANEthernetInterfaceConfig.%d.", iPortIndex);

            cObj->classInterface = cmsMem_strdup(tmpBuf);
            cObj->classQueue = queueKey;
            cObj->classificationOrder = ClsOrder;

            /* create a new Classification instance and push our class obj in */
            INIT_INSTANCE_ID_STACK(&iidStack);
            if ((ret = cmsObj_addInstance(MDMOID_Q_MGMT_CLASSIFICATION, &iidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("cmsObj_addInstance <MDMOID_Q_MGMT_CLASSIFICATION> returns error, ret=%d", ret);
            }
            else
            {
               if ((ret = cmsObj_set((void *)cObj, &iidStack)) != CMSRET_SUCCESS)
               {
                  cmsLog_error("cmsObj_set returns error, ret = %d", ret);
                  cmsObj_deleteInstance(MDMOID_Q_MGMT_CLASSIFICATION, &iidStack);
               }
            }

            cmsObj_free((void **)&cObj);
         }
         
      }
   }
   else
   {
      return -1;
   }

   cmsMgm_saveConfigToFlash();

   return 0;
}

int config_dot1qVlanStaticTable(pDOT1Q_VLAN_STATIC_ENTRY_INFO pEntry)
{

   InstanceIdStack iidStack, iidStackPrev;
   QMgmtClassificationObject *cObj = NULL;
   CmsRet ret;

   unsigned char PortListNum[4];
   util_convertPortListToNumber(PortListNum, pEntry->dot1qVlanStaticEgressPorts);

   /* Firstly Delete the Current Classification with the Same VLANID and EthPort*/
   INIT_INSTANCE_ID_STACK(&iidStack);
   iidStackPrev = iidStack;
   while ((ret = cmsObj_getNext(MDMOID_Q_MGMT_CLASSIFICATION, &iidStack, (void **)&cObj)) == CMSRET_SUCCESS)
   {
      int iPortIndex = 0;
      int MAX_OCTVALUE = 8;
      for(iPortIndex=1; iPortIndex<=8;iPortIndex++)
      {
         int PortIndexOffset = 0;
         PortIndexOffset = MAX_OCTVALUE - iPortIndex;
         if(PortListNum[0]&(1<<PortIndexOffset))
         {
            char tmpBuf[64];
            memset(tmpBuf, 0, sizeof(tmpBuf));
            sprintf(tmpBuf, "InternetGatewayDevice.LANDevice.1.LANEthernetInterfaceConfig.%d.", iPortIndex);

            if(NULL!=strstr(cObj->classInterface, tmpBuf))
            {
               if (pEntry->vlanId != cObj->X_BROADCOM_COM_VLANIDTag)
               {
                  if ((ret = cmsObj_deleteInstance(MDMOID_Q_MGMT_CLASSIFICATION, &iidStack)) != CMSRET_SUCCESS)
                  {
                     cmsLog_error("cmsObj_deleteInstance returns error, ret=%d", ret);
                     cmsObj_free((void **)&cObj);
                     return -1;
                  }
                  iidStack = iidStackPrev;
               }
            }
         }
      }

      if (pEntry->vlanId == cObj->X_BROADCOM_COM_VLANIDTag)
      {
         if ((ret = cmsObj_deleteInstance(MDMOID_Q_MGMT_CLASSIFICATION, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_deleteInstance returns error, ret=%d", ret);
            cmsObj_free((void **)&cObj);
            return -1;
         }
         iidStack = iidStackPrev;
      }
      cmsObj_free((void **)&cObj);

      /* save this iidStack in case we want to do a getNext from this instance */
      iidStackPrev = iidStack;
   }

   /* Secondly Add the new Classification */

   if(pEntry->dot1qVlanStaticRowStatus!=ROW_STATUS_DESTROY)
   {
      int ret = 0;
      ret = add_dot1qVlanStaticTable(pEntry);
      if(ret == -1)
      {
         cmsLog_error("add_dot1qVlanStaticTable returns error, ret=%d", ret);
         return -1;
      }
   }

   return 0;
}

int write_dot1qVlanStaticEntry(int action,unsigned char *var_val, unsigned char varval_type, 
                                 int var_val_len, unsigned char *statP, Oid *name, int name_len)
{
   int dot1qVlanIndex, column, value;
   int namelen;
   int intValue;
   pDOT1Q_VLAN_STATIC_ENTRY_INFO pEntry= NULL;
   int i, ret;
   static int set=0, copy=0;
   static int willCreate = 0; 

   static DOT1Q_VLAN_STATIC_ENTRY_INFO input;

   namelen = (int)name->namelen;

   column =  (int)name->name[namelen-2];

   dot1qVlanIndex = (int)name->name[namelen-1]; 
   
   if(set==0 && copy==0) 
   {  
      memset(&input,0,sizeof(DOT1Q_VLAN_STATIC_ENTRY_INFO));
   }   

   switch (action) 
   {
      case RESERVE1:
         switch (column) 
         {
            case I_dot1qVlanStaticName:

               if (varval_type != SNMP_STRING)
               {
                  return SNMP_ERROR_WRONGTYPE;
               }
               set |= DOT1Q_VLAN_STATIC_NAME_SET;
               copy = set;
               break;

            case I_dot1qVlanStaticEgressPorts:

               if (varval_type != SNMP_STRING)
               {
                  return SNMP_ERROR_WRONGTYPE;
               }
               set |= DOT1Q_VLAN_STATIC_EGRESSPORT_SET;
               copy = set;
               break;

            case I_dot1qVlanForbiddenEgressPorts:

               if (varval_type != SNMP_STRING)
               {
                  return SNMP_ERROR_WRONGTYPE;
               }
               set |= DOT1Q_VLAN_FORBIDDEN_EGRESSPORT_SET;
               copy = set;
               break;

            case I_dot1qVlanStaticUntaggedPorts:

               if (varval_type != SNMP_STRING)
               {
                  return SNMP_ERROR_WRONGTYPE;
               }
               set |= DOT1Q_VLAN_STATIC_UNTAGGEDPORTS_SET;
               copy = set;
               break;

            case I_dot1qVlanStaticRowStatus:

               value = (int)*var_val;
               if ((value != ROW_STATUS_ACTIVE) 
                    &&(value != ROW_STATUS_CREATE_AND_GO) 
                    &&(value != ROW_STATUS_CREATE_AND_WAIT) 
                    &&(value != ROW_STATUS_DESTROY)) 
               {
                  return SNMP_ERROR_WRONGVALUE;
               }

               if ((value == ROW_STATUS_CREATE_AND_GO) 
                    ||(value == ROW_STATUS_CREATE_AND_WAIT)) 
               {
                  willCreate = value;
               }

               set |= DOT1Q_VLAN_STATIC_ROWSTATUS_SET;
               copy = set;
               break;

            default:
               return SNMP_ERROR_NOTWRITABLE;
          } /* switch column */
          break;

      case RESERVE2:
          break;

      case COMMIT:
         switch (column) 
         {
            case I_dot1qVlanStaticName:

               if(NULL!=input.dot1qVlanStaticName)
               {
                   free(input.dot1qVlanStaticName);
               }
               input.dot1qVlanStaticName = (char *)malloc(var_val_len);
               if(NULL!=input.dot1qVlanStaticName)
               {
                  cmsUtl_strncpy(input.dot1qVlanStaticName, var_val, var_val_len);
               }                   
               set &= ~DOT1Q_VLAN_STATIC_NAME_SET;
               break;

            case I_dot1qVlanStaticEgressPorts:

               if(NULL!=input.dot1qVlanStaticEgressPorts)
               {
                   free(input.dot1qVlanStaticEgressPorts);
               }
               input.dot1qVlanStaticEgressPorts = (char *)malloc(var_val_len);
               if(NULL!=input.dot1qVlanStaticEgressPorts)
               {
                  cmsUtl_strncpy(input.dot1qVlanStaticEgressPorts, var_val, var_val_len);
               }                   

               set &= ~DOT1Q_VLAN_STATIC_EGRESSPORT_SET;
               break;

            case I_dot1qVlanForbiddenEgressPorts:

               if(NULL!=input.dot1qVlanForbiddenEgressPorts)
               {
                  free(input.dot1qVlanForbiddenEgressPorts);
               }
               input.dot1qVlanForbiddenEgressPorts = (char *)malloc(var_val_len);
               if(NULL!=input.dot1qVlanForbiddenEgressPorts)
               {
                  cmsUtl_strncpy(input.dot1qVlanForbiddenEgressPorts, var_val, var_val_len);
               }                   
               set &= ~DOT1Q_VLAN_FORBIDDEN_EGRESSPORT_SET;
               break;

            case I_dot1qVlanStaticUntaggedPorts:

               if(NULL!=input.dot1qVlanStaticUntaggedPorts)
               {
                  free(input.dot1qVlanStaticUntaggedPorts);
               }
               input.dot1qVlanStaticUntaggedPorts = (char *)malloc(var_val_len);
               if(NULL!=input.dot1qVlanStaticUntaggedPorts)
               {
                  cmsUtl_strncpy(input.dot1qVlanStaticUntaggedPorts, var_val, var_val_len);
               }                   
               set &= ~DOT1Q_VLAN_STATIC_UNTAGGEDPORTS_SET;
               break;

            case I_dot1qVlanStaticRowStatus:
   
               input.dot1qVlanStaticRowStatus = (int)*var_val;
               set &= ~DOT1Q_VLAN_STATIC_ROWSTATUS_SET;
               break;

            default:
               break;
          } /* switch */

          if ( set == 0 ) 
          {                        
             scan_dot1qVlanStaticTable();       

             pEntry = pDot1qVlanStaticList->head;
             while (pEntry) 
             {
                if(pEntry->vlanId == dot1qVlanIndex)
                { 
                   break;
                }
                else
                {
                   pEntry = pEntry->next;
                }
             }
             if (pEntry == NULL) 
             {
                if (willCreate) 
                {
                   input.dot1qVlanStaticRowStatus=willCreate;
                   input.vlanId = dot1qVlanIndex;
                   willCreate = 0;

                   if ((ret = config_dot1qVlanStaticTable(&input)) == -1) 
                   {
                      return SNMP_ERROR_COMMITFAILED;
                   }
                   else if (ret == 0) 
                   {
                   }

                }

                /* scan again to see if an entry is created successfully */
                scan_dot1qVlanStaticTable();
                pEntry = pDot1qVlanStaticList->head;
                while (pEntry) 
                {
                   if (pEntry->vlanId  == dot1qVlanIndex) 
                   {
                      break;
                   }
                   else
                   {
                      pEntry = pEntry->next;
                   }
                }
                if (pEntry == NULL) 
                {
                   copy = 0;
                   set = 0;
                   willCreate=0;
                   return SNMP_ERROR_COMMITFAILED;   
                }
                
             }
             else
             {
                if ((copy & DOT1Q_VLAN_STATIC_NAME_SET) == DOT1Q_VLAN_STATIC_NAME_SET)
                {  
                   if(pEntry->dot1qVlanStaticName)
                   {
                      free(pEntry->dot1qVlanStaticName);
                   }
                   pEntry->dot1qVlanStaticName = (char *)malloc(strlen(input.dot1qVlanStaticName));
                   strcpy(pEntry->dot1qVlanStaticName, input.dot1qVlanStaticName);

                }

                if ((copy & DOT1Q_VLAN_STATIC_EGRESSPORT_SET) == DOT1Q_VLAN_STATIC_EGRESSPORT_SET)
                {  
                   if(pEntry->dot1qVlanStaticEgressPorts)
                   {
                      free(pEntry->dot1qVlanStaticEgressPorts);
                   }
                   pEntry->dot1qVlanStaticEgressPorts = (char *)malloc(strlen(input.dot1qVlanStaticEgressPorts));

                   strcpy(pEntry->dot1qVlanStaticEgressPorts, input.dot1qVlanStaticEgressPorts);
                   
                }

                if ((copy & DOT1Q_VLAN_FORBIDDEN_EGRESSPORT_SET) == DOT1Q_VLAN_FORBIDDEN_EGRESSPORT_SET)
                {  
                   if(pEntry->dot1qVlanForbiddenEgressPorts)
                   {
                      free(pEntry->dot1qVlanForbiddenEgressPorts);
                   }
                   pEntry->dot1qVlanForbiddenEgressPorts = (char *)malloc(strlen(input.dot1qVlanForbiddenEgressPorts));
                   strcpy(pEntry->dot1qVlanForbiddenEgressPorts, input.dot1qVlanForbiddenEgressPorts);
                }

                if ((copy & DOT1Q_VLAN_STATIC_UNTAGGEDPORTS_SET) == DOT1Q_VLAN_STATIC_UNTAGGEDPORTS_SET)
                {  
                   if(pEntry->dot1qVlanStaticUntaggedPorts)
                   {
                      free(pEntry->dot1qVlanStaticUntaggedPorts);
                   }
                   pEntry->dot1qVlanStaticUntaggedPorts = (char *)malloc(strlen(input.dot1qVlanStaticUntaggedPorts));
                   strcpy(pEntry->dot1qVlanStaticUntaggedPorts, input.dot1qVlanStaticUntaggedPorts);
                }

                if ((copy & DOT1Q_VLAN_STATIC_ROWSTATUS_SET) == DOT1Q_VLAN_STATIC_ROWSTATUS_SET)
                {  
                   pEntry->dot1qVlanStaticRowStatus = input.dot1qVlanStaticRowStatus;
                }
                ret = config_dot1qVlanStaticTable(pEntry);

                if (ret == -1) 
                {
                   return SNMP_ERROR_COMMITFAILED;
                }
             }
          }
          break;

      case FREE:
          set = 0;
          copy = 0;
          if(input.dot1qVlanStaticName) free(input.dot1qVlanStaticName);
          if(input.dot1qVlanStaticEgressPorts) free(input.dot1qVlanStaticEgressPorts);
          if(input.dot1qVlanForbiddenEgressPorts) free(input.dot1qVlanForbiddenEgressPorts);
          if(input.dot1qVlanStaticUntaggedPorts) free(input.dot1qVlanStaticUntaggedPorts);
          memset(&input,0,sizeof(DOT1Q_VLAN_STATIC_ENTRY_INFO));
          break;
   } /* switch */
   return SNMP_ERROR_NOERROR;
}


unsigned char *
var_dot1qVlanStaticEntry(int *var_len, Oid *newoid, Oid *reqoid, int searchType, snmp_info_t *mesg, WRITE_METHOD *write_method)
{
   int column = newoid->name[(newoid->namelen - 1)];
   int result;
   pDOT1Q_VLAN_STATIC_ENTRY_INFO pEntry;
   int index;

#ifdef BUILD_SNMP_SET
   *write_method = (WRITE_METHOD)write_dot1qVlanStaticEntry;
#else
   *write_method = 0;
#endif

   index = newoid->namelen;

   newoid->namelen++;
   if( mesg->pdutype != SNMP_SET_REQ_PDU ) 
   {
      scan_dot1qVlanStaticTable();

      pEntry = pDot1qVlanStaticList->head;

      while (pEntry) 
      {
         newoid->name[index] = pEntry->vlanId;
         /* Determine whether it is the requested OID	*/
         result = compare(reqoid, newoid);
         if (((searchType == EXACT) && (result == 0)) ||((searchType == NEXT) && (result < 0)))
         {
            break; /* found */
         }
         pEntry = pEntry->next;
      } 

      if (pEntry == NULL)
      {
         return NO_MIBINSTANCE;
      }

      *var_len = sizeof(long);
      
      switch (column) 
      {
         case I_dot1qVlanStaticName:
            *var_len = strlen(pEntry->dot1qVlanStaticName);
            return (unsigned char *)pEntry->dot1qVlanStaticName;
         case I_dot1qVlanStaticEgressPorts:
            *var_len = strlen(pEntry->dot1qVlanStaticEgressPorts);
            return (unsigned char *)pEntry->dot1qVlanStaticEgressPorts;
         case I_dot1qVlanForbiddenEgressPorts:
            *var_len = strlen(pEntry->dot1qVlanForbiddenEgressPorts);
            return (unsigned char *)pEntry->dot1qVlanForbiddenEgressPorts;
         case I_dot1qVlanStaticUntaggedPorts:
            *var_len = strlen(pEntry->dot1qVlanStaticUntaggedPorts);
            return (unsigned char *)pEntry->dot1qVlanStaticUntaggedPorts;
         case I_dot1qVlanStaticRowStatus:
            return (unsigned char *)&pEntry->dot1qVlanStaticRowStatus;
         default:
            return NO_MIBINSTANCE;
      } /* switch */
   } /* != SNMP_SET_REQ_PDU */
   else 
   {
      newoid->name[index] = reqoid->name[index];
      /* return whatever as long as it's non-zero */
      return (unsigned char*)&long_return;
   }
}

void add_dot1qPortVlanInfoToList(pDOT1Q_PORT_VLAN_ENTRY_INFO p)
{
   if (pDot1qPortVlanList->head == NULL) 
   {
      pDot1qPortVlanList->head = p;
      pDot1qPortVlanList->tail = p;
   }
   else 
   {
      pDot1qPortVlanList->tail->next = p;
      pDot1qPortVlanList->tail = p;
   }
   pDot1qPortVlanList->count++;
}

void free_dot1qPortVlanInfo(void) 
{
   pDOT1Q_PORT_VLAN_ENTRY_INFO ptr;

   while (pDot1qPortVlanList->head) 
   {
      ptr = pDot1qPortVlanList->head;
      pDot1qPortVlanList->head = pDot1qPortVlanList->head->next;
      free(ptr);
   }
   pDot1qPortVlanList->count = 0;
}

int scan_dot1qPortVlanTable(void)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanEthIntfObject *ethObj = NULL;

   pDOT1Q_PORT_VLAN_ENTRY_INFO pEntry;

   free_dot1qPortVlanInfo();

   while (cmsObj_getNext(MDMOID_LAN_ETH_INTF, &iidStack, (void **) &ethObj) == CMSRET_SUCCESS)
   {
      if ((pEntry= (pDOT1Q_PORT_VLAN_ENTRY_INFO)malloc(sizeof(DOT1Q_PORT_VLAN_ENTRY_INFO))) != NULL) 
      {
         memset(pEntry,0,sizeof(DOT1Q_PORT_VLAN_ENTRY_INFO));
         pEntry->index = iidStack.instance[iidStack.currentDepth-1];
         pEntry->dot1qPvid= ethObj->X_BROADCOM_COM_dot1qPvid;
         add_dot1qPortVlanInfoToList(pEntry);
      }
      cmsObj_free((void **) &ethObj);
   }
   return 1;
}

int config_dot1qPortVlanTable(pDOT1Q_PORT_VLAN_ENTRY_INFO pEntry)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanEthIntfObject *ethObj = NULL;
   CmsRet ret;
           
   while (cmsObj_getNext(MDMOID_LAN_ETH_INTF, &iidStack, (void **) &ethObj) == CMSRET_SUCCESS)
   {
      if((iidStack.instance[iidStack.currentDepth-1])==pEntry->index)
      {
         ethObj->X_BROADCOM_COM_dot1qPvid = pEntry->dot1qPvid;
         if ((ret = cmsObj_set((void *)ethObj, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_set returns error. ret=%d", ret);
            cmsObj_free((void **) &ethObj);
            return -1;
         }         
      }   
      cmsObj_free((void **) &ethObj);   
   }

   cmsMgm_saveConfigToFlash();
   return 0;
}

int write_dot1qPortVlanEntry(int action,unsigned char *var_val, unsigned char varval_type, 
                                 int var_val_len, unsigned char *statP, Oid *name, int name_len)
{
   int index, column, value;
   int namelen;
   int intValue;
   pDOT1Q_PORT_VLAN_ENTRY_INFO pEntry= NULL;
   int i, ret;
   static int set=0, copy=0;
   static int willCreate = 0; 

   static DOT1Q_PORT_VLAN_ENTRY_INFO input;

   namelen = (int)name->namelen;

   column =  (int)name->name[namelen-2];
   
   if(set==0 && copy==0) 
   {  
      memset(&input,0,sizeof(DOT1Q_PORT_VLAN_ENTRY_INFO));
   }   

   switch (action) 
   {
      case RESERVE1:
         switch (column) 
         {
            case I_dot1qPvid:

               if (varval_type != SNMP_UINTEGER)
               {
                  return SNMP_ERROR_WRONGTYPE;
               }
               set |= DOT1Q_PVID_SET;
               copy = set;
               break;

            default:
               return SNMP_ERROR_NOTWRITABLE;
          } /* switch column */
          break;

      case RESERVE2:
          break;

      case COMMIT:
         switch (column) 
         {
            case I_dot1qPvid:
               value = (int)*var_val;
               input.dot1qPvid = value;
               set &= ~DOT1Q_PVID_SET;
               break;

            default:
               break;
          } /* switch */

          if ( set == 0 ) 
          {             
             index = (int)name->name[namelen-1]; 
             
             scan_dot1qPortVlanTable();       

             pEntry = pDot1qPortVlanList->head;
             
             while (pEntry) 
             {
                if(pEntry->index == index)
                { 
                   break;
                }
                else
                {
                   pEntry = pEntry->next;
                }
             }

             if (pEntry == NULL) 
             {
                return SNMP_ERROR_COMMITFAILED;
             }

             if ((copy & DOT1Q_PVID_SET) == DOT1Q_PVID_SET)
             {  
                pEntry->dot1qPvid = input.dot1qPvid;
             }

             ret = config_dot1qPortVlanTable(pEntry);

             if (ret == -1) 
             {
                return SNMP_ERROR_COMMITFAILED;
             }
          }
          break;

      case FREE:
          set = 0;
          copy = 0;
          memset(&input,0,sizeof(DOT1Q_PORT_VLAN_ENTRY_INFO));
          break;
   } /* switch */
   return SNMP_ERROR_NOERROR;
}


unsigned char *
var_dot1qPortVlanEntry(int *var_len, Oid *newoid, Oid *reqoid, int searchType, snmp_info_t *mesg, WRITE_METHOD *write_method)
{
   int column = newoid->name[(newoid->namelen - 1)];
   int result;
   pDOT1Q_PORT_VLAN_ENTRY_INFO pEntry;
   int index;

#ifdef BUILD_SNMP_SET
   *write_method = (WRITE_METHOD)write_dot1qPortVlanEntry;
#else
   *write_method = 0;
#endif


   index = newoid->namelen;
   newoid->namelen++;
   if( mesg->pdutype != SNMP_SET_REQ_PDU ) 
   {
      scan_dot1qPortVlanTable();
      pEntry = pDot1qPortVlanList->head;
      
      while (pEntry) 
      {         
         newoid->name[index] = pEntry->index;
         /* Determine whether it is the requested OID	*/
         result = compare(reqoid, newoid);
         if (((searchType == EXACT) && (result == 0)) ||((searchType == NEXT) && (result < 0)))
         {
            break; /* found */
         }
         pEntry = pEntry->next;
      } 

      if (pEntry == NULL)
      {
         return NO_MIBINSTANCE;
      }

      *var_len = sizeof(long);

      switch (column) 
      {
         case I_dot1qPvid:
            return (unsigned char *)&pEntry->dot1qPvid;
         default:
            return NO_MIBINSTANCE;
      } /* switch */
   } /* != SNMP_SET_REQ_PDU */
   else 
   {
      newoid->name[index] = reqoid->name[index];
      /* return whatever as long as it's non-zero */
      return (unsigned char*)&long_return;
   }
}

/******************** dot1qTpFdbEntry ********************************/
static oid dot1qTpFdbEntry_oid[] = { O_dot1qTpFdbEntry };
static Object dot1qTpFdbEntry_variables[] = 
{
   { SNMP_STRING, (RONLY| COLUMN), (void*)var_dot1qTpFdbEntry,
       {1, { I_dot1qTpFdbAddress}}},
   { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_dot1qTpFdbEntry,
       {1, { I_dot1qTpFdbPort}}},
   { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_dot1qTpFdbEntry,
       {1, { I_dot1qTpFdbStatus }}},
   { (int)NULL }
};
static SubTree dot1qTpFdbEntry_tree =  { NULL, dot1qTpFdbEntry_variables,
                                        (sizeof(dot1qTpFdbEntry_oid)/sizeof(oid)), dot1qTpFdbEntry_oid};

/******************** dot1qVlanCurrentEntry ********************************/
static oid dot1qVlanCurrentEntry_oid[] = { O_dot1qVlanCurrentEntry };
static Object dot1qVlanCurrentEntry_variables[] = 
{
   { SNMP_TIMETICKS, (NOACCESS| COLUMN), (void*)var_dot1qVlanCurrentEntry,
       {1, { I_dot1qVlanTimeMark}}},
   { SNMP_INTEGER, (NOACCESS| COLUMN), (void*)var_dot1qVlanCurrentEntry,
       {1, { I_dot1qVlanIndex}}},
   { SNMP_UINTEGER, (RONLY| COLUMN), (void*)var_dot1qVlanCurrentEntry,
       {1, { I_dot1qVlanFdbId}}},
   { SNMP_STRING, (RONLY| COLUMN), (void*)var_dot1qVlanCurrentEntry,
       {1, { I_dot1qVlanCurrentEgressPorts}}},
   { SNMP_STRING, (RONLY| COLUMN), (void*)var_dot1qVlanCurrentEntry,
       {1, { I_dot1qVlanCurrentUntaggedPorts}}},
   { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_dot1qVlanCurrentEntry,
       {1, { I_dot1qVlanStatus}}},
   { SNMP_TIMETICKS, (RONLY| COLUMN), (void*)var_dot1qVlanCurrentEntry,
       {1, { I_dot1qVlanCreationTime}}},
   { (int)NULL }
};
static SubTree dot1qVlanCurrentEntry_tree =  { NULL, dot1qVlanCurrentEntry_variables,
                                        (sizeof(dot1qVlanCurrentEntry_oid)/sizeof(oid)), dot1qVlanCurrentEntry_oid};


/******************** dot1qVlanStaticEntry ********************************/
static oid dot1qVlanStaticEntry_oid[] = { O_dot1qVlanStaticEntry };
static Object dot1qVlanStaticEntry_variables[] = 
{
   { SNMP_STRING, (RWRITE| COLUMN), (void*)var_dot1qVlanStaticEntry,
       {1, { I_dot1qVlanStaticName}}},
   { SNMP_STRING, (RWRITE| COLUMN), (void*)var_dot1qVlanStaticEntry,
       {1, { I_dot1qVlanStaticEgressPorts}}},
   { SNMP_STRING, (RWRITE| COLUMN), (void*)var_dot1qVlanStaticEntry,
       {1, { I_dot1qVlanForbiddenEgressPorts }}},
   { SNMP_STRING, (RWRITE| COLUMN), (void*)var_dot1qVlanStaticEntry,
       {1, { I_dot1qVlanStaticUntaggedPorts }}},
   { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_dot1qVlanStaticEntry,
       {1, { I_dot1qVlanStaticRowStatus }}},
   { (int)NULL }
};
static SubTree dot1qVlanStaticEntry_tree =  { NULL, dot1qVlanStaticEntry_variables,
                                        (sizeof(dot1qVlanStaticEntry_oid)/sizeof(oid)), dot1qVlanStaticEntry_oid};

/******************** dot1qVlanStaticEntry ********************************/
static oid dot1qPortVlanEntry_oid[] = { O_dot1qPortVlanEntry };
static Object dot1qPortVlanEntry_variables[] = 
{
   { SNMP_UINTEGER, (RWRITE| COLUMN), (void*)var_dot1qPortVlanEntry,
       {1, { I_dot1qPvid}}},
   { (int)NULL }
};
static SubTree dot1qPortVlanEntry_tree =  { NULL, dot1qPortVlanEntry_variables,
                                        (sizeof(dot1qPortVlanEntry_oid)/sizeof(oid)), dot1qPortVlanEntry_oid};


/* This is the MIB registration function. This should be called */
/* within the init_rmon-function */
void register_subtrees_of_qBridge()
{
   insert_group_in_mib(&dot1qTpFdbEntry_tree);
   insert_group_in_mib(&dot1qVlanStaticEntry_tree);
   insert_group_in_mib(&dot1qPortVlanEntry_tree);
}

/* rmon initialisation (must also register the MIB module tree) */
void init_qBridge()
{
    register_subtrees_of_qBridge();
    
    if ((pDot1qTpFdbList = (pDOT1Q_TP_FDB_TABLE_LIST)malloc(sizeof(DOT1Q_TP_FDB_TABLE_LIST))) == NULL) 
    {
       cmsLog_error("init_qBridge malloc memory failed for pDot1qTpFdbList");
       exit(0);
    } 
    memset(pDot1qTpFdbList,0,sizeof(DOT1Q_TP_FDB_TABLE_LIST));

    if ((pDot1qVlanStaticList = (pDOT1Q_VLAN_STATIC_TABLE_LIST)malloc(sizeof(DOT1Q_VLAN_STATIC_TABLE_LIST))) == NULL) 
    {
       cmsLog_error("init_qBridge malloc memory failed for pDot1qVlanStaticList");
       exit(0);
    } 
    memset(pDot1qVlanStaticList,0,sizeof(DOT1Q_VLAN_STATIC_TABLE_LIST));

    if ((pDot1qPortVlanList = (pDOT1Q_PORT_VLAN_TABLE_LIST)malloc(sizeof(DOT1Q_PORT_VLAN_TABLE_LIST))) == NULL) 
    {
       cmsLog_error("init_qBridge malloc memory failed for pDot1qPortVlanList");
       exit(0);
    } 
    memset(pDot1qPortVlanList,0,sizeof(DOT1Q_PORT_VLAN_TABLE_LIST));

}
#endif
