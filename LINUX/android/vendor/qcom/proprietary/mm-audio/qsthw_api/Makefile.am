if QSTHW_API
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = qsthw_api.pc
EXTRA_DIST = $(pkgconfig_DATA)

AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-short-enums \
        -fpic \
        -I .

lib_includedir = $(includedir)/mm-audio/qsthw_api
lib_include_HEADERS = qsthw_api.h qsthw_defs.h

lib_LTLIBRARIES = libqtisoundtriggerhw.la
libqtisoundtriggerhw_la_CC = @CC@
libqtisoundtriggerhw_la_SOURCES = qsthw_api.c
libqtisoundtriggerhw_la_LIBADD = -ltinyalsa -lhardware -lexpat -lcutils -llog -ldl
libqtisoundtriggerhw_la_CFLAGS = $(AM_CFLAGS)
libqtisoundtriggerhw_la_CFLAGS += -DNDEBUG
libqtisoundtriggerhw_la_LDFLAGS = -shared -module -avoid-version

###Generate test app ####
bin_PROGRAMS = qti_sound_trigger_test
qti_sound_trigger_test_SOURCES = qsthw_test.c
qti_sound_trigger_test_CFLAGS = $(AM_CFLAGS) -D__unused=__attribute__\(\(__unused__\)\) -DLINUX_ENABLED
qti_sound_trigger_test_LDADD = -lcutils libqtisoundtriggerhw.la -lpthread
endif
