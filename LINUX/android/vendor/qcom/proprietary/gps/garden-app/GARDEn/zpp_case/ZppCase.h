/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef ZPP_CASE_H
#define ZPP_CASE_H

#include <UlpCase.h>

namespace garden {

class ZppCase : public UlpCase {
public:
    ZppCase();
    virtual ~ZppCase();
    virtual GARDEN_RESULT preRun();
    virtual GARDEN_RESULT postRun();

    // Cases
    GARDEN_RESULT zppCaseScreenOn();
    GARDEN_RESULT zppCaseTimeZoneChange();
    GARDEN_RESULT zppCasePowerDisconnect();
    GARDEN_RESULT zppCaseThrotting();

    static void addCase(int idx);
};

} // namespace garden
#endif // ZPP_CASE_H
