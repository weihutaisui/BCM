/***********************************************************************
 *
 *  Copyright (c) 2006-2008 Broadcom
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:omcid

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
#ifndef __RUT_GPON_FLOW_H__
#define __RUT_GPON_FLOW_H__

#include "bpaapi.h"
#include "rut_gpon_vlan.h"

#define OMCI_QUOTE_ME(_x) #_x

#define OMCI_ETH_ACTION_NAME()                  \
    { OMCI_QUOTE_ME(e_ETH_ACTION_DA),            \
      OMCI_QUOTE_ME(e_ETH_ACTION_SA),             \
      OMCI_QUOTE_ME(e_ETH_ACTION_ETHTYPE),     \
      OMCI_QUOTE_ME(e_ETH_ACTION_PUSH_TAG),       \
      OMCI_QUOTE_ME(e_ETH_ACTION_POP_TAG),     \
      OMCI_QUOTE_ME(e_ETH_ACTION_MAX) }

#define OMCI_VLAN_ACTION_NAME()                 \
    { OMCI_QUOTE_ME(e_VLAN_ACTION_SET_TAG),      \
      OMCI_QUOTE_ME(e_VLAN_ACTION_SET_TPID), \
      OMCI_QUOTE_ME(e_VLAN_ACTION_SET_TCI), \
      OMCI_QUOTE_ME(e_VLAN_ACTION_SET_PBITS), \
      OMCI_QUOTE_ME(e_VLAN_ACTION_SET_DEI), \
      OMCI_QUOTE_ME(e_VLAN_ACTION_SET_VID), \
      OMCI_QUOTE_ME(e_VLAN_ACTION_COPY_TAG_FROM_TAG), \
      OMCI_QUOTE_ME(e_VLAN_ACTION_COPY_TPID_FROM_TAG), \
      OMCI_QUOTE_ME(e_VLAN_ACTION_COPY_TCI_FROM_TAG), \
      OMCI_QUOTE_ME(e_VLAN_ACTION_COPY_PBITS_FROM_TAG), \
      OMCI_QUOTE_ME(e_VLAN_ACTION_COPY_VID_FROM_TAG), \
      OMCI_QUOTE_ME(e_VLAN_ACTION_COPY_DEI_FROM_TAG), \
      OMCI_QUOTE_ME(e_VLAN_ACTION_XLATE_PBITS_FROM_DSCP), \
      OMCI_QUOTE_ME(e_VLAN_ACTION_XLATE_PBITS_FROM_ETHTYPE), \
      OMCI_QUOTE_ME(e_VLAN_ACTION_MAX) }

#define OMCI_DONT_CARE (~0)

#define OMCI_IS_DONT_CARE(_x) ( ((_x) == (typeof(_x))(OMCI_DONT_CARE)) )

#define OMCI_TCI_FILTER_IS_DONT_CARE(_pTciFilter)            \
    ( OMCI_IS_DONT_CARE((_pTciFilter)->pbits) &&             \
      OMCI_IS_DONT_CARE((_pTciFilter)->cfi) &&               \
      OMCI_IS_DONT_CARE((_pTciFilter)->vlanId) )

#define OMCI_ADD_ETH_ACTION(_flow, _type)                               \
    do {                                                                \
        if((_flow)->action.ethActionIx >= e_ETH_ACTION_MAX)             \
        {                                                               \
            cmsLog_error("ETH action overflow");                        \
        }                                                               \
        else                                                            \
        {                                                               \
            (_flow)->action.ethAction[(_flow)->action.ethActionIx] = (_type); \
            (_flow)->action.ethActionIx++;                              \
        }                                                               \
    } while(0)

#define OMCI_ADD_VLAN_ACTION(_flow, _type, _val, _toTag, _fromTag)      \
    do {                                                                \
        if((_flow)->action.vlanActionIx >= e_VLAN_ACTION_MAX)           \
        {                                                               \
            cmsLog_error("VLAN action overflow");                       \
        }                                                               \
        else                                                            \
        {                                                               \
            (_flow)->action.vlanAction[(_flow)->action.vlanActionIx].type = (_type); \
            (_flow)->action.vlanAction[(_flow)->action.vlanActionIx].val = (UINT16)(_val); \
            (_flow)->action.vlanAction[(_flow)->action.vlanActionIx].toTag = (_toTag); \
            (_flow)->action.vlanAction[(_flow)->action.vlanActionIx].fromTag = (_fromTag); \
            (_flow)->action.vlanActionIx++;                             \
        }                                                               \
    } while(0)

typedef struct {
    UINT16 tpid;
    UINT8  pbits;
    UINT8  dei;
    UINT16 vid;
} OmciFlowFilterVlanVal_t;

typedef struct {
    UINT8 nbrOfTags;
    OmciFlowFilterVlanVal_t outer;
    OmciFlowFilterVlanVal_t inner;
} OmciFlowFilterVlan_t;

typedef struct {
    BpaCmpOper_t cmpOp;
    UINT16 val;
} OmciFlowFilterEthType_t;

typedef struct {
    BpaCmpOper_t         cmpOp;
    BpaIpv4FilterType_t  type;
    UINT16 val;
} OmciFlowFilterIPv4Type_t;

typedef struct {
    OmciFlowFilterEthType_t ethType;
    OmciFlowFilterVlan_t vlan;
    OmciFlowFilterIPv4Type_t ipv4;
} OmciFlowFilter_t;

typedef struct {
    UINT8 fromTag;
    UINT8 toTag;
    UINT16 val;
    BpaVlanActionType_t type;
} OmciVlanAction_t;

typedef struct {
    /* ethernet actions */
//    UINT16 ethType;
    UINT8 ethActionIx;
    BpaEthActionType_t ethAction[e_ETH_ACTION_MAX];
    /* vlan actions */
    UINT8 vlanActionIx;
    OmciVlanAction_t vlanAction[e_VLAN_ACTION_MAX];
} OmciFlowAction_t;

typedef struct {
    OmciFlowFilter_t filter;
    OmciFlowAction_t action;
} OmciPathFlow_t;

CmsRet rutGpon_extractPathFlows(UINT16 vlanTagOperInputTpid,
                                UINT16 vlanTagOperOutputTpid,
                                UINT8  vlanTagOperDsMode,
                                OmciExtVlanTagOper_t *pExtVlanTagOper,

                                UINT8 vlanTciFilterFwdOper,
                                OmciVlanTciFilter_t *pVlanTciFilter,

                                UINT8 mapperPbits,

                                OmciPathFlow_t *pPathFlowUs,
                                OmciPathFlow_t *pPathFlowDs);

#if defined(CONFIG_BCM_GPON_802_1Q_ENABLED)
CmsRet rutGpon_extractDsPathFlows(UINT16 vlanTagOperInputTpid,
                                  UINT16 vlanTagOperOutputTpid,
                                  UINT8  vlanTagOperDsMode,
                                  OmciExtVlanTagOper_t *pExtVlanTagOper,

                                  UINT8 vlanTciFilterFwdOper,
                                  OmciVlanTciFilter_t *pVlanTciFilter,

                                  OmciPathFlow_t *pPathFlowUs,
                                  OmciPathFlow_t *pPathFlowDs);
#endif /* CONFIG_BCM_GPON_802_1Q_ENABLED */
#endif /* __RUT_GPON_FLOW_H__ */
