/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IWifiScanInitiator

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_IWIFISCANINITIATOR_H__
#define __IZAT_MANAGER_IWIFISCANINITIATOR_H__

namespace izat_manager {

// Use this interface to initiate/trigger wifi scans
class IWifiScanInitiator {

public:
    /* Initiates/Triggers Wifi Scan
     *
     * measurementAgeFilterInSecs : All the Wifi-scan measuresments which are
     * older than current time - filter age will be filtered out from the
     * response. So 0 age filter could potentially return no measurements in
     * cache.
     *
     * fallbackToleranceSec : At the time of evaluating this request, all the
     * requested channels should have last measurement timestamp equal to or
     * greater than current time - Fallback tolerance. If not, then fallback
     * to tigger a fresh scan. A value of 0 will trigger a fresh scan.
     *
     * timeoutTimestamp : Timestamp at which this request should be droped if
     * not processed already. Should be 0 if this is to be ignored.
     */
    virtual void initiateWifiScan
    (
        unsigned long int measurementAgeFilterInSecs,
        unsigned long int fallbackToleranceSec,
        long long int timeoutTimestamp,
        bool mIsEmergencyRequest
    ) = 0;

    // dtor
    virtual ~IWifiScanInitiator () {}
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IWIFISCANINITIATOR_H__

