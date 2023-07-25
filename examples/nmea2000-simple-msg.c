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

static void pgn_parser(struct nmea2000_msg_s *msg)
{
	uint32_t pgn;
	struct tm tm;
	char tmbuf[32];

	nmea2000_header2pgn(msg, &pgn);

	if (pgn == PGN_SYSTEM_TIME_ID) {
		nmea2000_time2tm(msg->data.system_time.date, msg->data.system_time.time, &tm);
		printf(" NMEA2000 system date/time : %s\n", asctime_r(&tm, tmbuf));
	}
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
