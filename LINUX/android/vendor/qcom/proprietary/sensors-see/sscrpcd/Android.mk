ifeq ($(call is-vendor-board-platform,QCOM),true)
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := \
        $(LOCAL_PATH)/inc \
        $(TARGET_OUT_HEADERS)/common/inc \
        $(LOCAL_PATH)/../sensors-log \
        $(LOCAL_PATH)/../ssc \
        $(TARGET_OUT_INTERMEDIATES)/../gen/SHARED_LIBRARIES/libssc_intermediates/proto

LOCAL_SRC_FILES:= \
        src/sscrpcd.cpp \

LOCAL_CFLAGS := -DADSP_DEFAULT_LISTENER_NAME=\"libssc_default_listener.so\"
LOCAL_SHARED_LIBRARIES := liblog libdl libssc libprotobuf-cpp-full libsensorslog
LOCAL_CFLAGS += -Werror -Wall -fexceptions
ifeq ($(LLVM_SENSORS_SEE),true)
LOCAL_CFLAGS += --compile-and-analyze --analyzer-perf --analyzer-Werror
endif

LOCAL_MODULE := sscrpcd
LOCAL_MODULE_TAGS := optional debug
LOCAL_PROPRIETARY_MODULE := true
LOCAL_PRELINK_MODULE := false
LOCAL_UNINSTALLABLE_MODULE :=
LOCAL_MODULE_OWNER := qti

LOCAL_COPY_HEADERS_TO := sscrpc/inc
LOCAL_COPY_HEADERS := ../inc/remote.h
include $(BUILD_EXECUTABLE)

endif
