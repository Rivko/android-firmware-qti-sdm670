ifneq ($(BUILD_TINY_ANDROID),true)

LOCAL_PATH := $(call my-dir)

DATAITEMS_HEADERS := \
    inc/DataItemConcreteTypeDefaultValues.h \
    inc/DataItemConcreteTypeFieldNames.h \
    inc/DataItemsFactory.h \
    inc/DataItemConcreteTypes.h \
    inc/IDataItemSerializer.h \
    inc/loc_ril.h

LOCAL_COPY_HEADERS := $(DATAITEMS_HEADERS)
LOCAL_COPY_HEADERS_TO := libdataitems
include $(BUILD_COPY_HEADERS)

include $(CLEAR_VARS)
LOCAL_MODULE := libdataitems_headers
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/inc
include $(BUILD_HEADER_LIBRARY)

DATAITEMS_SRCS := \
    src/DataItemsFactory.cpp \
    src/DataItemConcreteTypes.cpp

# Build libdataitems

include $(CLEAR_VARS)

LOCAL_MODULE := libdataitems
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

LOCAL_CFLAGS += \
    -D_ANDROID_

LOCAL_C_INCLUDES := \
    $(TOP)/system/core/include \
    $(LOCAL_PATH)/inc/internal \
    $(LOCAL_PATH)/inc \
    $(TARGET_OUT_HEADERS)/common/inc
LOCAL_HEADER_LIBRARIES := \
    libgps.utils_headers \
    libloc_core_headers \
    liblbs_core_headers \
    libloc_base_util_headers \
    izat_remote_api_headers \
    izat_remote_api_prop_headers \
    libloc_pla_headers

LOCAL_SHARED_LIBRARIES := \
    libutils \
    libcutils \
    libgps.utils \
    liblog

LOCAL_STATIC_LIBRARIES := \
    libloc_base_util

LOCAL_SRC_FILES := \
    $(DATAITEMS_SRCS)
LOCAL_CFLAGS += $(GNSS_CFLAGS)
include $(BUILD_SHARED_LIBRARY)

#####################################################################
endif
