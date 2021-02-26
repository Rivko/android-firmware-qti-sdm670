OLD_LOCAL_PATH := $(LOCAL_PATH)
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

include $(LOCAL_PATH)/../common.mk

#=============================================================================
# Core Defines
#=============================================================================
LOCAL_FILES := \
	utils/src/vpp_queue.c \
	utils/src/vpp_utils.c \
	utils/src/vpp_ion.c \
	core/src/vpp.c \
	core/src/vpp_ip.c \
	core/src/vpp_buf.c \
	core/src/vpp_uc.c \
	core/src/vpp_stats.c \
	core/src/vpp_callback.c \
	core/src/vpp_pipeline.c

LOCAL_CORE_INCLUDES := \
	$(LOCAL_PATH)/../inc \
	$(LOCAL_PATH)/core/inc \
	$(LOCAL_PATH)/utils/inc \
	$(TOP)/hardware/qcom/media/mm-core/inc

#=============================================================================
# GPU Defines
#=============================================================================
LOCAL_GPU_FILES := \
	ip/gpu/src/vpp_ip_gpu.c

LOCAL_GPU_INCLUDES := \
	$(LOCAL_PATH)/ip/gpu/inc \
	$(TARGET_OUT_HEADERS)/vpp_library-noship

LOCAL_GPU_SHARED_LIBS := \
	libmmsw_detail_enhancement \
	libmmsw_opencl \
	libmmsw_platform \
	libmmsw_math

#=============================================================================
# HVX Core Defines
#=============================================================================
LOCAL_HVX_CORE_FILES := \
	ip/hvx_core/src/vpp_ip_hvx_core.c \
	ip/hvx_core/src/vpp_ip_hvx_debug.c

LOCAL_HVX_CORE_INCLUDES := \
	$(LOCAL_PATH)/ip/hvx_core/inc \
	$(TARGET_OUT_HEADERS)/vpp_library-noship

LOCAL_HVX_CORE_SHARED_LIBS := \
	libvpphvx \
	$(DSP_RPC_LIB)

#=============================================================================
# HVX Defines
#=============================================================================
LOCAL_HVX_FILES := \
	ip/hvx/src/vpp_ip_hvx.c

LOCAL_HVX_INCLUDES := \
	$(LOCAL_PATH)/ip/hvx/inc \
	$(TARGET_OUT_HEADERS)/vpp_library-noship

LOCAL_HVX_SHARED_LIBS := \
	libvpphvx \
	$(DSP_RPC_LIB)

#=============================================================================
# FRC Defines
#=============================================================================
LOCAL_FRC_FILES := \
	ip/frc/src/vpp_ip_frc_core.c \
	ip/frc/src/vpp_ip_frc_mc.c \
	ip/frc/src/vpp_ip_frc_me.c \
	ip/frc/src/vpp_ip_frc_me_tme.c

LOCAL_FRC_INCLUDES := \
	$(LOCAL_PATH)/ip/frc/inc \
	$(TARGET_OUT_HEADERS)/vpp_library-noship

LOCAL_FRC_SHARED_LIBS := \
	$(DSP_RPC_LIB)

#=============================================================================
# HCP Defines
#=============================================================================
LOCAL_HCP_FILES := \
	ip/hcp/src/vpp_ip_hcp_dbg.c \
	ip/hcp/src/vpp_ip_hcp_hfi.c \
	ip/hcp/src/vpp_ip_hcp.c \
	ip/hcp/src/vpp_ip_hcp_tunings.c \

LOCAL_HCP_INCLUDES := \
	$(LOCAL_PATH)/ip/hcp/inc \
	$(TARGET_OUT_HEADERS)/vpp_library-noship

LOCAL_HCP_SHARED_LIBS := \
	libvpphcp \
	$(DSP_RPC_LIB)

#=============================================================================
# msm8937 target
#=============================================================================
ifeq ($(LOCAL_BOARD_PLATFORM),msm8937)
LOCAL_PLATFORM_FILES := \
	chip/msm8937/vpp_reg_8937.c

#=============================================================================
# msm8952 target
#=============================================================================
else ifeq ($(LOCAL_BOARD_PLATFORM),msm8952)
LOCAL_PLATFORM_FILES := \
	chip/msm8956/vpp_reg_8956.c

#=============================================================================
# msm8953 target
#=============================================================================
else ifeq ($(LOCAL_BOARD_PLATFORM),msm8953)
LOCAL_PLATFORM_FILES := \
	chip/msm8953/vpp_reg_8953.c

