/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "LocSvc_HIDL_DebugReportService"
#define LOG_NDEBUG 0

#include "LocHidlDebugReportService.h"
#include <log_util.h>

namespace vendor {
namespace qti {
namespace gnss {
namespace V1_0 {
namespace implementation {

using namespace loc_core;

// Methods from ::vendor::qti::gnss::V1_0::ILocHidlDebugReportService follow.
Return<bool> LocHidlDebugReportService::init() {

    if (mSystemStatus == NULL) {
        mSystemStatus = SystemStatus::getInstance(NULL);
        if (mSystemStatus == NULL) {
            ALOGE("Failed to get SystemStatus");
            return false;
        }
        ALOGE("SystemStatus instance: %p",mSystemStatus);
    } else {
        LOC_LOGE("system status already available !");
    }

    return true;
}

Return<bool> LocHidlDebugReportService::deinit() {

    /* NOOP */
    return true;
}

Return<void> LocHidlDebugReportService::getReport(
        int32_t maxReports, getReport_cb _hidl_cb) {

    LocHidlSystemStatusReports hidlReports = {};
    hidlReports.mSuccess = false;

    if (maxReports <= 0) {
        LOC_LOGE("Invalid maxReports %d", maxReports);
        _hidl_cb(hidlReports);
        return Void();
    }
    if (mSystemStatus == NULL) {
        LOC_LOGE("mSystemStatus NULL");
        _hidl_cb(hidlReports);
        return Void();
    }

    // Populate reports
    SystemStatusReports systemReports;
    mSystemStatus->getReport(systemReports, maxReports == 1);

    populateLocationReport(hidlReports, systemReports, maxReports);
    populateTimeAndClockReport(hidlReports, systemReports, maxReports);
    populateXoStateReport(hidlReports, systemReports, maxReports);
    populateRfParamsReport(hidlReports, systemReports, maxReports);
    populateErrRecoveryReport(hidlReports, systemReports, maxReports);
    populateInjectedPositionReport(hidlReports, systemReports, maxReports);
    populateBestPositionReport(hidlReports, systemReports, maxReports);
    populateXtraReport(hidlReports, systemReports, maxReports);
    populateEphemerisReport(hidlReports, systemReports, maxReports);
    populateSvHealthReport(hidlReports, systemReports, maxReports);
    populatePdrReport(hidlReports, systemReports, maxReports);
    populateNavDataReport(hidlReports, systemReports, maxReports);
    populatePositionFailureReport(hidlReports, systemReports, maxReports);

    hidlReports.mSuccess = true;
    _hidl_cb(hidlReports);
    return Void();
}

void LocHidlDebugReportService::populateLocationReport(
        LocHidlSystemStatusReports& /*hidlReports*/,
        SystemStatusReports& /*systemReports*/, int32_t /*maxReports*/) {

    /* Not yet supported */
}

void LocHidlDebugReportService::populateTimeAndClockReport(
        LocHidlSystemStatusReports& hidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = systemReports.mTimeAndClock.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        LOC_LOGE("Invalid count: %d", count);
        return;
    }
    hidlReports.mTimeAndClockVec.resize(count);

    for(auto p = make_pair(0, systemReports.mTimeAndClock.begin());
        (p.first < maxReports) && (p.second != systemReports.mTimeAndClock.end());
        p.first++, ++p.second) {

        LocHidlSystemStatusTimeAndClock& status = hidlReports.mTimeAndClockVec[p.first];

        status.mUtcTime.tv_sec = (*p.second).mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = (*p.second).mUtcTime.tv_nsec;

        status.mUtcReported.tv_sec = (*p.second).mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec = (*p.second).mUtcReported.tv_nsec;

        status.mGpsWeek = (*p.second).mGpsWeek;
        status.mGpsTowMs = (*p.second).mGpsTowMs;
        status.mTimeValid = (*p.second).mTimeValid;
        status.mTimeSource = (*p.second).mTimeSource;
        status.mTimeUnc = (*p.second).mTimeUnc;
        status.mClockFreqBias =  (*p.second).mClockFreqBias;
        status.mClockFreqBiasUnc = (*p.second).mClockFreqBiasUnc;
    }
}

void LocHidlDebugReportService::populateXoStateReport(
        LocHidlSystemStatusReports& hidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = systemReports.mXoState.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        LOC_LOGE("Invalid count: %d", count);
        return;
    }
    hidlReports.mXoStateVec.resize(count);

