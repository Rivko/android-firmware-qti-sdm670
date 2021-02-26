OBJDIR_ROOT = $(OBJDIR)
SRCDIR_ROOT = $(SRCDIR)
LIBDIR  = $(OBJDIR_ROOT)/lib
BINDIR  = $(OBJDIR_ROOT)/bin

APPNAME := fsmRemoteStorage
DIRS := qmi_rmt_storage

LIBS += -lqmi_csi -lqmi_cci -lqmi_encdec -lqmi_common -lpthread -lstringl \
	-lQctPrivs -lstdc++ -lQctInfraC -lQctOsal -lQctInfra -lfidwd \
	-lboost_program_options$(BOOST_LIBSUFFIX) \
	-lboost_thread$(BOOST_LIBSUFFIX) -lm

CPPFLAGS += -D_LARGEFILE64_SOURCE -DLOG_NIDEBUG=0 -DRMTS_FSM_BUILD \
	-DPROPERTY_VALUE_MAX=92

INSTALLDIR := $(BININSTALL)

include $(MAKESDIR)/make.linux.apps
