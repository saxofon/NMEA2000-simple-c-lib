/*
  Copyright (c) 2023 Per Hallsmark <per@hallsmark.se>
  SPDX-License-Identifier: GPL-2.0
*/

#ifndef __NMEA2000_VESSEL_HEADING_H__
#define __NMEA2000_VESSEL_HEADING_H__

#define PGN_VESSEL_HEADING_ID 127250

struct __attribute__((packed)) {
	unsigned int SID:8;
	unsigned int heading:16;
	int deviation:16;
	int variation:16;
	unsigned int reference:2;
	unsigned int reserved:6;
} vessel_heading;

#endif
