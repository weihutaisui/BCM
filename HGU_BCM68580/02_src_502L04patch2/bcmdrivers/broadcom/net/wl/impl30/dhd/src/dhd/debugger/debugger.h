/*
 * GDB server in DHD
 *
 * Copyright 2017 Broadcom
 *
 * This program is the proprietary software of Broadcom and/or
 * its licensors, and may only be used, duplicated, modified or distributed
 * pursuant to the terms and conditions of a separate, written license
 * agreement executed between you and Broadcom (an "Authorized License").
 * Except as set forth in an Authorized License, Broadcom grants no license
 * (express or implied), right to use, or waiver of any kind with respect to
 * the Software, and Broadcom expressly reserves all rights in and to the
 * Software and all intellectual property rights therein.  IF YOU HAVE NO
 * AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY
 * WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF
 * THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1. This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof, and to
 * use this information only in connection with your use of Broadcom
 * integrated circuit products.
 *
 * 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
 * "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR
 * OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL,
 * SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR
 * IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 * IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii)
 * ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF
 * OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY
 * NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
 *
 *
 * <<Broadcom-WL-IPTag/Proprietary:>>
 *
 * $Id$
 */

struct dhd_bus; /* forward declaration */

/**
 * Functions provided by the DHD BUS layer, and called by this debugger layer. These functions
 * read/write dongle memory.
 */
struct dhd_gdb_bus_ops_s {
	uint16 (*read_u16)(struct dhd_bus *bus, ulong offset);
	uint32 (*read_u32)(struct dhd_bus *bus, ulong offset);
	void   (*write_u32)(struct dhd_bus *bus, ulong offset, uint32 val);
};

void debugger_init(void *bus_handle, struct dhd_gdb_bus_ops_s *bus_ops, int gdb_svr_port,
	uint32 enum_base);
void debugger_close(void);

void dbg_mdelay(int d);
void dbg_fputc(char c, void *handle);
int dbg_fgetc(void *handle);
char *dbg_fgets(char *s, void *handle);
void dbg_fputs(char *s, void *handle);
void dbg_fclose(void *handle);

uint32_t dbg_bp_read32(uint32_t addr);
void dbg_bp_write32(uint32_t addr, uint32_t val);

void debugger_gdb_main(void *handle);

#if defined(DHD_DSCOPE)
/* then these function calls don't travel via debugger_arm.c, but directly to the backplane instead
 */
#define dbg_read16(addr) dbg_bp_read16(addr)
#define dbg_read32(addr) dbg_bp_read32(addr)
#endif /* DHD_SCOPE */

extern int sscanf(const char *s, const char *format, ...);
extern void do_exit(long);

#define FPUTC	dbg_fputc
#define FGETC	dbg_fgetc
#define FPUTS	dbg_fputs
#define FGETS	dbg_fgets
#define FCLOSE	dbg_fclose
