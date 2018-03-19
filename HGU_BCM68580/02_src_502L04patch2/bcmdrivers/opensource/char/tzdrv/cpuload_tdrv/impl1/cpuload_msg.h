/***************************************************************************
 * <:copyright-BRCM:2016:DUAL/GPL:standard
 * 
 *    Copyright (c) 2016 Broadcom 
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
 ***************************************************************************/
#ifndef CPU_LOAD_MSG_H
#define CPU_LOAD_MSG_H

/* echo uses invalid client id of 255 */
#define TZIOC_CLIENT_ID_ECHO            255

/* hello string (greet/reply) max */
#define CPU_LOAD_MSG_HELLO_MAX        64

enum
{
    CPU_LOAD_MSG_START = 0,           /* unused */
    CPU_LOAD_MSG_ECHO,                /* echo msg */
    CPU_LOAD_MSG_HELLO,               /* hello msg */
    CPU_LOAD_MSG_STOP,                /* stop msg */
    CPU_LOAD_MSG_LAST
};

struct cpu_load_msg_echo
{
    uint32_t value;
};

struct cpu_load_msg_hello_cmd
{
    char greet[CPU_LOAD_MSG_HELLO_MAX];
};

struct cpu_load_msg_hello_rpy
{
    char reply[CPU_LOAD_MSG_HELLO_MAX];
};

struct cpu_load_msg_stop
{
    uint32_t value;
};

#endif /* CPU_LOAD_MSG_H */