    for (auto p = make_pair(0, systemReports.mXoState.begin());
         (p.first < maxReports) && (p.second != systemReports.mXoState.end());
         p.first++, ++p.second) {

        LocHidlSystemStatusXoState& status = hidlReports.mXoStateVec[p.first];

        status.mUtcTime.tv_sec = (*p.second).mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = (*p.second).mUtcTime.tv_nsec;

        status.mUtcReported.tv_sec = (*p.second).mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec = (*p.second).mUtcReported.tv_nsec;

        status.mXoState = (*p.second).mXoState;
    }
}

void LocHidlDebugReportService::populateRfParamsReport(
        LocHidlSystemStatusReports& hidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = systemReports.mRfAndParams.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        LOC_LOGE("Invalid count: %d", count);
        return;
    }
    hidlReports.mRfAndParamsVec.resize(count);

    for (auto p = make_pair(0, systemReports.mRfAndParams.begin());
         (p.first < maxReports) && (p.second != systemReports.mRfAndParams.end());
         p.first++, ++p.second) {

        LocHidlSystemStatusRfAndParams& status = hidlReports.mRfAndParamsVec[p.first];

        status.mUtcTime.tv_sec = (*p.second).mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = (*p.second).mUtcTime.tv_nsec;

        status.mUtcReported.tv_sec = (*p.second).mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec = (*p.second).mUtcReported.tv_nsec;

        status.mPgaGain = (*p.second).mPgaGain;
        status.mGpsBpAmpI = (*p.second).mGpsBpAmpI;
        status.mGpsBpAmpQ = (*p.second).mGpsBpAmpQ;
        status.mAdcI = (*p.second).mAdcI;
        status.mAdcQ = (*p.second).mAdcQ;
        status.mJammerGps =(*p.second).mJammerGps;
        status.mJammerGlo = (*p.second).mJammerGlo;
        status.mJammerBds = (*p.second).mJammerBds;
        status.mJammerGal = (*p.second).mJammerGal;
    }
}

void LocHidlDebugReportService::populateErrRecoveryReport(
        LocHidlSystemStatusReports& hidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = systemReports.mErrRecovery.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        LOC_LOGE("Invalid count: %d", count);
        return;
    }
    hidlReports.mErrRecoveryVec.resize(count);

    for (auto p = make_pair(0, systemReports.mErrRecovery.begin());
         (p.first < maxReports) && (p.second != systemReports.mErrRecovery.end());
         p.first++, ++p.second) {

        LocHidlSystemStatusErrRecovery& status = hidlReports.mErrRecoveryVec[p.first];

        status.mUtcTime.tv_sec = (*p.second).mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = (*p.second).mUtcTime.tv_nsec;

        status.mUtcReported.tv_sec = (*p.second).mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec = (*p.second).mUtcReported.tv_nsec;
    }
}

void LocHidlDebugReportService::populateInjectedPositionReport(
        LocHidlSystemStatusReports& hidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = systemReports.mInjectedPosition.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        LOC_LOGE("Invalid count: %d", count);
        return;
    }
    hidlReports.mInjectedPositionVec.resize(count);

    for (auto p = make_pair(0, systemReports.mInjectedPosition.begin());
         (p.first < maxReports) && (p.second != systemReports.mInjectedPosition.end());
         p.first++, ++p.second) {

        LocHidlSystemStatusInjectedPosition& status = hidlReports.mInjectedPositionVec[p.first];

        status.mUtcTime.tv_sec = (*p.second).mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = (*p.second).mUtcTime.tv_nsec;

        status.mUtcReported.tv_sec = (*p.second).mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec = (*p.second).mUtcReported.tv_nsec;

        status.mEpiValidity = (*p.second).mEpiValidity;
        status.mEpiLat = (*p.second).mEpiLat;
        status.mEpiLon = (*p.second).mEpiLon;
        status.mEpiHepe = (*p.second).mEpiHepe;
        status.mEpiAlt = (*p.second).mEpiAlt;
        status.mEpiAltUnc = (*p.second).mEpiAltUnc;
        status.mEpiSrc = (*p.second).mEpiSrc;
    }
}

