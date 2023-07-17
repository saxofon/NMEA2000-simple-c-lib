/*
  Copyright (c) 2023 Per Hallsmark <per@hallsmark.se>
  SPDX-License-Identifier: GPL-2.0
*/

#ifndef __NMEA2000_ISO_ADDRESS_CLAIM_H__
#define __NMEA2000_ISO_ADDRESS_CLAIM_H__

#define PGN_ISO_ADDRESS_CLAIM_ID 60928

struct __attribute__((packed)) {
	unsigned int unique_number:21;
	unsigned int manufacturer_code:11;
	unsigned int device_instance_lower:3;
	unsigned int device_instance_upper:5;
	unsigned int device_function:8;
	unsigned int reserved1:1;
	unsigned int device_class:7;
	unsigned int system_instance:4;
	unsigned int industry_group:3;
	unsigned int arbitrary_address_capable:1;
} iso_address_claim;

#endif
