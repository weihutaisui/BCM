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
*****************************************************************************/
/**
*
*  @file    bosTypes.h
*
*  @brief   BOS definitions which are not OS-specific
*
*  This file contains constants, types, etc, which are not OS-specific.
*
****************************************************************************/

#ifndef BOSTYPES_H
#define BOSTYPES_H

#include <bosCfg.h>     /* include application-specific BOS config file */

#if BOS_OS_ECOS
#  include <../eCos/bosTypesECOS.h>
#elif BOS_OS_NUCLEUS
#  include <../Nucleus/bosTypesNucleus.h>
#elif BOS_OS_PSOS
#  include <../pSOS/bosTypesPSOS.h>
#elif BOS_OS_VXWORKS
#  include <../VxWorks/bosTypesVxWorks.h>
#elif BOS_OS_WIN32
#  include <../Win32/bosTypesWin32.h>
#elif BOS_OS_LINUXKERNEL
#  include <../LinuxKernel/bosTypesLinuxKernel.h>
#elif BOS_OS_LINUXUSER
#  include <../LinuxUser/bosTypesLinuxUser.h>
#else
#  error Unrecognized OS
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
** General
*/

/* Simple types */
typedef unsigned char   BOS_UINT8;     /** platform may not support 8-bit physical representation */
typedef signed char     BOS_SINT8;     /** platform may not support 8-bit physical representation */
typedef unsigned short  BOS_UINT16;    /** BOS_UINT16 is platform independant */
typedef signed short    BOS_SINT16;    /** BOS_SINT16 is platform independant */
typedef unsigned long   BOS_UINT32;    /** UINT32 is platform independant */
typedef long            BOS_SINT32;    /** SINT32 is platform independant */
typedef unsigned long long BOS_UINT64; /** UINT64 is platform independant */
typedef long long       BOS_SINT64;    /** SINT64 is platform independant */


/* Boolean */
typedef  int   BOS_BOOL;               /** BOS_TRUE or BOS_FALSE */
#define BOS_TRUE  1
#define BOS_FALSE 0

/**
 * BOS API return codes
 */

typedef enum
{
   BOS_STATUS_OK = 0,   /**< Sucessful    */
   BOS_STATUS_TIMEOUT,  /**< Timeout      */
   BOS_STATUS_ERR,      /**< Failed       */
   BOS_STATUS_DEBUG,    /**< Interrupted by a signal in debug group */
   BOS_STATUS_EXIT,     /**< Interrupted by a signal in exit group */
   BOS_STATUS_IO,       /**< Interrupted by a signal in io group */

   /** For socket operations, indicates that a send or recv would block on
       a non-blocking socket.                                              */
   BOS_STATUS_WOULD_BLOCK,

   BOS_STATUS_RESET,
} BOS_STATUS;

/** BOS modules enum for use error handler */
typedef enum
{
   BOS_EVENT_MODULE,
   BOS_MUTEX_MODULE,
   BOS_SEM_MODULE,
   BOS_SLEEP_MODULE,
   BOS_SOCKET_MODULE,
   BOS_TASK_MODULE,
   BOS_TIME_MODULE,
   BOS_MSGQ_MODULE,
   BOS_CRITSECT_MODULE,
   BOS_TIMER_MODULE,
   BOS_FILE_MODULE,

   BOS_NUM_MODULES

} BOS_MODULE;

/** Type for storing function ID's in. */
typedef BOS_UINT8 BOS_FUNCTION_ID;

/**
 * BOS_ARRAY_LEN can be used on arrays to determine their length. It doesn't
 * work with pointers.
 *
 * Different compilers return different signs for the array length, so if
 * you want your code to compile on different platforms you'll probably need
 * to use the _U or _I versions.
 */

#define  BOS_ARRAY_LEN( array )     ( sizeof( array ) / sizeof( array[ 0 ] ))
#define  BOS_ARRAY_LEN_U( array )   ((unsigned)BOS_ARRAY_LEN( array ))
#define  BOS_ARRAY_LEN_I( array )   ((int)BOS_ARRAY_LEN( array ))

#define  BOS_INLINE  static inline

/*
*****************************************************************************
** FUNCTION PROTOTYPES
*****************************************************************************
*/

#ifdef          __cplusplus
}                               /* End of C declarations */
#endif

#endif   /* BOSTYPES_H */
