ACLOCAL_AMFLAGS = -I m4

AM_CFLAGS = -Wundef \
            -Wstrict-prototypes \
            -Wno-trigraphs

AM_CPPFLAGS = -D__packed__= \
              -DFEATURE_LE_DIAG \
              -DFEATURE_MDM_LE \
              $(QMI_CFLAGS) \
              -I$(top_srcdir)/dsutils/inc \
              -Dstrlcpy=g_strlcpy \
              -Dstrlcat=g_strlcat

libqcmap_ipc_h_sources =
requiredlibs =
library_includedir = $(pkgincludedir)
library_include_HEADERS = \
                qcmap_utils.h

library_includedir = $(pkgincludedir)

lib_LTLIBRARIES = libqcmapipc.la libqcmaputils.la libqmi_ip.la
libqcmapipc_la_CC = @CC@
libqcmapipc_la_SOURCES = $(libqcmap_ipc_c_sources) $(libqcmap_ipc_h_sources)
libqcmapipc_la_CFLAGS = $(AM_CFLAGS) $(DIAG_CFLAGS) -fPIC
libqcmapipc_la_CPPFLAGS = $(AM_CPPFLAGS)
libqcmapipc_la_LDFLAGS = -shared -version-info 1:0:0
libqcmapipc_la_LIBADD = $(requiredlibs)

libqcmap_utils_h_sources = qcmap_utils.h
requiredlibs =
library_includedir = $(pkgincludedir)

libqcmap_utils_c_sources = qcmap_utils.c

libqcmaputils_la_CC = @CC@
libqcmaputils_la_SOURCES = $(libqcmap_utils_c_sources) $(libqcmap_utils_h_sources)
libqcmaputils_la_CFLAGS = $(AM_CFLAGS) $(DIAG_CFLAGS) -fPIC
libqcmaputils_la_CPPFLAGS = $(AM_CPPFLAGS)
libqcmaputils_la_LDFLAGS = -shared -version-info 1:0:0
libqcmaputils_la_LIBADD = $(requiredlibs)

libqmi_ip_h_sources =
requiredlibs =
library_includedir = $(pkgincludedir)

libqmi_ip_la_CC = @CC@
libqmi_ip_la_SOURCES = $(libqmi_ip_c_sources) $(libqmi_ip_h_sources)
libqmi_ip_la_CFLAGS = $(AM_CFLAGS) $(DIAG_CFLAGS) -fPIC
libqmi_ip_la_CPPFLAGS = $(AM_CPPFLAGS)
libqmi_ip_la_LDFLAGS = -shared -version-info 1:0:0
libqmi_ip_la_LIBADD = $(requiredlibs)
