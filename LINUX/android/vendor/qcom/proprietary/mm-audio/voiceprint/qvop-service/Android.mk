##########################################################
#
# Makefile for VoicePrint modules - Android
#
##########################################################
# Qvop lib
##########################################################
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CFLAGS += -Werror -Wno-unused-parameter -Wno-ignored-attributes
# debug flag
LOCAL_CFLAGS +=-DQVOP_DEBUG
LOCAL_C_INCLUDES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
                    $(LOCAL_PATH)/../../../securemsm/QSEEComAPI \
                    $(TARGET_OUT_HEADERS)/common/inc \
                    $(LOCAL_PATH)/..          \
                    $(LOCAL_PATH)/../../../diag/include \
                    external/connectivity/stlport/stlport
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/techpack/audio/include
LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
ifneq ($(filter sdm710 qcs605,$(TARGET_BOARD_PLATFORM)),)
  LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/vendor/qcom/opensource/audio-kernel/include
  LOCAL_ADDITIONAL_DEPENDENCIES += $(BOARD_VENDOR_KERNEL_MODULES)
endif
LOCAL_SRC_FILES := \
    logfile.cpp \
    QvopCommandService.cpp \
    qvop_interface.c \
    qvop_qsee_interface.c \
    qvop_qsee.c \
 QtiVoicePrintService.cpp \
 QtiVoicePrintServiceCallback.cpp

LOCAL_SHARED_LIBRARIES := \
 libandroid_runtime \
 libc \
 libcutils \
 liblog \
 libdiag \
 libutils \
 libQSEEComAPI \
 libhidlbase \
 libhidltransport \
 libhwbinder \
 vendor.qti.voiceprint@1.0

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE:= libqvop-service
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)

##########################################################
#
# Makefile for VoicePrint modules - Linux
#
##########################################################
# Qvop lib
##########################################################
# BOARD_SUPPORTS_QAHW - check equivalent to
# /hardware/qcom/audio/qahw_api
ifeq ($(strip $(BOARD_SUPPORTS_QAHW)),true)
include $(CLEAR_VARS)
LOCAL_CFLAGS += -Werror -Wno-unused-parameter -Wno-ignored-attributes
# debug flag
LOCAL_CFLAGS +=-DQVOP_DEBUG
LOCAL_CFLAGS +=-DQVOP_LNX_ONLY
LOCAL_C_INCLUDES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
                    $(LOCAL_PATH)/../../../securemsm/QSEEComAPI \
                    $(TARGET_OUT_HEADERS)/common/inc \
                    $(LOCAL_PATH)/..          \
                    $(LOCAL_PATH)/../../../diag/include \
                    external/connectivity/stlport/stlport
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/techpack/audio/include
LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
ifneq ($(filter sdm710 qcs605,$(TARGET_BOARD_PLATFORM)),)
  LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/vendor/qcom/opensource/audio-kernel/include
  LOCAL_ADDITIONAL_DEPENDENCIES += $(BOARD_VENDOR_KERNEL_MODULES)
endif
LOCAL_SRC_FILES := \
    logfile.cpp \
    QvopCommandService.cpp \
    QvopService.cpp \
    qvop_interface.c \
    qvop_qsee_interface.c \
    qvop_qsee.c

LOCAL_SHARED_LIBRARIES := libc
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libdiag
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libQSEEComAPI

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE:= libqvop-lnx-service
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)
endif
##########################################################
# Qvop Daemon
##########################################################
include $(CLEAR_VARS)
LOCAL_CFLAGS += -Werror -Wno-unused-parameter -Wno-error=date-time
LOCAL_CFLAGS +=-DQVOP_DEBUG
LOCAL_C_INCLUDES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
                    $(LOCAL_PATH)/../../../securemsm/QSEEComAPI \
                    $(TARGET_OUT_HEADERS)/common/inc \
                    $(LOCAL_PATH)/..          \
                    external/connectivity/stlport/stlport
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/techpack/audio/include
LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
ifneq ($(filter sdm710 qcs605,$(TARGET_BOARD_PLATFORM)),)
  LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/vendor/qcom/opensource/audio-kernel/include
  LOCAL_ADDITIONAL_DEPENDENCIES += $(BOARD_VENDOR_KERNEL_MODULES)
endif
LOCAL_SRC_FILES:= QvopDaemon.cpp logfile.cpp

LOCAL_SHARED_LIBRARIES := libqvop-service
LOCAL_SHARED_LIBRARIES += libhwbinder
LOCAL_SHARED_LIBRARIES += libandroid_runtime
LOCAL_SHARED_LIBRARIES += libc
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libhidlbase
LOCAL_SHARED_LIBRARIES += libhidltransport
LOCAL_SHARED_LIBRARIES += vendor.qti.voiceprint@1.0

LOCAL_MODULE:= qvop-daemon
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)

##########################################################
# Files required by Algorithm
##########################################################
include $(CLEAR_VARS)
LOCAL_MODULE := cmudict.bin
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_ETC)/qvop/
LOCAL_SRC_FILES := calib/cmudict.bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := poc_64_hmm.gmm
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_ETC)/qvop/
LOCAL_SRC_FILES := calib/poc_64_hmm.gmm
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := noisesample.bin
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_ETC)/qvop/
LOCAL_SRC_FILES := calib/noisesample.bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := antispoofing.bin
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_ETC)/qvop/
LOCAL_SRC_FILES := calib/antispoofing.bin
include $(BUILD_PREBUILT)


##########################################################
# Test app for linux.
##########################################################
# BOARD_SUPPORTS_QAHW - check equivalent to
# /hardware/qcom/audio/qahw_api
ifeq ($(strip $(BOARD_SUPPORTS_QAHW)),true)
include $(LOCAL_PATH)/test-app-lnx/Android.mk
endif
