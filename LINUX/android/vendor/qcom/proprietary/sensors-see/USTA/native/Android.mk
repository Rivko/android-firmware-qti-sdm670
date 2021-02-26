LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libUSTANative
LOCAL_MODULE_CLASS := SHARED_LIBRARIES

c_sources =
c_sources += sensor_cxt.cpp
c_sources += sensor_suid.cpp
c_sources += sensor.cpp
c_sources += sensor_descriptor_pool.cpp
c_sources += sensor_message_parser.cpp
c_sources += sensor_attributes.cpp
c_sources += logging_util.cpp
c_sources += com_qualcomm_qti_usta_core_SensorContextJNI.cpp

c_includes =
c_includes += $(LOCAL_PATH)
c_includes += $(TARGET_OUT_INTERMEDIATES)/../gen/SHARED_LIBRARIES/libssc_intermediates/proto
c_includes += $(LOCAL_PATH)/../../ssc
c_includes += $(LOCAL_PATH)/../../sensors-diag-log/inc

shared_libs =
shared_libs += libssc
shared_libs += liblog
shared_libs += libprotobuf-cpp-full
shared_libs += libsnsdiaglog

$(shell mkdir -p $(TARGET_OUT_ETC)/sensors/proto)

$(shell cp -f external/protobuf/src/google/protobuf/descriptor.proto $(TARGET_OUT_ETC)/sensors)
$(shell cp -f $(LOCAL_PATH)/../../ssc/proto/*.proto $(TARGET_OUT_ETC)/sensors)
$(shell mv -f $(TARGET_OUT_ETC)/sensors/*.proto $(TARGET_OUT_ETC)/sensors/proto)

c_flags = -Werror -Wno-unused-parameter -fexceptions
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

LOCAL_MODULE := libUSTANative_system
LOCAL_MODULE_CLASS := SHARED_LIBRARIES

c_sources =
c_sources += sensor_cxt.cpp
c_sources += sensor_suid.cpp
c_sources += sensor.cpp
c_sources += sensor_descriptor_pool.cpp
c_sources += sensor_message_parser.cpp
c_sources += sensor_attributes.cpp
c_sources += logging_util.cpp
c_sources += com_qualcomm_qti_usta_core_SensorContextJNI.cpp

c_includes =
c_includes += $(LOCAL_PATH)
c_includes += $(TARGET_OUT_INTERMEDIATES)/../gen/SHARED_LIBRARIES/libssc_system_intermediates/proto
c_includes += $(LOCAL_PATH)/../../ssc
c_includes += $(LOCAL_PATH)/../../sensors-diag-log/inc

shared_libs =
shared_libs += libssc_system
shared_libs += liblog
shared_libs += libprotobuf-cpp-full
shared_libs += libsnsdiaglog_system

LOCAL_INSTALLED_MODULE_STEM := libUSTANative.so


c_flags = -Werror -Wno-unused-parameter -fexceptions
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
