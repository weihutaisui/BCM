//
// <:copyright-BRCM:2014:proprietary:epon
// 
//    Copyright (c) 2014 Broadcom 
//    All Rights Reserved
// 
//  This program is the proprietary software of Broadcom and/or its
//  licensors, and may only be used, duplicated, modified or distributed pursuant
//  to the terms and conditions of a separate, written license agreement executed
//  between you and Broadcom (an "Authorized License").  Except as set forth in
//  an Authorized License, Broadcom grants no license (express or implied), right
//  to use, or waiver of any kind with respect to the Software, and Broadcom
//  expressly reserves all rights in and to the Software and all intellectual
//  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
//  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
//  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
// 
//  Except as expressly set forth in the Authorized License,
// 
//  1. This program, including its structure, sequence and organization,
//     constitutes the valuable trade secrets of Broadcom, and you shall use
//     all reasonable efforts to protect the confidentiality thereof, and to
//     use this information only in connection with your use of Broadcom
//     integrated circuit products.
// 
//  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
//     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
//     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
//     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
//     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
//     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
//     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
//     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
//     PERFORMANCE OF THE SOFTWARE.
// 
//  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
//     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
//     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
//     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
//     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
//     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
//     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
//     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
//     LIMITED REMEDY.
// :>
//


#if !defined(DataPathRules_h)
#define DataPathRules_h
////////////////////////////////////////////////////////////////////////////////
/// \file DataPathRules.h
/// \brief frame processing OAM handlers
///
////////////////////////////////////////////////////////////////////////////////


#include "Teknovus.h"
#include "Oam.h"
#include "OamUtil.h"
#include "TkPlatform.h"
#include "rdpactl_api.h"
#include "EponDevInfo.h"

#define OAM_VLAN_CTL
//#define OAM_VLAN_CTL_EXTEND
#ifdef EPON_SFU
#define OAM_RDPA
#endif

#define MAX_RX_ONLY_LINK_NUM_1G    8
#ifdef CONFIG_BCM96858
#define MAX_RX_ONLY_LINK_NUM_10G   16
#define MAX_LINK_NUM_10G           32
#define MAX_LINK_NUM_1G            16
#define MAX_LINK_NUM               MAX_LINK_NUM_10G
#else
#define MAX_RX_ONLY_LINK_NUM_10G   0
#define MAX_LINK_NUM_10G           0
#define MAX_LINK_NUM_1G            24
#define MAX_LINK_NUM               MAX_LINK_NUM_1G
#endif


#define GET_GENERAL_CLAUSE(x)        (x->clauseContent.generalClause)
#define GET_GENERAL_CLAUSE_MSB(x) GET_GENERAL_CLAUSE(x).msbMask
#define GET_GENERAL_CLAUSE_LSB(x) GET_GENERAL_CLAUSE(x).lsbMask
#define GET_GENERAL_CLAUSE_OP(x)  GET_GENERAL_CLAUSE(x).op
#define GET_GENERAL_CLAUSE_VALUE_LENGTH(x) GET_GENERAL_CLAUSE(x).matchValLen
#define GET_GENERAL_CLAUSE_VALUE_BYTE(x) GET_GENERAL_CLAUSE(x).matchVal.u8[0]
#define GET_GENERAL_CLAUSE_VALUE_RIGHT_BYTE(x) \
        GET_GENERAL_CLAUSE(x).matchVal.u8[GET_GENERAL_CLAUSE_VALUE_LENGTH(x)-1]
#define COPY_GENERAL_CLAUSE_VALUE_ALL_DATA(x, y, z) \
        do {memset((U8*)y, 0, sizeof(*y)); \
            memcpy((U8*)y+sizeof(*y)-z, &GET_GENERAL_CLAUSE(x).matchVal.u8[0], z);\
                }while(0);
#define COPY_GENERAL_CLAUSE_VALUE_RIGHT_BYTE(x, y) \
        do {memset((U8*)y, 0, sizeof(*y)); \
            memcpy(y, &GET_GENERAL_CLAUSE(x).matchVal.u8[GET_GENERAL_CLAUSE_VALUE_LENGTH(x)-1], 1);\
                }while(0);
