OBJDIR_ROOT = $(OBJDIR)
SRCDIR_ROOT = $(SRCDIR)
LIBDIR = $(OBJDIR_ROOT)/lib
BINDIR = $(OBJDIR_ROOT)/bin

APPNAME := qseecom_fsm_lte_client

CPPFLAGS += -I$(SRCDIR)/../QSEEComAPI

LIBS += -lstringl -lQseeComApi -lpthread -lQctPrivs -lstdc++

LIBDIRS  += -L$(OBJDIR_ROOT)/lib

SRCS := qseecom_fsm_lte_client.c

INSTALLDIR := $(TESTINSTALL)

SYSROOT_INCFILES = $(wildcard $(SRCDIR)/../QSEEComAPI/*.h $(SRCDIR)/*.h)

include $(MAKESDIR)/make.linux.apps

# TODO: Remove this override and fix code!
CONLYFLAGS += -Wno-error=missing-prototypes
