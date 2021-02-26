BOARD_PLATFORM_LIST := msm8960
BOARD_PLATFORM_LIST += msm8974
BOARD_PLATFORM_LIST += msm8226
BOARD_PLATFORM_LIST += msm8610
BOARD_PLATFORM_LIST += apq8084
BOARD_PLATFORM_LIST += mpq8092
BOARD_PLATFORM_LIST += msm8916
BOARD_PLATFORM_LIST += msm8994
BOARD_PLATFORM_LIST += msm8909
BOARD_PLATFORM_LIST += msm8992
BOARD_PLATFORM_LIST += msm8996
BOARD_PLATFORM_LIST += msm8996_gvmq
BOARD_PLATFORM_LIST += msm8952
BOARD_PLATFORM_LIST += msm8937
BOARD_PLATFORM_LIST += msm8953
BOARD_PLATFORM_LIST += msm8998
BOARD_PLATFORM_LIST += apq8098_latv
BOARD_PLATFORM_LIST += sdm660
BOARD_PLATFORM_LIST += sdm845
BOARD_PLATFORM_LIST += sdm670
BOARD_PLATFORM_LIST += qcs605

ifeq ($(call is-board-platform-in-list,$(BOARD_PLATFORM_LIST)),true)

IS_MSM8960:=$(strip $(call is-board-platform,msm8960))
IS_MSM8974:=$(strip $(call is-board-platform,msm8974))
IS_MSM8226:=$(strip $(call is-board-platform,msm8226))
IS_MSM8610:=$(strip $(call is-board-platform,msm8610))
IS_APQ8084:=$(strip $(call is-board-platform,apq8084))
IS_MPQ8092:=$(strip $(call is-board-platform,mpq8092))
IS_MSM8916:=$(strip $(call is-board-platform,msm8916))
IS_MSM8994:=$(strip $(call is-board-platform,msm8994))
IS_MSM8909:=$(strip $(call is-board-platform,msm8909))
IS_MSM8992:=$(strip $(call is-board-platform,msm8992))
IS_MSM8996:=$(strip $(call is-board-platform,msm8996))
IS_MSM8996GVMQ:=$(strip $(call is-board-platform,msm8996_gvmq))
IS_MSM8952:=$(strip $(call is-board-platform,msm8952))
IS_MSM8937:=$(strip $(call is-board-platform,msm8937))
IS_MSM8953:=$(strip $(call is-board-platform,msm8953))
IS_MSM8998:=$(strip $(call is-board-platform,msm8998))
IS_APQ8098LATV:=$(strip $(call is-board-platform,apq8098_latv))
IS_SDM660:=$(strip $(call is-board-platform,sdm660))
IS_SDM845:=$(strip $(call is-board-platform,sdm845))
IS_SDM670:=$(strip $(call is-board-platform,sdm670))
IS_QCS605:=$(strip $(call is-board-platform,qcs605))

LOCAL_PATH := $(call my-dir)

ifneq  ($(filter arm64 ,$(TARGET_ARCH)),)
common_CFLAGS := -Wall -Wextra -Wconversion -Wsign-conversion \
                 -Wno-error=conversion -Wno-error=sign-conversion \
                 -Wno-strict-aliasing \
		 -Wunused-variable -Wno-unused-parameter
else ifneq  ($(filter arm ,$(TARGET_ARCH)),)
common_CFLAGS := -Wall -Wextra \
                 -Wunused-variable -Wno-unused-parameter
endif

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := \
        $(TARGET_OUT_HEADERS)/common/inc \
        $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include/ \
        $(LOCAL_PATH)/inc \
	device/qcom/common/power

LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_SRC_FILES := \
        thermal.c \
        thermal_monitor.c \
        thermal_util.c \
        thermal_algorithm.c

LOCAL_SHARED_LIBRARIES :=

LOCAL_CFLAGS := -DCONFIG_FILE_DEFAULT='"/$(TARGET_COPY_OUT_VENDOR)/etc/thermal-engine.conf"'
LOCAL_CFLAGS += $(common_CFLAGS)

ifneq ($(call is-platform-sdk-version-at-least,20),true)
  LOCAL_CFLAGS += -include bionic/libc/kernel/arch-arm/asm/posix_types.h