#define GET_GENERAL_CLAUSE_VALUE_WORD(x)  GET_GENERAL_CLAUSE(x).matchVal.word[0]
#define COPY_GENERAL_CLAUSE_VALUE_RIGHT_WORD(x, y)  \
        do {memset((U8*)y, 0, sizeof(*y)); \
            memcpy(y, &GET_GENERAL_CLAUSE(x).matchVal.u8[GET_GENERAL_CLAUSE_VALUE_LENGTH(x)-2], 2);\
                }while(0);
#define GET_GENERAL_CLAUSE_VALUE_DWORD(x) GET_GENERAL_CLAUSE(x).matchVal.dword[0]
#define COPY_GENERAL_CLAUSE_VALUE_RIGHT_DWORD(x, y) \
        do {memset((U8*)y, 0, sizeof(*y)); \
            memcpy(y, &GET_GENERAL_CLAUSE(x).matchVal.u8[GET_GENERAL_CLAUSE_VALUE_LENGTH(x)-4], 4);\
                }while(0);

#define PBIT_START_OFFSET        13
#define PBIT_MASK                0x07
#define VID_MASK                 0x0FFF
#define GET_GENERAL_CLAUSE_PBIT(x)        \
        ((GET_GENERAL_CLAUSE(x).matchValLen==1) ? \
        (GET_GENERAL_CLAUSE_VALUE_BYTE(x)) >> (PBIT_START_OFFSET-GET_GENERAL_CLAUSE(x).lsbMask))

#define COPY_GENERAL_CLAUSE_PBIT_BY_WORD(x,y)        \
        do{*x = *x>>5; memcpy(y, (U8*)x, 1);        \
                }while(0)
        
#define GET_RESULT_FIELD(x)        (x->param.field)
#define GET_RESULT_FIELD_VALUE_LENGTH(x) (x-4)
#define GET_RESULT_FIELD_VALUE_BYTE(x) GET_RESULT_FIELD(x).actionVal.u8[0]
#define GET_RESULT_FIELD_VALUE_RIGHT_BYTE(x, y) \
        GET_RESULT_FIELD(x).actionVal.u8[y-1]
#define COPY_RESULT_FIELD_VALUE_RIGHT_BYTE(x, y, z) \
        do{memset((U8*)z, 0, sizeof(*z)); \
            memcpy(z, &(GET_RESULT_FIELD(x).actionVal.u8[y-1]), 1);\
                }while(0)
#define COPY_RESULT_FIELD_VALUE_ALL_DATA(x, y, z) \
        do {memset((U8*)z, 0, sizeof(*z)); \
            memcpy((U8*)z+sizeof(*z)-y, &(GET_RESULT_FIELD(x).actionVal.u8[0]), y);\
                }while(0);
#define GET_RESULT_FIELD_VALUE_WORD(x)  GET_RESULT_FIELD(x).actionVal.word[0]
#define COPY_RESULT_FIELD_VALUE_RIGHT_WORD(x, y, z)  \
        do{memset((U8*)z, 0, sizeof(*z)); \
            memcpy(z, &(GET_RESULT_FIELD(x).actionVal.u8[y-2]), 2);\
                }while(0)
#define GET_RESULT_FIELD_VALUE_DWORD(x) GET_RESULT_FIELD(x).actionVal.dword[0]
#define COPY_RESULT_FIELD_VALUE_RIGHT_DWORD(x, y, z) \
        do{memset((U8*)z, 0, sizeof(*z)); \
            memcpy(z, &(GET_RESULT_FIELD(x).actionVal.u8[y-4]), 4);\
                }while(0)


#ifdef OAM_RDPA
#define MAX_DEV_PRTY                63
#endif

//extend to 6 beside max vlan tag number for special use.
#define MAX_VLANCTL_RULES_PER_CLASS_RULE_1   6

#define TABLE_IDX_INVALID 0xFF

