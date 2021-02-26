ACLOCAL_AMFLAGS = -I m4
curr_path = $(top_srcdir)/QCamera2/HAL3/test

# Build command line test app: mm-hal3-app

bin_PROGRAMS = hal3-test-app

hal3_test_app_CFLAGS = -Wall -Wextra -Werror

hal3_test_app_CPPFLAGS = -DQCAMERA_HAL3_SUPPORT

if MSM8953
hal3_test_app_CPPFLAGS += -DCAMERA_CHIPSET_8953
endif

hal3_test_app_CPPFLAGS += \
    -I$(WORKSPACE)/system/media/camera/include \
    -I$(WORKSPACE)/system/media/private/camera/include \
    -I$(curr_path)/../ \
    -I$(curr_path)/../../stack/common \
    -I$(curr_path)/../../stack/mm-camera-interface/inc \
    -I$(WORKSPACE)/hardware/libhardware/include/hardware \
    -I$(WORKSPACE)/hardware/qcom/media/libstagefrighthw \
    -I$(WORKSPACE)/hardware/qcom/media/mm-core/inc \
    -I$(WORKSPACE)/system/core/include \
    -I$(WORKSPACE)/system/core/include/system \
    -I$(WORKSPACE)/system/media/camera/include/system

hal3_test_app_CPPFLAGS += -include stdint.h
hal3_test_app_CPPFLAGS += -include glib.h
hal3_test_app_CPPFLAGS += -include glibconfig.h
hal3_test_app_CPPFLAGS += -include sys/ioctl.h
hal3_test_app_CPPFLAGS += -include cutils/trace.h
hal3_test_app_CPPFLAGS += -include sys/stat.h

hal3_test_app_SOURCES = \
    QCameraHAL3Base.cpp \
    QCameraHAL3MainTestContext.cpp \
    QCameraHAL3VideoTest.cpp \
    QCameraHAL3PreviewTest.cpp \
    QCameraHAL3SnapshotTest.cpp \
    QCameraHAL3RawSnapshotTest.cpp \
    QCameraHAL3Test.cpp

hal3_test_app_LDADD = -lutils -lcamera_client -llog -lcamera_metadata -lcutils -lhardware
