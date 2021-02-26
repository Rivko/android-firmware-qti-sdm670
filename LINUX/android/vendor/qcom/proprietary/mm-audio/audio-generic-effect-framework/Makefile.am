pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = audio-generic-effect-framework.pc
EXTRA_DIST = $(pkgconfig_DATA)

AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        -I ./api/ \
        $(SYSTEM_MEDIA_CFLAGS)

c_sources = src/qti_gef_api.c

lib_includedir = $(includedir)/audio-generic-effect-framework
lib_include_HEADERS = api/qti_gef_api.h api/qti_gef_datatypes.h

lib_LTLIBRARIES = libqtigef.la
libqtigef_la_CC = @CC@
libqtigef_la_SOURCES = $(c_sources)
libqtigef_la_LIBADD = -lcutils -llog -ldl
libqtigef_la_CFLAGS = $(AM_CFLAGS) -D__unused=__attribute__\(\(__unused__\)\) -Dstrlcat=g_strlcat -DLINUX_ENABLED
libqtigef_la_LDFLAGS = -shared -module -avoid-version
