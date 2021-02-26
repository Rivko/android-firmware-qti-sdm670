/*
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/

/**
 * @file
 * @brief Implementation of SLIM thread, timer and messaging
 * @ingroup slim_Thread slim_IPC slim_Timer
 */

#include <sys/eventfd.h>
#include <sys/signalfd.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <unistd.h>
#include <inttypes.h>
#include <signal.h>
#include <time.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stringl.h>
#include <sys/syscall.h>

#include <slim_os_i.h>
#include <slim_os_log_api.h>
#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#endif
#if !defined(__ANDROID__)
pid_t gettid(void)
{
   return syscall(SYS_gettid);
}
#endif

#undef SLIM_LOG_LOCAL_MODULE
//! @brief Log messages as HLOS module.
#define SLIM_LOG_LOCAL_MODULE SLIM_LOG_MOD_HLOS

#ifndef FEATURE_LOCTECH_SLIM_OS_LOGS
#undef SLIM_OS_LOG
#undef SLIM_OS_LOG0
//! @brief Disable OS logging.
#define SLIM_OS_LOG(...) do {} while(0)
//! @brief Disable OS logging.
#define SLIM_OS_LOG0(...) do {} while(0)
#endif

/**
 * @brief Current thread context
 *
 * @private
 * @ingroup slim_Thread
 */
static pthread_key_t z_CurrentThreadKey;
static pthread_once_t z_ThreadOnce = PTHREAD_ONCE_INIT;

/**
 * @brief Main thread object.
 */
static slim_ThreadT z_ThreadMain;

/**
 * @brief Static global thread data
 * @ingroup slim_Thread
 */
slim_ThreadDataT slimz_ThreadData=
{
  .z_Threads  = SLIM_LIST_INITIALIZER,
  .z_Timers   = SLIM_LIST_INITIALIZER,
  .z_Mutex    = PTHREAD_MUTEX_INITIALIZER,
  .z_Cond     = PTHREAD_COND_INITIALIZER,
};

static void ThreadKeyDestroy(void *pData)
{
  slim_ThreadT *pz_Thread = pData;
  if (NULL != pz_Thread)
  {
    SLIM_OS_LOGV("Thread %ld exited", (long)pz_Thread->z_Tid);
    slim_ThreadPut(pz_Thread);
  }
}
static void ThreadOnceInit()
{
  pthread_key_create(&z_CurrentThreadKey, ThreadKeyDestroy);
}
static void ThreadSetCurrent
(
  slim_ThreadT *pz_Thread
)
{
  pthread_once(&z_ThreadOnce, ThreadOnceInit);
  slim_ThreadT *pz_OldThread = pthread_getspecific(z_CurrentThreadKey);
  pthread_setspecific(z_CurrentThreadKey, pz_Thread);
  if (NULL != pz_Thread)
  {
    // Ref + 1
    pthread_mutex_lock(&pz_Thread->z_Mutex);
    ++pz_Thread->q_Cnt;
    pthread_mutex_unlock(&pz_Thread->z_Mutex);
  }
  if (NULL != pz_OldThread)
  {
    // Ref - 1
    slim_ThreadPut(pz_OldThread);
  }
}

static void *ThreadStart
(
  void *arg
)
{
  slim_ThreadDataT *pz_ThreadData = &slimz_ThreadData;
  slim_ThreadT *pz_Thread = arg;
  ThreadSetCurrent(pz_Thread); // Ref + 1; Total: 2

  pz_Thread->z_Tid = gettid();

  if (0 == sigsetjmp(pz_Thread->z_JmpBuf, 1))
  {
    pz_Thread->pfn_Main(pz_Thread->p_Arg);
  }

  return arg;
}

