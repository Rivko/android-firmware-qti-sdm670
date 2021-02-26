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

#include <slim_os_i.h>
#include <slim_os_log_api.h>

#include <signal.h>
#include <time.h>
#include <inttypes.h>
#include <errno.h>
#include <string.h>

//! @addtogroup slim_Timer
//! @{

// Timers log as HLOS subsystem
#undef SLIM_LOG_LOCAL_MODULE
//! @brief Timers log as HLOS module.
#define SLIM_LOG_LOCAL_MODULE SLIM_LOG_MOD_HLOS

// Unless special flag is specified, logging is completely disabled.
#ifndef FEATURE_LOCTECH_SLIM_OS_LOGS
#undef SLIM_OS_LOG
#undef SLIM_OS_LOG0
//! @brief Disable OS logging.
#define SLIM_OS_LOG(...) do {} while(0)
//! @brief Disable OS logging.
#define SLIM_OS_LOG0(...) do {} while(0)
#endif
// Custom macro for signal target thread.
#ifndef sigev_notify_thread_id
//! @brief Timer event destination thread id.
#define sigev_notify_thread_id  _sigev_un._tid
#endif

/*!
 * @brief Internal timer creation helper
 *
 * @param[in] l_ClockId      OS clock type to use for timer.
 * @param[in] q_TimerId      SLIM timer identifier.
 * @param[in] q_DestThreadId IPC thread identifier.
 * @param[in] z_Tid          OS thread identifier.
 *
 * @return Timer object
 * @retval NULL on error.
 */
static slim_TimerT *TimerCreateInternal
(
  clock_t l_ClockId,
  uint32_t  q_TimerId,
  uint32_t  q_DestThreadId,
  pid_t   z_Tid
)
{
  struct sigevent z_Sev;
  slim_TimerT *pz_Timer;

  pz_Timer = malloc(sizeof(*pz_Timer));

  if (NULL == pz_Timer)
  {
     goto err_0;
  }
  memset(pz_Timer, 0, sizeof(*pz_Timer));
  SLIM_LINK_INIT(pz_Timer->z_Link);
  pz_Timer->q_TimerId = q_TimerId;
  pz_Timer->q_DestThreadId = q_DestThreadId;

  memset(&z_Sev, 0, sizeof(z_Sev));
  /* Notification types:
   * - SIGEV_SIGNAL -- Signal to process.
   * - SIGEV_THREAD_ID -- Signal to thread.
   * - SIGEV_THREAD -- Callback thread.
   * - SIGEV_NONE -- No event.
   */
  z_Sev.sigev_notify = SIGEV_THREAD_ID;
  z_Sev.sigev_notify_thread_id = z_Tid;
  z_Sev.sigev_signo = SLIM_SIG_TIMER;

  z_Sev.sigev_value.sival_ptr = pz_Timer;

  if (timer_create(l_ClockId, &z_Sev, &pz_Timer->z_Timer) == -1)
  {
     SLIM_OS_LOGE("Failed to create timer, err=%d", errno);
     goto err_1;
  }
  SLIM_OS_LOGV("Created timer %"PRIu32" for thread %ld",
      q_TimerId, (long)z_Tid);
  return pz_Timer;

err_1:
  free(pz_Timer);
err_0:
  return NULL;
}

/**
 * @brief Create new timer object
 *
 * @param[in] q_TimerId      Timer identifier.
 * @param[in] q_DestThreadId IPC identifier of destination thread.
 *
 * @return Constructed timer object.
 * @retval NULL On error.
 *
 * @sa slim_TimerCreateThread
 */
