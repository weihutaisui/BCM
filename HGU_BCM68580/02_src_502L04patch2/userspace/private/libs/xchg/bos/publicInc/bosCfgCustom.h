/***************************************************************************
*
* <:copyright-BRCM:2014:DUAL/GPL:standard
* 
*    Copyright (c) 2014 Broadcom 
*    All Rights Reserved
* 
* Unless you and Broadcom execute a separate written software license
* agreement governing use of this software, this software is licensed
* to you under the terms of the GNU General Public License version 2
* (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
* with the following added to such license:
* 
*    As a special exception, the copyright holders of this software give
*    you permission to link this software with independent modules, and
*    to copy and distribute the resulting executable under terms of your
*    choice, provided that you also meet, for each linked independent
*    module, the terms and conditions of the license of that module.
*    An independent module is a module which is not derived from this
*    software.  The special exception does not apply to any modifications
*    of the software.
* 
* Not withstanding the above, under no circumstances may you combine
* this software in any way with any other Broadcom software provided
* under a license other than the GPL, without Broadcom's express prior
* written consent.
* 
* :>
*
****************************************************************************/
#ifndef BOSCFGCUSTOM_H
#define BOSCFGCUSTOM_H

#define BOS_OS_LINUXUSER      1

#define BOS_CFG_CRITSECT      1
#define BOS_CFG_EVENT         1
#define BOS_CFG_INIT          1
#define BOS_CFG_IP_ADDRESS    1
#define BOS_CFG_IP_ADDR_V6_SUPPORT 1
#define BOS_CFG_LOG           0
#define BOS_CFG_MSGQ          1
#define BOS_CFG_MUTEX         1
#define BOS_CFG_SEM           1
#define BOS_CFG_SLEEP         1
#define BOS_CFG_SOCKET        1
#define BOS_CFG_TASK          1
#define BOS_CFG_TIME          1
#define BOS_CFG_TIMER         1

#define BOS_CFG_ERROR_NOASSERT        0
#define BOS_CFG_RESET_SUPPORT         0
#define BOS_CFG_CRITSECT_USE_GLOBAL   1

/* maximum number of messages per message queue */
#define BOS_CFG_MSGQ_MAX_MSG_PER_MSGQ 100
/* maximum number of message queues */
#define BOS_CFG_MSGQ_MAX_MSGQS        16
/* maximum message size in message queue */
#define BOS_CFG_MSGQ_MSG_MAX_SIZE     8

/* MTA's task classes */
#define MTA_TASK_DATA         BOS_CFG_TASK_MED_HIGH_VAL
#define MTA_TASK_HARDWARE     BOS_CFG_TASK_MED_HIGH_VAL
#define MTA_TASK_BACKGROUND   BOS_CFG_TASK_MED_LOW_VAL
#define MTA_TASK_ROOT         BOS_CFG_TASK_MED_HIGH_VAL
#define MTA_TASK_DEFAULT      BOS_CFG_TASK_MED_VAL

/* Number of external task classes used */
#define BOS_CFG_TASK_NUM_EXT_CLASSES   7

/* Task priority for MTA root task */
#define BOS_CFG_ROOT_TASK_PRIORITY   MTA_TASK_ROOT

/* Task priority for MTA application task */
#define BOS_CFG_APP_TASK_PRIORITY    MTA_TASK_DEFAULT

#endif   /* BOSCFGCUSTOM_H */
