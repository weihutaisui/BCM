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
#include "atmMib.h"
#include "atmMibHelper.h"
#include "atmMibDefs.h"
#include "cpeDslMibHelper.h"
#include "ifcdefs.h"
#ifdef BUILD_SNMP_DEBUG
#include "snmpDebug.h"
extern SNMP_DEBUG_FLAG snmpDebug;
#endif

#if defined (BUILD_SNMP_ATM_MIB) || defined(CPE_DSL_MIB)

extern ATM_INFO_LIST atmList;

void atmMibFree(void)
{
  ATM_VCL_TABLE *ptr;

  while (atmList.head) {
    ptr = atmList.head;
    atmList.head = atmList.head->next;
    free(ptr);
  }
  atmList.count = 0;
}
#endif /*  (BUILD_SNMP_ATM_MIB) || defined(CPE_DSL_MIB) */

#ifdef BUILD_SNMP_ATM_MIB
/* This list contains ATM interface configuration
 * parameters and state variables and is indexed
 * by ifIndex values of ATM interfaces.
 */
/*
int	write_atmInterfaceMaxVpcs(int action,
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
var_atmInterfaceConfEntry(int *var_len,
        Oid *newoid, Oid *reqoid, int searchType,
        snmp_info_t *mesg, int (**write_method)())
{
/* Last subOID of COLUMNAR OID is column */
int	column = newoid->name[(newoid->namelen - 1)];
int	result;

    while (0 /* Test whether the entry has been found */) {
        /* Add indexes for the entry to OID newname */
        /* Determine whether it is the requested OID  */
        result = compare(reqoid, newoid);
        if (((searchType == EXACT) && (result != 0)) ||
                ((searchType == NEXT) && (result >= 0))) {
            return NO_MIBINSTANCE;
        }
    }
    /* Place here the computation functionality */

    /* Set write-function */
    *write_method = 0;
    /* Set size (in bytes) and return address of the variable */
    *var_len = sizeof(long);
    switch (column) {
	case I_atmInterfaceMaxVpcs:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmInterfaceMaxVccs:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmInterfaceConfVpcs:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmInterfaceConfVccs:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmInterfaceMaxActiveVpiBits:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmInterfaceMaxActiveVciBits:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmInterfaceIlmiVpi:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmInterfaceIlmiVci:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmInterfaceAddressType:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmInterfaceAdminAddress:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmInterfaceMyNeighborIpAddress:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmInterfaceMyNeighborIfName:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmInterfaceCurrentMaxVpiBits:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmInterfaceCurrentMaxVciBits:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmInterfaceSubscrAddress:
	    return (unsigned char *) NO_MIBINSTANCE;
        default:
            return NO_MIBINSTANCE;
    }
}

/* This list contains DS3 PLCP parameters and
 * state variables at the ATM interface and is
 * indexed by the ifIndex value of the ATM interface.
 */

unsigned char *
var_atmInterfaceDs3PlcpEntry(int *var_len,
        Oid *newoid, Oid *reqoid, int searchType,
        snmp_info_t *mesg, int (**write_method)())
{
/* Last subOID of COLUMNAR OID is column */
int	column = newoid->name[(newoid->namelen - 1)];
int	result;

    while (0 /* Test whether the entry has been found */) {
        /* Add indexes for the entry to OID newname */
        /* Determine whether it is the requested OID  */
        result = compare(reqoid, newoid);
        if (((searchType == EXACT) && (result != 0)) ||
                ((searchType == NEXT) && (result >= 0))) {
            return NO_MIBINSTANCE;
        }
    }
    /* Place here the computation functionality */

    /* Set write-function */
    *write_method = 0;
    /* Set size (in bytes) and return address of the variable */
    *var_len = sizeof(long);
    switch (column) {
	case I_atmInterfaceDs3PlcpSEFSs:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmInterfaceDs3PlcpAlarmState:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmInterfaceDs3PlcpUASs:
	    return (unsigned char *) NO_MIBINSTANCE;
        default:
            return NO_MIBINSTANCE;
    }
}

/* This list contains TC Sublayer parameters
 * and state variables at the ATM interface and is
 * indexed by the ifIndex value of the ATM interface.
 */

unsigned char *
var_atmInterfaceTCEntry(int *var_len,
        Oid *newoid, Oid *reqoid, int searchType,
        snmp_info_t *mesg, int (**write_method)())
{
/* Last subOID of COLUMNAR OID is column */
int	column = newoid->name[(newoid->namelen - 1)];
int	result;

    while (0 /* Test whether the entry has been found */) {
        /* Add indexes for the entry to OID newname */
        /* Determine whether it is the requested OID  */
        result = compare(reqoid, newoid);
        if (((searchType == EXACT) && (result != 0)) ||
                ((searchType == NEXT) && (result >= 0))) {
            return NO_MIBINSTANCE;
        }
    }
    /* Place here the computation functionality */

    /* Set write-function */
    *write_method = 0;
    /* Set size (in bytes) and return address of the variable */
    *var_len = sizeof(long);
    switch (column) {
	case I_atmInterfaceOCDEvents:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmInterfaceTCAlarmState:
	    return (unsigned char *) NO_MIBINSTANCE;
        default:
            return NO_MIBINSTANCE;
    }
}

/* This list contains ATM traffic descriptor
 * type and the associated parameters.
 */

unsigned char *
var_atmTrafficDescrParamEntry(int *var_len,
        Oid *newoid, Oid *reqoid, int searchType,
        snmp_info_t *mesg, int (**write_method)())
{
/* Last subOID of COLUMNAR OID is column */
int	column = newoid->name[(newoid->namelen - 1)];
int	result;

    while (0 /* Test whether the entry has been found */) {
        /* Add indexes for the entry to OID newname */
        /* Determine whether it is the requested OID  */
        result = compare(reqoid, newoid);
        if (((searchType == EXACT) && (result != 0)) ||
                ((searchType == NEXT) && (result >= 0))) {
            return NO_MIBINSTANCE;
        }
    }
    /* Place here the computation functionality */

    /* Set write-function */
    *write_method = 0;
    /* Set size (in bytes) and return address of the variable */
    *var_len = sizeof(long);
    switch (column) {
	case I_atmTrafficDescrParamIndex:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmTrafficDescrType:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmTrafficDescrParam1:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmTrafficDescrParam2:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmTrafficDescrParam3:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmTrafficDescrParam4:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmTrafficDescrParam5:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmTrafficQoSClass:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmTrafficDescrRowStatus:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmServiceCategory:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmTrafficFrameDiscard:
	    return (unsigned char *) NO_MIBINSTANCE;
        default:
            return NO_MIBINSTANCE;
    }
}

