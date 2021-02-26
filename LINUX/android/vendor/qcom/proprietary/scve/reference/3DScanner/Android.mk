ifeq ($(call is-platform-sdk-version-at-least,24),true)
JPEG_ROOT := external/libjpeg-turbo
else
JPEG_ROOT := external/jpeg
endif

TOP_LOCAL_PATH := $(call my-dir)

########################################
# Build JNI library
########################################
include $(call all-subdir-makefiles)

########################################
# Build the 3DScanner apk
########################################
LOCAL_PATH:= $(TOP_LOCAL_PATH)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := qcom

LOCAL_SRC_FILES := $(call all-subdir-java-files)

LOCAL_STATIC_JAVA_LIBRARIES += android-support-v7-appcompat \
                               android-support-v7-recyclerview \
                               android-support-v13 \
                               android-support-v7-cardview \
                               android-common \
                               android-support-design \
                               android-support-v7-preference \
                               android-support-v14-preference

LOCAL_RESOURCE_DIR := \
   $(LOCAL_PATH)/res \
   frameworks/support/v7/appcompat/res \
   frameworks/support/v7/recyclerview/res \
   frameworks/support/v7/cardview/res \
   frameworks/support/design/res \
   frameworks/support/v7/preference/res \
   frameworks/support/v14/preference/res

LOCAL_AAPT_FLAGS := \
   --auto-add-overlay \
   --extra-packages android.support.v7.appcompat \
   --extra-packages android.support.v7.recyclerview \
   --extra-packages android.support.v13 \
   --extra-packages android.support.v7.cardview \
   --extra-packages android.common \
   --extra-packages android.support.design \
   --extra-packages android.support.v7.preference \
   --extra-packages android.support.v14.preference

LOCAL_PROGUARD_FLAG_FILES := proguard.flags

LOCAL_PACKAGE_NAME := 3DScanner

LOCAL_MODULE_PATH := $(PRODUCT_OUT)/data/qcom/scve

TARGET_BUILD_APPS           := true

include vendor/qcom/proprietary/scve/reference/common.mk

#####################################################
# Users are reponsible for downloading the libs below,
# and add them to LOCAL_JNI_SHARED_LIBRARIES.
#		libOniFile \
#		libOpenNI2.jni \
#		libOpenNI2 \
#		liborbbec \
#		libPSLink \
#		libusb-1.0.9 \
#####################################################
LOCAL_JNI_SHARED_LIBRARIES += libglviewer \
                              lib3DR \
                              lib3DRgl2jni \
										lib3drNative-lib \
#                              libOniFile \
#                              libOpenNI2.jni \
#                              libOpenNI2 \
#                              liborbbec \
#                              libPSLink \
#                              libusb-1.0.9

ifeq ($(SCVE_MAKE_STANDALONE_APK), true)
   LOCAL_JNI_SHARED_LIBRARIES += libscveScan3D \
                                 libscveCommon \
                                 libscveCommon_stub \
                                 libfastcvopt \
											libjpeg
   ifeq ($(CDSP_ENABLE),true)
      LOCAL_JNI_SHARED_LIBRARIES += libcdsprpc
   else
      LOCAL_JNI_SHARED_LIBRARIES += libadsprpc
   endif
endif

include $(BUILD_PACKAGE)
