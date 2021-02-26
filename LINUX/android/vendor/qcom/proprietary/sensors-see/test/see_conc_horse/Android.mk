LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

c_sources =
c_sources += see_conc_horse.cpp

c_includes =
c_includes += $(LOCAL_PATH)
c_includes += $(LOCAL_PATH)/../see_salt

c_flags = -Wno-unused-variable -Wno-unused-parameter -fexceptions -g
ifeq ($(LLVM_SENSORS_SEE),true)
c_flags += --compile-and-analyze --analyzer-perf --analyzer-Werror
endif
shared_libs =
shared_libs += libSEESalt

LOCAL_MODULE := see_conc_horse
LOCAL_MODULE_OWNER := qti
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true
LOCAL_SRC_FILES := $(c_sources)
LOCAL_C_INCLUDES := $(c_includes)
LOCAL_CFLAGS := $(c_flags)
LOCAL_SHARED_LIBRARIES := $(shared_libs)

include $(BUILD_EXECUTABLE)
