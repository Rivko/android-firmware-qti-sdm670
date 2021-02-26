ifneq ($(BUILD_TINY_ANDROID),true)


ROOT_DIR := $(call my-dir)
LOCAL_PATH := $(ROOT_DIR)
include $(CLEAR_VARS)


# ---------------------------------------------------------------------------------
#                               Common definitons
# ---------------------------------------------------------------------------------


# ---------------------------------------------------------------------------------
#                       Make the usf_sw_calib daemon
# ---------------------------------------------------------------------------------

include $(CLEAR_VARS)


LOCAL_MODULE            := usf_sw_calib
ifeq ($(PRODUCT_VENDOR_MOVE_ENABLED),true)
LOCAL_PROPRIETARY_MODULE := true
endif
LOCAL_MODULE_TAGS       := optional
LOCAL_C_INCLUDES        := $(LOCAL_PATH)/../../ual \
                           $(LOCAL_PATH)/../../ual_util \
                           $(LOCAL_PATH)/../usf_epos \
                           $(TARGET_OUT_HEADERS)/ultrasound/inc

LOCAL_SHARED_LIBRARIES  := liblog \
                           libcutils \
                           libual \
                           libualutil \
                           libdpencalib

LOCAL_SRC_FILES         := usf_sw_calib.cpp

LOCAL_MODULE_OWNER := qti
include $(BUILD_EXECUTABLE)

endif #BUILD_TINY_ANDROID
