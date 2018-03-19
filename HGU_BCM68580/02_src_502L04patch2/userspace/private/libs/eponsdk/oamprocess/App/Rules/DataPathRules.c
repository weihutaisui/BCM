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


#include "cms_util.h"
#include <string.h>
#include "Teknovus.h"
#include "stdio.h"
#include "Ethernet.h"
#include "vlanctl_api.h"
#include "rdpactl_api.h"
#include "DataPathRules.h"
#include "ApiResLayer.h"
#include "MultiByte.h"
#include "TkSdkInitApi.h"
#include "EponDevInfo.h"
#include "ethswctl_api.h"
#include "skb_defines.h"
#include "Build.h"
#include "PonConfigDb.h"
#include "UniConfigDb.h"
#include "eponctl_api.h"
#if GLOBAL_DN_SHAPER
#ifdef BCM_OAM_BUILD
#include "OamShaper.h"
#endif
#endif

#ifdef OAM_VLAN_CTL
typedef struct
    {
    U8 nTags;
    U8 destPort;
    vlanCtl_direction_t dir;
    } VlanctlRuleLookUpPara;
#endif

typedef struct
    {
    U8  svlanPbit;
    U16 svlanVid;
    U16 svlanTpid;
    U8  cvlanPbit;
    U16 cvlanVid;
    U16 cvlanTpid;
    } VlanctlVlanActionPara;

typedef struct
    {
    /* VLAN position, each byte indicates if this vlan is present */
    /* S vlan 0 | S vlan 1 | C vlan 0 | C vlan 1 */
    U32 vlanStackInfo;
    U8  tagsNum;   /* Vlan number */
    } VlanClauseField;

static OamAltVlan useAltCvlan;
static OamAltVlan useAltSvlan;

//#define OAM_DUMP_RULES      1
#define OAM_CUSTOMFIELD_DBUG 0

#define MAX_VLAN_INSTANCES          4
#define SVLAN_0_INST_ID             0
#define SVLAN_1_INST_ID             1
#define CVLAN_0_INST_ID             2
#define CVLAN_1_INST_ID             3

#define SVLAN_0_BIT  0x01000000
#define SVLAN_1_BIT  0x00010000
#define CVLAN_0_BIT  0x00000100
#define CVLAN_1_BIT  0x00000001

#define SVLAN_0_PRESENT(a)      (((a)&SVLAN_0_BIT) != 0)
#define SVLAN_1_PRESENT(a)      (((a)&SVLAN_1_BIT) != 0)
#define CVLAN_0_PRESENT(a)      (((a)&CVLAN_0_BIT) != 0)
#define CVLAN_1_PRESENT(a)      (((a)&CVLAN_1_BIT) != 0)
#define SVLAN_PRESENT(a)        (((a)&(SVLAN_0_BIT|SVLAN_1_BIT)) != 0)
#define CVLAN_PRESENT(a)        (((a)&(CVLAN_0_BIT|CVLAN_1_BIT)) != 0)

//database for rule configuration
RuleDesc globalUpRuleTable[MAX_NUM_UNI_PORTS][UNI_UP_TABLE_MAX];
RuleDesc globalDnRuleTable[PON_DN_TABLE_MAX];

ruleLocalPrtyInfo ruleUpLocalPrtyInfoTable[MAX_NUM_UNI_PORTS][UNI_UP_TABLE_MAX];
ruleLocalPrtyInfo ruleDnLocalPrtyInfoTable[PON_DN_TABLE_MAX];

rdpaCustomFieldInfo customFieldRdpaCfg[CustomFieldMaxNum] = {};

//for rule with same priority
RuleIndexEntry *priListHeader[TotalPortCount];
RuleIndexEntry globalUpPriList[MAX_NUM_UNI_PORTS][UNI_UP_TABLE_MAX];
RuleIndexEntry globalDnPriList[PON_DN_TABLE_MAX];
static BOOL g_isVirtualLink[MAX_LINK_NUM];

/* Supported vlan selections */
const VlanClauseField globalVlanFields[] =
    {        
        /* C vlan 1             : 2 tags, inner C tag */
        {0x00000001, 2},

        /* C vlan 0             : 1 tag, outer C tag */
        {0x00000100, 1},

        /* C vlan 0, C vlan 1   : 2 tag, outer C tag, inner C tag */
        {0x00000101, 2},

        /* S vlan 1             : 2 tag, inner S tag */
        {0x00010000, 2},

        /* S vlan 0             : 1 tag, outer S tag */        
        {0x01000000, 1},

        /* S vlan 0, C vlan 0   : 2 tag, outer S tag, inner C tag */        
        {0x01000100, 2},

        /* S vlan 0, S vlan 1   : 2 tag, outer S tag, inner S tag */        
        {0x01010000, 2}
    };

#define VLAN_CLAUSE_SUPPORTED_CASES   (sizeof(globalVlanFields)/sizeof(VlanClauseField)) /* Vlan combinations */


/* qosManagedByOAM:
TRUE: qos is managed by OAM
FALSE: qos is managed by other management domain, like TR069
Now qosManagedByOAM is TRUE in EPON SFU, and false in EPON HGU.
*/
static BOOL qosManagedByOAM;

#ifdef OAM_DUMP_RULES
static char *clauseName[OamFieldCodeMax] = 
    {
    "LinkIndex",
    "L2Da",
    "L2Sa",
    "L2Type",
    "BDa",
    "BSa",
    "ITag",
    "SVlan",
    "CVlan",
    "Mpls",
    "Ipv4Tos",
    "Ipv4Ttl",
    "IpProtocolType",
    "Ipv4Sa",
    "Ipv6Sa",
    "Ipv4Da",
    "Ipv6Da",
    "Ipv6NextHeader",
    "Ipv6FlowLabel",
    "TcpUdpSrcPort",
    "TcpUdpDestPort",
    "BTag",
    "Reserved1",
    "Reserved2",
    "Custom0",
    "Custom1",
    "Custom2",
    "Custom3",
    "Custom4",
    "Custom5",
    "Custom6",
    "Custom7"
    };
    
static char *opName[OamRuleOpNumOps] = 
    {
    "Never",
    "Equal",
    "NotEqual",
    "LessOrEqual",
    "GreaterOrEqual",
    "Exists",
    "NotExists",
    "Always"
    };

static char *resultName[OamResultCodeMax] = 
    {
    "Nop",
    "Discard",
    "Forward",
    "Queue",
    "Set",
    "Copy",
    "Delete",
    "Insert",
    "Replace",
    "ClearDel",
    "ClearIns"
    };

static void OamDumpClause(OamRuleClauseData* pClause)
    {
    U8 i;
    printf("\tField:\t\t%d, %s\n", pClause->fieldCode, clauseName[pClause->fieldCode]);
    printf("\tInstance:\t%d\n", pClause->clauseContent.generalClause.instance);
    printf("\tmsbMask:\t%d\n", pClause->clauseContent.generalClause.msbMask);
    printf("\tlsbMask:\t%d\n", pClause->clauseContent.generalClause.lsbMask);
    printf("\top:\t\t%d, %s\n", pClause->clauseContent.generalClause.op, opName[pClause->clauseContent.generalClause.op]);
    printf("\tvalLen:\t%d\n", pClause->clauseContent.generalClause.matchValLen);
    printf("\tvalue:\t\t");
    for (i = 0; i < pClause->clauseContent.generalClause.matchValLen; i ++)
        printf("%02x ", pClause->clauseContent.generalClause.matchVal.u8[i]);
    printf("\n\n");
    }

static void OamDumpResult(U8 length, OamRuleResultData* pResult)
    {
    U8 i;
    printf("Result:\t%d, %s\n", pResult->result, resultName[pResult->result]);
    if (pResult->result == OamResultQueue)
        {
        printf("\tQtype:\t\t%d\n", pResult->param.dest.type);
        printf("\tQinst:\t\t%d\n", pResult->param.dest.inst);
        printf("\tQindex:\t%d\n\n", pResult->param.dest.queue);
        }
    else if ((pResult->result == OamResultSet) ||
                (pResult->result == OamResultCopy) ||
                (pResult->result == OamResultDelete) ||
                (pResult->result == OamResultInsert) ||
                (pResult->result == OamResultReplace))
        {
        printf("\tField:\t\t%d\n", pResult->param.field.fldCode);
        printf("\tInstance:\t%d\n", pResult->param.field.instance);
        printf("\tvalLen:\t\t%d\n", length-3);
        printf("\tvalue:\t\t");
        for (i = 0; i < (length-3); i ++)
            printf("%02x ", pResult->param.field.actionVal.u8[i]);
        printf("\n\n");
        }
    }
/*
static void PrintGlobalRuleListByPort(U8 port)
    {//lookup the tablelist, find the right place
    RuleIndexEntry *tmpEntry = priListHeader[port]; //the buffer for previous entry
    if (tmpEntry == NULL)
        {
        printf("empty table for port %d\n", port);
        return;
        }
    
    printf("Index\tPriority\tNext\n");
    do {
        printf("%d\t%d\t%d\n", tmpEntry->curIdx, tmpEntry->oamPriority, tmpEntry->next);
        if(tmpEntry->next == INVALID_PRI_IDX)
            break;
        
        tmpEntry = GlobalRuleListEntryByPortIndex(port, tmpEntry->next);
        }while(1);
    }*/
#endif

static void PrintLocalPrtyInfoByIdxPortList(U8 port, U8 idx)
    {
#ifdef OAM_DUMP_RULES
    U8 i, j;
    ruleLocalPrtyInfo *list = GetPlatformInfo(port,idx);

    printf("Port %d, idx %d list:\n", port, idx);
    for(i=0; i<4; i++)
        {
        printf("item %d\n", i);
        printf("index:\t%d\tDestPort:\t%d\n", list->index, list->destPort);
        printf("\tUNI port %d\n",i);
        for(j=0;j<6;j++)
            {
            if(list->vlanCtlInfo[i][j].valid == 0)
                continue;
            printf("\t\tvlanctl dir %d, tags %d, ruleId %d\n",
                list->vlanCtlInfo[i][j].dir,
                list->vlanCtlInfo[i][j].nbrOfTags,
                list->vlanCtlInfo[i][j].tagRuleId);
            } 
        printf("\n");
        }
#endif
    }

static void leftShift(U8 *input, U16 len, U8 shiftVal, U8 *output)
    {
    int i, offset = len - 1, last;
    U8 overflowed, next, bitShift = 0, byteShift = 0;
    U8 tmpBuf[64] = {0};
    
    byteShift = shiftVal/8;
    bitShift = shiftVal%8;
    memset(output, 0, len);
    
    for (i = 0 ; i < (len - byteShift); i++)
        tmpBuf[i] = input[i + byteShift];

    last = (U8) (((tmpBuf[len - 1]&0xff) << bitShift));  
    output[offset--] = last;  
    for (i = len-1 ; i > 0 ; i--)
        {  
        overflowed = (U8) ((tmpBuf[i]&0xff) >> (8-bitShift));  
        next = (U8) (overflowed | ((tmpBuf[i-1]&0xff)<<bitShift));  
          
        output[offset--] = next;  
        }  
    }

////////////////////////////////////////////////////////////////////////////////
/// \brief  Is the current rule valid
///
/// \return
/// TRUE if rule is valid
////////////////////////////////////////////////////////////////////////////////
Bool RuleIsValid(RuleDesc* pRule)
    {
    return (pRule->isValid && 
                pRule->isTerminated && 
                pRule->clauseCount && 
                pRule->resultCount);
    }

RuleDesc* GlobalRuleTableByPortIndex(U8 port, U8 idx)
    {
    return ((port==0) ? (&globalDnRuleTable[idx]) : (&globalUpRuleTable[port-1][idx]));
    }


RuleIndexEntry* GlobalRuleListEntryByPortIndex(U8 port, U8 idx)
    {
    return ((port == 0) ? &globalDnPriList[idx] : &globalUpPriList[port-1][idx]);
    }

RuleIndexEntry *GlobalRuleListEntryNextByPortIndex(U8 port, U8 index)
    {
    // lookup the next listentry by index
    RuleIndexEntry *tmpEntry;

    tmpEntry = GlobalRuleListEntryByPortIndex(port, index);
    return ((tmpEntry->next == INVALID_PRI_IDX) ? 
        NULL : GlobalRuleListEntryByPortIndex(port, tmpEntry->next));
    }

RuleIndexEntry *GetRuleListHeadEntryByPortIndex(U8 port)
    {
    return priListHeader[port];
    }

void SetRuleListHeadEntryByPortIndex(U8 port, RuleIndexEntry *pIdxEntry)
    {
    priListHeader[port] = pIdxEntry;
    }

static U8 GlobalRuleCountByPort(U8 port)
    {
    return ((port == 0) ? PON_DN_TABLE_MAX : UNI_UP_TABLE_MAX);
    }

RuleDesc* GlobalRuleTableRequestByPortPrty(U8 port, U8 priority, U8 *index)
    {
    //find the associated ruletable according to the priority
    //lookup RuleIndexEntry table
    U8 i;
    U8 freeTableIdx = INVALID_PRI_IDX;
    RuleIndexEntry *iEntry = NULL;
    RuleIndexEntry *tmpEntry;
    
    if (GetRuleListHeadEntryByPortIndex(port) == NULL)
        {//the table is empty, the first table is for using
        tmpEntry = GlobalRuleListEntryByPortIndex(port, 0);
        SetRuleListHeadEntryByPortIndex(port, tmpEntry);
        tmpEntry->curIdx = 0;
        tmpEntry->oamPriority = priority;
        tmpEntry->next = INVALID_PRI_IDX;
        *index = 0;
        return GlobalRuleTableByPortIndex(port, 0);
        }

    //find a empty table
    freeTableIdx = INVALID_PRI_IDX;
    for (i = 0; i < GlobalRuleCountByPort(port); i ++)
        {
        iEntry = GlobalRuleListEntryByPortIndex(port, i);
        if (iEntry->oamPriority == INVALID_PRI_IDX)
            {//a empty table found
            freeTableIdx = i;
            iEntry->oamPriority = priority;
            iEntry->curIdx = freeTableIdx;
            break;
            }
        }
    
    if (freeTableIdx == INVALID_PRI_IDX)
        {
        cmsLog_error("no vaild entry table for using\n");
        *index = INVALID_PRI_IDX;
        return NULL;
        }

    
    //the smaller or same value, the higher priority
    tmpEntry = GetRuleListHeadEntryByPortIndex(port);
    if(priority < tmpEntry->oamPriority)
        {//a higher priority, replace the header
        //new header
        SetRuleListHeadEntryByPortIndex(port, iEntry);
        iEntry->next = tmpEntry->curIdx;
        }
    else 
        {//lookup the tablelist, find the right place
        RuleIndexEntry *buffEntry = tmpEntry; //the buffer for previous entry

        while(INVALID_PRI_IDX != buffEntry->next)
            {
            tmpEntry = GlobalRuleListEntryByPortIndex(port, buffEntry->next);
            if (tmpEntry->oamPriority > priority)
                {
                //the new table should be added before current table
                iEntry->next = tmpEntry->curIdx;
                buffEntry->next = iEntry->curIdx;
                break;
                }
            
            buffEntry = tmpEntry;
            }
       
        if(INVALID_PRI_IDX == buffEntry->next)
            {
            buffEntry->next = iEntry->curIdx;
            iEntry->next = INVALID_PRI_IDX;
            }
        }

    *index = freeTableIdx;
    
    return GlobalRuleTableByPortIndex(port, freeTableIdx);
    }

////////////////////////////////////////////////////////////////////////////////
/// \brief  Get the alternate C-VLAN ethertype
///
/// \param 
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamAltCVlanEthertypeGet(BOOL *insert,  U16* tpid)
    {
    *insert = useAltCvlan.insert;
    *tpid = useAltCvlan.tpid;   
    }

U16 OamAltCVlanTpidGet(void)
    {
    return ((useAltCvlan.insert==TRUE) ? useAltCvlan.tpid : EthertypeCvlan);
    }

////////////////////////////////////////////////////////////////////////////////
/// \brief  Get the alternate S-VLAN ethertype
///
/// \param 
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamAltSVlanEthertypeGet(BOOL *insert,  U16* tpid)
    {
    *insert = useAltSvlan.insert;
    *tpid = useAltSvlan.tpid;   
    }

U16 OamAltSVlanTpidGet(void)
    {
    return ((useAltSvlan.insert==TRUE) ? useAltSvlan.tpid : EthertypeSvlan);
    }

void OamAltVlanInit(void)
    {
    useAltCvlan.insert = FALSE;
    useAltCvlan.tpid = EthertypeCvlan;
    useAltSvlan.insert = FALSE;
    useAltSvlan.tpid = EthertypeSvlan;
    }

void ClearAllOamRules(void)
    {
    // for warm init, should flash all the existed rules here
    U8 i;
    for (i = 0; i < TotalPortCount; i ++)
        priListHeader[i] = NULL;
    
    memset(globalUpRuleTable, 0, 
                    sizeof(RuleDesc)*MAX_NUM_UNI_PORTS*UNI_UP_TABLE_MAX);
    memset(globalDnRuleTable, 0, sizeof(RuleDesc)*PON_DN_TABLE_MAX); 

    memset(globalUpPriList, 0xFF, 
                    sizeof(RuleIndexEntry)*MAX_NUM_UNI_PORTS*UNI_UP_TABLE_MAX);
    memset(globalDnPriList, 0xFF, sizeof(RuleIndexEntry)*PON_DN_TABLE_MAX);

    memset(g_isVirtualLink, 0, sizeof(g_isVirtualLink));
    }

void ruleLocalPrtyInfoInit(ruleLocalPrtyInfo *infoPtr)
    {
    memset(infoPtr, 0, sizeof(ruleLocalPrtyInfo));
#ifdef OAM_RDPA
    infoPtr->devPrty = 0xFF;
    infoPtr->devCalcPrty = 0xFF;
#endif
    }

////////////////////////////////////////////////////////////////////////////////
/// \brief 
///
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
//extern
void LocalPrtyInfoInit(void)
    {
    U8         i,j;

    for (j = 0; j < UNI_UP_TABLE_MAX; j ++)
        {
        //init for uni
        for (i = 0; i < MAX_NUM_UNI_PORTS; i ++)
            {
            ruleLocalPrtyInfoInit(&ruleUpLocalPrtyInfoTable[i][j]);
            }
        }
    
    for (j = 0; j < PON_DN_TABLE_MAX; j ++)
        {
        //init for pon
        ruleLocalPrtyInfoInit(&ruleDnLocalPrtyInfoTable[j]);
        }
    }

ruleLocalPrtyInfo *GetPlatformInfo(U8 port, U16 index)
    {
        if(port == 0)
            return &ruleDnLocalPrtyInfoTable[index];
        else
            return &ruleUpLocalPrtyInfoTable[port-1][index];
    }


BOOL RuleListEmptyCheck(void)
    {
    U8 i;
    for (i = 0; i < TotalPortCount; i++)
        {
        if (NULL != priListHeader[i])
            return FALSE;
        }
    return TRUE;
    }

#ifdef OAM_VLAN_CTL
////////////////////////////////////////////////////////////////////////////////
/// \brief  
///
/// \param reply    Container for response
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
void OamVlanCtlModeAllSet(void)
    {
    U8 i;

    vlanCtl_init();
    for (i = 0; i < 3; i ++)
        {
        vlanCtl_setDefaultAction(eponRealIfname, VLANCTL_DIRECTION_TX, i, VLANCTL_ACTION_DROP, NULL);
        }
    vlanCtl_cleanup();
    }

