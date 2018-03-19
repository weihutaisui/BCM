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
#ifdef CPE_DSL_MIB
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <bcm_local_kernel_include/linux/sockios.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <bcm_local_kernel_include/linux/if_arp.h>
#include <bcmtypes.h>
#include "asn1.h"
#include "agtMib.h"
#include "syscall.h"
#include "ifcdefs.h"
#include "dbapi.h"
#include "pppMibHelper.h"
#include "ifMib.h"
#include "ifcrtwanapi.h"
#include "cpeDslMib.h"

extern pPPP_SEC_LIST pPppSecretList;
extern pPPP_REFERENCE_LIST pPppReferenceList;

void pppFreeSecretInfo(void) {
  pPPP_SEC_ENTRY ptr;

  while (pPppSecretList->head) {
    ptr = pPppSecretList->head;
    pPppSecretList->head = pPppSecretList->head->next;
    free(ptr);
  }
  pPppSecretList->count = 0;
}

void pppAddSecretToList(pPPP_SEC_ENTRY p)
{
  pPPP_SEC_ENTRY ptr, ptrNext, ptrHead;

  /* add to list, order is sorted by vpi/vci */
  if (pPppSecretList->head == NULL) {
    pPppSecretList->head = p;
    pPppSecretList->tail = p;
  }
  else {
    ptrHead = ptr = pPppSecretList->head;
    ptrNext = ptr->next;
    while (ptr != NULL) {
      if ((ptrNext) && (p->secretLink > ptrNext->secretLink)) {
        /* move on */
        ptrHead = ptr;
        ptr = ptr->next;
        if (ptr != NULL)
          ptrNext = ptr->next;
        else
          ptrNext = NULL;
      }
      else {
        if (ptr->secretLink > p->secretLink) {
          p->next = ptr;
          //          ptrHead = p;
          pPppSecretList->head = p;
          break;
        } /* head */
        else if ((ptrNext == NULL) && (ptr->secretLink < p->secretLink)) {
          ptr->next = p;
          pPppSecretList->tail = p;
          break;
        } /* end */
        else {
          p->next = ptrNext;
          ptr->next = p;
          break;
        } /* middle */
      } /* else, do the adding */
    } /* while */
  } /* list != NULL */
  pPppSecretList->count++;
}

void pppScanPppInfo(void)
{
  WAN_CON_ID wanId;  /* vpi.vci.connId */
  WAN_CON_INFO wanInfo;
  WAN_PPP_INFO pppInfo;
  pPPP_SEC_ENTRY pEntry;
  int i = 0;
  char ifName[IFC_TINY_LEN];
  oid oid_pap[] = {O_pppSecurityPapProtocol};
  oid oid_chap[] = {O_pppSecurityChapMD5Protocol};
  pCPE_PPP_REFERENCE pSave;

  pppFreeSecretInfo();
  cpeFreeReferenceList();

  memset(&wanId,0,sizeof(wanId));
  while (BcmDb_getWanInfoNext(&wanId,&wanInfo) != DB_WAN_GET_NOT_FOUND) {
    if ((wanInfo.protocol != PROTO_PPPOE) && (wanInfo.protocol != PROTO_PPPOA))
      continue;
    if ((pEntry= (pPPP_SEC_ENTRY)malloc(sizeof(PPP_SEC_ENTRY))) != NULL) {
      memset(pEntry,0,sizeof(PPP_SEC_ENTRY));
      if (BcmDb_getPppSrvInfo(&wanId,&pppInfo) == DB_WAN_GET_NOT_FOUND)
        continue;
      pEntry->vpi = wanId.vpi;
      pEntry->vci = wanId.vci;
      pEntry->conId = wanId.conId;
      //      pEntry->secretIdIndex = i;     
      pEntry->secretDirection = PPP_SEC_LOCAL_TO_REMOTE;
      switch (pppInfo.authMethod)
        {
        case PPP_AUTH_METHOD_PAP:
          memcpy(pEntry->secretProtocol,oid_pap,sizeof(oid_pap));
          pEntry->protocolLen = sizeof(oid_pap);
          break;
        case PPP_AUTH_METHOD_CHAP:
        case PPP_AUTH_METHOD_MSCHAP:
        case PPP_AUTH_METHOD_AUTO:  /* auto is also Chap, no definition in MIB for auto */
          memcpy(pEntry->secretProtocol,oid_chap,sizeof(oid_chap));
          pEntry->protocolLen = sizeof(oid_chap);
          break;
        } /* switch */
      pEntry->secretIdLen = strlen(pppInfo.userName);
      strcpy(pEntry->secretId,pppInfo.userName);
      pEntry->secretLen = strlen(pppInfo.password);
      strcpy(pEntry->secret,pppInfo.password);
      pEntry->status = PPP_SEC_STATUS_VALID;
      memset(ifName,0,IFC_TINY_LEN);
      sprintf(ifName,"ppp_%d_%d_%d",wanId.vpi,wanId.vci,wanId.conId);

      /* link is 210,000; and index is 0,1,2 */
      pEntry->secretLink = ifGetIfIndexByName(ifName);   
      pEntry->secretIdIndex = i;
      if (pEntry->secretLink <= 0)
        pEntry->secretLink = IF_INDEX_PPP_BASE + i;
      i++;


	  pSave = (pCPE_PPP_REFERENCE)malloc(sizeof(CPE_PPP_REFERENCE));
	  if (pSave != NULL) {
        memset(pSave,0,(sizeof(CPE_PPP_REFERENCE)));
	    /* save this for username/password; this is a solution for not being
	       able to get ppp_%vpi%vci when PPP is not up; router 's limitation */
        /* link is 210000 */
	    pSave->pppIfIndex = (int)pEntry->secretLink;

#ifdef BUILD_SNMP_DEBUG
          printf("---------->pSave->pppIfIndex %d\n",pSave->pppIfIndex);
#endif
	    pSave->vpi = wanId.vpi;
	    pSave->vci = wanId.vci;
	    pSave->conId = 1;
	    cpeAddPppRefernceToList(pSave);
	  }

#ifdef BUILD_SNMP_DEBUG
        printf("---------->after saving refernce list, going to add secret entry to list\n");
#endif
      pppAddSecretToList(pEntry);

    } /* pEntry != NULL */
  } /* while */

#ifdef BUILD_SNMP_DEBUG
    if (pPppSecretList->count != 0) {
      printf("************---pppSecList.count %d---************\n",
             pPppSecretList->count);
      pEntry = pPppSecretList->head;
      while (pEntry) {
        printf("secretLink/Id %d/%d, vpi/vci %d/%d, username/password %s/%s\n",
               pEntry->secretLink,pEntry->secretIdIndex,pEntry->vpi,pEntry->vci,
               pEntry->secretId,pEntry->secret);
        pEntry = pEntry->next;
      }
    }
    if (pPppReferenceList->count != 0) {
      printf("************---pppReferenceList.count %d---************\n",
             pPppReferenceList->count);
      pSave = pPppReferenceList->head;
      while (pSave) {
        printf("index %d, vpi/vci %d/%d\n",
               pSave->pppIfIndex, pSave->vpi,pSave->vci);
        pSave = pSave->next;
      }
    }
    printf("end of pppScanInfo\n");
#endif
}

