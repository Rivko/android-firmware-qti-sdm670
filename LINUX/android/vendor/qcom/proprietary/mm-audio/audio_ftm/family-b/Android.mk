ifeq ($(MM_AUDIO_ENABLED_FTM),true)
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

ifeq ($(call is-board-platform-in-list,msm8960 msm8974 msm8226 msm8610 apq8084 msm8994 msm8992 msm8916 msm8909 msm8996 msm8952 msm8937 thorium msm8953 msmgold msm8998 apq8098_latv sdm660 sdm845 sdm710 qcs605),true)

mm-audio-ftm-def += -g -O2
mm-audio-ftm-def += -DQC_MODIFIED
mm-audio-ftm-def += -D_ANDROID_
mm-audio-ftm-def += -DQCT_CFLAGS
mm-audio-ftm-def += -DQCT_CPPFLAGS
mm-audio-ftm-def += -DVERBOSE
mm-audio-ftm-def += -D_DEBUG
mm-audio-ftm-def += -DMSM8960_ALSA

include $(CLEAR_VARS)

mm-audio-ftm-inc := $(LOCAL_PATH)/inc
mm-audio-ftm-inc += $(TARGET_OUT_HEADERS)/common/inc
mm-audio-ftm-inc += $(TARGET_OUT_HEADERS)/diag/include

mm-audio-ftm-inc += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
mm-audio-ftm-inc += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/techpack/audio/include
mm-audio-ftm-inc += external/tinyalsa/include
LOCAL_ADDITIONAL_DEPENDENCIES  := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
ifneq ($(filter sdm710 qcs605,$(TARGET_BOARD_PLATFORM)),)
  mm-audio-ftm-inc += $(TARGET_OUT_INTERMEDIATES)/vendor/qcom/opensource/audio-kernel/include
  LOCAL_ADDITIONAL_DEPENDENCIES += $(BOARD_VENDOR_KERNEL_MODULES)
endif

LOCAL_MODULE            := mm-audio-ftm
LOCAL_32_BIT_ONLY       := true
LOCAL_MODULE_TAGS       := optional
LOCAL_MODULE_OWNER      := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_CFLAGS            := $(mm-audio-ftm-def)
LOCAL_C_INCLUDES        := $(mm-audio-ftm-inc)
LOCAL_SRC_FILES         := src/DALSYS_common.c \
			 src/audio_ftm_afe_loopback.c \
			 src/audio_ftm_driver_fwk.c \
			 src/audio_ftm_dtmf_basic_op.c \
			 src/audio_ftm_dtmf_tone_gen.c \
			 src/audio_ftm_hw_drv-8960.c \
			 src/audio_ftm_pcm_loopback.c \
			 src/audio_ftm_pcm_record.c \
			 src/audio_ftm_tone_play.c \
			 src/audio_ftm_util_fifo.c \
			 src/ftm_audio_dispatch-8960.c \
			 src/ftm_audio_main.c \
			 src/audio_ftm_diag_mem.c \
			 src/audio_ftm_pcm_play.c \
			 src/audio_ftm_ext_loopback.c \
			 src/fft.c \

LOCAL_SHARED_LIBRARIES  := libdiag libcutils libtinyalsa libdl liblog

include $(BUILD_EXECUTABLE)

endif

ifeq ($(call is-board-platform,msm8660),true)

# ---------------------------------------------------------------------------------
#                               Common definitons
# ---------------------------------------------------------------------------------

mm-audio-ftm-def += -g -O2
mm-audio-ftm-def += -DQC_MODIFIED
mm-audio-ftm-def += -D_ANDROID_
mm-audio-ftm-def += -DQCT_CFLAGS
mm-audio-ftm-def += -DQCT_CPPFLAGS
mm-audio-ftm-def += -DVERBOSE
mm-audio-ftm-def += -D_DEBUG

# ---------------------------------------------------------------------------------
#                       Make the FTM main (mm-audio-ftm)
# ---------------------------------------------------------------------------------

include $(CLEAR_VARS)

mm-audio-ftm-inc := $(LOCAL_PATH)/inc
mm-audio-ftm-inc += $(TARGET_OUT_HEADERS)/common/inc
mm-audio-ftm-inc += $(TARGET_OUT_HEADERS)/diag/include
mm-audio-ftm-inc += $(AUDIO_ROOT)/audio-alsa/inc
mm-audio-ftm-inc += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
mm-audio-ftm-inc += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/techpack/audio/include
mm-audio-ftm-inc += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include/linux
mm-audio-ftm-inc += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/arch/arm/include
LOCAL_ADDITIONAL_DEPENDENCIES  := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
ifneq ($(filter sdm710 qcs605,$(TARGET_BOARD_PLATFORM)),)
  mm-audio-ftm-inc += $(TARGET_OUT_INTERMEDIATES)/vendor/qcom/opensource/audio-kernel/include
  LOCAL_ADDITIONAL_DEPENDENCIES += $(BOARD_VENDOR_KERNEL_MODULES)
endif

