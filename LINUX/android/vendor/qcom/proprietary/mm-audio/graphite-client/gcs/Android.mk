ifeq ($(call is-board-platform-in-list, msm8998 apq8098_latv sdm660 sdm845 sdm710 msm8953 qcs605),true)

ifneq ($(BUILD_TINY_ANDROID),true)

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

#----------------------------------------------------------------------------
#                 Common definitons
#----------------------------------------------------------------------------

gcs-def += -D_ANDROID_

#----------------------------------------------------------------------------
#             Make the Shared library (libgcs)
#----------------------------------------------------------------------------

LOCAL_C_INCLUDES := $(LOCAL_PATH)/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/mm-audio/graphite-client/osal
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/mm-audio/graphite-client/cal-wrapper
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/mm-audio/graphite-client/ipc
LOCAL_CFLAGS     := $(gcs-def)

LOCAL_SHARED_LIBRARIES := libgcs-osal \
                          libgcs-calwrapper \
                          libgcs-ipc \
                          liblog

LOCAL_SRC_FILES        := src/gcs.c

LOCAL_MODULE               := libgcs
LOCAL_MODULE_OWNER         := qti
LOCAL_MODULE_TAGS          := optional
LOCAL_PROPRIETARY_MODULE   := true

LOCAL_COPY_HEADERS_TO   := mm-audio/graphite-client/gcs
LOCAL_COPY_HEADERS      := inc/gcs_api.h \
                           inc/graphite_common.h

include $(BUILD_SHARED_LIBRARY)

endif # BUILD_TINY_ANDROID
endif # is-board-platform-in-list
