/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  Passive Location Listener

  Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZATPASSIVELOCATIONLISTENER_H__
#define __IZATPASSIVELOCATIONLISTENER_H__

#include <map>
#include <comdef.h>
#include <IzatTypes.h>
#include <IzatLocation.h>
#include <IOSListener.h>

namespace izat_manager {
class IzatLocation;

using namespace izat_manager;

class IzatPassiveLocationListener
    : public IOSListener {
public:

    static void startPassiveLocationListener(const std::string clientName,
                                             const IzatListenerMask listensTo,
                                             bool intermediateFixAccepted,
                                             IIzatManager * izatManager,
                                             IPCMessagingProxy *iPCMessagingProxy);
    static void stopAllPassiveLocationListers(IIzatManager * izatManager);

    // override IOSListener
    IzatListenerMask listensTo() const;
    void onLocationChanged(const IzatLocation *location, const IzatLocationStatus status);
    inline void onStatusChanged(const IzatProviderStatus /*status*/) {}
    void ulpLocationFromIzatLocation(const IzatLocation *izatLoc, UlpLocation *ulpLoc, GpsLocationExtended* locExtended);
    inline void onNmeaChanged(const IzatNmea* /*nmea*/) {}


private:

    typedef std::map<const std::string, IzatPassiveLocationListener *> IzatListenerMapType;
    static IzatListenerMapType sObjects;

    const std::string mClientName;
    const std::string mIpcName;
    bool mIntermediateFixAccepted;
    const IzatListenerMask mLocationListenerMask;
    IPCMessagingProxy *mIPCMessagingProxy;

    IzatPassiveLocationListener(const std::string clientName,
                                const IzatListenerMask listensTo,
                                bool intermediateFixAccepted,
                                IIzatManager * izatManager,
                                IPCMessagingProxy *iPCMessagingProxy);
    ~IzatPassiveLocationListener();
};

} // namespace izat_manager
#endif // #ifndef __IZATPASSIVELOCATIONLISTENER_H__

