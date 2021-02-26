ifeq ($(call is-vendor-board-platform,QCOM),true)
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := \
        $(LOCAL_PATH)/../sscrpcd/inc \
        $(TARGET_OUT_HEADERS)/common/inc \
        $(TARGET_OUT_HEADERS)/diag/include \
        $(LOCAL_PATH)/../sensors-log \
        $(LOCAL_PATH)/../ssc \
        $(TARGET_OUT_INTERMEDIATES)/../gen/SHARED_LIBRARIES/libssc_intermediates/proto

LOCAL_SRC_FILES:= \
        src/sensordaemon.cpp \
        src/sns_reg_config.cpp

LOCAL_CFLAGS :=
LOCAL_SHARED_LIBRARIES := liblog libdiag libssc libprotobuf-cpp-full libsensorslog libcutils
LOCAL_CFLAGS += -Werror -Wall -fexceptions
ifeq ($(LLVM_SENSORS_SEE),true)
LOCAL_CFLAGS += --compile-and-analyze --analyzer-perf --analyzer-Werror
endif

LOCAL_MODULE := sensors.qti
LOCAL_MODULE_TAGS := optional debug
LOCAL_PROPRIETARY_MODULE := true
LOCAL_PRELINK_MODULE := false
LOCAL_UNINSTALLABLE_MODULE :=
LOCAL_MODULE_OWNER := qti

include $(BUILD_EXECUTABLE)

endif
