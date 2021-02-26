LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

# ---------------------------------------------------------------------------------
#             Make MDTP image
# ---------------------------------------------------------------------------------

LOCAL_MODULE:= mdtp.img
LOCAL_MODULE_OWNER := qti
LOCAL_MODULE_PATH := $(PRODUCT_OUT)
LOCAL_SRC_FILES := mdtp.img
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
