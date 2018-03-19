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
/* General includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <signal.h>

/* SNMP includes */
#include "asn1.h"
#include "snmp.h"
#include "agtMib.h"
#include "snmpv2Mib.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

struct sysOREntry_struct snmpV2_value = { NULL, 1, {7, {O_snmpMIB}}, "The MIB module for SNMPv2 entities", 0};

static char strNotInitialized[] = "Not initialized";

extern char outputBuffer[];
extern struct timeval snmpUpTime;

/* SNMPv2_MIB initialisation (must also register the MIB module tree) */
void init_SNMPv2_MIB()
{
   register_subtrees_of_SNMPv2_MIB();
}

/* A textual description of the entity.  This value should
 * include the full name and version identification of the
 * system's hardware type, software operating-system, and
 * networking software.
 */

unsigned char *
var_sysDescr(int *var_len, Access_rec *access_id,
             WRITE_METHOD *write_method)
{
   int len;
   IGDDeviceInfoObject *obj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   if (cmsObj_get(MDMOID_IGD_DEVICE_INFO, &iidStack, 0, (void **) &obj) == CMSRET_SUCCESS)
   {
      len = strlen(obj->softwareVersion);
      *var_len = len;
      strcpy(outputBuffer,obj->softwareVersion);
      cmsObj_free((void **) &obj);
   }
   else
   {
      cmsLog_error("Unable to get MDMOID_IGD_DEVICE_INFO");
      *var_len = 0;
   }
   return (outputBuffer);
}

/* The vendor's authoritative identification of the network
 * allocated within the SMI enterprises subtree (1.3.6.1.4.1)
 * and provides an easy and unambiguous means for determining
 * `Flintstones, Inc.' was assigned the subtree
 * 1.3.6.1.4.1.4242, it could assign the identifier
 * 1.3.6.1.4.1.4242.1.1 to its `Fred Router'.
 */

unsigned char *
var_sysObjectID(int *var_len, Access_rec *access_id,
        WRITE_METHOD *write_method)
{
  static oid sysObjectID_value[] = {SYS_OBJ};

  *var_len = sizeof(sysObjectID_value);
  return (unsigned char *) &sysObjectID_value;
}

unsigned long current_sysUpTime()
{
  struct timeval  now;

  gettimeofday(&now, (struct timezone *)0);
  return ((now.tv_sec - snmpUpTime.tv_sec) * 100
    + (now.tv_usec - snmpUpTime.tv_usec) / 10000);
}

/* The time (in hundredths of a second) since the network
 * management portion of the system was last re-initialized.
 */
unsigned char *
var_sysUpTime(int *var_len, Access_rec *access_id,
              WRITE_METHOD *write_method)
{
  *write_method = 0;
  *var_len = sizeof(long);
  *(long*)outputBuffer =  current_sysUpTime();
  return (unsigned char *) outputBuffer;
}

/* The textual identification of the contact person for this
 * this person.  If no contact information is known, the value
 * is the zero-length string.
 */
#ifdef BUILD_SNMP_SET
int write_sysContact(int action,unsigned char *var_val, unsigned char varval_type, int var_val_len,
                     unsigned char *statP, Oid *name, int name_len)
{
   SnmpCfgObject *obj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   switch (action) {
   case RESERVE1:
      if (varval_type != SNMP_STRING)
         return SNMP_ERROR_WRONGTYPE;
      if (var_val_len > MAX_OCTSTR_LEN)
         return SNMP_ERROR_WRONGLENGTH;
      break;
   case RESERVE2:
      break;
   case COMMIT:
      if (cmsObj_get(MDMOID_SNMP_CFG, &iidStack, 0, (void **) &obj) == CMSRET_SUCCESS)
      {
         if (cmsUtl_strncmp(obj->sysContact,var_val,var_val_len) != 0)
         {
            cmsMem_free(obj->sysContact);
            obj->sysContact = cmsMem_strndup(var_val,var_val_len);
            cmsObj_set((void*)obj,&iidStack);
            cmsMgm_saveConfigToFlash();
            cmsLog_debug("set SysContact to %s (len %d) and saved)",obj->sysContact,
                         var_val_len);
         }
         cmsObj_free((void **) &obj);
      }
      else
      {
         cmsLog_error("Unable to get MDMOID_SNMP_CFG");
      }
      break;
   case FREE:
      break;
   }
   return SNMP_ERROR_NOERROR;
}
#endif

