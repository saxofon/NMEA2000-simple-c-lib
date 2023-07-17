/*
  Copyright (c) 2023 Per Hallsmark <per@hallsmark.se>
  SPDX-License-Identifier: GPL-2.0
*/

#ifndef __NMEA2000_MAGNETIC_VARIATION_H__
#define __NMEA2000_MAGNETIC_VARIATION_H__

#define PGN_MAGNETIC_VARIATION_ID 127258

struct __attribute__((packed)) {
	unsigned int SID:8;
	unsigned int source:4;
	unsigned int reserved1:4;
	unsigned int age_of_service:16;
	int variation:16;
	unsigned int reserved2:16;
} magnetic_variation;

#endif
