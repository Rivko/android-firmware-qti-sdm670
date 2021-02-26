APPNAME := PktRspTest

SRCS := PktRspTest.c

# Since we don't want this installed to the target, put it in a separate
# directory.
INSTALLDIR := internal

LIBS += -lpthread -ldiag

LIBDIRS += -L$(OBJDIR_ROOT)/lib

include $(MAKESDIR)/make.linux.apps
