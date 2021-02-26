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

LOCAL_JNI_SHARED_LIBRARIES  := libSTAWrapper
LOCAL_SRC_FILES             := $(call all-subdir-java-files)
LOCAL_PACKAGE_NAME          := STA
#TARGET_BUILD_APPS           := true
LOCAL_PROGUARD_FLAG_FILES   := proguard-project.txt

ifeq ($(SCVE_MAKE_STANDALONE_APK),true)
   LOCAL_JNI_SHARED_LIBRARIES += libscveTextReco \
                                 libscveTextReco_stub \
                                 libscveImageCorrection \
                                 libscveCommon \
                                 libscveCommon_stub \
                                 libscveTextRecoPostProcessing \
                                 libfastcvopt \
                                 libadsprpc \
                                 libc++ \
                                 libcutils
endif

include vendor/qcom/proprietary/scve/reference/common.mk

include $(BUILD_PACKAGE)
