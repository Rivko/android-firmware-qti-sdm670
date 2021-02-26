ifeq ($(call is-board-platform-in-list,msm8996 msm8937 msm8953 msm8998 apq8098_latv sdm660 sdm845 sdm710 qcs605),true)
ifneq ($(BUILD_TINY_ANDROID),true)

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# ---------------------------------------------------------------------------------
#                 Common definitons
# ---------------------------------------------------------------------------------

libOmxAmrwbplusDec-def := -g -O3
libOmxAmrwbplusDec-def += -DQC_MODIFIED
libOmxAmrwbplusDec-def += -D_ANDROID_
libOmxAmrwbplusDec-def += -D_ENABLE_QC_MSG_LOG_
libOmxAmrwbplusDec-def += -DVERBOSE
libOmxAmrwbplusDec-def += -D_DEBUG
libOmxAmrwbplusDec-def += -DAUDIOV2
libOmxAmrwbplusDec-def += -Wconversion
ifeq ($(call is-board-platform-in-list,msm8996 msm8937 msm8953 msm8998 apq8098_latv sdm660 sdm845 sdm710 qcs605),true)
libOmxAmrwbplusDec-def += -DQCOM_AUDIO_USE_SYSTEM_HEAP_ID
endif
# ---------------------------------------------------------------------------------
#             Make the Shared library (libOmxAmrwbplusDec)
# ---------------------------------------------------------------------------------

include $(CLEAR_VARS)

libOmxAmrwbplus-inc        := $(LOCAL_PATH)/inc
libOmxAmrwbplus-inc        += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
libOmxAmrwbplus-inc	   += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/techpack/audio/include
libOmxAmrwbplus-inc        += $(TARGET_OUT_HEADERS)/mm-core/omxcore
libOmxAmrwbplus-inc        += $(TARGET_OUT_HEADERS)/common/inc

LOCAL_ADDITIONAL_DEPENDENCIES  := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
ifneq ($(filter sdm710 qcs605,$(TARGET_BOARD_PLATFORM)),)
  libOmxAmrwbplus-inc += $(TARGET_OUT_INTERMEDIATES)/vendor/qcom/opensource/audio-kernel/include
  LOCAL_ADDITIONAL_DEPENDENCIES += $(BOARD_VENDOR_KERNEL_MODULES)
endif

LOCAL_MODULE            := libOmxAmrwbplusDec
LOCAL_MODULE_TAGS       := optional
LOCAL_CFLAGS            := $(libOmxAmrwbplusDec-def)
LOCAL_C_INCLUDES        := $(libOmxAmrwbplus-inc)
LOCAL_PRELINK_MODULE    := false
LOCAL_SHARED_LIBRARIES  := libutils liblog

LOCAL_SRC_FILES         := src/adec_svr.c
LOCAL_SRC_FILES         += src/omx_amrwbplus_adec.cpp

LOCAL_MODULE_OWNER := qcom
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)

# ---------------------------------------------------------------------------------
#             Make the apps-test (mm-adec-omxamrwbplus-test)
# ---------------------------------------------------------------------------------

include $(CLEAR_VARS)

mm-amrwbplus-dec-test-inc     := $(LOCAL_PATH)/inc
mm-amrwbplus-dec-test-inc     += $(LOCAL_PATH)/test
mm-amrwbplus-dec-test-inc     += $(TARGET_OUT_HEADERS)/mm-audio/audio-alsa
mm-amrwbplus-dec-test-inc     += $(TARGET_OUT_HEADERS)/mm-core/omxcore
mm-amrwbplus-dec-test-inc     += $(TARGET_OUT_HEADERS)/common/inc
mm-amrwbplus-dec-test-inc     += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
mm-amrwbplus-dec-test-inc     += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/techpack/audio/include
LOCAL_ADDITIONAL_DEPENDENCIES  := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
ifneq ($(filter sdm710 qcs605,$(TARGET_BOARD_PLATFORM)),)
  mm-amrwbplus-dec-test-inc += $(TARGET_OUT_INTERMEDIATES)/vendor/qcom/opensource/audio-kernel/include
  LOCAL_ADDITIONAL_DEPENDENCIES += $(BOARD_VENDOR_KERNEL_MODULES)
endif

LOCAL_MODULE            := mm-adec-omxamrwbplus-test
LOCAL_MODULE_TAGS       := optional
LOCAL_CFLAGS            := $(libOmxAmrwbplusDec-def)
LOCAL_C_INCLUDES        := $(mm-amrwbplus-dec-test-inc)
LOCAL_PRELINK_MODULE    := false
LOCAL_SHARED_LIBRARIES  := libmm-omxcore
LOCAL_SHARED_LIBRARIES  += libOmxAmrwbplusDec
LOCAL_SHARED_LIBRARIES  += libaudioalsa

LOCAL_SRC_FILES         := test/omx_amrwbplus_dec_test.c

include $(BUILD_EXECUTABLE)

endif #BUILD_TINY_ANDROID
endif #TARGET_BOARD_PLATFORM
# ---------------------------------------------------------------------------------
#                     END
# ---------------------------------------------------------------------------------