int pppConfigSecretInfo(pPPP_SEC_ENTRY pEntry, int index, int field)
{
  WAN_CON_ID wanId;  /* vpi.vci.connId */
  WAN_PPP_INFO pppInfo;
  oid oid_pap[] = {O_pppSecurityPapProtocol};
  pCPE_PPP_REFERENCE ptr;
  int change = 0;

#ifdef BUILD_SNMP_DEBUG
    printf("pppConfigSecretInfo(): index%d, field %d\n",index,field);
#endif

  pppScanPppInfo();
  ptr = pPppReferenceList->head;
  while (ptr) {
    if (ptr->pppIfIndex == index)
      break;
  }
  if (ptr == NULL)
    return -1;

  wanId.vpi = ptr->vpi;
  wanId.vci = ptr->vci;
  wanId.conId = ptr->conId;

#ifdef BUILD_SNMP_DEBUG
  printf("calling BcmDb_getPppSrvInfo(): wanId %d/%d/%d\n",wanId.vpi, wanId.vci, wanId.conId);
#endif

  if (BcmDb_getPppSrvInfo(&wanId,&pppInfo) == DB_WAN_GET_NOT_FOUND)
    return -1;
  switch (field)
    {
    case PPP_SEC_SECRET_DIR_FIELD:
      /* ignore, modem doesn't support */
      break;
    case PPP_SEC_SECRET_PROTOCOL_FIELD:
      if (memcmp(pEntry->secretProtocol,oid_pap,pEntry->protocolLen) == 0) {
	if (pppInfo.authMethod != PPP_AUTH_METHOD_PAP) {
	  pppInfo.authMethod = PPP_AUTH_METHOD_PAP;
	  change = 1;
	}
      }
      else {
	if (pppInfo.authMethod  != PPP_AUTH_METHOD_CHAP) {
	  pppInfo.authMethod = PPP_AUTH_METHOD_CHAP;
	  change = 1;
	}
      }
      break;
    case PPP_SEC_SECRET_ID_FIELD:
      if (strcmp(pppInfo.userName,pEntry->secretId) != 0) {
	cmsUtl_strncpy(pppInfo.userName,pEntry->secretId,pEntry->secretIdLen);
	pppInfo.userName[pEntry->secretIdLen] = '\0';
	change = 1;
      }
      break;
    case PPP_SEC_SECRET_PASSWORD_FIELD:
      if (strcmp(pppInfo.password,pEntry->secret) != 0) {
	cmsUtl_strncpy(pppInfo.password,pEntry->secret,pEntry->secretLen);
	pppInfo.password[pEntry->secretLen] = '\0';
	change = 1;
      }
      break;
    case PPP_SEC_SECRET_STATUS_FIELD:
      /* ignore, it's always assumed to be valid */
      break;
    default:
      return -1;
    }
  if (change) {
    BcmDb_setPppSrvInfo(&wanId,&pppInfo);
    //~~~ restart pppoe here
    if (field == PPP_SEC_SECRET_PASSWORD_FIELD) {
      BcmPppoe_stopPppd(&wanId);
      BcmPppoe_startPppd(&wanId);
    }
  }
  return 0;
}

#endif /* CPE_DSL_MIB */
