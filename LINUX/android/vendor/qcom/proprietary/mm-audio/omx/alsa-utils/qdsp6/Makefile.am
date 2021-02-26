pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ../../omx.pc
EXTRA_DIST = $(pkgconfig_DATA)

AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        -I ./inc/ \
        -I $(WORKSPACE)/system/core/include/ \
         $(MM_CORE_CFLAGS) \
         $(AUDIOALSA_CFLAGS) \
         $(ALSA_INTF_CFLAGS) \
         $(SYSTEM_MEDIA_CFLAGS)


c_sources = src/omx_alsa_utils.c

lib_includedir = $(includedir)/mm-audio/omx/alsa-utils/qdsp6/inc/
lib_include_HEADERS = inc/omx_alsa_utils.h

#LIBRARY for Android apps
lib_LTLIBRARIES = libalsautils.la
libalsautils_la_CC = @CC@
libalsautils_la_SOURCES = $(c_sources)
libalsautils_la_CFLAGS = $(AM_CFLAGS) -DQDSP6V2 -DUSE_GLIB @GLIB_CFLAGS@
libalsautils_la_LDFLAGS = $(ALSA_INTF_LIBS) $(MM_CORE_LIBS) $(AUDIOALSA_LIBS) $(GLIB_LIBS) -llog -shared -avoid-version
