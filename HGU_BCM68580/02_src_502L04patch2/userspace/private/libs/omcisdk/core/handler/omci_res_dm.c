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
*      OMCI resource data manager functions.
*      It maintains the processed information from the raw OMCI data model,
*      and the cached data (information, status, etc.) related to the driver
*      and other applications. Those data is not kept in the MDM.
*      The purpose of keeping intermediate/cached data is to improve the
*      service provisioning and deletion efficiency by avoiding the repetitive
*      MDM look up.
*      The OMCI resource data may include:
*      - The flows defined in the OMCI data model.
*      - WAN learning mode.
*      - Created VLAN interfaces, etc.
*      Notes:
*      - Functions in this file are expected to be data-centric and
*        self-contained. Avoid creating the dependency with other CMS/OMCI
*        modules, do not include the code related to the MDM access, ioctl,
*        messaging, and RUT handlers, etc.
*
*****************************************************************************/

#ifdef DMP_X_ITU_ORG_GPON_1

/* ---- Include Files ----------------------------------------------------- */

#include "os_defs.h"
#include "owsvc_api.h"
#include "common_ellist.h"
#include "rut_gpon_model.h"
#include "omci_res_dm.h"


/* ---- Private Constants and Types --------------------------------------- */

typedef struct
{
    UINT16 uniOid;
    UINT16 uniMeId;
} OmciDmUniCfgKey;

typedef struct
{
    BCM_COMMON_DECLARE_LL_ENTRY();
    /* Keyword must immediately follow the common LL fields. */
    OmciDmUniCfgKey uniKey;
    UBOOL8 configB;
} OmciDmUniCfgEntry;

typedef struct
{
    BCM_COMMON_DECLARE_LL_ENTRY();
    /* Keyword must immediately follow the common LL fields. */
    UINT16 meId;
    UINT16 drvIdx;  /* GEM port index used by driver. */
    UINT16 usPQId;  /* Associated US PQ ID. */
    UINT16 dsPQId;  /* Associated DS PQ ID. */
    COMMON_ELL linkedUniPairs;
} OmciDmGemEntry;

typedef struct
{
    UINT16 checkOid;
    UINT16 checkMeId;
    UINT16 uniOid;
    UINT16 uniMeId;
} OmciDmUniKey;

typedef struct
{
    BCM_COMMON_DECLARE_LL_ENTRY();
    /* Keyword must immediately follow the common LL fields. */
    OmciDmUniKey uniKey;
    UBOOL8 isNewUni;
    OmciMapFilterModelType modelType;
} OmciDmUniEntry;

typedef struct
{
    BCM_COMMON_DECLARE_LL_ENTRY();
    /* Keyword must immediately follow the common LL fields. */
    OmciDmIfPair pair;
} OmciDmVlanFlowIfPairEntry;

#define OMCIDM_SET_ONU_UNI_CFG_ENTRY(uo, um, n)\
{        			\
    u->uniKey.uniOid = uo;      \
    u->uniKey.uniMeId = um;     \
    u->configB = n;             \
}

#define dmPrintf printf

#define OMCIDM_SET_UNI_ENTRY(u, co, cm, uo, um, n, t)\
{        			\
    u->uniKey.checkOid = co;    \
    u->uniKey.checkMeId = cm;   \
    u->uniKey.uniOid = uo;      \
    u->uniKey.uniMeId = um;     \
    u->isNewUni = n;            \
    u->modelType = t;           \
}

#define OMCIDM_SET_UNI_INFO(i, g, u)\
{                                       \
    i.gemMeId = g->meId;                \
    i.gemDrvIdx = g->drvIdx;            \
    i.checkOid = u->uniKey.checkOid;    \
    i.checkMeId = u->uniKey.checkMeId;  \
    i.uniOid = u->uniKey.uniOid;        \
    i.uniMeId = u->uniKey.uniMeId;      \
    i.isNewUni = u->isNewUni;           \
    i.modelType = u->modelType;         \
}

#define CMSLOG_ERROR_ONU_UNI_CFG_KEY(u)         \
  cmsLog_error("%d:%d ",                        \
    u.uniOid, u.uniMeId);

#define CMSLOG_ERROR_UNI_KEY(u)                 \
  cmsLog_error("%d:%d:%d:%d ",                  \
    u.checkOid, u.checkMeId,                    \
    u.uniOid, u.uniMeId);

