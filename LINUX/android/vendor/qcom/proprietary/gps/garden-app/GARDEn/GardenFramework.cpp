/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include <dlfcn.h>
#include <GardenFramework.h>
namespace garden {

GardenFramework* GardenFramework::mInstance = nullptr;
GardenSetPhoneContextInterface* GardenFramework::mSetPhoneContextInterface = nullptr;

template<typename T>
static T* getPluginInterface(const char* library, const char* name) {
    gardenPrint("loading %s::%s ...", library, name);
    if (nullptr == library || nullptr == name) {
        return nullptr;
    }
    T* getter = nullptr;
    const char *error = nullptr;
    dlerror();
    void *handle = dlopen(library, RTLD_NOW);
    if (nullptr == handle)  {
        gardenPrint("dlopen for %s failed", library);
    } else if (nullptr != (error = dlerror())) {
        gardenPrint("dlopen for %s failed, error = %s", library, error);
    } else {
        getter = (T*)dlsym(handle, name);
        if ((error = dlerror()) != nullptr)  {
            gardenPrint("dlsym for %s::%s failed, error = %s", library, name, error);
            getter = nullptr;
        }
    }
    return getter;
}

GardenFramework* GardenFramework::getInstance() {
    if (mInstance == nullptr) {
        mInstance = new GardenFramework();
    }
    return mInstance;
}

void GardenFramework::destroyInstance() {
    if (mInstance != nullptr) {
        delete mInstance;
        mInstance = nullptr;
    }
}

void* GardenFramework::getSetPhoneContextMsg() {
    if (mSetPhoneContextInterface == nullptr) {
        mSetPhoneContextInterface =
            getPluginInterface<GardenSetPhoneContextInterface>(ULPLIB, "getSetPhoneContextMsg");
    }
    if (mSetPhoneContextInterface == nullptr) {
        return nullptr;
    }
    return mSetPhoneContextInterface();
}

void GardenFramework::addPlugin(std::string name, std::string args) {
    GardenPluginInterface* plugin =
        getPluginInterface<GardenPluginInterface>(name.c_str(), "GARDEN_Plugin");
    if (plugin != nullptr) {
        plugin(args);
    }
}

void GardenFramework::addCase(GardenCaseCreator creator) {
    mCreators.push_back(creator);
}

GARDEN_RESULT GardenFramework::run() {
    GARDEN_RESULT ret = GARDEN_RESULT_PASSED;
    static std::string resultStr[GARDEN_RESULT_MAX] = {
        "PASSED",
        "FAILED",
        "EXCEPTION",
        "ABORT",
        "INVALID",
    };
    int total = mCreators.size();
    int passed = 0;
    int failed = 0;
    int exception = 0;
    GARDEN_RESULT result = GARDEN_RESULT_INVALID;

    gardenPrint("=== Warp Speed, Engage!\n");
    for (auto creator : mCreators) {
        uint64_t start = getRealTimeNsec() / 1000000LL;
        gardenPrint("=== Begin time: %lld ms", start);

        result = GARDEN_RESULT_FAILED;
        try {
            IGardenCase* icase = creator();
            if (icase != nullptr) {
                std::string name = icase->getName();
                gardenPrint("=== Case name: %s", name.empty() ? "" : name.c_str());

                result = icase->preRun();
                if (result == GARDEN_RESULT_PASSED && icase->mRunFunc != nullptr) {
                    result = icase->mRunFunc(icase);
                }
                if (result <= GARDEN_RESULT_FAILED) {
                    GARDEN_RESULT post = icase->postRun();
                    if (result < post) {
                        result = post;
                    }
                }
                delete icase;
            }
        } catch (...) {
            gardenPrint("=== EXCEPTION!");
            result = GARDEN_RESULT_EXCEPTION;
        }

        std::string str;
        if (result < GARDEN_RESULT_PASSED || result > GARDEN_RESULT_INVALID) {
            str = resultStr[GARDEN_RESULT_INVALID];
        } else {
            str = resultStr[result];
        }
        gardenPrint("=== Result: %s", str.c_str());

        uint64_t end = getRealTimeNsec() / 1000000LL;
        gardenPrint("=== Time taked: %lld ms", end - start);
        gardenPrint("=== Finish time: %lld ms\n", end);

        if (ret < result) ret = result;
        switch (result) {
            case GARDEN_RESULT_PASSED:
                passed++;
                break;
            case GARDEN_RESULT_FAILED:
                failed++;
                break;
            case GARDEN_RESULT_EXCEPTION:
                exception++;
                break;
            default:
                break;
        }
        if (result == GARDEN_RESULT_ABORT) {
            gardenPrint("=== Abort!");
            break;
        }
    }

    gardenPrint("=== Report:");
    gardenPrint("=== Total: %d", total);
    gardenPrint("=== Passed: %d", passed);
    gardenPrint("=== Failed: %d", failed);
    gardenPrint("=== Exception: %d", exception);
    gardenPrint("=== Unchecked: %d", total - passed - failed - exception);

    return ret;
}
} // namespace garden
