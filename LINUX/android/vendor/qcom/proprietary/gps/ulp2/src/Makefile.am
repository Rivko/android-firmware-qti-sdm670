AM_CFLAGS = \
     $(GPSUTILS_CFLAGS) \
     $(LOCCORE_CFLAGS) \
     $(IZATCORE_CFLAGS) \
     $(LBSCORE_CFLAGS) \
     -I$(WORKSPACE)/gps/framework/native/service \
     $(FLP_CFLAGS) \
     $(DRPLUGIN_CFLAGS) \
     $(EVENTOBSERVER_CFLAGS) \
     -I./ \
     -I../inc \
     -D_HAS_LOC_V02_ \
     -D__func__=__PRETTY_FUNCTION__ \
     -DON_TARGET_TEST \
     -std=c++11

ACLOCAL_AMFLAGS = -I m4

libulp2_la_SOURCES = \
    ulp_main.cpp \
    ulp_data.cpp \
    ulp_msg.cpp \
    ulp_gnss.cpp \
    ulp_gnp.cpp \
    ulp_quipc.cpp \
    ulp_xtwifi.cpp \
    ulp_brain.cpp \
    ulp_monitor.cpp \
    ulp_debug.cpp \
    ulp_log.cpp \
    LocUlpProxy.cpp \
    LocAmtProxy.cpp \
    ulp_zpp.cpp \
    ulp_amt.cpp

if USE_GLIB
libulp2_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
libulp2_la_LDFLAGS = -lstdc++ -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
libulp2_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
libulp2_la_CFLAGS = $(AM_CFLAGS)
libulp2_la_LDFLAGS = -Wl,-z,defs -lpthread -shared -version-info 1:0:0
libulp2_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

libulp2_la_LIBADD = -lstdc++ -ldl $(GPSUTILS_LIBS) $(LOCCORE_LIBS) $(IZATCORE_LIBS) $(LBSCORE_LIBS) $(DRPLUGIN_LIBS) $(EVENTOBSERVER_LIBS)

library_include_HEADERS = \
    LocUlpProxy.h \
    ulp_data.h \
    ulp_internal.h \
    ulp_log.h

library_includedir = $(pkgincludedir)

#Create and Install libraries
lib_LTLIBRARIES = libulp2.la

