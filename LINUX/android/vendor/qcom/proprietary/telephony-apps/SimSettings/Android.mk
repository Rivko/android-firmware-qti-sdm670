ifneq ($(TARGET_NO_TELEPHONY), true)

ifeq ($(TARGET_USES_QCOM_BSP_ATEL), true)
 LOCAL_PATH:= $(call my-dir)
 include $(CLEAR_VARS)
 LOCAL_MODULE_TAGS := optional eng

 LOCAL_SRC_FILES := $(call all-subdir-java-files)

 LOCAL_JAVA_LIBRARIES := telephony-common telephony-ext
 LOCAL_STATIC_JAVA_LIBRARIES := android-support-v4

 LOCAL_PACKAGE_NAME :=SimSettings

 LOCAL_CERTIFICATE := platform

 include $(BUILD_PACKAGE)
endif

endif # TARGET_NO_TELEPHONY
