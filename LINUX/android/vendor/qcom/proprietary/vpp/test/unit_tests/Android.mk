OLD_LOCAL_PATH := $(LOCAL_PATH)
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
include $(LOCAL_PATH)/../../common.mk

LOCAL_FILES := \
	src/test_core.c \
	src/test_uc.c \
	src/test_queue.c \
	src/test_buf.c \
	src/test_pipeline.c \
	src/test_vpp.c \
	src/test_ip.c \
	src/test_reg.c \
	src/test_stats.c \
	src/test_vpp_utils.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../core/inc/ \
	$(LOCAL_PATH)/../utils/inc/ \
	$(LOCAL_PATH)/../../lib/utils/inc \
	$(LOCAL_PATH)/../../lib/core/inc \
	$(LOCAL_PATH)/../../inc

ifeq "$(VPP_TARGET_USES_GPU)" "YES"
LOCAL_FILES += \
	src/test_ip_gpu.c

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../../lib/ip/gpu/inc

endif

ifeq "$(VPP_TARGET_USES_HVX_CORE)" "YES"
LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/../../lib/ip/hvx_core/inc \
    $(TARGET_OUT_HEADERS)/vpp_library-noship
endif

ifeq "$(VPP_TARGET_USES_FRC)" "YES"
LOCAL_FILES += \
    src/test_ip_me.c \
    src/test_ip_mc.c

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/../../lib/ip/frc/inc \
    $(TARGET_OUT_HEADERS)/vpp_library-noship
endif

ifeq "$(VPP_TARGET_USES_HVX)" "YES"
LOCAL_FILES += \
    src/test_ip_hvx.c

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/../../lib/ip/hvx/inc \
    $(TARGET_OUT_HEADERS)/vpp_library-noship
endif

ifeq "$(VPP_TARGET_USES_HCP)" "YES"
LOCAL_FILES += \
    src/test_ip_hcp.c

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/../../lib/ip/hcp/inc \
    $(TARGET_OUT_HEADERS)/vpp_library-noship
endif

LOCAL_CFLAGS += -Wall -Werror -D_ANDROID_
LOCAL_CFLAGS += -DVPP_UNIT_TEST=1

LOCAL_32_BIT_ONLY := true
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_SRC_FILES := $(LOCAL_FILES)
LOCAL_MODULE := vpplibraryunittest
LOCAL_CLANG := true
LOCAL_SHARED_LIBRARIES += libcutils liblog libvpplibrary libvpptestutils
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti
include $(BUILD_EXECUTABLE)

LOCAL_PATH := $(OLD_LOCAL_PATH);
