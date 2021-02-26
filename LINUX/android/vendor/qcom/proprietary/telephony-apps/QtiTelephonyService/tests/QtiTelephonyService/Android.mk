LOCAL_PATH:= $(call my-dir)
#
# QtiTelephonyTests apk
#
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := debug

LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_JAVA_LIBRARIES := android.test.runner qcrilhook
LOCAL_STATIC_JAVA_LIBRARIES := mockito-target
LOCAL_PACKAGE_NAME := QtiTelephonyServiceTests

LOCAL_INSTRUMENTATION_FOR := QtiTelephonyService

LOCAL_PROGUARD_ENABLED := disabled

LOCAL_CERTIFICATE := platform

include $(BUILD_PACKAGE)