#define S_TAG_TPID 0x88A8
#define C_TAG_TPID 0x8100
////////////////////////////////////////////////////////////////////////////////
/// \brief  
///
/// \param reply    Container for response
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
void OamVlanCtlVlanAllSet(U16 sTpid, U16 cTpid)
    {
    U8 i;
    U32 tpidTable[BCM_VLAN_MAX_TPID_VALUES];

    // setup TPID table
    tpidTable[0] = sTpid;
    tpidTable[1] = cTpid;
    tpidTable[2] = S_TAG_TPID;
    tpidTable[3] = C_TAG_TPID;

    vlanCtl_init();
    for (i = 0; i < UniCfgDbGetOamManagedPortCount() + 1; i ++)
        {
        vlanCtl_setTpidTable(OamVlanCtlGetTxDevName(i), tpidTable);
        }
    vlanCtl_cleanup();
    }

#define VLAN_CTL_FILTER_SVLAN_INDEX        0
#define VLAN_CTL_FILTER_CVLAN_INDEX        1
#endif

////////////////////////////////////////////////////////////////////////////////
/// \brief  Set the alternate C-VLAN ethertype
///
/// \return
/// Pointer to last container filled
////////////////////////////////////////////////////////////////////////////////
//extern
void OamAltCVlanTpidSet(U16 cTpid, BOOL insert)
    {    
    useAltCvlan.insert = insert;
    useAltCvlan.tpid = cTpid;
    
#ifdef OAM_VLAN_CTL
    OamVlanCtlVlanAllSet(useAltSvlan.tpid, cTpid);
#endif
#ifdef OAM_RDPA
    rdpaCtl_set_detect_tpid(cTpid, TRUE);
    rdpaCtl_set_in_tpid(OamAltCVlanTpidGet());
#endif
    }

////////////////////////////////////////////////////////////////////////////////
/// \brief  Set the alternate S-VLAN ethertype
///
/// \return
/// Pointer to last container filled
////////////////////////////////////////////////////////////////////////////////
//extern
void OamAltSVlanTpidSet(U16 sTpid, BOOL insert)
    {
    useAltSvlan.insert = insert;
    useAltSvlan.tpid = sTpid;
    
#ifdef OAM_VLAN_CTL
    OamVlanCtlVlanAllSet(sTpid, useAltCvlan.tpid);
#endif
#ifdef OAM_RDPA
    rdpaCtl_set_detect_tpid(sTpid, FALSE);
    rdpaCtl_set_out_tpid((OamAltSVlanTpidGet()));
    rdpaCtl_set_always_tpid(OamAltSVlanTpidGet());
#endif
    }

////////////////////////////////////////////////////////////////////////////////
/// \brief  get ethertype from rule attribute - clause subtype
///
/// \return 
/// \output parameters
///         pEtherType which is parsed from clause
////////////////////////////////////////////////////////////////////////////////
void inline OamGetEthertypeFromClause(OamRuleClauseData *pClause,U8 valLen, U16* pEtherType)
    {																
    U16 etherType;												
    if (valLen == 1)											  
        {															  
        etherType = (U16)GET_GENERAL_CLAUSE_VALUE_BYTE(pClause);  
        }															  
    else														  
        {															  
        COPY_GENERAL_CLAUSE_VALUE_RIGHT_WORD(pClause, &etherType);
        etherType = OAM_NTOHS(etherType);						  
        }															  
    																  
    *pEtherType = etherType;									
    cmsLog_debug("ether type get from clasue: %d\n\r",*pEtherType);																	
    }	

////////////////////////////////////////////////////////////////////////////////
/// \brief  get ethertype from rule attribute - result subtype
///
/// \return 
/// \output parameters
///         pEtherType which is parsed from clause
////////////////////////////////////////////////////////////////////////////////
void inline OamGetEthertypeFromResult(OamRuleResultData *pResult,U8 valLen, U16* pEtherType)
    {																
    																
    U16 etherType;																					  
    																  
    if (valLen == 1)
        etherType = (U16)(GET_RESULT_FIELD_VALUE_BYTE(pResult));
    else
        {
        COPY_RESULT_FIELD_VALUE_RIGHT_WORD(pResult, valLen, &etherType);
        etherType = OAM_NTOHS(etherType);
        }
    *pEtherType = etherType;									
    cmsLog_debug("ether type get from result: %d\n\r",*pEtherType);
    }	

////////////////////////////////////////////////////////////////////////////////
/// \brief  get VLAN info from rule attribute - clause subtype
///
/// \return 
/// \output parameters
///         pVlanId/pVlanPbit/pVlanTpid which are parsed from clause
////////////////////////////////////////////////////////////////////////////////
void inline OamGetVlanFromClause(OamRuleClauseData *pClause,U8 valLen, U16* pVlanId, U8* pVlanPbit, U16* pVlanTpid)
    {
    
    // the length of value indicates the number of parameter
    // 3-4bytes: tpid + pbits + vid
    // 2bytes: pbits + vid
    // 1bytes: vid
    U32 tmpDword;
    U16 tmpWord;
    U16 tmpTpid;
    //get the value
    //if length is less than 4, just padding right side
    if (valLen > 4)
        {
        COPY_GENERAL_CLAUSE_VALUE_RIGHT_DWORD(pClause, &tmpDword);
        }
    else
        {
        COPY_GENERAL_CLAUSE_VALUE_ALL_DATA(pClause, &tmpDword, valLen);
        }
    memcpy((U8*)&tmpWord, &(((U8*)&tmpDword)[2]), sizeof(U16));
    tmpWord = OAM_NTOHS(tmpWord);
    
    if (GET_GENERAL_CLAUSE(pClause).lsbMask == 0)
        *pVlanId = tmpWord&VID_MASK;

    if ((GET_GENERAL_CLAUSE(pClause).msbMask >= 4) && (valLen == 2))
        {
        // do nothing on pbit
        }
    else if ((GET_GENERAL_CLAUSE(pClause).lsbMask <= 13) && 
        (valLen >= 2) && 
        (GET_GENERAL_CLAUSE(pClause).msbMask <= 16))
        {//pbits included
        *pVlanPbit = ((U8)(tmpWord>>13))&PBIT_MASK;
        }
    
    if ((GET_GENERAL_CLAUSE(pClause).msbMask == 0) && (valLen > 2))
        {//tpid included
        memcpy((U8*)&tmpTpid, (U8*)&tmpDword, sizeof(U16));
        *pVlanTpid = OAM_NTOHS(tmpTpid);
        }
    
    
    cmsLog_debug("VLAN info are tag %d, pbit %d, tpid 0x%x\n\r",*pVlanId, *pVlanPbit, *pVlanTpid);
    
    }											 

////////////////////////////////////////////////////////////////////////////////
/// \brief  get VLAN info from rule attribute - result subtype
///
/// \return 
/// \output parameters
///         pVlanId/pVlanPbit/pVlanTpid which are parsed from result
////////////////////////////////////////////////////////////////////////////////
void inline OamGetVlanFromResult(OamRuleResultData *pResult,U8 valLen, U16* pVlanId, U8* pVlanPbit, U16* pVlanTpid)
    {
    
    // the length of value indicates the number of parameter
    // 3-4bytes: tpid + pbits + vid
    // 2bytes: pbits + vid
    // 1bytes: vid
    U32 tmpDword;
    U16 tmpWord;
    U16 tmpTpid;
    
    if (valLen > 4)
        {
        COPY_RESULT_FIELD_VALUE_RIGHT_DWORD(pResult, valLen, &tmpDword);
        }
    else
        {
        COPY_RESULT_FIELD_VALUE_ALL_DATA(pResult, valLen, &tmpDword);
        }
    memcpy((U8*)&tmpWord, &(((U8*)&tmpDword)[2]), sizeof(U16));
    tmpWord = OAM_NTOHS(tmpWord);

    if (GET_RESULT_FIELD(pResult).lsbMask == 0)
        *pVlanId = tmpWord&VID_MASK;

    if ((GET_RESULT_FIELD(pResult).msbMask >= 4) && (valLen == 2))
        {
        // do nothing on pbit
        }
    else if ((GET_RESULT_FIELD(pResult).lsbMask <= 13) && 
        (valLen >= 2) && 
        (GET_RESULT_FIELD(pResult).msbMask <= 16))
        {//pbits included
        *pVlanPbit = ((U8)(tmpWord>>13))&PBIT_MASK;
        }
    
    if ((GET_RESULT_FIELD(pResult).msbMask == 0) && (valLen > 2))
        {//tpid included
        memcpy((U8*)&tmpTpid, (U8*)&tmpDword, sizeof(U16));
        *pVlanTpid = OAM_NTOHS(tmpTpid);
        }
    cmsLog_debug("VLAN info are tag %d, pbit %d, tpid 0x%x\n\r",*pVlanId, *pVlanPbit, *pVlanTpid);
    
    }

BOOL inline OamLlidIndexValid(U8 llidIndex)
    {
    if (llidIndex >= TkOnuMaxBiDirLlids)
        return FALSE;
    
    return TRUE;
    }

static BOOL OamGetDestPortAndQueue(RuleDesc* pRule, 
            U8 *destPort, U8 *queueIdx)
    {
    U8 i;
    OamRuleResultData *pResult = NULL;

    *destPort = DN_DIR_ALL;
    *queueIdx = DN_DIR_ALL;
    
    for (i = 0; i < pRule->resultCount; i ++)
        {
        pResult = &(pRule->resultData[i].result);
        if (pResult->result == OamResultQueue)
            {
            OamQueue *queueInfo;
            queueInfo = (OamQueue *)&(pResult->param.dest);
            if ((queueInfo->type == OamObjCtxtLink) || (queueInfo->type == OamObjCtxtUserPort))
                {
                *destPort = queueInfo->inst;
                *queueIdx = queueInfo->queue;
                //for downstream, the UNI port index should be defined as 1 ~ MAX
                if (queueInfo->type == OamObjCtxtUserPort)
                    *destPort = queueInfo->inst + 1;
                }
            break;
            }
        }

    return ((*destPort == DN_DIR_ALL) ? FALSE : TRUE);
    }

static BOOL isPortVirtualInterface(U8 port)
    {
#ifdef EPON_HGU
        return TRUE;
#endif
#ifdef EPON_SFU
    if(EponDevDpoePortType(port - 1) == DpoePortTypeMTA)
        return TRUE;
    return FALSE;
#endif    
    }

BOOL isVirtualLink(U8 linkIdx)
    {
    if (linkIdx < MAX_LINK_NUM)
        {
        return g_isVirtualLink[linkIdx];
        }
    else
        {
        cmsLog_error("linkIdx=%d invalid!\n", linkIdx);
        return FALSE;
        }  
    }

static OamError setVirtualLink(U8 linkIdx, BOOL isVirtual)
    {
    cmsLog_debug("linkIdx=%d, isVirtual=%d\n", linkIdx, isVirtual);
    if (linkIdx < MAX_LINK_NUM)
        {
        g_isVirtualLink[linkIdx] = isVirtual;
        }
    else
        {
        cmsLog_error("linkIdx=%d invalid!\n", linkIdx);
        return OamErrBadParams;
        }

    return OamErrNoError;      
    }
    
#ifdef OAM_VLAN_CTL
//database for rule configuration
U32 defaultPonFrwdRuleId[TkOnuNumTotalLlids][3];

char* OamVlanCtlGetTxDevName(U8 port)
    {
#if defined(EPON_SFU)
    return (port == 0) ? eponRealIfname : 
    (isPortVirtualInterface(port)?eponSfuVirtualIfname:uniRealIfname[port-1]);
#elif defined(EPON_HGU)
    return (port == 0) ? eponRealIfname : eponVeipIfname[port-1];
#else
#error
#endif
    }

char* OamVlanCtlGetRxDevName(U8 port)
    {
#if defined(EPON_SFU)
    return (port == 0) ? eponRealIfname : 
    (isPortVirtualInterface(port)?eponSfuVirtualIfname:uniRealIfname[port-1]);
#elif defined(EPON_HGU)
    return (port == 0) ? eponRealIfname : eponVeipIfname[port-1];
#else
#error
#endif
    }

typedef int (*parseFunc)(U8 port, void *arg1, void *arg2, void *arg3, void *arg4);

void VlanCtlRuleListParse(U8 port, 
        parseFunc func, void *arg1, void *arg2, void *result)
    {
    RuleIndexEntry *iEntry;
    RuleIndexEntry *tmpEntry;

    tmpEntry = priListHeader[port];
    do {
        if (tmpEntry == NULL)
            break;
        
        if (!(*func)(port, tmpEntry, arg1, arg2, result))
            break;
            
        iEntry = tmpEntry;
        if (iEntry->next == INVALID_PRI_IDX)
            break;

        tmpEntry = GlobalRuleListEntryByPortIndex(port, iEntry->next);
        }while(1);
    }

int VlanCtlDiscardRuleIdLookup(U8 port, 
        void *arg1, void *arg2, void *arg3, void *arg4)
    {
    ruleLocalPrtyInfo* infoPtr;
    RuleDesc *tmpRule;
    RuleIndexEntry *tmpEntry = (RuleIndexEntry *)arg1;
    RuleIndexEntry *pEntry = (RuleIndexEntry *)arg2;
    U8 nTags = *(U8*)arg3;
    U8 *resRuleId = (U8*)arg4;

    tmpRule = GlobalRuleTableByPortIndex(port, tmpEntry->curIdx);
    if (!tmpRule->isDiscard)//not discard that rx action, break
        return 1;
    
    if (pEntry->oamPriority > tmpEntry->oamPriority)//should find a lower/equal priority
        return 1;

    infoPtr = GetPlatformInfo(port, tmpEntry->curIdx);
    //rx just focus on the first rule
    if ((infoPtr->vlanCtlInfo[0][nTags].dir != VLANCTL_DIRECTION_RX) ||
            (!infoPtr->vlanCtlInfo[0][nTags].valid))
        return 1;

    *resRuleId = infoPtr->vlanCtlInfo[0][nTags].tagRuleId;
    return 0;
    }

static 
U8 VlanCtlRxInsertBeforeRuleId(U8 port, U8 index, U8 nTags)
    {
    U8 resRuleId = TABLE_IDX_INVALID;
    RuleIndexEntry *pEntry;
    U8 Tags = nTags;

    pEntry = GlobalRuleListEntryByPortIndex(port, index);
    VlanCtlRuleListParse(port, VlanCtlDiscardRuleIdLookup, 
                (void*)pEntry, (void*)(&Tags), (void*)(&resRuleId));

    return resRuleId;
    }


int VlanCtlTxRuleIdLookup(U8 port, 
        void *arg1, void *arg2, void *arg3, void *arg4)
    {
    ruleLocalPrtyInfo* infoPtr;
    RuleDesc *tmpRule;
    RuleIndexEntry *tmpEntry = (RuleIndexEntry *)arg1;
    RuleIndexEntry *pEntry = (RuleIndexEntry *)arg2;
    U8 nTags = (U8)(*(U16*)arg3&0x00FF);
    U8 destPort = (*(U16*)arg3&0xFF00) >> 8;
    U8 *resRuleId = (U8*)arg4;
    U8 rulePort = (port) ? 0 : (destPort-1); 

    tmpRule = GlobalRuleTableByPortIndex(port, tmpEntry->curIdx);
    if (tmpRule->isDiscard)//not discard that rx action, break
        return 1;
    
    if (pEntry->oamPriority > tmpEntry->oamPriority)//should find a lower/equal priority
        return 1;
    
    infoPtr = GetPlatformInfo(port, tmpEntry->curIdx);
    //rx just focus on the first rule
    //for upstream, focus on epon0 pon port, port = 0
    //for downstream, focus on destport uni port, ruleport = 1~4
    //find extend rule first
    //then normal rule
    if ((infoPtr->vlanCtlInfo[rulePort][nTags].dir == VLANCTL_DIRECTION_TX) &&
            (destPort == infoPtr->destPort) &&
            (infoPtr->vlanCtlInfo[rulePort][nTags].extend) &&
            (infoPtr->vlanCtlInfo[rulePort][nTags+3].valid))
            *resRuleId = infoPtr->vlanCtlInfo[rulePort][nTags+3].tagRuleId;
    else if ((infoPtr->vlanCtlInfo[rulePort][nTags].dir == VLANCTL_DIRECTION_TX) &&
            (destPort == infoPtr->destPort) &&        
            (infoPtr->vlanCtlInfo[rulePort][nTags].valid))
            *resRuleId = infoPtr->vlanCtlInfo[rulePort][nTags].tagRuleId;
    else
        return 1;

    return 0;
    }


static 
U8 VlanCtlTxInsertBeforeRuleId(U8 port, U8 destPort, U8 index, U8 nTags)
    {
    U8 resRuleId = TABLE_IDX_INVALID;
    RuleIndexEntry *pEntry;
    U16 portTags = (destPort << 8) | nTags;

    pEntry = GlobalRuleListEntryByPortIndex(port, index);
    VlanCtlRuleListParse(port, VlanCtlTxRuleIdLookup, 
            (void*)pEntry, (void*)(&portTags), (void*)(&resRuleId));

    return resRuleId;
    }
    
int VlanCtlRuleIdLookup(U8 port, 
        void *arg1, void *arg2, void *arg3, void *arg4)
    {
    ruleLocalPrtyInfo* infoPtr;
    RuleIndexEntry *tmpEntry = (RuleIndexEntry *)arg1;
    RuleIndexEntry *pEntry = (RuleIndexEntry *)arg2;
    VlanctlRuleLookUpPara *ruleLookUpPara = (VlanctlRuleLookUpPara*)arg3;
    U8 nTags = ruleLookUpPara->nTags;
    vlanCtl_direction_t dir = ruleLookUpPara->dir;
    U8 ruleIdx = 0;
    U8 *resRuleId = (U8*)arg4;

    cmsLog_debug("pEntry: curIdx %d, oamPriority %d, next %d\n",pEntry->curIdx, pEntry->oamPriority, pEntry->next);
    cmsLog_debug("tmpEntry: curIdx %d, oamPriority %d, next %d\n",tmpEntry->curIdx, tmpEntry->oamPriority, tmpEntry->next);
    
    if (pEntry->oamPriority >= tmpEntry->oamPriority)//should find a lower priority
        return 1;
    
    infoPtr = GetPlatformInfo(port, tmpEntry->curIdx);
        
    //upstream, ruleIdx is 0, downstream, ruleIdx is destport(1...)-1
    ruleIdx = port ? 0 : (infoPtr->destPort - 1);   
    
    //for upstream, rules stored with idx dest port pon(port 0)
    //for downstream, rules stored with idx dest port uni port(1 ~ MAX_NUM_UNI_PORTS)
    //find extend rule first
    //then normal rule
    if ((infoPtr->vlanCtlInfo[ruleIdx][nTags].dir == dir) &&
        (infoPtr->vlanCtlInfo[ruleIdx][nTags].extend) &&
        (infoPtr->vlanCtlInfo[ruleIdx][nTags+3].valid))
            *resRuleId = infoPtr->vlanCtlInfo[ruleIdx][nTags+3].tagRuleId;
    else if ((infoPtr->vlanCtlInfo[ruleIdx][nTags].dir == dir) &&
        (infoPtr->vlanCtlInfo[ruleIdx][nTags].valid))
            *resRuleId = infoPtr->vlanCtlInfo[ruleIdx][nTags].tagRuleId;
    else
        return 1;
    
    cmsLog_debug("resRuleId %d,\n", *resRuleId);   
    
    return 0;
    }

