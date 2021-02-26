ifneq ($(strip $(USE_CAMERA_STUB)),true)

ifneq ($(CAMX_CHICDK_PATH),)
LOCAL_PATH := $(CAMX_CHICDK_PATH)/vendor
else
LOCAL_PATH := $(call my-dir)
endif

# INclude this for definintions
include $(CAMX_CHICDK_PATH)/vendor/common.mk

include $(CAMX_VENDOR_PATH)/chioverride/default/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/node/chiutils/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/node/depth/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/node/gpu/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/node/ldcwarp/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/node/imagewarp/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/node/memcpy/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/node/split/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/node/shdr/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/node/yuvshdr/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/node/defog/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/node/dummysat/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/node/dummyrtb/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/node/remosaic/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/node/dummystich/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/sensor/default/imx318/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/sensor/default/max7366_eyetrack/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/sensor/default/imx334/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/sensor/default/imx577/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/sensor/default/imx362/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/sensor/default/imx386/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/sensor/default/imx576/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/sensor/default/ov12a10/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/sensor/default/max7366_6dof/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/sensor/default/ov13880/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/sensor/default/ov8856/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/sensor/default/ov7251/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/sensor/default/ov13855/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/sensor/default/s5k2l7/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/sensor/default/max7366_ov6211/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/sensor/default/max7366_ov9282/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/node/staticpdlibalgo/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/node/staticaecalgo/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/node/staticafalgo/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/node/staticawbalgo/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/node/awbwrapper/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/node/hvx/addconstant/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/node/hvx/binning/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/eeprom/sunny_gt24c64/build/android/Android.mk
include $(CAMX_VENDOR_PATH)/eeprom/truly_cmb433/build/android/Android.mk

endif #!USE_CAMERA_STUB
