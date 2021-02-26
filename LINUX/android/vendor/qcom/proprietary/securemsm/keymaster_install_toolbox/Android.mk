ifeq ($(KMGK_USE_QTI_SERVICE),true)

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
                   InstallKeybox.cpp \
                   DeviceIdProvision.cpp \
                   VerifyAttestation.cpp

LOCAL_C_INCLUDES += \
                    $(LOCAL_PATH)/inc                \
                    $(LOCAL_PATH)/../../inc          \
                    $(TARGET_OUT_HEADERS)/common/inc \
                    $(TARGET_OUT_HEADERS)/qmi-framework/inc \
                    $(TARGET_OUT_HEADERS)/qmi/inc \
                    $(TARGET_OUT_HEADERS)/diag/include \
                    $(TOP)/vendor/qcom/proprietary/securemsm/QSEEComAPI \
                    $(TOP)/vendor/qcom/proprietary/securemsm/keymaster_utils \
                    $(TOP)/external/boringssl/include \
                    $(TOP)/hardware/libhardware/include/hardware \
                    $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \

LOCAL_SHARED_LIBRARIES := \
                          liblog                \
                          libcutils \
                          libcrypto             \
                          libkeymasterprovision \
                          libqmi_cci \
                          libqmiservices \
                          libkeymasterutils \
                          libkeymasterdeviceutils \
                          android.hardware.keymaster@3.0-impl-qti

LOCAL_MODULE := KmInstallKeybox
LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS = -Wall -Werror -Wunused -DKEYMASTER_NAME_TAGS
LOCAL_CLANG := true
LOCAL_CLANG_CFLAGS += -fno-sanitize-coverage=edge,indirect-calls,8bit-counters,trace-cmp
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

include $(BUILD_EXECUTABLE)

endif #ifeq ($(KMGK_USE_QTI_SERVICE),true)
