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
*  @file    bosSem.h
*
*  @brief   Contains the BOS Semaphore interface.
*
****************************************************************************/
/**
*  @defgroup   bosSem   BOS Semaphore
*
*  @brief   Provides a counting semaphore.
*
*  The counting semaphore allows task to manage a counted resource
*  (for example tracking the number of items on a queue).
*
*  Typical usage:
*
*  @code
*     typedef struct
*     {
*        BOS_MUTEX      mutex;
*        BOS_SEM        availSem;
*        Node          *list;
*
*     } Queue;
*
*     void Insert( Queue *q, Node *node )
*     {
*        bosMutexAcquire( &q->mutex );
*        {
*           node->next = q->head;
*           q->head = node;
*        }
*        bosMutexRelease( &q->mutex );
*
*        // Do the bosSemGive outside of the mutual exclusion block so
*        // that we don't cause excessive context switches.
*        //
*        // If we were a lower priority task that than a task blocked on
*        // bosSemTake, and we did the Give inside the mutex, then we would
*        // immediately context switch to the higher priority task, who
*        // would try to acquire the mutex, which we hold. The high priority
*        // task would block, context switching back to us. We would then
*        // release the mutex, and context switch back to the high priority
*        // task.
*        //
*        // By doing the give outside, the higher priority task won't wake
*        // up until we do the give, and it will be able to acquire the
*        // mutual exclusion immediately. This saves two unnecessary
*        // context switches.
*
*        bosSemGive( &q->availSem );
*     }
*  @endcode
*
****************************************************************************/

#if !defined( BOSSEM_H )
#define BOSSEM_H              /**< Include Guard                           */

/* ---- Include Files ---------------------------------------------------- */

#if !defined( BOSCFG_H )
#  include <bosCfg.h>
#endif

#if !defined( BOSTYPES_H )
#  include <bosTypes.h>
#endif

#if !BOS_CFG_SEM
#error   BOS Sem module is not selected in bosCfg.h
#endif

#if BOS_OS_ECOS
#  include <../eCos/bosSemECOS.h>
#elif BOS_OS_PSOS
#  include <../pSOS/bosSemPSOS.h>
#elif BOS_OS_VXWORKS
#  include <../VxWorks/bosSemVxWorks.h>
#elif BOS_OS_WIN32
#  include <../Win32/bosSemWin32.h>
#elif BOS_OS_LINUXKERNEL
#  include <../LinuxKernel/bosSemLinuxKernel.h>
#elif BOS_OS_LINUXUSER
#  include <../LinuxUser/bosSemLinuxUser.h>
#else
#  error Unknown OS configuration from bosCfg.h
#endif

