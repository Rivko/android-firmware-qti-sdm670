LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE:=sns_hal_batch
LOCAL_MODULE_OWNER:=qti

LOCAL_SHARED_LIBRARIES += \
  libdl

LOCAL_SRC_FILES += \
  sns_espplus_testapp.c

LOCAL_CFLAGS += -D_GNU_SOURCE
LOCAL_CFLAGS += -DBOARD_PLATFORM=\"$(TARGET_BOARD_PLATFORM)\"

ifeq ($(USE_SENSOR_MULTI_HAL),true)
LOCAL_CFLAGS += -DUSES_MULTI_HAL=\"$(USE_SENSOR_MULTI_HAL)\"
endif

ifeq ($(PRODUCT_FULL_TREBLE),true)
LOCAL_CFLAGS += -DUSES_FULL_TREBLE=\"$(PRODUCT_FULL_TREBLE)\"
endif

ifeq ($(TARGET_SUPPORTS_WEARABLES),true)
LOCAL_CFLAGS += -DSNS_WEARABLES_TARGET
endif

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)

ifneq ($(TARGET_SUPPORTS_WEARABLES),true)
include $(CLEAR_VARS)
LOCAL_MODULE:=sns_hidl_hal_batch
LOCAL_MODULE_OWNER:=qti

LOCAL_SHARED_LIBRARIES += \
    liblog \
    libutils\
    libcutils\
    libhidlbase \
    libhidltransport \
    libhwbinder \
    android.hardware.sensors@1.0 \
    libdl \
    libbase

LOCAL_SRC_FILES += \
 sns_hidl_hal_batch.cpp \
 convert.cpp

LOCAL_CFLAGS += -D_GNU_SOURCE -Wall -Werror -Wno-unused-parameter
LOCAL_CFLAGS += -DBOARD_PLATFORM=\"$(TARGET_BOARD_PLATFORM)\"
ifeq ($(LLVM_SENSORS_SEE),true)
LOCAL_CFLAGS += --compile-and-analyze --analyzer-perf --analyzer-Werror
endif

LOCAL_EXPORT_SHARED_LIBRARY_HEADERS := \
    libsensor \

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)
endif
