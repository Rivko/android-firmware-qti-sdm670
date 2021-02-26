AM_CFLAGS = \
    -DDEBUG \
    -I./gnss_case \
    -I./geofence_case \
    -I./flp_case \
    -I./locationapi_api \
    -I./ulp_case \
    -I./clientapi_case \
    -I./gnss_multiclient_case \
    $(EVENTOBSERVER_CFLAGS) \
    $(GPSUTILS_CFLAGS) \
    $(LOCCORE_CFLAGS) \
    $(LOCATIONFLP_CFLAGS) \
    $(LOCATIONSERVICE_CFLAGS) \
    $(ULP2_CFLAGS)\
    $(QMIFW_CFLAGS)\
    $(QMI_CFLAGS)\
    $(LOCGLUE_CFLAGS) \
    $(LOCATIONCORE_CFLAGS) \
    $(LOCCLIENTAPI_CFLAGS) \
    $(LOCHALDAEMON_CFLAGS) \
    -std=c++11

required_libs = \
    $(GPSUTILS_LIBS) \
    $(LOCCORE_LIBS) \
    $(LOCHAL_LIBS) \
    $(LOCATIONAPI_LIBS) \
    $(LOCATIONSERVICE_LIBS) \
    $(ULP2_LIBS) \
    $(QMIFW_LIBS) \
    $(QMI_LIBS) \
    $(LOCGLUE_LIBS) \
    $(LOCCLIENTAPI_LIBS) \
    $(DRPLUGIN_LIBS) \
    $(DRCLIENT_LIBS) \
    $(GNSSPPS_LIBS)

libgarden_h_sources = \
    IGardenCase.h \
    GardenFramework.h \
    IGnssAPI.h \
    GnssCbBase.h \
    GardenUtil.h \
    gnss_case/GnssCase.h \
    gnss_case/GnssCb.h \
    geofence_case/GeofenceCb.h \
    geofence_case/GeofenceCase.h \
    flp_case/FlpCb.h \
    flp_case/FlpCase.h \
    locationapi_api/GnssLocationAPI.h \
    locationapi_api/GnssLocationAPIClient.h \
    locationapi_api/GeofenceLocationAPIClient.h \
    locationapi_api/FlpLocationAPIClient.h \
    clientapi_case/LocationClientApiCase.h \
    gnss_multiclient_case/GnssMultiClientCase.h \
    gnss_multiclient_case/GnssMultiClientCb.h \
    gnss_multiclient_case/GnssMultiClientLocationAPIClient.h \
    gnss_multiclient_case/GnssMultiClientCaseUtils.h

libgarden_c_sources = \
    GardenFramework.cpp \
    GardenUtil.cpp \
    GnssCbBase.cpp \
    gnss_case/GnssCase.cpp \
    gnss_case/GnssCb.cpp \
    gnss_multiclient_case/GnssMultiClientCase.cpp \
    gnss_multiclient_case/GnssMultiClientCb.cpp \
    gnss_multiclient_case/GnssMultiClientLocationAPIClient.cpp \
    gnss_multiclient_case/GnssMultiClientCaseUtils.cpp \
    geofence_case/GeofenceCb.cpp \
    geofence_case/GeofenceCase.cpp \
    flp_case/FlpCase.cpp \
    flp_case/FlpCb.cpp \
    locationapi_api/GnssLocationAPI.cpp \
    locationapi_api/GnssLocationAPIClient.cpp \
    locationapi_api/FlpLocationAPIClient.cpp \
    locationapi_api/GeofenceLocationAPIClient.cpp \
    clientapi_case/LocationClientApiCase.cpp

library_includedir = $(pkgincludedir)
library_include_HEADERS = $(libgarden_h_sources)

######################
# Build libgarden
######################
libgarden_la_SOURCES = $(libgarden_c_sources)

