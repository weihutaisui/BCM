/***********************************************************************
 *
 *  Copyright (c) 2002-2010  Broadcom Corporation
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

/*
//  Filename:       secdefs.h
//  Author:         Peter T. Tran
//  Creation Date:  06/10/02
//
//  Description:
//      Define the Global structs, enumerations for security configuations.
//
*****************************************************************************/

#ifndef __SEC_DEFS_H
#define __SEC_DEFS_H

//~#include "ifcdefs.h"

/********************** Include Files ***************************************/

/********************** Global Types ****************************************/

typedef enum {
   SEC_STS_OK = 0,
   SEC_STS_ERR_GENERAL,
   SEC_STS_ERR_MEMORY,
   SEC_STS_ERR_OBJECT_NOT_FOUND
} SEC_STATUS;

typedef enum {
   SEC_COMMAND_TYPE_ADD = 0,
   SEC_COMMAND_TYPE_INSERT,
   SEC_COMMAND_TYPE_REMOVE
} SEC_COMMAND_TYPE;

/********************** Global Constants ************************************/

#define SECURITY_APPID           "SecCfg"
#define SCM_VRT_SRV_NUM_ID       1
#define SCM_VRT_SRV_TBL_ID       2
#define SCM_FLT_OUT_NUM_ID       3
#define SCM_FLT_OUT_TBL_ID       4
#define SCM_DMZ_HOST_ID          7
#define SCM_PRT_TRG_NUM_ID       8
#define SCM_PRT_TRG_TBL_ID       9
#define SCM_MAC_FLT_NUM_ID       10
#define SCM_MAC_FLT_TBL_ID       11
#define SCM_ACC_CNTR_NUM_ID      12
#define SCM_ACC_CNTR_TBL_ID      13
#define SCM_ACC_CNTR_MODE_ID     14
#define QOS_CLS_NUM_ID           15
#define QOS_CLS_TBL_ID           16
#define SCM_FLT_IN_NUM_ID        17
#define SCM_FLT_IN_TBL_ID        18
#define SCM_SRV_CNTR_ID          19

#define SEC_BUFF_SRVNAME_LEN     32 
#define SEC_BUFF_MAX_LEN         16 
#define SEC_CMD_MAX_LEN          2048

#define SEC_MAC_MAX_LEN          6

#define SEC_PROTO_NONE           0
#define SEC_PROTO_PPPOE          1        // require PPP_DISC
#define SEC_PROTO_IPV4           2        // reuire ARP
#define SEC_PROTO_IPV6           3	      // require ARP
#define SEC_PROTO_ATALK	         4
#define SEC_PROTO_IPX	         5
#define SEC_PROTO_NEtBEUI        6
#define SEC_PROTO_IGMP           7

#define SEC_PTR_TRG_MAX          32       // max port triggering entries.
#define SEC_VIR_SRV_MAX          32       // max virtual server entries.
#define SEC_ADD_REMOVE_ROW       6        // display add remove button if row > 6

#define SEC_MAX_PORT_MIRRORS     2       // max virtual server entries.

/********************** Global Structs ************************************/

typedef struct {
   UINT32 id;
   char srvName[SEC_BUFF_SRVNAME_LEN];
   struct in_addr addr;
   unsigned short protocol;
   unsigned short eStart;
   unsigned short eEnd;
   unsigned short iStart;
   unsigned short iEnd;
} SEC_VRT_SRV_ENTRY, *PSEC_VRT_SRV_ENTRY;

typedef struct {
   int protocol;
   int fromPort;
   int toPort;
} SEC_FLT_PRT_ENTRY, *PSEC_FLT_PRT_ENTRY;

typedef struct {
   char fromAddress[SEC_BUFF_MAX_LEN];
   char toAddress[SEC_BUFF_MAX_LEN];
} SEC_FLT_ADDR_ENTRY, *PSEC_FLT_ADDR_ENTRY;

typedef struct {
   int protocol;
   char fltName[SEC_BUFF_MAX_LEN];
   char srcAddr[SEC_BUFF_MAX_LEN];
   char srcMask[SEC_BUFF_MAX_LEN];
   char srcPort[SEC_BUFF_MAX_LEN];
   char dstAddr[SEC_BUFF_MAX_LEN];
   char dstMask[SEC_BUFF_MAX_LEN];
   char dstPort[SEC_BUFF_MAX_LEN];
   char wanIf[BUFLEN_16];   
} SEC_FLT_ENTRY, *PSEC_FLT_ENTRY;


// PROTOCOL flag
// 16 bit: bit 0-7:  trigger protocol type -- defined above such as SEC_PROTO_TCP...
//         bit 8-15: open protocol type -- defined above such as SEC_PROTO_TCP...
typedef struct {
   unsigned short
   tProto:8,
   oProto:8;
} PROTO_FLAG;

typedef struct {
   PROTO_FLAG flag;
   unsigned short tStart;
   unsigned short tEnd;
   unsigned short oStart;
   unsigned short oEnd;
   char appName[SEC_BUFF_MAX_LEN];
} SEC_PRT_TRG_ENTRY, *PSEC_PRT_TRG_ENTRY;

#define ACL_FILE_NAME     "/var/acl.conf"

#define MAC_FORWARD 0
#define MAC_BLOCKED 1
#define LAN_TO_WAN  0
#define WAN_TO_LAN  1
#define BOTH        2
#define MAC_DIR_MAX 3
// mac entry flag define
// 32 bit
// bit 0-15 bit: protocal type -- defined above such as SEC_PROTO_PPPOE...
// bit 16-17   : direction 0=lan->wan, 1=wan->lan, 2=lan<->wan
// bit 18-31   : reserved
typedef struct {
   unsigned int
   protocol:16,
   direction:2,
   reserved:14;
} MAC_FIL_FLAG;

typedef struct {
   MAC_FIL_FLAG flag;         
   char destMac[SEC_MAC_MAX_LEN];
   char srcMac[SEC_MAC_MAX_LEN];
   char wanIf[BUFLEN_16];
} SEC_MAC_FLT_ENTRY, *PSEC_MAC_FLT_ENTRY;

#endif

