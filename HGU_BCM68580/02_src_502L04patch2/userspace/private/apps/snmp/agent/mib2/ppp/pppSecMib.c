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
/* General includes */
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

/* SNMP includes */
#include "asn1.h"
#include "snmp.h"
#include "agtMib.h"
#include "agtEngine.h"
#include "pppSecMib.h"
#include "pppMibHelper.h"

pPPP_SEC_LIST pPppSecretList = NULL;
void pppMibFree(void)
{
  pppFreeSecretInfo();
  if (pPppSecretList)
    free(pPppSecretList);
}

/* PPP_SEC_MIB initialisation (must also register the MIB module tree) */
void init_pppSecMib()
{
    register_subtrees_of_ppp_sec_mib();

    if ((pPppSecretList = malloc(sizeof(PPP_SEC_LIST))) == NULL) {
      printf("init_pppSecmib(): unable to allocation PPP SEC LIST\n");
      exit(0);
    } 
    memset(pPppSecretList,0,sizeof(PPP_SEC_LIST));
}

/* Secret information.
 */
int write_pppSecuritySecretsEntry(int action,unsigned char *var_val,
                                  unsigned char varval_type, 
                                  int var_val_len, unsigned char *statP, Oid *name, 
                                  SNMP_SET_HANDLER *set_handler, unsigned long *param)
{
  oid oid_pap[] = {O_pppSecurityPapProtocol};
  oid oid_chap[] = {O_pppSecurityChapMD5Protocol};
  oid inOid[PPP_SEC_PROTOCOL_LEN];
  int inOidLen;
  int value, column;
  int namelen;
  int index, link;
  PPP_SEC_ENTRY entry;
  static int set=0, copy=0;

  namelen = (int)name->namelen;
  /* column, link, idIndex, 0 ==> namelen at 0, column is at (namelen-3)*/
  column =  (int)name->name[namelen-3];

#ifdef BUILD_SNMP_DEBUG
  printf(" write_pppSecuritySecretsEntry (entry)\n");
#endif

  switch (action) 
    {
    case RESERVE1:
      switch (column) 
        {
        case I_pppSecuritySecretsLink:
        case I_pppSecuritySecretsIdIndex:
          return SNMP_ERROR_NOTWRITABLE;
        case I_pppSecuritySecretsDirection:
          if (varval_type != SNMP_INTEGER)
            return SNMP_ERROR_WRONGTYPE;
          if (var_val_len > sizeof(int)) 
            return SNMP_ERROR_WRONGLENGTH;
          value = (int)*var_val;
          if ((value != PPP_SEC_LOCAL_TO_REMOTE) && (value != PPP_SEC_REMOTE_TO_LOCAL))
            return SNMP_ERROR_WRONGVALUE;
          break;
        case I_pppSecuritySecretsProtocol:
          if (varval_type != SNMP_OBJID)
            return SNMP_ERROR_WRONGTYPE;
          if (var_val_len > PPP_SEC_PROTOCOL_LEN) 
            return SNMP_ERROR_WRONGLENGTH;
          
	  asn_parse_subidentifier(var_val,var_val_len,inOid,&inOidLen);

          if ((memcmp((void*)inOid,(void*)oid_pap,(sizeof(oid)*var_val_len)) != 0) &&
              (memcmp((void*)inOid,(void*)oid_chap,(sizeof(oid)*var_val_len)) != 0)) {
                return SNMP_ERROR_WRONGVALUE;
          }
          break;
        case I_pppSecuritySecretsIdentity:
        case I_pppSecuritySecretsSecret:
          if (varval_type != SNMP_STRING)
            return SNMP_ERROR_WRONGTYPE;
          if (var_val_len > 32) 
            return SNMP_ERROR_WRONGLENGTH;
	  if (column == I_pppSecuritySecretsIdentity)
	    set |= PPP_SEC_ID_SET;
	  else
	    set |= PPP_SEC_PASSWD_SET;
	  copy = set;
          break;
        case I_pppSecuritySecretsStatus:
          if (varval_type != SNMP_INTEGER)
            return SNMP_ERROR_WRONGTYPE;
          if (var_val_len > sizeof(int)) 
            return SNMP_ERROR_WRONGLENGTH;
          value = (int)*var_val;
          if ((value != PPP_SEC_STATUS_VALID) && (value != PPP_SEC_STATUS_INVALID))
            return SNMP_ERROR_WRONGVALUE;
          break;
        default:
          return SNMP_ERROR_NOTWRITABLE;
        } /* switch column */
      break;
    case RESERVE2:
      break;
    case COMMIT:
      /* link, idIndex, column, namelen at column  */
      index = (int)name->name[namelen-1];
      link = (int)name->name[namelen-2];

      switch (column) 
        {
        case I_pppSecuritySecretsLink:
        case I_pppSecuritySecretsIdIndex:
          return SNMP_ERROR_NOTWRITABLE;
        case I_pppSecuritySecretsDirection:
          /* ignore, always local to remote */
          break;
        case I_pppSecuritySecretsProtocol:
	  asn_parse_subidentifier(var_val,var_val_len,inOid,&inOidLen);
	  entry.protocolLen = inOidLen*sizeof(oid);
	  memset(entry.secretProtocol,0,sizeof(entry.secretProtocol));
          memcpy((void*)(entry.secretProtocol),(void*)inOid,entry.protocolLen);

#ifdef BUILD_SNMP_DEBUG
          printf("write_pppSecuritySecretsEntry(): call CFM to store secretProtocol\n");
#endif
          pppConfigSecretInfo(&entry,link,PPP_SEC_SECRET_PROTOCOL_FIELD);
          break;
        case I_pppSecuritySecretsIdentity:
          if (var_val_len > PPP_SEC_MAX_LENGTH)
            entry.secretIdLen = PPP_SEC_MAX_LENGTH;
          else
            entry.secretIdLen =  var_val_len;
          cmsUtl_strncpy(entry.secretId,var_val,entry.secretIdLen);
          entry.secretId[var_val_len] = '\0';
          set &= ~PPP_SEC_ID_SET;
#ifdef BUILD_SNMP_DEBUG
          printf("write_pppSecuritySecretsEntry(): call CFM to store secretId %s, len %d\n",
                 entry.secretId, entry.secretIdLen);
#endif
            pppConfigSecretInfo(&entry,link,PPP_SEC_SECRET_ID_FIELD);
          break;
        case I_pppSecuritySecretsSecret:
          if (var_val_len  > PPP_SEC_PASSWORD_LENGTH)
            entry.secretIdLen = PPP_SEC_PASSWORD_LENGTH;
          else
            entry.secretLen =  var_val_len;
          cmsUtl_strncpy(entry.secret,var_val,entry.secretLen);
          entry.secret[var_val_len] = '\0';
          set &= ~PPP_SEC_ID_SET;
#ifdef BUILD_SNMP_DEBUG
          printf("write_pppSecuritySecretsEntry(): call CFM to store secretId %s, len %d\n",
                 entry.secret, entry.secretLen);
#endif
          pppConfigSecretInfo(&entry,link,PPP_SEC_SECRET_PASSWORD_FIELD);
          break;
        case I_pppSecuritySecretsStatus:
          /* ignore this, it's always valid */
          break;
        default:
          return SNMP_ERROR_COMMITFAILED;
        }
        break;
      case FREE:
        break;
      } /* switch */
    return SNMP_ERROR_NOERROR;
}

