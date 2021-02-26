APPNAME := test_diag

SRCS := test_diag.c

INSTALLDIR := $(TESTINSTALL)

LIBS += -lstringl -lpthread -ldiag

LIBDIRS += -L$(OBJDIR_ROOT)/lib

include $(MAKESDIR)/make.linux.apps
