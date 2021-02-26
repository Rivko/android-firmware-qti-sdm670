LOCAL_PATH := $(call my-dir)
PREBUILT_DIR_PATH := $(LOCAL_PATH)

ifeq ($(call is-board-platform,qcs605),true)
  -include $(LOCAL_PATH)/target/product/qcs605/Android.mk
endif

ifeq ($(call is-board-platform,qcs605),true)
  -include $(LOCAL_PATH)/target/product/qcs605h/Android.mk
endif

-include $(sort $(wildcard $(PREBUILT_DIR_PATH)/*/*/Android.mk))
