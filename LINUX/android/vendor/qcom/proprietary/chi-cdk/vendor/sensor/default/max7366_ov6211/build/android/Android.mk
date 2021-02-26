ifeq ($(CAMX_CHICDK_PATH),)
LOCAL_PATH := $(abspath $(call my-dir)/../..)
CAMX_CHICDK_PATH := $(abspath $(LOCAL_PATH)/../../../..)
else
LOCAL_PATH := $(CAMX_CHICDK_PATH)/vendor/sensor/default/max7366_ov6211
endif

include $(CLEAR_VARS)

# Module supports function call tracing via ENABLE_FUNCTION_CALL_TRACE
# Required before including common.mk
SUPPORT_FUNCTION_CALL_TRACE := 1

# Get definitions common to the CAMX project here
include $(CAMX_CHICDK_PATH)/vendor/common.mk

LOCAL_SRC_FILES :=                   \
    max7366_ov6211_sensor.cpp

LOCAL_INC_FILES :=                   \


# Put here any libraries that should be linked by CAMX projects
LOCAL_C_LIBS := $(CAMX_C_LIBS)

# Paths to included headers
LOCAL_C_INCLUDES := $(CAMX_C_INCLUDES) \
                    $(CAMX_CDK_PATH)/node/

###############################################################################
# SPECIAL FOLDER SECTION START: cdk, system, and vendor
#
# For compatibility with the component repo directory structure, the include
# paths for these "special" folders must follow a specific format to prevent
# build failures.
#
# cdk folder:    MUST use "$(CAMX_CDK_PATH)", NOT "$(CAMX_PATH)/cdk/"
# vendor folder: MUST use "$(CAMX_VENDOR_PATH)", NOT "$(CAMX_PATH)/vendor/"
###############################################################################

LOCAL_C_INCLUDES +=           \
    $(CAMX_CDK_PATH)/sensor   \
    system/media/camera/include

###############################################################################
# SPECIAL FOLDER SECTION END: cdk, system, and vendor
###############################################################################

# Compiler flags
LOCAL_CFLAGS := $(CAMX_CFLAGS)
LOCAL_CPPFLAGS := $(CAMX_CPPFLAGS)

# Library name
LOCAL_MODULE := com.qti.sensor.max7366_ov6211

# Deployment path
LOCAL_MODULE_RELATIVE_PATH := camera

include $(BUILD_SHARED_LIBRARY)
-include $(CAMX_CHECK_WHINER)
