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
#include "xdata_source_top.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XData_source_top_CfgInitialize(XData_source_top *InstancePtr, XData_source_top_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Cntrl_BaseAddress = ConfigPtr->Cntrl_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#else

int XData_source_top_Initialize(XData_source_top *InstancePtr, u32 BaseAddr) {
    assert(InstancePtr != NULL);

    /* open /dev/mem descriptor for mapping the base address */
    if ((InstancePtr->mem_fd = open("/dev/mem", O_RDWR)) < 0) {
        printf("can't open /dev/mem on baseaddr: %x\n", BaseAddr);
        return XST_OPEN_DEVICE_FAILED;
    }

    /* memory map page size from the base address */
    InstancePtr->Cntrl_BaseAddress = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
        InstancePtr->mem_fd, BaseAddr);
    if (InstancePtr->Cntrl_BaseAddress == MAP_FAILED) {
        printf("register mmap error on baseaddr: %x\n", BaseAddr);
        close(InstancePtr->mem_fd);
        return XST_OPEN_DEVICE_FAILED;
    }

    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    InstancePtr->BaseAddr = BaseAddr;
    printf("OPEN mem map: %x\n", InstancePtr->BaseAddr);
    return XST_SUCCESS;
}

int XData_source_top_Release(XData_source_top *InstancePtr) {

    assert(InstancePtr != NULL);
    assert(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    /* unmap mapped memory and close the descriptor */
    munmap(InstancePtr->Cntrl_BaseAddress, MAP_SIZE);
    close(InstancePtr->mem_fd);
    printf("CLOSE mem map: %x\n", InstancePtr->BaseAddr);

    return XST_SUCCESS;
}

#endif


void XData_source_top_Start(XData_source_top *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XData_source_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_AP_CTRL) & 0x80;
    XData_source_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XData_source_top_IsDone(XData_source_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XData_source_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XData_source_top_IsIdle(XData_source_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XData_source_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XData_source_top_IsReady(XData_source_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XData_source_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XData_source_top_EnableAutoRestart(XData_source_top *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XData_source_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_AP_CTRL, 0x80);
}

void XData_source_top_DisableAutoRestart(XData_source_top *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XData_source_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_AP_CTRL, 0);
}

void XData_source_top_Set_fec_type_V(XData_source_top *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XData_source_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_FEC_TYPE_V_DATA, Data);
}

u32 XData_source_top_Get_fec_type_V(XData_source_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XData_source_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_FEC_TYPE_V_DATA);
    return Data;
}

void XData_source_top_Set_mod_type_V(XData_source_top *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XData_source_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_MOD_TYPE_V_DATA, Data);
}

u32 XData_source_top_Get_mod_type_V(XData_source_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XData_source_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_MOD_TYPE_V_DATA);
    return Data;
}

void XData_source_top_Set_skip_chan_V(XData_source_top *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XData_source_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_SKIP_CHAN_V_DATA, Data);
}

u32 XData_source_top_Get_skip_chan_V(XData_source_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XData_source_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_SKIP_CHAN_V_DATA);
    return Data;
}

void XData_source_top_Set_snr_V(XData_source_top *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XData_source_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_SNR_V_DATA, Data);
}

u32 XData_source_top_Get_snr_V(XData_source_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XData_source_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_SNR_V_DATA);
    return Data;
}

void XData_source_top_Set_inv_sigma_sq_V(XData_source_top *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XData_source_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_INV_SIGMA_SQ_V_DATA, Data);
}

u32 XData_source_top_Get_inv_sigma_sq_V(XData_source_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XData_source_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_INV_SIGMA_SQ_V_DATA);
    return Data;
}

void XData_source_top_Set_enc_ctrl_word_V(XData_source_top *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XData_source_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_ENC_CTRL_WORD_V_DATA, (u32)(Data));
    XData_source_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_ENC_CTRL_WORD_V_DATA + 4, (u32)(Data >> 32));
}

u64 XData_source_top_Get_enc_ctrl_word_V(XData_source_top *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XData_source_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_ENC_CTRL_WORD_V_DATA);
    Data += (u64)XData_source_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_ENC_CTRL_WORD_V_DATA + 4) << 32;
    return Data;
}

