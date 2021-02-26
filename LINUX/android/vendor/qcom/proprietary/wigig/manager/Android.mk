LOCAL_PATH:= $(call my-dir)

#=============================================
#  Wigig Manager
#=============================================
include $(CLEAR_VARS)

LOCAL_MODULE := wigig_manager
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-java-files-under, java) \
                   $(call all-Iaidl-files-under, java)

LOCAL_AIDL_INCLUDES += $(LOCAL_PATH)/java
LOCAL_PROGUARD_ENABLED := disabled

include $(BUILD_STATIC_JAVA_LIBRARY)

# Include this library in the build server's output directory
$(call dist-for-goals, dist_files, $(LOCAL_BUILT_MODULE):wigig_manager.jar)
