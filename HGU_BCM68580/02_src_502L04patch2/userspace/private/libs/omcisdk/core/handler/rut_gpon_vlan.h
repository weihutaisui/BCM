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
#ifndef __RUT_GPON_VLAN_H__
#define __RUT_GPON_VLAN_H__


/*!\file rut_gpon_vlan.h
 * \brief System level interface functions for generic GPON functionality.
 *
 * The functions in this file should only be called by
 * RCL, STL, and other RUT functions.
 */


#include "omci_object.h"

// VLAN filter and treatmen definitions
// are defined to match with
// ITU-T G.988 in section
// 9.3.13 Extended VLAN tagging operation
// configuration data

#define OMCI_TAGGING_RULE_SIZE        16

// start position and length below is
// defined in term of byte
#define OMCI_OUTER_FILTER_START       0
#define OMCI_INNER_FILTER_START       4
#define OMCI_FILTER_LEN               4
#define OMCI_OUTER_TREATMENT_START    8
#define OMCI_INNER_TREATMENT_START    12
#define OMCI_TREATMENT_LEN            4

// VLAN filter fields definitions
// start position and length below is
// defined in term of bit from right to left
#define OMCI_FILTER_PRIO_START        31
#define OMCI_FILTER_PRIO_LEN          4
#define OMCI_FILTER_VLANID_START      27
#define OMCI_FILTER_VLANID_LEN        13
#define OMCI_FILTER_TPID_DE_START     14
#define OMCI_FILTER_TPID_DE_LEN       3
#define OMCI_FILTER_ETHER_TYPE_START  3
#define OMCI_FILTER_ETHER_TYPE_LEN    4

// Filter priority definition
#define OMCI_FILTER_PRIO_MIN          0
#define OMCI_FILTER_PRIO_MAX          7
#define OMCI_FILTER_PRIO_NONE         8
#define OMCI_FILTER_PRIO_DEFAULT      14
#define OMCI_FILTER_IGNORE            15

// Filter VLANID definition
#define OMCI_FILTER_VLANID_MIN        0
#define OMCI_FILTER_VLANID_MAX        4094
#define OMCI_FILTER_VLANID_NONE       4096

#define OMCI_FILTER_DONT_CARE         (-1)

// Filter type definition
typedef enum
{
    OMCI_FILTER_TYPE_UNTAG = 0,
    OMCI_FILTER_TYPE_SINGLE_TAG,
    OMCI_FILTER_TYPE_DOUBLE_TAG
} OmciFilterType;

// Filter TPID/DE definition
typedef enum
{
    OMCI_FILTER_TPID_DE_NONE = 0,
    OMCI_FILTER_TPID_8100_DE_X = 4,
    OMCI_FILTER_TPID_INPUT_DE_X,
    OMCI_FILTER_TPID_INPUT_DE_0,
    OMCI_FILTER_TPID_INPUT_DE_1
} OmciFilterTpidDeType;

// Filter Ethernet definition
#define OMCI_FILTER_ETHER_0800        0x0800
#define OMCI_FILTER_ETHER_8863        0x8863
#define OMCI_FILTER_ETHER_8864        0x8864
#define OMCI_FILTER_ETHER_0806        0x0806

typedef enum
{
    OMCI_FILTER_ETHER_NONE = 0,
    OMCI_FILTER_ETHER_IPOE,
    OMCI_FILTER_ETHER_PPPOE, /* add etherType = 0x8863 */
    OMCI_FILTER_ETHER_ARP,
    OMCI_FILTER_ETHER_PPPOE_2 /* ETHER_PPPOE needs to add 0x8863 and 0x8864,
                                 create PPPOE_2 to add etherType = 0x8864 */
} OmciFilterEtherType;

// VLAN tag filter information
typedef struct OmciVlanTagFilter
{
    UINT8                 prio;
    UINT16                vlanId;
    OmciFilterTpidDeType  tpid_de;
} OmciVlanTagFilter_t;