LOCAL_MODULE            := mm-audio-ftm
LOCAL_MODULE_TAGS       := optional
LOCAL_MODULE_OWNER       := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_CFLAGS            := $(mm-audio-ftm-def)
LOCAL_C_INCLUDES        := $(mm-audio-ftm-inc)
LOCAL_SRC_FILES         := src/DALSYS_common.c \
			 src/audio_ftm_afe_loopback.c \
			 src/audio_ftm_driver_fwk.c \
			 src/audio_ftm_dtmf_basic_op.c \
			 src/audio_ftm_dtmf_tone_gen.c \
			 src/audio_ftm_hw_drv.c \
			 src/audio_ftm_pcm_loopback.c \
			 src/audio_ftm_pcm_record.c \
			 src/audio_ftm_tone_play.c \
			 src/audio_ftm_util_fifo.c \
			 src/ftm_audio_dispatch.c \
			 src/ftm_audio_main.c \
			 src/audio_ftm_diag_mem.c \
			 src/fft.c \

LOCAL_SHARED_LIBRARIES  := libdiag libcutils liblog libtinyalsa libcommondefs libandroid_runtime

include $(BUILD_EXECUTABLE)

endif


ifeq ($(call is-board-platform,msm8226),true)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/8x26/ftm_test_config
include $(BUILD_PREBUILT)

endif


ifeq ($(call is-board-platform,msm8610),true)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/8x10/ftm_test_config
include $(BUILD_PREBUILT)

endif

ifeq ($(call is-board-platform,msm8974),true)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/8974/ftm_test_config
include $(BUILD_PREBUILT)

endif

ifeq ($(call is-board-platform,apq8084),true)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/8084/ftm_test_config
include $(BUILD_PREBUILT)

endif

ifeq ($(call is-board-platform,msm8916),true)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/8x16/ftm_test_config
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config_mtp
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/8x16/ftm_test_config_mtp
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config_wcd9306
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/8x16/ftm_test_config_wcd9306
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config_msm8939-snd-card-skul
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/8x16/ftm_test_config_msm8939-snd-card-skul
include $(BUILD_PREBUILT)

endif

ifeq ($(call is-board-platform,msm8952),true)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/8952/ftm_test_config
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config_qrd
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/8952/ftm_test_config_qrd
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config_wcd9330
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/8952/ftm_test_config_wcd9330
include $(BUILD_PREBUILT)

endif

ifeq ($(call is-board-platform,msm8994 msm8992),true)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/8994/ftm_test_config
include $(BUILD_PREBUILT)

endif

ifeq ($(call is-board-platform,msm8909),true)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/8909/ftm_test_config
include $(BUILD_PREBUILT)

endif

ifeq ($(call is-board-platform,msm8996),true)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/8996/ftm_test_config
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config_msm8996-dtp-tasha-snd-card
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/8996/ftm_test_config_msm8996-dtp-tasha-snd-card
include $(BUILD_PREBUILT)

endif

ifeq ($(call is-board-platform,msm8937),true)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/8937/ftm_test_config
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config_wcd9335
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/8937/ftm_test_config_wcd9335
include $(BUILD_PREBUILT)

endif

ifeq ($(call is-board-platform,msm8953),true)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/8953/ftm_test_config
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config_mtp
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/8953/ftm_test_config_mtp
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config_wcd9335
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/8953/ftm_test_config_wcd9335
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config_msm8953-sku3-tasha-snd-card
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/8953/ftm_test_config_msm8953-sku3-tasha-snd-card
include $(BUILD_PREBUILT)

endif

ifeq ($(call is-board-platform-in-list,msm8998 apq8098_latv),true)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/8998/ftm_test_config
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config_msm8998-tasha-snd-card
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/8998/ftm_test_config_msm8998-tasha-snd-card
include $(BUILD_PREBUILT)
endif

ifeq ($(call is-board-platform,sdm660),true)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/sdm660/ftm_test_config
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config_sdm660-tavil-snd-card
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/sdm660/ftm_test_config_sdm660-tavil-snd-card
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config_sdm660-tasha-snd-card
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/sdm660/ftm_test_config_sdm660-tasha-snd-card
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config_sdm660-tasha-skus-snd-card
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/sdm660/ftm_test_config_sdm660-tasha-skus-snd-card
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config_sdm660-snd-card-skush
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/sdm660/ftm_test_config_sdm660-snd-card-skush
include $(BUILD_PREBUILT)
endif

ifeq ($(call is-board-platform,sdm845),true)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/msmskunk/ftm_test_config
include $(BUILD_PREBUILT)
endif

ifeq ($(call is-board-platform,sdm710),true)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/sdm710/ftm_test_config
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config_sdm710-tavil-snd-card
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/sdm710/ftm_test_config_sdm710-tavil-snd-card
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config_sdm710-tasha-snd-card
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/sdm710/ftm_test_config_sdm710-tasha-snd-card
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := ftm_test_config_sdm670-skuw-snd-card
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/
LOCAL_SRC_FILES    := config/sdm710/ftm_test_config_sdm670-skuw-snd-card
include $(BUILD_PREBUILT)
endif
endif
# ---------------------------------------------------------------------------------
#                                       END
# ---------------------------------------------------------------------------------

