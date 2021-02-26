ifeq ($(call is-board-platform-in-list,msm8974 msm8960 msm8660 msm8226 msm8610 msm8916 msm8916_32 msm8916_32_k64 msm8916_64 apq8084 msm8939 msm8994 msm8992 msm8952 msm8937 msm8953 msm8909 msm8996 msm8998 sdm660),true)

ifneq (true,$(TARGET_BUILD_PDK))
  include $(call all-subdir-makefiles)
endif

endif # end filter
