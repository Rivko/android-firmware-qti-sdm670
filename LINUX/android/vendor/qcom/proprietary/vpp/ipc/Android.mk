LOCAL_PATH := $(call my-dir)
#######################################################################
#
# libvppservice
#
#######################################################################
include $(CLEAR_VARS)

LOCAL_MODULE := vppservice

LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti
LOCAL_CPPFLAGS := -Wall -Werror -Wextra -D_ANDROID_ -DVPP_IPC
LOCAL_SRC_FILES := \
	src/VppService.cpp \
	src/HidlVpp.cpp \
	src/HidlVppService.cpp \
	src/HidlVppUtils.cpp

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/inc \
	$(LOCAL_PATH)/../inc \
	$(LOCAL_PATH)/../lib/core/inc \
	$(LOCAL_PATH)/../lib/utils/inc \
	$(LOCAL_PATH)/../test/utils/inc \
	$(TARGET_OUT_HEADERS)/qcom/display

LOCAL_SHARED_LIBRARIES := \
	libhidlbase \
	libhidltransport \
	liblog \
	libhwbinder \
	libutils \
	libhardware \
	vendor.qti.hardware.vpp@1.1_vendor \
	android.hidl.base@1.0 \
	libcutils \
	libvpplibrary \
	libqdMetaData

include $(BUILD_EXECUTABLE)

#######################################################################
#
# libvppclient
#
#######################################################################
include $(CLEAR_VARS)
include $(LOCAL_PATH)/../common.mk

LOCAL_FILES := \
	src/VppClient.cpp \
	src/HidlVppCallbacks.cpp \
	src/HidlVppUtils.cpp

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/inc \
	$(LOCAL_PATH)/../inc \
	$(LOCAL_PATH)/../lib/core/inc \
	$(LOCAL_PATH)/../lib/utils/inc \
	$(LOCAL_PATH)/../test/utils/inc \
	$(TARGET_OUT_HEADERS)/qcom/display

LOCAL_CPPFLAGS += -Wall -Werror -D_ANDROID_ -DVPP_IPC

LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_SRC_FILES := $(LOCAL_FILES)
LOCAL_MODULE := libvppclient
LOCAL_CLANG := true
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti
LOCAL_SHARED_LIBRARIES := \
	libhidlbase \
	libhidltransport \
	liblog \
	libhwbinder \
	libutils \
	libhardware \
	vendor.qti.hardware.vpp@1.1_vendor \
	android.hidl.base@1.0 \
	libvpplibrary \
	libcutils \
	libqdMetaData

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
include $(BUILD_SHARED_LIBRARY)