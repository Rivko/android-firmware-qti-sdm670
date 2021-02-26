/******************************************************************************
  @file  PerfGlueLayer.cpp
  @brief  PerfGlueLayer glues modules to the framework which needs perf events

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define LOG_TAG "ANDR-PERF-GLUELAYER"
#include <cutils/log.h>
#include <cutils/properties.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <algorithm>
#include "PerfGlueLayer.h"
#include "mp-ctl.h"

#if QC_DEBUG
#  define QLOGE(...)    ALOGE(__VA_ARGS__)
#  define QLOGD(...)    ALOGD(__VA_ARGS__)
#  define QLOGW(...)    ALOGW(__VA_ARGS__)
#  define QLOGI(...)    ALOGI(__VA_ARGS__)
#  define QLOGV(...)    ALOGV(__VA_ARGS__)
#else
#  define QLOGE(...)
#  define QLOGD(...)
#  define QLOGW(...)
#  define QLOGI(...)
#  define QLOGV(...)
#endif

using namespace std;

PerfModule PerfGlueLayer::mModules[MAX_MODULES];
vector<string> PerfGlueLayer::mLoadedModules;
PerfModule *PerfGlueLayer::mMpctlMod = NULL;
pthread_mutex_t PerfGlueLayer::mMutex = PTHREAD_MUTEX_INITIALIZER;
bool PerfGlueLayer::mBootComplete = false;

PerfGlueLayer::PerfGlueLayer() {
}

PerfGlueLayer::PerfGlueLayer(const char *libname, int *events, int numevents) {
    Register(libname, events, numevents);
}

PerfGlueLayer::~PerfGlueLayer() {
}

//interface for modules to be glued to this layer
int PerfGlueLayer::Register(const char *libname, int *events, int numevents) {
    int ret = -1, len = 0;
    bool exists = false;

    if ((NULL == libname) || (NULL == events)) {
        QLOGE("registration of perfmodule with perf-hal unsuccessful: no lib or events");
        return ret;
    }

    pthread_mutex_lock(&mMutex);
    //check if already registered
    for (int i=0; i<MAX_MODULES; i++) {
        len = strlen(mModules[i].GetLib().mLibFileName);
        if (!mModules[i].IsEmpty() &&
            !strncmp(mModules[i].GetLib().mLibFileName, libname, len)) {
            exists = true;
            break;
        }
    }

    if (exists) {
        pthread_mutex_unlock(&mMutex);
        QLOGI("Register: already registered:%s", libname);
        return ret;
    }

    //now get first available node
    for (int i=0; i<MAX_MODULES; i++) {
        if (mModules[i].IsEmpty()){
            ret = mModules[i].Register(libname, events, numevents);
            if (ret >= 0) {
                if ((VENDOR_PERF_HINT_BEGIN == events[0]) && (VENDOR_POWER_HINT_END == events[1])) {
                    mMpctlMod = &mModules[i];
                }
                ret = i;
            }
            break;
        }
    }
    pthread_mutex_unlock(&mMutex);
    return ret;
}

void PerfGlueLayer::PerfGlueLayerBootComplete() {
    char boot_completed[PROPERTY_VALUE_MAX];
    int value = 0;

    if (property_get(boot_cmplt_prop, boot_completed, "0")) {
        value = atoi(boot_completed);
    }

    if (!value) {
        QLOGI("Boot not completed");
        mBootComplete = false;
    } else {
        QLOGI("Boot completed");
        mBootComplete = true;
    }
}

//layer need to be exposed to HAL
int PerfGlueLayer::PerfGlueLayerSubmitRequest(mpctl_msg_t *msg) {
    int handle = -1, ret = 0;
    int cmd = 0;

    if (NULL == msg) {
        return handle;
    }

    pthread_mutex_lock(&mMutex);
    //boot complete check
    if (!mBootComplete) {
        QLOGI("boot is not complete");
        PerfGlueLayerBootComplete();
        if (!mBootComplete) {
            QLOGE("boot is not complete");
            pthread_mutex_unlock(&mMutex);
            return handle;
        }
    }

    cmd = msg->req_type;

    if ((cmd >= MPCTL_CMD_PERFLOCKACQ) &&
        (cmd <= MPCTL_CMD_PERFLOCK_RESTORE_GOV_PARAMS)) {
        //direct perflock request
        QLOGI("Direct perflock request");
        if (mMpctlMod && !mMpctlMod->IsEmpty()) {
            if (mMpctlMod->GetLib().SubmitRequest)
                handle = mMpctlMod->GetLib().SubmitRequest(msg);
        }
    }
    else if (MPCTL_CMD_PERFLOCKHINTACQ == cmd) {
        //hint, this can be for any module which were registered earlier
        QLOGI("Hint request");
        handle = 0;
        int hint = msg->hint_id;
        for (int i = 0; i < MAX_MODULES; i++) {
            if (!mModules[i].IsEmpty() &&
                mModules[i].IsThisEventRegistered(hint)){
                if (mModules[i].GetLib().SubmitRequest) {
                    ret = mModules[i].GetLib().SubmitRequest(msg);
                    if (&mModules[i] == mMpctlMod)
                        handle = ret;
                }
            }
        }
    }
    pthread_mutex_unlock(&mMutex);

    return handle;
}

int PerfGlueLayer::PerfGlueLayerInit() {
    int ret = -1;

    pthread_mutex_lock(&mMutex);
    for (int i = 0; i < MAX_MODULES; i++) {
        //init all registered modules
        if (!mModules[i].IsEmpty()){
            if (mModules[i].GetLib().Init) {
                ret = mModules[i].GetLib().Init();
            }
            if (ret < 0) {
                //load or init failed, deregister module
                mModules[i].Deregister();
            }
        } /*if (!mModules[i].IsEmpty())*/
    }
    pthread_mutex_unlock(&mMutex);

    return ret;
}

