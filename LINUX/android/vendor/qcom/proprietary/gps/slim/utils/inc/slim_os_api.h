/**
 * Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
**/
#ifndef SLIM_OS_API_H_INCLUDED
#define SLIM_OS_API_H_INCLUDED
/**
 * @file
 * @brief Platform OS API on top of LA SLIM
 *
 * This file contains definition of types and definitions merely compatible
 * with modem OS API, but implemented on top of Linux-Android SLIM integration
 * layer.
 * SLIM thread API provides services for thread management, messaging,
 * and timers.
 *
 * @sa mpss_os_api
 *
 * @sa slim_Thread
 * @sa slim_Timer
 * @sa slim_IPC
 */

/**
 * @defgroup slim SLIM
 * @brief Sensor Location Interface Manager
 *
 * SLIM provides core facilities for threading, mutexes, locks, times and
 * messaging.
 *
 * @{
 *
 * @defgroup slim_Thread Threading
 * @brief Thread management facilities.
 *
 * @defgroup slim_IPC Messaging
 * @brief Messaging facilities
 *
 * @defgroup slim_Timer Timers
 * @brief Event timers
 *
 * @defgroup slim_Log Logging
 * @brief Logging
 *
 * @defgroup mpss_os_api Modem OS API
 * @brief Modem OS API for modem code portability
 *
 * The current implementation provides matching types for locking, messaging
 * and timers. The actual implementation is based on top of SLIM API.
 *
 * The API is based on top of Posix with some Linux extensions. Is is source
 * compatible with modem OS API.
 *
 * @}
 */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
/* Platform library headers */
#include <pthread.h>
#include <setjmp.h>
#include <time.h>

/* Library headers */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <qmi_idl_lib.h>
#include <qmi_csi.h>

/* Component headers */
#include <slim_list_api.h>

/* Additional headers */
#include <comdef.h>

#ifdef __cplusplus
extern "C" {
#endif

// #define SLIM_UNUSED(x)  ((void)(x))
//! @brief Macro for memory allocation function
#define slim_Malloc     malloc
//! @brief Macro for memory release function
#define slim_Free       free
//! @brief Macro for static inline
#define SLIM_INLINE    static inline

/**
 * @brief Registers native callback thread with SLIM framework
 *
 * The macro performs registration of native callback thread with SLIM
 * framework. This enables some of the messaging and debugging facilities.
 *
 * Each instance of the macro must be accompanied with \a SLIM_CB_CTX_POP()
 * counterpart.
 *
 * @param[in] z_Name Name of the context
 *
 * @sa SLIM_CB_CTX_POP
 * @ingroup slim_Thread
 */
#define SLIM_CB_CTX_PUSH(z_Name) \
{ \
  slim_ThreadT  __z_Thread; \
  bool       __b_ThreadAttached = slim_ThreadAttach(&__z_Thread, z_Name); \

/**
 * @brief Restores native thread context
 *
 * This macro must accompany \a SLIM_CB_CTX_PUSH
 *
 * @sa SLIM_CB_CTX_PUSH
 * @ingroup slim_Thread
 */
#define SLIM_CB_CTX_POP() \
  if (__b_ThreadAttached) slim_ThreadDetach(); \
}

/**
 * @brief buffer data related parameters
 */
#define BMI160_ACC_RESL (0.061)
#define CONVERT_ACC (0.0098) //library output is in mg = 0.0098 m/s^2
/* In NDK provider we need below conversion
* Once LA automotive uses new BMI HAL as LE, we can remove this logic
*/
#ifdef __ANDROID__
#define CONVERT_GYRO (0.000066605)
#else
#define CONVERT_GYRO (0.000066322) //125deg/Second Full scale configuration
#endif
// iio devices for buffed data read
#define ENABLE_ACCEL_BUFFER_DATA "/sys/bus/iio/devices/iio:device0/read_acc_boot_sample"
#define ACCEL_BUFF_FILE "bmi160_accbuf"

#define ENABLE_GYRO_BUFFER_DATA "/sys/bus/iio/devices/iio:device0/read_gyro_boot_sample"
#define GYRO_BUFF_FILE "bmi160_gyrobuf"
#define MAX_LENGTH  50
#define PATH "/sys/class/input/"
#define SNS_PATH "/sys/class/sensors/"
#define INPUT_DEV_PATH "/dev/input/"
/* sleep for 2 ms every batch of data sent to application */
#define TIME_BW_BUFFERED_DATA_BATCHES 2000
/* sleep value in nsec between retry attempts for service enablement status */
#define TIME_WAIT_FOR_SLIM_SERVICE 5000

#define CLOSE_FILE_HANDLE(fd) do {  \
  if (fd) {                         \
    fflush(fd);                     \
    fclose(fd);                     \
    fd = NULL;                      \
  }                                 \
} while(0)

