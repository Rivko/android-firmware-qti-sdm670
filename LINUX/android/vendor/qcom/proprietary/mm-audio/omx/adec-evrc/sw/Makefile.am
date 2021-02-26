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
        -I ./test/ \
         $(MM_CORE_CFLAGS) \
         $(AUDIOALSA_CFLAGS)

test_sources = test/omx_evrc_dec_test.c

bin_PROGRAMS = mm_adec_omxevrc_test
mm_adec_omxevrc_test_CC = @CC@
mm_adec_omxevrc_test_LDADD = $(MM_CORE_LIBS) $(AUDIOALSA_LIBS) $(OMX_NOSHIP_LIBS)
mm_adec_omxevrc_test_SOURCES = $(test_sources)
mm_adec_omxevrc_test_CFLAGS = $(AM_CFLAGS) -DAUDIOV2 -DQC_MODIFIED -DPROFILE_DECODER -DVERBOSE -D_DEBUG
