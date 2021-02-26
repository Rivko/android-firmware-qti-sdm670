LOCAL_PATH := $(my-dir)

########################
include $(CLEAR_VARS)

LOCAL_MODULE := qti_permissions.xml

LOCAL_MODULE_CLASS := ETC

# This will install the file in /system/etc/permissions
#
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions

ifeq ($(TARGET_USES_QCOM_BSP_ATEL), true)
    LOCAL_SRC_FILES := qti_permissions.xml
else
    LOCAL_SRC_FILES := qti_permissions_2.xml
endif

LOCAL_MODULE_CLASS := ETC

LOCAL_MODULE_TAGS := optional

include $(BUILD_PREBUILT)
