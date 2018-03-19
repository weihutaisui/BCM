/***********************************************************************
 *
 *  Copyright (c) 2007-2008  Broadcom Corporation
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

#ifndef __OAL_H__
#define __OAL_H__

#include "cms.h"


/*
 * This header file contains all the functions exported by the
 * OS Adaptation Layer (OAL).  The OAL functions live under the
 * directory with the name of the OS, e.g. linux, ecos.
 * The Make system will automatically compile the appropriate files
 * and link them in with the final executable based on the TARGET_OS
 * variable, which is set by make menuconfig.
 */


/* from oal_mem_monitor.c */
extern int oal_msm_getSystemMemInfo(void);

/* from oal_event.c */
/** Initialize the event and messaging sub-system, including the dynamic
 *  launch service (dls).
 *
 * @return CmsRet enum.
 */
CmsRet oalEvent_init(void);



void oalEvent_cleanup(void);
extern CmsRet oal_processEvents(void);


/** Do system initialization.
 *
 * In oal_system.c.
 *
 * @return CmsRet enum.
 */
CmsRet oalSystem_init(void);


/** Launch any apps that requested launch on boot.
 *
 * @param stage (IN) Launching processes by smd is divided up into 2 stages.
 *                   In the first stage, only ssk is launched.
 *                   ssk initializes the MDM, and sends an event msg back to
 *                   smd.  smd then calls this function again for launching
 *                   stage 2 apps.
 */
void oal_launchOnBoot(UINT32 stage);


/** Do system cleanup.
 *
 * In oal_system.c.
 *
 * @return CmsRet enum.
 */
void oalSystem_cleanup(void);


/** Handler function for sending delayed messages from a timer callback.
 *
 * @param id (IN)  The delayed message id that was specified by the requestor
 *                 of the delayed message.
 * @param ctx (IN) This is actually the dInfo struct of the requestor.
 */
void oal_sendDelayedMsg(UINT32 id, void *ctx);


/** Reboot the system by doing an ioctl into the kernel.
 *
 */
void oal_sysMipsSoftReset(void);


/** This macro allows us to keep track of the maximum fd number.
 *
 * Useful during prctl_spawnChild and select.
 */
#define UPDATE_MAXFD(f)  (maxFd = (f > maxFd) ? f : maxFd)

/** MDM lock timeout.
 * 
 * smd should minimize it access to MDM because if any RCL/STL tries to send a 
 * message out, we could create a deadlock.
 */
#define SMD_LOCK_TIMEOUT  (6 * MSECS_IN_SEC)

#endif /* __OAL_H__ */
