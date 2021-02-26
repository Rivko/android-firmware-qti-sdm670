#ifndef SLIM_TASK_H
#define SLIM_TASK_H
/**
@file
@brief  Interface for SLIM owning task and SLIM module

  This interface is provided by SLIM for SLIM owning task. It contains
  the SLIM module initialization method as well as the IPC message
  handlers for requests targeted to SLIM service. It also contains the
  definition of SLIM state data that is stored in SLIM owning task.

@ingroup slim_CoreTaskApi
*/
/*
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

               Copyright (c) 2013-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
*/
/* $Header: //components/rel/gnss8.mpss/8.5/gnss/slim/common/core/inc/slim_task.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "slim_processor.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @addtogroup slim_CoreTaskApi
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/**
@brief Minimum IPC message number that shall be forwarded to SLIM.

SLIM processes messages that are in [#IPC_SLIM_TO_TASK_MIN;#IPC_SLIM_TO_TASK_MAX]
range.

@sa IPC_SLIM_TO_TASK_FIRST_QMISLIM IPC_SLIM_TO_TASK_MAX
@sa slim_TaskIpcHandler
*/
#define IPC_SLIM_TO_TASK_MIN           (9000)
/**
@brief Minimum IPC message number handled by QMI-SLIM sub-component of SLIM.

QMI-SLIM processes messages that are in
[#IPC_SLIM_TO_TASK_FIRST_QMISLIM;#IPC_SLIM_TO_TASK_MAX] range.

@sa IPC_SLIM_TO_TASK_MIN IPC_SLIM_TO_TASK_MAX
@sa slim_QSTaskIpcHandler
*/
#define IPC_SLIM_TO_TASK_FIRST_QMISLIM (9900)
/**
@brief Maximum IPC message number that shall be forwarded to SLIM.

SLIM processes messages that are in [#IPC_SLIM_TO_TASK_MIN;#IPC_SLIM_TO_TASK_MAX]
range.

@sa IPC_SLIM_TO_TASK_FIRST_QMISLIM IPC_SLIM_TO_TASK_MIN
@sa slim_TaskIpcHandler
*/
#define IPC_SLIM_TO_TASK_MAX           (9999)

/**
@brief Minimum identifier reserved for SLIM timers.

All timer messages in range [#IPC_SLIM_TIMER_ID_MIN;#IPC_SLIM_TIMER_ID_MAX]
shall be forwarded to SLIM by owner component.

@sa IPC_SLIM_TIMER_ID_MIN IPC_SLIM_TIMER_ID_FIRST_QMISLIM IPC_SLIM_TIMER_ID_MAX
@sa slim_TaskTimerHandler
*/
#define IPC_SLIM_TIMER_ID_MIN           (9000)
/**
@brief Minimum identifier reserved for QMI-SLIM timers.

All timer messages in range [#IPC_SLIM_TIMER_ID_FIRST_QMISLIM;
#IPC_SLIM_TIMER_ID_MAX] are handled by QMI-SLIM component.

@sa IPC_SLIM_TIMER_ID_MIN IPC_SLIM_TIMER_ID_FIRST_QMISLIM IPC_SLIM_TIMER_ID_MAX
@sa slim_QSTaskTimerHandler
*/
#define IPC_SLIM_TIMER_ID_FIRST_QMISLIM (9900)
/**
@brief Maximum identifier reserved for SLIM timers.

All timer messages in range [#IPC_SLIM_TIMER_ID_MIN;#IPC_SLIM_TIMER_ID_MAX]
shall be forwarded to SLIM by owner component.

@sa IPC_SLIM_TIMER_ID_MIN IPC_SLIM_TIMER_ID_FIRST_QMISLIM IPC_SLIM_TIMER_ID_MAX
@sa slim_TaskTimerHandler
*/
#define IPC_SLIM_TIMER_ID_MAX           (9999)

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
@brief Enumeration for SLIM IPC message ids.

The enumeration maps all supported IPC message identifiers into a single
enumeration type.

