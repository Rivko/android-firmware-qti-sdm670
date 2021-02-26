ifeq ($(call is-board-platform-in-list,msm8974 msm8960 msm8660 msm8226 msm8610 msm8916 msm8916_32 msm8916_32_k64 msm8916_64 apq8084 msm8939 msm8994 msm8909 msm8996 msm8992 msm8952 msm8937 msm8953 msm8998 apq8098_latv sdm660 sdm845 sdm710),true)

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
                    $(TARGET_OUT_HEADERS)/common/inc \
                    $(TOP)/vendor/qcom/proprietary/securemsm/keymaster_utils \
                    $(TOP)/hardware/libhardware/include/hardware \
                    $(TOP)/vendor/qcom/proprietary/securemsm/QSEEComAPI \

LOCAL_SHARED_LIBRARIES := \
        libc \
        liblog \
        libcutils \
        libutils \
        libdl \
        libkeymasterprovision \
        libkeymasterutils \
        libkeymasterdeviceutils \

LOCAL_MODULE := SoterProvisioningTool
LOCAL_SRC_FILES := SoterProvisioningTool.cpp
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qti
LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)

endif # end filter
