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

#include "linux/types.h"
#include "gpon_general.h"
#include "gpon_pwm.h"

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   api_pwm_param_set                                                        */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   BL Lilac CH - Sets Power Management parameter                            */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function sets Power Management parameter                            */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   xi_val - Parameter value                                                 */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   BL_PWM_ERR_DTS - Return code                                             */
/*                                                                            */
/******************************************************************************/
BL_PWM_ERR_DTS api_pwm_param_set ( BL_PWM_PARAM_DTE	xi_param,
								   uint16_t		xi_val )
{
   return 0 ;
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   api_pwm_param_get                                                        */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   BL Lilac CH - Gets Power Management parameter                            */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function gets Power Management parameter                            */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   xo_val - Parameter value                                                 */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   BL_PWM_ERR_DTS - Return code                                             */
/*                                                                            */
/******************************************************************************/
BL_PWM_ERR_DTS api_pwm_param_get ( BL_PWM_PARAM_DTE		xi_param,
								   uint16_t* const	xo_val )
{
   return 0 ;
}


/* Callbacks */

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   api_pwm_reg_cbs                                                          */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   BL Lilac CH - Registers Host Application's callback functions            */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function registers Host Application's callback functions            */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   xi_cbs - Callback functions                                              */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   BL_PWM_ERR_DTS - Return code                                             */
/*                                                                            */
/******************************************************************************/
BL_PWM_ERR_DTS api_pwm_reg_cbs ( const BL_PWM_CBS_DTE* const xi_cbs )
{
   return 0 ;
}


/* Management */

/* External */

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   api_pwm_ctrl                                                             */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   BL Lilac CH - Control (enable/disable) Power Management                  */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function controls (enables/disables) Power Management               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   xi_ctrl_flg - Control flag (enable/disable)                              */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   BL_PWM_ERR_DTS - Return code                                             */
/*                                                                            */
/******************************************************************************/
BL_PWM_ERR_DTS api_pwm_ctrl ( CS_BL_PWM_CTRL xi_ctrl_flg )
{
   return 0 ;
}

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   api_pwm_switch_mode                                                      */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   BL Lilac CH - Initiates Power Management mode                            */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function initiates Power Management mode                            */
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
BL_PWM_ERR_DTS api_pwm_switch_mode ( BL_PWM_MODE_DTE xi_mode )
{
   return 0 ;
}
/* Internal */

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
BL_PWM_ERR_DTS api_pwm_init ( void )
{
   return 0 ;
}


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
BL_PWM_ERR_DTS api_pwm_free ( void )
{
   return 0 ;
}


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
BL_PWM_ERR_DTS api_pwm_activate ( void )
{
   return 0 ;
}


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
BL_PWM_ERR_DTS api_pwm_deactivate ( void )
{
   return 0 ;
}


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
BL_PWM_ERR_DTS api_pwm_handle_sa_on ( void )
{
   return 0 ;
}


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
BL_PWM_ERR_DTS api_pwm_handle_sa_off ( void )
{
   return 0 ;
}

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
BL_PWM_ERR_DTS api_pwm_disable_rx ( void )
{
   return 0 ;
}


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
BL_PWM_ERR_DTS api_pwm_disable_tx ( void )
{
   return 0 ;
}

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
BL_PWM_ERR_DTS api_pwm_enable_rx ( void )
{
   return 0 ;
}


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
BL_PWM_ERR_DTS api_pwm_enable_tx ( void )
{
   return 0 ;
}


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
BL_PWM_ERR_DTS api_pwm_get_data ( BL_PWM_DATA_DTS* const xo_pwm_data )
{
   return 0 ;
}



