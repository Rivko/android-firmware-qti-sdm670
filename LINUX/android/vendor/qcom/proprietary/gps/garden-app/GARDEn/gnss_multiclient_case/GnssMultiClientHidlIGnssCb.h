/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#ifndef GNSS_MULTI_CLIENT_HIDL_IGNSS_CB_H
#define GNSS_MULTI_CLIENT_HIDL_IGNSS_CB_H
#include "GnssMultiClientCase.h"
#include "GnssCbBase.h"

#include <android/hardware/gnss/1.0/IGnss.h>
#include <android/hardware/gnss/1.1/IGnss.h>
#include <android/hardware/gnss/1.0/IGnssMeasurement.h>
#include <android/hardware/gnss/1.1/IGnssMeasurement.h>

using android::hardware::Return;
using android::hardware::Void;

using IGnss_V1_0 = android::hardware::gnss::V1_0::IGnss;
using IGnss_V1_1 = android::hardware::gnss::V1_1::IGnss;
using IGnssMeasurement_V1_0 = android::hardware::gnss::V1_0::IGnssMeasurement;
using IGnssMeasurement_V1_1 = android::hardware::gnss::V1_1::IGnssMeasurement;
using IGnssMeasurementCallback_V1_0 =
        android::hardware::gnss::V1_0::IGnssMeasurementCallback;
using IGnssMeasurementCallback_V1_1 =
        android::hardware::gnss::V1_1::IGnssMeasurementCallback;

namespace garden {

class GnssMultiClientHidlIGnss;

class GnssMultiClientHidlIGnssCb : public IGnssMeasurementCallback_V1_1
{
public:
    GnssMultiClientHidlIGnssCb(GnssMultiClientHidlIGnss* hidlIGnss);
    ~GnssMultiClientHidlIGnssCb();

    Return<void> gnssMeasurementCb(
            const IGnssMeasurementCallback_V1_1::GnssData& data) override;
    Return<void> GnssMeasurementCb(
            const IGnssMeasurementCallback_V1_0::GnssData& data) override;

private:
    GnssMultiClientHidlIGnss* mHidlGnss = nullptr;
};

} //namespace garden

#endif //GNSS_MULTI_CLIENT_HIDL_IGNSS_CB_H
