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
*  @file    bosCritSect.h
*
*  @brief   Contains the BOS CritSect interface.
*
****************************************************************************/
/**
*  @defgroup   bosCritSect   BOS CritSect
*
*  @brief   Provides critical section.
*
*  The critical section which is provided allows multiple tasks to share
*  the critSect at one time. The critSec provide here is nestable and is
*  generally used for very short 'critical' sections of code that should not
*  be interrupted.
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
*        BOS_CRITSECT      critSect;
*        Node          *head;
*
*     } List;
*
*     void Insert( List *list, Node *node )
*     {
*        bosCritSectEnter( &list->critSect );
*        {
*           node->next = list->head;
*           list->head = node;
*        }
*        bosCritSectLeave( &list->critSect );
*     }
*  @endcode
*
****************************************************************************/

#if !defined( BOSCRITSECT_H )
#define BOSCRITSECT_H            /**< Include Guard                           */

/* ---- Include Files ---------------------------------------------------- */

#if !defined( BOSCFG_H )
#  include <bosCfg.h>
#endif

#if !defined( BOSTYPES_H )
#  include <bosTypes.h>
#endif

#if !BOS_CFG_CRITSECT
#error   BOS CritSect module is not selected in bosCfg.h
#endif

#if BOS_OS_ECOS
#  include <../eCos/bosCritSectECOS.h>
#elif BOS_OS_PSOS
#  include <../pSOS/bosCritSectPSOS.h>
#elif BOS_OS_VXWORKS
#  include <../VxWorks/bosCritSectVxWorks.h>
#elif BOS_OS_WIN32
#  include <../Win32/bosCritSectWin32.h>
#elif BOS_OS_LINUXKERNEL
#  include <../LinuxKernel/bosCritSectLinuxKernel.h>
#elif BOS_OS_LINUXUSER
#  include <../LinuxUser/bosCritSectLinuxUser.h>
#else
#  error Unknown OS configuration from bosCfg.h
#endif

#if defined( __cplusplus )
extern "C"
{
#endif

/**
 * @addtogroup bosCritSect
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
 *       you make the corresponding updates in the bosCritSectInitFunctionNames
 *       routine.
 */

typedef enum
{
   BOS_CRITSECT_INIT_FUNC_ID,          /**< bosCritSectInit                      */
   BOS_CRITSECT_TERM_FUNC_ID,          /**< bosCritSectTerm                      */
   BOS_CRITSECT_CREATE_FUNC_ID,        /**< bosCritSectCreate                    */
   BOS_CRITSECT_DESTROY_FUNC_ID,       /**< bosCritSectDestroy                   */
   BOS_CRITSECT_ENTER_FUNC_ID,         /**< bosCritSectEnter                     */
   BOS_CRITSECT_LEAVE_FUNC_ID,         /**< bosCritSectLeave                     */

   BOS_CRITSECT_NUM_FUNC_ID   /**< Number of functions in the CritSect module.   */

} BOS_CRITSECT_FUNC_ID;

/* ---- Variable Externs ------------------------------------------------- */

/**
 * Contains string versions of the critSect function names. This table is
 * indexed using values from the BOS_CRITSECT_FUNC_ID enumeration.
 */

extern const char *gBosCritSectFunctionName[];

#if BOS_CFG_CRITSECT_USE_GLOBAL
extern BOS_CRITSECT *BOS_CFG_GLOBAL_CRITSECT;
#endif


/* ---- Function Prototypes ---------------------------------------------- */

/***************************************************************************/
/**
*  Initializes the BOS CritSect module.
*
*  @return  BOS_STATUS_OK if the initialization was successful, or an error
*           code if some type of error occured.
*/

BOS_STATUS bosCritSectInit( void );

/***************************************************************************/
/**
*  Terminates the BOS CritSect module.
*
*  @return  BOS_STATUS_OK if the termination was successful, or an error
*           code if some type of error occured.
*/

BOS_STATUS bosCritSectTerm( void );

/***************************************************************************/
/**
*  Creates a critSect object, which can subsequently be used to control
*  mutually exclusive access to a resource.
*
*  @param   name  (in)  Name to assign to the critSect (must be unique)
*  @param   critSect (out) CritSect object to initialize.
*
*  @return  BOS_STATUS_OK if the critSect was created successfully, or an
*           error code if the critSect could not be created.
*/

BOS_STATUS bosCritSectCreate( BOS_CRITSECT *critSect );

/***************************************************************************/
/**
*  Destroys a previously created critSect object.
*
*  @param   critSect (mod) CritSect object to destroy.
*
*  @return  BOS_STATUS_OK if the critSect was destroyed successfully, or an
*           error code if the critSect could not be destroyed.
*/

BOS_STATUS bosCritSectDestroy( BOS_CRITSECT *critSect );

/***************************************************************************/
/**
*  Acquires the indicated mutual exclusion object. If the object is
*  currently acquired by another task, then this function will block
*  until the other task releases the critSect.
*
*  @param   critSect (mod) CritSect object to acquire
*
*  @return  BOS_STATUS_OK if the critSect was acquired, or an error code if
*           the critSect could not be acquired.
*
*  @note    It is allowable for the same task to acquire the critSect multiple
*           times without deadlocking itself, provided that it releases
*           the critSect the same number of times that it acquires it.
*/

BOS_STATUS bosCritSectEnter( BOS_CRITSECT *critSect );

/***************************************************************************/
/**
*  Releases the indicated mutual exclusion object. This makes it available
*  for another task to acquire (except when the same task has acquried it
*  multiple times, and this isn't the final release).
*
*  @param   critSect (mod) CritSect object to acquire
*
*  @return  BOS_STATUS_OK if the critSect was acquired, or an error code if
*           the critSect could not be acquired.
*/

BOS_STATUS bosCritSectLeave( BOS_CRITSECT *critSect );

/** @} */

#if defined( __cplusplus )
}
#endif

#endif /* BOSCRITSECT_H */

