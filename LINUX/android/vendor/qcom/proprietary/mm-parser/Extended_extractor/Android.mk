LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)


# ---------------------------------------------------------------------------------
#    If TARGET_ENABLE_QC_AV_ENHANCEMENTS is defined, then compile AAL Code
# ---------------------------------------------------------------------------------
ifeq ($(TARGET_ENABLE_QC_AV_ENHANCEMENTS),true)

LOCAL_SRC_FILES+=                                    \
    src/QCExtractor.cpp

LOCAL_C_INCLUDES:=                                   \
    $(LOCAL_PATH)/../../common/inc                   \
    $(LOCAL_PATH)/inc                                \

LOCAL_SHARED_LIBRARIES :=       \
        libutils                \
        libcutils               \
        libdl                   \
        libstagefright          \
        liblog                  \

LOCAL_MODULE:= libExtendedExtractor

LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)

endif #TARGET_ENABLE_QC_AV_ENHANCEMENTS