slim_TimerT *slim_TimerCreate
(
  uint32_t q_TimerId,
  uint32_t q_DestThreadId
)
{
  slim_ThreadT *pz_Thread = NULL;
  pid_t         z_Tid = 0;

  slim_ThreadDataT *pz_ThreadData = &slimz_ThreadData;
  pthread_mutex_lock(&pz_ThreadData->z_Mutex);

  SLIM_LIST_FIND(pz_ThreadData->z_Threads, pz_Thread, z_Link,
    pz_Thread->q_IpcId == q_DestThreadId);

  if (NULL != pz_Thread)
  {
    z_Tid = pz_Thread->z_Tid;
  }
  pthread_mutex_unlock(&pz_ThreadData->z_Mutex);

  if (NULL == pz_Thread)
  {
    SLIM_OS_LOGE("Thread not found");
    goto err_0;
  }

  return TimerCreateInternal(SLIM_CLOCK_ID, q_TimerId, q_DestThreadId, z_Tid);
err_0:
  return NULL;
}

/**
 * @brief Create new timer object
 *
 * @param[in] q_TimerId      Timer identifier.
 * @param[in] pz_Thread      Destination thread.
 *
 * @return Constructed timer object.
 * @retval NULL On error.
 *
 * @sa slim_TimerCreate
 */
slim_TimerT *slim_TimerCreateThread
(
  uint32_t q_TimerId,
  slim_ThreadT *pz_Thread
)
{
  slim_ThreadT *pz_Thread1 = NULL;
  pid_t         z_Tid = 0;
  uint32_t        q_IpcId = 0;

  slim_ThreadDataT *pz_ThreadData = &slimz_ThreadData;
  pthread_mutex_lock(&pz_ThreadData->z_Mutex);

  SLIM_LIST_FIND(pz_ThreadData->z_Threads, pz_Thread1, z_Link,
      pz_Thread1 == pz_Thread);

  if (NULL != pz_Thread1)
  {
    z_Tid = pz_Thread1->z_Tid;
    q_IpcId = pz_Thread1->q_IpcId;
  }
  pthread_mutex_unlock(&pz_ThreadData->z_Mutex);

  if (NULL == pz_Thread1)
  {
    SLIM_OS_LOGE("Thread not found");
    goto err_0;
  }

  return TimerCreateInternal(SLIM_CLOCK_ID, q_TimerId, q_IpcId, z_Tid);
err_0:
  return NULL;
}
/**
 * @brief Starts the timer
 *
 * If the timer is already started, it is restarted.
 *
 * @retval true  On success.
 * @retval false On error.
 *
 * @sa slim_TimerMsgT
 */
bool slim_TimerStart
(
  slim_TimerT *pz_Timer,
  /**< [in] Initialized timer */
  uint32_t q_Msecs,
  /**< [in] Number of milliseconds for next timer event */
  uint32_t q_Param
  /**< [in] User-defined parameter */
)
{
  slim_ThreadDataT *pz_ThreadData = &slimz_ThreadData;
  struct itimerspec z_Spec;

  if (NULL == pz_Timer)
  {
    goto err_0;
  }
  pz_Timer->q_Param = q_Param;

  memset(&z_Spec, 0, sizeof(z_Spec));
  z_Spec.it_value.tv_sec += q_Msecs / 1000;
  z_Spec.it_value.tv_nsec += (q_Msecs % 1000) * 1000000l;

  if (!pz_Timer->b_Started)
  {
    pthread_mutex_lock(&pz_ThreadData->z_Mutex);
    pz_Timer->b_Started = true;
    SLIM_LIST_ADD(pz_ThreadData->z_Timers, pz_Timer, z_Link);
    pthread_mutex_unlock(&pz_ThreadData->z_Mutex);
  }

  if (timer_settime(pz_Timer->z_Timer, 0, &z_Spec, NULL) == -1)
  {
    goto err_1;
  }

  return true;

err_1:
  pthread_mutex_lock(&pz_ThreadData->z_Mutex);
  pz_Timer->b_Started = false;
  SLIM_LIST_REMOVE(pz_ThreadData->z_Timers, pz_Timer, z_Link);
  pthread_mutex_unlock(&pz_ThreadData->z_Mutex);
err_0:
  return false;
}

/**
 * @brief Stops the timer
 *
 * @retval true  On success.
 * @retval false On error.
 */
