# Copyright (c) 2023 Per Hallsmark <per@hallsmark.se>
# SPDX-License-Identifier: GPL-2.0

APPS += build/nmea2000-bus-dumper
APPS += build/nmea2000-data-viewer
APPS += build/nmea2000-simple-msg

LIB += build/libnmea2000.so

LIB_SRCS += src/nmea2000.c
LIB_SRCS += src/ydwg-02.c

LIB_CFLAGS += -fPIC -Iinclude
LIB_LDFLAGS += -shared

CFLAGS += -Iinclude -g
LIBS += -lm -lcurses -lpthread
LIBS += -Lbuild -lnmea2000

all: $(LIB) $(APPS)

$(LIB): $(LIB_SRCS)
	mkdir -p build
	$(CC) $(LIB_CFLAGS) $(LIB_LDFLAGS) -o $@ $^

build/nmea2000-bus-dumper: examples/nmea2000-bus-dumper.c
	mkdir -p build
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^

build/nmea2000-data-viewer: examples/nmea2000-data-viewer.c
	mkdir -p build
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^

build/nmea2000-simple-msg: examples/nmea2000-simple-msg.c
	mkdir -p build
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^

clean:
	$(RM) -r build

find-unknown: build/nmea2000-bus-dumper
	build/nmea2000-bus-dumper > log &
	sleep 30
	killall $(APP)
	grep unknown log | awk '{print $5}' | sort -h | uniq  | wc
