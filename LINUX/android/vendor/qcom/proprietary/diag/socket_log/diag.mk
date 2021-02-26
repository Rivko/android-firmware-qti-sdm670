APPNAME := diag_socket_log

SRCS := diag_socket_log.c

INSTALLDIR := $(BININSTALL)

LIBS += -lstringl -lpthread -ldiag

LIBDIRS += -L$(OBJDIR_ROOT)/lib

include $(MAKESDIR)/make.linux.apps
