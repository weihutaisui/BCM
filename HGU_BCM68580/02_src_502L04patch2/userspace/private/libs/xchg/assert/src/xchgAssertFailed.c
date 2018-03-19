/****************************************************************************
*
* Copyright (c) 2002 Broadcom Corporation, All Rights Reserved.
* Contains proprietary and confidential information.
*
* No portions of this material may be reproduced in any form without the
* written permission of:
*
* Broadcom Corporation
* 16215 Alton Parkway
* P.O. Box 57013
* Irvine, California 92619-7013
*
* All information contained in this document is Broadcom Corporation
* company private, proprietary, and trade secret.
*
*****************************************************************************/
/**
*
*  @file    xchgAssertFailed.c
*
*  @brief   Provides the xchgAssertFailed function.
*
*  The xchgAssertFailed function is called by the XCHG_ASSERT macro if an
*  assertion fails.
*
****************************************************************************/

/* ---- Include Files ---------------------------------------------------- */

#include <xchgAssert.h>
#include <xchgAssertCfg.h>
#include <string.h>
#include <bosCfg.h>

#if BOS_OS_LINUXKERNEL /* To remove compiler warning */
#include <bosSleep.h>
#endif 


/* ---- Public Variables ------------------------------------------------- */
/* ---- Private Constants and Types -------------------------------------- */
/* ---- Private Variables ------------------------------------------------ */

/**
 * Pointer to the function which will be called when an assertion failure
 * occurs.
 */
static XCHG_ASSERT_HANDLER gxchgAssertFailed = NULL;


/* ---- Private Function Prototypes -------------------------------------- */
#if ( XCHG_ASSERT_CFG_NOASSERT == 0 )
static void DefaultAssertFailed( const char *exprStr, const char *fileName, unsigned lineNum );
#endif

/* ---- Functions -------------------------------------------------------- */

/**
 * @addtogroup xchgAssert
 * @{
 */

#if ( XCHG_ASSERT_CFG_NOASSERT == 0 )

/***************************************************************************/
/**
*  Called when an assertion fails.
*
*  @param   exprStr  (in)  Expression which failed.
*  @param   fileName (in)  Filename containing the failed assertion.
*  @param   lineNum  (in)  Line number of the failed assertion.
*/
void xchgAssertFailed( const char *exprStr, const char *fileName, unsigned lineNum )
{
   if ( gxchgAssertFailed == NULL )
   {
      DefaultAssertFailed( exprStr, fileName, lineNum );
   }
   else
   {
      gxchgAssertFailed( exprStr, fileName, lineNum );
   }

} /* xchgAssertFailed */


/***************************************************************************/
/**
*  Called when an assertion fails.
*
*  @param   exprStr  (in)  Expression which failed.
*  @param   op1      (in)  First operand of failed assertion.
*  @param   cond     (in)  Operator of failed assertion.
*  @param   op2      (in)  Second operand of failed assertion.
*  @param   fileName (in)  Filename containing the failed assertion.
*  @param   lineNum  (in)  Line number of the failed assertion.
*/
void xchgAssertCondFailed
( 
   const char *exprStr, 
   int         op1, 
   const char *cond, 
   int         op2, 
   const char *fileName, 
   unsigned    lineNum 
)
{
   char buf[128];

   sprintf( buf, "%s ( %d %s %d )", exprStr, op1, cond, op2 );

   xchgAssertFailed( buf, fileName, lineNum );
}


/***************************************************************************/
/**
*  Default handler function which is called when an assertion fails.
*
*  This function logs the error and
*
*  @param   exprStr  (in)  Expression which failed.
*  @param   fileName (in)  Filename containing the failed assertion.
*  @param   lineNum  (in)  Line number of the failed assertion.
*/
void DefaultAssertFailed( const char *exprStr, const char *fileName, unsigned lineNum )
{
   /* Avoid compiler warning for unused variables if LOG macro is stubbed out. */
   (void) exprStr;
   (void) fileName;
   (void) lineNum;

   XCHG_ASSERT_CFG_LOG(( "Assertion failed: %s, file %s, line %d\n", exprStr, fileName, lineNum ));

   XCHG_ASSERT_CFG_ABORT();

} /* DefaultAssertFailed */

#endif /* XCHG_ASSERT_CFG_NOASSERT */


/***************************************************************************/
/**
*  Used to register a client-specific assert handler.
*
*  @param   newHandler  (in)  Assert handler to register.
*
*  @return  The old assert handler.
*/
XCHG_ASSERT_HANDLER xchgAssertSetHandler( XCHG_ASSERT_HANDLER newHandler )
{
   XCHG_ASSERT_HANDLER oldHandler = gxchgAssertFailed;

   gxchgAssertFailed = newHandler;

   return ( oldHandler );

} /* xchgAssertSetHandler */

/** @} */