endif
ifneq ($(call is-platform-sdk-version-at-least,17),true)
  # Override socket-related kernel headers with Bionic version before JB MR1
  LOCAL_CFLAGS += -include bionic/libc/kernel/arch-arm/asm/byteorder.h
  LOCAL_CFLAGS += -include bionic/libc/kernel/common/linux/posix_types.h
  LOCAL_CFLAGS += -include bionic/libc/kernel/common/linux/types.h
  LOCAL_CFLAGS += -include bionic/libc/kernel/common/linux/socket.h
  LOCAL_CFLAGS += -include bionic/libc/kernel/common/linux/in.h
  LOCAL_CFLAGS += -include bionic/libc/kernel/common/linux/netlink.h
  LOCAL_CFLAGS += -include bionic/libc/kernel/common/linux/un.h
endif

LOCAL_CFLAGS += -D_PLATFORM_BASE='"/devices/soc/"'

ifneq ($(IS_MSM8960),)
LOCAL_SRC_FILES += ./thermal_monitor-data.c \
                   ./sensors/sensors-tsens.c ./sensors/sensors-adc.c \
                   ./sensors/sensors-gen.c ./sensors/sensors-bcl.c \
                   ./sensors/sensors_manager.c ./sensors/sensors-8960.c \
                   ./sensors/sensors-pm8821.c \
                   ./devices/devices_manager.c ./devices/devices.c \
                   ./devices/devices_actions.c
LOCAL_CFLAGS += -DSENSORS_8960
LOCAL_CFLAGS += -DENABLE_TSENS_INTERRUPT
ENABLE_OLD_PARSER:=1
ENABLE_MODEM_MITIGATION_QMI:=1
ENABLE_MODEM_TS_QMI:=1
ENABLE_PID:=1
ENABLE_SS:=1
ENABLE_THERMAL_SERVER:=1
endif

ifneq ($(IS_MSM8974),)
LOCAL_SRC_FILES += ./thermal_monitor-data.c \
                   ./sensors/sensors-tsens.c ./sensors/sensors-adc.c \
                   ./sensors/sensors-gen.c ./sensors/sensors-bcl.c \
                   ./sensors/sensors-thermal.c \
                   ./sensors/sensors_manager.c ./sensors/sensors-8974.c \
                   ./devices/devices_manager.c ./devices/devices.c \
                   ./devices/devices_actions.c
LOCAL_CFLAGS += -DSENSORS_8974
LOCAL_CFLAGS += -DENABLE_TSENS_INTERRUPT
ENABLE_OLD_PARSER:=1
ENABLE_MODEM_MITIGATION_QMI:=1
ENABLE_MODEM_TS_QMI:=1
ENABLE_PID:=1
ENABLE_SS:=1
ENABLE_THERMAL_SERVER:=1
ENABLE_SPEAKER_CAL:=1
ENABLE_VIRTUAL_SENSORS:=1
LOCAL_CFLAGS += -DENABLE_THERMAL_IOCTL
endif

ifneq ($(IS_MSM8226),)
LOCAL_SRC_FILES += ./thermal_monitor-data.c \
                   ./sensors/sensors-tsens.c ./sensors/sensors-adc.c \
                   ./sensors/sensors-gen.c ./sensors/sensors-bcl.c \
                   ./sensors/sensors_manager.c ./sensors/sensors-8226.c \
                   ./devices/devices_manager.c ./devices/devices.c \
                   ./devices/devices_actions.c
LOCAL_CFLAGS += -DSENSORS_8226
LOCAL_CFLAGS += -DENABLE_TSENS_INTERRUPT
ENABLE_OLD_PARSER:=1
ENABLE_MODEM_MITIGATION_QMI:=1
ENABLE_MODEM_TS_QMI:=1
ENABLE_PID:=1
ENABLE_SS:=1
ENABLE_THERMAL_SERVER:=1
ENABLE_VIRTUAL_SENSORS:=1
LOCAL_CFLAGS += -DENABLE_THERMAL_IOCTL
endif

ifneq ($(IS_MSM8610),)
LOCAL_SRC_FILES += ./thermal_monitor-data.c \
                   ./sensors/sensors-tsens.c ./sensors/sensors-adc.c \
                   ./sensors/sensors-gen.c ./sensors/sensors-bcl.c \
                   ./sensors/sensors_manager.c ./sensors/sensors-8610.c \
                   ./devices/devices_manager.c ./devices/devices.c \
                   ./devices/devices_actions.c
LOCAL_CFLAGS += -DSENSORS_8610
LOCAL_CFLAGS += -DENABLE_TSENS_INTERRUPT
ENABLE_OLD_PARSER:=1
ENABLE_MODEM_MITIGATION_QMI:=1
ENABLE_MODEM_TS_QMI:=1
ENABLE_PID:=1
ENABLE_SS:=1
ENABLE_THERMAL_SERVER:=1
ENABLE_VIRTUAL_SENSORS:=1
LOCAL_CFLAGS += -DENABLE_THERMAL_IOCTL
endif

