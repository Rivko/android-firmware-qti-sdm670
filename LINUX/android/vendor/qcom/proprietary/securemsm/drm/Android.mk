ifeq ($(call is-board-platform-in-list,msm8960 msm8974 msm8660 msm7627a msm7630_surf msm7630_fusion msm8226 msm8610 msm8916 msm8916_32 msm8916_32_k64 msm8916_64 apq8084 msm8939 msm8994 msm8909 msm8996 msm8992 msm8952 msm8937 msm8953 msm8998 apq8098_latv sdm660 sdm845 sdm710 qcs605),true)

include $(call all-subdir-makefiles)

endif