unsigned char *
var_sysContact(int *var_len, Access_rec *access_id,
               WRITE_METHOD *write_method)
{
   SnmpCfgObject *obj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   char *pVar = strNotInitialized;

#ifdef BUILD_SNMP_SET
   *write_method = (WRITE_METHOD)write_sysContact;
#else
   *write_method = 0;
#endif

   if (cmsObj_get(MDMOID_SNMP_CFG, &iidStack, 0, (void **) &obj) == CMSRET_SUCCESS)
   {
      if (obj->sysContact)
      {
         pVar = obj->sysContact;
      }
      *var_len = strlen(pVar);
      strcpy(outputBuffer, pVar);
      cmsObj_free((void **) &obj);
   }
   else
   {
      cmsLog_error("Unable to get MDMOID_SNMP_CFG");
      *var_len = 0;
   }
   return (outputBuffer);
}

/* An administratively-assigned name for this managed node.
 * By convention, this is the node's fully-qualified domain
 * name.  If the name is unknown, the value is the zero-length
 * string.
 */
#ifdef BUILD_SNMP_SET
int write_sysName(int action, unsigned char *var_val, unsigned char varval_type, int var_val_len,
                  unsigned char *statP, Oid *name, int name_len)
{
   SnmpCfgObject *obj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   switch (action) 
   {
   case RESERVE1:
      if (varval_type != SNMP_STRING)
         return SNMP_ERROR_WRONGTYPE;
      if (var_val_len > MAX_OCTSTR_LEN)
         return SNMP_ERROR_WRONGLENGTH;
      break;
   case RESERVE2:
      break;
   case COMMIT:
      if (cmsObj_get(MDMOID_SNMP_CFG, &iidStack, 0, (void **) &obj) == CMSRET_SUCCESS)
      {
         if (cmsUtl_strncmp(obj->sysName,var_val,var_val_len) != 0)
         {
            cmsMem_free(obj->sysName);
            obj->sysName = cmsMem_strndup(var_val,var_val_len);
            cmsObj_set((void*)obj,&iidStack);
            cmsMgm_saveConfigToFlash();
            cmsLog_debug("set SysName to %s (len %d) and saved",obj->sysName,
                         var_val_len);
         }
         cmsObj_free((void **) &obj);
      }
      else
      {
         cmsLog_error("Unable to get MDMOID_SNMP_CFG");
      }
      break;
    case FREE:
       break;
   }
   return SNMP_ERROR_NOERROR;
}
#endif /* BUILD_SNMP_SET */

unsigned char *
var_sysName(int *var_len, Access_rec *access_id,
            WRITE_METHOD *write_method)
{
   SnmpCfgObject *obj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   char *pVar = strNotInitialized;

#ifdef BUILD_SNMP_SET
   *write_method = (WRITE_METHOD)write_sysName;
#else
   *write_method = 0;
#endif

   if (cmsObj_get(MDMOID_SNMP_CFG, &iidStack, 0, (void **) &obj) == CMSRET_SUCCESS)
   {
      if (obj->sysName)
      {
         pVar = obj->sysName;
      }
      *var_len = strlen(pVar);
      strcpy(outputBuffer, pVar);
      cmsObj_free((void **) &obj);
   }
   else
   {
      cmsLog_error("Unable to get MDMOID_SNMP_CFG");
      *var_len = 0;
   }

   return (outputBuffer);
}