#define DMPRINTF_GEMUNI_INFO(i)                         \
  dmPrintf("%-8d %-4d (%5d:%5d:%5d:%5d) %6d %6d\n",     \
    i->gemMeId, i->gemDrvIdx, i->checkOid, i->checkMeId,\
    i->uniOid, i->uniMeId, i->isNewUni, i->modelType);


/* ---- Private Function Prototypes --------------------------------------- */

static void uniLListPrintEntryCb(COMMON_ELL_ENTRY *entryP,
  void *dataP __attribute__((unused)));
static void uniLListClearConfigStateEntryCb(COMMON_ELL_ENTRY *entryP,
  void *dataP __attribute__((unused)));
static void uniLListRemoveEntryCb(COMMON_ELL_ENTRY *entryP,
  void *dataP __attribute__((unused)));

static void gemLListPrintEntryCb(COMMON_ELL_ENTRY *entryP,
  void *dataP __attribute__((unused)));
static void gemLListRemoveEntryCb(COMMON_ELL_ENTRY *entryP,
  void *dataP __attribute__((unused)));
static void gemUniLListRemoveEntryCb(COMMON_ELL_ENTRY *gemUniEntryP,
  void *dataP __attribute__((unused)));
static void printGemUniInfo(OmciDmGemUniPairInfo *infoP);
static void dmVlanFlowIfPairLListRemoveEntryCb(COMMON_ELL_ENTRY *entryP,
  void *usrDataP);
static void vlanFlowIfPairLListPrintEntryCb(COMMON_ELL_ENTRY *entryP,
  void *dataP __attribute__((unused)));


/* ---- Public Variables -------------------------------------------------- */


/* ---- Private Variables ------------------------------------------------- */

static BCM_COMMON_DECLARE_ELL(dmUniLL);
static BCM_COMMON_DECLARE_ELL(dmGemLL);
static BCM_COMMON_DECLARE_ELL(dmVlanFlowIfPairLL);
static UBOOL8 gponWanLearningMode = GPON_WAN_LEARNING_MODE_DEFAULT;


/* ---- Functions --------------------------------------------------------- */

static void uniLListPrintEntryCb(COMMON_ELL_ENTRY *entryP,
  void *dataP __attribute__((unused)))
{
    OmciDmUniCfgEntry *uniEntryP = (OmciDmUniCfgEntry*)entryP;

    dmPrintf("%-8d %-4d %6d\n",     \
      uniEntryP->uniKey.uniOid, uniEntryP->uniKey.uniMeId,
      uniEntryP->configB);
}

static void uniLListClearConfigStateEntryCb(COMMON_ELL_ENTRY *entryP,
  void *dataP __attribute__((unused)))
{
    OmciDmUniCfgEntry *uniEntryP = (OmciDmUniCfgEntry*)entryP;
    uniEntryP->configB = FALSE;
}

static void uniLListRemoveEntryCb(COMMON_ELL_ENTRY *entryP,
  void *usrDataP __attribute__((unused)))
{
    if (entryP != NULL)
    {
        cmsMem_free(entryP);
    }
}

void omciDm_addUniEntry(UINT16 uniOid, UINT16 uniMeId)
{
    OmciDmUniCfgEntry *uniEntryP;
    COMMON_ELL_ENTRY *entryP;

    uniEntryP = cmsMem_alloc(sizeof(OmciDmUniCfgEntry), ALLOC_ZEROIZE);
    if (uniEntryP == NULL)
    {
        cmsLog_error("cmsMem_alloc() failed");
        return;
    }

    uniEntryP->uniKey.uniOid = uniOid;
    uniEntryP->uniKey.uniMeId = uniMeId;
    entryP = ellAddEntry(&dmUniLL, (COMMON_ELL_ENTRY*)uniEntryP);
    if (entryP == NULL)
    {
        cmsLog_error("ellAddEntry() failed");
        CMSLOG_ERROR_ONU_UNI_CFG_KEY(uniEntryP->uniKey);
        cmsMem_free(uniEntryP);
    }
}