#define OAM_CLAUSE_MAX        6
#define OAM_RESULT_MAX        6

#define CustomFieldMaxNum     8

#ifdef OAM_VLAN_CTL
#define VLAN_CTL_SVLAN_TAGS        0
#define VLAN_CTL_CVLAN_TAGS        1
#endif

typedef enum
    {
    OamFieldLinkIndex       = 0x00,
    OamFieldL2Da            = 0x01,
    OamFieldL2Sa            = 0x02,
    OamFieldL2Type          = 0x03,
    OamFieldBDa             = 0x04,
    OamFieldBSa             = 0x05,
    OamFieldITag            = 0x06,
    OamFieldSVlan           = 0x07,
    OamFieldCVlan           = 0x08,
    OamFieldMpls            = 0x09,
    OamFieldIpv4Tos         = 0x0A,
    OamFieldIpv4Ttl         = 0x0B,
    OamFieldIpProtocolType  = 0x0C,
    OamFieldIpv4Sa          = 0x0D,
    OamFieldIpv6Sa          = 0x0E,
    OamFieldIpv4Da          = 0x0F,
    OamFieldIpv6Da          = 0x10,
    OamFieldIpv6NextHeader  = 0x11,
    OamFieldIpv6FlowLabel   = 0x12,
    OamFieldTcpUdpSrcPort   = 0x13,
    OamFieldTcpUdpDestPort  = 0x14,
    OamFieldBTag            = 0x15,
    OamFieldReserved1       = 0x16,
    OamFieldReserved2       = 0x17,
    OamFieldCustom0         = 0x18,
    OamFieldCustom1         = 0x19,
    OamFieldCustom2         = 0x1A,
    OamFieldCustom3         = 0x1B,
    OamFieldCustom4         = 0x1C,
    OamFieldCustom5         = 0x1D,
    OamFieldCustom6         = 0x1E,
    OamFieldCustom7         = 0x1F,
    OamFieldReservedForDPoG = 0x20,
    OamFieldLlidValue       = 0x21,
    OamFieldCodeMax
    }PACK OamFieldCode;

typedef enum
    {
    OamResultNop        = 0x00,
    OamResultDiscard    = 0x01,
    OamResultForward    = 0x02,
    OamResultQueue      = 0x03,
    OamResultSet        = 0x04,
    OamResultCopy       = 0x05,
    OamResultDelete     = 0x06,
    OamResultInsert     = 0x07,
    OamResultReplace    = 0x08,
    OamResultClearDel   = 0x09,
    OamResultClearIns   = 0x0A,

    OamResultCodeMax
    } PACK OamResultCode;

typedef enum
    {
    OamObjCtxtOnu           = 0x0000,
    OamObjCtxtNetPon        = 0x0001,
    OamObjCtxtLink          = 0x0002,
    OamObjCtxtUserPort      = 0x0003,
    OamObjCtxtQueue         = 0x0004,

    OamObjCtxtForce16       = 0x7FFF
    } PACK OamObjCtxt;

typedef enum
    {
    OamLayerL2          = 0,
    OamLayer802Dot1ah   = 1,
    OamLayerEtherType   = 2,
    OamLayerSVlan       = 3,
    OamLayerCVlan       = 4,
    OamLayerMpls        = 5,
    OamLayerIPv4        = 6,
    OamLayerIPv6        = 7,
    OamLayerL3          = 8,
    OamLayerTcpUdp      = 9,
    OamLayerL4          = 10,

    OamLayerCount
    } PACK OamCustomLayer;

typedef struct
    {
    U16     tpid;
    BOOL    insert;
    } PACK OamAltVlan;

#define INVALID_PRI_IDX 0xFF
typedef struct
    {
    U8 curIdx;
    U8 oamPriority;
    U8 next;
    } PACK RuleIndexEntry;

typedef struct 
    {
    U8 valid;
    U8 extend;
    vlanCtl_direction_t dir;
    unsigned int nbrOfTags;
    unsigned int tagRuleId;
    } PACK OamVlanCtlData;

