/******************************************************************************
* Copyright (C) 2016-2022 Xilinx, Inc. All rights reserved.
* Copyright (C) 2022-2024 Advanced Micro Devices, Inc. All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

#ifndef XSTATS_TOP_H
#define XSTATS_TOP_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
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
#include "xstats_top_hw.h"

/**************************** Type Definitions ******************************/
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

typedef struct {
    u32 word_0;
    u32 word_1;
    u32 word_2;
    u32 word_3;
} XStats_top_Mask_v;

/***************** Macros (Inline Functions) Definitions *********************/
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

/************************** Function Prototypes *****************************/
int XStats_top_Initialize(XStats_top *InstancePtr, u32 BaseAddress);
int XStats_top_Release(XStats_top *InstancePtr);

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