static bool ThreadInit
(
  slim_ThreadT *pz_Thread,
  slim_ThreadMainFn *pfn_Main,
  void *p_Arg,
  const char *pz_Name
)
{
  slim_ThreadDataT *pz_ThreadData = &slimz_ThreadData;

  memset(pz_Thread, 0, sizeof(*pz_Thread));
  SLIM_LINK_INIT(pz_Thread->z_Link);
  SLIM_LIST_INIT(pz_Thread->z_IpcQueue);
  SLIM_LIST_INIT(pz_Thread->z_OsParams);
  pz_Thread->b_EventState = false;
  pz_Thread->i_EventFd = -1;
  pz_Thread->i_SignalFd = -1;
  pz_Thread->q_Flags = 0;
  pz_Thread->q_IpcId = (uint32_t)THREAD_ID_ERROR;
  pz_Thread->pfn_Main = NULL;
  pz_Thread->p_Arg = NULL;
  pz_Thread->q_Cnt = 0;

  if (pthread_mutex_init(&pz_Thread->z_Mutex, NULL))
  {
    goto err_0;
  }

  pz_Thread->i_EventFd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
  if (pz_Thread->i_EventFd < 0)
  {
    goto err_1;
  }
  slim_sigemptyset(&pz_Thread->z_OldSigSet);

  slim_sigset_t z_SigMask;
  slim_sigemptyset(&z_SigMask);
  slim_sigaddset(&z_SigMask, SIGUSR1);
  slim_sigaddset(&z_SigMask, SIGUSR2);
  slim_sigaddset(&z_SigMask, SLIM_SIG_TIMER);

  pz_Thread->i_SignalFd = slim_signalfd(-1, &z_SigMask, SFD_NONBLOCK | SFD_CLOEXEC);
  if (-1 == pz_Thread->i_SignalFd)
  {
    goto err_2;
  }

  strlcpy(pz_Thread->z_Name, pz_Name, sizeof(pz_Thread->z_Name));
  pz_Thread->pfn_Main = pfn_Main;
  pz_Thread->p_Arg = p_Arg;

  /* Make the thread visible to others including itself */
  pz_Thread->q_Cnt = 1;
  pthread_mutex_lock(&pz_ThreadData->z_Mutex);
  SLIM_LIST_ADD(pz_ThreadData->z_Threads, pz_Thread, z_Link);
  pthread_mutex_unlock(&pz_ThreadData->z_Mutex);

  return true;

err_2:
  close(pz_Thread->i_EventFd);
err_1:
  pthread_mutex_destroy(&pz_Thread->z_Mutex);
err_0:
  return false;
}

slim_ThreadT *slim_ThreadSelf(void)
{
  pthread_once(&z_ThreadOnce, ThreadOnceInit);
  return pthread_getspecific(z_CurrentThreadKey);
}

bool slim_ThreadCreate
(
  slim_ThreadT *pz_Thread,
  slim_ThreadMainFn *pfn_Main,
  void *p_Arg,
  const char *pz_Name
)
{
  slim_sigset_t z_SigMask;
  int e;

  pthread_once(&z_ThreadOnce, ThreadOnceInit);
  if (!ThreadInit(pz_Thread, pfn_Main, p_Arg, pz_Name))
  {
    goto err_0;
  }

  slim_sigmask(SIG_SETMASK, NULL, &pz_Thread->z_OldSigSet);

  e = pthread_create(&pz_Thread->z_Thread, NULL, ThreadStart, pz_Thread);

  if (0 != e)
  {
    /* Initialized but never started - try to release */
    goto err_1;
  }

  return true;

err_1:
  slim_ThreadPut(pz_Thread);
err_0:
  return false;
}

