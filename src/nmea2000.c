/*
  Copyright (c) 2023 Per Hallsmark <per@hallsmark.se>
  SPDX-License-Identifier: GPL-2.0
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "nmea2000.h"

struct nmea2000_stats_s nmea2000_stats;

void nmea2000_time2tm(unsigned int date, unsigned int time, struct tm *tm)
{
        time_t msgtime;

        msgtime = date * 3600 * 24 + time * 0.0001;
        localtime_r(&msgtime, tm);
}

void nmea2000_header2pgn(struct nmea2000_msg_s *msg, uint32_t *pgn)
{
#if 1
  // this is how canboat construct pgn
  // seems to work fine
  unsigned int id = (((uint32_t)(msg->header.i)));
  unsigned char PF = (unsigned char) (id >> 16);
  unsigned char PS  = (unsigned char) (id >> 8);
  unsigned char RDP = (unsigned char) (id >> 24) & 3; // Use R + DP bits
  if (PF < 240) {
    /* PDU1 format, the PS contains the destination address */
    *pgn = (RDP << 16) + (PF << 8);
  } else {
    /* PDU2 format, the destination is implied global and the PGN is extended */
    *pgn = (RDP << 16) + (PF << 8) + PS;
  }

#else
  // this should be same as canboat really
  // works for address claim but not for generic
	if (msg->header.s.pf < 240) {
		*pgn = (uint32_t)(((uint32_t)(msg->header.i))>>8) & 0x1FF00;
	} else {
		*pgn = (uint32_t)(((uint32_t)(msg->header.i))>>8) & 0x1FFFF;
	}
#endif
}

void nmea2000_dump_msg(struct nmea2000_msg_s *msg)
{
	int pgn;
	int i;

	nmea2000_header2pgn(msg, &pgn);
	printf("  msg/pgn   : %08X %05X %d\n", msg->header.i, pgn, pgn);

	printf("  prio      : %d\n", msg->header.s.prio);
	printf("  r         : %d\n", msg->header.s.r);
	printf("  dp        : %d\n", msg->header.s.dp);
	printf("  pf        : %d\n", msg->header.s.pf);
	printf("  ps        : %d\n", msg->header.s.ps);
	printf("  sa        : %d\n", msg->header.s.sa);

	printf("  data (%1d)  :", msg->dlen);
	for (i=0; i<msg->dlen; i++) {
		printf(" %02X", msg->data.d[i]);
	}

	printf("\n");
}



