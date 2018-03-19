/***********************************************************************
 *
 *  Copyright (c) 2014 Broadcom
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

#ifndef __OMCI_PM_SYNC_H__
#define __OMCI_PM_SYNC_H__

/*!\file omci_pm_sync.h
 * Synchronization functions for GPON Performance Monitoring functionality.
 *
 */


#include "os_defs.h"
#include "omci_pm.h"

UINT16 omci_pm_findEnetPort(UINT16 objID);
UINT16 omci_pm_findUpDnEnetPort(UINT16 objID);
UINT32 omci_pm_getEnetPptpInfo(UINT16 obj_ID);
UINT16 omci_pm_findGemPort(UINT16 obj_ID);
UINT32 omci_pm_getEnetPptpObjIDs(int portIndex);
UINT32 omci_pm_getAniGObjID(void);

#ifdef DMP_X_ITU_ORG_VOICE_1
UINT16 omci_pm_findVoipPort(UINT16 objID);
UINT32 omci_pm_getVoipPptpObjIDs(int portIndex);
#endif   // DMP_X_ITU_ORG_VOICE_1

UBOOL8 omci_pm_isAdminStateEnabled(UINT32 mdmObjId, int portIndex);
UBOOL8 omci_pm_getEnetSetupFlag(void);

UINT16 omci_pm_findIwtpGemPort(UINT16 objID);
UINT16 omci_pm_findMcastIwtpGemPort(UINT16 objID);
UINT16 omci_pm_find1pMapperGemPort(UINT16 objID);
UINT16 omci_pm_findPhysicalPort(UINT16 objID, UINT8 *tpType);

#endif  // __OMCI_PM_SYNC_H__
