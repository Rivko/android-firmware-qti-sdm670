/*
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/

/**
 * @file
 * @brief Implementation of SLIM messaging
 * @ingroup slim_Thread slim_IPC slim_Timer
 */

#include <slim_os_i.h>
#include <slim_os_log_api.h>

#include <poll.h>
#include <unistd.h>
#include <sys/eventfd.h>
#include <sys/signalfd.h>
#include <inttypes.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
//! @addtogroup slim_Thread
//! @{

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
 * @brief Enables IPC message receiving for the current thread
 *
 * @param[in] q_IpcId IPC identifier for the current thread.
 *
 * @retval true on success
 * @retval false on error
 */
bool slim_IpcStart
(
  uint32_t q_IpcId
)
{
  slim_ThreadT *pz_Thread = slim_ThreadSelf();
  if (NULL != pz_Thread && 0 == pz_Thread->q_IpcId)
  {
    pz_Thread->q_IpcId = q_IpcId;

    slim_sigset_t z_SigMask;
    slim_sigemptyset(&z_SigMask);
    slim_sigaddset(&z_SigMask, SIGINT);
    slim_sigaddset(&z_SigMask, SIGQUIT);
    slim_sigaddset(&z_SigMask, SIGUSR1);
    slim_sigaddset(&z_SigMask, SIGUSR2);
    slim_sigaddset(&z_SigMask, SLIM_SIG_TIMER);
    slim_sigemptyset(&pz_Thread->z_OldSigSet);
    if (slim_sigmask(SIG_BLOCK, &z_SigMask, 0) == -1)
    {
      SLIM_OS_LOGE("sigprocmask error=%d %s", errno, strerror(errno));
    }

    SLIM_OS_LOGV("Registered IPC%"PRIu32, q_IpcId);

    return true;
  }

  SLIM_OS_LOGE("Failed to start IPC(%"PRIu32")", q_IpcId);

  return false;
}

slim_IpcMsgT *slim_IpcAllocate
(
  uint32_t q_Size
)
{
  slim_IpcMsgT *pz_Msg;

  pz_Msg = malloc(sizeof(*pz_Msg) + q_Size);
  if (NULL == pz_Msg)
  {
    errno = ENOMEM;
    goto err;
  }
  memset((void*) pz_Msg, 0, sizeof(*pz_Msg) + q_Size);
  SLIM_LINK_INIT(pz_Msg->z_Link);
  pz_Msg->q_Size = q_Size;

  return pz_Msg;
err:
  return NULL;
}

void slim_IpcDelete
(
  slim_IpcMsgT *pz_Msg
)
{
  if (NULL != pz_Msg)
  {
    free(pz_Msg);
  }
}

/**
 * @brief Send message into targeted thread queue.
 *
 * @param[in] pz_Thread Destination thread.
 * @param[in] pz_Msg    Message to send.
 *
 * @return Operation status.
 * @retval true  If message has been sent.
 * @retval false If there has been an error.
 */
bool slim_IpcSendThread
(
  slim_ThreadT *pz_Thread,
  slim_IpcMsgT *pz_Msg
)
{
  slim_ThreadDataT *pz_ThreadData = &slimz_ThreadData;
  slim_ThreadT *pz_ThisThread = slim_ThreadSelf();
  slim_ThreadT *pz_DestThread;

  if (NULL == pz_Msg || NULL == pz_Thread)
  {
    errno = EINVAL;
    goto err;
  }
  if (NULL != pz_ThisThread && 0 == pz_Msg->q_SrcThreadId)
  {
    pz_Msg->q_SrcThreadId = pz_ThisThread->q_IpcId;
  }
  pz_Msg->q_DestThreadId = pz_Thread->q_IpcId;
  pthread_mutex_lock(&pz_ThreadData->z_Mutex);

  SLIM_LIST_FIND(pz_ThreadData->z_Threads, pz_DestThread, z_Link,
      pz_DestThread == pz_Thread);

  pthread_mutex_unlock(&pz_ThreadData->z_Mutex);

  if (NULL != pz_DestThread)
  {
    SLIM_OS_LOGV("IPC(%"PRIu32") -> [%d/%s]", pz_Msg->q_MsgId,
        pz_DestThread->q_IpcId, pz_DestThread->z_Name);

    pthread_mutex_lock(&pz_DestThread->z_Mutex);
    pz_DestThread->q_Cnt++;
    SLIM_LIST_ADD(pz_DestThread->z_IpcQueue, pz_Msg, z_Link);
    slim_ThreadWakeupUnsafe(pz_DestThread);
    pthread_mutex_unlock(&pz_DestThread->z_Mutex);
    slim_ThreadPut(pz_DestThread);

    SLIM_OS_LOGV("IPC Sent");

    return true;
  }
err:
  return false;
}