int PerfGlueLayer::PerfGlueLayerExit() {
    int ret = 0;

    QLOGI("PerfGlueLayer:Exit");
    pthread_mutex_lock(&mMutex);
    for (int i = 0; i < MAX_MODULES; i++) {
        //Exit all registered modules
        if (!mModules[i].IsEmpty()){
            if (mModules[i].GetLib().Exit) {
                mModules[i].GetLib().Exit();
            }
            mModules[i].Deregister();
        } /*if (!mModules[i].IsEmpty())*/
    }
    mLoadedModules.clear();
    pthread_mutex_unlock(&mMutex);

    return ret;
}

int PerfGlueLayer::LoadPerfLib(const char *libname) {
    int ret = -1, len = -1, libret = -1, i = 0;
    PerfModule tmp;
    bool loaded = false;
    bool goahead = false;

    if (NULL == libname) {
        return ret;
    }

    pthread_mutex_lock(&mMutex);
    //exceeding num loaded modules
    if (mLoadedModules.size() >= MAX_MODULES) {
        pthread_mutex_unlock(&mMutex);
        QLOGI("LoadPerfLib(%s): exceeding limit of loaded modules", libname);
        return ret;
    }

    //is this lib loading currently or loaded already
    if (!mLoadedModules.empty() &&
        std::find(mLoadedModules.begin(),
                  mLoadedModules.end(),
                  libname) != mLoadedModules.end()) {
        pthread_mutex_unlock(&mMutex);
        QLOGI("LoadPerfLib: already loaded or loading:%s", libname);
        return ret;
    }

    //check if we can load
    for (i = 0; i < MAX_MODULES; i++) {
        if (mModules[i].IsEmpty()) {
            goahead = true;
            break;
        }
    }
    if (!goahead) {
        pthread_mutex_unlock(&mMutex);
        QLOGI("LoadPerfLib: no space left for loading: %s", libname);
        return ret;
    }

    mLoadedModules.push_back(libname);
    pthread_mutex_unlock(&mMutex);

    //This will call the register for lib
    libret = tmp.LoadPerfLib(libname);

    pthread_mutex_lock(&mMutex);
    if (libret < 0) {
        mLoadedModules.erase(std::remove(mLoadedModules.begin(),
                                         mLoadedModules.end(),
                                         libname),
                             mLoadedModules.end());
        QLOGE("LoadPerfLib: library didn't load");
        pthread_mutex_unlock(&mMutex);
        return ret;
    }

    for (int i = 0; i < MAX_MODULES; i++) {
        len = strlen(mModules[i].GetLib().mLibFileName);
        if (!mModules[i].IsEmpty() &&
            !strncmp(mModules[i].GetLib().mLibFileName, libname, len) &&
            !mModules[i].GetLib().is_opened) {
            //load the module
            mModules[i].GetLib().is_opened = tmp.GetLib().is_opened;
            mModules[i].GetLib().dlhandle = tmp.GetLib().dlhandle;
            mModules[i].GetLib().Init = tmp.GetLib().Init;
            mModules[i].GetLib().Exit = tmp.GetLib().Exit;
            mModules[i].GetLib().SubmitRequest = tmp.GetLib().SubmitRequest;
            ret = i;
            QLOGI("PerfGlueLayer:LoadPerfLib loading %s at %d", libname, ret);
            break;
        }
    }
    pthread_mutex_unlock(&mMutex);

    if (ret < 0) {
        QLOGI("PerfGlueLayer:UnLoadPerfLib %s as \"ret < 0\"\n", libname);
        tmp.UnloadPerfLib();

        pthread_mutex_lock(&mMutex);
        mLoadedModules.erase(std::remove(mLoadedModules.begin(),
                                         mLoadedModules.end(),
                                         libname),
                             mLoadedModules.end());
        pthread_mutex_unlock(&mMutex);
    }

    return ret;
}

