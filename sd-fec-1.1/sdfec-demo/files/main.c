/******************************************************************************
* Copyright (C) 2016-2022 Xilinx, Inc. All rights reserved.
* Copyright (C) 2022-2025 Advanced Micro Devices, Inc. All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
#include "demo_test_drivers.h"
#include "sdfec_usr_intf.h"
#include "usr_params.h"

#define MAX_LINE_LEN 512

#define FEC_DEC  "/dev/xsdfec0"
#define FEC_ENC  "/dev/xsdfec1"

void xsdfec_calculate_shared_ldpc_table_entry_size(
	struct xsdfec_ldpc_params *ldpc,
	struct xsdfec_ldpc_param_table_sizes *table_sizes);
struct gpiod_line_request *find_chip_params(char *const line_name, int *offset, struct gpiod_chip *chip);

int main(void)
{
	int ret_val = -1;
	int enc_fd  = -1;
	int dec_fd  = -1;
	int valid_config_params = 0;

	unsigned int code_id = 0;

	XData_source_top  data_source_top;
	XStats_top        stats_top;
	struct demo_monitors_t mons;

	struct xsdfec_user_ldpc_table_offsets  dec_user_offsets;
	struct xsdfec_user_ldpc_table_offsets  enc_user_offsets;
	struct xsdfec_ldpc_params              dec_ldpc_params;
	struct xsdfec_ldpc_params              enc_ldpc_params;

	dec_user_offsets.sc_off = 0;
	dec_user_offsets.la_off = 0;
	dec_user_offsets.qc_off = 0;
	enc_user_offsets.sc_off = 0;
	enc_user_offsets.la_off = 0;
	enc_user_offsets.qc_off = 0;

	struct demo_params_t params      = { 0, 100, 6, 8, 0, 2, 0, 0 };
	struct demo_params_t prev_params = { 255, 0, 0, 0, 0, 0, 0, 0 };
	struct demo_stats_t  stats;

	int offset_led0, offset_led1, offset_reset;
	struct gpiod_line_request *request_led0, *request_led1, *request_reset;
	struct gpiod_chip *chip_led0 = NULL;
	struct gpiod_chip *chip_led1 = NULL;
	struct gpiod_chip *chip_reset = NULL;

	printf("Initialize all devices\n");
	request_led0 = find_chip_params(GPIO_PIN_LED0, &offset_led0, chip_led0);
	if (request_led0 == NULL) {
		printf("Failed find_chip_params() couldn't find LED0 gpios\n");
		exit(1);
	}

	request_led1 = find_chip_params(GPIO_PIN_LED1, &offset_led1, chip_led1);
	if (request_led1 == NULL) {
		printf("Failed find_chip_params() couldn't find LED1 gpios\n");
		exit(1);
	}

	request_reset = find_chip_params(GPIO_PIN_RESET, &offset_reset, chip_reset);
	if (request_reset == NULL) {
		printf("find_chip_params() couldn't find RESET gpio\n");
		exit(1);
	}

	gpiod_line_request_set_value(request_reset, offset_reset, GPIOD_LINE_VALUE_INACTIVE);

	ret_val = demo_data_source_initialize(&data_source_top);
	if (ret_val < 0) {
		printf("Failed to initialize data source block\n");
		exit(1);
	}
	ret_val = demo_stats_initialize(&stats_top);
	if (ret_val < 0) {
		printf("Failed to initialize stats block\n");
		exit(1);
	}
	ret_val = demo_monitors_initialize(&mons);
	if (ret_val < 0) {
		printf("Failed to initialize monitor blocks\n");
		exit(1);
	}

	// LED0 off
	gpiod_line_request_set_value(request_led0, offset_led0, GPIOD_LINE_VALUE_INACTIVE);

	printf("Open FECs\n");
	dec_fd = open_xsdfec(FEC_DEC);
	if (dec_fd < 0) {
		printf("Failed to open dev %s\n", FEC_DEC);
		goto release_monitors;
	}

	enc_fd = open_xsdfec(FEC_ENC);
	if (enc_fd < 0) {
		printf("Failed to open dev %s\n", FEC_ENC);
		goto release_monitors;
	}

	// LED0 on only
	gpiod_line_request_set_value(request_led0, offset_led0, GPIOD_LINE_VALUE_ACTIVE);

	// Fixed SD-FEC & tests parameters
	// Example design require in-order outputs.
	set_order_xsdfec(dec_fd, XSDFEC_MAINTAIN_ORDER);
	set_order_xsdfec(enc_fd, XSDFEC_MAINTAIN_ORDER);
	demo_data_source_set_fec_type(&data_source_top);
	do {
		gpiod_line_request_set_value(request_led1, offset_led1, GPIOD_LINE_VALUE_INACTIVE);
		if (params.code != prev_params.code) {
			printf("Setup LDPC code for decoder\n");
			ret_val = prepare_ldpc_code(dec_codes[params.code],
						    &dec_user_offsets,
						    &dec_ldpc_params, code_id);
			if (ret_val != 0) {
				printf("Failed to perpare user ldpc code\n");
				goto close_devices;
			}

			ret_val = add_ldpc_xsdfec(dec_fd, &dec_ldpc_params);
			if (ret_val != 0) {
				printf("Failed to add ldpc code\n");
				goto close_devices;
			}

			printf("Setup LDPC code for encoder\n");
			ret_val = prepare_ldpc_code(enc_codes[params.code],
						    &enc_user_offsets,
						    &enc_ldpc_params, code_id);
			if (ret_val != 0) {
				printf("Failed to perpare user ldpc code\n");
				goto close_devices;
			}

			ret_val = add_ldpc_xsdfec(enc_fd, &enc_ldpc_params);
			if (ret_val != 0) {
				printf("Failed to add ldpc code\n");
				goto close_devices;
			}
		}

		printf("Start decoder FEC\n");
		ret_val = start_xsdfec(dec_fd);
		if (ret_val != 0) {
			printf("Failed to start device\n");
			goto close_devices;
		}

		printf("Start encoder FEC\n");
		ret_val = start_xsdfec(enc_fd);
		if (ret_val != 0) {
			printf("Failed to start device\n");
			goto close_devices;
		}

		// Code sizes
		unsigned int k = dec_codes[params.code]->k;
		unsigned int n = dec_codes[params.code]->n;

		if(demo_params_changed(&params, &prev_params)) {
			printf("Setup data source, stats and monitors\n");
			//
			demo_data_source_setup(&data_source_top, &params, k, n);

			// Setup stats
			demo_stats_setup(&stats_top, &params, k, n);

			// Setup performance monitor blocks
			demo_monitors_setup(&mons, &params);
		}
		// LED1 on only
		gpiod_line_request_set_value(request_led0, offset_led0, GPIOD_LINE_VALUE_INACTIVE);
		gpiod_line_request_set_value(request_led1, offset_led1, GPIOD_LINE_VALUE_ACTIVE);

		// Start data source and stats
		demo_monitors_start(&mons);
		demo_stats_start(&stats_top);
		demo_data_source_start(&data_source_top);

		printf("Wait for finish\n");
		while (!demo_stats_is_idle(&stats_top)) { }

		printf("Collect stats\n");
		demo_stats_collect(&stats_top, &mons, &stats);

		// All LEDs on
		gpiod_line_request_set_value(request_led0, offset_led0, GPIOD_LINE_VALUE_ACTIVE);
		gpiod_line_request_set_value(request_led1, offset_led1, GPIOD_LINE_VALUE_ACTIVE);

		// Print results
		demo_stats_print(&stats);

		printf("Stop encoder FEC\n");
		ret_val = stop_xsdfec(enc_fd);
		if (ret_val  != 0) {
			printf("Failed to stop dev %s\n", FEC_ENC);
			goto close_devices;
		}

		printf("Stop decoder FEC\n");
		ret_val = stop_xsdfec(dec_fd);
		if (ret_val  != 0) {
			printf("Failed to stop dev %s\n", FEC_DEC);
			goto close_devices;
		}

		/*
		 * Get valid parameters, the following cases will result in valid
		 * parmeters:
		 *  - User continues with prev_params
		 *  - User enters valid params
		 * if invalid parameters are entered then the user will be reasked
		 * the above options.
		 */
		prev_params = params;
		do {
			valid_config_params = demo_params_get(&params);
		} while (valid_config_params == 0);

	} while (1);

close_devices:
	printf("Close FEC encoder\n");
	(void)close_xsdfec(enc_fd);
	printf("Close FEC decoder\n");
	(void)close_xsdfec(dec_fd);

release_monitors:
	demo_monitors_release(&mons);
	demo_stats_release(&stats_top);
	demo_data_source_release(&data_source_top);

	gpiod_chip_close(chip_led0);
	gpiod_chip_close(chip_led1);
	gpiod_chip_close(chip_reset);

	return 0;
}

static int chip_dir_filter(const struct dirent *entry)
{
	struct stat sb;
	int ret = 0;
	char *path;

	if (asprintf(&path, "/dev/%s", entry->d_name) < 0)
		return 0;

	if ((lstat(path, &sb) == 0) && (!S_ISLNK(sb.st_mode)) && gpiod_is_gpiochip_device(path))
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