/**
 * @brief Thread id numbers
 *
 * These numbers are used with SLIM IPC API for addressing target threads.
 * Changing these numbers is discouraged, however enumeration can be
 * extended.
 *
 * @ingroup slim_IPC
 */
typedef enum
{
  THREAD_ID_ERROR,
  /**< Thread id number when undefined */
  THREAD_ID_MAIN,
  /**< main application thread */
  THREAD_ID_SLIM = THREAD_ID_MAIN,
  /**< SLIM daemon thread */
  THREAD_ID_NDK,
  /**< SLIM NDK provider thread */
  THREAD_ID_SENSOR1,
  /**< SLIM Sensor1 provider thread */
  THREAD_ID_QMILOC,
  /**< SLIM QMI-LOC client thread */
  THREAD_ID_SLIM_API,
  /**< SLIM API RPC worker thread */
  THREAD_ID_VNW,
  /**< SLIM VNW provider thread */
  THREAD_ID_SENSOR_HAL,
  /**< SLIM Sensor HAL provider thread */
  THREAD_ID_SENSOR_HAL_BUFF,
  /**< SLIM_HAL buffer data handler thread */
  THREAD_ID_VNS,
  /**< SLIM VNS provider thread */
  THREAD_ID_SENSOR_HAL_TEMP_SENSOR,
  /**< SLIM Sensor HAL provider Temperature sensor thread */
  THREAD_ID_USER_FIRST = 0x100,
  /**< User-defined thread id */
} os_ThreadIdEnumType;

/**
 * @brief Target-specific message identifiers.
 */
typedef enum
{
  C_OS_MSG_ID_NONE,
  /**< Invalid message identifier */
  C_OS_MSG_ID_TIMER_EXPIRY,
  /**< Timer expiration message */
  C_OS_MSG_ID_QMI_EVENT,
  /**< QCSI event message */
  C_OS_MSG_ID_SIGINT,
  /**< OS signal */
  C_OS_MSG_ID_SIGUSR1,
  /**< OS user signal */
  C_OS_MSG_ID_SIGUSR2,
  /**< OS user signal */
  C_USR_MSG_ID_FIRST = 0x100,
  /**< User IPC message IDs shall not collapse into OS ones */
} os_MsgIdEnumType;


/**
 * @brief Message id for SIGINT signal
 * @ingroup slim_IPC
 */
#define M_IPC_MSG_SIGINT  ((uint32_t)C_OS_MSG_ID_SIGINT)
/**
 * Message id for SIGUSR1 signal
 * @ingroup slim_IPC
 */
#define M_IPC_MSG_SIGUSR1 ((uint32_t)C_OS_MSG_ID_SIGUSR1)
/**
 * Message id for SIGUSR2 signal
 * @ingroup slim_IPC
 */
#define M_IPC_MSG_SIGUSR2 ((uint32_t)C_OS_MSG_ID_SIGUSR2)
/**
 * Message id for timer message
 * @ingroup slim_IPC
 */
#define M_IPC_MSG_TIMER   ((uint32_t)C_OS_MSG_ID_TIMER_EXPIRY)
/**
 * Message id for QMI stack message
 * @ingroup slim_IPC
 */
#define M_IPC_MSG_QMI     ((uint32_t)C_OS_MSG_ID_QMI_EVENT)
/**
 * Message id for task initialization done message
 * @ingroup slim_IPC
 */
#define M_IPC_MSG_ACK     ((uint32_t)C_OS_MSG_ID_SIGUSR2 + 1)

/**
 * Internal Posix RT signal number for use with timers
 * @ingroup slim_Timer
 */
#define SLIM_SIG_TIMER (SIGRTMIN)

/** @copydoc slim_IpcMsgS */
typedef struct slim_IpcMsgS   slim_IpcMsgT;
/** @copydoc slim_ThreadS */
typedef struct slim_ThreadS   slim_ThreadT;
/** @copydoc slim_TimerS */
typedef struct slim_TimerS    slim_TimerT;
/** @copydoc slim_TimerMsgS */
typedef struct slim_TimerMsgS slim_TimerMsgT;
/** @copydoc slim_OsParamsS */
typedef struct slim_OsParamsS slim_OsParamsT;
/** @copydoc slim_QCSIMsgS */
typedef struct slim_QCSIMsgS  slim_QCSIMsgT;

/*!
 * @brief IPC message allocation type
 *
 * This constant is used with #os_IpcCreate method.
 *
 * @sa os_IpcCreate
 * @ingroup mpss_os_api
 */
#define IPC_ALLOCATION_DYNAMIC  ((uint32_t)0)

//! @addtogroup slim_Thread
//! @{

/**
 * @brief Function type for SLIM thread definitions
 *
 * @param[in] p_Arg User-specified parameter
 *
 * @sa slim_ThreadCreate()
 */
typedef void slim_ThreadMainFn(void *p_Arg);

