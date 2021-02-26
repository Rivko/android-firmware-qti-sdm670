##########################################################
#
# Makefile for VoicePrint modules - Android
#
##########################################################
# Qvop lib
##########################################################
# BOARD_SUPPORTS_QAHW - check equivalent to
# /hardware/qcom/audio/qahw_api
ifeq ($(strip $(BOARD_SUPPORTS_QAHW)),true)
LOCAL_PATH:= $(call my-dir)

##########################################################
# Qvop Tester LNX
##########################################################
include $(CLEAR_VARS)
LOCAL_CFLAGS += -Werror -Wno-unused-parameter
LOCAL_CFLAGS +=-DQVOP_DEBUG
LOCAL_CFLAGS +=-DQVOP_LNX_ONLY
LOCAL_C_INCLUDES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
                    $(LOCAL_PATH)/../../../../securemsm/QSEEComAPI \
                    $(TARGET_OUT_HEADERS)/common/inc \
                    $(LOCAL_PATH)/..        \
                    external/connectivity/stlport/stlport \
                    $(TARGET_OUT_HEADERS)/mm-audio/qahw_api/inc
LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
LOCAL_SRC_FILES:= QvopTesterLnx.cpp ../logfile.cpp
LOCAL_SHARED_LIBRARIES := libcutils libutils libqvop-lnx-service liblog libqahw
LOCAL_MODULE:= qvop-tester-lnx
LOCAL_MODULE_TAGS := eng
include $(BUILD_EXECUTABLE)
endif
