MODULE_NAME        := DSI
MODULE_ROOT        := ../../../data-noship
MODULE_CFLAGS      := -DDSI_NETCTRL_OFFTARGET -DFEATURE_DSI_TEST

MODULE_LIBDEPS     := ../../../data-noship/stubs
MODULE_LIBDEPS     += ../../../data-noship/tf
MODULE_LIBDEPS     += ../../../data-noship/qmi_sim
MODULE_LIBDEPS     += ../../configdb/src
MODULE_LIBDEPS     += ../../../qmi/src
MODULE_LIBDEPS     += ../../../qmi/qmi_client_qmux
MODULE_LIBDEPS     += ../../../qmi-framework/encdec
MODULE_LIBDEPS     += ../../../qmi-framework/common/src
MODULE_LIBDEPS     += ../../../qmi-framework/qcci/src
MODULE_LIBDEPS     += ../../../qmi/qmuxd

MODULE_C_SOURCES   := dsi_config.c \
                      dsi_netctrl.c \
                      dsi_netctrl_cb_thrd.c \
                      dsi_netctrl_embms.c \
                      dsi_netctrli.c \
                      dsi_netctrl_init.c \
                      dsi_netctrl_mni.c \
                      dsi_netctrl_mni_cb.c \
                      dsi_netctrl_multimodem.c \
                      dsi_netctrl_netmgr.c \
                      dsi_netctrl_platform.c \
                      dsi_netctrl_qmi.c \
                      dsi_netctrl_qmi_wds.c \
                      dsi_netctrl_qmi_qos.c \
                      dsi_netctrl_qos.c

MODULE_CPP_SOURCES := $(wildcard *.cpp)
MODULE_HEADERS     := $(wildcard *.h)
MODULE_LIB_LOC     := ../lib
MODULE_LIB_TARGET  := libdsi_netctrl.so
MODULE_BIN_LOC     :=
MODULE_BIN_TARGET  :=

MODULE_PREBUILTS   := dsi_config.xml

include $(MODULE_ROOT)/test.mk

