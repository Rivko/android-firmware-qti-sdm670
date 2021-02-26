LOCAL_PATH := $(call my-dir)
QCRIL_DIR := ${LOCAL_PATH}/../..
#
include $(CLEAR_VARS)

LOCAL_CFLAGS               += -Wall -Werror $(qcril_cflags)
LOCAL_CXXFLAGS             += -std=c++14 $(qcril_cppflags)
LOCAL_CPPFLAGS             += -std=c++14 $(qcril_cppflags)
LOCAL_SRC_FILES            += $(call all-cpp-files-under, src)
LOCAL_SRC_FILES            += $(call all-c-files-under, src)

LOCAL_MODULE               := qcrilNasModule
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional
LOCAL_C_INCLUDES           += hardware/ril/include/telephony
LOCAL_C_INCLUDES           += $(QCRIL_DIR)/modules/uim
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/qcril
LOCAL_C_INCLUDES           += $(QCRIL_DIR)/qcril_qmi/
LOCAL_C_INCLUDES           += $(QCRIL_DIR)/qcril_qmi/utilities/
LOCAL_C_INCLUDES           += $(QCRIL_DIR)/common/uim/
LOCAL_C_INCLUDES           += $(QCRIL_DIR)/common/data/
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/data/inc
LOCAL_C_INCLUDES           += $(QCRIL_DIR)/qcril_qmi/ims_socket/
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/common/inc/
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/qmi-framework/inc/
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/qmi/inc/
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/qcril/framework
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/diag/include/
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/time-services/
LOCAL_C_INCLUDES           += external/nanopb-c
LOCAL_C_INCLUDES           += external/sqlite/dist/
LOCAL_SHARED_LIBRARIES     += libsqlite
LOCAL_COPY_HEADERS_TO      := qcril/modules/nas
LOCAL_COPY_HEADERS         += $(call all-named-files-under,*.h,../../include/modules/nas)
LOCAL_REQUIRED_MODULES     += qcril_features_def.h
LOCAL_ADDITIONAL_DEPENDENCIES += qcril_features_def.h
#LOCAL_WHOLE_STATIC_LIBRARIES += qcrilQmiModule
LOCAL_ADDITIONAL_DEPENDENCIES += qcril.db

include $(BUILD_STATIC_LIBRARY)