All SLIM messages shall be within range [#IPC_SLIM_TO_TASK_MIN;
#IPC_SLIM_TO_TASK_MAX].

QMI-SLIM messages shall be within range [#IPC_SLIM_TO_TASK_FIRST_QMISLIM;
#IPC_SLIM_TO_TASK_MAX].

@sa IPC_SLIM_TO_TASK_MIN IPC_SLIM_TO_TASK_FIRST_QMISLIM IPC_SLIM_TO_TASK_MAX
*/
typedef enum
{
  /* SLIM task IPC message identifiers */
  M_IPC_SLIM_TO_TASK_OPEN_REQ_0 = IPC_SLIM_TO_TASK_MIN,
  M_IPC_SLIM_TO_TASK_GENERIC_REQ_0, /* Obsolete */
  M_IPC_SLIM_TO_TASK_ENABLE_SENSOR_DATA_REQ_0,
  M_IPC_SLIM_TO_TASK_ENABLE_MOTION_DATA_REQ_0,
  M_IPC_SLIM_TO_TASK_ENABLE_PEDOMETER_REQ_0,
  M_IPC_SLIM_TO_TASK_ENABLE_QMD_DATA_REQ_0,
  M_IPC_SLIM_TO_TASK_ENABLE_SMD_DATA_REQ_0,
  M_IPC_SLIM_TO_TASK_ENABLE_DISTANCE_BOUND_REQ_0,
  M_IPC_SLIM_TO_TASK_SET_DISTANCE_BOUND_REQ_0,
  M_IPC_SLIM_TO_TASK_ENABLE_VEHICLE_DATA_REQ_0,
  M_IPC_SLIM_TO_TASK_ENABLE_PED_ALIGNMENT_REQ_0,
  M_IPC_SLIM_TO_TASK_PROVIDER_MSG_0,
  M_IPC_SLIM_TO_TASK_ENABLE_MAG_FIELD_DATA_REQ_0,
  M_IPC_SLIM_TO_TASK_GET_PROVIDER_TIME_REQ_0, /* Obsolete */
  M_IPC_SLIM_TO_TASK_CONFIGURATION_CHANGE_REQ_0,
  M_IPC_SLIM_TO_TASK_CLOSE_REQ_0,
  M_IPC_SLIM_TO_TASK_GET_DISTANCE_BOUND_REQ_0,

  /* Add new SLIM IPCs above this line */

  /* QMI-SLIM IPC message identifiers */
  M_IPC_SLIM_QS_TO_TASK_QMI_SERVICE_INC_0 = IPC_SLIM_TO_TASK_FIRST_QMISLIM,
  /**< New QMI service registration event */
  M_IPC_SLIM_QS_TO_TASK_QMI_SERVICE_DEC_0,
  /**< QMI service drop event */
  M_IPC_SLIM_QS_TO_TASK_QCSI_SIGNAL_MSG_0,
  /**< QCSI event notification */
  M_IPC_SLIM_QS_TO_TASK_SLIM_CALLBACK_MSG_0,
  /**< Message from SLIM to QMI-SLIM service using from callback interface */

  /* Add new QMI-SLIM IPCs above this line */

  /* Other values */
  //! @cond
  M_IPC_SLIM_TO_TASK_MAX = 2147483647 /* To force a 32 bit signed enum.
                                         Do not change or use */
  //! @endcond
} slim_IpcMessageCmdIdEnumType;

/**
@brief Enum for SLIM timer identifiers.

The enumeration maps all supported timer identifiers into a single enumeration
type.

All SLIM timers shall be within range [@ref IPC_SLIM_TIMER_ID_MIN; @ref
IPC_SLIM_TIMER_ID_MAX].

QMI-SLIM timers shall be within range [@ref IPC_SLIM_TIMER_ID_FIRST_QMISLIM;
@ref IPC_SLIM_TIMER_ID_MAX].
*/
typedef enum
{
  M_IPC_SLIM_TX_TIMER_ID = IPC_SLIM_TIMER_ID_MIN,
  /**< Timer for use SLIM for timeout controls */
  M_IPC_SLIM_QS_TIMER_ID = IPC_SLIM_TIMER_ID_FIRST_QMISLIM,
  /**< Timer used by QMI-SLIM provider and sub-components for timed operations */
  //! @cond
  M_IPC_SLIM_TIMER_ID_MAX = 2147483647 /* To force a 32 bit signed enum.
                                          Do not change or use */
  //! @endcond
} slim_IpcTimerIdEnumType;

/** Enum for SLIM IPC messages
*/
typedef enum
{
  IPC_SLIM_TO_TASK_NONE = 0,
  /**< Invalid */

  IPC_SLIM_TO_TASK_OPEN_REQ                   = M_IPC_SLIM_TO_TASK_OPEN_REQ_0,
  /**< IPC id for open request */

  IPC_SLIM_TO_TASK_ENABLE_SENSOR_DATA_REQ     = M_IPC_SLIM_TO_TASK_ENABLE_SENSOR_DATA_REQ_0,
  /**< IPC id for enabling sensor reporting request */

  IPC_SLIM_TO_TASK_ENABLE_MOTION_DATA_REQ     = M_IPC_SLIM_TO_TASK_ENABLE_MOTION_DATA_REQ_0,
  /**< IPC id for enabling motion data reporting request */

  IPC_SLIM_TO_TASK_ENABLE_PEDOMETER_REQ       = M_IPC_SLIM_TO_TASK_ENABLE_PEDOMETER_REQ_0,
  /**< IPC id for enabling pedometer reporting request */

  IPC_SLIM_TO_TASK_ENABLE_QMD_DATA_REQ        = M_IPC_SLIM_TO_TASK_ENABLE_QMD_DATA_REQ_0,
  /**< IPC id for enabling QMD data reporting request */

  IPC_SLIM_TO_TASK_ENABLE_SMD_DATA_REQ        = M_IPC_SLIM_TO_TASK_ENABLE_SMD_DATA_REQ_0,
  /**< IPC id for enabling SMD data reporting request */

  IPC_SLIM_TO_TASK_ENABLE_DISTANCE_BOUND_REQ  = M_IPC_SLIM_TO_TASK_ENABLE_DISTANCE_BOUND_REQ_0,
  /**< IPC id for enabling distance bound reporting request */

  IPC_SLIM_TO_TASK_SET_DISTANCE_BOUND_REQ     = M_IPC_SLIM_TO_TASK_SET_DISTANCE_BOUND_REQ_0,
  /**< IPC id for distance bound set request */

  IPC_SLIM_TO_TASK_ENABLE_VEHICLE_DATA_REQ    = M_IPC_SLIM_TO_TASK_ENABLE_VEHICLE_DATA_REQ_0,
  /**< IPC id for enabling vehicle data reporting request */

  IPC_SLIM_TO_TASK_ENABLE_PED_ALIGNMENT_REQ   = M_IPC_SLIM_TO_TASK_ENABLE_PED_ALIGNMENT_REQ_0,
  /**< IPC id for enabling pedestrian alignment data reporting request */

  IPC_SLIM_TO_TASK_PROVIDER_MSG   = M_IPC_SLIM_TO_TASK_PROVIDER_MSG_0,
  /**< IPC id for provider use */

  IPC_SLIM_TO_TASK_ENABLE_MAG_FIELD_DATA_REQ  = M_IPC_SLIM_TO_TASK_ENABLE_MAG_FIELD_DATA_REQ_0,
  /**< IPC id for enabling magnetic field data reporting request */

  IPC_SLIM_TO_TASK_CONFIGURATION_CHANGE_REQ = M_IPC_SLIM_TO_TASK_CONFIGURATION_CHANGE_REQ_0,
  /**< IPC id for configuration change request */

  IPC_SLIM_TO_TASK_CLOSE_REQ                = M_IPC_SLIM_TO_TASK_CLOSE_REQ_0,
  /**< IPC message close request payload */

  IPC_SLIM_TO_TASK_GET_DISTANCE_BOUND_REQ   = M_IPC_SLIM_TO_TASK_GET_DISTANCE_BOUND_REQ_0,
  /**< IPC id for distance bound get request */

  IPC_SLIM_QS_TO_TASK_QMI_SERVICE_INC = M_IPC_SLIM_QS_TO_TASK_QMI_SERVICE_INC_0,
  /**< IPC id for QMI-SLIM service discovery */

  IPC_SLIM_QS_TO_TASK_QMI_SERVICE_DEC = M_IPC_SLIM_QS_TO_TASK_QMI_SERVICE_DEC_0,
  /**< IPC id for QMI-SLIM service drop */

  IPC_SLIM_QS_TO_TASK_QCSI_SIGNAL_MSG =  M_IPC_SLIM_QS_TO_TASK_QCSI_SIGNAL_MSG_0,
  /**< IPC id for QMI CSI service message (new CSI event pending) */

  IPC_SLIM_QS_TO_TASK_SLIM_CALLBACK_MSG = M_IPC_SLIM_QS_TO_TASK_SLIM_CALLBACK_MSG_0,
  /**< IPC id for message from SLIM to QMI-SLIM service */

  IPC_SLIM_TO_TASK_MAXIMUM                    = 2147483647
  /**< To force a 32 bit signed enum. Do not change or use */
} slim_IpcMessageCmdEnumType;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
@brief SLIM module initialization.

This function initializes SLIM module. It should be called when SLIM owning
task is initialized. No SLIM services are available before this function
is called.
*/
void slim_TaskInit
(
  void
);

/**
@brief SLIM module post-start initialization

This function initializes SLIM module components that require IPC communication
facilities. The function shall be called after RCF startup is confirmed.
*/
void slim_TaskStart
(
  void
);

/**
@brief Handler for SLIM IPC messages.

This function should be called when SLIM owning task receives SLIM IPC message.

@param[in]  pz_IpcMsgIn Received IPC message.
*/
void slim_TaskIpcHandler
(
  const os_IpcMsgType *pz_IpcMsgIn
);

/**
@brief Handler for SLIM IPC timer messages.

This function should be called when SLIM owning task receives timer IPC message
with SLIM timer ID.

@param[in] q_TimerId    Timer message identifier.
@param[in] q_TimerParam Timer parameter.
*/
void slim_TaskTimerHandler
(
  uint32 q_TimerId,
  uint32 q_TimerParam
);

/**
@brief Sends IPC message to SLIM owning task.

Function is called under SLIM client task context to create an IPC message
and send it to the SLIM owning task.

@param[in] e_IpcMessageId IPC message id.
@param[in] p_Payload      Pointer to IPC payload.
@param[in] q_PayloadSize  Size of the IPC payload.
@return 0 if IPC is sent successfully.
*/
uint32 slim_SendIpcToTask
(
  slim_IpcMessageCmdEnumType e_IpcMessageId,
  const void *p_Payload,
  uint32 q_PayloadSize
);

/**
@brief Sends IPC message to SLIM owning task or executes it directly.

Function is called under SLIM client task context to create an IPC message
and send it to the SLIM owning task or execute it directly if the client
task context matches SLIM task context.

@param[in] e_IpcMessageId IPC message id.
@param[in] u_ForceSend    When TRUE message is sent even when client's task
                          context matches SLIM context.
@param[in] p_Payload      Pointer to IPC payload.
@param[in] q_PayloadSize  Size of the IPC payload.

@return 0 if IPC is sent successfully.
*/
uint32 slim_ForwardIpcToTask
(
  slim_IpcMessageCmdEnumType e_IpcMessageId,
  boolean u_ForceSend,
  const void *p_Payload,
  uint32 q_PayloadSize
);

/**
@brief Verifies if the current task context belongs to SLIM

@retval TRUE  If the current task context belongs to SLIM.
@retval FALSE If the current task context doesn't belong to SLIM.
*/
boolean slim_IsSlimTaskContext
(
);

//! @}

#ifdef __cplusplus
}
#endif

#endif /* #ifndef SLIM_TASK_H */

