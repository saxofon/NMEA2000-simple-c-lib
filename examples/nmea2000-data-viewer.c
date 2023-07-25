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
#include <math.h>
#define __USE_XOPEN
#include <time.h>
#include <curses.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#include "nmea2000.h"
#include "ydwg-02.h"

#define SA struct sockaddr

//#define MSG_DUMP(...) printf(__VA_ARGS__);
#define MSG_DUMP(...) {}

static struct s_nmea2000_data {
	struct tm tm;
	double latitude;
	double longitude;
	double depth;
	double SOG;
	double STW;
	double COG;
	double heading;
	double autopilot_heading_true;
	double autopilot_heading_magnetic;
	double drift_speed;
	double drift_angle;
	double AWS;
	double AWA;
	double AWD;
	double TWS;
	double TWD;
	double rudder_angle;
	double water_temp;
	double inside_air_humidity;
	double inside_air_temp;
	double outside_air_humidity;
	double outside_air_temp;
	double atmospheric_pressure;
} nmea2000_data;

static void pgn_parser(struct nmea2000_msg_s *msg)
{
	uint32_t pgn;
	double a, b;

	nmea2000_header2pgn(msg, &pgn);

	switch (pgn) {
		case PGN_ISO_ADDRESS_CLAIM_ID:
			MSG_DUMP("PGN iso address claim       : %X %X %d\n", msg->header.i, pgn, pgn);
			MSG_DUMP("  sender                    : %d\n", msg->header.s.sa, msg->header.s.sa);
			MSG_DUMP("  manufacurer_code          : %d\n", msg->data.iso_address_claim.manufacturer_code);
			MSG_DUMP("  device_instance_lower     : %d\n", msg->data.iso_address_claim.device_instance_lower);
			MSG_DUMP("  device_instance_upper     : %d\n", msg->data.iso_address_claim.device_instance_upper);
			MSG_DUMP("  device_function           : %d\n", msg->data.iso_address_claim.device_function);
			MSG_DUMP("  device_class              : %d\n", msg->data.iso_address_claim.device_class);
			MSG_DUMP("  system_instance           : %d\n", msg->data.iso_address_claim.system_instance);
			MSG_DUMP("  industry_group            : %d\n", msg->data.iso_address_claim.industry_group);
			MSG_DUMP("  arbitrary_address_capable : %d\n", msg->data.iso_address_claim.arbitrary_address_capable);
			break;
		case PGN_SEATALK_ALARM_ID:
			MSG_DUMP("PGN seatalk alarm           : %X %X %d\n", msg->header.i, pgn, pgn);
			MSG_DUMP("  sender                    : %d\n", msg->header.s.sa, msg->header.s.sa);
			MSG_DUMP("  manufacurer_code          : %d\n", msg->data.seatalk_alarm.manufacturer_code);
			MSG_DUMP("  industry_code             : %d\n", msg->data.seatalk_alarm.industry_code);
			MSG_DUMP("  SID                       : %d\n", msg->data.seatalk_alarm.SID);
			MSG_DUMP("  alarm_status              : %d\n", msg->data.seatalk_alarm.alarm_status);
			MSG_DUMP("  alarm_id                  : %d\n", msg->data.seatalk_alarm.alarm_id);
			MSG_DUMP("  alarm_group               : %d\n", msg->data.seatalk_alarm.alarm_group);
			MSG_DUMP("  alarm_priority            : %d\n", msg->data.seatalk_alarm.alarm_priority);
			break;
		case PGN_SEATALK_PILOT_HEADING_ID:
			if (msg->data.seatalk_pilot_heading.heading_true != 0xFFFF)
				nmea2000_data.autopilot_heading_true     = msg->data.seatalk_pilot_heading.heading_true*0.0001f;
			if (msg->data.seatalk_pilot_heading.heading_magnetic != 0xFFFF)
				nmea2000_data.autopilot_heading_magnetic = msg->data.seatalk_pilot_heading.heading_magnetic*0.0001f;
			break;
		case PGN_SYSTEM_TIME_ID:
			nmea2000_time2tm(msg->data.system_time.date, msg->data.system_time.time, &nmea2000_data.tm);
			break;
		case PGN_RUDDER_ID:
			nmea2000_data.rudder_angle = msg->data.rudder.position*0.0001f;
			break;
		case PGN_VESSEL_HEADING_ID:
			nmea2000_data.heading = msg->data.vessel_heading.heading*0.0001f;
			break;
		case PGN_MAGNETIC_VARIATION_ID:
			MSG_DUMP("PGN magnetic variation      : %X %X %d\n", msg->header.i, pgn, pgn);
			MSG_DUMP("  sender                    : %d\n", msg->header.s.sa, msg->header.s.sa);
			MSG_DUMP("  SID                       : %d\n", msg->data.magnetic_variation.SID);
			MSG_DUMP("  source                    : %d\n", msg->data.magnetic_variation.source);
			MSG_DUMP("  age_of_service            : %d\n", msg->data.magnetic_variation.age_of_service);
			MSG_DUMP("  variation                 : %f\n", msg->data.magnetic_variation.variation*0.0001f);
			break;
		case PGN_SPEED_ID:
			nmea2000_data.STW = msg->data.speed.speed_ref_water*0.01f;
			//nmea2000_data.SOG = msg->data.speed.speed_ref_ground*0.01f;
			break;
		case PGN_WATER_DEPTH_ID:
			nmea2000_data.depth = msg->data.water_depth.depth*0.01;
			// %0.4f m,  msg->data.water_depth.offset*0.001
			break;
		case PGN_POSITION_RAPID_UPDATE_ID:
			nmea2000_data.longitude = msg->data.position_rapid_update.longitude*PGN_POSITION_SCALE;
			nmea2000_data.latitude = msg->data.position_rapid_update.latitude*PGN_POSITION_SCALE;
			break;
		case PGN_COGSOG_RAPID_UPDATE_ID:
			if (msg->data.cogsog_rapid_update.cog != 0xFFFF)
				nmea2000_data.COG = msg->data.cogsog_rapid_update.cog*0.0001f;
			if (msg->data.cogsog_rapid_update.sog != 0xFFFF)
				nmea2000_data.SOG = msg->data.cogsog_rapid_update.sog*0.01f;
			break;
		case PGN_TIME_DATE_ID:
			//nmea2000_time2tm(msg->data.time_date.date, msg->data.time_date.time, &nmea2000_data.tm);
			break;
		case PGN_CROSSTRACK_ERROR_ID:
			MSG_DUMP("PGN crosstrack error        : %X %X %d\n", msg->header.i, pgn, pgn);
			MSG_DUMP("  sender                    : %d\n", msg->header.s.sa, msg->header.s.sa);
			MSG_DUMP("  SID                       : %d\n", msg->data.crosstrack_error.SID);
			MSG_DUMP("  xte_mode                  : %d\n", msg->data.crosstrack_error.xte_mode);
			MSG_DUMP("  navigation_terminated     : %d\n", msg->data.crosstrack_error.navigation_terminated);
			MSG_DUMP("  xte                       : %f m\n", msg->data.crosstrack_error.xte*0.01f);
			break;
		case PGN_DRIFT_RAPID_UPDATE_ID:
			nmea2000_data.drift_angle = msg->data.drift_rapid_update.set*0.0001f;
			nmea2000_data.drift_speed = msg->data.drift_rapid_update.drift*0.01f;
			break;
		case PGN_WIND_DATA_ID:
			nmea2000_data.AWS = msg->data.wind_data.speed*0.01f;
			nmea2000_data.AWA = msg->data.wind_data.direction*0.0001f;
			nmea2000_data.AWD = nmea2000_data.heading + nmea2000_data.AWA;
			while (nmea2000_data.AWD >= 2*M_PI)
				nmea2000_data.AWD -= 2*M_PI;
			while (nmea2000_data.AWD < 0.0)
				nmea2000_data.AWD += 2*M_PI;
			a = nmea2000_data.SOG*sin(nmea2000_data.SOG) - nmea2000_data.AWS*sin(nmea2000_data.AWD);
			b = nmea2000_data.SOG*cos(nmea2000_data.SOG) - nmea2000_data.AWS*cos(nmea2000_data.AWD);
			nmea2000_data.TWS = sqrt(a*a + b*b);
			nmea2000_data.TWD = atan(a/b);
			break;
		case PGN_ENV_PARAM_ID:
			if (msg->data.env_param.temp_source == 0) {
				nmea2000_data.water_temp = KTOC(msg->data.env_param.temperature);
			} else if (msg->data.env_param.temp_source == 1) {
				nmea2000_data.outside_air_temp = KTOC(msg->data.env_param.temperature);
			} else if (msg->data.env_param.temp_source == 2) {
				nmea2000_data.inside_air_temp = KTOC(msg->data.env_param.temperature);
			}
			if (msg->data.env_param.humidity_source == 0) {
				nmea2000_data.inside_air_humidity = msg->data.env_param.humidity*0.004f;
			} else if (msg->data.env_param.humidity_source == 1) {
				nmea2000_data.outside_air_humidity = msg->data.env_param.humidity*0.004f;
			}
			if (msg->data.env_param.atmospheric_pressure != 0xFFFF)
				nmea2000_data.atmospheric_pressure = msg->data.env_param.atmospheric_pressure*100.0f;
			break;
		case PGN_TEMPERATURE_ID:
			if (msg->data.temperature.source == 1) {
				nmea2000_data.outside_air_temp = KTOC(msg->data.temperature.temperature);
			}
			break;
		case PGN_TEMPERATURE_EXTENDED_ID:
			if ( msg->data.temperature_extended.source == 1) {
				nmea2000_data.outside_air_temp = KTOC(msg->data.temperature_extended.temperature);
			}
			break;
	}
}