static 
U8 VlanCtlInsertBeforeRuleId(U8 port, U8 destPort, U8 index, U8 nTags, vlanCtl_direction_t dir)
    {
    U8 resRuleId = TABLE_IDX_INVALID;
    RuleIndexEntry *pEntry;
    VlanctlRuleLookUpPara ruleLookUpPara;
    
    ruleLookUpPara.nTags = nTags;
    ruleLookUpPara.destPort = destPort;
    ruleLookUpPara.dir = dir;
    
    pEntry = GlobalRuleListEntryByPortIndex(port, index);
    VlanCtlRuleListParse(port, VlanCtlRuleIdLookup, 
            (void*)pEntry, (void*)(&ruleLookUpPara), (void*)(&resRuleId));
    
    return resRuleId;
    }

static inline U8 VlanCtlGetDestPort(OamQueue *pQueue)
    {
    return (pQueue->type == OamObjCtxtLink) ? 0 : (pQueue->inst + 1);
    }

#define VLANCTL_SVLAN_INDEX        0
#define VLANCTL_CVLAN_INDEX        1

#define VLANCTL_EXTEND_INIT 0
#define VLANCTL_EXTEND_TODO 1
#define VLANCTL_EXTEND_NOW 2
#define VLANCTL_EXTEND_NONE 3

static void VlanCtlClauseParseFilterSourcePort(RuleDesc* pRule)
    {

    /*Upstream source port filter implementation is different on HGU or SFU
      HGU: filter tx interface
      SFU: filter skbMarkFlowId for physical interafce
           filter tx interface for virtual interface*/ 
#if defined(EPON_HGU)
    if (pRule->port)
        vlanCtl_filterOnTxVlanDevice(OamVlanCtlGetTxDevName(pRule->port));
#elif defined(EPON_SFU)
    if (pRule->port && !pRule->isDiscard)
        {
        if (isPortVirtualInterface(pRule->port))
            vlanCtl_filterOnTxVlanDevice(OamVlanCtlGetTxDevName(pRule->port));
        else
            vlanCtl_filterOnSkbMarkFlowId(pRule->port - 1);
        }
#else
#error
#endif
    }

static void VlanCtlFillVlanInfor(U16 vlanTpid, U16 vlanVid, U8 vlanPbit,
                                 BOOL isSvlan, U8 tags)
    {
    U16 vlanTpidFilter = isSvlan?OamAltSVlanTpidGet():OamAltCVlanTpidGet();

    if (vlanTpid != INVALID_WORD)
        vlanTpidFilter = vlanTpid;

    if (tags == 0)
        vlanCtl_filterOnEthertype((U32)vlanTpidFilter);
    else
        vlanCtl_filterOnTagEtherType((U32)vlanTpidFilter, tags-1);
    
    if (vlanPbit != INVALID_BYTE)
         vlanCtl_filterOnTagPbits((U32)vlanPbit, tags);
    
    if (vlanVid != INVALID_WORD)
         vlanCtl_filterOnTagVid((U32)vlanVid, tags);

    }

static U8 VlanCtlClauseParse(U8 *num_tags, 
                             RuleDesc* pRule,
                             U8 *flowid, 
                             U8 *extend,
                             BOOL isSFU)
    {
    U8 i;
    U8 valLen;
    U8 clauseValid = 1;
    U8 vlanInst = 0;
    U32 vlanFieldMark= 0;
    U8 vlanPbit[MAX_VLAN_INSTANCES] = {INVALID_BYTE, INVALID_BYTE, INVALID_BYTE, INVALID_BYTE};
    U16 vlanVid[MAX_VLAN_INSTANCES] = {INVALID_WORD, INVALID_WORD, INVALID_WORD, INVALID_WORD};
    U16 vlanTpid[MAX_VLAN_INSTANCES] = {INVALID_WORD, INVALID_WORD, INVALID_WORD, INVALID_WORD};    
    U16 etherTypeFilter = INVALID_WORD;
    OamRuleClauseData *pClause = NULL;
    U16 llidValue = INVALID_WORD;
    U8 linkIndex = INVALID_BYTE;
     
    *flowid = 0xFF;

    VlanCtlClauseParseFilterSourcePort(pRule);

    if (pRule->isAlways)
        {
        goto extend_clause;
        }

    //start parsing
    //clause filter
    for (i = 0; i < pRule->clauseCount; i ++)
        {
        if (clauseValid == 0)
            {//parameter error happened, just break
            cmsLog_debug("clause parse error return\n");
            break;
            }
                
        pClause = &(pRule->clauseData[i].clause);
        valLen = GET_GENERAL_CLAUSE_VALUE_LENGTH(pClause);
        cmsLog_debug("clause fieldcode (%x), length (%d)\n", pClause->fieldCode, valLen);
        switch (pClause->fieldCode)
            {
            case OamFieldLinkIndex:
                *flowid = GET_GENERAL_CLAUSE_VALUE_BYTE(pClause);
                //the enet driver already set skb->mark to record downstream flow id information
                //here we can classify link index by the skb->mark
                if(OamLlidIndexValid(*flowid))
                    vlanCtl_filterOnSkbMarkPort(*flowid);
                else
                    clauseValid = 0;

                break;
               
             case OamFieldLlidValue:
                COPY_GENERAL_CLAUSE_VALUE_RIGHT_WORD(pClause, &llidValue);
                if((!eponStack_CtlGetLinkIndex(OAM_NTOHS(llidValue), &linkIndex)) &&
                    (OamLlidIndexValid(linkIndex)))
					{
					*flowid = linkIndex;
					vlanCtl_filterOnSkbMarkPort(*flowid);
					}
                else
                    clauseValid = 0;
                break;
                
            case OamFieldL2Type:
                OamGetEthertypeFromClause(pClause,valLen,&etherTypeFilter);
                break;

            case OamFieldSVlan:
                {
                vlanInst = pClause->clauseContent.generalClause.instance?SVLAN_1_INST_ID:SVLAN_0_INST_ID;
                OamGetVlanFromClause(pClause,valLen,&vlanVid[vlanInst], &vlanPbit[vlanInst], &vlanTpid[vlanInst]);
                vlanFieldMark |= ((vlanInst==SVLAN_1_INST_ID)?SVLAN_1_BIT:SVLAN_0_BIT);                
                }
                break;
            
            case OamFieldCVlan:
                {
                vlanInst = pClause->clauseContent.generalClause.instance?CVLAN_1_INST_ID:CVLAN_0_INST_ID;
                OamGetVlanFromClause(pClause,valLen,&vlanVid[vlanInst], &vlanPbit[vlanInst], &vlanTpid[vlanInst]);
                vlanFieldMark |= ((vlanInst==CVLAN_1_INST_ID)?CVLAN_1_BIT:CVLAN_0_BIT);                
                }
                break;
        
            case OamFieldIpv4Tos:
                vlanCtl_filterOnDscp(GET_GENERAL_CLAUSE_VALUE_RIGHT_BYTE(pClause));
                break;
        
            case OamFieldIpProtocolType:
                vlanCtl_filterOnIpProto(GET_GENERAL_CLAUSE_VALUE_RIGHT_BYTE(pClause));
                break;

            case OamFieldL2Sa:
            case OamFieldL2Da:
            case OamFieldIpv4Sa:
            case OamFieldIpv6Sa:
            case OamFieldIpv4Da:
            case OamFieldIpv6Da:
            case OamFieldIpv6FlowLabel:
            case OamFieldTcpUdpSrcPort:
            case OamFieldTcpUdpDestPort:
                //vlanctl doesn't support these classification, just pass them
            default:
                clauseValid = 0;
                break;
            }
        }

        /* Check if vlan case is supported */
        for (i = 0; i < VLAN_CLAUSE_SUPPORTED_CASES && vlanFieldMark; i++)
            {
            if (globalVlanFields[i].vlanStackInfo == vlanFieldMark)
                break;
            }
        if (i >= VLAN_CLAUSE_SUPPORTED_CASES)            
            {
            clauseValid = FALSE;
            }


        if (clauseValid)
            {
            if(vlanFieldMark && (*num_tags == 0))
                *num_tags = globalVlanFields[i].tagsNum;

            // for rule with tags >= 1, we should set tagethertype but not ethertype.                    
            if (etherTypeFilter != INVALID_WORD)                       
                {
                if (*num_tags == 0)
                    vlanCtl_filterOnEthertype((U32)etherTypeFilter);
                else
                    vlanCtl_filterOnTagEtherType((U32)etherTypeFilter, *num_tags-1);
                }
            
            switch (*num_tags)
                {
                cmsLog_debug("Vlan Selection 0x08X\n",vlanFieldMark);
                case 2:
                    if (SVLAN_0_PRESENT(vlanFieldMark))
                        {

                        VlanCtlFillVlanInfor(vlanTpid[SVLAN_0_INST_ID], vlanVid[SVLAN_0_INST_ID],
                                             vlanPbit[SVLAN_0_INST_ID], TRUE, 0);                        
                        }

                    if (SVLAN_1_PRESENT(vlanFieldMark))
                        {
                        VlanCtlFillVlanInfor(vlanTpid[SVLAN_1_INST_ID], vlanVid[SVLAN_1_INST_ID],
                                             vlanPbit[SVLAN_1_INST_ID], TRUE, 1);                        
                        }

                    if (CVLAN_0_PRESENT(vlanFieldMark))
                        {
                        U8 tags = SVLAN_PRESENT(vlanFieldMark)?1:0;
                        
                        VlanCtlFillVlanInfor(vlanTpid[CVLAN_0_INST_ID], vlanVid[CVLAN_0_INST_ID],
                                             vlanPbit[CVLAN_0_INST_ID], FALSE, tags);
                        }
                    
                    if (CVLAN_1_PRESENT(vlanFieldMark))
                        {
                        VlanCtlFillVlanInfor(vlanTpid[CVLAN_1_INST_ID], vlanVid[CVLAN_1_INST_ID],
                                             vlanPbit[CVLAN_1_INST_ID], FALSE, 1);                        
                        }
                case 1:
                    if (SVLAN_0_PRESENT(vlanFieldMark))
                        {
                        VlanCtlFillVlanInfor(vlanTpid[SVLAN_0_INST_ID], vlanVid[SVLAN_0_INST_ID],
                                             vlanPbit[SVLAN_0_INST_ID], TRUE, 0);                        
                        }
                    else if(CVLAN_0_PRESENT(vlanFieldMark))
                        {
                        VlanCtlFillVlanInfor(vlanTpid[CVLAN_0_INST_ID], vlanVid[CVLAN_0_INST_ID],
                                             vlanPbit[CVLAN_0_INST_ID], FALSE, 0);                        
                        }
                    else
                        {}  /* Should not here */
                    break;
                default:
                    //here no need check vlan tag
                    break;
                }
            }

extend_clause:
        if (*extend==VLANCTL_EXTEND_TODO)
            {
#ifdef OAM_VLAN_CTL_EXTEND
            if (*num_tags)
                {//add priority-tagged filter
                vlanCtl_filterOnTagVid(0, VLANCTL_SVLAN_INDEX);
                }
            else
                {
                clauseValid = 0;
                //this extend should be deperated
                }
#else
            clauseValid = 0;
#endif           
            *extend = VLANCTL_EXTEND_NOW;
            return clauseValid;
            }
        
        if (*extend == VLANCTL_EXTEND_INIT)
            *extend = VLANCTL_EXTEND_TODO;
        
    return clauseValid;
    }


static U8 VlanCtlResultDeleteVlanProcess(U8 num_tags, 
                             BOOL deleteSvlanTag,
                             BOOL deleteCvlanTag)
    {
    U8 resultValid = 1;
    BOOL isPop2 = ((deleteSvlanTag == TRUE) && (deleteCvlanTag == TRUE));

    if ((isPop2)&&(num_tags >= 2))
        {
        vlanCtl_cmdPopVlanTag();
        vlanCtl_cmdPopVlanTag();
        }
    else if ((!isPop2)&&(num_tags >= 1))
        vlanCtl_cmdPopVlanTag();
    else
        resultValid = 0;
    
    return resultValid;
    }

static U8 VlanCtlResultReplaceVlanProcess(U8 num_tags, 
                             BOOL replaceSvlanTag,
                             BOOL replaceCvlanTag,
                             VlanctlVlanActionPara* pVlanActionValue)
    {
    U8 resultValid = 0;
    U8 cVlanIndex = (num_tags == 1)?VLANCTL_SVLAN_INDEX:VLANCTL_CVLAN_INDEX;

    if(!num_tags)
        return resultValid;
    
    resultValid = 1;        
    //replace cvlan action
    if (replaceCvlanTag == TRUE)
        {
        if (pVlanActionValue->cvlanPbit != INVALID_BYTE)
            vlanCtl_cmdSetTagPbits((U32)(pVlanActionValue->cvlanPbit), cVlanIndex);
        if (pVlanActionValue->cvlanVid != INVALID_WORD)
            vlanCtl_cmdSetTagVid((U32)(pVlanActionValue->cvlanVid), cVlanIndex);
        }
        
    //replace svlan action
    if (replaceSvlanTag == TRUE)
        {
        if (pVlanActionValue->svlanPbit != INVALID_BYTE)
            vlanCtl_cmdSetTagPbits((U32)(pVlanActionValue->svlanPbit), VLANCTL_SVLAN_INDEX);
        if (pVlanActionValue->svlanVid != INVALID_WORD)
            vlanCtl_cmdSetTagVid((U32)(pVlanActionValue->svlanVid), VLANCTL_SVLAN_INDEX);
        }
    
    return resultValid;
    }

static U8 VlanCtlResultPushVlanProcess(U8 num_tags, 
                             BOOL pushSvlanTag,
                             BOOL pushCvlanTag,
                             VlanctlVlanActionPara* pVlanActionValue)
    {
    U8 resultValid = 0;

    cmsLog_debug("pushSvlanTag %d, pushCvlanTag %d\n", pushSvlanTag, pushCvlanTag);
    cmsLog_debug("svlanVid %d, cvlanVid %d\n", pVlanActionValue->svlanVid, pVlanActionValue->cvlanVid);
    
    //insert both svlan and cvlan is not supported
    if (pushSvlanTag && pushCvlanTag)
        {
        return resultValid;
        }

    if ((pushSvlanTag && (pVlanActionValue->svlanVid == INVALID_WORD))||
        (pushCvlanTag && (pVlanActionValue->cvlanVid == INVALID_WORD)))
        {
        return resultValid;
        }

    resultValid = 1;        
    if (pushSvlanTag)
        {
        vlanCtl_cmdPushVlanTag();
        vlanCtl_cmdSetTagVid((U32)(pVlanActionValue->svlanVid), VLANCTL_SVLAN_INDEX);
        if (pVlanActionValue->svlanPbit != INVALID_BYTE)
            vlanCtl_cmdSetTagPbits((U32)pVlanActionValue->svlanPbit, VLANCTL_SVLAN_INDEX);
        vlanCtl_cmdSetEtherType((pVlanActionValue->svlanTpid!=INVALID_WORD) ? pVlanActionValue->svlanTpid : OamAltSVlanTpidGet());
        }
    
    if (pushCvlanTag)
        {        
        vlanCtl_cmdPushVlanTag();
        vlanCtl_cmdSetTagVid((U32)pVlanActionValue->cvlanVid, VLANCTL_SVLAN_INDEX);
        if (pVlanActionValue->cvlanPbit != INVALID_BYTE)
            vlanCtl_cmdSetTagPbits((U32)pVlanActionValue->cvlanPbit, VLANCTL_SVLAN_INDEX);
        vlanCtl_cmdSetEtherType((pVlanActionValue->cvlanTpid!=INVALID_WORD) ? pVlanActionValue->cvlanTpid : OamAltCVlanTpidGet());
        }
    cmsLog_debug("result parse result %d \n", resultValid);
    return resultValid;
    }

