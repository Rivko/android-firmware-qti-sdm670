LOCAL_PATH := $(call my-dir)
QCRIL_DIR := ${LOCAL_PATH}/../..
#
include $(CLEAR_VARS)

LOCAL_CFLAGS               += -Wall -Werror $(qcril_cflags)
LOCAL_CXXFLAGS             += -std=c++14 $(qcril_cppflags)
LOCAL_CPPFLAGS             += -std=c++14 $(qcril_cppflags)
LOCAL_SRC_FILES            += $(call all-cpp-files-under, src)

LOCAL_MODULE               := qcrilLpaModule
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional
LOCAL_C_INCLUDES           += hardware/ril/include/telephony
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/qcril
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/common/inc/
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/qmi-framework/inc/
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/qmi/inc/
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.lpa@1.0_vendor

LOCAL_COPY_HEADERS_TO      := qcril/modules/lpa
LOCAL_COPY_HEADERS         += $(call all-named-files-under,*.h,../../include/modules/lpa)
LOCAL_REQUIRED_MODULES     += qcril_features_def.h
LOCAL_ADDITIONAL_DEPENDENCIES += qcril_features_def.h

include $(BUILD_STATIC_LIBRARY)
