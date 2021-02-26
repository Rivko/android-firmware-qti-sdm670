LOCAL_PATH := $(call my-dir)

# Build libgardencaseulp
include $(CLEAR_VARS)

LOCAL_MODULE := libgardencasesample
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRITARY_MODULE := true
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES += \
    SampleCase.cpp \

LOCAL_CFLAGS += \
    -fno-short-enums \
    -fexceptions \

LOCAL_HEADER_LIBRARIES := \
    libgarden_headers

LOCAL_SHARED_LIBRARIES := \
    libgarden \
    liblog \

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)
