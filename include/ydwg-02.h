/* 
  Copyright (c) 2023 Per Hallsmark <per@hallsmark.se>
  SPDX-License-Identifier: GPL-2.0
*/

#ifndef __YDWG02_H__
#define __YDWG02_H__

struct ydwg_stats_s {
	uint32_t packets;
	uint32_t msgs;
	uint32_t packet_errors;
	uint32_t msg_errors;
};

extern struct ydwg_stats_s ydwg_stats;

struct ydwg_msg_s {
	struct tm time;
	union {
		uint32_t i;
		struct {
			unsigned int unused:3;
			unsigned int prio:3;
			unsigned int r:1;
			unsigned int dp:1;
			unsigned int pf:8;
			unsigned int ps:8;
			unsigned int sa:8;
		} s;
	} pgn_header;
	uint32_t dlen;
	uint8_t data[8];
};

extern int ydwg_rx(int sockfd, void(*pgn_parser)(struct nmea2000_msg_s *msg));
extern int ydwg_tx(int sockfd, struct nmea2000_msg_s *msg);
extern void ydwg_init(void);

#endif
