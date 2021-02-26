IMS_DIR := $(call my-dir)
include $(CLEAR_VARS)
#ifeq ($(call is-board-platform-in-list,msm8953 msm8960 msm8974 msm8226 msm8994 apq8084 msm8916 msm8909 msm8952 msm8952_32 msm8952_64 msm8998 apq8098_latv msm8996 msm8996_gvmq msm8992 msm8937 titanium sdm660 sdm845 msmpeafowl sdm710 msmnile),true)
  ifeq ($(TARGET_USES_IMS),true)
ifeq ($(call is-board-platform-in-list,msm8909 msm8953 msm8937 msm8952 msm8976),true)
include $(IMS_DIR)/imscamera/Android.mk
endif
      include $(IMS_DIR)/vtext/Android.mk
      include $(IMS_DIR)/qmi/Android.mk
      include $(IMS_DIR)/rcs/Android.mk
  else
    include $(IMS_DIR)/qmi/Android.mk
  endif
#endif