typedef union
    {
    U8      u8[16]; 
    U16     word[8];
    U32     dword[4]; 
    } PACK OamMatchValue;

typedef struct
    {
    U8                  instance;
    U8                  msbMask;
    U8                  lsbMask;
    OamRuleOp           op;
    U8                  matchValLen;
    OamMatchValue       matchVal;
    } PACK OamGeneralClause;

typedef union
    {
    OamGeneralClause        generalClause;
//    OamCustomClause        customClause;
    } PACK OamClauseData;

typedef struct
    {
    OamFieldCode        fieldCode;
    OamClauseData       clauseContent;   
    } PACK OamRuleClauseData;

typedef struct
    {
    OamObjCtxt      type;
    U8              inst;
    U8              queue;
    } PACK OamQueue;

typedef union
    {
    U8      u8[16]; 
    U16     word[8];
    U32      dword[4]; 
    } PACK OamActionValue;

typedef struct
    {
    OamFieldCode    fldCode;
    U8              instance;
    U8              msbMask;
    U8              lsbMask;
    OamActionValue  actionVal;
    } PACK OamField;

typedef union
    {
    OamQueue        dest;
    OamField        field;
    } PACK OamResultParam;

typedef struct
    {
    OamResultCode   result;
    OamResultParam  param;
    } PACK OamRuleResultData;

typedef struct
    {
    U8                        length;
    OamRuleClauseData         clause;
    } PACK OamRuleClauseSaving;

typedef struct
    {
    U8                        length;
    OamRuleResultData        result;
    } PACK OamRuleResultSaving;

// These unpacked structures are used internally for tracking purposes
// Used to track rule metadata during installation/removal
typedef struct
    {
    U8                      flags;       // Currently only Volatile Rule bit 0
    U8                      port;        //for Pon or Uni index
    U8                      priority;       // Priority of the rule
    BOOL                    isValid;        // TRUE as long as everything is OK
    BOOL                    isTerminated;   // Is the rule totally defined?
    BOOL                    isAlways;       // Does rule have an always clause?
    BOOL                    isDiscard;      // Does rule discard action
    BOOL                    isGreaterLess;  // Does the rule have <= and <= operators
    U16                     range;          // range limted by Greater and Less than
    OamRuleClauseSaving        clauseData[OAM_CLAUSE_MAX];
    OamRuleResultSaving        resultData[OAM_RESULT_MAX];
    U8                      clauseCount;    // Number of clauses
    U8                      resultCount;    // Number of results
    } PACK RuleDesc;

typedef struct
    {
    OamCustomLayer      layerSelect;
    U8                  dwordOffset;
    U8                  lsb;
    U8                  bitWidth;
    } PACK OamCustomField;

typedef enum
    {
    OamErrNoError       = 0x80,
    OamErrTooLong       = 0x81,
    OamErrBadParams     = 0x86,
    OamErrNoResources   = 0x87,
    OamErrSystemBusy    = 0x88,
    OamErrUndetermined  = 0xA0,
    OamErrUnsupported   = 0xA1,
    OamErrCorrupted     = 0xA2,
    OamErrHwFailure     = 0xA3,
    OamErrOverflow      = 0xA4
    } OamError;

#define UNI_UP_TABLE_MAX    64
#define PON_DN_TABLE_MAX    64

typedef struct
    {
    U8        index;
#ifdef OAM_RDPA
    U8        devPrty;                //priority in hardware
    U8        devCalcPrty;          //priority by calculation
#endif
#ifdef OAM_VLAN_CTL
    U8        destPort;
    OamVlanCtlData        vlanCtlInfo[MAX_NUM_UNI_PORTS][MAX_VLANCTL_RULES_PER_CLASS_RULE_1];
#endif
    } PACK ruleLocalPrtyInfo;

typedef struct
    {
    OamCustomField  oamCustomCfg;
    rdpactl_ic_gen_rule_cfg_t customFieldRdpaInfo;
    uint32_t fieldMask;
    uint16_t matchCount;
    uint16_t matchValue[2];
    }rdpaCustomFieldInfo;

