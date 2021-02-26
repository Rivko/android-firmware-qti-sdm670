////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxthreadcommon.h
/// @brief Provides common data structures internal to thread pool implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CAMXTHREADCOMMON_H
#define CAMXTHREADCOMMON_H

#include "camxtypes.h"
#include "camxosutils.h"
#include "camxthreadmanager.h"

CAMX_NAMESPACE_BEGIN

// Forward declaration(s)
struct RuntimeJob;

// Default max number of threads per pool
/// @todo (CAMX-28) - Replace hardcoding with setting
static const UINT32 MaxThreadsPerPool   = 5;

// Max registered jobs in a pool
static const UINT32 MaxNameLength       = 128;

// Max data partitions allowed inside a job
static const UINT32 MaxDataPartition    = 3;

// Max registered jobs in a pool
static const UINT32 MaxRuntimeJobs      = 4096;

// Max registered jobs in a pool
static const UINT32 MaxRegisteredJobs   = 256;

// One queue per priority level
static const UINT MaxNumQueues          = static_cast<UINT>(JobPriority::Invalid);

/// @brief Core status type
typedef UINT8 CoreStatus;

/// @brief Core status type
typedef UINT8 FlushStatus;

// The following status codes are defined as native types since
// atomic operations are not friendly with abstract types, like enum

// Thread core status
static const CoreStatus Error       = 0;       ///< Error state
static const CoreStatus Initialized = 1;       ///< Initialized and running state
static const CoreStatus Stopped     = 2;       ///< Stopped state

// Flush status
static const FlushStatus Noflush        = 0;    ///< Default state
static const FlushStatus FlushRequested = 1;    ///< A flush is requested
static const FlushStatus Flushed        = 2;    ///< Flush request has been fulfilled

/// @brief Status of a runtime job in the threadpool
enum class JobStatus
{
    Submitted,  ///< A job is being added to one of the job Queues
    Ready,      ///< A submitted job is being dispatched
    OnHold,     ///< A submitted job is being put on hold since one of the previous jobs in family hasn't finished
    Stopped,    ///< A flush is issued on the job family and the job is being stopped
    Invalid     ///< Invalid status
};

/// @brief Structure describing the properties of a registered Job family
struct RegisteredJob
{
    JobFunc     funcAddr;                   ///< Address of job function for job family
    CHAR        name[MaxNameLength];        ///< Text name of job function for job family
    JobCb       flushDoneCb;                ///< Flush done callback address for job family
    JobPriority priority;                   ///< Priority of jobs in the job family
    BOOL        isSerial;                   ///< Specifies if the jobs in the family need to be executed in a serial fashion

    UINT32      slot;                       ///< The slot in the job registry in which a job family is registered
    UINT32      jobCount;                   ///< Number of outstanding runtime jobs of the family
    UINT32      inflightCount;              ///< Number jobs in the family that are currently executing
    UINT32      holdCount;                  ///< Number of jobs in the family that are currently on hold
    UINT64      hRegister;                  ///< Opaque handle to the registered job

    FlushStatus flushStatus;                ///< Current flush status of the registered job
    VOID*       pFlushUserData;             ///< Userdata, opaque to the threadpool, to be passed with flush done callback
    BOOL        blockFlush;                 ///< Indicates that caller is blocked on flush
    Semaphore*  pFlushSemaphore;            ///< Semaphore to block caller of flush

    RuntimeJob* pFirstJob;                  ///< Pointer to the first runtime job, of this family, in the job list
    RuntimeJob* pLastJob;                   ///< Pointer to the last runtime job, of this family, in the job list
};

/// @brief Structure describing the properties of a runtime Job
struct RuntimeJob
{
    UINT64      hJob;                       ///< Opaque handle to the registered job
    JobCb       stoppedCb;                  ///< Stopped callback of a job, called when a flush is issued on the job family
    VOID*       pData[MaxDataPartition];    ///< Null terminated data pointers for a job
    BOOL        isSplitable;                ///< If the job can be split into multiple jobs, and executed in parallel
    BOOL        isBlocking;                 ///< If the caller is blocking on the job to complete execute

    UINT32      numPartitions;              ///< Number of data partitions for the job
    JobStatus   status;                     ///< Current status of the runtime job
    UINT32      slot;                       ///< Slot in the job list, where the runtime job sits
    Semaphore*  pJobSemaphore;              ///< Semaphore to block caller of job

    RuntimeJob* pNext;                      ///< Pointer to the next serial job in the job list
};

/// @brief Structure describing a Worker Thread
struct ThreadConfig
{
    UINT32          threadId;               ///< Logical thread number
    OSThreadHandle  hWorkThread;            ///< Platform thread id
    JobFunc         workThreadFunc;         ///< Thread entry function
    VOID*           pContext;               ///< Opaque configuration handle
};

CAMX_NAMESPACE_END

#endif // CAMXTHREADCOMMON_H