void omciDm_removeUniEntry(UINT16 uniOid, UINT16 uniMeId)
{
    OmciDmUniCfgEntry *uniEntryP;
    OmciDmUniCfgKey uniKey;

    uniKey.uniOid = uniOid;
    uniKey.uniMeId = uniMeId;
    uniEntryP = (OmciDmUniCfgEntry*)ellRemoveEntry(&dmUniLL, (void*)&uniKey);
    if (uniEntryP != NULL)
    {
        uniLListRemoveEntryCb((COMMON_ELL_ENTRY*)uniEntryP, NULL);
    }
    else
    {
        cmsLog_error("ellRemoveEntry() failed");
        CMSLOG_ERROR_ONU_UNI_CFG_KEY(uniKey);
    }
}

UBOOL8 omciDm_getUniEntryConfigState(UINT16 uniOid, UINT16 uniMeId)
{
    OmciDmUniCfgEntry *uniEntryP;
    OmciDmUniCfgKey uniKey;
    UBOOL8 configState = FALSE;

    uniKey.uniOid = uniOid;
    uniKey.uniMeId = uniMeId;
    uniEntryP = (OmciDmUniCfgEntry*)ellFindEntry(&dmUniLL, (void*)&uniKey);
    if (uniEntryP == NULL)
    {
        cmsLog_notice("ellFindEntry() failed, uni(%d:%d)",
          uniKey.uniOid, uniKey.uniMeId);
    }
    else
    {
        configState = uniEntryP->configB;
    }

    return configState;
}

void omciDm_setUniEntryConfigState(UINT16 uniOid, UINT16 uniMeId,
  UBOOL8 configInd)
{
    OmciDmUniCfgEntry *uniEntryP;
    OmciDmUniCfgKey uniKey;

    uniKey.uniOid = uniOid;
    uniKey.uniMeId = uniMeId;
    uniEntryP = (OmciDmUniCfgEntry*)ellFindEntry(&dmUniLL, (void*)&uniKey);
    if (uniEntryP == NULL)
    {
        cmsLog_notice("ellFindEntry() failed, uni(%d:%d)",
          uniKey.uniOid, uniKey.uniMeId);
    }
    else
    {
        uniEntryP->configB = configInd;
    }
}

void omciDm_clearUniListConfigState(void)
{
    ellIterateList(&dmUniLL, uniLListClearConfigStateEntryCb, NULL, 0);
}

void omciDm_removeAllUniEntries(void)
{
    ellIterateList(&dmUniLL, uniLListRemoveEntryCb, NULL, 1);
}

void omciDm_dumpUniList(void)
{
    if (ELL_GET_COUNT(dmUniLL) == 0)
    {
        return;
    }

    dmPrintf("\nUNI OID  MEID state\n");
    dmPrintf("======== ==== ======\n");
    ellIterateList(&dmUniLL, uniLListPrintEntryCb, NULL, 0);
    dmPrintf("======== ==== ======\n");
}

static void gemLListPrintEntryCb(COMMON_ELL_ENTRY *entryP,
  void *dataP __attribute__((unused)))
{
    OmciDmGemEntry *gemEntryP = (OmciDmGemEntry*)entryP;

    dmPrintf("%-8d %-4d %6d %6d\n",     \
      gemEntryP->meId, gemEntryP->drvIdx,
      gemEntryP->usPQId, gemEntryP->dsPQId);
}

static void gemLListRemoveEntryCb(COMMON_ELL_ENTRY *entryP,
  void *usrDataP __attribute__((unused)))
{
    OmciDmGemEntry *dmGemEntryP;

    dmGemEntryP = (OmciDmGemEntry*)entryP;
    if (!BCM_COMMON_LL_IS_EMPTY(&(dmGemEntryP->linkedUniPairs.list)))
    {
        ellIterateList(&(dmGemEntryP->linkedUniPairs),
          gemUniLListRemoveEntryCb, NULL, 1);
    }
    cmsMem_free(entryP);
}

static void gemUniLListRemoveEntryCb(COMMON_ELL_ENTRY *entryP,
  void *usrDataP __attribute__((unused)))
{
    if (entryP != NULL)
    {
        cmsMem_free(entryP);
    }
}

static void printGemUniInfo(OmciDmGemUniPairInfo *infoP)
{
    DMPRINTF_GEMUNI_INFO(infoP);
}

