LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CLANG := true
LOCAL_MODULE = libtlvutil
LOCAL_MODULE_TAGS := debug eng optional
LOCAL_CFLAGS =
LOCAL_SRC_FILES = \
				  templete/_cmd_txParmTxtTemplate.c \
				  templete/_cmd_txParmBinTemplate.c \
				  templete/_cmd_rxParmTxtTemplate.c \
				  templete/_cmd_rxParmBinTemplate.c \
				  templete/_cmd_calParmTxtTemplate.c \
				  templete/_cmd_calParmBinTemplate.c \
				  templete/_cmd_calDoneParmTxtTemplate.c \
				  templete/_cmd_calDoneParmBinTemplate.c \
				  templete/_cmd_rspGenericParmTxtTemplate.c \
				  templete/_cmd_rspGenericParmBinTemplate.c \
				  templete/_cmd_submitreportParmTxtTemplate.c \
				  templete/_cmd_submitreportParmBinTemplate.c \
				  templete/_cmd_pmParmTxtTemplate.c \
				  templete/_cmd_pmParmBinTemplate.c \
				  templete/_cmd_nartCmdParmTxtTemplate.c \
				  templete/_cmd_nartCmdParmBinTemplate.c \
				  templete/_cmd_nartRspParmTxtTemplate.c \
				  templete/_cmd_nartRspParmBinTemplate.c \
				  templete/_cmd_txStatusParmTxtTemplate.c \
				  templete/_cmd_txStatusParmBinTemplate.c \
				  templete/_cmd_rxStatusParmTxtTemplate.c \
				  templete/_cmd_rxStatusParmBinTemplate.c \
				  hex_dump.c testUtil.c parseRxBinCmdTlv.c genTxBinCmdTlv.c cmdParmsTemplate.c \
				  CliTlvCmd_if.c

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../include \
					$(LOCAL_PATH)/templete \
					$(LOCAL_PATH)/common \
					$(LOCAL_PATH)/defParm \

LOCAL_CFLAGS+= -D_HOST_SIM_TESTING -D_FOR_HOST_TESTING

LOCAL_SHARED_LIBRARIES :=
LOCAL_STATIC_LIBRARIES :=
LOCAL_COPY_HEADERS_TO := libtlvutil
LOCAL_COPY_HEADERS := common/tcmdHostInternal.h

include $(BUILD_STATIC_LIBRARY)
