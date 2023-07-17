/*
  Copyright (c) 2023 Per Hallsmark <per@hallsmark.se>
  SPDX-License-Identifier: GPL-2.0
*/

#ifndef __NMEA2000_ENV_PARAM_H__
#define __NMEA2000_ENV_PARAM_H__

#define PGN_ENV_PARAM_ID 130311

struct __attribute__((packed)) {
	unsigned int SID:8;
	unsigned int temp_source:6;
	unsigned int humidity_source:2;
	unsigned int temperature:16;
	int humidity:16;
	unsigned int atmospheric_pressure:16;
} env_param;

#endif
