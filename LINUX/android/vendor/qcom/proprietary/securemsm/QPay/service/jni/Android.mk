# ---------------------------------------------------------------------------------
#                 Common definitions
# ---------------------------------------------------------------------------------
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
SECUREMSM_SHIP_PATH   := vendor/qcom/proprietary/securemsm

COMMON_INCLUDES     :=  $(SECUREMSM_SHIP_PATH)/eSEService \
                        $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
                        $(SECUREMSM_SHIP_PATH)/proxydaemon \
                        $(SECUREMSM_SHIP_PATH)/QSEEComAPI \
                        external/libcxx/include


# ---------------------------------------------------------------------------------
#             Make the library
# ---------------------------------------------------------------------------------



LOCAL_CPP_EXTENSION := .cpp #.cxx .cc
LOCAL_CFLAGS := -g -fdiagnostics-show-option -Wno-format \
                -Wno-missing-braces -Wno-missing-field-initializers \
                -std=gnu++0x -fpermissive -Wno-unused-parameter

LOCAL_PRELINK_MODULE := false

# Add all source file names to be included in lib separated by a whitespace
LOCAL_SRC_FILES := QPayAdapter.cpp
LOCAL_SHARED_LIBRARIES := \
        libbinder \
        libandroid_runtime \
        liblog \
        libutils \
        libcutils \
        libesepm \
        libQSEEComAPI

LOCAL_C_INCLUDES += $(COMMON_INCLUDES)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libQPayJNI
LOCAL_CERTIFICATE := platform

LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti

include $(BUILD_SHARED_LIBRARY)
