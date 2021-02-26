/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  Passive Nmea Listener

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZATPASSIVENMEALISTENER_H__
#define __IZATPASSIVENMEALISTENER_H__

#include <map>
#include <comdef.h>
#include <IzatTypes.h>
#include <IzatNmea.h>
#include <IOSListener.h>


namespace izat_manager {
class IzatNmea;

class IzatPassiveNmeaListener
    : public IOSListener {
public:

    static void startPassiveNmeaListener(const std::string clientName,
                                             const IzatListenerMask listensTo,
                                             IIzatManager * izatManager,
                                             IPCMessagingProxy *iPCMessagingProxy);
    static void stopAllPassiveNmeaListers(IIzatManager * izatManager);
     // override IOSListener
    IzatListenerMask listensTo() const;
    inline void onLocationChanged(const IzatLocation* /*location*/,
            const IzatLocationStatus /*status*/) {}
    void onNmeaChanged(const IzatNmea *nmea);
    inline void onStatusChanged(const IzatProviderStatus /*status*/) {}

private:

    typedef std::map<const std::string, IzatPassiveNmeaListener *> IzatListenerMapType;
    static IzatListenerMapType sObjects;

    const std::string mClientName;
    const std::string mIpcName;
    const IzatListenerMask mListenerMask;
    IPCMessagingProxy *mIPCMessagingProxy;

    IzatPassiveNmeaListener(const std::string clientName,
                                const IzatListenerMask listensTo,
                                IIzatManager * izatManager,
                                IPCMessagingProxy *iPCMessagingProxy);
    ~IzatPassiveNmeaListener();
};

} // namespace izat_manager
#endif // #ifndef __IZATPASSIVENMEALISTENER_H__

