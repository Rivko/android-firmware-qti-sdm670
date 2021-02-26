/******************************************************************************
  @file    Request.cpp
  @brief   Implementation of perflock requests

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <cstdio>
#include <cstring>
#include <pthread.h>

#define LOG_TAG "ANDR-PERF-REQUEST"
#include <cutils/log.h>

#include <time.h>
#include "mp-ctl.h"
#include "performance.h"
#include "Request.h"
#include "ResourceInfo.h"
#include "MpctlUtils.h"
#include "ResourceQueues.h"
#include "client.h"

Request::Request(int duration, pid_t client_pid, pid_t client_tid, enum client_t client) {
    mVersion = LOCK_UNKNOWN_VERSION;
    mTimerCreated = false;
    mTimer = 0;
    mPid = client_pid;
    mTid = client_tid;
    mClient = client;
    mDuration = duration;
    mNumArgs = 0;
    mResources = NULL;
    mPriority = REGULAR_PRIORITY;
    if (mClient == HIGH_PRIORITY_PROFILE_CLIENT) {
        //high profile client request is our priority
        mPriority = HIGH_PRIORITY;
    }
}

Request::~Request() {
    DeleteTimer();
    if (mResources) {
        delete[] mResources;
        mResources = NULL;
    }
    mPriority = REGULAR_PRIORITY;
}

Request::Request(Request const& req) {
    mVersion = req.mVersion;
    mTimer = req.mTimer;
    mPid = req.mPid;
    mTid = req.mTid;
    mClient = req.mClient;
    mDuration = req.mDuration;
    mNumArgs = req.mNumArgs;
    mPriority = req.mPriority;
    if (mResources) {
        mResources = new ResourceInfo[mNumArgs];
    }
    if (mResources) {
        memcpy(mResources, req.mResources, mNumArgs*sizeof(ResourceInfo ));
    }
}

Request& Request::operator=(Request const& req) {
    mVersion = req.mVersion;
    mTimer = req.mTimer;
    mPid = req.mPid;
    mTid = req.mTid;
    mClient = req.mClient;
    mDuration = req.mDuration;
    mNumArgs = req.mNumArgs;
    mPriority = req.mPriority;
    if (mResources) {
        mResources = new ResourceInfo[mNumArgs];
    }
    if (mResources) {
        memcpy(mResources, req.mResources, mNumArgs*sizeof(ResourceInfo ));
    }
    return *this;
}

bool Request::operator==(Request const& req) {
    bool ret = true;
    unsigned int i = 0;

    ret = (mDuration == req.mDuration) && (mNumArgs == req.mNumArgs);

    if (ret && mResources && req.mResources) {
        for (i=0; i<req.mNumArgs; i++) {
            ret = (mResources[i] == req.mResources[i]);
            if (!ret) {
                break;
            }
        } /*for (i=0; i<req.mNumArgs; i++)*/
    }
    else {
        ret = false;
    }

    return ret;
}

bool Request::operator!=(Request const& req) {
    return !(*this == req);
}

int Request::GetNumLocks() {
    return mNumArgs;
}

ResourceInfo *Request::GetResource(unsigned int idx) {
    if (mResources && (idx < mNumArgs)) {
        return &mResources[idx];
    }
    else {
        return NULL;
    }
}

unsigned int Request::OverrideClientValue(int major, int minor,
                                                      int value) {
            if ((major == SCHED_MAJOR_OPCODE) &&
                    ((minor == SCHED_GROUP_OPCODE) ||
                    (minor == SCHED_FREQ_AGGR_GROUP_OPCODE &&
                    value == 1)))
                return mTid;
            return value;
}

bool Request::IsResAlwaysResetOnRelease(int major, int minor) {
            return ((major == SCHED_MAJOR_OPCODE) &&
                    ((minor == SCHED_GROUP_OPCODE) ||
                    (minor == SCHED_FREQ_AGGR_GROUP_OPCODE)));
}

/*******************************************************************
 **   request processing which includes                           **
 **   1.parsing                                                   **
 **   2.validating                                                **
 **   3.physical translation                                      **
 **   4.backward compatibility                                    **
 *******************************************************************/
