/***********************************************************************
 *
 *  Copyright (c) 2016 Broadcom
 *  All Rights Reserved
 *
<:label-BRCM:2016:proprietary:omcid

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

/*****************************************************************************
*    Description:
*
*      OMCI data model resource header file.
*
*****************************************************************************/

#ifndef _OMCI_DM_UTIL_H
#define _OMCI_DM_UTIL_H

/* ---- Include Files ----------------------------------------------------- */

#include "os_defs.h"
#include "omci_dm_defs.h"


/* ---- Constants and Types ----------------------------------------------- */

#define GPON_WAN_LEARNING_MODE_DEFAULT TRUE

typedef struct
{
    UINT16 gemMeId;
    UINT16 gemDrvIdx;
    UINT16 checkOid;
    UINT16 checkMeId;
    UINT16 uniOid;
    UINT16 uniMeId;
    UBOOL8 isNewUni;
    OmciMapFilterModelType modelType;
} OmciDmGemUniPairInfo;

typedef struct
{
    char rxIf[CMS_IFNAME_LENGTH];
    char txIf[CMS_IFNAME_LENGTH];
} OmciDmIfPair;

typedef void (*GEMUNIPAIR_CB)(OmciDmGemUniPairInfo *infoP);
typedef void (*VLANIFPAIR_CB)(OmciDmIfPair *infoP);


/* ---- Variable Externs -------------------------------------------------- */

/* ---- Function Prototypes ----------------------------------------------- */

void omciDm_init(void);

void omciDm_addUniEntry(UINT16 uniOid, UINT16 uniMeId);
void omciDm_removeUniEntry(UINT16 uniOid, UINT16 uniMeId);
UBOOL8 omciDm_getUniEntryConfigState(UINT16 uniOid, UINT16 uniMeId);
void omciDm_setUniEntryConfigState(UINT16 uniOid, UINT16 uniMeId, UBOOL8 configInd);
void omciDm_clearUniListConfigState(void);
void omciDm_removeAllUniEntries(void);
void omciDm_dumpUniList(void);

void omciDm_addGemEntry(UINT16 meId, UINT16 drvIdx, UINT16 usPQId,
  UINT16 dsPQId);
void omciDm_removeGemEntry(UINT16 meId);
CmsRet omciDm_FindGemEntry(UINT16 gemMeId, UINT16 *drvIdxP);
UBOOL8 omciDm_getGemPortIdxUsQid(UINT16 gemDrvIdx, UINT16 *usQIdP);
UBOOL8 omciDm_getGemPortIdxDsQid(UINT16 gemDrvIdx, UINT16 *dsQIdP);
void omciDm_dumpGemList(void);

void omciDm_addGemUniPair(UINT16 gemMeId, UINT16 checkOid, UINT16 checkMeId,
  UINT16 uniOid, UINT16 uniMeId, UBOOL8 isNewUni, OmciMapFilterModelType type);
void omciDm_removeGemUniPair(UINT16 gemMeId, UINT16 checkOid,
  UINT16 checkMeId, UINT16 uniOid, UINT16 uniMeId);
void omciDm_traverseGemUniLList(GEMUNIPAIR_CB actionP);
void omciDm_dumpGemUniList(void);

void omciDm_addVlanFlowIfPair(char *rxIfP, char *txIfP);
void omciDm_removeVlanFlowIfPair(char *rxIfP, char *txIfP);
void omciDm_dumpVlanFlowIfPair(void);
void omciDm_traverseSavedVlanFlowIfPair(VLANIFPAIR_CB actionP);
UINT32 omciDm_getVlanFlowIfPairCount(void);

UBOOL8 omciDm_getWanLearningMode(void);
void omciDm_setWanLearningMode(UBOOL8 mode);


#endif /* _OMCI_DM_UTIL_H */
