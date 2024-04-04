/******************************************************************************
* Copyright (C) 2016-2022 Xilinx, Inc. All rights reserved.
* Copyright (C) 2022-2024 Advanced Micro Devices, Inc. All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/****************************** Include Files *********************************/
#include "tstb_drivers.h"
#include "sdfec_usr_intf.h"

static void run_testbench(user_params* code,
			  struct tstb_params_t* params,
			  struct tstb_stats_t* stats,
			  XData_source_top* data_source_top,
			  XStats_top* stats_top,
			  struct tstb_monitors_t* mons,
			  unsigned int code_id);

void xsdfec_calculate_shared_ldpc_table_entry_size(
	struct xsdfec_ldpc_params *ldpc,
	struct xsdfec_ldpc_param_table_sizes *table_sizes);

int main(void)
{
	int ret_val = -1;
	int enc_fd  = -1;
	int dec_fd  = -1;
	unsigned int code_id = 0;
	int gpiochip_num_reset;

	XData_source_top       data_source_top;
	XStats_top             stats_top;
	struct tstb_monitors_t mons;

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
	struct tstb_stats_t  stats;

	printf("Initialize all devices\n");
	gpiochip_num_reset = gpio_find_chipname(GPIO_PIN_RESET);
	if (gpiochip_num_reset < 0) {
		printf("Failed to find gpiochip for reset line\n");
		exit(1);
	}
	ret_val = gpio_write_value(gpiochip_num_reset, GPIO_PIN_RESET, 0);
	if (ret_val < 0) {
		snprintf(err_msg, MAX_ERR_MSG_SIZE, "Failed on RESET gpio ");
		goto check_error;
	}
	ret_val = tstb_data_source_initialize(&data_source_top);
	if (ret_val < 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to initialize data source block");
		goto check_error;
	}
	ret_val = tstb_stats_initialize(&stats_top);
	if (ret_val < 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to initialize stats block");
		goto check_error;
	}
	ret_val = tstb_monitors_initialize(&mons);
	if (ret_val < 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to initialize monitor blocks");
		goto check_error;
	}

	/* Open FECs */
	dec_fd = open_xsdfec(FEC_DEC);
	if (dec_fd < 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to open dev %s",
			 FEC_DEC);
		goto release_tstb_devices;
	}

	enc_fd = open_xsdfec(FEC_ENC);
	if (enc_fd < 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to open dev %s",
			 FEC_ENC);
		goto release_tstb_devices;
	}

	/* Fixed SD-FEC & tests parameters */
	/* Example design require in-order outputs */
	set_order_xsdfec(dec_fd, XSDFEC_MAINTAIN_ORDER);
	set_order_xsdfec(enc_fd, XSDFEC_MAINTAIN_ORDER);

	/* Add LDPC Codes where 0 is short, 1 is medium and 2 is long */
	for (code_id = 0; code_id < NUM_LDPC_CODES; code_id++) {
		/* Setup LDPC code for decoder */
		params.code = code_id;
		ret_val = load_ldpc_code(dec_fd,
					 dec_codes[params.code],
					 &dec_user_offsets,
					 &dec_ldpc_params,
					 code_id,
					 FEC_DEC);
		if (ret_val != 0) {
			goto close_devices;
		}

		ret_val = load_ldpc_code(enc_fd,
					 enc_codes[params.code],
					 &enc_user_offsets,
					 &enc_ldpc_params,
					 code_id,
					 FEC_ENC);
		if (ret_val != 0) {
			goto close_devices;
		}
	}
	for (unsigned int code_id = 0; code_id < NUM_LDPC_CODES; code_id++) {
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

		params.code = code_id;
		run_testbench(dec_codes[params.code],
			      &params,
			      &stats,
			      &data_source_top,
			      &stats_top,
			      &mons,
			      code_id);

		/* Stop decoder FEC */
		ret_val = stop_xsdfec(dec_fd);
		if (ret_val != 0) {
			snprintf(err_msg,
				 MAX_ERR_MSG_SIZE,
				 "Failed to stop dev %s",
				 FEC_DEC);
			goto close_devices;
		}

		/* Stop encoder FEC */
		ret_val = stop_xsdfec(enc_fd);
		if (ret_val != 0) {
			snprintf(err_msg,
				 MAX_ERR_MSG_SIZE,
				 "Failed to stop dev %s",
				 FEC_ENC);
			goto close_devices;
		}
	}

close_devices:
	/* Close FEC encoder */
	(void)close_xsdfec(enc_fd);
	/* Close FEC decoder */
	(void)close_xsdfec(dec_fd);

release_tstb_devices:
	tstb_monitors_release(&mons);
	tstb_stats_release(&stats_top);
	tstb_data_source_release(&data_source_top);

check_error:
	/* Check for errors */
	if(strcmp(err_msg, "") != 0) {
		printf("ERROR: %s\n", err_msg);
		printf("\nsdfec-multi-ldpc-codes: FAIL\n");
		return -1;
	}

	printf("\nsdfec-multi-ldpc-codes: PASS\n");
	return 0;
}

/* Only use this function for multiple_ldpc_code test */
static void run_testbench(user_params* dec_code,
			  struct tstb_params_t* params,
			  struct tstb_stats_t* stats,
			  XData_source_top* data_source_top,
			  XStats_top* stats_top,
			  struct tstb_monitors_t* mons,
			  unsigned int code_id) {
	int ret = -1;
	unsigned int k;
	unsigned int n;
	unsigned int enc_code_size;
	unsigned int dec_code_size;

	/* Code sizes */
	k = dec_code->k;
	n = dec_code->n;
	enc_code_size = code_id;
	dec_code_size = code_id;

	/*
	 * Release testbench drivers after each run so that data blocks are
	 * cleared This is not necessary for the first run but it's left in for
	 * consistency.
	 */
	tstb_monitors_release(mons);
	tstb_stats_release(stats_top);
	tstb_data_source_release(data_source_top);

	ret = tstb_data_source_initialize(data_source_top);
	if (ret < 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to initialize data source block");
	}
	ret = tstb_stats_initialize(stats_top);
	if (ret < 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to initialize stats block");
	}
	ret = tstb_monitors_initialize(mons);
	if (ret < 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to initialize monitor blocks");
	}

	tstb_data_source_set_fec_type(data_source_top);

	/* Setup data source */
	tstb_data_source_setup(data_source_top,
			       params,
			       k,
			       n,
			       enc_code_size,
			       dec_code_size);
	/* Setup stats */
	tstb_stats_setup(stats_top, params, k, n);

	/* Setup performance monitor blocks */
	tstb_monitors_setup(mons, params);

	/* Start data source, monitor and stats block */
	tstb_monitors_start(mons);
	tstb_stats_start(stats_top);
	tstb_data_source_start(data_source_top);

	info_msg("Wait for stats to finish...\n"
		 "* Test may hang if there is a critical error. "
		 " A reboot is required if there is a long delay *\n");
	while (!tstb_stats_is_idle(stats_top)) { }

	/* Collect stats */
	tstb_stats_collect(stats_top, mons, stats);

	/* Print results */
	printf("\n--- LDPC CODE IDX %u ---", code_id);
	tstb_stats_print(stats);
}
