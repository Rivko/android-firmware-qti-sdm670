OBJDIR_ROOT = $(OBJDIR)
SRCDIR_ROOT = $(SRCDIR)
LIBDIR = $(OBJDIR_ROOT)/lib
BINDIR = $(OBJDIR_ROOT)/bin

LIBNAME := libdiag.so

CINCLUDE += \
	-I$(SRCDIR)/include \
	-I$(SRCDIR)/src

CPPFLAGS += -DFEATURE_LE_DIAG -DTARGET_FSM_PRODUCTS

# These are also inherited by subdirs
LIBS += -ltime_genoff -lstringl

SYSROOT_INCFILES := $(wildcard $(SRCDIR)/include/*.h)

include $(MAKESDIR)/make.linux.lib

ifdef BUILD_TEST
SUBDIRS := test klog PktRspTest socket_log

$(SUBDIRS): $(LIBNAME)

MAKEFILE := diag.mk

include $(MAKESDIR)/make.subdirs
endif

# TODO: Remove this override and fix code!
CONLYFLAGS += -Wno-error=missing-prototypes -Wno-error=strict-prototypes \
	-Wno-error=implicit-function-declaration -Wno-error=nested-externs \
	-Wno-error=unused-variable -Wno-error=unused-function