static U8 VlanCtlResultParse(U8 num_tags, 
                             ruleLocalPrtyInfo* infoPtr,
                             RuleDesc* pRule,
                             U8 extend)
    {
    U8 i;
    U8 valLen;
    U8 resultValid = 1;
    U8 svlanPbit = INVALID_BYTE;
    U16 svlanVid = INVALID_WORD;
    U16 svlanTpid = INVALID_WORD;
    U8 cvlanPbit = INVALID_BYTE;
    U16 cvlanVid = INVALID_WORD;
    U16 cvlanTpid = INVALID_WORD;
    BOOL pushSvlanTag = FALSE;
    BOOL pushCvlanTag = FALSE;
    BOOL replaceSvlanTag = FALSE;
    BOOL replaceCvlanTag = FALSE;
    BOOL deleteSvlanTag = FALSE;
    BOOL deleteCvlanTag = FALSE;
    OamRuleResultData *pResult= NULL;
    U16 etherTypeResult;
    VlanctlVlanActionPara vlanActionValue;

    //start parsing
    //result
    for (i = 0; i < pRule->resultCount; i ++)
        {
        if (resultValid == 0)
            {//parameter error happened, just break now
            cmsLog_debug("result parse error return\n");
            break;
            }
        
        pResult = &(pRule->resultData[i].result);
        valLen = pRule->resultData[i].length - 3;
        cmsLog_debug("result type (%d), length (%d)\n", pResult->result, valLen);
        switch (pResult->result)
            {
            case OamResultDiscard:
                vlanCtl_cmdDropFrame();
                break;
            
            case OamResultForward:
            //Queue configuration already parsed
            case OamResultQueue:
                break;
            
            case OamResultSet:
                {
                switch(pResult->param.field.fldCode)
                    {
                    case OamFieldL2Type:
                        //currently now support
                        OamGetEthertypeFromResult(pResult,valLen,&etherTypeResult);
                        
                        //set ethertype
                        if(!num_tags)
                            vlanCtl_cmdSetEtherType((U32)etherTypeResult);
                        else
                            vlanCtl_cmdSetTagEtherType((U32)etherTypeResult, num_tags-1);          
                        break;
                    
                    case OamFieldSVlan:
                        OamGetVlanFromResult(pResult,valLen,&svlanVid,&svlanPbit,&svlanTpid);                    
                        break;
                    
                    case OamFieldCVlan:
                        OamGetVlanFromResult(pResult,valLen,&cvlanVid,&cvlanPbit,&cvlanTpid);
                        break;
                    
                    case OamFieldIpv4Tos:
                        //currently now support
                        vlanCtl_cmdSetDscp((U32)GET_RESULT_FIELD_VALUE_RIGHT_BYTE(pResult, valLen));
                        break;
                    
                    default:
                        cmsLog_debug("The given result field (%d) is not supported by vlanctl for Set operation\n", 
                            pResult->param.field.fldCode);
                        resultValid = 0;
                        break;
                    }
                }
                break;
            
            case OamResultCopy:
                {
                //per DPoE specification
                //the copy result copies the value of some field into the specified output field. 
                //the source field is the field used in the last clause of the rule condition
                //typically this result is used to copy priority fields.
                //TBD
                OamRuleClauseData *pClause = &(pRule->clauseData[pRule->clauseCount].clause);
                switch(pResult->param.field.fldCode)
                    {
                    case OamFieldSVlan:
                        if (pClause->fieldCode == OamFieldIpv4Tos)
                            vlanCtl_cmdDscpToPbits(VLANCTL_SVLAN_INDEX);
                        else if(pClause->fieldCode == OamFieldCVlan)
                            {
                            vlanCtl_cmdCopyTagCfi(VLANCTL_CVLAN_INDEX, VLANCTL_SVLAN_INDEX);
                            vlanCtl_cmdCopyTagPbits(VLANCTL_CVLAN_INDEX, VLANCTL_SVLAN_INDEX);
                            vlanCtl_cmdCopyTagVid(VLANCTL_CVLAN_INDEX, VLANCTL_SVLAN_INDEX);
                            }
                        else
                            {
                            cmsLog_debug("The given clause field (%d) is not supported by vlanctl for Svlan Copy operation\n", 
                                pClause->fieldCode);
                            resultValid = 0;
                            }
                        break;
                        
                    case OamFieldCVlan:
                        if (pClause->fieldCode == OamFieldIpv4Tos)
                            vlanCtl_cmdDscpToPbits(VLANCTL_CVLAN_INDEX);
                        else if(pClause->fieldCode == OamFieldSVlan)
                            {
                            vlanCtl_cmdCopyTagCfi(VLANCTL_SVLAN_INDEX, VLANCTL_CVLAN_INDEX);
                            vlanCtl_cmdCopyTagPbits(VLANCTL_SVLAN_INDEX, VLANCTL_CVLAN_INDEX);
                            vlanCtl_cmdCopyTagVid(VLANCTL_SVLAN_INDEX, VLANCTL_CVLAN_INDEX);
                            }
                        else
                            {
                            cmsLog_debug("The given clause field (%d) is not supported by vlanctl for Cvlan Copy operation\n", 
                                pClause->fieldCode);
                            resultValid = 0;
                            }
                        break;
                    default:
                        cmsLog_debug("The given result field (%d) is not supported by vlanctl for Copy operation\n", 
                            pResult->param.field.fldCode);
                        resultValid = 0;
                        break;
                    }
                }
                break;
            
            case OamResultDelete:
                {
                cmsLog_debug("result field (%d) for Delete\n", pResult->param.field.fldCode);
                switch(pResult->param.field.fldCode)
                    {
                    case OamFieldSVlan:
                        deleteSvlanTag = TRUE;
                        break;
                        
                    case OamFieldCVlan:
                        deleteCvlanTag = TRUE;
                        break;
                    
                    default:
                        cmsLog_debug("The given result field (%d) is not supported by vlanctl for Delete operation\n", 
                            pResult->param.field.fldCode);
                        resultValid = 0;
                        break;
                    }
                }
                break;
            
            case OamResultInsert:
                {
                cmsLog_debug("result field (%d) for Insert\n", pResult->param.field.fldCode);
                switch(pResult->param.field.fldCode)
                    {
                    case OamFieldSVlan:
                        pushSvlanTag = TRUE;
                        break;
                        
                    case OamFieldCVlan:
                        pushCvlanTag = TRUE;
                        break;
                    
                    default:
                        cmsLog_debug("The given result field (%d) is not supported by vlanctl for Insert operation\n", 
                            pResult->param.field.fldCode);
                        resultValid = 0;
                        break;
                    }
                }
                break;
            
            case OamResultReplace:
                {
                cmsLog_debug("result field (%d) for Insert\n", pResult->param.field.fldCode);
                switch(pResult->param.field.fldCode)
                    {
                    case OamFieldSVlan:
                        replaceSvlanTag = TRUE;
                        break;
                        
                    case OamFieldCVlan:
                        replaceCvlanTag = TRUE;
                        break;
                    
                    default:
                        cmsLog_debug("The given result field (%d) is not supported by vlanctl for Replace operation\n", 
                            pResult->param.field.fldCode);
                        resultValid = 0;
                        break;
                    }
                }
                break;
            
            case OamResultClearDel:
                //TBD
            
            case OamResultClearIns:
                //TBD
            
            default:
                cmsLog_debug("The given result type (%d) is not supported by vlanctl\n", pResult->result);
                resultValid = 0;
                break;
            }
        }

        cmsLog_debug("By now resultValid is %d, replaceSvlanTag %d, replaceCvlanTag %d, pushSvlanTag %d, pushCvlanTag %d\n", 
            resultValid, replaceSvlanTag, replaceCvlanTag, pushSvlanTag, pushCvlanTag);
        cmsLog_debug("svlanVid %d, svlanPbit %d, svlanTpid %x, cvlanVid %d, cvlanPbit %d, cvlanTpid %dx \n", 
            svlanVid, svlanPbit, svlanTpid, cvlanVid, cvlanPbit, cvlanTpid);

        if (resultValid)
            {
            vlanActionValue.svlanVid  = svlanVid;
            vlanActionValue.svlanPbit = svlanPbit;
            vlanActionValue.svlanTpid = svlanTpid;
            vlanActionValue.cvlanVid  = cvlanVid;
            vlanActionValue.cvlanPbit = cvlanPbit;
            vlanActionValue.cvlanTpid = cvlanTpid;

            if (((num_tags == 2) && (extend == VLANCTL_EXTEND_NOW))||
                ((num_tags == 1) && (extend == VLANCTL_EXTEND_NOW) && (replaceSvlanTag || replaceCvlanTag )))
                {//ignore this rule
                resultValid = 0;
                return resultValid;
                }

            if (deleteSvlanTag || deleteCvlanTag)
                resultValid = VlanCtlResultDeleteVlanProcess(num_tags, deleteSvlanTag, deleteCvlanTag);                            
            if (replaceSvlanTag || replaceCvlanTag)
                resultValid = VlanCtlResultReplaceVlanProcess(num_tags, replaceSvlanTag, replaceCvlanTag, &vlanActionValue);            
            if (pushSvlanTag || (pushCvlanTag))
                resultValid |= VlanCtlResultPushVlanProcess(num_tags, pushSvlanTag, pushCvlanTag, &vlanActionValue);            
            }
        
    return resultValid;
    }

static void vlanctlInsertRulePosition(U8 port, U8 tags, U32 *position, U32 *posRuleId)
    {
    if (*posRuleId != TABLE_IDX_INVALID)
        {
        *position = VLANCTL_POSITION_BEFORE;
        return;
        }
/* For SFU, downstream ingress rule configured on ethx.
            There is a default rule for lo interface with 0 tag on each eth interface.   
   For HGU, no such loop interface rule*/
#if defined(EPON_SFU)
    if (!port && !tags)
        {
        *position = VLANCTL_POSITION_BEFORE;
        *posRuleId = 0 ;
        }
    else
#endif
        {
        *position = VLANCTL_POSITION_APPEND;
        }
    }

static OamError OamAddSlowPathForVirtualInterface(RuleDesc* pRule, ruleLocalPrtyInfo *infoPtr)
    {
    U8 num_tags;
    U32 ruleId = 0, posRuleId = VLANCTL_DONT_CARE, position = VLANCTL_POSITION_BEFORE;
    U8 clauseValid;
    U8 resultValid;
    char ruleIfName[IFNAMESIZ];
    U8 flowid;
    U8 extend;
    vlanCtl_direction_t vlanCtlRuleDir = VLANCTL_DIRECTION_MAX;
    BOOL isUpstream;
    U8 linkIdx = INVALID_BYTE;
    U8 queueIdx = INVALID_BYTE;
    U8 destPort = 0;
    U8 ruleIdx = 0;
    U8 ruleTagNum = 0;

    cmsLog_debug("OamAddVlanctl for rule of port: %d, priority: %d\n", pRule->port, pRule->priority);

    num_tags = 0;
    isUpstream = (pRule->port != 0);
    vlanCtlRuleDir = (isUpstream) ? VLANCTL_DIRECTION_TX : VLANCTL_DIRECTION_RX;
    snprintf(ruleIfName, IFNAMESIZ, eponRealIfname);
       
    /* note HGU do not support DN_DIR_ALL
       upstream  : link and queue must be set when not discard
       downstream: egress_port must be set when not discard */
    if ((!pRule->isDiscard) && 
        (!OamGetDestPortAndQueue(pRule, &(infoPtr->destPort), &queueIdx)))
        {
        cmsLog_error("HGU: dest port is not set, return here");
        return OamErrBadParams;
        }
        
    if (!isUpstream && (infoPtr->destPort > UniCfgDbGetOamManagedPortCount()))
        {
        cmsLog_error("HGU: downstream dest port(%d) is out of range", infoPtr->destPort);
        return OamErrBadParams;
        }

    /* for upstream, store link index in linkIdx
       restore upstream destination port to PON */
    if ((!pRule->isDiscard) && (isUpstream))
        {
        linkIdx = infoPtr->destPort;
        infoPtr->destPort = 0;
        }
        
    while (num_tags < MAX_VLANCTL_TAGS)
        {
        extend = VLANCTL_EXTEND_INIT;
extend_action:
        clauseValid = resultValid = 1;
        
        vlanCtl_initTagRule();
        //clause filter
        clauseValid = VlanCtlClauseParse(&num_tags, pRule, &flowid, &extend, FALSE);
        cmsLog_debug("after clause check, clauseValid: %d\n", clauseValid);
        if (clauseValid) 
            {
            vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_IS_UNICAST|
                                  BCM_VLAN_FILTER_FLAGS_IS_MULTICAST|
                                  BCM_VLAN_FILTER_FLAGS_IS_BROADCAST);
            
            resultValid = VlanCtlResultParse(num_tags, infoPtr, pRule, extend);
            cmsLog_debug("after result check, dstport: %x, resultValid: %d\n", infoPtr->destPort, resultValid);
            if (resultValid)
                {
                vlanCtl_setVlanRuleTableType(BCM_VLAN_RULE_TYPE_FLOW);
                                
                //this section needs extend rule for priority-tagged frame
                //normal rule when extend==VLANCTL_EXTEND_TODO.
                //extend rule when extend==VLANCTL_EXTEND_NOW.
                ruleTagNum = (extend == VLANCTL_EXTEND_TODO) ? num_tags : num_tags+3;
                
                //upstream should specify mark and queue
                if ((isUpstream) && (linkIdx != INVALID_BYTE) && (queueIdx != INVALID_BYTE))
                    {
                    vlanCtl_cmdSetSkbMarkPort(linkIdx);
                    if (qosManagedByOAM)
                        vlanCtl_cmdSetSkbMarkQueue(queueIdx);

                    if (0 == num_tags) /* just do one time*/
                        {
                        setVirtualLink(linkIdx, TRUE);
                        }
                    }
                                                   
                //for downsteam, configure rule on port 1~UniCfgDbGetOamManagedPortCount uni port.
                //for upstream, configure rule on port 0 pon port.                
                destPort = infoPtr->destPort;  
                    
                if (vlanCtlRuleDir == VLANCTL_DIRECTION_RX)
                    {
                    if(!pRule->isDiscard)
                        {
                        vlanCtl_setReceiveVlanDevice(OamVlanCtlGetRxDevName(destPort));
#if defined(EPON_SFU)                         
                        vlanCtl_filterOnVlanDeviceMacAddr(1);
#endif
                        }
                    else
                        //receive interface is required for RX rule, for discard rule, not used really
                        vlanCtl_setReceiveVlanDevice(OamVlanCtlGetRxDevName(destPort));
                    }

                //upstream, ruleIdx is 0, downstream, ruleIdx is destport(1...)-1
                ruleIdx = isUpstream ? destPort : (destPort - 1);   
                //lookup the position of this rule by priority
                if (extend == VLANCTL_EXTEND_TODO)
                    {
                    posRuleId = VlanCtlInsertBeforeRuleId(pRule->port, destPort, infoPtr->index, num_tags, vlanCtlRuleDir);
                    position = (posRuleId == TABLE_IDX_INVALID)?VLANCTL_POSITION_APPEND:VLANCTL_POSITION_BEFORE;
#if defined(EPON_SFU)  
                    //upstream SFU virtual interface, the position should be highest priority
                    //FIXME when there are more than 1 virtual interface on SFU
                    if (!isUpstream)
                        {
                        position = VLANCTL_POSITION_BEFORE;
                        posRuleId = VLANCTL_DONT_CARE;
                        }
#endif                    
                    }
                else
                    {
                    position = VLANCTL_POSITION_BEFORE;
                    posRuleId = infoPtr->vlanCtlInfo[ruleIdx][num_tags].tagRuleId;
                    }
                
                vlanCtl_insertTagRule(ruleIfName, 
                        vlanCtlRuleDir, num_tags, position, posRuleId, &ruleId);
                cmsLog_debug("ruleIfname %s, dir %d tags %d position %d, posRuleId %d\n",ruleIfName,
                        vlanCtlRuleDir, num_tags, position, posRuleId);               
                infoPtr->vlanCtlInfo[ruleIdx][ruleTagNum].valid = 1;
                if (extend == VLANCTL_EXTEND_NOW)
                    infoPtr->vlanCtlInfo[ruleIdx][num_tags].extend = 1;
                infoPtr->vlanCtlInfo[ruleIdx][ruleTagNum].nbrOfTags = num_tags;
                infoPtr->vlanCtlInfo[ruleIdx][ruleTagNum].dir = vlanCtlRuleDir;
                infoPtr->vlanCtlInfo[ruleIdx][ruleTagNum].tagRuleId = ruleId;
                
                if (extend == VLANCTL_EXTEND_TODO)
                    goto extend_action;
                }//result valid
            } //clasue valid
            num_tags ++;
        }
        PrintLocalPrtyInfoByIdxPortList(pRule->port, infoPtr->index);  
        return OamErrNoError;
    }

static void OamDelSlowPathForVirtualInterface(RuleDesc* pRule, ruleLocalPrtyInfo *infoPtr)
    {
    U8 i, j;
    char ruleIfName[IFNAMESIZ];
    U8 queueIdx = INVALID_BYTE;
    
    cmsLog_debug("OamDelSlowPath for rule with source port: %d destport: %d, isDiscard %d\n", 
        pRule->port, infoPtr->destPort, pRule->isDiscard); 
    
    snprintf(ruleIfName, IFNAMESIZ, eponRealIfname);
    for (i =0; i < UniCfgDbGetOamManagedPortCount(); i ++)
        {
        
        for (j = 0; j < MAX_VLANCTL_TAGS; j++)
            {
            
            if (infoPtr->vlanCtlInfo[i][j].valid)
                {
                vlanCtl_removeTagRule(ruleIfName,
                                infoPtr->vlanCtlInfo[i][j].dir,
                                infoPtr->vlanCtlInfo[i][j].nbrOfTags,
                                infoPtr->vlanCtlInfo[i][j].tagRuleId);
                if (1 == infoPtr->vlanCtlInfo[i][j].dir && 0 == j) /* just do one time for upstream*/
                    {
                        if ((!pRule->isDiscard) && 
                        (!OamGetDestPortAndQueue(pRule, &(infoPtr->destPort), &queueIdx)))
                            {
                            cmsLog_error("HGU: dest port is not set!");
                            }
                        else
                            {
                            setVirtualLink(infoPtr->destPort, FALSE);
                            }
                    }
                if (infoPtr->vlanCtlInfo[j][i].extend && (infoPtr->vlanCtlInfo[i][j+3].valid))
                    vlanCtl_removeTagRule(ruleIfName,
                                infoPtr->vlanCtlInfo[i][j+3].dir,
                                infoPtr->vlanCtlInfo[i][j+3].nbrOfTags,
                                infoPtr->vlanCtlInfo[i][j+3].tagRuleId);
                }
            }
        }
    }

static OamError OamAddSlowPathForPhysicalInterface(RuleDesc* pRule, ruleLocalPrtyInfo *infoPtr)
    {
    U8 num_tags;
    U32 ruleId = 0, posRuleId = 0, position;
    U8 clauseValid;
    U8 resultValid;
    char ruleIfName[IFNAMESIZ];
    U8 flowid;
    U8 extend;
    BOOL isQueueSet = FALSE;
    
    cmsLog_debug("OamAddVlanctl for rule of port: %d, priority: %d\n", pRule->port, pRule->priority);

    num_tags = 0;
    while (num_tags < MAX_VLANCTL_TAGS)
        {
        extend = VLANCTL_EXTEND_INIT;
extend_action:
        clauseValid = resultValid = 1;
        
        vlanCtl_initTagRule();
        if (!pRule->isDiscard)//discard implemented on rx, should specific rx device
            vlanCtl_filterOnRxRealDevice(OamVlanCtlGetRxDevName(pRule->port));

        //clause filter
        clauseValid = VlanCtlClauseParse(&num_tags, pRule, &flowid, &extend, TRUE);
        cmsLog_debug("after clause check, clauseValid: %d\n", clauseValid);
        if (clauseValid) 
            {
            U8 queueIdx = INVALID_BYTE;
            U8 linkIdx = INVALID_BYTE;
            vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_IS_UNICAST|
                                  BCM_VLAN_FILTER_FLAGS_IS_MULTICAST|
                                  BCM_VLAN_FILTER_FLAGS_IS_BROADCAST);
            isQueueSet = OamGetDestPortAndQueue(pRule, &(infoPtr->destPort), &queueIdx);
            //upstream must set link and queue when not discard
            if ((!pRule->isDiscard) && 
                (pRule->port) &&
                (!isQueueSet))
                {
                num_tags++;
                continue;
                }

            //for upstream TX rule, should filter tx vlan interface
            if ((!pRule->isDiscard) && (pRule->port))
                {
                snprintf(ruleIfName, IFNAMESIZ, "%s", eponVlanIfname);
                vlanCtl_filterOnTxVlanDevice(ruleIfName);
                //restore upstream destination port to PON
                linkIdx = infoPtr->destPort;
                //0 mean Pon side
                infoPtr->destPort = 0;
                }

            resultValid = VlanCtlResultParse(num_tags, infoPtr, pRule, extend);
            cmsLog_debug("after result check, dstport: %x, resultValid: %d\n", infoPtr->destPort, resultValid);
            if (resultValid)
                {
                //U8 i;
                vlanCtl_setVlanRuleTableType(BCM_VLAN_RULE_TYPE_FLOW);

                if (pRule->isDiscard)
                    {//rx action only for discard
                    //this section doesn't not extend rule
                    infoPtr->destPort = pRule->port;
                    
                    snprintf(ruleIfName, IFNAMESIZ, "%s.%d", 
                        OamVlanCtlGetRxDevName(infoPtr->destPort), 
                        (pRule->port == 0)?(flowid==0xFF?1:(flowid+1)):0);
                    vlanCtl_setReceiveVlanDevice(ruleIfName);

                    //lookup the position of this rule by priority
                    posRuleId = VlanCtlRxInsertBeforeRuleId(pRule->port, infoPtr->index, num_tags);
                    if (posRuleId == TABLE_IDX_INVALID)//add before a rule
                        posRuleId = 0;

                    vlanCtl_insertTagRule(OamVlanCtlGetRxDevName(infoPtr->destPort), 
                            VLANCTL_DIRECTION_RX, num_tags, VLANCTL_POSITION_BEFORE, posRuleId, &ruleId);
                    
                    infoPtr->vlanCtlInfo[0][num_tags].valid = 1;
                    infoPtr->vlanCtlInfo[0][num_tags].nbrOfTags = num_tags;
                    infoPtr->vlanCtlInfo[0][num_tags].dir = VLANCTL_DIRECTION_RX;
                    infoPtr->vlanCtlInfo[0][num_tags].tagRuleId = ruleId;
                    }
                else
                    {
                    //this section needs extend rule for priority-tagged frame
                    //normal rule when extend==VLANCTL_EXTEND_TODO.
                    //extend rule when extend==VLANCTL_EXTEND_NOW.
                    U8 destPort, i;
                    U8 ruleTagNum = (extend==VLANCTL_EXTEND_TODO) ? num_tags : num_tags+3;
                    U8 portMax = (pRule->port || (infoPtr->destPort != DN_DIR_ALL)) ? 1 : UniCfgDbGetActivePortCount();

                    if ((pRule->port) && (linkIdx != INVALID_BYTE) && (queueIdx != INVALID_BYTE))
                        {//upstream should specific new mark and queue
                        vlanCtl_cmdSetSkbMarkPort(linkIdx);
                        if (qosManagedByOAM)
                            vlanCtl_cmdSetSkbMarkQueue(queueIdx);
                        }

                    //for downsteam, configure rule on port 1~4 uni port.
                    //for upstream, configure rule on port 0 pon port.
                    for (i = 0; i < portMax; i ++)
                        {
                        destPort = (infoPtr->destPort == DN_DIR_ALL) ? (i+1) : infoPtr->destPort;
                        //lookup the position of this rule by priority
                        if (extend == VLANCTL_EXTEND_TODO)
                            {
                            posRuleId = VlanCtlTxInsertBeforeRuleId(pRule->port, destPort, infoPtr->index, num_tags);
                            vlanctlInsertRulePosition(pRule->port, num_tags, &position, &posRuleId);
                            }
                        else
                            {
                            position = VLANCTL_POSITION_BEFORE;
                            posRuleId = infoPtr->vlanCtlInfo[i][num_tags].tagRuleId;
                            }

                        vlanCtl_insertTagRule( OamVlanCtlGetTxDevName(destPort), 
                                VLANCTL_DIRECTION_TX, num_tags, position, posRuleId, &ruleId);
                        
                        infoPtr->vlanCtlInfo[i][ruleTagNum].valid = 1;
                        if (extend == VLANCTL_EXTEND_NOW)
                            infoPtr->vlanCtlInfo[i][num_tags].extend = 1;
                        infoPtr->vlanCtlInfo[i][ruleTagNum].nbrOfTags = num_tags;
                        infoPtr->vlanCtlInfo[i][ruleTagNum].dir = VLANCTL_DIRECTION_TX;
                        infoPtr->vlanCtlInfo[i][ruleTagNum].tagRuleId = ruleId;
                        }

                    if (extend == VLANCTL_EXTEND_TODO)
                        goto extend_action;
                    }
                }
            } 
            num_tags ++;
        }
