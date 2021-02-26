/******************************************************************************
 *   @file    LearningModule.cpp
 *   @brief   Implementation of Android Framework Learning Module
 *
 *   DESCRIPTION
 *      Learning Module is a framework that enables "Feature"s to be developed
 *    to "learn" at runtime and apply actions based on such learning.
 *    Each "Feature" contains a triplet - "Meters, Algorithm, Action". Feature
 *    takes "Input" from its Meters based on triggers from the system and
 *    stores it in an Input Database. Algorithm takes the stored Input from
 *    Database, "learns" from input and writes output for the Action to an
 *    Output Database. Action uses the Output Database to apply actions.
 *      Learning Module class provides the core functionality of the framework.
 *    LM creates a thread (lmCoreThread) on initialization and all further
 *    logic is dispatched to this dedicated thread to relieve the caller
 *    (perf-hal). The thread starts by initializing the framework and features.
 *    Once initialization is done, LM waits for triggers from perf-hal.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/

#include <dirent.h>
#include <dlfcn.h>
#include <fstream>
#include <errno.h>
#include <cutils/properties.h>
#define ATRACE_TAG ATRACE_TAG_ALWAYS
#include <cutils/trace.h>
#include "internal/LearningModule.h"
#include "internal/FeatureXML.h"
#include "internal/BatteryInfo.h"
#include "Registry.h"
#include "lmDB.h"

#define LM_THREAD_NAME "lmCoreThread"

/* MAX defines are to be used for rate control */
#define MAX_CONCURR_METAMETERS 1
#define MAX_CONCURR_ALGOS 1
#define IDLE_BATTERY_THRESHOLD 50
#define TARGET_VENDOR_LM_DIR "/vendor/etc/lm/"
/* Use TARGET_DATA_LM_DIR defined in LMDB */
#define TARGET_DATA_LM_DIR LMDB_ROOT_PATH
#define LIBMETERS_NAME "libmeters.so"
#define WAIT_RETRIES_FOR_DATA_DIR 20
#define TRIGGERQ_FLUSH_THRESHOLD 10
#define boot_cmplt_prop "sys.post_boot.parsed"

using namespace std;

/* Instantiate featureList of FeatureRegistry */
using FeatureRegistryBase = RegistryBase<Feature, FeatureInfo>;
template<> FeatureRegistryBase::NameClassMap* FeatureRegistryBase::classList = nullptr;
template<> mutex FeatureRegistryBase::mMutex {};

/* Instantiate classList of MeterRegistry */
using MeterRegistryBase = RegistryBase<Meter, MeterInfo>;
template<> MeterRegistryBase::NameClassMap* MeterRegistryBase::classList = nullptr;
template<> mutex MeterRegistryBase::mMutex {};

LearningModule::LearningModule() : mTriggerQueue("LMTriggerQ", TRIGGERQ_FLUSH_THRESHOLD) {
    /*
     * All LM inits are done from LM Thread to relieve caller (Perf HAL) for
     * further initializations.
     */

    /* create and init thread infrastructure */
    lmThreadReady = false;
    DEBUGV(LOG_TAG_LM,"LM Constructor");
    idleAlgoThrBusy = false;
    algoThreadInterrupt = false;
    mEnableLMtracing = (property_get_bool("debug.trace.perf", 0) == 1);
    lmThread = thread(&LearningModule::lmMain, this);
}

LearningModule::~LearningModule() {
    DEBUGV(LOG_TAG_LM,"LM destructor, deleting objects");

    /* Exit the LM Core Thread */
    if(lmThreadReady) {
        /* Signal the LM Core Thread if it's running */
        TriggerInfo exitThreadTrigger;
        exitThreadTrigger.exitLMThread = true;
        mTriggerQueue.push(exitThreadTrigger);
        /* The thread would exit only after all triggers are processed. */
    }
    if(lmThread.joinable()) {
        lmThread.join();
    }

    /* Clear the featureList */
    for (Feature* currFeature : mFeatureList) {
        delete currFeature;
        currFeature = nullptr;
    }
    mFeatureList.clear();

    /* De-init the Database */
    LMDBGlobal::LMDBGlobalDeConfig();
    /* Close dl handles */
    for(void* dlHandle : dlLibHandles) {
        if(dlHandle) {
            int ret_code = dlclose(dlHandle);
            if(ret_code!= 0) {
                DEBUGE(LOG_TAG_LM,"Unable to close dlopened library");
            }
        }
    }
    dlLibHandles.clear();
    DEBUGV(LOG_TAG_LM,"~LearningModule done");
}

