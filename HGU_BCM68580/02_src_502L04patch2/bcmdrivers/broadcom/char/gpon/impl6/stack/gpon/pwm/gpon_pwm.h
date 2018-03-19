/*
* <:copyright-BRCM:2013:proprietary:gpon
* 
*    Copyright (c) 2013 Broadcom 
*    All Rights Reserved
* 
*  This program is the proprietary software of Broadcom and/or its
*  licensors, and may only be used, duplicated, modified or distributed pursuant
*  to the terms and conditions of a separate, written license agreement executed
*  between you and Broadcom (an "Authorized License").  Except as set forth in
*  an Authorized License, Broadcom grants no license (express or implied), right
*  to use, or waiver of any kind with respect to the Software, and Broadcom
*  expressly reserves all rights in and to the Software and all intellectual
*  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
*  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
*  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
* 
*  Except as expressly set forth in the Authorized License,
* 
*  1. This program, including its structure, sequence and organization,
*     constitutes the valuable trade secrets of Broadcom, and you shall use
*     all reasonable efforts to protect the confidentiality thereof, and to
*     use this information only in connection with your use of Broadcom
*     integrated circuit products.
* 
*  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
*     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
*     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
*     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
*     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
*     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
*     PERFORMANCE OF THE SOFTWARE.
* 
*  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
*     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
*     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
*     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
*     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
*     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
*     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
*     LIMITED REMEDY.
* :> 
*/


#ifndef _GPON_PWM_H_
#define _GPON_PWM_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "bl_gpon_pwm_gpl.h"
#include <bdmf_system.h>

/* Parameters */

#define MS_BL_PWM_PARAM_IHOLD_SET()			( pwm_data.params.ihold_is_set		== BDMF_TRUE )
#define MS_BL_PWM_PARAM_IAWARE_SET()		( pwm_data.params.iaware_is_set		== BDMF_TRUE )
#define MS_BL_PWM_PARAM_ISLEEP_SET()		( pwm_data.params.isleep_is_set		== BDMF_TRUE )
#define MS_BL_PWM_PARAM_ITRANSINIT_SET()	( pwm_data.params.itransinit_is_set	== BDMF_TRUE )
#define MS_BL_PWM_PARAM_ITXINIT_SET()		( pwm_data.params.itxinit_is_set	== BDMF_TRUE )

#define MS_BL_PWM_PARAMS_SET() ( MS_BL_PWM_PARAM_IHOLD_SET()        && \
								 MS_BL_PWM_PARAM_IAWARE_SET()       && \
								 MS_BL_PWM_PARAM_ISLEEP_SET()       && \
								 MS_BL_PWM_PARAM_ITRANSINIT_SET()   && \
								 MS_BL_PWM_PARAM_ITXINIT_SET() )

/* Callbacks */
#define MS_BL_PWM_CBS_SET() ( ( pwm_data.cbs.pwm_cb_disable_tx		!= NULL ) && \
							  ( pwm_data.cbs.pwm_cb_disable_rx_tx	!= NULL ) && \
							  ( pwm_data.cbs.pwm_cb_enable_tx		!= NULL ) && \
							  ( pwm_data.cbs.pwm_cb_enable_rx_tx	!= NULL ) && \
							  ( pwm_data.cbs.pwm_cb_update_info		!= NULL ) )


/* Timers */

#define MS_BL_PWM_TMR_THOLD			( pwm_data.params.ihold )

#define MS_BL_PWM_TMR_TTRANSINIT	( pwm_data.params.itransinit )
#define MS_BL_PWM_TMR_TTXINIT		( pwm_data.params.itxinit )

#define MS_BL_PWM_TMR_TSLEEP		( pwm_data.params.isleep - pwm_data.params.itransinit )
#define MS_BL_PWM_TMR_TAWARE		( pwm_data.params.iaware )

#define MS_BL_PWM_TMR_TSLEEP_DOZE	( pwm_data.params.isleep - pwm_data.params.itxinit )
#define MS_BL_PWM_TMR_TSLEEP_SLEEP	( pwm_data.params.isleep - pwm_data.params.itransinit )

