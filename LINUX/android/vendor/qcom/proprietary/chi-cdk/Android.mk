CAMX_CHICDK_PATH := $(call my-dir)
CAMX_CDK_PATH := $(CAMX_CHICDK_PATH)/cdk
CAMX_VENDOR_PATH := $(CAMX_CHICDK_PATH)/vendor

# Take backup of SDLLVM specific flag and version defs as other modules (adreno)
# also maintain their own version of it.
OLD_SDCLANG_FLAG_DEFS    := $(SDCLANG_FLAG_DEFS)
OLD_SDCLANG_VERSION_DEFS := $(SDCLANG_VERSION_DEFS)

include $(CAMX_VENDOR_PATH)/Android.mk
include $(CAMX_CHICDK_PATH)/test/hal3-test/Android.mk

include $(CAMX_CDK_PATH)/generated/build/android/Android.mk

# Restore previous value of sdllvm flag and version defs
SDCLANG_FLAG_DEFS    := $(OLD_SDCLANG_FLAG_DEFS)
SDCLANG_VERSION_DEFS := $(OLD_SDCLANG_VERSION_DEFS)
