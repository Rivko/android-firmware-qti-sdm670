AM_CFLAGS = \
     $(GPSUTILS_CFLAGS) \
     $(LOCCORE_CFLAGS) \
     $(LOCBASEUTIL_CFLAGS) \
     $(LOCMQCLIENT_CFLAGS) \
     $(LOCATIONSERVICE_CFLAGS) \
     -I./ \
     -std=c++11

libizat_client_api_la_SOURCES = \
    ./IzatRemoteApi.cpp \
    ./izat_remote_api.cpp

if USE_GLIB
libizat_client_api_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
libizat_client_api_la_LDFLAGS = -lstdc++ -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
libizat_client_api_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
libizat_client_api_la_CFLAGS = $(AM_CFLAGS)
libizat_client_api_la_LDFLAGS = -Wl,-z,defs -lpthread -shared -version-info 1:0:0
libizat_client_api_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

libizat_client_api_la_LIBADD = -lstdc++ $(LOCCORE_LIBS) $(GPSUTILS_LIBS) $(LOCBASEUTIL_LIBS) $(LOCMQCLIENT_LIBS)


#Create and Install libraries
lib_LTLIBRARIES = libizat_client_api.la

library_includedir = $(pkgincludedir)
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = izat-client-api.pc
EXTRA_DIST = $(pkgconfig_DATA)




