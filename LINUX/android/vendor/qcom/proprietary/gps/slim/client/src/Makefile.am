# Makefile.am - Automake script for libslimclient

AM_CFLAGS = \
            -DFEATURE_COARSE_POS_TIMESTAMP_AGE \
            -DFEATURE_LOC_MW_EFS_ACCESS \
            -DFEATURE_LOCTECH_SLIM_NOHANDLES \
            -fvisibility=hidden \
            -DSLIM_API="__attribute__ ((visibility (\"default\")))" \
            -D_GNU_SOURCE \
            -I../inc \
            $(GPSUTILS_CFLAGS) \
            $(SLIMUTILS_CFLAGS) \
            $(SLIMCOMMON_CFLAGS) \
            $(QMIF_CFLAGS) \
            $(DIAG_CFLAGS) \
            -D__func__=__PRETTY_FUNCTION__ \
            -DHAVE_LIBLOG

AM_CPPFLAGS = \
            -D__STDINT_LIMITS \
            -D__STDINT_MACROS \
            -D__STDC_FORMAT_MACROS \
            -D__STDC_CONSTANT_MACROS \
            -D__STDC_LIMIT_MACROS
h_sources = \
           ../inc/SlimClientBase.h

c_sources = \
         QmiSlimClient.cpp \
         QmiSlimMonitor.cpp \
         SlimClientApi.cpp \
         SlimClientBase.cpp \
         SlimProxy.cpp \
         SlimProxyConnection.cpp \
         SlimProviderConf_dummy.cpp

requiredlibs = -lrt $(GPSUTILS_LIBS) $(SLIMUTILS_LIBS) $(SLIMCOMMON_LIBS)

library_includedir = $(pkgincludedir)
library_include_HEADERS = $(h_sources)

libslimclient_la_SOURCES = $(c_sources) $(h_sources)

if USE_GLIB
libslimclient_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
libslimclient_la_LDFLAGS = -lstdc++ -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
libslimclient_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
libslimclient_la_CFLAGS = $(AM_CFLAGS)
libslimclient_la_LDFLAGS = -shared -Wl,-z,defs -version-info 1:0:0
libslimclient_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

libslimclient_la_LIBADD = $(requiredlibs)
lib_LTLIBRARIES = libslimclient.la
