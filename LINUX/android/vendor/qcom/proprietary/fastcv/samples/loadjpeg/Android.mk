#
#
# Makefile for FastCV Sample app as built from source tree.
#
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_PRELINK_MODULE:= false

LOCAL_MODULE    := libloadjpeg
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS    := -Werror
LOCAL_SRC_FILES := \
    jni/loadjpeg.cpp \

LOCAL_SHARED_LIBRARIES := libdl liblog libGLESv2 libfastcvopt

LOCAL_C_INCLUDES += vendor/qcom/proprietary/fastcv/samples/loadjpeg/jni
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/fastcv/inc

# Source tree build defaults to using neon, here we need to shut it
# off for cross-device compatibility.
LOCAL_CFLAGS := \
   -O3 -fpic -fno-exceptions -fno-short-enums -fsigned-char \
   -fno-math-errno -fno-signed-zeros \
   -fno-tree-vectorize
LOCAL_MODULE_OWNER := qcom

include $(BUILD_SHARED_LIBRARY)

#
# Build sample application package
#
include $(CLEAR_VARS)

LOCAL_JNI_SHARED_LIBRARIES  := libloadjpeg
LOCAL_SRC_FILES             := $(call all-subdir-java-files)
LOCAL_PACKAGE_NAME          := LoadJpeg

include $(BUILD_PACKAGE)

include $(CLEAR_VARS)

#ifeq ($(FASTCV_MAKE_STANDALONE_APK),true)
     LOCAL_JNI_SHARED_LIBRARIES  += libfastcvopt \
                                             libcutils \
                                             libfastcvopt \
                                             liblog \
                                             libGLESv2 \
                                             libm \
                                             libc++ \
                                             libc \
                                             libdl \

#endif

LOCAL_JNI_SHARED_LIBRARIES   := libloadjpeg
LOCAL_SRC_FILES              := $(call all-subdir-java-files)
LOCAL_PACKAGE_NAME           := LoadJpeg64
#TARGET_BUILD_APPS            := true
LOCAL_MODULE_STEM_32         := LoadJpeg
LOCAL_MODULE_STEM_64         := LoadJpeg64
LOCAL_MULTILIB := both
include $(BUILD_PACKAGE)
