LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_COPY_HEADERS_TO := qmi/inc

LOCAL_COPY_HEADERS := qmi_ims_vt_v01.h
LOCAL_COPY_HEADERS += ip_multimedia_subsystem_video_telephony_v01.h
LOCAL_COPY_HEADERS += ip_multimedia_subsystem_presence_v01.h
LOCAL_COPY_HEADERS += ip_multimedia_subsystem_settings_v01.h
LOCAL_COPY_HEADERS += ip_multimedia_subsystem_application_v01.h
LOCAL_COPY_HEADERS += ip_multimedia_subsystem_rtp_v01.h
LOCAL_COPY_HEADERS += ip_multimedia_subsystem_dcm_v01.h
LOCAL_COPY_HEADERS += ip_multimedia_subsystem_ims_common_v01.h

LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi/src
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi/platform
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi/core/lib/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc

LOCAL_SHARED_LIBRARIES := libidl

LOCAL_SRC_FILES += qmi_ims_vt_v01.c
LOCAL_SRC_FILES += ip_multimedia_subsystem_video_telephony_v01.c
LOCAL_SRC_FILES += ip_multimedia_subsystem_presence_v01.c
LOCAL_SRC_FILES += ip_multimedia_subsystem_settings_v01.c
LOCAL_SRC_FILES += ip_multimedia_subsystem_application_v01.c
LOCAL_SRC_FILES += ip_multimedia_subsystem_rtp_v01.c
LOCAL_SRC_FILES += ip_multimedia_subsystem_dcm_v01.c
LOCAL_SRC_FILES += ip_multimedia_subsystem_ims_common_v01.c

LOCAL_MODULE:= libimsqmiservices

LOCAL_MODULE_TAGS := optional

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true


include $(BUILD_STATIC_LIBRARY)
