/***********************************************************************
 *
 *  Copyright (c) 2005-2010  Broadcom Corporation
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
//  Filename:       atmtrffdscrtbl.h
//  Author:         Peter T. Tran
//  Creation Date:  02/08/05
//
******************************************************************************
//  Description:
//      Define the AtmTrffDscrTbl class and functions.
//
*****************************************************************************/

#ifndef __ATM_TRFF_DSCR_TBL_H__
#define __ATM_TRFF_DSCR_TBL_H__

/********************** Include Files ***************************************/

//~#include "ifcdefs.h"
//~#include "psiapi.h"

#define ATM_TD_MAX      8

#if defined(__cplusplus)
extern "C" {
#endif
void          AtmTd_init(void);
UINT16        AtmTd_set(const PIFC_ATM_TD_INFO info);
UINT16        AtmTd_setToPsiOnly(const PIFC_ATM_TD_INFO info);
void          AtmTd_setTdInfoFromObjectId(const UINT16 objectId, const PIFC_ATM_TD_INFO info);
UINT16        AtmTd_getTdInfoFromObjectId(const UINT16 objectId, const PIFC_ATM_TD_INFO info);
UINT32        AtmTd_getTrffDscrIndexFromObjectId(const UINT16 objectId);
UINT16        AtmTd_getObjectIdFromInfo(const PIFC_ATM_TD_INFO info);
UINT16        AtmTd_getObjectIdFromIndex(const UINT32 index);
UINT16        AtmTd_getObjectIdFromDeleteCmd(char *cmdLine);
UINT32        AtmTd_getAvailableIndex(void);
void          AtmTd_removeFromPsiOnly(const UINT16 objectId);
void          AtmTd_trffDscrConfig(char *cmdLine);
UINT16        AtmTd_validateCmd(char *cmdLine);
UINT16        AtmTd_getAvailableObjectId(void);
UINT32        AtmTd_getTrffDscrIndexFromInfo(const PIFC_ATM_TD_INFO info);
UINT32        AtmTd_strToTrffDscrCategory(char *param);
void          AtmTd_categoryPsiToAtmCtl(char *psi, char *atmctl);
void          AtmTd_categoryAtmCtlToPsi(char *atmctl, char *psi);
void          AtmTd_atmCtlTrffDscrAdd(const PIFC_ATM_TD_INFO entry);
#if defined(__cplusplus)
}
#endif

#endif
