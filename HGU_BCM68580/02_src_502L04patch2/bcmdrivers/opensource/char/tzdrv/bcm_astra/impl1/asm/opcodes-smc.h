/*
 * opcodes-smc.h: Opcode definitions for the ARM smcualization extensions
 * Copyright (C) 2012  Linaro Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef __ASM_ARM_OPCODES_SMC_H
#define __ASM_ARM_OPCODES_SMC_H

#include <asm/opcodes.h>

#define __SMC(imm4) __inst_arm_thumb32(      \
    0xE1600070 | (((imm4) & 0xF)      ), \
    0xF7F08000 | (((imm4) & 0xF) << 16)  \
)

#endif /* ! __ASM_ARM_OPCODES_SMC_H */
