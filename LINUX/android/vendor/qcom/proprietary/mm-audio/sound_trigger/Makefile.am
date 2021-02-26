pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = sound_trigger.pc
EXTRA_DIST = $(pkgconfig_DATA)

AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        $(TINYALSA_CFLAGS) \
        $(HARDWARE_CFLAGS) \
        $(SYSTEM_MEDIA_CFLAGS) \
        $(AUDIO_ROUTE_CFLAGS)

c_sources = sound_trigger_hw.c \
            sound_trigger_platform.c

if SOUND_TRIGGER_TRANSITION
c_sources += sound_trigger_transition.c
AM_CFLAGS += -DST_TRANSITION_ENABLED
endif

lib_includedir = $(includedir)/sound_trigger
lib_include_HEADERS = sound_trigger_prop_intf.h

lib_LTLIBRARIES = sound_trigger_primary_default.la
sound_trigger_primary_default_la_CC = @CC@
sound_trigger_primary_default_la_SOURCES = $(c_sources)
sound_trigger_primary_default_la_LIBADD = -ltinyalsa -laudioroute -lexpat -lcutils -llog -ldl
sound_trigger_primary_default_la_CFLAGS = $(AM_CFLAGS) -DPLATFORM_DEFAULT -DSOUND_TRIGGER_PLATFORM=msm8953 -D__unused=__attribute__\(\(__unused__\)\) -DUINT_MAX=\(~0U\) -Dstrlcat=g_strlcat -include glib.h -DUSE_GLIB @GLIB_CFLAGS@ -DLINUX_ENABLED
sound_trigger_primary_default_la_LDFLAGS = -shared -module -avoid-version $(GLIB_LIBS)
