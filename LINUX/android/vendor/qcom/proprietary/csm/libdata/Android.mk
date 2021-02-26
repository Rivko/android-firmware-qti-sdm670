LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_ARM_MODE := arm
LOCAL_SRC_FILES:= DataCtl.cpp \
                  NetdDataCtl.cpp

LOCAL_CLANG := true
LOCAL_MODULE:= libcsm_data
LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES := libcutils \
                          liblog \
                          libsysutils \
                          libutils \
                          liblogwrap

LOCAL_C_INCLUDES += system/netd/server

LOCAL_CPPFLAGS += -Wall -Werror
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)