// VLAN treatment fields definitions
#define OMCI_TREATMENT_REMOVE_START      31
#define OMCI_TREATMENT_REMOVE_LEN        2
#define OMCI_TREATMENT_PRIO_START        19
#define OMCI_TREATMENT_PRIO_LEN          4
#define OMCI_TREATMENT_VLANID_START      15
#define OMCI_TREATMENT_VLANID_LEN        13
#define OMCI_TREATMENT_TPID_DE_START     2
#define OMCI_TREATMENT_TPID_DE_LEN       3

// Treatment priority definition
#define OMCI_TREATMENT_PRIO_MIN                 0
#define OMCI_TREATMENT_PRIO_MAX                 7
#define OMCI_TREATMENT_PRIO_COPY_FROM_INNER     8
#define OMCI_TREATMENT_PRIO_COPY_FROM_OUTER     9
#define OMCI_TREATMENT_PRIO_DERIVE_FROM_DSCP    10
#define OMCI_TREATMENT_PRIO_DONT_ADD            15

// Treatment VLANID definition
#define OMCI_TREATMENT_VLANID_MIN               0
#define OMCI_TREATMENT_VLANID_MAX               4094
#define OMCI_TREATMENT_VLANID_COPY_FROM_INNER   4096
#define OMCI_TREATMENT_VLANID_COPY_FROM_OUTER   4097

// Treatment TPID/DE definition
typedef enum
{
    OMCI_TREATMENT_TPID_DE_COPY_FROM_INNER = 0,
    OMCI_TREATMENT_TPID_DE_COPY_FROM_OUTER,
    OMCI_TREATMENT_TPID_OUTPUT_DE_COPY_FROM_INNER,
    OMCI_TREATMENT_TPID_OUTPUT_DE_COPY_FROM_OUTER,
    OMCI_TREATMENT_TPID_8100_DE_X,
    OMCI_TREATMENT_TPID_DE_RESERVED,
    OMCI_TREATMENT_TPID_OUTPUT_DE_0,
    OMCI_TREATMENT_TPID_OUTPUT_DE_1
} OmciTreatmentTpidDeType;

// VLAN tag treatment information
typedef struct OmciVlanTagTreatment
{
    UINT8                   prio;
    UINT16                  vlanId;
    OmciTreatmentTpidDeType tpid_de;
} OmciVlanTagTreatment_t;

// VLAN tag treatment removal type
typedef enum
{
    OMCI_TREATMENT_REMOVE_NONE = 0,
    OMCI_TREATMENT_REMOVE_OUTER,
    OMCI_TREATMENT_REMOVE_BOTH,
    OMCI_TREATMENT_DISCARD_FRAME
} OmciTreatmentRemoveType;

typedef struct OmciVlanTagRule
{
    OmciVlanTagFilter_t    filter;
    OmciVlanTagTreatment_t treatment;
} OmciVlanTagRule_t;

typedef struct OmciExtVlanTagOper
{
    OmciVlanTagRule_t       outer;
    OmciVlanTagRule_t       inner;
    OmciFilterType          filterType;
    OmciFilterEtherType     etherType;
    OmciTreatmentRemoveType removeType;
} OmciExtVlanTagOper_t;

typedef enum
{
    OMCI_XVLAN_DOWNSTREAM_INVERSE = 0,
    OMCI_XVLAN_DOWNSTREAM_NONE
} OmciExtVlanDownstreamMode;

typedef struct OmciExtVlanTagOperInfo
{
    UINT16                    inputTpid;
    UINT16                    outputTpid;
    UINT32                    numberOfEntries;
    OmciExtVlanDownstreamMode downstreamMode;
    OmciExtVlanTagOper_t      *pTagOperTbl;
} OmciExtVlanTagOperInfo_t;

// Max number of VLAN filter entries in VLAN filter list
#define OMCI_TCI_NUM_MAX               12

//VLAN TCI length is defined in term of byte
#define OMCI_TCI_LEN                   2

// VLAN TCI filter fields definitions
// start position and length below is
// defined in term of bit from right to left
#define OMCI_TCI_PBITS_START           15
#define OMCI_TCI_PBITS_LEN             3
#define OMCI_TCI_CFI_START             12
#define OMCI_TCI_CFI_LEN               1
#define OMCI_TCI_VLANID_START          11
#define OMCI_TCI_VLANID_LEN            12