/* An entry in the VPL table.  This entry is
 * used to model a bi-directional VPL.
 * To create a VPL at an ATM interface,
 * either of the following procedures are used:
 * 
 * Negotiated VPL establishment
 * 
 * (1) The management application creates
 * a VPL entry in the atmVplTable
 * by setting atmVplRowStatus to createAndWait(5).
 * This may fail for the following reasons:
 * - The selected VPI value is unavailable,
 * - The selected VPI value is in use.
 * Otherwise, the agent creates a row and
 * 
 */

unsigned char *
var_atmVplEntry(int *var_len,
        Oid *newoid, Oid *reqoid, int searchType,
        snmp_info_t *mesg, int (**write_method)())
{
/* Last subOID of COLUMNAR OID is column */
int	column = newoid->name[(newoid->namelen - 1)];
int	result;

    while (0 /* Test whether the entry has been found */) {
        /* Add indexes for the entry to OID newname */
        /* Determine whether it is the requested OID  */
        result = compare(reqoid, newoid);
        if (((searchType == EXACT) && (result != 0)) ||
                ((searchType == NEXT) && (result >= 0))) {
            return NO_MIBINSTANCE;
        }
    }
    /* Place here the computation functionality */

    /* Set write-function */
    *write_method = 0;
    /* Set size (in bytes) and return address of the variable */
    *var_len = sizeof(long);
    switch (column) {
	case I_atmVplVpi:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmVplAdminStatus:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmVplOperStatus:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmVplLastChange:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmVplReceiveTrafficDescrIndex:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmVplTransmitTrafficDescrIndex:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmVplCrossConnectIdentifier:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmVplRowStatus:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmVplCastType:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmVplConnKind:
	    return (unsigned char *) NO_MIBINSTANCE;
        default:
            return NO_MIBINSTANCE;
    }
}
#endif /* BUILD_SNMP_ATM_MIB */

#if defined(BUILD_SNMP_ATM_MIB) || defined(CPE_DSL_MIB)
//static ATM_VCL_TABLE *pAtmVclTable=NULL;
int write_atmVclEntry(int action,unsigned char *var_val, unsigned char varval_type, int var_val_len,
                      unsigned char *statP, Oid *name, int name_len)
{
    int index, value, column;
    int namelen;
    int vpi, vci;
    int ret;
    static ATM_VCL_TABLE vclEntry;
    static int start = 1;
    static int willCreate = 0; 
    static int created = 0;
    namelen = (int)name->namelen;
    /* column, ifIndex, vpiIndex, vciIndex, 0 ==> namelen at 0, column is at (namelen-4)*/
    column =  (int)name->name[namelen-4];

    /* first time, init entry */
    if (start) {
      vclSetDefValues(&vclEntry);
      /* for cpeMib these are not required for setting, even though they should be */
      /* set them to default so they would be set correctly if NMS doesn't set them */
      vclEntry.atmVclAdminStatus = ATM_VCL_ADMIN_STATUS_UP;
      vclEntry.atmVclReceiveTrafficDescrIndex = ATM_VCL_TD_INDEX_DEFAULT;
      vclEntry.atmVclTransmitTrafficDescrIndex = ATM_VCL_TD_INDEX_DEFAULT;
      vclEntry.atmVccAal5EncapsType = ATM_VCC_ENCAP_LLC;
      start = 0;
    }
    value = (int)*var_val;

    switch (action) 
      {
      case RESERVE1:
        if (varval_type != SNMP_INTEGER)
          return SNMP_ERROR_WRONGTYPE;
        if (var_val_len > sizeof(int)) 
          return SNMP_ERROR_WRONGLENGTH;
        switch (column) 
          {
          case I_atmVclVpi:
          case I_atmVclVci:
            return SNMP_ERROR_NOACCESS;
          case I_atmVclAdminStatus:
            if ((value != ATM_ADMIN_UP ) && (value != ATM_ADMIN_DOWN)) {
              return SNMP_ERROR_WRONGVALUE;
            }
            break;
          case I_atmVclOperStatus:
            return SNMP_ERROR_NOTWRITABLE;
          case I_atmVclLastChange:
            return SNMP_ERROR_NOTWRITABLE;
          case I_atmVclReceiveTrafficDescrIndex:
          case I_atmVclTransmitTrafficDescrIndex:
            /* in our modem, TX and RX TD are the same, index is always 1 based */
            if (value < 1 ) {
              return SNMP_ERROR_WRONGVALUE;
            }
            break;
          case I_atmVccAalType:
            /* other values not supported */
            if ((value != ATM_VCL_AALTYPE_AAL5) && (value != ATM_VCL_AALTYPE_AAL2))
              return SNMP_ERROR_WRONGVALUE;
            break;
          case I_atmVccAal5CpcsTransmitSduSize:
          case I_atmVccAal5CpcsReceiveSduSize:
            if ((value < 1) || (value > ATM_VCL_SDUSIZE)) {
              return SNMP_ERROR_WRONGVALUE;
            }
            break;
          case I_atmVccAal5EncapsType:
            if ((value != ATM_VCC_ENCAP_ROUTED) && (value != ATM_VCC_ENCAP_BR_8023) &&
                (value != ATM_VCC_ENCAP_LLC) && (value != ATM_VCC_ENCAP_OTHER) &&
                (value != ATM_VCC_ENCAP_UNKNOWN)) {
              return SNMP_ERROR_WRONGVALUE;
            }
            break;
          case I_atmVclCrossConnectIdentifier:
            return SNMP_ERROR_NOTWRITABLE;
          case I_atmVclRowStatus:
            if ((value == ATM_ROWSTATUS_NOT_READY) ||
                ((value < ATM_ROWSTATUS_ACTIVE) &&
                 (value > ATM_ROWSTATUS_CREATE_DESTROY))) {
              return SNMP_ERROR_WRONGVALUE;
            }
            if ((value == ATM_ROWSTATUS_CREATE_GO) ||
                (value == ATM_ROWSTATUS_CREATE_WAIT)) {
              willCreate = value;
            }
            break;
          case I_atmVclCastType:
            if ((value < ATM_CAST_TYPE_P2P) &&
                (value > ATM_CAST_TYPE_P2MPLEAVE)) {
              return SNMP_ERROR_WRONGVALUE;
            }
            break;
          case I_atmVclConnKind:
            if ((value < ATM_CONN_KIND_PVC) &&
                (value > ATM_CONN_KIND_SPVC_TARGET)) {
              return SNMP_ERROR_WRONGVALUE;
            }
            break;
          default:
            return SNMP_ERROR_NOTWRITABLE;
          } /* switch */
        break;
      case RESERVE2:
        break;
      case COMMIT:
        /* ifIndex, vpiIndex, vciIndex, column, 0 ==> namelen at 0, column is at (namelen-3)*/
        index = (int)name->name[namelen-3];
        vpi = (int)name->name[namelen-2];
        vci = (int)name->name[namelen-1];
        if ((index == 0) || ((vpi < 0) && (vpi > 255)) || ((vci < 32) && (vci > 65535))) {
          return SNMP_ERROR_WRONGVALUE;
        }

        /* this is to resolve problem of CPE mib; vcc needs to be create for PVC
           entry configuration; due to the order of objects coming in, virtual
           PVC is not created before PVC entry configuration */
        if (willCreate) {
          vclEntry.atmVclRowStatus = willCreate;
          willCreate = 0;
          vclEntry.atmVclVpi = vpi;
          vclEntry.atmVclVci = vci;
          if ((ret = vclAddEntry(&vclEntry)) == -1) {
#ifdef BUILD_SNMP_DEBUG
            if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_ATM) == SNMP_DEBUG_LEVEL_MIB_ATM)
              printf("write_atmVclEntry(Precreate VCC FAILED): adding vpi/vci %d/%d return nonzero\n",
                     vpi,vci);
#endif
            return SNMP_ERROR_COMMITFAILED;
          }
          else if (ret == 0) {
            created = 1;
          }
        }

        switch (column) 
          {
          case I_atmVclAdminStatus:
#ifdef BUILD_SNMP_DEBUG
            if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_ATM) == SNMP_DEBUG_LEVEL_MIB_ATM)
              printf("write_atmVclEntry(): setting AdminStatus %d for vpi/vci %d/%d\n",
                     value,vpi,vci);
