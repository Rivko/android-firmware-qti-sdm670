AM_CFLAGS = \
     -I./ \
     $(LOCPLA_CFLAGS) \
     $(LOCMQCLIENT_CFLAGS) \
     $(GPSUTILS_CFLAGS) \
     $(LOCBASEUTIL_CFLAGS) \
     -D__func__=__PRETTY_FUNCTION__ \
     -fno-short-enums \
     -std=c++11

loc_launcher_SOURCES = \
    process_launcher.cpp \
    mq_server/mq_server.cpp \
    mq_server/mq_client_info.cpp \
    mq_server/mq_server_launcher.cpp

library_includedir = $(pkgincludedir)
#library_include_HEADERS = $(h_sources)

if USE_GLIB
loc_launcher_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
loc_launcher_LDFLAGS = -lstdc++ -g -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
loc_launcher_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
loc_launcher_CFLAGS = $(AM_CFLAGS)
loc_launcher_LDFLAGS = -Wl,-z,defs -lpthread -shared -version-info 1:0:0
loc_launcher_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

loc_launcher_LDADD = -lstdc++ -ldl -llog $(LOCBASEUTIL_LIBS) $(GPSUTILS_LIBS) $(LOCMQCLIENT_LIBS)

#Create and Install libraries
bin_PROGRAMS = loc_launcher