PerfModule::PerfModule() {
    mRegistered = false;
    mEventsLowerBound = -1;
    mEventsUpperBound = -1;
    mNumEvents = 0;
    for (int j = 0; j < MAX_EVENTS; j++) {
        mEvents[j] = -1;
    }
    memset(&mLibHandle,0x00, sizeof(mLibHandle));
}

PerfModule::~PerfModule() {
}

int PerfModule::LoadPerfLib(const char *libname) {
    const char *rc = NULL;
    char buf[PROPERTY_VALUE_MAX];
    int ret = -1;

    if (!mLibHandle.is_opened && (NULL != libname)) {
         mLibHandle.dlhandle = dlopen(libname, RTLD_NOW | RTLD_LOCAL);
         if (mLibHandle.dlhandle == NULL) {
             QLOGE("%s Failed to (dl)open %s\n", __func__, libname);
             return ret;
         }

         dlerror();

         *(void **) (&mLibHandle.Init) = dlsym(mLibHandle.dlhandle, "perfmodule_init");
         if ((rc = dlerror()) != NULL) {
             QLOGE("%s Failed to get perfmodule_init\n", __func__);
             dlclose(mLibHandle.dlhandle);
             mLibHandle.dlhandle = NULL;
             return ret;
         }

         *(void **) (&mLibHandle.Exit) = dlsym(mLibHandle.dlhandle, "perfmodule_exit");
         if ((rc = dlerror()) != NULL) {
             QLOGE("%s Failed to get perfmodule_exit\n", __func__);
             dlclose(mLibHandle.dlhandle);
             mLibHandle.dlhandle = NULL;
             return ret;
         }

         *(void **) (&mLibHandle.SubmitRequest) = dlsym(mLibHandle.dlhandle, "perfmodule_submit_request");
         if ((rc = dlerror()) != NULL) {
             QLOGE("%s Failed to get perfmodule_submit_request\n", __func__);
             dlclose(mLibHandle.dlhandle);
             mLibHandle.dlhandle = NULL;
             return ret;
         }

         mLibHandle.is_opened = true;
         strlcpy(mLibHandle.mLibFileName, libname, MAX_FILE_NAME);
         ret = 0;
    }

    return ret;
}

void PerfModule::UnloadPerfLib() {
    if (mLibHandle.is_opened && mLibHandle.dlhandle) {
        QLOGI("PerfModule:UnloadPerfLib %s", mLibHandle.mLibFileName);
            dlclose(mLibHandle.dlhandle);
            mLibHandle.dlhandle = NULL;
        }
        mLibHandle.Init = NULL;
        mLibHandle.Exit = NULL;
        mLibHandle.SubmitRequest = NULL;
        mLibHandle.is_opened = false;
        mLibHandle.mLibFileName[0] = '\0';

    return;
}

bool PerfModule::IsThisEventRegistered(int event) {
    bool ret = false;
    ret = (event >= mEventsLowerBound) && (event <= mEventsUpperBound);
    if (!ret) {
        //only search when event not in range
        for (int i=0; i < mNumEvents; i++) {
            if (event == mEvents[i]) {
                ret = true;
                break;
            }
        } /*for (int i=0; i<mNumEvents; i++)*/
    }
    return ret;
}

int PerfModule::Register(const char *libname, int *events, int numevents) {
    int ret = -1;

    if ((NULL == libname) || (NULL == events) || (numevents < MIN_EVENTS)) {
        QLOGE("no lib or events");
        return ret;
    }

    if (numevents > MAX_EVENTS) {
        numevents = MAX_EVENTS;
    }

    if (!mRegistered) {
        mEventsLowerBound = events[0];
        mEventsUpperBound = events[1];
        strlcpy(mLibHandle.mLibFileName, libname, MAX_FILE_NAME);
        mRegistered = true;
        mNumEvents = numevents-2;
        for (int j = 0; j < mNumEvents; j++) {
            mEvents[j] = events[j+2];
        }
        ret = 0;
    }
    return ret;
}

bool PerfModule::Deregister(){
    QLOGI("PerfModule:Deregister %s\n", mLibHandle.mLibFileName);
    UnloadPerfLib();
    mEventsLowerBound = -1;
    mEventsUpperBound = -1;
    for (int j = 0; j < MAX_EVENTS; j++) {
        mEvents[j] = -1;
    }
    mRegistered = false;
    return true;
}

void PerfModule::Dump() {
    QLOGI("mRegistered:%d mEventsLowerBound:0x%x mEventsUpperBound:0x%x mNumEvents:%d mLibHandle.mLibFileName:%s",
            mRegistered, mEventsLowerBound, mEventsUpperBound, mNumEvents, mLibHandle.mLibFileName);
}
