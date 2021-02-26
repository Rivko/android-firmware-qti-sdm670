ifeq ($(strip $(TARGET_USES_QTIC)),true)
    include $(call all-subdir-makefiles)
else
    ifeq ($(strip $(TARGET_USES_QTIC_EXTENSION)),true)
        LOCAL_PATH := vendor/qcom/proprietary/qrdplus/Extension
        include $(LOCAL_PATH)/apps/CameraExtension/Android.mk
    endif
endif
