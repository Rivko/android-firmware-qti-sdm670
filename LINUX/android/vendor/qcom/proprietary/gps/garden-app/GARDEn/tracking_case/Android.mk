LOCAL_PATH := $(call my-dir)

# Build libgardencaseulp
include $(CLEAR_VARS)

LOCAL_MODULE := libgardencasetracking
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES += \
    garden_app_session_tracker.cpp \
    TrackingCase.cpp \

LOCAL_CFLAGS += \
    -fno-short-enums

LOCAL_HEADER_LIBRARIES := \
    libgarden_headers \
    libgps.utils_headers \
    libloc_pla_headers \
    liblocation_api_headers \
    libevent_observer_headers \
    liblocationservice_headers \
    libgardencasetracking_headers

LOCAL_SHARED_LIBRARIES := \
    libgarden \
    libgps.utils \
    libloc_core \
    liblocation_api \
    liblog \
    liblocationservice \
    libulp2 \
    libevent_observer \

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

#build libgardencasetracking_headers
include $(CLEAR_VARS)
LOCAL_MODULE := libgardencasetracking_headers
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)
include $(BUILD_HEADER_LIBRARY)
