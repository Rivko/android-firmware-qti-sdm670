
MODULE_CFLAGS      += -DNETMGR_OFFTARGET -DFEATURE_DATA_PREFIX_DELEGATION

MODULE_LIBDEPS     += ../../../qmi/core/lib/src
MODULE_LIBDEPS     += ../../../qmi/services
MODULE_LIBDEPS     += ../../dsutils/src
MODULE_LIBDEPS     += ../../qdi/src
MODULE_LIBDEPS     += ../../qdp/src
MODULE_LIBDEPS     += ../../dsi_netctrl/src
MODULE_LIBDEPS     += ../../../qcril/common/data
MODULE_LIBDEPS     += ../../../../opensource/dataservices/rmnetctl/src
MODULE_LIBDEPS     += ../../../xmllib/src
MODULE_LIBDEPS     += ../../../data-noship/stubs
MODULE_LIBDEPS     += ../../../data-noship/tf
MODULE_LIBDEPS     += ../../../data-noship/qmi_sim
MODULE_LIBDEPS     += ../../configdb/src
MODULE_LIBDEPS     += ../../../qmi/src
MODULE_LIBDEPS     += ../../../qmi/qmi_client_qmux
MODULE_LIBDEPS     += ../../../qmi-framework/encdec
MODULE_LIBDEPS     += ../../../qmi-framework/common/src
MODULE_LIBDEPS     += ../../../qmi-framework/qcci/src
MODULE_LIBDEPS     += ../../../qmi/qmuxd

VPATH = plugins/modem_proxy

MODULE_NAME        := NETMGR
MODULE_ROOT        := ../../../data-noship
MODULE_C_SOURCES   := $(wildcard *.c)
MODULE_C_SOURCES   += nm_mdmprxy_iptables_helper.c
MODULE_C_SOURCES   += nm_mdmprxy_common.c
MODULE_C_SOURCES   += nm_mdmprxy_mark_provider_factory.c
MODULE_C_SOURCES   += nm_mdmprxy_legacy_mark_provider.c
MODULE_C_SOURCES   += nm_mdmprxy_socket_mark_provider.c
MODULE_C_SOURCES   += nm_mdmprxy_mark_provider_common.c
MODULE_C_SOURCES   += nm_mdmprxy_route_provider_factory.c
MODULE_C_SOURCES   += nm_mdmprxy_legacy_route_provider.c
MODULE_C_SOURCES   += nm_mdmprxy_ndc_route_provider.c
MODULE_C_SOURCES   += nm_mdmprxy_main.c
MODULE_CPP_SOURCES := $(wildcard *.cpp)
MODULE_HEADERS     := $(wildcard *.h)
MODULE_LIB_LOC     := ../lib
MODULE_LIB_TARGET  := libnetmgr.so
MODULE_BIN_LOC     := ../bin
MODULE_BIN_TARGET  := netmgrd

MODULE_LOG_FILE    := netmgrd.log
MODULE_PREBUILTS   := netmgr_config.xml

include $(MODULE_ROOT)/test.mk

