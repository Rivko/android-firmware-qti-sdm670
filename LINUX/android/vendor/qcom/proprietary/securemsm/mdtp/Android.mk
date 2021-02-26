ifeq ($(call is-board-platform-in-list,msm8974 msm8960 msm8660 msm8226 msm8610 msm8916 msm8916_32 msm8916_32_k64 msm8916_64 apq8084 msm8939 msm8994 msm8909 msm8996 msm8992 msm8952 msm8937 titanium_64 titanium_32 titanium msm8953 msm8953_32 msm8953_64 msm8998 apq8098_latv msm8998 sdm660),true)

include $(call all-subdir-makefiles)

endif # end filter
