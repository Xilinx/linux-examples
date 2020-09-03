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

#ifndef XMONITOR_H
#define XMONITOR_H

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
#include <metal/sys.h>
#include <metal/io.h>
#include <metal/irq.h>
#include <metal/atomic.h>
#include <metal/cpu.h>
#include <metal/device.h>
#endif
#include "xmonitor_hw.h"

/**************************** Type Definitions ******************************/
#ifdef __linux__
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef struct {
    struct metal_io_region *Cntrl_BaseAddress;
    struct metal_device *Cntrl_DevicePtr;
    u32 IsReady;
} XMonitor;
#else
typedef struct {
    u16 DeviceId;
    u32 Cntrl_BaseAddress;
} XMonitor_Config;

typedef struct {
    u32 Cntrl_BaseAddress;
    u32 IsReady;
} XMonitor;
#endif


/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XMonitor_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XMonitor_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XMonitor_WriteReg(BaseAddress, RegOffset, Data) \
    metal_io_write32((BaseAddress), (RegOffset), (Data))
#define XMonitor_ReadReg(BaseAddress, RegOffset) \
    metal_io_read32((BaseAddress), (RegOffset))

#define Xil_AssertVoid(expr)    assert(expr)
#define Xil_AssertNonvoid(expr) assert(expr)

#define XST_SUCCESS             0
#define XST_DEVICE_NOT_FOUND    2
#define XST_OPEN_DEVICE_FAILED  3
#define XIL_COMPONENT_IS_READY  1
#endif

/************************** Function Prototypes *****************************/
#ifndef __linux__
int XMonitor_Initialize(XMonitor *InstancePtr, u16 DeviceId);
XMonitor_Config* XMonitor_LookupConfig(u16 DeviceId);
int XMonitor_CfgInitialize(XMonitor *InstancePtr, XMonitor_Config *ConfigPtr);
#else
int XMonitor_Initialize(XMonitor *InstancePtr, char* BusName, char* DeviceName);
int XMonitor_Release(XMonitor *InstancePtr);
#endif

void XMonitor_Start(XMonitor *InstancePtr);
u32 XMonitor_IsDone(XMonitor *InstancePtr);
u32 XMonitor_IsIdle(XMonitor *InstancePtr);
u32 XMonitor_IsReady(XMonitor *InstancePtr);
void XMonitor_EnableAutoRestart(XMonitor *InstancePtr);
void XMonitor_DisableAutoRestart(XMonitor *InstancePtr);

void XMonitor_Set_num_blocks_V(XMonitor *InstancePtr, u32 Data);
u32 XMonitor_Get_num_blocks_V(XMonitor *InstancePtr);
u32 XMonitor_Get_first_V(XMonitor *InstancePtr);
u32 XMonitor_Get_last_V(XMonitor *InstancePtr);
u32 XMonitor_Get_stalled_V(XMonitor *InstancePtr);

void XMonitor_InterruptGlobalEnable(XMonitor *InstancePtr);
void XMonitor_InterruptGlobalDisable(XMonitor *InstancePtr);
void XMonitor_InterruptEnable(XMonitor *InstancePtr, u32 Mask);
void XMonitor_InterruptDisable(XMonitor *InstancePtr, u32 Mask);
void XMonitor_InterruptClear(XMonitor *InstancePtr, u32 Mask);
u32 XMonitor_InterruptGetEnabled(XMonitor *InstancePtr);
u32 XMonitor_InterruptGetStatus(XMonitor *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
