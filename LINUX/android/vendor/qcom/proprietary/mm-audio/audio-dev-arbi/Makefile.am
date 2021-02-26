ACLOCAL_AMFLAGS = -I m4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = aud-dev-arbi.pc
EXTRA_DIST = $(pkgconfig_DATA)

AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g  -O3 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        -I $(WORKSPACE)/system/core/include/

AM_CPPFLAGS = $(SYSTEM_MEDIA_CFLAGS) \
             $(CUTILS_CFLAGS) \
             $(GLIB_CFLAGS) \
             $(LIBHARDWARE_CFLAGS)

h_sources = aud_dev_arbi_client_if.h \
            aud_dev_arbi.h

c_sources = aud_dev_arbi.cpp \
            aud_dev_arbi_server_if.cpp \
            aud_dev_arbi_client_if.cpp

lib_LTLIBRARIES = libaudiodevarb.la
libaudiodevarb_la_CC = @CC@
libaudiodevarb_la_SOURCES = $(c_sources) $(h_sources)
libaudiodevarb_la_LIBADD = -lcutils -llog
libaudiodevarb_la_CPPFLAGS = $(AM_CPPFLAGS) $(AM_CFLAGS) -Dstrlcpy=g_strlcpy -DUSE_GLIB @GLIB_CFLAGS@ -include glib.h
libaudiodevarb_la_LDFLAGS = @GLIB_LIBS@ -shared -avoid-version
