/******************************************************************************
* Copyright (C) 2016-2022 Xilinx, Inc. All rights reserved.
* Copyright (C) 2022-2025 Advanced Micro Devices, Inc. All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/****************************** Include Files *********************************/
#include "tstb_drivers.h"

/************************** Function Implementation ***************************/

/*
 * Helper function to determine tkeep for various data streams in the example
 * design
 *      o Determined in S/W to simplify H/W
 *      o tkeep indicates bytes
 */
int tstb_data_source_initialize(XData_source_top* data_source_top,
				struct sdfec_config *cfg)
{
	int ret;
	ret = XData_source_top_Initialize(data_source_top,
					  cfg->data_dev_baseaddr);

	return ret;
}

u64 tstb_data_source_calc_keep(unsigned int packet_size,
			       unsigned int per_trans,
			       bool is_bits)
{
	unsigned int rem = packet_size % per_trans;

	if (is_bits)
		rem = (rem + 7) / 8;
	/* If no remainder keep all on last transaction */
	if (rem == 0)
		return 0xFFFFFFFF; // Max
	else
		return ((u64)1 << rem) - 1;
}

void tstb_data_source_set_fec_type(XData_source_top* data_source_top)
{
	XData_source_top_Set_fec_type_V(data_source_top, 0); // LDPC = 0
}

void tstb_data_source_setup(XData_source_top* data_source_top,
			    struct tstb_params_t* params,
			    unsigned int k,
			    unsigned int n,
			    unsigned int enc_code,
			    unsigned int dec_code)
{
	int factorial_10bit;
	int factorial_11bit;
	u64 word = 0;
	int chan_symbls;
	int chan_rem;

	XData_source_top_Set_zero_data_V(data_source_top, params->zero_data);
	XData_source_top_Set_mod_type_V(data_source_top, params->mod_type);
	XData_source_top_Set_skip_chan_V(data_source_top, params->skip_chan);

	factorial_10bit = (int)params->snr*2048;
	factorial_11bit = (int)powf(10.0, (params->snr*0.1))*1024;
	XData_source_top_Set_snr_V(data_source_top, factorial_10bit);
	XData_source_top_Set_inv_sigma_sq_V(data_source_top, factorial_11bit);


	word += enc_code << 0; // Code
	word += 1 << 14; // Hard output
	XData_source_top_Set_enc_ctrl_word_V(data_source_top, word);
	word = 0;
	word += dec_code << 0; // Code
	word += 1 << 14; // Hard output
	word += params->term_on_pass << 16; // Term on pass
	word += params->max_iter << 18; // Max iter
	XData_source_top_Set_dec_ctrl_word_V(data_source_top, word);
	XData_source_top_Set_num_blocks_V(data_source_top, params->num_blocks);
	XData_source_top_Set_source_words_V(data_source_top, (int)(k+127)/128);
	XData_source_top_Set_source_keep_V(data_source_top,
					  (u32)tstb_data_source_calc_keep(k,
									  128,
									  true));
	XData_source_top_Set_enc_keep_V(data_source_top,
					tstb_data_source_calc_keep(n,
								   96,
								   true));
	XData_source_top_Set_dec_keep_V(data_source_top,
					tstb_data_source_calc_keep(k,
								   128,
								   true));

	switch (params->mod_type) {
		case 0: {
			chan_symbls = (n+3)/4; // 1 (BPSK) symbol x 4
			chan_rem    = n % 4;
			break;
		}
		case 1: {
			chan_symbls = (n+7)/8; // 2 (QPSK/QAM4) symbols x 4
			chan_rem    = n % 8;
			break;
		}
		case 2: {
			chan_symbls = (n+11)/12; // 4 (QAM16) symbols x 3
			chan_rem    = n % 12;
			break;
		}
		case 3: {
			chan_symbls = (n+23)/24; // 6 (QAM64) symbols x 4
			chan_rem    = n % 24;
			break;
		}
		default: {
			chan_symbls = (n+3)/4; // 1 (BPSK) symbol x 4
			chan_rem    = n % 4;
			break;
		}
	}

	XData_source_top_Set_chan_symbls_V(data_source_top, chan_symbls);
	XData_source_top_Set_chan_rem_V(data_source_top, chan_rem);
}

