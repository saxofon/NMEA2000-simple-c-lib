/* 
  Copyright (c) 2023 Per Hallsmark <per@hallsmark.se>
  SPDX-License-Identifier: GPL-2.0
*/

#ifndef __NMEA2000_SYSTEM_TIME_H__
#define __NMEA2000_SYSTEM_TIME_H__

#define PGN_SYSTEM_TIME_ID 126992

struct __attribute__((packed)) {
	unsigned int SID:8;
	unsigned int source:4;
	unsigned int reserved1:4;
	unsigned int date:16;
	unsigned int time:32;
} system_time;

#endif