ifneq ($(IS_APQ8084),)
LOCAL_SRC_FILES += ./thermal_monitor-data.c \
                   ./sensors/sensors-tsens.c ./sensors/sensors-adc.c \
                   ./sensors/sensors-gen.c ./sensors/sensors-bcl.c \
                   ./sensors/sensors_manager.c ./sensors/sensors-8084.c \
                   ./devices/devices_manager.c ./devices/devices.c \
                   ./devices/devices_actions.c ./sensors/sensors-thermal.c
LOCAL_CFLAGS += -DENABLE_TSENS_INTERRUPT
ENABLE_MODEM_MITIGATION_QMI:=1
ENABLE_MODEM_TS_QMI:=1
ENABLE_PID:=1
ENABLE_SS:=1
ENABLE_THERMAL_SERVER:=1
ENABLE_SPEAKER_CAL:=1
ENABLE_VIRTUAL_SENSORS:=1
LOCAL_CFLAGS += -DENABLE_THERMAL_IOCTL
endif

ifneq ($(IS_MPQ8092),)
LOCAL_SRC_FILES += ./thermal_monitor-data.c \
                   ./sensors/sensors-tsens.c ./sensors/sensors-adc.c \
                   ./sensors/sensors-gen.c \
                   ./sensors/sensors_manager.c ./sensors/sensors-8092.c \
                   ./devices/devices_manager.c ./devices/devices.c \
                   ./devices/devices_actions.c
LOCAL_CFLAGS += -DENABLE_TSENS_INTERRUPT
ENABLE_PID:=1
ENABLE_SS:=1
ENABLE_THERMAL_SERVER:=1
ENABLE_VIRTUAL_SENSORS:=1
LOCAL_CFLAGS += -DENABLE_THERMAL_IOCTL
endif

ifneq ($(IS_MSM8916),)
LOCAL_SRC_FILES += ./thermal_monitor-data-8916.c \
                   ./sensors/sensors-tsens.c ./sensors/sensors-adc.c \
                   ./sensors/sensors-thermal.c ./sensors/sensors-bcl.c \
                   ./sensors/sensors_manager.c ./sensors/sensors-8916.c \
                   ./sensors/sensors-bcm.c \
                   ./devices/devices_manager.c ./devices/devices.c \
                   ./devices/devices_actions.c target-8939-algo.c
LOCAL_CFLAGS += -DSENSORS_8916
LOCAL_CFLAGS += -DENABLE_TSENS_INTERRUPT
LOCAL_CFLAGS += -DENABLE_TARGET_ALGO
ENABLE_MODEM_MITIGATION_QMI:=1
ENABLE_MODEM_TS_QMI:=1
ENABLE_PID:=1
ENABLE_SS:=1
ENABLE_THERMAL_SERVER:=1
ENABLE_VIRTUAL_SENSORS:=1
LOCAL_CFLAGS += -DENABLE_THERMAL_IOCTL
endif

ifneq (,$(IS_MSM8996)$(IS_MSM8996GVMQ))
LOCAL_CFLAGS += -DENABLE_CPU_VOLTAGE_DEV
endif

ifneq (,$(IS_MSM8994)$(IS_MSM8992)$(IS_MSM8996)$(IS_MSM8996GVMQ)$(IS_MSM8998)$(IS_APQ8098LATV)$(IS_SDM660))
LOCAL_SRC_FILES += ./thermal_monitor-data.c ./sensors/sensors-bcl.c \
                   ./sensors/sensors-tsens.c ./sensors/sensors-adc.c \
                   ./sensors/sensors_manager.c ./sensors/sensors.c \
                   ./devices/devices_manager.c ./devices/devices.c \
                   ./devices/devices_actions.c ./sensors/sensors-thermal.c
LOCAL_CFLAGS += -DENABLE_TSENS_INTERRUPT
ENABLE_MODEM_MITIGATION_QMI:=1
ENABLE_MODEM_TS_QMI:=1
ENABLE_PID:=1
ENABLE_SS:=1
ENABLE_THERMAL_SERVER:=1
ENABLE_SPEAKER_CAL:=1
ENABLE_VIRTUAL_SENSORS:=1
ENABLE_TB:=1
ENABLE_LIMITS_PRIV:=1
LOCAL_CFLAGS += -DENABLE_THERMAL_IOCTL
endif

