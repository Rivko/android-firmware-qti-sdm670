MODULE_NAME        := LQE
MODULE_ROOT        := ../../../../../../data-noship
MODULE_CFLAGS      := -DLQE_DATA_OFFTARGET -DFEATURE_LQE_TEST

MODULE_LIBDEPS     := ../../../../../../data-noship/stubs
MODULE_LIBDEPS     += ../../../../../../data-noship/tf
MODULE_LIBDEPS     += ../../../../../../data-noship/qmi_sim
MODULE_LIBDEPS     += ../../../../../../data/configdb/src
MODULE_LIBDEPS     += ../../../../../../qmi/src
MODULE_LIBDEPS     += ../../../../../../qmi/qmi_client_qmux
MODULE_LIBDEPS     += ../../../../../../qmi-framework/encdec
MODULE_LIBDEPS     += ../../../../../../qmi-framework/common/src
MODULE_LIBDEPS     += ../../../../../../qmi-framework/qcci/src
MODULE_LIBDEPS     += ../../../../../../qmi/qmuxd

MODULE_C_SOURCES   := lqe.c \
                      lqe_cmdq.c \
                      lqe_qmi_wds.c \
                      lqe_qmi_ott.c

MODULE_CPP_SOURCES := $(wildcard *.cpp)
MODULE_HEADERS     := $(wildcard *.h)
MODULE_LIB_LOC     := ../lib
MODULE_LIB_TARGET  := liblqe.so
MODULE_BIN_LOC     :=
MODULE_BIN_TARGET  :=

include $(MODULE_ROOT)/test.mk