// VLAN TCI filter information
typedef struct OmciVlanTciFilter
{
    UINT8                 pbits;
    UINT8                 cfi;
    UINT16                vlanId;
} OmciVlanTciFilter_t;

// VLAN TCI filter table information
typedef struct OmciVlanTciFilterInfo
{
    UINT8                 numberOfEntries;
    UINT8                 forwardOperation;
    OmciVlanTciFilter_t *pVlanTciTbl;
} OmciVlanTciFilterInfo_t;

typedef enum
{
    OMCI_VLAN_UPSTREAM_AS_IS = 0,
    OMCI_VLAN_UPSTREAM_TAG,
    OMCI_VLAN_UPSTREAM_PREPEND
} OmciVlanUpstreamMode;

typedef enum
{
    OMCI_VLAN_DOWNSTREAM_AS_IS = 0,
    OMCI_VLAN_DOWNSTREAM_STRIP
} OmciVlanDownstreamMode;

typedef struct OmciVlanTagOperInfo
{
    UBOOL8                 tagIsExisted;
    OmciVlanUpstreamMode   upstreamMode;
    OmciVlanTciFilter_t    upstreamTci;
    OmciVlanDownstreamMode downstreamMode;
} OmciVlanTagOperInfo_t;


// Maximum number of DSCP
#define OMCI_DSCP_NUM_MAX 64
// Maximum value of Pbit
#define OMCI_PRIO_VAL_MAX 8
// Size of DSCP to Pbit Mapping
#define OMCI_DSCP_PBIT_MAPPING_SIZE 24

typedef enum
{
    OMCI_UNMARKED_FRAME_DSCP_TO_PBIT = 0,
    OMCI_UNMARKED_FRAME_DEFAULT_PBIT,
    OMCI_UNMARKED_FRAME_NONE
} OmciUnmarkedFrameOption;

typedef struct OmciDscpPbitMappingInfo
{
    UINT8                   dscpToPbit[OMCI_DSCP_NUM_MAX];
    UINT32                  defaultPBitMarking;
    OmciUnmarkedFrameOption unmarkedFrameOption;
} OmciDscpPbitMappingInfo_t;

//=======================  Public GPON CMF functions ========================

/** allocate memory for array of extended VLAN tagging rules
 *
 * @param meOid        (IN) CMS Object ID of object that is linked to
 *                     ExtendedVlanTaggingOperationConfigurationDataObject.
 * @param meId         (IN) Management entity ID of object that is linked to
 *                     ExtendedVlanTaggingOperationConfigurationDataObject.
 * @param pTagOperInfo (OUT) pointer to data that is converted from
 *                     ExtendedVlanTaggingOperationConfigurationDataObject.
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_allocExtTagOperInfo
    (const UINT32 meOid,
     const UINT32 meId,
     OmciExtVlanTagOperInfo_t *pTagOperInfo);

/** allocate memory for array of VLAN TCI filter
 *
 * @param meId         (IN) Management entity ID of
 *                     VlanTaggingFilterDataObject.
 * @param bridgeMeId   (INT) ME ID of MacBridgeServiceProfileObject
 * @param pVlanTciFilterInfo (OUT) pointer to VLAN Tag Filter Data
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_allocTagFilterInfo
    (const UINT32 meId,
     const UINT32 bridgeMeId,
     OmciVlanTciFilterInfo_t *pVlanTciFilterInfo);

/** get VlanTaggingOperationConfigurationData information
 *
 * @param meOid            (IN) CMS object ID of object that is linked to
 *                         VlanTaggingOperationConfigurationDataObject.
 * @param meId             (IN) Management entity ID of object that is linked to
 *                         VlanTaggingOperationConfigurationDataObject.
 * @param pVlanTagOperInfo (OUT) pointer to VLAN Tag Operation Data
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_getTagOperInfo
    (const UINT32 meOid,
     const UINT32 meId,
     OmciVlanTagOperInfo_t *pVlanTagOperInfo);

/** convert vlanFilterList in VlanTaggingFilterDataObject
 *  to array of VLAN TCIs
 *
 * @param vlan                 (IN) pointer to
 *                                  VlanTaggingFilterDataObject.
 * @param OmciVlanTciFilter_t (OUT) pointer to array VLAN TCI
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_getTagFilters
    (const VlanTaggingFilterDataObject *vlan,
     OmciVlanTciFilter_t *pVlanTciFilter);

/** extract info from MapperServiceProfileObject, convert
 * dscpToPBitMapping to array of 64 Pbits (unsigned bytes),
 * and store them to dscpToPbitInfo
 *
 * @param mapper         (IN) pointer to MapperServiceProfileObject
 * @param dscpToPbitInfo (OUT) pointer to OmciDscpPbitMappingInfo_t
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_getDscpToPbitFromMapperServiceProfile
    (const MapperServiceProfileObject *mapper,
     OmciDscpPbitMappingInfo_t *dscpToPbitInfo);


/** extract info from ExtendedVlanTaggingOperationConfigurationDataObject,
 * convert dscpToPBitMapping to array of 64 Pbits (unsigned bytes),
 * and store them to dscpToPbitInfo
 *
 * @param xVlan          (IN) pointer to ExtendedVlanTaggingOperationConfigurationDataObject
 * @param dscpToPbitInfo (OUT) pointer to OmciDscpPbitMappingInfo_t
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_getDscpToPbitFromExtendedVlanTaggingOperationConfigurationData
    (const ExtendedVlanTaggingOperationConfigurationDataObject *xVlan,
     OmciDscpPbitMappingInfo_t *dscpToPbitInfo);


/** Check extended VLAN tagging default rule
 *
 * @param pExtVlanTag (IN) pointer to  rule of ExtendedVlanTaggingOperationTable
 *
 * @return 1: defaut rule, 0: not default rule.
 */

