/*
  Copyright (c) 2023 Per Hallsmark <per@hallsmark.se>
  SPDX-License-Identifier: GPL-2.0
*/

#ifndef __NMEA2000_TIME_DATE_H__
#define __NMEA2000_TIME_DATE_H__

#define PGN_TIME_DATE_ID 129033

struct __attribute__((packed)) {
	unsigned int date:16;
	unsigned int time:32;
	int local_offset:16;
} time_date;

#endif
