CUR_DIR := $(call my-dir)

ifeq ($(call is-board-platform-in-list, $(MSM_VIDC_TARGET_LIST)),true)

include $(CUR_DIR)/common.mk
include $(call all-subdir-makefiles)

endif
