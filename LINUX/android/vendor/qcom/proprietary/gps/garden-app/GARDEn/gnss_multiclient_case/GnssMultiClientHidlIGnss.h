/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#ifndef GNSS_MULTI_CLIENT_HIDL_IGNSS_H
#define GNSS_MULTI_CLIENT_HIDL_IGNSS_H

#include "IGnssAPI.h"
#include <LocationAPI.h>
#include <GnssMultiClientHidlIGnssCb.h>

#include <android/hardware/gnss/1.1/IGnss.h>

#include "IGardenCase.h"

using android::OK;
using android::sp;
using android::wp;
using android::status_t;

using android::hardware::Return;
using android::hardware::Void;
using android::hardware::hidl_vec;
using android::hardware::hidl_death_recipient;
using android::hidl::base::V1_0::IBase;

using android::hardware::gnss::V1_0::GnssLocation;

using android::hardware::gnss::V1_1::IGnss;
using android::hardware::gnss::V1_1::IGnssCallback;
using android::hardware::gnss::V1_1::IGnssConfiguration;
using android::hardware::gnss::V1_1::IGnssMeasurement;

namespace garden {

/* Data structures for HIDL service interaction */
struct GnssDeathRecipient : virtual public hidl_death_recipient {
    virtual void serviceDied(uint64_t /*cookie*/, const wp<IBase>& /*who*/) override;
    virtual ~GnssDeathRecipient() {};
};

/* GNSS MULTI CLIENT HIDL CLIENT FOR IGNSS INTERFACE */
class GnssMultiClientHidlIGnss {

    friend class GnssMultiClientHidlIGnssCb;

private:
    static GnssMultiClientHidlIGnss* sInstance;
    GnssMultiClientHidlIGnss() {
        mHidlIGnssCb = new GnssMultiClientHidlIGnssCb(this);
    };

public:
    static GnssMultiClientHidlIGnss& get() {
        if (nullptr == sInstance) {
            sInstance = new GnssMultiClientHidlIGnss();
        }
        return *sInstance;
    }

public:
    GARDEN_RESULT menuTest();

private:
    /* Individual test cases triggered from menu test */
    GARDEN_RESULT createHidlClient();
    GARDEN_RESULT IGnss_1_0_start();
    GARDEN_RESULT IGnss_1_0_stop();
    GARDEN_RESULT IGnss_1_1_setPositionMode_1_1();
    GARDEN_RESULT IGnss_1_1_injectBestLocation();
    GARDEN_RESULT IGnssMeasurement_1_1_setCallback_1_1();
    GARDEN_RESULT IGnssConfiguration_1_1_setBlacklist();

public:
    sp<IGnss> mGnssIface = nullptr;
    sp<IGnssConfiguration> mGnssConfigurationIface = nullptr;
    sp<IGnssMeasurement> mGnssMeasurementIface = nullptr;
    sp<GnssDeathRecipient> mGnssDeathRecipient = nullptr;

    sp<GnssMultiClientHidlIGnssCb> mHidlIGnssCb = nullptr;
};

} //namespace garden


#endif //GNSS_MULTI_CLIENT_HIDL_IGNSS_H