void LocHidlDebugReportService::populateBestPositionReport(
        LocHidlSystemStatusReports& hidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = systemReports.mBestPosition.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        LOC_LOGE("Invalid count: %d", count);
        return;
    }
    hidlReports.mBestPositionVec.resize(count);

    for (auto p = make_pair(0, systemReports.mBestPosition.begin());
         (p.first < maxReports) && (p.second != systemReports.mBestPosition.end());
         p.first++, ++p.second) {

        LocHidlSystemStatusBestPosition& status = hidlReports.mBestPositionVec[p.first];

        status.mUtcTime.tv_sec = (*p.second).mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = (*p.second).mUtcTime.tv_nsec;

        status.mUtcReported.tv_sec = (*p.second).mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec = (*p.second).mUtcReported.tv_nsec;

        status.mBestLat = (*p.second).mBestLat;
        status.mBestLon = (*p.second).mBestLon;
        status.mBestHepe = (*p.second).mBestHepe;
        status.mBestAlt = (*p.second).mBestAlt;
        status.mBestAltUnc = (*p.second).mBestAltUnc;
    }
}

void LocHidlDebugReportService::populateXtraReport(
        LocHidlSystemStatusReports& hidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = systemReports.mXtra.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        LOC_LOGE("Invalid count: %d", count);
        return;
    }
    hidlReports.mXtraVec.resize(count);

    for (auto p = make_pair(0, systemReports.mXtra.begin());
         (p.first < maxReports) && (p.second != systemReports.mXtra.end());
         p.first++, ++p.second) {

        LocHidlSystemStatusXtra& status = hidlReports.mXtraVec[p.first];

        status.mUtcTime.tv_sec = (*p.second).mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = (*p.second).mUtcTime.tv_nsec;

        status.mUtcReported.tv_sec = (*p.second).mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec = (*p.second).mUtcReported.tv_nsec;

        status.mXtraValidMask = (*p.second).mXtraValidMask;
        status.mGpsXtraAge = (*p.second).mGpsXtraAge;
        status.mGloXtraAge = (*p.second).mGloXtraAge;
        status.mBdsXtraAge = (*p.second).mBdsXtraAge;
        status.mGalXtraAge = (*p.second).mGalXtraAge;
        status.mQzssXtraAge = (*p.second).mQzssXtraAge;
        status.mGpsXtraValid = (*p.second).mGpsXtraValid;
        status.mGloXtraValid = (*p.second).mGloXtraValid;
        status.mBdsXtraValid = (*p.second).mBdsXtraValid;
        status.mGalXtraValid =(*p.second).mGalXtraValid;
        status.mQzssXtraValid = (*p.second).mQzssXtraValid;
    }
}

void LocHidlDebugReportService::populateEphemerisReport(
        LocHidlSystemStatusReports& hidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = systemReports.mEphemeris.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        LOC_LOGE("Invalid count: %d", count);
        return;
    }
    hidlReports.mEphemerisVec.resize(count);

    for (auto p = make_pair(0, systemReports.mEphemeris.begin());
         (p.first < maxReports) && (p.second != systemReports.mEphemeris.end());
         p.first++, ++p.second) {

        LocHidlSystemStatusEphemeris& status = hidlReports.mEphemerisVec[p.first];

        status.mUtcTime.tv_sec = (*p.second).mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = (*p.second).mUtcTime.tv_nsec;

        status.mUtcReported.tv_sec = (*p.second).mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec = (*p.second).mUtcReported.tv_nsec;

        status.mGpsEpheValid = (*p.second).mGpsEpheValid;
        status.mGloEpheValid = (*p.second).mGloEpheValid;
        status.mBdsEpheValid = (*p.second).mBdsEpheValid;
        status.mGalEpheValid = (*p.second).mGalEpheValid;
        status.mQzssEpheValid = (*p.second).mQzssEpheValid;
    }
}

