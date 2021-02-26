ifneq ($(BUILD_TINY_ANDROID),true)

LOCAL_PATH := $(call my-dir)

# ---------------------------------------------------------------------------------
# Base Utility Library
# ---------------------------------------------------------------------------------
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE := libloc_base_util
LOCAL_PROPRIETARY_MODULE := true
LOCAL_CFLAGS := $(COMMON_CFLAGS)
LOCAL_CFLAGS += -fno-short-enums
ifeq (, $(filter aarch64 arm64, $(TARGET_ARCH)))
LOCAL_CFLAGS += -DANDROID_32_BIT_PTR_SUPPORT
endif

LOCAL_C_INCLUDES += \
  $(LOCAL_PATH) \
  $(LOCAL_PATH)/../.. \
  external/boringssl/src/include \
  external/sqlite/dist

LOCAL_SRC_FILES:= \
  config_file.cpp \
  log.cpp \
  memorystream.cpp \
  nvparam_mgr.cpp \
  postcard.cpp \
  sync.cpp \
  string_routines.cpp \
  time_routines.cpp \
  fault_tolerant_file.cpp

LOCAL_CFLAGS += $(QC_LOC_FW_LOCAL_C_FLAGS) \
    $(GNSS_CFLAGS)

LOCAL_MODULE_OWNER := qti

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libloc_base_util_headers
LOCAL_EXPORT_C_INCLUDE_DIRS := \
    $(LOCAL_PATH)/.. \
    $(LOCAL_PATH)/../..
include $(BUILD_HEADER_LIBRARY)

endif # not BUILD_TINY_ANDROID
