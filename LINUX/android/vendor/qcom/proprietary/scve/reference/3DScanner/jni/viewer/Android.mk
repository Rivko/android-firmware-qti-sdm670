LOCAL_PATH:= $(call my-dir)

COMMON_LDLIBS    := libm liblog libandroid libGLESv3 libEGL

MY_GNU_C_FLAGS   := -mfloat-abi=softfp -O3 -mfpu=neon -mtune=cortex-a57 -Wno-write-strings

include $(CLEAR_VARS)

LOCAL_MODULE    := libglviewer

LOCAL_ARM_NEON   := true
LOCAL_ARM_MODE   := arm
LOCAL_CFLAGS    := -mfloat-abi=softfp -mfpu=neon -mtune=cortex-a15 -Wno-write-strings -gdwarf-3

LOCAL_SRC_FILES         := Main.cpp \
                           Matrix44.cpp \
                           Mesh.cpp \
                           MeshViewer.cpp \
                           Shader.cpp \
                           simple_pixel.cpp \
                           simple_vertex.cpp \
                           ViewController.cpp \
                           gl_code.cpp

LOCAL_C_INCLUDES += $(LOCAL_PATH)/.. \
						  $(LOCAL_PATH)/../scan/libjpeg

LOCAL_SHARED_LIBRARIES += ${COMMON_LDLIBS}

LOCAL_STATIC_LIBRARIES += libjpgIO

LOCAL_SHARED_LIBRARIES += libjpeg

include vendor/qcom/proprietary/scve/reference/common.mk

include $(BUILD_SHARED_LIBRARY)


