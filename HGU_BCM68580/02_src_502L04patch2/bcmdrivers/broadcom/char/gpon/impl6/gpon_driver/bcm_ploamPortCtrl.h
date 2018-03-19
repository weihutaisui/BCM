/* 
* <:copyright-BRCM:2007:proprietary:gpon
* 
*    Copyright (c) 2007 Broadcom 
*    All Rights Reserved
* 
*  This program is the proprietary software of Broadcom and/or its
*  licensors, and may only be used, duplicated, modified or distributed pursuant
*  to the terms and conditions of a separate, written license agreement executed
*  between you and Broadcom (an "Authorized License").  Except as set forth in
*  an Authorized License, Broadcom grants no license (express or implied), right
*  to use, or waiver of any kind with respect to the Software, and Broadcom
*  expressly reserves all rights in and to the Software and all intellectual
*  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
*  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
*  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
* 
*  Except as expressly set forth in the Authorized License,
* 
*  1. This program, including its structure, sequence and organization,
*     constitutes the valuable trade secrets of Broadcom, and you shall use
*     all reasonable efforts to protect the confidentiality thereof, and to
*     use this information only in connection with your use of Broadcom
*     integrated circuit products.
* 
*  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
*     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
*     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
*     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
*     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
*     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
*     PERFORMANCE OF THE SOFTWARE.
* 
*  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
*     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
*     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
*     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
*     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
*     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
*     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
*     LIMITED REMEDY.
* :>
*/ 
#ifndef BCM_PLOAM_PORT_CTRL_H
#define BCM_PLOAM_PORT_CTRL_H

#include "bcm_OS_Deps.h"
#include <bcm_ploam_api.h>
#include "bcm_gponBasicDefs.h"
#include "bcm_ploamEvent.h"
#include "rdpa_gpon.h"

#define BCM_PLOAM_PORTCTRL_NUM_TCONTS BCM_PLOAM_NUM_DATA_TCONTS
#define BCM_PLOAM_PORTCTRL_NUM_GEMPORTS BCM_PLOAM_NUM_DATA_GEM_PORTS
#define BCM_PLOAM_PORTCTRL_US_QUEUE_IDX_UNASSIGNED  (BCM_PLOAM_US_QUEUE_IDX_UNASSIGNED)

typedef struct {
/*public*/
  bool   enabled;     /* AllocId property */
  uint16_t tcontIdx;  /* tcontIdx property */
  uint16_t allocId;   /* AllocId property */
  int    refCount;    /* AllocId property */
  uint32_t queueMap;  /* tcontIdx property */
  uint8_t  bDefArbitrationWfq; /* tcontIdx property */
  uint8_t schdPolicy; /* tcontIdx property */ /* SP or WRR - this comes from OMCI TCONT ME */
} BCM_TcontInfo;

typedef struct {
/*public type*/
  uint16_t          usQueueIdx;
  uint16_t          tcontIdx;
  uint8_t           priority;
  bool              is_shaper_cfg;
  bool              is_qos_cfg;
  uint8_t           connect_in_index;
} BCM_UsQueueCfgInfo;

typedef struct {
/*public*/
  bool enabled;
  bool encrypted;
  uint16_t portId;

} BCM_OmciPortInfo;

typedef struct {
   UINT16 algo;    /* 0-UBR, 1-VBR, 2-CBR */
   UINT8 mcrEnable;
   UINT8  shaperAlgW;
   UINT32  shaperPCRVal;
   UINT32  shaperSCRVal;
   int shaperBT;
   UINT32  shaperMBS;
}BCM_GemPortShaperHwSpecificInfo;

