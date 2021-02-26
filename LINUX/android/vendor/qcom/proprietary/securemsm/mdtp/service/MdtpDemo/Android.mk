
LOCAL_PATH := $(call my-dir)/src/main
include $(CLEAR_VARS)

MDTP_API_PATH := ../../../Mdtp/src

LOCAL_PACKAGE_NAME := MdtpDemo

# Build all java files in the java subdirectory
LOCAL_SRC_FILES := $(call all-java-files-under, $(MDTP_API_PATH)/java) $(call all-java-files-under, java) $(call all-Iaidl-files-under, $(MDTP_API_PATH)/aidl)

LOCAL_AIDL_INCLUDES := $(LOCAL_PATH)/$(MDTP_API_PATH)/aidl_import

LOCAL_PROGUARD_ENABLED := disabled

LOCAL_CERTIFICATE := platform

# Name of the APK to build
LOCAL_PACKAGE_NAME := MdtpDemo

# Set a permitted MDTP package name
MDTP_PACKAGE_NAME := com.qualcomm.qti.securemsm.mdtp.MdtpDemo
ADDITIONAL_BUILD_PROPERTIES += ro.mdtp.package_name2=$(MDTP_PACKAGE_NAME)

LOCAL_STATIC_JAVA_LIBRARIES := \
    android-support-v4 \
    services.devicepolicy \

# Tell it to build an APK
include $(BUILD_PACKAGE)
