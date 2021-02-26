////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxperf.cpp
/// @brief Perf class implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "chxperf.h"
#include "chxextensionmodule.h"

typedef enum {
    MPCTLV3_MIN_FREQ_CLUSTER_BIG_CORE_0    = 0x40800000,
    MPCTLV3_MIN_FREQ_CLUSTER_BIG_CORE_1    = 0x40800010,
    MPCTLV3_MIN_FREQ_CLUSTER_BIG_CORE_2    = 0x40800020,
    MPCTLV3_MIN_FREQ_CLUSTER_BIG_CORE_3    = 0x40800030,
    MPCTLV3_MIN_FREQ_CLUSTER_LITTLE_CORE_0 = 0x40800100,
    MPCTLV3_MIN_FREQ_CLUSTER_LITTLE_CORE_1 = 0x40800110,
    MPCTLV3_MIN_FREQ_CLUSTER_LITTLE_CORE_2 = 0x40800120,
    MPCTLV3_MIN_FREQ_CLUSTER_LITTLE_CORE_3 = 0x40800130,

    MPCTLV3_MAX_FREQ_CLUSTER_BIG_CORE_0    = 0x40804000,
    MPCTLV3_MAX_FREQ_CLUSTER_BIG_CORE_1    = 0x40804010,
    MPCTLV3_MAX_FREQ_CLUSTER_BIG_CORE_2    = 0x40804020,
    MPCTLV3_MAX_FREQ_CLUSTER_BIG_CORE_3    = 0x40804030,
    MPCTLV3_MAX_FREQ_CLUSTER_LITTLE_CORE_0 = 0x40804100,
    MPCTLV3_MAX_FREQ_CLUSTER_LITTLE_CORE_1 = 0x40804110,
    MPCTLV3_MAX_FREQ_CLUSTER_LITTLE_CORE_2 = 0x40804120,
    MPCTLV3_MAX_FREQ_CLUSTER_LITTLE_CORE_3 = 0x40804130,

    MPCTLV3_MIN_ONLINE_CPU_CLUSTER_BIG     = 0x41000000,
    MPCTLV3_MIN_ONLINE_CPU_CLUSTER_LITTLE  = 0x41000100,
    MPCTLV3_MAX_ONLINE_CPU_CLUSTER_BIG     = 0x41004000,
    MPCTLV3_MAX_ONLINE_CPU_CLUSTER_LITTLE  = 0x41004100,

    MPCTLV3_ALL_CPUS_PWR_CLPS_DIS          = 0x40400000,
    MPCTLV3_SCHED_BOOST                    = 0x40C00000,

    MPCTLV3_SCHED_UPMIGRATE                = 0x40C1C000,
    MPCTLV3_SCHED_DOWNMIGRATE              = 0x40C20000,

    POWER_HINT_ID_VIDEO_ENCODE             = 0x00001331,
    POWER_HINT_ID_VIDEO_ENCODE_60FPS       = 0x00001332,
    POWER_HINT_ID_VIDEO_ENCODE_HFR         = 0x00001333

} perf_lock_params;

typedef enum {
    CPU_SILVER_LEVEL_0 = 3,
    CPU_SILVER_LEVEL_1 = 480,
    CPU_SILVER_LEVEL_2 = 576,
    CPU_SILVER_LEVEL_3 = 652,
    CPU_SILVER_LEVEL_4 = 748,
    CPU_SILVER_LEVEL_5 = 825,
    CPU_SILVER_LEVEL_6 = 902,
    CPU_SILVER_LEVEL_7 = 979,
    CPU_SILVER_LEVEL_8 = 1056,
    CPU_SILVER_LEVEL_9 = 1132,
    CPU_SILVER_LEVEL_10 = 1228,
    CPU_SILVER_LEVEL_11 = 1324,
    CPU_SILVER_LEVEL_12 = 1420,
    CPU_SILVER_LEVEL_13 = 1516,
    CPU_SILVER_LEVEL_14 = 1612,
    CPU_SILVER_LEVEL_15 = 1689,
    CPU_SILVER_LEVEL_16 = 1766,
    CPU_SILVER_LEVEL_MAX = 1766
} cpu_silver_level_t;

