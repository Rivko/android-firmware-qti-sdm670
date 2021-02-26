#/******************************************************************************
#*@file Android.mk
#* brief Rules for compiling native code associated with libulp library
#*  ******************************************************************************/
ifneq ($(BUILD_TINY_ANDROID),true)

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

# This is the target being built.
LOCAL_MODULE:= libevent_observer

# All of the source files that we will compile.
LOCAL_SRC_FILES:= \
    EventObserver.cpp

# All of the shared libraries we link against.
LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    libgps.utils \
    liblog

LOCAL_HEADER_LIBRARIES := \
    libloc_core_headers \
    libgps.utils_headers \
    liblocationservice_headers \
    libloc_pla_headers \
    liblocation_api_headers

LOCAL_CFLAGS+=$(GPS_FEATURES) \
    -D_ANDROID_ \

LOCAL_LDFLAGS += -Wl,--export-dynamic
LOCAL_MODULE_TAGS := optional

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

LOCAL_CFLAGS += $(GNSS_CFLAGS)
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libevent_observer_headers
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)
include $(BUILD_HEADER_LIBRARY)

endif # not BUILD_TINY_ANDROID



