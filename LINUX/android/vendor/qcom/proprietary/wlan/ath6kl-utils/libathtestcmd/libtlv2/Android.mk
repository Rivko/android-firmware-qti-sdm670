LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE = libtlv2
LOCAL_MODULE_TAGS := debug eng optional
LOCAL_CFLAGS =

LOCAL_SRC_FILES = \
                   cmdEncoder/tlvCmdEncoder.c \
                   cmdParser/cmdAdcCaptureHandler.c \
                   cmdParser/cmdBasicRspHandler.c \
                   cmdParser/cmdCalCalTimeHandler.c \
                   cmdParser/cmdCommitOtpStream.c \
                   cmdParser/cmdConfigHandler.c \
                   cmdParser/cmdCustOtpSpaceGetSize.c \
                   cmdParser/cmdCustOtpSpaceRead.c \
                   cmdParser/cmdCustOtpSpaceWrite.c \
                   cmdParser/cmdDPDAgc2Pwr.c \
                   cmdParser/cmdDPDComplete.c \
                   cmdParser/cmdDPDLoopbackAttn.c \
                   cmdParser/cmdDPDLoopbackTiming.c \
                   cmdParser/cmdDPDTrainingQuality.c \
                   cmdParser/cmdEnableDfeHandler.c \
                   cmdParser/cmdGenWaveformHandler.c \
                   cmdParser/cmdGetTgtPwr.c \
                   cmdParser/cmdHandlerTbl.c \
                   cmdParser/cmdHwCalHandler.c \
                   cmdParser/cmdLMChannelListAndRspHandler.c \
                   cmdParser/cmdLMGoHandler.c \
                   cmdParser/cmdLMHWCALHandler.c \
                   cmdParser/cmdLMHWCALRSPHandler.c \
                   cmdParser/cmdLMQueryHandler.c \
                   cmdParser/cmdLMRxHandler.c \
                   cmdParser/cmdLMTxHandler.c \
                   cmdParser/cmdLMTxInitHandler.c \
                   cmdParser/cmdMemReadHandler.c \
                   cmdParser/cmdMemWriteHandler.c \
                   cmdParser/cmdMoreSegmentHandler.c \
                   cmdParser/cmdRegReadHandler.c \
                   cmdParser/cmdRegWriteHandler.c \
                   cmdParser/cmdRxDcoHandler.c \
                   cmdParser/cmdRxGainCalHandler.c \
                   cmdParser/cmdRxHandler.c \
                   cmdParser/cmdRxStatusHandler.c \
                   cmdParser/cmdSARHandler.c \
                   cmdParser/cmdSetPCIEConfigParam.c \
                   cmdParser/cmdSetPhyRfMode.c \
                   cmdParser/cmdSetRegDmn.c \
                   cmdParser/cmdTPCCALHandler.c \
                   cmdParser/cmdTPCCALPWRHandler.c \
                   cmdParser/cmdTxHandler.c \
                   cmdParser/cmdTxStatusHandler.c \
                   cmdParser/cmdXtalCalHandler.c \
                   cmdParser/parseBinCmdStream_v2.c \
                   cmdParser/sysCmdGeneric.c \
                   cmdParser/sysCmdHandlerTbl.c \
                   cmdParser/sysCmdPHYDBGdump.c \
                   cmdParser/sysCmdSetChannel.c \
                   cmdParser/sysCmdTestData.c \
                   cmdParser/sysCmdtlvCaptureCtrl.c \
                   cmdParser/cmdDPDLoopbackInfo.c \
                   common/cmdRspParmsDict.c \
                   common/cmdRspParmsInternal.c \
                   common/sysCmdRspParmsDict.c \
                   tlvcmdrsp/tlvCmdRsp.c \
                   util/testUtil.c

LOCAL_C_INCLUDES += \
                        $(LOCAL_PATH)/include \
                        $(LOCAL_PATH)/../include \
                        $(LOCAL_PATH)/cmdEncoder \
                        $(LOCAL_PATH)/cmdParser \
                        $(LOCAL_PATH)/common \
                        $(LOCAL_PATH)/tlvcmdrsp

LOCAL_CFLAGS+= -D_HOST_SIM_TESTING -D_FOR_HOST_TESTING -DLinux

LOCAL_SHARED_LIBRARIES :=
LOCAL_STATIC_LIBRARIES :=
LOCAL_COPY_HEADERS_TO := libtlv2
LOCAL_COPY_HEADERS :=

include $(BUILD_STATIC_LIBRARY)
