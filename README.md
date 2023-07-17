# NMEA2000-simple-c-lib
Simple C library for NMEA2000 communication.

It's beeing developed a rainy day onboard a Jeanneau SO37.1 with mostly Raymarine based instruments
(MFD, triducer, autopilot, VHF) but also a TrueHeading AIS and a LCJ_CV7-W_M wind sensor.

Connection to NMEA2000 bus (or really Raymarine STNG bus, same signals but different contacts...)
is made via YDWG-02 https://www.yachtd.com/products/wifi_gateway.html

## library
source is in include and src, currently not integrated as shared lib... still on todo list :)

```
[per@phlap2 NMEA2000-simple-c-lib]$ tree include/ src/
include/
├── nmea2000.h
├── PGNS
│   ├── 126992_system_time.h
│   ├── 127245_rudder.h
│   ├── 127250_vessel_heading.h
│   ├── 127258_magnetic_variation.h
│   ├── 128259_speed.h
│   ├── 128267_water_depth.h
│   ├── 129025_position_rapid_update.h
│   ├── 129026_cogsog_rapid_update.h
│   ├── 129033_time_date.h
│   ├── 129283_crossstrack_error.h
│   ├── 129291_drift_rapid_update.h
│   ├── 130306_wind_data.h
│   ├── 130311_env_param.h
│   ├── 130312_temperature.h
│   ├── 130316_temperature_extended.h
│   ├── 60928_iso_address_claim.h
│   ├── 65288_seatalk_alarm.h
│   └── 65359_seatalk_pilot_heading.h
└── ydwg-02.h
src/
├── nmea2000.c
└── ydwg-02.c

3 directories, 22 files
[per@phlap2 NMEA2000-simple-c-lib]$
```

Each NMEA2000 PGN message is implemented in header file, which is then included via nmea2000.h.
Merely a union definition that overlays the data portion of the message received, and the nmea2000.h
also have a header union that overlays the header portion of the message received. All very similar
to making an IP stack for example.

## Example applications

There are two example applications, one for dumping the NMEA2000 messages as a detailed log
and one curses based simple "dashboard" application. Both helpers used during creation of this
library.

### build examples

Examples are built via a simple "make" :

```
[per@phlap2 NMEA2000-simple-c-lib]$ make
mkdir -p build
cc -Iinclude -g -o build/nmea2000-bus-dumper examples/nmea2000-bus-dumper.c src/nmea2000.c src/ydwg-02.c
mkdir -p build
cc -Iinclude -g -o build/nmea2000-data-viewer examples/nmea2000-data-viewer.c src/nmea2000.c src/ydwg-02.c -lcurses -lpthread
[per@phlap2 NMEA2000-simple-c-lib]$ 
```

and we now have the example applications in our build directory :

```
[per@phlap2 NMEA2000-simple-c-lib]$ tree build/
build/
├── nmea2000-bus-dumper
└── nmea2000-data-viewer

1 directory, 2 files
[per@phlap2 NMEA2000-simple-c-lib]$ 
```

### running examples

No options or so, currently IP/port is hardcoded in the examples so we just run them standalone like :
```
[per@phlap2 NMEA2000-simple-c-lib]$ ./build/nmea2000-bus-dumper 
PGN wind data               : 9FD0219 1FD02 130306
  sender                    : 9
  SID                       : 255
  AWS                       : 6.7 m/s
  AWA                       : 250.0 degrees
  reference                 : 2
PGN water depth             : DF50B23 1F50B 128267
  sender                    : 13
  SID                       : 255
  depth                     : 3.05 m
  offset                    : 0.0000 m
  range                     : 255
PGN wind data               : 9FD0220 1FD02 130306
  sender                    : 9
  SID                       : 255
  AWS                       : 6.7 m/s
  AWA                       : 250.0 degrees
  reference                 : 2
...
```