typedef enum {
    CPU_GOLD_LEVEL_0 = 3,
    CPU_GOLD_LEVEL_1 = 480,
    CPU_GOLD_LEVEL_2 = 576,
    CPU_GOLD_LEVEL_3 = 652,
    CPU_GOLD_LEVEL_4 = 748,
    CPU_GOLD_LEVEL_5 = 825,
    CPU_GOLD_LEVEL_6 = 902,
    CPU_GOLD_LEVEL_7 = 979,
    CPU_GOLD_LEVEL_8 = 1056,
    CPU_GOLD_LEVEL_9 = 1132,
    CPU_GOLD_LEVEL_10 = 1209,
    CPU_GOLD_LEVEL_11 = 1286,
    CPU_GOLD_LEVEL_12 = 1363,
    CPU_GOLD_LEVEL_13 = 1492,
    CPU_GOLD_LEVEL_14 = 1536,
    CPU_GOLD_LEVEL_15 = 1612,
    CPU_GOLD_LEVEL_16 = 1689,
    CPU_GOLD_LEVEL_17 = 1766,
    CPU_GOLD_LEVEL_18 = 1843,
    CPU_GOLD_LEVEL_19 = 1920,
    CPU_GOLD_LEVEL_20 = 1996,
    CPU_GOLD_LEVEL_21 = 2092,
    CPU_GOLD_LEVEL_22 = 2169,
    CPU_GOLD_LEVEL_23 = 2246,
    CPU_GOLD_LEVEL_24 = 2323,
    CPU_GOLD_LEVEL_25 = 2400,
    CPU_GOLD_LEVEL_26 = 2476,
    CPU_GOLD_LEVEL_27 = 2553,
    CPU_GOLD_LEVEL_28 = 2649,
    CPU_GOLD_LEVEL_MAX = 2649
} cpu_gold_level_t;

static INT32 perfLockParamsOpenCamera[] =
{
    // Disable power collapse and set CPU clocks to turbo
    MPCTLV3_ALL_CPUS_PWR_CLPS_DIS,          0x1,
    MPCTLV3_SCHED_BOOST,                    0x1,
    MPCTLV3_MAX_FREQ_CLUSTER_BIG_CORE_0,    0xFFF,
    MPCTLV3_MIN_FREQ_CLUSTER_BIG_CORE_0,    0xFFF,
    MPCTLV3_MAX_FREQ_CLUSTER_LITTLE_CORE_0, 0xFFF,
    MPCTLV3_MIN_FREQ_CLUSTER_LITTLE_CORE_0, 0xFFF
};

static INT32 perfLockParamsCloseCamera[] =
{
    // Disable power collapse and set CPU clocks to turbo
    MPCTLV3_ALL_CPUS_PWR_CLPS_DIS,          0x1,
    MPCTLV3_SCHED_BOOST,                    0x1,
    MPCTLV3_MAX_FREQ_CLUSTER_BIG_CORE_0,    0xFFF,
    MPCTLV3_MIN_FREQ_CLUSTER_BIG_CORE_0,    0xFFF,
    MPCTLV3_MAX_FREQ_CLUSTER_LITTLE_CORE_0, 0xFFF,
    MPCTLV3_MIN_FREQ_CLUSTER_LITTLE_CORE_0, 0xFFF
};

static INT32 perfLockParamsPowerHintStartPreview[] =
{
    POWER_HINT_ID_VIDEO_ENCODE
};

static INT32 perfLockParamsPowerHintVideoEncode[] =
{
    POWER_HINT_ID_VIDEO_ENCODE
};


static INT32 perfLockParamsMFNRSnapshot[] = {
    // Set little cluster cores to 1.132 GHz
    MPCTLV3_ALL_CPUS_PWR_CLPS_DIS,          0x1,
    MPCTLV3_MIN_FREQ_CLUSTER_LITTLE_CORE_0, CPU_SILVER_LEVEL_9,
    // Set big cluster cores to 1.363 GHz
    MPCTLV3_MIN_FREQ_CLUSTER_BIG_CORE_0,    CPU_GOLD_LEVEL_12,
    // sched changes
    MPCTLV3_SCHED_DOWNMIGRATE,              0x14,  //(20)
    MPCTLV3_SCHED_UPMIGRATE,                0x1E   //(30)
};

