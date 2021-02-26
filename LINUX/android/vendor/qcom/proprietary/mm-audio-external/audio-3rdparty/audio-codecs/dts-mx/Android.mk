LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := libomx-dts.so
LOCAL_MODULE := libomx-dts
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/lib
LOCAL_MODULE_TAGS := debug
LOCAL_MULTILIB := 32
include $(BUILD_PREBUILT)

# Any prebuilt files with default TAGS can use the below:
prebuilt_files :=

$(call add-prebuilt-files, SHARED_LIBRARIES, $(prebuilt_files))
