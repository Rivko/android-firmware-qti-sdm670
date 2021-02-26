ifeq ($(call is-vendor-board-platform,QCOM),true)
ifneq ($(call is-board-platform,copper),true)

WFD_HDCP40_PLATFORM_LIST := sdm845 sdm670

include $(call all-subdir-makefiles)

endif
endif # TARGET_USES_WFD
