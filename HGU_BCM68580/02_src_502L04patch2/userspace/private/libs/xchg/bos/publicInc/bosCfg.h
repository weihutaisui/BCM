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
*****************************************************************************
*
*  @file    bosCfg.h
*
*  @brief   BOS configuration file
*
*  BOS default application configuration file
*
****************************************************************************/

#ifndef BOSCFG_H
#define BOSCFG_H

#include <bosCfgCustom.h>


/* ---------------------------------------------------------------------------
** This is provided for backwards compatibility. Previously, the make environment
** used to create the macro BOS_OS_XXX (all upper-case), as follows:
**
**    -DBOS_OS_$(shell $(ECHO) $(XCHG_OS) | $(TR) 'a-z' 'A-Z')=1
**
** This is problematic because if multiple build environments are used (with
** different versions of cygwin), the version of 'sh.exe' and 'tr.exe' may
** not be compatible, and will result in build errors.
**
** Therefore, the macro is simply defined as follows:
**
**    -DBOS_OS_$(XCHG_OS)  (mixed-case)
**
** and the mapping below to upper-case is provided instead.
*/

#ifndef BOS_OS_ECOS
   #define BOS_OS_ECOS        BOS_OS_eCos
#endif

#ifndef BOS_OS_LINUXKERNEL
   #define BOS_OS_LINUXKERNEL BOS_OS_LinuxKernel
#endif

#ifndef BOS_OS_LINUXUSER
   #define BOS_OS_LINUXUSER   BOS_OS_LinuxUser
#endif

#ifndef BOS_OS_NONE
   #define BOS_OS_NONE        BOS_OS_None
#endif

#ifndef BOS_OS_PSOS
   #define BOS_OS_PSOS        BOS_OS_pSOS
#endif

#ifndef BOS_OS_VXWORKS
   #define BOS_OS_VXWORKS     BOS_OS_VxWorks
#endif

#ifndef BOS_OS_WIN32
   #define BOS_OS_WIN32       BOS_OS_Win32
#endif

#ifndef BOS_OS_ZSP
   #define BOS_OS_ZSP         BOS_OS_Zsp
#endif



/*
 * Use ***'s to separate the various modules.
 * Use ---'s or ==='s to separate major things within a module.
 */

/***************************************************************************/
/**
 * Set BOS_CFG_ERROR_NOASSERT to 1 if you wish to have BOS_ASSERT's compiled out.
 */

#ifndef BOS_CFG_ERROR_NOASSERT
#define BOS_CFG_ERROR_NOASSERT   0
#endif

/**
 * Determines if extra memory will be allocated to hold tables to convert
 * function ID's from their numberic form into a human readable ASCIi form.
 */

#ifndef BOS_CFG_ERROR_FUNCTION_STRINGS
#define BOS_CFG_ERROR_FUNCTION_STRINGS 1
#endif

/***************************************************************************/
/**
 * Set BOS_CFG_INIT to 1 in <bosCfgCustom.h> if INIT module support is
 * desired.
 */

#ifndef BOS_CFG_INIT
#define BOS_CFG_INIT          0
#endif

/***************************************************************************/
/**
 * Set BOS_CFG_MUTEX to 1 in <bosCfgCustom.h> if MUTEX module support is
 * desired.
 */

#ifndef BOS_CFG_MUTEX
#define BOS_CFG_MUTEX         0
#endif


/***************************************************************************/
/**
 * @page bosOs BOS OS Selection
 *
 * One of the following macros must be defined in the bosCfgCustom.h file
 * to determine which OS BOS is being configured for.
 *
 * - BOS_OS_LINUXKERNEL
 * - BOS_OS_LINUXUSER
 * - BOS_OS_NUCLEUS
 * - BOS_OS_PSOS
 * - BOS_OS_VXWORKS
 * - BOS_OS_WIN32
 * - BOS_OS_ECOS
 */

/***************************************************************************/
/**
 * Set BOS_CFG_SLEEP to 1 in <bosCfgCustom.h> if SLEEP module support is
 * desired.
 */

#ifndef BOS_CFG_SLEEP
#define BOS_CFG_SLEEP        0
#endif

/***************************************************************************/
/**
 * Set BOS_CFG_SEM to 1 in <bosCfgCustom.h> if SEM module support is
 * desired.
 */

