# Makefile.am - Automake script for gps location-service-glue
#

AM_CFLAGS = \
     -I./jni/inc \
     -I./inc \
     -I./common/inc/internal \
     $(GPSUTILS_CFLAGS) \
     $(LOCCORE_CFLAGS) \
     $(LOCFLP_CFLAGS) \
     $(LOCGF_CFLAGS) \
     $(DATAITEMS_CFLAGS) \
     $(ULP2_CFLAGS) \
     $(LOCSERVICE_CFLAGS) \
     $(LOCBASEUTIL_CFLAGS) \
     $(QMIF_CFLAGS) \
     $(QMI_CFLAGS) \
     $(DATA_CFLAGS) \
     $(DSUTILS_CFLAGS) \
     $(DATAITEMS_CFLAGS) \
     -D__func__=__PRETTY_FUNCTION__ \
     -DON_TARGET_TEST \
     -std=c++11

if USE_QCMAP
  AM_CFLAGS += $(LOCNETIFACE_CFLAGS) \
               -DUSE_QCMAP
endif

ACLOCAL_AMFLAGS = -I m4

liblocationservice_glue_la_SOURCES = \
        src/RawCommandInjector.cpp \
        src/OSFrameworkCleaner.cpp \
        src/OsNpGlue.cpp \
        src/Subscription.cpp \
        src/FrameworkActionRequest.cpp \
        src/OSFramework.cpp


if USE_GLIB
liblocationservice_glue_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
liblocationservice_glue_la_LDFLAGS = -lstdc++ -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
liblocationservice_glue_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
liblocationservice_glue_la_CFLAGS = $(AM_CFLAGS)
liblocationservice_glue_la_LDFLAGS = -Wl,-z,defs -lpthread -shared -version-info 1:0:0
liblocationservice_glue_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

liblocationservice_glue_la_LIBADD = -lstdc++ $(GPSUTILS_LIBS) $(LOCCORE_LIBS) $(LOCFLP_LIBS) $(LBSCORE_LIBS) $(ULP2_LIBS) $(LOCSERVICE_LIBS)
if USE_QCMAP
liblocationservice_glue_la_LIBADD += $(LOCNETIFACE_LIBS)
endif

library_include_HEADERS = \
         inc/OSFramework.h \
         inc/OsNpGlue.h \
         inc/Subscription.h \
         inc/FrameworkActionRequest.h

library_includedir = $(pkgincludedir)

#Create and Install libraries
lib_LTLIBRARIES = liblocationservice_glue.la

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = loc-glue.pc
EXTRA_DIST = $(pkgconfig_DATA)
