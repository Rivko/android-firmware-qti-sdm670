LOCAL_PATH:= $(call my-dir)
SECUREMSM_PATH = $(TOP)/vendor/qcom/proprietary/securemsm

COMMON_C_INCLUDES = \
                    $(SECUREMSM_PATH)/smcinvoke/inc \
                    $(LOCAL_PATH) \

COMMON_SHARED_LIBRARIRS = \
                          liblog \
                          libtzcom \

include $(CLEAR_VARS)
LOCAL_C_INCLUDES += $(COMMON_C_INCLUDES)
LOCAL_SHARED_LIBRARIES += $(COMMON_SHARED_LIBRARIRS)
LOCAL_CFLAGS += -Wno-unused-parameter
#LOCAL_CFLAGS += -DPVCLICENSE_SAMPLE_DAEMON

LOCAL_MODULE := pvclicense_sample
LOCAL_SRC_FILES := pvclicense.cpp
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true


include $(BUILD_EXECUTABLE)
