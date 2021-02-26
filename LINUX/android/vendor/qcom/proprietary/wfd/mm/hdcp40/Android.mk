ifeq ($(call is-vendor-board-platform,QCOM),true)
ifneq ($(call is-board-platform,copper),true)
ifeq ($(call is-board-platform-in-list,$(WFD_HDCP40_PLATFORM_LIST)),true)

include $(call all-subdir-makefiles)

endif
endif
endif
