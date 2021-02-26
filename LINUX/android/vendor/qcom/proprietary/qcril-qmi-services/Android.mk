LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_COPY_HEADERS_TO := qmi/inc

LOCAL_COPY_HEADERS := access_terminal_service_v01.h
LOCAL_COPY_HEADERS += card_application_toolkit_v02.h
LOCAL_COPY_HEADERS += circuit_switched_video_telephony_v01.h
LOCAL_COPY_HEADERS += coexistence_manager_v01.h
LOCAL_COPY_HEADERS += coexistence_service_v01.h
LOCAL_COPY_HEADERS += device_management_service_v01.h
LOCAL_COPY_HEADERS += network_access_service_common_v01.h
LOCAL_COPY_HEADERS += network_access_service_v01.h
LOCAL_COPY_HEADERS += persistent_device_configuration_v01.h
LOCAL_COPY_HEADERS += phonebook_manager_service_v01.h
LOCAL_COPY_HEADERS += radio_frequency_radiated_performance_enhancement_v01.h
LOCAL_COPY_HEADERS += specific_absorption_rate_v01.h
LOCAL_COPY_HEADERS += subsystem_control_v02.h
LOCAL_COPY_HEADERS += user_identity_module_remote_v01.h
LOCAL_COPY_HEADERS += user_identity_module_v01.h
LOCAL_COPY_HEADERS += user_identity_module_http_v01.h
LOCAL_COPY_HEADERS += voice_service_common_v02.h
LOCAL_COPY_HEADERS += voice_service_v02.h
LOCAL_COPY_HEADERS += voice_nas_services_common_v01.h
LOCAL_COPY_HEADERS += wireless_messaging_service_v01.h
LOCAL_COPY_HEADERS += lte_v01.h

LOCAL_SHARED_LIBRARIES := libidl

LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi/inc

LOCAL_SRC_FILES := access_terminal_service_v01.c
LOCAL_SRC_FILES += card_application_toolkit_v02.c
LOCAL_SRC_FILES += circuit_switched_video_telephony_v01.c
LOCAL_SRC_FILES += coexistence_manager_v01.c
LOCAL_SRC_FILES += coexistence_service_v01.c
LOCAL_SRC_FILES += device_management_service_v01.c
LOCAL_SRC_FILES += network_access_service_common_v01.c
LOCAL_SRC_FILES += network_access_service_v01.c
LOCAL_SRC_FILES += persistent_device_configuration_v01.c
LOCAL_SRC_FILES += phonebook_manager_service_v01.c
LOCAL_SRC_FILES += radio_frequency_radiated_performance_enhancement_v01.c
LOCAL_SRC_FILES += specific_absorption_rate_v01.c
LOCAL_SRC_FILES += subsystem_control_v02.c
LOCAL_SRC_FILES += user_identity_module_remote_v01.c
LOCAL_SRC_FILES += user_identity_module_v01.c
LOCAL_SRC_FILES += user_identity_module_http_v01.c
LOCAL_SRC_FILES += voice_service_common_v02.c
LOCAL_SRC_FILES += voice_service_v02.c
LOCAL_SRC_FILES += voice_nas_services_common_v01.c
LOCAL_SRC_FILES += wireless_messaging_service_v01.c
LOCAL_SRC_FILES += lte_v01.c

LOCAL_MODULE := librilqmiservices
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_STATIC_LIBRARY)
