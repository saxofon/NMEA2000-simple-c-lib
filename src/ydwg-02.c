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

struct ydwg_stats_s ydwg_stats;

static int ydwg2msg(char* ydwgmsg, struct nmea2000_msg_s *msg)
{
	struct ydwg_msg_s ydwg_msg;
	char timestr[12];
	int status;
	int i;

	if (strlen(ydwgmsg) < 21) {
		return -1;
	}
		
	strptime(ydwgmsg, "%H:%M:%S", &ydwg_msg.time);
	status = sscanf(&ydwgmsg[15], "%X", &ydwg_msg.pgn_header.i);
	if (status>0)
		msg->header.i = ydwg_msg.pgn_header.i;
	else
		return -1;
	for (i=0; i<8; i++) {
		status = sscanf(&ydwgmsg[24+i*3], "%X", &ydwg_msg.data[i]);
		if (status>0) {
			msg->data.d[i] = ydwg_msg.data[i];
			msg->dlen++;
		} else {
			break;
		}
	}

	return 0;
}

static int ydwg2demux(char *buf, void(*msg_parser)(struct nmea2000_msg_s *msg))
{
	char *ydwgmsg;
	char *save;
	struct nmea2000_msg_s *msg;
	int status;

	ydwgmsg = strtok_r(buf, "\n", &save);
	while (ydwgmsg && strlen(ydwgmsg)) {
		ydwg_stats.msgs++;
		if (strlen(ydwgmsg) < 21) {
			ydwg_stats.msg_errors++;
			//printf("small len %d, \"%s\"\n", strlen(ydwgmsg), ydwgmsg);
			ydwgmsg = strtok_r(NULL, "\n", &save);
			continue;
		}
		msg = (struct nmea2000_msg_s*)malloc(sizeof(struct nmea2000_msg_s));
		if (msg == NULL) {
			perror("malloc");
			continue;
		}
		memset(msg, 0, sizeof(struct nmea2000_msg_s));
		status = ydwg2msg(ydwgmsg, msg);
		if (!status) {
			nmea2000_stats.msgs++;
			msg_parser(msg);
		} else {
			nmea2000_stats.errors++;
			free(msg);
		}
		ydwgmsg = strtok_r(NULL, "\n", &save);
	}

	free(buf);

	return 0;
}

int ydwg_rx(int sockfd, void(*msg_parser)(struct nmea2000_msg_s *msg))
{
	char *buf;
	int sz;
	for (;;) {
		buf = (char*)malloc(MAX);
		if (buf == NULL) {
			perror("malloc");
			continue;
		}
		memset(buf, 0, MAX);
		sz = recv(sockfd, buf, MAX, 0);
		if (sz > 0) {
			ydwg_stats.packets++;
			ydwg2demux(buf, msg_parser);
		} else if (sz == 0) {
			printf("YDWG closed connection\n");
		} else {
			perror("recv");
			ydwg_stats.packet_errors++;
		}
	}
}

void ydwg_init(void)
{
	memset(&ydwg_stats, 0, sizeof(ydwg_stats));
}