//        PrintLocalPrtyInfoByIdxPortList(pRule->port, 0);   
    return OamErrNoError;
    }

static void OamDelSlowPathForPhysicalInterface(RuleDesc* pRule, ruleLocalPrtyInfo *infoPtr)
    {
    U8 i, j;

    cmsLog_debug("OamDelSlowPath for rule of destport: %d\n", infoPtr->destPort);
    /* Upstream 
    discard, dir rx, eth0,eth1,eth2,eth3
    forward, dir tx, epon0*/
    /* Downstream 
    discard, dir rx, epon
    forward, dir tx, eth0~eth3
                         eth0,eth1,eth2,eth3*/

    for (i =0; i < MAX_VLANCTL_TAGS; i ++)
            {//i = nTags.
            if (pRule->isDiscard)
                {//dir rx
                for (j = 0; j < UniCfgDbGetActivePortCount(); j++)
                    {
                    if (infoPtr->vlanCtlInfo[j][i].valid)
                        {
                        vlanCtl_removeTagRule(OamVlanCtlGetRxDevName(pRule->port),
                                        infoPtr->vlanCtlInfo[j][i].dir,
                                        infoPtr->vlanCtlInfo[j][i].nbrOfTags,
                                        infoPtr->vlanCtlInfo[j][i].tagRuleId);
                        }
                    }
                }
            else
                {//dir tx
                //upstream destport epon0 pon port
                //downstream destport eth0/eth1/eth2/eth3 if Queue specificed
                //downstream destport eth0~eth3 flooding
                U8 portMax = (pRule->port || (infoPtr->destPort != DN_DIR_ALL)) ? 1 : UniCfgDbGetActivePortCount();
                U8 txPort;

                for (j = 0; j < portMax; j++)
                    {
                    txPort = (infoPtr->destPort == DN_DIR_ALL) ? (j+1) : infoPtr->destPort;
                    
                    if (infoPtr->vlanCtlInfo[j][i].valid)
                        {
                        vlanCtl_removeTagRule(OamVlanCtlGetTxDevName(txPort),
                                        infoPtr->vlanCtlInfo[j][i].dir,
                                        infoPtr->vlanCtlInfo[j][i].nbrOfTags,
                                        infoPtr->vlanCtlInfo[j][i].tagRuleId);
                        if (infoPtr->vlanCtlInfo[j][i].extend && (infoPtr->vlanCtlInfo[j][i+3].valid))
                            vlanCtl_removeTagRule(OamVlanCtlGetTxDevName(txPort),
                                        infoPtr->vlanCtlInfo[j][i+3].dir,
                                        infoPtr->vlanCtlInfo[j][i+3].nbrOfTags,
                                        infoPtr->vlanCtlInfo[j][i+3].tagRuleId);
                        }
                    }
                }
            }
    }

static OamError OamAddSlowPath(RuleDesc* pRule, ruleLocalPrtyInfo *infoPtr, BOOL isVirtualPortRule)
    {
    OamError rc = OamErrNoError;
    vlanCtl_init();
    rc = isVirtualPortRule?OamAddSlowPathForVirtualInterface(pRule, infoPtr):
             OamAddSlowPathForPhysicalInterface(pRule, infoPtr);
    vlanCtl_cleanup();
    return rc;
    }

static OamError OamDelSlowPath(RuleDesc* pRule, ruleLocalPrtyInfo *infoPtr, BOOL isVirtualPortRule)
    {
    vlanCtl_init();
    isVirtualPortRule?OamDelSlowPathForVirtualInterface(pRule, infoPtr):
             OamDelSlowPathForPhysicalInterface(pRule, infoPtr);
    vlanCtl_cleanup();
    return OamErrNoError;
    }
#endif

#ifdef OAM_RDPA
static U16 prtyRefCount[2][MAX_DEV_PRTY+1] ={{0,0}};

static void dumpPrtyRefCount(void)
    {
#ifdef OAM_DUMP_RULES
    U16 idx;

    printf("IC Ds prty resource :\n");
    for (idx = 0; idx <= MAX_DEV_PRTY; idx++)
        {
        if (prtyRefCount[0][idx] == 0)
            continue;
        printf("\tPrty %02d : %02d\n", idx, prtyRefCount[0][idx]);
        }

    printf("\nIC Us prty resource :\n");
    for (idx = 0; idx <= MAX_DEV_PRTY; idx++)
        {
        if (prtyRefCount[1][idx] == 0)
            continue;
        printf("\tPrty %02d : %02d\n", idx, prtyRefCount[1][idx]);
        }    
#endif
    }

static void increasePrtyRef(U16 port, U16 prtyIdx)
    {
    U16 dir = (port == 0)?0:1;

    prtyRefCount[dir][prtyIdx]++;

    dumpPrtyRefCount();
    }

static void decreasePrtyRef(U16 port, U16 prtyIdx)
    {
    U16 dir = (port == 0)?0:1;

    if (prtyRefCount[dir][prtyIdx] == 0)
        cmsLog_error("IC %s prty %d resource error !\n", 
                        (dir == 0)?"Ds":"Us", prtyIdx);
    else
        prtyRefCount[dir][prtyIdx]--;

    dumpPrtyRefCount();
    }

static S16 findFreePrtyIdx(U16 port)
    {
    U16 dir = (port == 0)?0:1;
    S16 idx;

    for (idx = MAX_DEV_PRTY; idx >= 0; idx--)
        {
        if (prtyRefCount[dir][idx] == 0)
            break;
        }
    return idx;
    }

static U32 caculateMask(U8 lsb, U8 width)
    {
    U32 mask = 0;
    U8 i;

    for (i = 0; i < width; i++)
        mask |= (1<<(lsb+i));

    return mask;
    }

static U32 OamGetCustomMatchValue(U8  *data, U8 valLen, U8 shiftVal)
    {
    U32 tmpValue = 0 , retValue = 0;
    U8 validLen = (valLen > sizeof(U32))?sizeof(U32):valLen;

    memcpy((U8*)&tmpValue+sizeof(U32)-valLen, data, validLen);

    if (validLen < sizeof(U32))
        leftShift((U8 *)&tmpValue, sizeof(U32), shiftVal, (U8 *)&retValue);
    else
        retValue = tmpValue;

    return retValue;
    }

static BOOL OamRdpaGenericRuleParse(rdpactl_classification_rule_t *rdpaRule, 
                                    OamRuleClauseData *pClause)
    {
    U8 valLen = GET_GENERAL_CLAUSE_VALUE_LENGTH(pClause);
    U8 lsb = GET_GENERAL_CLAUSE_LSB(pClause);
    U8 msb = GET_GENERAL_CLAUSE_MSB(pClause);
    U8 totalBits, fieldLen, baseOffset = 0;
    rdpa_offset_t type;
    U32 offset, mask;  
    U32 tmpDword;
    U32 dWord;
    U8  newLsb;
    U8  valBuf[16] = {0};
    U8  tmpBuf[16] = {0};
    U8  bitWidth, alignByte;
    rdpactl_ic_gen_rule_cfg_t *rdpa_gen_rule = NULL;
    
    switch (pClause->fieldCode)
        {
        case OamFieldL2Da:
            baseOffset = 0;
            fieldLen = 6;
            type = RDPA_OFFSET_L2;
            break;

        case OamFieldL2Sa:
            baseOffset = 6;
            fieldLen = 6;
            type = RDPA_OFFSET_L2;
            break;
            
        case OamFieldIpv4Sa:
            baseOffset = 12;
            fieldLen = 4;
            type = RDPA_OFFSET_L3;
            break;

        case OamFieldIpv4Da:
            baseOffset = 16;            
            fieldLen = 4;
            type = RDPA_OFFSET_L3;
            break;

        case OamFieldIpv6Sa:
            baseOffset = 8;
            fieldLen = 16;            
            type = RDPA_OFFSET_L3;
            break;

        case OamFieldIpv6Da:
            baseOffset = 24; 
            fieldLen = 16;            
            type = RDPA_OFFSET_L3;
            break;
        default:
            return FALSE;
        }


/*     <-       Field Len (Bytes)                  ->       */
/*     ----------------------------------------------       */
/*     |        |                    |              |       */
/*     |        |                    |              |       */
/*     ----------------------------------------------       */
/*       msb  ->                     <-   lsb               */


    if (fieldLen*8 < (msb+lsb))
        return FALSE;
    
    totalBits = fieldLen*8 - msb - lsb;
    if (totalBits > 64)
        return FALSE; /* We only have total 8 bytes custom fields */

    if ((totalBits == 64) && (lsb%16 != 0))
        return FALSE; /* Total len is 8 bytes, but not 2 bytes aligned */

    if (valLen == fieldLen)
        memcpy(valBuf, pClause->clauseContent.generalClause.matchVal.u8, valLen);
    else
        {
        memcpy(tmpBuf+fieldLen-valLen, pClause->clauseContent.generalClause.matchVal.u8,
               valLen);
        leftShift(tmpBuf, fieldLen, lsb, valBuf);
        }
    
    alignByte = msb/16*2;
    offset = baseOffset + alignByte;
    msb = msb - alignByte*8;
    
    for (bitWidth = totalBits;;)
        {        
   
        if (bitWidth >= (32 - msb))
            newLsb = 0;
        else
            newLsb = 32 - msb - bitWidth;
        
        tmpDword = OamGetCustomMatchValue(&valBuf[offset - baseOffset], 4, newLsb);
        dWord = OAM_NTOHL(tmpDword);
        
        mask = caculateMask(newLsb, 32 - msb - newLsb);
            
        if((rdpaRule->field_mask & RDPACTL_IC_MASK_GENERIC_1) == 0)
            {
            rdpaRule->field_mask |= RDPACTL_IC_MASK_GENERIC_1;
            rdpa_gen_rule = &rdpaRule->gen_rule_cfg1;
            rdpaRule->gen_rule_key_1 = dWord & mask;
            }
        else if((rdpaRule->field_mask & RDPACTL_IC_MASK_GENERIC_2) == 0)
            {
            rdpaRule->field_mask |= RDPACTL_IC_MASK_GENERIC_2;
            rdpa_gen_rule = &rdpaRule->gen_rule_cfg2;
            rdpaRule->gen_rule_key_2 = dWord & mask;
            }
        else
            { /* Don't support more than 2 generic fields */
            cmsLog_error("Don't have enough resource\n");
            return FALSE;
            }
        rdpa_gen_rule->type = type;
        rdpa_gen_rule->offset = offset;
        rdpa_gen_rule->mask = mask;

        /* Next 4 bytes */
        offset += 4;  
        if (bitWidth <= (32-msb))
            break;

        bitWidth = bitWidth - (32 - msb);
        msb = 0;            
        }

    return TRUE;
    }

static void OamRdpaClauseFillVlanInfo(int validIdx, U16* vlanTpid, U8* vlanPbit,
                                      U16* vlanId, rdpactl_classification_rule_t *rdpaRule)
    {    
    if (SVLAN_0_PRESENT(globalVlanFields[validIdx].vlanStackInfo))
        {
        /* Set Outer TPID */
        rdpaRule->field_mask |= RDPACTL_IC_MASK_OUTER_TPID;
        rdpaRule->outer_tpid = (vlanTpid[SVLAN_0_INST_ID] != INVALID_WORD) ? vlanTpid[SVLAN_0_INST_ID] : OamAltSVlanTpidGet();

        if (vlanId[SVLAN_0_INST_ID] != INVALID_WORD)
            {
            rdpaRule->field_mask |= RDPACTL_IC_MASK_OUTER_VID;
            rdpaRule->outer_vid = vlanId[SVLAN_0_INST_ID];
            }

        if (vlanPbit[SVLAN_0_INST_ID] != INVALID_BYTE)
            {
            rdpaRule->field_mask |= RDPACTL_IC_MASK_OUTER_PBIT;
            rdpaRule->outer_pbits = vlanPbit[SVLAN_0_INST_ID];
            }
        }

    if (SVLAN_1_PRESENT(globalVlanFields[validIdx].vlanStackInfo))
        {
        /* Set Inner TPID */
        rdpaRule->field_mask |= RDPACTL_IC_MASK_INNER_TPID;
        rdpaRule->inner_tpid = (vlanTpid[SVLAN_1_INST_ID] != INVALID_WORD) ? vlanTpid[SVLAN_1_INST_ID] : OamAltSVlanTpidGet();
    
        if (vlanId[SVLAN_1_INST_ID] != INVALID_WORD)
            {
            rdpaRule->field_mask |= RDPACTL_IC_MASK_INNER_VID;
            rdpaRule->inner_vid = vlanId[SVLAN_1_INST_ID];
            }
    
        if (vlanPbit[SVLAN_1_INST_ID] != INVALID_BYTE)
            {
            rdpaRule->field_mask |= RDPACTL_IC_MASK_INNER_PBIT;
            rdpaRule->inner_pbits = vlanPbit[SVLAN_1_INST_ID];
            }
        }

    if (CVLAN_0_PRESENT(globalVlanFields[validIdx].vlanStackInfo))
        {
        /* Set Outer TPID */
        rdpaRule->field_mask |= (SVLAN_PRESENT(globalVlanFields[validIdx].vlanStackInfo)?
                                    RDPACTL_IC_MASK_INNER_TPID:RDPACTL_IC_MASK_OUTER_TPID);
        if (SVLAN_PRESENT(globalVlanFields[validIdx].vlanStackInfo))
            rdpaRule->inner_tpid = (vlanTpid[CVLAN_0_INST_ID] != INVALID_WORD) ? vlanTpid[CVLAN_0_INST_ID] : OamAltCVlanTpidGet();
        else
            rdpaRule->outer_tpid = (vlanTpid[CVLAN_0_INST_ID] != INVALID_WORD) ? vlanTpid[CVLAN_0_INST_ID] : OamAltCVlanTpidGet();
    
        if (vlanId[CVLAN_0_INST_ID] != INVALID_WORD)
            {
            rdpaRule->field_mask |= (SVLAN_PRESENT(globalVlanFields[validIdx].vlanStackInfo)?
                                    RDPACTL_IC_MASK_INNER_VID:RDPACTL_IC_MASK_OUTER_VID);
            if (SVLAN_PRESENT(globalVlanFields[validIdx].vlanStackInfo))
                rdpaRule->inner_vid = vlanId[CVLAN_0_INST_ID];
            else
                rdpaRule->outer_vid = vlanId[CVLAN_0_INST_ID];                
            }
    
        if (vlanPbit[CVLAN_0_INST_ID] != INVALID_BYTE)
            {
            rdpaRule->field_mask |= (SVLAN_PRESENT(globalVlanFields[validIdx].vlanStackInfo)?
                                    RDPACTL_IC_MASK_INNER_PBIT:RDPACTL_IC_MASK_OUTER_PBIT);
            if (SVLAN_PRESENT(globalVlanFields[validIdx].vlanStackInfo))
                rdpaRule->inner_pbits = vlanPbit[CVLAN_0_INST_ID];
            else
                rdpaRule->outer_pbits = vlanPbit[CVLAN_0_INST_ID];
            }
        }

    if (CVLAN_1_PRESENT(globalVlanFields[validIdx].vlanStackInfo))
        {
        /* Set Inner TPID */
        rdpaRule->field_mask |= RDPACTL_IC_MASK_INNER_TPID;
        rdpaRule->inner_tpid = (vlanTpid[CVLAN_1_INST_ID] != INVALID_WORD) ? vlanTpid[CVLAN_1_INST_ID] : OamAltCVlanTpidGet();
    
        if (vlanId[CVLAN_1_INST_ID] != INVALID_WORD)
            {
            rdpaRule->field_mask |= RDPACTL_IC_MASK_INNER_VID;
            rdpaRule->inner_vid = vlanId[CVLAN_1_INST_ID];
            }
    
        if (vlanPbit[CVLAN_1_INST_ID] != INVALID_BYTE)
            {
            rdpaRule->field_mask |= RDPACTL_IC_MASK_INNER_PBIT;
            rdpaRule->inner_pbits = vlanPbit[CVLAN_1_INST_ID];
            }
        }

    return;
    }
