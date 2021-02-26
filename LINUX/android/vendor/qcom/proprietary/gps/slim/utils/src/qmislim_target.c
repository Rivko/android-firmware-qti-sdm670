/*
Copyright (c) 2014 Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/
/**
@file
@brief QMI-SLIM processor-specific utilities.
*/
#include <qmislim_target.h>
#include <slim_os_log_api.h>

#include <inttypes.h>
#include <string.h>

#undef SLIM_LOG_LOCAL_MODULE
//! @brief Log messages as HLOS module.
#define SLIM_LOG_LOCAL_MODULE SLIM_LOG_MOD_HLOS

/**
@brief Translates SLIM OS polling result message into QCSI container

@param[in]  pz_IpcMsgIn   IPC message with slim_QCSIMsgT payload.
@param[out] pzOsParamsOut QCSI container.
*/
void slim_QSOsParamsUnpackIpc
(
  const os_IpcMsgType *pz_IpcMsgIn,
  qmi_csi_os_params *pzOsParamsOut
)
{
  slim_QCSIMsgT z_Data;
  memcpy((void*)&z_Data, (void*)pz_IpcMsgIn->p_Data, sizeof(z_Data));
  *pzOsParamsOut = z_Data.z_OsParams;
}

boolean slim_QSMutexInit
(
  slim_QSMutexT *pz_Mutex
)
{
  boolean u_RetVal = FALSE;
  pthread_mutexattr_t z_Attr;

  pthread_mutexattr_init(&z_Attr);
#if defined(__ANDROID__)
  pthread_mutexattr_settype(&z_Attr, PTHREAD_MUTEX_RECURSIVE);
#else
  pthread_mutexattr_settype(&z_Attr, PTHREAD_MUTEX_RECURSIVE_NP);
#endif
  u_RetVal = (0 == pthread_mutex_init(pz_Mutex, &z_Attr));
  pthread_mutexattr_destroy(&z_Attr);

  return u_RetVal;
}

boolean slim_QSMutexDeinit
(
  slim_QSMutexT *pz_Mutex
)
{
  return 0 == pthread_mutex_destroy(pz_Mutex) ? TRUE : FALSE;
}

void slim_QSMutexLock
(
  slim_QSMutexT *pz_Mutex
)
{
  if (0 != pthread_mutex_lock(pz_Mutex))
  {
    SLIM_LOGE("Failed to lock mutex");
  }
}

void slim_QSMutexUnlock
(
  slim_QSMutexT *pz_Mutex
)
{
  if (0 != pthread_mutex_unlock(pz_Mutex))
  {
    SLIM_LOGE("Failed to unlock mutex");
  }
}
