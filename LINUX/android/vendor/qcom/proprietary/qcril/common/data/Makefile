MODULE_NAME        := QCRIL_DATA
MODULE_ROOT        := ../../../data-noship

MODULE_CFLAGS := -DRIL_REQUEST_SET_INITIAL_ATTACH_APN

MODULE_C_SOURCES   := qcril_data_embms.c \
                      qcril_data_stubs.c \
                      qcril_data_utils.c \
                      qcril_data_client.c \
                      qcril_data_qmi_wds.c \
                      qcril_data_qos.c \
                      qcril_data_lqe.c \
                      qcril_data_netctrl.c \
                      qcril_data_req_resp.c

MODULE_CPP_SOURCES := $(wildcard *.cpp)
MODULE_HEADERS     := $(wildcard *.h)

MODULE_LIB_LOC     := ../lib
MODULE_LIB_TARGET  := libqcrildata.so

include $(MODULE_ROOT)/test.mk
