LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

#-----------------------------------------------------------------
# Define
#-----------------------------------------------------------------
LOCAL_CFLAGS := -D_ANDROID_
LOCAL_CFLAGS += -Wconversion
LOCAL_CFLAGS += -USYSTRACE_PROFILE
#----------------------------------------------------------------
# SRC CODE
#----------------------------------------------------------------
LOCAL_SRC_FILES := src/WFD_HdcpCP.cpp
LOCAL_SRC_FILES += src/HDCPManager.cpp

#----------------------------------------------------------------
# INCLUDE PATH
#----------------------------------------------------------------
LOCAL_C_INCLUDES := $(LOCAL_PATH)/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/mm-osal/include
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/wfd/include
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc
LOCAL_C_INCLUDES += $(TOP)/bionic/libc/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../source/utils/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/mm-core/omxcore
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/hdcp-mgr/inc
LOCAL_C_INCLUDES += $(TOP)/frameworks/native/include/utils

LOCAL_COPY_HEADERS_TO := wfd/include
LOCAL_COPY_HEADERS := inc/WFD_HdcpCP.h
LOCAL_COPY_HEADERS += inc/HDCPManager.h


#----------------------------------------------------------------
# Dx HDCP COMPILE TIME
#----------------------------------------------------------------
COMPILE_HDCP_LIB := false
WFD_NOSHIP_HDCP_PATH  := $(LOCAL_PATH)/../../../../wfd-noship/mm/hdcp
WFD_INTERNAL_LIB_PATH := $(TOP)/vendor/qcom/proprietary/wfd-internal/mm/hdcp/lib
COMPILE_HDCP_LIB := $(shell if [[ -d $(WFD_NOSHIP_HDCP_PATH) && -d $(WFD_INTERNAL_LIB_PATH) ]] ; then echo true; fi)

LOCAL_SHARED_LIBRARIES := libmmosal_proprietary
ifeq ($(strip $(COMPILE_HDCP_LIB)),true)
$(info Compiling HDCP module)
LOCAL_C_INCLUDES += $(WFD_NOSHIP_HDCP_PATH)
LOCAL_C_INCLUDES += $(WFD_NOSHIP_HDCP_PATH)/HDCP_API
LOCAL_SHARED_LIBRARIES_32 += libDxHdcp
LOCAL_SHARED_LIBRARIES += libmm-hdcpmgr
LOCAL_CFLAGS_32 += -DWFD_HDCP_ENABLED
else
$(info Not enabling HDCP compilation)
endif
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libdl
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libcutils

LOCAL_MODULE:= libwfdhdcpcp

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)
