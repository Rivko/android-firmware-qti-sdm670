LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS               += -Wall -Werror -DRIL_SHLIB $(qcril_cflags)
LOCAL_CXXFLAGS             += -std=c++14
LOCAL_SRC_FILES            += $(call all-cpp-files-under, src)
LOCAL_SRC_FILES            += $(call all-c-files-under, src)
LOCAL_SRC_FILES            += $(call all-cc-files-under, src)

LOCAL_MODULE               := libqcrilFramework
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/qcril
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/common/inc
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/diag/include
LOCAL_C_INCLUDES           += hardware/ril/include/telephony
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/qmi/inc #This is due to am requiring voice_service_v02.h
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/qmi-framework/inc
LOCAL_C_INCLUDES           += external/nanopb-c/

LOCAL_SHARED_LIBRARIES += libdiag \
	liblog \

LOCAL_SHARED_LIBRARIES += libsettings

LOCAL_COPY_HEADERS_TO      := qcril/framework
LOCAL_COPY_HEADERS         += $(call all-named-files-under,*.h,../include/framework)
LOCAL_ADDITIONAL_DEPENDENCIES += qcril_features_def.h

include $(BUILD_SHARED_LIBRARY)