#endif
            ret = vclSetAdminStatus(0,vpi,vci,value);
            if (ret == -1)
              return SNMP_ERROR_COMMITFAILED;
            else if (ret) /* to be set later */
              vclEntry.atmVclAdminStatus = value;
            break;
          case I_atmVclReceiveTrafficDescrIndex:
          case I_atmVclTransmitTrafficDescrIndex:
#ifdef BUILD_SNMP_DEBUG
            if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_ATM) == SNMP_DEBUG_LEVEL_MIB_ATM)
              printf("write_atmVclEntry(): setting TdIndex %d for vpi/vci %d/%d\n",
                     value,vpi,vci);
#endif
            ret = vclSetTdIndex(ATM_PORT_NUM,vpi,vci,value);
            if (ret == -1)
              return SNMP_ERROR_COMMITFAILED;
            else if (ret) {
              vclEntry.atmVclReceiveTrafficDescrIndex = value;
              vclEntry.atmVclTransmitTrafficDescrIndex = value;
            }
            break;
          case I_atmVccAalType:
#ifdef BUILD_SNMP_DEBUG
            if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_ATM) == SNMP_DEBUG_LEVEL_MIB_ATM)
              printf("write_atmVclEntry(): setting aalType %d for vpi/vci %d/%d\n",
                     value,vpi,vci);
#endif
            ret = vclSetAalType(ATM_PORT_NUM,vpi,vci,value);
            if (ret == -1)
              return SNMP_ERROR_COMMITFAILED;
            else if (ret) 
              vclEntry.atmVccAalType = value;
            break;
          case I_atmVccAal5CpcsTransmitSduSize: 
#if 0 /* not supported now */
            ret = vclSetTxCpcsSdu(ATM_PORT_NUM,vpi,vci,value);
            if (ret == -1)
              return SNMP_ERROR_COMMITFAILED;
            else if (ret) 
              vclEntry.atmVccAal5CpcsTransmitSduSize = value;
            break;
          case I_atmVccAal5CpcsReceiveSduSize:
            ret = vclSetRxCpcsSdu(ATM_PORT_NUM,vpi,vci,value);
            if (ret == -1)
              return SNMP_ERROR_COMMITFAILED;
            else if (ret) 
              vclEntry.atmVccAal5CpcsReceiveSduSize = value;
#endif 
            break;
          case I_atmVccAal5EncapsType:
#ifdef BUILD_SNMP_DEBUG
            if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_ATM) == SNMP_DEBUG_LEVEL_MIB_ATM)
              printf("write_atmVclEntry(): setting AalEncapType %d for vpi/vci %d/%d\n",
                     value,vpi,vci);
#endif
            ret = vclSetAal5EncapType(ATM_PORT_NUM,vpi,vci,value);
            if (ret == -1)
              return SNMP_ERROR_COMMITFAILED;
            else if (ret) 
              vclEntry.atmVccAal5EncapsType = value;

            break;
          case I_atmVclRowStatus:
            start = 1;
#ifdef BUILD_SNMP_DEBUG
            if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_ATM) == SNMP_DEBUG_LEVEL_MIB_ATM)
              printf("write_atmVclEntry(): setting rowStatus %d for vpi/vci %d/%d\n",
                     value,vpi,vci);
