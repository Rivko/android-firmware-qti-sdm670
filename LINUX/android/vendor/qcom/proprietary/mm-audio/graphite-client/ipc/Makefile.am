pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ipc.pc
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

c_sources = src/packetizer.c \
            src/ipc.c \
            src/platform_info.c

lib_includedir = $(includedir)/graphite-client/ipc
lib_include_HEADERS = ./inc/packetizer.h

lib_LTLIBRARIES = libgcs-ipc.la
libgcs_ipc_la_CC = @CC@
libgcs_ipc_la_SOURCES = $(c_sources)
libgcs_ipc_la_LIBADD = -lexpat -llog ../osal/libgcs-osal.la
libgcs_ipc_la_CFLAGS = $(AM_CFLAGS) -include stdint.h -DLINUX_ENABLED -DUINT_MAX=\(~0U\) -D__unused=__attribute__\(\(__unused__\)\)
libgcs_ipc_la_LDFLAGS = -shared
