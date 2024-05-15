/*
  Copyright (c) 2023 Per Hallsmark <per@hallsmark.se>
  SPDX-License-Identifier: GPL-2.0
*/

#include <arpa/inet.h>
#include <linux/can.h>
#include <linux/if.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include "nmea2000.h"
#include "ydwg-02.h"

#define SA struct sockaddr

/* TODO
 -h host/IP to YDWG-02
 -p port on YDWG-02
 -c can interface
*/

static char* ydwg_host = "192.168.3.100";
static uint16_t ydwg_port = 1457;
static char* can_dev = "vcan0";

static int socket_ydwg;
static int socket_can;

static void socketcan_tx(struct nmea2000_msg_s *msg)
{
	struct can_frame frame;

	frame.can_id = CAN_EFF_FLAG | msg->header.i;
	frame.can_dlc = msg->dlen;
	memcpy(frame.data, msg->data.d, msg->dlen);
	if (write(socket_can, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
		perror("socket_can");
		return;
	}
}

int main()
{
	struct sockaddr_in ydwg_addr;
	struct sockaddr_can can_addr;
	struct ifreq can_ifr;

	tzset();

	// YDWG-02 TCP socket
	socket_ydwg = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_ydwg == -1) {
		perror("socket_ydwg");
		exit(0);
	}

	bzero(&ydwg_addr, sizeof(ydwg_addr));

	ydwg_addr.sin_family = AF_INET;
	ydwg_addr.sin_addr.s_addr = inet_addr(ydwg_host);
	ydwg_addr.sin_port = htons(ydwg_port);

	if (connect(socket_ydwg, (SA*)&ydwg_addr, sizeof(ydwg_addr)) != 0) {
		perror("socket_ydwg");
		exit(0);
	}

	// SocketCAN socket
	if ((socket_can = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("socket_can");
		return 1;
	}
	
	strncpy(can_ifr.ifr_name, can_dev, IFNAMSIZ);
	ioctl(socket_can, SIOCGIFINDEX, &can_ifr);

	bzero(&can_addr, sizeof(can_addr));
	can_addr.can_family = AF_CAN;
	can_addr.can_ifindex = can_ifr.ifr_ifindex;
	bind(socket_can, (struct sockaddr *)&can_addr, sizeof(can_addr));

	// YDWG-02 -> socketcan
	ydwg_rx(socket_ydwg, socketcan_tx);

	close(socket_ydwg);
	close(socket_can);
}

