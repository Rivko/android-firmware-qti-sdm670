
MODULE_NAME        := QDI
MODULE_CFLAGS      := -DQDI_OFFTARGET
MODULE_ROOT        := ../../../data-noship

MODULE_LIBDEPS     := ../../../data-noship/stubs
MODULE_LIBDEPS     += ../../../data-noship/tf
MODULE_LIBDEPS     += ../../../data-noship/qmi_sim
MODULE_LIBDEPS     += ../../configdb/src
MODULE_LIBDEPS     += ../../../qcril/common/data
MODULE_LIBDEPS     += ../../../qmi/src
MODULE_LIBDEPS     += ../../../qmi/qmi_client_qmux
MODULE_LIBDEPS     += ../../../qmi-framework/encdec
MODULE_LIBDEPS     += ../../../qmi-framework/common/src
MODULE_LIBDEPS     += ../../../qmi-framework/qcci/src
MODULE_LIBDEPS     += ../../../qmi/qmuxd

MODULE_C_SOURCES   := $(wildcard *.c)
MODULE_CPP_SOURCES := $(wildcard *.cpp)
MODULE_HEADERS     := $(wildcard *.h)
MODULE_LIB_LOC     := ../lib
MODULE_LIB_TARGET  := libqdi.so
MODULE_BIN_LOC     :=
MODULE_BIN_TARGET  :=

include $(MODULE_ROOT)/test.mk


