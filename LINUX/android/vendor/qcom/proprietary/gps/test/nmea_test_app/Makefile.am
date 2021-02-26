AM_CFLAGS = \
    $(LOCHAL_CFLAGS) \
    $(LOCAPIV02_CFLAGS) \
    $(QMIFW_CFLAGS)

required_libs = \
    $(LOCHAL_LIBS) \
    $(LOCAPIV02_LIBS)

nmea_test_app_c_sources = \
     nmea_test_app.c

nmea_test_app_SOURCES = $(nmea_test_app_c_sources)

if USE_GLIB
nmea_test_app_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
nmea_test_app_LDFLAGS = -lstdc++ -g -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
nmea_test_app_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
nmea_test_app_CFLAGS = $(AM_CFLAGS)
nmea_test_app_LDFLAGS = -Wl,-z,defs -lpthread -shared -version-info 1:0:0
nmea_test_app_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

nmea_test_app_LDADD = -lcutils -ldl $(required_libs)

bin_PROGRAMS = nmea_test_app
