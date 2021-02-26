LOCAL_PATH := $(call my-dir)

VAM_TOP_SRCDIR := $(LOCAL_PATH)/../..

include $(VAM_TOP_SRCDIR)/build.mk

ifneq (,$(BUILD_VAM))

# Build VAMReport

include $(CLEAR_VARS)

include $(VAM_TOP_SRCDIR)/common.mk

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_C_INCLUDES += $(VAM_TOP_SRCDIR)/vam_utils/JSON/common/include
LOCAL_C_INCLUDES += $(VAM_TOP_SRCDIR)/vam_utils/JSON/json_apis/include/

LOCAL_SRC_FILES := source/test.cc
LOCAL_SRC_FILES += source/metadata_comparisor.cc

LOCAL_SHARED_LIBRARIES += libjson_apis libjsoncpp_vendor

LOCAL_MODULE = VAMReport

include $(BUILD_EXECUTABLE)

endif # BUILD_VAM