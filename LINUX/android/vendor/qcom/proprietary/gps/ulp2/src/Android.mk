#/******************************************************************************
#*@file Android.mk
#* brief Rules for compiling native code associated with libulp library
#*  ******************************************************************************/
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
ULP2_ROOT_DIR = ../

# This is the target being built.
LOCAL_MODULE:= libulp2

# All of the source files that we will compile.
LOCAL_SRC_FILES:= \
    ulp_main.cpp \
    ulp_data.cpp \
    ulp_msg.cpp \
    ulp_gnss.cpp \
    ulp_amt.cpp \
    ulp_gnp.cpp \
    ulp_quipc.cpp \
    ulp_xtwifi.cpp \
    ulp_brain.cpp \
    ulp_monitor.cpp \
    ulp_debug.cpp \
    ulp_log.cpp \
    LocUlpProxy.cpp \
    LocAmtProxy.cpp \
    ulp_zpp.cpp

# All of the shared libraries we link against.
LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    libgps.utils \
    libdl \
    libloc_core \
    libizat_core \
    liblog \
    liblbs_core \
    libDRPlugin \
    libevent_observer

LOCAL_C_INCLUDES += \
  $(LOCAL_PATH)/$(ULP2_ROOT_DIR)/inc \
  $(TARGET_OUT_HEADERS)/common/inc \
  $(TARGET_OUT_HEADERS)/libizat_core \
  $(TARGET_OUT_HEADERS)/libDRPlugin

LOCAL_HEADER_LIBRARIES := \
    libgps.utils_headers \
    libloc_core_headers \
    liblocationservice_headers \
    libevent_observer_headers \
    liblbs_core_headers \
    libloc_pla_headers \
    liblocation_api_headers

LOCAL_CFLAGS+=$(GPS_FEATURES) \
    -D_ANDROID_ \

LOCAL_LDFLAGS += -Wl,--export-dynamic
LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_OWNER := qcom
LOCAL_PROPRIETARY_MODULE := true

LOCAL_CFLAGS += $(GNSS_CFLAGS)
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libulp2_headers
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/inc \
                               $(LOCAL_PATH)/src
include $(BUILD_HEADER_LIBRARY)