void XData_source_top_Set_dec_ctrl_word_V(XData_source_top *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XData_source_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_DEC_CTRL_WORD_V_DATA, (u32)(Data));
    XData_source_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_DEC_CTRL_WORD_V_DATA + 4, (u32)(Data >> 32));
}

u64 XData_source_top_Get_dec_ctrl_word_V(XData_source_top *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XData_source_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_DEC_CTRL_WORD_V_DATA);
    Data += (u64)XData_source_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_DEC_CTRL_WORD_V_DATA + 4) << 32;
    return Data;
}

void XData_source_top_Set_num_blocks_V(XData_source_top *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XData_source_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_NUM_BLOCKS_V_DATA, Data);
}

u32 XData_source_top_Get_num_blocks_V(XData_source_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XData_source_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_NUM_BLOCKS_V_DATA);
    return Data;
}

void XData_source_top_Set_source_words_V(XData_source_top *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XData_source_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_SOURCE_WORDS_V_DATA, Data);
}

u32 XData_source_top_Get_source_words_V(XData_source_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XData_source_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_SOURCE_WORDS_V_DATA);
    return Data;
}

void XData_source_top_Set_chan_symbls_V(XData_source_top *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XData_source_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_CHAN_SYMBLS_V_DATA, Data);
}

u32 XData_source_top_Get_chan_symbls_V(XData_source_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XData_source_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_CHAN_SYMBLS_V_DATA);
    return Data;
}

void XData_source_top_Set_chan_rem_V(XData_source_top *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XData_source_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_CHAN_REM_V_DATA, Data);
}

u32 XData_source_top_Get_chan_rem_V(XData_source_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XData_source_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_CHAN_REM_V_DATA);
    return Data;
}

void XData_source_top_Set_zero_data_V(XData_source_top *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XData_source_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_ZERO_DATA_V_DATA, Data);
}

u32 XData_source_top_Get_zero_data_V(XData_source_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XData_source_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_ZERO_DATA_V_DATA);
    return Data;
}

void XData_source_top_Set_source_keep_V(XData_source_top *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XData_source_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_SOURCE_KEEP_V_DATA, Data);
}

u32 XData_source_top_Get_source_keep_V(XData_source_top *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XData_source_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_SOURCE_KEEP_V_DATA);
    return Data;
}

void XData_source_top_Set_enc_keep_V(XData_source_top *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XData_source_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_ENC_KEEP_V_DATA, (u32)(Data));
    XData_source_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_ENC_KEEP_V_DATA + 4, (u32)(Data >> 32));
}

u64 XData_source_top_Get_enc_keep_V(XData_source_top *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XData_source_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_ENC_KEEP_V_DATA);
    Data += (u64)XData_source_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_ENC_KEEP_V_DATA + 4) << 32;
    return Data;
}

void XData_source_top_Set_dec_keep_V(XData_source_top *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XData_source_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_DEC_KEEP_V_DATA, (u32)(Data));
    XData_source_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_DEC_KEEP_V_DATA + 4, (u32)(Data >> 32));
}

u64 XData_source_top_Get_dec_keep_V(XData_source_top *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XData_source_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_DEC_KEEP_V_DATA);
    Data += (u64)XData_source_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_DEC_KEEP_V_DATA + 4) << 32;
    return Data;
}

void XData_source_top_InterruptGlobalEnable(XData_source_top *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XData_source_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_GIE, 1);
}

void XData_source_top_InterruptGlobalDisable(XData_source_top *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XData_source_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_GIE, 0);
}

void XData_source_top_InterruptEnable(XData_source_top *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XData_source_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_IER);
    XData_source_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_IER, Register | Mask);
}

void XData_source_top_InterruptDisable(XData_source_top *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XData_source_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_IER);
    XData_source_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_IER, Register & (~Mask));
}

void XData_source_top_InterruptClear(XData_source_top *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XData_source_top_WriteReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_ISR, Mask);
}

u32 XData_source_top_InterruptGetEnabled(XData_source_top *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XData_source_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_IER);
}

u32 XData_source_top_InterruptGetStatus(XData_source_top *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XData_source_top_ReadReg(InstancePtr->Cntrl_BaseAddress, XDATA_SOURCE_TOP_CNTRL_ADDR_ISR);
}
