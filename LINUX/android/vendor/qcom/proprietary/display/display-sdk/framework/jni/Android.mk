# @file Android.mk
#

LOCAL_PATH:= $(call my-dir)
ifneq ($(TARGET_DISABLE_DISPLAY),true)

include $(CLEAR_VARS)
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:= libsd_sdk_display
LOCAL_MODULE_OWNER := qti

LOCAL_SRC_FILES:= displaysdk_jni.c

LOCAL_SHARED_LIBRARIES := liblog libutils libsdm-disp-apis

libmm-abl-inc  := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include

LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qdcm/inc/
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc/
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qcom/display/sdm/

LOCAL_CFLAGS += -Wno-multichar -Wno-unused-parameter -Werror
LOCAL_CLANG  := true

LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog

LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE:= false

include $(BUILD_SHARED_LIBRARY)
endif #TARGET_DISABLE_DISPLAY
