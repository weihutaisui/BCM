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
#ifndef __RUT_GPON_RULE_H__
#define __RUT_GPON_RULE_H__


/*!\file rut_gpon_rule.h
 * \brief System level interface functions for generic GPON functionality.
 *
 * The functions in this file should only be called by
 * RCL, STL, and other RUT functions.
 */


#include "os_defs.h"


#include "rut_gpon.h"
#include "rut_gpon_model.h"
#include "omci_res_dm.h"


typedef enum
{
    OMCI_TAG_DO_NOTHING = 0,
    OMCI_SINGLE_TAG_ADD,
    OMCI_SINGLE_TAG_REMOVE,
    OMCI_SINGLE_TAG_REPLACE,
    OMCI_DOUBLE_TAG_ADD,
    OMCI_DOUBLE_TAG_REMOVE,
    OMCI_DOUBLE_TAG_REPLACE
} OmciTagAction;

#define OMCI_GEM_SERVICES_MAX    16

typedef struct OmciGemServicesInfo
{
    UINT32              numberOfEntries;
    OmciServiceMsgBody services[OMCI_GEM_SERVICES_MAX];
} OmciGemServicesInfo_t;


//=======================  Public GPON functions ========================

/** generate physical interface name by allocating
 *  memory and assigning name based on the given
 *  physical type and physical ID. Caller should free
 *  its memory when it's not used anymore.
 * 
 * @param phyType         (IN) physical type.
 * @param phyId           (IN) physical ID.
 * @param interfaceName   (OUT) interface name.
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_getInterfaceName
    (const OmciPhyType phyType,
     const UINT32      phyId,
     char              **interfaceName);

/** generate VLAN virtual interface name by allocating
 *  memory and assigning name based on the given
 *  physical type, physical ID, and device virtual ID.
 *  Caller should free its memory when it's not used anymore.
 * 
 * @param phyType         (IN) physical type.
 * @param phyId           (IN) physical ID.
 * @param devId           (IN) device virtual ID.
 * @param virtualName     (OUT) virtual interface name.
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_getVirtualInterfaceName
    (const OmciPhyType phyType,
     const UINT32      phyId,
     UINT32            devId,
     char              **virtualName);

CmsRet rutGpon_createRules
    (const GemPortNetworkCtpObject *ctp,
     const UBOOL8                  isNewUni,
     const UINT32                  checkOid,
     const UINT32                  checkMeId,
     const UINT32                  reachableOid,
     const UINT32                  reachableMeId,
     const OmciMapFilterModelType  type);

CmsRet rutGpon_editRules
    (const GemPortNetworkCtpObject *ctp,
     const UBOOL8                  isNewUni,
     const UINT32                  checkOid,
     const UINT32                  checkMeId,
     const UINT32                  reachableOid,
     const UINT32                  reachableMeId,
     const OmciMapFilterModelType  type);

CmsRet rutGpon_deleteRules
    (const GemPortNetworkCtpObject *ctp,
     const UBOOL8                  isNewUni,
     const UINT32                  checkOid,
     const UINT32                  checkMeId,
     const UINT32                  reachableOid,
     const UINT32                  reachableMeId,
     const OmciMapFilterModelType  type);

void createVlanFlowsAction(OmciDmIfPair *infoP);


//=======================  Private GPON functions ========================


#endif /* __RUT_GPON_RULE_H__ */
