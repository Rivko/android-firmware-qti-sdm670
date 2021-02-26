LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

#-----------------------------------------------------------------
# Define
#-----------------------------------------------------------------
LOCAL_CFLAGS := -D_ANDROID_

#----------------------------------------------------------------
# SRC CODE
#----------------------------------------------------------------
LOCAL_SRC_FILES := src/WFDSurfaceMediaSource.cpp

#----------------------------------------------------------------
# INCLUDE PATH
#----------------------------------------------------------------
LOCAL_C_INCLUDES := $(LOCAL_PATH)/inc
LOCAL_C_INCLUDES += $(TOP)/frameworks/native/include/media/openmax
LOCAL_C_INCLUDES += $(TOP)/frameworks/native/include/media/hardware
LOCAL_C_INCLUDES += $(TOP)/frameworks/av/include/media
ifeq ($(TARGET_USES_QCOM_DISPLAY_BSP), true)
    LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qcom/display
    LOCAL_CFLAGS += -DQTI_BSP
endif
LOCAL_SHARED_LIBRARIES := liblog
LOCAL_SHARED_LIBRARIES += libbinder
LOCAL_SHARED_LIBRARIES += libmediautils
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libmedia
LOCAL_SHARED_LIBRARIES += libstagefright
LOCAL_SHARED_LIBRARIES += libgui
LOCAL_SHARED_LIBRARIES += libui
LOCAL_SHARED_LIBRARIES += libstagefright_foundation

LOCAL_COPY_HEADERS_TO := wfd/include
LOCAL_COPY_HEADERS := inc/WFDSurfaceMediaSource.h

LOCAL_MODULE:= libwfdavenhancements

LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
