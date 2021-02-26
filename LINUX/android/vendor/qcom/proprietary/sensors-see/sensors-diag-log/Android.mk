LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libsnsdiaglog
LOCAL_MODULE_CLASS := SHARED_LIBRARIES

# Local source files
c_sources =
c_sources += src/sensor_diag_log.cpp

# Include paths
c_includes =
c_includes += $(TARGET_OUT_INTERMEDIATES)/../gen/SHARED_LIBRARIES/libssc_intermediates/proto
c_includes += $(LOCAL_PATH)/../sensors-log
c_includes += $(TARGET_OUT_HEADERS)/diag/include
c_includes += $(TARGET_OUT_HEADERS)/common/inc
c_includes += $(LOCAL_PATH)/inc/
c_includes += $(LOCAL_PATH)/../sensors-hal/framework

# shared libraries
shared_libs =
shared_libs += liblog
shared_libs += libprotobuf-cpp-full
shared_libs += libdiag
shared_libs += libssc
shared_libs += libsensorslog
shared_libs += libutils
shared_libs += libcutils

c_export_includes =
c_export_includes += $(LOCAL_PATH)

## Wno-unused-parameter required to ignore some warnings
## coming from google protobuf code
c_flags = -Werror -Wall -Wunused-parameter -fexceptions

ifeq ($(LLVM_SENSORS_SEE),true)
c_flags += --compile-and-analyze --analyzer-perf --analyzer-Werror
endif

# auto-generated protobuf files require this preprocessor macro
cpp_flags = -DGOOGLE_PROTOBUF_NO_RTTI

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
LOCAL_EXPORT_C_INCLUDE_DIRS := $(c_export_includes)
LOCAL_SHARED_LIBRARIES = $(shared_libs)

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libsnsdiaglog_system
LOCAL_MODULE_CLASS := SHARED_LIBRARIES

# Local source files
c_sources =
c_sources += src/sensor_diag_log.cpp

# Include paths
c_includes =
c_includes += $(TARGET_OUT_INTERMEDIATES)/../gen/SHARED_LIBRARIES/libssc_system_intermediates/proto
c_includes += $(LOCAL_PATH)/../sensors-log
c_includes += $(TARGET_OUT_HEADERS)/diag/include
c_includes += $(TARGET_OUT_HEADERS)/common/inc
c_includes += $(LOCAL_PATH)/inc/
c_includes += $(LOCAL_PATH)/../sensors-hal/framework

# shared libraries
shared_libs =
shared_libs += liblog
shared_libs += libprotobuf-cpp-full
shared_libs += libdiag_system
shared_libs += libssc_system
shared_libs += libsensorslog_system
shared_libs += libutils
shared_libs += libcutils

c_export_includes =
c_export_includes += $(LOCAL_PATH)

## Wno-unused-parameter required to ignore some warnings
## coming from google protobuf code
c_flags = -Werror -Wall -Wunused-parameter -fexceptions

ifeq ($(LLVM_SENSORS_SEE),true)
c_flags += --compile-and-analyze --analyzer-perf --analyzer-Werror
endif

# auto-generated protobuf files require this preprocessor macro
cpp_flags = -DGOOGLE_PROTOBUF_NO_RTTI

## C/C++ compiler flags
LOCAL_CFLAGS   = $(c_flags)
ifneq ($(TARGET_SUPPORTS_WEARABLES),true)
LOCAL_INSTALLED_MODULE_STEM := libsnsdiaglog.so
endif
LOCAL_CLANG := true
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_SRC_FILES = $(c_sources)
LOCAL_CPPFLAGS = $(cpp_flags)
LOCAL_C_INCLUDES = $(c_includes)
LOCAL_EXPORT_C_INCLUDE_DIRS := $(c_export_includes)
LOCAL_SHARED_LIBRARIES = $(shared_libs)

include $(BUILD_SHARED_LIBRARY)


# TODO: add export_includes for this library
