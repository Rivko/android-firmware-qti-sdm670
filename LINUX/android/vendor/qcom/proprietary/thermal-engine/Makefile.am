if ENABLE_THERMAL_ENGINE
AM_CFLAGS = -Wall \
		-Wundef \
		-Wstrict-prototypes \
		-Wno-trigraphs \
		-Werror \
		-Wno-strict-aliasing \
		-g -O0 \
		-fno-inline \
		-fno-short-enums \
		-fpic

AM_CPPFLAGS = -D_GNU_SOURCE \
	      -DCONFIG_FILE_DEFAULT='"/etc/thermal-engine.conf"' \
	      -I./inc

sources = \
	 ./thermal.c \
	 ./thermal_util.c \
	 ./thermal_monitor.c \
	 ./sensors/sensors-tsens.c \
	 ./sensors/sensors_manager.c \
	 ./devices/devices_manager.c \
	 ./devices/devices.c \
	 ./devices/devices_actions.c \
	 ./thermal_algorithm.c

if ENABLE_PLATFORM_BASE_DIR
AM_CPPFLAGS += -D_PLATFORM_BASE='"/devices/soc/"'
endif

local_LDFLAGS :=

if ENABLE_THERMAL_IOCTL_LIB
thermal_engine_LDADD := libthermalioctl.la
AM_CPPFLAGS  += -DENABLE_THERMAL_IOCTL
endif

# Legacy targets with old sensor model implementation
if IS_MDM9625
sources += ./sensors/sensors-9625.c
AM_CPPFLAGS += -DENABLE_TSENS_INTERRUPT \
	      -DSENSORS_9625
endif

if IS_MDM9635
sources += ./sensors/sensors-9625.c
AM_CPPFLAGS += -DENABLE_TSENS_INTERRUPT \
	      -DSENSORS_9635
endif

if IS_MSM8974
sources += \
	 ./sensors/sensors-adc.c \
	 ./sensors/sensors-gen.c \
	 ./sensors/sensors-bcl.c \
	 ./sensors/sensors-thermal.c \
	 ./sensors/sensors-8974.c \
	 ./pid_algorithm.c \
	 ./pid-data.c \
	 ./ss_algorithm.c \
	 ./ss-data.c \
	 ./server/thermal_server.c \
	 ./server/thermal_lib_common.c \
	 ./speaker_cal.c \
	 ./speaker_cal-data.c \
	 ./sensors/sensors-virtual.c

AM_CPPFLAGS += -DSENSORS_8974 \
	      -DENABLE_TSENS_INTERRUPT \
	      -DENABLE_PID \
	      -DENABLE_SS \
	      -DENABLE_THERMAL_SERVER \
	      -DENABLE_SPEAKER_CAL \
	      -DENABLE_VIRTUAL_SENSORS
endif


if IS_MSM8610
sources += \
	 ./sensors/sensors-adc.c \
	 ./sensors/sensors-gen.c \
	 ./sensors/sensors-bcl.c \
	 ./sensors/sensors-8610.c \
	 ./pid_algorithm.c \
	 ./pid-data.c \
	 ./ss_algorithm.c \
	 ./ss-data.c \
	 ./server/thermal_server.c \
	 ./server/thermal_lib_common.c \
	 ./sensors/sensors-virtual.c

AM_CPPFLAGS += -DSENSORS_8610 \
	      -DENABLE_TSENS_INTERRUPT \
	      -DENABLE_PID \
	      -DENABLE_SS \
	      -DENABLE_THERMAL_SERVER \
	      -DENABLE_VIRTUAL_SENSORS
endif

if IS_MSM8226
sources += \
	 ./sensors/sensors-adc.c \
	 ./sensors/sensors-gen.c \
	 ./sensors/sensors-bcl.c \
	 ./sensors/sensors-8226.c \
	 ./pid_algorithm.c \
	 ./pid-data.c \
	 ./ss_algorithm.c \
	 ./ss-data.c \
	 ./server/thermal_server.c \
	 ./server/thermal_lib_common.c \
	 ./sensors/sensors-virtual.c

AM_CPPFLAGS += -DSENSORS_8226 \
	      -DENABLE_TSENS_INTERRUPT \
	      -DENABLE_PID \
	      -DENABLE_SS \
	      -DENABLE_THERMAL_SERVER \
	      -DENABLE_VIRTUAL_SENSORS
endif

# Use thermal-engine MDM/MSM source code for new sensor
# model targets

if ENABLE_THERMAL_ENGINE_MDM
sources += ./sensors/sensors.c
sources += ./sensors/sensors-thermal.c
sources += ./sensors/sensors-bcl.c
AM_CPPFLAGS += -DENABLE_TSENS_INTERRUPT
endif

if IS_MDM9607
sources += ./ss_algorithm.c
sources += ./ss-data.c
AM_CPPFLAGS += -DENABLE_SS
endif