#if defined(__ANDROID__) && !defined(__LP64__)
/*
Custom implementation for signal functions in SLIM.
See comments in: bionic/libc/private/kernel_sigset_t.h

Android ARM/x86 ABI has a signal mask of 32-bit, while kernel supports 64.
To enable RT signal operation, a usage of @ref syscall is required with
functions that operate against sigset_t.
*/
typedef struct slim_sigset
{
  unsigned long qMask[_NSIG / LONG_BIT];
} slim_sigset_t;
/** @sa sigemptyset */
int slim_sigemptyset
(
  slim_sigset_t *pzSigSet
);
/** @sa sigaddset */
int slim_sigaddset
(
  slim_sigset_t *pzSigSet,
  int lSigNo
);
/** @sa sigismember */
int slim_sigismember
(
  const slim_sigset_t *pzSigSet,
  int lSigNo
);
/** @sa pthread_sigmask */
int slim_sigmask
(
  int lAction,
  const slim_sigset_t *pzSigSet,
  slim_sigset_t *pzSigSetOut
);
/** @sa signalfd */
int slim_signalfd
(
  int lFd,
  const slim_sigset_t *pzSigMask,
  int lFlags
);
#else
/*
If the platform has proper signals implementation, map SLIM API to platform
functions.
*/
//! @brief Signal mask
#define slim_sigset_t    sigset_t
//! @brief Clear signal mask macro
#define slim_sigemptyset sigemptyset
//! @brief Add signal to mask macro
#define slim_sigaddset   sigaddset
//! @brief Test signal from mask macro
#define slim_sigismember sigismember
//! @brief Signal descriptor for poll operations
#define slim_signalfd    signalfd
//! @brief Thread signal mask set macro
#define slim_sigmask     pthread_sigmask
#endif

//! @}

//! @brief Thread abort
#define SLIM_THREAD_FLAG_ABORT     0x01
//! @brief Thread ready
#define SLIM_THREAD_FLAG_READY_ACK 0x02

/**
 * @brief SLIM thread object
 *
 * This structure contains fields used by SLIM framework.
 *
 * @sa slim_ThreadCreate()
 * @sa slim_ThreadExit()
 * @sa slim_ThreadCancel()
 * @sa slim_ThreadAttach()
 * @sa slim_ThreadDetach()
 * @sa slim_ThreadSelf()
 * @ingroup slim_Thread
 */
struct slim_ThreadS
{
  SLIM_LINK(slim_ThreadT,z_Link);
  /**< @brief Link for thread registration list
   *
   * All threads are registered in a global list.
   */
  SLIM_LIST(slim_IpcMsgT,z_IpcQueue);
  /**< @brief List of enqueued but unprocessed messages
   *
   * Messages are added into this list by @ref slim_IpcSend and
   * removed by @ref slim_IpcReceive.
   */
  SLIM_LIST(slim_OsParamsT,z_OsParams);
  /**< @brief List of QMI CSI poll objects
   * This list contains a list of CSI OS parameters for polling.
   */

  pthread_t        z_Thread;
  /**< @brief Thread handle */
  pid_t            z_Tid;
  /**< @brief Linux-specific thread id for signaling */
  pthread_mutex_t  z_Mutex;
  /**< @brief Thread state lock object */
  volatile bool b_EventState;
  /**< @brief Thread state
   *
   * When the value is TRUE, the thread is in event waiting state and shall
   * be waken up for event processing */
  int              i_EventFd;
  /**< @brief Event descriptor
   *
   * Event descriptor is used for waking up thread
   * @see slim_IpcReceive
   * @see slim_IpcSend
   */
  int              i_SignalFd;
  /**< @brief Signal descriptor
   *
   * Signal descriptor is used for receiving OS signals, including:
   * - SIGINT -- Cancel request
   * - SIGUSR1 -- User signal
   * - SIGUSR2 -- User signal
   * - SLIM_SIG_TIMER -- Timer signal
   */
  char             z_Name[8];
  /**< @brief Thread name used by @ref slim_Log module */

  uint32_t           q_IpcId;
  /**< @brief IPC identifier of the thread.
   *
   * By default threads do not have IPC enabled, To enable, user must call
   * @ref slim_IpcStart method and supplied IPC identifier is stored in this
   * field */

  slim_sigset_t    z_OldSigSet;
  /**< @brief Signal mask */
  jmp_buf          z_JmpBuf;
  /**< @brief Jump buffer for thread cancellation */
  slim_ThreadMainFn *pfn_Main;
  /**< @brief Main user function
   * @sa slim_ThreadStart */
  void              *p_Arg;
  /**< @brief Main user function argument
   * @sa slim_ThreadStart */

  /*!
   * @brief Thread lock counter
   *
   * Lock counter is used for preventing thread structure release when there is
   * at least one reference to the thread structure.
   *
   * @sa slim_ThreadPut
   */
  uint32_t           q_Cnt;
  /*!
   * @brief Flags
   */
  volatile uint32_t  q_Flags;
};