static U8 OamRdpaClauseParse(rdpactl_classification_rule_t *rdpaRule, 
                             RuleDesc *pRule)
    {
    int i;
    U8 icValid = 1;
    U8 valLen;
    U8 vlanInst = 0;
    U8 vlanPbit[MAX_VLAN_INSTANCES] = {INVALID_BYTE, INVALID_BYTE, INVALID_BYTE, INVALID_BYTE};
    U16 vlanVid[MAX_VLAN_INSTANCES] = {INVALID_WORD, INVALID_WORD, INVALID_WORD, INVALID_WORD};
    U16 vlanTpid[MAX_VLAN_INSTANCES] = {INVALID_WORD, INVALID_WORD, INVALID_WORD, INVALID_WORD};
    U16 llidValue = INVALID_WORD;
    U8  linkIndex = INVALID_BYTE;
    U8  idx      = 0;
    U32 addrIpv4 = 0;
    U32 tmpDword = 0;
    U32 dWord    = 0;
    U16 port     = 0;
    U32 vlanFieldMark = 0;

    OamRuleClauseData *pClause = NULL;
    
    if (pRule->isAlways)
        {
        return icValid;
        }
        
    for (i = 0; i < pRule->clauseCount; i++) 
        {
        if (icValid == 0)
            {//parameter error happened, just reture
            break;
            }
        
        pClause = &pRule->clauseData[i].clause;
        valLen = GET_GENERAL_CLAUSE_VALUE_LENGTH(pClause);
        switch (pClause->fieldCode) {
            case OamFieldLinkIndex:
                rdpaRule->field_mask |= RDPACTL_IC_MASK_INGRESS_WANFLOW;
                rdpaRule->ingress_wan_flow = GET_GENERAL_CLAUSE_VALUE_RIGHT_BYTE(pClause);
                break;

            case OamFieldLlidValue:
                rdpaRule->field_mask |= RDPACTL_IC_MASK_INGRESS_WANFLOW;
                COPY_GENERAL_CLAUSE_VALUE_RIGHT_WORD(pClause, &llidValue);
                if((!eponStack_CtlGetLinkIndex(OAM_NTOHS(llidValue), &linkIndex)) &&
                    (OamLlidIndexValid(linkIndex)))
                    rdpaRule->ingress_wan_flow = linkIndex;
                else
                    icValid = 0;
                break;
                
            case OamFieldL2Da:
                if (GET_GENERAL_CLAUSE_MSB(pClause) || 
                    GET_GENERAL_CLAUSE_LSB(pClause))
                    {
                    icValid = OamRdpaGenericRuleParse(rdpaRule, pClause)?1:0;
                    }
                else
                    {
                    rdpaRule->field_mask |= RDPACTL_IC_MASK_DST_MAC;
                    memcpy(rdpaRule->dst_mac, &(GET_GENERAL_CLAUSE(pClause).matchVal.u8[0]), sizeof(rdpaRule->dst_mac));
                    }                
                break;

            case OamFieldL2Sa:
                 if (GET_GENERAL_CLAUSE_MSB(pClause) || 
                    GET_GENERAL_CLAUSE_LSB(pClause))
                    {
                    icValid = OamRdpaGenericRuleParse(rdpaRule, pClause)?1:0;
                    }
                else
                    {                
                    rdpaRule->field_mask |= RDPACTL_IC_MASK_SRC_MAC;
                    memcpy(rdpaRule->src_mac, &(GET_GENERAL_CLAUSE(pClause).matchVal.u8[0]), sizeof(rdpaRule->src_mac));
                    }
                break;
       
            case OamFieldL2Type:
                rdpaRule->field_mask |= RDPACTL_IC_MASK_ETHER_TYPE;
                OamGetEthertypeFromClause(pClause,valLen,&(rdpaRule->etype));
                break;
 
            case OamFieldBDa:
            case OamFieldBSa:
            case OamFieldITag:
                icValid = 0;
                cmsLog_debug("RDPA doesn't support it\n");
                break;

            case OamFieldSVlan:
                vlanInst = pClause->clauseContent.generalClause.instance?SVLAN_1_INST_ID:SVLAN_0_INST_ID;
                OamGetVlanFromClause(pClause,valLen,&vlanVid[vlanInst], &vlanPbit[vlanInst], &vlanTpid[vlanInst]);
                vlanFieldMark |= ((vlanInst==SVLAN_1_INST_ID)?SVLAN_1_BIT:SVLAN_0_BIT);
                break;
            
            case OamFieldCVlan:
                vlanInst = pClause->clauseContent.generalClause.instance?CVLAN_1_INST_ID:CVLAN_0_INST_ID;
                OamGetVlanFromClause(pClause,valLen,&vlanVid[vlanInst], &vlanPbit[vlanInst], &vlanTpid[vlanInst]);
                vlanFieldMark |= ((vlanInst==CVLAN_1_INST_ID)?CVLAN_1_BIT:CVLAN_0_BIT);                
                break;
                                               
            case OamFieldMpls:
                icValid = 0;
                cmsLog_debug("RDPA doesn't support it\n");
                break;
       
            case OamFieldIpv4Tos:
                rdpaRule->field_mask |= RDPACTL_IC_MASK_DSCP;
                rdpaRule->dscp = GET_GENERAL_CLAUSE_VALUE_RIGHT_BYTE(pClause);
                break;
                                               
            case OamFieldIpv4Ttl:
                icValid = 0;
                cmsLog_debug("RDPA doesn't support it\n");
                break;
       
            case OamFieldIpProtocolType:
                rdpaRule->field_mask |= RDPACTL_IC_MASK_IP_PROTOCOL;
                rdpaRule->protocol = GET_GENERAL_CLAUSE_VALUE_RIGHT_BYTE(pClause);
                break;
                        
            case OamFieldIpv4Sa:
                if (GET_GENERAL_CLAUSE_MSB(pClause) || 
                    GET_GENERAL_CLAUSE_LSB(pClause))
                    {
                    icValid = OamRdpaGenericRuleParse(rdpaRule, pClause)?1:0;
                    }
                else
                    {                
                    if (valLen < 4)
                        {//too short
                        cmsLog_error("IPv4 Sa is too short (%d)\n", valLen);
                        icValid = 0;
                        break;
                        }
                    rdpaRule->field_mask |= RDPACTL_IC_MASK_SRC_IP;
                    rdpaRule->ip_family = RDPACTL_IP_FAMILY_IPV4;
                    COPY_GENERAL_CLAUSE_VALUE_RIGHT_DWORD(pClause, (U8*)&(addrIpv4));
                    rdpaRule->src_ip.ipv4 = OAM_NTOHL(addrIpv4);
                    }
                break;
                        
            case OamFieldIpv6Sa:
                if (GET_GENERAL_CLAUSE_MSB(pClause) || 
                    GET_GENERAL_CLAUSE_LSB(pClause))
                    {
                    icValid = OamRdpaGenericRuleParse(rdpaRule, pClause)?1:0;
                    }
                else
                    {                
                    if (valLen < 16)
                        {//too short
                        cmsLog_error("IPv6 Sa is too short (%d)\n", valLen);
                        icValid = 0;
                        break;
                        }
                    rdpaRule->field_mask |= RDPACTL_IC_MASK_SRC_IP;
                    rdpaRule->ip_family = RDPACTL_IP_FAMILY_IPV6;
                    memcpy(rdpaRule->src_ip.ipv6, &(GET_GENERAL_CLAUSE(pClause).matchVal.u8[0]), 
                                                        sizeof(rdpaRule->src_ip.ipv6));
                    }
                break;
                        
            case OamFieldIpv4Da:
                if (GET_GENERAL_CLAUSE_MSB(pClause) || 
                    GET_GENERAL_CLAUSE_LSB(pClause))
                    {
                    icValid = OamRdpaGenericRuleParse(rdpaRule, pClause)?1:0;
                    }
                else
                    {                
                    if (valLen < 4)
                        {//too short
                        cmsLog_error("IPv4 Sa is too short (%d)\n", valLen);
                        icValid = 0;
                        break;
                        }
                    rdpaRule->field_mask |= RDPACTL_IC_MASK_DST_IP;
                    rdpaRule->ip_family = RDPACTL_IP_FAMILY_IPV4;
                    
                    COPY_GENERAL_CLAUSE_VALUE_RIGHT_DWORD(pClause, (U8*)&(addrIpv4));
                    rdpaRule->dst_ip.ipv4 = OAM_NTOHL(addrIpv4);
                    }
                break;
       
            case OamFieldIpv6Da:
                if (GET_GENERAL_CLAUSE_MSB(pClause) || 
                    GET_GENERAL_CLAUSE_LSB(pClause))
                    {
                    icValid = OamRdpaGenericRuleParse(rdpaRule, pClause)?1:0;
                    }
                else
                    {                
                    if (valLen < 16)
                        {//too short
                        cmsLog_error("IPv6 Sa is too short (%d)\n", valLen);
                        icValid = 0;
                        break;
                        }
                    rdpaRule->field_mask |= RDPACTL_IC_MASK_DST_IP;
                    rdpaRule->ip_family = RDPACTL_IP_FAMILY_IPV6;
                    memcpy(rdpaRule->dst_ip.ipv6, &(GET_GENERAL_CLAUSE(pClause).matchVal.u8[0]), 
                                                        sizeof(rdpaRule->dst_ip.ipv6));
                    }
                break;
                                               
            case OamFieldIpv6NextHeader:
                icValid = 0;
                cmsLog_debug("RDPA doesn't support it\n");
                break;
                
            case OamFieldIpv6FlowLabel:
                rdpaRule->field_mask |= RDPACTL_IC_MASK_IPV6_FLOW_LABEL;
                if (valLen > 4)
                    {
                    COPY_GENERAL_CLAUSE_VALUE_RIGHT_DWORD(pClause, &tmpDword);
                    }
                else
                    {
                    COPY_GENERAL_CLAUSE_VALUE_ALL_DATA(pClause, &tmpDword, valLen);
                    }
                
                rdpaRule->ipv6_label = OAM_NTOHL(tmpDword);
                break;
            case OamFieldTcpUdpSrcPort:
                if (pRule->isGreaterLess && GET_GENERAL_CLAUSE_OP(pClause) != OamRuleOpGreaterEq)
                    break;
                
                rdpaRule->field_mask |= RDPACTL_IC_MASK_SRC_PORT;
                if (valLen == 1)
                    {
                    rdpaRule->src_port = (U16)GET_GENERAL_CLAUSE_VALUE_BYTE(pClause);
                    }
                else
                    {
                    COPY_GENERAL_CLAUSE_VALUE_RIGHT_WORD(pClause, &port);
                    rdpaRule->src_port = OAM_NTOHS(port);
                    }
                break;
       
            case OamFieldTcpUdpDestPort:
                if (pRule->isGreaterLess && GET_GENERAL_CLAUSE_OP(pClause) != OamRuleOpGreaterEq)
                    break;
                
                rdpaRule->field_mask |= RDPACTL_IC_MASK_DST_PORT;
                if (valLen == 1)
                    {
                    rdpaRule->dst_port = (U16)GET_GENERAL_CLAUSE_VALUE_BYTE(pClause);
                    }
                else
                    {
                    COPY_GENERAL_CLAUSE_VALUE_RIGHT_WORD(pClause, &port);
                    rdpaRule->dst_port = OAM_NTOHS(port);
                    }
                break;

            case OamFieldCustom0 ... OamFieldCustom7:
                {
                idx = pClause->fieldCode - OamFieldCustom0;
                rdpactl_ic_gen_rule_cfg_t *rdpa_gen_rule = NULL;

                 tmpDword = OamGetCustomMatchValue(pClause->clauseContent.generalClause.matchVal.u8, 
                                                  valLen,
                                        customFieldRdpaCfg[idx].oamCustomCfg.lsb);

                dWord = OAM_NTOHL(tmpDword);
                if((rdpaRule->field_mask & RDPACTL_IC_MASK_GENERIC_1) == 0)
                    {
                    rdpaRule->field_mask |= RDPACTL_IC_MASK_GENERIC_1;
                    rdpa_gen_rule = &rdpaRule->gen_rule_cfg1;
                    rdpaRule->gen_rule_key_1 = dWord&customFieldRdpaCfg[idx].customFieldRdpaInfo.mask;
                    }
                else if((rdpaRule->field_mask & RDPACTL_IC_MASK_GENERIC_2) == 0)
                    {
                    rdpaRule->field_mask |= RDPACTL_IC_MASK_GENERIC_2;
                    rdpa_gen_rule = &rdpaRule->gen_rule_cfg2;
                    rdpaRule->gen_rule_key_2 = dWord&customFieldRdpaCfg[idx].customFieldRdpaInfo.mask;
                    }
                else
                    { /* Don't support more than 2 generic fields */
                    cmsLog_error("Don't have enough resource\n");
                    icValid = 0;
                    break;
                    }
                rdpa_gen_rule->type = customFieldRdpaCfg[idx].customFieldRdpaInfo.type;
                rdpa_gen_rule->offset = customFieldRdpaCfg[idx].customFieldRdpaInfo.offset;
                rdpa_gen_rule->mask = customFieldRdpaCfg[idx].customFieldRdpaInfo.mask;

                if (customFieldRdpaCfg[idx].fieldMask != 0)
                    {
                    rdpaRule->field_mask |= customFieldRdpaCfg[idx].fieldMask;

                    if (customFieldRdpaCfg[idx].fieldMask & RDPACTL_IC_MASK_OUTER_TPID)
                        rdpaRule->outer_tpid = customFieldRdpaCfg[idx].matchValue[0];
                    else if (customFieldRdpaCfg[idx].fieldMask & RDPACTL_IC_MASK_L3_PROTOCOL)
                        rdpaRule->version = customFieldRdpaCfg[idx].matchValue[0];
                    else if (customFieldRdpaCfg[idx].fieldMask & RDPACTL_IC_MASK_IP_PROTOCOL)
                        rdpaRule->protocol = customFieldRdpaCfg[idx].matchValue[0];
                    }
                break;
                }                                               
            case OamFieldBTag:            
            default:
                icValid = 0;
                cmsLog_debug("RDPA doesn't support it\n");
                break;
            }
        }

    //do vlan tag classification here
    if (icValid && vlanFieldMark)
        {
        for (i = 0; i < VLAN_CLAUSE_SUPPORTED_CASES - 1; i++)
            {
            if (globalVlanFields[i].vlanStackInfo == vlanFieldMark)
                break;
            }
        if (i < VLAN_CLAUSE_SUPPORTED_CASES)
            {
            // Handle Vlan field
            OamRdpaClauseFillVlanInfo(i, &vlanTpid, &vlanPbit, &vlanVid, rdpaRule);
            }
        else
            {
            icValid = FALSE;
            }
        }
    return icValid;
    }

static U8 OamRdpaResultDeleteVlanProcess(BOOL deleteSvlanTag,
                                         BOOL deleteCvlanTag,
                                         rdpactl_vtag_action_t* pRdpaVlanAction)
    {
    U8 resultValid = 1;
    BOOL isPop2 = ((deleteSvlanTag == TRUE) && (deleteCvlanTag == TRUE));

    if (isPop2)
        pRdpaVlanAction->cmd = RDPACTL_VLAN_CMD_POP2;
    else
        pRdpaVlanAction->cmd = RDPACTL_VLAN_CMD_POP;
    
    return resultValid;
    }

static U8 OamRdpaResultReplaceVlanProcess(BOOL replaceSvlanTag,
                             BOOL replaceCvlanTag,
                             VlanctlVlanActionPara* pVlanActionValue,
                             rdpactl_vtag_action_t* pRdpaVlanAction)
    {
    U8 resultValid = 1;
    BOOL isOnlyCvlanReplaced = (replaceSvlanTag == FALSE) && (replaceCvlanTag == TRUE);
    U8 cVlanIndex = isOnlyCvlanReplaced?RDPACTL_VLAN_TAG_OUT:RDPACTL_VLAN_TAG_IN;
    
    if (replaceSvlanTag == TRUE)
        {
        if (pVlanActionValue->svlanVid== INVALID_WORD)
            resultValid = 0;
        
        pRdpaVlanAction->cmd = RDPACTL_VLAN_CMD_REPLACE;
        pRdpaVlanAction->parm[RDPACTL_VLAN_TAG_OUT].vid = pVlanActionValue->svlanVid;
        if (pVlanActionValue->svlanPbit != INVALID_BYTE)
            {
            pRdpaVlanAction->cmd |= RDPACTL_VLAN_CMD_REMARK;
            pRdpaVlanAction->parm[RDPACTL_VLAN_TAG_OUT].pbit = pVlanActionValue->svlanPbit;
            }
        if (pVlanActionValue->svlanTpid != INVALID_WORD)
            {
            pRdpaVlanAction->cmd |= RDPACTL_VLAN_CMD_TPID_REMARK;
            pRdpaVlanAction->parm[RDPACTL_VLAN_TAG_OUT].tpid = pVlanActionValue->svlanTpid;
            }
        }
    if (replaceCvlanTag == TRUE)
        {
        if (pVlanActionValue->cvlanVid == INVALID_WORD)
            resultValid = 0;
        
        pRdpaVlanAction->cmd = isOnlyCvlanReplaced?RDPACTL_VLAN_CMD_REPLACE:RDPACTL_VLAN_CMD_REPLACE2;
        pRdpaVlanAction->parm[cVlanIndex].vid = pVlanActionValue->cvlanVid;
        if (pVlanActionValue->cvlanPbit != INVALID_BYTE)
            {
            pRdpaVlanAction->cmd |= RDPACTL_VLAN_CMD_REMARK;
            pRdpaVlanAction->parm[cVlanIndex].pbit = pVlanActionValue->cvlanPbit;
            }
        if (pVlanActionValue->cvlanTpid != INVALID_WORD)
            {
            pRdpaVlanAction->cmd |= RDPACTL_VLAN_CMD_TPID_REMARK;
            pRdpaVlanAction->parm[cVlanIndex].tpid = pVlanActionValue->cvlanTpid;
            }
        }
    return resultValid;
    }

static U8 OamRdpaResultPushVlanProcess(BOOL pushSvlanTag,
                             BOOL pushCvlanTag,
                             VlanctlVlanActionPara* pVlanActionValue,
                             rdpactl_vtag_action_t* pRdpaVlanAction)
    {
    U8 resultValid = 1;

    cmsLog_debug("pushSvlanTag %d, pushCvlanTag %d\n", pushSvlanTag, pushCvlanTag);
    cmsLog_debug("svlanVid %d, cvlanVid %d\n", pVlanActionValue->svlanVid, pVlanActionValue->cvlanVid);
    
    /*Insert vlan action
    1. insert svlan
    2. insert cvlan
    3. insert 2 tags is not supported
    */
    if (pushSvlanTag && pushCvlanTag)
        resultValid = 0;
    
    //svlan
    if (pushSvlanTag)
        {
        if (pVlanActionValue->svlanVid == INVALID_WORD)
            resultValid = 0;
            
        pRdpaVlanAction->cmd = RDPACTL_VLAN_CMD_PUSH_ALWAYS;
        pRdpaVlanAction->parm[RDPACTL_VLAN_TAG_OUT].vid = pVlanActionValue->svlanVid;
        if (pVlanActionValue->svlanPbit != INVALID_BYTE)
            {
            pRdpaVlanAction->cmd |= RDPACTL_VLAN_CMD_REMARK;
            pRdpaVlanAction->parm[RDPACTL_VLAN_TAG_OUT].pbit = pVlanActionValue->svlanPbit;
            }
        if (pVlanActionValue->svlanTpid != INVALID_WORD)
            {
            pRdpaVlanAction->cmd |= RDPACTL_VLAN_CMD_TPID_REMARK;
            pRdpaVlanAction->parm[RDPACTL_VLAN_TAG_OUT].tpid = pVlanActionValue->svlanTpid;
            pRdpaVlanAction->parm[RDPACTL_VLAN_TAG_IN].tpid = RDPA_VLAN_ACTION_TPID_VALUE_DONT_CARE;
            }
        }
    
    //cvlan
    if (pushCvlanTag)
        {
        if (pVlanActionValue->cvlanVid == INVALID_WORD)
            resultValid = 0;
            
        pRdpaVlanAction->cmd = RDPACTL_VLAN_CMD_PUSH_ALWAYS;
        pRdpaVlanAction->parm[RDPACTL_VLAN_TAG_OUT].vid = pVlanActionValue->cvlanVid;
        if (pVlanActionValue->cvlanPbit != INVALID_BYTE)
            {
            pRdpaVlanAction->cmd |= RDPACTL_VLAN_CMD_REMARK;
            pRdpaVlanAction->parm[RDPACTL_VLAN_TAG_OUT].pbit = pVlanActionValue->cvlanPbit;
            }
        if (pVlanActionValue->cvlanTpid != INVALID_WORD)
            {
            pRdpaVlanAction->cmd |= RDPACTL_VLAN_CMD_TPID_REMARK;
            pRdpaVlanAction->parm[RDPACTL_VLAN_TAG_OUT].tpid = pVlanActionValue->cvlanTpid;
            pRdpaVlanAction->parm[RDPACTL_VLAN_TAG_IN].tpid = RDPA_VLAN_ACTION_TPID_VALUE_DONT_CARE;
            }
        }
    
    cmsLog_debug("result parse result %d \n", resultValid);
    return resultValid;
    }

