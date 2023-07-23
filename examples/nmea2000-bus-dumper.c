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

#define DEBUG
#ifdef DEBUG
#define MSG_DUMP(msg) nmea2000_dump_msg(msg)
#define MSG_LOG(...) printf(__VA_ARGS__)
#else
#define MSG_DUMP(msg) {}
#define MSG_LOG(...) {}
#endif

static void pgn_parser(struct nmea2000_msg_s *msg)
{
	uint32_t pgn;
	struct tm tm;
	char tmbuf[32];

	nmea2000_header2pgn(msg, &pgn);

	switch (pgn) {
		case PGN_ISO_ADDRESS_CLAIM_ID:
			MSG_LOG("PGN iso address claim       : %X %X %d\n", msg->header.i, pgn, pgn);
			MSG_DUMP(msg);
			MSG_LOG("  manufacurer_code          : %d\n", msg->data.iso_address_claim.manufacturer_code);
			MSG_LOG("  device_instance_lower     : %d\n", msg->data.iso_address_claim.device_instance_lower);
			MSG_LOG("  device_instance_upper     : %d\n", msg->data.iso_address_claim.device_instance_upper);
			MSG_LOG("  device_function           : %d\n", msg->data.iso_address_claim.device_function);
			MSG_LOG("  device_class              : %d\n", msg->data.iso_address_claim.device_class);
			MSG_LOG("  system_instance           : %d\n", msg->data.iso_address_claim.system_instance);
			MSG_LOG("  industry_group            : %d\n", msg->data.iso_address_claim.industry_group);
			MSG_LOG("  arbitrary_address_capable : %d\n", msg->data.iso_address_claim.arbitrary_address_capable);
			break;
		case PGN_SEATALK_ALARM_ID:
			MSG_LOG("PGN seatalk alarm           : %X %X %d\n", msg->header.i, pgn, pgn);
			MSG_DUMP(msg);
			MSG_LOG("  manufacurer_code          : %d\n", msg->data.seatalk_alarm.manufacturer_code);
			MSG_LOG("  industry_code             : %d\n", msg->data.seatalk_alarm.industry_code);
			MSG_LOG("  SID                       : %d\n", msg->data.seatalk_alarm.SID);
			MSG_LOG("  alarm_status              : %d\n", msg->data.seatalk_alarm.alarm_status);
			MSG_LOG("  alarm_id                  : %d\n", msg->data.seatalk_alarm.alarm_id);
			MSG_LOG("  alarm_group               : %d\n", msg->data.seatalk_alarm.alarm_group);
			MSG_LOG("  alarm_priority            : %d\n", msg->data.seatalk_alarm.alarm_priority);
			break;
		case PGN_SEATALK_PILOT_HEADING_ID:
			MSG_LOG("PGN seatalk pilot heading : %X %X %d\n", msg->header.i, pgn, pgn);
			MSG_DUMP(msg);
			MSG_LOG("  manufacurer_code          : %d\n", msg->data.seatalk_pilot_heading.manufacturer_code);
			MSG_LOG("  industry_code             : %d\n", msg->data.seatalk_pilot_heading.industry_code);
			MSG_LOG("  SID                       : %d\n", msg->data.seatalk_pilot_heading.SID);
			MSG_LOG("  heading_true              : %f\n", RAD2DEG(msg->data.seatalk_pilot_heading.heading_true*0.0001f));
			MSG_LOG("  heading_magnetic          : %f\n", RAD2DEG(msg->data.seatalk_pilot_heading.heading_magnetic*0.0001f));
			break;
		case PGN_SYSTEM_TIME_ID:
			MSG_LOG("PGN system time             : %X %d\n", msg->header.i, pgn, pgn);
			MSG_DUMP(msg);
			MSG_LOG("  SID                       : %d\n", msg->data.system_time.SID);
			MSG_LOG("  source                    : %d\n", msg->data.system_time.source);
			MSG_LOG("  date                      : %d\n", msg->data.system_time.date);
			MSG_LOG("  time                      : %f\n", msg->data.system_time.time*0.0001f);
			nmea2000_time2tm(msg->data.system_time.date, msg->data.system_time.time, &tm);
			MSG_LOG("  date/time                 : %s\n", asctime_r(&tm, tmbuf));
			break;
		case PGN_RUDDER_ID:
			MSG_LOG("PGN rudder                  : %X %X %d\n", msg->header.i, pgn, pgn);
			MSG_DUMP(msg);
			MSG_LOG("  instance                  : %d\n", msg->data.rudder.instance);
			MSG_LOG("  direction_order           : %d\n", msg->data.rudder.direction_order);
			MSG_LOG("  angle_order               : %f degrees\n", RAD2DEG(msg->data.rudder.angle_order*0.0001f));
			MSG_LOG("  position                  : %f degrees\n", RAD2DEG(msg->data.rudder.position*0.0001f));
			break;
		case PGN_VESSEL_HEADING_ID:
			MSG_LOG("PGN vessel heading          : %X %X %d\n", msg->header.i, pgn, pgn);
			MSG_DUMP(msg);
			MSG_LOG("  SID                       : %d\n", msg->data.vessel_heading.SID);
			MSG_LOG("  heading                   : %f\n", RAD2DEG(msg->data.vessel_heading.heading*0.0001f));
			MSG_LOG("  deviation                 : %f\n", RAD2DEG(msg->data.vessel_heading.deviation*0.0001f));
			MSG_LOG("  variation                 : %f\n", RAD2DEG(msg->data.vessel_heading.variation*0.0001f));
			MSG_LOG("  reference                 : %d\n", msg->data.vessel_heading.reference);
			break;
		case PGN_MAGNETIC_VARIATION_ID:
			MSG_LOG("PGN magnetic variation      : %X %X %d\n", msg->header.i, pgn, pgn);
			MSG_DUMP(msg);
			MSG_LOG("  SID                       : %d\n", msg->data.magnetic_variation.SID);
			MSG_LOG("  source                    : %d\n", msg->data.magnetic_variation.source);
			MSG_LOG("  age_of_service            : %d\n", msg->data.magnetic_variation.age_of_service);
			MSG_LOG("  variation                 : %f\n", RAD2DEG(msg->data.magnetic_variation.variation*0.0001f));
			break;
		case PGN_SPEED_ID:
			MSG_LOG("PGN speed                   : %X %X %d\n", msg->header.i, pgn, pgn);
			MSG_DUMP(msg);
			MSG_LOG("  SID                       : %d\n", msg->data.speed.SID);
			MSG_LOG("  speed ref water           : %f m/s\n", msg->data.speed.speed_ref_water*0.01f);
			MSG_LOG("  speed ref ground          : %f m/s\n", msg->data.speed.speed_ref_ground*0.01f);
			MSG_LOG("  speed ref water type      : %d\n", msg->data.speed.speed_ref_water_type);
			MSG_LOG("  speed direction           : %d\n", msg->data.speed.speed_direction);
			break;
		case PGN_WATER_DEPTH_ID:
			MSG_LOG("PGN water depth             : %X %X %d\n", msg->header.i, pgn, pgn);
			MSG_DUMP(msg);
			MSG_LOG("  SID                       : %d\n", msg->data.water_depth.SID);
			MSG_LOG("  depth                     : %0.2f m\n", msg->data.water_depth.depth*0.01);
			MSG_LOG("  offset                    : %0.4f m\n", msg->data.water_depth.offset*0.001);
			MSG_LOG("  range                     : %d\n", msg->data.water_depth.range);
			break;
		case PGN_POSITION_RAPID_UPDATE_ID:
			MSG_LOG("PGN position rapid update   : %X %X %d\n", msg->header.i, pgn, pgn);
			MSG_DUMP(msg);
			MSG_LOG("  longitude                 : %f degrees\n", msg->data.position_rapid_update.longitude*PGN_POSITION_SCALE);
			MSG_LOG("  latitude                  : %f degrees\n", msg->data.position_rapid_update.latitude*PGN_POSITION_SCALE);
			MSG_LOG("  google maps url           : https://www.google.com/maps/@?api=1&map_action=map&center=%f,%f&basemap=satellite\n",
				msg->data.position_rapid_update.longitude*PGN_POSITION_SCALE,
				msg->data.position_rapid_update.latitude*PGN_POSITION_SCALE);
			break;
		case PGN_COGSOG_RAPID_UPDATE_ID:
			MSG_LOG("PGN cogsog rapid update     : %X %X %d\n", msg->header.i, pgn, pgn);
			MSG_DUMP(msg);
			MSG_LOG("  SID                       : %d\n", msg->data.cogsog_rapid_update.SID);
			MSG_LOG("  cog reference             : %d\n", msg->data.cogsog_rapid_update.cog_ref);
			MSG_LOG("  cog                       : %f degrees\n", RAD2DEG(msg->data.cogsog_rapid_update.cog*0.0001f));
			MSG_LOG("  sog                       : %f m/s\n", msg->data.cogsog_rapid_update.sog*0.01f);
			break;
		case PGN_TIME_DATE_ID:
			MSG_LOG("PGN time date               : %X %X %d\n", msg->header.i, pgn, pgn);
			MSG_DUMP(msg);
			MSG_LOG("  date                      : %d\n", msg->data.time_date.date);
			MSG_LOG("  time                      : %d\n", msg->data.time_date.time);
			MSG_LOG("  local_offset              : %d minutes from UTC\n", msg->data.time_date.local_offset);
			nmea2000_time2tm(msg->data.time_date.date, msg->data.time_date.time, &tm);
			MSG_LOG("  date/time                 : %s\n", asctime_r(&tm, tmbuf));
			break;
		case PGN_CROSSTRACK_ERROR_ID:
			MSG_LOG("PGN crosstrack error        : %X %X %d\n", msg->header.i, pgn, pgn);
			MSG_DUMP(msg);
			MSG_LOG("  SID                       : %d\n", msg->data.crosstrack_error.SID);
			MSG_LOG("  xte_mode                  : %d\n", msg->data.crosstrack_error.xte_mode);
			MSG_LOG("  navigation_terminated     : %d\n", msg->data.crosstrack_error.navigation_terminated);
			MSG_LOG("  xte                       : %f m\n", msg->data.crosstrack_error.xte*0.01f);
			break;
		case PGN_DRIFT_RAPID_UPDATE_ID:
			MSG_LOG("PGN drift rapid update      : %X %X %d\n", msg->header.i, pgn, pgn);
			MSG_DUMP(msg);
			MSG_LOG("  SID                       : %d\n", msg->data.drift_rapid_update.SID);
			MSG_LOG("  reference                 : %d\n", msg->data.drift_rapid_update.set_reference);
			MSG_LOG("  set                       : %f degrees\n", RAD2DEG(msg->data.drift_rapid_update.set*0.0001f));
			MSG_LOG("  drift                     : %f m/s\n", msg->data.drift_rapid_update.drift*0.01f);
			break;
		case PGN_WIND_DATA_ID:
			MSG_LOG("PGN wind data               : %X %X %d\n", msg->header.i, pgn, pgn);
			MSG_DUMP(msg);
			MSG_LOG("  SID                       : %d\n", msg->data.wind_data.SID);
			MSG_LOG("  AWS                       : %0.1f m/s\n", msg->data.wind_data.speed*0.01f);
			MSG_LOG("  AWA                       : %0.1f degrees\n", RAD2DEG((msg->data.wind_data.direction*0.0001f)));
			MSG_LOG("  reference                 : %d\n", msg->data.wind_data.reference);
			break;
		case PGN_ENV_PARAM_ID:
			MSG_LOG("PGN env param               : %X %X %d\n", msg->header.i, pgn, pgn);
			MSG_DUMP(msg);
			MSG_LOG("  SID                       : %d\n", msg->data.env_param.SID);
			MSG_LOG("  temp source               : %d\n", msg->data.env_param.temp_source);
			MSG_LOG("  humidity source           : %d\n", msg->data.env_param.humidity_source);
			MSG_LOG("  temperature               : %f degrees celsius\n", msg->data.env_param.temperature*0.01f-273.15);
			MSG_LOG("  humidity                  : %f %%\n", msg->data.env_param.humidity*0.004f);
			MSG_LOG("  atmospheric pressure      : %f Pascal\n", msg->data.env_param.atmospheric_pressure*100.0f);
			break;
		case PGN_TEMPERATURE_ID:
			MSG_LOG("PGN temperature             : %X %X %d\n", msg->header.i, pgn, pgn);
			MSG_DUMP(msg);
			MSG_LOG("  SID                       : %d\n", msg->data.temperature.SID);
			MSG_LOG("  instance                  : %d\n", msg->data.temperature.instance);
			MSG_LOG("  source                    : %d\n", msg->data.temperature.source);
			MSG_LOG("  temperature               : %f degrees celsius\n", msg->data.temperature.temperature*0.01f-273.15);
			MSG_LOG("  set_temperature           : %f degrees celsius\n", msg->data.temperature.set_temperature*0.01f-273.15);
			break;
		case PGN_TEMPERATURE_EXTENDED_ID:
			MSG_LOG("PGN temperature extended    : %X %X %d\n", msg->header.i, pgn, pgn);
			MSG_DUMP(msg);
			MSG_LOG("  SID                       : %d\n", msg->data.temperature_extended.SID);
			MSG_LOG("  instance                  : %d\n", msg->data.temperature_extended.instance);
			MSG_LOG("  source                    : %d\n", msg->data.temperature_extended.source);
			MSG_LOG("  temperature               : %f degrees celsius\n", msg->data.temperature_extended.temperature*0.001f-273.15f);
			MSG_LOG("  set_temperature           : %f degrees celsius\n", msg->data.temperature_extended.set_temperature*0.1f-273.15f);
			break;
		default:
			printf("Unknown message!\n");
			nmea2000_dump_msg(msg);
	}

	free(msg);
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