if USE_GLIB
libgarden_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
libgarden_la_LDFLAGS = -lstdc++ -lpthread @GLIB_LIBS@ -shared -avoid-version
libgarden_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
libgarden_la_CFLAGS = $(AM_CFLAGS)
libgarden_la_LDFLAGS = -lpthread -shared -version-info 1:0:0
libgarden_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

libgarden_la_LIBADD = -lcutils -ldl $(LOC_LIBS) $(required_libs)
library_include_HEADERS = $(libgarden_h_sources)

lib_LTLIBRARIES = libgarden.la

######################
# Build sample_case
######################
libgardencasesample_la_SOURCES = sample_case/SampleCase.cpp

if USE_GLIB
libgardencasesample_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
libgardencasesample_la_LDFLAGS = -lstdc++ -lpthread @GLIB_LIBS@ -shared -avoid-version
libgardencasesample_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
libgardencasesample_la_CFLAGS = $(AM_CFLAGS)
libgardencasesample_la_LDFLAGS = -lpthread -shared -version-info 1:0:0
libgardencasesample_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

libgardencasesample_la_LIBADD = libgarden.la -lcutils -ldl $(LOC_LIBS) $(required_libs)
lib_LTLIBRARIES += libgardencasesample.la

######################
# Build ulp_case
######################
libgardencaseulp_la_SOURCES = ulp_case/UlpCase.cpp

if USE_GLIB
libgardencaseulp_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
libgardencaseulp_la_LDFLAGS = -lstdc++ -lpthread @GLIB_LIBS@ -shared -avoid-version
libgardencaseulp_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
libgardencaseulp_la_CFLAGS = $(AM_CFLAGS)
libgardencaseulp_la_LDFLAGS = -lpthread -shared -version-info 1:0:0
libgardencaseulp_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

libgardencaseulp_la_LIBADD = libgarden.la -lcutils -ldl $(LOC_LIBS) $(required_libs)
lib_LTLIBRARIES += libgardencaseulp.la

######################
# Build zpp_case
######################
libgardencasezpp_la_SOURCES = zpp_case/ZppCase.cpp

if USE_GLIB
libgardencasezpp_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
libgardencasezpp_la_LDFLAGS = -lstdc++ -lpthread @GLIB_LIBS@ -shared -avoid-version
libgardencasezpp_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
libgardencasezpp_la_CFLAGS = $(AM_CFLAGS)
libgardencasezpp_la_LDFLAGS = -lpthread -shared -version-info 1:0:0
libgardencasezpp_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

libgardencasezpp_la_LIBADD = libgarden.la libgardencaseulp.la -lcutils -ldl $(LOC_LIBS) $(required_libs)
lib_LTLIBRARIES += libgardencasezpp.la

######################
# Build tracking_case
######################
libgardencasetracking_la_SOURCES = tracking_case/garden_app_session_tracker.cpp tracking_case/TrackingCase.cpp

if USE_GLIB
libgardencasetracking_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
libgardencasetracking_la_LDFLAGS = -lstdc++ -lpthread @GLIB_LIBS@ -shared -avoid-version
libgardencasetracking_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
libgardencasetracking_la_CFLAGS = $(AM_CFLAGS)
libgardencasetracking_la_LDFLAGS = -lpthread -shared -version-info 1:0:0
libgardencasetracking_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

libgardencasetracking_la_LIBADD = libgarden.la libgardencaseulp.la -lcutils -ldl $(LOC_LIBS) $(required_libs)
lib_LTLIBRARIES += libgardencasetracking.la

######################
# Build garden_app
######################
garden_app_SOURCES = main.cpp

if USE_GLIB
garden_app_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
garden_app_LDFLAGS = -lstdc++ -g -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
garden_app_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
garden_app_CFLAGS = $(AM_CFLAGS)
garden_app_LDFLAGS = -Wl,-z,defs -lpthread -shared -version-info 1:0:0
garden_app_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

garden_app_LDADD = libgarden.la -lcutils -ldl $(required_libs)

bin_PROGRAMS = garden_app
