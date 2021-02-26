pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = osal.pc
EXTRA_DIST = $(pkgconfig_DATA)

AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        -I ./inc/

c_sources = src/osal_lock.c \
            src/osal_mem.c \
            src/osal_cond.c \
            src/osal_thread.c \
            src/osal_dev.c

lib_includedir = $(includedir)/graphite-client/osal
lib_include_HEADERS = inc/osal_types.h \
                      inc/osal_log.h \
                      inc/osal_lock.h \
                      inc/osal_list.h \
                      inc/osal_mem.h \
                      inc/osal_cond.h \
                      inc/osal_thread.h \
                      inc/osal_dev.h \
                      inc/osal_pack_start.h \
                      inc/osal_pack_end.h \
                      inc/osal_errorcodes.h

lib_LTLIBRARIES = libgcs-osal.la
libgcs_osal_la_CC = @CC@
libgcs_osal_la_SOURCES = $(c_sources)
libgcs_osal_la_LIBADD = -llog
libgcs_osal_la_CFLAGS = $(AM_CFLAGS) -D__unused=__attribute__\(\(__unused__\)\) -include stdint.h -include pthread.h
libgcs_osal_la_LDFLAGS = -shared






