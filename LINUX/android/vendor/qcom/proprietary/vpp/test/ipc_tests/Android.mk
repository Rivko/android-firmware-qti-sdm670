OLD_LOCAL_PATH := $(LOCAL_PATH)
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
include $(LOCAL_PATH)/../../common.mk

LOCAL_FILES := \
	src/test_ipc_main.cpp \
	src/test_ipc_utils.cpp \
	src/test_ipc_client.cpp \
	src/test_ipc_service.cpp \
	../../ipc/src/HidlVpp.cpp \
	../../ipc/src/HidlVppUtils.cpp

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../../ipc/inc \
	$(LOCAL_PATH)/../core/inc/ \
	$(LOCAL_PATH)/../utils/inc/ \
	$(LOCAL_PATH)/../../lib/utils/inc \
	$(LOCAL_PATH)/../../lib/core/inc \
	$(LOCAL_PATH)/../../inc \
	$(TARGET_OUT_HEADERS)/qcom/display

LOCAL_CPPFLAGS += -Wall -Werror -D_ANDROID_ -fexceptions

LOCAL_32_BIT_ONLY := true
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_SRC_FILES := $(LOCAL_FILES)
LOCAL_MODULE := vppipcunittest
LOCAL_CLANG := true
LOCAL_SHARED_LIBRARIES += libcutils liblog libvpplibrary libvpptestutils
LOCAL_SHARED_LIBRARIES += \
	libhidlbase \
	libhidltransport \
	libhwbinder \
	libutils \
	libhardware \
	vendor.qti.hardware.vpp@1.1_vendor \
	android.hidl.base@1.0 \
	libvppclient \
	libqdMetaData

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti
include $(BUILD_EXECUTABLE)

LOCAL_PATH := $(OLD_LOCAL_PATH);