ifneq (,$(IS_MSM8909)$(IS_MSM8952))
LOCAL_SRC_FILES += ./thermal_monitor-data-8916.c ./sensors/sensors-bcl.c \
                   ./sensors/sensors-tsens.c ./sensors/sensors_manager.c \
                   ./sensors/sensors.c ./sensors/sensors-thermal.c \
                   ./devices/devices_manager.c ./devices/devices.c \
                   ./devices/devices_actions.c ./target-8939-algo.c
LOCAL_CFLAGS += -DENABLE_TSENS_INTERRUPT
LOCAL_CFLAGS += -DENABLE_TARGET_ALGO
ENABLE_MODEM_MITIGATION_QMI:=1
ENABLE_MODEM_TS_QMI:=1
ENABLE_PID:=1
ENABLE_SS:=1
ENABLE_THERMAL_SERVER:=1
ENABLE_VIRTUAL_SENSORS:=1
ENABLE_SPEAKER_CAL:=1
LOCAL_CFLAGS += -DENABLE_THERMAL_IOCTL
endif

ifneq (,$(IS_SDM845)$(IS_QCS605)$(IS_SDM670)$(IS_MSM8953)$(IS_MSM8937))
LOCAL_SRC_FILES += ./thermal_monitor-data.c ./sensors/sensors-bcl.c \
                   ./sensors/sensors-tsens.c ./sensors/sensors-adc.c \
                   ./sensors/sensors_manager.c ./sensors/sensors.c \
                   ./devices/devices_manager.c ./devices/devices.c \
                   ./devices/devices_actions.c ./sensors/sensors-thermal.c
LOCAL_CFLAGS += -DENABLE_TSENS_INTERRUPT
ENABLE_MODEM_MITIGATION_QMI:=1
ENABLE_PID:=1
ENABLE_SS:=1
ENABLE_THERMAL_SERVER:=1
ENABLE_SPEAKER_CAL:=1
ENABLE_VIRTUAL_SENSORS:=1
USE_THERMAL_FRAMEWORK:=1
endif

ifneq (,$(IS_SDM845))
ENABLE_MODEM_TS_QMI:=1
LOCAL_CFLAGS += -DENABLE_MODEM_QFE_SENSOR
endif

ifneq (,$(IS_QCS605))
ENABLE_HVX_MITIGATION:=1
endif

ifneq ($(USE_THERMAL_FRAMEWORK),)
LOCAL_CFLAGS += -DUSE_THERMAL_FRAMEWORK
LOCAL_SHARED_LIBRARIES += libcutils liblog libdl
else
INCLUDE_IOCTL_LIB:=1
LOCAL_SHARED_LIBRARIES += libcutils libthermalioctl liblog libdl
endif

ifneq ($(ENABLE_OLD_PARSER),)
LOCAL_SRC_FILES += thermal_config.c
LOCAL_CFLAGS += -DENABLE_OLD_PARSER
else
LOCAL_SRC_FILES += thermal_config_v2.c
endif

ifneq ($(ENABLE_MODEM_MITIGATION_QMI),)
LOCAL_CFLAGS += -DENABLE_MODEM_MITIGATION
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi-framework/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi/inc
LOCAL_SRC_FILES += ./devices/devices-qmi.c
LOCAL_SRC_FILES += ./devices/thermal_mitigation_device_service_v01.c
LOCAL_SHARED_LIBRARIES += libqmi_cci libqmi_common_so
endif

ifneq ($(ENABLE_MODEM_TS_QMI),)
LOCAL_CFLAGS += -DENABLE_MODEM_TS
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi-framework/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi/inc
LOCAL_SRC_FILES += ./sensors/sensors-qmi.c
LOCAL_SRC_FILES += ./sensors/thermal_sensor_service_v01.c
LOCAL_SHARED_LIBRARIES += libqmi_cci libqmi_common_so
endif

ifneq ($(ENABLE_PID),)
LOCAL_CFLAGS += -DENABLE_PID
LOCAL_SRC_FILES += pid_algorithm.c
LOCAL_SRC_FILES += pid-data.c
endif

ifneq ($(ENABLE_SS),)
LOCAL_CFLAGS += -DENABLE_SS
LOCAL_SRC_FILES += ss_algorithm.c
LOCAL_SRC_FILES += ss-data.c
endif

ifneq ($(ENABLE_TB),)
LOCAL_CFLAGS += -DENABLE_TB
LOCAL_SRC_FILES += tb-data.c
endif

