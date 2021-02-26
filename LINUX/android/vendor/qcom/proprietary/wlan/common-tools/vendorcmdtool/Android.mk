ifeq ($(BOARD_HAS_QCOM_WLAN), true)

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_REQUIRED_MODULES :=
LOCAL_CFLAGS += -Wno-unused-parameter -Wno-int-to-pointer-cast
LOCAL_CFLAGS += -Wno-maybe-uninitialized -Wno-parentheses

ifeq ($(PRODUCT_VENDOR_MOVE_ENABLED),true)
LOCAL_PROPRIETARY_MODULE := true
endif
LOCAL_MODULE := vendor_cmd_tool
LOCAL_CLANG := true
LOCAL_MODULE_TAGS := optional

LOCAL_C_INCLUDES :=
LOCAL_C_INCLUDES += \
    $(LOCAL_PATH) \
    external/libnl/include \
    external/libxml2/include \
    external/icu/icu4c/source/common

LOCAL_SHARED_LIBRARIES :=

ifneq ($(wildcard external/libnl),)
LOCAL_SHARED_LIBRARIES += libnl
endif

LOCAL_SHARED_LIBRARIES += libdl
LOCAL_SHARED_LIBRARIES += libxml2

LOCAL_SRC_FILES :=
LOCAL_SRC_FILES += nl_cmd_wrapper.c
LOCAL_SRC_FILES += ven_cmd_tool.c

LOCAL_LDLIBS += -Lpthread

include $(BUILD_EXECUTABLE)

endif # Global WLAN enable check