#ifndef BOS_CFG_SEM
#define BOS_CFG_SEM          0
#endif

/*-------------------------------------------------------------------------
 * Semaphore count
 */
#ifndef BOS_CFG_SEM_INIT_COUNT
#define BOS_CFG_SEM_INIT_COUNT      0
#endif

#ifndef BOS_CFG_SEM_MAX_COUNT
#define BOS_CFG_SEM_MAX_COUNT       256
#endif

/***************************************************************************/
/**
 * Set BOS_CFG_SPINLOCK to 1 in <bosCfgCustom.h> if SPINLOCK module support is
 * desired.
 */

#ifndef BOS_CFG_SPINLOCK
#define BOS_CFG_SPINLOCK          0
#endif

#if BOS_CFG_SPINLOCK
   #if !( BOS_OS_LINUXKERNEL )
      #error "I'm confused, spinlocks should not be supported for this OS!"
   #endif
#endif


/***************************************************************************/
/**
 * Set BOS_CFG_SOCKET to 1 in <bosCfgCustom.h> if SOCKET module support is
 * desired.
 */

#ifndef BOS_CFG_SOCKET
#define BOS_CFG_SOCKET       0
#endif

#ifndef  BOS_CFG_IP_ADDRESS
#define  BOS_CFG_IP_ADDRESS   BOS_CFG_SOCKET
#endif

/***************************************************************************/
/**
 * Set BOS_CFG_FILE to 1 in <bosCfgCustom.h> if FILE support is
 * desired.
 */

#ifndef BOS_CFG_FILE
#define BOS_CFG_FILE       0
#endif

/*-------------------------------------------------------------------------
 * Set BOS_CFG_SOCKET_INTERFACE_SUPPORT to 1 in <bosCfgCustom.h> if interface
 * support is desired for the SOCKET module.
 */

 #ifndef BOS_CFG_SOCKET_INTERFACE_SUPPORT
 #define BOS_CFG_SOCKET_INTERFACE_SUPPORT 0
 #endif

/***************************************************************************/
/**
 * Set BOS_CFG_MSGQ to 1 in <bosCfgCustom.h> if MSGQ module support is
 * desired.
 */

#ifndef BOS_CFG_MSGQ
#define BOS_CFG_MSGQ         0
#endif

/***************************************************************************/
/**
 * Set BOS_CFG_EVENT to 1 in <bosCfgCustom.h> if EVENT module support is
 * desired.
 */

#ifndef BOS_CFG_EVENT
#define BOS_CFG_EVENT        0
#endif

/***************************************************************************/
/**
 * Set BOS_CFG_TIMER to 1 in <bosCfgCustom.h> if TIMER module support is
 * desired.
 */

#ifndef BOS_CFG_TIMER
#define BOS_CFG_TIMER        0
#endif

#if BOS_CFG_TIMER
   #if !( BOS_OS_WIN32 || BOS_OS_ECOS || BOS_OS_VXWORKS || BOS_OS_LINUXUSER || BOS_OS_LINUXKERNEL )
      #error "I'm confused, timers should not be supported for this OS!"
   #endif
#endif


/***************************************************************************/
/**
 * Determines the maximum allowable length of a hostname.
 */

#ifndef BOS_CFG_SOCKET_MAX_HOSTNAME_LEN
#define BOS_CFG_SOCKET_MAX_HOSTNAME_LEN   64
#endif

/***************************************************************************/
/**
*  Used to determine if the platform in
*/

#ifndef BOS_CFG_SOCKET_HAS_GETHOSTNAME
#  if ( BOS_OS_WIN32 || BOS_OS_LINUXUSER )
#     define  BOS_CFG_SOCKET_HAS_GETHOSTNAME 1
#  else
#     define  BOS_CFG_SOCKET_HAS_GETHOSTNAME 0
#  endif
#endif

/***************************************************************************/
/**
 * Set BOS_CFG_TIME to 1 in <bosCfgCustom.h> if TIME module support is
 * desired.
 */

#ifndef BOS_CFG_TIME
#define BOS_CFG_TIME         0
#endif

/***************************************************************************/
/**
 * Set BOS_CFG_TASK to 1 in <bosCfgCustom.h> if TASK module support is
 * desired.
 */

