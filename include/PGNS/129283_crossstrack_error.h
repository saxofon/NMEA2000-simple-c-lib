/*
  Copyright (c) 2023 Per Hallsmark <per@hallsmark.se>
  SPDX-License-Identifier: GPL-2.0
*/

#ifndef __NMEA2000_CROSSTRACK_ERROR_H__
#define __NMEA2000_CROSSTRACK_ERROR_H__

#define PGN_CROSSTRACK_ERROR_ID 129283

struct __attribute__((packed)) {
	unsigned int SID:8;
	unsigned int xte_mode:4;
	unsigned int reserved1:2;
	unsigned int navigation_terminated:2;
	int xte:32;
	unsigned int reserved2:16;
} crosstrack_error;

#endif
