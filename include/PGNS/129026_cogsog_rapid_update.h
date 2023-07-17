/*
  Copyright (c) 2023 Per Hallsmark <per@hallsmark.se>
  SPDX-License-Identifier: GPL-2.0
*/

#ifndef __NMEA2000_COGSOG_RAPID_UPDATE_H__
#define __NMEA2000_COGSOG_RAPID_UPDATE_H__

#define PGN_COGSOG_RAPID_UPDATE_ID 129026

struct __attribute__((packed)) {
	unsigned int SID:8;
	unsigned int cog_ref:2;
	unsigned int reserved1:6;
	unsigned int cog:16;
	unsigned int sog:16;
	unsigned int reserved2:16;
} cogsog_rapid_update;

#endif
