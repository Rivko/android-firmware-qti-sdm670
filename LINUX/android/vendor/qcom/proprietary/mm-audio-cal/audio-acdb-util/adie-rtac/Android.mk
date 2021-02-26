ifeq ($(call is-board-platform-in-list,msm8909 msm8996 msm8937 msm8953 msm8998 apq8098_latv sdm660 sdm845 sdm710 qcs605),true)

ifneq ($(BUILD_TINY_ANDROID),true)

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# ---------------------------------------------------------------------------------
#                 Common definitons
# ---------------------------------------------------------------------------------

libadiertac-def := -g -O3
libadiertac-def += -D_ANDROID_
libadiertac-def += -D_ENABLE_QC_MSG_LOG_
# ---------------------------------------------------------------------------------
#             Make the Shared library (libadiertac)
# ---------------------------------------------------------------------------------

libadiertac-inc     := $(LOCAL_PATH)/inc
libadiertac-inc     += $(LOCAL_PATH)/src

LOCAL_MODULE            := libadiertac
LOCAL_MODULE_TAGS       := optional
LOCAL_CFLAGS            := $(libadiertac-def)
LOCAL_C_INCLUDES        := $(libadiertac-inc)
LOCAL_C_INCLUDES        += $(TARGET_OUT_HEADERS)/common/inc
LOCAL_C_INCLUDES        += $(TARGET_OUT_HEADERS)/mm-audio/audcal
LOCAL_C_INCLUDES        += $(TARGET_OUT_HEADERS)/mm-audio/audio-acdb-util
LOCAL_C_INCLUDES        += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_C_INCLUDES 	+= $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/techpack/audio/include

ifeq ($(call is-board-platform,msm8610),true)
LOCAL_CFLAGS  += -DCDC_REG_DIG_BASE_READ=0x400
LOCAL_CFLAGS  += -DCDC_REG_DIG_OFFSET=0x200
else
LOCAL_CFLAGS  += -DCDC_REG_DIG_BASE_READ=0x200
LOCAL_CFLAGS  += -DCDC_REG_DIG_OFFSET=0x000
endif
LOCAL_CFLAGS  += -DCDC_REG_DIG_BASE_WRITE=0x200

LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
ifneq ($(filter sdm710 qcs605,$(TARGET_BOARD_PLATFORM)),)
  LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/vendor/qcom/opensource/audio-kernel/include
  LOCAL_ADDITIONAL_DEPENDENCIES += $(BOARD_VENDOR_KERNEL_MODULES)
endif

LOCAL_PRELINK_MODULE    := false
LOCAL_SHARED_LIBRARIES  := libcutils libutils liblog libaudcal
LOCAL_COPY_HEADERS_TO   := mm-audio/audio-acdb-util

LOCAL_COPY_HEADERS      := inc/adie-rtac.h

LOCAL_SRC_FILES         := src/adie-rtac.c




LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)

endif
endif # is-board-platform-in-list

# ---------------------------------------------------------------------------------
#                     END
# ---------------------------------------------------------------------------------