bool slim_IpcSend
(
  uint32_t q_DestThreadId,
  slim_IpcMsgT *pz_Msg
)
{
  slim_ThreadDataT *pz_ThreadData = &slimz_ThreadData;
  slim_ThreadT *pz_ThisThread = slim_ThreadSelf();
  slim_ThreadT *pz_DestThread;

  if (NULL == pz_Msg || 0 == q_DestThreadId)
  {
    errno = EINVAL;
    goto err;
  }

  if (NULL != pz_ThisThread && 0 == pz_Msg->q_SrcThreadId)
  {
    pz_Msg->q_SrcThreadId = pz_ThisThread->q_IpcId;
  }
  pz_Msg->q_DestThreadId = q_DestThreadId;

  pthread_mutex_lock(&pz_ThreadData->z_Mutex);

  SLIM_LIST_FIND(pz_ThreadData->z_Threads, pz_DestThread, z_Link,
      pz_DestThread->q_IpcId == q_DestThreadId);

  pthread_mutex_unlock(&pz_ThreadData->z_Mutex);

  if (NULL != pz_DestThread)
  {
    SLIM_OS_LOGV("IPC(%"PRIu32") -> [%d/%s]", pz_Msg->q_MsgId,
        q_DestThreadId, pz_DestThread->z_Name);

    pthread_mutex_lock(&pz_DestThread->z_Mutex);
    pz_DestThread->q_Cnt++;
    SLIM_LIST_ADD(pz_DestThread->z_IpcQueue, pz_Msg, z_Link);
    slim_ThreadWakeupUnsafe(pz_DestThread);
    pthread_mutex_unlock(&pz_DestThread->z_Mutex);
    slim_ThreadPut(pz_DestThread);

    SLIM_OS_LOGV("IPC Sent");

    return true;
  }
err:
  return false;
}

bool slim_IpcSendEmpty
(
  uint32_t      q_DestThreadId,
  uint32_t      q_MsgId
)
{
  return slim_IpcSendData(q_DestThreadId, q_MsgId, NULL, 0);
}

bool slim_IpcSendData
(
  uint32_t    q_DestThreadId,
  uint32_t    q_MsgId,
  const void *p_Data,
  uint32_t    q_DataSize
)
{
  slim_IpcMsgT *pz_Msg;

  if (0 != q_DataSize && NULL == p_Data)
  {
    SLIM_OS_LOGE("NULL argument");
    goto err_0;
  }

  pz_Msg = slim_IpcAllocate(q_DataSize);
  if (NULL == pz_Msg)
  {
    goto err_0;
  }
  pz_Msg->q_MsgId = q_MsgId;
  if (q_DataSize)
  {
    memcpy((void*) pz_Msg->p_Data, p_Data, q_DataSize);
  }
  if (!slim_IpcSend(q_DestThreadId, pz_Msg))
  {
    SLIM_OS_LOGE("IPC Send Error");
    goto err_1;
  }
  return true;
err_1:
  slim_IpcDelete(pz_Msg);
err_0:
  return false;
}


