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
*  @file    bosError.h
*
*  @brief   Contains the BOS Error interface.
*
****************************************************************************/
/**
*  @defgroup   bosError   BOS Error
*
*  @brief      Common error handling routines for BOS modules.
*
*  The BOS Error module provides helper routines used by other BOS modules.
*  It allows assertion handlers, and other error handlers to be provided.
*
****************************************************************************/

#if !defined( BOSERROR_H )
#define BOSERROR_H            /**< Include Guard                           */

/* ---- Include Files ---------------------------------------------------- */

#if !defined( BOSTYPES_H )
#  include "bosTypes.h"
#endif

#if !defined( BOSCFG_H )
#  include "bosCfg.h"
#endif

#include <stdio.h>   /* For printf, size_t                                 */
#include <stdlib.h>  /* For abort                                          */
#include <assert.h>  /* For assert                                         */

#if defined( __cplusplus )
extern "C"
{
#endif

/**
 * @addtogroup bosError
 * @{
 */

/* ---- Constants and Types ---------------------------------------------- */

/** General error typedef */
typedef enum
{
   DUMMY_VALUE
} BOS_ERR_ID;

/*
 * Define BOS_CFG_ERROR_NOASSERT in bosCfg.h if you'd like all of the BOS_ASSERT's
 * to be compiled out.
 *
 */

#if BOS_CFG_ERROR_NOASSERT

   /*
    * NOTE: BOS_ASSERT, like the ANSI C assert, is designed so that it can be
    *       used in a comma expression.
    *
    * So doing something like:
    *
    *    x = BOS_ASSERT( something ), CallAFunc();
    *
    * is perfectly legal and needs to compile when BOS_ASSERT is disabled.
    */

#  define   BOS_ASSERT(exp)   ((void)0)
#else
#  define   BOS_ASSERT(exp)   (void)(assert(exp))
#endif



/**
 * Used to identify the source (function) which caused an error. This is
 * typically used to aid in debugging.
 */

typedef  BOS_UINT32  BOS_ERROR_SOURCE;

/**
 * Combine a module and function ID together to create an error source.
 */

#define  BOS_MAKE_ERROR_SOURCE( module,  funcId ) (( (module) << 8 ) | funcId )

/**
 * Given an error source, determine which module the error source identifies.
 */

#define  BOS_ERROR_MODULE( errSrc )    ((BOS_MODULE)(( (errSrc) >> 8 ) & 0x00FF ))

/**
 * Given an error source, determine which function the error source identifies.
 */

#define  BOS_ERROR_FUNC_ID( errSrc )   ((BOS_FUNCTION_ID)( (errSrc) & 0x00FF ))

/* ---- Variable Externs ------------------------------------------------- */
/* ---- Function Prototypes ---------------------------------------------- */

BOS_STATUS bosErrorInit( void );
BOS_STATUS bosErrorTerm( void );

#if BOS_CFG_ERROR_FUNCTION_STRINGS
BOS_STATUS bosErrorAddFunctionStrings( BOS_MODULE moduleId, const char **funcEntry, int numFuncIds );
#endif

void bosErrorFunctionStr( BOS_ERROR_SOURCE errSrc, char *outStr, size_t maxLen );

/** @} */

#if defined( __cplusplus )
}
#endif

#endif /* BOSERROR_H */

