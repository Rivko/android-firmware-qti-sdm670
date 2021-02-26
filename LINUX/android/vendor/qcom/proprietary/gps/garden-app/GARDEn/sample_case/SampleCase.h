/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef SAMPLE_CASE_H
#define SAMPLE_CASE_H

#include "IGardenCase.h"

namespace garden {

class SampleCase : public IGardenCase{
public:
    SampleCase();
    virtual ~SampleCase();
    virtual GARDEN_RESULT preRun();
    virtual GARDEN_RESULT postRun();
    bool isLeapYear(int32_t year);
    GARDEN_RESULT failedCase();
};

} // namespace garden
#endif // SAMPLE_CASE_H
