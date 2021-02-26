VPATH += ../platform/

ACLOCAL_AMFLAGS = -I m4

AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs

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
        $(DSUTILS_CFLAGS) \
        $(CONFIGDB_CFLAGS)

h_sources = ../inc/qmi.h \
        ../inc/qmi_errors.h\
        ../inc/qmi_port_defs.h \
        ../inc/qmi_eap_srvc.h \
        ../inc/qmi_wds_srvc.h \
        ../inc/qmi_wds_utils.h \
        ../inc/qmi_nas_srvc.h \
        ../inc/qmi_qos_srvc.h \
        ../inc/qmi_atcop_srvc.h \
        ../services/common_v01.h \
        ../platform/qmi_platform.h \
        ../platform/qmi_platform_config.h \
        ../platform/qmi_platform_qmux_if.h \
        ../platform/qmi_platform_qmux_io.h \
        ../tests/qmi_client_utils.h \
        ../core/lib/inc/qmi_idl_lib.h \
        ../core/lib/inc/qmi_idl_lib_internal.h \
        qmi_i.h \
        qmi_util.h \
        qmi_service.h \
        qmi_qmux_if.h \
        qmi_qmux.h \
        qmi_qos_srvc_i.h \
        qmi_wds_srvc_i.h

requiredlibs = -lpthread -lconfigdb -ldsutils \
        ../core/lib/src/libqmiidl.la \
        ../services/libqmiservices.la

library_includedir = $(pkgincludedir)
library_include_HEADERS = $(h_sources)

c_sources  = qmi_service.c \
        qmi.c \
        qmi_wds_srvc.c \
        qmi_qos_srvc.c \
        qmi_nas_srvc.c \
        qmi_eap_srvc.c \
        qmi_atcop_srvc.c

lib_LTLIBRARIES = libqmi.la
libqmi_la_CC = @CC@
libqmi_la_SOURCES = $(c_sources) $(h_sources)
libqmi_la_CFLAGS = -DUSE_GLIB -Dstrlcpy=g_strlcpy $(AM_CFLAGS) $(DIAG_CFLAGS) $(CONFIGDB_CFLAGS) @GLIB_CFLAGS@ -fPIC
libqmi_la_CPPFLAGS =$(AM_CPPFLAGS)
libqmi_la_LDFLAGS = $(DIAG_LIBS) $(CONFIGDB_LIBS) -shared -version-info 1:0:0
libqmi_la_LIBADD = $(requiredlibs)
