# SLIM Daemon make file

# If the platform doesn't have QMI-LOC, SLIM daemon is not needed.
ifeq ($(GPS_LOC_API_V02_ENABLED),true)

LOCAL_PATH := $(call my-dir)
LOCTECH_SLIM_ROOT := $(LOCAL_PATH)

# Feature triggers:
# GPS_LOC_API_V02_ENABLED If true, enables QMI-LOC client by default
ifeq ($(BOARD_VENDOR_QCOM_LOC_PDK_FEATURE_SET),true)
FEATURE_LOCTECH_SLIM_DSPS := false
FEATURE_LOCTECH_SLIM_VNS := false
endif
ifndef FEATURE_LOCTECH_SLIM_DSPS
FEATURE_LOCTECH_SLIM_DSPS := true
endif
ifeq ($(GPS_LOC_API_V02_ENABLED),true)
ifndef FEATURE_LOCTECH_SLIM_QMILOC
FEATURE_LOCTECH_SLIM_QMILOC := true
endif
endif
ifndef FEATURE_LOCTECH_SLIM_NDK
FEATURE_LOCTECH_SLIM_NDK := true
endif
# MP data source is enabled by default
ifndef FEATURE_LOCTECH_QMISLIM_PROVIDER_MP
FEATURE_LOCTECH_QMISLIM_PROVIDER_MP := false
endif
# Test data source is disabled by default
ifndef FEATURE_LOCTECH_QMISLIM_PROVIDER_TEST
FEATURE_LOCTECH_QMISLIM_PROVIDER_TEST := false
endif

# slim_daemon
# FEATURE_LOCTECH_SLIM_NDK    := true  Enables Android Sensor provider
# FEATURE_LOCTECH_SLIM_VNW    := true  Enables Vehicle Network provider
# FEATURE_LOCTECH_SLIM_VNS    := true  Enables Vehicle Network Services (N Car)provider
# FEATURE_LOCTECH_SLIM_DSPS   := true  Enables Sensor1 provider
# FEATURE_LOCTECH_SLIM_QMILOC := true  Enables QMI-LOC client support

include $(CLEAR_VARS)

LOCAL_MODULE := slim_daemon
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_C_INCLUDES := \
  $(LOCAL_PATH)/apss/listener \
  $(LOCAL_PATH)/apss/daemon/ \
  $(LOCAL_PATH)/provider/common \
  $(TARGET_OUT_HEADERS)/common/inc \
  $(TARGET_OUT_HEADERS)/qmi-framework/inc \
  $(TARGET_OUT_HEADERS)/libslimcommon \
  $(TARGET_OUT_HEADERS)/libflp \
  $(TARGET_OUT_HEADERS)/diag/include
LOCAL_HEADER_LIBRARIES := \
  libgps.utils_headers \
  izat_remote_api_headers \
  izat_remote_api_prop_headers \
  libloc_pla_headers \
  liblocation_api_headers
LOCAL_SRC_FILES:= \
  apss/daemon/SlimDaemonManager.cpp \
  apss/listener/ClientListener.cpp \
  provider/common/ProviderBase.cpp \
  provider/common/MultiplexingProvider.cpp \
  provider/common/SlimProviderConf.cpp \
  provider/common/KalmanFilter.cpp
LOCAL_SHARED_LIBRARIES := \
  libgps.utils \
  liblog \
  libdiag \
  libqmi_csi \
  libqmi_cci \
  libqmi_common_so \
  libqmi_encdec

LOCAL_STATIC_LIBRARIES := \
  libslimcommon \
  libslimutils
LOCAL_CFLAGS += \
  $(GPS_FEATURES) \
  -DFEATURE_LOCTECH_SLIM_NOHANDLES \
  -DLOG_TAG=\"slim\" \
  -fvisibility=hidden \
  -DSLIM_API=""
LOCAL_CPPFLAGS += \
  -D__STDC_FORMAT_MACROS \
  -D__STDINT_LIMITS \
  -D__STDINT_MACROS
# Android NDK Provider Support
ifeq ($(FEATURE_LOCTECH_SLIM_NDK),true)
LOCAL_C_INCLUDES += \
  frameworks/native/libs/sensor/include \
  frameworks/hardware/interfaces/sensorservice/libsensorndkbridge \
  hardware/interfaces/sensors/1.0/default/include \
  $(LOCAL_PATH)/provider/ndk
LOCAL_SRC_FILES += \
  provider/ndk/SlimNDKProvider.cpp