/**
 * @brief SLIM message
 *
 * The message object shall be constructed with call to \a slim_IpcAllocate()
 * method.
 *
 * @code{.c}
 * struct x_DataT z_MyData;
 * z_MyData = ...;
 * slim_IpcMsgT *pz_Msg = slim_IpcAllocate(sizeof(z_MyData));
 * if (NULL != pz_Msg)
 * {
 *   pz_Msg->q_MsgId = M_IPC_MSG_MY_DATA_MESSAGE;
 *   memcpy((void*)pz_Msg->p_Data, &z_MyData, sizeof(z_MyData));
 *   if (!slim_IpcSend(M_TARGET_THREAD_ID, pz_Msg))
 *   {
 *     slim_IpcDelete(pz_Msg);
 *   }
 * }
 * @endcode
 *
 * @sa slim_IpcAllocate()
 * @sa slim_IpcSend()
 * @sa slim_IpcReceive()
 * @sa slim_IpcDelete()
 * @sa slim_IpcSendEmpty()
 * @sa slim_IpcSendData()
 * @ingroup slim_IPC
 */
struct slim_IpcMsgS
{
  SLIM_LINK(slim_IpcMsgT,z_Link);
  /**< Field for internal message linking */
  uint32_t  q_MsgId;
  /**< Message identifier */
  uint32_t  q_SrcThreadId;
  /**< IPC identifier of the sender thread */
  uint32_t  q_DestThreadId;
  /**< IPC identifier of the destination thread */
  uint32_t  q_Size;
  /**< Payload size */
  uint8_t   p_Data[];
  /**< Payload pointer */
};

/**
 * @brief Timer event message
 *
 * Message sent by expired timer. The application code must be prepared to
 * receive late timer message, i.e. if the timer has been expired right before
 * cancellation.
 *
 * @sa slim_TimerStart()
 * @sa slim_IpcReceive()
 * @ingroup slim_Timer
 */
struct slim_TimerMsgS
{
  uint32_t  q_TimerId;
  /**< @brief Identifier of timer that has generated this message */
  uint32_t  q_Param;
  /**< @brief User-supplied parameter */
};


/**
 * @brief QMI CSI Event
 *
 * QMI CSI requires a subset of registration parameters for operation.
 *
 * @sa slim_OsParamsRegister
 * @sa slim_OsParamsUnregister
 * @ingroup slim_Thread
 */
struct slim_QCSIMsgS
{
   qmi_csi_os_params z_OsParams;
   /**< Structure for QCSI event processor */
   /** @todo refactor for handling generic poll event
    * @code
   struct
   {
     int lFd;
     int lEvents;
   } zFdSet[16];
   unsigned lCount;
   @endcode
   */
};

/**
 * @brief Timer type
 *
 * @sa slim_TimerCreate()
 * @sa slim_TimerStart()
 * @sa slim_TimerStop()
 * @sa slim_TimerDestroy()

 * @ingroup slim_Timer
 */
struct slim_TimerS
{
  SLIM_LINK(slim_TimerT,z_Link);
  /**< @brief Internal field for linking */
  uint64  t_ExpireTimeMs;
  /**< @brief Timer expiration time */
  uint32_t  q_TimerId;
  /**< @brief Timer identifier */
  uint32_t  q_Param;
  /**< @brief User-supplied timer parameter */
  uint32_t  q_DestThreadId;
  /**< @brief IPC identifier for timer messages */
  bool b_Started;
  /**< @brief Timer state */
  timer_t z_Timer;
  /**< @brief OS timer handle */
};

/**
 * @brief Structure for integration QMI CSI events into SLIM
 * @details This structure is used for QMI service framework integration. It
 * enables @ref slim to perform poll operations when thread state is suitable
 * for such operations.
 *
 * @sa slim_OsParamsRegister
 * @sa slim_OsParamsUnregister
 * @sa slim_IpcReceive
 *
 * @ingroup slim_Thread
 */
struct slim_OsParamsS
{
  SLIM_LINK(slim_OsParamsT,z_Link);
  /**< @brief Field for internal linking */
  qmi_csi_os_params *pz_OsParams;
  /**< @brief QMI registration parameters.
   * @details These parameters are used for registering QMI service. @ref slim
   * framework uses them for executing poll operations.
   */
  uint32_t            q_MsgId;
  /**< @brief IPC message identifier.
   * @details When there are pending operations, an IPC message is created with
   * given identifier.
   */
};

/*----------------------------------------------------------------------------
 * Thread functions.
 * -------------------------------------------------------------------------*/

//! @addtogroup slim_Thread
//! @{

/**
 * Creates new thread
 *
 * @param[in] pz_Thread Allocated thread buffer
 * @param[in] pfn_Main  Thread main function
 * @param[in] p_Arg     Argument for thread main function
 * @param[in] pz_Name   Thread name (up to 8 characters)
 *
 * @retval true  On success.
 * @retval false On error.
 */