static void *nmea2000_rx(void *arg)
{
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;


	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}

	ydwg_init();

	while (true) {
		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = inet_addr("192.168.3.100");
		servaddr.sin_port = htons(1457);

		if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
			printf("connection with the server failed...\n");
			exit(0);
		}

		ydwg_rx(sockfd, pgn_parser);
	}
}

static void *data_viewer(void *arg)
{
	char tmbuf[32];

	initscr();
	cbreak();
	noecho();
	clear();

	for (;;) {
		mvprintw( 0,0, "Date&time         : %s", asctime_r(&nmea2000_data.tm, tmbuf));
		mvprintw( 2,0, "SOG/COG           : %5.2f m/s %3.0f°", nmea2000_data.SOG, RAD2DEG(nmea2000_data.COG));
		mvprintw( 3,0, "STW               : %5.2f m/s", nmea2000_data.STW);
		mvprintw( 5,0, "Drift speed/angle : %5.2f m/s %3.0f°", nmea2000_data.drift_speed, RAD2DEG(nmea2000_data.drift_angle));
		mvprintw( 7,0, "AWS/AWA/AWD       : %5.2f m/s %3.0f° %4.0f°", nmea2000_data.AWS, RAD2DEG(nmea2000_data.AWA), RAD2DEG(nmea2000_data.AWD));
		mvprintw( 8,0, "TWS/TWD           : %5.2f m/s %3.0f°", nmea2000_data.TWS, RAD2DEG(nmea2000_data.TWD));
		mvprintw(10,0, "Rudder angle      : %3.0f°", RAD2DEG(nmea2000_data.rudder_angle));
		mvprintw(12,0, "Vessel heading    : %3.0f°M", RAD2DEG(nmea2000_data.heading));
		mvprintw(13,0, "Autopilot heading true/magnetic : %3.0f°T/%3.0f°M",
			RAD2DEG(nmea2000_data.autopilot_heading_true), RAD2DEG(nmea2000_data.autopilot_heading_magnetic));
		mvprintw(18,0, "Water depth/temp  : %4.2f m %4.1f °C", nmea2000_data.depth, nmea2000_data.water_temp);
		mvprintw(19,0, "Inside Air humidity/temp  : %4.2f %% %4.1f °C", nmea2000_data.inside_air_humidity, nmea2000_data.inside_air_temp);
		mvprintw(20,0, "Outside Air humidity/temp : %4.2f %% %4.1f °C", nmea2000_data.outside_air_humidity, nmea2000_data.outside_air_temp);
		mvprintw(20,45, "Atmospheric pressure : %4.1f hPa", nmea2000_data.atmospheric_pressure/100.0);
		mvprintw(22, 0, "map url : https://www.google.com/maps/@?api=1&map_action=map&center=%f,%f&basemap=satellite",
			nmea2000_data.longitude, nmea2000_data.latitude);
		mvprintw(23, 0, "map url : https://www.google.com/maps/search/?api=1&query=%f,%f",
			nmea2000_data.longitude, nmea2000_data.latitude);
		mvprintw(25,0, "YDWG02 stats   : packets %d (of which are errors %d), msgs %d ( errors %d)",
			ydwg_stats.packets, ydwg_stats.packet_errors, ydwg_stats.msgs, ydwg_stats.msg_errors);
		mvprintw(26,0, "NMEA2000 stats : msgs %d, errors %d",
			nmea2000_stats.msgs, nmea2000_stats.errors);
		refresh();
		sleep(1);
	}

}

int main(int argc, char *argv[])
{
	pthread_t nmea2000_rx_tid;
	pthread_t data_viewer_tid;

	tzset();

	pthread_create(&nmea2000_rx_tid, NULL, nmea2000_rx, NULL);
	pthread_create(&data_viewer_tid, NULL, data_viewer, NULL);

	for (;;) {
		sleep(1);
	}
	exit(0);
}