#if defined( __cplusplus )
extern "C"
{
#endif

/**
 * @addtogroup bosSem
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
 *       you make the corresponding updates in the bosSemInitFunctionNames
 *       routine.
 */

typedef enum
{
   BOS_SEM_INIT_FUNC_ID,         /**< bosSemInit                        */
   BOS_SEM_TERM_FUNC_ID,         /**< bosSemTerm                        */
   BOS_SEM_CREATE_FUNC_ID,       /**< bosSemCreate                      */
   BOS_SEM_DESTROY_FUNC_ID,      /**< bosSemDestroy                     */
   BOS_SEM_GIVE_FUNC_ID,         /**< bosSemGive                        */
   BOS_SEM_TAKE_FUNC_ID,         /**< bosSemTake                        */
   BOS_SEM_TIMED_TAKE_FUNC_ID,   /**< bosSemTimedTake                   */

   BOS_SEM_NUM_FUNC_ID   /**< Number of functions in the Sem module.   */

} BOS_SEM_FUNC_ID;

/* ---- Variable Externs ------------------------------------------------- */

/**
 * Contains string versions of the mutex function names. This table is
 * indexed using values from the BOS_SEM_FUNC_ID enumeration.
 */

extern const char *gBosSemFunctionName[];

/* ---- Function Prototypes ---------------------------------------------- */

#if ( BOS_CFG_RESET_SUPPORT )

   #if ( !BUILDING_BOS)
      /* If reset support is enabled, application code (non-OS) will use the
      ** re-defined function prototypes for the reset-aware versions of the following
      ** blocking functions */
      #define bosSemTimedTake       bosSemTimedTakeR
      #define bosSemTake            bosSemTakeR
   #else
      /***************************************************************************/
      /**
      *  Decrements the count associated with the semaphore @a sem. If the count
      *  is less than zero, then the calling task will become blocked until
      *  another thread does a give on the semaphore. This function will only
      *  block the calling thread for @a timeoutMsec milliseconds, before
      *  returning with BOS)STATUS_TIMEOUT.
      *
      *  @param   sem         (mod) Semaphore object to take.
      *  @param   timeoutMsec (in)  Number of milliseconds to wait for the
      *                             semaphore to enter a state where it can be
      *                             taken.
      *
      *  @return  BOS_STATUS_OK if the semaphore was taken, BOS_STATUS_TIMEOUT if
      *           the timeout period expired, or an error code if the semaphore could
      *           not be taken for another reason.
      *
      *  @note    bosSemTimedTakeR() is the reset equivalent of bosSemTimedTake().
      *           It will return BOS_STATUS_RESET if the calling task has been
      *           notified of a reset.
      */
      BOS_STATUS bosSemTimedTakeR( BOS_SEM *sem, BOS_TIME_MS timeout );
   #endif

#endif   /* BOS_CFG_RESET_SUPPORT */

/***************************************************************************/
/**
*  Initializes the BOS Sem module.
*
*  @return  BOS_STATUS_OK if the initialization was successful, or an error
*           code if some type of error occured.
*/

BOS_STATUS bosSemInit( void );

/***************************************************************************/
/**
*  Terminates the BOS Sem module.
*
*  @return  BOS_STATUS_OK if the termination was successful, or an error
*           code if some type of error occured.
*/

BOS_STATUS bosSemTerm( void );

/***************************************************************************/
/**
*  Creates a counting semaphore object, which can subsequently be used to
*  guard multiple instances of a given resource.
*
*  @param   name        (in)  Name to assign to the semaphore (must be unique).
*  @param   initCount   (in)  Initial count that the semaphore should have.
*  @param   maxCount    (in)  Maximum count that the sempahore can have.
*  @param   sem         (out) Place to put the newly created semaphore.
*
*  @return  BOS_STATUS_OK if the semaphore was created successfully, or an
*           error code if the semaphore could not be created.
*
*  @note    Some OS's will ignore maxCount, others require it.
*           Some OS's will ignore name, others will require it.
*/

BOS_STATUS bosSemCreate
(
   const char *name,
   int         initCount,
   int         maxCount,
   BOS_SEM    *sem
);

/***************************************************************************/
/**
*  Destroys a previously created semaphore object.
*
*  @param   sem   (mod) Semaphore object to destroy.
*
*  @return  BOS_STATUS_OK if the semaphore was destroyed successfully, or an
*           error code if the semaphore could not be destroyed.
*/

BOS_STATUS bosSemDestroy( BOS_SEM *sem );

/***************************************************************************/
/**
*  Increments the count associated with the semaphore @a sem. This will
*  cause one thread blocked on a take to wake up.
*
*  @param   sem   (mod) Semaphore object to give.
*
*  @return  BOS_STATUS_OK if the semaphore was given, or an error code if
*           the semaphore could not be given.
*/

BOS_STATUS bosSemGive( BOS_SEM *sem );

/***************************************************************************/
/**
*  Decrements the count associated with the semaphore @a sem. If the count
*  is less than zero, then the calling task will become blocked until
*  another thread does a give on the semaphore.
*
*  @param   sem   (mod) Semaphore object to take.
*
*  @return  BOS_STATUS_OK if the semaphore was taken, or an error code if
*           the semaphore could not be taken.
*/

BOS_STATUS bosSemTake( BOS_SEM *sem );

/***************************************************************************/
/**
*  Decrements the count associated with the semaphore @a sem. If the count
*  is less than zero, then the calling task will become blocked until
*  another thread does a give on the semaphore. This function will only
*  block the calling thread for @a timeoutMsec milliseconds, before
*  returning with BOS)STATUS_TIMEOUT.
*
*  @param   sem         (mod) Semaphore object to take.
*  @param   timeoutMsec (in)  Number of milliseconds to wait for the
*                             semaphore to enter a state where it can be
*                             taken.
*
*  @return  BOS_STATUS_OK if the semaphore was taken, BOS_STATUS_TIMEOUT if
*           the timeout period expired, or an error code if the semaphore could
*           not be taken for another reason.
*
*  @note:   Calling bosSemTimedTake( semaphore, BOS_WAIT_FOREVER ) is
*           equivalent to calling bosSemTake.
*/

BOS_STATUS bosSemTimedTake( BOS_SEM *sem, BOS_TIME_MS timeoutMsec );

/*
*****************************************************************************
 * Function Name: bosSemTakeBlock
 * Description  : This function is used by tasks to take a BOS semaphore.
 *                The task could end up being blocked waiting for the semaphore.
 * Returns      : BOS_STATUS_OK  = successful
 *                BOS_STATUS_ERR = failed
*****************************************************************************
*/
BOS_STATUS bosSemTakeBlock( BOS_SEM *sem );

/** @} */

#define bosSemTakeR( sem )     bosSemTimedTakeR( sem, BOS_WAIT_FOREVER )

#if defined( __cplusplus )
}
#endif

#endif /* BOSSEM_H */

