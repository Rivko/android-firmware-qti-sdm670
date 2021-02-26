#/******************************************************************************
#*@file Android.mk
#* brief Rules to create an package which exposes the location api
#*******************************************************************************/
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE:= com.qti.location.sdk

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-java-files-under, java)

LOCAL_STATIC_JAVA_LIBRARIES := izat.lib.glue

LOCAL_PROGUARD_FLAG_FILES := proguard.flags

LOCAL_MODULE_OWNER := qti

LOCAL_DEX_PREOPT_APP_IMAGE := true

$(LOCAL_MODULE): com.qualcomm.location
	vendor/qcom/proprietary/gps/libs/addon/scripts/mksdkjar.sh out/host/common/izatsdk >/dev/null 2>&1

include $(BUILD_JAVA_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE:= com.qti.location.sdk.xml

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_CLASS := ETC

LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions

LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)

include $(call all-makefiles-under,$(LOCAL_PATH))