void LearningModule::trigger(AsyncTriggerPayload payload) {
    /* Don't start delivering triggers until LM Thread is ready */
    if(lmThreadReady) {
        /* Push the trigger to the TriggerQueue */
        TriggerInfo trigger;
        trigger.payload = payload;
        trigger.exitLMThread = false;
        mTriggerQueue.push(trigger);
    }
}

void LearningModule::detectAndRunIdle(bool debugIdleTrigger) {
    idleAlgoThrBusy = true;
    /* If the trigger is not from debug trigger, run idle detection logic */
    DEBUGV(LOG_TAG_LM, "Checking for debug Idle Trigger = %s \n", debugIdleTrigger ? "true" : "false");
    if(!debugIdleTrigger) {
        /*
         * Open sysfs files and read battery status and level
         */
        DEBUGV(LOG_TAG_LM, "Reading Battery Status");
        ifstream batteryStatusFile(SYSFS_BATTERY_STATUS_PATH);
        if(!batteryStatusFile.is_open()) {
            DEBUGE(LOG_TAG_LM, "Not able to open %s, Error: %s",SYSFS_BATTERY_STATUS_PATH, strerror(errno));
            /* Not able to read battery charge status */
            idleAlgoThrBusy = false;
            return;
        }

        string batteryStatus;
        /* Assumes the sysfs file would contain single line */
        getline(batteryStatusFile,batteryStatus);
        BatteryStatusMap statusMap;
        BatteryStatus chargeStatus = statusMap[batteryStatus];
        DEBUGV(LOG_TAG_LM, "getChargeStatus() returned %d \n", chargeStatus);
        if(chargeStatus != BatteryStatus::CHARGING && chargeStatus != BatteryStatus::FULL) {
            DEBUGV(LOG_TAG_LM, "getChargeStatus() is NOT CHARGING \n");
            /* Battery is not charging don't run algo's*/
            idleAlgoThrBusy = false;
            return;
        }

        DEBUGV(LOG_TAG_LM, "getChargeStatus() is CHARGING / FULL \n");
        unsigned int batteryLevel;
        do {
            ifstream batteryLevelFile(SYSFS_BATTERY_LEVEL_PATH);
            if(!batteryLevelFile.is_open()) {
                DEBUGE(LOG_TAG_LM, "Not able to open %s, Error: %s",SYSFS_BATTERY_LEVEL_PATH, strerror(errno));
                /* Not able to read battery charge level */
                idleAlgoThrBusy = false;
                return;
            }
            batteryLevelFile >> batteryLevel;
            DEBUGV(LOG_TAG_LM, "Battery Level is %u \n", batteryLevel);
            DEBUGV(LOG_TAG_LM,"interruptAlgoThread %p = %s", &algoThreadInterrupt, algoThreadInterrupt ? "true" : "false");
            if(batteryLevel < IDLE_BATTERY_THRESHOLD) {
                this_thread::sleep_for(5min);
            }
        } while (batteryLevel < IDLE_BATTERY_THRESHOLD && !algoThreadInterrupt);

        if(algoThreadInterrupt) {
            /* Interrupted from LM Core thread */
            idleAlgoThrBusy = false;
            return;
        }
    }

    for (Feature* currFeature : mFeatureList) {
        if(currFeature) {
            DEBUGV(LOG_TAG_LM,"Calling runAsyncIdle for %s", currFeature->getFeatureName().c_str());
            if(mEnableLMtracing) {
                string traceLog = "LM-runAsyncIdle() " + currFeature->getFeatureName();
                ATRACE_ASYNC_BEGIN(traceLog.c_str(), 0);
            }
            currFeature->runAsyncIdle(algoThreadInterrupt);
            if(mEnableLMtracing) {
                string traceLog = "LM-runAsyncIdle() " + currFeature->getFeatureName();
                ATRACE_ASYNC_END(traceLog.c_str(), 0);
            }
            DEBUGV(LOG_TAG_LM,"runAsyncIdle for %s returned", currFeature->getFeatureName().c_str());
        }
    }
    idleAlgoThrBusy = false;
}

