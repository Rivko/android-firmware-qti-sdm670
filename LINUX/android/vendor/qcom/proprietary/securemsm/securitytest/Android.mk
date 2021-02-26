ifeq ($(call is-board-platform-in-list,msm8974 msm8960 msm8660 msm8226 msm8610 msm8916 msm8916_32 msm8916_32_k64 msm8916_64 apq8084 msm8939 msm8994 msm8909 msm8996 msm8992 msm8952),true)

ifneq ($(TARGET_USES_AOSP),true)

LOCAL_PATH:= $(call my-dir)
ifeq ($(call is-board-platform,msm8960),true)
	QSEECOM_CFLAGS := -DQSEECOM_TARGET_MSM8960
endif

ifeq ($(call is-board-platform,msm8974),true)
	QSEECOM_CFLAGS := -DQSEECOM_TARGET_MSM8974
endif

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
                    $(LOCAL_PATH)/../QSEEComAPI \
                    $(TARGET_OUT_HEADERS)/common/inc
LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_SHARED_LIBRARIES := \
        libc \
        libcutils \
        libutils \
        libdl \
        libQSEEComAPI \
        libdrmfs \


LOCAL_MODULE := qseecom_security_test
LOCAL_SRC_FILES := qseecom_security_test.c
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS := $(QSEECOM_CFLAGS)
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qcom
LOCAL_32_BIT_ONLY := true
include $(BUILD_EXECUTABLE)

endif
endif # end filter
