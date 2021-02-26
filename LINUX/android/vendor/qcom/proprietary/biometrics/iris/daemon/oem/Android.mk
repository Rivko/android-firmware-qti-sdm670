LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CFLAGS += -Werror -Wno-unused-parameter
LOCAL_C_INCLUDES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include frameworks/native/libs/arect/include
LOCAL_SRC_FILES :=  iris_qti_default.cpp
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog
LOCAL_MODULE:= libiris_oem_default
#LOCAL_MODULE_TAGS := optional

LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti
LOCAL_MULTILIB := 32
#CPPFLAGS += -I$(SYSCORE_DIR)/include
include $(BUILD_SHARED_LIBRARY)

