LOCAL_PATH := $(call my-dir)
QCRIL_DIR := ${LOCAL_PATH}/../..

include $(CLEAR_VARS)

PPCAL_CFLAGS               += -Wall -Werror $(qcril_cflags)
LOCAL_CXXFLAGS             += -std=c++14 $(qcril_cppflags)
LOCAL_CPPFLAGS             += -std=c++14 $(qcril_cppflags)
LOCAL_SRC_FILES            += $(call all-cpp-files-under, src)

LOCAL_MODULE               := qcrilSmsModule
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional

LOCAL_C_INCLUDES           += hardware/ril/include/telephony
LOCAL_C_INCLUDES           += external/nanopb-c/
LOCAL_C_INCLUDES           += external/sqlite/dist/
LOCAL_C_INCLUDES           += $(LOCAL_PATH)/../../qcril_qmi
LOCAL_C_INCLUDES           += $(LOCAL_PATH)/../../qcril_qmi/nanopb_utils
LOCAL_C_INCLUDES           += $(LOCAL_PATH)/../../qcril_qmi/oem_socket
LOCAL_C_INCLUDES           += $(LOCAL_PATH)/../../qcril_qmi/radio_config
LOCAL_C_INCLUDES           += $(LOCAL_PATH)/../../qcril_qmi/utilities
LOCAL_C_INCLUDES           += $(LOCAL_PATH)/../../qcril_qmi/ims_socket
LOCAL_C_INCLUDES           += $(LOCAL_PATH)/../../common/uim
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/qcril
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/common/inc
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/qmi-framework/inc/
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/qmi/inc
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/diag/include

LOCAL_COPY_HEADERS_TO      := qcril/modules/sms
LOCAL_COPY_HEADERS         += $(call all-named-files-under,*.h,../../include/modules/sms)
LOCAL_COPY_HEADERS         += src/qcril_qmi_sms.h
LOCAL_COPY_HEADERS         += src/qcril_qmi_sms_errors.h
LOCAL_COPY_HEADERS         += src/qcril_sms_legacy.h
LOCAL_ADDITIONAL_DEPENDENCIES += qcril_features_def.h

include $(BUILD_STATIC_LIBRARY)

