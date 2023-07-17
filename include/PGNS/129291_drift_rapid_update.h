/*
  Copyright (c) 2023 Per Hallsmark <per@hallsmark.se>
  SPDX-License-Identifier: GPL-2.0
*/

#ifndef __NMEA2000_DRIFT_RAPID_UPDATE_H__
#define __NMEA2000_DRIFT_RAPID_UPDATE_H__

#define PGN_DRIFT_RAPID_UPDATE_ID 129291
#define PGN_DRIFTRON_SCALE 0.0000001

struct __attribute__((packed)) {
	unsigned int SID:8;
	unsigned int set_reference:2;
	unsigned int reserved1:6;
	unsigned int set:16;
	unsigned int drift:16;
	unsigned int reserved2:6;
} drift_rapid_update;

#endif
