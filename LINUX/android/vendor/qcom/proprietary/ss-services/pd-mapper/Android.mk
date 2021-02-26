PD_MAPPER_ROOT := $(call my-dir)
LOCAL_PATH := $(PD_MAPPER_ROOT)

ifeq ($(TARGET_PD_SERVICE_ENABLED),true)
include $(CLEAR_VARS)
LOCAL_CFLAGS += -DANDROID_BUILD
LOCAL_MODULE := pd-mapper
LOCAL_C_INCLUDES += $(LOCAL_PATH)/pd-mapper-svc \
		    $(LOCAL_PATH)/pd-mapper-idl \
		    external/connectivity/stlport/stlport \
		    $(TARGET_OUT_HEADERS)/common/inc/ \
		    $(TARGET_OUT_HEADERS)/libjson/inc/ \
		    $(TARGET_OUT_HEADERS)/qmi-framework/inc \
		    $(TARGET_OUT_HEADERS)/qmi/inc
LOCAL_SRC_FILES := pd-mapper-svc/pd-mapper.cpp \
	pd-mapper-idl/service_registry_locator_v01.c
ifeq ($(TARGET_COMPILE_WITH_MSM_KERNEL),true)
	LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
	LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
endif
ifeq ($(TARGET_BUILD_VARIANT),userdebug)
LOCAL_CFLAGS += -D__USERDEBUG__
endif
LOCAL_SHARED_LIBRARIES += libcutils \
			  libutils \
			  liblog \
			  libjson \
			  libqmi_cci \
			  libqmi_common_so \
			  libqmi_encdec \
			  libqmi_csi
LOCAL_MODULE_TAG := optional
LOCAL_CFLAGS += -Wall
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)
endif

include $(CLEAR_VARS)
LOCAL_CFLAGS += -DANDROID_BUILD
LOCAL_MODULE := libpdmapper
ifeq ($(TARGET_PD_SERVICE_ENABLED),true)
	LOCAL_CFLAGS += -DLIBPDMAPPER_SUPPORTED
else
$(warning libpdmapper not supported.Compiling stubbed version)
endif
LOCAL_C_INCLUDES += $(LOCAL_PATH)/pd-mapper-idl \
		    $(TARGET_OUT_HEADERS)/common/inc/ \
		    $(TARGET_OUT_HEADERS)/libjson/inc/ \
		    $(TARGET_OUT_HEADERS)/qmi-framework/inc \
		    $(TARGET_OUT_HEADERS)/qmi/inc
LOCAL_SRC_FILES := libpdmapper/libpdmapper.c \
	pd-mapper-idl/service_registry_locator_v01.c
ifeq ($(TARGET_COMPILE_WITH_MSM_KERNEL),true)
	LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
	LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
endif
LOCAL_SHARED_LIBRARIES += libcutils \
			  libutils \
			  liblog \
			  libjson \
			  libqmi_cci \
			  libqmi_common_so \
			  libqmi_encdec

LOCAL_COPY_HEADERS := libpdmapper/libpdmapper.h
LOCAL_COPY_HEADERS += pd-mapper-idl/service_registry_locator_v01.h
LOCAL_MODULE_TAG := optional
LOCAL_CFLAGS += -Wall
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)