slim_IpcMsgT *slim_IpcReceive
(
   void
)
{
  slim_ThreadDataT *pz_ThreadData = &slimz_ThreadData;
  slim_ThreadT *pz_Thread = slim_ThreadSelf();
  slim_IpcMsgT *pz_Msg = NULL;

  if (NULL == pz_Thread)
  {
    SLIM_OS_LOGE("No SLIM thread context");
    errno = EINVAL;
    goto err;
  }

  for (;;)
  {
    bool v_Abort = false;
    struct pollfd z_Pfd[64];
    uint16_t w_FdCnt = 0;
    slim_OsParamsT *pz_OsParams;

    z_Pfd[0].fd = pz_Thread->i_EventFd;
    z_Pfd[0].events = POLLIN;
    z_Pfd[1].fd = pz_Thread->i_SignalFd;
    z_Pfd[1].events = POLLIN;
    w_FdCnt = 2;

    pthread_mutex_lock(&pz_Thread->z_Mutex);
    v_Abort = 0 != (pz_Thread->q_Flags & SLIM_THREAD_FLAG_ABORT);
    if (!v_Abort)
    {
      pz_Msg = SLIM_LIST_HEAD(pz_Thread->z_IpcQueue);
      if (NULL != pz_Msg)
      {
        SLIM_LIST_REMOVE(pz_Thread->z_IpcQueue, pz_Msg, z_Link);
      }
      if (NULL == SLIM_LIST_HEAD(pz_Thread->z_IpcQueue))
      {
        uint64_t t_Temp;
        read(pz_Thread->i_EventFd, &t_Temp, sizeof(t_Temp));
        pz_Thread->b_EventState = false;
      }
      if (NULL != pz_Msg)
      {
        goto out;
      }

      pz_OsParams = SLIM_LIST_HEAD(pz_Thread->z_OsParams);
      while (NULL != pz_OsParams)
      {
        int i_Fd;
        qmi_csi_os_params *pz_Qmi = pz_OsParams->pz_OsParams;

        for (i_Fd = 0; i_Fd <= pz_Qmi->max_fd; ++i_Fd)
        {
          if (FD_ISSET(i_Fd, &pz_Qmi->fds))
          {
            z_Pfd[w_FdCnt].fd = i_Fd;
            z_Pfd[w_FdCnt].events = POLL_IN;
            ++w_FdCnt;

            if (ARR_SIZE(z_Pfd) <= w_FdCnt)
            {
              SLIM_OS_LOGW("Reached maximum number of poll descriptors");
              goto out;
            }
          }
        }

        pz_OsParams = SLIM_LINK_NEXT(pz_OsParams->z_Link);
      }
out: ;
    }
    pthread_mutex_unlock(&pz_Thread->z_Mutex);

    if (v_Abort)
    {
      SLIM_OS_LOGD("Aborting thread");
      longjmp(pz_Thread->z_JmpBuf, 1);
    }
    else if (NULL != pz_Msg)
    {
      SLIM_OS_LOGV("IPC(%"PRIu32") <= [%"PRIu32"]", pz_Msg->q_MsgId,
          pz_Msg->q_SrcThreadId);
      return pz_Msg;
    }

    SLIM_OS_LOGV("Starting poll: cnt=%d", w_FdCnt);

    int i_Res = poll(z_Pfd, w_FdCnt, -1);

    SLIM_OS_LOGV("Poll result=%d", i_Res);

    if (i_Res > 0)
    {
      if (z_Pfd[0].revents & POLLIN)
      {
        uint64_t t_Temp;
        read(pz_Thread->i_EventFd, &t_Temp, sizeof(t_Temp));
        --i_Res;
      }
      if (z_Pfd[1].revents & POLLIN)
      {
        struct signalfd_siginfo z_SigInfo;
        ssize_t t_Res;

        t_Res = read(pz_Thread->i_SignalFd, &z_SigInfo, sizeof(z_SigInfo));
        if (sizeof(z_SigInfo) == t_Res)
        {
          switch (z_SigInfo.ssi_signo)
          {
          case SIGINT:
          case SIGQUIT:
            SLIM_OS_LOGV("Detected SIGINT/SIGQUIT");
            pz_Msg = slim_IpcAllocate(0);
            if (NULL != pz_Msg)
            {
              pz_Msg->q_MsgId = M_IPC_MSG_SIGINT;
              pz_Msg->q_SrcThreadId = 0;
              pz_Msg->q_DestThreadId = pz_Thread->q_IpcId;
            }
            return pz_Msg;
          case SIGUSR1:
            SLIM_OS_LOGV("Detected SIGUSR1");
            pz_Msg = slim_IpcAllocate(0);
            if (NULL != pz_Msg)
            {
              pz_Msg->q_MsgId = M_IPC_MSG_SIGUSR1;
              pz_Msg->q_SrcThreadId = 0;
              pz_Msg->q_DestThreadId = pz_Thread->q_IpcId;
            }
            return pz_Msg;
          case SIGUSR2:
            SLIM_OS_LOGV("Detected SIGUSR2");
            pz_Msg = slim_IpcAllocate(0);
            if (NULL != pz_Msg)
            {
              pz_Msg->q_MsgId = M_IPC_MSG_SIGUSR2;
              pz_Msg->q_SrcThreadId = 0;
              pz_Msg->q_DestThreadId = pz_Thread->q_IpcId;
            }
            return pz_Msg;
          default:
            if ((unsigned)SLIM_SIG_TIMER == z_SigInfo.ssi_signo)
            {
              SLIM_OS_LOGV("Detected TIMER");
              {
                slim_TimerMsgT z_TimerMsg;
                uint32_t q_DestThreadId;

                pz_Msg = NULL;

                if (slim_TimerMarkStopped(
                    (slim_TimerT*) (uintptr_t) z_SigInfo.ssi_ptr,
                    &z_TimerMsg,
                    &q_DestThreadId))
                {
                  pz_Msg = slim_IpcAllocate(sizeof(z_TimerMsg));
                  if (NULL != pz_Msg)
                  {
                    pz_Msg->q_MsgId = M_IPC_MSG_TIMER;
                    pz_Msg->q_DestThreadId = q_DestThreadId;
                    memcpy((void*) pz_Msg->p_Data, &z_TimerMsg,
                        sizeof(z_TimerMsg));

                    return pz_Msg;
                  }
                }
              }
            }
            break;
          }
        }
      }

      pz_OsParams = SLIM_LIST_HEAD(pz_Thread->z_OsParams);
      while (NULL != pz_OsParams)
      {
        uint32_t q_FdIdx;
        int i_MaxFd = -1;
        bool b_Signal = false;
        slim_QCSIMsgT z_Data;

        memset(&z_Data, 0, sizeof(z_Data));
        FD_ZERO(&z_Data.z_OsParams.fds);

        for (q_FdIdx = 0; q_FdIdx < w_FdCnt; ++q_FdIdx)
        {
          int i_Fd = z_Pfd[q_FdIdx].fd;

          if (0 != (z_Pfd[q_FdIdx].revents & POLLIN) &&
              i_Fd <= pz_OsParams->pz_OsParams->max_fd &&
              FD_ISSET(i_Fd, &pz_OsParams->pz_OsParams->fds))
          {
            FD_SET(i_Fd, &z_Data.z_OsParams.fds);
            if (i_Fd > i_MaxFd)
            {
              i_MaxFd = i_Fd;
            }
            /*!
            @todo Generic poll event message support
            @code
            if (z_Data.lCount < ARR_SIZE(z_Data.zFdSet))
            {
              z_Data.zFdSet[z_Data.lCount].lFd = i_Fd;
              z_Data.zFdSet[z_Data.lCount].lEvents |= SLIM_POLLIN;
              ++z_Data.lCount;
            }
            @endcode
            */
            b_Signal = true;
          }
        }
        z_Data.z_OsParams.max_fd = i_MaxFd;

        if (b_Signal)
        {
          SLIM_OS_LOGV("Detected QMI event: %"PRIu32, pz_OsParams->q_MsgId);

          pz_Msg = slim_IpcAllocate(sizeof(z_Data));
          if (NULL != pz_Msg)
          {
            pz_Msg->q_MsgId = pz_OsParams->q_MsgId;
            memcpy((void*) pz_Msg->p_Data, &z_Data, sizeof(z_Data));

            return pz_Msg;
          }
        }
        pz_OsParams = SLIM_LINK_NEXT(pz_OsParams->z_Link);
      }
    }
  }
err:
  return NULL;
}

