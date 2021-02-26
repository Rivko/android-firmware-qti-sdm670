
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    OmaDrmRightsManager.c

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/../common \
    external/libxml2/include \
    external/icu/icu4c/source/common \
    external/openssl/include \

LOCAL_SHARED_LIBRARIES := \
    libxml2 \
    libicuuc \
    libcrypto \

LOCAL_MODULE := libomadrm-rightsmanager

LOCAL_MODULE_TAGS := optional

include $(BUILD_STATIC_LIBRARY)