#endif
            if (value == ATM_ROWSTATUS_CREATE_DESTROY) {
#ifdef BUILD_SNMP_DEBUG
            if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_ATM) == SNMP_DEBUG_LEVEL_MIB_ATM)
              printf("write_atmVclEntry(): deleting vpi/vci %d/%d\n",
                     vpi,vci);
#endif
              if (vclDeleteEntry(ATM_PORT_NUM,vpi,vci) != 0) {
                return SNMP_ERROR_COMMITFAILED;
              }
              else 
                return SNMP_ERROR_NOERROR;
            }
            else {
              if (!created) {
                vclEntry.atmVclRowStatus = value;
                vclEntry.atmVclVpi = vpi;
                vclEntry.atmVclVci = vci;
                ret = vclAddEntry(&vclEntry);
		if ((ret == -1) || ((ret == 1) &&
		    /* created, but special VCC */
				    (vpi != ATM_AUX_CHANNEL_VPI) &&
				    (vci != ATM_AUX_CHANNEL_VCI))) {
#ifdef BUILD_SNMP_DEBUG
                  if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_ATM) == SNMP_DEBUG_LEVEL_MIB_ATM)
                    printf("write_atmVclEntry(ADD FAILED): adding vpi/vci %d/%d return nonzero\n",
                           vpi,vci);
#endif
                  return SNMP_ERROR_COMMITFAILED;
                }
              }
            }
            break;
          case I_atmVclCastType:
            vclEntry.atmVclCastType = value;
            break;
          case I_atmVclConnKind:
            vclEntry.atmVclConnKind = value;
            break;
          } /* switch column */
        break;
      case FREE:
        vpi = (int)name->name[namelen-2];
        vci = (int)name->name[namelen-1];
#ifdef BUILD_SNMP_DEBUG
        if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_ATM) == SNMP_DEBUG_LEVEL_MIB_ATM)
          printf("write_atmVclEntry(): FREE action encountered for vpi/vci %d/%d\n",
                 vpi,vci);
#endif
        if (created) {
          /* remove precreated VCC since set fails */
          vclDeleteEntry(ATM_PORT_NUM,vpi,vci);
          created=0;
          willCreate=0;
        }
        if ((vclEntry.atmVclVpi == vpi) && (vclEntry.atmVclVci == vci)) {
          start = 1;
        }
        break;
      } /* switch action */
    return SNMP_ERROR_NOERROR;
}

/* An entry in the VCL table. This entry is
 * used to model a bi-directional VCL.
 * To create a VCL at an ATM interface,
 * either of the following procedures are used:
 * 
 * Negotiated VCL establishment
 * 
 * (1) The management application creates
 * a VCL entry in the atmVclTable
 * by setting atmVclRowStatus to createAndWait(5).
 * This may fail for the following reasons:
 * - The selected VPI/VCI values are unavailable,
 * - The selected VPI/VCI values are in use.
 * Otherwise, the agent creates a
 */
unsigned char *
var_atmVclEntry(int *var_len, Oid *newoid, Oid *reqoid, int searchType,
                snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  int	column = newoid->name[(newoid->namelen - 1)];
  int	result;
  ATM_VCL_TABLE *pVcc;
  int index, vpiIndex, vciIndex;

  /* Set write-function */
#ifdef CPE_DSL_MIB
  *write_method = (WRITE_METHOD)write_atmVclEntry;
#else
  *write_method = 0;
#endif

  index = newoid->namelen++;
  vpiIndex = newoid->namelen++;
  vciIndex = newoid->namelen++;
  if( mesg->pdutype != SNMP_SET_REQ_PDU ) {
    atmMibFree();

    if ( atmMibHelper_fillAtmVclTable() != 0 ) { 
      return NO_MIBINSTANCE;
    }

    pVcc = atmList.head;

    while (pVcc) {
      newoid->name[index] = pVcc->ifIndex;
      newoid->name[vpiIndex] = pVcc->atmVclVpi;
      newoid->name[vciIndex] = pVcc->atmVclVci;
      result = compare(reqoid, newoid);
      if (((searchType == EXACT) && (result == 0)) ||
          ((searchType == NEXT) && (result < 0)))
        break; /* found */
      pVcc = pVcc->next;
    }
    if (pVcc == NULL) {
      return NO_MIBINSTANCE;
    }

    *var_len = sizeof(long);
    switch (column) {
    case I_atmVclVpi:
      return (unsigned char *)&pVcc->atmVclVpi;
    case I_atmVclVci:
      return (unsigned char *)&pVcc->atmVclVci;
    case I_atmVclAdminStatus:
      return (unsigned char *)&pVcc->atmVclAdminStatus;
    case I_atmVclOperStatus:
      return (unsigned char *)&pVcc->atmVclOperStatus;
    case I_atmVclLastChange:
      return (unsigned char *)&pVcc->atmVclLastChange;
    case I_atmVclReceiveTrafficDescrIndex:
      return (unsigned char *)&pVcc->atmVclReceiveTrafficDescrIndex;
    case I_atmVclTransmitTrafficDescrIndex:
      return (unsigned char *)&pVcc->atmVclTransmitTrafficDescrIndex;
    case I_atmVccAalType:
      return (unsigned char *)&pVcc->atmVccAalType;
    case I_atmVccAal5CpcsTransmitSduSize:
      return (unsigned char *)&pVcc->atmVccAal5CpcsTransmitSduSize;
    case I_atmVccAal5CpcsReceiveSduSize:
      return (unsigned char *)&pVcc->atmVccAal5CpcsReceiveSduSize;
    case I_atmVccAal5EncapsType:
      return (unsigned char *)&pVcc->atmVccAal5EncapsType;
    case I_atmVclCrossConnectIdentifier:
      return (unsigned char *)&pVcc->atmVclCrossConnectIdentifier;
    case I_atmVclRowStatus:
      return (unsigned char *)&pVcc->atmVclRowStatus;
    case I_atmVclCastType:
      return (unsigned char *)&pVcc->atmVclCastType;
    case I_atmVclConnKind:
      return (unsigned char *)&pVcc->atmVclConnKind;
    default:
      return NO_MIBINSTANCE;
    }
  } /* if mesg->pdutype == SNMP_GET_REQ_PDU */
  else {
    newoid->name[index] = reqoid->name[index];
    newoid->name[vpiIndex] = reqoid->name[vpiIndex];
    newoid->name[vciIndex] = reqoid->name[vciIndex];
    return (unsigned char*)1;
  }
}
#endif /* defined(BUILD_SNMP_ATM_MIB) || defined(CPE_DSL_MIB) */

