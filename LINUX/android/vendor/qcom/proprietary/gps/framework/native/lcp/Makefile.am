# Makefile.am - Automake script for gps location-service
#

AM_CFLAGS = \
     $(GPSUTILS_CFLAGS) \
     $(LOCCORE_CFLAGS) \
     $(LOWICLIENT_CFLAGS) \
     $(LOCBASEUTIL_CFLAGS) \
     $(IZATCORE_CFLAGS) \
     $(DATAITEMS_CFLAGS) \
     -I../core \
     -I../service \
     -I../adapter \
     -I./izat_api  \
     -I./observers/IPCHandler/inc \
     -I./observers/IPCHandler/inc/internal \
     -I./observers/IPCClient/inc \
     -I./observers/IPCClient/inc/internal \
     -I./observers/FreeWifiScanObserver/inc \
     -I./observers/FreeWifiScanObserver/inc/internal \
     -I./izat-provider-service/inc \
     -I./izat-provider-service/inc/internal \
     -I./izat-listeners/inc \
     -I./izat-listeners/inc/internal \
     -I./ipc-messaging-proxy/inc \
     -I./ipc-messaging-proxy/inc/internal \
     -I./utils/inc \
     -I./utils/inc/internal \
     -I./wiper/inc \
     -I./wiper/inc/internal \
     -D__func__=__PRETTY_FUNCTION__ \
     -DON_TARGET_TEST \
     -std=c++11

ACLOCAL_AMFLAGS = -I m4

liblocationservice_la_SOURCES = \
        ../adapter/LBSAdapter.cpp \
        observers/IPCClient/src/IPCClient.cpp \
        observers/IPCHandler/src/IPCHandler.cpp \
        observers/FreeWifiScanObserver/src/FreeWifiScanObserver.cpp \
        izat-provider-service/src/LocationProvider.cpp \
        izat-provider-service/src/IzatManager.cpp \
        izat-provider-service/src/ComboNetworkProvider.cpp \
        izat-provider-service/src/QNP.cpp \
        izat-provider-service/src/GTPWiFiProxy.cpp \
        izat-provider-service/src/GTPWWanProxy.cpp \
        izat-provider-service/src/ZppProxy.cpp \
        izat-provider-service/src/OsNpProxy.cpp \
        izat-listeners/src/IzatPassiveLocationListener.cpp \
        izat-listeners/src/IzatPassiveNmeaListener.cpp \
        izat-listeners/src/IzatPassiveSvInfoListener.cpp \
        wiper/src/Wiper.cpp \
        utils/src/Utils.cpp

if USE_GLIB
liblocationservice_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
liblocationservice_la_LDFLAGS = -lstdc++ -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
liblocationservice_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
liblocationservice_la_CFLAGS = $(AM_CFLAGS)
liblocationservice_la_LDFLAGS = -Wl,-z,defs -lpthread -shared -version-info 1:0:0
liblocationservice_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

liblocationservice_la_LIBADD = -lstdc++ $(GPSUTILS_LIBS) $(LOCCORE_LIBS) $(LOWICLIENT_LIBS) $(LBSCORE_LIBS) $(LOCBASEUTIL_LIBS) $(LOCMQCLIENT_LIBS) $(ULP2_LIBS) $(DATAITEMS_LIBS)

library_include_HEADERS = \
    ../service/ulp_service.h \
    ../service/loc_extended.h \
    ../adapter/LBSAdapter.h \
    ../geofence/GeoFencerProxyMsgs.h \
    izat_api/IIzatManager.h \
    izat_api/IOSFramework.h \
    izat_api/IzatLocation.h \
    izat_api/IOSLocationProvider.h \
    izat_api/IIzatListener.h \
    izat_api/IOSListener.h \
    izat_api/IzatTypes.h \
    izat_api/IzatRequest.h \
    izat_api/IRawCommandInjector.h \
    izat_api/IOSFrameworkCleaner.h \
    utils/inc/IzatDefines.h \
    izat_api/IzatNmea.h \
    izat_api/IzatSvInfo.h

library_includedir = $(pkgincludedir)

#Create and Install libraries
lib_LTLIBRARIES = liblocationservice.la

