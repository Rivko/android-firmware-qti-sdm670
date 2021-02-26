ACLOCAL_AMFLAGS = -I m4

AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g  -O3 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        -I ./inc \
        -I $(WORKSPACE)/system/core/include \
        $(AUDCAL_CFLAGS)

c_sources = src/acdb-fts.c

lib_LTLIBRARIES = libacdb_fts.la
libacdb_fts_la_CC = @CC@
libacdb_fts_la_SOURCES = $(c_sources)
libacdb_fts_la_LIBADD = -laudcal -lcutils -llog
libacdb_fts_la_CFLAGS = $(AM_CFLAGS) -D_ENABLE_QC_MSG_LOG_
libacdb_fts_la_LDFLAGS = -shared -avoid-version
