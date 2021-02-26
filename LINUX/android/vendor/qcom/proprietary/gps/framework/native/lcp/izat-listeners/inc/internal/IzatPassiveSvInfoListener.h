/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  Passive Location Listener

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZATPASSIVESVINFOLISTENER_H__
#define __IZATPASSIVESVINFOLISTENER_H__

#include <map>
#include <comdef.h>
#include <IzatTypes.h>
#include <IzatSvInfo.h>
#include <IOSListener.h>

namespace izat_manager {
class IzatSvInfo;

using namespace izat_manager;

class IzatPassiveSvInfoListener
    : public IOSListener {
public:

    static void startPassiveSvInfoListener(const std::string clientName,
                                             const IzatListenerMask listensTo,
                                             IIzatManager * izatManager,
                                             IPCMessagingProxy *iPCMessagingProxy);
    static void stopAllPassiveSvInfoListeners(IIzatManager * izatManager);

    // override IOSListener
    IzatListenerMask listensTo() const;
    inline void onLocationChanged(const IzatLocation* /*location*/,
            const IzatLocationStatus /*status*/){}
    inline void onStatusChanged(const IzatProviderStatus /*status*/) {}
    inline void onNmeaChanged(const IzatNmea* /*nmea*/) {}
    void onSvReportChanged(const IzatSvInfo *svInfo) ;


private:

    typedef std::map<const std::string, IzatPassiveSvInfoListener *> IzatListenerMapType;
    static IzatListenerMapType sObjects;

    const std::string mClientName;
    const std::string mIpcName;
    const IzatListenerMask mListenerMask;
    IPCMessagingProxy *mIPCMessagingProxy;

    IzatPassiveSvInfoListener(const std::string clientName,
                                const IzatListenerMask listensTo,
                                IIzatManager * izatManager,
                                IPCMessagingProxy *iPCMessagingProxy);
    ~IzatPassiveSvInfoListener();
};

} // namespace izat_manager
#endif // #ifndef __IZATPASSIVESVINFOLISTENER_H__

