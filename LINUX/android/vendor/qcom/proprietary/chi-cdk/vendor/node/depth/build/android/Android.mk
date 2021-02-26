ifeq ($(CAMX_CHICDK_PATH),)
LOCAL_PATH := $(abspath $(call my-dir)/../..)
CAMX_CHICDK_PATH := $(abspath $(LOCAL_PATH)/../../..)
else
LOCAL_PATH := $(CAMX_CHICDK_PATH)/vendor/node/depth
endif

### Make dummy depth lib
include $(CLEAR_VARS)

# Module supports function call tracing via ENABLE_FUNCTION_CALL_TRACE
# Required before including common.mk
SUPPORT_FUNCTION_CALL_TRACE := 1

# Get definitions common to the CAMX project here
include $(CAMX_CHICDK_PATH)/vendor/common.mk

LOCAL_INC_FILES :=              \
    depthmapwrapper.h

LOCAL_SRC_FILES :=              \
    depthmapwrapper.cpp

# Compiler flags
LOCAL_CFLAGS := $(CAMX_CFLAGS)
LOCAL_CPPFLAGS := $(CAMX_CPPFLAGS)

LOCAL_MODULE := libdepthmapwrapper

LOCAL_MODULE_RELATIVE_PATH := $(CAMX_LIB_OUTPUT_PATH)

include $(BUILD_SHARED_LIBRARY)
### End of make dummy depth lib

include $(CLEAR_VARS)

# Get definitions common to the CAMX project here
include $(CAMX_CHICDK_PATH)/vendor/common.mk

LOCAL_INC_FILES :=              \
    camxchinodedepth.h

LOCAL_SRC_FILES :=              \
    camxchinodedepth.cpp

LOCAL_C_LIBS := $(CAMX_C_LIBS)

LOCAL_C_INCLUDES :=               \
    $(CAMX_C_INCLUDES)            \
    $(CAMX_CHICDK_PATH)/node/     \
    system/media/camera/include

# Compiler flags
LOCAL_CFLAGS := $(CAMX_CFLAGS)
LOCAL_CPPFLAGS := $(CAMX_CPPFLAGS)

LOCAL_SHARED_LIBRARIES :=           \
    libcamera_metadata              \
    libcom.qti.chinodeutils

LOCAL_MODULE := com.qti.node.depth

LOCAL_MODULE_RELATIVE_PATH := $(CAMX_LIB_OUTPUT_PATH)

include $(BUILD_SHARED_LIBRARY)
