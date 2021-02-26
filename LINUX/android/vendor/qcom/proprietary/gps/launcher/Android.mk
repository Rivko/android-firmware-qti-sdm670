LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := loc_launcher
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti

LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libutils \
    libcutils \
    libgps.utils

LOCAL_STATIC_LIBRARIES := \
  libloc_base_util \
  libloc_mq_client

LOCAL_SRC_FILES += \
    process_launcher.cpp \
    mq_server/mq_server.cpp \
    mq_server/mq_client_info.cpp \
    mq_server/mq_server_launcher.cpp

LOCAL_CFLAGS += \
    -fno-short-enums \
    -D_ANDROID_

## Includes
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/mq_server
LOCAL_HEADER_LIBRARIES := \
    libgps.utils_headers \
    libloc_base_util_headers \
    libloc_mq_client_headers \
    libloc_pla_headers

LOCAL_CFLAGS += $(GNSS_CFLAGS)
include $(BUILD_EXECUTABLE)
