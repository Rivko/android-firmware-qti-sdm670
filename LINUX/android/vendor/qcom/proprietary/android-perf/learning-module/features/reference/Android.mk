# LM_PATH points to learning-module folder and comes from main Android.mk
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := ReferenceFeature.cpp ReferenceMetaMeter.cpp ReferenceAlgo.cpp ReferenceAction.cpp
LOCAL_SHARED_LIBRARIES := liblog libc libcutils liblearningmodule libmeters libsqlite
LOCAL_C_INCLUDES := $(LM_INCLUDES) \
    $(LM_PATH)/meters/reference \
    $(MPCTL_PATH) \
    $(PERFHAL_PATH) \
    $(LM_PATH)/sharedactions
LOCAL_MULTILIB := first
LOCAL_MODULE := libreffeature
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)
