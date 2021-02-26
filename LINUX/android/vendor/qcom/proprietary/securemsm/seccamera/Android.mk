ifeq ($(call is-board-platform-in-list,msm8998 sdm660 sdm845 sdm710),true)

include $(call all-subdir-makefiles)

endif # end filter
