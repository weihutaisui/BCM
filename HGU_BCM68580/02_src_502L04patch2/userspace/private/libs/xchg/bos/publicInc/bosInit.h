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
*  @file    bosInit.h
*
*  @brief   Contains the BOS Init interface.
*
****************************************************************************/
/**
*  @defgroup   bosInit   BOS Initialization/Termination
*
*  @brief   Provides a centralized initialization scheme.
*
*  bosInit calls the initialization routines for all of the modules which
*  have been configured (in bosCfgCustom.h).
*
****************************************************************************/

#if !defined( BOSINIT_H )
#define BOSINIT_H             /**< Include Guard                           */

/* ---- Include Files ---------------------------------------------------- */

#if !defined( BOSCFG_H )
#  include <bosCfg.h>
#endif

#if !BOS_CFG_INIT
#error BOS Init module is not selected in bosCfg.h
#endif

#if !defined( BOSTYPES_H )
#  include <bosTypes.h>
#endif

#if defined( __cplusplus )
extern "C"
{
#endif

/**
 * @addtogroup bosInit
 * @{
 */

/* ---- Constants and Types ---------------------------------------------- */
/* ---- Variable Externs ------------------------------------------------- */
/* ---- Function Prototypes ---------------------------------------------- */

/***************************************************************************/
/**
*  Initializes configured BOS modules.
*/

BOS_STATUS bosInit( void );

/***************************************************************************/
/**
*  Terminates configured BOS modules.
*/

BOS_STATUS bosTerm( void );

/***************************************************************************/
/**
*  Checks BOS initialization status
*/
BOS_BOOL bosIsInit( void );

/** @} */

#if defined( __cplusplus )
}
#endif

#endif /* BOSINIT_H */

