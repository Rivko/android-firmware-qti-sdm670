OLD_LOCAL_PATH := $(LOCAL_PATH)
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
include $(LOCAL_PATH)/../../common.mk

USE_VPP_SERVICE := true
LOCAL_FILES := \
	src/test_main.cpp \
	src/test_buffer_exchange.cpp \
	src/test_concurrency.cpp \
	src/test_stub.cpp

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/inc/ \
	$(LOCAL_PATH)/../core/inc/ \
	$(LOCAL_PATH)/../utils/inc/ \
	$(LOCAL_PATH)/../../lib/utils/inc \
	$(LOCAL_PATH)/../../lib/core/inc \
	$(LOCAL_PATH)/../../lib/ip/gpu/inc \
	$(LOCAL_PATH)/../../lib/ip/frc/inc \
	$(LOCAL_PATH)/../../inc \
	$(LOCAL_PATH)/../../ipc/inc

LOCAL_CPPFLAGS := -Wall -Werror -Wextra -D_ANDROID_ -DVPP_FUNCTIONAL_TEST
ifeq ($(USE_VPP_SERVICE),true)
	LOCAL_CPPFLAGS += -DVPP_SERVICE
	LOCAL_SHARED_LIBRARIES += \
		libhidlbase \
		libhidltransport \
		libhwbinder \
		libutils \
		libhardware \
		vendor.qti.hardware.vpp@1.1_vendor \
		android.hidl.base@1.0 \
		libvppclient
endif

LOCAL_32_BIT_ONLY := true
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_SRC_FILES := $(LOCAL_FILES)
LOCAL_MODULE := vpplibraryfunctionaltest
LOCAL_CLANG := true
LOCAL_SHARED_LIBRARIES += libcutils liblog libvpplibrary libvpptestutils
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti
include $(BUILD_EXECUTABLE)

LOCAL_PATH := $(OLD_LOCAL_PATH);
