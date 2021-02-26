ifeq ($(call is-board-platform-in-list, msm8998 apq8098_latv sdm660 sdm845 sdm710 msm8953 qcs605),true)

ifneq ($(BUILD_TINY_ANDROID),true)

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

#----------------------------------------------------------------------------
#                 Common definitons
#----------------------------------------------------------------------------

cal-wrapper-def += -D_ANDROID_

#----------------------------------------------------------------------------
#             Make the Shared library (libgcs-calwrapper)
#----------------------------------------------------------------------------

LOCAL_C_INCLUDES := $(LOCAL_PATH)/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/mm-audio/graphite-client/osal
LOCAL_CFLAGS := $(cal-wrapper-def)

LOCAL_SRC_FILES := src/gcs_acdb.c

LOCAL_SHARED_LIBRARIES := libgcs-osal \
                          libdl \
                          liblog

LOCAL_MODULE := libgcs-calwrapper
LOCAL_MODULE_OWNER := qti
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true

LOCAL_COPY_HEADERS_TO   := mm-audio/graphite-client/cal-wrapper
LOCAL_COPY_HEADERS      := inc/cal_wrapper.h

include $(BUILD_SHARED_LIBRARY)

endif # BUILD_TINY_ANDROID
endif # is-board-platform-in-list
