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

void pgn_time2tm(unsigned int date, unsigned int time, struct tm *tm)
{
        time_t msgtime;

        msgtime = date * 3600 * 24 + time * 0.0001;
        localtime_r(&msgtime, tm);
}

void nmea2000_dump_msg(struct PNG_s *pgn)
{
	int pgni;
	int i;

	pgni = (uint32_t)(((uint32_t)(pgn->header.i))>>8) & 0x1FFFF;

	printf("PGN unknown : %X %X %d\n", pgn->header.i, pgni, pgni);
	printf("  sender    : %d\n", pgn->header.s.sa, pgn->header.s.sa);

	printf("  data      :");
	for (i=0; i<8; i++){
		printf(" %X", pgn->data.d[i]);
	}
	printf("\n");
}



