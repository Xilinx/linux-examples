/*******************************************************************************
 *
 * Copyright (C) 2019 Xilinx, Inc.  All rights reserved.
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
// 0x10 : Data signal of num_blocks_V
//        bit 15~0 - num_blocks_V[15:0] (Read/Write)
//        others   - reserved
// 0x14 : reserved
// 0x18 : Data signal of first_V
//        bit 31~0 - first_V[31:0] (Read)
// 0x1c : reserved
// 0x20 : Data signal of last_V
//        bit 31~0 - last_V[31:0] (Read)
// 0x24 : reserved
// 0x28 : Data signal of stalled_V
//        bit 31~0 - stalled_V[31:0] (Read)
// 0x2c : reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

#define XMONITOR_CNTRL_ADDR_AP_CTRL           0x00
#define XMONITOR_CNTRL_ADDR_GIE               0x04
#define XMONITOR_CNTRL_ADDR_IER               0x08
#define XMONITOR_CNTRL_ADDR_ISR               0x0c
#define XMONITOR_CNTRL_ADDR_NUM_BLOCKS_V_DATA 0x10
#define XMONITOR_CNTRL_BITS_NUM_BLOCKS_V_DATA 16
#define XMONITOR_CNTRL_ADDR_FIRST_V_DATA      0x18
#define XMONITOR_CNTRL_BITS_FIRST_V_DATA      32
#define XMONITOR_CNTRL_ADDR_LAST_V_DATA       0x20
#define XMONITOR_CNTRL_BITS_LAST_V_DATA       32
#define XMONITOR_CNTRL_ADDR_STALLED_V_DATA    0x28
#define XMONITOR_CNTRL_BITS_STALLED_V_DATA    32
