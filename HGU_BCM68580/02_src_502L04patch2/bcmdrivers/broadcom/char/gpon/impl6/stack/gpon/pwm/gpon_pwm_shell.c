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

#include "bdmf_shell.h"
#include "gpon_general.h"
#include "gpon_pwm_shell.h"


#define CS_STR_LEN_MAX 32

char* gs_pwm_strs_in[] =
{
	"LDI",
	"LSI",
	"LWI",

	"Activate",
	"Deactivate",

	"SA(On)",
	"SA(Off)",

	"Cfg",
    
	"Thold",
	"Taware",
	"Tsleep",
	"Ttransinit"
} ;

char* gs_pwm_strs_state[] =
{
	"Inactive",
	"ActiveHeld",
	"ActiveFree",
	"SleepAware",
	"Asleep",
	"DozeAware",
	"Listen",
	"AsleepPost",
	"ListenPost"
} ;

char* gs_pwm_strs_mode[] =
{
	"Awake",
	"Doze",
	"Sleep"
} ;

char* gs_pwm_strs_sa[] =
{
	"Off",
	"On"
} ;

char* gs_pwm_strs_lper[] =
{
	"Cycle",
	"Awake",
	"Deactivate"
} ;

void f_bl_convert_to_str_ctrl(CS_BL_PWM_CTRL xi_ctrl, char* const xo_str)
{
    if ( xi_ctrl == CS_BL_PWM_CTRL_ENABLE )
    {
        sprintf( xo_str, "%-s", "Enabled" ) ;
    }
    else
    {
        sprintf( xo_str, "%-s", "Disabled" ) ;
    }
}


void f_bl_convert_to_str_bool(bool xi_bool, char* const xo_str)
{
    if ( xi_bool == BDMF_TRUE )
    {
        sprintf( xo_str, "%-s", "True" ) ;
    }
    else
    {
        sprintf( xo_str, "%-s", "False" ) ;
    }
}



BL_PWM_ERR_DTS api_pwm_shell_init(bdmfmon_handle_t base_dir)
{
    static bdmfmon_handle_t dir;

    dir = bdmfmon_dir_add(base_dir, "pwm", "Power Management", BDMF_ACCESS_GUEST, NULL);
    if (!dir)
    {
        bdmf_session_print(NULL, "Can't create gpon_drv_dir directory\n");
        return CS_BL_PWM_ERR_SHELL;
    }

    return CS_BL_PWM_ERR_OK;
}