void slim_ThreadPut
(
  slim_ThreadT *pz_Thread
)
{
  slim_ThreadDataT *pz_ThreadData = &slimz_ThreadData;
  SLIM_LIST(slim_IpcMsgT, z_IpcQueue);
  uint32_t q_Cnt;
  slim_IpcMsgT *pz_Msg;

  SLIM_LIST_INIT(z_IpcQueue);

  pthread_mutex_lock(&pz_Thread->z_Mutex);
  q_Cnt = --pz_Thread->q_Cnt;
  if (0 == q_Cnt)
  {
    /* Copy IPC messages into common pool */
    while (NULL != (pz_Msg = SLIM_LIST_HEAD(pz_Thread->z_IpcQueue)))
    {
      SLIM_LIST_REMOVE(pz_Thread->z_IpcQueue, pz_Msg, z_Link);
      SLIM_LIST_ADD(z_IpcQueue, pz_Msg, z_Link);
    }
  }
  pthread_mutex_unlock(&pz_Thread->z_Mutex);

  pthread_mutex_lock(&pz_ThreadData->z_Mutex);
  if (0 == q_Cnt)
  {
    /* Remove thread from thread list */
    SLIM_LIST_REMOVE(pz_ThreadData->z_Threads, pz_Thread, z_Link);
  }
  pthread_mutex_unlock(&pz_ThreadData->z_Mutex);

  while (NULL != (pz_Msg = SLIM_LIST_HEAD(z_IpcQueue)))
  {
    SLIM_LIST_REMOVE(z_IpcQueue, pz_Msg, z_Link);
    slim_IpcDelete(pz_Msg);
  }

  if (0 == q_Cnt)
  {
    /* Release thread data */
    pthread_mutex_destroy(&pz_Thread->z_Mutex);
    close(pz_Thread->i_EventFd);
    close(pz_Thread->i_SignalFd);
  }
}

void slim_ThreadWakeupUnsafe
(
  slim_ThreadT *pz_Thread
)
{
  if (!pz_Thread->b_EventState)
  {
    static const uint64_t zt_Evt = 1;

    SLIM_OS_LOGV("Sending event to %s", pz_Thread->z_Name);

    pz_Thread->b_EventState = true;
    write(pz_Thread->i_EventFd, &zt_Evt, sizeof(zt_Evt));
  }
}

bool slim_ThreadCancel
(
  slim_ThreadT *pz_Thread
)
{
  if (NULL != pz_Thread)
  {
    SLIM_OS_LOGD("Terminating thread (%s)", pz_Thread->z_Name);

    pthread_mutex_lock(&pz_Thread->z_Mutex);
    pz_Thread->q_Flags |= SLIM_THREAD_FLAG_ABORT;
    slim_ThreadWakeupUnsafe(pz_Thread);
    pthread_mutex_unlock(&pz_Thread->z_Mutex);
    return true;
  }
  return false;
}

bool slim_ThreadJoin
(
  slim_ThreadT *pz_Thread
)
{
  slim_ThreadDataT *pz_ThreadData = &slimz_ThreadData;

  if (NULL != pz_Thread)
  {
    pthread_join(pz_Thread->z_Thread, NULL);
    slim_ThreadPut(pz_Thread);

    return true;
  }
err_0:
  return false;
}

/**
 * @brief Registers native thread with SLIM framework
 *
 * @param[in] pz_Thread Allocated thread buffer
 * @param[in] pz_Name   Thread name (up to 8 characters)
 *
 * @retval true  On success.
 * @retval false On error.
 *
 * @sa slim_ThreadDetach
 */
bool slim_ThreadAttach
(
  slim_ThreadT *pz_Thread,
  const char *pz_Name
)
{
  if (NULL != slim_ThreadSelf())
  {
    SLIM_OS_LOGV("Double attach call");
    goto err_0;
  }
  if (!ThreadInit(pz_Thread, NULL, NULL, pz_Name))  // Ref = 1
  {
    SLIM_OS_LOGE("Thread init error");
    goto err_0;
  }

  pz_Thread->z_Thread = pthread_self();
  pz_Thread->z_Tid = gettid();
  // Store signal mask
  slim_sigmask(SIG_SETMASK, NULL, &pz_Thread->z_OldSigSet);

  ThreadSetCurrent(pz_Thread); // Ref = 2
  slim_ThreadPut(pz_Thread);   // Ref = 1

  SLIM_OS_LOGD("Thread (%s/%ld) Attached", pz_Thread->z_Name,
      (long)pz_Thread->z_Tid);

  if (0 != sigsetjmp(z_ThreadMain.z_JmpBuf, 1))
  {
    SLIM_OS_LOGW("Attached thread canceled");
    pthread_exit(0);
  }

  return true;
err_0:
  return false;
}
/**
 * @brief Removes current thread registration from SLIM framework
 *
 * @retval true  On success.
 * @retval false On error.
 *
 * @sa slim_ThreadAttach()
 */
