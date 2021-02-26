ifneq ($(BUILD_TINY_ANDROID),true)
ifneq ($(BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE),)

LOCAL_PATH := $(call my-dir)

# Select which RPC lib to link to
LOC_API_USE_LOCAL_RPC:=0
LOC_API_USE_QCOM_AUTO_RPC:=1

# Target-specific makefile
GPS_BUILD_DIR:=$(LOCAL_PATH)/build
include $(GPS_BUILD_DIR)/target_specific_features.mk

ifneq ($(LOC_UNSUPPORTED_TARGET),true)
ifeq ($(LW_FEATURE_SET),true)
DIR_LIST := $(LOCAL_PATH)/etc/
DIR_LIST += $(LOCAL_PATH)/framework/utils/
DIR_LIST += $(LOCAL_PATH)/framework/native/core/
DIR_LIST += $(LOCAL_PATH)/garden-app/
include $(addsuffix Android.mk, $(DIR_LIST))
else ifeq ($(BOARD_VENDOR_QCOM_LOC_PDK_FEATURE_SET),true)
DIR_LIST := $(LOCAL_PATH)/etc/
DIR_LIST += $(LOCAL_PATH)/framework/utils/
DIR_LIST += $(LOCAL_PATH)/framework/native/core/
DIR_LIST += $(LOCAL_PATH)/framework/native/lcp/data-items/
DIR_LIST += $(LOCAL_PATH)/launcher/
DIR_LIST += $(LOCAL_PATH)/slim/
DIR_LIST += $(LOCAL_PATH)/garden-app/
include $(addsuffix Android.mk, $(DIR_LIST))
else
#include $(call all-subdir-makefiles)
include $(LOCAL_PATH)/*/Android.mk
endif

endif # LOC_UNSUPPORTED_TARGET

endif # BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE
endif # BUILD_TINY_ANDROID
