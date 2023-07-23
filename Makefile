# Copyright (c) 2023 Per Hallsmark <per@hallsmark.se>
# SPDX-License-Identifier: GPL-2.0

APPS += build/nmea2000-bus-dumper
APPS += build/nmea2000-data-viewer

SRCS += src/nmea2000.c
SRCS += src/ydwg-02.c

INCS += include/PGNS/127250_vessel_heading.h

CFLAGS += -Iinclude -g
LIBS += -lm -lcurses -lpthread

all: $(APPS) build/nmea2000-data-viewer

$(SRCS): $(INCS)

build/nmea2000-bus-dumper: examples/nmea2000-bus-dumper.c $(SRCS)
	mkdir -p build
	$(CC) $(CFLAGS) -o $@ $^

build/nmea2000-data-viewer: examples/nmea2000-data-viewer.c $(SRCS)
	mkdir -p build
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	$(RM) -r build

find-unknown: build/nmea2000-bus-dumper
	build/nmea2000-bus-dumper > log &
	sleep 30
	killall $(APP)
	grep unknown log | awk '{print $5}' | sort -h | uniq  | wc
