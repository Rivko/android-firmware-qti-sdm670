LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(call all-subdir-java-files)

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE:= com.qualcomm.qti.camera

# This will install the file in /system/framework
# This line is NOT needed and will cause location binding which may impact Android O
# LOCAL_MODULE_PATH := $(TARGET_OUT_JAVA_LIBRARIES)

include $(BUILD_JAVA_LIBRARY)