#ifndef BOS_CFG_TASK
#define BOS_CFG_TASK         0
#endif

/***************************************************************************/
/**
 * Set BOS_CFG_CRITSECT to 1 in <bosCfgCustom.h> if critical section module
 * support is desired.
 */

#ifndef BOS_CFG_CRITSECT
#define BOS_CFG_CRITSECT   0
#endif

/***************************************************************************/
/**
 * Set BOS_CFG_LOG to 1 in <bosCfgCustom.h> if LOG module support is
 * desired.
 */

#ifndef BOS_CFG_LOG
#define BOS_CFG_LOG       1
#endif

/***************************************************************************/
/**
 * Set BOS_CFG_MISCELLANEOUS to 1 in <bosCfgCustom.h> if MISCELLANEOUS module support is
 * desired.
 */

#ifndef BOS_CFG_MISCELLANEOUS
#define BOS_CFG_MISCELLANEOUS          0
#endif

#if BOS_CFG_MISCELLANEOUS
   #if !( BOS_OS_LINUXKERNEL )
      #error "I'm confused, miscellaneous should not be supported for this OS!"
   #endif
#endif


#if BOS_CFG_CRITSECT

   /*
   ** Set BOS_CFG_CRITSECT_USE_GLOBAL to 1 in <bosCfgCustom.h> if the application
   ** should use a single critical section object
   */
   #ifndef BOS_CFG_CRITSECT_USE_GLOBAL
   #define BOS_CFG_CRITSECT_USE_GLOBAL 0
   #endif

   #if BOS_CFG_CRITSECT_USE_GLOBAL

      /*
      ** Global critical section object name
      */
      #ifndef BOS_CFG_GLOBAL_CRITSECT
      #define BOS_CFG_GLOBAL_CRITSECT  gBosCritSect
      #endif

   #endif

#endif


#if BOS_CFG_TASK

   #ifndef BOS_CFG_APP_TASK_PRIORITY
   #define BOS_CFG_APP_TASK_PRIORITY   BOS_TASK_CLASS_MED
   #endif

   /*
   ** Set BOS_CFG_RESET_SUPPORT to 1 in <bosCfgCustom.h> to enable application
   ** reset support.
   */
   #ifndef BOS_CFG_RESET_SUPPORT
   #define BOS_CFG_RESET_SUPPORT 0
   #endif

   /*
   ** Set BOS_CFG_RESET_RESURRECT_SUPPORT to 0 in <bosCfgCustom.h> to disable 
   ** reset resurrect support.
   **
   ** Resurrect support is a feature part of the original Reset support. 
   **
   ** When the feature is enabled, application task will not exit after reset. 
   ** It can be either restarted (resurrected) by bosTaskCreateR, or 
   ** destroyed by bosTaskDestoryR.
   **
   ** Note 1: 
   **    By default the feature is enabled for backward compatibility reason.
   **    If restarting of task is not needed, this feature can be disabled.
   ** Note 2: 
   **    Either the feature is enabled or disabled, bosTaskDestroyR must 
   **    always be called to fully clean up the task table.
   */
   #ifndef BOS_CFG_RESET_RESURRECT_SUPPORT
   #define BOS_CFG_RESET_RESURRECT_SUPPORT       1
   #endif


   #if BOS_CFG_RESET_SUPPORT

      /*
      ** The reset granularity is the rate at which blocking OS calls check if
      ** an application reset has been initiated.
      */
      #ifndef BOS_CFG_RESET_GRANULARITY
      #define BOS_CFG_RESET_GRANULARITY   500
      #endif
      
      /* 
      ** Do not use high resolution time for larger timeout, use accumulative 
      ** decrement 
      */
      #ifndef BOS_CFG_TIME_FOR_TIMED_FUNCTION
      #define BOS_CFG_TIME_FOR_TIMED_FUNCTION   0
      #endif
   #endif
#endif



/*-------------------------------------------------------------------------
 * Task priority definitions
 */

/** @def  BOS_CFG_TASK_HIGH_VAL     OS task priority for High priority BOS Tasks          */
/** @def  BOS_CFG_TASK_MED_HIGH_VAL OS task priority for Medium-High priority BOS Tasks   */
/** @def  BOS_CFG_TASK_MED_VAL      OS task priority for Medium priority BOS Tasks        */
/** @def  BOS_CFG_TASK_MED_LOW_VAL  OS task priority for Medium-Low priority BOS Tasks    */
/** @def  BOS_CFG_TASK_LOW_VAL      OS task priority for Low priority BOS Tasks           */

