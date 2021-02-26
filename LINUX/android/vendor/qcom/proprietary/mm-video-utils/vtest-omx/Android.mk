LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

ifeq ($(call is-board-platform-in-list, $(MSM_VIDC_TARGET_LIST)),true)

# Forcing O0 flag to remove all compiler optimizations as a workaround for
# stack corruptions seen on 64 bit.
vtest-def := \
    -g -O0 -Werror\
    -D_POSIX_SOURCE -DPOSIX_C_SOURCE=199506L \
    -D_XOPEN_SOURCE=600 \
    -D_ANDROID_ \
    -Wno-deprecated-declarations \
    -Wno-mismatched-tags \
    -fno-stack-protector \
    -Wno-macro-redefined

TARGETS_THAT_NEED_SW_VENC := sdm845 sdm670
TAEGWTS_THAT_NEED_SW_VDEC := sdm845 sdm670

ifneq ($(LINUX_OMX_TEST_ONLY_ENCODE), true)
vtest-inc        := $(TARGET_OUT_HEADERS)/mm-core/omxcore
vtest-inc        += $(TARGET_OUT_HEADERS)/mm-video/fastcrc
endif
vtest-inc        += $(TARGET_OUT_HEADERS)/mm-video/streamparser
vtest-inc        += $(TARGET_OUT_HEADERS)/mm-video/utils
vtest-inc        += $(LOCAL_PATH)/common/inc
vtest-inc        += $(LOCAL_PATH)/utils/inc
ifneq ($(LINUX_OMX_TEST_ONLY_ENCODE), true)
vtest-inc        += $(TOP)/vendor/qcom/proprietary/common/inc
vtest-inc        += $(TOP)/frameworks/native/include/media/hardware
vtest-inc        += $(TOP)/hardware/qcom/media/mm-video-v4l2/vidc
vtest-inc        += $(TOP)/hardware/qcom/media/mm-video-v4l2/vidc/common/inc
vtest-inc        += $(TOP)/hardware/qcom/media/mm-core/inc
vtest-inc        += $(TOP)/hardware/qcom/media/libc2dcolorconvert
vtest-inc        += $(TOP)/hardware/qcom/media/libstagefrighthw/
vtest-inc        += $(TOP)/frameworks/native/include/gui/
vtest-inc        += $(TOP)/frameworks/native/include/ui/
vtest-inc        += $(TOP)/frameworks/native/include/
vtest-inc        += $(TOP)/external/connectivity/stlport/stlport/
vtest-inc        += $(TARGET_OUT_HEADERS)/qcom/display
vtest-inc        += $(TOP)/system/core/include/system/
endif
vtest-inc        += $(LOCAL_PATH)/xmlparser/inc

CP_COPY_PATH := $(TOP)/vendor/qcom/proprietary/securemsm/sampleclient/
ifeq ($(shell if test -f $(CP_COPY_PATH)/content_protection_copy.h; then echo true; else echo false; fi;),true)
$(warning SECURE_COPY_ENABLED)
vtest-def        += -DSECURE_COPY_ENABLED
vtest-inc        += $(CP_COPY_PATH)
vtest-inc        += $(TOP)/vendor/qcom/proprietary/securemsm/QSEEComAPI/
endif

ifeq ($(call is-board-platform-in-list, $(TARGETS_THAT_NEED_SW_VENC)),true)
vtest-def        += -DSW_VENC
endif

ifeq ($(call is-board-platform-in-list, $(TARGETS_THAT_NEED_SW_VDEC)),true)
vtest-def        += -DSW_VDEC
endif

ifeq ($(call is-board-platform-in-list, $(MASTER_SIDE_CP_TARGET_LIST)),true)
$(warning MASTER_SIDE_CP_ENABLED)
vtest-def        += -DMASTER_SIDE_CP
endif

vtest-def        += -DUSE_NATIVE_HANDLE_SOURCE
vtest-def        += -DNWFIX=0

LOCAL_MODULE            := mm-vidc-omx-test
LOCAL_MODULE_TAGS       := optional
LOCAL_32_BIT_ONLY       := true
LOCAL_CFLAGS            := $(vtest-def)

ifdef LLVM_REPORT_DIR
LOCAL_CFLAGS      += --compile-and-analyze $(LLVM_REPORT_DIR)
LOCAL_CPPFLAGS    += --compile-and-analyze $(LLVM_REPORT_DIR)
endif

