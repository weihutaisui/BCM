/*
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

#ifndef ARM_DEBUG_CORE_H
#define ARM_DEBUG_CORE_H

#define CHIPIDADRESS 	0x18000000
#define CHIPIDMASK	0xffff
#define CHIPID4350	0X4350

#define DEBUG_BASE_DEFAULT 0x18007000
#define DEBUG_BASE_4350	0x18009000

#define ARMDEBUGTIMEOUT 100

/* CR4TRM 11.3.4 */

#define DTRRX		32	/* Data Transfer Register */

#define ITR		33	/* Instruction Transfer Register */

#define DSCR		34	/* Debug Status and Control Register */

#define DSCR_HALTED	(1 << 0)
#define DSCR_RESTARTED	(1 << 1)
#define DSCR_ITR_ENA	(1 << 13)
#define DSCR_HALT	(1 << 14)
#define DSCR_INS_COMP	(1 << 24)
#define DSCR_TX_FULL	(1 << 29)
#define DSCR_RX_FULL	(1 << 30)

#define DTRTX		35	/* Data Transfer Register */

#define DRCR		36	/* Debug Run Control Register */

#define DRCR_HALT	(1 << 0)	/* Halt request */
#define DRCR_RESTART	(1 << 1)	/* Restart request */
#define DRCR_CLEAR	(1 << 2)	/* Clear sticky exceptions */

#define LOCKACCESS	1004	/* Lock Access Register */

#define LOCKSTATUS	1005	/* Lock Status Register */

#define LOCKSTATUS_LOCKED (1 << 1)

/* ARM Registers */
typedef struct regs_type {
	uint32_t r[12];       // System and User  r00..r11
	uint32_t ip;          // System and User  r12 = ???             (IP)
	uint32_t sp;          // System and User  r13 = Stack Pointer   (SP)
	uint32_t lr;          // System and User  r14 = Link Register   (LR)
	uint32_t pc;          // System and User  r15 = Process Counter (PC)
	uint32_t cpsr;        // Current Programm Status Register       (CPSR)
} regs_type;

#define ARM_NOT_INIT	0
#define ARM_RUNNING	1
#define ARM_HALTED	2
#define ARM_SS		3
#define ARM_BP		4

uint32_t	dbg_rpc(void);
void		dbg_wpc (uint32_t val);
uint32_t	dbg_rar(uint32_t reg);
void		dbg_war(uint32_t reg, uint32_t val);
uint32_t	dbg_cpsr(void);
void		dbg_init(void);
void		dbg_halt(void);
uint32_t	dbg_read32(uint32_t addr);
unsigned short	dbg_read16(uint32_t addr);
void		dbg_read_arm_regs(regs_type *r);
int		dbg_get_arm_status(void);
void		dbg_run(void);
uint32_t	dbg_ss(void);
void		dbg_brk(uint32_t num, uint32_t addr, uint32_t thumb, uint32_t ss);
void		dbg_cbrk(uint32_t num);

#endif /* ARM_DEBUG_CORE_H */
