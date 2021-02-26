////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxperf.h
/// @brief CHX perf class declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHXPERF_H
#define CHXPERF_H

#define ATRACE_TAG (ATRACE_TAG_CAMERA | ATRACE_TAG_HAL)
#include <cutils/trace.h>

#include "chxincs.h"
#ifdef ANDROID
#include <hardware/power.h>         // Perf Lock APIs
#endif // ANDROID

/// @brief Perflock acquire function type
typedef INT32(*PerfLockAcquireFunc)(INT handle, INT duration_ms, INT params[], INT size);
/// @brief Perflock release function type
typedef INT32(*PerfLockReleaseFunc)(INT);
/// @brief Perfhint function type
typedef INT32(*PerfHintFunc)(INT hint, const char* pkg, INT duration, INT type);

struct PerfLockOps
{
    PerfLockAcquireFunc      perfLockAcquire;  ///< Acquire function to perflock API
    PerfLockReleaseFunc      perfLockRelease;  ///< Release function to perflock API
    PerfHintFunc             powerHint;        ///< Powerhint function to perflock API
};

/// @brief Perf lock enums
typedef enum
{
    PERF_LOCK_OPEN_CAMERA,
    PERF_LOCK_CLOSE_CAMERA,
    PERF_LOCK_MFNR_SNAPSHOT,
    PERF_LOCK_POWER_HINT_PREVIEW,
    PERF_LOCK_POWER_HINT_VIDEO_ENCODE,
    PERF_LOCK_POWER_HINT_VIDEO_ENCODE_60FPS,
    PERF_LOCK_POWER_HINT_VIDEO_ENCODE_HFR,
    PERF_LOCK_COUNT
}PerfLockType;

/// @brief Perf lock manager state enums
typedef enum
{
    LOCK_MGR_STATE_UNINITIALIZED,
    LOCK_MGR_STATE_READY,
    LOCK_MGR_STATE_ERROR
} PerfLockMgrState;

/// @brief Perf lock params info
typedef struct
{
    INT32*      perfLockParams;
    UINT32      perfLockParamsCount;
} PerfLockInfo;

/// @brief Perf hint enums
typedef enum
{
    PERF_HINT_NO_TYPE = -1
} PerfHintType;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Portable perf lock implementation
///
/// Allows system performance characteristics to be customized based on the use case needs.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class PerfLock final
{
public:

    /// Static method to create an instance of Perflock
    static PerfLock* Create(
        PerfLockOps*    pPerfLockOps,
        PerfLockInfo*   pPerfLockInfo);

    /// Method to delete an instance of PerfLock
    VOID Destroy();

    /// Method to acquire a perflock
    CDKResult AcquirePerfLock(
        PerfLockType type,
        BOOL         forceReacquire,
        UINT32       timer = 0);

    /// Method to release the perflock
    CDKResult ReleasePerfLock(
        PerfLockType type);

private:

    /// Default constructor for PerfLock object
    PerfLock() = default;

    /// Default destructor for PerfLock object.
    ~PerfLock() = default;

    PerfLock(const PerfLock&) = delete;                ///< Disallow the copy constructor

    PerfLock& operator=(const PerfLock&) = delete;     ///< Disallow assignment operator

    CDKResult Initialize(                              /// Initialize a perflock object
        PerfLockOps*    pPerfLockOps,
        PerfLockInfo*   pPerfLockInfo);

    CDKResult PowerHintInternal(UINT32 duration);       /// Method to set powerhint

    const char* LockTypeToString(PerfLockType type);   /// Convert lock type to string

    CHX_INLINE BOOL IsPowerHintType(PerfLockType lockType)         /// Check lock type is of powerhint type
    {
        return (lockType == (PERF_LOCK_POWER_HINT_VIDEO_ENCODE)       ||
                lockType == (PERF_LOCK_POWER_HINT_VIDEO_ENCODE_60FPS) ||
                lockType == (PERF_LOCK_POWER_HINT_VIDEO_ENCODE_HFR)   ||
                lockType == (PERF_LOCK_POWER_HINT_PREVIEW));
    };

    Mutex*                 m_pMutex;                                ///< Mutex object
    INT32                  mHandle;                                 ///< Perflock handle
    UINT32                 mRefCount;                               ///< Number of references to this lock
    PerfLockInfo*          m_pPerflockInfo;                         ///< PerfLock info
    PerfLockOps*           m_pPerfLockOps;                          ///< Perf lock API entry
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Perf lock manager implementation
///
/// Allows system performance characteristics to be customized based on the use case needs.
class PerfLockManager
{
public:

    /// Static method to create an instance of Perflock
    static PerfLockManager* Create();

    /// Method to delete an instance of PerfLockManager
    VOID Destroy();

    /// Method to acquire a perflock
    CDKResult AcquirePerfLock(
        PerfLockType lockType,
        UINT32       timer = 0);

    /// Method to release the perflock
    CDKResult ReleasePerfLock(PerfLockType lockType);

private:

    /// Default constructor for PerfLockManager object
    PerfLockManager() = default;

    /// Default destructor for PerfLock object.
    ~PerfLockManager() = default;

    PerfLockManager(const PerfLockManager&) = delete;                   ///< Disallow the copy constructor

    PerfLockManager& operator=(const PerfLockManager&) = delete;        ///< Disallow assignment operator

    CHX_INLINE BOOL IsValidPerfLock(PerfLockType lockType)              /// Check lock type is valid
    {
        return (lockType < PERF_LOCK_COUNT);
    };

    CDKResult Initialize(PerfLockInfo*   pPerfLockInfo);                /// Initialize a perflock object

    PerfLockOps         mPerfLockOps;                                    ///< Perf lock API entry
    PerfLockMgrState    mState;                                          ///< Perf lock manager state
    PerfLock*           m_pPerfLocks[PERF_LOCK_COUNT];                   ///< Created perf lock pointers
    static PerfLockInfo mPerfLockInfo[PERF_LOCK_COUNT];                  ///< Static perf lock info
};

#endif // CHXPERF_H