#=============================================================================
# msm8996 target
#=============================================================================
else ifeq ($(LOCAL_BOARD_PLATFORM),msm8996)
LOCAL_PLATFORM_FILES := \
	chip/msm8996/vpp_reg_8996.c

#=============================================================================
# msm8998 target
#=============================================================================
else ifeq ($(LOCAL_BOARD_PLATFORM),msm8998)
LOCAL_PLATFORM_FILES := \
	chip/msm8998/vpp_reg_msm8998.c

#=============================================================================
# sdm660 target
#=============================================================================
else ifeq ($(LOCAL_BOARD_PLATFORM),sdm660)
LOCAL_PLATFORM_FILES := \
	chip/sdm660/vpp_reg_sdm660.c

#=============================================================================
# sdm670 target
#=============================================================================
else ifeq ($(LOCAL_BOARD_PLATFORM),sdm670)
LOCAL_PLATFORM_FILES := \
	chip/sdm670/vpp_reg_sdm670.c

#=============================================================================
# sdm845 target
#=============================================================================
else ifeq ($(LOCAL_BOARD_PLATFORM),sdm845)
LOCAL_PLATFORM_FILES := \
	chip/sdm845/vpp_reg_sdm845.c

#=============================================================================
# others
#=============================================================================
else
LOCAL_PLATFORM_FILES := \
	chip/default/vpp_reg_default.c
endif

ifeq "$(VPP_TARGET_USES_GPU)" "YES"
LOCAL_PLATFORM_FILES += $(LOCAL_GPU_FILES)
LOCAL_PLATFORM_INCLUDES += $(LOCAL_GPU_INCLUDES)
LOCAL_PLATFORM_SHARED_LIBRARIES += $(LOCAL_GPU_SHARED_LIBS)
endif

ifeq "$(VPP_TARGET_USES_HVX_CORE)" "YES"
LOCAL_PLATFORM_FILES += $(LOCAL_HVX_CORE_FILES)
LOCAL_PLATFORM_INCLUDES += $(LOCAL_HVX_CORE_INCLUDES)
LOCAL_PLATFORM_SHARED_LIBRARIES += $(LOCAL_HVX_CORE_SHARED_LIBS)
endif

ifeq "$(VPP_TARGET_USES_HVX)" "YES"
LOCAL_PLATFORM_FILES += $(LOCAL_HVX_FILES)
LOCAL_PLATFORM_INCLUDES += $(LOCAL_HVX_INCLUDES)
LOCAL_PLATFORM_SHARED_LIBRARIES += $(LOCAL_HVX_SHARED_LIBS)
endif

ifeq "$(VPP_TARGET_USES_FRC)" "YES"
LOCAL_PLATFORM_FILES += $(LOCAL_FRC_FILES)
LOCAL_PLATFORM_INCLUDES += $(LOCAL_FRC_INCLUDES)
LOCAL_PLATFORM_SHARED_LIBRARIES += $(LOCAL_FRC_SHARED_LIBS)
endif

ifeq "$(VPP_TARGET_USES_HCP)" "YES"
LOCAL_PLATFORM_FILES += $(LOCAL_HCP_FILES)
LOCAL_PLATFORM_INCLUDES += $(LOCAL_HCP_INCLUDES)
LOCAL_PLATFORM_SHARED_LIBRARIES += $(LOCAL_HCP_SHARED_LIBS)
endif

LOCAL_C_INCLUDES := \
	$(LOCAL_CORE_INCLUDES) \
	$(LOCAL_PLATFORM_INCLUDES) \
	$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
	$(TARGET_OUT_HEADERS)/qcom/display

#LOCAL_CFLAGS += -Wall -Werror
LOCAL_CFLAGS += -DVPP_LIBRARY -D_ANDROID_
LOCAL_COPY_HEADERS_TO :=
LOCAL_COPY_HEADERS +=
LOCAL_SRC_FILES := $(LOCAL_FILES) $(LOCAL_PLATFORM_FILES)
LOCAL_MODULE := libvpplibrary
LOCAL_PRELINK_MODULE := false
LOCAL_SHARED_LIBRARIES := libdl libcutils liblog libqdMetaData
LOCAL_SHARED_LIBRARIES += $(LOCAL_PLATFORM_SHARED_LIBRARIES)
LOCAL_MODULE_TAGS := optional
LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti
include $(BUILD_SHARED_LIBRARY)

LOCAL_PATH := $(OLD_LOCAL_PATH);
