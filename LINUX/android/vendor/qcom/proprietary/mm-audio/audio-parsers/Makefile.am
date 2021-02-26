pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = audio-parsers.pc
EXTRA_DIST = $(pkgconfig_DATA)

AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        -I ./inc/

c_sources = src/audio_parsers_interface.c \
            src/audio_parsers_ac3.c \
            src/audio_parsers_dts.c

if DTSHD_PARSER
c_sources += src/audio_parsers_dtshd.c
AM_CFLAGS += -DDTSHD_PARSER_ENABLED
endif

lib_includedir = $(includedir)/audio/mm-audio/audio-parsers
lib_include_HEADERS = inc/audio_parsers.h

#LIBRARY for Android apps
lib_LTLIBRARIES = libaudioparsers.la
libaudioparsers_la_CC = @CC@
libaudioparsers_la_SOURCES = $(c_sources)
libaudioparsers_la_CFLAGS = $(AM_CFLAGS)
libaudioparsers_la_CFLAGS += -D__unused=__attribute__\(\(__unused__\)\)
if DTSHD_PARSER
libaudioparsers_la_LIBADD = -ldtshdpacketizer
endif
libaudioparsers_la_LDFLAGS = -shared -avoid-version
