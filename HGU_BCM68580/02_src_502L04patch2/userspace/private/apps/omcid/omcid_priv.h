/***********************************************************************
 *
 *  Copyright (c) 2015 Broadcom
 *  All Rights Reserved
 *
<:label-BRCM:2014:proprietary:omcid

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
*      OMCID local header file.
*
*****************************************************************************/

#ifndef OMCID_PRIV_H
#define OMCID_PRIV_H

/* ---- Include Files ----------------------------------------------------- */

#include "omcid.h"
#include "omci_util.h"
#if defined (BRCM_CMS_BUILD)
#include "cms_msg.h"
#endif /* BRCM_CMS_BUILD */


/* ---- Constants and Types ----------------------------------------------- */

typedef struct THREAD_DATA
{
    CmsEntityId entityId;
    void        *pMsgHandle;
    void        *pTmrHandle;
} THREAD_DATA, *PTHREAD_DATA;

typedef enum
{
    OMCI_ERR_NONE = 0,
    OMCI_ERR_SWDL_SECTION_HOLE,
    OMCI_ERR_SWDL_SECTION_RSP,
    OMCI_ERR_SWDL_IMAGE_CRC
} OmciDbgErr_t;


/* ---- Variable Externs -------------------------------------------------- */

extern void *msgHandle;
extern void *tmrHandle;
extern void *pmTmrHandle;

/*
 * mutex, signal data for synchronization between
 * omcid main and omcipm thread. They're used in
 * omcid_pm.c, omci_pm_alarm.c
 */
extern pthread_mutex_t gpon_link_mutex;
extern int gponLinkChange;

extern OmciCaptureReplay_t OmciCapture;
extern OmciDbgErr_t OmciDbgErr;


/* ---- Function Prototypes ----------------------------------------------- */

#if defined (BRCM_CMS_BUILD)
int omci_cmsmsg_handler(void);
void omci_app_send_postmdm_cmsmsg(void);
CmsRet omci_cmsInit(const CmsEntityId entityId, SINT32 shmId);
void omci_cmsCleanup(void);
#endif /* BRCM_CMS_BUILD */
void scheduleHandleModelPath(UBOOL8);


#endif /* OMCID_PRIV_H */
