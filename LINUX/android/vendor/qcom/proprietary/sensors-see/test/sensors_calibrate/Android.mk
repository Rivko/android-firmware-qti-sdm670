LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_COPY_HEADERS_TO := sensors/inc
LOCAL_COPY_HEADERS := sensors_calibrate.h

COMMON_INCLUDES :=
COMMON_INCLUDES += $(TARGET_OUT_HEADERS)/qmi-framework/inc
COMMON_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/../gen/SHARED_LIBRARIES/libssc_intermediates/proto
COMMON_INCLUDES += $(LOCAL_PATH)/../../ssc
COMMON_INCLUDES += $(LOCAL_PATH)/../../sensors-log

LOCAL_C_INCLUDES := $(COMMON_INCLUDES)
LOCAL_C_INCLUDES += $(shell find $(LOCAL_PATH) -type d -name 'inc' -print )

LOCAL_MODULE := libsensorcal
LOCAL_CLANG := true
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libprotobuf-cpp-full
LOCAL_SHARED_LIBRARIES += libsensorslog
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libssc

LOCAL_SRC_FILES += \
  $(shell find $(LOCAL_PATH)/* -name '*.cpp' | sed s:^$(LOCAL_PATH)/::g )

LOCAL_CFLAGS += -Wall -Wextra -Werror -Wno-missing-field-initializers -Wno-maybe-uninitialized
ifeq ($(LLVM_SENSORS_SEE),true)
LOCAL_CFLAGS += --compile-and-analyze --analyzer-perf --analyzer-Werror
endif

LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
