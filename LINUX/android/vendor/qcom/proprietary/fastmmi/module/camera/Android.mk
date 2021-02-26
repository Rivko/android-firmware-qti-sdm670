
EN_CAM_COMPILE := false
ifneq ($(USE_CAMERA_STUB), true)
EN_CAM_COMPILE := true
endif
## ignore USE_CAMERA_STUB and force compile mmi camera
## for specific hw platform
ifeq ($(call is-board-platform-in-list, msm8909), true)
EN_CAM_COMPILE := true
endif

ifeq ($(EN_CAM_COMPILE), true)
ifneq ($(TARGET_SUPPORTS_WEARABLES),true)
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_SRC_FILES := camera.cpp

LOCAL_MODULE := mmi_camera
LOCAL_32_BIT_ONLY := true
LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS += -DCAMERA_ION_HEAP_ID=ION_CAMERA_HEAP_ID
LOCAL_CFLAGS += -Wall

ifeq ($(strip $(TARGET_USES_ION)),true)
LOCAL_CFLAGS += -DUSE_ION
endif

# use legacy camera api for specific hw platform
ifeq ($(call is-board-platform-in-list, msm8909), true)
LOCAL_CFLAGS += -DCAM_API_LEGACY
endif

LOCAL_C_INCLUDES := $(QC_PROP_ROOT)/fastmmi/libmmi \
                    bootable/recovery/minui \
                    external/libcxx/include \
                    $(QC_PROP_ROOT)/mm-camera/apps/appslib \
                    $(QC_PROP_ROOT)/mm-still/ipl/inc \
                    $(QC_PROP_ROOT)/mm-still/jpeg/inc \
                    hardware/qcom/camera/QCamera2/stack/common \
                    hardware/qcom/camera/QCamera2/stack/mm-camera-test/inc \
                    hardware/qcom/camera/QCamera2/stack/mm-camera-interface/inc \
                    hardware/qcom/camera/mm-image-codec/qexif \
                    hardware/qcom/camera/mm-image-codec/qomx_core \
                    frameworks/native/include/media/openmax \
                    frameworks/base/libs/androidfw/include \
                    external/skia/include/core

LOCAL_C_INCLUDES += system/media/camera/include

LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include/media \
                    $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include

ifeq ($(TARGET_COMPILE_WITH_MSM_KERNEL),true)
LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
endif

LOCAL_SHARED_LIBRARIES := libcutils libutils liblog libmmi libdl libmm-qcamera libmmcamera_interface libui libbinder libgui libskia libandroid libc++

include $(BUILD_SHARED_LIBRARY)
endif
endif
