
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

# The flag below turns on local debug printouts
#LOCAL_CFLAGS += -DRM_OMA_DRM_ENGINE_DEBUG

base := frameworks/av

# Determine whether the DRM framework uses 64-bit data types for file offsets and do the same.
ifneq ($(shell grep -c 'off64_t offset' $(base)/drm/libdrmframework/plugins/common/include/IDrmEngine.h), 0)
LOCAL_CFLAGS += -DUSE_64BIT_DRM_API
endif

LOCAL_SRC_FILES:= \
    src/OmaDrmEngine.cpp \
    src/OmaDrmMimeTypeUtil.cpp

LOCAL_SHARED_LIBRARIES := \
    libicui18n \
    libicuuc \
    libutils \
    liblog \
    libdl \
    libcrypto \
    libssl \
    libdrmframework \
    libxml2 \

LOCAL_STATIC_LIBRARIES := \
    libdrmutility \
    libdrmframeworkcommon \
    libomadrm-common \
    libomadrm-converter \
    libomadrm-decoder \
    libomadrm-rightsmanager \



LOCAL_C_INCLUDES += \
    $(base)/include/drm \
    $(base)/drm/libdrmframework/plugins/common/include \
    $(base)/drm/libdrmframework/plugins/common/util/include \
    $(LOCAL_PATH)/../common \
    $(LOCAL_PATH)/../internal/common \
    $(LOCAL_PATH)/../internal/converter \
    $(LOCAL_PATH)/../internal/decoder \
    $(LOCAL_PATH)/../internal/rightsmanager \
    $(LOCAL_PATH)/include \
    external/openssl/include \
    external/libxml2/include \

LOCAL_MODULE := libomadrmengine
LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_RELATIVE_PATH := drm
LOCAL_MODULE_OWNER := qcom
LOCAL_PROPRIETARY_MODULE := true
LOCAL_32_BIT_ONLY := true

include $(BUILD_SHARED_LIBRARY)
