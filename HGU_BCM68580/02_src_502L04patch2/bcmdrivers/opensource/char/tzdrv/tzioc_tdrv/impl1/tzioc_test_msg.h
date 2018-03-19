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
#ifndef TZIOC_TEST_MSG_H
#define TZIOC_TEST_MSG_H

/* echo uses invalid client id of 255 */
#define TZIOC_CLIENT_ID_ECHO            255

/* hello string (greet/reply) max */
#define TZIOC_TEST_MSG_HELLO_MAX        64

enum
{
    TZIOC_TEST_MSG_START = 0,           /* unused */
    TZIOC_TEST_MSG_ECHO,                /* echo msg, to echo client */
    TZIOC_TEST_MSG_HELLO,               /* hello msg, to tzioc_tapp */
    TZIOC_TEST_MSG_MEM_ALLOC,           /* mem alloc msg, to tzioc_tapp */
    TZIOC_TEST_MSG_MAP_PADDR,           /* map paddr msg, to tzioc_tapp */
    TZIOC_TEST_MSG_MAP_PADDRS,          /* map paddr msg, to tzioc_tapp */
    TZIOC_TEST_MSG_LAST
};

struct tzioc_test_msg_echo
{
    uint32_t value;
};

struct tzioc_test_msg_hello_cmd
{
    char greet[TZIOC_TEST_MSG_HELLO_MAX];
};

struct tzioc_test_msg_hello_rpy
{
    char reply[TZIOC_TEST_MSG_HELLO_MAX];
};

struct tzioc_test_msg_mem_alloc_cmd
{
    uint32_t offset;
    uint32_t size;
};

struct tzioc_test_msg_mem_alloc_rpy
{
    uint32_t checksum;
};

struct tzioc_test_msg_map_paddr_cmd
{
    uint32_t paddr;
    uint32_t size;
    uint32_t flags;
};

struct tzioc_test_msg_map_paddr_rpy
{
    uint32_t checksum;
};

struct tzioc_test_msg_map_paddrs_cmd
{
    uint32_t count;
    uint32_t paddrs[32];
    uint32_t sizes[32];
    uint32_t flags[32];
};

struct tzioc_test_msg_map_paddrs_rpy
{
    uint32_t checksum;
};

#endif /* TZIOC_TEST_MSG_H */
