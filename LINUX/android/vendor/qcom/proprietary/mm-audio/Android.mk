ifeq ($(call is-board-platform-in-list, msm8937 msm8996 msm8909),true)
    USE_SOUND_TRIGGER_LEGACY_HAL := true
endif

ifeq ($(call is-board-platform-in-list,msm8909 msm8996 msm8937 msm8953 msm8998 apq8098_latv sdm660 sdm845 sdm710 qcs605),true)
include $(call all-subdir-makefiles)
endif