bool slim_ThreadCreate
(
  slim_ThreadT      *pz_Thread,
  slim_ThreadMainFn *pfn_Main,
  void              *p_Arg,
  const char        *pz_Name
);
/**
 * @brief Find thread by IPC id.
 *
 * The function locates thread entry by IPC identifier. If found, the entry
 * reference counter is incremented and shall be released by #slim_ThreadPut.
 *
 * @param[in] q_IpcId IPC identifier
 *
 * @return Thread entry
 * @internal
 */
slim_ThreadT *slim_ThreadFind
(
  uint32_t q_IpcId
);
/**
 * @brief Provides current thread object
 *
 * @return Current thread object or NULL if the thread is not attached to SLIM
 */
slim_ThreadT *slim_ThreadSelf
(
  void
);
/**
 * @brief Request thread cancellation
 *
 * The method marks target thread as cancelled and returns. The target threaad
 * will terminate as soon as it reaches cancellation point.
 *
 * @retval TRUE Success
 * @retval FALSE Error
 */
bool slim_ThreadCancel
(
  slim_ThreadT *pz_Thread
  /**< [in] Target thread */
);

/*!
 * @brief Blocks current thread until another thread terminates.
 *
 * @param[in] pz_Thread Thread to await termination for.
 *
 * @retval true  On success.
 * @retval false On error.
 */
bool slim_ThreadJoin
(
  slim_ThreadT *pz_Thread
);

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
);
/**
 * @brief Removes current thread registration from SLIM framework
 *
 * @retval true  On success.
 * @retval false On error.
 *
 * @sa slim_ThreadAttach()
 */
bool slim_ThreadDetach
(
  void
);

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
);

/**
 * @brief Deinitializes main thread object @a main.
 */
bool slim_MainThreadDeinit
(
);

/**
 * @brief Registers QMI CSI polling data
 */
bool slim_OsParamsRegister
(
  qmi_csi_os_params *pz_Params,
  slim_ThreadT      *pz_Thread,
  uint32_t             q_SigNo
);
/**
 * @brief Unregisters QMI CSI polling data
 */
bool slim_OsParamsUnregister
(
  qmi_csi_os_params *pz_Params,
  slim_ThreadT      *pz_Thread
);

//! @}

/*----------------------------------------------------------------------------
 * SLIM IPC functions.
 * -------------------------------------------------------------------------*/

//! @addtogroup slim_IPC
//! @{

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
);
/**
 * @brief Allocates message with sufficiently large payload buffer
 *
 * @return Allocated message or NULL on error
 * @sa slim_IpcDelete
 * @sa slim_IpcSend
 */
slim_IpcMsgT *slim_IpcAllocate
(
  uint32_t q_Size
  /**< [in] Size of allocated payload buffer, can be 0 */
);
/**
 * @brief Releases IPC message
 *
 * Every IPC message must be released after processing, except for messages
 * that are entered into delivery queue with \a slim_IpcSend calls.
 *
 * @sa slim_IpcAllocate
 */
void slim_IpcDelete
(
  slim_IpcMsgT *pz_Msg
  /**< [in] IPC message object */
);
/**
 * @brief Schedules message for delivery
 * @retval TRUE message is scheduled
 * @retval FALSE message is not scheduled and must be released
 * @sa slim_IpcAllocate
 * @sa slim_IpcSendEmpty
 * @sa slim_IpcSendData
 */
bool slim_IpcSend
(
  uint32_t q_DestThreadId,
  /**< [in] IPC identifier of the destination thread */
  slim_IpcMsgT *pz_Msg
  /**< [in] Message structure with payload attached */
);
/**
 * @brief Blocks thread execution until IPC event comes
 *
 * The method can return when the following conditions are met:
 * - IPC message is received
 * - OS Signal is received
 *   - SIGINT
 *   - SIGUSR1
 *   - SIGUSR2
 * - Timer expiration signal is received
 * - QMI CSI event
 * - Supurios wakeup
 *
 * It the thread receives cancel request from @ref slim_ThreadCancel call, the
 * method never returns.
 *
 * @return IPC message or NULL (if spurios wakeup)
 * @retval NULL on interrupt
 */
slim_IpcMsgT *slim_IpcReceive
(
  void
);
/**
 * @brief Utility method for sending IPC message without payload
 *
 * The call is equivalent to:
 * @code{.c}
 * slim_IpcSendData(q_DestThreadId, q_MsgId, NULL, 0)
 * @endcode
 *
 * @retval TRUE on success
 */
