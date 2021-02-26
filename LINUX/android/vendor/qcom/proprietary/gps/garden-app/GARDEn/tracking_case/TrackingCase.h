/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef TRACKING_CASE_H
#define TRACKING_CASE_H

#include <GnssCase.h>
#include "garden_app_session_tracker.h"

namespace garden {

class TrackingCase : public GnssCase {
public:
    TrackingCase();
    virtual ~TrackingCase();
    virtual GARDEN_RESULT preRun();
    virtual GARDEN_RESULT postRun();

    // Cases
    GARDEN_RESULT tracking();

    static EventObserver* mEventObserver;
    static PowerEvtHandler* mPowerEventObserver;
    static garden_app_session_tracker* mGardenSessionControl;
};

} // namespace garden
#endif // TRACKING_CASE_H
