APPNAME := \
	qmi_test_service_clnt_test_async \
	qmi_test_service_clnt_test_sync \
	qmi_test_service_clnt_test_ind

CINCLUDE += -I$(SRCDIR) -I$(SRCDIR_ROOT)/system_health_monitor

INSTALLDIR := $(TESTINSTALL)

COMMON_OBJS := \
	test_service_v01.o \
	qmi_test_service_clnt_common.o \
	qmi_test_service_clnt_common_stats.o

QMI_TEST_SVC_ASYNC_OBJS := qmi_test_service_clnt_test_async.o $(COMMON_OBJS)
QMI_TEST_SVC_SYNC_OBJS := qmi_test_service_clnt_test_sync.o $(COMMON_OBJS)
QMI_TEST_SVC_IND_OBJS := qmi_test_service_clnt_test_ind.o $(COMMON_OBJS)

# Specify all the sources here, because we need all the objects compiled
# regardless.
SRCS := \
	$(subst qmi_test_service_clnt_test_,femto/qmi_test_service_clnt_test_,\
	$(patsubst %.o,%.c,\
		$(QMI_TEST_SVC_ASYNC_OBJS) \
		$(QMI_TEST_SVC_SYNC_OBJS) $(QMI_TEST_SVC_IND_OBJS)))

LIBS += -lpthread -lrt -lqmi_cci -lqmi_common -lqmi_encdec

LIBDIRS += -L$(OBJDIR_ROOT)/lib

CFLAGS += -Wp,-w

# These target-specific overrides are needed to ensure only the necessary
# objects are included in the apps
qmi_test_service_clnt_test_async: override OBJS=$(QMI_TEST_SVC_ASYNC_OBJS)

qmi_test_service_clnt_test_sync: override OBJS=$(QMI_TEST_SVC_SYNC_OBJS)

qmi_test_service_clnt_test_ind: override OBJS=$(QMI_TEST_SVC_IND_OBJS)

include $(MAKESDIR)/make.linux.apps
