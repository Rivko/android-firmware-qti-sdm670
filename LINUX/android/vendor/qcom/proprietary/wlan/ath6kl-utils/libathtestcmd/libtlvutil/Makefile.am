AM_CFLAGS = -Wall \
            -g -O0 \
            -D_HOST_SIM_TESTING \
            -D_FOR_HOST_TESTING \
            -I ../include \
            -I templete \
            -I common \
            -I defParm

if USE_GLIB
AM_CFLAGS += -DUSE_GLIB $(GLIB_CFLAGS)
endif

c_sources = templete/_cmd_txParmTxtTemplate.c \
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
            hex_dump.c \
            testUtil.c \
            parseRxBinCmdTlv.c \
            genTxBinCmdTlv.c \
            cmdParmsTemplate.c \
            CliTlvCmd_if.c

lib_LIBRARIES  = libtlvutil.a
libtlvutil_a_SOURCES = $(c_sources)
libtlvutil_a_LDFLAGS = -static
pkginclude_HEADERS = common/tcmdHostInternal.h
