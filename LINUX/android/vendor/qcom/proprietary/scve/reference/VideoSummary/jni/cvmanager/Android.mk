LOCAL_PATH := $(call my-dir)
LOCAL_CV_PATH := $(call my-dir)
LOCAL_DIR_PATH:= $(call my-dir)

COMMON_CFLAGS := -Wno-non-virtual-dtor -fno-exceptions -Wno-format-security -Wno-unused-parameter
COMMON_CFLAGS += -D_ANDROID_

COMMON_CFLAGS += -D_CP_USE_FAST_CV_

include $(CLEAR_VARS)
LOCAL_MULTILIB := 32
LOCAL_MODULE           := libcvmanager
LOCAL_MODULE_TAGS      := optional


LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include

LOCAL_CFLAGS := $(COMMON_CFLAGS)
LOCAL_SRC_FILES += CVManagerNative.cpp
LOCAL_SRC_FILES += CVManagerCommon.cpp CVManagerCalc.cpp CVManagerUtil.cpp
LOCAL_SRC_FILES += CVModuleMotiondetect.cpp CVModuleVideoSummary.cpp
LOCAL_SRC_FILES += MotionDetect.cpp
LOCAL_SRC_FILES += CVModuleASD.cpp
LOCAL_SRC_FILES += CVModuleFaceReco.cpp


#LOCAL_SHARED_LIBRARIES :=
LOCAL_SHARED_LIBRARIES += liblog
#LOCAL_SHARED_LIBRARIES += libfastcvopt libscveVideoSummary libscveFaceRecognition libscveCommon
LOCAL_SHARED_LIBRARIES += libfastcvopt libscveVideoSummary libscveCommon

include vendor/qcom/proprietary/scve/reference/common.mk

#LOCAL_MODULE_TAGS      := optional debug
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)