#define DN_DIR_ALL  0xFF

#define INVALID_BYTE        0xFF
#define INVALID_WORD        0xFFFF


#define TotalPonPortCount        1
#define TotalPortCount          (MAX_NUM_UNI_PORTS+TotalPonPortCount)

extern Bool RuleIsValid(RuleDesc* pRule);

extern OamError OamAddRuleApply(RuleDesc* pRule, ruleLocalPrtyInfo *infoPtr);
extern OamError OamDelRuleApply(RuleDesc* pRule, ruleLocalPrtyInfo *infoPtr);
extern void OamAddCustomField(U8 idx, OamCustomField *customField);

#ifdef OAM_VLAN_CTL
extern char* OamVlanCtlGetTxDevName(U8 port);
extern char* OamVlanCtlGetRxDevName(U8 port);
extern void OamVlanCtlVlanAllSet(U16 sTpid, U16 cTpid);
extern void OamVlanCtlModeAllSet(void);
extern void OamVlanctlFrmProcRuleClr(void);
#endif

// Vlan Tpid Set/Get
extern void OamAltCVlanEthertypeGet(BOOL *insert,  U16* tpid);
extern U16 OamAltCVlanTpidGet(void);
extern void OamAltSVlanEthertypeGet(BOOL *insert,  U16* tpid);
extern U16 OamAltSVlanTpidGet(void);
extern void OamAltSVlanTpidSet(U16 sTpid, BOOL insert);
extern void OamAltCVlanTpidSet(U16 cTpid, BOOL insert);

////////////////////////////////////////////////////////////////////////////////
/// \brief  get rule list
///
/// \param 
///
/// \return 
////////////////////////////////////////////////////////////////////////////////
extern 
RuleIndexEntry* GlobalRuleListEntryByPortIndex(U8 port, U8 idx);


////////////////////////////////////////////////////////////////////////////////
/// \brief  get rule list
///
/// \param 
///
/// \return 
////////////////////////////////////////////////////////////////////////////////
extern 
RuleIndexEntry *GlobalRuleListEntryNextByPortIndex(U8 port, U8 index);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Get rule description
///
/// \param 
///
/// \return 
////////////////////////////////////////////////////////////////////////////////
extern
RuleDesc* GlobalRuleTableByPortIndex(U8 port, U8 idx);

extern
RuleIndexEntry *GetRuleListHeadEntryByPortIndex(U8 port);

extern
void SetRuleListHeadEntryByPortIndex(U8 port, RuleIndexEntry *pIdxEntry);

extern
RuleDesc* GlobalRuleTableRequestByPortPrty(U8 port, U8 priority, U8 *index);
    
extern 
void ClearAllOamRules(void);

extern 
void OamAltVlanInit(void);

extern
void ruleLocalPrtyInfoInit(ruleLocalPrtyInfo *infoPtr);
    
extern 
void LocalPrtyInfoInit(void);    

extern
ruleLocalPrtyInfo *GetPlatformInfo(U8 port, U16 index);

extern
void OamVlanctlFrmProcRuleInit(U8 link, BOOL add);

extern 
BOOL RuleListEmptyCheck(void);

extern 
void OamGetVlanFromClause(OamRuleClauseData *pClause,U8 valLen, U16* pVlanId, U8* pVlanPbit, U16* pVlanTpid);

extern
void OamGetVlanFromResult(OamRuleResultData *pResult,U8 valLen, U16* pVlanId, U8* pVlanPbit, U16* pVlanTpid);

////////////////////////////////////////////////////////////////////////////////
/// \brief  Initialize DPoE OAM frame porcessing data path
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern 
void OamDataPathInit(void);

////////////////////////////////////////////////////////////////////////////////
/// \brief  Is a link associated with physical/virtual port.
///
/// \param 
///          linkIdx:  Link index
/// \return
///          Ture/False
////////////////////////////////////////////////////////////////////////////////
extern 
BOOL isVirtualLink(U8 linkIdx);    
#endif // DataPathRules_h.h

