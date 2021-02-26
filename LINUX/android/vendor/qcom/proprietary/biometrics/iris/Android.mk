ifeq ($(call is-vendor-board-platform,QCOM),true)
ifeq ($(call is-board-platform-in-list,msm8998 sdm660),true)

include $(call all-subdir-makefiles)

endif
endif

