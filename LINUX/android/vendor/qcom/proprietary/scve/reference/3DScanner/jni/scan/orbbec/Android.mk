LOCAL_PATH:= $(call my-dir)

#####################################################
# Users are reponsible for downloading the libs below.
# Once they are saved in 3DScanner/jni/scan/orbbec,
# uncomment lines in this Android.mk referencing any
# of these libraries.
#		libOniFile \
#		libOpenNI2.jni \
#		libOpenNI2 \
#		liborbbec \
#		libPSLink \
#		libusb-1.0.9 \
#####################################################

#include $(CLEAR_VARS)
#LOCAL_MODULE            := libOniFile
#LOCAL_SRC_FILES         := libOniFile.so
#LOCAL_MULTILIB     := 32
#LOCAL_MODULE_CLASS      := SHARED_LIBRARIES
#LOCAL_MODULE_TAGS  := optional
#LOCAL_MODULE_OWNER := qti
#LOCAL_PROPRIETARY_MODULE := true
#LOCAL_MODULE_SUFFIX := .so
#include $(BUILD_PREBUILT)

#include $(CLEAR_VARS)
#LOCAL_MODULE            := libOpenNI2.jni
#LOCAL_SRC_FILES         := libOpenNI2.jni.so
#LOCAL_MULTILIB     := 32
#LOCAL_MODULE_CLASS      := SHARED_LIBRARIES
#LOCAL_MODULE_TAGS  := optional
#LOCAL_MODULE_OWNER := qti
#LOCAL_PROPRIETARY_MODULE := true
#LOCAL_MODULE_SUFFIX := .so
#include $(BUILD_PREBUILT)

#include $(CLEAR_VARS)
#LOCAL_MODULE            := libOpenNI2
#LOCAL_SRC_FILES         := libOpenNI2.so
#LOCAL_MULTILIB     := 32
#LOCAL_MODULE_CLASS      := SHARED_LIBRARIES
#LOCAL_MODULE_TAGS  := optional
#LOCAL_MODULE_OWNER := qti
#LOCAL_PROPRIETARY_MODULE := true
#LOCAL_MODULE_SUFFIX := .so
#include $(BUILD_PREBUILT)

#include $(CLEAR_VARS)
#LOCAL_MODULE            := liborbbec
#LOCAL_SRC_FILES         := liborbbec.so
#LOCAL_MULTILIB     := 32
#LOCAL_MODULE_CLASS      := SHARED_LIBRARIES
#LOCAL_MODULE_TAGS  := optional
#LOCAL_MODULE_OWNER := qti
#LOCAL_PROPRIETARY_MODULE := true
#LOCAL_MODULE_SUFFIX := .so
#include $(BUILD_PREBUILT)

#include $(CLEAR_VARS)
#LOCAL_MODULE            := libPSLink
#LOCAL_SRC_FILES         := libPSLink.so
#LOCAL_MULTILIB     := 32
#LOCAL_MODULE_CLASS      := SHARED_LIBRARIES
#LOCAL_MODULE_TAGS  := optional
#LOCAL_MODULE_OWNER := qti
#LOCAL_PROPRIETARY_MODULE := true
#LOCAL_MODULE_SUFFIX := .so
#include $(BUILD_PREBUILT)

#include $(CLEAR_VARS)
#LOCAL_MODULE            := libusb-1.0.9
#LOCAL_SRC_FILES         := libusb-1.0.9.so
#LOCAL_MULTILIB     := 32
#LOCAL_MODULE_CLASS      := SHARED_LIBRARIES
#LOCAL_MODULE_TAGS  := optional
#LOCAL_MODULE_OWNER := qti
#LOCAL_PROPRIETARY_MODULE := true
#LOCAL_MODULE_SUFFIX := .so
#include $(BUILD_PREBUILT)


include $(CLEAR_VARS)
LOCAL_MODULE            := lib3drNative-lib
LOCAL_C_INCLUDES        := $(LOCAL_PATH)/include
LOCAL_SRC_FILES         := native-lib.cpp \
                           cmd.cpp
LOCAL_LDLIBS    := -llog
include vendor/qcom/proprietary/scve/reference/common.mk
#LOCAL_SHARED_LIBRARIES := libusb-1.0.9 libOpenNI2
include $(BUILD_SHARED_LIBRARY)

