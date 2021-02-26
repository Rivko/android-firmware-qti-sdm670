# Makefile.am - Automake script for gps event observer
#

AM_CFLAGS = \
     $(LOCPLA_CFLAGS) \
     $(GPSUTILS_CFLAGS) \
     -I$(WORKSPACE)/gps/framework/native/service \
     -std=c++11

libevent_observer_la_SOURCES = \
                      EventObserver.cpp

if USE_GLIB
libevent_observer_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
libevent_observer_la_LDFLAGS = -lstdc++ -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
libevent_observer_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
libevent_observer_la_CFLAGS = $(AM_CFLAGS)
libevent_observer_la_LDFLAGS = -Wl,-z,defs -lpthread -shared -version-info 1:0:0
libevent_observer_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

library_include_HEADERS = \
                    EventObserver.h

libevent_observer_la_LIBADD = -lcutils $(GPSUTILS_LIBS)

#Create and Install libraries
lib_LTLIBRARIES = libevent_observer.la

library_includedir = $(pkgincludedir)
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = event-observer.pc
EXTRA_DIST = $(pkgconfig_DATA)

