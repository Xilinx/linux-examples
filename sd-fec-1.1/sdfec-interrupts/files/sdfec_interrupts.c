/******************************************************************************
* Copyright (C) 2016-2022 Xilinx, Inc. All rights reserved.
* Copyright (C) 2022-2025 Advanced Micro Devices, Inc. All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/****************************** Include Files *********************************/
#include "tstb_drivers.h"
#include "sdfec_usr_intf.h"

#define MAX_LINE_LEN 512

void xsdfec_calculate_shared_ldpc_table_entry_size(
	struct xsdfec_ldpc_params *ldpc,
	struct xsdfec_ldpc_param_table_sizes *table_sizes);
struct gpiod_line_request *find_chip_params(char *const line_name, int *offset, struct gpiod_chip *chip);

int main(int argc, char *argv[])
{
	int ret_val = -1;
	int enc_fd  = -1;
	int dec_fd  = -1;
	unsigned int code_id = 2;

	struct sdfec_config cfg;
	const char *config_path = (argc > 1) ? argv[1] : SDFEC_DEFAULT_CONFIG;

	ret_val = sdfec_config_load(&cfg, config_path);
	if (ret_val < 0) {
		printf("Failed to load config file: %s\n", config_path);
		exit(1);
	}

	XData_source_top data_source_top;

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

	printf("Initialize all devices\n");
	int offset_reset;
	struct gpiod_line_request *request_reset;
	struct gpiod_chip *chip_reset = NULL;

	request_reset = find_chip_params(cfg.gpio_pin_reset, &offset_reset, chip_reset);
	if (request_reset == NULL) {
		printf("find_chip_params() couldn't find RESET gpio\n");
		exit(1);
	}

	gpiod_line_request_set_value(request_reset, offset_reset, GPIOD_LINE_VALUE_INACTIVE);

	ret_val = tstb_data_source_initialize(&data_source_top, &cfg);
	if (ret_val < 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to initialize data source block");
		goto check_error;
	}
	/* Open FECs */
	dec_fd = open_xsdfec(cfg.fec_dec);
	if (dec_fd < 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to open dev %s",
			 cfg.fec_dec);
		goto release_data_source;
	}

	enc_fd = open_xsdfec(cfg.fec_enc);
	if (enc_fd < 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to open dev %s",
			 cfg.fec_enc);
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
			 cfg.fec_dec);
		goto close_devices;
	}

	ret_val = add_ldpc_xsdfec(dec_fd, &dec_ldpc_params);
	if (ret_val != 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to add ldpc code on dev %s",
			 cfg.fec_dec);
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
			 cfg.fec_enc);
		goto close_devices;
	}

	ret_val = add_ldpc_xsdfec(enc_fd, &enc_ldpc_params);
	if (ret_val != 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to add ldpc code on dev %s",
			 cfg.fec_enc);
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
			 cfg.fec_dec);
		goto close_devices;
	}
	/* Enable encoder interrupts */
	ret_val = set_irq_xsdfec(enc_fd, &irq);
	if (ret_val != 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to set irq on dev %s",
			 cfg.fec_enc);
		goto close_devices;
	}

	/* Start decoder FEC */
	ret_val = start_xsdfec(dec_fd);
	if (ret_val != 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to start dev %s",
			 cfg.fec_dec);
		goto close_devices;
	}

	/* Start encoder FEC */
	ret_val = start_xsdfec(enc_fd);
	if (ret_val != 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to start dev %s",
			 cfg.fec_enc);
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
			 cfg.fec_dec);
		goto close_devices;
	}
	print_status_xsdfec(&status, cfg.fec_dec);

	ret_val = get_stats_xsdfec(dec_fd, &stats);
	if (ret_val != 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to get stats on dev %s",
			 cfg.fec_dec);
		goto close_devices;
	}
	print_stats_xsdfec(&stats, cfg.fec_dec);

	ret_val = get_status_xsdfec(enc_fd, &status);
	if (ret_val != 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to get status on dev %s",
			 cfg.fec_enc);
		goto close_devices;
	}
	print_status_xsdfec(&status, cfg.fec_enc);

	ret_val = get_stats_xsdfec(enc_fd, &stats);
	if (ret_val != 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to get stats on dev %s",
			 cfg.fec_enc);
		goto close_devices;
	}
	print_stats_xsdfec(&stats, cfg.fec_enc);

	/* Clear the stats */
	ret_val = clear_stats_xsdfec(dec_fd);
	if (ret_val != 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to clear stats on dev %s",
			 cfg.fec_dec);
		goto close_devices;
	}
	ret_val = clear_stats_xsdfec(enc_fd);
	if (ret_val != 0) {
		snprintf(err_msg,
			 MAX_ERR_MSG_SIZE,
			 "Failed to clear stats on dev %s",
			 cfg.fec_enc);
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

check_error:
	gpiod_chip_close(chip_reset);
	/* Check for errors */
	if(strcmp(err_msg, "") != 0) {
		printf("ERROR: %s\n", err_msg);
		printf("\nsdfec-interrupts: FAIL\n");
		return -1;
	}

	printf("\nsdfec-interrupts: PASS\n");
	return 0;
}

static int chip_dir_filter(const struct dirent *entry)
{
	struct stat sb;
	int ret = 0;
	char *path;

	if (asprintf(&path, "/dev/%s", entry->d_name) < 0)
		return 0;

	if ((lstat(path, &sb) == 0) && (!S_ISLNK(sb.st_mode)) &&
	    gpiod_is_gpiochip_device(path))
		ret = 1;

	free(path);

	return ret;
}

static int all_chip_paths(char ***paths_ptr)
{
	int i, j, num_chips, ret = 0;
	struct dirent **entries;
	char **paths;

	num_chips = scandir("/dev/", &entries, chip_dir_filter, alphasort);
	if (num_chips < 0)
		return 0;

	paths = calloc(num_chips, sizeof(*paths));
	if (!paths)
		return 0;

	for (i = 0; i < num_chips; i++) {
		if (asprintf(&paths[i], "/dev/%s", entries[i]->d_name) < 0) {
			for (j = 0; j < i; j++)
				free(paths[j]);

			free(paths);
			return 0;
		}
	}

	*paths_ptr = paths;
	ret = num_chips;

	for (i = 0; i < num_chips; i++)
		free(entries[i]);

	free(entries);
	return ret;
}

struct gpiod_line_request *find_chip_params(char *const line_name, int *offset, struct gpiod_chip *chip)
{
	/* Example configuration - customize to suit your situation. */
	struct gpiod_chip_info *cinfo;
	int i, num_chips;
	char **chip_paths;
	struct gpiod_line_request *request;
	/*
	 * Names are not guaranteed unique, so this finds the first line with
	 * the given name.
	 */
	num_chips = all_chip_paths(&chip_paths);
	for (i = 0; i < num_chips; i++) {
		chip = gpiod_chip_open(chip_paths[i]);
		if (!chip)
			continue;

		*offset = gpiod_chip_get_line_offset_from_name(chip, line_name);
		if (*offset == -1)
			goto close_chip;

		cinfo = gpiod_chip_get_info(chip);
		if (!cinfo)
			goto close_chip;

		printf("%s: %s %d\n", line_name, gpiod_chip_info_get_name(cinfo), *offset);
		request = request_output_line(chip, *offset, GPIOD_LINE_VALUE_INACTIVE);
		return request;

close_chip:
		gpiod_chip_close(chip);
	}

	printf("line '%s' not found\n", line_name);
	return NULL;
}
