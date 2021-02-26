/*
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/
/*!
 * @file
 * @brief Platform-specific definitions for SLIM component
 *
 * @ingroup slim_APSS
 */
#ifndef SLIM_QS_TARGET_H
#define SLIM_QS_TARGET_H

#include <string.h>

#include "slim_os_log_api.h"
#include "slim_os_api.h"

//! @addtogroup slim_APSS

#ifdef __cplusplus
extern "C" {
#endif

/**
 @brief Produces high-32 bits from a pointer value.
 @param[in] x Pointer
 @return 0 on modem, high-32 bits on 64-bit architectures.
 @sa SLIM_QS_LOW
 */
#define SLIM_QS_HIGH(x) ((uint32)((uint64)(uintptr_t)(x) >> 32))
/**
 @brief Produces low-32 bits from a pointer value.
 @param[in] x Pointer
 @return low-32 bits from a pointer value.
 @sa SLIM_QS_HIGH
 */
#define SLIM_QS_LOW(x)  ((uint32)(uintptr_t)(x))
/**
  @brief Logs QMI-SLIM message without parameters.

  @param[in] Level Message logging level.
  @param[in] pStr  Message text.
*/
#define SLIM_QS_MSG0(Level,pStr) SLIM_LOG0(SLIM_LOG_MOD_QS,Level,pStr)
/**
  @brief Logs QMI-SLIM message with a single parameter.

  @param[in] Level Message logging level.
  @param[in] pStr  Message text.
  @param[in] qA    Parameter casted to uint32.
*/
#define SLIM_QS_MSG1(Level, pStr, qA) \
    SLIM_LOG3(SLIM_LOG_MOD_QS,Level,pStr,qA,0,0)

/**
  @brief Logs QMI-SLIM message with two parameters.

  @param[in] Level Message logging level.
  @param[in] pStr  Message text.
  @param[in] qA    Parameter casted to uint32.
  @param[in] qB    Parameter casted to uint32.
*/
#define SLIM_QS_MSG2(Level, pStr, qA, qB) \
    SLIM_LOG3(SLIM_LOG_MOD_QS,Level,pStr,qA,qB,0)

/**
  @brief Logs QMI-SLIM message with three parameters.

  @param[in] Level Message logging level.
  @param[in] pStr  Message text.
  @param[in] qA    Parameter casted to uint32.
  @param[in] qB    Parameter casted to uint32.
  @param[in] qC    Parameter casted to uint32.
*/
#define SLIM_QS_MSG3(Level, pStr, qA, qB, qC) \
    SLIM_LOG3(SLIM_LOG_MOD_QS,Level,pStr,qA,qB,qC)

/**
  @brief Logs QMI-SLIM message without parameters with low priority.

  @param[in] pStr Message text.
*/
#define SLIM_QS_MSG0L(pStr) SLIM_QS_MSG0(SLIM_LOG_DEBUG, pStr)
/**
  @brief Logs QMI-SLIM message without parameters with medium priority.

  @param[in] pStr Message text.
*/
#define SLIM_QS_MSG0M(pStr) SLIM_QS_MSG0(SLIM_LOG_INFO, pStr)
/**
  @brief Logs QMI-SLIM message without parameters with high priority.

  @param[in] pStr Message text.
*/
#define SLIM_QS_MSG0H(pStr) SLIM_QS_MSG0(SLIM_LOG_WARNING, pStr)
/**
  @brief Logs QMI-SLIM message without parameters with error priority.

  @param[in] pStr Message text.
*/
#define SLIM_QS_MSG0E(pStr) SLIM_QS_MSG0(SLIM_LOG_ERROR, pStr)

/**
  @brief Logs QMI-SLIM message with a single parameter with low priority.

  @param[in] pStr  Message text.
  @param[in] qA    Parameter casted to uint32.
*/
#define SLIM_QS_MSG1L(pStr ,qA) SLIM_QS_MSG1(SLIM_LOG_DEBUG, pStr, qA)
/**
  @brief Logs QMI-SLIM message with a single parameter with medium priority.

  @param[in] pStr  Message text.
  @param[in] qA    Parameter casted to uint32.
*/
#define SLIM_QS_MSG1M(pStr, qA) SLIM_QS_MSG1(SLIM_LOG_INFO, pStr, qA)
/**
  @brief Logs QMI-SLIM message with a single parameter with high priority.

  @param[in] pStr  Message text.
  @param[in] qA    Parameter casted to uint32.
*/
#define SLIM_QS_MSG1H(pStr, qA) SLIM_QS_MSG1(SLIM_LOG_WARNING, pStr, qA)
/**
  @brief Logs QMI-SLIM message with a single parameter with error priority.

  @param[in] pStr  Message text.
  @param[in] qA    Parameter casted to uint32.
*/
#define SLIM_QS_MSG1E(pStr, qA) SLIM_QS_MSG1(SLIM_LOG_ERROR, pStr, qA)
/**
  @brief Logs QMI-SLIM message with two parameters with low priority.

  @param[in] pStr  Message text.
  @param[in] qA    Parameter casted to uint32.
  @param[in] qB    Parameter casted to uint32.
*/
#define SLIM_QS_MSG2L(pStr, qA, qB) SLIM_QS_MSG2(SLIM_LOG_DEBUG, pStr, qA, qB)
/**
  @brief Logs QMI-SLIM message with two parameters with medium priority.

  @param[in] pStr  Message text.
  @param[in] qA    Parameter casted to uint32.
  @param[in] qB    Parameter casted to uint32.
*/
#define SLIM_QS_MSG2M(pStr, qA, qB) SLIM_QS_MSG2(SLIM_LOG_INFO, pStr, qA, qB)
/**
  @brief Logs QMI-SLIM message with two parameters with high priority.

  @param[in] pStr  Message text.
  @param[in] qA    Parameter casted to uint32.
  @param[in] qB    Parameter casted to uint32.
*/
#define SLIM_QS_MSG2H(pStr, qA, qB) SLIM_QS_MSG2(SLIM_LOG_WARNING, pStr, qA, qB)
/**
  @brief Logs QMI-SLIM message with two parameters with error priority.

  @param[in] pStr  Message text.
  @param[in] qA    Parameter casted to uint32.
  @param[in] qB    Parameter casted to uint32.
*/
#define SLIM_QS_MSG2E(pStr, qA, qB) \
  SLIM_QS_MSG2(SLIM_LOG_ERROR, pStr, qA, qB)

/**
  @brief Logs QMI-SLIM message with three parameters with low priority.

  @param[in] pStr  Message text.
  @param[in] qA    Parameter casted to uint32.
  @param[in] qB    Parameter casted to uint32.
  @param[in] qC    Parameter casted to uint32.
*/
#define SLIM_QS_MSG3L(pStr, qA, qB, qC) \
  SLIM_QS_MSG3(SLIM_LOG_DEBUG, pStr, qA, qB, qC)
/**
  @brief Logs QMI-SLIM message with three parameters with medium priority.

  @param[in] pStr  Message text.
  @param[in] qA    Parameter casted to uint32.
  @param[in] qB    Parameter casted to uint32.
  @param[in] qC    Parameter casted to uint32.
*/
#define SLIM_QS_MSG3M(pStr, qA, qB, qC) \
  SLIM_QS_MSG3(SLIM_LOG_INFO, pStr, qA, qB, qC)
/**
  @brief Logs QMI-SLIM message with three parameters with high priority.

  @param[in] pStr  Message text.
  @param[in] qA    Parameter casted to uint32.
  @param[in] qB    Parameter casted to uint32.
  @param[in] qC    Parameter casted to uint32.
*/
#define SLIM_QS_MSG3H(pStr, qA, qB, qC) \
  SLIM_QS_MSG3(SLIM_LOG_WARNING, pStr, qA, qB, qC)
/**
  @brief Logs QMI-SLIM message with three parameters with error priority.

  @param[in] pStr  Message text.
  @param[in] qA    Parameter casted to uint32.
  @param[in] qB    Parameter casted to uint32.
  @param[in] qC    Parameter casted to uint32.
*/
#define SLIM_QS_MSG3E(pStr, qA, qB, qC) \
  SLIM_QS_MSG3(SLIM_LOG_ERROR, pStr, qA, qB, qC)

/*
  Execution context for logging. These macros are used for marking logging
  execution context scope on APSS platform. On MPSS the macros are not doing
  anything.
 */
/**
  @brief Execution context push macro

  The macro associates a name with a callback thread. The macro shall be
  accompanied with #SLIM_QS_CB_CTX_POP.

  On modem macro does nothing.

  @param[in] pName Thread context name.
  @sa SLIM_QS_CB_CTX_POP
*/
#define SLIM_QS_CB_CTX_PUSH(pName) SLIM_CB_CTX_PUSH(pName)
/**
  @brief Execution context pop macro

  Marks end of callback execution context.

  On modem macro does nothing.

  @sa SLIM_QS_CB_CTX_POP
*/
#define SLIM_QS_CB_CTX_POP() SLIM_CB_CTX_POP()

/* Target configuration constants */
/** @brief APSS service provider always uses AP node identifier */
#define SLIM_QS_THIS_NODE eSLIM_QS_NODE_AP

/* QMI stack integration */

/**
  @brief QCSI parameters initialization

  Prepares OS parameters for usage with QCSI framework. This operation
  is required on MP and not required on AP.

  @ingroup slim_QSQmi
  @sa slim_QSQmi
 */
#define SLIM_QS_OS_PARAMS_INIT(OsParams) \
   do \
   { \
      memset(&(OsParams), 0, sizeof(OsParams)); \
   } while(0)

/**
  @brief Register QCSI parameters for IPC messaging
  @ingroup slim_QSQmi
  @sa slim_QSQmi
 */
#define SLIM_QS_OS_PARAMS_ENABLE(OsParams) \
   slim_OsParamsRegister(\
       &(OsParams), slim_ThreadSelf(), IPC_SLIM_QS_TO_TASK_QCSI_SIGNAL_MSG)

/**
  @brief Unregister QCSI parameters from IPC messaging
  @ingroup slim_QSQmi
  @sa slim_QSQmi
 */
#define SLIM_QS_OS_PARAMS_DISABLE(OsParams) \
   slim_OsParamsUnregister(&(OsParams), slim_ThreadSelf())

/**
  @brief Translates QMI IPC message into QCSI OS parameters

  On REX the IPC message shall contain signal mask.

  On Linux/Windows IPC message shall contain @a qcsi_os_params contents.

  @ingroup slim_QSQmi
  @sa slim_QSQmi
 */
#define SLIM_QS_OS_PARAMS_UNPACK_IPC(zIpcMsgIn,zOsParamsOut) \
    slim_QSOsParamsUnpackIpc(&(zIpcMsgIn), &(zOsParamsOut))

/**
  @brief Initializes QCCI OS parameters

  @param[out] OsParams Parameters to initialize

  @ingroup slim_QSQmi
  @sa slim_QSQmi
 */
#define SLIM_QS_QCCI_PARAMS_INIT(OsParams) \
   do \
   { \
      memset(&(OsParams), 0, sizeof(OsParams)); \
   } while(0)

/**
  @brief Maximum number of QMI-SLIM provider objects (total)

  @ingroup slim_QSAlloc
 */
#define SLIM_QS_ALLOCATOR_PROVIDER_COUNT        3
/**
  @brief Maximum number of total opened connections

  Maximum number of total opened connections shall be a multiplication of
  total number of providers per total number of supported clients.

  @ingroup slim_QSAlloc
 */
#define SLIM_QS_ALLOCATOR_PROVIDER_CONN_COUNT   10
/**
  @brief Maximum number of opened transactions

  Each SLIM-Core request opens a transaction if the operation requires
  a response.

  @ingroup slim_QSAlloc
 */
#define SLIM_QS_ALLOCATOR_PROVIDER_TX_COUNT     30
/**
  @brief Maximum number of opened QMI-SLIM transactions

  @ingroup slim_QSAlloc
 */
#define SLIM_QS_ALLOCATOR_QMI_CLT_TX_COUNT      30
/**
  @brief Maximum number of QMI-SLIM service client connections

  @ingroup slim_QSAlloc
 */
#define SLIM_QS_ALLOCATOR_QMI_SVC_CLT_COUNT     5

/**
  @brief Maximum number of test connections

  @ingroup slim_QSAlloc
 */
#define SLIM_QS_TEST_CONNECTION_COUNT 2u
/**
  @brief Maximum number of AP connections

  @ingroup slim_QSAlloc
 */
#define SLIM_QS_AP_CONNECTION_COUNT   10u
/**
  @brief Maximum number of MP connections

  @ingroup slim_QSAlloc
 */
#define SLIM_QS_MP_CONNECTION_COUNT   1u

/*!
 * @brief Mutex type used by QMI-SLIM
 *
 * QMI-SLIM uses native mutex in reentrant mode.
 *
 * @sa slim_QSMutexInit
 * @sa slim_QSMutexDeinit
 * @sa slim_QSMutexLock
 * @sa slim_QSMutexUnlock
 */
typedef pthread_mutex_t slim_QSMutexT;

/*!
 * @brief Initializes QMI-SLIM mutex
 *
 * @param[out] pz_Mutex Mutex to initialize
 *
 * @retval TRUE On success.
 */
boolean slim_QSMutexInit
(
  slim_QSMutexT *pz_Mutex
);
/*!
 * @brief Releases QMI-SLIM mutex
 *
 * @param[out] pz_Mutex Mutex to release
 *
 * @retval TRUE On success.
 */
boolean slim_QSMutexDeinit
(
  slim_QSMutexT *pz_Mutex
);
/*!
 * @brief Locks QMI-SLIM mutex
 *
 * @param[in] pz_Mutex Mutex to lock
 */
void slim_QSMutexLock
(
  slim_QSMutexT *pz_Mutex
);
/*!
 * @brief Unlocks QMI-SLIM mutex
 *
 * @param[in] pz_Mutex Mutex to unlock
 */
void slim_QSMutexUnlock
(
  slim_QSMutexT *pz_Mutex
);

/**
@brief Translates SLIM OS polling result message into QCSI container

@param[in]  pz_IpcMsgIn   IPC message with slim_QCSIMsgT payload.
@param[out] pzOsParamsOut QCSI container.
*/
void slim_QSOsParamsUnpackIpc
(
  const os_IpcMsgType *pz_IpcMsgIn,
  qmi_csi_os_params *pzOsParamsOut
);

#ifdef __cplusplus
}
#endif

//! @}

#endif

