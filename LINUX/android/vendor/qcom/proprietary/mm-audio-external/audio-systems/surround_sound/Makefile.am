pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = surround-sound.pc
EXTRA_DIST = $(pkgconfig_DATA)

AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        -I $(WORKSPACE)/mm-audio-external-noship/audio-systems/audio-effects/safx/CSIM/common/comdef/

h_sources = ola_filters/surround_filters_interface.h \
            libsurround_proc/profile.h

lib_includedir = $(includedir)/surround_sound/
lib_include_HEADERS = $(h_sources)

lib_LTLIBRARIES = libsurround_sound_headers.la
libsurround_sound_headers_la_SOURCES = comdef.h
libsurround_sound_headers_la_LDFLAGS = -shared -version-info 1:0:0
