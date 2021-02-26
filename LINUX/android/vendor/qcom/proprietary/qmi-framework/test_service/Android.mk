LOCAL_PATH := $(call my-dir)

commonSources := test_service_v01.c qmi_test_service_clnt_common.c qmi_test_service_clnt_common_stats.c
commonIncludes := $(TARGET_OUT_HEADERS)/qmi-framework/inc
commonIncludes += $(TARGET_OUT_HEADERS)/qmi-framework/qcsi/inc
commonIncludes += $(TARGET_OUT_HEADERS)/qmi-framework/qcci/inc
commonIncludes += $(TARGET_OUT_HEADERS)/qmi-framework/common/inc
commonIncludes += $(TARGET_OUT_HEADERS)/qmi/inc

common_test_clnt_cflags := -g
common_test_clnt_cflags += -O2
common_test_clnt_cflags += -fno-inline
common_test_clnt_cflags += -fno-short-enums
common_test_clnt_cflags += -fpic
common_test_clnt_cflags += -Werror
common_test_clnt_cflags += -Wno-unused-parameter

include $(CLEAR_VARS)
LOCAL_MODULE := qmi_test_service_test
LOCAL_CFLAGS := $(common_test_clnt_cflags)
LOCAL_C_INCLUDES := $(commonIncludes)
LOCAL_SRC_FILES  := test_service_v01.c qmi_test_service_clnt.c
LOCAL_MODULE_TAGS := debug
LOCAL_SHARED_LIBRARIES := libqmi_cci libqmi_common_so
LOCAL_MODULE_RELATIVE_PATH := qmi-framework-tests
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := qmi_test_service_clnt_test_0000
LOCAL_CFLAGS := $(common_test_clnt_cflags)
LOCAL_C_INCLUDES := $(commonIncludes)
LOCAL_SRC_FILES  := $(commonSources) qmi_test_service_clnt_test_0000.c
LOCAL_MODULE_TAGS := debug
LOCAL_SHARED_LIBRARIES := libqmi_cci libqmi_common_so
LOCAL_MODULE_RELATIVE_PATH := qmi-framework-tests
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := qmi_test_service_clnt_test_0001
LOCAL_CFLAGS := $(common_test_clnt_cflags)
LOCAL_C_INCLUDES := $(commonIncludes)
LOCAL_SRC_FILES  := $(commonSources) qmi_test_service_clnt_test_0001.c
LOCAL_MODULE_TAGS := debug
LOCAL_SHARED_LIBRARIES := libqmi_cci libqmi_common_so
LOCAL_MODULE_RELATIVE_PATH := qmi-framework-tests
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := qmi_test_service_clnt_test_1000
LOCAL_CFLAGS := $(common_test_clnt_cflags)
LOCAL_C_INCLUDES := $(commonIncludes)
LOCAL_SRC_FILES  := $(commonSources) qmi_test_service_clnt_test_1000.c
LOCAL_MODULE_TAGS := debug
LOCAL_SHARED_LIBRARIES := libqmi_cci libqmi_common_so
LOCAL_MODULE_RELATIVE_PATH := qmi-framework-tests
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := qmi_test_service_clnt_test_1001
LOCAL_CFLAGS := $(common_test_clnt_cflags)
LOCAL_C_INCLUDES := $(commonIncludes)
LOCAL_SRC_FILES  := $(commonSources) qmi_test_service_clnt_test_1001.c
LOCAL_MODULE_TAGS := debug
LOCAL_SHARED_LIBRARIES := libqmi_cci libqmi_common_so
LOCAL_MODULE_RELATIVE_PATH := qmi-framework-tests
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := qmi_test_service_clnt_test_2000
LOCAL_CFLAGS := $(common_test_clnt_cflags)
LOCAL_C_INCLUDES := $(commonIncludes)
LOCAL_SRC_FILES  := $(commonSources) qmi_test_service_clnt_test_2000.c
LOCAL_MODULE_TAGS := debug
LOCAL_SHARED_LIBRARIES := libqmi_cci libqmi_common_so
LOCAL_MODULE_RELATIVE_PATH := qmi-framework-tests
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := qmi_test_service_clnt_test_3000
LOCAL_CFLAGS := $(common_test_clnt_cflags)
LOCAL_C_INCLUDES := $(commonIncludes)
LOCAL_SRC_FILES  := $(commonSources) qmi_test_service_clnt_test_3000.c
LOCAL_MODULE_TAGS := debug
LOCAL_SHARED_LIBRARIES := libqmi_cci libqmi_common_so libqmi_encdec
LOCAL_MODULE_PATH := $(TARGET_OUT_EXECUTABLES)/qmi-framework-tests
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := qmi_test_service_clnt_test_3001
LOCAL_CFLAGS := $(common_test_clnt_cflags)
LOCAL_C_INCLUDES := $(commonIncludes)
LOCAL_SRC_FILES  := $(commonSources) qmi_test_service_clnt_test_3001.c
LOCAL_MODULE_TAGS := debug
LOCAL_SHARED_LIBRARIES := libqmi_cci libqmi_common_so libqmi_encdec
LOCAL_MODULE_PATH := $(TARGET_OUT_EXECUTABLES)/qmi-framework-tests
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := qmi_test_service_clnt_test_4000
LOCAL_CFLAGS := $(common_test_clnt_cflags)
LOCAL_C_INCLUDES := $(commonIncludes)
LOCAL_SRC_FILES  := $(commonSources) qmi_test_service_clnt_test_4000.c
LOCAL_MODULE_TAGS := debug
LOCAL_SHARED_LIBRARIES := libqmi_cci libqmi_common_so libqmi_encdec
LOCAL_MODULE_PATH := $(TARGET_OUT_EXECUTABLES)/qmi-framework-tests
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := qmi_test_service_clnt_test_4001
LOCAL_CFLAGS := $(common_test_clnt_cflags)
LOCAL_C_INCLUDES := $(commonIncludes)
LOCAL_SRC_FILES  := $(commonSources) qmi_test_service_clnt_test_4001.c
LOCAL_MODULE_TAGS := debug
LOCAL_SHARED_LIBRARIES := libqmi_cci libqmi_common_so
LOCAL_MODULE_PATH := $(TARGET_OUT_EXECUTABLES)/qmi-framework-tests
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := qmi_test_service_start_svc
LOCAL_CFLAGS := $(common_test_clnt_cflags)
LOCAL_C_INCLUDES := $(commonIncludes)
LOCAL_SRC_FILES  := $(commonSources) qmi_test_service_ping_svc.c qmi_test_service_start_svc.c
LOCAL_MODULE_TAGS := debug
LOCAL_SHARED_LIBRARIES := libqmi_csi libqmi_common_so
LOCAL_MODULE_PATH := $(TARGET_OUT_EXECUTABLES)/qmi-framework-tests
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := qmi_test_mt_client_init_instance
LOCAL_CFLAGS := $(common_test_clnt_cflags)
LOCAL_C_INCLUDES := $(commonIncludes)
LOCAL_SRC_FILES  := $(commonSources) qmi_test_mt_client_init_instance.c
LOCAL_MODULE_TAGS := debug
LOCAL_SHARED_LIBRARIES := libqmi_cci libqmi_common_so
LOCAL_MODULE_RELATIVE_PATH := qmi-framework-tests
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := check_system_health
LOCAL_CFLAGS := $(common_test_clnt_cflags)
LOCAL_C_INCLUDES := $(TARGET_OUT_HEADERS)/system_health_monitor/inc
LOCAL_SRC_FILES  := check_system_health.c
LOCAL_MODULE_TAGS := debug
LOCAL_SHARED_LIBRARIES := libsystem_health_mon
LOCAL_MODULE_RELATIVE_PATH := qmi-framework-tests
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)
