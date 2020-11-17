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

#ifndef XDATA_SOURCE_TOP_H
#define XDATA_SOURCE_TOP_H

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
#include "xdata_source_top_hw.h"

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
} XData_source_top;
#else
typedef struct {
    u16 DeviceId;
    u32 Cntrl_BaseAddress;
} XData_source_top_Config;

typedef struct {
    u32 Cntrl_BaseAddress;
    u32 IsReady;
} XData_source_top;
#endif


/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XData_source_top_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XData_source_top_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
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

#endif

/************************** Function Prototypes *****************************/
#ifndef __linux__
int XData_source_top_Initialize(XData_source_top *InstancePtr, u16 DeviceId);
XData_source_top_Config* XData_source_top_LookupConfig(u16 DeviceId);
int XData_source_top_CfgInitialize(XData_source_top *InstancePtr, XData_source_top_Config *ConfigPtr);
#else
int XData_source_top_Initialize(XData_source_top *InstancePtr, u32 BaseAddress);
int XData_source_top_Release(XData_source_top *InstancePtr);
#endif

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
