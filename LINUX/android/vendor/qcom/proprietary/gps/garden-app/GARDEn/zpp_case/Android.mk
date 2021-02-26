LOCAL_PATH := $(call my-dir)

# Build libgardencasezpp
include $(CLEAR_VARS)

LOCAL_MODULE := libgardencasezpp
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES += \
    ZppCase.cpp \

LOCAL_CFLAGS += \
    -fno-short-enums

LOCAL_HEADER_LIBRARIES := \
    libgarden_headers \
    libgardencaseulp_headers \
    libgps.utils_headers \
    libloc_pla_headers \
    liblocation_api_headers \
    liblocationservice_headers \

LOCAL_SHARED_LIBRARIES := \
    libgarden \
    libgps.utils \
    libloc_core \
    liblocation_api \
    liblog \
    liblocationservice \
    libulp2 \
    libgardencaseulp \

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := libgardencasezpp_headers
LOCAL_EXPORT_C_INCLUDE_DIRS := \
    $(LOCAL_PATH) \
include $(BUILD_HEADER_LIBRARY)
