/*
  Copyright (c) 2023 Per Hallsmark <per@hallsmark.se>
  SPDX-License-Identifier: GPL-2.0
*/

#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#define __USE_XOPEN
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#include "nmea2000.h"
#include "ydwg-02.h"

#define SA struct sockaddr

#define MSG_DUMP(...) printf(__VA_ARGS__);

static void pgn_parser(struct PNG_s *pgn)
{
	uint32_t pgni;
	struct tm tm;
	char tmbuf[32];

	pgni = (uint32_t)(((uint32_t)(pgn->header.i))>>8) & 0x1FFFF;

	switch (pgni) {
		case PGN_ISO_ADDRESS_CLAIM_ID:
			MSG_DUMP("PGN iso address claim       : %X %X %d\n", pgn->header.i, pgni, pgni);
			MSG_DUMP("  sender                    : %d\n", pgn->header.s.sa, pgn->header.s.sa);
			MSG_DUMP("  manufacurer_code          : %d\n", pgn->data.iso_address_claim.manufacturer_code);
			MSG_DUMP("  device_instance_lower     : %d\n", pgn->data.iso_address_claim.device_instance_lower);
			MSG_DUMP("  device_instance_upper     : %d\n", pgn->data.iso_address_claim.device_instance_upper);
			MSG_DUMP("  device_function           : %d\n", pgn->data.iso_address_claim.device_function);
			MSG_DUMP("  device_class              : %d\n", pgn->data.iso_address_claim.device_class);
			MSG_DUMP("  system_instance           : %d\n", pgn->data.iso_address_claim.system_instance);
			MSG_DUMP("  industry_group            : %d\n", pgn->data.iso_address_claim.industry_group);
			MSG_DUMP("  arbitrary_address_capable : %d\n", pgn->data.iso_address_claim.arbitrary_address_capable);
			break;
		case PGN_SEATALK_ALARM_ID:
			MSG_DUMP("PGN seatalk alarm           : %X %X %d\n", pgn->header.i, pgni, pgni);
			MSG_DUMP("  sender                    : %d\n", pgn->header.s.sa, pgn->header.s.sa);
			MSG_DUMP("  manufacurer_code          : %d\n", pgn->data.seatalk_alarm.manufacturer_code);
			MSG_DUMP("  industry_code             : %d\n", pgn->data.seatalk_alarm.industry_code);
			MSG_DUMP("  SID                       : %d\n", pgn->data.seatalk_alarm.SID);
			MSG_DUMP("  alarm_status              : %d\n", pgn->data.seatalk_alarm.alarm_status);
			MSG_DUMP("  alarm_id                  : %d\n", pgn->data.seatalk_alarm.alarm_id);
			MSG_DUMP("  alarm_group               : %d\n", pgn->data.seatalk_alarm.alarm_group);
			MSG_DUMP("  alarm_priority            : %d\n", pgn->data.seatalk_alarm.alarm_priority);
			break;
		case PGN_SEATALK_PILOT_HEADING_ID:
			MSG_DUMP("PGN seatalk pilot heading : %X %X %d\n", pgn->header.i, pgni, pgni);
			MSG_DUMP("  sender                    : %d\n", pgn->header.s.sa, pgn->header.s.sa);
			MSG_DUMP("  manufacurer_code          : %d\n", pgn->data.seatalk_pilot_heading.manufacturer_code);
			MSG_DUMP("  industry_code             : %d\n", pgn->data.seatalk_pilot_heading.industry_code);
			MSG_DUMP("  SID                       : %d\n", pgn->data.seatalk_pilot_heading.SID);
			MSG_DUMP("  heading_true              : %f\n", RAD2DEG(pgn->data.seatalk_pilot_heading.heading_true*0.0001f));
			MSG_DUMP("  heading_magnetic          : %f\n", RAD2DEG(pgn->data.seatalk_pilot_heading.heading_magnetic*0.0001f));
			break;
		case PGN_SYSTEM_TIME_ID:
			MSG_DUMP("PGN system time             : %X %d\n", pgn->header.i, pgni, pgni);
			MSG_DUMP("  sender                    : %d\n", pgn->header.s.sa, pgn->header.s.sa);
			MSG_DUMP("  SID                       : %d\n", pgn->data.system_time.SID);
			MSG_DUMP("  source                    : %d\n", pgn->data.system_time.source);
			MSG_DUMP("  date                      : %d\n", pgn->data.system_time.date);
			MSG_DUMP("  time                      : %f\n", pgn->data.system_time.time*0.0001f);
			pgn_time2tm(pgn->data.system_time.date, pgn->data.system_time.time, &tm);
			MSG_DUMP("  date/time                 : %s\n", asctime_r(&tm, tmbuf));
			break;
		case PGN_RUDDER_ID:
			MSG_DUMP("PGN rudder                  : %X %X %d\n", pgn->header.i, pgni, pgni);
			MSG_DUMP("  sender                    : %d\n", pgn->header.s.sa, pgn->header.s.sa);
			MSG_DUMP("  instance                  : %d\n", pgn->data.rudder.instance);
			MSG_DUMP("  direction_order           : %d\n", pgn->data.rudder.direction_order);
			MSG_DUMP("  angle_order               : %f degrees\n", RAD2DEG(pgn->data.rudder.angle_order*0.0001f));
			MSG_DUMP("  position                  : %f degrees\n", RAD2DEG(pgn->data.rudder.position*0.0001f));
			break;
		case PGN_VESSEL_HEADING_ID:
			MSG_DUMP("PGN vessel heading          : %X %X %d\n", pgn->header.i, pgni, pgni);
			MSG_DUMP("  sender                    : %d\n", pgn->header.s.sa, pgn->header.s.sa);
			MSG_DUMP("  SID                       : %d\n", pgn->data.vessel_heading.SID);
			MSG_DUMP("  heading                   : %f\n", RAD2DEG(pgn->data.vessel_heading.heading*0.0001f));
			MSG_DUMP("  deviation                 : %f\n", RAD2DEG(pgn->data.vessel_heading.deviation*0.0001f));
			MSG_DUMP("  variation                 : %f\n", RAD2DEG(pgn->data.vessel_heading.variation*0.0001f));
			MSG_DUMP("  reference                 : %d\n", pgn->data.vessel_heading.reference);
			break;
		case PGN_MAGNETIC_VARIATION_ID:
			MSG_DUMP("PGN magnetic variation      : %X %X %d\n", pgn->header.i, pgni, pgni);
			MSG_DUMP("  sender                    : %d\n", pgn->header.s.sa, pgn->header.s.sa);
			MSG_DUMP("  SID                       : %d\n", pgn->data.magnetic_variation.SID);
			MSG_DUMP("  source                    : %d\n", pgn->data.magnetic_variation.source);
			MSG_DUMP("  age_of_service            : %d\n", pgn->data.magnetic_variation.age_of_service);
			MSG_DUMP("  variation                 : %f\n", RAD2DEG(pgn->data.magnetic_variation.variation*0.0001f));
			break;
		case PGN_SPEED_ID:
			MSG_DUMP("PGN speed                   : %X %X %d\n", pgn->header.i, pgni, pgni);
			MSG_DUMP("  sender                    : %d\n", pgn->header.s.sa, pgn->header.s.sa);
			MSG_DUMP("  SID                       : %d\n", pgn->data.speed.SID);
			MSG_DUMP("  speed ref water           : %f m/s\n", pgn->data.speed.speed_ref_water*0.01f);
			MSG_DUMP("  speed ref ground          : %f m/s\n", pgn->data.speed.speed_ref_ground*0.01f);
			MSG_DUMP("  speed ref water type      : %d\n", pgn->data.speed.speed_ref_water_type);
			MSG_DUMP("  speed direction           : %d\n", pgn->data.speed.speed_direction);
			break;
		case PGN_WATER_DEPTH_ID:
			MSG_DUMP("PGN water depth             : %X %X %d\n", pgn->header.i, pgni, pgni);
			MSG_DUMP("  sender                    : %d\n", pgn->header.s.sa, pgn->header.s.sa);
			MSG_DUMP("  SID                       : %d\n", pgn->data.water_depth.SID);
			MSG_DUMP("  depth                     : %0.2f m\n", pgn->data.water_depth.depth*0.01);
			MSG_DUMP("  offset                    : %0.4f m\n", pgn->data.water_depth.offset*0.001);
			MSG_DUMP("  range                     : %d\n", pgn->data.water_depth.range);
			break;
		case PGN_POSITION_RAPID_UPDATE_ID:
			MSG_DUMP("PGN position rapid update   : %X %X %d\n", pgn->header.i, pgni, pgni);
			MSG_DUMP("  sender                    : %d\n", pgn->header.s.sa, pgn->header.s.sa);
			MSG_DUMP("  longitude                 : %f degrees\n", pgn->data.position_rapid_update.longitude*PGN_POSITION_SCALE);
			MSG_DUMP("  latitude                  : %f degrees\n", pgn->data.position_rapid_update.latitude*PGN_POSITION_SCALE);
			MSG_DUMP("  google maps url           : https://www.google.com/maps/@?api=1&map_action=map&center=%f,%f&basemap=satellite\n",
				pgn->data.position_rapid_update.longitude*PGN_POSITION_SCALE,
				pgn->data.position_rapid_update.latitude*PGN_POSITION_SCALE);
			break;
		case PGN_COGSOG_RAPID_UPDATE_ID:
			MSG_DUMP("PGN cogsog rapid update     : %X %X %d\n", pgn->header.i, pgni, pgni);
			MSG_DUMP("  sender                    : %d\n", pgn->header.s.sa, pgn->header.s.sa);
			MSG_DUMP("  SID                       : %d\n", pgn->data.cogsog_rapid_update.SID);
			MSG_DUMP("  cog reference             : %d\n", pgn->data.cogsog_rapid_update.cog_ref);
			MSG_DUMP("  cog                       : %f degrees\n", RAD2DEG(pgn->data.cogsog_rapid_update.cog*0.0001f));
			MSG_DUMP("  sog                       : %f m/s\n", pgn->data.cogsog_rapid_update.sog*0.01f);
			break;
		case PGN_TIME_DATE_ID:
			MSG_DUMP("PGN time date               : %X %X %d\n", pgn->header.i, pgni, pgni);
			MSG_DUMP("  date                      : %d\n", pgn->data.time_date.date);
			MSG_DUMP("  time                      : %d\n", pgn->data.time_date.time);
			MSG_DUMP("  local_offset              : %d minutes from UTC\n", pgn->data.time_date.local_offset);
			pgn_time2tm(pgn->data.time_date.date, pgn->data.time_date.time, &tm);
			MSG_DUMP("  date/time                 : %s\n", asctime_r(&tm, tmbuf));
			break;
		case PGN_CROSSTRACK_ERROR_ID:
			MSG_DUMP("PGN crosstrack error        : %X %X %d\n", pgn->header.i, pgni, pgni);
			MSG_DUMP("  sender                    : %d\n", pgn->header.s.sa, pgn->header.s.sa);
			MSG_DUMP("  SID                       : %d\n", pgn->data.crosstrack_error.SID);
			MSG_DUMP("  xte_mode                  : %d\n", pgn->data.crosstrack_error.xte_mode);
			MSG_DUMP("  navigation_terminated     : %d\n", pgn->data.crosstrack_error.navigation_terminated);
			MSG_DUMP("  xte                       : %f m\n", pgn->data.crosstrack_error.xte*0.01f);
			break;
		case PGN_DRIFT_RAPID_UPDATE_ID:
			MSG_DUMP("PGN drift rapid update      : %X %X %d\n", pgn->header.i, pgni, pgni);
			MSG_DUMP("  sender                    : %d\n", pgn->header.s.sa, pgn->header.s.sa);
			MSG_DUMP("  SID                       : %d\n", pgn->data.drift_rapid_update.SID);
			MSG_DUMP("  reference                 : %d\n", pgn->data.drift_rapid_update.set_reference);
			MSG_DUMP("  set                       : %f degrees\n", RAD2DEG(pgn->data.drift_rapid_update.set*0.0001f));
			MSG_DUMP("  drift                     : %f m/s\n", pgn->data.drift_rapid_update.drift*0.01f);
			break;
		case PGN_WIND_DATA_ID:
			MSG_DUMP("PGN wind data               : %X %X %d\n", pgn->header.i, pgni, pgni);
			MSG_DUMP("  sender                    : %d\n", pgn->header.s.sa, pgn->header.s.sa);
			MSG_DUMP("  SID                       : %d\n", pgn->data.wind_data.SID);
			MSG_DUMP("  AWS                       : %0.1f m/s\n", pgn->data.wind_data.speed*0.01f);
			MSG_DUMP("  AWA                       : %0.1f degrees\n", RAD2DEG((pgn->data.wind_data.direction*0.0001f)));
			MSG_DUMP("  reference                 : %d\n", pgn->data.wind_data.reference);
			break;
		case PGN_ENV_PARAM_ID:
			MSG_DUMP("PGN env param               : %X %X %d\n", pgn->header.i, pgni, pgni);
			MSG_DUMP("  sender                    : %d\n", pgn->header.s.sa, pgn->header.s.sa);
			MSG_DUMP("  SID                       : %d\n", pgn->data.env_param.SID);
			MSG_DUMP("  temp source               : %d\n", pgn->data.env_param.temp_source);
			MSG_DUMP("  humidity source           : %d\n", pgn->data.env_param.humidity_source);
			MSG_DUMP("  temperature               : %f degrees celsius\n", pgn->data.env_param.temperature*0.01f-273.15);
			MSG_DUMP("  humidity                  : %f %%\n", pgn->data.env_param.humidity*0.004f);
			MSG_DUMP("  atmospheric pressure      : %f Pascal\n", pgn->data.env_param.atmospheric_pressure*100.0f);
			break;
		case PGN_TEMPERATURE_ID:
			MSG_DUMP("PGN temperature             : %X %X %d\n", pgn->header.i, pgni, pgni);
			MSG_DUMP("  sender                    : %d\n", pgn->header.s.sa, pgn->header.s.sa);
			MSG_DUMP("  SID                       : %d\n", pgn->data.temperature.SID);
			MSG_DUMP("  instance                  : %d\n", pgn->data.temperature.instance);
			MSG_DUMP("  source                    : %d\n", pgn->data.temperature.source);
			MSG_DUMP("  temperature               : %f degrees celsius\n", pgn->data.temperature.temperature*0.01f-273.15);
			MSG_DUMP("  set_temperature           : %f degrees celsius\n", pgn->data.temperature.set_temperature*0.01f-273.15);
			break;
		case PGN_TEMPERATURE_EXTENDED_ID:
			MSG_DUMP("PGN temperature extended    : %X %X %d\n", pgn->header.i, pgni, pgni);
			MSG_DUMP("  sender                    : %d\n", pgn->header.s.sa, pgn->header.s.sa);
			MSG_DUMP("  SID                       : %d\n", pgn->data.temperature_extended.SID);
			MSG_DUMP("  instance                  : %d\n", pgn->data.temperature_extended.instance);
			MSG_DUMP("  source                    : %d\n", pgn->data.temperature_extended.source);
			MSG_DUMP("  temperature               : %f degrees celsius\n", pgn->data.temperature_extended.temperature*0.001f-273.15f);
			MSG_DUMP("  set_temperature           : %f degrees celsius\n", pgn->data.temperature_extended.set_temperature*0.1f-273.15f);
			break;
		default:
			nmea2000_dump_msg(pgn);
	}

	free(pgn);
}

int main(int argc, char *argv[])
{
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;

	tzset();

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("192.168.3.100");
	servaddr.sin_port = htons(1457);

	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}

	ydwg_rx(sockfd, pgn_parser);

	close(sockfd);
}
