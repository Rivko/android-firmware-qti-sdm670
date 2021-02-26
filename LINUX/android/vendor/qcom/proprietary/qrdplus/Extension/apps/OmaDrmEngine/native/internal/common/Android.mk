
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    OmaDrmGlue.c

LOCAL_C_INCLUDES := \
    external/openssl/include

LOCAL_SHARED_LIBRARIES := libcrypto

LOCAL_MODULE := libomadrm-common

LOCAL_MODULE_TAGS := optional

include $(BUILD_STATIC_LIBRARY)
