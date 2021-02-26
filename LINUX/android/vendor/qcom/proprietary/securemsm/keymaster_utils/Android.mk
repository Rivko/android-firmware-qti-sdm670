LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libkeymasterutils

LOCAL_SRC_FILES := \
    authorization_set.cpp \
    serializable.cpp \
    attestation_record.cpp

LOCAL_C_INCLUDES := \
                    $(TOP)/vendor/qcom/proprietary/securemsm/QSEEComAPI \
                    $(TOP)/external/openssl/include \
                    $(TOP)/vendor/qcom/proprietary/securemsm/keymaster_install_toolbox


#LOCAL_CFLAGS := $(keymaster-def)
LOCAL_CFLAGS := -Wall -Werror -Wunused
LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libQSEEComAPI \
    liblog \
    libcrypto

LOCAL_CLANG := true
LOCAL_CLANG_CFLAGS += -Wno-error=unused-const-variable -Wno-error=unused-private-field
LOCAL_CLANG_CFLAGS += -fno-sanitize-coverage=edge,indirect-calls,8bit-counters,trace-cmp
LOCAL_ADDITIONAL_DEPENDENCIES := $(LOCAL_PATH)/Android.mk
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/include
LOCAL_MODULE_TAGS := optional

LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)