static void dmVlanFlowIfPairLListRemoveEntryCb(COMMON_ELL_ENTRY *entryP,
  void *usrDataP)
{
    OmciDmVlanFlowIfPairEntry *ifPairEntryP;
    VLANIFPAIR_CB actionP;

    ifPairEntryP = (OmciDmVlanFlowIfPairEntry*)entryP;
    actionP = (VLANIFPAIR_CB)usrDataP;
    if (actionP != NULL)
    {
        cmsLog_notice("ifPair(%s, %s)",
          ifPairEntryP->pair.rxIf, ifPairEntryP->pair.txIf);
        actionP(&ifPairEntryP->pair);
    }

    cmsMem_free(entryP);
}

static void vlanFlowIfPairLListPrintEntryCb(COMMON_ELL_ENTRY *entryP,
  void *dataP __attribute__((unused)))
{
    OmciDmVlanFlowIfPairEntry *ifPairEntryP;

    ifPairEntryP = (OmciDmVlanFlowIfPairEntry*)entryP;
    dmPrintf("rxIf=%s, txIf=%s\n",
      ifPairEntryP->pair.rxIf, ifPairEntryP->pair.txIf);
}

void omciDm_init(void)
{
    ellInit(&dmUniLL, sizeof(OmciDmUniCfgKey));
    ellInit(&dmGemLL, sizeof(UINT16));
    ellInit(&dmVlanFlowIfPairLL, sizeof(OmciDmIfPair));
}

void omciDm_addGemEntry(UINT16 meId, UINT16 drvIdx, UINT16 usPQId,
  UINT16 dsPQId)
{
    OmciDmGemEntry *gemEntryP;
    COMMON_ELL_ENTRY *entryP;

    gemEntryP = cmsMem_alloc(sizeof(OmciDmGemEntry), ALLOC_ZEROIZE);
    if (gemEntryP == NULL)
    {
        cmsLog_error("cmsMem_alloc() failed, meId=%d", meId);
        return;
    }

    //cmsLog_error("omciDm_addGemEntry(in), meId=%d, drvIdx=%d", meId, drvIdx);
    gemEntryP->meId = meId;
    gemEntryP->drvIdx = drvIdx;
    gemEntryP->usPQId = usPQId;
    gemEntryP->dsPQId = dsPQId;
    entryP = ellAddEntry(&dmGemLL, (COMMON_ELL_ENTRY*)gemEntryP);
    if (entryP != NULL)
    {
        if (entryP == (COMMON_ELL_ENTRY*)gemEntryP)
        {
            ellInit(&(gemEntryP->linkedUniPairs), sizeof(OmciDmUniKey));
        }
        else
        {
            cmsLog_notice("ellAddEntry() duplicated entry, meId=%d", meId);
            cmsMem_free(gemEntryP);
        }
    }
    else
    {
        cmsLog_error("ellAddEntry() failed, meId=%d", meId);
        cmsMem_free(gemEntryP);
    }
}

void omciDm_removeGemEntry(UINT16 meId)
{
    OmciDmGemEntry *gemEntryP;

    gemEntryP = (OmciDmGemEntry*)ellRemoveEntry(&dmGemLL, &meId);
    if (gemEntryP != NULL)
    {
        /* Remove all linked pairs. */
        gemLListRemoveEntryCb((COMMON_ELL_ENTRY*)gemEntryP, NULL);
    }
    else
    {
        cmsLog_error("ellRemoveEntry() failed, meId=%d", meId);
    }
}

CmsRet omciDm_FindGemEntry(UINT16 gemMeId, UINT16 *drvIdxP)
{
    OmciDmGemEntry *gemEntryP;

    gemEntryP = (OmciDmGemEntry*)ellFindEntry(&dmGemLL, &gemMeId);
    if (gemEntryP == NULL)
    {
        cmsLog_error("ellFindEntry() failed, gemMeId=%d", gemMeId);
        return CMSRET_OBJECT_NOT_FOUND;
    }

    *drvIdxP = gemEntryP->drvIdx;
    return CMSRET_SUCCESS;
}

