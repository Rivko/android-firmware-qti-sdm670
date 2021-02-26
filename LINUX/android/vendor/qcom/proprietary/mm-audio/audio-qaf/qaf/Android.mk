QAF_PROCESSING_ROOT := $(call my-dir)

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := mm-audio/qaf
LOCAL_COPY_HEADERS := qti_audio.h
include $(BUILD_COPY_HEADERS)
