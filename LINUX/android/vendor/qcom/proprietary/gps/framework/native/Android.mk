#=#====#====#====#====#====#====#====#====#====#====#====#====#====#====#====#
#
#        Location Service module
#
# GENERAL DESCRIPTION
#   Location Service module makefile
#
#=============================================================================
ifneq ($(BUILD_TINY_ANDROID),true)

LOCAL_PATH := $(call my-dir)

include $(addsuffix /Android.mk,$(addprefix $(LOCAL_PATH)/, core lcp framework-glue framework-jni))

endif # not BUILD_TINY_ANDROID