LOCAL_SHARED_LIBRARIES += \
  libutils \
  libhwbinder \
  libsensorndkbridge \
  android.hardware.sensors@1.0 \
  android.frameworks.sensorservice@1.0

LOCAL_CFLAGS           += -DFEATURE_LOCTECH_SLIM_NDK -DFEATURE_LOCTECH_NEW_NDK_API

endif # FEATURE_LOCTECH_SLIM_NDK

# QMI-SLIM Provider Support
ifeq ($(FEATURE_LOCTECH_QMISLIM_PROVIDER_MP),true)
LOCAL_CFLAGS           += -DFEATURE_LOCTECH_QMISLIM_PROVIDER_MP
endif
ifeq ($(FEATURE_LOCTECH_QMISLIM_PROVIDER_TEST),true)
LOCAL_CFLAGS           += -DFEATURE_LOCTECH_QMISLIM_PROVIDER_TEST
endif
ifneq ($(FEATURE_LOCTECH_QMISLIM_PROVIDER_MP)$(FEATURE_LOCTECH_QMISLIM_PROVIDER_TEST),)
LOCAL_CFLAGS           += -DFEATURE_LOCTECH_QMISLIM_PROVIDER
endif
# Sensor1 Provider Support
ifeq ($(FEATURE_LOCTECH_SLIM_DSPS),true)
LOCAL_C_INCLUDES += \
  $(LOCAL_PATH)/provider/sensor1 \
  $(TARGET_OUT_HEADERS)/sensors/inc
LOCAL_SRC_FILES += \
  provider/sensor1/SlimSensor1Provider.cpp
LOCAL_SHARED_LIBRARIES += libsensor1
LOCAL_CFLAGS           += -DFEATURE_LOCTECH_SLIM_DSPS
endif
# Vehicle Network Provider Support
ifeq ($(FEATURE_LOCTECH_SLIM_VNW),true)
LOCAL_C_INCLUDES += \
  $(LOCAL_PATH)/provider/vnw \
  $(TARGET_OUT_HEADERS)/CanWrapper
LOCAL_SRC_FILES += \
  provider/vnw/VehicleNetworkProvider.cpp \
  provider/vnw/VehicleNetworkConfiguration.cpp \
  provider/vnw/VehicleNetworkMessageProcessor.cpp \
  provider/vnw/BitStreamReader.cpp
LOCAL_SHARED_LIBRARIES += \
  libcanwrapper
LOCAL_CFLAGS           += -DFEATURE_LOCTECH_SLIM_VNW
endif
# Vehicle Network Service (N car)Provider Support
ifeq ($(FEATURE_LOCTECH_SLIM_VNS),true)
LOCAL_C_INCLUDES += \
  $(LOCAL_PATH)/provider/vns \
  hardware/interfaces/automotive/vehicle/2.0/default/common/include/ \
  $(TOP)/android/hardware/automotive/vehicle/2.0/
LOCAL_SRC_FILES += \
  provider/vns/VNSUtils.cpp \
  provider/vns/VNSProvider.cpp \
  provider/vns/VNSListener.cpp
LOCAL_SHARED_LIBRARIES += libbase \
  libbinder \
  libhidlbase \
  libhidltransport \
  libhwbinder \
  liblog \
  libprotobuf-cpp-lite \
  libutils \
  android.hardware.automotive.vehicle@2.0 \
  android.hardware.automotive.vehicle@2.0-manager-lib-shared

LOCAL_CFLAGS           += -DFEATURE_LOCTECH_SLIM_VNS
endif
# QMI-LOC Modem Client Support
ifeq ($(FEATURE_LOCTECH_SLIM_QMILOC),true)
LOCAL_HEADER_LIBRARIES += \
  libloc_api_v02_headers \
  libloc_core_headers \
  liblbs_core_headers
LOCAL_SHARED_LIBRARIES += \
  libloc_api_v02 \
  libloc_core \
  liblbs_core
LOCAL_SRC_FILES += \
  apss/listener/QLClientListener.cpp
LOCAL_CFLAGS           += -DFEATURE_LOCTECH_SLIM_QMILOC
endif

ifeq ($(TARGET_BUILD_VARIANT),$(filter $(TARGET_BUILD_VARIANT),userdebug eng))
LOCAL_CFLAGS           += -DFEATURE_LOCTECH_SLIM_DEBUG
endif

LOCAL_CFLAGS += $(GNSS_CFLAGS)
include $(BUILD_EXECUTABLE)

include $(call all-makefiles-under,$(LOCAL_PATH))

LOCTECH_SLIM_ROOT :=

endif

