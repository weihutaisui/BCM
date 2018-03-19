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
*  @file    bosSleep.h
*
*  @brief   Contains the BOS Sleep interface.
*
****************************************************************************/
/**
*  @defgroup   bosSleep   BOS Sleep
*
*  @brief   Provides the ability to have a task goto sleep.
*
****************************************************************************/

#if !defined( BOSSLEEP_H )
#define BOSSLEEP_H            /**< Include Guard                           */

/* ---- Include Files ---------------------------------------------------- */

#if !defined( BOSCFG_H )
#  include <bosCfg.h>
#endif

#if !defined( BOSTYPES_H )
#  include <bosTypes.h>
#endif

#if !BOS_CFG_SLEEP
#error   BOS Sleep module is not selected in bosCfg.h
#endif

#if defined( __cplusplus )
extern "C"
{
#endif

/**
 * @addtogroup bosSleep
 * @{
 */

/* ---- Constants and Types ---------------------------------------------- */

/**
 * Function index, used with the error handler to report which function
 * an error originated from.
 *
 * These constants would typically only be used if you provide your own
 * error handler.
 *
 * @note If you change anything in this enumeration, please ensure that
 *       you make the corresponding updates in the bosMutexInitFunctionNames
 *       routine.
 */

typedef enum
{
   BOS_SLEEP_INIT_FUNC_ID,    /**< bosSleepInit                            */
   BOS_SLEEP_TERM_FUNC_ID,    /**< bosSleepTerm                            */
   BOS_SLEEP_SLEEP_FUNC_ID,   /**< bosSleep                                */

   BOS_SLEEP_NUM_FUNC_ID   /**< Number of functions in the Sleep module.   */

} BOS_SLEEP_FUNC_ID;

/* ---- Variable Externs ------------------------------------------------- */

/**
 * Contains string versions of the sleep function names. This table is
 * indexed using values from the BOS_SLEEP_FUNC_ID enumeration.
 */

extern const char *gBosSleepFunctionName[];

/* ---- Function Prototypes ---------------------------------------------- */

#if ( BOS_CFG_RESET_SUPPORT )

   #if ( !BUILDING_BOS)
      /* If reset support is enabled, application code (non-OS) will use the
      ** re-defined function prototypes for the reset-aware versions of the following
      ** blocking functions */
      #define bosSleep              bosSleepR
   #else
      /***************************************************************************/
      /**
      *  Sleeps for the indicated number of milli-seconds.
      *
      *  @param   msec  (in)  Number of milliseconds to sleep for.
      *
      *  @return  BOS_STATUS_OK if the sleep passed successfully, or an error code
      *           if the sleep could not be performed.
      *
      *  @note    Where supported, requesting a sleep of zero milliseconds will
      *           cause the calling task to give up its timeslice to other tasks
      *           with an equal or higher priority. Otherwise a sleep of zero will
      *           sleep for the smallest amount of time that OS allows.
      *
      *  @note    bosSleepR() is the reset equivalent of bosSleep().
      *           It will return BOS_STATUS_RESET if the calling task has been
      *           notified of a reset.
      */
      BOS_STATUS bosSleepR( BOS_TIME_MS sleeptime );
   #endif

#endif   /* BOS_CFG_RESET_SUPPORT */

/***************************************************************************/
/**
*  Initializes the BOS Sleep module.
*
*  @return  BOS_STATUS_OK if the initialization was successful, or an error
*           code if some type of error occured.
*/

BOS_STATUS bosSleepInit( void );

/***************************************************************************/
/**
*  Terminates the BOS Sleep module.
*
*  @return  BOS_STATUS_OK if the termination was successful, or an error
*           code if some type of error occured.
*/

BOS_STATUS bosSleepTerm( void );

/***************************************************************************/
/**
*  Sleeps for the indicated number of milli-seconds.
*
*  @param   msec  (in)  Number of milliseconds to sleep for.
*
*  @return  BOS_STATUS_OK if the sleep passed successfully, or an error code
*           if the sleep could not be performed.
*
*  @note    Where supported, requesting a sleep of zero milliseconds will
*           cause the calling task to give up its timeslice to other tasks
*           with an equal or higher priority. Otherwise a sleep of zero will
*           sleep for the smallest amount of time that OS allows.
*/

BOS_STATUS bosSleep( BOS_TIME_MS mSec );

/***************************************************************************/
/**
*  Sleeps for the indicated number of nano-seconds.
*
*  @param   nsec  (in)  Number of nanoseconds to sleep for.
*
*  @return  BOS_STATUS_OK if the sleep passed successfully, or an error code
*           if the sleep could not be performed.
*
*  @note    Where supported, requesting a sleep of zero nanoseconds will
*           cause the calling task to give up its timeslice to other tasks
*           with an equal or higher priority. Otherwise a sleep of zero will
*           sleep for the smallest amount of time that OS allows.
*/
BOS_STATUS bosSleepNs( BOS_TIME_NS nSec );

/** @} */

#if defined( __cplusplus )
}
#endif

#endif /* BOSSLEEP_H */

