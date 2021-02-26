ifeq ($(strip $(TARGET_HAS_LOW_RAM)),true)
    TARGET_USES_QTI_CSM := false
endif
ifneq ($(strip $(TARGET_USES_QTI_CSM)),false)
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(call all-subdir-java-files)

LOCAL_PACKAGE_NAME := Csm
LOCAL_CERTIFICATE := platform
LOCAL_JAVA_LIBRARIES += telephony-common
LOCAL_MODULE_TAGS := debug optional
LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_RESOURCE_DIR = \
        $(LOCAL_PATH)/res \
        frameworks/support/v7/appcompat/res \
        frameworks/support/v7/recyclerview/res \
        frameworks/support/design/res
LOCAL_AAPT_FLAGS := \
        --auto-add-overlay \
        --extra-packages android.support.v7.appcompat \
        --extra-packages android.support.v7.recyclerview \
        --extra-packages android.support.design \
        --no-version-vectors
LOCAL_STATIC_JAVA_LIBRARIES := \
        android-support-v4 \
        android-support-v7-appcompat \
        android-support-v7-recyclerview \
        android-support-design \
        com.android.vcard

LOCAL_PROGUARD_ENABLED := disabled
include $(BUILD_PACKAGE)
include $(call all-makefiles-under,$(LOCAL_PATH))
endif
