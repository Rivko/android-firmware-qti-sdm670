LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

c_sources =
c_sources += ssc_threshold.cpp

c_includes =
c_includes += $(LOCAL_PATH)
c_includes += $(TARGET_OUT_INTERMEDIATES)/../gen/SHARED_LIBRARIES/libssc_intermediates/proto
c_includes += $(LOCAL_PATH)/../../ssc
c_includes += $(LOCAL_PATH)/../../sensors-log
c_includes += $(TARGET_OUT_HEADERS)/qmi-framework/inc

c_flags = -Wno-unused-variable -Wno-unused-parameter -fexceptions -g
ifeq ($(LLVM_SENSORS_SEE),true)
c_flags += --compile-and-analyze --analyzer-perf --analyzer-Werror
endif


shared_libs =
shared_libs += libssc
shared_libs += liblog
shared_libs += libsensorslog
shared_libs += libprotobuf-cpp-full

LOCAL_MODULE := ssc_threshold
LOCAL_MODULE_OWNER := qti
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true
LOCAL_SRC_FILES := $(c_sources)
LOCAL_C_INCLUDES := $(c_includes)
LOCAL_CFLAGS := $(c_flags)
LOCAL_SHARED_LIBRARIES := $(shared_libs)

include $(BUILD_EXECUTABLE)
