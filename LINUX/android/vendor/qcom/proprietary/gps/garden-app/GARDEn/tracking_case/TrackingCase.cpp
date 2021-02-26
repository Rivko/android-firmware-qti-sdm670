/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include <GardenFramework.h>
#include <GardenUtil.h>
#include <EventObserver.h>
#include "PowerEvtHandler.h"
#include "TrackingCase.h"
#include "GnssCb.h"

namespace garden {

EventObserver* TrackingCase::mEventObserver = nullptr;
PowerEvtHandler* TrackingCase::mPowerEventObserver = nullptr;
garden_app_session_tracker* TrackingCase::mGardenSessionControl = nullptr;

static void rxSystemEvent(unsigned int systemEvent) {
    gardenPrint("%s: SYSTEM EVENT = %d\n", __func__, systemEvent);
    if (TrackingCase::mGardenSessionControl != nullptr) {
        TrackingCase::mGardenSessionControl->process_system_event(systemEvent);
    }
}

TrackingCase::TrackingCase() {
}

TrackingCase::~TrackingCase() {
    if (mEventObserver != nullptr) {
        delete mEventObserver;
        mEventObserver = nullptr;
    }
    if (mPowerEventObserver!= nullptr) {
        delete mPowerEventObserver;
        mPowerEventObserver = nullptr;
    }
    if (mGardenSessionControl != nullptr) {
        delete mGardenSessionControl;
        mGardenSessionControl = nullptr;
    }
}

GARDEN_RESULT TrackingCase::preRun() {
    IGnssAPI* api = this->getGnssAPI();
    GARDEN_ASSERT_NEQ(api, nullptr, "GnssAPI is nullptr.");
    if(!mCb) {
        mCb = new GnssCb(this);
        mGnssAPI->setGnssCbs(mCb);
    }
    if (mGardenSessionControl == nullptr) {
        mGardenSessionControl = new garden_app_session_tracker(api);
        GARDEN_ASSERT_NEQ(mGardenSessionControl, nullptr,
                "Failed to Create garden_tracking_session_control.");
    }

    if (mPowerEventObserver == nullptr) {
        mPowerEventObserver = new PowerEvtHandler();
        GARDEN_ASSERT_NEQ(mPowerEventObserver, nullptr,
                "Failed to Create PowerEvtHandler.");
    }

    if (mEventObserver == nullptr) {
        mEventObserver = new EventObserver(rxSystemEvent);
        GARDEN_ASSERT_NEQ(mEventObserver, nullptr,
                "Failed to Create EventObserver.");
    }
#if USE_GLIB
    sendMsg((LocMsg*)GARDEN_GET_PHONE_CONTEXT_MSG());
#endif

    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT TrackingCase::postRun() {
    GARDEN_RESULT ret = GnssCase::postRun();
    return ret;
}

GARDEN_RESULT TrackingCase::tracking() {
    std::lock_guard<mutex> guard(mLock);
    // no one will clear this flag, so postRun will never return.
    setFlag(GNSS_CASE_FOREVER);
    getGnssAPI()->gnssStart();
    return GARDEN_RESULT_PASSED;
}

extern "C" void GARDEN_Plugin(std::string args) {
    gardenPrint("Adding TrackingCase with args: %s", args.c_str());
    GARDEN_ADD(TrackingCase, "TrackingCase",
        {
            TrackingCase* trackingCase = (TrackingCase*)icase;
            return trackingCase->tracking();
        });
}

} // namespace garden
