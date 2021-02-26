LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libSensorCalLibNative
LOCAL_MODULE_CLASS := SHARED_LIBRARIES

c_sources =
c_sources += see_calibration_accel.cpp
c_sources += see_calibration_jni.cpp

c_includes =
c_includes += $(LOCAL_PATH)
c_includes += $(TARGET_OUT_INTERMEDIATES)/../gen/SHARED_LIBRARIES/libssc_intermediates/proto
c_includes += $(LOCAL_PATH)/../../ssc
c_includes += $(LOCAL_PATH)/../../sensors-log

shared_libs =
shared_libs += libssc
shared_libs += liblog
shared_libs += libprotobuf-cpp-full
shared_libs += libsensorslog

c_flags = -Werror -Wno-unused-parameter
ifeq ($(LLVM_SENSORS_SEE),true)
c_flags += --compile-and-analyze --analyzer-perf --analyzer-Werror
endif
## C/C++ compiler flags
LOCAL_CFLAGS   = $(c_flags)

LOCAL_CLANG := true
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_SRC_FILES = $(c_sources)
LOCAL_CPPFLAGS = $(cpp_flags)
LOCAL_C_INCLUDES = $(c_includes)
LOCAL_SHARED_LIBRARIES = $(shared_libs)

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libSensorCalLibNative_system
LOCAL_MODULE_CLASS := SHARED_LIBRARIES

c_sources =
c_sources += see_calibration_accel.cpp
c_sources += see_calibration_jni.cpp

c_includes =
c_includes += $(LOCAL_PATH)
c_includes += $(TARGET_OUT_INTERMEDIATES)/../gen/SHARED_LIBRARIES/libssc_system_intermediates/proto
c_includes += $(LOCAL_PATH)/../../ssc
c_includes += $(LOCAL_PATH)/../../sensors-log

shared_libs =
shared_libs += libssc_system
shared_libs += liblog
shared_libs += libprotobuf-cpp-full
shared_libs += libsensorslog_system


LOCAL_INSTALLED_MODULE_STEM := libSensorCalLibNative.so


c_flags = -Werror -Wno-unused-parameter
ifeq ($(LLVM_SENSORS_SEE),true)
c_flags += --compile-and-analyze --analyzer-perf --analyzer-Werror
endif
## C/C++ compiler flags
LOCAL_CFLAGS   = $(c_flags)

LOCAL_CLANG := true
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_SRC_FILES = $(c_sources)
LOCAL_CPPFLAGS = $(cpp_flags)
LOCAL_C_INCLUDES = $(c_includes)
LOCAL_SHARED_LIBRARIES = $(shared_libs)

include $(BUILD_SHARED_LIBRARY)
