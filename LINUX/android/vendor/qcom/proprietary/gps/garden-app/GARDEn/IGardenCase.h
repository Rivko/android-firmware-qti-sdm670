/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef IGARDEN_CASE_H
#define IGARDEN_CASE_H

#include <functional>
#include <string>

namespace garden {

typedef enum
{
    GARDEN_RESULT_PASSED = 0,
    GARDEN_RESULT_FAILED,
    GARDEN_RESULT_EXCEPTION,
    GARDEN_RESULT_ABORT,
    GARDEN_RESULT_INVALID,
    GARDEN_RESULT_MAX,
} GARDEN_RESULT;

class IGardenCase {
    using GardenCaseFunc = std::function<GARDEN_RESULT(::garden::IGardenCase* icase)>;
public:
    IGardenCase() = default;
    virtual ~IGardenCase() = default;
    inline virtual GARDEN_RESULT preRun() { return GARDEN_RESULT_PASSED; }
    inline virtual GARDEN_RESULT postRun() { return GARDEN_RESULT_PASSED; }
    inline void setup(std::string name, GardenCaseFunc run) {
        mName = name;
        mRunFunc = run;
    }
    inline std::string getName() { return mName; }
private:
    IGardenCase(const IGardenCase&) = delete;
    IGardenCase& operator=(const IGardenCase&) = delete;
public:
    GardenCaseFunc mRunFunc;
private:
    std::string mName;
};

} // namespace garden
#endif // IGARDEN_CASE_H