#ifdef BUILD_SNMP_ATM_MIB
/* This object contains an appropriate value to
 * be used for atmVpCrossConnectIndex when creating
 * entries in the atmVpCrossConnectTable.  The value
 * 0 indicates that no unassigned entries are
 * available. To obtain the atmVpCrossConnectIndex
 * value for a new entry, the manager issues a
 * management protocol retrieval operation to obtain
 * the current value of this object.  After each
 * retrieval, the agent should modify the value to
 * the next unassigned index.
 * After a manager retrieves a
 */

unsigned char *
var_atmVpCrossConnectIndexNext(int *var_len, snmp_info_t *mesg,
        int (**write_method)())
{
    /* Add value computations */

    /* Set size (in bytes) and return address of the variable */
    *var_len = sizeof(long);
    return (unsigned char *) NO_MIBINSTANCE;
}

/* An entry in the ATM VP Cross Connect table.
 * This entry is used to model a bi-directional
 * ATM VP cross-connect which cross-connects
 * two VPLs.
 * 
 * Step-wise Procedures to set up a VP Cross-connect
 * 
 * Once the entries in the atmVplTable are created,
 * the following procedures are used
 * to cross-connect the VPLs together.
 * 
 * (1) The manager obtains a unique
 * atmVpCrossConnectIndex by reading the
 * atmVpCrossConnectIndexNext object.
 * 
 * (2) Next, the manager creates a set of one
 */

unsigned char *
var_atmVpCrossConnectEntry(int *var_len,
        Oid *newoid, Oid *reqoid, int searchType,
        snmp_info_t *mesg, int (**write_method)())
{
/* Last subOID of COLUMNAR OID is column */
int	column = newoid->name[(newoid->namelen - 1)];
int	result;

    while (0 /* Test whether the entry has been found */) {
        /* Add indexes for the entry to OID newname */
        /* Determine whether it is the requested OID  */
        result = compare(reqoid, newoid);
        if (((searchType == EXACT) && (result != 0)) ||
                ((searchType == NEXT) && (result >= 0))) {
            return NO_MIBINSTANCE;
        }
    }
    /* Place here the computation functionality */

    /* Set write-function */
    *write_method = 0;
    /* Set size (in bytes) and return address of the variable */
    *var_len = sizeof(long);
    switch (column) {
	case I_atmVpCrossConnectIndex:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmVpCrossConnectLowIfIndex:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmVpCrossConnectLowVpi:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmVpCrossConnectHighIfIndex:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmVpCrossConnectHighVpi:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmVpCrossConnectAdminStatus:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmVpCrossConnectL2HOperStatus:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmVpCrossConnectH2LOperStatus:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmVpCrossConnectL2HLastChange:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmVpCrossConnectH2LLastChange:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmVpCrossConnectRowStatus:
	    return (unsigned char *) NO_MIBINSTANCE;
        default:
            return NO_MIBINSTANCE;
    }
}

/* This object contains an appropriate value to
 * be used for atmVcCrossConnectIndex when creating
 * entries in the atmVcCrossConnectTable.  The value
 * 0 indicates that no unassigned entries are
 * available. To obtain the atmVcCrossConnectIndex
 * value for a new entry, the manager issues a
 * management protocol retrieval operation to obtain
 * the current value of this object.  After each
 * retrieval, the agent should modify the value to
 * the next unassigned index.
 * After a manager retrieves a
 */

unsigned char *
var_atmVcCrossConnectIndexNext(int *var_len, snmp_info_t *mesg,
        int (**write_method)())
{
    /* Add value computations */

    /* Set size (in bytes) and return address of the variable */
    *var_len = sizeof(long);
    return (unsigned char *) NO_MIBINSTANCE;
}

/* An entry in the ATM VC Cross Connect table.
 * This entry is used to model a bi-directional ATM
 * VC cross-connect cross-connecting two end points.
 * 
 * Step-wise Procedures to set up a VC Cross-connect
 * 
 * 
 * Once the entries in the atmVclTable are created,
 * the following procedures are used
 * to cross-connect the VCLs together to
 * form a VCC segment.
 * 
 * (1) The manager obtains a unique
 * atmVcCrossConnectIndex by reading the
 * atmVcCrossConnectIndexNext object.
 * 
 * (2) Next, the 
 */

unsigned char *
var_atmVcCrossConnectEntry(int *var_len,
        Oid *newoid, Oid *reqoid, int searchType,
        snmp_info_t *mesg, int (**write_method)())
{
/* Last subOID of COLUMNAR OID is column */
int	column = newoid->name[(newoid->namelen - 1)];
int	result;

    while (0 /* Test whether the entry has been found */) {
        /* Add indexes for the entry to OID newname */
        /* Determine whether it is the requested OID  */
        result = compare(reqoid, newoid);
        if (((searchType == EXACT) && (result != 0)) ||
                ((searchType == NEXT) && (result >= 0))) {
            return NO_MIBINSTANCE;
        }
    }
    /* Place here the computation functionality */

    /* Set write-function */
    *write_method = 0;
    /* Set size (in bytes) and return address of the variable */
    *var_len = sizeof(long);
    switch (column) {
	case I_atmVcCrossConnectIndex:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmVcCrossConnectLowIfIndex:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmVcCrossConnectLowVpi:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmVcCrossConnectLowVci:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmVcCrossConnectHighIfIndex:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmVcCrossConnectHighVpi:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmVcCrossConnectHighVci:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmVcCrossConnectAdminStatus:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmVcCrossConnectL2HOperStatus:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmVcCrossConnectH2LOperStatus:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmVcCrossConnectL2HLastChange:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmVcCrossConnectH2LLastChange:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_atmVcCrossConnectRowStatus:
	    return (unsigned char *) NO_MIBINSTANCE;
        default:
            return NO_MIBINSTANCE;
    }
}

