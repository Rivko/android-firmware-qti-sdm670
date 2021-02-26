LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

# Only compile source java files in this apk.
LOCAL_SRC_FILES := $(call all-java-files-under, src)
LOCAL_SRC_FILES += src/com/qti/storagecleaner/ICleanerCallBack.aidl \
                   src/com/qti/storagecleaner/IStorageCleanerService.aidl

LOCAL_PACKAGE_NAME := StorageCleaner
LOCAL_PROPRIETARY_MODULE := true

#LOCAL_SDK_VERSION := current
#LOCAL_PROGUARD_FLAG_FILES := proguard.flags

LOCAL_CERTIFICATE := platform

include $(BUILD_PACKAGE)

# Use the following include to make our test apk.
include $(call all-makefiles-under,$(LOCAL_PATH))
