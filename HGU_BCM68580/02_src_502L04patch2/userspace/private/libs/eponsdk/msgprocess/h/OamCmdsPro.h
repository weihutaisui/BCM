/*
* <:copyright-BRCM:2017:proprietary:epon
* 
*    Copyright (c) 2017 Broadcom 
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
*/


/*
* \file EponCmdsPro.h
* \brief the oam commands process handler
*
*/

#ifndef EPONCMDSPRO_h
#define EPONCMDSPRO_h

#include "bcm_epon_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EPON_USER_MSG_ADDR  "/var/eponapp_sever_addr"

#define MAX_EPON_CMDS_PARA_LEN 5


typedef enum 
{
    EponCmdDumpOamState = 0,
    EponCmdDumpOamAlm,
    EponCmdDumpOamMsg,
    EponCmdDumpOamVars,
    EponCmdTotalCount
}EponCmdsType;

typedef enum
    {
    OAM_DUMP_OFF         = 0,    /*  No OAM message dump. */
    OAM_DUMP_ALARM       = 1,    /*  Only alarm OAM message dump. */
    OAM_DUMP_NORMAL      = 2,    /*  All OAM message dump except for opcode=OamOpInfo */
    OAM_DUMP_ALL         = 0xff  /*  All OAM message dump */
    } OamMsgLogLevel;

typedef enum
    {
    OAM_DUMP_CMD_SHOW,
    OAM_DUMP_CMD_DUMP,
    OAM_DUMP_CMD_CONSOLE,
    OAM_DUMP_CMD_FILE,
    OAM_DUMP_CMD_FILE_OVERWRITE
    } OamMsgLogCmd;

typedef enum
    {
    OAM_VARS_QUEUE_RL
    } OamVarsCmd;

typedef struct oam_cmds_msg
{
    EponCmdsType cmdType;
    U8        paraLength;   //indicate how many valid para this msg have
    U8        paraData[MAX_EPON_CMDS_PARA_LEN];   
} OamCmdsMsg;


////////////////////////////////////////////////////////////////////////////////
/// OamCmdsProcess:  Hanle the detail commands for oam stack from eponctl CLI
///
// Parameters:
/// \param cmds     the detail command message
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamCmdsProcess(OamCmdsMsg *cmds);

extern
OamMsgLogLevel OamMsgLog_getLevel(U8 link);

extern
void OamMsgLog_setLevel(U8 link, OamMsgLogLevel level);
#ifdef __cplusplus
}
#endif

#endif
