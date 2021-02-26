LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CLANG := true
LOCAL_PROPRIETARY_MODULE := true
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/libtcmd \
			  $(LOCAL_PATH)/../libathtestcmd \
			  $(LOCAL_PATH)/../libathtestcmd/include \
			  $(LOCAL_PATH)/libtlvutil \
			  $(LOCAL_PATH)/libtlvutil/common \
			  external/zlib \

LOCAL_MODULE := myftm
BOARD_HAS_ATH_WLAN_AR6320:= true

ifeq ($(BOARD_HAS_ATH_WLAN_AR6004),true)
LOCAL_CFLAGS+= -DBOARD_HAS_ATH_WLAN_AR6004
endif


LOCAL_SRC_FILES:= \
    myftm_wlan.c \
    myftm_main.c \
    myftm_qtip.c

LOCAL_MODULE_TAGS := optional

#LOCAL_LDLIBS += -lpthread -lrt

LOCAL_CFLAGS+= -DCONFIG_FTM_WLAN_AUTOLOAD
LOCAL_CFLAGS+= -DCONFIG_FTM_WLAN
LOCAL_CFLAGS+= -DBOARD_HAS_ATH_WLAN_AR6320

LOCAL_SHARED_LIBRARIES += libcutils libz
LOCAL_WHOLE_STATIC_LIBRARIES := libtestcmd6174

ifneq ($(wildcard system/core/libnl_2),)
# ICS ships with libnl 2.0
LOCAL_SHARED_LIBRARIES += libnl_2
else
LOCAL_SHARED_LIBRARIES += libnl
endif

LOCAL_MODULE_OWNER := qti

include $(BUILD_EXECUTABLE)
