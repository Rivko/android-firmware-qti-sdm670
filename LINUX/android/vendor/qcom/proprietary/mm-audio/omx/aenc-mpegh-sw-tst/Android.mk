ifneq ($(filter qcs605,$(TARGET_BOARD_PLATFORM)),)

LOCAL_PATH := $(call my-dir)

OmxMpeghEncSwTest_Inc  := $(TARGET_OUT_HEADERS)/mpegh-enc
OmxMpeghEncSwTest_Inc  += $(TARGET_OUT_HEADERS)/mm-core/omxcore

OmxMpeghEncSwTest_flag := -O0 -g -Wall -Werror -Wno-sign-compare -Wno-missing-field-initializers

include $(CLEAR_VARS)

LOCAL_CFLAGS      += $(OmxMpeghEncSwTest_flag) -Wno-strlcpy-strlcat-size
LOCAL_CFLAGS      += -DPCM24_SUPPORT -DPCM16_SUPPORT
LOCAL_C_INCLUDES  := $(OmxMpeghEncSwTest_Inc)
LOCAL_MODULE      := mm-aenc-omxmpegh-sw-test
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES   := tst/omx_mpegh_enc_test.cpp
LOCAL_SRC_FILES   += $(patsubst $(LOCAL_PATH)/%, %, $(wildcard $(LOCAL_PATH)/tst/*.c))
LOCAL_SHARED_LIBRARIES   := libOmxMpeghEncSw
LOCAL_SHARED_LIBRARIES   += libmm-omxcore liblog
LOCAL_MODULE_OWNER       := qti
LOCAL_32_BIT_ONLY        := true
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_EXECUTABLE)
endif
