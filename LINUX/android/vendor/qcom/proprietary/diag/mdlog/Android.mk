LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

libdiag_includes := \
        $(LOCAL_PATH)/../include \
	$(LOCAL_PATH)/../src

LOCAL_C_INCLUDES := $(libdiag_includes)
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc

LOCAL_SRC_FILES := \
	diag_mdlog.c

commonSharedLibraries :=libdiag liblog

LOCAL_MODULE := diag_mdlog
LOCAL_MODULE_TAGS := optional debug
LOCAL_SHARED_LIBRARIES := $(commonSharedLibraries)

ifdef TARGET_FS_CONFIG_GEN
LOCAL_SHARED_LIBRARIES += liboemaids_vendor
LOCAL_CFLAGS += -DTARGET_FS_CONFIG_GEN
endif

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)
