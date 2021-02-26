LOCAL_PATH := $(call my-dir)

ifneq ($(TARGET_SUPPORTS_WEARABLES),true)
# Include the prebuilt sns_low_lat_stream_skel shared library
include $(CLEAR_VARS)
LOCAL_SRC_FILES     := prebuilt/lib/q6/libsns_low_lat_stream_skel.so
LOCAL_MODULE        := libsns_low_lat_stream_skel
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS  := ETC
LOCAL_MODULE_TAGS   := optional debug
LOCAL_MODULE_OWNER  := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH   := $(TARGET_OUT_VENDOR)/lib/rfsa/adsp
include $(BUILD_PREBUILT)


include $(CLEAR_VARS)
LOCAL_SRC_FILES     := prebuilt/lib/q6/libsns_low_lat_stream_skel.so
LOCAL_MODULE        := libsns_low_lat_stream_skel_system
LOCAL_MODULE_CLASS  := ETC
LOCAL_MODULE_TAGS   := optional debug
LOCAL_MODULE_PATH   := $(TARGET_OUT)/lib/rfsa/adsp
LOCAL_INSTALLED_MODULE_STEM := libsns_low_lat_stream_skel.so
include $(BUILD_PREBUILT)


# Compile the sns_low_lat_stream_stub shared library
include $(CLEAR_VARS)
LOCAL_SRC_FILES := src/sns_low_lat_stream_stub.c
LOCAL_MODULE    := libsns_low_lat_stream_stub
LOCAL_C_INCLUDES := $(LOCAL_PATH)/inc
LOCAL_CFLAGS := -Werror -Wall -Wno-unused-parameter -fexceptions
LOCAL_SHARED_LIBRARIES += libdl liblog libc

#slpi based
ifeq ($(call is-board-platform-in-list,sdm845),true)
LOCAL_SHARED_LIBRARIES += libsdsprpc
else
#adsp based
LOCAL_SHARED_LIBRARIES += libadsprpc
endif

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER  := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_SRC_FILES := src/sns_low_lat_stream_stub.c
LOCAL_MODULE    := libsns_low_lat_stream_stub_system
LOCAL_C_INCLUDES := $(LOCAL_PATH)/inc
LOCAL_CFLAGS := -Werror -Wall -Wno-unused-parameter -fexceptions
LOCAL_SHARED_LIBRARIES += libdl liblog libc libprotobuf-cpp-full
#slpi base
ifeq ($(call is-board-platform-in-list,sdm845),true)
LOCAL_SHARED_LIBRARIES += libsdsprpc_system
else
#adsp based
LOCAL_SHARED_LIBRARIES += libadsprpc_system
endif

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
LOCAL_INSTALLED_MODULE_STEM := libsns_low_lat_stream_stub.so
include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)
commonSources  :=
commonIncludes := $(TARGET_OUT_HEADERS)/common/inc
commonIncludes += $(TARGET_OUT_HEADERS)/qmi/inc
commonIncludes += $(TARGET_OUT_HEADERS)/sensors/inc
commonIncludes += $(TARGET_OUT_HEADERS)/diag/include
commonIncludes += $(TARGET_OUT_HEADERS)/qmi-framework/inc
commonIncludes += $(TARGET_OUT_INTERMEDIATES)/../gen/SHARED_LIBRARIES/libssc_intermediates/proto

LOCAL_C_INCLUDES := $(commonIncludes)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../adsprpc/inc/
LOCAL_C_INCLUDES += $(LOCAL_PATH)

LOCAL_MODULE    := libsensor_low_lat
LOCAL_SRC_FILES += jni/src/sensor_low_lat.cpp
LOCAL_SRC_FILES += src/rpcmem_android.c

LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libdiag
LOCAL_SHARED_LIBRARIES += libssc
#slpi base
ifeq ($(call is-board-platform-in-list,sdm845),true)
LOCAL_SHARED_LIBRARIES += libsdsprpc
else
#adsp based
LOCAL_CFLAGS += -DADSPRPC
LOCAL_SHARED_LIBRARIES += libadsprpc
endif
LOCAL_SHARED_LIBRARIES += libprotobuf-cpp-full
LOCAL_SHARED_LIBRARIES += libdl libc
LOCAL_SHARED_LIBRARIES += libsns_low_lat_stream_stub

LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog
LOCAL_PRELINK_MODULE:=false

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)
commonSources  :=
commonIncludes := $(TARGET_OUT_HEADERS)/common/inc
commonIncludes += $(TARGET_OUT_HEADERS)/qmi/inc
commonIncludes += $(TARGET_OUT_HEADERS)/sensors/inc
commonIncludes += $(TARGET_OUT_HEADERS)/diag/include
commonIncludes += $(TARGET_OUT_HEADERS)/qmi-framework/inc
commonIncludes += $(TARGET_OUT_INTERMEDIATES)/../gen/SHARED_LIBRARIES/libssc_intermediates/proto

LOCAL_C_INCLUDES := $(commonIncludes)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../adsprpc/inc/

LOCAL_MODULE    := libsensor_low_lat_system
LOCAL_SRC_FILES += jni/src/sensor_low_lat.cpp
LOCAL_SRC_FILES += src/rpcmem_android.c

LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libdiag_system
LOCAL_SHARED_LIBRARIES += libprotobuf-cpp-full
#slpi base
ifeq ($(call is-board-platform-in-list,sdm845),true)
LOCAL_SHARED_LIBRARIES += libsdsprpc_system
else
#adsp based
LOCAL_CFLAGS += -DADSPRPC
LOCAL_SHARED_LIBRARIES += libadsprpc_system
endif
LOCAL_SHARED_LIBRARIES += libdl libc
LOCAL_SHARED_LIBRARIES += libssc_system
LOCAL_SHARED_LIBRARIES += libsns_low_lat_stream_stub_system

LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog
LOCAL_PRELINK_MODULE:=false
LOCAL_INSTALLED_MODULE_STEM := libsensor_low_lat.so

include $(BUILD_SHARED_LIBRARY)
endif