bool Request::Process(int nargs, int list[]) {
    int version = 0;
    unsigned i = 0;
    bool ret = true;
    int state = LOCK_UNKNOWN_VERSION;
    bool potentialOldRequest = false;
    unsigned int opcode = 0;

    if (NULL == list) {
        return false;
    }

    /** inital validation                                    **
     ** 1.num args checking                                  **
     ** 2.odd number of args in a new request                **
     ** 3.unsupported version                                */
    potentialOldRequest = ((nargs%2) != 0);
    version = (list[0] & EXTRACT_VERSION) >> SHIFT_BIT_VERSION;

    if (LOCK_OLD_VERSION == version) {
        mNumArgs = nargs;
        state = LOCK_OLD_VERSION;
    } else if (LOCK_NEW_VERSION == version) {
        //in a new type of request we're getting odd number of args
        if (potentialOldRequest) {
            ret = false;
        }
        mNumArgs = nargs / 2;
        state = LOCK_NEW_VERSION;
    } else {
        //not supported version
        ret = false;
    }

    mVersion = version;

    if (mNumArgs >= MAX_RESOURCES_PER_REQUEST) {
        //truncating/discarding few locks might not be a good idea as requestor
        //might be expecting all locks, returning error will give requestor a chance
        //to correct the requet
        QLOGI("max resources per request exceeded");
        return false;
    }

    //initial sanity check before we create memory
    if (!ret) {
        return ret;
    }

    //create memory for resources
    mResources = new ResourceInfo[mNumArgs];
    if (NULL == mResources) {
        return false;
    }

    /** next level of validation (for all resources)         **
     ** all the below checks are performed per resource      **
     ** 1.mixed args checking (not allowed)                  **
     ** 2.major/minor out of bounds                          **
     ** 3.unsupported opcode                                 **
     ** 4.unsupported version                                */
    QLOGI("mNumArgs=%d", mNumArgs);
    for (i = 0; i < mNumArgs; i++) {
        if (LOCK_NEW_VERSION == state) {
            //new request processing
            opcode = list[i*2];
            version = (opcode & EXTRACT_VERSION) >> SHIFT_BIT_VERSION;
            if (version != LOCK_NEW_VERSION) {
                //request conatins mixed old, new type resources
                ret = false;
                break;
            }
            QLOGI("populate_resource_info: opcde 0x%x", opcode);

            //parse new request
            mResources[i].ParseNewRequest(opcode);

            if (mResources[i].GetQIndex() == UNSUPPORTED_Q_INDEX) {
                //major/minor out of bounds or unsupported opcode was provided
                QLOGI("Qindx %d is  not supported", mResources[i].GetQIndex());
                ret = false;
                break;
            }

            unsigned int override_val = OverrideClientValue(
                                         mResources[i].GetMajor(),
                                         mResources[i].GetMinor(), list[i*2+1]);
            //store value which was in next arg
            mResources[i].SetValue(override_val);
            QLOGI("value stored index=%d", (i*2+1));

            //translate to physical
            if (!mResources[i].TranslateToPhysical()) {
                QLOGI("Translate to physical failed");
                ret = false;
                break;
            }

            //value mapping to physical
            if (!mResources[i].TranslateValueMapToPhysical()) {
                QLOGI("value mapping to physical failed");
                ret = false;
                break;
            }
            QLOGI("Translate to physical value succed ret=%d", ret);
        }
        else if (LOCK_OLD_VERSION == state) {
            //old request processing
            int resource = 0;
            int old_opcode = list[i];
            opcode = list[i];
            QLOGI("resource is old type");
            version = (opcode & EXTRACT_VERSION) >> SHIFT_BIT_VERSION;

            if (version != LOCK_OLD_VERSION) {
                QLOGE("request conatins mixed old, new type resource");
                //request conatins mixed old, new type resources
                ret = false;
                break;
            }

            if (opcode > 0xFFFF) {
                QLOGE("opcode 0x%x size is gretaer than valid old opcode, not valid request", opcode);
                ret = false;
                break;
            }

            //backward compatibility
            //convert old opcode to new
            opcode = TranslateOpcodeToNew(opcode);

            //parse new opcode now
            mResources[i].ParseNewRequest(opcode);

            if (mResources[i].GetQIndex() == UNSUPPORTED_Q_INDEX) {
                QLOGE("Unsupported q index %d for resource 0x%x",mResources[i].GetQIndex(), opcode);
                //major/minor out of bounds or unsupported opcode was provided
                ret = false;
                break;
            }

            //Physical translation layer not needed
            //The old opcodes donot use logical mapping, they are already physical
            //however correct physical convention need to be used
            if (!mResources[i].TranslateToPhysicalOld()) {
                ret = false;
                break;
            }

            //backward compatibility
            //convert old value to new, store value which was in next arg
            unsigned int val = TranslateValueToNew(mResources[i].GetMajor(), mResources[i].GetMinor(), old_opcode);

            QLOGI("Value for old resource returned as 0x%x %d", val, val);
            mResources[i].SetValue(val);
        }
        else {
            //unsupported version args
            ret = false;
            break;
        }

        if (ret) {
            //check for display off case
            if (mResources[i].GetQIndex() == DISPLAY_OFF_INDEX) {
                //display off request gets priority
                mPriority = HIGH_PRIORITY;
            }
        }
        QLOGI("ret=%d",ret);
    } /*for (i = 0; i < nargs; i++)*/
    QLOGI("ret=%d outside for loop", ret);

    if (!ret) {
        //invalid request, let us clean up and bail out
        QLOGI("invalid request");
        delete[] mResources;
        mResources = NULL;
    }

    return ret;
}