UBOOL8 omciDm_getGemPortIdxUsQid(UINT16 gemDrvIdx, UINT16 *usQIdP)
{
    OmciDmGemEntry *gemEntryP;
    UBOOL8 found = FALSE;

    gemEntryP = BCM_COMMON_LL_GET_HEAD(dmGemLL.list);
    while (gemEntryP)
    {
        if (gemEntryP->drvIdx == gemDrvIdx)
        {
            *usQIdP = gemEntryP->usPQId;
            found = TRUE;
            break;
        }
        gemEntryP = BCM_COMMON_LL_GET_NEXT(gemEntryP);
    };

    return found;
}

UBOOL8 omciDm_getGemPortIdxDsQid(UINT16 gemDrvIdx, UINT16 *dsQIdP)
{
    OmciDmGemEntry *gemEntryP;
    UBOOL8 found = FALSE;

    gemEntryP = BCM_COMMON_LL_GET_HEAD(dmGemLL.list);
    while (gemEntryP)
    {
        if (gemEntryP->drvIdx == gemDrvIdx)
        {
            *dsQIdP = gemEntryP->dsPQId;
            found = TRUE;
            break;
        }
        gemEntryP = BCM_COMMON_LL_GET_NEXT(gemEntryP);
    };

    return found;
}

void omciDm_dumpGemList(void)
{
    if (ELL_GET_COUNT(dmGemLL) == 0)
    {
        return;
    }

    dmPrintf("\nGEM MEID IDX  UsPQId DsPQId\n");
    dmPrintf("======== ==== ====== ======\n");
    ellIterateList(&dmGemLL, gemLListPrintEntryCb, NULL, 0);
    dmPrintf("======== ==== ====== ======\n");
}

void omciDm_addGemUniPair(UINT16 gemMeId, UINT16 checkOid, UINT16 checkMeId,
  UINT16 uniOid, UINT16 uniMeId, UBOOL8 isNewUni, OmciMapFilterModelType type)
{
    OmciDmGemEntry *gemEntryP;
    OmciDmUniEntry *uniEntryP;
    COMMON_ELL_ENTRY *entryP;

    gemEntryP = (OmciDmGemEntry*)ellFindEntry(&dmGemLL, &gemMeId);
    if (gemEntryP == NULL)
    {
        cmsLog_error("ellFindEntry() failed, gemMeId=%d", gemMeId);
        return;
    }

    uniEntryP = (OmciDmUniEntry*)cmsMem_alloc(sizeof(OmciDmUniEntry),
      ALLOC_ZEROIZE);
    if (uniEntryP == NULL)
    {
        cmsLog_error("cmsMem_alloc() failed");
        return;
    }

    OMCIDM_SET_UNI_ENTRY(uniEntryP, checkOid, checkMeId, uniOid, uniMeId,
      isNewUni, type);

    entryP = ellAddEntry(&gemEntryP->linkedUniPairs,
      (COMMON_ELL_ENTRY*)uniEntryP);
    if (entryP == NULL)
    {
        cmsLog_error("ellAddEntry() failed");
        CMSLOG_ERROR_UNI_KEY(uniEntryP->uniKey);
        cmsMem_free(uniEntryP);
    }
    else if (entryP != (COMMON_ELL_ENTRY*)uniEntryP)
    {
        cmsLog_notice("ellAddEntry() duplicated entry");
        cmsMem_free(uniEntryP);
    }
}

void omciDm_removeGemUniPair(UINT16 gemMeId, UINT16 checkOid,
  UINT16 checkMeId, UINT16 uniOid, UINT16 uniMeId)
{
    OmciDmGemEntry *gemEntryP;
    OmciDmUniEntry *uniEntryP;
    OmciDmUniKey uniKey;

    gemEntryP = (OmciDmGemEntry*)ellFindEntry(&dmGemLL, &gemMeId);
    if (gemEntryP == NULL)
    {
        cmsLog_error("ellFindEntry() failed, gemMeId=%d", gemMeId);
        return;
    }

    uniKey.checkOid = checkOid;
    uniKey.checkMeId = checkMeId;
    uniKey.uniOid = uniOid;
    uniKey.uniMeId = uniMeId;
    uniEntryP = (OmciDmUniEntry*)ellRemoveEntry(&gemEntryP->linkedUniPairs,
      (void*)&uniKey);
    if (uniEntryP != NULL)
    {
        cmsMem_free(uniEntryP);
    }
    else
    {
        cmsLog_notice("ellRemoveEntry() failed");
        CMSLOG_ERROR_UNI_KEY(uniKey);
    }
}

