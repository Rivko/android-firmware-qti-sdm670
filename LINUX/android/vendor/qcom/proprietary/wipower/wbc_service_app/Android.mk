LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_PACKAGE_NAME := com.quicinc.wbcserviceapp
LOCAL_SRC_FILES := $(call all-java-files-under,src)
LOCAL_JAVA_LIBRARIES := com.quicinc.wbc
LOCAL_JAVA_LIBRARIES += com.quicinc.wbcservice
LOCAL_CERTIFICATE := platform
include $(BUILD_PACKAGE)