#if BOS_CFG_TASK
#  if BOS_OS_ECOS

      /**
       * Default priority levels in eCos is 0-31, where a smaller number
       * corresponds to a higher priority of the thread
       */

#     ifndef BOS_CFG_TASK_HIGH_VAL
#     define BOS_CFG_TASK_HIGH_VAL       17     /* highest value that POSIX application threads can use */
#     endif
#     ifndef BOS_CFG_TASK_MED_HIGH_VAL
#     define BOS_CFG_TASK_MED_HIGH_VAL   20
#     endif
#     ifndef BOS_CFG_TASK_MED_VAL
#     define BOS_CFG_TASK_MED_VAL        23
#     endif
#     ifndef BOS_CFG_TASK_MED_LOW_VAL
#     define BOS_CFG_TASK_MED_LOW_VAL    26
#     endif
#     ifndef BOS_CFG_TASK_LOW_VAL
#     define BOS_CFG_TASK_LOW_VAL        31
#     endif

      /**
       * eCos specific options for task creation (cyg_thread_create)
       */

#     ifndef BOS_CFG_TASK_ECOS_DEFAULT_STACK_SIZE
#     define BOS_CFG_TASK_ECOS_DEFAULT_STACK_SIZE  ( 4 * 1024 )
#     endif

      /**
       * eCos specific options for mutex creation (cyg_mutex_init).
       */

#     ifndef BOS_CFG_ECOS_MUTEX_OPTIONS
#     define BOS_CFG_ECOS_MUTEX_OPTIONS  NULL
#     endif

      /**
       * eCos specific options for semaphore creation (cyg_semaphore_init).
       */

#     ifndef BOS_CFG_ECOS_SEM_OPTIONS
#     define BOS_CFG_ECOS_SEM_OPTIONS    NULL
#     endif

#  elif ( BOS_OS_LINUXKERNEL || BOS_OS_LINUXUSER )

#     ifndef BOS_CFG_TASK_HIGH_VAL
#     define BOS_CFG_TASK_HIGH_VAL       99
#     endif
#     ifndef BOS_CFG_TASK_MED_HIGH_VAL
#     define BOS_CFG_TASK_MED_HIGH_VAL   90
#     endif
#     ifndef BOS_CFG_TASK_MED_VAL
#     define BOS_CFG_TASK_MED_VAL        85
#     endif
#     ifndef BOS_CFG_TASK_MED_LOW_VAL
#     define BOS_CFG_TASK_MED_LOW_VAL    80
#     endif
#     ifndef BOS_CFG_TASK_LOW_VAL
#     define BOS_CFG_TASK_LOW_VAL        75
#     endif

#     ifndef BOS_CFG_OS_HAS_COMMAND_LINE
#     define BOS_CFG_OS_HAS_COMMAND_LINE  1
#     endif

      /**
       * Linux specific options for task creation (taskSpawn)
       */

#     ifndef BOS_CFG_TASK_LINUX_DEFAULT_SPAWN_OPTIONS
#     define BOS_CFG_TASK_LINUX_DEFAULT_SPAWN_OPTIONS 0
#     endif

#     ifndef BOS_CFG_TASK_LINUX_DEFAULT_STACK_SIZE
#     define BOS_CFG_TASK_LINUX_DEFAULT_STACK_SIZE  ( 4 * 1024 )
#     endif

#  elif BOS_OS_NUCLEUS

#     error NUCLEUS task priorities need to be initialized

#     ifndef BOS_CFG_TASK_HIGH_VAL
#     define BOS_CFG_TASK_HIGH_VAL       0
#     endif
#     ifndef BOS_CFG_TASK_MED_HIGH_VAL
#     define BOS_CFG_TASK_MED_HIGH_VAL   0
#     endif
#     ifndef BOS_CFG_TASK_MED_VAL
#     define BOS_CFG_TASK_MED_VAL        0
#     endif
#     ifndef BOS_CFG_TASK_MED_LOW_VAL
#     define BOS_CFG_TASK_MED_LOW_VAL    0
#     endif
#     ifndef BOS_CFG_TASK_LOW_VAL
#     define BOS_CFG_TASK_LOW_VAL        0
#     endif