/* The physical location of this node (e.g., `telephone
 * closet, 3rd floor').  If the location is unknown, the value
 * is the zero-length string.
 */
#ifdef BUILD_SNMP_SET
int	write_sysLocation(int action,
	unsigned char *var_val, unsigned char varval_type, int var_val_len,
	unsigned char *statP, Oid *name, int name_len)
{
   SnmpCfgObject *obj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   switch (action) 
   {
   case RESERVE1:
      if (varval_type != SNMP_STRING)
         return SNMP_ERROR_WRONGTYPE;
      if (var_val_len > MAX_OCTSTR_LEN)
         return SNMP_ERROR_WRONGLENGTH;
      break;
   case RESERVE2:
      break;
   case COMMIT:
      if (cmsObj_get(MDMOID_SNMP_CFG, &iidStack, 0, (void **) &obj) == CMSRET_SUCCESS)
      {
         if (cmsUtl_strncmp(obj->sysLocation,var_val,var_val_len) != 0)
         {
            cmsMem_free(obj->sysLocation);
            obj->sysLocation = cmsMem_strndup(var_val,var_val_len);
            cmsObj_set((void*)obj,&iidStack);
            cmsMgm_saveConfigToFlash();
            cmsLog_debug("set SysContact to %s (len %d) and saved)",obj->sysLocation,
                         var_val_len);
         }
         cmsObj_free((void **) &obj);
      }
      else
      {
         cmsLog_error("Unable to get MDMOID_SNMP_CFG");
      }

      break;
   case FREE:
      break;
   }
   return SNMP_ERROR_NOERROR;
}
#endif /* BUILD_SNMP_SET */

unsigned char *
var_sysLocation(int *var_len, Access_rec *access_id,
        WRITE_METHOD *write_method)
{
   SnmpCfgObject *obj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   char *pVar = strNotInitialized;

#ifdef BUILD_SNMP_SET
  *write_method = (WRITE_METHOD)write_sysLocation;
#else
  *write_method = 0;
#endif

   if (cmsObj_get(MDMOID_SNMP_CFG, &iidStack, 0, (void **) &obj) == CMSRET_SUCCESS)
   {
      if (obj->sysLocation)
      {
         pVar = obj->sysLocation;
      }
      *var_len = strlen(pVar);
      strcpy(outputBuffer, pVar);
      cmsObj_free((void **) &obj);
   }
   else
   {
      cmsLog_error("Unable to get MDMOID_SNMP_CFG");
      *var_len = 0;
   }
   return (outputBuffer);
}

/* A value which indicates the set of services that this
 * entity may potentially offers.  The value is a sum.  This
 * sum initially takes the value zero, Then, for each layer, L,
 * in the range 1 through 7, that this node performs
 * transactions for, 2 raised to (L - 1) is added to the sum.
 * For example, a node which performs only routing functions
 * would have a value of 4 (2^(3-1)).  In contrast, a node
 * which is a host offering application services would have a
 * value of 72 (2^(4-1) + 2^(7-
 */

unsigned char *
var_sysServices(int *var_len, Access_rec *access_id,
        WRITE_METHOD *write_method)
{
  *var_len = sizeof(long);
  *(long*)outputBuffer =  SYS_SERVICES;
  return (unsigned char *) outputBuffer;
}

/* The value of sysUpTime at the time of the most recent
 * change in state or value of any instance of sysORID.
 */

unsigned char *
var_sysORLastChange(int *var_len, Access_rec *access_id,
        WRITE_METHOD *write_method)
{
   *write_method = 0;
   *var_len = sizeof(long);
   *(long*)outputBuffer =  current_sysUpTime();
   return (outputBuffer);
}

/* An entry (conceptual row) in the sysORTable.
 */

