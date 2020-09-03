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
#include "xstats_top.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XStats_top_CfgInitialize(XStats_top *InstancePtr, XStats_top_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Cntrl_BaseAddress = ConfigPtr->Cntrl_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#else
int XStats_top_Initialize(XStats_top *InstancePtr, char* BusName, char* DeviceName) {
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

int XStats_top_Release(XStats_top *InstancePtr) {
    assert(InstancePtr != NULL);
    assert(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    metal_device_close(InstancePtr->Cntrl_DevicePtr);

    return XST_SUCCESS;
}
#endif

void XStats_top_Start(XStats_top *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XStats_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_AP_CTRL) & 0x80;
    XStats_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XStats_top_IsDone(XStats_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XStats_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XStats_top_IsIdle(XStats_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XStats_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XStats_top_IsReady(XStats_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XStats_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XStats_top_EnableAutoRestart(XStats_top *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XStats_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_AP_CTRL, 0x80);
}

void XStats_top_DisableAutoRestart(XStats_top *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XStats_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_AP_CTRL, 0);
}

void XStats_top_Set_k_V(XStats_top *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XStats_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_K_V_DATA, Data);
}

u32 XStats_top_Get_k_V(XStats_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XStats_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_K_V_DATA);
    return Data;
}

void XStats_top_Set_n_V(XStats_top *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XStats_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_N_V_DATA, Data);
}

u32 XStats_top_Get_n_V(XStats_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XStats_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_N_V_DATA);
    return Data;
}

void XStats_top_Set_mask_V(XStats_top *InstancePtr, XStats_top_Mask_v Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XStats_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_MASK_V_DATA + 0, Data.word_0);
    XStats_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_MASK_V_DATA + 4, Data.word_1);
    XStats_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_MASK_V_DATA + 8, Data.word_2);
    XStats_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_MASK_V_DATA + 12, Data.word_3);
}

XStats_top_Mask_v XStats_top_Get_mask_V(XStats_top *InstancePtr) {
    XStats_top_Mask_v Data;

    Data.word_0 = XStats_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_MASK_V_DATA + 0);
    Data.word_1 = XStats_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_MASK_V_DATA + 4);
    Data.word_2 = XStats_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_MASK_V_DATA + 8);
    Data.word_3 = XStats_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_MASK_V_DATA + 12);
    return Data;
}

void XStats_top_Set_num_blocks_V(XStats_top *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XStats_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_NUM_BLOCKS_V_DATA, Data);
}

u32 XStats_top_Get_num_blocks_V(XStats_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XStats_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_NUM_BLOCKS_V_DATA);
    return Data;
}

void XStats_top_Set_src_inc_parity_V(XStats_top *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XStats_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_SRC_INC_PARITY_V_DATA, Data);
}

u32 XStats_top_Get_src_inc_parity_V(XStats_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XStats_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_SRC_INC_PARITY_V_DATA);
    return Data;
}

u32 XStats_top_Get_raw_berr_V(XStats_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XStats_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_RAW_BERR_V_DATA);
    return Data;
}

u32 XStats_top_Get_raw_blerr_V(XStats_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XStats_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_RAW_BLERR_V_DATA);
    return Data;
}

u32 XStats_top_Get_cor_berr_V(XStats_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XStats_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_COR_BERR_V_DATA);
    return Data;
}

u32 XStats_top_Get_cor_blerr_V(XStats_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XStats_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_COR_BLERR_V_DATA);
    return Data;
}

u32 XStats_top_Get_iter_cnt_V(XStats_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XStats_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_ITER_CNT_V_DATA);
    return Data;
}

u32 XStats_top_Get_block_cnt_V(XStats_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XStats_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_BLOCK_CNT_V_DATA);
    return Data;
}

void XStats_top_InterruptGlobalEnable(XStats_top *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XStats_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_GIE, 1);
}

void XStats_top_InterruptGlobalDisable(XStats_top *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XStats_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_GIE, 0);
}

void XStats_top_InterruptEnable(XStats_top *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XStats_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_IER);
    XStats_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_IER, Register | Mask);
}

void XStats_top_InterruptDisable(XStats_top *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XStats_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_IER);
    XStats_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_IER, Register & (~Mask));
}

void XStats_top_InterruptClear(XStats_top *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XStats_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_ISR, Mask);
}

u32 XStats_top_InterruptGetEnabled(XStats_top *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XStats_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_IER);
}

u32 XStats_top_InterruptGetStatus(XStats_top *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XStats_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XSTATS_TOP_CNTRL_ADDR_ISR);
}
