VIDEO_DIR := $(call my-dir)
LOCAL_PATH := $(VIDEO_DIR)
include $(CLEAR_VARS)

ifeq ($(call is-board-platform-in-list, $(MSM_VIDC_TARGET_LIST)),true)
include $(VIDEO_DIR)/omx_vpp/Android.mk
endif
