pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = smwrapper.pc
EXTRA_DIST = $(pkgconfig_DATA)

AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        $(SYSTEM_MEDIA_CFLAGS)

if QSTHW_API
AM_CFLAGS += -DSM_EXTN_ENABLED
AM_CFLAGS += -I ${STAGING_DIR}/mm-audio/qsthw_api
endif

c_sources = smwrapper.c

lib_LTLIBRARIES = libsmwrapper.la
libsmwrapper_la_CC = @CC@
libsmwrapper_la_SOURCES = $(c_sources)
libsmwrapper_la_LIBADD = -lcutils
libsmwrapper_la_CFLAGS = $(AM_CFLAGS)
libsmwrapper_la_LDFLAGS = -shared -avoid-version
