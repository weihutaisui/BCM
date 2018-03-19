/***************************************************************************
* <:copyright-BRCM:2002:proprietary:standard
* 
*    Copyright (c) 2002 Broadcom 
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
* :>
****************************************************************************
*
*    Filename: bosPrivateEvent.h
*
****************************************************************************
*    Description:
*
*      bos Event module private internal header file
*
****************************************************************************/

#ifndef BOSPRIVATEEVENT_H
#define BOSPRIVATEEVENT_H

#ifndef BOSCFG_H
#include <bosCfg.h>     /* include application-specific BOS config file */
#endif

#ifndef BOSTYPES_H
#include "bosTypes.h"
#endif

#if !BOS_CFG_EVENT
#error bos Event module not selected in bosCfg.h
#endif

#ifdef          __cplusplus
extern  "C" {                           /* C declarations in C++ */
#endif

/*
*****************************************************************************
** CONSTANTS
*****************************************************************************
*/

/*
*****************************************************************************
** TYPE DEFINITIONS
*****************************************************************************
*/

/*
*****************************************************************************
** FUNCTION PROTOTYPES
*****************************************************************************
*/


/***************************************************************************/
/**
*  Initializes the gBosEventFunctionName table. This function is called
*  from bosEventInit().
*/

void bosEventInitFunctionNames( void );


/**
 * DESCRIPTION:
 *           - Register a newly-created task with the event module.
 *
 * PARAMETERS:
 *           - taskId (I)
 *                    - Task ID associated with task to register.
 *
 * RETURNS:
 *           - BOS_STATUS_OK
 *              - if successful
 *           - error code
 *              - on failure
 *
 * COMMENTS:
 */
BOS_STATUS bosEventRegisterTask( BOS_TASK_ID *taskId );

/**
 * DESCRIPTION:
 *           - Un-register a previously-registered task with the event module.
 *
 * PARAMETERS:
 *           - taskId (I)
 *                    - Task ID associated with task to register.
 *
 * RETURNS:
 *           - BOS_STATUS_OK
 *              - if successful
 *           - error code
 *              - on failure
 *
 * COMMENTS:
 */
BOS_STATUS bosEventUnRegisterTask( BOS_TASK_ID *taskId );

#ifdef          __cplusplus
}                               /* End of C declarations */
#endif

#endif   /* BOSPRIVATEEVENT_H */