#define GET_GEM_PORT_FLAG(m,f) \
        (((m) & (BCM_PLOAM_GEM_PORT_FLAGS_##f)) >> BCM_PLOAM_GEM_PORT_FLAGS_##f##_BIT_POS)

#define SET_GEM_PORT_FLAG(m,f,v) \
        ((m) = (~(BCM_PLOAM_GEM_PORT_FLAGS_##f) & (m)) | \
                 ((BCM_PLOAM_GEM_PORT_FLAGS_##f) & ((v)<<(BCM_PLOAM_GEM_PORT_FLAGS_##f##_BIT_POS))))

#define GEM_PORT_ENABLED(m) GET_GEM_PORT_FLAG((m), PORT_ENABLED)
#define GEM_PORT_OPERATIONAL(m) GET_GEM_PORT_FLAG((m), PORT_OPERATIONAL)
#define GEM_PORT_ENCRYPTED(m) GET_GEM_PORT_FLAG((m), PORT_ENCRYPTED)
#define GEM_PORT_DS_Q_PBIT_BASED(m) GET_GEM_PORT_FLAG((m), DS_Q_PBIT_BASED)
#define GEM_PORT_DS_Q_USE_TAG1_PBIT(m) GET_GEM_PORT_FLAG((m), DS_Q_USE_TAG1_PBIT)

#define SET_GEM_PORT_OPERATIONAL(m,v) SET_GEM_PORT_FLAG(m,PORT_OPERATIONAL,v)
#define SET_GEM_PORT_ENCRYPTED(m,v) SET_GEM_PORT_FLAG(m,PORT_ENCRYPTED,v)
#define SET_GEM_PORT_ENABLED(m,v) SET_GEM_PORT_FLAG(m,PORT_ENABLED,v)
#define SET_GEM_DS_Q_PBIT_BASED(m,v) SET_GEM_PORT_FLAG(m,DS_Q_PBIT_BASED,v)
#define SET_GEM_DS_Q_USE_TAG1_PBIT(m,v) SET_GEM_PORT_FLAG(m,DS_Q_USE_TAG1_PBIT,v)

typedef struct {
/*public type*/
  uint16_t portIdx;
  uint16_t portId;
  uint16_t allocId;
  uint16_t mibIdx;
  BCM_Ploam_McastFilterMode mcastFilterMode;
  uint8_t dsQueueIdx;
  uint8_t usQueueIdx;
  uint8_t flags;
} BCM_GemPortInfo;

/**
 *Public Functions:
 **/

/*(Re)Initialize this module*/
void bcm_ploamPortCtrlInit(bool keepConfig, BCM_PloamEventState* ploamEventObjp);

/*Create a Tcont. Newly created Tconts are enabled by default.
 *Creating an existing Tcont will not change its state. This will not be flagged as an error.
 *Returns zero if successful.
 */
int bcm_ploamPortCtrlCreateTcont(uint16_t allocId, OUT BCM_TcontInfo* tcontInfo);
int bcm_ploamPortCtrlCfgTcontAllocId(uint16_t omciTcontIdx, uint16_t allocId);
int bcm_ploamPortCtrlDecfgTcontAllocId(uint16_t omciTcontIdx);
int bcm_ploamPortCtrlEnableTcont(uint16_t tcontIdx, bool enable);
void bcm_ploamPortCtrlEnableAllTconts(bool enable);
int bcm_ploamPortCtrlRemoveTcont(uint16_t tcontIdx);
int bcm_ploamPortCtrlGetTcontInfo(uint16_t tcontIdx, OUT BCM_TcontInfo* tcontInfo);

/*Return the number of Tconts currently existing in the system*/
int bcm_ploamPortCtrlGetNumTconts(void);

/*Iterator: Return pointer to the next created tcont in the system. Pass in the
 *current iterator as 'current' argument. Pass in 0 to get the first created tcont
 *in the system.
 *Returns 0 if there is no next tcont*/
BCM_TcontInfo* bcm_ploamPortCtrlGetNextTcont(IN BCM_TcontInfo* cur);

/*Returns nonzero if allocId not found*/
int bcm_ploamPortCtrlAllocIdtoIdx(uint16_t allocId, OUT uint16_t* tcontIdx);
int bcm_ploamPortCtrlAllocIdtoIdxOmci(uint16_t allocId, OUT uint16_t* tcontIdx);
int bcm_ploamPortCtrlGetTcontCfgInfo(uint16_t tcontIdx, BCM_Ploam_TcontInfo *tcontInfop);

/*returns 0 if successful. Newly created GemPorts are disabled by default.
 *Error codes: -EBUSY, -ENOENT
 */
int bcm_ploamPortCtrlCreateGemPort(uint16_t gemPortIdx, /*Set to BCM_PLOAM_GEM_PORT_IDX_UNASSIGNED to create by ID*/
                                   uint16_t gemPortId,
                                   uint16_t allocId, uint8_t usQueueIdx, 
                                   uint8_t  dsMibIdx,
                                   OUT BCM_GemPortInfo* gemPortInfo);

int bcm_ploamPortCtrlRemoveGemPort(uint16_t portIdx);

/*SW enabled bi-dir gems are HW enabled when their TCONT is created.
 *DS only gems have no TCONT -> HW enable SW enabled DS only gems when entering O5*/
void bcm_ploamPortCtrlHwEnableDsOnlyGemPorts(bool enable);

/*Error codes: ENOENT*/
int bcm_ploamPortCtrlEnableGemPort(uint16_t portIdx, bool enable);
int bcm_ploamPortCtrlEncryptGemPortById(uint16_t gemPortId, bool encrypt);
int bcm_ploamPortCtrlEncryptGemPortByIdx(uint16_t gemPortIdx, bool encrypt);
int bcm_ploamPortCtrlReadLut(uint16_t gemPortId, uint32_t *lutData);
int bcm_ploamPortCtrlWriteLut(uint16_t gemPortId, uint32_t lutData);
uint32_t bcm_ploamPortCtrlGetMaxPktLen(void);
int bcm_ploamPortCtrlSetMaxPktLen(uint32_t pktLen);
int bcm_ploamPortCtrlCfgGemPortGtcDs(uint16_t gemPortIdx, uint8_t queueIdx,
                                     BCM_Ploam_McastFilterMode mcastFilterMode,
                                     uint8_t destQPbitBased, uint8_t useTag1Pbits);

/*Returns error if port not found*/
int bcm_ploamPortCtrlGetGemPortInfo(uint16_t portIdx, OUT BCM_GemPortInfo* gemPortInfo);

/*return 0 if succesful*/
int bcm_ploamPortCtrlGemPortIdtoIdx(uint16_t gemPortId, OUT uint16_t* gemPortIdx);
int bcm_ploamPortCtrlGemPortIdxtoId(uint16_t gemPortIdx, OUT uint16_t* gemPortId);
int bcm_ploamPortCtrlGemPortIdxtoUsQ(uint16_t gemPortIdx, OUT uint8_t* usQueue);

/*Return the number of GemPorts currently existing in the system*/
int bcm_ploamPortCtrlGetNumGemPorts(void);

/*Iterator: Return pointer to the next created GEM port in the system. Pass in the
 *current iterator as 'current' argument. Pass in 0 to get the first created
 *GEM port in the system.
 *Returns 0 if there is no next GEM port */
BCM_GemPortInfo* bcm_ploamPortCtrlGetNextGemPort(IN BCM_GemPortInfo* cur);

/*Create the OMCI port. A newly created OMCI port is enabled, not encrypted by default.
 *Returns nonzero if an OMCI port is already created.*/
int bcm_ploamPortCtrlCreateOmciPort(uint16_t portId);
int bcm_ploamPortCtrlRemoveOmciPort(void);

/*Return non-zero if OMCI port is not created.
 *The omciPortInfo output argument is always filled in.
 *If the OMCI port is removed, the previously configured OMCI port ID is returned.
 *If there is no previously configured OMCI port ID, BCM_PLOAM_GEM_PORT_ID_UNASSIGNED
 *is returned.*/
int bcm_ploamPortCtrlGetOmciPortInfo(OUT BCM_OmciPortInfo* omciPortInfo);
int bcm_ploamPortCtrlEncryptOmciPort(bool encrypt);

/* Return true if Gem Port at given index is created & HwEnabled, otherwise false */
bool bcm_ploamPortCtrlIsGemHwEnabled(uint16_t gemPortIndex);

int bcm_ploamGetGemMibIdxRefCount(uint32_t idx);
int bcm_ploamGetGemMibIdx(uint16_t gemPortIdx, uint16_t *gemMibIdx);


void GponDrvIndicationCallback(rdpa_pon_indication xi_indication_type,
                               rdpa_callback_indication indParam);

int bcm_cfgGemPort(BCM_Ploam_CfgGemPortInfo* cfgGemPortInfop);


#endif /*BCM_PLOAM_PORT_CTRL_H*/