/****************************************************************************
 **Backward compatibility layer                                            **
 ***************************************************************************/
#define UNSUPPORTED_OPCODE 0x42C00000
#define OLD_RESOURCE_COUNT 85

static int old_opcde_to_new_opcde[OLD_RESOURCE_COUNT] = {
    0x40000000, /*DISPLAY*/
    0x40400000, /*POWER_COLLAPSE,*/
    0x40800000, /*CPU0_MIN_FREQ */
    0x40800010, /*CPU1_MIN_FREQ*/
    0x40800020, /*CPU2_MIN_FREQ*/
    0x40800030, /*CPU3_MIN_FREQ*/
    UNSUPPORTED_OPCODE, /*UNSUPPORTED_0,*/
    0x41000000, /*CLUSTR_0_CPUS_ON,*/
    0x41004000,    /*CLUSTR_0_MAX_CORES, */
    UNSUPPORTED_OPCODE, /*UNSUPPORTED_2,*/
    UNSUPPORTED_OPCODE, /* UNSUPPORTED_3,*/
    0x42400000, /*SAMPLING_RATE, */
    0x42404000, /*ONDEMAND_IO_IS_BUSY,*/
    0x42408000, /*ONDEMAND_SAMPLING_DOWN_FACTOR,*/
    0x41424000, /*INTERACTIVE_TIMER_RATE,*/
    0x41414000, /*INTERACTIVE_HISPEED_FREQ,*/
    0x41410000, /*INTERACTIVE_HISPEED_LOAD,*/
    0x4240C000, /*SYNC_FREQ,*/
    0x42410000, /*OPTIMAL_FREQ,*/
    UNSUPPORTED_OPCODE,/*SCREEN_PWR_CLPS,*/ //TODO
    0x4241C000, /*THREAD_MIGRATION, */
    0x40804000, /*CPU0_MAX_FREQ,*/
    0x40804010, /*CPU1_MAX_FREQ, */
    0x40804020, /*CPU2_MAX_FREQ,*/
    0x40804030, /*CPU3_MAX_FREQ,*/
    0x42414000, /*ONDEMAND_ENABLE_STEP_UP,*/
    0x42418000, /*ONDEMAND_MAX_INTERMEDIATE_STEPS, */
    0x41418000, /*INTERACTIVE_IO_BUSY,*/
    0x42000000, /*KSM_RUN_STATUS,*/
    0x42004000, /*KSM_PARAMS,*/
    0x40C00000, /*SCHED_BOOST,*/
    0x40800100, /*CPU4_MIN_FREQ,*/
    0x40800110, /*CPU5_MIN_FREQ,*/
    0x40800120, /*CPU6_MIN_FREQ,*/
    0x40800130, /*CPU7_MIN_FREQ,*/
    0x40804100, /*CPU4_MAX_FREQ,*/
    0x40804110, /*CPU5_MAX_FREQ,*/
    0x40804120, /*CPU6_MAX_FREQ,*/
    0x40804130, /*CPU7_MAX_FREQ,*/
    0x41400000, /*CPU0_INTERACTIVE_ABOVE_HISPEED_DELAY,*/
    0x41404000, /*CPU0_INTERACTIVE_BOOST,*/
    0x41408000, /*CPU0_INTERACTIVE_BOOSTPULSE,*/
    0x4140C000, /*CPU0_INTERACTIVE_BOOSTPULSE_DURATION,*/
    0x41410000, /*CPU0_INTERACTIVE_GO_HISPEED_LOAD,*/
    0x41414000, /*CPU0_INTERACTIVE_HISPEED_FREQ,*/
    0x41418000, /*CPU0_INTERACTIVE_IO_IS_BUSY,*/
    0x4141C000, /*CPU0_INTERACTIVE_MIN_SAMPLE_TIME,*/
    0x41420000, /*CPU0_INTERACTIVE_TARGET_LOADS,*/
    0x41424000, /*CPU0_INTERACTIVE_TIMER_RATE,*/
    0x41428000, /*CPU0_INTERACTIVE_TIMER_SLACK,*/
    0x41400100, /*CPU4_INTERACTIVE_ABOVE_HISPEED_DELAY,*/
    0x41404100, /*CPU4_INTERACTIVE_BOOST,*/
    0x41408100, /*CPU4_INTERACTIVE_BOOSTPULSE,*/
    0x4140C100, /*CPU4_INTERACTIVE_BOOSTPULSE_DURATION,*/
    0x41410100, /*CPU4_INTERACTIVE_GO_HISPEED_LOAD,*/
    0x41414100, /*CPU4_INTERACTIVE_HISPEED_FREQ,*/
    0x41418100, /*CPU4_INTERACTIVE_IO_IS_BUSY,*/
    0x4141C100, /*CPU4_INTERACTIVE_MIN_SAMPLE_TIME,*/
    0x41420100, /*CPU4_INTERACTIVE_TARGET_LOADS,*/
    0x41424100, /*CPU4_INTERACTIVE_TIMER_RATE,*/
    0x41428100, /*CPU4_INTERACTIVE_TIMER_SLACK,*/
    0x41004100, /*CLUSTR_1_MAX_CORES,*/
    0x40C04000, /*SCHED_PREFER_IDLE,*/
    0x40C08000, /*SCHED_MIGRATE_COST,*/
    0x40C0C000, /*SCHED_SMALL_TASK,*/
    0x40C10000, /*SCHED_MOSTLY_IDLE_LOAD,*/
    0x40C14000, /*SCHED_MOSTLY_IDLE_NR_RUN,*/
    0x40C18000, /*SCHED_INIT_TASK_LOAD,*/
    0x41C04000, /*VIDEO_DECODE_PLAYBACK_HINT,*/
    0x41C08000, /*DISPLAY_LAYER_HINT,*/
    0x41C00000, /*VIDEO_ENCODE_PLAYBACK_HINT,*/
    0x41800000, /*CPUBW_HWMON_MIN_FREQ,*/
    0x41804000, /*CPUBW_HWMON_DECAY_RATE,*/
    0x41808000, /*CPUBW_HWMON_IO_PERCENT,*/
    0x4142C000, /*CPU0_INTERACTIVE_MAX_FREQ_HYSTERESIS,*/
    0x4142C100, /*CPU4_INTERACTIVE_MAX_FREQ_HYSTERESIS,*/
    0x42800000, /*GPU_DEFAULT_PWRLVL,*/
    0x41000100, /*CLUSTR_1_CPUS_ON,*/
    0x40C1C000, /*SCHED_UPMIGRATE,*/
    0x40C20000, /*SCHED_DOWNMIGRATE*/
    0x40C24000, /*SCHED_MOSTLY_IDLE_FREQ*/
    0x42C04000, /*IRQ_BALANCER*/
    0x41430000, /*CPU0_INTERACTIVE_USE_SCED_LOAD,*/
    0x41434000, /*CPU0_INTERACTIVE_USE_MIGRATION_NOTIF,*/
    0x42C08000  /*INPUT_BOSST_RESET*/
};