unsigned char *
var_sysOREntry(int *var_len,
        Oid *newoid, Oid *reqoid, int searchType,
        Access_rec *access_id, WRITE_METHOD *write_method)
{
  /* Last subOID of COLUMNAR OID is column */
  int     column = newoid->name[(newoid->namelen - 1)];
  int     sysORIndex = newoid->namelen++;
  /* Variables defined */
  int     result;
  struct sysOREntry_struct *current = &snmpV2_value;

  while (current) { 
    newoid->name[ sysORIndex ] = current->index;
    result = compare(reqoid, newoid);
    if (((searchType == EXACT) && (result == 0)) ||
        ((searchType == NEXT) && (result < 0))) {
      break;
    }
    current = current->next;
  }
  if (current == NULL) return(NULL);

  *write_method = 0;

  switch (column) {
  case I_sysORIndex:
    return (unsigned char *) NO_MIBINSTANCE;
  case I_sysORID:
    *var_len = current->oid_value.namelen * sizeof(oid);
    return (unsigned char *) &(current->oid_value.name);
  case I_sysORDescr:
    *var_len = strlen(current->descr);
    return (unsigned char *) current->descr;
  case I_sysORUpTime:
    *var_len = sizeof(unsigned long);
    return (unsigned char *) &(current->lastChange);
  default:
    return NO_MIBINSTANCE;
  }
}

#ifdef BUILD_SNMP_MIB
/* The total number of messages delivered to the SNMP entity
 * from the transport service.
 */

unsigned char *
var_snmpInPkts(int *var_len, Access_rec *access_id,
        WRITE_METHOD *write_method)
{
    extern long snmpInPkts_instance;
    *write_method = 0;
    *var_len = sizeof(long);
    return (unsigned char *) &snmpInPkts_instance;
}

/* The total number of SNMP messages which were delivered to
 * the SNMP entity and were for an unsupported SNMP version.
 */

unsigned char *
var_snmpInBadVersions(int *var_len, Access_rec *access_id,
        WRITE_METHOD *write_method)
{
    extern long snmpInBadVersions_instance;
    *write_method = 0;
    *var_len = sizeof(long);
    return (unsigned char *) &snmpInBadVersions_instance;
}

/* The total number of SNMP messages delivered to the SNMP
 * entity which used a SNMP community name not known to said
 * entity.
 */

unsigned char *
var_snmpInBadCommunityNames(int *var_len, Access_rec *access_id,
        WRITE_METHOD *write_method)
{
    extern long snmpInBadCommunityNames_instance;
    *write_method = 0;
    *var_len = sizeof(long);
    return (unsigned char *) &snmpInBadCommunityNames_instance;
}

/* The total number of SNMP messages delivered to the SNMP
 * entity which represented an SNMP operation which was not
 * allowed by the SNMP community named in the message.
 */

unsigned char *
var_snmpInBadCommunityUses(int *var_len, Access_rec *access_id,
        WRITE_METHOD *write_method)
{
    extern long snmpInBadCommunityUses_instance;
    *write_method = 0;
    *var_len = sizeof(long);
    return (unsigned char *) &snmpInBadCommunityUses_instance;
}

/* The total number of ASN.1 or BER errors encountered by the
 * SNMP entity when decoding received SNMP messages.
 */

unsigned char *
var_snmpInASNParseErrs(int *var_len, Access_rec *access_id,
        WRITE_METHOD *write_method)
{
    extern long snmpInASNParseErrs_instance;
    *write_method = 0;
    *var_len = sizeof(long);
    return (unsigned char *) &snmpInASNParseErrs_instance;
}

/* Indicates whether the SNMP entity is permitted to generate
 * authenticationFailure traps.  The value of this object
 * overrides any configuration information; as such, it
 * provides a means whereby all authenticationFailure traps may
 * be disabled.
 * 
 * Note that it is strongly recommended that this object be
 * stored in non-volatile memory so that it remains constant
 * across re-initializations of the network management system.
 */
