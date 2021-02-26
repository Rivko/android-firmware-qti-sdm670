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

LOCAL_JNI_SHARED_LIBRARIES  := libfeature_cv_jni \
                               libUtilsColorConv \
                               libVideoFrameUtils \
                               libfastcvopt \
                               libscveVideoSummary \
                               libcvmanager \
                               libscveCommon

LOCAL_RESOURCE_DIR += $(LOCAL_PATH)/res
LOCAL_RESOURCE_DIR += frameworks/support/v7/appcompat/res
LOCAL_RESOURCE_DIR += frameworks/support/v7/recyclerview/res
LOCAL_RESOURCE_DIR += frameworks/support/v7/cardview/res
LOCAL_RESOURCE_DIR += frameworks/support/design/res

LOCAL_STATIC_JAVA_LIBRARIES += android-support-v4
LOCAL_STATIC_JAVA_LIBRARIES += android-support-v7-appcompat
LOCAL_STATIC_JAVA_LIBRARIES += android-support-v7-cardview
LOCAL_STATIC_JAVA_LIBRARIES += android-support-v7-recyclerview
LOCAL_STATIC_JAVA_LIBRARIES += android-support-design

LOCAL_AAPT_FLAGS := --auto-add-overlay
LOCAL_AAPT_FLAGS += --extra-packages android.support.v4
LOCAL_AAPT_FLAGS += --extra-packages android.support.v7.appcompat
LOCAL_AAPT_FLAGS += --extra-packages android.support.v7.recyclerview
LOCAL_AAPT_FLAGS += --extra-packages android.support.v7.cardview
LOCAL_AAPT_FLAGS += --extra-packages android.support.design

LOCAL_MODULE_TAGS := tests
LOCAL_SRC_FILES             := $(call all-subdir-java-files)
LOCAL_PACKAGE_NAME          := VideoSummaryRef
#TARGET_BUILD_APPS          := true
LOCAL_PROGUARD_FLAG_FILES   := proguard-project.txt
ifeq ($(SCVE_MAKE_STANDALONE_APK),true)
   LOCAL_JNI_SHARED_LIBRARIES  +=   libscveCommon_stub \
                                    libscveVideoSummary_stub \
                                    libcutils \
                                    libc++
   ifeq ($(CDSP_ENABLE),true)
      LOCAL_JNI_SHARED_LIBRARIES += libcdsprpc
   else
      LOCAL_JNI_SHARED_LIBRARIES += libadsprpc
   endif

   ifneq ($(call is-board-platform-in-list,sdm845),true)
      LOCAL_JNI_SHARED_LIBRARIES += libfastcvadsp_stub
   endif

endif
include vendor/qcom/proprietary/scve/reference/common.mk

include $(BUILD_PACKAGE)
