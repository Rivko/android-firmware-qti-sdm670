LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE           := libSTAWrapper
LOCAL_SRC_FILES        := STAWrapper.cpp

#LOCAL_C_INCLUDES       := vendor/qcom/proprietary/scve-noship/modules/AL/api/

LOCAL_LDFLAGS          := -ljnigraphics
LOCAL_SHARED_LIBRARIES := liblog libscveTextReco libscveTextReco_stub libscveImageCorrection libscveCommon libscveTextRecoPostProcessing

include vendor/qcom/proprietary/scve/reference/common.mk

include $(BUILD_SHARED_LIBRARY)
