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
*  @file    bosMutex.h
*
*  @brief   Contains the BOS Mutex interface.
*
****************************************************************************/
/**
*  @defgroup   bosMutex   BOS Mutex
*
*  @brief   Provides mutual exclusion (mutex).
*
*  The mutual exclusion which is provided allows only a single task to own
*  the mutex at one time. The mutex provided here is also nestable, which
*  means that the same task can acquire the mutex without deadlocking
*  itself, provided that it releases the mutex the same number of times
*  that it acquires it.
*
*  Typical usage:
*
*  @code
*     typedef struct Node_s
*     {
*        void          *data;
*        struct Node_s *next;
*
*     } Node;
*
*     typedef struct
*     {
*        BOS_MUTEX      mutex;
*        Node          *head;
*
*     } List;
*
*     void Insert( List *list, Node *node )
*     {
*        bosMutexAcquire( &list->mutex );
*        {
*           node->next = list->head;
*           list->head = node;
*        }
*        bosMutexRelease( &list->mutex );
*     }
*  @endcode
*
****************************************************************************/

#if !defined( BOSMUTEX_H )
#define BOSMUTEX_H            /**< Include Guard                           */

/* ---- Include Files ---------------------------------------------------- */

#if !defined( BOSCFG_H )
#  include <bosCfg.h>
#endif

#if !defined( BOSTYPES_H )
#  include <bosTypes.h>
#endif

#if !BOS_CFG_MUTEX
#error   BOS Mutex module is not selected in bosCfg.h
#endif

#if BOS_OS_ECOS
#  include <../eCos/bosMutexECOS.h>
#elif BOS_OS_PSOS
#  include <../pSOS/bosMutexPSOS.h>
#elif BOS_OS_VXWORKS
#  include <../VxWorks/bosMutexVxWorks.h>
#elif BOS_OS_WIN32
#  include <../Win32/bosMutexWin32.h>
#elif BOS_OS_LINUXKERNEL
#  include <../LinuxKernel/bosMutexLinuxKernel.h>
#elif BOS_OS_LINUXUSER
#  include <../LinuxUser/bosMutexLinuxUser.h>
#else
#  error Unknown OS configuration from bosCfg.h
#endif

#if defined( __cplusplus )
extern "C"
{
#endif

/**
 * @addtogroup bosMutex
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
   BOS_MUTEX_INIT_FUNC_ID,          /**< bosMutexInit                      */
   BOS_MUTEX_TERM_FUNC_ID,          /**< bosMutexTerm                      */
   BOS_MUTEX_CREATE_FUNC_ID,        /**< bosMutexCreate                    */
   BOS_MUTEX_DESTROY_FUNC_ID,       /**< bosMutexDestroy                   */
   BOS_MUTEX_ACQUIRE_FUNC_ID,       /**< bosMutexAcquire                   */
   BOS_MUTEX_TIMED_ACQUIRE_FUNC_ID, /**< bosMutexTimedAcquire              */
   BOS_MUTEX_RELEASE_FUNC_ID,       /**< bosMutexRelease                   */

   BOS_MUTEX_NUM_FUNC_ID   /**< Number of functions in the Mutex module.   */

} BOS_MUTEX_FUNC_ID;

/* ---- Variable Externs ------------------------------------------------- */

/**
 * Contains string versions of the mutex function names. This table is
 * indexed using values from the BOS_MUTEX_FUNC_ID enumeration.
 */

extern const char *gBosMutexFunctionName[];

/* ---- Function Prototypes ---------------------------------------------- */