static U8 OamRdpaResultParse(rdpactl_classification_rule_t *rdpaRule, 
                                   RuleDesc *pRule)
    {
    U8 i;
    U8 valLen;
    U8 resultValid = 1;
    U16 sTpid = INVALID_WORD;
    U16 sVid = INVALID_WORD;
    U8 sPbits = INVALID_BYTE;
    U16 cTpid = INVALID_WORD;
    U16 cVid = INVALID_WORD;
    U8 cPbits = INVALID_BYTE;
    BOOL pushSvlanTag = FALSE;
    BOOL pushCvlanTag = FALSE;
    BOOL replaceSvlanTag = FALSE;
    BOOL replaceCvlanTag = FALSE;
    BOOL deleteSvlanTag = FALSE;
    BOOL deleteCvlanTag = FALSE;
    BOOL speQueue = FALSE;
    OamRuleResultData *pResult= NULL;
    VlanctlVlanActionPara vlanActionValue;

    //start parsing
    //result
    for (i = 0; i < pRule->resultCount; i ++)
        {
        if (resultValid == 0)
            {//parameter error happened, just break now
            cmsLog_debug("result parse error return\n");
            break;
            }
        
        pResult = &(pRule->resultData[i].result);
        valLen = pRule->resultData[i].length - 3;
        cmsLog_debug("result type (%d), length (%d)\n", pResult->result, valLen);
        switch (pResult->result)
            {
            case OamResultDiscard:
                //rdpa_forward_action_drop
                rdpaRule->action = 4;
                break;
            
            case OamResultForward:
                //rdpa_forward_action_forward
                rdpaRule->action = 1;
                break;
            
            case OamResultQueue:
                {
                OamQueue queueInfo;
                memcpy(&queueInfo, &pResult->param.dest, sizeof(OamQueue));                    

                if (pRule->port > 0)
                    {//us
                    rdpaRule->egress_port = 0;//wan0
                    rdpaRule->wan_flow = queueInfo.inst;
                    }
                else//ds
                    rdpaRule->egress_port = bcm_enet_map_oam_idx_to_phys_port(queueInfo.inst)+RDPACTL_IF_LAN0;
                
                rdpaRule->forw_mode = rdpactl_forwarding_mode_flow;
                rdpaRule->queue_id = queueInfo.queue;
                speQueue = TRUE;
                }
                break;
            
            case OamResultSet:
                {
                switch(pResult->param.field.fldCode)
                    {
                    case OamFieldSVlan:
                        {
                        OamGetVlanFromResult(pResult,valLen,&sVid,&sPbits,&sTpid);
                        }
                    
                        break;
                        
                    case OamFieldCVlan:
                        {
                        OamGetVlanFromResult(pResult,valLen,&cVid,&cPbits,&cTpid);
                        }
                        break;
                    
/*             Tos conflict with DSCP. waiting for solution       
                case OamDpoeFieldIpv4Tos:
                    rdpaRule->dscp_remark = GET_RESULT_FIELD_VALUE_RIGHT_BYTE(pResult, valLen);
                    break;
*/                    
                    default:
                        cmsLog_debug("The given result field (%d) is not supported by vlanctl for Set operation\n", 
                            pResult->param.field.fldCode);
                        resultValid = 0;
                        break;
                    }
                }
                break;
            
            case OamResultCopy:
                //TBD
                break;
            
            case OamResultDelete:
                {
                cmsLog_debug("result field (%d) for Delete\n", pResult->param.field.fldCode);
                switch(pResult->param.field.fldCode)
                    {
                    case OamFieldSVlan:
                        deleteSvlanTag = TRUE;
                        break;
                        
                    case OamFieldCVlan:
                        deleteCvlanTag = TRUE;
                        break;
                    
                    default:
                        cmsLog_debug("The given result field (%d) is not supported by vlanctl for Delete operation\n", 
                            pResult->param.field.fldCode);
                        resultValid = 0;
                        break;
                    }
                }
                break;
            
            case OamResultInsert:
                {
                cmsLog_debug("result field (%d) for Insert\n", pResult->param.field.fldCode);
                switch(pResult->param.field.fldCode)
                    {
                    case OamFieldSVlan:
                        pushSvlanTag = TRUE;
                        break;
                        
                    case OamFieldCVlan:
                        pushCvlanTag = TRUE;
                        break;
                    
                    default:
                        cmsLog_debug("The given result field (%d) is not supported by vlanctl for insert operation\n", 
                            pResult->param.field.fldCode);
                        resultValid = 0;
                        break;
                    }
                }
                break;
            
            case OamResultReplace:
                {
                cmsLog_debug("result field (%d) for Insert\n", pResult->param.field.fldCode);
                switch(pResult->param.field.fldCode)
                    {
                    case OamFieldSVlan:
                        replaceSvlanTag = TRUE;
                        break;
                        
                    case OamFieldCVlan:
                        replaceCvlanTag = TRUE;
                        break;
                    
                    default:
                        cmsLog_debug("The given result field (%d) is not supported by vlanctl for replace operation\n", 
                            pResult->param.field.fldCode);
                        resultValid = 0;
                        break;
                    }
                }
                break;
            
            case OamResultClearDel:
                //TBD
            
            case OamResultClearIns:
                //TBD
            
            default:
                cmsLog_debug("The given result type (%d) is not supported by vlanctl\n", pResult->result);
                resultValid = 0;
                break;
            }
        }

    cmsLog_debug("is result valid? %s\n",resultValid?"yes":"no");
    if (resultValid)
        {
        
        vlanActionValue.svlanVid  = sVid;
        vlanActionValue.svlanPbit = sPbits;
        vlanActionValue.svlanTpid = sTpid;
        vlanActionValue.cvlanVid  = cVid;
        vlanActionValue.cvlanPbit = cPbits;
        vlanActionValue.cvlanTpid = cTpid;

        if (deleteSvlanTag || deleteCvlanTag)
            resultValid = OamRdpaResultDeleteVlanProcess(deleteSvlanTag, deleteCvlanTag, 
                             &(rdpaRule->vlan_action));            
        if (replaceSvlanTag || replaceCvlanTag)
            resultValid = OamRdpaResultReplaceVlanProcess(replaceSvlanTag, replaceCvlanTag, 
                             &vlanActionValue, &(rdpaRule->vlan_action));            
        if (pushSvlanTag || pushCvlanTag)
            resultValid |= OamRdpaResultPushVlanProcess(pushSvlanTag, pushCvlanTag, 
                             &vlanActionValue, &(rdpaRule->vlan_action));            

        //queue
        if (!speQueue)
            {//configure the default queue
            if (pRule->port > 0)
                {//us
                rdpaRule->egress_port = 0;//wan0
                rdpaRule->wan_flow = 0;
                rdpaRule->forw_mode = rdpactl_forwarding_mode_flow;
                }
            else//ds
                {
                rdpaRule->egress_port = 0+RDPACTL_IF_LAN0;
                rdpaRule->forw_mode = rdpactl_forwarding_mode_pkt;
                }                        
            rdpaRule->queue_id = 0;
            }

//Currently, only BCM support the feature
#ifdef BCM_OAM_BUILD
#if GLOBAL_DN_SHAPER
        {
        rdpaRule->service_queue_info = RDPACTL_SERVICEACT_Q_MASK | 0;
        }
#endif
#endif

        }

    return resultValid;
    }

static U8 RdpaCustomFieldHasMoreFlow(RuleDesc *pRule, U8 *idx)
    {
    int i;
    OamRuleClauseData *pClause = NULL;
    int ret = 0;

    for (i = 0; i < pRule->clauseCount; i++) 
        {
        pClause = &pRule->clauseData[i].clause;
        switch (pClause->fieldCode)
            {
            case OamFieldCustom0 ... OamFieldCustom7:                
                {
                U8 index = pClause->fieldCode - OamFieldCustom0;

                if (customFieldRdpaCfg[index].matchCount > 1)
                    {
                    ret = 1;
                    *idx = index;
                    }
                break;
                }
            default:
                break;
            }
        }
    return ret;
    }

////////////////////////////////////////////////////////////////////////////////
/// \brief  
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//RDPA only accept == operation, here don't check op again.
//and ignore MASK, because RPDA doen't support it
static U8 OamCreateRdpaIcRule(rdpactl_classification_rule_t *rdpaRule, 
                                   RuleDesc *pRule, ruleLocalPrtyInfo *infoPtr)
    {
    U8 icValid = 1;

    //init rdpa rule
    memset(rdpaRule, 0, sizeof(rdpactl_classification_rule_t));
    rdpaRule->dscp_remark = -1;
    rdpaRule->opbit_remark = -1;
    rdpaRule->ipbit_remark =-1;
    rdpaRule->type = RDPACTL_IC_TYPE_FLOW;
    rdpaRule->prty = infoPtr->devCalcPrty;
    
    if (pRule->port > 0)
        {//uni port
        rdpaRule->field_mask = RDPACTL_IC_MASK_INGRESS_PORT;
        rdpaRule->dir = rdpactl_dir_us;
        rdpaRule->ingress_port_id = RDPACTL_IF_LAN0 + bcm_enet_map_oam_idx_to_phys_port(pRule->port - 1) ;
        }
    else
        {
        rdpaRule->dir = rdpactl_dir_ds;
        }

    icValid = OamRdpaClauseParse(rdpaRule, pRule);
    if (icValid)
        return OamRdpaResultParse(rdpaRule, pRule);

    return icValid;
    }


static int OamAddRdpa(RuleDesc* pRule, ruleLocalPrtyInfo *infoPtr)
    {
    int ret = 0;
    rdpactl_classification_rule_t rdpaRule; 

    cmsLog_debug("port: %d, priority: %d\n", pRule->port, infoPtr->devCalcPrty);
    if (!OamCreateRdpaIcRule(&rdpaRule, pRule, infoPtr))
        return 1;

    if (pRule->isAlways)
        {//for downsteam alwaysmatch, vlan_num is the best classify field
        U8 i ;
        rdpaRule.field_mask |= RDPACTL_IC_MASK_NUM_OF_VLANS;
        for (i = 0; i < MAX_VLANCTL_TAGS; i ++)
            {
            rdpaRule.number_of_vlans = i;
            ret = rdpaCtl_add_classification_rule(&rdpaRule, &infoPtr->devPrty);
            }
        }
    else        
        {
        U8 idx = 0;
        ret = rdpaCtl_add_classification_rule(&rdpaRule, &infoPtr->devPrty);

        if (pRule->isGreaterLess)
            {
            uint16_t i = 0;
            
            for (i = 0; i < pRule->range; i++)
                {
                if ((rdpaRule.field_mask & RDPACTL_IC_MASK_SRC_PORT) == RDPACTL_IC_MASK_SRC_PORT)
                    rdpaRule.src_port++;
                else
                    rdpaRule.dst_port++;

                ret = (ret == 0)? rdpaCtl_add_classification_rule(&rdpaRule, &infoPtr->devPrty) : ret;
                }
            }
        
        if (RdpaCustomFieldHasMoreFlow(pRule, &idx) != 0)
           {
           if(customFieldRdpaCfg[idx].fieldMask & RDPACTL_IC_MASK_IP_PROTOCOL)
               {
               rdpaRule.protocol = customFieldRdpaCfg[idx].matchValue[1];
               ret = rdpaCtl_add_classification_rule(&rdpaRule, &infoPtr->devPrty);
               }
           }
        }

  
    return ret;
    }


static int OamDelRdpa(RuleDesc* pRule, ruleLocalPrtyInfo *infoPtr)
    {
    int ret = 0;
    rdpactl_classification_rule_t rdpaRule;

#ifdef OAM_DUMP_RULES
    {
    U16 i;

    for(i=0; i< pRule->clauseCount; i++)
        {
        printf("Clause %d:\t\n", i);
        OamDumpClause(&pRule->clauseData[i].clause);
        }

    for(i=0; i< pRule->resultCount; i++)
        {
        printf("Result %d:\t\n", i);
        OamDumpResult(pRule->resultData[i].length, &pRule->resultData[i].result);
        }    
    }
#endif
    
    if(!OamCreateRdpaIcRule(&rdpaRule, pRule, infoPtr))
        return -1;
    
    if (pRule->isAlways)
        {//for downsteam alwaysmatch, vlan_num is the best classify field
        U8 i ;
        rdpaRule.field_mask |= RDPACTL_IC_MASK_NUM_OF_VLANS;
        for (i = 0; i < MAX_VLANCTL_TAGS; i ++)
            {
            rdpaRule.number_of_vlans = i;
            ret = rdpaCtl_del_classification_rule(&rdpaRule);
            }
        }
    else
        {
        U8 idx = 0;
        ret = rdpaCtl_del_classification_rule(&rdpaRule);

        if (pRule->isGreaterLess)
            {
            uint16_t i = 0;
            
            for (i = 0; i < pRule->range; i++)
                {
                if ((rdpaRule.field_mask & RDPACTL_IC_MASK_SRC_PORT) == RDPACTL_IC_MASK_SRC_PORT)
                    rdpaRule.src_port++;
                else
                    rdpaRule.dst_port++;

                ret = (ret == 0)? rdpaCtl_del_classification_rule(&rdpaRule) : ret;
                }
            }
        
        if (RdpaCustomFieldHasMoreFlow(pRule, &idx) != 0)
            {
            if(customFieldRdpaCfg[idx].fieldMask & RDPACTL_IC_MASK_IP_PROTOCOL)
                {
                rdpaRule.protocol = customFieldRdpaCfg[idx].matchValue[1];
                ret = rdpaCtl_del_classification_rule(&rdpaRule);
                }
            }        
        }
    return ret;
    }


static OamError OamAddFastPath(RuleDesc* pRule, ruleLocalPrtyInfo *infoPtr, BOOL isVirtualPortRule)
    {
    rdpactl_classification_rule_t rdpaRule;
    RuleIndexEntry *tmpEntry = NULL;
    RuleDesc *rulePtr = NULL;
    ruleLocalPrtyInfo *pInfo; 
    OamError ret = OamErrNoError;
    
    cmsLog_debug("port: %d, isVirtualPortRule %d priority: %d\n", pRule->port, isVirtualPortRule, pRule->priority);
    
    if(isVirtualPortRule)
        return OamErrNoError;

#ifdef OAM_DUMP_RULES
    {
    U16 i;

    for(i=0; i< pRule->clauseCount; i++)
        {
        printf("Clause %d:\t\n", i);
        OamDumpClause(&pRule->clauseData[i].clause);
        }

    for(i=0; i< pRule->resultCount; i++)
        {
        printf("Result %d:\t\n", i);
        OamDumpResult(pRule->resultData[i].length, &pRule->resultData[i].result);
        }    
    }
#endif

    if(!OamCreateRdpaIcRule(&rdpaRule, pRule, infoPtr))
        {
        cmsLog_error("apply RDPA entry index: %d is invalid\n", infoPtr->index);
        return OamErrBadParams;
        }

    //delete the rules behind current rule, and add them again
    tmpEntry = GlobalRuleListEntryNextByPortIndex(pRule->port, infoPtr->index);

    //remap all the dev priority and flush all rules
    do {
        if (tmpEntry == NULL)
            break;

        rulePtr = GlobalRuleTableByPortIndex(pRule->port, tmpEntry->curIdx);
        
        if (RuleIsValid(rulePtr))
            {//delete the rule one by one
            pInfo = GetPlatformInfo(pRule->port, tmpEntry->curIdx);
            cmsLog_debug("delete rule index: %d\n", pInfo->index);
            if (OamDelRdpa(rulePtr, pInfo) == 0)
                decreasePrtyRef(pRule->port, pInfo->devPrty);

            PrintLocalPrtyInfoByIdxPortList(pRule->port, pInfo->index);
            }
        
        tmpEntry = GlobalRuleListEntryNextByPortIndex(pRule->port, tmpEntry->curIdx);
        }while(1);

    //re-install the rules again.
    tmpEntry = GlobalRuleListEntryByPortIndex(pRule->port, infoPtr->index);
    do {
        if (tmpEntry == NULL)
            break;
        
        if (tmpEntry->curIdx == infoPtr->index)
            rulePtr = pRule;
        else
            rulePtr = GlobalRuleTableByPortIndex(pRule->port, tmpEntry->curIdx);
        
        if (RuleIsValid(rulePtr))
            {//install the rules one by one
            pInfo = GetPlatformInfo(pRule->port, tmpEntry->curIdx);
            pInfo->devCalcPrty = findFreePrtyIdx(pRule->port);
            cmsLog_debug("add rule index: %d\n", pInfo->index);
            //record current rule application result
            if (OamAddRdpa(rulePtr, pInfo) == 0)
                {
                increasePrtyRef(pRule->port, pInfo->devPrty);
                }
            else if (tmpEntry->curIdx == infoPtr->index)
                {
                ret = OamErrBadParams;
                }
            else
                {/* Roll back, delete new one, re-install all old entries.
                    We assume old entries can be installed success */
                RuleDesc *newRulePtr = NULL;
                ruleLocalPrtyInfo *newpInfo = NULL; 
                
                newRulePtr = GlobalRuleTableByPortIndex(pRule->port, infoPtr->index);
                newpInfo = GetPlatformInfo(pRule->port, infoPtr->index);
                if(OamDelRdpa(newRulePtr, newpInfo) == 0)
                    decreasePrtyRef(pRule->port, newpInfo->devPrty);
                
                if (OamAddRdpa(rulePtr, pInfo) == 0)
                    {
                    increasePrtyRef(pRule->port, pInfo->devPrty);
                    }
                ret = OamErrBadParams;
                }
            PrintLocalPrtyInfoByIdxPortList(pRule->port, pInfo->index);
            }
        
        tmpEntry = GlobalRuleListEntryNextByPortIndex(pRule->port, tmpEntry->curIdx);
        }while(1);

    return ret;
    }

static OamError OamDelFastPath(RuleDesc* pRule, ruleLocalPrtyInfo *infoPtr, BOOL isVirtualPortRule)
    {
    OamError rc = OamErrNoError;
    cmsLog_debug("port: %d, priority: %d\n", pRule->port, infoPtr->devCalcPrty);

    if(isVirtualPortRule)
        return rc;

    if (OamDelRdpa(pRule, infoPtr)== 0)
        decreasePrtyRef(pRule->port, infoPtr->devPrty);
    else
        rc = OamErrBadParams;
    
    return rc;
    }

#if OAM_CUSTOMFIELD_DBUG
static void dumpCustomField(void)
    {
    U8 i,j;

    for (i = 0; i < CustomFieldMaxNum; i++)
        {
        printf("Item %d:\n", i);
        printf("\tType %s,\tOffset %d,\tMask 0x%08X\n",(customFieldRdpaCfg[i].customFieldRdpaInfo.type==RDPA_OFFSET_L2?"L2":
            (customFieldRdpaCfg[i].customFieldRdpaInfo.type==RDPA_OFFSET_L3?"L3":"L4")),
             customFieldRdpaCfg[i].customFieldRdpaInfo.offset,
             customFieldRdpaCfg[i].customFieldRdpaInfo.mask);
        printf("\tfield_mask 0x%x,\n\tmatch count %d:\n",customFieldRdpaCfg[i].fieldMask,
            customFieldRdpaCfg[i].matchCount);
        for (j = 0; j < customFieldRdpaCfg[i].matchCount; j++)
            printf("\t\tvalue %d: 0x%x\n", j, customFieldRdpaCfg[i].matchValue[j]);
        }

    }
#endif

#endif /* end of OAM_RDPA */

