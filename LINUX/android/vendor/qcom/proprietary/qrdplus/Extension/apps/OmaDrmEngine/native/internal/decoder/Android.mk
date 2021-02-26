
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    OmaDrmFile.c \
    OmaDrmDcfFile.c

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/../common \
    $(LOCAL_PATH)/../rightsmanager \
    external/openssl/include

LOCAL_SHARED_LIBRARIES := libcrypto

LOCAL_MODULE := libomadrm-decoder

LOCAL_MODULE_TAGS := optional

include $(BUILD_STATIC_LIBRARY)