if ENABLE_THERMAL_ENGINE_MSM
sources += \
          ./sensors/sensors-bcl.c  \
          ./sensors/sensors.c  \
          ./sensors/sensors-thermal.c  \
          ./pid_algorithm.c  \
          ./pid-data.c \
          ./ss_algorithm.c  \
          ./ss-data.c  \
          ./server/thermal_server.c  \
          ./server/thermal_lib_common.c  \
          ./sensors/sensors-virtual.c \
          ./sensors/sensor-virtual-data.c \
          ./speaker_cal.c \
          ./speaker_cal-data.c

AM_CPPFLAGS +=  -DENABLE_TSENS_INTERRUPT \
               -DENABLE_PID \
               -DENABLE_SS \
               -DENABLE_THERMAL_SERVER \
               -DENABLE_VIRTUAL_SENSORS \
               -DENABLE_SPEAKER_CAL

endif

if IS_QCS605
AM_CPPFLAGS += -DENABLE_QCS605_LC
endif

if ENABLE_CPU_VOLTAGE_DEV
AM_CPPFLAGS += -DENABLE_CPU_VOLTAGE_DEV
endif

if USE_THERMAL_FRAMEWORK
AM_CPPFLAGS += -DUSE_THERMAL_FRAMEWORK
endif

if ENABLE_HVX_MITIGATION
local_LDFLAGS += -lcdsprpc
AM_CPPFLAGS  += -DENABLE_HVX_MITIGATION \
		-I ${ADSPRPC_INCLUDE_PATH}/inc

sources += ./devices/sysmonhvxthrottle_stub.c
endif

if ENABLE_TM_DATA_8916
sources += ./thermal_monitor-data-8916.c
else
sources += ./thermal_monitor-data.c
endif

if ENABLE_OLD_PARSER
sources += ./thermal_config.c
AM_CPPFLAGS += -DENABLE_OLD_PARSER
else
sources += ./thermal_config_v2.c
endif

if USE_QMI
AM_CPPFLAGS += $(QMIF_CFLAGS) \
	       $(QMI_CFLAGS)

local_LDFLAGS += $(QMIF_LIBS) $(QMI_LIBS)

if QMI_TMD
AM_CPPFLAGS += -DENABLE_MODEM_MITIGATION
sources += ./devices/devices-qmi.c \
	   ./devices/thermal_mitigation_device_service_v01.c
endif

if QMI_TS
AM_CPPFLAGS += -DENABLE_MODEM_TS \
			   -DENABLE_MODEM_FE_SENSOR
sources += ./sensors/sensors-qmi.c \
	   ./sensors/thermal_sensor_service_v01.c
endif
endif

if USE_GLIB
common_CFLAGS =  -DUSE_GLIB @GLIB_CFLAGS@
common_LDFLAGS = -lpthread -lrt @GLIB_LIBS@
else
common_LDFLAGS = -lpthread -lrt
endif

thermal_engine_SOURCES = $(sources)

thermal_engine_CFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS) $(common_CFLAGS)
thermal_engine_LDFLAGS = -lpthread -lrt $(common_LDFLAGS) $(local_LDFLAGS)

bin_PROGRAMS = thermal-engine

#Export config file to /etc/
sysconf_DATA = thermal-engine.conf

if IS_MSM8974
sysconf_DATA += thermal-engine-8974.conf
endif

if IS_MSM8610
sysconf_DATA += thermal-engine-8610.conf
endif

if IS_MSM8226
sysconf_DATA += thermal-engine-8226.conf
endif

lib_LTLIBRARIES :=

#Install thermal IOCTL library
if ENABLE_THERMAL_IOCTL_LIB
lib_LTLIBRARIES += libthermalioctl.la
libthermalioctl_la_CFLAGS := $(AM_CFLAGS) $(common_CFLAGS) $(DIAG_CFLAGS) \
                             -I./inc -DENABLE_THERMAL_IOCTL

libthermalioctl_la_SOURCES:= ./ioctl/thermal_ioctl_interface.c
libthermalioctl_la_LDFLAGS := -shared $(common_LDFLAGS) $(DIAG_LIBS)
endif

#Install thermal client library
if ENABLE_THERMAL_CLIENT_LIB
lib_LTLIBRARIES += libthermalclient.la

libthermalclient_la_CFLAGS := $(AM_CFLAGS) $(common_CFLAGS)
libthermalclient_la_CFLAGS += -I$(WORKSPACE)/common/inc
libthermalclient_la_CFLAGS += -I./inc -I./server

libthermalclient_la_SOURCES := ./client/thermal_client.c ./server/thermal_lib_common.c

libthermalclient_la_LDFLAGS := -shared $(common_LDFLAGS)

libthermalclient_la_LIBADD := -lcutils
endif
endif

