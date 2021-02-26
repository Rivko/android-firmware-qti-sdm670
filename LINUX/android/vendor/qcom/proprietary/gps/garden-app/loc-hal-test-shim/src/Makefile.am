AM_CFLAGS = \
            -I. \
            $(GPSUTILS_CFLAGS) \
            $(LOCCORE_CFLAGS) \
            $(LOCHAL_CFLAGS) \
            $(GTI_CFLAGS) \
            $(LOCATIONSERVICE_CFLAGS) \
            -DHAVE_LIBLOG

requiredlibs = $(GPSUTILS_LIBS) $(LOCCORE_LIBS) $(LOCHAL_LIBS)

h_sources = \
    location_callbacks_android_gps.h \
    location_test_interface_android_gps.h

c_sources = \
    location_test_interface_android_gps.cpp

libloc_hal_test_shim_la_SOURCES = $(c_sources) $(h_sources)

if USE_GLIB
libloc_hal_test_shim_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
libloc_hal_test_shim_la_LDFLAGS = -lstdc++ -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
libloc_hal_test_shim_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
libloc_hal_test_shim_la_CFLAGS = $(AM_CFLAGS)
libloc_hal_test_shim_la_LDFLAGS = -shared -Wl,-z,defs -version-info 1:0:0
libloc_hal_test_shim_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

libloc_hal_test_shim_la_LIBADD = $(requiredlibs)

libloc_hal_test_shim_extended_la_SOURCES = \
    location_test_interface_android_gps_extended.cpp

if USE_GLIB
libloc_hal_test_shim_extended_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
libloc_hal_test_shim_extended_la_LDFLAGS = -lstdc++ -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
libloc_hal_test_shim_extended_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
libloc_hal_test_shim_extended_la_CFLAGS = $(AM_CFLAGS)
libloc_hal_test_shim_extended_la_LDFLAGS = -shared -Wl,-z,defs -version-info 1:0:0
libloc_hal_test_shim_extended_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

libloc_hal_test_shim_extended_la_LIBADD = $(requiredlibs) $(LOCATIONSERVICE_LIBS)

h_sources += \
    location_test_interface_android_gps_extended.h

library_include_HEADERS = $(h_sources)

library_includedir = $(pkgincludedir)

lib_LTLIBRARIES = libloc_hal_test_shim.la libloc_hal_test_shim_extended.la
