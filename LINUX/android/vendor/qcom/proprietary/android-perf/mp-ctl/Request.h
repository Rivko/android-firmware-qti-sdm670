/******************************************************************************
  @file    Request.h
  @brief   Implementation of performance server module

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __REQUEST__H_
#define __REQUEST__H_

class ResourceInfo;

enum {
  LOCK_UNKNOWN_VERSION = -1,
  LOCK_OLD_VERSION = 0,
  LOCK_NEW_VERSION,
};

enum client_t {
    REGULAR_CLIENT,
    PROFILE_CLIENT,
    HIGH_PRIORITY_PROFILE_CLIENT,
};

enum request_prio_t {
    REGULAR_PRIORITY,
    HIGH_PRIORITY
};

class Request {
    private:
        int mVersion;
        timer_t mTimer;
        bool mTimerCreated;
        pid_t mPid;
        pid_t mTid;
        enum client_t mClient;
        int mDuration;
        unsigned int mNumArgs;
        ResourceInfo *mResources;
        request_prio_t mPriority;

    public:
        //request processing which includes
        //1.parsing
        //2.validating
        //3.physical translation
        //4.backward compatibility
        bool Process(int nargs, int list[]);

    public:
        //backward compatibility
        int TranslateOpcodeToNew(int opcde);
        int TranslateValueToNew(int major, int minor, int opcode);

    public:
        Request(int duration, pid_t client_pid, pid_t client_tid, enum client_t client);
        ~Request();
        Request(Request const& req);
        Request& operator=(Request const& req);
        bool operator==(Request const& req);
        bool operator!=(Request const& req);

        int GetNumLocks();
        ResourceInfo* GetResource(unsigned int idx);

        timer_t GetTimer() {
            return mTimer;
        }

        int GetDuration() {
            return mDuration;
        }

        int GetPid() {
            return mPid;
        }

        enum client_t GetClient() {
            return mClient;
        }

        enum request_prio_t GetPriority() {
            return mPriority;
        }

        unsigned int OverrideClientValue(int major, int minor, int value);
        bool IsResAlwaysResetOnRelease(int major, int minor);

        static void RequestTimerCallback(sigval_t pvData);
        int CreateTimer(int req_handle);
        int SetTimer();
        int DeleteTimer();
};

#endif
