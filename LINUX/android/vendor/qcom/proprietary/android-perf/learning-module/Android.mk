ifeq ($(TARGET_USES_LM),true)
    # LM_PATH points to learning-module folder and used in all child Android.mk files
    LM_PATH:= $(call my-dir)
    ANDROID_PERF_SHIP_PATH:= $(LM_PATH)/..
    MPCTL_PATH:= $(ANDROID_PERF_SHIP_PATH)/mp-ctl
    PERFHAL_PATH:= $(ANDROID_PERF_SHIP_PATH)/perf-hal
    LM_INC_PATH:= $(LM_PATH)/framework/inc
    LM_INCLUDES:= $(LM_INC_PATH)/core \
        $(LM_INC_PATH)/metameters \
        $(LM_INC_PATH)/utils \
        $(LM_INC_PATH)/database \
        external/tinyxml2 \
        external/sqlite/dist

    include $(call all-subdir-makefiles)

    # Call all feature Makefiles
    include $(call all-makefiles-under, $(LM_PATH)/features)
endif
