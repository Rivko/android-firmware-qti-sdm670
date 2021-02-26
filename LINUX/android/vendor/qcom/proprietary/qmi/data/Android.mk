LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS := -Wno-missing-field-initializers

LOCAL_COPY_HEADERS_TO := qmi/inc

LOCAL_COPY_HEADERS += wireless_data_service_common_v01.h
LOCAL_COPY_HEADERS += wireless_data_service_v01.h
LOCAL_COPY_HEADERS += wireless_data_administrative_service_v01.h
LOCAL_COPY_HEADERS += data_system_determination_v01.h
LOCAL_COPY_HEADERS += data_filter_service_v01.h
LOCAL_COPY_HEADERS += data_port_mapper_v01.h
LOCAL_COPY_HEADERS += data_common_v01.h
LOCAL_COPY_HEADERS += quality_of_service_v01.h
LOCAL_COPY_HEADERS += over_the_top_v01.h
LOCAL_COPY_HEADERS += application_traffic_pairing_v01.h
LOCAL_COPY_HEADERS += authentication_service_v01.h

LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc
LOCAL_C_INCLUDES += vendor/qcom/proprietary/qmi-framework/inc

LOCAL_SRC_FILES += wireless_data_service_common_v01.c
LOCAL_SRC_FILES += wireless_data_service_v01.c
LOCAL_SRC_FILES += wireless_data_administrative_service_v01.c
LOCAL_SRC_FILES += data_system_determination_v01.c
LOCAL_SRC_FILES += data_port_mapper_v01.c
LOCAL_SRC_FILES += data_common_v01.c
LOCAL_SRC_FILES += data_filter_service_v01.c
LOCAL_SRC_FILES += quality_of_service_v01.c
LOCAL_SRC_FILES += over_the_top_v01.c
LOCAL_SRC_FILES += application_traffic_pairing_v01.c
LOCAL_SRC_FILES += authentication_service_v01.c

LOCAL_MODULE:= libdataqmiservices
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_STATIC_LIBRARY)

