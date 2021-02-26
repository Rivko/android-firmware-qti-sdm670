ifneq ($(call is-platform-sdk-version-at-least,26),true)
ifeq ($(strip $(TARGET_USES_QPAY)),true)

include $(call all-subdir-makefiles)

endif
endif # end filter