#  elif BOS_OS_PSOS

#     ifndef BOS_CFG_TASK_HIGH_VAL
#     define BOS_CFG_TASK_HIGH_VAL       255
#     endif
#     ifndef BOS_CFG_TASK_MED_HIGH_VAL
#     define BOS_CFG_TASK_MED_HIGH_VAL   192
#     endif
#     ifndef BOS_CFG_TASK_MED_VAL
#     define BOS_CFG_TASK_MED_VAL        128
#     endif
#     ifndef BOS_CFG_TASK_MED_LOW_VAL
#     define BOS_CFG_TASK_MED_LOW_VAL    64
#     endif
#     ifndef BOS_CFG_TASK_LOW_VAL
#     define BOS_CFG_TASK_LOW_VAL        10
#     endif

      /**
       * PSOS specific options for task creation (t_create).
       */

#     ifndef BOS_CFG_PSOS_TASK_DEFAULT_STACK_SIZE
#     define BOS_CFG_PSOS_TASK_DEFAULT_STACK_SIZE  ( 4 * 1024 )
#     endif

#     ifndef BOS_CFG_PSOS_TASK_FLAGS
#     define BOS_CFG_PSOS_TASK_FLAGS     ( T_LOCAL | T_NOFPU )
#     endif

      /**
       * PSOS specific options for task initiation (t_start).
       */

#     ifndef BOS_CFG_PSOS_TASK_OPTIONS
#     define BOS_CFG_PSOS_TASK_OPTIONS   ( T_SUPV | T_PREEMPT | T_NOTSLICE | T_ISR )
#     endif

      /**
       * PSOS specific options for mutex creation (mu_create).
       */

#     ifndef BOS_CFG_PSOS_MUTEX_OPTIONS
#     define BOS_CFG_PSOS_MUTEX_OPTIONS   ( MU_LOCAL | MU_PRIOR | MU_RECURSIVE )
#     endif

      /**
       * PSOS specific options for semaphore creation (sm_create).
       */

#     ifndef BOS_CFG_PSOS_SEM_OPTIONS
#     define BOS_CFG_PSOS_SEM_OPTIONS     ( SM_LOCAL | SM_PRIOR | SM_UNBOUNDED )
#     endif

      /**
       * PSOS specific options for message queue creation (q_create).
       */

#     ifndef BOS_CFG_PSOS_MSGQ_OPTIONS
#     define BOS_CFG_PSOS_MSGQ_OPTIONS     ( Q_LOCAL | Q_LIMIT | Q_PRIBUF | Q_FIFO )
#     endif

#  elif BOS_OS_VXWORKS

#     ifndef BOS_CFG_TASK_HIGH_VAL
#     define BOS_CFG_TASK_HIGH_VAL       25
#     endif
#     ifndef BOS_CFG_TASK_MED_HIGH_VAL
#     define BOS_CFG_TASK_MED_HIGH_VAL   64
#     endif
#     ifndef BOS_CFG_TASK_MED_VAL
#     define BOS_CFG_TASK_MED_VAL        128
#     endif
#     ifndef BOS_CFG_TASK_MED_LOW_VAL
#     define BOS_CFG_TASK_MED_LOW_VAL    192
#     endif
#     ifndef BOS_CFG_TASK_LOW_VAL
#     define BOS_CFG_TASK_LOW_VAL        245
#     endif

      /**
       * VxWorks specific options for task creation (taskSpawn)
       */

#     ifndef BOS_CFG_TASK_VXWORKS_DEFAULT_SPAWN_OPTIONS
#     define BOS_CFG_TASK_VXWORKS_DEFAULT_SPAWN_OPTIONS 0
#     endif

#     ifndef BOS_CFG_TASK_VXWORKS_DEFAULT_STACK_SIZE
#     define BOS_CFG_TASK_VXWORKS_DEFAULT_STACK_SIZE  ( 4 * 1024 )
#     endif

      /**
       * VxWorks specific options for mutex creation (semMCreate).
       */

