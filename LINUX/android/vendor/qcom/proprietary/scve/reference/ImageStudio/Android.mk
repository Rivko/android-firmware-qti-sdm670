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

LOCAL_STATIC_JAVA_LIBRARIES += android-support-v4

LOCAL_JNI_SHARED_LIBRARIES  := libBitmapUtils \
                               libFastCV \
                               libImageCloningSample \
                               libImageRemoval \
                               libSoftSegmentation
LOCAL_SRC_FILES             := $(call all-renderscript-files-under, rs) \
                               $(call all-subdir-java-files)
LOCAL_PACKAGE_NAME          := ImageStudioRef
#TARGET_BUILD_APPS           := true
LOCAL_PROGUARD_FLAG_FILES   := proguard-project.txt

ifeq ($(SCVE_MAKE_STANDALONE_APK),true)
   LOCAL_JNI_SHARED_LIBRARIES  += libscveObjectSegmentation \
                                  libscveObjectMatting \
                                  libscveObjectMatting_stub \
                                  libscveImageCloning \
                                  libscveImageRemoval \
                                  libscveCommon \
                                  libscveCommon_stub \
                                  libfastcvopt \
                                  libadsprpc
endif


include vendor/qcom/proprietary/scve/reference/common.mk

include $(BUILD_PACKAGE)