bool slim_IpcSendEmpty
(
  uint32_t      q_DestThreadId,
  /**< [in] IPC thread ID for destination thread */
  uint32_t      q_MsgId
  /**< [in] Message identifier */
);
/**
 * @brief Utility method for sending IPC message with payload
 *
 * The call is equivalent to:
 * @code{.c}
 * {
 *   slim_IpcMsgT *pz_Msg = slim_IpcAllocate(q_DataSize);
 *   pz_Msg->q_MsgId = q_MsgId;
 *   slim_IpcSend(q_DestThreadId, pz_Msg);
 * }
 * @endcode
 *
 * @retval TRUE on success
 */
bool slim_IpcSendData
(
  uint32_t      q_DestThreadId,
  /**< [in] IPC thread ID for destination thread */
  uint32_t      q_MsgId,
  /**< [in] Message identifier */
  const void *p_Data,
  /**< [in] Pointer to payload, or NULL if \a q_DataSize is 0 */
  uint32_t      q_DataSize
  /**< [in] Size of the payload */
);

/**
 * @brief Notify control service that the task is ready
 *
 * @sa slim_TaskReadyWait
 */
void slim_TaskReadyAck();
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
);

//! @}

/*----------------------------------------------------------------------------
 * Timer functions.
 * -------------------------------------------------------------------------*/

//! @addtogroup slim_Timer
//! @{

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
);

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
);
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
);
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
);
/**
 * @brief Provides estimated timer event time
 *
 * @return Timer event time in milliseconds.
 */
uint32_t slim_TimerCheck
(
  slim_TimerT *pz_Timer
  /**< [in] Timer object */
);
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
);
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
);

//! @}

/*----------------------------------------------------------------------------
 * OS API functions.
 *
 * OS API functions use BSP types defined by comdef.h header and governed
 * by modem APIs for source level compatibility.
 * -------------------------------------------------------------------------*/

//! @addtogroup mpss_os_api
//! @{

/**
 * @brief Message type
 *
 * @sa slim_IpcMsgT
 */
typedef slim_IpcMsgT os_IpcMsgType;
/**
 * @brief Timer type
 *
 */
typedef slim_TimerT os_TimerMsgType;
/**
 * @brief Timer expiration message type
 */
typedef slim_TimerMsgT os_TimerExpiryType;

/*!
 * @brief Mutex context type
 *
 * Mutex context type is defined for modem OS compatibility.
 */
typedef enum
{
  MUTEX_ISR_CONTEXT,              //!< @brief lock between thread & ISR
  MUTEX_TASK_ONLY_CONTEXT,        //!< @brief lock between threads only
  MUTEX_DATA_ONLY_CONTEXT         //!< @brief use rex critical sections
} os_MutexEnumType;

/*!
 * @brief Memory scopes
 */
typedef enum
{
  OS_MEM_SCOPE_TASK,         //!< @brief Task (thread) scope
  OS_MEM_SCOPE_PROCESSOR,    //!< @brief Processor scope
  OS_MEM_SCOPE_INTERPROCESSOR//!< @brief Inter-processor scope
} os_MemScopeT;

/**
@brief Mutex structure

Reentrant mutex compartible with modem OS api layer.

@sa os_MutexEnumType
*/
typedef struct
{
  uint32            q_InitValue;  /*!< @brief Nonzero if mutex is initialized */
  os_MutexEnumType  e_MutexType;  /*!< @brief Mutex Type: one of three types */
  pthread_mutex_t   z_CritSect;   /*!< @brief Actual OS mutex implementation */
} os_MutexBlockType;


/**
 * @brief Creates IPC message
 *
 * @return Pointer to allocated message
 * @sa slim_IpcAllocate
 */
static inline os_IpcMsgType *os_IpcCreate
(
   uint32_t q_Size,
   /**< [in] Size of message payload in bytes */
   uint32_t q_DestThreadId,
   /**< [in] Assumed destination of the message.
    * Not used */
   uint32_t q_Type
   /**< [in] Type of the message.
    * Not used */
)
{
   (void)q_DestThreadId;
   (void)q_Type;
   return slim_IpcAllocate(q_Size);
}
/**
 * @brief Schedule message for delivery
 *
 * @retval TRUE on success
 * @retval FALSE on error. Caller must release message with @ref os_IpcDelete
 * @sa slim_IpcSend
 */
static inline boolean os_IpcSend
(
   os_IpcMsgType *pz_Msg,
   /**< [in] Message for delivery */
   uint32_t q_DestThreadId
   /**< [in] IPC identifier of the destination thread */
)
{
   return slim_IpcSend(q_DestThreadId, pz_Msg) ? TRUE : FALSE;
}

/*!
 * @brief Deletes message object
 *
 * @param[in] pz_Msg IPC message to delete.
 */
static inline void os_IpcDelete
(
   os_IpcMsgType *pz_Msg
)
{
   slim_IpcDelete(pz_Msg);
}

/*!
 * @brief Blocks execution until IPC event appears
 *
 * Blocks calling thread until any of IPC events become available.
 *
 * @return IPC event
 * @retval NULL on spurious wakeup.
 *
 * @sa slim_IpcReceive
 */
