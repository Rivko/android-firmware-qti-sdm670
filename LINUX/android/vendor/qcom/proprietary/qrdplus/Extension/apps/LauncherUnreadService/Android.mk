LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

# Only compile source java files in this apk.
LOCAL_SRC_FILES := $(call all-java-files-under, src)
LOCAL_SRC_FILES += src/com/qti/launcherunreadservice/IGetUnreadNumber.aidl \
                   src/com/qti/launcherunreadservice/IUnreadNumberCallback.aidl
LOCAL_PACKAGE_NAME := LauncherUnreadService

LOCAL_SDK_VERSION := current
#LOCAL_JAVA_LIBRARIES := telephony-common mms-common
LOCAL_JAVA_LIBRARIES := telephony-common
LOCAL_PROGUARD_ENABLED := disabled
#LOCAL_PROGUARD_FLAG_FILES := proguard.flags

LOCAL_CERTIFICATE := platform

include $(BUILD_PACKAGE)

# Use the following include to make our test apk.
include $(call all-makefiles-under,$(LOCAL_PATH))

