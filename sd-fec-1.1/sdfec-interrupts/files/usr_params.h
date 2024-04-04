/******************************************************************************
* Copyright (C) 2016-2022 Xilinx, Inc. All rights reserved.
* Copyright (C) 2022-2024 Advanced Micro Devices, Inc. All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
#ifndef _USR_PARAMS_H_
#define _USR_PARAMS_H_
/****************************** Include Files *********************************/
#include "xsd_fec_dec_docsis_short_params.h"
#include "xsd_fec_dec_docsis_medium_params.h"
#include "xsd_fec_dec_docsis_long_params.h"
#include "xsd_fec_enc_docsis_short_params.h"
#include "xsd_fec_enc_docsis_medium_params.h"
#include "xsd_fec_enc_docsis_long_params.h"

/*************************** Constant Definitions *****************************/
/* Constants used to access devices other than the sd-fec */
#define NUM_LDPC_CODES		(3UL)
#define MAX_ERR_MSG_SIZE	(256UL)
#define FEC_DEC			"/dev/xsdfec0"
#define FEC_ENC			"/dev/xsdfec1"

/*************************** Macro Definitions ********************************/

/* Informative message */
#define info_msg(...) printf(__VA_ARGS__)

extern char err_msg[MAX_ERR_MSG_SIZE];
extern user_params* enc_codes[NUM_LDPC_CODES];
extern user_params* dec_codes[NUM_LDPC_CODES];

#endif
