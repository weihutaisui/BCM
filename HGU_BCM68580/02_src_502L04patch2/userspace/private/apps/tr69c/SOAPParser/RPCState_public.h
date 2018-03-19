
/***********************************************************************
 *
 *  Copyright (c) 2006-2007  Broadcom Corporation
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

#ifndef RPCSTATE_PUBLIC_H
#define RPCSTATE_PUBLIC_H

/*
 * This header file declares the functions from RPCState.c and RPCState.h 
 * that can be included by main.c, which customers can recompile.
 * The functions in this file does not depend on any other TR69C header files
 * which we do not give to customers.
 */

#include "inc/tr69cdefs.h"

/* these strings are passed to TR69 stack at registration time */
#define VENDOR_RPC_PREFIX                "X_"
#define VENDOR_GET_DATA_MODEL_INFO       "X_BROADCOM_COM_GetDataModelInfo"
#define VENDOR_GET_DATA_MODEL_INFO_RESPONSE  "X_BROADCOM_COM_GetDataModelInfoResponse"

typedef struct vendorGetDataModelInfoReq {
   char *commandKey;
} VendorGetDataModelInfoReq;

/* This structure hold parameters pertaining to Vendor specific RPC */
typedef struct vendorRPCAction {
   char *rpcName;
   int     arrayItemCnt;   /* cnt of items in parameter list-not used */
   union {
      VendorGetDataModelInfoReq getDataModelInfoReq; 
      /* more items here later for each rpc method*/
   } ud;
} VendorRPCAction;

typedef void (*DO_VENDOR_RPC_FUNC)(VendorRPCAction *rpcAction);

/* this is is an item in the vendor specific list that RPC state machine
 * keeps track of.   It is a link list of each vendor RPC that is registered
 * to the TR69 protocol stack.
 */
typedef struct vendorSpecificRPCItem
{
   char *rpcName; /* this is the tag Name in xmlNodeDesc */
   XmlNodeDesc *rpcNodeDesc; /* this contains XML info and function to parse the XML info */
   DO_VENDOR_RPC_FUNC doRpcFunc;
   struct vendorSpecificRPCItem *next;
} VendorSpecificRPCItem;


void doGetCurrentDataModel(VendorRPCAction *a);
void getCwmpPrefix(char *prefix);
void sendVendorResponseToAcs(char *response);
void registerVendorRPC(char *rpcNameStr, XmlNodeDesc *rpcNodeDesc, 
                       DO_VENDOR_RPC_FUNC doRpcAction);

#endif /* RPCSTATE_PUBLIC_H */