static inline os_IpcMsgType *os_IpcReceive
(
   void
)
{
   return slim_IpcReceive();
}

/*!
 * @brief Attaches current thread to IPC messaging
 *
 * @param[in] q_ThreadId IPC identifier.
 *
 * @retval true On success.
 */
static inline bool os_IpcStart
(
   uint32_t q_ThreadId
)
{
   return slim_IpcStart(q_ThreadId);
}
/*!
 * @brief Returns IPC identifier.
 *
 * @return IPC identifier for the current thread.
 */
static inline uint32_t os_CurrentThreadIdGet
(
  void
)
{
  slim_ThreadT *pz_Self = slim_ThreadSelf();
  if (NULL != pz_Self)
  {
    return pz_Self->q_IpcId;
  }
  return 0;
}

/**
 * @brief Initializes recursive mutex
 *
 * @retval TRUE on success
 * @retval FALSE on error
 * @sa pthread_mutex_init
 */
static inline boolean os_MutexInit
(
  os_MutexBlockType *pz_Mutex,
  /**< [in] mutex object */
  os_MutexEnumType e_Type
  /**< [in] Mutex type. Unused. */
)
{
   pthread_mutexattr_t z_Attr;

   pthread_mutexattr_init(&z_Attr);
#if defined(__ANDROID__)
   pthread_mutexattr_settype(&z_Attr, PTHREAD_MUTEX_RECURSIVE);
#else
   pthread_mutexattr_settype(&z_Attr, PTHREAD_MUTEX_RECURSIVE_NP);
#endif
   pz_Mutex->q_InitValue = 1;
   pz_Mutex->e_MutexType = e_Type;
   pthread_mutex_init(&pz_Mutex->z_CritSect, &z_Attr);
   pthread_mutexattr_destroy(&z_Attr);

   return TRUE;
}

/**
 * @brief Releases unused mutex
 *
 * This function doesn't have an alternative in original Aires OS API,
 * but is required for non-REX platforms.
 * @sa pthread_mutex_destroy
 */
static inline void os_MutexDeinit
(
  os_MutexBlockType *pz_Mutex
  /**< [in] Mutex to release */
)
{
   pthread_mutex_destroy(&pz_Mutex->z_CritSect);
   pz_Mutex->q_InitValue = 0;
}

/**
 * @brief Locks the mutex
 *
 * @retval TRUE on success
 * @retval FALSE on error
 * @sa pthread_mutex_lock
 */
static inline boolean os_MutexLock
(
  os_MutexBlockType *pz_Mutex
  /**< [in] Mutex to lock */
)
{
  return pthread_mutex_lock(&pz_Mutex->z_CritSect) == 0 ? TRUE : FALSE;
}

/**
 * @brief Releases the mutex
 * @retval TRUE on success
 * @retval FALSE on error
 * @sa pthread_mutex_unlock
 */
static inline boolean os_MutexUnlock
(
    os_MutexBlockType *pz_Mutex
   /**< [in] Mutex to unlock */
)
{
   return pthread_mutex_unlock(&pz_Mutex->z_CritSect) == 0 ? TRUE : FALSE;
}

/**
 * @brief Creates new timer
 */
static inline os_TimerMsgType *os_TimerCreate
(
   uint32 q_TimerId,
   /**< [in] Timer object */
   uint32 q_DestThreadId
   /**< [in] IPC identifier of thread that receives timer expiration messages */
)
{
   return slim_TimerCreate(q_TimerId, q_DestThreadId);
}
/**
 * @brief Starts (or restarts) the timer
 */
static inline boolean os_TimerStart
(
   os_TimerMsgType *pz_Timer,
   /**< [in] Timer object */
   uint32 q_Msecs,
   /**< [in] Time for expiration in milliseconds.
    * Must be greater than 0 */
   uint32 q_Param
   /**< [in] Additional parameter for expiration message.
    */
)
{
   return slim_TimerStart(pz_Timer, q_Msecs, q_Param) ? TRUE : FALSE;
}
/**
 * @brief Stops the timer
 */
static inline boolean os_TimerStop
(
   os_TimerMsgType *pz_Timer
   /**< [in] Timer object */
)
{
  return slim_TimerStop(pz_Timer) ? TRUE : FALSE;
}
/**
 * @brief Stops the timer and releases its resources
 */
static inline void os_TimerDestroy
(
   os_TimerMsgType *pz_Timer
   /**< [in] Timer object */
)
{
  slim_TimerDestroy(pz_Timer);
}

static inline void *os_MemAlloc
(
  size_t       sSize,
  os_MemScopeT eScope
)
{
  switch (eScope)
  {
  case OS_MEM_SCOPE_PROCESSOR:
  case OS_MEM_SCOPE_TASK:
    return malloc(sSize);
  case OS_MEM_SCOPE_INTERPROCESSOR:
  default:
    return NULL;
  }
}

