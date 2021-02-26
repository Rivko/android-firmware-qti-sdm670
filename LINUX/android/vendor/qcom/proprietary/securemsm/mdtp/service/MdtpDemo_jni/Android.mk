# ---------------------------------------------------------------------------------
#                 Common definitions
# ---------------------------------------------------------------------------------

ifeq ($(call is-board-platform-in-list,msm8996 msm8937 titanium_64 titanium_32 titanium msm8953 msm8953_32 msm8953_64 msm8998 apq8098_latv),true)

LOCAL_PATH   := $(call my-dir)

OPENSSL_PATH := $(LOCAL_PATH)/../openssl

COMMON_INCLUDES := $(LOCAL_PATH) \
                   $(TARGET_OUT_INTERMEDIATES)/include/libmdtp_crypto \
                   $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \

include $(CLEAR_VARS)

LOCAL_MODULE := libmdtpdemojni

LOCAL_C_INCLUDES += $(COMMON_INCLUDES)

# Add all source file names to be included in lib separated by a whitespace
LOCAL_SRC_FILES := ServerSigner_jni.c mdtp_msg_asn1.c

LOCAL_SHARED_LIBRARIES := libmdtp_crypto liblog libutils libcutils

LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti
LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
include $(BUILD_SHARED_LIBRARY)

endif # end filter
