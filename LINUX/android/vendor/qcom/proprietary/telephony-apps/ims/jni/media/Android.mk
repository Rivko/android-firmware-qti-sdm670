LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE := false

LOCAL_SRC_FILES:= \
    jni_main.cpp \
    ims_media_jni.cpp \


LOCAL_C_INCLUDES += \
    $(JNI_H_INCLUDE) \

LOCAL_SHARED_LIBRARIES := \
    libnativehelper \
    libcutils \
    libutils  \
    libdl \
    libgui \
    libbinder \
    liblog \


LOCAL_CFLAGS += -O0 -g

LOCAL_MODULE := libimsmedia_jni
LOCAL_MODULE_TAGS := optional
LOCAL_PROGUARD_ENABLED := disabled

ifeq ($(ATEL_ENABLE_LLVM_SA),true)
    LLVM_SA_OUTPUT_DIR := $(ANDROID_PRODUCT_OUT)/atel-llvm-sa-results/$(LOCAL_MODULE)
    LLVM_SA_FLAG := --compile-and-analyze $(LLVM_SA_OUTPUT_DIR)
    LOCAL_CFLAGS += $(LLVM_SA_FLAG)
    LOCAL_CPPFLAGS += $(LLVM_SA_FLAG)
endif

include $(BUILD_SHARED_LIBRARY)