#if ( BOS_CFG_RESET_SUPPORT )

   #if ( !BUILDING_BOS)
      /* If reset support is enabled, application code (non-OS) will use the
      ** re-defined function prototypes for the reset-aware versions of the following
      ** blocking functions */
      #define bosMutexTimedAcquire     bosMutexTimedAcquireR
   #else
      /***************************************************************************/
      /**
      *  Acquires the indicated mutual exclusion object. If the object is
      *  currently acquired by another task, then this function will wait for
      *  @a timeoutMsec milli-seconds before returning with BOS_STATUS_TIMEOUT.
      *
      *  @param   mutex       (mod) Mutex object to acquire
      *  @param   timeoutMsec (in) Number of milliseconds to wait for the mutex
      *                       to be acquired.
      *
      *  @return  BOS_STATUS_OK if the mutex was acquired, BOS_STATUS_TIMEOUT if
      *           the timeout period expired, or an error code if the mutex could
      *           not be acquired for another reason.
      *
      *  @note    bosMutexTimedAcquireR() is the reset equivalent of bosMutexTimedAcquire().
      *           It will return BOS_STATUS_RESET if the calling task has been
      *           notified of a reset.
      */
      BOS_STATUS bosMutexTimedAcquireR( BOS_MUTEX *mutex, BOS_TIME_MS timeout );
   #endif

#endif   /* BOS_CFG_RESET_SUPPORT */

/***************************************************************************/
/**
*  Initializes the BOS Mutex module.
*
*  @return  BOS_STATUS_OK if the initialization was successful, or an error
*           code if some type of error occured.
*/

BOS_STATUS bosMutexInit( void );

/***************************************************************************/
/**
*  Terminates the BOS Mutex module.
*
*  @return  BOS_STATUS_OK if the termination was successful, or an error
*           code if some type of error occured.
*/

BOS_STATUS bosMutexTerm( void );

/***************************************************************************/
/**
*  Creates a mutex object, which can subsequently be used to control
*  mutually exclusive access to a resource.
*
*  @param   name  (in)  Name to assign to the mutex (must be unique)
*  @param   mutex (out) Mutex object to initialize.
*
*  @return  BOS_STATUS_OK if the mutex was created successfully, or an
*           error code if the mutex could not be created.
*/

BOS_STATUS bosMutexCreate( const char *name, BOS_MUTEX *mutex );

/***************************************************************************/
/**
*  Destroys a previously created mutex object.
*
*  @param   mutex (mod) Mutex object to destroy.
*
*  @return  BOS_STATUS_OK if the mutex was destroyed successfully, or an
*           error code if the mutex could not be destroyed.
*/

BOS_STATUS bosMutexDestroy( BOS_MUTEX *mutex );

/***************************************************************************/
/**
*  Acquires the indicated mutual exclusion object. If the object is
*  currently acquired by another task, then this function will block
*  until the other task releases the mutex.
*
*  @param   mutex (mod) Mutex object to acquire
*
*  @return  BOS_STATUS_OK if the mutex was acquired, or an error code if
*           the mutex could not be acquired.
*
*  @note    It is allowable for the same task to acquire the mutex multiple
*           times without deadlocking itself, provided that it releases
*           the mutex the same number of times that it acquires it.
*/

BOS_STATUS bosMutexAcquire( BOS_MUTEX *mutex );

/***************************************************************************/
/**
*  Acquires the indicated mutual exclusion object. If the object is
*  currently acquired by another task, then this function will wait for
*  @a timeoutMsec milli-seconds before returning with BOS_STATUS_TIMEOUT.
*
*  @param   mutex       (mod) Mutex object to acquire
*  @param   timeoutMsec (in) Number of milliseconds to wait for the mutex
*                       to be acquired.
*
*  @return  BOS_STATUS_OK if the mutex was acquired, BOS_STATUS_TIMEOUT if
*           the timeout period expired, or an error code if the mutex could
*           not be acquired for another reason.
*
*  @note:   Calling bosMutexTimedAcquire( mutex, BOS_WAIT_FOREVER ) is
*           equivalent to calling bosMutexAcquire.
*/

BOS_STATUS bosMutexTimedAcquire( BOS_MUTEX *mutex, BOS_TIME_MS timeoutMsec );

/***************************************************************************/
/**
*  Releases the indicated mutual exclusion object. This makes it available
*  for another task to acquire (except when the same task has acquried it
*  multiple times, and this isn't the final release).
*
*  @param   mutex (mod) Mutex object to acquire
*
*  @return  BOS_STATUS_OK if the mutex was acquired, or an error code if
*           the mutex could not be acquired.
*/

BOS_STATUS bosMutexRelease( BOS_MUTEX *mutex );


/** @} */

#if defined( __cplusplus )
}
#endif

#endif /* BOSMUTEX_H */

