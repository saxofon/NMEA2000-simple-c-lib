/*
  Copyright (c) 2023 Per Hallsmark <per@hallsmark.se>
  SPDX-License-Identifier: GPL-2.0
*/

#include <errno.h>
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

#define MAX 65536

struct ydwg_stats_s ydwg_stats;

static int ydwg2msg(char* ydwgmsg, struct nmea2000_msg_s *msg)
{
	struct ydwg_msg_s ydwg_msg;
	char timestr[12];
	int status;
	int i;

	// filter out too small msgs
	if (strlen(ydwgmsg) < 21) {
		return -1;
	}
		
	// filter out our own sent msgs
	if (ydwgmsg[13] == 'T') {
		return -1;
	}
	strptime(ydwgmsg, "%H:%M:%S", &ydwg_msg.time);
	errno=0;
	ydwg_msg.pgn_header.i = strtol(&ydwgmsg[15], NULL, 16);
	if (errno)
		return -1;
	msg->header.i = ydwg_msg.pgn_header.i;
	for (i=0; i<8; i++) {
		errno=0;
		ydwg_msg.data[i] = strtol(&ydwgmsg[24+i*3], NULL, 16);
		if (errno)
			break;
		msg->data.d[i] = ydwg_msg.data[i];
		msg->dlen++;
	}

	return 0;
}

static int msg2ydwg(struct nmea2000_msg_s *msg, char *ydwgmsg)
{
	int idx_str=0;
	int idx_data;

	// frame header
	idx_str = sprintf(&ydwgmsg[0], "%8.8X", msg->header.i);

	// frame data bytes
	for (idx_data=0; idx_data < msg->dlen; idx_data++) {
		idx_str += sprintf(&ydwgmsg[idx_str], " %2.2X", msg->data.d[idx_data]);
	}

	// frame end
	idx_str += sprintf(&ydwgmsg[idx_str], "\r\n");

	return(idx_str);
}

static int ydwg2demux(char *buf, void(*msg_parser)(struct nmea2000_msg_s *msg))
{
	char *ydwgmsg;
	char *save;
	struct nmea2000_msg_s *msg;
	int status;

	ydwgmsg = strtok_r(buf, "\n", &save);
	while (ydwgmsg && strlen(ydwgmsg)) {
		//printf("msg \"%s\"\n", ydwgmsg);
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
		} 
		free(msg);
		ydwgmsg = strtok_r(NULL, "\n", &save);
	}

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
			free(buf);
			return(-1);
		} else {
			perror("recv");
			ydwg_stats.packet_errors++;
		}
		free(buf);
	}
}

int ydwg_tx(int sockfd, struct nmea2000_msg_s *msg)
{
	char *buf;
	int sz;
	int status;

	buf = (char*)malloc(MAX);
	if (buf == NULL) {
		perror("malloc");
		return(-1);
	}

	sz = msg2ydwg(msg, buf);
	if (sz < 0) {
		perror("can frame to ydwg frame conversion");
		return(-1);
	}

	status = write(sockfd, buf, sz);
	if (status != sz) {
		perror("ydwg write");
		return(-1);
	}
}

void ydwg_init(void)
{
	memset(&ydwg_stats, 0, sizeof(ydwg_stats));
}