/*
int	write_snmpEnableAuthenTraps(int action,
	unsigned char *var_val, unsigned char varval_type, int var_val_len,
	unsigned char *statP, oid *name, int name_len)
{
    switch (action) {
    case RESERVE1:
    case RESERVE2:
    case COMMIT:
    case FREE:
    }
}
*/

unsigned char *
var_snmpEnableAuthenTraps(int *var_len, Access_rec *access_id,
        WRITE_METHOD *write_method)
{
    extern long snmpEnableAuthenTraps_instance;
    /* Set write-function (uncomment if you want to implement it)  */
    /* *write_method = &write_snmpEnableAuthenTraps(); */
    snmpEnableAuthenTraps_instance = 2;
    *write_method = 0;
    *var_len = sizeof(long);
    return (unsigned char *) &snmpEnableAuthenTraps_instance;
}

/* The total number of GetRequest-PDUs, GetNextRequest-PDUs,
 * GetBulkRequest-PDUs, SetRequest-PDUs, and InformRequest-PDUs
 * delivered to the SNMP entity which were silently dropped
 * because the size of a reply containing an alternate
 * Response-PDU with an empty variable-bindings field was
 * greater than either a local constraint or the maximum
 * message size associated with the originator of the request.
 */

unsigned char *
var_snmpSilentDrops(int *var_len, Access_rec *access_id,
        WRITE_METHOD *write_method)
{
    extern long snmpSilentDrops_instance;
    *write_method = 0;
    *var_len = sizeof(long);
    return (unsigned char *) &snmpSilentDrops_instance;
}

/* The total number of GetRequest-PDUs, GetNextRequest-PDUs,
 * GetBulkRequest-PDUs, SetRequest-PDUs, and InformRequest-PDUs
 * delivered to the SNMP entity which were silently dropped
 * because the transmission of the (possibly translated)
 * time-out) such that no Response-PDU could be returned.
 */

unsigned char *
var_snmpProxyDrops(int *var_len, Access_rec *access_id,
        WRITE_METHOD *write_method)
{
    extern long snmpProxyDrops_instance;
    *write_method = 0;
    *var_len = sizeof(long);
    return (unsigned char *) &snmpProxyDrops_instance;
}

/* An advisory lock used to allow several cooperating SNMPv2
 * use of the SNMPv2 set operation.
 * 
 * This object is used for coarse-grain coordination.  To
 * achieve fine-grain coordination, one or more similar objects
 * might be defined within each MIB group, as appropriate.
 */
/*
int	write_snmpSetSerialNo(int action,
	unsigned char *var_val, unsigned char varval_type, int var_val_len,
	unsigned char *statP, oid *name, int name_len)
{
    switch (action) {
    case RESERVE1:
    case RESERVE2:
    case COMMIT:
    case FREE:
    }
}
*/

unsigned char *
var_snmpSetSerialNo(int *var_len, Access_rec *access_id,
        WRITE_METHOD *write_method)
{
    /* Add value computations */

    /* Set write-function (uncomment if you want to implement it)  */
    /* *write_method = &write_snmpSetSerialNo(); */
    /* Set size (in bytes) and return address of the variable */
    *var_len = sizeof(long);
    return (unsigned char *) NO_MIBINSTANCE;
}
#endif /* BUILD_SNMP_MIB */

