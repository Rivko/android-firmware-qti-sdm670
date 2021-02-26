ifneq ($(TARGET_SUPPORTS_ANDROID_WEAR),true)
include $(call all-subdir-makefiles)
endif
