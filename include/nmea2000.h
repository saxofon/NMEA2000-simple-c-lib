/*
  Copyright (c) 2023 Per Hallsmark <per@hallsmark.se>
  SPDX-License-Identifier: GPL-2.0
*/

#ifndef __NMEA2000_H__
#define __NMEA2000_H__

#include <stdint.h>
#include <math.h>

struct PNG_s {
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

#define RAD2DEG(x) ((float)(x)*(180.0f/(float)(M_PI)))

void pgn_time2tm(unsigned int date, unsigned int time, struct tm *tm);
void nmea2000_dump_msg(struct PNG_s *pgn);

#endif
