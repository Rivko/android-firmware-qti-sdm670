pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = gcs.pc
EXTRA_DIST = $(pkgconfig_DATA)

AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        -I ../osal/inc/ \
        -I ../ipc/inc/ \
        -I ../cal-wrapper/inc/ \
        -I ./inc/

c_sources = src/gcs.c

lib_includedir = $(includedir)/graphite-client/gcs
lib_include_HEADERS = inc/graphite_common.h \
                      inc/gcs_api.h

lib_LTLIBRARIES = libgcs.la
libgcs_la_CC = @CC@
libgcs_la_SOURCES = $(c_sources)
libgcs_la_LIBADD = -llog ../osal/libgcs-osal.la ../cal-wrapper/libgcs-calwrapper.la ../ipc/libgcs-ipc.la
libgcs_la_CFLAGS = $(AM_CFLAGS) -DLINUX_ENABLED -include stdint.h
libgcs_osal_la_LDFLAGS = -shared






