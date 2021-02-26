LOCAL_PATH := $(call my-dir)
QCRIL_DIR := ${LOCAL_PATH}/../..
#
include $(CLEAR_VARS)

LOCAL_CFLAGS               += -Wall -Werror $(qcril_cflags)
LOCAL_CXXFLAGS             += -std=c++14 $(qcril_cppflags)
LOCAL_CPPFLAGS             += -std=c++14 $(qcril_cppflags)
LOCAL_SRC_FILES            += $(call all-cpp-files-under, src)

LOCAL_MODULE               := qcrilImsModule
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional
LOCAL_SHARED_LIBRARIES	   += libhidlbase
LOCAL_SHARED_LIBRARIES	   += libhidltransport
LOCAL_SHARED_LIBRARIES	   += libhwbinder
LOCAL_SHARED_LIBRARIES	   += vendor.qti.hardware.radio.ims@1.0_vendor
LOCAL_C_INCLUDES           += hardware/ril/include/telephony
LOCAL_C_INCLUDES           += external/nanopb-c/
# IMSA REFACTOR TODO: remove dependency to these paths
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/data/inc
LOCAL_C_INCLUDES           += $(LOCAL_PATH)/../../../data/dsutils/inc
LOCAL_C_INCLUDES           += $(LOCAL_PATH)/../../../data/dsi_netctrl/inc
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/qcril
LOCAL_C_INCLUDES            += $(LOCAL_PATH)/../../common/data
LOCAL_C_INCLUDES           += $(LOCAL_PATH)/../../qcril_qmi
LOCAL_C_INCLUDES           += $(LOCAL_PATH)/../../qcril_qmi/utilities
LOCAL_C_INCLUDES           += $(LOCAL_PATH)/src/ims_socket
LOCAL_C_INCLUDES           += $(LOCAL_PATH)/../../qcril_qmi/radio_config
LOCAL_C_INCLUDES           += $(LOCAL_PATH)/../../qcril_qmi/nanopb_utils/
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/qcril/framework
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/common/inc/
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/qmi-framework/inc/
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/qmi/inc/
LOCAL_C_INCLUDES           += system/core/include/
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/diag/include/
LOCAL_C_INCLUDES           += $(LOCAL_PATH)/../../common/uim/
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/qcril/modules/nas

LOCAL_COPY_HEADERS_TO      := qcril/modules/ims
LOCAL_COPY_HEADERS         += $(call all-named-files-under,*.h,../../include/modules/ims)
LOCAL_COPY_HEADERS         += src/ims_socket/imsIF.pb.h
LOCAL_COPY_HEADERS         += src/ims_socket/qcril_qmi_ims_radio_agent.h
LOCAL_COPY_HEADERS         += src/ims_socket/qcril_qmi_ims_misc.h
LOCAL_COPY_HEADERS         += src/ims_socket/qcril_qmi_ims_flow_control.h
LOCAL_COPY_HEADERS         += src/qcril_qmi_ims.h
LOCAL_COPY_HEADERS         += src/qcril_qmi_imsa.h
LOCAL_COPY_HEADERS         += src/qcril_qmi_imss.h
LOCAL_COPY_HEADERS         += src/qcril_qmi_imss_v02.h
LOCAL_COPY_HEADERS         += src/qcril_qmi_imss_qmi.h
LOCAL_COPY_HEADERS         += src/qcril_qmi_imss_internal.h
LOCAL_ADDITIONAL_DEPENDENCIES += qcril_features_def.h

include $(BUILD_STATIC_LIBRARY)