static oid system_oid[] = { O_system };
static Object system_variables[] = {
    { SNMP_STRING, (RONLY| SCALAR), (void*)var_sysDescr,
                 {2, { I_sysDescr, 0 }}},
    { SNMP_OBJID, (RONLY| SCALAR), (void*)var_sysObjectID,
                 {2, { I_sysObjectID, 0 }}},
    { SNMP_TIMETICKS, (RONLY| SCALAR), (void*)var_sysUpTime,
                 {2, { I_sysUpTime, 0 }}},
    { SNMP_STRING, (RWRITE| SCALAR), (void*)var_sysContact,
                 {2, { I_sysContact, 0 }}},
    { SNMP_STRING, (RWRITE| SCALAR), (void*)var_sysName,
                 {2, { I_sysName, 0 }}},
    { SNMP_STRING, (RWRITE| SCALAR), (void*)var_sysLocation,
                 {2, { I_sysLocation, 0 }}},
    { SNMP_INTEGER, (RONLY| SCALAR), (void*)var_sysServices,
                 {2, { I_sysServices, 0 }}},
    { SNMP_TIMETICKS, (RONLY| SCALAR), (void*)var_sysORLastChange,
                 {2, { I_sysORLastChange, 0 }}},
    { SNMP_OBJID, (RONLY| COLUMN), (void*)var_sysOREntry,
                {3, { I_sysORTable, I_sysOREntry, I_sysORID }}},
    { SNMP_STRING, (RONLY| COLUMN), (void*)var_sysOREntry,
                {3, { I_sysORTable, I_sysOREntry, I_sysORDescr }}},
    { SNMP_TIMETICKS, (RONLY| COLUMN), (void*)var_sysOREntry,
                {3, { I_sysORTable, I_sysOREntry, I_sysORUpTime }}},
    { (int)NULL }
    };
static SubTree system_tree =  { NULL, system_variables,
	        (sizeof(system_oid)/sizeof(oid)), system_oid};

#ifdef BUILD_SNMP_MIB
static oid snmp_oid[] = { O_snmp };
static Object snmp_variables[] = {
    { SNMP_COUNTER, (RONLY| SCALAR), (void*)var_snmpInPkts,
                 {2, { I_snmpInPkts, 0 }}},
    { SNMP_COUNTER, (RONLY| SCALAR), (void*)var_snmpInBadVersions,
                 {2, { I_snmpInBadVersions, 0 }}},
    { SNMP_COUNTER, (RONLY| SCALAR), (void*)var_snmpInBadCommunityNames,
                 {2, { I_snmpInBadCommunityNames, 0 }}},
    { SNMP_COUNTER, (RONLY| SCALAR), (void*)var_snmpInBadCommunityUses,
                 {2, { I_snmpInBadCommunityUses, 0 }}},
    { SNMP_COUNTER, (RONLY| SCALAR), (void*)var_snmpInASNParseErrs,
                 {2, { I_snmpInASNParseErrs, 0 }}},
    { SNMP_INTEGER, (RWRITE| SCALAR), (void*)var_snmpEnableAuthenTraps,
                 {2, { I_snmpEnableAuthenTraps, 0 }}},
    { SNMP_COUNTER, (RONLY| SCALAR), (void*)var_snmpSilentDrops,
                 {2, { I_snmpSilentDrops, 0 }}},
    { SNMP_COUNTER, (RONLY| SCALAR), (void*)var_snmpProxyDrops,
                 {2, { I_snmpProxyDrops, 0 }}},
    { (int)NULL }
    };
static SubTree snmp_tree =  { NULL, snmp_variables,
	        (sizeof(snmp_oid)/sizeof(oid)), snmp_oid};

/*
 * static oid snmpSet_oid[] = { O_snmpSet };
 * static Object snmpSet_variables[] = {
 *     { SNMP_INTEGER, (RWRITE| SCALAR), (void*)var_snmpSetSerialNo,
 *                  {2, { I_snmpSetSerialNo, 0 }}},
 *     { NULL }
 *     };
 * static SubTree snmpSet_tree =  { NULL, snmpSet_variables,
 *                  (sizeof(snmpSet_oid)/sizeof(oid)), snmpSet_oid};
 */
#endif /* BUILD_SNMP_MIB */

/* This is the MIB registration function. This should be called */
/* within the init_SNMPv2_MIB-function */
void register_subtrees_of_SNMPv2_MIB()
{
    insert_group_in_mib(&system_tree);
#ifdef BUILD_SNMP_MIB 
    insert_group_in_mib(&snmp_tree);
/*  insert_group_in_mib(&snmpSet_tree); */
#endif /* BUILD_SNMP_MIB */
}

