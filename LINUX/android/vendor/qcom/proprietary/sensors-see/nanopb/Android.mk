LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := sensors/nanopb/inc
LOCAL_COPY_HEADERS := inc/pb.h
LOCAL_COPY_HEADERS += inc/pb_common.h
LOCAL_COPY_HEADERS += inc/pb_encode.h
LOCAL_COPY_HEADERS += inc/pb_decode.h
include $(BUILD_COPY_HEADERS)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(shell find $(LOCAL_PATH) -type d -name 'inc' -print )

LOCAL_MODULE := libnanopb
LOCAL_CLANG := true
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

LOCAL_SRC_FILES += \
  $(shell find $(LOCAL_PATH)/* -name '*.c' | grep  'src/.*\.c' | sed s:^$(LOCAL_PATH)/::g )

LOCAL_CFLAGS += -std=c11 -DPB_FIELD_16BIT
LOCAL_CFLAGS += -Wall -Wextra -Werror -Wno-missing-field-initializers -Wno-maybe-uninitialized
ifeq ($(LLVM_SENSORS_SEE),true)
LOCAL_CFLAGS += --compile-and-analyze --analyzer-perf --analyzer-Werror
endif

LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)
