# ---------------------------------------------------------------------------------
#                 Common definitions
# ---------------------------------------------------------------------------------
LOCAL_PATH            := $(call my-dir)
SECUREMSM_SHIP_PATH   := vendor/qcom/proprietary/securemsm
AUTH_SHIP             := $(SECUREMSM_SHIP_PATH)/authentication/include

COMMON_INCLUDES     :=  $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
                        $(AUTH_SHIP) \
                        $(SECUREMSM_SHIP_PATH)/QTEEConnector/include \
                        $(SECUREMSM_SHIP_PATH)/QSEEComAPI \

include $(CLEAR_VARS)
LOCAL_CPP_EXTENSION := .cpp
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libSecureSampleAuthJNI
LOCAL_CFLAGS := -g -fdiagnostics-show-option -Wno-format \
                -Wno-missing-braces -Wno-missing-field-initializers \
                -std=gnu++0x -fpermissive -Wno-unused-parameter

LOCAL_PRELINK_MODULE := false

LOCAL_C_INCLUDES += $(COMMON_INCLUDES)

# Add all source file names to be included in lib separated by a whitespace
LOCAL_SRC_FILES := SecureSampleAuthImpl_interface.cpp
LOCAL_SHARED_LIBRARIES += \
        liblog \
        libutils \
        libhidlbase \
        libQTEEConnector_system \
        vendor.qti.hardware.qteeconnector@1.0

LOCAL_MODULE_OWNER := qti
LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
include $(BUILD_SHARED_LIBRARY)