bool slim_TimerStop
(
  slim_TimerT *pz_Timer
  /**< [in] Timer object */
)
{
  slim_ThreadDataT *pz_ThreadData = &slimz_ThreadData;
  struct itimerspec z_Spec;
  bool b_Stop = false;

  if (NULL == pz_Timer)
  {
    return false;
  }

  pthread_mutex_lock(&pz_ThreadData->z_Mutex);
  if (pz_Timer->b_Started)
  {
    pz_Timer->b_Started = false;
    b_Stop = true;
    SLIM_LIST_REMOVE(pz_ThreadData->z_Timers, pz_Timer, z_Link);
  }
  pthread_mutex_unlock(&pz_ThreadData->z_Mutex);

  if (b_Stop)
  {
    memset(&z_Spec, 0, sizeof(z_Spec));
    if (timer_settime(pz_Timer->z_Timer, 0, &z_Spec, NULL) == -1)
    {
      return false;
    }
  }
  return true;
}

/**
 * @brief Releases timer object
 *
 * @retval true  On success.
 * @retval false On error.
 */
bool slim_TimerDestroy
(
  slim_TimerT *pz_Timer
  /**< [in] Timer object */
)
{
  if (NULL == pz_Timer)
  {
    goto err_0;
  }

  slim_TimerStop(pz_Timer);
  free(pz_Timer);

  return true;

err_0:
  return false;
}

/**
 * @brief Provides estimated timer event time
 *
 * @return Timer event time in milliseconds.
 */
uint32_t slim_TimerCheck
(
  slim_TimerT *pz_Timer
  /**< [in] Timer object */
)
{
  uint64_t t_ResMs;
  struct itimerspec z_Its;

  if (NULL == pz_Timer)
  {
    goto err_0;
  }

  if (!pz_Timer->b_Started)
  {
    t_ResMs = 0;
  }
  else if (timer_gettime(pz_Timer->z_Timer, &z_Its))
  {
    t_ResMs = 0;
  }
  else
  {
    t_ResMs = (uint64_t) z_Its.it_value.tv_sec * 1000;
    t_ResMs += z_Its.it_value.tv_nsec / 1000000;
    if (z_Its.it_value.tv_nsec % 1000000)
    {
      t_ResMs++;
    }
  }
  return t_ResMs < UINT32_MAX ? (uint32_t) t_ResMs : UINT32_MAX;

err_0:
  return 0;
}
/**
 * @brief Internal method for marking timer stopped.
 *
 * @param[in]  pzTimer        Timer object pointer.
 * @param[out] pzTimerMsg     Message container for timer parameters.
 * @param[out] pqDestThreadId Target thread number.
 *
 * @retval true  On success.
 * @retval false On error.
 */
bool slim_TimerMarkStopped
(
  slim_TimerT *pzTimer,
  slim_TimerMsgT *pzTimerMsg,
  uint32_t *pqDestThreadId
)
{
  slim_ThreadDataT *pz_ThreadData = &slimz_ThreadData;
  slim_TimerT *pzIter;
  bool uRetVal = false;

  pthread_mutex_lock(&pz_ThreadData->z_Mutex);
  SLIM_LIST_FIND(pz_ThreadData->z_Timers, pzIter, z_Link, pzTimer == pzIter);
  if (NULL != pzIter)
  {
    pzIter->b_Started = false;
    SLIM_LIST_REMOVE(pz_ThreadData->z_Timers, pzIter, z_Link);
    if (0 != pqDestThreadId)
    {
      *pqDestThreadId = pzIter->q_DestThreadId;
    }
    if (0 != pzTimerMsg)
    {
      pzTimerMsg->q_TimerId = pzIter->q_TimerId;
      pzTimerMsg->q_Param = pzIter->q_Param;
    }
    uRetVal = true;
  }
  pthread_mutex_unlock(&pz_ThreadData->z_Mutex);
  return uRetVal;
}

//! @}
