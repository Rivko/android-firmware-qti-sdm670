OBJDIR_ROOT = $(OBJDIR)
SRCDIR_ROOT = $(SRCDIR)
LIBDIR = $(OBJDIR_ROOT)/lib
BINDIR = $(OBJDIR_ROOT)/bin

LIBNAME := libqmi_common.so libqmi_encdec.so libqmi_cci.so libqmi_csi.so

LIBS += -lpthread

CINCLUDE += \
	-I$(SRCDIR)/inc \
	-I$(SRCDIR)/common/inc \
	-I$(SRCDIR)/qcci/inc \
	-I$(SRCDIR)/qcsi/inc \
	-I$(SRCDIR)/smem_log

QMI_COMMON_SRC := \
	common/src/common_v01.c
QMI_COMMON_OBJ := $(notdir $(patsubst %.c,%.o,$(QMI_COMMON_SRC)))

QMI_ENCDEC_SRC := \
	encdec/qmi_idl_accessor.c \
	encdec/qmi_idl_lib.c
QMI_ENCDEC_OBJ := $(notdir $(patsubst %.c,%.o,$(QMI_ENCDEC_SRC)))

QMI_CCI_SRC := \
	qcci/src/qmi_cci_common.c \
	qcci/src/qmi_cci_target.c \
	qcci/src/qmi_cci_xport_ipc_router.c \
	smem_log/smem_log.c
QMI_CCI_OBJ := $(notdir $(patsubst %.c,%.o,$(QMI_CCI_SRC)))

QMI_CSI_SRC := \
	qcsi/src/qmi_csi_common.c \
	qcsi/src/qmi_csi_target.c \
	qcsi/src/qmi_csi_xport_ipc_router.c \
	smem_log/smem_log.c
QMI_CSI_OBJ := $(notdir $(patsubst %.c,%.o,$(QMI_CSI_SRC)))

# It's ok to specify all of the sources together, because we want all the
# object files built regardless.
SRCS := $(QMI_CSI_SRC) $(QMI_CCI_SRC) $(QMI_ENCDEC_SRC) $(QMI_COMMON_SRC)

SYSROOT_INCFILES := \
	$(wildcard $(SRCDIR)/inc/*.h) \
	$(wildcard $(SRCDIR)/common/inc/*.h) \
	$(wildcard $(SRCDIR)/qcci/inc/*.h) \
	$(wildcard $(SRCDIR)/qcsi/inc/*.h)

include $(MAKESDIR)/make.linux.lib

CFLAGS += -fno-inline -fno-short-enums -DQMI_FW_SYSLOG \
	-DQMI_FW_SYSLOG_LEVEL=LOG_ERR -DINCLUDE_LINUX_MSM_IPC

SUBDIRS := test_service

MAKEFILE := qmi-framework.mk

# Ensure the libraries are built before the tests
$(SUBDIRS): $(LIBNAME)

# These target-specific overrides are needed to ensure only the necessary
# objects are built into the libraries.
libqmi_common.so: override OBJS=$(QMI_COMMON_OBJ)

libqmi_encdec.so: override OBJS=$(QMI_ENCDEC_OBJ)

libqmi_cci.so: override OBJS=$(QMI_CCI_OBJ)

libqmi_csi.so: override OBJS=$(QMI_CSI_OBJ)

include $(MAKESDIR)/make.subdirs

# TODO: Remove these overrides and fix code!
CONLYFLAGS += -Wno-error=missing-prototypes -Wno-error=strict-prototypes \
	-Wno-error=unused-function -Wno-error=unused-variable
