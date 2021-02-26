# Disable include all subdir makefiles, If any one module can normal compile, please include module makefile in here.
#include $(call all-subdir-makefiles)
ifeq ($(TARGET_HAS_LOW_RAM),true)
TARGET_USES_QTIC_CARRIER_DISABLE := true
endif

BASE_PATH := $(call my-dir)
include $(BASE_PATH)/WapPush/Android.mk
include $(BASE_PATH)/CameraExtension/Android.mk
include $(BASE_PATH)/CarrierCacheService/Android.mk
include $(BASE_PATH)/CarrierConfigure/Android.mk
include $(BASE_PATH)/CarrierLoadService/Android.mk
include $(BASE_PATH)/PowerOffAlarm/Android.mk
