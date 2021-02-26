OBJDIR_ROOT = $(OBJDIR)
SRCDIR_ROOT = $(SRCDIR)
LIBDIR = $(OBJDIR_ROOT)/lib
BINDIR = $(OBJDIR_ROOT)/bin

APPNAME := thermal-engine

SRCS := \
	thermal.c thermal_monitor.c \
	thermal_monitor-data.c \
	thermal_util.c thermal_config_v2.c \
	sensors/sensors-tsens.c sensors/sensors-fsm9900.c \
	sensors/sensors-thermal.c sensors/sensors_manager.c \
	sensors/sensors-gen.c sensors/sensors-adc.c \
	devices/devices.c devices/devices_actions.c \
	devices/devices_manager.c \
	thermal_algorithm.c pid_algorithm.c pid-data.c \
	ss_algorithm.c ss-data.c

CINCLUDE += -I$(SRCDIR) -I$(SRCDIR)/inc -I$(SRCDIR)/server

CFLAGS += \
	-DENABLE_PID -DENABLE_SS -O0 -fno-inline -fno-short-enums -fpic \
	-DSENSORS_FSM9900 -DENABLE_TSENS_INTERRUPT -D_GNU_SOURCE \
	-DCONFIG_FILE_DEFAULT='"/mnt/flash/etc/thermal.conf"'

LIBS += -lpthread -lstringl -lm -lrt

INSTALLDIR := $(BININSTALL)

include $(MAKESDIR)/make.linux.apps

# TODO: Remove this override and fix code!
CONLYFLAGS += -Wno-error=missing-prototypes -Wno-error=strict-prototypes
