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

/***************************** Include Files *********************************/
#include "xmonitor.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XMonitor_CfgInitialize(XMonitor *InstancePtr, XMonitor_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Cntrl_BaseAddress = ConfigPtr->Cntrl_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#else
int XMonitor_Initialize(XMonitor *InstancePtr, char* BusName, char* DeviceName) {
    int Result = -1;

    assert(InstancePtr != NULL);

    /* Open shared memory device */
    Result = metal_device_open(BusName, DeviceName, &(InstancePtr->Cntrl_DevicePtr));
    if (Result < 0) {
        printf("%s(): Open device failed\n", __func__);
        return Result;
    }

    /* Get shared memory device IO region */
    InstancePtr->Cntrl_BaseAddress = metal_device_io_region(InstancePtr->Cntrl_DevicePtr, 0);
    assert(InstancePtr->Cntrl_BaseAddress);

    if (InstancePtr->Cntrl_BaseAddress == NULL) {
        printf("%s(): Get shared memory device IO region failed\n", __func__);
    }

    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}

int XMonitor_Release(XMonitor *InstancePtr) {
    assert(InstancePtr != NULL);
    assert(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    metal_device_close(InstancePtr->Cntrl_DevicePtr);

    return XST_SUCCESS;
}
#endif

void XMonitor_Start(XMonitor *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XMonitor_ReadReg(InstancePtr->Cntrl_BaseAddress, XMONITOR_CNTRL_ADDR_AP_CTRL) & 0x80;
    XMonitor_WriteReg(InstancePtr->Cntrl_BaseAddress, XMONITOR_CNTRL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XMonitor_IsDone(XMonitor *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XMonitor_ReadReg(InstancePtr->Cntrl_BaseAddress, XMONITOR_CNTRL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XMonitor_IsIdle(XMonitor *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XMonitor_ReadReg(InstancePtr->Cntrl_BaseAddress, XMONITOR_CNTRL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XMonitor_IsReady(XMonitor *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XMonitor_ReadReg(InstancePtr->Cntrl_BaseAddress, XMONITOR_CNTRL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XMonitor_EnableAutoRestart(XMonitor *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XMonitor_WriteReg(InstancePtr->Cntrl_BaseAddress, XMONITOR_CNTRL_ADDR_AP_CTRL, 0x80);
}

void XMonitor_DisableAutoRestart(XMonitor *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XMonitor_WriteReg(InstancePtr->Cntrl_BaseAddress, XMONITOR_CNTRL_ADDR_AP_CTRL, 0);
}

void XMonitor_Set_num_blocks_V(XMonitor *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XMonitor_WriteReg(InstancePtr->Cntrl_BaseAddress, XMONITOR_CNTRL_ADDR_NUM_BLOCKS_V_DATA, Data);
}

u32 XMonitor_Get_num_blocks_V(XMonitor *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XMonitor_ReadReg(InstancePtr->Cntrl_BaseAddress, XMONITOR_CNTRL_ADDR_NUM_BLOCKS_V_DATA);
    return Data;
}

u32 XMonitor_Get_first_V(XMonitor *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XMonitor_ReadReg(InstancePtr->Cntrl_BaseAddress, XMONITOR_CNTRL_ADDR_FIRST_V_DATA);
    return Data;
}

u32 XMonitor_Get_last_V(XMonitor *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XMonitor_ReadReg(InstancePtr->Cntrl_BaseAddress, XMONITOR_CNTRL_ADDR_LAST_V_DATA);
    return Data;
}

u32 XMonitor_Get_stalled_V(XMonitor *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XMonitor_ReadReg(InstancePtr->Cntrl_BaseAddress, XMONITOR_CNTRL_ADDR_STALLED_V_DATA);
    return Data;
}

void XMonitor_InterruptGlobalEnable(XMonitor *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XMonitor_WriteReg(InstancePtr->Cntrl_BaseAddress, XMONITOR_CNTRL_ADDR_GIE, 1);
}

void XMonitor_InterruptGlobalDisable(XMonitor *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XMonitor_WriteReg(InstancePtr->Cntrl_BaseAddress, XMONITOR_CNTRL_ADDR_GIE, 0);
}

void XMonitor_InterruptEnable(XMonitor *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XMonitor_ReadReg(InstancePtr->Cntrl_BaseAddress, XMONITOR_CNTRL_ADDR_IER);
    XMonitor_WriteReg(InstancePtr->Cntrl_BaseAddress, XMONITOR_CNTRL_ADDR_IER, Register | Mask);
}

void XMonitor_InterruptDisable(XMonitor *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XMonitor_ReadReg(InstancePtr->Cntrl_BaseAddress, XMONITOR_CNTRL_ADDR_IER);
    XMonitor_WriteReg(InstancePtr->Cntrl_BaseAddress, XMONITOR_CNTRL_ADDR_IER, Register & (~Mask));
}

void XMonitor_InterruptClear(XMonitor *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XMonitor_WriteReg(InstancePtr->Cntrl_BaseAddress, XMONITOR_CNTRL_ADDR_ISR, Mask);
}

u32 XMonitor_InterruptGetEnabled(XMonitor *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XMonitor_ReadReg(InstancePtr->Cntrl_BaseAddress, XMONITOR_CNTRL_ADDR_IER);
}

u32 XMonitor_InterruptGetStatus(XMonitor *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XMonitor_ReadReg(InstancePtr->Cntrl_BaseAddress, XMONITOR_CNTRL_ADDR_ISR);
}
