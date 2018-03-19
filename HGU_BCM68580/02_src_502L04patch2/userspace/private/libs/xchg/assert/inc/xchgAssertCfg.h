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
*  @file    xchgAssertCfg.h
*
*  @brief   ASSERT configuration file
*
*  ASSERT default application configuration file
*
****************************************************************************/

#ifndef XCHG_ASSERT_CFG_H
#define XCHG_ASSERT_CFG_H

#include <xchgAssertCfgCustom.h>

#ifndef XCHG_ASSERT_CFG_LOG
   #include <stdio.h>   /* For printf, size_t                                 */
#endif

#ifndef XCHG_ASSERT_CFG_ABORT
   #include <stdlib.h>  /* For abort                                          */
#endif

/***************************************************************************/
/**
 * Set XCHG_ASSERT_CFG_NOASSERT to 1 if you wish to have XCHG_ASSERT's compiled out.
 */

#ifndef XCHG_ASSERT_CFG_NOASSERT
#define XCHG_ASSERT_CFG_NOASSERT    0
#endif


#ifndef XCHG_ASSERT_CFG_ABORT
#define XCHG_ASSERT_CFG_ABORT       abort
#endif


#ifndef XCHG_ASSERT_CFG_LOG
#define XCHG_ASSERT_CFG_LOG(exp)    printf exp;
#endif



#endif   /* XCHG_ASSERT_CFG_H */
