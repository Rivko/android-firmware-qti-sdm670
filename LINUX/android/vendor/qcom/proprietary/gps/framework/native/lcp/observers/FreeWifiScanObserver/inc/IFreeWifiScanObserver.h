/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IFreeWifiScanObserver

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZAT_MANAGER_IFREEWIFISCANOBSERVER_H__
#define __IZAT_MANAGER_IFREEWIFISCANOBSERVER_H__

namespace qc_loc_fw {
class LOWIResponse;
}

namespace izat_manager {

using namespace qc_loc_fw;

// Implement this interface to get notified of wifi scans.
// use this interface with IFreeWifiScanSubscription
class IFreeWifiScanObserver {

public:

    virtual void notify
    (
        const LOWIResponse * response
    ) = 0;

    virtual ~IFreeWifiScanObserver () {}
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IFREEWIFISCANOBSERVER_H__
