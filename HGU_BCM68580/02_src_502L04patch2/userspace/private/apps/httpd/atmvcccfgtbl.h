/***********************************************************************
 *
 *  Copyright (c) 2001-2010  Broadcom Corporation
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
//
//  Filename:       atmvpccfgtbl.h
//  Author:         Peter T. Tran
//  Creation Date:  12/26/01
//
******************************************************************************
//  Description:
//      Define the AtmVccCfgTbl class and functions.
//
*****************************************************************************/

#ifndef __ATM_VCC_CFG_TBL_H__
#define __ATM_VCC_CFG_TBL_H__

/********************** Include Files ***************************************/

//#include "psiapi.h"
//~ #include "ifcdefs.h"
// mwang #include "bcmatmapi.h"

// mwang: httpd should not be accessing these low level funcs directly
// This entire file should be deleted in the next round of cleanup.
#ifdef dont_use
#if defined(__cplusplus)
extern "C" {
#endif
void       AtmVcc_init(void);
UINT16     AtmVcc_findByAddressFromCmd(char *addr);
UINT16     AtmVcc_getFirstAddress(PATM_VCC_ADDR pAddr);
UINT16     AtmVcc_setByObject(const PIFC_ATM_VCC_INFO pInfo);
UINT16     AtmVcc_setByObjectToPsiOnly(const PIFC_ATM_VCC_INFO pInfo);
UINT16     AtmVcc_setByAddress(const PATM_VCC_ADDR pAddr);
UINT16     AtmVcc_setByObjectIdToPsiOnly(const UINT16 objectId, const PIFC_ATM_VCC_INFO pInfo);
UINT16     AtmVcc_getByObjectId(const UINT16 objectId, PIFC_ATM_VCC_INFO pInfo);
void       AtmVcc_removeByAddress(const PATM_VCC_ADDR pAddr);
void       AtmVcc_removeByObjectId(const UINT16 objectId);
void       AtmVcc_removeByObjectIdFromPsiOnly(const UINT16 objectId);
void       AtmVcc_vccCfg(char *cmdLine);
void       AtmVcc_parseVccAddr(char *addr, PATM_VCC_ADDR pVccAddr);
int        AtmVcc_isDeletedVccInUse(char *cmdLine);

UINT16     AtmVcc_findByAddress(const PATM_VCC_ADDR pVccAddr);
UINT16     AtmVcc_getAvailableObjectId(void);
UINT16     AtmVcc_getObjectIdFromInstanceId(UINT16 instId);
UINT16     AtmVcc_getInstanceIdFromVcc(UINT16 vpi, UINT16 vci);
int        AtmVcc_isTrffDscrInUsed(UINT32 idx);
int        AtmVcc_isVccInUsed(UINT16 vpi, UINT16 vci);
void       AtmVcc_atmCtlVccAdd(const PIFC_ATM_VCC_INFO pInfo);
UINT32     AtmVcc_getNewInstId(void);
#if defined(__cplusplus)
}
#endif

#endif /* dontuse*/

#endif
