ifneq ($(SDM845_DISABLE_MODULE),true)

LOCAL_PATH := $(call my-dir)
qdc_cpp_flags := -Wall -Werror -Wno-unused-parameter
wificond_includes := system/connectivity/

###
### wificond qdc binary.
###
include $(CLEAR_VARS)
LOCAL_MODULE := qdc
LOCAL_CPPFLAGS := $(qdc_cpp_flags)
LOCAL_C_INCLUDES := $(wificond_includes)
LOCAL_SRC_FILES := \
    qdc.cpp
LOCAL_SHARED_LIBRARIES := \
    libbase \
    libbinder \
    liblog \
    libutils \
    libwifi-system \
    libwificond_ipc
include $(BUILD_EXECUTABLE)

endif#SDM845_DISABLE_MODULE
