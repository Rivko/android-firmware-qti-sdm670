ifneq ($(call is-board-platform-in-list,msmnile),true)
include $(call all-subdir-makefiles)
endif