/* Map old opcode to new opcode.
 * Also populate various elements of the ResourceInfo
 * data structure.
 * */
int Request::TranslateOpcodeToNew(int opcode) {
    QLOGI("Map old to new opcde 0x%x", opcode);
    opcode = opcode & 0xffff;
    if (opcode >= 0) {
        int tmp = (opcode & 0xff00)>>8 ;
        if (tmp < OLD_RESOURCE_COUNT) {
            QLOGI("Mapping old resource to new, indexing with 0x%x", tmp);
            opcode = old_opcde_to_new_opcde[tmp];
        }
        else {
            QLOGE("Unsupported old resource id 0x%x",opcode);
            opcode = UNSUPPORTED_OPCODE;
        }
    }
    else {
        opcode = UNSUPPORTED_OPCODE;
    }
    return opcode;
}

/* This function converts old version values to new
 * version values.
 * */
int Request::TranslateValueToNew(int major, int minor, int opcode) {
    //this is an old opcode which has opcode+value, get the value now
    int value = (opcode & 0xff);
    int resource = (opcode & 0xff00) >> 8;

    //Reset value passed with old opcode, change it to MAX_LVL
    if ((resource == ONDEMAND_IO_IS_BUSY) || (resource == THREAD_MIGRATION) ||
        (resource >= ONDEMAND_ENABLE_STEP_UP && resource <= INTERACTIVE_IO_BUSY) ||
        (resource == CPU0_INTERACTIVE_ABOVE_HISPEED_DELAY) ||
        (resource == CPU0_INTERACTIVE_BOOST) ||
        (resource >= CPU0_INTERACTIVE_IO_IS_BUSY &&
        resource <= CPU0_INTERACTIVE_TARGET_LOADS) ||
        (resource == CPU4_INTERACTIVE_ABOVE_HISPEED_DELAY) ||
        (resource == CPU4_INTERACTIVE_BOOST) ||
        (resource >= CPU4_INTERACTIVE_IO_IS_BUSY &&
        resource <= CPU4_INTERACTIVE_TARGET_LOADS) ||
        resource == SCHED_BOOST) {
        if (value == 0xFF) {
            value = MAX_LVL;
            return value;
        }
    } else {
        if (value == 0) {
            value = MAX_LVL;
            return value;
        }
    }

    if (major == CPUFREQ_MAJOR_OPCODE) {
        value = value * CONVERT_IN_MHZ;
    } else if (major == ONDEMAND_MAJOR_OPCODE) {
        switch (minor) {
            case OND_OPIMAL_FREQ_OPCODE:
            case OND_SYNC_FREQ_OPCODE:
                value = value * CONVERT_IN_MHZ;
                break;
            case OND_SAMPLING_RATE_OPCODE:
                value = 0xFF - (value & 0xFF);
                value = value * CONVERT_IN_MSEC;
                break;
        }
    } else if (major == CORE_HOTPLUG_MAJOR_OPCODE) {
        if (minor == CORE_HOTPLUG_MAX_CORE_ONLINE_OPCODE) {
            value = 0xF - (value & 0xF);
        }
    } else if (major == INTERACTIVE_MAJOR_OPCODE) {
        switch (minor) {
            case INTERACTIVE_HISPEED_FREQ_OPCODE:
                value = value * CONVERT_IN_MHZ;
                break;
            case INTERACTIVE_MIN_SAMPLE_TIME_OPCODE:
            case INTERACTIVE_BOOSTPULSE_DURATION_OPCODE:
                value = value * CONVERT_IN_MSEC;
                break;
            case INTERACTIVE_TIMER_RATE_OPCODE:
            case INTERACTIVE_TIMER_SLACK_OPCODE:
                value = 0xFF - (value & 0xFF);
                value = value * CONVERT_IN_MSEC;
                break;
        }
    } else if (major == SCHED_MAJOR_OPCODE) {
        if (minor == SCHED_MOSTLY_IDLE_FREQ_OPCODE){
            value = value * CONVERT_IN_MHZ;
        }
    }

    return value;
}