static inline void os_MemFree
(
  void **ppData
)
{
  if (0 != ppData)
  {
    free(*ppData);
    *ppData = 0;
  }
}

static inline void *os_MemReAlloc
(
  void **ppData,
  size_t sSize,
  os_MemScopeT eScope
)
{
  if (0 != ppData)
  {
    switch (eScope)
    {
    case OS_MEM_SCOPE_PROCESSOR:
    case OS_MEM_SCOPE_TASK:
    {
      void *pTmp = realloc(*ppData, sSize);
      if (0 != sSize && 0 == pTmp)
      {
        return NULL;
      }
      *ppData = pTmp;
      return pTmp;
    }
    case OS_MEM_SCOPE_INTERPROCESSOR:
    default:
      return NULL;
    }
  }
  else
  {
    return 0;
  }
}

static inline void os_TaskReadyAck()
{
  slim_TaskReadyAck();
}
//! @}

#ifdef __cplusplus

}

/*!
 * @brief Root SLIM namespace
 */
namespace slim
{
class Mutex;
class MutexLock;
class Condition;

/**
 * @brief Generic Mutex class.
 *
 * Mutex class enables generic in-process locking technology.
 * @ingroup slim_Thread
 */
class Mutex
{
  friend class MutexLock;
  friend class Condition;
  pthread_mutex_t m_zMutex;
public:
  /**
   * @brief Constructs object
   *
   * @param[in] uRecursive When true, the mutex supports recursive locking.
   */
  Mutex
  (
    bool uRecursive = false
  );
  /** Object destructor */
  ~Mutex();
  /** Locks object */
  void lock();
  /** Unlocks object */
  void unlock();
};
/**
 * @brief Helper class for maintaining mutex lock
 *
 * The class locks mutex on construction and unlocks it when destroyed.
 * @ingroup slim_Thread
 */
class MutexLock
{
  pthread_mutex_t &m_zMutex;
public:
  /**
   * @brief Constructs lock object and locks given mutex.
   * @param[in] zMutex Mutex to lock
   */
  MutexLock
  (
    Mutex &zMutex
  );
  /**
   * @brief Constructs lock object and locks given mutex.
   * @param[in] zMutex Mutex to lock
   */
  MutexLock
  (
      pthread_mutex_t &zMutex
  );
  /**
   * @brief Unlocks mutex and destroys object.
   */
  ~MutexLock();
};
/**
 * @brief Condition variable class
 * @ingroup slim_Thread
 */
class Condition
{
  pthread_cond_t m_zCond;
public:
  //! Constructs condition variable object.
  Condition();
  //! Destroys condition variable object.
  ~Condition();
  /**
   * @brief Blocks execution until signaled
   * @param[in] zMutex Mutex for synchronization. Mutex shall be in locked
   *                   state and owned by the caller thread.
   */
  void wait
  (
    Mutex &zMutex
  );
  /**
   * @brief Signals to a single waiting thread
   */
  void signal();
  /**
   * @brief Signals to all waiting threads
   */
  void broadcast();
};
inline Mutex::Mutex
(
  bool uRecursive
)
{
  if (uRecursive)
  {
    pthread_mutexattr_t zAttr;
    pthread_mutexattr_init(&zAttr);
    pthread_mutexattr_settype(&zAttr, PTHREAD_MUTEX_RECURSIVE_NP);
    pthread_mutex_init(&m_zMutex, &zAttr);
    pthread_mutexattr_destroy(&zAttr);
  }
  else
  {
    pthread_mutex_init(&m_zMutex, 0);
  }
}
inline Mutex::~Mutex()
{
  pthread_mutex_destroy(&m_zMutex);
}
inline void Mutex::lock()
{
  pthread_mutex_lock(&m_zMutex);
}
inline void Mutex::unlock()
{
  pthread_mutex_unlock(&m_zMutex);
}
inline MutexLock::MutexLock
(
  Mutex &zMutex
) : m_zMutex(zMutex.m_zMutex)
{
  pthread_mutex_lock(&m_zMutex);
}
inline MutexLock::MutexLock
(
  pthread_mutex_t &zMutex
) : m_zMutex(zMutex)
{
  pthread_mutex_lock(&m_zMutex);
}
inline MutexLock::~MutexLock()
{
  pthread_mutex_unlock(&m_zMutex);
}
inline Condition::Condition()
{
  pthread_cond_init(&m_zCond, 0);
}
inline Condition::~Condition()
{
  pthread_cond_destroy(&m_zCond);
}
inline void Condition::wait
(
  Mutex &zMutex
)
{
  pthread_cond_wait(&m_zCond, &zMutex.m_zMutex);
}
inline void Condition::signal()
{
  pthread_cond_signal(&m_zCond);
}
inline void Condition::broadcast()
{
  pthread_cond_broadcast(&m_zCond);
}

} /* namespace slim */

#endif

#endif /* SLIM_OS_API_H_INCLUDED */
