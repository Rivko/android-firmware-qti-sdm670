RCS_DIR := $(call my-dir)
#include $(CLEAR_VARS)
#include $(RCS_DIR)/interfaces/Android.mk
#include $(RCS_DIR)/java/PresenceApp/Android.mk
#include $(RCS_DIR)/java/uceShimService/Android.mk
include $(call first-makefiles-under, $(RCS_DIR))