void OamAddCustomField(U8 idx, OamCustomField *customField)
    {
#ifdef OAM_RDPA
        rdpactl_ic_gen_rule_cfg_t *rdpa_gen_rule_cfg = &customFieldRdpaCfg[idx].customFieldRdpaInfo;

        customFieldRdpaCfg[idx].fieldMask = 0;
        customFieldRdpaCfg[idx].matchCount = 0;
        customFieldRdpaCfg[idx].matchValue[0] = 0;
        customFieldRdpaCfg[idx].matchValue[1] = 0;        
        customFieldRdpaCfg[idx].oamCustomCfg.layerSelect = customField->layerSelect;
        customFieldRdpaCfg[idx].oamCustomCfg.dwordOffset = customField->dwordOffset;
        customFieldRdpaCfg[idx].oamCustomCfg.lsb = customField->lsb;
        customFieldRdpaCfg[idx].oamCustomCfg.bitWidth = customField->bitWidth;
        switch (customField->layerSelect)
            {
            case OamLayerEtherType:
                rdpa_gen_rule_cfg->type = RDPA_OFFSET_L2;
                rdpa_gen_rule_cfg->offset = 10 + customField->dwordOffset * 4;
                rdpa_gen_rule_cfg->mask = caculateMask(customField->lsb, customField->bitWidth);
                break;

            case OamLayerSVlan:
                rdpa_gen_rule_cfg->type = RDPA_OFFSET_L2;
                rdpa_gen_rule_cfg->offset = 12 + customField->dwordOffset * 4;
                rdpa_gen_rule_cfg->mask = caculateMask(customField->lsb, customField->bitWidth);
                customFieldRdpaCfg[idx].fieldMask = RDPACTL_IC_MASK_OUTER_TPID;
                customFieldRdpaCfg[idx].matchValue[0] = useAltSvlan.tpid;
                customFieldRdpaCfg[idx].matchCount = 1;
                break;

            case OamLayerCVlan:
                rdpa_gen_rule_cfg->type = RDPA_OFFSET_L2;
                rdpa_gen_rule_cfg->offset = 12 + customField->dwordOffset * 4;
                rdpa_gen_rule_cfg->mask = caculateMask(customField->lsb, customField->bitWidth);
                customFieldRdpaCfg[idx].fieldMask = RDPACTL_IC_MASK_OUTER_TPID;
                customFieldRdpaCfg[idx].matchValue[0] = useAltCvlan.tpid;;        
                customFieldRdpaCfg[idx].matchCount = 1;                
                break;

            case OamLayerIPv4:
                rdpa_gen_rule_cfg->type = RDPA_OFFSET_L3;
                rdpa_gen_rule_cfg->offset = customField->dwordOffset * 4;
                rdpa_gen_rule_cfg->mask = caculateMask(customField->lsb, customField->bitWidth);
                customFieldRdpaCfg[idx].fieldMask = RDPACTL_IC_MASK_L3_PROTOCOL;
                customFieldRdpaCfg[idx].matchValue[0] = RDPA_L3_PROTOCOL_IPV4;
                customFieldRdpaCfg[idx].matchCount = 1;                
                break;

            case OamLayerIPv6:
                rdpa_gen_rule_cfg->type = RDPA_OFFSET_L3;
                rdpa_gen_rule_cfg->offset = customField->dwordOffset * 4;
                rdpa_gen_rule_cfg->mask = caculateMask(customField->lsb, customField->bitWidth);
                customFieldRdpaCfg[idx].fieldMask = RDPACTL_IC_MASK_L3_PROTOCOL;
                customFieldRdpaCfg[idx].matchValue[0] = RDPA_L3_PROTOCOL_IPV6;
                customFieldRdpaCfg[idx].matchCount = 1;                
                break;

            case OamLayerL3:
                rdpa_gen_rule_cfg->type = RDPA_OFFSET_L3;
                rdpa_gen_rule_cfg->offset = customField->dwordOffset * 4;
                rdpa_gen_rule_cfg->mask = caculateMask(customField->lsb, customField->bitWidth);
                customFieldRdpaCfg[idx].fieldMask = RDPACTL_IC_MASK_L3_PROTOCOL;
                customFieldRdpaCfg[idx].matchValue[0] = RDPA_L3_PROTOCOL_OTHER;
                customFieldRdpaCfg[idx].matchCount = 1;                
                break;

            case OamLayerTcpUdp:
                rdpa_gen_rule_cfg->type = RDPA_OFFSET_L4;
                rdpa_gen_rule_cfg->offset = customField->dwordOffset * 4;
                rdpa_gen_rule_cfg->mask = caculateMask(customField->lsb, customField->bitWidth);
                customFieldRdpaCfg[idx].fieldMask = RDPACTL_IC_MASK_IP_PROTOCOL;
                /* Tcp or Udp */
                customFieldRdpaCfg[idx].matchValue[0] = 0x11;
                customFieldRdpaCfg[idx].matchValue[1] = 0x6;                
                customFieldRdpaCfg[idx].matchCount = 2;                
                break;

            default:
                cmsLog_error("The given layer select (%d) is not yet supported\n", customField->layerSelect);
                break;
            }
#if OAM_CUSTOMFIELD_DBUG        
        dumpCustomField();        
#endif
#endif        
    }

#ifdef OAM_VLAN_CTL
////////////////////////////////////////////////////////////////////////////////
/// \brief        Force set all of rules id as 0xFF
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamVlanctlFrmProcRuleClr(void)
    {
    memset(defaultPonFrwdRuleId, 0xFF, sizeof(defaultPonFrwdRuleId));
    }

////////////////////////////////////////////////////////////////////////////////
/// \brief        warm Initialize OAM frame processing module
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static void OamFrmProcPonConfig(void)
    {
    OamVlanCtlVlanAllSet(OamAltSVlanTpidGet(), OamAltCVlanTpidGet());
    OamVlanCtlModeAllSet();
    }

////////////////////////////////////////////////////////////////////////////////
/// \brief        get Max Link Number which includes BiDir and Rx-Only
///
/// \return
/// MaxLinkNum
////////////////////////////////////////////////////////////////////////////////
U16 OamMaxLinkNumGet(void)
    {
    U16 linkNum;

    if (EponDevGetUpPonRate() == LaserRate10G)
        linkNum = MAX_LINK_NUM_10G;
    else
        linkNum = MAX_LINK_NUM_1G;
            
    return linkNum;
    }

////////////////////////////////////////////////////////////////////////////////
/// \brief        get Max Rx-Only Link Number
///
/// \return
/// MaxRxOnlyLinkNum
////////////////////////////////////////////////////////////////////////////////
U16 OamMaxRxOnlyLinkNumGet(void)
    {
    U16 linkNum;

    if (EponDevGetUpPonRate() == LaserRate10G)
        linkNum = MAX_RX_ONLY_LINK_NUM_10G;
    else
        linkNum = MAX_RX_ONLY_LINK_NUM_1G;
            
    return linkNum;
    }

////////////////////////////////////////////////////////////////////////////////
/// \brief        warm Initialize OAM frame processing module
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamVlanctlFrmProcRuleInit(U8 link, BOOL add)
    {
    U8 i;

    if ((link == 0) && add)//for base link, configure vlanctl tpid information
        OamFrmProcPonConfig();
    
    //vlanctl rule that forward rule to assciated vlanctl interface
    vlanCtl_init();
    for (i = 0; i < 3; i ++)
        {
        if (add)
            {
            vlanCtl_initTagRule();
            vlanCtl_filterOnSkbMarkPort(link);
            vlanCtl_setReceiveVlanDevice(eponVlanIfname);
            vlanCtl_insertTagRule(eponRealIfname, 
                                    VLANCTL_DIRECTION_RX, i, VLANCTL_POSITION_APPEND, 0, &defaultPonFrwdRuleId[link][i]);
            }
        else
            {
            if (defaultPonFrwdRuleId[link][i] != BCM_VLAN_DONT_CARE)
                vlanCtl_removeTagRule(eponRealIfname, VLANCTL_DIRECTION_RX, i, defaultPonFrwdRuleId[link][i]);
            }
        }

    if (link == 0)
        {//base link, broadcast/mcast frame forward to epon0.1
        U16 LinkIdx, MaxLinkNum, MaxRxOnlyLinkNum;

        MaxLinkNum = OamMaxLinkNumGet();
        MaxRxOnlyLinkNum = OamMaxRxOnlyLinkNumGet();
        for (i = 0; i < 3; i ++)
            {
            if (add)
                {
                for (LinkIdx = MaxLinkNum - MaxRxOnlyLinkNum; LinkIdx < MaxLinkNum; LinkIdx++)
                    {
                    vlanCtl_initTagRule();
                    vlanCtl_filterOnSkbMarkPort(LinkIdx);
                    vlanCtl_setReceiveVlanDevice(eponVlanIfname);
                    vlanCtl_insertTagRule(eponRealIfname, 
                                            VLANCTL_DIRECTION_RX, i, VLANCTL_POSITION_APPEND, 0, &defaultPonFrwdRuleId[LinkIdx][i]);
                    }
                }
            else
                {
                for (LinkIdx = MaxLinkNum - MaxRxOnlyLinkNum; LinkIdx < MaxLinkNum; LinkIdx++)
                    {
                    if (defaultPonFrwdRuleId[LinkIdx][i] != BCM_VLAN_DONT_CARE)
                        vlanCtl_removeTagRule(eponRealIfname, VLANCTL_DIRECTION_RX, i, defaultPonFrwdRuleId[LinkIdx][i]);
                    }
                }
            }
        }
    vlanCtl_cleanup();
    }
#else
void OamVlanctlFrmProcRuleInit(U8 link, BOOL add)
    {
    return;
    }
#endif

#ifdef EPON_SFU
void OamDataPathInit(void)
    {    
    U8 i, j;//i for UNI ID loop, j for VLANTAG supported loop
    
    //vlanctl datapath init
    /*
    LAN port need use skbMarkFlowId to record the uni port
    create LLID interfaces, when enable/disable user traffic will up/down the epon0.x interfaces
    */
#ifdef OAM_VLAN_CTL
    char uniVlanIfname[IFNAMESIZ];
    U32 ruleId;
    
    vlanCtl_init();
    for (i = 0; i < UniCfgDbGetActivePortCount(); i++)
        {
        //add mark for frame from UNI
        snprintf(uniVlanIfname, IFNAMESIZ, "%s.0", uniRealIfname[i]);
        for (j = 0; j < MAX_VLANCTL_TAGS; j ++)
            {
            vlanCtl_initTagRule();
            vlanCtl_cmdSetSkbMarkFlowId(i);
            vlanCtl_setReceiveVlanDevice(uniVlanIfname);
            vlanCtl_insertTagRule(uniRealIfname[i], 
                VLANCTL_DIRECTION_RX, j, VLANCTL_POSITION_APPEND, 0, &ruleId);
            }
        }
       
    vlanCtl_createVlanInterfaceByName(eponRealIfname, eponVlanIfname, 0, 1);
    vlanCtl_setRealDevMode(eponRealIfname, BCM_VLAN_MODE_RG);
    vlanCtl_cleanup(); 

    OamVlanctlFrmProcRuleClr();

#endif  
    //disable flow cahce
    system("fc disable");
    
    //disable vlan isolation
    for (i = 0; i < UniCfgDbGetActivePortCount(); i++)
        {
        bcm_port_vlan_isolation_set(0, bcm_enet_map_oam_idx_to_phys_port(i), 0, 0);
        }
    
    rdpaCtl_set_always_tpid(OamAltSVlanTpidGet());

    qosManagedByOAM = TRUE;
    }
#else
void OamDataPathInit(void)
    {
#ifdef OAM_VLAN_CTL
    char cmdStr[64];  
    int i;

    for (i = 0; i < UniCfgDbGetOamManagedPortCount() && i < MAX_VEIP_If_NUM; i++)
        {
        vlanCtl_init();
        vlanCtl_createVlanInterfaceByName(eponRealIfname, eponVeipIfname[i], 1, 1);
        vlanCtl_cleanup();

        memset(cmdStr, 0,sizeof(cmdStr));
        snprintf(cmdStr, sizeof(cmdStr), "ifconfig %s up", eponVeipIfname[i]);
        system(cmdStr);
        }
#endif
    
    qosManagedByOAM = FALSE;
    }
#endif

#if defined(BCM_OAM_BUILD) && defined(EPON_HGU)
static BOOL ValidateVidMapLlidRule(RuleDesc* pRule, U16 *vid, U8 *link, U8 *queue)
    {
    U16 i;
    
    for (i=0; i< pRule->clauseCount; i++)
        {
        //svlan / cvlan
        if (pRule->clauseData[i].clause.fieldCode == 7 || pRule->clauseData[i].clause.fieldCode == 8)
            {
            U16 vlanId = 0;
            OamRuleClauseData *pClause = NULL;
            pClause = &pRule->clauseData[i].clause;            
            COPY_GENERAL_CLAUSE_VALUE_RIGHT_WORD(pClause, &vlanId);
            *vid = vlanId & VID_MASK;
            break;
            }
        }
    if (i >= pRule->clauseCount)
        {
        return FALSE;
        }
    for (i=0; i< pRule->resultCount; i++)
        {
        //dir = up, link [0, 7]
        if (pRule->resultData[i].result.result == OamResultQueue &&
            pRule->resultData[i].result.param.dest.type == OamObjCtxtLink &&
            pRule->resultData[i].result.param.dest.inst >= 0 &&
            pRule->resultData[i].result.param.dest.inst <= 7)
            {
            *link = pRule->resultData[i].result.param.dest.inst;
            *queue = pRule->resultData[i].result.param.dest.queue;
            break;
            }
        }
    if (i >= pRule->resultCount)
        {
        return FALSE;
        }
    return TRUE;
    }

static void VidMapLlidRuleApply(RuleDesc* pRule, ruleLocalPrtyInfo *infoPtr)
    {
    U8 tag;
    U32 tagRuleId = VLANCTL_DONT_CARE;
    U16 vid = 100;
    U8 link = 0;
    U8 queue = 8;
    
    cmsLog_debug("VidMapLlidRuleApply for rule of port: %d, priority: %d\n", pRule->port, pRule->priority);
#ifdef OAM_DUMP_RULES
        {
        U16 i;
    
        for (i=0; i< pRule->clauseCount; i++)
            {
            printf("Clause %d:\t\n", i);
            OamDumpClause(&pRule->clauseData[i].clause);
            }
    
        for (i=0; i< pRule->resultCount; i++)
            {
            printf("Result %d:\t\n", i);
            OamDumpResult(pRule->resultData[i].length, &pRule->resultData[i].result);
            }    
        }
#endif

    if(!ValidateVidMapLlidRule(pRule, &vid, &link, &queue))
        {
        cmsLog_error("%s:%d valid vid map llid rule fail!\n", __FUNCTION__, __LINE__);
        return;
        }
    vlanCtl_init();
    for (tag = 0; tag < 3; tag++)
        {
        vlanCtl_initTagRule();
        if (vid != 0x0FFF)
            {
            vlanCtl_filterOnTagVid(vid, 0);
            }
        vlanCtl_cmdSetSkbMarkPort(link);
        if (queue < 8)
            {
            vlanCtl_cmdSetSkbMarkQueue(queue);
            }
        if (vid == 0x0FFF) //dont care vid
            {
            vlanCtl_insertTagRule(eponRealIfname, VLANCTL_DIRECTION_TX, tag,
                    VLANCTL_POSITION_LAST, VLANCTL_DONT_CARE, (unsigned int *)&tagRuleId);
            }
        else
            {
            vlanCtl_insertTagRule(eponRealIfname, VLANCTL_DIRECTION_TX, tag,
                    VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, (unsigned int *)&tagRuleId);
            }
        infoPtr->vlanCtlInfo[0][tag].valid = 1;
        infoPtr->vlanCtlInfo[0][tag].nbrOfTags = tag;
        infoPtr->vlanCtlInfo[0][tag].dir = VLANCTL_DIRECTION_TX;
        infoPtr->vlanCtlInfo[0][tag].tagRuleId = tagRuleId;
        }
    vlanCtl_cleanup();
    if(!send_bcm_hgu_notification_to_cms(link, TRUE, vid, -1))
        {
        cmsLog_error("start wan_service fail\n");
        }
    }

static void VidMapLlidRuleDel(RuleDesc* pRule, ruleLocalPrtyInfo *infoPtr)
    {
    U8 tag;
    U16 vid = 100;
    U8 link = 0;
    U8 queue = 8;

#ifdef OAM_DUMP_RULES
        {
        U16 i;

        for (i=0; i< pRule->clauseCount; i++)
            {
            printf("Clause %d:\t\n", i);
            OamDumpClause(&pRule->clauseData[i].clause);
            }

        for (i=0; i< pRule->resultCount; i++)
            {
            printf("Result %d:\t\n", i);
            OamDumpResult(pRule->resultData[i].length, &pRule->resultData[i].result);
            }    
        }
#endif
    if(ValidateVidMapLlidRule(pRule, &vid, &link, &queue))
        {
        if(!send_bcm_hgu_notification_to_cms(link, FALSE, vid, -1))
            {
            cmsLog_error("stop wan_service fail\n");
            }
        }

    vlanCtl_init();
    for (tag =0; tag < MAX_VLANCTL_TAGS; tag++)
        {
        vlanCtl_removeTagRule(eponRealIfname,
            infoPtr->vlanCtlInfo[0][tag].dir,
            infoPtr->vlanCtlInfo[0][tag].nbrOfTags,
            infoPtr->vlanCtlInfo[0][tag].tagRuleId);
        }
    vlanCtl_cleanup();
    }
#endif

OamError OamAddRuleApply(RuleDesc* pRule, ruleLocalPrtyInfo *infoPtr)
    {
    OamError ret = OamErrNoError;

#if defined(BCM_OAM_BUILD) && defined(EPON_HGU)
    //vid<->LLID mapping rule
    VidMapLlidRuleApply(pRule, infoPtr);
#else  
    BOOL isQueueSet = FALSE;
    BOOL isVirtualPortRule = FALSE;
    U8 port = 0, queue = 0;
    
    if(pRule->port)
        isVirtualPortRule = isPortVirtualInterface(pRule->port);
    else
        {
        isQueueSet = OamGetDestPortAndQueue(pRule, &port, &queue);
        if(isQueueSet)
            isVirtualPortRule = isPortVirtualInterface(port);
        }
    cmsLog_debug("pRule->port %d, isQueueSet %d (port %d queue %d), isVirtualPortRule %d\n", 
        pRule->port, isQueueSet, port, queue, isVirtualPortRule);

#ifdef OAM_RDPA
    //update rdpa table
    if(OamAddFastPath(pRule, infoPtr, isVirtualPortRule) != OamErrNoError)
        ret = OamErrBadParams;
#endif
#ifdef OAM_VLAN_CTL
    //update vlanctl table
    if (ret == OamErrNoError)
        ret = OamAddSlowPath(pRule, infoPtr, isVirtualPortRule);
#endif
#endif
    return ret;
    }

OamError OamDelRuleApply(RuleDesc* pRule, ruleLocalPrtyInfo *infoPtr)
    {
    OamError ret = OamErrNoError;

#if defined(BCM_OAM_BUILD) && defined(EPON_HGU)
    //vid<->LLID mapping rule
    VidMapLlidRuleDel(pRule, infoPtr);
#else  
    BOOL isQueueSet = FALSE;
    BOOL isVirtualPortRule = FALSE;
    U8 port = 0, queue = 0;
    
    if(pRule->port)
        isVirtualPortRule = isPortVirtualInterface(pRule->port);
    else
        {
        isQueueSet = OamGetDestPortAndQueue(pRule, &port, &queue);
        if(isQueueSet)
            isVirtualPortRule = isPortVirtualInterface(port);
        }
    cmsLog_debug("pRule->port %d, isQueueSet %d (port %d queue %d), isVirtualPortRule %d\n", 
        pRule->port, isQueueSet, port, queue, isVirtualPortRule);

#ifdef OAM_VLAN_CTL
    //update vlanctl table
    ret = OamDelSlowPath(pRule, infoPtr, isVirtualPortRule);
#endif

#ifdef OAM_RDPA
    //update rdpa table
    if (ret == OamErrNoError)
        ret = OamDelFastPath(pRule, infoPtr, isVirtualPortRule);
#endif
#endif
    return ret;
    }
// end DataPathRules.c

