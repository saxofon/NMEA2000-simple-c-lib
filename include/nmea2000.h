/*
  Copyright (c) 2023 Per Hallsmark <per@hallsmark.se>
  SPDX-License-Identifier: GPL-2.0
*/

#ifndef __NMEA2000_H__
#define __NMEA2000_H__

#include <stdint.h>
#include <math.h>

#define RAD2DEG(x) ((float)(x)*(180.0f/(float)(M_PI)))
#define KTOC(x) (x*0.01f-273.15f)

#define NMEA2000_ADDR_GLOBAL  0xFF
#define NMEA2000_ADDR_UNKNOWN 0xFE

struct nmea2000_stats_s {
	uint32_t msgs;
	uint32_t errors;
};

extern struct nmea2000_stats_s nmea2000_stats;

struct nmea2000_msg_s {
	union {
		uint32_t i;
		struct __attribute__((packed)) {
			unsigned int reserved:3;
			unsigned int prio:3;
			unsigned int r:1;
			unsigned int dp:1;
			unsigned int pf:8;
			unsigned int ps:8;
			unsigned int sa:8;
		} s;
	} header;
	uint32_t dlen;
	union {
		uint8_t d[8];
#include "PGNS/60928_iso_address_claim.h"
#include "PGNS/65288_seatalk_alarm.h"
#include "PGNS/65359_seatalk_pilot_heading.h"
#include "PGNS/126992_system_time.h"
#include "PGNS/127245_rudder.h"
#include "PGNS/127250_vessel_heading.h"
#include "PGNS/127258_magnetic_variation.h"
#include "PGNS/128259_speed.h"
#include "PGNS/128267_water_depth.h"
#include "PGNS/129025_position_rapid_update.h"
#include "PGNS/129026_cogsog_rapid_update.h"
#include "PGNS/129033_time_date.h"
#include "PGNS/129283_crossstrack_error.h"
#include "PGNS/129291_drift_rapid_update.h"
#include "PGNS/130306_wind_data.h"
#include "PGNS/130311_env_param.h"
#include "PGNS/130312_temperature.h"
#include "PGNS/130316_temperature_extended.h"
	} data;
};

void nmea2000_time2tm(unsigned int date, unsigned int time, struct tm *tm);
void nmea2000_dump_msg(struct nmea2000_msg_s *msg);
void nmea2000_header2pgn(struct nmea2000_msg_s *msg, uint32_t *pgn);

#endif
