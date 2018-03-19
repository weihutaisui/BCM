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
****************************************************************************
*
*  Filename: bosTime.h
*
****************************************************************************
*  Description:
*
*
****************************************************************************/
/**
*
*  @file    bosTime.h
*
*  @brief   Contains the BOS Time module interface
*
****************************************************************************/
/**
*  @defgroup   bosTime   BOS Time
*
*  @brief      Provides time module interface
*
*  The BOS time interface provides the ability to get and set the application
*  system time.
*
****************************************************************************/

#ifndef BOSTIME_H
#define BOSTIME_H

#ifndef BOSCFG_H
#include <bosCfg.h>     /* include application-specific BOS config file */
#endif
#ifndef BOSTYPES_H
#include "bosTypes.h"
#endif

#ifdef          __cplusplus
extern  "C" {                           /* C declarations in C++ */
#endif

#if !BOS_CFG_TIME
#error BOS Time module not selected in bosCfg.h
#endif

#include <time.h>

/* ---- Constants and Types ---------------------------------------------- */

/* Definitions for the maximum value that may be returned by bosGetTimeMs().
 * This is used to handle wrapping conditions when calculating delta system
 * times. */
#define BOS_MAX_TIME   (0xFFFFFFFFuL)

/**
 * Function index, used with the error handler to report which function
 * an error originated from.
 *
 * These constants would typically only be used if you provide your own
 * error handler.
 *
 * @note If you change anything in this enumeration, please ensure that
 *       you make the corresponding updates in the bosTimeInitFunctionNames
 *       routine.
 */

typedef enum
{
   BOS_TIME_INIT_FUNC_ID,           /**< bosTimeInit                       */
   BOS_TIME_TERM_FUNC_ID,           /**< bosTimeTerm                       */
   BOS_TIME_GET_TIME_MS_FUNC_ID,    /**< bosTimeGetMs                      */
   BOS_TIME_DELTA_TIME_MS_FUNC_ID,  /**< bosTimeCalcDeltaMs                */
   BOS_TIME_SET_WALL_TIME_ID,       /**< bosTimeSetWallTime                */
   BOS_TIME_GET_GMTIME,             /**< bosTimeGetGmTime                  */
   BOS_TIME_GET_LOCALTIME,          /**< bosTimeGetLocalTime               */
   BOS_TIME_MKTIME,                 /**< bosTimeMkTime                     */
   BOS_TIME_GET_TIME_OF_DAY_ID,     /**< bosTimeGetTimeOfDay               */
   BOS_TIME_NUM_FUNC_ID   /**< Number of functions in the Time module.   */

} BOS_TIME_FUNC_ID;

typedef unsigned int BOS_TIME_SEC;
typedef unsigned int BOS_TIME_USEC;

typedef struct 
{
   BOS_TIME_SEC sec;
   BOS_TIME_USEC usec;
} BOS_TIMESTAMP;

/* ---- Variable Externs ------------------------------------------------- */

/**
 * Contains string versions of the time function names. This table is
 * indexed using values from the BOS_TIME_FUNC_ID enumeration.
 */

extern const char *gBosTimeFunctionName[];

/* ---- Function Prototypes ---------------------------------------------- */

/***************************************************************************/
/**
*  Initializes the BOS Time module.
*
*  @return  BOS_STATUS_OK if the initialization was successful, or an error
*           code if some type of error occured.
*/

BOS_STATUS bosTimeInit( void );

/***************************************************************************/
/**
*  Terminates the BOS Time module.
*
*  @return  BOS_STATUS_OK if the termination was successful, or an error
*           code if some type of error occured.
*/

BOS_STATUS bosTimeTerm( void );

/**
 * DESCRIPTION:
 *           - Get time of day in sec and microsec, since Epoch
 *
 * PARAMETERS:
 *           - osTimeSec      - Current Syetem Time (second).
 *           - osTimeSec      - Current Syetem Time (microsecond).
 *
 * RETURNS:
 *           - BOS_STATUS_OK
 *              - if successful
 *           - error code
 *              - on failure
 *
 * COMMENTS:
 */
BOS_STATUS bosTimeGetTimeOfDay (BOS_TIME_SEC *osTimeSec, BOS_TIME_USEC *osTimeMicroSec);


/**
 * DESCRIPTION:
 *           - Get time in ms; use bosGetDeltaTimeMs to delta two values retrived with this function.
 *
 * PARAMETERS:
 *           - bosTime (O)
 *                   - Time in milliseconds.
 *
 * RETURNS:
 *           - BOS_STATUS_OK
 *              - if successful
 *           - error code
 *              - on failure
 *
 * COMMENTS:
 *     - The return value is not relative to any absolute base reference;
 *       different OS implementations are free to use different base reference
 *       values. The bosCalcDeltaTimeMs function is available to calculate the
 *       delta between two measured values.
 */
BOS_STATUS bosTimeGetMs (BOS_TIME_MS *bosTime);

/**
 * DESCRIPTION:
 *           - Calculate the difference between two measured time values in ms.
 *
 * PARAMETERS:
 *           - firstTime (I)
 *                   - Older measured time value.
 *           - secondTime (I)
 *                   - More recent measured time value.
 *           - deltaTime (O)
 *                   - Difference between the two input time values.
 *
 * RETURNS:
 *           - BOS_STATUS_OK
 *              - if successful
 *           - error code
 *              - on failure
 *
 * COMMENTS:
 *     - The wrap condition (secondTime < firstTime) is accounted for.
 */
BOS_STATUS bosTimeCalcDeltaMs
(
   const BOS_TIME_MS *firstTime,
   const BOS_TIME_MS *secondTime,
   BOS_TIME_MS *deltaTime
);

/**
 * DESCRIPTION:
 *           - Set the wall (current system) time.
 *
 * PARAMETERS:
 *           - wallTime (I)
 *                   - # of seconds elapsed since midnight Jan 1, 1970
 *
 * RETURNS:
 *           - BOS_STATUS_OK
 *              - if successful
 *           - error code
 *              - on failure
 *
 * COMMENTS:
 *
 */
BOS_STATUS bosTimeSetWallTime ( const time_t *wallTime );

/**
 * DESCRIPTION:
 *           - This function converts the calendar time pointed to by 'gmTime'
 *             into broken-down time, expressed as Coordinated Universal Time
 *             (UTC). The calendar time is generally obtained through a call
 *             to time().
 *
 * PARAMETERS:
 *           - wallTime (I)
 *                   - # of seconds elapsed since midnight Jan 1, 1970
 *           - brokenDownTime (O)
 *                   - broken-down time (expressed as UTC)
 *
 * RETURNS:
 *           - BOS_STATUS_OK
 *              - if successful
 *           - error code
 *              - on failure
 *
 * COMMENTS:
 *           - This is the reentrant version of the ANSI function gmtime().
 */
BOS_STATUS bosTimeGetGmTime ( const time_t *gmTime, struct tm *brokenDownTime );

/**
 * DESCRIPTION:
 *           - This function converts the calendar time pointed to by 'localTime'
 *             into broken-down time. The time is represented in local time.
 *             The calendar time is generally obtained through a call to
 *             time().
 *
 * PARAMETERS:
 *           - wallTime (I)
 *                   - # of seconds elapsed since midnight Jan 1, 1970
 *           - brokenDownTime (O)
 *                   - broken-down time (local)
 *
 * RETURNS:
 *           - BOS_STATUS_OK
 *              - if successful
 *           - error code
 *              - on failure
 *
 * COMMENTS:
 *           - This is the reentrant version of the ANSI function localtime().
 */
BOS_STATUS bosTimeGetLocalTime ( const time_t *localTime, struct tm *brokenDownTime );

/**
 * DESCRIPTION:
 *           - This function converts the broken-down time, expressed as local
 *             time into calendar time with the same encoding as the time()
 *             function.
 *
 * PARAMETERS:
 *           - brokenDownTime (I)
 *                   - broken-down time (local)
 *           - time (O)
 *                   - calendar time value
 *
 * RETURNS:
 *           - BOS_STATUS_OK
 *              - if successful
 *           - error code
 *              - on failure
 *
 * COMMENTS:
 *           - This is the reentrant version of the ANSI function mktime().
 */
BOS_STATUS bosTimeMkTime ( struct tm *brokenDownTime, time_t *pTime );


#ifdef          __cplusplus
}                               /* End of C declarations */
#endif

#endif   /* BOSTIME_H */
