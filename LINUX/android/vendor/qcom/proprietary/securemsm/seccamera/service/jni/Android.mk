ifneq ($(BUILD_TINY_ANDROID),true)

LOCAL_PATH := $(call my-dir)

HAS_SECURE_PROCESSOR := $(call is-board-platform-in-list,8998 sdm845i sdm855 sdm1000)

include $(CLEAR_VARS)

LOCAL_MODULE        := libseccamservice
LOCAL_MODULE_TAGS    := optional

LOCAL_CFLAGS        := $(COMMON_CFLAGS) \
                        -fno-short-enums \
                        -g -fdiagnostics-show-option -Wno-format \
                        -Wno-missing-braces -Wno-missing-field-initializers \
                        -fpermissive -Wno-unused-parameter

ifeq ($(HAS_SECURE_PROCESSOR),true)
  LOCAL_CFLAGS += -DHAS_SECURE_PROCESSOR
endif

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := false

SECUREMSM_SHIP_PATH :=  $(QC_PROP_ROOT)/securemsm

LOCAL_C_INCLUDES += \
  $(LOCAL_PATH) \
  $(SECUREMSM_SHIP_PATH)/seccamera/lib \
  $(SECUREMSM_SHIP_PATH)/QSEEComAPI \
  $(TARGET_OUT_HEADERS)/common/inc

ifeq ($(HAS_SECURE_PROCESSOR),true)
   LOCAL_C_INCLUDES += $(SECUREMSM_SHIP_PATH)/sp_iris_lib/inc
endif


LOCAL_SHARED_LIBRARIES := \
    libc \
    liblog \
    libandroid \
    libnativewindow \
    libseccam

ifeq ($(HAS_SECURE_PROCESSOR),true)
    LOCAL_SHARED_LIBRARIES += libspiris
endif


LOCAL_SRC_FILES := \
    jni_if.cpp \
    jni_vendor_if.cpp

include $(BUILD_SHARED_LIBRARY)

endif # not BUILD_TINY_ANDROID