typedef struct _bl_pwm_tmrs_dts
{
	/* Thold */
	uint16_t thold ;

	/* Ttransinit */
	uint16_t ttransinit ;

	/* Ttxinit */
	uint16_t ttxinit ;

	/* Tsleep */
	uint16_t tsleep ;

	/* Taware */
	uint16_t taware ;

	/* Tsleep (Doze) */
	uint16_t tsleep_doze ;

	/* Tsleep (Sleep) */
	uint16_t tsleep_sleep ;
}
BL_PWM_TMRS_DTS ;

typedef enum
{
	CS_BL_PWM_STATE_INACTIVE	= 0,
	CS_BL_PWM_STATE_ACTIVE_HELD	= 1,
	CS_BL_PWM_STATE_ACTIVE_FREE	= 2,
	CS_BL_PWM_STATE_SLEEP_AWARE	= 3,
	CS_BL_PWM_STATE_ASLEEP		= 4,
	CS_BL_PWM_STATE_DOZE_AWARE	= 5,
	CS_BL_PWM_STATE_LISTEN		= 6,
	CS_BL_PWM_STATE_ASLEEP_POST	= 7,
	CS_BL_PWM_STATE_LISTEN_POST	= 8
}
BL_PWM_STATE_DTS ;

typedef enum
{
	CS_BL_PWM_IN_LDI = 0,
	CS_BL_PWM_IN_LSI,
	CS_BL_PWM_IN_LWI,

	CS_BL_PWM_IN_ACTIVATE,
	CS_BL_PWM_IN_DEACTIVATE,

	CS_BL_PWM_IN_SA_ON,
	CS_BL_PWM_IN_SA_OFF,

	CS_BL_PWM_IN_CFG_SET,

	CS_BL_PWM_IN_THOLD,
	CS_BL_PWM_IN_TAWARE,
	CS_BL_PWM_IN_TSLEEP,
	CS_BL_PWM_IN_TTRANSINIT
}
BL_PWM_IN_DTS ;

typedef enum
{
	CS_BL_PWM_SA_OFF = 0,
	CS_BL_PWM_SA_ON
}
BL_PWM_SLEEP_ALLOW_DTS ;

typedef enum
{
	CS_BL_PWM_SR_DOZE	= 1,
	CS_BL_PWM_SR_SLEEP	= 2,
	CS_BL_PWM_SR_AWAKE	= 0
}
BL_PWM_SLEEP_REQUEST_DTS ;

typedef enum
{
	CS_BL_PWM_LPER_CYCLE = 0,
	CS_BL_PWM_LPER_AWAKE,
	CS_BL_PWM_LPER_DEACT
}
BL_PWM_LOW_POWER_EXIT_REASON_DTS ;

typedef struct _bl_pwm_data_dts
{
	CS_BL_PWM_CTRL ctrl_flg ;

	bool is_activated ;
	
	BL_PWM_PARAMS_DTS	params	;
	BL_PWM_CBS_DTE		cbs		;
	BL_PWM_TMRS_DTS		tmrs	;

	BL_PWM_MODE_DTE		mode	; /* Info */
	BL_PWM_IN_DTS		in		; /* Info */
	BL_PWM_STATE_DTS	state	;

	BL_PWM_SLEEP_ALLOW_DTS sa ;

	BL_PWM_LOW_POWER_EXIT_REASON_DTS lper ;

	bdmf_timer_t	tmr		;
	bool		is_thold_exp	;

	bdmf_semaphore_t smf ;
}
BL_PWM_DATA_DTS ;

