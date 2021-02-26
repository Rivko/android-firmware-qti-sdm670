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

LOCAL_JNI_SHARED_LIBRARIES  := libobjecttrackerref_khr_egl_extensions \
                               libobjecttrackerref_khr_gles_extensions \
                               libobjecttrackerref_jni
LOCAL_SRC_FILES             := $(call all-subdir-java-files)
LOCAL_PACKAGE_NAME          := ObjectTrackerRef
#TARGET_BUILD_APPS           := true
LOCAL_PROGUARD_FLAG_FILES   := proguard-project.txt

ifeq ($(SCVE_MAKE_STANDALONE_APK), true)
   LOCAL_JNI_SHARED_LIBRARIES += libscveObjectTracker \
                                 libscveObjectTracker_stub \
                                 libscveObjectSegmentation \
                                 libscveCommon \
                                 libscveCommon_stub \
                                 libfastcvopt \
                                 libOpenCL \
                                 libgsl \
                                 libCB
   ifeq ($(CDSP_ENABLE),true)
      LOCAL_JNI_SHARED_LIBRARIES += libcdsprpc
   else
      LOCAL_JNI_SHARED_LIBRARIES += libadsprpc
   endif
endif

include vendor/qcom/proprietary/scve/reference/common.mk

include $(BUILD_PACKAGE)
