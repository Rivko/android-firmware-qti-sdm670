LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

c_sources =
c_sources += parse.cpp
c_sources += por.cpp
c_sources += salt_usta.cpp
c_sources += see_salt.cpp
c_sources += see_salt_payloads.cpp
c_sources += see_salt_sensor.cpp
ifneq ($(BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE),)
c_sources += see_salt_sleep.cpp
endif

c_includes =
c_includes += $(LOCAL_PATH)
c_includes += $(TARGET_OUT_INTERMEDIATES)/../gen/SHARED_LIBRARIES/libssc_intermediates/proto
c_includes += $(LOCAL_PATH)/../../ssc
c_includes += $(LOCAL_PATH)/../../sensors-log
c_includes += $(LOCAL_PATH)/../../USTA/native
c_includes += $(LOCAL_PATH)/../../sensors-diag-log/inc

ifneq ($(BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE),)
LOCAL_HEADER_LIBRARIES += \
    libgps.utils_headers \
    libloc_pla_headers
endif

c_flags = -Wno-unused-variable -Wno-unused-parameter -fexceptions -g
ifeq ($(LLVM_SENSORS_SEE),true)
c_flags += --compile-and-analyze --analyzer-perf --analyzer-Werror
endif

shared_libs =
shared_libs += libUSTANative
shared_libs += libprotobuf-cpp-full
shared_libs += libssc
ifneq ($(BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE),)
shared_libs += libgps.utils
endif

LOCAL_MODULE := libSEESalt
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_CLANG := true
LOCAL_MODULE_OWNER := qti
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true
LOCAL_SRC_FILES := $(c_sources)
LOCAL_C_INCLUDES := $(c_includes)
LOCAL_CFLAGS := $(c_flags)
LOCAL_SHARED_LIBRARIES := $(shared_libs)

include $(BUILD_SHARED_LIBRARY)