void LearningModule::lmMain() {
    int ret_code = 0;
    char boot_completed[PROPERTY_VALUE_MAX];
    int value = 0;

    DEBUGV(LOG_TAG_LM,"Checking boot complete ");
    do {
        if (property_get(boot_cmplt_prop, boot_completed, "0")) {
            value = atoi(boot_completed);
        }
        if(!value)
            usleep(100000);
    } while(!value);
    DEBUGV(LOG_TAG_LM,"Boot complete, post boot executed");
    DEBUGV(LOG_TAG_LM, "lmMain starting\n");

    if(mEnableLMtracing) {
        ATRACE_ASYNC_BEGIN("LM-Core Init", 0);
    }

    DEBUGV(LOG_TAG_LM, "calling pthread_setname_np()\n");
    ret_code = pthread_setname_np(pthread_self(),LM_THREAD_NAME);
    if (ret_code != 0) {
        DEBUGE(LOG_TAG_LM, "Failed to setname ThreadName: %s\n",
                LM_THREAD_NAME);
        return;
    }

    /* Initialize LM */
    /* Initialize the Database */
    DEBUGV(LOG_TAG_LM, "LMDB initial config\n");
    LMDBGlobal::LMDBGlobalConfig();

    /* Load LM non-core shared libraries */
    DEBUGV(LOG_TAG_LM, "Loading libraries \n");
    void* handle = dlopen(LIBMETERS_NAME, RTLD_NOW | RTLD_LOCAL);
    if (!handle) {
        DEBUGE(LOG_TAG_LM,"Unable to load library %s: %s", LIBMETERS_NAME,dlerror());
        return;
    }
    dlLibHandles.push_back(handle);

    /*
     * Wait for data filesystem to become available.
     * This is to ensure that the db folder is ready before we call Feature constructors
     * failing which the constructors won't be able to initialize the db
     */
    unsigned int numRetries = 0;
    while (opendir(TARGET_DATA_LM_DIR) == NULL) {
        DEBUGE(LOG_TAG_LM, "Waiting for LM data dir: %s, Error: %s", TARGET_DATA_LM_DIR,strerror(errno));
        this_thread::sleep_for(1s);
        numRetries++;
        if(numRetries > WAIT_RETRIES_FOR_DATA_DIR) {
            DEBUGE(LOG_TAG_LM, "Timeout waiting for LM data dir: %s, exiting", TARGET_DATA_LM_DIR);
            return;
        }
    }

    /* Read the xml files from TARGET_VENDOR_LM_DIR and construct the Feature objects */
    /* Assumes sane usage of xml files and feature names:
     *      No check for multiple xml files with same feature name, etc.
     */
    DEBUGV(LOG_TAG_LM, "Parsing xml's \n");
    DIR* lmDir;
    if ((lmDir = opendir(TARGET_VENDOR_LM_DIR)) == NULL) {
        DEBUGE(LOG_TAG_LM, "Fatal: can't open feature xml dir: %s, Error: %s", TARGET_VENDOR_LM_DIR,strerror(errno));
        return;
    }

    struct dirent* lmDirEnt;
    while ((lmDirEnt = readdir(lmDir)) != NULL) {
        string fileName = TARGET_VENDOR_LM_DIR + string(lmDirEnt->d_name);
        /* if file name contains .xml, use it to construct Feature objects */
        if (fileName.find(".xml") != string::npos) {
            DEBUGV(LOG_TAG_LM, "Calling getFeatureLibName for xml: %s",
                    fileName.c_str());
            FeatureXML currFeatureXML(fileName);
            string libName = currFeatureXML.getFeatureLibName();
            if(!libName.empty()) {
                handle = dlopen(libName.c_str(), RTLD_NOW | RTLD_LOCAL);
                if (!handle) {
                    DEBUGE(LOG_TAG_LM,"Unable to load library %s: %s", libName.c_str(),dlerror());
                } else {
                    dlLibHandles.push_back(handle);
                    DEBUGV(LOG_TAG_LM, "Calling getFeature for xml: %s",
                            fileName.c_str());
                    FeatureInfo currFeatureInfo = currFeatureXML.getFeatureInfo();
                    if(!currFeatureInfo.name.empty()) {
                        /* RegistryBase::get() calls new operator for corresponding element */
                        Feature* currFeature = FeatureRegistryBase::get(currFeatureInfo);
                        if(currFeature != nullptr) {
                            mFeatureList.push_back(currFeature);
                            /* Construct the trigger to feature map */
                            for(auto currAsyncId : currFeatureInfo.asyncIdList) {
                                mAsyncFeatMap[currAsyncId.first].push_back(currFeature);
                            }
                        }
                    }
                }
            }
        }
    }
    closedir(lmDir);

    /* Set lm thread readiness flag */
    lmThreadReady = true;

    if(mEnableLMtracing) {
        ATRACE_ASYNC_END("LM-Core Init", 0);
    }

    /* loop forever to receive triggers from perf HAL - exit from LM destructor */
    bool shouldExit = false;
    do {
        DEBUGV(LOG_TAG_LM,"Waiting on trigger Queue");
        TriggerInfo trigger = mTriggerQueue.pop();
        shouldExit = trigger.exitLMThread;
        if(!shouldExit) {
            auto currAsyncEntry = mAsyncFeatMap.find(trigger.payload.hintID);
            if(currAsyncEntry != mAsyncFeatMap.end()) {
                for (Feature* currFeature : currAsyncEntry->second) {
                    if(currFeature) {
                        DEBUGV(LOG_TAG_LM,"Calling runAsync for %s", currFeature->getFeatureName().c_str());
                        if(mEnableLMtracing) {
                            string traceLog = "LM-runAsync() " + currFeature->getFeatureName();
                            ATRACE_ASYNC_BEGIN(traceLog.c_str(), 0);
                        }
                        currFeature->runAsync(trigger.payload);
                        if(mEnableLMtracing) {
                            string traceLog = "LM-runAsync() " + currFeature->getFeatureName();
                            ATRACE_ASYNC_END(traceLog.c_str(), 0);
                        }
                        DEBUGV(LOG_TAG_LM,"runAsync for %s returned", currFeature->getFeatureName().c_str());
                    }
                }
            }
            if(trigger.payload.hintID == VENDOR_HINT_DISPLAY_OFF) {
                DEBUGV(LOG_TAG_LM,"trigger is VENDOR_HINT_DISPLAY_OFF");
                algoThreadInterrupt = false;
                /*
                 * LMTESTS_HINTTYPE is debug addition to trigger Algos
                 * manually from lmtests
                 */
                if(!idleAlgoThrBusy) {
                    /* Idle Algo thread not spawned yet or thread spawned earlier has completed*/
                    if(idleAlgoThread.joinable()) {
                        /* Idle Algo thread thread spawned earlier has completed, so join */
                        idleAlgoThread.join();
                    }
                    idleAlgoThread = thread(&LearningModule::detectAndRunIdle, this, (trigger.payload.hintType == LMTESTS_HINTTYPE));
                }
            }
            if(trigger.payload.hintID == VENDOR_HINT_DISPLAY_ON) {
                DEBUGV(LOG_TAG_LM,"trigger is VENDOR_HINT_DISPLAY_ON, sending algo interrupt");
                algoThreadInterrupt = true;
                /*
                 * Send interrupt to the algo thread and detach so that algo
                 * not exiting won't block LM and the impact if any of algo
                 * continuing execution ignoring interrupt is only on CPU
                 * resources
                 */
                if(idleAlgoThread.joinable()) {
                    idleAlgoThread.join();
                }
            }
        }
    } while (!shouldExit);

    /* Interrupt and wait for Algo thread */
    if(idleAlgoThread.joinable()) {
        algoThreadInterrupt = true;
        idleAlgoThread.join();
    }

    DEBUGV(LOG_TAG_LM,"Exiting LMCore Thread");
    return;
}
