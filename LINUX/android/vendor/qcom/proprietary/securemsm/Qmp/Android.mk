ifeq ($(call is-board-platform-in-list,msm8974 msm8960 msm8660 msm8226 msm8610 msm8916 msm8916_32 msm8916_32_k64 msm8916_64 apq8084 msm8939 msm8994 msm8909 msm8996 msm8998 msm8992 msm8952 sdm660 sdm845 sdm710),true)

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

sourceFiles := \
               Seemplog.c

commonSharedLibraries := \
                        libc \
                        libcutils \
                        libutils \

LOCAL_MODULE:= libSeemplog
LOCAL_SHARED_LIBRARIES := $(commonSharedLibraries)
LOCAL_SRC_FILES := $(sourceFiles)
LOCAL_CFLAGS := -Werror

LOCAL_MODULE_TAGS       := optional
LOCAL_MODULE_OWNER := qti

include $(BUILD_SHARED_LIBRARY)

ifeq ($(call is-board-platform-in-list,sdm660 sdm845 sdm710),true)
include $(CLEAR_VARS)
LOCAL_MODULE := app_whitelist.cfg
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES := sphinx/app_whitelist.cfg
LOCAL_MODULE_TAGS := optional debug
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_ETC)/seemp/config/
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

endif # end filter
