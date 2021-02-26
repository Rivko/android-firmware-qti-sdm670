ifneq ($(BUILD_TINY_ANDROID),true)


ROOT_DIR := $(call my-dir)
LOCAL_PATH := $(ROOT_DIR)
include $(CLEAR_VARS)

# ---------------------------------------------------------------------------------
#                               Common definitons
# ---------------------------------------------------------------------------------


# ---------------------------------------------------------------------------------
#                       Make the usf_gesture daemon
# ---------------------------------------------------------------------------------


include $(call all-makefiles-under, $(LOCAL_PATH))

endif #BUILD_TINY_ANDROID
