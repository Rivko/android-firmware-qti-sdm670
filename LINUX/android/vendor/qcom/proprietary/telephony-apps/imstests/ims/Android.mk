ifneq ($(TARGET_NO_TELEPHONY), true)
ifeq ("disable_build_true","") #(Disable this project)
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := tests

LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_JAVA_LIBRARIES := android.test.runner telephony-common

LOCAL_PACKAGE_NAME := imstests

LOCAL_INSTRUMENTATION_FOR := ims

LOCAL_PROGUARD_ENABLED := disabled

LOCAL_CERTIFICATE := platform

LOCAL_MODULE_OWNER := qcom

include $(BUILD_PACKAGE)
endif
endif # TARGET_NO_TELEPHONY
