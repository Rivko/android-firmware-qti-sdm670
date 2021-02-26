ifeq ($(strip $(BOARD_HAS_QCOM_WIGIG)),true)
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := ftmtest
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true

LOCAL_CFLAGS := -Wall -Werror -Wno-unused-parameter

LOCAL_C_INCLUDES += \
        external/wpa_supplicant_8/src/drivers \
        external/wpa_supplicant_8/src/common \
        $(TARGET_OUT_HEADERS)/libaoa

LOCAL_SHARED_LIBRARIES := \
        libnl \
        libaoa \

LOCAL_SRC_FILES := \
        ftmtest.c \

include $(BUILD_EXECUTABLE)

endif
