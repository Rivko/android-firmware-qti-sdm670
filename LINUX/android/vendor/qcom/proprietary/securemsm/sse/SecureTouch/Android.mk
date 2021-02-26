ifeq ($(call is-board-platform-in-list, msm8974 apq8084 msm8994 msm8916 msm8916_32 msm8916_32_k64 msm8916_64 msm8939 msm8996 msm8952 msm8937 msm8953 msm8998 apq8098_latv sdm660 sdm845 msm8909 sdm710),true)
  include $(call all-subdir-makefiles)
endif

