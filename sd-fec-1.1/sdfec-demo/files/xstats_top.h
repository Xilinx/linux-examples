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

#ifndef XSTATS_TOP_H
#define XSTATS_TOP_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#ifndef __linux__
#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"
#include "xil_io.h"
#else
#include <stdint.h>
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stddef.h>
#endif
#include "xstats_top_hw.h"

/**************************** Type Definitions ******************************/
#ifdef __linux__
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef struct {
    int mem_fd;
    void *Cntrl_BaseAddress;
    u32 BaseAddr;
    u32 IsReady;
} XStats_top;
#else
typedef struct {
    u16 DeviceId;
    u32 Cntrl_BaseAddress;
} XStats_top_Config;

typedef struct {
    u32 Cntrl_BaseAddress;
    u32 IsReady;
} XStats_top;
#endif

typedef struct {
    u32 word_0;
    u32 word_1;
    u32 word_2;
    u32 word_3;
} XStats_top_Mask_v;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XStats_top_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XStats_top_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XStats_top_WriteReg(BaseAddress, RegOffset, Data) \
    *((volatile u32 *)(BaseAddress + RegOffset)) = Data;
#define XStats_top_ReadReg(BaseAddress, RegOffset) \
    (*(volatile u32 *)(BaseAddress + RegOffset))

#define Xil_AssertVoid(expr)    assert(expr)
#define Xil_AssertNonvoid(expr) assert(expr)

#define XST_SUCCESS             0
#define XST_DEVICE_NOT_FOUND    2
#define XST_OPEN_DEVICE_FAILED  3
#define XIL_COMPONENT_IS_READY  1
#define MAP_SIZE                4096UL

#endif

/************************** Function Prototypes *****************************/
#ifndef __linux__
int XStats_top_Initialize(XStats_top *InstancePtr, u16 DeviceId);
XStats_top_Config* XStats_top_LookupConfig(u16 DeviceId);
int XStats_top_CfgInitialize(XStats_top *InstancePtr, XStats_top_Config *ConfigPtr);
#else
int XStats_top_Initialize(XStats_top *InstancePtr, u32 BaseAddress);
int XStats_top_Release(XStats_top *InstancePtr);
#endif

void XStats_top_Start(XStats_top *InstancePtr);
u32 XStats_top_IsDone(XStats_top *InstancePtr);
u32 XStats_top_IsIdle(XStats_top *InstancePtr);
u32 XStats_top_IsReady(XStats_top *InstancePtr);
void XStats_top_EnableAutoRestart(XStats_top *InstancePtr);
void XStats_top_DisableAutoRestart(XStats_top *InstancePtr);

void XStats_top_Set_k_V(XStats_top *InstancePtr, u32 Data);
u32 XStats_top_Get_k_V(XStats_top *InstancePtr);
void XStats_top_Set_n_V(XStats_top *InstancePtr, u32 Data);
u32 XStats_top_Get_n_V(XStats_top *InstancePtr);
void XStats_top_Set_mask_V(XStats_top *InstancePtr, XStats_top_Mask_v Data);
XStats_top_Mask_v XStats_top_Get_mask_V(XStats_top *InstancePtr);
void XStats_top_Set_num_blocks_V(XStats_top *InstancePtr, u32 Data);
u32 XStats_top_Get_num_blocks_V(XStats_top *InstancePtr);
void XStats_top_Set_src_inc_parity_V(XStats_top *InstancePtr, u32 Data);
u32 XStats_top_Get_src_inc_parity_V(XStats_top *InstancePtr);
u32 XStats_top_Get_raw_berr_V(XStats_top *InstancePtr);
u32 XStats_top_Get_raw_blerr_V(XStats_top *InstancePtr);
u32 XStats_top_Get_cor_berr_V(XStats_top *InstancePtr);
u32 XStats_top_Get_cor_blerr_V(XStats_top *InstancePtr);
u32 XStats_top_Get_iter_cnt_V(XStats_top *InstancePtr);
u32 XStats_top_Get_block_cnt_V(XStats_top *InstancePtr);

void XStats_top_InterruptGlobalEnable(XStats_top *InstancePtr);
void XStats_top_InterruptGlobalDisable(XStats_top *InstancePtr);
void XStats_top_InterruptEnable(XStats_top *InstancePtr, u32 Mask);
void XStats_top_InterruptDisable(XStats_top *InstancePtr, u32 Mask);
void XStats_top_InterruptClear(XStats_top *InstancePtr, u32 Mask);
u32 XStats_top_InterruptGetEnabled(XStats_top *InstancePtr);
u32 XStats_top_InterruptGetStatus(XStats_top *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
