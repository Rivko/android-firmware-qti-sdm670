ifeq ($(BOARD_HAVE_BLUETOOTH_QCOM),true)

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
ifeq ($(TARGET_HAS_SPLIT_A2DP_FEATURE),true)
include $(LOCAL_PATH)/1.0/default/Android.mk
endif # TARGET_USES_AOSP

endif # BOARD_HAVE_BLUETOOTH_QCOM
