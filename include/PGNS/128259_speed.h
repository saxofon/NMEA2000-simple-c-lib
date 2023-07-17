/*
  Copyright (c) 2023 Per Hallsmark <per@hallsmark.se>
  SPDX-License-Identifier: GPL-2.0
*/

#ifndef __NMEA2000_SPEED_H__
#define __NMEA2000_SPEED_H__

#define PGN_SPEED_ID 128259

struct __attribute__((packed)) {
	unsigned int SID:8;
	unsigned int speed_ref_water:16;
	unsigned int speed_ref_ground:16;
	unsigned int speed_ref_water_type:8;
	unsigned int speed_direction:4;
	unsigned int reserved:12;
} speed;

#endif