static INT32 perfLockParamsPowerHintVideoEncodeHFR[] =
{
    POWER_HINT_ID_VIDEO_ENCODE_HFR
};

static INT32 perfLockParamsPowerHintVideoEncode60FPS[] =
{
    POWER_HINT_ID_VIDEO_ENCODE_60FPS
};

PerfLockInfo PerfLockManager::mPerfLockInfo[PERF_LOCK_COUNT] = {
    { // PERF_LOCK_OPEN_CAMERA
        perfLockParamsOpenCamera,
        sizeof(perfLockParamsOpenCamera) / sizeof(INT32)
    },
    { // PERF_LOCK_CLOSE_CAMERA
        perfLockParamsCloseCamera,
        sizeof(perfLockParamsCloseCamera) / sizeof(INT32)
    },
    { // PERF_LOCK_MFNR_SNAPSHOT
        perfLockParamsMFNRSnapshot,
        sizeof(perfLockParamsMFNRSnapshot) / sizeof(INT32)
    },
    { // PERF_LOCK_POWER_HINT_PREVIEW
        perfLockParamsPowerHintStartPreview,
        sizeof(perfLockParamsPowerHintStartPreview) / sizeof(INT32)
    },
    { // PERF_LOCK_POWER_HINT_VIDEO_ENCODE
        perfLockParamsPowerHintVideoEncode,
        sizeof(perfLockParamsPowerHintVideoEncode) / sizeof(INT32)
    },
    { // POWER_HINT_ID_VIDEO_ENCODE_60FPS
        perfLockParamsPowerHintVideoEncode60FPS,
        sizeof(perfLockParamsPowerHintVideoEncode60FPS) / sizeof(INT32)
    },
    { // POWER_HINT_ID_VIDEO_ENCODE_HFR
        perfLockParamsPowerHintVideoEncodeHFR,
        sizeof(perfLockParamsPowerHintVideoEncodeHFR) / sizeof(INT32)
    },
};

#if defined (_LINUX)