ifneq ($(ENABLE_THERMAL_SERVER),)
LOCAL_CFLAGS += -DENABLE_THERMAL_SERVER
LOCAL_SRC_FILES += ./server/thermal_server.c
LOCAL_SRC_FILES += ./server/thermal_lib_common.c
endif

ifneq ($(ENABLE_SPEAKER_CAL),)
LOCAL_CFLAGS += -DENABLE_SPEAKER_CAL
LOCAL_SRC_FILES += ./speaker_cal.c
LOCAL_SRC_FILES += ./speaker_cal-data.c
endif

ifneq ($(ENABLE_VIRTUAL_SENSORS),)
LOCAL_CFLAGS += -DENABLE_VIRTUAL_SENSORS
LOCAL_SRC_FILES += ./sensors/sensors-virtual.c
LOCAL_SRC_FILES += ./sensors/sensor-virtual-data.c
endif

ifneq ($(ENABLE_LIMITS_PRIV),)
LOCAL_STATIC_LIBRARIES := liblimits_priv
endif

ifneq ($(ENABLE_HVX_MITIGATION),)
LOCAL_CFLAGS += -DENABLE_HVX_MITIGATION
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/include/fastrpc/inc
LOCAL_SHARED_LIBRARIES += libcdsprpc
LOCAL_SRC_FILES += ./devices/sysmonhvxthrottle_stub.c
endif

LOCAL_CFLAGS += \
        -DUSE_ANDROID_LOG \

LOCAL_MODULE := thermal-engine
LOCAL_CLANG := true
LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_EXECUTABLE)

ifneq ($(call is-platform-sdk-version-at-least,26),true)
#Install thermal client library
include $(CLEAR_VARS)

LOCAL_C_INCLUDES := \
        $(TARGET_OUT_HEADERS)/common/inc \
        $(LOCAL_PATH)/inc \
        $(LOCAL_PATH)/server


LOCAL_SHARED_LIBRARIES := libcutils liblog

LOCAL_SRC_FILES:= ./client/thermal_client.c ./server/thermal_lib_common.c
LOCAL_CLANG := true
LOCAL_MODULE:= libthermalclient
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS := -DUSE_ANDROID_LOG
LOCAL_CFLAGS += $(common_CFLAGS)
LOCAL_MODULE_OWNER := qcom
LOCAL_PROPRIETARY_MODULE := true
LOCAL_C_INCLUDES += device/qcom/common/thermal-engine/

include $(BUILD_SHARED_LIBRARY)
endif  # thermal client lib

#Install thermal ioctl library
ifneq ($(INCLUDE_IOCTL_LIB),)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := \
        $(TARGET_OUT_HEADERS)/common/inc \
        $(LOCAL_PATH)/inc \
        $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include/

LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
LOCAL_SHARED_LIBRARIES := libcutils liblog

LOCAL_SRC_FILES:= ./ioctl/thermal_ioctl_interface.c
LOCAL_MODULE:= libthermalioctl
LOCAL_CLANG := true
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS := -DUSE_ANDROID_LOG
LOCAL_CFLAGS += $(common_CFLAGS)
LOCAL_CFLAGS += -DENABLE_THERMAL_IOCTL
LOCAL_MODULE_OWNER := qcom
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)
endif

include $(CLEAR_VARS)
LOCAL_MODULE       := thermal-engine.conf
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := thermal-engine.conf
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

# Install thermal configuration
ifneq ($(IS_MSM8974),)
include $(CLEAR_VARS)
LOCAL_MODULE       := thermal-engine-8974.conf
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := thermal-engine-8974.conf
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

else ifneq ($(IS_MSM8226),)
include $(CLEAR_VARS)
LOCAL_MODULE       := thermal-engine-8226.conf
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := thermal-engine-8226.conf
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

else ifneq ($(IS_MSM8610),)
include $(CLEAR_VARS)
LOCAL_MODULE       := thermal-engine-8610.conf
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := thermal-engine-8610.conf
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

else ifneq ($(IS_MSM8960),)
include $(CLEAR_VARS)
LOCAL_MODULE       := thermal-engine-8960.conf
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := thermal-engine-8960.conf
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := thermal-engine-8064.conf
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := thermal-engine-8064.conf
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := thermal-engine-8064ab.conf
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := thermal-engine-8064ab.conf
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := thermal-engine-8930.conf
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := thermal-engine-8930.conf
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

endif
