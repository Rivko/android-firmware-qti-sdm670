ifneq ($(TARGET_NO_TELEPHONY), true)
include $(call all-subdir-makefiles)
endif # TARGET_NO_TELEPHONY