LOCAL_C_INCLUDES        := $(vtest-inc)
LOCAL_C_INCLUDES        += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_C_INCLUDES        += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include/media
LOCAL_C_INCLUDES        += $(TOP)/external/libxml2 $(TOP)/external/libxml2/include $(TOP)/external/icu/icu4c/source/common
LOCAL_ADDITIONAL_DEPENDENCIES    := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_SHARED_LIBRARIES  := libOmxCore libOmxVenc libbinder libcutils libfastcrc liblog libvideoutils
LOCAL_SHARED_LIBRARIES  += libdl libhidltransport libgui libui libutils libqdMetaData libxml2 libstreamparser

LOCAL_SRC_FILES         := \
                xmlparser/src/vtest_XmlParser.cpp \
                xmlparser/src/vtest_XmlParserHelper.cpp \
                utils/src/vt_queue.c \
                utils/src/vt_semaphore.c \
                utils/src/vt_signal.c \
                utils/src/vt_file.c \
                utils/src/vt_ion_allocator.c \
                common/src/vtest_Config.cpp \
                common/src/vtest_File.cpp \
                common/src/vtest_Mutex.cpp \
                common/src/vtest_Parser.cpp \
                common/src/vtest_Queue.cpp \
                common/src/vtest_Script.cpp \
                common/src/vtest_Signal.cpp \
                common/src/vtest_SignalQueue.cpp \
                common/src/vtest_Sleeper.cpp \
                common/src/vtest_Thread.cpp \
                common/src/vtest_Time.cpp \
                common/src/vtest_Crypto.cpp \
                common/src/vtest_BufferManager.cpp \
                common/src/vtest_ISource.cpp \
                common/src/vtest_ITestCase.cpp \
                common/src/vtest_IPostProc.cpp \
                common/src/vtest_PostProcFactory.cpp \
                common/src/vtest_TestCaseFactory.cpp \
                common/src/vtest_Encoder.cpp \
                common/src/vtest_EncoderFileSink.cpp \
                common/src/vtest_EncoderFileSource.cpp \
                common/src/vtest_PostProcSource.cpp \
                common/src/vtest_TestEncode.cpp \
                app/src/vtest_App.cpp

ifneq ($(LINUX_OMX_TEST_ONLY_ENCODE), true)
LOCAL_SRC_FILES         += \
                common/src/vtest_Decoder.cpp \
                common/src/vtest_DecoderFileSource.cpp \
                common/src/vtest_DecoderFileSink.cpp \
                common/src/vtest_NativeWindow.cpp \
                common/src/vtest_MdpSource.cpp \
                common/src/vtest_MdpOverlaySink.cpp \
                common/src/vtest_TestDecode.cpp \
                common/src/vtest_TestTranscode.cpp \
                common/src/vtest_TestPostProc.cpp
endif

ifeq ($(BOARD_HAVE_ADRENO), true)
GPU_PP_PATH := $(TOP)/vendor/qcom/proprietary/gles/adreno200/tools/gpupostprocessing
ifeq ($(shell if test -f $(GPU_PP_PATH)/gpupostprocessing.h; then echo true; else echo false; fi;),true)
vtest-pp-def            += -DGPU_PP_ENABLED
vtest-pp-inc            += $(GPU_PP_PATH)
vtest-pp-inc            += $(TOP)/system/core/include/system/
vtest-pp-src            += common/src/vtest_GpuPostProc.cpp
endif

C2DCC_PP_PATH := $(TOP)/hardware/qcom/media/libc2dcolorconvert
ifeq ($(shell if test -f $(C2DCC_PP_PATH)/C2DColorConverter.h; then echo true; else echo false; fi;),true)
vtest-pp-def            += -DC2DCC_PP_ENABLED
vtest-pp-inc            += $(C2DCC_PP_PATH)
vtest-pp-inc            += $(TARGET_OUT_HEADERS)/qcom/display
vtest-pp-src            += common/src/vtest_C2dCCPostProc.cpp
endif
endif # ($(BOARD_HAVE_ADRENO), true)

MMCC_PP_PATH := $(TOP)/vendor/qcom/proprietary/mm-color-convertor/libmmcolorconvertor/
ifeq ($(shell if test -f $(MMCC_PP_PATH)/MMColorConvert.h; then echo true; else echo false; fi;),true)
vtest-pp-def            += -DMMCC_PP_ENABLED
vtest-pp-inc            += $(MMCC_PP_PATH)
vtest-pp-src            += common/src/vtest_MmCCPostProc.cpp
endif

$(warning $(vtest-pp-def))
LOCAL_CFLAGS            += $(vtest-pp-def)
LOCAL_C_INCLUDES        += $(vtest-pp-inc)
LOCAL_SRC_FILES         += $(vtest-pp-src)

include $(BUILD_EXECUTABLE)

endif
