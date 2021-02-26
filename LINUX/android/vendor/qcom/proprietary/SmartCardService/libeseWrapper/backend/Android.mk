# ---------------------------------------------------------------------------------
#                 Common definitions
# ---------------------------------------------------------------------------------
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
SMSM_PATH           := vendor/qcom/proprietary/securemsm
LIB_ESE_PATH        := external/libese

COMMON_INCLUDES     :=  $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
                        $(LIB_ESE_PATH)/libese/include/ese \
                        $(SMSM_PATH)/GPTEE/inc \
                        $(TARGET_OUT_HEADERS)/common/inc \
                        $(SMSM_PATH)/esepowermanager \
                        external/libcxx/include


LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
# ---------------------------------------------------------------------------------
#             Make the library
# ---------------------------------------------------------------------------------

LOCAL_CPP_EXTENSION := .cpp #.cxx .cc
LOCAL_CFLAGS := -Wall -Wextra -Wshadow \
                -Wcast-qual -Wcast-align \
                -Wreorder -Wmissing-prototypes \
                -Wbad-function-cast \
                -Wunreachable-code \
                -Wheader-hygiene

LOCAL_PRELINK_MODULE := false

# Add all source file names to be included in lib separated by a whitespace
LOCAL_SRC_FILES := src/gpqese-be.cpp
LOCAL_SHARED_LIBRARIES := \
        libhidlbase \
        libhidltransport \
        liblog \
        libutils \
        libese \
        vendor.qti.esepowermanager@1.0_vendor \
        libGPTEE_vendor

LOCAL_C_INCLUDES := $(COMMON_INCLUDES) \
                    $(LOCAL_PATH)/inc

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libgpqese
LOCAL_CERTIFICATE := platform
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti

include $(BUILD_SHARED_LIBRARY)
#copying libgpqese.so to /system
include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(COMMON_INCLUDES) \
                    $(LOCAL_PATH)/inc
LOCAL_SRC_FILES := src/gpqese-be.cpp
LOCAL_SHARED_LIBRARIES := \
        libhidlbase \
        libhidltransport \
        liblog \
        libutils \
        vendor.qti.esepowermanager@1.0 \
        libese_system \
        libGPTEE_system

LOCAL_MODULE := libgpqese_system
LOCAL_MODULE_TAGS := optional
LOCAL_INSTALLED_MODULE_STEM := libgpqese.so

include $(BUILD_SHARED_LIBRARY)