void omciDm_traverseGemUniLList(GEMUNIPAIR_CB actionP)
{
    OmciDmGemEntry *gemEntryP;
    OmciDmUniEntry *uniEntryP = NULL;
    OmciDmGemUniPairInfo info;

    gemEntryP = BCM_COMMON_LL_GET_HEAD(dmGemLL.list);
    while (gemEntryP)
    {
        if (!BCM_COMMON_LL_IS_EMPTY(&(gemEntryP->linkedUniPairs.list)))
        {
            uniEntryP = BCM_COMMON_LL_GET_HEAD(gemEntryP->linkedUniPairs.list);
            while (uniEntryP)
            {
                if (actionP != NULL)
                {
                    OMCIDM_SET_UNI_INFO(info, gemEntryP, uniEntryP);
                    actionP(&info);
                }
                uniEntryP = BCM_COMMON_LL_GET_NEXT(uniEntryP);
            };
        }
        gemEntryP = BCM_COMMON_LL_GET_NEXT(gemEntryP);
    };
}

void omciDm_dumpGemUniList(void)
{
    if (ELL_GET_COUNT(dmGemLL) == 0)
    {
        return;
    }

    dmPrintf("\nGEM MEID IDX  Check(O,ME)   Uni(O,ME)   NEWUNI Type\n");
    dmPrintf("======== ==== ========================= ====== ======\n");
    omciDm_traverseGemUniLList(printGemUniInfo);
    dmPrintf("======== ==== ========================= ====== ======\n");
}

void omciDm_addVlanFlowIfPair(char *rxIfP, char *txIfP)
{
    OmciDmVlanFlowIfPairEntry *ifPairEntryP;
    COMMON_ELL_ENTRY *entryP;

    if ((rxIfP == NULL) || (txIfP == NULL))
    {
        cmsLog_error("Invalid parameters, rxIf=0x%p, txIf=0x%p",
          rxIfP, txIfP);
        return;
    }

    ifPairEntryP = (OmciDmVlanFlowIfPairEntry*)cmsMem_alloc(
      sizeof(OmciDmVlanFlowIfPairEntry), ALLOC_ZEROIZE);
    if (ifPairEntryP == NULL)
    {
        cmsLog_error("cmsMem_alloc() failed, rxIf=%s, txIf=%s", rxIfP, txIfP);
        return;
    }

    cmsUtl_strncpy(ifPairEntryP->pair.rxIf, rxIfP, CMS_IFNAME_LENGTH);
    cmsUtl_strncpy(ifPairEntryP->pair.txIf, txIfP, CMS_IFNAME_LENGTH);

    entryP = ellAddEntry(&dmVlanFlowIfPairLL, (COMMON_ELL_ENTRY*)ifPairEntryP);
    if (entryP == NULL)
    {
        cmsLog_error("ellAddEntry() failed, rxIf=%s, txIf=%s", rxIfP, txIfP);
        cmsMem_free(ifPairEntryP);
    }
    else if (entryP != (COMMON_ELL_ENTRY*)ifPairEntryP)
    {
        cmsLog_notice("ellAddEntry() duplicated entry, rxIf=%s, txIf=%s", rxIfP, txIfP);
        cmsMem_free(ifPairEntryP);
    }
}

void omciDm_removeVlanFlowIfPair(char *rxIfP, char *txIfP)
{
    OmciDmVlanFlowIfPairEntry *ifPairEntryP;
    OmciDmIfPair pairKey;

    if ((rxIfP == NULL) || (txIfP == NULL))
    {
        cmsLog_error("Invalid parameters, rxIf=0x%p, txIf=0x%p",
          rxIfP, txIfP);
        return;
    }

    memset(&pairKey, 0x0, sizeof(OmciDmIfPair));
    cmsUtl_strncpy(pairKey.rxIf, rxIfP, CMS_IFNAME_LENGTH);
    cmsUtl_strncpy(pairKey.txIf, txIfP, CMS_IFNAME_LENGTH);
    ifPairEntryP = (OmciDmVlanFlowIfPairEntry*)ellRemoveEntry(
      &dmVlanFlowIfPairLL, (void*)&pairKey);
    if (ifPairEntryP != NULL)
    {
        cmsMem_free(ifPairEntryP);
    }
    else
    {
        cmsLog_error("ellRemoveEntry() failed, rxIf=%s, txIf=%s",
          rxIfP, txIfP);
    }
}

