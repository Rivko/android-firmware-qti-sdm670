
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

av := frameworks/av

# Source files of DRM1 Java Native Interfaces
LOCAL_SRC_FILES :=      \
    src/com_oma_drm_util_OmaDrmUtils.cpp \
    src/OmaDrmInputStream.cpp \

LOCAL_STATIC_LIBRARIES := \

LOCAL_SHARED_LIBRARIES := \
    libdrmframework \
    liblog \
    libutils \
    libandroid_runtime \
    libnativehelper \
    libbinder \
    libdl \


LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/include \
    $(LOCAL_PATH)/../common \
    $(LOCAL_PATH)/../internal/rightsmanager \
    $(av)/include \
    $(av)/include/drm \
    $(av)/drm/libdrmframework/plugins/common/util/include \

LOCAL_MODULE := libomadrmutils_jni

include $(BUILD_SHARED_LIBRARY)
