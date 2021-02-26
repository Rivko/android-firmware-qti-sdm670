ifneq ($(call is-platform-sdk-version-at-least,26),true)
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES += \
	BneSEPowerManager.cpp \
	eSEPowerManager.cpp \
	IeSEPowerManager.cpp \
	eSEPowerManagerServer.cpp

LOCAL_C_INCLUDES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
					$(TOP)/vendor/qcom/proprietary/securemsm/QSEEComAPI \
					$(TARGET_OUT_HEADERS)/common/inc

LOCAL_SHARED_LIBRARIES += \
	libbinder \
	libcutils \
	libutils \
	libQSEEComAPI \
	liblog

LOCAL_MODULE := esepmdaemon
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_SRC_FILES += \
	BneSEPowerManager.cpp \
	IeSEPowerManager.cpp \
	BpeSEPowerManager.cpp

LOCAL_SHARED_LIBRARIES += \
	libbinder \
	libcutils \
	libutils \
	liblog

LOCAL_MODULE := libesepm
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := eSEPowerManagerClient.cpp
LOCAL_SHARED_LIBRARIES += \
	libbinder \
	libcutils \
	libutils \
	libesepm \
	liblog

LOCAL_MODULE := esepmclient
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)
endif
