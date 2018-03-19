/*
 * <:copyright-BRCM:2013:proprietary:epon 
 * 
 *    Copyright (c) 2013 Broadcom 
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
:>
*/

#if !defined(CtcClassification_h)
#define CtcClassification_h
////////////////////////////////////////////////////////////////////////////////
/// \file CtcClassification.h
/// \brief China Telecom classification module
///
////////////////////////////////////////////////////////////////////////////////
#include "Teknovus.h"

#if defined(__cplusplus)
extern "C" {
#endif

//##############################################################################
//               Ctc Classification Structure and Const data
//##############################################################################
#define CtcNumRulePrec			        8

typedef enum
    {
    CtcRuleFieldDaMac            = 0,
    CtcRuleFieldSaMac            = 1,
    CtcRuleFieldVlanPri          = 2,
    CtcRuleFieldVlanId           = 3,
    CtcRuleFieldEthertype        = 4,
    CtcRuleFieldDestIp           = 5,
    CtcRuleFieldSrcIp            = 6,
    CtcRuleFieldIpProtocol       = 7,
    CtcRuleFieldIpTos            = 8,
    CtcRuleFieldIpv6Class        = 9,
    CtcRuleFieldL4SrcPort        = 10,
    CtcRuleFieldL4DestPort       = 11,
    CtcRuleFieldIPVersion        = 12,
    CtcRuleFieldIPv6FlowLabel    = 13,
    CtcRuleFieldIPv6Da           = 14,
    CtcRuleFieldIPv6Sa           = 15,
    CtcRuleFieldIPv6DaPrefix     = 16,
    CtcRuleFieldIPv6SaPrefix     = 17,
    CtcRuleFieldIpv6NextHeader   = 18,
    
    CtcRuleFieldNum
    } PACK CtcRuleField;

typedef enum
    {
    CtcRuleOpFalse,
    CtcRuleOpEqual,
    CtcRuleOpNotEq,
    CtcRuleOpLtEq,
    CtcRuleOpGtEq,
    CtcRuleOpExist,
    CtcRuleOpNotEx,
    CtcRuleOpTrue,
    CtcNumRuleOp
    } PACK CtcRuleOp;

typedef enum
    {
	CtcRuleRetCodeOk,
	CtcRuleRetCodeBadParameters,
	CtcRuleRetCodeNoResource,

	CtcRuleRetCodeNum
	} CtcRuleRetCode;


typedef union
    {
    U8      byte[6];
    U16     word[3];
    } CtcMatchValue6;

// This structure is used when OLT and ONU negotiate to Ipv4
typedef struct
    {
    CtcRuleField         field;
    U8                   value[6];
    CtcRuleOp            op;
    } PACK CtcRuleClause6;

// This structure is used when OLT and ONU negotiate to Ipv6
typedef struct
    {
    CtcRuleField         field;
    U8                   value[16];
    CtcRuleOp            op;
    } PACK CtcRuleClause16;


typedef union
    {
    CtcRuleField         field;
    CtcRuleClause6       clause6;
    CtcRuleClause16      clause16;
    } CtcRuleClause;

typedef struct
    {
    U8                      prec;
    U8                      length;
    U8                      queueMapped;
    U8                      pri;
    U8                      numClause;
	U8                      clause[1];
    } CtcRule;

typedef enum
    {
    CtcRuleActionDel            = 0x00,
    CtcRuleActionAdd            = 0x01,
    CtcRuleActionClear          = 0x02,
    CtcRuleActionList           = 0x03
    } CtcRuleAction;

typedef struct
    {
    U8       action;
    U8                  numRules;
    CtcRule             rule[1];
    } CtcTlvClassMarking;

typedef struct
    {
    U8       action;
    U8                  numPrec;
    U8                  precToDelete[1];
    } CtcTlvClassMarkingDelete;


//##############################################################################
//          Ctc Classification management database and map structue
//##############################################################################
typedef enum
	{
	CtcRuleForward,
	CtcRuleVlanTransCos,
	CtcRuleVlanAddCos,
	CtcRuleUseNum
	} CtcRuleUsePerPri;


typedef enum
    {
    CtcIpV4Version = 4,
    CtcIpV6Version = 6
    } CtcRuleVersion;


//##############################################################################
//          Ctc Classification module const data and macro
//##############################################################################
#define InvalidRuleQueue                TkOnuNumUpQueues

// The remark(the priority field of Ctc rule) related const
#define CtcRuleMaxPri					7
#define CtcRuleNoPriMarking				0xFF

// Ctc classification rule base
#define CtcRulePrecBase                 4

// Ctc Rule Record Buffer Size
#define CtcRuleMaxSize                  128

////////////////////////////////////////////////////////////////////////////////
/// CtcQToTekQ - get Ctc Rule queue to Tk Rule queue mapping.
///
/// This function get from Ctc queue to TK queue mapping
///
 // Parameters:
/// \param port Port number.
/// \param llid Get link number.
/// \param lq Get Queue number.
///
/// \return
/// TkOnuQueue Id
////////////////////////////////////////////////////////////////////////////////
extern
TkOnuQueue CtcQToTekQ (U8 q);


//##############################################################################
//                  Ctc Classification interface APIs
//##############################################################################
////////////////////////////////////////////////////////////////////////////////
/// CtcClassificationInit - Initialize CTC Classification module
/// 
/// This function initialize the CTC Classification module on
/// all of the user ports.	
/// 
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcClassificationInit (void);


////////////////////////////////////////////////////////////////////////////////
/// CtcClassificationAdd - Add the CTC classification rules
/// 
/// This function processes a CTC classification add action.  Processing
/// includes validation of the rules and install them into the hardware.
/// 
/// \param port    Ethernet port number
/// \param ctcRule Pointer to the CTC Rule
/// 
/// \return
/// return code
////////////////////////////////////////////////////////////////////////////////
extern
CtcRuleRetCode CtcClassificationAdd (TkOnuEthPort port, 
									  OamCtcTlvClassMarking *ctcRule);


////////////////////////////////////////////////////////////////////////////////
/// CtcClassificationDel - Delete CTC Classification rules
/// 
/// This function processes a CTC classification delete action.  A delete action
/// consists of a list of precedences to delete.  The format is first validated
/// and then the rules at each precedence level listed are deleted.
/// 
/// \param port     Ethernet port number
/// \param ctcRule  Pointer to the CTC Rule
/// 
/// \return
/// return code
///////////////////////////////////////////////////////////////////////////////
extern
CtcRuleRetCode CtcClassificationDel (TkOnuEthPort port, 
                                    CtcTlvClassMarkingDelete BULK* ctcRule);


////////////////////////////////////////////////////////////////////////////////
/// CtcClassificationClr - Delete all rules for port
/// 
/// This function deletes all rules at all precedence levels for a given port.
/// 
/// \param port Ethernet port number
/// 
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcClassificationClr (TkOnuEthPort port);


////////////////////////////////////////////////////////////////////////////////
/// CtcClassificationGet - Process CTC classification get request
/// 
/// This function fills a CTC get request response with the current CTC rule
/// set.  It returns the number of bytes that have been added to the TLV.  If no
/// rules have been allocated the response will be empty save the action and the
/// rule count.
/// 
/// \param port     Ethernet port number
/// \param ctcRule  Pointer to the CTC Rule
/// 
/// \return
/// Size of TLV in bytes
////////////////////////////////////////////////////////////////////////////////
extern
OamVarContainer * CtcClassificationGet (TkOnuEthPort port, 
                                            OamVarContainer *cont);


////////////////////////////////////////////////////////////////////////////////
/// CtcClassificationLinkRuleClr - Delete all rules of the link
/// 
/// This function deletes all rules of the link.
/// 
/// \param link  link number
/// 
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcClassificationLinkRuleClr (LinkIndex link);


////////////////////////////////////////////////////////////////////////////////
/// CtcClassificationAdjust - Adjust all rules installed
/// 
/// This function recalculates the rule queue for all rules, and deletes the 
/// old rules, then install new rules.
/// 
/// \param None
/// 
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcClassificationAdjust (void);


////////////////////////////////////////////////////////////////////////////////
/// CtcClassificationClrAllRules - clera all rules except the default rule
/// 
/// This function clears all the rules configured on the ONU.
/// 
/// \param NONE
/// 
/// \return
/// NONE
////////////////////////////////////////////////////////////////////////////////
extern
void CtcClassificationClrAllRules (void);




#if defined(__cplusplus)
}
#endif

#endif

// End of File CtcClassification.h

