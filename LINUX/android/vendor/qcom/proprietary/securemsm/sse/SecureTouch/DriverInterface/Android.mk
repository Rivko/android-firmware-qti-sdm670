# =============================================================================
#
# Module: Secure Touch Driver Interface
#
# =============================================================================

LOCAL_PATH          := $(call my-dir)
SECUREMSM_PATH      := vendor/qcom/proprietary/securemsm

include $(CLEAR_VARS)

ifeq ($(call is-board-platform,msm8974),true)
  ST_TARGET_CFLAGS := -DST_TARGET_MSM8974
else ifeq ($(call is-board-platform,apq8084),true)
  ST_TARGET_CFLAGS := -DST_TARGET_APQ8084
else ifeq ($(call is-board-platform,msm8994),true)
  ST_TARGET_CFLAGS := -DST_TARGET_MSM8994
else ifeq ($(call is-board-platform,msm8916),true)
  ST_TARGET_CFLAGS := -DST_TARGET_MSM8916
else ifeq ($(call is-board-platform,msm8996),true)
  ST_TARGET_CFLAGS := -DST_TARGET_MSM8996
else ifeq ($(call is-board-platform,msm8952),true)
  ST_TARGET_CFLAGS := -DST_TARGET_MSM8952
else ifeq ($(call is-board-platform,msm8937),true)
  ST_TARGET_CFLAGS := -DST_TARGET_THORIUM
else ifeq ($(call is-board-platform,msm8953),true)
  ST_TARGET_CFLAGS := -DST_TARGET_MSM8953
else ifeq ($(call is-board-platform-in-list,msm8998 apq8098_latv),true)
  ST_TARGET_CFLAGS := -DST_TARGET_MSM8998
else ifeq ($(call is-board-platform,sdm660),true)
  ST_TARGET_CFLAGS := -DST_TARGET_SDM660
else ifeq ($(call is-board-platform,sdm845),true)
  ST_TARGET_CFLAGS := -DST_TARGET_SDM845
else ifeq ($(call is-board-platform,sdm710),true)
  ST_TARGET_CFLAGS := -DST_TARGET_SDM710
endif

LOCAL_MODULE        := libStDrvInt

LOCAL_MODULE_TAGS := optional

LOCAL_C_INCLUDES   := \
                    $(LOCAL_PATH)/include \
                    $(TARGET_OUT_HEADERS)/common/inc

LOCAL_CFLAGS        := $(ST_TARGET_CFLAGS) -g -fdiagnostics-show-option

LOCAL_SRC_FILES     := \
                    src/StDrvInt.c \

LOCAL_SHARED_LIBRARIES := liblog

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)
