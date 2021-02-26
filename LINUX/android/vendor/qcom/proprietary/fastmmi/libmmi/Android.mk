LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_OWNER := qti

LOCAL_MODULE:= libmmi

LOCAL_SRC_FILES := util_comm.cpp \
                   util_string.cpp \
                   util_system.cpp \
                   util_ui.cpp \
                   textview.cpp \
                   button.cpp \
                   listview.cpp \
                   layout.cpp \
                   nv.cpp \
                   events.cpp \
                   xmlparse.cpp \
                   lang.cpp \
                   board.cpp \
                   mmi_log.cpp \
                   interface.cpp


LOCAL_C_INCLUDES := external/libcxx/include \
                    external/skia/include/core \
                    external/libxml2/include \
                    external/icu/icu4c/source/common \
                    frameworks/base/libs/androidfw/include \
                    $(QC_PROP_ROOT)/diag/include \
                    $(QC_PROP_ROOT)/diag/src/ \
                    $(TARGET_OUT_HEADERS)/common/inc \
                    frameworks/native/libs/nativewindow/include

LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS := -Wall

ifeq ($(TARGET_SUPPORTS_WEARABLES),true)
LOCAL_CFLAGS += -DANDROID_WEARABLES
else
LOCAL_CFLAGS += -DANDROID
endif

LOCAL_SHARED_LIBRARIES := libcutils liblog libdiag libft2 libutils libxml2 libicuuc libc libui libbinder libgui libskia libc++ libc_malloc_debug

LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
ifeq ($(TARGET_COMPILE_WITH_MSM_KERNEL),true)
LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
endif

include $(BUILD_SHARED_LIBRARY)
