/****************************************************************************
*
*     Copyright (c) 2001 Broadcom Corporation
*           All Rights Reserved
*
*     No portions of this material may be reproduced in any form without the
*     written permission of:
*
*           Broadcom Corporation
*           16251 Laguna Canyon Road
*           Irvine, California  92618
*
*     All information contained in this document is Broadcom Corporation
*     company private, proprietary, and trade secret.
*
*****************************************************************************/
/**
*
*  @file    atmMibDefs.h
*
*  @brief   ATM MIB constant definitions
*
****************************************************************************/

#if !defined( ATM_MIB_DEFS_H )
#define ATM_MIB_DEFS_H

/* ---- Include Files ---------------------------------------------------- */


/* ---- Constants and Types ---------------------------------------------- */

#define ATMIFCONFTBL
#define ATMIFTCTBL
#define ATMTRAFFDESCTBL
#define ATMVCLTBL
#define AAL5VCCTBL

#define ATM_INTERFACE_COUNT               1  /* Number of ATM interfaces */
#define ATM_PORT_NUM                      0 /* one port supported */
#define ATM_LOGICAL_PORT_NUM              0 /* one port supported */

#define ATM_INTF_MAX_VPCS						0
#define ATM_INTF_MAX_VCCS						8
#define ATM_INTF_CONF_VPCS						0
#define ATM_INTF_MAX_ACTIVE_VPI_BITS		12
#define ATM_INTF_MAX_ACTIVE_VCI_BITS		16
#define ATM_INTF_ILMI_VPI						0
#define ATM_INTF_ILMI_VCI_UNSUPPORTED					0
#define ATM_INTF_ILMI_VCI        					16
#define ATM_INTF_CURRENT_MAX_VPI_BITS		ATM_INTF_MAX_ACTIVE_VPI_BITS
#define ATM_INTF_CURRENT_MAX_VCI_BITS		ATM_INTF_MAX_ACTIVE_VCI_BITS

#define ATM_INTF_TC_OCD                   0

#define ATM_INTF_ADDRTYPE_PRIVATE         1
#define ATM_INTF_ADDRTYPE_NSAPI_E164      2
#define ATM_INTF_ADDRTYPE_NATIVE_E164     3
#define ATM_INTF_ADDRTYPE_OTHER           4

#define ATM_INTF_TC_ALMST_NOALARM         1
#define ATM_INTF_TC_ALMST_LCDFAIL         2

#define ATM_TRAFDESC_PARAM                0
#define ATM_TRAFDESC_QOSCLASS             0
#define ATM_TRAFDESC_ROWSTATUS_ACTIVE     1
#define ATM_TRAFDESC_SERVICE_CAT_UBR      6

#define ATM_VCL_AALTYPE_AAL1              1
#define ATM_VCL_AALTYPE_AAL34             2
#define ATM_VCL_AALTYPE_AAL5              3
#define ATM_VCL_AALTYPE_OTHER             4
#define ATM_VCL_AALTYPE_UNKNOWN           5
#define ATM_VCL_AALTYPE_AAL2              6
#define ATM_VCL_SDUSIZE                   0xFFFF
#define ATM_VCL_ROWSTATUS_ACTIVE          1
#define ATM_VCL_CAST_TYPE_P2P             1
#define ATM_VCL_CONN_KIND_PVC             1

#define ATM_ADMIN_UP                      1
#define ATM_ADMIN_DOWN                 2

/* read or write */
#define ATM_ROWSTATUS_ACTIVE              1
#define ATM_ROWSTATUS_NOT_IN_SERVICE      2
/* read only */
#define ATM_ROWSTATUS_NOT_READY           3
/* write only */
#define ATM_ROWSTATUS_CREATE_GO           4
#define ATM_ROWSTATUS_CREATE_WAIT         5
#define ATM_ROWSTATUS_CREATE_DESTROY      6

#define ATM_CAST_TYPE_P2P                 1
#define ATM_CAST_TYPE_P2MPROOT            2
#define ATM_CAST_TYPE_P2MPLEAVE           3
#define ATM_CONN_KIND_PVC                 1
#define ATM_CONN_KIND_SVC                 2
#define ATM_CONN_KIND_SPVC_OUT            3
#define ATM_CONN_KIND_SPVC_INITIATOR      4
#define ATM_CONN_KIND_SPVC_TARGET         5

#define ATM_VCC_ENCAP_ROUTED              1
#define ATM_VCC_ENCAP_BR_8023             2
#define ATM_VCC_ENCAP_BR_8025             3
#define ATM_VCC_ENCAP_BR_8026             4
#define ATM_VCC_ENCAP_LAN_EMU_8023        5
#define ATM_VCC_ENCAP_LAN_EMU_8025        6
#define ATM_VCC_ENCAP_LLC                 7
#define ATM_VCC_ENCAP_FRAME_RELAY_SSCS    8
#define ATM_VCC_ENCAP_OTHER               9
#define ATM_VCC_ENCAP_UNKNOWN             10

/* ---- Constants and Types ---------------------------------------------- */
/* ---- Variable Externs ------------------------------------------------- */
/* ---- Function Prototypes ---------------------------------------------- */


#endif /* ATM_MIB_DEFS_H */
