/*
  Copyright (c) 2023 Per Hallsmark <per@hallsmark.se>
  SPDX-License-Identifier: GPL-2.0
*/

#ifndef __NMEA2000_TEMPERATURE_H__
#define __NMEA2000_TEMPERATURE_H__

#define PGN_TEMPERATURE_ID 130312

struct __attribute__((packed)) {
	unsigned int SID:8;
	unsigned int instance:8;
	unsigned int source:8;
	unsigned int temperature:16;
	unsigned int set_temperature:16;
	unsigned int reserved:16;
} temperature;

#endif