void tstb_data_source_start(XData_source_top* data_source_top)
{
    XData_source_top_Start(data_source_top);
}

void tstb_data_source_release(XData_source_top* data_source_top)
{
	XData_source_top_Release(data_source_top);
}

int tstb_monitors_initialize(struct tstb_monitors_t* mons,
			     struct sdfec_config *cfg)
{
	int ret_enc_ip;
	int ret_enc_op;
	int ret_dec_ip;
	int ret_dec_op;
	int ret = 0;

	ret_enc_ip = XMonitor_Initialize(&(mons->enc_ip),
					 cfg->enc_ip_mon_dev_baseaddr);
	ret_enc_op = XMonitor_Initialize(&(mons->enc_op),
					 cfg->enc_op_mon_dev_baseaddr);
	ret_dec_ip = XMonitor_Initialize(&(mons->dec_ip),
					 cfg->dec_ip_mon_dev_baseaddr);
	ret_dec_op = XMonitor_Initialize(&(mons->dec_op),
					 cfg->dec_op_mon_dev_baseaddr);

	if (ret_enc_ip < 0 || ret_enc_op < 0 ||
	    ret_dec_ip < 0 || ret_dec_op < 0) {
		ret = ret_enc_ip | ret_enc_op | ret_dec_ip | ret_dec_op;
	}
	return ret;
}

void tstb_monitors_setup(struct tstb_monitors_t* mons,
			 struct tstb_params_t* params)
{
	XMonitor_Set_num_blocks_V(&(mons->enc_ip), params->num_blocks);
	XMonitor_Set_num_blocks_V(&(mons->enc_op), params->num_blocks);
	XMonitor_Set_num_blocks_V(&(mons->dec_ip), params->num_blocks);
	XMonitor_Set_num_blocks_V(&(mons->dec_op), params->num_blocks);
}

void tstb_monitors_start(struct tstb_monitors_t* mons)
{
	XMonitor_Start(&(mons->enc_ip));
	XMonitor_Start(&(mons->enc_op));
	XMonitor_Start(&(mons->dec_ip));
	XMonitor_Start(&(mons->dec_op));
}

void tstb_monitors_release(struct tstb_monitors_t* mons)
{
	XMonitor_Release(&(mons->dec_op));
	XMonitor_Release(&(mons->dec_ip));
	XMonitor_Release(&(mons->enc_op));
	XMonitor_Release(&(mons->enc_ip));
}

int tstb_stats_initialize(XStats_top* stats_top,
			  struct sdfec_config *cfg)
{
	int ret;

	ret = XStats_top_Initialize(stats_top, cfg->stats_dev_baseaddr);

	return ret;
}

// Helper function to determine the mask for the stats block to apply to the
// last comparison
XStats_top_Mask_v tstb_stats_calc_mask(unsigned int k)
{
	int bits = k % 128;
	XStats_top_Mask_v mask;

	if (bits == 0) {
		mask.word_0 = 0xFFFFFFFF;
		mask.word_1 = 0xFFFFFFFF;
		mask.word_2 = 0xFFFFFFFF;
		mask.word_3 = 0xFFFFFFFF;
		return mask;
	}
	if (bits >= 32) {
		mask.word_0 = 0xFFFFFFFF;
		bits -= 32;
	} else {
		mask.word_0 = 0x7FFFFFFF >> (31-bits); // Much faster
		bits = 0;
	}
	if (bits >= 32) {
		mask.word_1 = 0xFFFFFFFF;
		bits -= 32;
	} else {
		mask.word_1 = 0x7FFFFFFF >> (31-bits);
		bits = 0;
	}
	if (bits >= 32) {
		mask.word_2 = 0xFFFFFFFF;
		bits -= 32;
	} else {
		mask.word_2 = 0x7FFFFFFF >> (31-bits);
		bits = 0;
	}
	if (bits >= 32) {
		mask.word_3 = 0xFFFFFFFF;
		bits -= 32;
	} else {
		mask.word_3 = 0x7FFFFFFF >> (31-bits);
		bits = 0;
	}
	return mask;
}

