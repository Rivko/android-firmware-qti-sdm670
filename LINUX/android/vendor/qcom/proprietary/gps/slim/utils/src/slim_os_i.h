/*
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/
/*!
 * @file
 * @brief Internal SLIM OS utilities API
 *
 * @ingroup slim_APSS
 */
#ifndef __SLIM_OS_I_H_INCLUDED__
#define __SLIM_OS_I_H_INCLUDED__

#include <slim_os_api.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @def SLIM_CLOCK_ID
 * @brief SLIM timer clock type
 *
 * This clock type is used by SLIM timers to measure time intervals between
 * events. The clock should be available in suspend mode, but can also work
 * in monotonic scope.
 *
 * @ingroup slim_Timers
 */
#if defined(CLOCK_BOOTTIME)
#define SLIM_CLOCK_ID CLOCK_BOOTTIME
#else
#define SLIM_CLOCK_ID CLOCK_MONOTONIC
#endif

//! @addtogroup slim_Thread
//! @{

/**
 * @copydoc slim_ThreadDataS
 */
typedef struct slim_ThreadDataS slim_ThreadDataT;
/**
 * @brief Structure for keeping global thread information
 */
struct slim_ThreadDataS
{
   SLIM_LIST(slim_ThreadT,z_Threads);
   /**< @brief List of all registered threads */
   SLIM_LIST(slim_TimerT,z_Timers);
   /**< @brief Global timer list */
   pthread_mutex_t  z_Mutex;
   /**< @brief Mutex fo synchronizing access */
   pthread_cond_t   z_Cond;
   /**< @brief Condition for some events */
};

/**
 * @brief Static global thread data
 */
extern slim_ThreadDataT slimz_ThreadData;

/*!
 * @brief Wake up thread if it is in wait state.
 *
 * The function interrupts thread wait state if appropriate. Unless thread has
 * been woken up, subsequent calls are ignored.
 *
 * This function shall be called from a critical section protected by thread
 * lock.
 *
 * @param[in] pz_Thread Thread to wake up.
 */
void slim_ThreadWakeupUnsafe
(
  slim_ThreadT *pz_Thread
);
/*!
 * @brief Reduce thread reference counter.
 *
 * The method indicates that the thread entry is no longer needed. This method
 * reduces thread reference counter, and when counter reaches 0, the thread
 * is disposed.
 *
 * This call must be called once for every thread reference counter increase.
 *
 * @param[in] pz_Thread Thread to release
 */
void slim_ThreadPut
(
  slim_ThreadT *pz_Thread
);

//! @}

#ifdef __cplusplus
}
#endif

#endif
