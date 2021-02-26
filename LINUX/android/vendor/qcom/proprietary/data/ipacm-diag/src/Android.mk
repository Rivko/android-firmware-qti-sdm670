ifneq (,$(filter $(QCOM_BOARD_PLATFORMS),$(TARGET_BOARD_PLATFORM)))
ifneq (, $(filter arm64 arm, $(TARGET_ARCH)))

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../dsutils/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../common/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../diag/include

LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr


LOCAL_CFLAGS := -v
LOCAL_CFLAGS += -DDEBUG
LOCAL_CFLAGS += -DFEATURE_DATA_LOG_QXDM
LOCAL_CFLAGS += -DFEATURE_IPA_ANDROID

ifeq ($(TARGET_ARCH),arm)
LOCAL_CFLAGS += -include bionic/libc/kernel/uapi/asm-arm/asm/posix_types.h
LOCAL_CFLAGS += -include bionic/libc/kernel/uapi/asm-arm/asm/byteorder.h
endif

LOCAL_SRC_FILES := ipacmdiag_main.c

LOCAL_MODULE := ipacm-diag
LOCAL_CLANG := true
LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES := libdsutils
LOCAL_SHARED_LIBRARIES += libdiag
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_EXECUTABLES)

include $(BUILD_EXECUTABLE)

endif # $(TARGET_ARCH)
endif