bool slim_ThreadDetach(void)
{
  slim_ThreadDataT *pz_ThreadData = &slimz_ThreadData;
  slim_ThreadT *pz_Thread;

  pz_Thread = slim_ThreadSelf();
  if (NULL == pz_Thread || NULL != pz_Thread->pfn_Main)
  {
    goto err_0;
  }
  // Restore old signal mask
  if(slim_sigmask(SIG_SETMASK, &pz_Thread->z_OldSigSet, NULL) == -1)
  {
    SLIM_OS_LOGE("Failed to restore signal mask: %d %s", errno, strerror(errno));
  }

  SLIM_OS_LOGD("Thread (%s/%ld) detached", pz_Thread->z_Name,
      (long)pz_Thread->z_Tid);

  ThreadSetCurrent(NULL); // REF = 0

  return true;
err_0:
  return false;
}

/**
 * @brief Initializes thread object for @a main
 *
 * Main initialization does the following:
 * - Configures thread signal mask
 * - Attaches thread object
 * - Starts main thread message processing
 *
 * @retval true on success.
 */
bool slim_MainThreadInit
(
  const char *pz_Name,
  /**< [in] Thread name (up to 8 characters) */
  uint32_t q_IpcId
  /**< [in] IPC thread number */
)
{
  pthread_once(&z_ThreadOnce, ThreadOnceInit);

  slim_ThreadAttach(&z_ThreadMain, pz_Name);

  slim_IpcStart(q_IpcId);

  // Modify signal descriptor.
  slim_sigset_t z_SigMask;
  slim_sigemptyset(&z_SigMask);
  slim_sigaddset(&z_SigMask, SIGINT);
  slim_sigaddset(&z_SigMask, SIGQUIT);
  slim_sigaddset(&z_SigMask, SIGUSR1);
  slim_sigaddset(&z_SigMask, SIGUSR2);
  slim_sigaddset(&z_SigMask, SLIM_SIG_TIMER);

  z_ThreadMain.i_SignalFd = slim_signalfd(
      z_ThreadMain.i_SignalFd, &z_SigMask, SFD_NONBLOCK | SFD_CLOEXEC);

  if (0 != sigsetjmp(z_ThreadMain.z_JmpBuf, 1))
  {
    SLIM_OS_LOGW("Main thread canceled");
    exit(1);
  }

  return true;
err_0:
  return false;
}

bool slim_MainThreadDeinit
(
)
{
  if (slim_ThreadDetach())
  {
    SLIM_OS_LOGD("Main thread detached.");
    return true;
  }
  else
  {
    SLIM_OS_LOGE("Main thread is not attached.");
    return false;
  }
}

/**
 * @brief Notify control service that the task is ready
 *
 * @sa slim_TaskReadyWait
 */
void slim_TaskReadyAck()
{
  slim_ThreadDataT *pz_ThreadData = &slimz_ThreadData;
  slim_ThreadT *pz_Thread = slim_ThreadSelf();
  if (NULL != pz_Thread)
  {
    pthread_mutex_lock(&pz_ThreadData->z_Mutex);
    pz_Thread->q_Flags |= SLIM_THREAD_FLAG_READY_ACK;
    pthread_cond_broadcast(&pz_ThreadData->z_Cond);
    pthread_mutex_unlock(&pz_ThreadData->z_Mutex);
  }
  else
  {
    // non-SLIM thread
  }
}
/**
 * @brief Block until task is ready
 *
 * @param[in] q_ThreadId Task to wait until ready.
 *
 * @sa slim_TaskReadyAck
 */
