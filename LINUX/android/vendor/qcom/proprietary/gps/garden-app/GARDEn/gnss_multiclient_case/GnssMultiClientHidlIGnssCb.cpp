/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#define LOG_NDEBUG 0
#define LOG_TAG "GARDEN_GMCC_HidlIGnssCb"

#include "GnssMultiClientHidlIGnssCb.h"
#include "GnssMultiClientCaseUtils.h"

namespace garden {

GnssMultiClientHidlIGnssCb::GnssMultiClientHidlIGnssCb(
        GnssMultiClientHidlIGnss* hidlGnss)
{
    mHidlGnss = hidlGnss;
}

GnssMultiClientHidlIGnssCb::~GnssMultiClientHidlIGnssCb() {}

Return<void>
GnssMultiClientHidlIGnssCb::gnssMeasurementCb(
        const IGnssMeasurementCallback_V1_1::GnssData& )
{
    return Void();
}

Return<void> GnssMultiClientHidlIGnssCb::GnssMeasurementCb(
        const IGnssMeasurementCallback_V1_0::GnssData& )
{
    return Void();
}

} // namespace garden
