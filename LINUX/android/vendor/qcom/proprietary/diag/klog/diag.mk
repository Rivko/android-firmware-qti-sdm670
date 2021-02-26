APPNAME := diag_klog

SRCS := diag_klog.c

INSTALLDIR := $(TESTINSTALL)

LIBS += -lstringl -lpthread -ldiag

LIBDIRS += -L$(OBJDIR_ROOT)/lib

include $(MAKESDIR)/make.linux.apps
