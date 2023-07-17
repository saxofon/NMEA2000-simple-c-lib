/*
  Copyright (c) 2023 Per Hallsmark <per@hallsmark.se>
  SPDX-License-Identifier: GPL-2.0
*/

#ifndef __NMEA2000_WATER_DEPTH_H__
#define __NMEA2000_WATER_DEPTH_H__

#define PGN_WATER_DEPTH_ID 128267

struct __attribute__((packed)) {
	unsigned int SID:8;
	unsigned int depth:32;
	int offset:16;
	unsigned int range:8;
} water_depth;

#endif
