ifeq ($(call is-board-platform-in-list,sdm845 sdm710 qcs605),true)
TARGET_SUPPORT_DIRECT_REPORT :=true
endif

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

c_sources = $(call all-cpp-files-under, .)

#$(info sources = $(c_sources))

# Include paths
c_includes =
c_includes += $(LOCAL_PATH)/framework
c_includes += hardware/libhardware/include
c_includes += hardware/libhardware/include/hardware
ifeq ($(TARGET_SUPPORT_DIRECT_REPORT), true)
    c_includes += $(LOCAL_PATH)/../sns_low_lat/inc
endif
# TODO: try to use automatic export_includes from libssc.so, to remove following
# dependencies
c_includes += $(TARGET_OUT_INTERMEDIATES)/../gen/SHARED_LIBRARIES/libssc_intermediates/proto
c_includes += $(LOCAL_PATH)/../ssc
c_includes += $(LOCAL_PATH)/../sensors-log
c_includes += $(LOCAL_PATH)/../sensors-diag-log/inc

# shared libraries to link
shared_libs =
shared_libs += libssc
shared_libs += liblog
shared_libs += libcutils
shared_libs += libsensorslog
shared_libs += libprotobuf-cpp-full
shared_libs += libutils
shared_libs += libsnsdiaglog
shared_libs += libhardware
shared_libs += libdl
ifeq ($(TARGET_SUPPORT_DIRECT_REPORT), true)
    shared_libs += libsns_low_lat_stream_stub
    ifeq ($(call is-board-platform-in-list, sdm710 qcs605),true)
     shared_libs += libadsprpc
    endif
endif

c_flags = -Werror -Wall -fexceptions
ifeq ($(LLVM_SENSORS_SEE),true)
c_flags += --compile-and-analyze --analyzer-perf --analyzer-Werror
endif

c_flags += -Wno-unused-variable -Wno-unused-parameter
$(info $(c_flags))
ifeq ($(TARGET_SUPPORTS_WEARABLES),true)
c_flags +=-DSNS_WEARABLES_TARGET
endif

ifeq ($(USE_SENSOR_MULTI_HAL),true)
    LOCAL_MODULE := sensors.ssc
else
    LOCAL_MODULE := sensors.$(TARGET_BOARD_PLATFORM)
    LOCAL_MODULE_RELATIVE_PATH := hw
endif

LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_CLANG := true
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_SRC_FILES = $(c_sources)
LOCAL_CFLAGS   := $(c_flags)
ifeq ($(TARGET_SUPPORT_DIRECT_REPORT), true)
    SRC_C_LIST :=$(LOCAL_PATH)/../sns_low_lat/src/rpcmem_android.c
    LOCAL_SRC_FILES += $(SRC_C_LIST:$(LOCAL_PATH)/%=%)
    LOCAL_CFLAGS += -DSNS_DIRECT_REPORT_SUPPORT
    ifeq ($(call is-board-platform-in-list, sdm710 qcs605 ),true)
        LOCAL_CFLAGS += -DADSPRPC
    endif
else
    LOCAL_SRC_FILES :=$(filter-out framework/direct_channel.cpp, $(LOCAL_SRC_FILES))
endif
LOCAL_C_INCLUDES := $(c_includes)
LOCAL_EXPORT_C_INCLUDE_DIRS := $(c_export_includes)
LOCAL_SHARED_LIBRARIES := $(shared_libs)

include $(BUILD_SHARED_LIBRARY)
