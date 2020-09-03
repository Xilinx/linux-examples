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
/****************************** Include Files *********************************/
#include "tstb_drivers.h"
#include "sdfec_usr_intf.h"


void xsdfec_calculate_shared_ldpc_table_entry_size(
	struct xsdfec_ldpc_params *ldpc,
	struct xsdfec_ldpc_param_table_sizes *table_sizes);

int main(void)
{
	int ret_val = -1;
	int enc_fd  = -1;
	int dec_fd  = -1;
	unsigned int code_id = 2;

	XData_source_top data_source_top;

	struct metal_init_params metal_param = METAL_INIT_DEFAULTS;
	struct xsdfec_status status;
	struct xsdfec_stats  stats;
	struct xsdfec_irq    irq;
	struct xsdfec_user_ldpc_table_offsets dec_user_offsets;
	struct xsdfec_user_ldpc_table_offsets enc_user_offsets;
	struct xsdfec_ldpc_params             dec_ldpc_params;
	struct xsdfec_ldpc_params             enc_ldpc_params;

	dec_user_offsets.sc_off = 0;
	dec_user_offsets.la_off = 0;
	dec_user_offsets.qc_off = 0;
	enc_user_offsets.sc_off = 0;
	enc_user_offsets.la_off = 0;
	enc_user_offsets.qc_off = 0;

	struct tstb_params_t params = { code_id, 100, 6, 8, 0, 2, 0, 0 };

	/* Initialize all devices */
	ret_val = tstb_gpio_initialize(GPIO_RESET_ID);
	if (ret_val < 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to initialize gpio dev %s",
			 GPIO_RESET_ID);
		goto check_error;
	}
	metal_init(&metal_param);
	ret_val = tstb_data_source_initialize(&data_source_top);
	if (ret_val < 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to initialize data source block");
		goto check_error;
	}
	/* Open FECs */
	dec_fd = open_xsdfec(FEC_DEC);
	if (dec_fd < 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to open dev %s",
			 FEC_DEC);
		goto release_data_source;
	}

	enc_fd = open_xsdfec(FEC_ENC);
	if (enc_fd < 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to open dev %s",
			 FEC_ENC);
		goto release_data_source;
	}

	/* Fixed SD-FEC & tests parameters */
	/* Example design require in-order outputs */
	set_order_xsdfec(dec_fd, XSDFEC_MAINTAIN_ORDER);
	set_order_xsdfec(enc_fd, XSDFEC_MAINTAIN_ORDER);
	tstb_data_source_set_fec_type(&data_source_top);

	/* Setup LDPC code for decoder */
	ret_val = prepare_ldpc_code(dec_codes[params.code],
				    &dec_user_offsets,
				    &dec_ldpc_params,
				    code_id);
	if (ret_val != 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to prepare user ldpc code on dev %s",
			 FEC_DEC);
		goto close_devices;
	}

	ret_val = add_ldpc_xsdfec(dec_fd, &dec_ldpc_params);
	if (ret_val != 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to add ldpc code on dev %s",
			 FEC_DEC);
		goto close_devices;
	}

	/* Setup LDPC code for encoder */
	ret_val = prepare_ldpc_code(enc_codes[params.code],
				    &enc_user_offsets,
				    &enc_ldpc_params,
				    code_id);
	if (ret_val != 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to prepare user ldpc code on dev %s",
			 FEC_ENC);
		goto close_devices;
	}

	ret_val = add_ldpc_xsdfec(enc_fd, &enc_ldpc_params);
	if (ret_val != 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to add ldpc code on dev %s",
			 FEC_ENC);
		goto close_devices;
	}

	irq.enable_isr = true;
	irq.enable_ecc_isr = true;

	/* Enable decoder interrupts */
	ret_val = set_irq_xsdfec(dec_fd, &irq);
	if (ret_val != 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to set irq on dev %s",
			 FEC_DEC);
		goto close_devices;
	}
	/* Enable encoder interrupts */
	ret_val = set_irq_xsdfec(enc_fd, &irq);
	if (ret_val != 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to set irq on dev %s",
			 FEC_ENC);
		goto close_devices;
	}

	/* Start decoder FEC */
	ret_val = start_xsdfec(dec_fd);
	if (ret_val != 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to start dev %s",
			 FEC_DEC);
		goto close_devices;
	}

	/* Start encoder FEC */
	ret_val = start_xsdfec(enc_fd);
	if (ret_val != 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to start dev %s",
			 FEC_ENC);
		goto close_devices;
	}

	/* Generates an interrupt on both the Encoder and Decoder */
	trigger_interrupt(dec_codes[params.code],
			  &params,
			  &data_source_top);

	/* Poll for event */
	poll_event(dec_fd);
	poll_event(enc_fd);

	/*
	 * Get and print status
	 * Single bit errors are correctable
	 * Read and print the stats
	 */
	ret_val = get_status_xsdfec(dec_fd, &status);
	if (ret_val != 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to get status on dev %s",
			 FEC_DEC);
		goto close_devices;
	}
	print_status_xsdfec(&status);

	ret_val = get_stats_xsdfec(dec_fd, &stats);
	if (ret_val != 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to get stats on dev %s",
			 FEC_DEC);
		goto close_devices;
	}
	print_stats_xsdfec(&status, &stats);

	ret_val = get_status_xsdfec(enc_fd, &status);
	if (ret_val != 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to get status on dev %s",
			 FEC_ENC);
		goto close_devices;
	}
	print_status_xsdfec(&status);

	ret_val = get_stats_xsdfec(enc_fd, &stats);
	if (ret_val != 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to get stats on dev %s",
			 FEC_ENC);
		goto close_devices;
	}
	print_stats_xsdfec(&status, &stats);

	/* Clear the stats */
	ret_val = clear_stats_xsdfec(dec_fd);
	if (ret_val != 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to clear stats on dev %s",
			 FEC_DEC);
		goto close_devices;
	}
	ret_val = clear_stats_xsdfec(enc_fd);
	if (ret_val != 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to clear stats on dev %s",
			 FEC_ENC);
		goto close_devices;
	}

	/* Reset */
	tstb_gpio_reset(GPIO_RESET_ID);

	/* Set default configuration */
	ret_val = set_default_config_xsdfec(dec_fd);
	if (ret_val != 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to set default configuration on dev %s",
			 FEC_DEC);
		goto close_devices;
	}

	ret_val = set_default_config_xsdfec(enc_fd);
	if (ret_val != 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to set default configuration on dev %s",
			 FEC_ENC);
		goto close_devices;
	}

	irq.enable_isr = false;
	irq.enable_ecc_isr = false;

	/* Disable decoder interrupts */
	ret_val = set_irq_xsdfec(dec_fd, &irq);
	if (ret_val != 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to set irq on dev %s",
			 FEC_DEC);
		goto close_devices;
	}
	/* Disable encoder interrupts */
	ret_val = set_irq_xsdfec(enc_fd, &irq);
	if (ret_val != 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to set irq on dev %s",
			 FEC_ENC);
		goto close_devices;
	}

close_devices:
	/* Close FEC encoder */
	(void)close_xsdfec(enc_fd);
	/* Close FEC decoder */
	(void)close_xsdfec(dec_fd);

release_data_source:
	/* Release data source */
	tstb_data_source_release(&data_source_top);
	tstb_gpio_release(GPIO_RESET_ID);

check_error:
	/* Check for errors */
	if(strcmp(err_msg, "") != 0) {
		printf("ERROR: %s\n", err_msg);
		printf("\nsdfec-interrupts: FAIL\n");
		return -1;
	}

	printf("\nsdfec-interrupts: PASS\n");
	return 0;
}
