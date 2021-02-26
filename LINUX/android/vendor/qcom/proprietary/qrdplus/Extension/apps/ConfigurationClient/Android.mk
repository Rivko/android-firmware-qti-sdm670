LOCAL_PATH:= $(call my-dir)

# ---------------------------------------
# Build ConfigurationClient application
#
# Output: ConfigurationClient.apk
# ----------------------------------------

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_RESOURCE_DIR = \
        $(LOCAL_PATH)/res \

LOCAL_PACKAGE_NAME := ConfigurationClient

LOCAL_STATIC_JAVA_LIBRARIES := \
        android-support-v4

LOCAL_MODULE_TAGS := optional

LOCAL_PRIVILEGED_MODULE := true

include $(BUILD_PACKAGE)

include $(call all-makefiles-under,$(LOCAL_PATH))
