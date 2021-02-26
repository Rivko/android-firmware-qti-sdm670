////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxthreadmanager.h
/// @brief Landing API declaration to use the services of the Thread Pool.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CAMXTHREADMANAGER_H
#define CAMXTHREADMANAGER_H

#include "camxdefs.h"
#include "camxtypes.h"

CAMX_NAMESPACE_BEGIN

/// @brief Opaque handle to a job
typedef UINT64 JobHandle;
/// @brief Function pointer type for a job function
typedef VOID* (*JobFunc)(VOID* pArg);
/// @brief Function pointer type for a job callback function
typedef VOID (*JobCb)(VOID* pUserData);

/// @brief Relative priority of a job submitted
enum struct JobPriority
{
    Critical = 0,   ///< Critical (highest) priority job in Camx
    High     = 1,   ///< Next to Critical priority in Camx
    Normal   = 2,   ///< Next to High priority in Camx
    Invalid  = 3    ///< Invalid priority
};

// Invalid thread handle
static const JobHandle InvalidJobHandle = 0;

// Forward declaration of implementation classes
class JobRegistry;
class JobList;
class ThreadCore;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Thread Library API
///
/// ThreadManager provides the API for using the Thread Library
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ThreadManager
{
public:

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Create
    ///
    /// @brief  Static method to create an instance of ThreadManager
    ///
    /// @param  ppInstance Instance pointer to be returned
    /// @param  pName      Name of the pool
    /// @param  numThreads Suggested number of threads in the pool
    ///
    /// @return Success or EFailed
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CamxResult Create(
        ThreadManager** ppInstance,
        const CHAR*     pName,
        UINT32          numThreads);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Destroy
    ///
    /// @brief  Method to delete an instance of ThreadManager
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID Destroy();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// RegisterJobFamily
    ///
    /// @brief  Register a job family to the thread library. A job family is the set of jobs with the same function name and
    ///         address, and priority
    ///
    /// @param  jobFuncAddr   Job function address
    /// @param  pJobFuncName  Job function name
    /// @param  flushDoneCb   Flush done callback, if the job family is flushed
    /// @param  priority      Priority of all jobs in the family
    /// @param  isSerialize   If the jobs in the family executes in serial fashion
    /// @param  phJob         Handle to the job family, returned from the library
    ///
    /// @return Success or EFailed
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult RegisterJobFamily(
        JobFunc     jobFuncAddr,
        const CHAR* pJobFuncName,
        JobCb       flushDoneCb,
        JobPriority priority,
        BOOL        isSerialize,
        JobHandle*  phJob);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// UnregisterJobFamily
    ///
    /// @brief  Un-register a job family from the thread library.
    ///
    /// @param  jobFuncAddr  Job function address
    /// @param  pJobFuncName Job function name
    /// @param  hJob         Handle to the job family
    ///
    /// @return Success or EFailed
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult UnregisterJobFamily(
        JobFunc     jobFuncAddr,
        const CHAR* pJobFuncName,
        JobHandle   hJob);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// PostJob
    ///
    /// @brief  Post a job, which may belong to a previously registered family
    ///
    /// @param  hJob         Handle to the job returned from RegisterJobFamily
    /// @param  stoppedCb    Job stopped callback for this job, If the job family is flushed
    /// @param  ppData       Array or 1 or more partitioned data, terminated by NULL pointer
    /// @param  isSplitable  If a job comes with cleanly partitioned data payloads, and this flag is true, then it will
    ///                      be split up into multiple subjobs and executed in parallel from the library. The flush,
    ///                      blocking and callback behavior will remain unchanged. Typical for image processing jobs
    /// @param  isBlocking   If the caller of the job blocks on its completion
    ///
    /// @return Success or EFailed
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult PostJob(
        JobHandle   hJob,
        JobCb       stoppedCb,
        VOID**      ppData,
        BOOL        isSplitable,
        BOOL        isBlocking);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FlushJobFamily
    ///
    /// @brief  Flush a registered family of job
    ///
    /// @param  hJob         Handle to the job returned from RegisterJobFamily
    /// @param  pUserData    User data passed back in flushDoneCB, which thread library doesn't look into
    /// @param  isBlocking   If the caller of the flush blocks on its completion
    ///
    /// @return Success or EFailed
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult FlushJobFamily(
        JobHandle   hJob,
        VOID*       pUserData,
        BOOL        isBlocking);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetJobCount
    ///
    /// @brief  Retrieve current job count of a registered job
    ///
    /// @param  hJob Handle to previously registered job
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 GetJobCount(
        JobHandle  hJob) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetInFlightCount
    ///
    /// @brief  Retrieve number of jobs currently executing
    ///
    /// @param  hJob Handle to previously registered job
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 GetInFlightCount(
        JobHandle  hJob) const;

private:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ThreadManager
    ///
    /// @brief  Constructor for ThreadManager object.
    ///
    /// @param  numThreads Suggested number of threads in the pool
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    explicit ThreadManager(
        UINT32 numThreads);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ThreadManager
    ///
    /// @brief  Default constructor for ThreadManager object.
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ThreadManager();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ~ThreadManager
    ///
    /// @brief  Destructor for ThreadManager object.
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ~ThreadManager();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Initialize
    ///
    /// @brief  Initialize a newly created ThreadManager object
    ///
    /// @param  pName      Name of the pool
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult Initialize(
        const CHAR* pName);

    // Disable copy constructor and assignment operator
    ThreadManager(const ThreadManager&) = delete;
    ThreadManager& operator=(const ThreadManager&) = delete;

    UINT32        m_numThreads;   ///< suggested number of threads in this pool
    JobRegistry*  m_pJobRegistry; ///< Pointer to job registry
    JobList*      m_pJobList;     ///< Pointer to job list
    ThreadCore*   m_pCore;        ///< Pointer to thread core
};

CAMX_NAMESPACE_END

#endif // CAMXTHREADMANAGER_H
