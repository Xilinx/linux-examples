/******************************************************************************
* Copyright (C) 2022-2025 Advanced Micro Devices, Inc. All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "sdfec_config.h"

#define MAX_LINE 512

static int check_required_keys(struct sdfec_config *cfg)
{
	int ret = 0;

	if (!cfg->data_dev_baseaddr) {
		fprintf(stderr, "Missing config key: DATA_DEV_BASEADDR\n");
		ret = -1;
	}
	if (!cfg->stats_dev_baseaddr) {
		fprintf(stderr, "Missing config key: STATS_DEV_BASEADDR\n");
		ret = -1;
	}
	if (!cfg->enc_ip_mon_dev_baseaddr) {
		fprintf(stderr, "Missing config key: ENC_IP_MON_DEV_BASEADDR\n");
		ret = -1;
	}
	if (!cfg->enc_op_mon_dev_baseaddr) {
		fprintf(stderr, "Missing config key: ENC_OP_MON_DEV_BASEADDR\n");
		ret = -1;
	}
	if (!cfg->dec_ip_mon_dev_baseaddr) {
		fprintf(stderr, "Missing config key: DEC_IP_MON_DEV_BASEADDR\n");
		ret = -1;
	}
	if (!cfg->dec_op_mon_dev_baseaddr) {
		fprintf(stderr, "Missing config key: DEC_OP_MON_DEV_BASEADDR\n");
		ret = -1;
	}
	if (!cfg->gpio_pin_reset[0]) {
		fprintf(stderr, "Missing config key: GPIO_PIN_RESET\n");
		ret = -1;
	}
	if (!cfg->fec_dec[0]) {
		fprintf(stderr, "Missing config key: FEC_DEC\n");
		ret = -1;
	}
	if (!cfg->fec_enc[0]) {
		fprintf(stderr, "Missing config key: FEC_ENC\n");
		ret = -1;
	}

	return ret;
}

static char *strip(char *s)
{
	while (isspace((unsigned char)*s))
		s++;

	char *end = s + strlen(s) - 1;
	while (end > s && isspace((unsigned char)*end))
		*end-- = '\0';

	return s;
}

static int parse_addr(const char *val, unsigned long *out)
{
	char *endp;

	*out = strtoul(val, &endp, 0);
	if (endp == val || (*endp != '\0' && !isspace((unsigned char)*endp))) {
		fprintf(stderr, "Invalid address value: %s\n", val);
		return -1;
	}

	return 0;
}

static void set_str(char *dst, const char *src, size_t sz)
{
	memset(dst, 0, sz);
	strncpy(dst, src, sz - 1);
}

int sdfec_config_load(struct sdfec_config *cfg, const char *path)
{
	FILE *fp;
	char line[MAX_LINE];

	memset(cfg, 0, sizeof(*cfg));

	fp = fopen(path, "r");
	if (!fp) {
		fprintf(stderr, "Cannot open config file: %s\n", path);
		return -1;
	}

	while (fgets(line, sizeof(line), fp)) {
		char *s = strip(line);

		/* Skip comments and blank lines */
		if (*s == '#' || *s == '\0')
			continue;

		char *eq = strchr(s, '=');
		if (!eq) {
			fprintf(stderr, "Malformed config line: %s\n", s);
			fclose(fp);
			return -1;
		}

		*eq = '\0';
		char *key = strip(s);
		char *val = strip(eq + 1);

		if (strcmp(key, "DATA_DEV_BASEADDR") == 0) {
			if (parse_addr(val, &cfg->data_dev_baseaddr))
				goto err;
		} else if (strcmp(key, "STATS_DEV_BASEADDR") == 0) {
			if (parse_addr(val, &cfg->stats_dev_baseaddr))
				goto err;
		} else if (strcmp(key, "ENC_IP_MON_DEV_BASEADDR") == 0) {
			if (parse_addr(val, &cfg->enc_ip_mon_dev_baseaddr))
				goto err;
		} else if (strcmp(key, "ENC_OP_MON_DEV_BASEADDR") == 0) {
			if (parse_addr(val, &cfg->enc_op_mon_dev_baseaddr))
				goto err;
		} else if (strcmp(key, "DEC_IP_MON_DEV_BASEADDR") == 0) {
			if (parse_addr(val, &cfg->dec_ip_mon_dev_baseaddr))
				goto err;
		} else if (strcmp(key, "DEC_OP_MON_DEV_BASEADDR") == 0) {
			if (parse_addr(val, &cfg->dec_op_mon_dev_baseaddr))
				goto err;
		} else if (strcmp(key, "GPIO_PIN_RESET") == 0) {
			set_str(cfg->gpio_pin_reset, val, sizeof(cfg->gpio_pin_reset));
		} else if (strcmp(key, "GPIO_PIN_LED0") == 0) {
			set_str(cfg->gpio_pin_led0, val, sizeof(cfg->gpio_pin_led0));
		} else if (strcmp(key, "GPIO_PIN_LED1") == 0) {
			set_str(cfg->gpio_pin_led1, val, sizeof(cfg->gpio_pin_led1));
		} else if (strcmp(key, "FEC_DEC") == 0) {
			set_str(cfg->fec_dec, val, sizeof(cfg->fec_dec));
		} else if (strcmp(key, "FEC_ENC") == 0) {
			set_str(cfg->fec_enc, val, sizeof(cfg->fec_enc));
		} else {
			fprintf(stderr, "Unknown config key: %s\n", key);
		}
	}

	fclose(fp);
	return check_required_keys(cfg);

err:
	fclose(fp);
	return -1;
}
