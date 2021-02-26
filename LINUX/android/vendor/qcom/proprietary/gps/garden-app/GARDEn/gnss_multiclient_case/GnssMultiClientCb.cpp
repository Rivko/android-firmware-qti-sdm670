/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#define LOG_NDEBUG 0
#define LOG_TAG "GARDEN_GMCC_Cb"

#include "GardenUtil.h"
#include "GnssMultiClientCb.h"
#include "GnssMultiClientCaseUtils.h"

namespace garden {
GnssMultiClientCb::GnssMultiClientCb(GnssMultiClientCase* gCase) {
    mGnssCase = gCase;
}

GnssMultiClientCb::~GnssMultiClientCb() {
    if(mGnssCase) {
        mGnssCase = nullptr;
    }
}

void GnssMultiClientCb::gnssLocationCb(Location& location) {

    CONDPRINTLN("## %s]: () ##, LAT: %f, LON: %f, ALT: %f ACC: %f, TIME: %llu",
             __FUNCTION__, location.latitude, location.longitude, location.altitude,
            location.accuracy, (long long)location.timestamp);

    if(mGnssCase && mGnssCase->mStartTime != 0 && mGnssCase->mTTFF == 0) {
        uint64_t now = getUpTimeSec();
        if (now > mGnssCase->mStartTime) {
            mGnssCase->mTTFF = now - mGnssCase->mStartTime;
            gardenPrint("TTFF: %lu", mGnssCase->mTTFF);
        }
    }
    if (mGnssCase && mGnssCase->hasFlag(GNSS_CASE_SESSION) &&
            LOC_GPS_POSITION_RECURRENCE_SINGLE == mGnssCase->mRecurrence) {
        lock_guard guard(mGnssCase->mLock);
        mGnssCase->unsetFlag(GNSS_CASE_SESSION);
    }
}

void GnssMultiClientCb::gnssStatusCb(uint32_t status) {
    if(LOC_GPS_STATUS_ENGINE_OFF == status) {
        lock_guard guard(mGnssCase->mLock);
        mGnssCase->unsetFlag(GNSS_CASE_ENGINEOFF);
    }
}

void GnssMultiClientCb::gnssSetCapabilitiesCb(uint32_t /*capabilities*/) {
    // ignore
}

void GnssMultiClientCb::gnssSvStatusCb(GnssSvNotification& /*svNotify*/) {

    // print sv info if printing enabled
}

} // namespace garden