static const CHAR* pPerfModule = "libqti-perfd-client.so";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PerfLockManager::Create
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PerfLockManager * PerfLockManager::Create()
{
    PerfLockManager* pPerfLockManager = CHX_NEW PerfLockManager();

    if (NULL != pPerfLockManager)
    {
        pPerfLockManager->mState = LOCK_MGR_STATE_UNINITIALIZED;
        /// @todo Make mPerfLockInfo configurable externally
        if (CDKResultSuccess != pPerfLockManager->Initialize(&mPerfLockInfo[0]))
        {
            pPerfLockManager->mState = LOCK_MGR_STATE_ERROR;
            CHX_DELETE(pPerfLockManager);
            pPerfLockManager = NULL;
        }
        else
        {
            pPerfLockManager->mState = LOCK_MGR_STATE_READY;
        }
    }

    return pPerfLockManager;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PerfLockManager::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID PerfLockManager::Destroy()
{
    for (INT i = 0; i < PERF_LOCK_COUNT; i++)
    {
        m_pPerfLocks[i]->Destroy();
    }

    CHX_DELETE(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PerfLockManager::AcquirePerfLock
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult PerfLockManager::AcquirePerfLock(
    PerfLockType lock,
    UINT32 timer)
{
    CDKResult result = CDKResultSuccess;

    if (mState == LOCK_MGR_STATE_READY &&
        IsValidPerfLock(lock))
    {
        CHX_LOG_INFO("Acquire perflock type: %d", lock);
        result = m_pPerfLocks[lock]->AcquirePerfLock(lock, true, timer);

        if (CDKResultSuccess != result)
        {
            CHX_LOG_ERROR("Failed to acquire perflock type: %d", lock);
        }
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PerfLockManager::ReleasePerfLock
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult PerfLockManager::ReleasePerfLock(PerfLockType lockType)
{
    CDKResult result = CDKResultSuccess;

    if (mState == LOCK_MGR_STATE_READY &&
        IsValidPerfLock(lockType))
    {
        CHX_LOG_INFO("Release perflock type: %d", lockType);
        result = m_pPerfLocks[lockType]->ReleasePerfLock(lockType);

        if (CDKResultSuccess != result)
        {
            CHX_LOG_ERROR("Failed to release perflock type: %d", lockType);
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PerfLockManager::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult PerfLockManager::Initialize(
    PerfLockInfo * pPerfLockInfo)
{
    CDK_UNUSED_PARAM(pPerfLockInfo);

    CDKResult result = CDKResultSuccess;

    OSLIBRARYHANDLE perfHandle = ExtensionModule::GetInstance()->GetPerfLibHandle();

    if (NULL != perfHandle)
    {
        mPerfLockOps.perfLockAcquire = reinterpret_cast<PerfLockAcquireFunc>(ChxUtils::LibGetAddr(perfHandle, "perf_lock_acq"));
        mPerfLockOps.perfLockRelease = reinterpret_cast<PerfLockReleaseFunc>(ChxUtils::LibGetAddr(perfHandle, "perf_lock_rel"));
        mPerfLockOps.powerHint       = reinterpret_cast<PerfHintFunc>       (ChxUtils::LibGetAddr(perfHandle, "perf_hint"));

        for (INT i = 0; i < PERF_LOCK_COUNT; i++)
        {
            m_pPerfLocks[i] = PerfLock::Create(&mPerfLockOps, &mPerfLockInfo[i]);

            if (m_pPerfLocks[i] == NULL)
            {
                result = CDKResultEFailed;

                for (INT j = i; j < PERF_LOCK_COUNT; j++)
                {
                    m_pPerfLocks[j] = NULL;
                }
                return result;
            }
        }
    }
    else
    {
        CHX_LOG_ERROR("Unable to find perfd library");
        result = CDKResultEFailed;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PerfLock::Create
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PerfLock * PerfLock::Create(
    PerfLockOps * pPerfLockOps,
    PerfLockInfo * pPerfLockInfo)
{
    PerfLock* pPerfLock = CHX_NEW PerfLock();

    if (NULL != pPerfLock)
    {
        if (CDKResultSuccess != pPerfLock->Initialize(pPerfLockOps,
                                                      pPerfLockInfo))
        {
            CHX_DELETE(pPerfLock);
            pPerfLock = NULL;
        }
    }
    return pPerfLock;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PerfLockManager::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult PerfLock::Initialize(
    PerfLockOps * pPerfLockOps,
    PerfLockInfo * pPerfLockInfo)
{
    CDKResult result = CDKResultSuccess;

    m_pPerfLockOps  = pPerfLockOps;
    m_pPerflockInfo = pPerfLockInfo;

    mRefCount = 0;
    mHandle   = 0;

    m_pMutex = Mutex::Create();

    if (m_pMutex == NULL)
    {
        result = CDKResultSuccess;
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PerfLock::PowerHintInternal
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult PerfLock::PowerHintInternal(
    UINT32       duration)
{
    CDKResult result  = CDKResultSuccess;
    INT32     hintId  = m_pPerflockInfo->perfLockParams[0];

    if (mRefCount == 0)
    {
        mHandle = (m_pPerfLockOps->powerHint(hintId,
                                             NULL,
                                             duration,
                                             PERF_HINT_NO_TYPE));

        if (mHandle > 0)
        {
            ++mRefCount;
            CHX_LOG_INFO("Perfhint handle: %d, updated refCount: %d", mHandle, mRefCount);
        }
        else
        {
            CHX_LOG_ERROR("Acquire perfhint failed type %d", hintId);
            result = CDKResultEFailed;
        }
    }
    else
    {
        CHX_LOG_ERROR("Perflock already acquired, not reacquiring");
        ++mRefCount;
    }

    return result;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PerfLock::LockTypeToString
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char * PerfLock::LockTypeToString(PerfLockType type)
{
    const char* pString = NULL;

    switch (type)
    {
        case PERF_LOCK_OPEN_CAMERA:
            pString = "PERF_LOCK_OPEN_CAMERA";
            break;

        case PERF_LOCK_CLOSE_CAMERA:
            pString = "PERF_LOCK_CLOSE_CAMERA";
            break;

        case PERF_LOCK_POWER_HINT_PREVIEW:
            pString = "PERF_LOCK_POWER_HINT_PREVIEW";
            break;

        case PERF_LOCK_POWER_HINT_VIDEO_ENCODE:
            pString = "PERF_LOCK_POWER_HINT_VIDEO_ENCODE";
            break;

        case PERF_LOCK_MFNR_SNAPSHOT:
            pString = "PERF_LOCK_MFNR_SNAPSHOT";
            break;

        default:
            pString = "Unknown";
            break;
    }

    return pString;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PerfLock::AcquirePerfLock
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult PerfLock::AcquirePerfLock(PerfLockType type,
                                    BOOL         forceReacquire,
                                    UINT32       timer)
{

    CDKResult result = CDKResultSuccess;

    m_pMutex->Lock();
    ATRACE_ASYNC_BEGIN(LockTypeToString(type), type);

    if (IsPowerHintType(type))
    {
        result = PowerHintInternal(timer);
    }

    else if (mRefCount == 0 ||
             forceReacquire == TRUE)
    {
        mHandle = (m_pPerfLockOps->perfLockAcquire(mHandle,
                                                   timer,
                                                   m_pPerflockInfo->perfLockParams,
                                                   m_pPerflockInfo->perfLockParamsCount));

        if (mHandle > 0)
        {
            ++mRefCount;
            CHX_LOG_INFO("Perflock handle: %d, updated refCount: %d", mHandle, mRefCount);
        }
        else
        {
            CHX_LOG_ERROR("Acquire perflock failed");
            result = CDKResultEFailed;
        }
    }
    else
    {
        ++mRefCount;
        CHX_LOG_INFO("Perflock already acquired, not reacquiring");
    }

    m_pMutex->Unlock();

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PerfLock::ReleasePerfLock
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult PerfLock::ReleasePerfLock(PerfLockType type)
{
    CDK_UNUSED_PARAM(type);

    CDKResult result = CDKResultSuccess;

    m_pMutex->Lock();
    ATRACE_ASYNC_END(LockTypeToString(type), type);

    if (mHandle > 0)
    {
        if (--mRefCount == 0)
        {
            INT returnCode = m_pPerfLockOps->perfLockRelease(mHandle);

            mHandle = 0;

            if (returnCode < 0)
            {
                CHX_LOG_ERROR("Release perflock failed");

                result = CDKResultEFailed;
            }
        }
    }
    else
    {
        CHX_LOG_INFO("Perflock already released or not acquired");
    }

    m_pMutex->Unlock();

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PerfLock::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID PerfLock::Destroy()
{
    if (mRefCount != 0)
    {
        CHX_LOG_ERROR("Non zero references to this perflock");
    }
    m_pMutex->Destroy();

    CHX_DELETE(this);
}

#else

PerfLockManager * PerfLockManager::Create()
{
    PerfLockManager* pPerfLockManager = CHX_NEW PerfLockManager();

    pPerfLockManager->mState = LOCK_MGR_STATE_UNINITIALIZED;

    if (NULL != pPerfLockManager)
    {
        if (CDKResultSuccess != pPerfLockManager->Initialize(&mPerfLockInfo[0]))
        {
            CHX_DELETE(pPerfLockManager);
            pPerfLockManager = NULL;
        }
    }

    return pPerfLockManager;
}

VOID PerfLockManager::Destroy()
{
    CHX_DELETE(this);
}

CDKResult PerfLockManager::AcquirePerfLock(
    PerfLockType lock,
    UINT32 timer)
{
    CDK_UNUSED_PARAM(lock);
    CDK_UNUSED_PARAM(timer);

    return CDKResultSuccess;
}

CDKResult PerfLockManager::ReleasePerfLock(
    PerfLockType lockType)
{
    CDK_UNUSED_PARAM(lockType);

    return CDKResultSuccess;
}

CDKResult PerfLockManager::Initialize(
    PerfLockInfo * pPerfLockInfo)
{
    CDK_UNUSED_PARAM(pPerfLockInfo);

    return CDKResultSuccess;
}

#endif //_LINUX
