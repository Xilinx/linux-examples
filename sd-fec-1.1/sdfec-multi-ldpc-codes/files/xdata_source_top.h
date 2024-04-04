/******************************************************************************
* Copyright (C) 2016-2022 Xilinx, Inc. All rights reserved.
* Copyright (C) 2022-2024 Advanced Micro Devices, Inc. All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

#ifndef XDATA_SOURCE_TOP_H
#define XDATA_SOURCE_TOP_H

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
#include "xdata_source_top_hw.h"

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
} XData_source_top;

/***************** Macros (Inline Functions) Definitions *********************/
#define XData_source_top_WriteReg(BaseAddress, RegOffset, Data) \
    *((volatile u32 *)(BaseAddress + RegOffset)) = Data;
#define XData_source_top_ReadReg(BaseAddress, RegOffset) \
    (*(volatile u32 *)(BaseAddress + RegOffset))


#define Xil_AssertVoid(expr)    assert(expr)
#define Xil_AssertNonvoid(expr) assert(expr)

#define XST_SUCCESS             0
#define XST_DEVICE_NOT_FOUND    2
#define XST_OPEN_DEVICE_FAILED  3
#define XIL_COMPONENT_IS_READY  1
#define MAP_SIZE                4096UL

/************************** Function Prototypes *****************************/
int XData_source_top_Initialize(XData_source_top *InstancePtr, u32 BaseAddress);
int XData_source_top_Release(XData_source_top *InstancePtr);

void XData_source_top_Start(XData_source_top *InstancePtr);
u32 XData_source_top_IsDone(XData_source_top *InstancePtr);
u32 XData_source_top_IsIdle(XData_source_top *InstancePtr);
u32 XData_source_top_IsReady(XData_source_top *InstancePtr);
void XData_source_top_EnableAutoRestart(XData_source_top *InstancePtr);
void XData_source_top_DisableAutoRestart(XData_source_top *InstancePtr);

void XData_source_top_Set_fec_type_V(XData_source_top *InstancePtr, u32 Data);
u32 XData_source_top_Get_fec_type_V(XData_source_top *InstancePtr);
void XData_source_top_Set_mod_type_V(XData_source_top *InstancePtr, u32 Data);
u32 XData_source_top_Get_mod_type_V(XData_source_top *InstancePtr);
void XData_source_top_Set_skip_chan_V(XData_source_top *InstancePtr, u32 Data);
u32 XData_source_top_Get_skip_chan_V(XData_source_top *InstancePtr);
void XData_source_top_Set_snr_V(XData_source_top *InstancePtr, u32 Data);
u32 XData_source_top_Get_snr_V(XData_source_top *InstancePtr);
void XData_source_top_Set_inv_sigma_sq_V(XData_source_top *InstancePtr, u32 Data);
u32 XData_source_top_Get_inv_sigma_sq_V(XData_source_top *InstancePtr);
void XData_source_top_Set_enc_ctrl_word_V(XData_source_top *InstancePtr, u64 Data);
u64 XData_source_top_Get_enc_ctrl_word_V(XData_source_top *InstancePtr);
void XData_source_top_Set_dec_ctrl_word_V(XData_source_top *InstancePtr, u64 Data);
u64 XData_source_top_Get_dec_ctrl_word_V(XData_source_top *InstancePtr);
void XData_source_top_Set_num_blocks_V(XData_source_top *InstancePtr, u32 Data);
u32 XData_source_top_Get_num_blocks_V(XData_source_top *InstancePtr);
void XData_source_top_Set_source_words_V(XData_source_top *InstancePtr, u32 Data);
u32 XData_source_top_Get_source_words_V(XData_source_top *InstancePtr);
void XData_source_top_Set_chan_symbls_V(XData_source_top *InstancePtr, u32 Data);
u32 XData_source_top_Get_chan_symbls_V(XData_source_top *InstancePtr);
void XData_source_top_Set_chan_rem_V(XData_source_top *InstancePtr, u32 Data);
u32 XData_source_top_Get_chan_rem_V(XData_source_top *InstancePtr);
void XData_source_top_Set_zero_data_V(XData_source_top *InstancePtr, u32 Data);
u32 XData_source_top_Get_zero_data_V(XData_source_top *InstancePtr);
void XData_source_top_Set_source_keep_V(XData_source_top *InstancePtr, u32 Data);
u32 XData_source_top_Get_source_keep_V(XData_source_top *InstancePtr);
void XData_source_top_Set_enc_keep_V(XData_source_top *InstancePtr, u64 Data);
u64 XData_source_top_Get_enc_keep_V(XData_source_top *InstancePtr);
void XData_source_top_Set_dec_keep_V(XData_source_top *InstancePtr, u64 Data);
u64 XData_source_top_Get_dec_keep_V(XData_source_top *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