void tstb_stats_setup(XStats_top* stats_top,
		 struct tstb_params_t* params,
		 unsigned int k,
		 unsigned int n)
{
	XStats_top_Set_num_blocks_V(stats_top, params->num_blocks);
	XStats_top_Set_k_V(stats_top, k);
	XStats_top_Set_n_V(stats_top, n);
	XStats_top_Set_mask_V(stats_top, tstb_stats_calc_mask(k));
	XStats_top_Set_src_inc_parity_V(stats_top, 0);
}

void tstb_stats_start(XStats_top* stats_top)
{
	XStats_top_Start(stats_top);
}

u32 tstb_stats_is_idle(XStats_top* stats_top)
{
	return XStats_top_IsIdle(stats_top);
}

void tstb_stats_collect(XStats_top* stats_top,
		   struct tstb_monitors_t* mons,
		   struct tstb_stats_t* stats)
{
	stats->raw_berr  = XStats_top_Get_raw_berr_V(stats_top);
	stats->raw_blerr = XStats_top_Get_raw_blerr_V(stats_top);
	stats->cor_berr  = XStats_top_Get_cor_berr_V(stats_top);
	stats->cor_blerr = XStats_top_Get_cor_blerr_V(stats_top);
	stats->iter_cnt  = XStats_top_Get_iter_cnt_V(stats_top);
	stats->block_cnt = XStats_top_Get_block_cnt_V(stats_top);

	stats->enc_ip_first   = XMonitor_Get_first_V(&(mons->enc_ip));
	stats->enc_ip_last    = XMonitor_Get_last_V(&(mons->enc_ip));
	stats->enc_ip_stalled = XMonitor_Get_stalled_V(&(mons->enc_ip));
	stats->enc_op_first   = XMonitor_Get_first_V(&(mons->enc_op));
	stats->enc_op_last    = XMonitor_Get_last_V(&(mons->enc_op));
	stats->enc_op_stalled = XMonitor_Get_stalled_V(&(mons->enc_op));
	stats->dec_ip_first   = XMonitor_Get_first_V(&(mons->dec_ip));
	stats->dec_ip_last    = XMonitor_Get_last_V(&(mons->dec_ip));
	stats->dec_ip_stalled = XMonitor_Get_stalled_V(&(mons->dec_ip));
	stats->dec_op_first   = XMonitor_Get_first_V(&(mons->dec_op));
	stats->dec_op_last    = XMonitor_Get_last_V(&(mons->dec_op));
	stats->dec_op_stalled = XMonitor_Get_stalled_V(&(mons->dec_op));
}

void tstb_stats_print(struct tstb_stats_t* stats)
{
	printf("\n---- RESULTS ----\n");
	printf("\tStats: \n");
	printf("\t\tDecode iteration count = %u\n", stats->iter_cnt);
	printf("\t\tChannel bit error count = %u\n", stats->raw_berr);
	printf("\t\tChannel block error count = %u\n", stats->raw_blerr);
	printf("\t\tUncorrected bit error count after Decode = %u\n",
		stats->cor_berr);
	printf("\t\tUncorrected block error count after Decode = %u\n",
		stats->cor_blerr);
	printf("\tEncoder Monitors:\n");
	printf("\t\tIn Monitor First Timestamp = %u\n", stats->enc_ip_first);
	printf("\t\tIn Monitor Last Timestamp = %u\n", stats->enc_ip_last);
	printf("\t\tIn Monitor Num Stalled Clk Cycles = %u\n",
		stats->enc_ip_stalled);
	printf("\t\tOut Monitor First Timestamp = %u\n", stats->enc_op_first);
	printf("\t\tOut Monitor Last Timestamp =  %u\n", stats->enc_op_last);
	printf("\t\tOut Monitor Num Stalled Clk Cycles = %u\n",
		stats->enc_op_stalled);
	printf("\tDecoder Monitors:\n");
	printf("\t\tIn Monitor First Timestamp = %u\n", stats->dec_ip_first);
	printf("\t\tIn Monitor Last Timestamp = %u\n", stats->dec_ip_last);
	printf("\t\tIn Monitor Num Stalled Clk Cycles = %u\n",
		stats->dec_ip_stalled);
	printf("\t\tOut Monitor First Timestamp = %u\n", stats->dec_op_first);
	printf("\t\tOut Monitor Last Timestamp =  %u\n", stats->dec_op_last);
	printf("\t\tOut Monitor Num Stalled Clk Cycles = %u\n",
		stats->dec_op_stalled);
}

