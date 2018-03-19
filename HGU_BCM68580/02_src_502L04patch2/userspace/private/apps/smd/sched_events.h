/***********************************************************************
 *
 *  Copyright (c) 2006-2008  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:standard

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

#ifndef __SCHED_EVENTS_H__
#define __SCHED_EVENTS_H__

#include "cms.h"
#include "cms_eid.h"
#include "cms_msg.h"


/*!\file event_interest.h
 * \brief Header file for scheduled/timer event manipulation functions.
 *
 */

/** Initialize the delayed message scheduler.
 *
 * @return CmsRet enum.
 */
CmsRet sched_init(void);


/** Clean up delayed message scheduler.
 */
void sched_cleanup(void);


/** Create a delayed message event.
 *
 * @param id  (IN) The delayed msg id.
 * @param ctx (IN) The dInfo structure.
 * @param ms  (IN) The number of milliseconds in the future to send this 
 *                 delayed message.
 *
 * @return CmsRet enum.
 */
CmsRet sched_set(UINT32 id, void *ctx, SINT32 ms);


/** Cancel a delayed message event.
 *
 * @param id  (IN) The delayed msg id.
 * @param ctx (IN) The dInfo structure.
 *
 * @return CmsRet enum.
 */
CmsRet sched_cancel(UINT32 id, void *ctx);


/** Get number of milliseconds to the next event.
 *
 * @return number of milliseconds to the next event.  If there are no events,
 *         then UINT32_MAX is returned.
 */
UINT32 sched_getTimeToNextEvent(void);


/** Return time remaining for timer matching specified delayed msg id and 
 *  dInfo structure.
 *
 * @param id  (IN) The delayed msg id.
 * @param ctx (IN) The dInfo structure.
 * @param ms  (IN) The number of milliseconds in the future to send this 
 *                 delayed message.
 *
 * @return CmsRet enum.
 */
CmsRet sched_getTimeRemaining(UINT32 id, void *ctx, UINT32 *ms);


/** Called by smd_main to execute any expired events.
 */
void sched_process(void);


#endif /* __SCHED_EVENTS_H__ */
