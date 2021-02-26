LOCAL_PATH:= $(call my-dir)
#######################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
     BufferManager.cpp \
     TestLog.cpp \
     QCameraHAL3Base.cpp \
     QCameraHAL3Device.cpp \
     QCameraHAL3TestSnapshot.cpp \
     QCameraHAL3TestPreview.cpp \
     QCameraHALTestMain.cpp \
     QCameraHAL3TestVideo.cpp \
     QCameraTestVideoEncoder.cpp \
     QCameraHAL3TestConfig.cpp \

#start for autogen version info file
CAMXVERSIONTOOL := perl $(LOCAL_PATH)/version.pl
CAMX_VERSION_OUTPUT = $(LOCAL_PATH)/g_version.h
$(info $(shell $(CAMXVERSIONTOOL) $(CAMX_VERSION_OUTPUT)))
#End for autogen version info file

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    liblog \
    libhardware \
    libcamera_metadata \
    libcamera_client \
    libomx_encoder \

LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include

LOCAL_C_INCLUDES += \
    system/media/camera/include \
    system/media/private/camera/include \
    system/core/include/cutils \
    system/core/include/system \
    system/core/libsystem/include \
    frameworks/native/libs/nativebase/include \
    frameworks/native/libs/arect/include \
    frameworks/native/libs/nativewindow/include/ \
    frameworks/native/include/media/hardware \
    system/core/libgrallocusage/include \
    hardware/libhardware/include  \
    hardware/qcom/display/libgralloc1 \
    hardware/qcom/media/mm-core/inc \
    hardware/qcom/media/libstagefrighthw \

LOCAL_MODULE:= camx-hal3-test
LOCAL_CFLAGS += -Wall -Wextra -Werr -Wno-unused-parameter -Wno-unused-variable
LOCAL_CFLAGS += -DCAMERA_STORAGE_DIR="\"/data/misc/camera/\""
LOCAL_CFLAGS += -std=c++14 -std=gnu++0x
LOCAL_CFLAGS += -DDISABLE_META_MODE=1
#LOCAL_CFLAGS += -DUSE_GRALLOC1
LOCAL_MODULE_TAGS := tests
LOCAL_32_BIT_ONLY := true
include $(BUILD_EXECUTABLE)

#######################################################
include $(CLEAR_VARS)
LOCAL_SRC_FILES:= \
    OMX_Encoder.cpp \
    BufferManager.cpp \

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    liblog \
    libhardware \
    libOmxCore \

LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include

LOCAL_C_INCLUDES += \
    system/media/camera/include \
    system/media/private/camera/include \
    system/core/include/cutils \
    system/core/include/system \
    system/core/libsystem/include \
    frameworks/native/libs/nativebase/include \
    frameworks/native/libs/nativewindow/include/ \
    frameworks/native/libs/arect/include \
    frameworks/native/include/media/hardware \
    system/core/libgrallocusage/include \
    hardware/libhardware/include  \
    hardware/qcom/media/mm-core/inc \
    hardware/qcom/display/libgralloc1 \
    hardware/qcom/media/libstagefrighthw \

LOCAL_MODULE:= libomx_encoder
LOCAL_CFLAGS += -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable
LOCAL_CFLAGS += -DCAMERA_STORAGE_DIR="\"/data/misc/camera/\""
LOCAL_CFLAGS += -std=c++14 -std=gnu++0x
LOCAL_CFLAGS += -DDISABLE_META_MODE=1
#LOCAL_CFLAGS += -DUSE_GRALLOC1

LOCAL_MODULE_TAGS := tests
LOCAL_32_BIT_ONLY := true

include $(BUILD_SHARED_LIBRARY)