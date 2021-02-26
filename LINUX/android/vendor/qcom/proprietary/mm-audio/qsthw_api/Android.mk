ifeq ($(strip $(BOARD_SUPPORTS_SOUND_TRIGGER)),true)

#compile qti wrapper library
ifeq ($(strip $(BOARD_SUPPORTS_QSTHW_API)),true)
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
        qsthw_api.c

LOCAL_C_INCLUDES := \
        external/tinyalsa/include \
        $(TARGET_OUT_HEADERS)/mm-audio/sound_trigger

LOCAL_SHARED_LIBRARIES := \
        liblog \
        libcutils \
        libdl \
        libtinyalsa \
        libhardware

LOCAL_MODULE := libqtisoundtriggerhw
LOCAL_MODULE_OWNER := qti
LOCAL_MODULE_TAGS := optional
LOCAL_MULTILIB := both
LOCAL_PROPRIETARY_MODULE := true

LOCAL_COPY_HEADERS_TO   := mm-audio/qsthw_api
LOCAL_COPY_HEADERS      := qsthw_api.h
LOCAL_COPY_HEADERS      += qsthw_defs.h

include $(BUILD_SHARED_LIBRARY)

#compile qti sound trigger test app
include $(CLEAR_VARS)
LOCAL_SRC_FILES := qsthw_test.c
LOCAL_C_INCLUDES += \
        $(TARGET_OUT_HEADERS)/mm-audio/qsthw_api

LOCAL_SHARED_LIBRARIES := \
        libcutils \
        libqtisoundtriggerhw

LOCAL_MODULE := qti_sound_trigger_test
LOCAL_MODULE_OWNER := qti
LOCAL_MODULE_TAGS := optional
LOCAL_32_BIT_ONLY := true
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_EXECUTABLE)

endif #BOARD_SUPPORTS_QSTHW_API
endif #BOARD_SUPPORTS_SOUND_TRIGGER
