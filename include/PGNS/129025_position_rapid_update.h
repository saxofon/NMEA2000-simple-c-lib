/*
  Copyright (c) 2023 Per Hallsmark <per@hallsmark.se>
  SPDX-License-Identifier: GPL-2.0
*/

#ifndef __NMEA2000_POSITION_RAPID_UPDATE_H__
#define __NMEA2000_POSITION_RAPID_UPDATE_H__

#define PGN_POSITION_RAPID_UPDATE_ID 129025
#define PGN_POSITION_SCALE 0.0000001

struct __attribute__((packed)) {
	unsigned int longitude:32;
	unsigned int latitude:32;
} position_rapid_update;

#endif
