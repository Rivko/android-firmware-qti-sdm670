AM_CFLAGS = \
     -I../ \
     -I../../ \
     $(LOCBASEUTIL_CFLAGS) \
     $(GPSUTILS_CFLAGS) \
     -D__func__=__PRETTY_FUNCTION__ \
     -std=c++11

AM_CPPFLAGS = \
      -std=c++11

h_sources = \
        ../mq_client.h \
        ../mq_client_controller.h \
        ../IPCMessagingProxy.h

libloc_mq_client_la_SOURCES = \
        mq_client.cpp \
        mq_client_controller.cpp \
        IPCMessagingProxy.cpp

library_includedir = $(pkgincludedir)
library_include_HEADERS = $(h_sources)


if USE_GLIB
libloc_mq_client_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
libloc_mq_client_la_LDFLAGS = -lstdc++ -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
libloc_mq_client_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
libloc_mq_client_la_CFLAGS = $(AM_CFLAGS)
libloc_mq_client_la_LDFLAGS = -Wl,-z,defs -lpthread -shared -version-info 1:0:0
libloc_mq_client_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

libloc_mq_client_la_LIBADD = -lstdc++ -ldl $(LOCBASEUTIL_LIBS) $(GPSUTILS_LIBS)

#Create and Install libraries
lib_LTLIBRARIES = libloc_mq_client.la
