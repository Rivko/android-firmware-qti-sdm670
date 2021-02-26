ifneq ($(TARGET_NO_TELEPHONY), true)

ifneq ($(TARGET_HAS_LOW_RAM),true)

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS += -Wall -Werror
ifeq ($(call is-platform-sdk-version-at-least,18),true)
 # JB MR2 or later
 LOCAL_CFLAGS += -DANDROID_JB_MR2=1
endif

LOCAL_C_INCLUDES := \
system/kernel_headers/ \
        $(LOCAL_PATH) \
        $(TARGET_OUT_HEADERS)/qmi/inc/ \
        $(TARGET_OUT_HEADERS)/qmi-framework/inc/ \
        $(TARGET_OUT_HEADERS)/common/inc/ \
        $(TARGET_OUT_HEADERS)/libmdmdetect/inc \

LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc/
LOCAL_SRC_FILES := \
        atfwd_daemon.c \
        sendcmd.cpp

LOCAL_SHARED_LIBRARIES += libcutils libutils libc libqmi libmdmdetect libqmi_client_qmux libqmi_client_helper libqmiservices libqmi_cci liblog
LOCAL_SHARED_LIBRARIES += libbase libhidlbase libhidltransport libhwbinder vendor.qti.hardware.radio.atcmdfwd@1.0_vendor

LOCAL_CPPFLAGS += -std=c++14

LOCAL_MODULE:= ATFWD-daemon

LOCAL_CLANG:=true

LOCAL_MODULE_TAGS := optional

LDLIBS += -lpthread

LOCAL_MODULE_OWNER := qcom

LOCAL_PROPRIETARY_MODULE := true

ifeq ($(ATEL_ENABLE_LLVM_SA),true)
    LLVM_SA_OUTPUT_DIR := $(ANDROID_PRODUCT_OUT)/atel-llvm-sa-results/$(LOCAL_MODULE)
    LLVM_SA_FLAG := --compile-and-analyze $(LLVM_SA_OUTPUT_DIR)
    LOCAL_CFLAGS += $(LLVM_SA_FLAG)
    LOCAL_CPPFLAGS += $(LLVM_SA_FLAG)
endif

include $(BUILD_EXECUTABLE)

endif
endif #TARGET_NO_TELEPHONY
