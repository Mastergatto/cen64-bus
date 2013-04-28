/* ============================================================================
 *  Address.h: Device address list.
 *
 *  BUSSIM: Reality Co-Processor BUS SIMulator.
 *  Copyright (C) 2013, Tyler J. Stachecki.
 *  All rights reserved.
 *
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE', which is part of this source code package.
 * ========================================================================= */
#ifndef __BUS__ADDRESS_H__
#define __BUS__ADDRESS_H__

/* Audio Interface Registers. */
#define AI_REGS_BASE_ADDRESS      0x04500000
#define AI_REGS_ADDRESS_LEN       0x00000018

/* Display Processor Registers. */
#define DP_REGS_BASE_ADDRESS      0x04100000
#define DP_REGS_ADDRESS_LEN       0x00000020

/* MIPS Interface Registers. */
#define MI_REGS_BASE_ADDRESS      0x04300000
#define MI_REGS_ADDRESS_LEN       0x00000010

/* Parallel Interface Regs. */
#define PI_REGS_BASE_ADDRESS      0x04600000
#define PI_REGS_ADDRESS_LEN       0x00100000

/* Peripheral Interface RAM. */
#define PIF_RAM_BASE_ADDRESS      0x1FC007C0
#define PIF_RAM_ADDRESS_LEN       0x00000040

/* Peripheral Interface ROM. */
#define PIF_ROM_BASE_ADDRESS      0x1FC00000
#define PIF_ROM_ADDRESS_LEN       0x000007C0

/* RDRAM (Memory). */
#define RDRAM_BASE_ADDRESS        0x00000000
#define RDRAM_ADDRESS_LEN         0x00800000

/* RDRAM Registers. */
#define RDRAM_REGS_BASE_ADDRESS   0x03F00000
#define RDRAM_REGS_ADDRESS_LEN    0x00000028

/* RDRAM Interface Registers. */
#define RI_REGS_BASE_ADDRESS      0x04700000
#define RI_REGS_ADDRESS_LEN       0x00000020

/* ROM Cartridge Interface. */
#define ROM_CART_BASE_ADDRESS     0x10000000
#define ROM_CART_ADDRESS_LEN      0x0FC00000

/* RSP/DMEM. */
#define RSP_DMEM_BASE_ADDRESS     0x04000000
#define RSP_DMEM_ADDRESS_LEN      0x00001000

/* RSP/IMEM. */
#define RSP_IMEM_BASE_ADDRESS     0x04001000
#define RSP_IMEM_ADDRESS_LEN      0x00001000

/* Serial Interface Registers. */
#define SI_REGS_BASE_ADDRESS      0x04800000
#define SI_REGS_ADDRESS_LEN       0x0000001C

/* SP Interface Registers. */
#define SP_REGS_BASE_ADDRESS      0x04040000
#define SP_REGS_ADDRESS_LEN       0x00000020

/* SP Interface Registers [2]. */
#define SP_REGS2_BASE_ADDRESS     0x04080000
#define SP_REGS2_ADDRESS_LEN      0x00000008

/* Video Interface Registers. */
#define VI_REGS_BASE_ADDRESS      0x04400000
#define VI_REGS_ADDRESS_LEN       0x00000038

#endif