/* This list contains the AAL5 VCC
 * performance parameters and is indexed
 * by ifIndex values of AAL5 interfaces
 * and the associated VPI/VCI values.
 */

unsigned char *
var_aal5VccEntry(int *var_len,
        Oid *newoid, Oid *reqoid, int searchType,
        snmp_info_t *mesg, int (**write_method)())
{
/* Last subOID of COLUMNAR OID is column */
int	column = newoid->name[(newoid->namelen - 1)];
int	result;

    while (0 /* Test whether the entry has been found */) {
        /* Add indexes for the entry to OID newname */
        /* Determine whether it is the requested OID  */
        result = compare(reqoid, newoid);
        if (((searchType == EXACT) && (result != 0)) ||
                ((searchType == NEXT) && (result >= 0))) {
            return NO_MIBINSTANCE;
        }
    }
    /* Place here the computation functionality */

    /* Set write-function */
    *write_method = 0;
    /* Set size (in bytes) and return address of the variable */
    *var_len = sizeof(long);
    switch (column) {
	case I_aal5VccVpi:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_aal5VccVci:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_aal5VccCrcErrors:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_aal5VccSarTimeOuts:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_aal5VccOverSizedSDUs:
	    return (unsigned char *) NO_MIBINSTANCE;
        default:
            return NO_MIBINSTANCE;
    }
}

/* This object contains an appropriate value to
 * be used for atmTrafficDescrParamIndex when
 * creating entries in the
 * atmTrafficDescrParamTable.
 * The value 0 indicates that no unassigned
 * entries are available. To obtain the
 * atmTrafficDescrParamIndex value for a new
 * entry, the manager issues a management
 * protocol retrieval operation to obtain the
 * current value of this object.  After each
 * retrieval, the agent should modify the value
 * to the next unassigned index.
 * After a manag
 */

unsigned char *
var_atmTrafficDescrParamIndexNext(int *var_len, snmp_info_t *mesg,
        int (**write_method)())
{
    /* Add value computations */

    /* Set size (in bytes) and return address of the variable */
    *var_len = sizeof(long);
    return (unsigned char *) NO_MIBINSTANCE;
}
#endif /* BUILD_SNMP_ATM_MIB */

#if defined(BUILD_SNMP_ATM_MIB) || defined(CPE_DSL_MIB) 

