/****************************************************************************
*
*     Copyright (c) 2001 Broadcom Corporation
*           All Rights Reserved
*
*     No portions of this material may be reproduced in any form without the
*     written permission of:
*
*           Broadcom Corporation
*           16215 Alton Parkway
*           P.O. Box 57013
*           Irvine, California 92619-7013
*
*     All information contained in this document is Broadcom Corporation
*     company private, proprietary, and trade secret.
*
*****************************************************************************/
/**
*
*  @file    xchgAssert.h
*
*  @brief   Contains the assert interface.
*
****************************************************************************/
/**
*  @defgroup   xchgAssert   ASSERT handler.
*
*  @brief      Common assert handling routines.
*
****************************************************************************/

#if !defined( XCHG_ASSERT_H )
#define XCHG_ASSERT_H            /**< Include Guard                           */

/* ---- Include Files ---------------------------------------------------- */

#if !defined( XCHG_ASSERT_CFG_H )
#  include <xchgAssertCfg.h>
#endif

#if defined( __cplusplus )
extern "C"
{
#endif

/**
 * @addtogroup xchgAssert
 * @{
 */

/* ---- Constants and Types ---------------------------------------------- */

/*
 * Define XCHG_ASSERT_CFG_NOASSERT in xchgAssertCfg.h if you'd like all of the
 * XCHG_ASSERT's to be compiled out.
 *
 */

#if XCHG_ASSERT_CFG_NOASSERT

   /*
    * NOTE: XCHG_ASSERT, like the ANSI C assert, is designed so that it can be
    *       used in a comma expression.
    *
    * So doing something like:
    *
    *    x = XCHG_ASSERT( something ), CallAFunc();
    *
    * is perfectly legal and needs to compile when XCHG_ASSERT is disabled.
    */

#  define   XCHG_ASSERT(exp)   ((void)0)
#else
#  ifndef XCHG_ASSERT
#     define   XCHG_ASSERT(exp)   (void)( (exp) || ( xchgAssertFailed( #exp, __FILE__, __LINE__ ), 0 ))
#  endif

   /*
    * NOTE: XCHG_ASSERT_COND is similar to XCHG_ASSERT; however, provides extra
    *       debug information when an assert occurs. Asserts that are normally
    *       written like this:
    * 
    *          XCHG_ASSERT( len > width );
    * 
    *       can be written like this instead:
    * 
    *          XCHG_ASSERT_COND( len, >, width );
    * 
    * 
    *       If/when an assert occurs, the _values_ of 'len' and 'width' will be
    *       logged in addition to the normal assert info (line, file etc).
    * 
    *       Therefore, you get asserts like the following:
    * 
    *          Assertion failed: len > width ( 0 > 2 ), file foobar.c, line 263. 
    * 
    *       instead of this:        
    * 
    *          Assertion failed: len > width , file foobar.c, line 263.
    */

#  ifndef XCHG_ASSERT_COND
#     define   XCHG_ASSERT_COND( op1, cond, op2 ) \
                  (void)( ((op1) cond (op2)) || ( xchgAssertCondFailed( #op1" "#cond" "#op2, op1, #cond, op2, __FILE__, __LINE__ ), 0 ))
#  endif
#endif


typedef void (*XCHG_ASSERT_HANDLER)( const char *exprStr, const char *fileName, unsigned lineNum );


/* ---- Variable Externs ------------------------------------------------- */
/* ---- Function Prototypes ---------------------------------------------- */

#if ( XCHG_ASSERT_CFG_NOASSERT == 0 )
void xchgAssertFailed( const char *exprStr, const char *fileName, unsigned lineNum );

void xchgAssertCondFailed
( 
   const char *exprStr, 
   int         op1, 
   const char *cond, 
   int         op2, 
   const char *fileName, 
   unsigned    lineNum 
);
#endif

XCHG_ASSERT_HANDLER xchgAssertSetHandler( XCHG_ASSERT_HANDLER newHandler );


/** @} */

#if defined( __cplusplus )
}
#endif

#endif /* XCHG_ASSERT_H */
