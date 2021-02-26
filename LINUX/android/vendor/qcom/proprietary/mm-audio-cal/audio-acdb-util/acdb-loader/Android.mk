ifeq ($(call is-board-platform-in-list,msm8909 msm8996 msm8937 msm8953 msm8998 apq8098_latv sdm660 sdm845 sdm710 qcs605),true)

ifneq ($(BUILD_TINY_ANDROID),true)

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# ---------------------------------------------------------------------------------
#                 Common definitons
# ---------------------------------------------------------------------------------

libacdbloader-def := -g -O3
libacdbloader-def += -D_ANDROID_
libacdbloader-def += -D_ENABLE_QC_MSG_LOG_
libacdbloader-def += -D ACDB_BIN_PATH=\"/vendor/etc/acdbdata\"
libacdbloader-def += -D ETC_ROOT_PATH=\"/vendor/etc\"
libacdbloader-def += -D DEFAULT_BOARD=\"MTP\"

libacdbloader-def += -D MIXER_CONTROL_FILE_PATH=\"/vendor/etc/audio_tuning_mixer.txt\"
libacdbloader-def += -D TAVIL_MIXER_CONTROL_FILE_PATH=\"/vendor/etc/audio_tuning_mixer_tavil.txt\"
libacdbloader-def += -D TASHA_MIXER_CONTROL_FILE_PATH=\"/vendor/etc/audio_tuning_mixer_tasha.txt\"
libacdbloader-def += -D_TINY_ALSA_LIB_

ifeq ($(call is-board-platform-in-list,msm8996 msm8937 msm8953 msm8998 apq8098_latv sdm660 sdm845 sdm710 qcs605),true)
libacdbloader-def += -D AUDIO_USE_SYSTEM_HEAP_ID
endif
# ---------------------------------------------------------------------------------
#             Make the Shared library (libaudcalctrl)
# ---------------------------------------------------------------------------------

libacdbloader-inc     := $(LOCAL_PATH)/inc
libacdbloader-inc     += $(LOCAL_PATH)/src
libacdbloader-inc     += $(LOCAL_PATH)/../acdb-mcs/inc
libacdbloader-inc     += $(LOCAL_PATH)/../acdb-mcs/src

LOCAL_MODULE            := libacdbloader
LOCAL_MODULE_TAGS       := optional
LOCAL_CFLAGS            := $(libacdbloader-def)
LOCAL_C_INCLUDES        := $(libacdbloader-inc)
LOCAL_C_INCLUDES        += $(TARGET_OUT_HEADERS)/common/inc
LOCAL_C_INCLUDES        += $(TARGET_OUT_HEADERS)/mm-audio/audcal
LOCAL_C_INCLUDES        += $(TARGET_OUT_HEADERS)/mm-audio/audio-acdb-util
LOCAL_C_INCLUDES        += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_C_INCLUDES 	+= $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/techpack/audio/include

LOCAL_C_INCLUDES        += external/tinyalsa/include

LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
ifneq ($(filter sdm710 qcs605,$(TARGET_BOARD_PLATFORM)),)
  LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/vendor/qcom/opensource/audio-kernel/include
  LOCAL_ADDITIONAL_DEPENDENCIES += $(BOARD_VENDOR_KERNEL_MODULES)
endif

LOCAL_PRELINK_MODULE    := false
LOCAL_SHARED_LIBRARIES  := libcutils libutils liblog libaudcal libtinyalsa
LOCAL_COPY_HEADERS_TO   := mm-audio/audio-acdb-util

ifeq ($(call is-board-platform-in-list,msm8909 msm8996 msm8937 msm8953 msm8998 apq8098_latv sdm660 sdm845 sdm710 qcs605),true)
LOCAL_SHARED_LIBRARIES  += libacdbrtac
LOCAL_SHARED_LIBRARIES  += libadiertac
LOCAL_SHARED_LIBRARIES  += libacdb-fts
LOCAL_COPY_HEADERS      := inc/8974/acdb-loader.h
LOCAL_COPY_HEADERS      += inc/acdb-id-mapper.h
LOCAL_COPY_HEADERS      += inc/acdb-anc-tabla.h
LOCAL_COPY_HEADERS      += inc/acdb-anc-taiko.h
LOCAL_COPY_HEADERS      += inc/anc_map_api.h
LOCAL_COPY_HEADERS      += inc/anc_bitmasks.h
LOCAL_COPY_HEADERS      += inc/wcd9335_anc_registers.h
LOCAL_COPY_HEADERS      += inc/vbat_map_api.h
LOCAL_COPY_HEADERS      += ../acdb-mcs/inc/acdb-mcs.h
endif

LOCAL_COPY_HEADERS      += inc/acdb-anc-general.h

ifeq ($(call is-board-platform-in-list,msm8909 msm8996 msm8937 msm8953 msm8998 apq8098_latv sdm660 sdm845 sdm710 qcs605),true)
LOCAL_SRC_FILES         := src/acdb-loader.c \
				src/anc_map.c \
				src/vbat_map.c
endif

#MCS
LOCAL_SRC_FILES         += ../acdb-mcs/src/acdb-mcs.c \
				../acdb-mcs/src/mcs-route-ctrl.c


LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)

endif
endif # is-board-platform-in-list

# ---------------------------------------------------------------------------------
#                     END
# ---------------------------------------------------------------------------------
