LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

# Local source files
c_sources =
c_sources += sensors_log.cpp

# Include paths
c_includes =
c_includes += $(LOCAL_PATH)

# shared libraries
shared_libs =
shared_libs += liblog

c_export_includes =
c_export_includes += $(LOCAL_PATH)

## Wno-unused-parameter required to ignore some warnings
## coming from google protobuf code
c_flags = -Werror -Wall -Wno-unused-parameter -fexceptions
ifeq ($(LLVM_SENSORS_SEE),true)
c_flags += --compile-and-analyze --analyzer-perf --analyzer-Werror
endif

LOCAL_MODULE := libsensorslog
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
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

# Local source files
c_sources =
c_sources += sensors_log.cpp

# Include paths
c_includes =
c_includes += $(LOCAL_PATH)

# shared libraries
shared_libs =
shared_libs += liblog

c_export_includes =
c_export_includes += $(LOCAL_PATH)

## Wno-unused-parameter required to ignore some warnings
## coming from google protobuf code
c_flags = -Werror -Wall -Wno-unused-parameter -fexceptions
ifeq ($(LLVM_SENSORS_SEE),true)
c_flags += --compile-and-analyze --analyzer-perf --analyzer-Werror
endif

LOCAL_MODULE := libsensorslog_system
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_CFLAGS   = $(c_flags)
LOCAL_CLANG := true
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_SRC_FILES = $(c_sources)
LOCAL_CPPFLAGS = $(cpp_flags)
LOCAL_C_INCLUDES = $(c_includes)
LOCAL_EXPORT_C_INCLUDE_DIRS := $(c_export_includes)
LOCAL_SHARED_LIBRARIES = $(shared_libs)
ifneq ($(TARGET_SUPPORTS_WEARABLES),true)
LOCAL_INSTALLED_MODULE_STEM := libsensorslog.so
endif
include $(BUILD_SHARED_LIBRARY)