void Request::RequestTimerCallback(sigval_t pvData) {
    if (pvData.sival_int != 0) {
        perf_lock_rel(pvData.sival_int);
    }
}

int Request::CreateTimer(int req_handle) {
    int rc = 0;
    struct sigevent sigEvent;

    sigEvent.sigev_notify = SIGEV_THREAD;
    sigEvent.sigev_notify_function = &RequestTimerCallback;
    sigEvent.sigev_notify_attributes = NULL;
    sigEvent.sigev_value.sival_int = req_handle;

    rc = timer_create(CLOCK_MONOTONIC, &sigEvent, &mTimer);
    if (rc != 0) {
        QLOGE("Failed to create timer");
        return rc;
    }
    mTimerCreated = true;

    return rc;
}

int Request::SetTimer() {
    int rc = FAILED;
    int uTime = 0;
    struct itimerspec mTimeSpec;

    uTime = GetDuration();

    mTimeSpec.it_value.tv_sec = uTime / TIME_MSEC_IN_SEC;
    mTimeSpec.it_value.tv_nsec = (uTime % TIME_MSEC_IN_SEC) * TIME_NSEC_IN_MSEC;
    mTimeSpec.it_interval.tv_sec = 0;
    mTimeSpec.it_interval.tv_nsec = 0;

    //if timer created then arm it otherwise don't
    if (mTimerCreated) {
        rc = timer_settime(mTimer, 0, &mTimeSpec, NULL);
        if (rc != 0) {
            QLOGE("Failed to set timer");
        }
    }

    return rc;
}

int Request::DeleteTimer() {
    int rc = FAILED;

    if (mTimerCreated) {
        rc = timer_delete(mTimer);
        if (rc != 0) {
            QLOGE("Failed to delete timer for a request");
        }
        mTimerCreated = false;
    }
    return rc;
}
