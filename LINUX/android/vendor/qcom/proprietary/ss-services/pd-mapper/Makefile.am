AM_CFLAGS = -Wall \
        -Wundef \
        -Wstrict-prototypes \
        -g -O0  \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        $(QMIF_CFLAGS) \
        $(LIBJSON_CFLAGS) \
        -I .

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -D__packed__=

c_sources = ./pd-mapper-svc/pd-mapper.cpp

h_sources = ./pd-mapper-svc/pd-mapper.h


requiredlibs = -lm \
               -lpthread \
               $(LIBJSON_LIBS)

bin_PROGRAMS = pdmappersvc
pdmappersvc_CC = @CC@
pkgconfigdir = $(libdir)/pkgconfig
pdmappersvc_SOURCES = $(c_sources) $(h_sources)
pdmappersvc_LDADD = $(requiredlibs) $(QMIF_LIBS)
