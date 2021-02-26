/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IFreeWifiScanSubscription

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_IFREEWIFISCANSUBSCRIPTION_H__
#define __IZAT_MANAGER_IFREEWIFISCANSUBSCRIPTION_H__

namespace qc_loc_fw {
class LOWIAsyncDiscoveryScanResultRequest;
}

namespace izat_manager {

using namespace qc_loc_fw;

class IFreeWifiScanObserver;

// provides interface to subscribe/unsubscribe for wifi scans
class IFreeWifiScanSubscription {

public:
    virtual void subscribe (const IFreeWifiScanObserver * observer) = 0;
    virtual void unsubscribe (const IFreeWifiScanObserver * observer) = 0;
    virtual ~IFreeWifiScanSubscription () {}
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IFREEWIFISCANSUBSCRIPTION_H__

