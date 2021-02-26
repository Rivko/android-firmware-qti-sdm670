LOCAL_PATH:=$(call my-dir)

# Build command line test app: mm-hal3-app

include $(CLEAR_VARS)

LOCAL_C_INCLUDES += \
    hardware/libhardware/include/hardware \
    system/media/camera/include \
    system/media/private/camera/include \
    $(LOCAL_PATH)/ \
    $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \

LOCAL_SRC_FILES := \
    QCameraHAL3MainTestContext.cpp \
    QCameraHAL3Base.cpp \
    QCameraHAL3SnapshotTest.cpp \
    QCameraHAL3Test.cpp \
    QCameraHAL3PreviewTest.cpp \
    QCameraHAL3RawSnapshotTest.cpp \
    QCameraHAL3VideoTest.cpp \

LOCAL_SHARED_LIBRARIES:= libutils libcamera_client liblog libcamera_metadata libcutils libhardware libdl libbinder
LOCAL_32_BIT_ONLY := $(BOARD_QTI_CAMERA_32BIT_ONLY)
LOCAL_MODULE:= hal3-test-app
LOCAL_CFLAGS += -Wall -Wextra
LOCAL_CFLAGS += -std=c++11 -std=gnu++0x
include $(BUILD_EXECUTABLE)