#     ifndef BOS_CFG_VXWORKS_MUTEX_OPTIONS
#     define BOS_CFG_VXWORKS_MUTEX_OPTIONS   ( SEM_Q_PRIORITY | SEM_DELETE_SAFE | SEM_INVERSION_SAFE )
#     endif

      /**
       * VxWorks specific options for semaphore creation (sm_create).
       */

#     ifndef BOS_CFG_VXWORKS_SEM_OPTIONS
#     define BOS_CFG_VXWORKS_SEM_OPTIONS     ( SEM_Q_PRIORITY )
#     endif

      /**
       * VxWorks specific options for message queue creation (msgQCreate).
       */

#     ifndef BOS_CFG_VXWORKS_MSGQ_OPTIONS
#     define BOS_CFG_VXWORKS_MSGQ_OPTIONS    MSG_Q_FIFO
#     endif

#  elif BOS_OS_WIN32

#     ifndef BOS_CFG_TASK_HIGH_VAL
#     define BOS_CFG_TASK_HIGH_VAL       THREAD_PRIORITY_HIGHEST
#     endif
#     ifndef BOS_CFG_TASK_MED_HIGH_VAL
#     define BOS_CFG_TASK_MED_HIGH_VAL   THREAD_PRIORITY_ABOVE_NORMAL
#     endif
#     ifndef BOS_CFG_TASK_MED_VAL
#     define BOS_CFG_TASK_MED_VAL        THREAD_PRIORITY_NORMAL
#     endif
#     ifndef BOS_CFG_TASK_MED_LOW_VAL
#     define BOS_CFG_TASK_MED_LOW_VAL    THREAD_PRIORITY_BELOW_NORMAL
#     endif
#     ifndef BOS_CFG_TASK_LOW_VAL
#     define BOS_CFG_TASK_LOW_VAL        THREAD_PRIORITY_LOWEST
#     endif

#     ifndef BOS_CFG_OS_HAS_COMMAND_LINE
#     define BOS_CFG_OS_HAS_COMMAND_LINE  1
#     endif

      /**
       * WIN32 specific options for mutex creation (CreateMutex).
       */

#     ifndef BOS_CFG_WIN32_MUTEX_OPTIONS
#     define BOS_CFG_WIN32_MUTEX_OPTIONS  NULL
#     endif

      /**
       * WIN32 specific options for semaphore creation (CreateSemaphore).
       */

#     ifndef BOS_CFG_WIN32_SEM_OPTIONS
#     define BOS_CFG_WIN32_SEM_OPTIONS    NULL
#     endif

#  else

#     error No BOS_OS_xxx defined - must define on the command line (done with make).

#  endif /* BOS_OS_xxx */
#endif   /* BOS_CFG_TASK   */

#ifndef BOS_CFG_OS_HAS_COMMAND_LINE
#define BOS_CFG_OS_HAS_COMMAND_LINE 0
#endif


/*-------------------------------------------------------------------------
 * BSD socket compatibility mode.
 */

#if BOS_CFG_SOCKET
   # if BOS_OS_ECOS
      #ifndef BOS_CFG_SOCKET_BSD_4_3
      #define BOS_CFG_SOCKET_BSD_4_3    0
      #endif
      #ifndef BOS_CFG_SOCKET_BSD_4_4
      #define BOS_CFG_SOCKET_BSD_4_4    1
      #endif
   # elif ( BOS_OS_LINUXKERNEL || BOS_OS_LINUXUSER )
      #ifndef BOS_CFG_SOCKET_BSD_4_3
      #define BOS_CFG_SOCKET_BSD_4_3    1
      #endif
      #ifndef BOS_CFG_SOCKET_BSD_4_4
      #define BOS_CFG_SOCKET_BSD_4_4    0
      #endif
   # elif BOS_OS_PSOS
      #ifndef BOS_CFG_SOCKET_BSD_4_3
      #define BOS_CFG_SOCKET_BSD_4_3    0
      #endif
      #ifndef BOS_CFG_SOCKET_BSD_4_4
      #define BOS_CFG_SOCKET_BSD_4_4    1
      #endif
   # elif BOS_OS_WIN32
      #ifndef BOS_CFG_SOCKET_BSD_4_3
      #define BOS_CFG_SOCKET_BSD_4_3    1
      #endif
      #ifndef BOS_CFG_SOCKET_BSD_4_4
      #define BOS_CFG_SOCKET_BSD_4_4    0
      #endif
   # elif BOS_OS_VXWORKS
      #ifndef BOS_CFG_SOCKET_BSD_4_3
      #define BOS_CFG_SOCKET_BSD_4_3    0
      #endif
      #ifndef BOS_CFG_SOCKET_BSD_4_4
      #define BOS_CFG_SOCKET_BSD_4_4    1
      #endif
   # else
      # error No BOS_OS_xxx defined - must define on the command line (done with make).
   # endif /* BOS_OS_xxx */

   #if (BOS_CFG_SOCKET_BSD_4_3 && BOS_CFG_SOCKET_BSD_4_4)
      #error "Can't enable both BSD 4.3 and 4.4!"
   #endif