static oid atmMIBObjects_oid[] = { O_atmMIBObjects };
static Object atmMIBObjects_variables[] = {

#ifdef BUILD_SNMP_ATM_MIB
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_atmInterfaceConfEntry,
                {3, { I_atmInterfaceConfTable, I_atmInterfaceConfEntry, I_atmInterfaceMaxVpcs }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_atmInterfaceConfEntry,
                {3, { I_atmInterfaceConfTable, I_atmInterfaceConfEntry, I_atmInterfaceMaxVccs }}},
    { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_atmInterfaceConfEntry,
                {3, { I_atmInterfaceConfTable, I_atmInterfaceConfEntry, I_atmInterfaceConfVpcs }}},
    { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_atmInterfaceConfEntry,
                {3, { I_atmInterfaceConfTable, I_atmInterfaceConfEntry, I_atmInterfaceConfVccs }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_atmInterfaceConfEntry,
                {3, { I_atmInterfaceConfTable, I_atmInterfaceConfEntry, I_atmInterfaceMaxActiveVpiBits }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_atmInterfaceConfEntry,
                {3, { I_atmInterfaceConfTable, I_atmInterfaceConfEntry, I_atmInterfaceMaxActiveVciBits }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_atmInterfaceConfEntry,
                {3, { I_atmInterfaceConfTable, I_atmInterfaceConfEntry, I_atmInterfaceIlmiVpi }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_atmInterfaceConfEntry,
                {3, { I_atmInterfaceConfTable, I_atmInterfaceConfEntry, I_atmInterfaceIlmiVci }}},
    { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_atmInterfaceConfEntry,
                {3, { I_atmInterfaceConfTable, I_atmInterfaceConfEntry, I_atmInterfaceAddressType }}},
    { SNMP_STRING, (RONLY| COLUMN), (void*)var_atmInterfaceConfEntry,
                {3, { I_atmInterfaceConfTable, I_atmInterfaceConfEntry, I_atmInterfaceAdminAddress }}},
    { SNMP_IPADDRESS, (RWRITE| COLUMN), (void*)var_atmInterfaceConfEntry,
                {3, { I_atmInterfaceConfTable, I_atmInterfaceConfEntry, I_atmInterfaceMyNeighborIpAddress }}},
    { SNMP_STRING, (RWRITE| COLUMN), (void*)var_atmInterfaceConfEntry,
                {3, { I_atmInterfaceConfTable, I_atmInterfaceConfEntry, I_atmInterfaceMyNeighborIfName }}},
    { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_atmInterfaceConfEntry,
                {3, { I_atmInterfaceConfTable, I_atmInterfaceConfEntry, I_atmInterfaceCurrentMaxVpiBits }}},
    { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_atmInterfaceConfEntry,
                {3, { I_atmInterfaceConfTable, I_atmInterfaceConfEntry, I_atmInterfaceCurrentMaxVciBits }}},
    { SNMP_STRING, (RWRITE| COLUMN), (void*)var_atmInterfaceConfEntry,
                {3, { I_atmInterfaceConfTable, I_atmInterfaceConfEntry, I_atmInterfaceSubscrAddress }}},
    { SNMP_COUNTER, (RONLY| COLUMN), (void*)var_atmInterfaceDs3PlcpEntry,
                {3, { I_atmInterfaceDs3PlcpTable, I_atmInterfaceDs3PlcpEntry, I_atmInterfaceDs3PlcpSEFSs }}},
    { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_atmInterfaceDs3PlcpEntry,
                {3, { I_atmInterfaceDs3PlcpTable, I_atmInterfaceDs3PlcpEntry, I_atmInterfaceDs3PlcpAlarmState }}},
    { SNMP_COUNTER, (RONLY| COLUMN), (void*)var_atmInterfaceDs3PlcpEntry,
                {3, { I_atmInterfaceDs3PlcpTable, I_atmInterfaceDs3PlcpEntry, I_atmInterfaceDs3PlcpUASs }}},
    { SNMP_COUNTER, (RONLY| COLUMN), (void*)var_atmInterfaceTCEntry,
                {3, { I_atmInterfaceTCTable, I_atmInterfaceTCEntry, I_atmInterfaceOCDEvents }}},
    { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_atmInterfaceTCEntry,
                {3, { I_atmInterfaceTCTable, I_atmInterfaceTCEntry, I_atmInterfaceTCAlarmState }}},
    { SNMP_OBJID, (NOACCESS| COLUMN), (void*)var_atmTrafficDescrParamEntry,
                {3, { I_atmTrafficDescrParamTable, I_atmTrafficDescrParamEntry, I_atmTrafficDescrType }}},
    { SNMP_INTEGER, (NOACCESS| COLUMN), (void*)var_atmTrafficDescrParamEntry,
                {3, { I_atmTrafficDescrParamTable, I_atmTrafficDescrParamEntry, I_atmTrafficDescrParam1 }}},
    { SNMP_INTEGER, (NOACCESS| COLUMN), (void*)var_atmTrafficDescrParamEntry,
                {3, { I_atmTrafficDescrParamTable, I_atmTrafficDescrParamEntry, I_atmTrafficDescrParam2 }}},
    { SNMP_INTEGER, (NOACCESS| COLUMN), (void*)var_atmTrafficDescrParamEntry,
                {3, { I_atmTrafficDescrParamTable, I_atmTrafficDescrParamEntry, I_atmTrafficDescrParam3 }}},
    { SNMP_INTEGER, (NOACCESS| COLUMN), (void*)var_atmTrafficDescrParamEntry,
                {3, { I_atmTrafficDescrParamTable, I_atmTrafficDescrParamEntry, I_atmTrafficDescrParam4 }}},
    { SNMP_INTEGER, (NOACCESS| COLUMN), (void*)var_atmTrafficDescrParamEntry,
                {3, { I_atmTrafficDescrParamTable, I_atmTrafficDescrParamEntry, I_atmTrafficDescrParam5 }}},
    { SNMP_INTEGER, (NOACCESS| COLUMN), (void*)var_atmTrafficDescrParamEntry,
                {3, { I_atmTrafficDescrParamTable, I_atmTrafficDescrParamEntry, I_atmTrafficQoSClass }}},
    { SNMP_INTEGER, (NOACCESS| COLUMN), (void*)var_atmTrafficDescrParamEntry,
                {3, { I_atmTrafficDescrParamTable, I_atmTrafficDescrParamEntry, I_atmTrafficDescrRowStatus }}},
    { SNMP_INTEGER, (NOACCESS| COLUMN), (void*)var_atmTrafficDescrParamEntry,
                {3, { I_atmTrafficDescrParamTable, I_atmTrafficDescrParamEntry, I_atmServiceCategory }}},
    { SNMP_INTEGER, (NOACCESS| COLUMN), (void*)var_atmTrafficDescrParamEntry,
                {3, { I_atmTrafficDescrParamTable, I_atmTrafficDescrParamEntry, I_atmTrafficFrameDiscard }}},
    { SNMP_INTEGER, (NOACCESS| COLUMN), (void*)var_atmVplEntry,
                {3, { I_atmVplTable, I_atmVplEntry, I_atmVplAdminStatus }}},
    { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_atmVplEntry,
                {3, { I_atmVplTable, I_atmVplEntry, I_atmVplOperStatus }}},
    { SNMP_TIMETICKS, (RONLY| COLUMN), (void*)var_atmVplEntry,
                {3, { I_atmVplTable, I_atmVplEntry, I_atmVplLastChange }}},
    { SNMP_INTEGER, (NOACCESS| COLUMN), (void*)var_atmVplEntry,
                {3, { I_atmVplTable, I_atmVplEntry, I_atmVplReceiveTrafficDescrIndex }}},
    { SNMP_INTEGER, (NOACCESS| COLUMN), (void*)var_atmVplEntry,
                {3, { I_atmVplTable, I_atmVplEntry, I_atmVplTransmitTrafficDescrIndex }}},
    { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_atmVplEntry,
                {3, { I_atmVplTable, I_atmVplEntry, I_atmVplCrossConnectIdentifier }}},
    { SNMP_INTEGER, (NOACCESS| COLUMN), (void*)var_atmVplEntry,
                {3, { I_atmVplTable, I_atmVplEntry, I_atmVplRowStatus }}},
    { SNMP_INTEGER, (NOACCESS| COLUMN), (void*)var_atmVplEntry,
                {3, { I_atmVplTable, I_atmVplEntry, I_atmVplCastType }}},
    { SNMP_INTEGER, (NOACCESS| COLUMN), (void*)var_atmVplEntry,
                {3, { I_atmVplTable, I_atmVplEntry, I_atmVplConnKind }}},
#endif /* BUILD_SNMP_ATM_MIB */

    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_atmVclEntry,
                {3, { I_atmVclTable, I_atmVclEntry, I_atmVclAdminStatus }}},
    { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_atmVclEntry,
                {3, { I_atmVclTable, I_atmVclEntry, I_atmVclOperStatus }}},
    { SNMP_TIMETICKS, (RONLY| COLUMN), (void*)var_atmVclEntry,
                {3, { I_atmVclTable, I_atmVclEntry, I_atmVclLastChange }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_atmVclEntry,
                {3, { I_atmVclTable, I_atmVclEntry, I_atmVclReceiveTrafficDescrIndex }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_atmVclEntry,
                {3, { I_atmVclTable, I_atmVclEntry, I_atmVclTransmitTrafficDescrIndex }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_atmVclEntry,
                {3, { I_atmVclTable, I_atmVclEntry, I_atmVccAalType }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_atmVclEntry,
                {3, { I_atmVclTable, I_atmVclEntry, I_atmVccAal5CpcsTransmitSduSize }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_atmVclEntry,
                {3, { I_atmVclTable, I_atmVclEntry, I_atmVccAal5CpcsReceiveSduSize }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_atmVclEntry,
                {3, { I_atmVclTable, I_atmVclEntry, I_atmVccAal5EncapsType }}},
    { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_atmVclEntry,
                {3, { I_atmVclTable, I_atmVclEntry, I_atmVclCrossConnectIdentifier }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_atmVclEntry,
                {3, { I_atmVclTable, I_atmVclEntry, I_atmVclRowStatus }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_atmVclEntry,
                {3, { I_atmVclTable, I_atmVclEntry, I_atmVclCastType }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_atmVclEntry,
                {3, { I_atmVclTable, I_atmVclEntry, I_atmVclConnKind }}},

#ifdef BUILD_SNMP_ATM_MIB
    { SNMP_INTEGER, (RONLY| SCALAR), (void*)var_atmVpCrossConnectIndexNext,
                 {2, { I_atmVpCrossConnectIndexNext, 0 }}},
    { SNMP_INTEGER, (NOACCESS| COLUMN), (void*)var_atmVpCrossConnectEntry,
                {3, { I_atmVpCrossConnectTable, I_atmVpCrossConnectEntry, I_atmVpCrossConnectAdminStatus }}},
    { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_atmVpCrossConnectEntry,
                {3, { I_atmVpCrossConnectTable, I_atmVpCrossConnectEntry, I_atmVpCrossConnectL2HOperStatus }}},
    { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_atmVpCrossConnectEntry,
                {3, { I_atmVpCrossConnectTable, I_atmVpCrossConnectEntry, I_atmVpCrossConnectH2LOperStatus }}},
    { SNMP_TIMETICKS, (RONLY| COLUMN), (void*)var_atmVpCrossConnectEntry,
                {3, { I_atmVpCrossConnectTable, I_atmVpCrossConnectEntry, I_atmVpCrossConnectL2HLastChange }}},
    { SNMP_TIMETICKS, (RONLY| COLUMN), (void*)var_atmVpCrossConnectEntry,
                {3, { I_atmVpCrossConnectTable, I_atmVpCrossConnectEntry, I_atmVpCrossConnectH2LLastChange }}},
    { SNMP_INTEGER, (NOACCESS| COLUMN), (void*)var_atmVpCrossConnectEntry,
                {3, { I_atmVpCrossConnectTable, I_atmVpCrossConnectEntry, I_atmVpCrossConnectRowStatus }}},
    { SNMP_INTEGER, (RONLY| SCALAR), (void*)var_atmVcCrossConnectIndexNext,
                 {2, { I_atmVcCrossConnectIndexNext, 0 }}},
    { SNMP_INTEGER, (NOACCESS| COLUMN), (void*)var_atmVcCrossConnectEntry,
                {3, { I_atmVcCrossConnectTable, I_atmVcCrossConnectEntry, I_atmVcCrossConnectAdminStatus }}},
    { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_atmVcCrossConnectEntry,
                {3, { I_atmVcCrossConnectTable, I_atmVcCrossConnectEntry, I_atmVcCrossConnectL2HOperStatus }}},
    { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_atmVcCrossConnectEntry,
                {3, { I_atmVcCrossConnectTable, I_atmVcCrossConnectEntry, I_atmVcCrossConnectH2LOperStatus }}},
    { SNMP_TIMETICKS, (RONLY| COLUMN), (void*)var_atmVcCrossConnectEntry,
                {3, { I_atmVcCrossConnectTable, I_atmVcCrossConnectEntry, I_atmVcCrossConnectL2HLastChange }}},
    { SNMP_TIMETICKS, (RONLY| COLUMN), (void*)var_atmVcCrossConnectEntry,
                {3, { I_atmVcCrossConnectTable, I_atmVcCrossConnectEntry, I_atmVcCrossConnectH2LLastChange }}},
    { SNMP_INTEGER, (NOACCESS| COLUMN), (void*)var_atmVcCrossConnectEntry,
                {3, { I_atmVcCrossConnectTable, I_atmVcCrossConnectEntry, I_atmVcCrossConnectRowStatus }}},
    { SNMP_COUNTER, (RONLY| COLUMN), var_aal5VccEntry,
                {3, { I_aal5VccTable, I_aal5VccEntry, I_aal5VccCrcErrors }}},
    { SNMP_COUNTER, (RONLY| COLUMN), var_aal5VccEntry,
                {3, { I_aal5VccTable, I_aal5VccEntry, I_aal5VccSarTimeOuts }}},
    { SNMP_COUNTER, (RONLY| COLUMN), var_aal5VccEntry,
                {3, { I_aal5VccTable, I_aal5VccEntry, I_aal5VccOverSizedSDUs }}},
    { SNMP_INTEGER, (RONLY| SCALAR), (void*)var_atmTrafficDescrParamIndexNext,
                 {2, { I_atmTrafficDescrParamIndexNext, 0 }}},
#endif /* ATM_MIB */
    { (int)NULL }
    };
static SubTree atmMIBObjects_tree =  { NULL, atmMIBObjects_variables,
	        (sizeof(atmMIBObjects_oid)/sizeof(oid)), atmMIBObjects_oid};

/* This is the MIB registration function. This should be called */
/* within the init_atmMib-function */
void register_subtrees_of_atmMib()
{
    insert_group_in_mib(&atmMIBObjects_tree);
}

/* atmMib initialisation (must also register the MIB module tree) */
void init_atmMib()
{
  memset(&atmList,0,sizeof(atmList));
  register_subtrees_of_atmMib();
}

#endif /*  defined(BUILD_SNMP_ATM_MIB) || defined(CPE_DSL_MIB) */
