/*
  Copyright (c) 2023 Per Hallsmark <per@hallsmark.se>
  SPDX-License-Identifier: GPL-2.0
*/

#ifndef __NMEA2000_SEATALK_ALARM_H__
#define __NMEA2000_SEATALK_ALARM_H__

#define PGN_SEATALK_ALARM_ID 65288

struct __attribute__((packed)) {
	unsigned int manufacturer_code:11;
	unsigned int reserved1:2;
	unsigned int industry_code:3;
	unsigned int SID:8;
	unsigned int alarm_status:8;
	unsigned int alarm_id:8;
	unsigned int alarm_group:8;
	unsigned int alarm_priority:16;
} seatalk_alarm;

#endif
