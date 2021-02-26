pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = calwrapper.pc
EXTRA_DIST = $(pkgconfig_DATA)

AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        -I ../osal/inc/ \
        -I ./inc/

c_sources = src/gcs_acdb.c

lib_includedir = $(includedir)/graphite-client/cal-wrapper
lib_include_HEADERS = inc/cal_wrapper.h

lib_LTLIBRARIES = libgcs-calwrapper.la
libgcs_calwrapper_la_CC = @CC@
libgcs_calwrapper_la_SOURCES = $(c_sources)
libgcs_calwrapper_la_LIBADD = -llog ../osal/libgcs-osal.la -ldl
libgcs_calwrapper_la_CFLAGS = $(AM_CFLAGS) -include stdint.h -include errno.h
libgcs_calwrapper_la_LDFLAGS = -shared
