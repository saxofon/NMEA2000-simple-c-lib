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

#include "nmea2000.h"
#include "ydwg-02.h"

#define MAX 2048

static int ydwg2pgn(char* msg, struct PNG_s *pgn)
{
	struct ydwg_msg_s ydwg_msg;
	char timestr[12];
	int status;
	int i;

	if (strlen(msg) < 24) {
		return -1;
	}
		
	strptime(msg, "%H:%M:%S", &ydwg_msg.time);
	status = sscanf(&msg[15], "%X", &ydwg_msg.pgn_header.i);
	pgn->header.i = ydwg_msg.pgn_header.i;
	for (i=0; i<8; i++) {
		status = sscanf(&msg[24+i*3], "%X", &ydwg_msg.data[i]);
		pgn->data.d[i] = ydwg_msg.data[i];
	}
//	pgn->header.i = (uint32_t)(((uint32_t)(ydwg_msg.pgn_header.i))>>8) & 0x1FFFF;

	return 0;
}

static int ydwg2demux(char *buf, void(*msg_parser)(struct PNG_s *pgn))
{
	char *msg;
	char *save;
	struct PNG_s *pgn;
	int status;

	msg = strtok_r(buf, "\n", &save);
	while (msg && strlen(msg)) {
		if (strlen(msg) < 24) {
			msg = strtok_r(NULL, "\n", &save);
			continue;
		}
		pgn = (struct PNG_s*)malloc(sizeof(struct PNG_s));
		status = ydwg2pgn(msg, pgn);
		if (!status) {
			msg_parser(pgn);
		} else {
			free(pgn);
		}
		msg = strtok_r(NULL, "\n", &save);
	}

	free(buf);

	return 0;
}

int ydwg_rx(int sockfd, void(*msg_parser)(struct PNG_s *pgn))
{
	char *buf;
	int sz;
	for (;;) {
		buf = (char*)malloc(MAX);
		sz = recv(sockfd, buf, MAX, 0);
		if (sz > 0) {
			ydwg2demux(buf, msg_parser);
		} else if (sz == 0) {
			printf("YDWG closed connection\n");
		} else {
			perror("recv");
		}
	}
}
