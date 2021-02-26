#include $(call all-subdir-makefiles)

CUR_DIR:=$(call my-dir)

include $(CUR_DIR)/../common.mk

include $(CUR_DIR)/utils/Android.mk
include $(CUR_DIR)/unit_tests/Android.mk
include $(CUR_DIR)/functional_tests/Android.mk
include $(CUR_DIR)/ipc_tests/Android.mk