void slim_TaskReadyWait
(
  uint32_t q_ThreadId
)
{
  slim_ThreadDataT *pz_ThreadData = &slimz_ThreadData;
  slim_ThreadT *pz_Thread = 0;

  pthread_mutex_lock(&pz_ThreadData->z_Mutex);
  SLIM_LIST_FIND(pz_ThreadData->z_Threads, pz_Thread, z_Link,
    pz_Thread->q_IpcId == q_ThreadId);

  // Wait until thread is found and has READY_ACK flag set
  while (0 == pz_Thread ||
         0 == (pz_Thread->q_Flags & SLIM_THREAD_FLAG_READY_ACK))
  {
    pthread_cond_wait(&pz_ThreadData->z_Cond, &pz_ThreadData->z_Mutex);
    SLIM_LIST_FIND(pz_ThreadData->z_Threads, pz_Thread, z_Link,
      pz_Thread->q_IpcId == q_ThreadId);
  }
  pthread_mutex_unlock(&pz_ThreadData->z_Mutex);
}

#if defined(__ANDROID__) && !defined(__LP64__)
/*
Custom implementation for signal functions in SLIM.
See comments in: bionic/libc/private/kernel_sigset_t.h

Android ARM/x86 ABI has a signal mask of 32-bit, while kernel supports 64.
To enable RT signal operation, a usage of @ref syscall is required with
functions that operate against sigset_t.
*/

/** @sa sigemptyset */
int slim_sigemptyset
(
  slim_sigset_t *pzSigSet
)
{
  if (NULL != pzSigSet)
  {
    memset(pzSigSet, 0, sizeof(*pzSigSet));
    return 0;
  }
  else
  {
    errno = EINVAL;
    return -1;
  }
}
/** @sa sigaddset */
int slim_sigaddset
(
  slim_sigset_t *pzSigSet,
  int lSigNo
)
{
  // _NSIG is a KERNEL_NSIG + 1, as user signals are counted from 1.
  // Some older platforms have _NSIG equal to KERNEL_NSIG, in which case the
  // last signal number would not be accessible
  if (lSigNo > 0 && lSigNo < _NSIG && NULL != pzSigSet)
  {
    int lBit = lSigNo - 1;
    pzSigSet->qMask[lBit / LONG_BIT] |= 1UL << (lBit % LONG_BIT);
    return 0;
  }
  else
  {
    errno = EINVAL;
    return -1;
  }
}

/** @sa sigismember */
int slim_sigismember
(
  const slim_sigset_t *pzSigSet,
  int lSigNo
)
{
  // _NSIG is a KERNEL_NSIG + 1, as user signals are counted from 1.
  // Some older platforms have _NSIG equal to KERNEL_NSIG, in which case the
  // last signal number would not be accessible
  if (lSigNo > 0 && lSigNo < _NSIG && NULL != pzSigSet)
  {
    int lBit = lSigNo - 1;
    unsigned long qMask = 1UL << (lBit % LONG_BIT);
    if (0 != (pzSigSet->qMask[lBit / LONG_BIT] & qMask))
    {
      return 1;
    }
    return 0;
  }
  else
  {
    errno = EINVAL;
    return -1;
  }
}

/** @sa pthread_sigmask */
int slim_sigmask
(
  int lAction,
  const slim_sigset_t *pzSigSet,
  slim_sigset_t *pzSigSetOut
)
{
  if (pzSigSet && ARR_SIZE(pzSigSet->qMask) >= 2)
  {
    SLIM_OS_LOGV("Mask change (%d): %08lX:%08lX",
                 lAction, pzSigSet->qMask[1], pzSigSet->qMask[0]);
  }
  return syscall(
      SYS_rt_sigprocmask,
      lAction,
      pzSigSet,
      pzSigSetOut,
      sizeof(*pzSigSet));
}

/** @sa signalfd */
int slim_signalfd
(
  int lFd,
  const slim_sigset_t *pzSigMask,
  int lFlags
)
{
  return syscall(SYS_signalfd4, lFd, pzSigMask, sizeof(*pzSigMask), lFlags);
}
#endif /* __ANDROID__ && !__LP64__ */
