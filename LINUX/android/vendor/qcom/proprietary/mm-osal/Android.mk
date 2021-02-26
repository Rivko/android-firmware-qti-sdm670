LOCAL_PATH:= $(call my-dir)
############################################################
########## Build the library for system partition ##########
############################################################
include $(CLEAR_VARS)

LOCAL_COPY_HEADERS_TO := mm-osal/include
LOCAL_COPY_HEADERS := inc/AEEStdDef.h
LOCAL_COPY_HEADERS += inc/AEEstd.h
LOCAL_COPY_HEADERS += inc/AEEVaList.h
LOCAL_COPY_HEADERS += inc/MMCriticalSection.h
LOCAL_COPY_HEADERS += inc/MMDebugMsg.h
LOCAL_COPY_HEADERS += inc/MMFile.h
LOCAL_COPY_HEADERS += inc/MMMalloc.h
LOCAL_COPY_HEADERS += inc/MMMemory.h
LOCAL_COPY_HEADERS += inc/MMSignal.h
LOCAL_COPY_HEADERS += inc/MMThread.h
LOCAL_COPY_HEADERS += inc/MMTime.h
LOCAL_COPY_HEADERS += inc/MMTimer.h

LOCAL_CFLAGS := \
    -D_ANDROID_

LOCAL_SRC_FILES:=       \
     ./src/MMMalloc.c \
     ./src/MMCriticalSection.c \
     ./src/MMDebug.c \
     ./src/MMTimer.c \
     ./src/MMThread.c \
     ./src/MMSignal.c \
     ./src/MMFile.c  \
     ./src/AEEstd.c

LOCAL_C_INCLUDES:= \
    $(LOCAL_PATH)/./inc \
    $(LOCAL_PATH)/../common/inc

LOCAL_SHARED_LIBRARIES := \
    libutils  \
    libcutils \
    liblog

LOCAL_PRELINK_MODULE:= false

LOCAL_MODULE:= libmmosal
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)
############################################################
########## Build the library for vendor partition ##########
############################################################
include $(CLEAR_VARS)

LOCAL_CFLAGS := \
    -D_ANDROID_

LOCAL_SRC_FILES:=       \
     ./src/MMMalloc.c \
     ./src/MMCriticalSection.c \
     ./src/MMDebug.c \
     ./src/MMTimer.c \
     ./src/MMThread.c \
     ./src/MMSignal.c \
     ./src/MMFile.c  \
     ./src/AEEstd.c

LOCAL_C_INCLUDES:= \
    $(LOCAL_PATH)/./inc \
    $(LOCAL_PATH)/../common/inc

LOCAL_SHARED_LIBRARIES := \
    libutils  \
    libcutils \
    liblog

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

LOCAL_MODULE:= libmmosal_proprietary
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)
