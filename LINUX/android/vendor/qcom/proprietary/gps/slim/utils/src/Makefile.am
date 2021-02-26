AM_CFLAGS = \
            -DFEATURE_LOCTECH_SLIM_NOFLOCK \
            -DFEATURE_LOCTECH_SLIM_DEBUG \
            -DFEATURE_LOCTECH_SLIM_LOG_DIAG \
            -DFEATURE_LOCTECH_SLIM_LOG_CONSOLE \
            -DFEATURE_LOCTECH_SLIM_OS_LOGS \
            -I./ \
            -I../inc \
            -I../src \
            $(QMIF_CFLAGS) \
            $(DIAG_CFLAGS) \
            $(LOCPLA_CFLAGS) \
            $(GPSUTILS_CFLAGS) \
            -D_GNU_SOURCE \
            -D__func__=__PRETTY_FUNCTION__ \
            -DHAVE_LIBLOG

AM_CPPFLAGS = \
            -D__STDC_FORMAT_MACROS \
            -D__STDINT_LIMITS \
            -D__STDINT_MACROS \
            -D_GNU_SOURCE \
            -I./ \
            -I../inc \
            -I../src \
            $(QMIF_CFLAGS) \
            $(DIAG_CFLAGS) \
            -DHAVE_LIBLOG

requiredlibs = -lrt $(DIAG_LIBS) $(GPSUTILS_LIBS)

h_sources = \
            ./slim_os_i.h \
            ./slim_log_utils_i.h \
            ../inc/qmislim_target.h \
            ../inc/slim_list_api.h \
            ../inc/slim_os_api.h \
            ../inc/slim_os_log_api.h \
            ../inc/slim_processor.h

c_sources = \
            slim_internal.c \
            slim_timetick.cpp \
            qmislim_target.c \
            slim_log_utils.c \
            slim_timer.c \
            slim_messaging.c \
            slim_thread.c


library_includedir = $(pkgincludedir)
library_include_HEADERS = $(h_sources)

libslimutils_la_SOURCES = $(c_sources) $(h_sources)

if USE_GLIB
libslimutils_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
libslimutils_la_LDFLAGS = -lstdc++ -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
libslimutils_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
libslimutils_la_CFLAGS = $(AM_CFLAGS)
libslimutils_la_LDFLAGS = -shared -Wl,-z,defs -version-info 1:0:0
libslimutils_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

libslimutils_la_LIBADD = $(requiredlibs)

lib_LTLIBRARIES = libslimutils.la
