ifneq ($(BUILD_TINY_ANDROID),true)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

# ---------------------------------------------------------------------------------
# IPC Message Queue Client Library
# ---------------------------------------------------------------------------------

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE            := libloc_mq_client
LOCAL_PROPRIETARY_MODULE := true
LOCAL_CFLAGS              := $(COMMON_CFLAGS)
LOCAL_CFLAGS                += -fno-short-enums
LOCAL_CFLAGS                    += $(QC_LOC_FW_LOCAL_C_FLAGS)

LOCAL_PRELINK_MODULE        := false
LOCAL_SHARED_LIBRARIES        := libc libcutils
LOCAL_STATIC_LIBRARIES      :=   libloc_base_util

LOCAL_C_INCLUDES += \
  $(LOCAL_PATH) \
  $(LOCAL_PATH)/..
LOCAL_HEADER_LIBRARIES := \
  libgps.utils_headers \
  libloc_base_util_headers \
  libloc_pla_headers

LOCAL_SRC_FILES:= \
  mq_client.cpp \
  mq_client_controller.cpp \
  IPCMessagingProxy.cpp

LOCAL_MODULE_OWNER := qti

LOCAL_CFLAGS += $(GNSS_CFLAGS)
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libloc_mq_client_headers
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/../..
include $(BUILD_HEADER_LIBRARY)

endif # not BUILD_TINY_ANDROID
