ifeq ($(CAMX_CHICDK_PATH),)
LOCAL_PATH := $(abspath $(call my-dir)/../..)
CAMX_CHICDK_PATH := $(abspath $(LOCAL_PATH)/../..)
else
LOCAL_PATH := $(CAMX_CHICDK_PATH)/vendor/eeprom
endif

include $(CLEAR_VARS)

# Module supports function call tracing via ENABLE_FUNCTION_CALL_TRACE
# Required before including common.mk
SUPPORT_FUNCTION_CALL_TRACE := 1

# Get definitions common to the CAMX project here
include $(CAMX_VENDOR_PATH)/common.mk

LOCAL_SRC_FILES :=                   \
    truly_cmb433/truly_cmb433_eeprom.cpp

LOCAL_INC_FILES :=                   \


# Put here any libraries that should be linked by CAMX projects
LOCAL_C_LIBS := $(CAMX_C_LIBS)

# Paths to included headers
LOCAL_C_INCLUDES := $(CAMX_C_INCLUDES)

LOCAL_C_INCLUDES +=           \
    $(CAMX_CDK_PATH)/sensor

# Compiler flags
LOCAL_CFLAGS := $(CAMX_CFLAGS)
LOCAL_CPPFLAGS := $(CAMX_CPPFLAGS)

# Library name
LOCAL_MODULE := com.qti.eeprom.truly_cmb433

# Deployment path
LOCAL_MODULE_RELATIVE_PATH := camera

include $(BUILD_SHARED_LIBRARY)
-include $(CAMX_CHECK_WHINER)