/* APIs */

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   api_pwm_init                                                             */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   BL Lilac CH - Initializes Power Management resources                     */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function initializes Power Management resources                     */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   None                                                                     */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   BL_PWM_ERR_DTS - Return code                                             */
/*                                                                            */
/******************************************************************************/
BL_PWM_ERR_DTS api_pwm_init ( void ) ;


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   api_pwm_free                                                             */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   BL Lilac CH - Free Power Management resources                            */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function frees Power Management resources                           */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   None                                                                     */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   BL_PWM_ERR_DTS - Return code                                             */
/*                                                                            */
/******************************************************************************/
BL_PWM_ERR_DTS api_pwm_free ( void ) ;

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   api_pwm_activate                                                         */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   BL Lilac CH - Activate Power Management                                  */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function activates Power Management                                 */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   None                                                                     */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   BL_PWM_ERR_DTS - Return code                                             */
/*                                                                            */
/******************************************************************************/
BL_PWM_ERR_DTS api_pwm_activate ( void ) ;

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   api_pwm_deactivate                                                       */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   BL Lilac CH - Deactivate Power Management                                */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function deactivates Power Management                               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   None                                                                     */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   BL_PWM_ERR_DTS - Return code                                             */
/*                                                                            */
/******************************************************************************/
BL_PWM_ERR_DTS api_pwm_deactivate ( void ) ;

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   api_pwm_handle_sa_on                                                     */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   BL Lilac CH - Handle Sleep_Allow(On) receiving            	              */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function handles Sleep_Allow(On) receiving             	          */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   None                                                                     */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   BL_PWM_ERR_DTS - Return code                                             */
/*                                                                            */
/******************************************************************************/
BL_PWM_ERR_DTS api_pwm_handle_sa_on ( void ) ;


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   api_pwm_handle_sa_off                                                    */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   BL Lilac CH - Handle Sleep_Allow(Off) receiving                	      */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function handles Sleep_Allow(Off) receiving	                      */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   None                                                                     */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   BL_PWM_ERR_DTS - Return code                                             */
/*                                                                            */
/******************************************************************************/
BL_PWM_ERR_DTS api_pwm_handle_sa_off ( void ) ;

/* Auxiliary */

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   api_pwm_disable_rx                                                       */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   BL Lilac CH - Disable Rx direction of the tranceiver                     */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function disables Rx direction of the tranceiver                    */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   None                                                                     */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   BL_PWM_ERR_DTS - Return code                                             */
/*                                                                            */
/******************************************************************************/
BL_PWM_ERR_DTS api_pwm_disable_rx ( void ) ;

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   api_pwm_disable_tx                                                       */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   BL Lilac CH - Disable Tx direction of the tranceiver                     */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function disables Tx direction of the tranceiver                    */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   None                                                                     */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   BL_PWM_ERR_DTS - Return code                                             */
/*                                                                            */
/******************************************************************************/
BL_PWM_ERR_DTS api_pwm_disable_tx ( void ) ;

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   api_pwm_enable_rx                                                        */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   BL Lilac CH - Enable Rx direction of the tranceiver                      */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function enables Rx direction of the tranceiver                     */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   None                                                                     */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   BL_PWM_ERR_DTS - Return code                                             */
/*                                                                            */
/******************************************************************************/
BL_PWM_ERR_DTS api_pwm_enable_rx ( void ) ;

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   api_pwm_enable_tx                                                        */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   BL Lilac CH - Enable Tx direction of the tranceiver                      */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function enables Tx direction of the tranceiver                     */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   None                                                                     */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   BL_PWM_ERR_DTS - Return code                                             */
/*                                                                            */
/******************************************************************************/
BL_PWM_ERR_DTS api_pwm_enable_tx ( void ) ;

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   api_pwm_get_data                                                         */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   BL Lilac CH - Get Power Management data                                  */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function gets Power Management data                                 */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   xo_gpon_iw_db - Power Management data                                    */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   BL_PWM_ERR_DTS - Return code                                             */
/*                                                                            */
/******************************************************************************/
BL_PWM_ERR_DTS api_pwm_get_data ( BL_PWM_DATA_DTS* const xo_pwm_data ) ;

#ifdef __cplusplus
}
#endif

#endif /* BL_GPON_PWM_H_INCLUDED */

