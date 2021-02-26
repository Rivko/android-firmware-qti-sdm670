LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
    MmiDiagJNIInterface.cpp

LOCAL_C_INCLUDES += \
    $(JNI_H_INCLUDE) \
    $(TARGET_OUT_HEADERS)/common/inc \
    $(TARGET_OUT_HEADERS)/diag/include

LOCAL_SHARED_LIBRARIES := \
    libnativehelper \
    libutils \
    libcutils \
    libdiag \
    liblog

LOCAL_MODULE:= libmmi_jni
LOCAL_MODULE_TAGS := debug

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)