unsigned char *
var_pppSecuritySecretsEntry(int *var_len,
        Oid *newoid, Oid *reqoid, int searchType,
        snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  int	column = newoid->name[(newoid->namelen - 1)];
  int	result;
  pPPP_SEC_ENTRY pEntry;
  int index, link;

  /* Set write-function */
  *write_method = (WRITE_METHOD)write_pppSecuritySecretsEntry;
  /* index.link:  210000.0 */
  link = newoid->namelen++;
  index = newoid->namelen++;

  if( mesg->pdutype != SNMP_SET_REQ_PDU ) {
    /* index of this table is ifIndex, vpi, vci */
    pppScanPppInfo();

#ifdef BUILD_SNMP_DEBUG
    printf("var_pppSecuritySecretsEntry(): after pppScanPppInfo\n");
#endif

    if (pPppSecretList->count == 0) 
      return NO_MIBINSTANCE;

    pEntry = pPppSecretList->head;
    while (pEntry) {

#ifdef BUILD_SNMP_DEBUG
      printf("var_pppSecuritySecretsEntry(): looping secret list\n");
#endif

      newoid->name[link] = pEntry->secretLink;
      newoid->name[index] = pEntry->secretIdIndex;

      result = compare(reqoid, newoid);
      if (((searchType == EXACT) && (result == 0)) ||
          ((searchType == NEXT) && (result < 0)))
        break; /* found */
      pEntry = pEntry->next;
    }
    if (pEntry == NULL) {
      return NO_MIBINSTANCE;
    }

    /* Set size (in bytes) and return address of the variable */
    *var_len = sizeof(long);
    switch (column) 
      {
      case I_pppSecuritySecretsLink:
        return (unsigned char *)&pEntry->secretLink;
      case I_pppSecuritySecretsIdIndex:
        return (unsigned char *)&pEntry->secretIdIndex;
      case I_pppSecuritySecretsDirection:
        return (unsigned char *)&pEntry->secretDirection;
      case I_pppSecuritySecretsProtocol:
        *var_len = pEntry->protocolLen;
        return (unsigned char *)pEntry->secretProtocol;
      case I_pppSecuritySecretsIdentity:
        *var_len = pEntry->secretIdLen;
        return (unsigned char *)pEntry->secretId;
      case I_pppSecuritySecretsSecret:
        *var_len = pEntry->secretLen;
        return (unsigned char *)pEntry->secret;
      case I_pppSecuritySecretsStatus:
        return (unsigned char *)&pEntry->status;
      default:
        return NO_MIBINSTANCE;
      } /* switch */
  } /* if mesg->pdutype == SNMP_GET_REQ_PDU */
  else {
    newoid->name[link] = reqoid->name[link];
    newoid->name[index] = reqoid->name[index];
    /* return whatever as long as it's non-zero */
    return (unsigned char*)&long_return;
  }
}

