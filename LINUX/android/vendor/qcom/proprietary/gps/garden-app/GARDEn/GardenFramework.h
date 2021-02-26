/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef GARDEN_FRAMEWORK_H
#define GARDEN_FRAMEWORK_H

#include <vector>
#include <IGardenCase.h>
#include <GardenUtil.h>

#define ULPLIB "libgardencaseulp.so"
#define ZPPLIB "libgardencasezpp.so"
#define TRACKINGLIB "libgardencasetracking.so"

#define GARDEN_GET_PHONE_CONTEXT_MSG()\
::garden::GardenFramework::getInstance()->getSetPhoneContextMsg()

#define GARDEN_ADD_PLUGIN(name, arg)\
::garden::GardenFramework::getInstance()->addPlugin(name, arg)

#define GARDEN_ADD(class_name, case_name, run_func)\
::garden::GardenFramework::getInstance()->addCase([=](){\
        auto icase = new class_name();\
        icase->setup(case_name, [=](::garden::IGardenCase* icase){\
            (void)(icase);\
            run_func\
            return ::garden::GARDEN_RESULT_PASSED;});\
        return icase;\
    })

#define GARDEN_RUN()\
::garden::GardenFramework::getInstance()->run()

#define GARDEN_DESTROY()\
::garden::GardenFramework::destroyInstance()


namespace garden {

using GardenCaseCreator = std::function<IGardenCase*()>;
using GardenPluginInterface = void(std::string args);
using GardenSetPhoneContextInterface = void*();

class GardenFramework {
public:
    static GardenFramework* getInstance();
    static void destroyInstance();
    void* getSetPhoneContextMsg();
    void addPlugin(std::string name, std::string args);
    void addCase(GardenCaseCreator creator);
    GARDEN_RESULT run();
private:
    GardenFramework(const GardenFramework&) = delete;
    GardenFramework& operator=(const GardenFramework&) = delete;
    GardenFramework() = default;
    virtual ~GardenFramework() {}
private:
    static GardenFramework* mInstance;
    static GardenSetPhoneContextInterface* mSetPhoneContextInterface;
    std::vector<GardenCaseCreator> mCreators;
};

} // namespace garden
#endif // GARDEN_FRAMEWORK_H
