pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = qap_wrapper.pc
EXTRA_DIST = $(pkgconfig_DATA)

AM_CFLAGS = -DLINUX_ENABLED
AM_CFLAGS = -Werror
AM_CFLAGS += -D__unused=__attribute__\(\(__unused__\)\) $(GLIB_CFLAGS)

qap_sources = inc/qap_module.h \
              inc/qap_api.h \
              inc/qap_defs.h \
              inc/dolby_ms12.h \
              inc/dts_m8.h

qap_includedir = $(includedir)
qap_include_HEADERS = $(qap_sources)

AM_CFLAGS += -I ./ \
             -I ./inc

c_sources = src/qap_api.c

#LIBRARY for LE apps
lib_LTLIBRARIES = libqap_wrapper.la
libqap_wrapper_la_CC = @CC@
libqap_wrapper_la_LIBADD = -laudio_ip_handler -ldl -lexpat $(GLIB_LIBS)
libqap_wrapper_la_SOURCES = $(c_sources)
libqap_wrapper_la_CFLAGS = $(AM_CFLAGS) -include stdio.h
libqap_wrapper_la_LDFLAGS = $(LDFLAGS) -llog -shared -avoid-version

test_sources = test/qap_wrapper_test.c

bin_PROGRAMS = qap_wrapper_test
qap_wrapper_test_CC = @CC@
qap_wrapper_test_SOURCES = $(test_sources)
qap_wrapper_test_LDADD = libqap_wrapper.la -llog
qap_wrapper_test_CFLAGS = $(AM_CFLAGS)
