/*
  Copyright (c) 2023 Per Hallsmark <per@hallsmark.se>
  SPDX-License-Identifier: GPL-2.0
*/

#ifndef __NMEA2000_WIND_DATA_H__
#define __NMEA2000_WIND_DATA_H__

#define PGN_WIND_DATA_ID 130306

#define PGN_WIND_DATA_SPEED_SCALE 0.01
#define PGN_WIND_DATA_DIRECTION_SCALE 0.0001

#define PGN_WIND_DATA_REF_TRUE_GROUND 0
#define PGN_WIND_DATA_REF_MAGNETIC_GROUND 1
#define PGN_WIND_DATA_REF_APPARENT 2
#define PGN_WIND_DATA_REF_TRUE_BOAT 3
#define PGN_WIND_DATA_REF_TRUE_WATER 4

struct __attribute__((packed)) {
	unsigned int SID:8;
	unsigned int speed:16;
	unsigned int direction:16;
	unsigned int reference:3;
	unsigned int reserved:21;
} wind_data;

#endif
