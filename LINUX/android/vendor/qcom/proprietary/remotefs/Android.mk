ifeq ($(call is-vendor-board-platform,QCOM),true)

REMOTEFS_DIR := $(call my-dir)

include $(REMOTEFS_DIR)/qmi_rmt_storage/Android.mk

endif #(is-vendor-board-platform,QCOM)
