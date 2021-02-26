ifeq ($(CAMX_CHICDK_PATH),)
LOCAL_PATH := $(abspath $(call my-dir)/../..)
CAMX_CHICDK_PATH := $(abspath $(LOCAL_PATH)/../../..)
else
LOCAL_PATH := $(CAMX_CHICDK_PATH)/vendor/chioverride/default
endif

include $(CLEAR_VARS)

# Module supports function call tracing via ENABLE_FUNCTION_CALL_TRACE
# Required before including common.mk
SUPPORT_FUNCTION_CALL_TRACE := 1

# Get definitions common to the CAMX project here
include $(CAMX_CHICDK_PATH)/vendor/common.mk

$(info $(shell perl $(CAMX_CDK_PATH)/topology/usecaseconverter.pl $(CAMX_VENDOR_PATH)/topology/default/titan17x_usecases.xml $(LOCAL_PATH)/g_pipelines.h))

LOCAL_INC_FILES :=             \
    cdkutils.h                 \
    chxadvancedcamerausecase.h \
    chxdefs.h                  \
    chxextensionmodule.h       \
    chxfeature.h               \
    chxfeaturehdr.h            \
    chxfeatureshdr.h          \
    chxfeaturemultiframe.h     \
    chxfeaturemfnr.h           \
    chxfeatureyuvcb.h          \
    chxfeaturezsl.h            \
    chxfeaturerawjpeg.h        \
    chxincs.h                  \
    chxmulticamcontroller.h    \
    chxpipeline.h              \
    chxsensorselectmode.h      \
    chxsession.h               \
    chxusecase.h               \
    chxusecasedefault.h        \
    chxusecasemc.h             \
    chxusecasevrmc.h           \
    chxusecasequadcfa.h        \
    chxusecasetorch.h          \
    chxusecaseutils.h          \
    chxutils.h                 \
    chxzoomtranslator.h        \
    chxperf.h                  \
    g_pipelines.h

LOCAL_SRC_FILES :=               \
    chxadvancedcamerausecase.cpp \
    chxextensioninterface.cpp    \
    chxextensionmodule.cpp       \
    chxfeaturehdr.cpp            \
    chxfeatureshdr.cpp          \
    chxfeaturemultiframe.cpp     \
    chxfeaturemfnr.cpp           \
    chxfeatureyuvcb.cpp          \
    chxfeaturezsl.cpp            \
    chxfeaturerawjpeg.cpp        \
    chxmulticamcontroller.cpp    \
    chxpipeline.cpp              \
    chxsensorselectmode.cpp      \
    chxsession.cpp               \
    chxusecase.cpp               \
    chxusecasedefault.cpp        \
    chxusecasemc.cpp             \
    chxusecasevrmc.cpp           \
    chxusecasequadcfa.cpp        \
    chxusecasetorch.cpp          \
    chxusecaseutils.cpp          \
    chxutils.cpp                 \
    chxperf.cpp                  \
    chxzoomtranslator.cpp

LOCAL_C_LIBS := $(CAMX_C_LIBS)

LOCAL_C_INCLUDES :=                          \
    $(CAMX_C_INCLUDES)                       \
    $(CAMX_CDK_PATH)/utils                   \
    hardware/libhardware/include/hardware    \
    hardware/qcom/display/libgralloc         \
    system/core/libsync/include              \
    system/core/include                      \
    system/media/camera/include              \
    system/media/private/camera/include

# Compiler flags
LOCAL_CFLAGS := $(CAMX_CFLAGS)
LOCAL_CPPFLAGS := $(CAMX_CPPFLAGS)
LOCAL_CPPFLAGS += -Wno-inline-new-delete

LOCAL_SHARED_LIBRARIES +=           \
    libcamera_metadata              \
    libhardware                     \
    liblog                          \
    libcutils                       \

# Libraries to link
# Build in vendor partition
LOCAL_PROPRIETARY_MODULE := true

# Deployment path under lib/lib64
LOCAL_MODULE_RELATIVE_PATH := hw

LOCAL_MODULE := com.qti.chi.override

include $(BUILD_SHARED_LIBRARY)
