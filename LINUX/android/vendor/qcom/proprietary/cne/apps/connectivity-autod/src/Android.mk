LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:=  main.cpp
LOCAL_C_INCLUDES += $(QC_PROP_ROOT)/data/dsi_netctrl/inc \
                    $(TARGET_OUT_HEADERS)/qmi/inc \
                    $(TARGET_OUT_HEADERS)/cne/common/inc
LOCAL_MODULE:= autod
LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES :=  libcutils \
                           liblog \
                           libdsi_netctrl \
                           libcne
LOCAL_CLANG := true
include $(BUILD_EXECUTABLE)
