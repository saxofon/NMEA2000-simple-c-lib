/*
  Copyright (c) 2023 Per Hallsmark <per@hallsmark.se>
  SPDX-License-Identifier: GPL-2.0
*/

#ifndef __NMEA2000_RUDDER_H__
#define __NMEA2000_RUDDER_H__

#define PGN_RUDDER_ID 127245

struct __attribute__((packed)) {
	unsigned int instance:8;
	unsigned int direction_order:3;
	unsigned int reserved1:5;
	int angle_order:16;
	int position:16;
	unsigned int reserved2:16;
} rudder;

#endif
