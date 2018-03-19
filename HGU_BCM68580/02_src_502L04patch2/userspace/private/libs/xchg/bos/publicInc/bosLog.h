/****************************************************************************
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
*****************************************************************************/
/**
*
*  @file    bosLog.h
*
*  @brief   Contains the BOS Logging interface.
*
****************************************************************************/
/**
*  @defgroup   bosLog   BOS Log
*
*  @brief      Common logging routines for BOS modules.
*
*  The BOS logging module provides helper routines used by other BOS modules.
*  It initializes and interfaces to the Broadcom logging (blog) module.
*
****************************************************************************/

#if !defined( BOS_LOG_H )
#define BOS_LOG_H            /**< Include Guard                           */

/* ---- Include Files ---------------------------------------------------- */

#if !defined( BOSTYPES_H )
#  include "bosTypes.h"
#endif

#if !defined( BOSCFG_H )
#  include "bosCfg.h"
#endif

#if defined( __cplusplus )
extern "C"
{
#endif

/**
 * @addtogroup bosLog
 * @{
 */

/* ---- Constants and Types ---------------------------------------------- */

typedef enum {
    BOS_LOGLEVEL_ALWAYS,
    BOS_LOGLEVEL_EMERG,
    BOS_LOGLEVEL_CRIT,
    BOS_LOGLEVEL_ALERT,
    BOS_LOGLEVEL_ERROR,
    BOS_LOGLEVEL_WARNING,
    BOS_LOGLEVEL_NOTICE,
    BOS_LOGLEVEL_INFO,
    BOS_LOGLEVEL_DEBUG,
} BOS_LOGLEVEL;

#ifndef BOS_LOG
#define BOS_LOG(exp)    bosLog(exp)
#endif

/***************************************************************************/
/**
*  External logging function.  This function MUST be implemented externally!
*
*  @return  None
*
*/
void extBosLog(BOS_LOGLEVEL loglevel, const char* fmt, ...);

#define bosLog(fmt, ...)        extBosLog(BOS_LOGLEVEL_ALWAYS,  fmt, ##__VA_ARGS__)
#define bosLogEmerg(fmt, ...)   extBosLog(BOS_LOGLEVEL_EMERG,   fmt, ##__VA_ARGS__)
#define bosLogCrit(fmt, ...)    extBosLog(BOS_LOGLEVEL_CRIT,    fmt, ##__VA_ARGS__)
#define bosLogAlert(fmt, ...)   extBosLog(BOS_LOGLEVEL_ALERT,   fmt, ##__VA_ARGS__)
#define bosLogErr(fmt, ...)     extBosLog(BOS_LOGLEVEL_ERROR,   fmt, ##__VA_ARGS__)
#define bosLogWarning(fmt, ...) extBosLog(BOS_LOGLEVEL_WARNING, fmt, ##__VA_ARGS__)
#define bosLogNotice(fmt, ...)  extBosLog(BOS_LOGLEVEL_NOTICE,  fmt, ##__VA_ARGS__)
#define bosLogInfo(fmt, ...)    extBosLog(BOS_LOGLEVEL_INFO,    fmt, ##__VA_ARGS__)
#define bosLogDebug(fmt, ...)   extBosLog(BOS_LOGLEVEL_DEBUG,   fmt, ##__VA_ARGS__)



/***********************************************************
 * extBosLog MUST be implemented externally.
 ***********************************************************/
/*
#define bosLog(fmt, ...)        voiceLog(VOICE_MODULE_BOS, VOICE_LOGLEVEL_ALWAYS,  fmt, ##__VA_ARGS__)
#define bosLogEmerg(fmt, ...)   voiceLog(VOICE_MODULE_BOS, VOICE_LOGLEVEL_EMERG,   fmt, ##__VA_ARGS__)
#define bosLogCrit(fmt, ...)    voiceLog(VOICE_MODULE_BOS, VOICE_LOGLEVEL_CRIT,    fmt, ##__VA_ARGS__)
#define bosLogAlert(fmt, ...)   voiceLog(VOICE_MODULE_BOS, VOICE_LOGLEVEL_ALERT,   fmt, ##__VA_ARGS__)
#define bosLogErr(fmt, ...)     voiceLog(VOICE_MODULE_BOS, VOICE_LOGLEVEL_ERROR,   fmt, ##__VA_ARGS__)
#define bosLogWarning(fmt, ...) voiceLog(VOICE_MODULE_BOS, VOICE_LOGLEVEL_WARNING, fmt, ##__VA_ARGS__)
#define bosLogNotice(fmt, ...)  voiceLog(VOICE_MODULE_BOS, VOICE_LOGLEVEL_NOTICE,  fmt, ##__VA_ARGS__)
#define bosLogInfo(fmt, ...)    voiceLog(VOICE_MODULE_BOS, VOICE_LOGLEVEL_INFO,    fmt, ##__VA_ARGS__)
#define bosLogDebug(fmt, ...)   voiceLog(VOICE_MODULE_BOS, VOICE_LOGLEVEL_DEBUG,   fmt, ##__VA_ARGS__)
*/

/* ---- Variable Externs ------------------------------------------------- */
/* ---- Function Prototypes ---------------------------------------------- */
#define bosLogInit()
#define bosLogTerm()


/** @} */

#if defined( __cplusplus )
}
#endif

#endif /* BOS_LOG_H */
