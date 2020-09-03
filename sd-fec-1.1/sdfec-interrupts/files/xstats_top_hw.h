/*******************************************************************************
 *
 * Copyright (C) 2016 - 2020 Xilinx, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of the Xilinx shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from Xilinx.
 *
 ******************************************************************************/

// CNTRL
// 0x00 : Control signals
//        bit 0  - ap_start (Read/Write/COH)
//        bit 1  - ap_done (Read/COR)
//        bit 2  - ap_idle (Read)
//        bit 3  - ap_ready (Read)
//        bit 7  - auto_restart (Read/Write)
//        others - reserved
// 0x04 : Global Interrupt Enable Register
//        bit 0  - Global Interrupt Enable (Read/Write)
//        others - reserved
// 0x08 : IP Interrupt Enable Register (Read/Write)
//        bit 0  - Channel 0 (ap_done)
//        bit 1  - Channel 1 (ap_ready)
//        others - reserved
// 0x0c : IP Interrupt Status Register (Read/TOW)
//        bit 0  - Channel 0 (ap_done)
//        bit 1  - Channel 1 (ap_ready)
//        others - reserved
// 0x10 : Data signal of k_V
//        bit 31~0 - k_V[31:0] (Read/Write)
// 0x14 : reserved
// 0x18 : Data signal of n_V
//        bit 31~0 - n_V[31:0] (Read/Write)
// 0x1c : reserved
// 0x20 : Data signal of mask_V
//        bit 31~0 - mask_V[31:0] (Read/Write)
// 0x24 : Data signal of mask_V
//        bit 31~0 - mask_V[63:32] (Read/Write)
// 0x28 : Data signal of mask_V
//        bit 31~0 - mask_V[95:64] (Read/Write)
// 0x2c : Data signal of mask_V
//        bit 31~0 - mask_V[127:96] (Read/Write)
// 0x30 : reserved
// 0x34 : Data signal of num_blocks_V
//        bit 31~0 - num_blocks_V[31:0] (Read/Write)
// 0x38 : reserved
// 0x3c : Data signal of raw_berr_V
//        bit 31~0 - raw_berr_V[31:0] (Read)
// 0x40 : reserved
// 0x44 : Data signal of raw_blerr_V
//        bit 31~0 - raw_blerr_V[31:0] (Read)
// 0x48 : reserved
// 0x4c : Data signal of cor_berr_V
//        bit 31~0 - cor_berr_V[31:0] (Read)
// 0x50 : reserved
// 0x54 : Data signal of cor_blerr_V
//        bit 31~0 - cor_blerr_V[31:0] (Read)
// 0x58 : reserved
// 0x5c : Data signal of iter_cnt_V
//        bit 31~0 - iter_cnt_V[31:0] (Read)
// 0x60 : reserved
// 0x64 : Data signal of block_cnt_V
//        bit 31~0 - block_cnt_V[31:0] (Read)
// 0x68 : reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

#define XSTATS_TOP_CNTRL_ADDR_AP_CTRL               0x00
#define XSTATS_TOP_CNTRL_ADDR_GIE                   0x04
#define XSTATS_TOP_CNTRL_ADDR_IER                   0x08
#define XSTATS_TOP_CNTRL_ADDR_ISR                   0x0c
#define XSTATS_TOP_CNTRL_ADDR_K_V_DATA              0x10
#define XSTATS_TOP_CNTRL_BITS_K_V_DATA              32
#define XSTATS_TOP_CNTRL_ADDR_N_V_DATA              0x18
#define XSTATS_TOP_CNTRL_BITS_N_V_DATA              32
#define XSTATS_TOP_CNTRL_ADDR_MASK_V_DATA           0x20
#define XSTATS_TOP_CNTRL_BITS_MASK_V_DATA           128
#define XSTATS_TOP_CNTRL_ADDR_NUM_BLOCKS_V_DATA     0x34
#define XSTATS_TOP_CNTRL_BITS_NUM_BLOCKS_V_DATA     32
#define XSTATS_TOP_CNTRL_ADDR_SRC_INC_PARITY_V_DATA 0x3c
#define XSTATS_TOP_CNTRL_BITS_SRC_INC_PARITY_V_DATA 32
#define XSTATS_TOP_CNTRL_ADDR_RAW_BERR_V_DATA       0x44
#define XSTATS_TOP_CNTRL_BITS_RAW_BERR_V_DATA       32
#define XSTATS_TOP_CNTRL_ADDR_RAW_BLERR_V_DATA      0x4c
#define XSTATS_TOP_CNTRL_BITS_RAW_BLERR_V_DATA      32
#define XSTATS_TOP_CNTRL_ADDR_COR_BERR_V_DATA       0x54
#define XSTATS_TOP_CNTRL_BITS_COR_BERR_V_DATA       32
#define XSTATS_TOP_CNTRL_ADDR_COR_BLERR_V_DATA      0x5c
#define XSTATS_TOP_CNTRL_BITS_COR_BLERR_V_DATA      32
#define XSTATS_TOP_CNTRL_ADDR_ITER_CNT_V_DATA       0x64
#define XSTATS_TOP_CNTRL_BITS_ITER_CNT_V_DATA       32
#define XSTATS_TOP_CNTRL_ADDR_BLOCK_CNT_V_DATA      0x6c
#define XSTATS_TOP_CNTRL_BITS_BLOCK_CNT_V_DATA      32
