AM_CFLAGS = \
     $(QMIF_CFLAGS) \
     $(GPSUTILS_CFLAGS) \
     $(LOCCORE_CFLAGS) \
     $(LOCAPIV02_CFLAGS) \
     $(LOCDS_CFLAGS) \
     $(IZATCORE_CFLAGS) \
     $(LOCAPI_CFLAGS) \
     -I./ \
     -D_HAS_LOC_V02_ \
     -D__func__=__PRETTY_FUNCTION__ \
     -DON_TARGET_TEST \
     -std=c++11

ACLOCAL_AMFLAGS = -I m4

liblbs_core_la_SOURCES = \
    LBSAdapterBase.cpp \
    LBSApiBase.cpp \
    LBSProxy.cpp \
    LocApiProxy.cpp \
    LBSApiV02.cpp

if USE_GLIB
liblbs_core_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
liblbs_core_la_LDFLAGS = -lstdc++ -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
liblbs_core_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
liblbs_core_la_CFLAGS = $(AM_CFLAGS)
liblbs_core_la_LDFLAGS = -Wl,-z,defs -lpthread -shared -version-info 1:0:0
liblbs_core_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

liblbs_core_la_LIBADD = -lstdc++ -ldl $(GPSUTILS_LIBS) $(LOCCORE_LIBS) $(LOCAPIV02_LIBS) $(LOCDS_LIBS) $(IZATCORE_LIBS) $(LOCBASEUTIL_LIBS)

library_include_HEADERS = \
    LBSAdapterBase.h \
    LBSApiBase.h \
    LBSApiV02.h \
    WiperData.h \
    LBSProxy.h \
    LBSApiRpc.h \
    geofence.h \
    LocApiProxy.h \
    SlimSensor.h

library_includedir = $(pkgincludedir)

#Create and Install libraries
lib_LTLIBRARIES = liblbs_core.la

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = lbs-core.pc
sysconf_DATA = $(WORKSPACE)/gps/etc/izat.conf
EXTRA_DIST = $(pkgconfig_DATA)
