LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

filetoadd = bionic/libc/kernel/arch-arm/asm/posix_types.h
LOCAL_CFLAGS += $(shell if [ -a $(filetoadd) ] ; then echo -include $(filetoadd) ; fi ;)
filetoadd = bionic/libc/kernel/arch-arm/asm/byteorder.h
LOCAL_CFLAGS += $(shell if [ -a $(filetoadd) ] ; then echo -include $(filetoadd) ; fi ;)
filetoadd = ../../qmi-framework/inc/qmi_client.h
LOCAL_CFLAGS += -include $(filetoadd)

LOCAL_CFLAGS += -DFEATURE_QMI_ANDROID
LOCAL_CFLAGS += -DFEATURE_QMI_IWLAN
LOCAL_CFLAGS += -DFEATURE_QMUXD_DEPRECATION

# Logging Features. Turn any one ON at any time

#LOCAL_CFLAGS += -DFEATURE_DATA_LOG_STDERR
#LOCAL_CFLAGS += -DFEATURE_DATA_LOG_ADB
LOCAL_CFLAGS += -DFEATURE_DATA_LOG_QXDM

ifeq ($(call is-board-platform,msm7630_fusion),true)
LOCAL_CFLAGS += -DFEATURE_QMI_FUSION
LOCAL_CFLAGS += -DFEATURE_WAIT_FOR_MODEM_HACK=0
endif

ifeq ($(call is-board-platform-in-list,msm8660 msm8960),true)
LOCAL_CFLAGS += -DFEATURE_WAIT_FOR_MODEM_HACK=0
endif

LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../qmi-framework/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../qmi-framework/qcci/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../qmi-framework/common/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../src
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../platform
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../core/lib/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../data/configdb/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../data/dsutils/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/data/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/diag/include
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi/inc
LOCAL_C_INCLUDES += system/core/include/cutils
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include

LOCAL_SRC_FILES := linux_qmi_qmux_if_server.c
LOCAL_SRC_FILES += qmi_platform_qmux_io.c
LOCAL_SRC_FILES += qmi_platform.c
LOCAL_SRC_FILES += qmi_platform_config.c
LOCAL_SRC_FILES += qmi_qmux.c
LOCAL_SRC_FILES += ../src/qmi_util.c


ifeq ($(call is-board-platform-in-list,apq8084 msm8916 msm8939 msm8909 msm8992 msm8994 msm8952 msm8976 msm8226),true)
  LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

  LOCAL_MODULE:= qmuxd

  LOCAL_PROPRIETARY_MODULE := true
  LOCAL_MODULE_OWNER := qti

  LOCAL_MODULE_TAGS := optional

  LOCAL_SHARED_LIBRARIES := liblog libdiag
  LOCAL_SHARED_LIBRARIES += libcutils
  LOCAL_SHARED_LIBRARIES += libconfigdb
  LOCAL_SHARED_LIBRARIES += libqmiservices
  LOCAL_SHARED_LIBRARIES += libqmi_cci
  LOCAL_SHARED_LIBRARIES += libqmi_common_so
  LOCAL_SHARED_LIBRARIES += libqmi_client_qmux
  LOCAL_SHARED_LIBRARIES += libqmi
  LOCAL_SHARED_LIBRARIES += libdsutils

  LOCAL_CLANG := true

  LDLIBS += -lpthread

  include $(BUILD_EXECUTABLE)

  include $(CLEAR_VARS)
  LOCAL_MODULE := qmi_config.xml
  LOCAL_MODULE_CLASS := ETC
  LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_ETC)/data
  LOCAL_MODULE_TAGS := optional
  LOCAL_SRC_FILES := qmi_config.xml
  LOCAL_MODULE_OWNER := qti
  include $(BUILD_PREBUILT)

endif

