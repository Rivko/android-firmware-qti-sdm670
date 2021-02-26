# ---------------------------------------------------------------------------------
#                 Common definitions
# ---------------------------------------------------------------------------------
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LIB_ESE_PATH := external/libese
COMMON_INCLUDES     :=  $(LIB_ESE_PATH)/libese/include/ese \
                        external/libcxx/include

# ---------------------------------------------------------------------------------
#             Make the library
# ---------------------------------------------------------------------------------

LOCAL_CPP_EXTENSION := .cpp #.cxx .cc
LOCAL_CFLAGS := -Wall -Wextra -Wshadow \
                -Wcast-qual -Wcast-align \
                -Wbad-function-cast \
                -Wunreachable-code \
                -Wno-unused-variable \
                -Wheader-hygiene

LOCAL_PRELINK_MODULE := false

# Add all source file names to be included in lib separated by a whitespace
LOCAL_SRC_FILES := eSEWrapper.cpp
LOCAL_SHARED_LIBRARIES := \
        liblog \
        libutils \
        libese_system \
        libgpqese_system

LOCAL_C_INCLUDES := $(COMMON_INCLUDES)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libeSEWrapperJNI
LOCAL_CERTIFICATE := platform
LOCAL_MODULE_OWNER := qti


include $(BUILD_SHARED_LIBRARY)
