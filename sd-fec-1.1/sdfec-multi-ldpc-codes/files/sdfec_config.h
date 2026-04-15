/******************************************************************************
* Copyright (C) 2022-2025 Advanced Micro Devices, Inc. All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
#ifndef _SDFEC_CONFIG_H_
#define _SDFEC_CONFIG_H_

#define SDFEC_DEFAULT_CONFIG "/etc/sdfec.conf"
#define SDFEC_MAX_PATH 32
#define SDFEC_MAX_GPIO_NAME 16

struct sdfec_config {
	/* Base addresses of PL IP blocks */
	unsigned long data_dev_baseaddr;
	unsigned long stats_dev_baseaddr;
	unsigned long enc_ip_mon_dev_baseaddr;
	unsigned long enc_op_mon_dev_baseaddr;
	unsigned long dec_ip_mon_dev_baseaddr;
	unsigned long dec_op_mon_dev_baseaddr;

	/* GPIO line names */
	char gpio_pin_reset[SDFEC_MAX_GPIO_NAME];
	char gpio_pin_led0[SDFEC_MAX_GPIO_NAME];
	char gpio_pin_led1[SDFEC_MAX_GPIO_NAME];

	/* SD-FEC device nodes */
	char fec_dec[SDFEC_MAX_PATH];
	char fec_enc[SDFEC_MAX_PATH];
};

/*
 * Load configuration from file.
 * Returns 0 on success, -1 on error.
 * The struct is initialized with defaults before parsing,
 * so the config file only needs to override what changed.
 */
int sdfec_config_load(struct sdfec_config *cfg, const char *path);

#endif
