ACLOCAL_AMFLAGS = -I m4

AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g  -O3 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        -I $(WORKSPACE)/system/core/include/ \
        -I ./inc \
        $(TINYALSA_CFLAGS) \
        $(ACDBLOADER_CFLAGS) \
        $(AUDCAL_CFLAGS)

c_sources = src/acdb-mcs-test.c \
            src/acdb-mcs.c \
            src/mcs-route-ctrl.c

bin_PROGRAMS = acdb_mcs_test
acdb_mcs_test_CC = @CC@
acdb_mcs_test_SOURCES = $(c_sources)
acdb_mcs_test_LDADD = -lacdbloader -laudcal -lcutils -ltinyalsa -llog -lpthread
acdb_mcs_test_CFLAGS = $(AM_CFLAGS) -D_TINY_ALSA_LIB_ -D TAVIL_MIXER_CONTROL_FILE_PATH=\"/system/etc/audio_tuning_mixer_tavil.txt\" -D MIXER_CONTROL_FILE_PATH=\"/system/etc/audio_tuning_mixer.txt\" -D TASHA_MIXER_CONTROL_FILE_PATH=\"/system/etc/audio_tuning_mixer_tasha.txt\"
acdb_mcs_test_CFLAGS += -D__unused=__attribute__\(\(__unused__\)\)
