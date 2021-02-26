/******************************************************************************
  @file  PerfGlueLayer.h
  @brief  Perf Hal glue layer interface

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#ifndef __PERF_GLUE_LAYER_H__
#define __PERF_GLUE_LAYER_H__


#include <pthread.h>
#include <string>
#include <vector>

#define MAX_MODULES 4
#define MAX_EVENTS 32
#define MIN_EVENTS 2
#define MAX_FILE_NAME 128

class EventQueue;
struct mpctl_msg_t;

typedef struct ModuleLibInfo {
    bool is_opened;
    char mLibFileName[MAX_FILE_NAME];
    void *dlhandle;
    int  (*Init)(void);
    void (*Exit)(void);
    int (*SubmitRequest)(mpctl_msg_t*);
} ModuleLibInfo;

class PerfModule {
protected:
    bool mRegistered;
    int mEventsLowerBound;
    int mEventsUpperBound;
    int mNumEvents;
    int mEvents[MAX_EVENTS];
    ModuleLibInfo mLibHandle;

public:
    PerfModule();
    ~PerfModule();
    int LoadPerfLib(const char *libname);
    void UnloadPerfLib();
    bool IsThisEventRegistered(int event);
    int Register(const char *libname, int *events, int numevents);
    bool Deregister();
    void Dump();

    bool IsEmpty() {
        return !mRegistered;
    }

    ModuleLibInfo &GetLib() {
        return mLibHandle;
    }
};

class PerfGlueLayer {
protected:
    static PerfModule mModules[MAX_MODULES];
    static std::vector<std::string> mLoadedModules;
    static PerfModule *mMpctlMod;
    static pthread_mutex_t mMutex;
    static bool mBootComplete;

    bool Deregister(int handle);
    void PerfGlueLayerBootComplete();
    int Register(const char *libname, int *events, int numevents);
public:
    PerfGlueLayer();

    //events need to be passed as an array, first two elements should contain range
    //of the events interested in (if not interested in range, -1, -1 should be specified),
    //next elements should contain individual events, max is limited by MAX_EVENTS
    //numevents should be first two elements (range) + number of next elements
    PerfGlueLayer(const char *libname, int *events, int numevents);
    int LoadPerfLib(const char *libname);

    ~PerfGlueLayer();

    //interface to be exposed for HAL
    int PerfGlueLayerSubmitRequest(mpctl_msg_t *msg);
    int PerfGlueLayerInit();
    int PerfGlueLayerExit();
};

#endif