static oid pppSecuritySecretsEntry_oid[] = { O_pppSecuritySecretsEntry };
static Object pppSecuritySecretsEntry_variables[] = {
    { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_pppSecuritySecretsEntry,
                {1, { I_pppSecuritySecretsLink }}},
    { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_pppSecuritySecretsEntry,
                {1, { I_pppSecuritySecretsIdIndex }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_pppSecuritySecretsEntry,
                {1, { I_pppSecuritySecretsDirection }}},
    { SNMP_OBJID, (RWRITE| COLUMN), (void*)var_pppSecuritySecretsEntry,
                {1, { I_pppSecuritySecretsProtocol }}},
    { SNMP_STRING, (RWRITE| COLUMN), (void*)var_pppSecuritySecretsEntry,
                {1, { I_pppSecuritySecretsIdentity }}},
    { SNMP_STRING, (RWRITE| COLUMN), (void*)var_pppSecuritySecretsEntry,
                {1, { I_pppSecuritySecretsSecret }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_pppSecuritySecretsEntry,
                {1, { I_pppSecuritySecretsStatus }}},
    { (int)NULL }
    };
static SubTree pppSecuritySecretsEntry_tree =  { NULL, pppSecuritySecretsEntry_variables,
	        (sizeof(pppSecuritySecretsEntry_oid)/sizeof(oid)), pppSecuritySecretsEntry_oid};

/* This is the MIB registration function. This should be called */
/* within the init_PPP_SEC_MIB-function */
void register_subtrees_of_ppp_sec_mib()
{
    insert_group_in_mib(&pppSecuritySecretsEntry_tree);
}

#endif /* CPE_DSL_MIB */