void LocHidlDebugReportService::populateSvHealthReport(
        LocHidlSystemStatusReports& hidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = systemReports.mSvHealth.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        LOC_LOGE("Invalid count: %d", count);
        return;
    }
    hidlReports.mSvHealthVec.resize(count);

    for (auto p = make_pair(0, systemReports.mSvHealth.begin());
         (p.first < maxReports) && (p.second != systemReports.mSvHealth.end());
         p.first++, ++p.second) {

        LocHidlSystemStatusSvHealth& status = hidlReports.mSvHealthVec[p.first];

        status.mUtcTime.tv_sec = (*p.second).mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = (*p.second).mUtcTime.tv_nsec;

        status.mUtcReported.tv_sec = (*p.second).mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec = (*p.second).mUtcReported.tv_nsec;

        status.mGpsGoodMask = (*p.second).mGpsGoodMask;
        status.mGloGoodMask = (*p.second).mGloGoodMask;
        status.mBdsGoodMask = (*p.second).mBdsGoodMask;
        status.mGalGoodMask = (*p.second).mGalGoodMask;
        status.mQzssGoodMask = (*p.second).mQzssGoodMask;

        status.mGpsBadMask = (*p.second).mGpsBadMask;
        status.mGloBadMask = (*p.second).mGloBadMask;
        status.mBdsBadMask = (*p.second).mBdsBadMask;
        status.mGalBadMask = (*p.second).mGalBadMask;
        status.mQzssBadMask = (*p.second).mQzssBadMask;

        status.mGpsUnknownMask = (*p.second).mGpsUnknownMask;
        status.mGloUnknownMask = (*p.second).mGloUnknownMask;
        status.mBdsUnknownMask = (*p.second).mBdsUnknownMask;
        status.mGalUnknownMask = (*p.second).mGalUnknownMask;
        status.mQzssUnknownMask = (*p.second).mQzssUnknownMask;
    }
}

void LocHidlDebugReportService::populatePdrReport(
        LocHidlSystemStatusReports& hidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = systemReports.mPdr.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        LOC_LOGE("Invalid count: %d", count);
        return;
    }
    hidlReports.mPdrVec.resize(count);

    for (auto p = make_pair(0, systemReports.mPdr.begin());
         (p.first < maxReports) && (p.second != systemReports.mPdr.end());
         p.first++, ++p.second) {

        LocHidlSystemStatusPdr& status = hidlReports.mPdrVec[p.first];

        status.mUtcTime.tv_sec = (*p.second).mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = (*p.second).mUtcTime.tv_nsec;

        status.mUtcReported.tv_sec = (*p.second).mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec = (*p.second).mUtcReported.tv_nsec;

        status.mFixInfoMask = (*p.second).mFixInfoMask;
    }
}

void LocHidlDebugReportService::populateNavDataReport(
        LocHidlSystemStatusReports& /*hidlReports*/,
        SystemStatusReports& /*systemReports*/, int32_t /*maxReports*/) {

    /* Not yet supported */
}

void LocHidlDebugReportService::populatePositionFailureReport(
        LocHidlSystemStatusReports& hidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = systemReports.mPositionFailure.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        LOC_LOGE("Invalid count: %d", count);
        return;
    }
    hidlReports.mPositionFailureVec.resize(count);

    for (auto p = make_pair(0, systemReports.mPositionFailure.begin());
         (p.first < maxReports) && (p.second != systemReports.mPositionFailure.end());
         p.first++, ++p.second) {

        LocHidlSystemStatusPositionFailure& status = hidlReports.mPositionFailureVec[p.first];

        status.mUtcTime.tv_sec = (*p.second).mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = (*p.second).mUtcTime.tv_nsec;

        status.mUtcReported.tv_sec = (*p.second).mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec = (*p.second).mUtcReported.tv_nsec;

        status.mFixInfoMask = (*p.second).mFixInfoMask;
        status.mHepeLimit = (*p.second).mHepeLimit;
    }
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