#endif   /* BOS_CFG_SOCKET */




/*-------------------------------------------------------------------------
 * Figure out endianess
 */

/**
 * @def  BOS_CFG_LITTLE_ENDIAN   will have a value of 1 for little endian
 *                               machines.
 *
 * @def  BOS_CFG_BIG_ENDIAN      will have a value of 1 for big endian
 *                               machines.
 */

#if BOS_OS_ECOS

   /*
    * The following header file needs to be included to ensure that BYTE_ORDER
    * is set properly by the eCos header files.
    */

#include <cyg/hal/basetype.h>

#  if !defined( BOS_CFG_LITTLE_ENDIAN ) && !defined( BOS_CFG_BIG_ENDIAN )
#     if CYG_BYTEORDER == CYG_MSBFIRST
#        define   BOS_CFG_BIG_ENDIAN      1
#     else
#        define   BOS_CFG_LITTLE_ENDIAN   1
#     endif
#  endif


   /*
    * If we ever support a little endian eCos distribution then the
    * following test will need to be removed.
    */

#  if BOS_CFG_LITTLE_ENDIAN
#     error "I'm confused: MIPS-eCos should be BIG_ENDIAN"
#  endif

#elif ( BOS_OS_LINUXKERNEL || BOS_OS_LINUXUSER )

#   if !defined( BOS_CFG_LITTLE_ENDIAN ) && !defined( BOS_CFG_BIG_ENDIAN )
#      if defined ( __BYTE_ORDER__ )
#         if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#            define BOS_CFG_BIG_ENDIAN       1
#         elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#            define BOS_CFG_LITTLE_ENDIAN   1
#         else
#            error "GCC __BYTE_ORDER__ flag not set!"
#         endif
#      else
#         error "GCC __BYTE_ORDER__ flag not defined!"
#      endif
#   endif

#elif BOS_OS_NUCLEUS

#elif BOS_OS_PSOS

#   if !defined( BOS_CFG_LITTLE_ENDIAN ) && !defined( BOS_CFG_BIG_ENDIAN )

       /*
        * The only references to endianness that I could find in the pSOS
        * header files was BSP_LITTLE_ENDIAN, so that's what I use here.
        */

#      if BSP_LITTLE_ENDIAN
#         define   BOS_CFG_LITTLE_ENDIAN   1
#      else
#         define   BOS_CFG_BIG_ENDIAN      1
#      endif
#   endif

   /*
    * If we ever support a little endian pSOS distribution then the
    * following test will need to be removed.
    */

#  if BOS_CFG_LITTLE_ENDIAN
#     error "I'm confused: MIPS-pSOS should be BIG_ENDIAN"
#  endif

#elif BOS_OS_VXWORKS

   /*
    * The following header file needs to be included to ensure that _BYTE_ORDER
    * is set properly by the VxWorks header files.
    */

#if !defined( __INCvxWorksh )
#  include <vxWorks.h>
#endif

#   if !defined( BOS_CFG_LITTLE_ENDIAN ) && !defined( BOS_CFG_BIG_ENDIAN )
#      if ( _BYTE_ORDER == _LITTLE_ENDIAN )
#         define   BOS_CFG_LITTLE_ENDIAN   1
#         define   BOS_CFG_BIG_ENDIAN      0
#      else
#         define   BOS_CFG_LITTLE_ENDIAN   0
#         define   BOS_CFG_BIG_ENDIAN      1
#      endif
#   endif

   /*
    * If we ever support a little endian VxWorks distribution then the
    * following test will need to be removed.
    */

