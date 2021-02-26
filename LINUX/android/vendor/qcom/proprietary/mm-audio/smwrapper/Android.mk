ifeq ($(strip $(BOARD_SUPPORTS_SOUND_TRIGGER)),true)
    FEATURE_ENABLE := true
endif

ifeq ($(strip $(BOARD_SUPPORTS_SOUND_TRIGGER_HAL)),true)
    FEATURE_ENABLE := true
endif

ifeq ($(FEATURE_ENABLE), true)
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := smwrapper.c

ifeq ($(strip $(BOARD_SUPPORTS_QSTHW_API)),true)
    LOCAL_CFLAGS += -DSM_EXTN_ENABLED
    LOCAL_C_INCLUDES += \
            $(TARGET_OUT_HEADERS)/mm-audio/qsthw_api
endif

LOCAL_SHARED_LIBRARIES := \
        liblog \
        libcutils

LOCAL_MODULE := libsmwrapper
LOCAL_MODULE_OWNER := qti
LOCAL_MODULE_TAGS := optional
LOCAL_32_BIT_ONLY := true
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)

endif
