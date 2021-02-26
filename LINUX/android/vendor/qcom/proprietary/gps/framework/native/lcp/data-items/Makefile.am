AM_CFLAGS = \
     $(LOCBASEUTIL_CFLAGS) \
     -I./inc/ \
     -I./inc/internal \
     $(GPSUTILS_CFLAGS) \
     -D_HAS_LOC_V02_ \
     -I$(WORKSPACE)/hardware/qcom/gps/core/data-items \
     -D__func__=__PRETTY_FUNCTION__ \
     -DON_TARGET_TEST \
     -std=c++11

ACLOCAL_AMFLAGS = -I m4

libdataitems_la_SOURCES = \
    src/DataItemsFactory.cpp \
    src/DataItemConcreteTypes.cpp

if USE_GLIB
libdataitems_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
libdataitems_la_LDFLAGS = -lstdc++ -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
libdataitems_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
libdataitems_la_CFLAGS = $(AM_CFLAGS)
libdataitems_la_LDFLAGS = -Wl,-z,defs -lpthread -shared -version-info 1:0:0
libdataitems_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

libdataitems_la_LIBADD = -lstdc++ $(LOCBASEUTIL_LIBS) $(GPSUTILS_LIBS)

library_include_HEADERS = \
    inc/DataItemConcreteTypeDefaultValues.h \
    inc/DataItemConcreteTypeFieldNames.h \
    inc/DataItemsFactory.h \
    inc/DataItemConcreteTypes.h \
    inc/IDataItemSerializer.h \
    inc/loc_ril.h

#Create and Install libraries
lib_LTLIBRARIES = libdataitems.la
library_includedir = $(pkgincludedir)
pkgconfigdir = $(libdir)/pkgconfig

pkgconfig_DATA = data-items.pc
EXTRA_DIST = $(pkgconfig_DATA)
