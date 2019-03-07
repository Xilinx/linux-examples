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

#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xdata_source_top.h"

extern XData_source_top_Config XData_source_top_ConfigTable[];

XData_source_top_Config *XData_source_top_LookupConfig(u16 DeviceId) {
    XData_source_top_Config *ConfigPtr = NULL;

    int Index;

    for (Index = 0; Index < XPAR_XDATA_SOURCE_TOP_NUM_INSTANCES; Index++) {
        if (XData_source_top_ConfigTable[Index].DeviceId == DeviceId) {
            ConfigPtr = &XData_source_top_ConfigTable[Index];
            break;
        }
    }

    return ConfigPtr;
}

int XData_source_top_Initialize(XData_source_top *InstancePtr, u16 DeviceId) {
    XData_source_top_Config *ConfigPtr;

    Xil_AssertNonvoid(InstancePtr != NULL);

    ConfigPtr = XData_source_top_LookupConfig(DeviceId);
    if (ConfigPtr == NULL) {
        InstancePtr->IsReady = 0;
        return (XST_DEVICE_NOT_FOUND);
    }

    return XData_source_top_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif
