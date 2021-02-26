AM_CFLAGS = -Wall \
		-Wundef \
		-Wstrict-prototypes \
		-Wno-trigraphs

AM_CPPFLAGS = -D__packed__= \
		-DIMAGE_APPS_PROC \
		-DFEATURE_Q_SINGLE_LINK \
		-DFEATURE_Q_NO_SELF_QPTR \
		-DFEATURE_LINUX \
		-DFEATURE_NATIVELINUX \
		-DFEATURE_DSM_DUP_ITEMS \
		-DFEATURE_LE_DIAG \
                $(QMIFRAMEWORK_CFLAGS) \
                $(QMI_CFLAGS) \
                $(DIAG_FLAGS) \
		-I../tests \
		-I../tests/wmsts

qmi_simple_ril_test_SOURCES = \
	qmi_client_utils.c \
        qmi_simple_ril_core.c    \
        qmi_simple_ril_voice.c    \
	qmi_simple_ril_misc.c \
	qmi_simple_ril_nas.c \
	qmi_simple_ril_sms.c \
	qmi_simple_ril_ss.c \
        qmi_simple_ril_suite.c \
        qmi_test_console.c \
        fota.c \
        fota_cdma.c \
        wmsts/wmsts.c \
        wmsts/wmstscdma.c \
        wmsts/bit.c

qmi_simple_ril_test_LDFLAGS = -lpthread $(QMIFRAMEWORK_LIBS) -lqmiservices -ldl

bin_PROGRAMS = qmi_simple_ril_test

qmi_simple_ril_test_CC = @CC@