void tstb_stats_release(XStats_top* stats_top)
{
	XStats_top_Release(stats_top);
}

struct gpiod_line_request *request_output_line(struct gpiod_chip *chip, unsigned int offset, enum gpiod_line_value value)
{
	struct gpiod_request_config *req_cfg = NULL;
	struct gpiod_line_request *request = NULL;
	struct gpiod_line_settings *settings;
	struct gpiod_line_config *line_cfg;
	int ret;

	settings = gpiod_line_settings_new();
	if (!settings)
		return NULL;

	gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);
	gpiod_line_settings_set_output_value(settings, value);

	line_cfg = gpiod_line_config_new();
	if (!line_cfg)
		goto free_settings;

	ret = gpiod_line_config_add_line_settings(line_cfg, &offset, 1, settings);
	if (ret)
		goto free_line_config;

	request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);

	gpiod_request_config_free(req_cfg);

free_line_config:
	gpiod_line_config_free(line_cfg);

free_settings:
	gpiod_line_settings_free(settings);

	return request;
}

int load_ldpc_code(int fd,
		   user_params* code,
		   struct xsdfec_user_ldpc_table_offsets* user_offsets,
		   struct xsdfec_ldpc_params* ldpc_params,
		   unsigned int code_id,
		   char* dev) {
	int ret = -1;

	ret = prepare_ldpc_code(code, user_offsets, ldpc_params, code_id);
	if (ret != 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to prepare user ldpc code on dev %s",
			 dev);
	}

	ret = add_ldpc_xsdfec(fd, ldpc_params);
	if (ret != 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to add ldpc code on dev %s",
			 dev);
	}

	/* updates offsets so table entries are in sequence */
	ret = update_lpdc_table_offsets(ldpc_params, user_offsets);
	if (ret != 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to update ldpc table offsets on dev %s",
			 dev);
	}

	return ret;
}

void trigger_interrupt(user_params* dec_code,
		       struct tstb_params_t* params,
		       XData_source_top* data_source_top)
{
	(void)dec_code;
	unsigned int k;
	unsigned int n;
	unsigned int enc_code_size;
	unsigned int dec_code_size;

	/* Code sizes */
	k = dec_code->k;
	n = dec_code->n;

	/*
	 * Specify a Code word size of 4 (doesn't exist) for both the Encoder
	 * and Decoder
	 */
	enc_code_size = 4;
	dec_code_size = 4;

	/* Setup data source */
	tstb_data_source_setup(data_source_top,
			       params,
			       k,
			       n,
			       enc_code_size,
			       dec_code_size);

	/* Start data source and trigger interrupt */
	tstb_data_source_start(data_source_top);
}

void poll_event(int fd) {
	int ret;
	struct pollfd pfd;

	/* Set up the poll */
	pfd.fd = fd;
	pfd.events = POLLIN | POLLPRI | POLLRDNORM;

	/* poll for event */
	ret = poll(&pfd, 1, 1000);
	if (ret <= 0) {
		snprintf(err_msg, MAX_ERR_MSG_SIZE, "Poll for event timeout");
	}
	else {
		/* An event on fd has occurred. */
		if (pfd.revents & pfd.events) {
			info_msg("%s: Poll caught correct event.\n", __func__);
		}
		else {
			snprintf(err_msg,
				 MAX_ERR_MSG_SIZE,
				 "Incorrect poll event occured");
		}
	}
}