void omciDm_traverseSavedVlanFlowIfPair(VLANIFPAIR_CB actionP)
{
    ellIterateList(&dmVlanFlowIfPairLL, dmVlanFlowIfPairLListRemoveEntryCb,
      actionP, 1);
}

void omciDm_dumpVlanFlowIfPair(void)
{
    if (ELL_GET_COUNT(dmVlanFlowIfPairLL) == 0)
    {
        return;
    }

    dmPrintf("\nVLAN flow interface pair list: keyLen=%d, count=%d\n",
      dmVlanFlowIfPairLL.keyLen, dmVlanFlowIfPairLL.count);
    ellIterateList(&dmVlanFlowIfPairLL, vlanFlowIfPairLListPrintEntryCb,
      NULL, 0);
    dmPrintf("\n");
}

UINT32 omciDm_getVlanFlowIfPairCount(void)
{
    return dmVlanFlowIfPairLL.count;
}

UBOOL8 omciDm_getWanLearningMode(void)
{
    return gponWanLearningMode;
}

void omciDm_setWanLearningMode(UBOOL8 mode)
{
    gponWanLearningMode = mode;
}

void omciDm_debug(void)
{
    UINT16 checkOid;
    UINT16 checkMeId;
    UINT16 uniOid;
    UINT16 uniMeId;
    UBOOL8 isNewUni;
    OmciMapFilterModelType modelType;
    UINT16 baseMeId = 0x101; 
    UINT16 unPortNum = 128; 
    UBOOL8 configState = FALSE; 

    ellInit(&dmGemLL, sizeof(UINT16));
    omciDm_addGemEntry(1, 1, 0, 0);
    omciDm_addGemEntry(2, 2, 0, 0);
    omciDm_addGemEntry(3, 3, 0, 0);

    checkOid = MDMOID_PPTP_ETHERNET_UNI;
    checkMeId = 0x101;
    uniOid = MDMOID_PPTP_ETHERNET_UNI;
    uniMeId = 0x101;
    isNewUni = 1;
    modelType = 2;

    omciDm_addGemUniPair(1, checkOid, checkMeId, uniOid, uniMeId, isNewUni,
      modelType);
    omciDm_addGemUniPair(2, checkOid, checkMeId, uniOid, uniMeId, isNewUni,
      modelType);
    omciDm_addGemUniPair(3, checkOid, checkMeId, uniOid, uniMeId, isNewUni,
      modelType);
    omciDm_addGemUniPair(4, checkOid, checkMeId, uniOid, uniMeId, isNewUni,
      modelType);

    omciDm_removeGemUniPair(1, checkOid, checkMeId, uniOid, uniMeId);
    omciDm_removeGemUniPair(2, checkOid, checkMeId, uniOid, uniMeId);
    omciDm_dumpGemUniList();

    omciDm_removeGemEntry(1);
    omciDm_removeGemEntry(2);
    omciDm_removeGemEntry(3);

    ellInit(&dmUniLL, sizeof(OmciDmUniCfgKey));

    uniOid = MDMOID_PPTP_ETHERNET_UNI;
    for (uniMeId = baseMeId; uniMeId < (baseMeId + unPortNum); uniMeId++)
    {
        omciDm_addUniEntry(uniOid, uniMeId);
    }

    for (uniMeId = baseMeId; uniMeId < (baseMeId + unPortNum); uniMeId++)
    {
        configState = uniMeId % 2; 
        omciDm_setUniEntryConfigState(uniOid, uniMeId, configState);
        dmPrintf("\nSet UNI OID %d, MEID %d state %d\n",
          uniOid, uniMeId, configState);
    }

    omciDm_clearUniListConfigState();

    for (uniMeId = baseMeId; uniMeId < (baseMeId + unPortNum); uniMeId++)
    {
        configState = omciDm_getUniEntryConfigState(uniOid, uniMeId);
        dmPrintf("\nUNI OID %d, MEID %d state %d\n",
          uniOid, uniMeId, configState);
    }

    ellIterateList(&dmUniLL, uniLListRemoveEntryCb, NULL, 0);
}

#endif /* DMP_X_ITU_ORG_GPON_1 */
