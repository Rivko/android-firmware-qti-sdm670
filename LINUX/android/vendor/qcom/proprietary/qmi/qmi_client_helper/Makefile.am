VPATH += ../platform/

ACLOCAL_AMFLAGS = -I m4

AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        $(DIAG_CFLAGS) \
        $(QMIFRAMEWORK_CFLAGS) \
        $(QMI_CFLAGS) \
        $(DSUTILS_CFLAGS)


AM_CPPFLAGS = -D__packed__= \
        -DIMAGE_APPS_PROC \
        -DFEATURE_Q_SINGLE_LINK \
        -DFEATURE_Q_NO_SELF_QPTR \
        -DFEATURE_LINUX \
        -DFEATURE_NATIVELINUX \
        -I../core/lib/inc/ \
        -I../services \
        -I../src \
        -I../inc \
        -I../proxy \
        -I../platform \
        $(DIAG_CFLAGS) \
        $(QMIFRAMEWORK_CFLAGS) \
        $(QMI_CFLAGS) \
        $(DSUTILS_CFLAGS)


h_sources = qmi_client_helper.h

requiredlibs = -lpthread

library_includedir = $(pkgincludedir)
library_include_HEADERS = $(h_sources)

c_sources  = qmi_client_helper.c

lib_LTLIBRARIES = libqmi_client_helper.la
libqmi_client_helper_la_CC = @CC@

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = qmi-client-helper.pc
EXTRA_DIST = $(pkgconfig_DATA)

libqmi_client_helper_la_SOURCES = $(c_sources) $(h_sources)
libqmi_client_helper_la_CFLAGS = $(AM_CFLAGS) $(DSUTILS_CFLAGS) $(QMIFRAMEWORK_CFLAGS) $(QMI_CFLAGS)  -fPIC
libqmi_client_helper_la_CPPFLAGS = $(AM_CPPFLAGS)
libqmi_client_helper_la_LDFLAGS = $(QMIFRAMEWORK_LIBS) $(QMI_LIBS) $(DSUTILS_LIBS) -shared -version-info 1:0:0
libqmi_client_helper_la_LIBADD = $(requiredlibs)
