LOCAL_PATH := $(call my-dir)

VAM_TOP_SRCDIR := $(LOCAL_PATH)/..

include $(VAM_TOP_SRCDIR)/build.mk

ifneq (,$(BUILD_VAM))

# Build VASim

include $(CLEAR_VARS)

include $(VAM_TOP_SRCDIR)/common.mk

LOCAL_C_INCLUDES := $(LOCAL_PATH)/inc
LOCAL_C_INCLUDES += $(VAM_TOP_SRCDIR)/vam_utils/JSON/json_apis/include/

LOCAL_SRC_FILES := src/VASim.cc
LOCAL_SRC_FILES += src/ConfigBase.cc
LOCAL_SRC_FILES += src/VASimConfig.cc
LOCAL_SRC_FILES += src/VAMExtRuleConfig.cc
LOCAL_SRC_FILES += src/VAMJSONRuleConfig.cc
LOCAL_SRC_FILES += src/VASimFRConfig.cc

LOCAL_SHARED_LIBRARIES += libjson_apis
LOCAL_SHARED_LIBRARIES += libVAManager
LOCAL_SHARED_LIBRARIES += libavcodec

LOCAL_MODULE = VASim

#include $(BUILD_EXECUTABLE)

endif # BUILD_VAM
