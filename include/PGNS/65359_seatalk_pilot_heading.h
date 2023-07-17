/*
  Copyright (c) 2023 Per Hallsmark <per@hallsmark.se>
  SPDX-License-Identifier: GPL-2.0
*/

#ifndef __NMEA2000_SEATALK_PILOT_HEADING_H__
#define __NMEA2000_SEATALK_PILOT_HEADING_H__

#define PGN_SEATALK_PILOT_HEADING_ID 65359

struct __attribute__((packed)) {
	unsigned int manufacturer_code:11;
	unsigned int reserved1:2;
	unsigned int industry_code:3;
	unsigned int SID:8;
	unsigned int heading_true:16;
	unsigned int heading_magnetic:16;
	unsigned int reserved2:8;
} seatalk_pilot_heading;

#endif
