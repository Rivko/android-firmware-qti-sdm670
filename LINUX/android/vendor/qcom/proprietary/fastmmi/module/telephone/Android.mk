LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_SRC_FILES := telephone.cpp

LOCAL_MODULE := mmi_telephone
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS := -Wall
LOCAL_CFLAGS += -g
LOCAL_C_INCLUDES := external/libcxx/include \
                    $(QC_PROP_ROOT)/fastmmi/libmmi \
                    $(QC_PROP_ROOT)/diag/include \
                    $(QC_PROP_ROOT)/diag/src/ \
                    $(TARGET_OUT_HEADERS)/common/inc \
                    $(TARGET_OUT_HEADERS)/qmi-framework/inc \
                    $(QC_PROP_ROOT)/qmi/inc \
                    $(QC_PROP_ROOT)/qmi/core/lib/inc \
                    $(QC_PROP_ROOT)/qmi/services \
                    $(QC_PROP_ROOT)/qmi/src \
                    $(QC_PROP_ROOT)/qmi/platform \
                    $(QC_PROP_ROOT)/qcril-qmi-services \

LOCAL_SHARED_LIBRARIES := libcutils
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libmmi
LOCAL_SHARED_LIBRARIES += libdiag
LOCAL_SHARED_LIBRARIES += libc++
LOCAL_SHARED_LIBRARIES += libidl
LOCAL_SHARED_LIBRARIES += libqmiservices
LOCAL_SHARED_LIBRARIES += librilqmiservices
LOCAL_SHARED_LIBRARIES += libqmi_cci

LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
ifeq ($(TARGET_COMPILE_WITH_MSM_KERNEL),true)
LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
endif

include $(BUILD_SHARED_LIBRARY)
