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
         $(MM_CORE_CFLAGS) \
         $(AUDIOALSA_CFLAGS)

AM_CPPFLAGS = -I ./inc/ \
              $(MM_CORE_CFLAGS)

c_sources = src/adec_svr.c \
            src/omx_g711_adec.cpp

#LIBRARY for Android apps
lib_LTLIBRARIES = libOmxG711Dec.la
libOmxG711Dec_la_CC = @CC@
libOmxG711Dec_la_SOURCES = $(c_sources)
libOmxG711Dec_la_CFLAGS = $(AM_CFLAGS) -DUSE_GLIB @GLIB_CFLAGS@ -DQC_MODIFIED -D_ENABLE_QC_MSG_LOG_ -DVERBOSE -D_DEBUG -Wconversion -D__unused=__attribute__\(\(__unused__\)\)
libOmxG711Dec_la_CPPFLAGS = $(AM_CPPFLAGS) -Dstrlcpy=g_strlcpy -DUSE_GLIB @GLIB_CFLAGS@  -DQC_MODIFIED -D_ENABLE_QC_MSG_LOG_ -DVERBOSE -D_DEBUG -Wconversion -include glib.h -D__unused=__attribute__\(\(__unused__\)\)
libOmxG711Dec_la_LDFLAGS = $(GLIB_LIBS) -shared -avoid-version

test_sources = test/omx_g711_dec_test.c

bin_PROGRAMS = mm_adec_omxg711_test
mm_adec_omxg711_test_CC = @CC@
mm_adec_omxg711_test_LDADD = $(MM_CORE_LIBS) $(AUDIOALSA_LIBS) libOmxG711Dec.la
mm_adec_omxg711_test_SOURCES = $(test_sources)
mm_adec_omxg711_test_CFLAGS = $(AM_CFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
mm_adec_omxg711_test_LDFLAGS = -llog