bool slim_OsParamsRegister
(
  qmi_csi_os_params *pz_QmiOsParams,
  slim_ThreadT *pz_Thread,
  uint32_t q_MsgId
)
{
  slim_OsParamsT *pz_OsParams;

  if (NULL == pz_QmiOsParams || NULL == pz_Thread || 0 == q_MsgId)
  {
    goto err;
  }
  pz_OsParams = malloc(sizeof(*pz_OsParams));
  if (NULL == pz_OsParams)
  {
    goto err;
  }

  pz_OsParams->pz_OsParams = pz_QmiOsParams;
  pz_OsParams->q_MsgId = q_MsgId;
  SLIM_LINK_INIT(pz_OsParams->z_Link);

  pthread_mutex_lock(&pz_Thread->z_Mutex);
  SLIM_LIST_ADD(pz_Thread->z_OsParams, pz_OsParams, z_Link);
  slim_ThreadWakeupUnsafe(pz_Thread);
  pthread_mutex_unlock(&pz_Thread->z_Mutex);

  SLIM_OS_LOGV("QMI event (%"PRIu32") registered for %s", q_MsgId,
      pz_Thread->z_Name);

  return true;

err:
  SLIM_OS_LOGE("QMI event error: %"PRIu32, q_MsgId);

  return false;
}

bool slim_OsParamsUnregister
(
  qmi_csi_os_params *pz_QmiOsParams,
  slim_ThreadT *pz_Thread
)
{
  slim_OsParamsT *pz_It;

  if (NULL == pz_QmiOsParams || NULL == pz_Thread)
  {
    goto err_0;
  }

  pthread_mutex_lock(&pz_Thread->z_Mutex);
  SLIM_LIST_FIND(pz_Thread->z_OsParams, pz_It, z_Link,
      pz_It->pz_OsParams == pz_QmiOsParams);
  if (NULL != pz_It)
  {
    SLIM_LIST_REMOVE(pz_Thread->z_OsParams, pz_It, z_Link);
    slim_ThreadWakeupUnsafe(pz_Thread);
  }
  pthread_mutex_unlock(&pz_Thread->z_Mutex);
  if (NULL == pz_It)
  {
    goto err_0;
  }

  SLIM_OS_LOGV("QMI event (%"PRIu32") unregistered from %s",
      pz_It->q_MsgId, pz_Thread->z_Name);

  free(pz_It);

  return true;
err_0:
  return false;
}

//! @}