#  if BOS_CFG_LITTLE_ENDIAN
#     error "I'm confused: MIPS-VxWorks should be BIG_ENDIAN"
#  endif

#elif BOS_OS_WIN32

#   if !defined( BOS_CFG_LITTLE_ENDIAN ) && !defined( BOS_CFG_BIG_ENDIAN )
#      define BOS_CFG_LITTLE_ENDIAN     1
#   endif

#elif BOS_OS_NONE

   /*
   ** "None" is a valid operating system. However, in this case, there is
   ** no mechanism to determine the endian type of the processor. It is
   ** upto the user to specify this in the bosCfgCustom.h
   */

#else

#     error No BOS_OS_xxx defined - must define on the command line (done with make).

#endif   /* BOS_OS_xxx for endianess */


/**
 * For OS's that require tables to store per-task information,
 * BOS_CFG_TASK_MAX_TASKS is used to specify the number of entries in the table.
 */

#ifndef BOS_CFG_TASK_MAX_TASKS
#define BOS_CFG_TASK_MAX_TASKS    16
#endif

/**
 * For OS's that don't associate a name with a task, the name needs to be
 * stored in a table. This specifies the maximum length of task name that
 * the calling application will use.
 */

#ifndef BOS_CFG_TASK_MAX_TASK_NAME_LEN
#define BOS_CFG_TASK_MAX_TASK_NAME_LEN    32
#endif

/***************************************************************************/
/**
 * For OS's that do not use native support of message queues,
 * BOS_CFG_MSGQ_MAX_MSGQS is used to specify the maximum number of message
 * queues in the OS-independent message queue implementation.
 */
#ifndef BOS_CFG_MSGQ_MAX_MSGQS
#define BOS_CFG_MSGQ_MAX_MSGQS            4
#endif

/**
 * For OS's that do not use native support of message queues,
 * BOS_CFG_MSGQ_MAX_MSG_PER_MSGQ is used to specify the maximum number of
 * messages per message queue in the OS-independent message queue
 * implementation.
 */
#ifndef BOS_CFG_MSGQ_MAX_MSG_PER_MSGQ
#define BOS_CFG_MSGQ_MAX_MSG_PER_MSGQ     20
#endif

/**
 * Register each task created by BOS to extern application
**/
#ifndef BOS_CFG_REGISTER_THREAD_INFO
#define BOS_CFG_REGISTER_THREAD_INFO       0
#endif
/***************************************************************************/
/**
 * Determine the number of milliseconds in each clock tick.  The BOS utility
 * module provides an interface to access system calls to retrieve the clock
 * tick.  For OS's that support such system calls, the BOS utility module must
 * be enabled (BOS_CFG_UTIL).  For all other OS's, hardcoded tick rate should
 * be used instead.
*/

#if BOS_OS_ECOS

   #ifndef BOS_CFG_UTIL
   #define BOS_CFG_UTIL       1
   #endif

#endif

#if BOS_OS_VXWORKS

   #ifndef BOS_CFG_UTIL
   #define BOS_CFG_UTIL       1
   #endif

#endif

#if BOS_OS_PSOS

   #ifndef BOS_CFG_PSOS_MSEC_PER_TICK
   #define BOS_CFG_PSOS_MSEC_PER_TICK  10
   #endif

   #ifndef BOS_CFG_UTIL
   #define BOS_CFG_UTIL       0
   #endif

#endif

#if ( BOS_OS_LINUXKERNEL || BOS_OS_LINUXUSER )

   #ifndef BOS_CFG_LINUX_MSEC_PER_TICK
   #define BOS_CFG_LINUX_MSEC_PER_TICK  5
   #endif

   #ifndef BOS_CFG_UTIL
   #define BOS_CFG_UTIL       0
   #endif

   #ifndef BOS_MSGQ_MSG_MAX_SIZE
   #define BOS_MSGQ_MSG_MAX_SIZE       8 //5
   #endif

#endif



/* IPv6 support backwards compatibility */
#ifndef BOS_CFG_IP_ADDR_V6_SUPPORT
#define BOS_CFG_IP_ADDR_V6_SUPPORT   0
#endif

#endif   /* BOSCFG_H */
