#/******************************************************************************
#*@file Android.mk
#*brief Rules for compiling all code related to garden-app
#*******************************************************************************/
ifneq ($(BUILD_TINY_ANDROID),true)

include $(call all-subdir-makefiles)

endif # BUILD_TINY_ANDROID
