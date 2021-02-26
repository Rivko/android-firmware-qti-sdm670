ifeq ($(TARGET_PD_SERVICE_ENABLED),true)
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_CFLAGS += -DANDROID_BUILD
LOCAL_MODULE := pd-api-test
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc/ \
		    $(TARGET_OUT_HEADERS)/qmi-framework/inc \
		    $(TARGET_OUT_HEADERS)/qmi/inc \
		    $(TARGET_OUT_HEADERS)/libpdmapper/inc \
		    $(TARGET_OUT_HEADERS)/libpdnotifier/inc
LOCAL_SRC_FILES := pd-api-test.c
ifeq ($(TARGET_COMPILE_WITH_MSM_KERNEL),true)
	LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
	LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
endif
LOCAL_SHARED_LIBRARIES += libcutils \
			  libutils \
			  liblog \
			  libpdmapper \
			  libqmi_cci \
			  libqmi_common_so \
			  libqmi_encdec \
			  libpdnotifier
LOCAL_MODULE_TAG := debug
LOCAL_CFLAGS += -Wall
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)
endif
