LOCAL_PATH := $(call my-dir)
QCRIL_DIR := ${LOCAL_PATH}/../..

include $(CLEAR_VARS)

LOCAL_CFLAGS               += -Wall -Werror $(qcril_cflags) -DFEATURE_QCRIL_UIM_QMI -DFEATURE_QCRIL_UIM_ISIM_REFRESH -DFEATURE_QCRIL_QMI_CAT
LOCAL_CXXFLAGS             += -std=c++14 $(qcril_cppflags)
LOCAL_CPPFLAGS             += -std=c++14 $(qcril_cppflags)  -DFEATURE_QCRIL_UIM_QMI -DFEATURE_QCRIL_UIM_ISIM_REFRESH -DFEATURE_QCRIL_QMI_CAT

LOCAL_SRC_FILES            += $(call all-cpp-files-under, src)
LOCAL_SRC_FILES            += $(call all-c-files-under, src)

LOCAL_MODULE               := qcrilUimModule
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional
LOCAL_C_INCLUDES           += hardware/ril/include/telephony
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/qcril
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/common/inc/
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/qmi-framework/inc/
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/qmi/inc
LOCAL_SHARED_LIBRARIES     += libril-qc-hal-qmi
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.uim@1.0_vendor
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.uim@1.1_vendor
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.uim_remote_server@1.0_vendor

LOCAL_COPY_HEADERS_TO      := qcril/modules/uim
LOCAL_COPY_HEADERS         += $(call all-named-files-under,*.h,../../include/modules/uim)
LOCAL_ADDITIONAL_DEPENDENCIES += qcril_features_def.h

include $(BUILD_STATIC_LIBRARY)
