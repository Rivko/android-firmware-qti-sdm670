#=#====#====#====#====#====#====#====#====#====#====#====#====#====#====#====#
#
#        Location Service module - glue
#
# GENERAL DESCRIPTION
#   Glue location service module makefile
#
#=============================================================================
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := liblocationservice_glue
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := src/RawCommandInjector.cpp
LOCAL_SRC_FILES += src/OSFrameworkCleaner.cpp
LOCAL_SRC_FILES += src/OsNpGlue.cpp
LOCAL_SRC_FILES += src/Subscription.cpp
LOCAL_SRC_FILES += src/FrameworkActionRequest.cpp
LOCAL_SRC_FILES += src/OSFramework.cpp

LOCAL_SHARED_LIBRARIES := liblocationservice
LOCAL_SHARED_LIBRARIES += libulp2
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libloc_core
LOCAL_SHARED_LIBRARIES += libgps.utils
LOCAL_SHARED_LIBRARIES += libhardware_legacy
LOCAL_SHARED_LIBRARIES += liblbs_core
LOCAL_SHARED_LIBRARIES += libdataitems
LOCAL_SHARED_LIBRARIES += libgeofence
LOCAL_SHARED_LIBRARIES += libflp

LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libhidlbase
LOCAL_SHARED_LIBRARIES += libhidltransport
LOCAL_SHARED_LIBRARIES += libhwbinder
LOCAL_SHARED_LIBRARIES += android.hidl.base@1.0
LOCAL_SHARED_LIBRARIES += android.hardware.gnss@1.0
LOCAL_SHARED_LIBRARIES += vendor.qti.gnss@1.0_vendor

ifeq ($(LOC_HIDL_VERSION),1.1)
LOCAL_SHARED_LIBRARIES += vendor.qti.gnss@1.1_vendor
endif

ifeq ($(LOC_HIDL_VERSION),1.2)
LOCAL_SHARED_LIBRARIES += vendor.qti.gnss@1.1_vendor
LOCAL_SHARED_LIBRARIES += vendor.qti.gnss@1.2_vendor
endif

LOCAL_C_INCLUDES := $(LOCAL_PATH)/inc
LOCAL_C_INCLUDES += $(TOP)/frameworks/base/include/android_runtime
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/libflp
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/libizat_core
LOCAL_HEADER_LIBRARIES := \
    libgps.utils_headers \
    libloc_core_headers \
    liblbs_core_headers \
    liblocationservice_headers \
    izat_remote_api_headers \
    izat_remote_api_prop_headers \
    libdataitems_headers \
    libloc_pla_headers \
    liblocation_api_headers

LOCAL_CFLAGS += $(GPS_FEATURES) \
    $(GNSS_CFLAGS)

ifeq ($(LOC_HIDL_VERSION),1.0)
LOCAL_CFLAGS += -DLOC_HIDL_VERSION_1_0
endif

ifeq ($(LOC_HIDL_VERSION),1.1)
LOCAL_CFLAGS += -DLOC_HIDL_VERSION_1_1
endif

ifeq ($(LOC_HIDL_VERSION),1.2)
LOCAL_CFLAGS += -DLOC_HIDL_VERSION_1_2
endif

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := liblocationservice_glue
LOCAL_COPY_HEADERS := inc/OSFramework.h
LOCAL_COPY_HEADERS += inc/OsNpGlue.h
LOCAL_COPY_HEADERS += inc/Subscription.h
LOCAL_COPY_HEADERS += inc/FrameworkActionRequest.h
include $(BUILD_COPY_HEADERS)

include $(CLEAR_VARS)
LOCAL_MODULE := liblocationservice_glue_headers
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/inc
include $(BUILD_HEADER_LIBRARY)
