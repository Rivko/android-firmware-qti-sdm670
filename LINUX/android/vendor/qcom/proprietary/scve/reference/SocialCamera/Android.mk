TOP_LOCAL_PATH := $(call my-dir)

SCVE_MAKE_FACERECO_USE_FACELANDMARKS := false
SCVE_MAKE_FACERECOGNITION_QDSP_AVAILABLE := false
SCVE_MAKE_USE_FACEPROC1_SW := false
SCVE_MAKE_USE_FACEPROC2_SW := true

SCVE_MAKE_QDSP_HVX_TARGETS     := msm8996 msmcobalt msm8998 sdm660 sdm845 sdm710 qcs605
ifneq ($(call is-board-platform-in-list,$(SCVE_MAKE_QDSP_HVX_TARGETS)),true)
MAKE_CPU_ONLY             := true
endif

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

LOCAL_JNI_SHARED_LIBRARIES  := liballocationimage \
                               libFaceRecognitionJNI \
                               libsocialcamera_util \
                               libsocialcamera_khr_egl_extensions \
                               libsocialcamera_khr_gles_extensions

ifeq ($(SCVE_MAKE_STANDALONE_APK), true)
   LOCAL_JNI_SHARED_LIBRARIES += libscveCommon \
                                 libscveFaceRecognition \
                                 libfastcvopt \
                                 libOpenCL \
                                 libgsl \
                                 libCB
   ifeq ($(SCVE_MAKE_USE_FACEPROC2_SW), true)
      LOCAL_JNI_SHARED_LIBRARIES += libmmcamera_faceproc2
   else
      LOCAL_JNI_SHARED_LIBRARIES += libmmcamera_faceproc
   endif
   ifneq ($(MAKE_CPU_ONLY), true)
      LOCAL_JNI_SHARED_LIBRARIES += libscveCommon_stub
      ifeq ($(SCVE_MAKE_FACERECOGNITION_QDSP_AVAILABLE),true)
         LOCAL_JNI_SHARED_LIBRARIES += libscveFaceRecognition_stub
      endif
      ifeq ($(CDSP_ENABLE),true)
         LOCAL_JNI_SHARED_LIBRARIES += libcdsprpc
      else
         LOCAL_JNI_SHARED_LIBRARIES += libadsprpc
      endif
   endif

   ifeq ($(SCVE_MAKE_FACERECO_USE_FACELANDMARKS), true)
      LOCAL_JNI_SHARED_LIBRARIES += libscveFaceLandmarks
      ifneq ($(MAKE_CPU_ONLY), true)
         ifeq ($(SCVE_MAKE_FACELANDMARKS_QDSP_AVAILABLE), true)
            LOCAL_JNI_SHARED_LIBRARIES += libscveFaceLandmarks_stub
         endif
      endif
   endif
endif

LOCAL_SRC_FILES             := $(call all-java-files-under, java/android) \
                               $(call all-java-files-under, java/com/qualcomm/qti/facerecognition) \
                               $(call all-java-files-under, java/com/qualcomm/qti/fr_camera) \
                               $(call all-java-files-under, java/com/qualcomm/qti/khronos) \
                               $(call all-renderscript-files-under, rs)

ifeq ($(call is-platform-sdk-version-at-least,23), true)
LOCAL_SRC_FILES             += $(call all-java-files-under, java/com/qualcomm/qti/permission/request)
else
LOCAL_SRC_FILES             += $(call all-java-files-under, java/com/qualcomm/qti/permission/dummy)
endif


LOCAL_PACKAGE_NAME          := SocialCamera
#TARGET_BUILD_APPS           := true
LOCAL_PROGUARD_FLAG_FILES   := proguard-project.txt

LOCAL_RENDERSCRIPT_TARGET_API := 21

include vendor/qcom/proprietary/scve/reference/common.mk

include $(BUILD_PACKAGE)