UINT8 rutGpon_isDefaultRuleAction(OmciExtVlanTagOper_t *pExtVlanTag);

/** convert Vlan info from OmciVlanTagOperInfo_t
 *  to ExtVlan info OmciExtVlanTagOper_t
 *
 * @param pTagOperInfo (IN) pointer to OmciVlanTagOperInfo_t
 * @param pExtVlanTagOper (OUT) pointer to OmciExtVlanTagOper_t
 *
 * @return none.
 */

void rutGpon_convertVlanToXVlan
    (const OmciVlanTagOperInfo_t *pTagOperInfo,
     OmciExtVlanTagOper_t *pExtVlanTagOper);

//=======================  Private GPON CMF functions ========================

UINT32 getFieldFromWord
    (UINT32 word, UINT8 startBit, UINT8 len);

void getTagRuleInfo
    (const UINT8 *tagRule, OmciExtVlanTagOper_t *tagOper);

CmsRet getNumberOfTagRules
    (const UINT32 meId,
     OmciExtVlanTagOperInfo_t *pTagOperInfo);

CmsRet getTagRules
    (const UINT32 meId,
     OmciExtVlanTagOper_t *tagRules,
     const UINT32 max);

UINT32 getFieldFromHalfWord
    (UINT16 halfWord, UINT8 startBit, UINT8 len);

CmsRet getNumberOfTagFilters
    (const UINT32 meId,
     const UINT32 bridgeMeId,
     OmciVlanTciFilterInfo_t *pVlanTciFilterInfo);

CmsRet getTagFilters
    (const UINT32 meId,
     const UINT32 bridgeMeId,
     OmciVlanTciFilterInfo_t *pVlanTciFilterInfo);

UINT8 mapDscpToPbit
    (const UINT8 *inDscp, const UINT32 pos);

void mapDscpListToPbitList
    (const UINT8 *inDscp, UINT8 *outPbit);

int rutGpon_vlanCtl_createVlanInterface(char *realDevName,
  unsigned int vlanDevId, int isRouted, int isMulticast);
int rutGpon_vlanCtl_createVlanInterfaceByName(char *realDevName,
  char *vlanDevName, int isRouted, int isMulticast);
int rutGpon_vlanCtl_deleteVlanInterface(char *vlanDevName);

int rutGpon_vlanIf_addIf(char *ifname);
int rutGpon_vlanIf_search(const char * ifname);
int rutGpon_vlanIf_delete(char *ifname);

#endif /* __RUT_GPON_VLAN_H__ */
