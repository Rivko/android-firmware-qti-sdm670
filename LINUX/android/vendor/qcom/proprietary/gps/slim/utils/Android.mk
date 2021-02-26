
LOCAL_PATH := $(call my-dir)

# libslimutils
# Test application for verification of SLIM API client library operation.
# Application supports multilib
include $(CLEAR_VARS)
LOCAL_MODULE := libslimutils
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_SRC_FILES:= \
  src/slim_internal.c \
  src/slim_timetick.cpp \
  src/qmislim_target.c \
  src/slim_log_utils.c \
  src/slim_timer.c \
  src/slim_messaging.c \
  src/slim_thread.c
LOCAL_C_INCLUDES := \
  $(TARGET_OUT_HEADERS)/common/inc \
  $(TARGET_OUT_HEADERS)/qmi-framework/inc \
  $(TARGET_OUT_HEADERS)/diag/include \
  $(TARGET_OUT_HEADERS)/libslimcommon \
  $(LOCAL_PATH)/inc \
  $(LOCAL_PATH)/src
LOCAL_HEADER_LIBRARIES := \
  libgps.utils_headers \
  libloc_pla_headers
LOCAL_CFLAGS += \
  $(GPS_FEATURES) \
  -DFEATURE_LOCTECH_SLIM_LOG_LOGCAT \
  -DFEATURE_LOCTECH_SLIM_NOFLOCK \
  -fvisibility=hidden
LOCAL_CPPFLAGS += \
  -D__STDC_FORMAT_MACROS \
  -D__STDINT_LIMITS \
  -D__STDINT_MACROS
# LOCAL_STATIC_LIBRARIES :=
LOCAL_SHARED_LIBRARIES := libutils
LOCAL_EXPORT_C_INCLUDE_DIRS := \
  $(LOCAL_PATH)/inc
ifeq ($(TARGET_BUILD_VARIANT),$(filter $(TARGET_BUILD_VARIANT),userdebug eng))
LOCAL_CFLAGS           += \
  -DFEATURE_LOCTECH_SLIM_DEBUG \
  -DFEATURE_LOCTECH_SLIM_LOG_DIAG \
  -DFEATURE_LOCTECH_SLIM_LOG_CONSOLE
endif
ifeq (true,$(FEATURE_LOCTECH_SLIM_OS_LOGS))
LOCAL_CFLAGS           += \
  -DFEATURE_LOCTECH_SLIM_OS_LOGS
endif
LOCAL_CFLAGS += $(GNSS_CFLAGS)
include $(BUILD_STATIC_LIBRARY)
