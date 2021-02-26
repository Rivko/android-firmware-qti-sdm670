TOP_LOCAL_PATH := $(call my-dir)

#
# Build JNI library
#

include $(call all-subdir-makefiles)

#
# Build sample application package
#

LOCAL_PATH:= $(TOP_LOCAL_PATH)
include $(CLEAR_VARS)

LOCAL_MODULE_PATH := $(PRODUCT_OUT)/data/qcom/scve

LOCAL_JNI_SHARED_LIBRARIES  := libclevercapture_khr_egl_extensions \
                               libclevercapture_khr_gles_extensions \
                               libclevercapture_util \
                               libclevercapture \
                               libimagebufferutils

LOCAL_SRC_FILES             := $(call all-subdir-java-files)
LOCAL_PACKAGE_NAME          := CleverCaptureDemo
#TARGET_BUILD_APPS           := true
#LOCAL_PROGUARD_FLAG_FILES   := proguard-project.txt

ifeq ($(SCVE_MAKE_STANDALONE_APK),true)
   LOCAL_JNI_SHARED_LIBRARIES  += libscveCleverCapture \
                                  libscveCleverCapture_stub \
                                  libscveCommon \
                                  libscveCommon_stub \
                                  libscveMotionVector \
                                  libfastcvopt \
                                  libadsprpc
endif

include vendor/qcom/proprietary/scve/reference/common.mk

include $(BUILD_PACKAGE)
