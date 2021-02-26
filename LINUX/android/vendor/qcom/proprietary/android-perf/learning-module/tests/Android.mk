# LM_PATH points to learning-module folder and comes from main Android.mk
LOCAL_PATH:= $(call my-dir)
LMCORE_TEST_PATH:= ModuleTests/core
LMDB_TEST_PATH:= ModuleTests/database
UTILS_PATH:= utils
LMDB_SRC_PATH:= ../framework/database

include $(CLEAR_VARS)
LOCAL_SRC_FILES := $(UTILS_PATH)/PerfHintHelper.cpp
LOCAL_SHARED_LIBRARIES := liblog libc libcutils
LOCAL_CFLAGS := -Wno-unused-parameter -Werror
LOCAL_C_INCLUDES := $(LM_INCLUDES) \
                    $(MPCTL_PATH)
LOCAL_MULTILIB := first
LOCAL_MODULE := libperfhinthelper
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := lmtests.cpp \
                    $(LMCORE_TEST_PATH)/LMCore.cpp \
                    $(LMDB_SRC_PATH)/lmDB.cpp \
                    $(LMDB_TEST_PATH)/iopDB.cpp \
                    $(LMDB_TEST_PATH)/LMDBTest.cpp
LOCAL_SHARED_LIBRARIES := liblog libc libcutils libsqlite libperfhinthelper
LOCAL_CFLAGS := -Wno-unused-parameter -Werror
LOCAL_C_INCLUDES := $(LM_INCLUDES) \
                    $(LOCAL_PATH)/$(UTILS_PATH) \
                    $(MPCTL_PATH) \
                    $(PERFHAL_PATH) \
                    $(LOCAL_PATH)/$(LMCORE_TEST_PATH) \
                    $(LOCAL_PATH)/$(LMDB_TEST_PATH)
LOCAL_MULTILIB := first
LOCAL_MODULE := lmtests
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)
