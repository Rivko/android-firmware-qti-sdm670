LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

# ------------------------------------------------------------------------------
#            Common definitons
# ------------------------------------------------------------------------------

LOCAL_CFLAGS := -D_ANDROID_
ifeq ($(call is-android-codename,ICECREAM_SANDWICH),true)
LOCAL_CFLAGS += -DWFD_ICS
endif

ifeq ($(call is-board-platform-in-list,msm8996),true)
LOCAL_CFLAGS += -DDISPLAY_DS_APIS_AVAILABLE
endif

# ------------------------------------------------------------------------------
#            WFD COMMON UTILS SRC
# ------------------------------------------------------------------------------

LOCAL_SRC_FILES := \
    src/wfd_cfg_parser.cpp \
    src/wfd_netutils.cpp \
    src/wfd_cfg_utils.cpp

# ------------------------------------------------------------------------------
#             WFD COMMON UTILS INC
# ------------------------------------------------------------------------------

LOCAL_C_INCLUDES := $(LOCAL_PATH)/./inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/mm-osal/include
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/mm-core/omxcore
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/wfd/include
LOCAL_C_INCLUDES += external/libcxx/include
LOCAL_C_INCLUDES += external/libnl/include
LOCAL_C_INCLUDES += $(TOP)/system/core/include/utils
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qcom/display

# ---------------------------------------------------------------------------------
#            COPY INCLUDE FILES TO OUT
# ---------------------------------------------------------------------------------

LOCAL_COPY_HEADERS_TO := wfd/include
LOCAL_COPY_HEADERS := inc/wfd_cfg_parser.h
LOCAL_COPY_HEADERS += inc/wfd_netutils.h
LOCAL_COPY_HEADERS += inc/wfd_cfg_utils.h

# ------------------------------------------------------------------------------
#            WFD COMMON UTILS SHARED LIB
# ------------------------------------------------------------------------------

LOCAL_SHARED_LIBRARIES := libmmosal
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libqservice
LOCAL_SHARED_LIBRARIES += libqdutils
LOCAL_SHARED_LIBRARIES += libbinder
LOCAL_SHARED_LIBRARIES += libnl
LOCAL_SHARED_LIBRARIES += libwfdmminterface
LOCAL_LDLIBS += -llog

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= libwfdconfigutils

LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
include $(BUILD_SHARED_LIBRARY)
include $(call all-makefiles-under,$(LOCAL_PATH))

# ------------------------------------------------------------------------------
#            END
# ------------------------------------------------------------------------------
