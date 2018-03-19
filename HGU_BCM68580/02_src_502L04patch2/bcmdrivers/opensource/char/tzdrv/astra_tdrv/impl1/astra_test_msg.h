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
#ifndef ASTRA_TEST_MSG_H
#define ASTRA_TEST_MSG_H

typedef struct astra_test_msg_hdr {
    uint8_t  ucType;                    /* msg type */
    uint8_t  ucSeq;                     /* msg sequence number */
    uint8_t  ucUnused[2];
} astra_test_msg_hdr;

#define ASTRA_TEST_MSG_PAYLOAD(pHdr)    ((uint8_t *)pHdr + sizeof(astra_test_msg_hdr))

enum
{
    ASTRA_TEST_MSG_START = 0,           /* unused */
    ASTRA_TEST_MSG_HELLO,               /* hello msg, to astra_tapp */
    ASTRA_TEST_MSG_MEM_ALLOC,           /* mem alloc msg, to astra_tapp */
    ASTRA_TEST_MSG_MAP_PADDR,           /* map paddr msg, to astra_tapp */
    ASTRA_TEST_MSG_MAP_PADDRS,          /* map paddr msg, to astra_tapp */
    ASTRA_TEST_MSG_LAST
};

struct astra_test_msg_echo
{
    uint32_t value;
};

struct astra_test_msg_hello_cmd
{
    char greet[64];
};

struct astra_test_msg_hello_rpy
{
    char reply[64];
};

struct astra_test_msg_mem_alloc_cmd
{
    uint32_t offset;
    uint32_t size;
};

struct astra_test_msg_mem_alloc_rpy
{
    uint32_t checksum;
};

struct astra_test_msg_map_paddr_cmd
{
    uint32_t paddr;
    uint32_t size;
    uint32_t flags;
};

struct astra_test_msg_map_paddr_rpy
{
    uint32_t checksum;
};

struct astra_test_msg_map_paddrs_cmd
{
    uint32_t count;
    uint32_t paddrs[32];
    uint32_t sizes[32];
    uint32_t flags[32];
};

struct astra_test_msg_map_paddrs_rpy
{
    uint32_t checksum;
};

#endif /* ASTRA_TEST_MSG_H */
