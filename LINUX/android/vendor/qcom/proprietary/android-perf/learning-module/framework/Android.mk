# LM_PATH points to learning-module folder and comes from main Android.mk
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_SRC_FILES := metameters/MetaMeter.cpp \
    metameters/MeterMgr.cpp \
    metameters/Meter.cpp \
    metameters/MeterFwk.cpp \
    metameters/ThreadHelper.cpp \
    core/LearningModule.cpp \
    core/Feature.cpp \
    core/FeatureState.cpp \
    core/FeatureXML.cpp \
    core/LMPerfGlue.cpp \
    database/lmDB.cpp

LOCAL_SHARED_LIBRARIES := liblog libc libcutils libutils libtinyxml2 libsqlite libperfgluelayer
LOCAL_CFLAGS := -Wno-unused-parameter
# Enable this for debug logs
#LOCAL_CFLAGS += -DQC_DEBUG
LOCAL_C_INCLUDES := $(LM_INCLUDES) \
    $(MPCTL_PATH) \
    $(PERFHAL_PATH)
LOCAL_MULTILIB := first
LOCAL_MODULE := liblearningmodule
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)
