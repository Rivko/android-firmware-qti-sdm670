
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(call all-subdir-java-files)

LOCAL_PACKAGE_NAME := UnifiedSensorTestApp
LOCAL_CERTIFICATE := platform
LOCAL_MODULE_TAGS := debug optional tests
LOCAL_SRC_FILES += $(call all-java-files-under, java)

ifeq ($(PLATFORM_VERSION), P)
LOCAL_AAPT2_ONLY := true
endif

LOCAL_RESOURCE_DIR = \
        $(LOCAL_PATH)/res \
        frameworks/support/v7/appcompat/res \
        frameworks/support/v7/recyclerview/res \
        frameworks/support/design/res

ifeq ($(PLATFORM_VERSION), P)
LOCAL_RESOURCE_DIR += frameworks/support/core-ui/res
endif

LOCAL_AAPT_FLAGS := \
        --auto-add-overlay \
        --extra-packages android.support.design \
        --extra-packages android.support.v7.appcompat \
        --extra-packages android.support.v7.recyclerview \
        --no-version-vectors
LOCAL_STATIC_JAVA_LIBRARIES := \
        android-support-v4 \
        android-support-v7-appcompat \
        android-support-v7-recyclerview \
        android-support-design


LOCAL_PROGUARD_ENABLED := disabled
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti
include $(BUILD_PACKAGE)

include $(call all-makefiles-under,$(LOCAL_PATH))

