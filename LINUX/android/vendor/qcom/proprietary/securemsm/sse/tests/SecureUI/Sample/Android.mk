LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../../QSEEComAPI \
                    $(LOCAL_PATH)/../../../../sse/SecureUILib \
                    $(LOCAL_PATH)/../../../SecureUI \
                    $(TARGET_OUT_HEADERS)/common/inc

LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_SHARED_LIBRARIES := \
        libcutils \
        libutils \
        liblog \
        libQSEEComAPI \
        libsecureui_svcsock \
        libSecureUILib \
        libStDrvInt

LOCAL_MODULE := secure_ui_sample_client
ifeq ($(PRODUCT_VENDOR_MOVE_ENABLED),true)
LOCAL_PROPRIETARY_MODULE := true
endif
LOCAL_SRC_FILES := secure_ui_sample_client.c
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS := $(QSEECOM_CFLAGS)
LOCAL_MODULE_OWNER := qcom
include $(BUILD_EXECUTABLE)
