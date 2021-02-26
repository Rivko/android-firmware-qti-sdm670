LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

DEVICE_COMMON_PATH := device/qcom/common


ifeq ($(TARGET_PRODUCT),msm8909w)
	DEVICE_COMMON_PATH := $(BOARD_COMMON_DIR)
endif

LOCAL_SRC_FILES := \
	ActiveRequestList.cpp \
	ResourceQueues.cpp \
	OptsHandler.cpp \
	RestoreHandler.cpp \
	OptsData.cpp \
	ResourceInfo.cpp \
	Request.cpp \
	Target.cpp \
	TargetInit.cpp \
	MpctlUtils.cpp \
	mp-ctl.cpp \
	BoostConfigReader.cpp

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	liblog \
	libutils \
	libxml2 \
	libqti-util \
	libhidlbase \
	libhidltransport \
	vendor.qti.hardware.perf@1.0_vendor \
	libbase \
	libthermalclient \
	libqti-perfd-client \
	libperfgluelayer \
	libfeedbackhandler

LOCAL_CFLAGS += \
	-DSERVER \
	-DCLIENT \
	-DPERFD \
	-g0 \
	-Wall \
	-Wno-unused-variable \
	-Wno-unused-parameter \
	-Wno-unused-function \
	-DOEM_PROFILE_RESTORE_ON_FAIL=0 \
	-DVENDOR_DIR=\"/vendor/etc\" \
	-DPROFILE_MANAGER \
	-DQC_DEBUG=0

LOCAL_C_INCLUDES := \
	$(DEVICE_COMMON_PATH)/power \
	$(DEVICE_COMMON_PATH)/thermal-engine \
	external/libxml2/include \
	external/icu/icu4c/source/common \
	vendor/qcom/proprietary/android-perf/perf-util \
	vendor/qcom/proprietary/android-perf/perf-hal


LOCAL_MODULE := libqti-perfd
LOCAL_PROPRIETARY_MODULE := true


include $(BUILD_SHARED_LIBRARY)
