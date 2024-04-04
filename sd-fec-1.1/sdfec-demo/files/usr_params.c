/******************************************************************************
* Copyright (C) 2016-2022 Xilinx, Inc. All rights reserved.
* Copyright (C) 2022-2024 Advanced Micro Devices, Inc. All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/****************************** Include Files *********************************/
#include "usr_params.h"

/*************************** Type Definitions *********************************/
char err_msg[MAX_ERR_MSG_SIZE] = {0};

/* Parameter struct */
user_params* enc_codes[NUM_LDPC_CODES] = {
	&xsd_fec_enc_docsis_short_params,
	&xsd_fec_enc_docsis_medium_params,
	&xsd_fec_enc_docsis_long_params
};

user_params* dec_codes[NUM_LDPC_CODES] = {
	&xsd_fec_dec_docsis_short_params,
	&xsd_fec_dec_docsis_medium_params,
	&xsd_fec_dec_docsis_long_params
};
