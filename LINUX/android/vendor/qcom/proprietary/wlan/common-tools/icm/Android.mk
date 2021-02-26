LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

include $(LOCAL_PATH)/android.config

ifeq ($(SUPPORT_ICM),y)

LOCAL_MODULE := icm
LOCAL_CLANG := true
LOCAL_MODULE_TAGS := optional

ifeq ($(PRODUCT_VENDOR_MOVE_ENABLED),true)
LOCAL_PROPRIETARY_MODULE := true
endif

LOCAL_C_INCLUDES :=
LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/common
LOCAL_C_INCLUDES += $(LOCAL_PATH)/driver
LOCAL_C_INCLUDES += \
    $(LOCAL_PATH) \
    external/libnl/include

LOCAL_SHARED_LIBRARIES :=

# libnl is required for this module. this condition must always be true
ifneq ($(wildcard external/libnl),)
LOCAL_SHARED_LIBRARIES += libnl
endif

LOCAL_SHARED_LIBRARIES += libdl
LOCAL_SHARED_LIBRARIES += libc libcutils liblog

LOCAL_SRC_FILES :=
LOCAL_SRC_FILES += icm.c icm_test.c icm_scan.c icm_socket.c icm_spectral.c icm_utils.c icm_selector.c  icm_cmds.c icm_wal.c icm_wal_ioctl.c icm_wal_mblcfg.c driver/events.c driver/driver_nl80211.c

#LOCAL_CFLAGS := -Wall -Werror

ifeq ($(BUILD_DEBUG), y)
LOCAL_CFLAGS += -g
endif

ifeq (${BUILD_OPTIMIZED}, y)
LOCAL_CFLAGS += -Os -pipe -mips32r2 -mtune=mips32r2 -funit-at-a-time
endif

ifeq (${CONFIG_RTR_DRIVER}, y)
LOCAL_CFLAGS += -DICM_RTR_DRIVER
endif

LOCAL_LDLIBS :=
LOCAL_LDLIBS += -Lpthread
LOCAL_LDLIBS += -Lm

include $(BUILD_EXECUTABLE)

endif # Global SUPPORT_ICM enable check
