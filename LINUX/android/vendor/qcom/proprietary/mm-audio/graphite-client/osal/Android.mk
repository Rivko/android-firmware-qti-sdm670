ifeq ($(call is-board-platform-in-list, msm8998 apq8098_latv sdm660 sdm845 sdm710 msm8953 qcs605),true)

ifneq ($(BUILD_TINY_ANDROID),true)

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

#----------------------------------------------------------------------------
#                 Common definitons
#----------------------------------------------------------------------------

osal-def += -D_ANDROID_

#----------------------------------------------------------------------------
#             Make the Shared library (libgcs-osal)
#----------------------------------------------------------------------------

LOCAL_C_INCLUDES := $(LOCAL_PATH)/inc
LOCAL_CFLAGS := $(osal-def)

LOCAL_SRC_FILES := src/osal_lock.c \
                   src/osal_mem.c \
                   src/osal_cond.c \
                   src/osal_thread.c \
                   src/osal_dev.c

LOCAL_SHARED_LIBRARIES := liblog

LOCAL_MODULE := libgcs-osal
LOCAL_MODULE_OWNER := qti
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true

LOCAL_COPY_HEADERS_TO := mm-audio/graphite-client/osal
LOCAL_COPY_HEADERS := inc/osal_types.h \
                      inc/osal_log.h \
                      inc/osal_lock.h \
                      inc/osal_list.h \
                      inc/osal_mem.h \
                      inc/osal_cond.h \
                      inc/osal_thread.h \
                      inc/osal_dev.h \
                      inc/osal_pack_start.h \
                      inc/osal_pack_end.h \
                      inc/osal_errorcodes.h

include $(BUILD_SHARED_LIBRARY)

endif # BUILD_TINY_ANDROID
endif # is-board-platform-in-list
