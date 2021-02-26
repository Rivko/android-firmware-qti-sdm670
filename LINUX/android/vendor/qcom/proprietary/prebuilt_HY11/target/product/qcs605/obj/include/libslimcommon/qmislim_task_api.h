#ifndef QMISLIM_TASK_API_H
#define QMISLIM_TASK_API_H

/**
 @file
 @brief Declaration of target integration facilities
 @ingroup slim_QS

 The file declares tasks for APSS/MPSS integration.
*/
/*
  Copyright (c) 2014-2015 Qualcomm Atheros, Inc.
  All Rights Reserved.
  Qualcomm Atheros Confidential and Proprietary.

 */
/* $Header: //components/rel/gnss8.mpss/8.5/gnss/slim/qmislim/common/inc/qmislim_task_api.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qmislim_common_api.h"
#include "slim_client_types.h"
#include "qmi_client.h"
#include "qmislim_qmi_service_api.h"
#include "qmislim_qmi_monitor_api.h"
#include "qmislim_provider_api.h"
#include "slim_task.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/**
 @brief Total maximum number of incoming QMI-SLIM connections
 @ingroup slim_QS
 */
#define SLIM_QS_CONNECTION_COUNT \
  (SLIM_QS_TEST_CONNECTION_COUNT + SLIM_QS_AP_CONNECTION_COUNT + \
   SLIM_QS_AP_CONNECTION_COUNT)

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
  @brief QMI Monitor Task

  QMI Monitor Task integrates general @ref slim_QSQmiMonitor functionality
  into Aries-enabled platform.

  QMI Monitor Task encapsulates instance of @ref slim_QSQmiMonitor and
  implements @ref slim_QSQmiMonitorCb interface.

  @sa slim_QSQmiMonitor
  @sa slim_QSQmiMonitorCb
  @ingroup slim_QSQmiMonitorTask
 */
typedef struct slim_QSQmiMonitorTaskS
{
  slim_QSQmiMonitorT             z_QmiMonitor;
  /**< @brief QMI Monitor */
  const slim_QSQmiMonitorCb_VtT *pz_QmiMonitorCb;
  /**< @brief Interface for QMI Monitor callback */
  uint32                         q_MessageThreadId;
  /**< Target thread identifier for IPC communication */
} slim_QSQmiMonitorTaskT;

/**
  @brief Inner callback for QMI-SLIM QMI Service Task

  This callback integrates QMI-SLIM QMI Service and SLIM Client within
  the task context.

  @sa slim_QSQmiSvcTaskT
  @ingroup slim_QSQmiSvcTask
 */
typedef struct slim_QSQmiSvcTaskSvcCbS
{
  const slim_QSQmiSvcCb_VtT *pz_Vt;
  /**< @brief Virtual function table pointer */
  uint32                 q_Handle;
  /**< @brief SLIM client handle to use */
  slimClientContextT     z_SlimContext;
  /**< @brief SLIM context */
  boolean                u_InUse;
  /**< @brief Flag if in use */
  boolean                u_Enabled;
  /**< @brief Flag if can be used.
   * When the object is not enabled, QMI-SLIM disables all calls to SLIM core.
   * Services become unavailable and no service notifications are given. */
  struct slim_QSQmiSvcTaskS *pz_Task;
  /**< @brief Owner task */
  slim_QSNodeT           e_PeerNode;
  /**< @brief Peer node identifier */
} slim_QSQmiSvcTaskSvcCbT;

/**
  @brief QMI Service Task

  QMI Service task integrates QMI-SLIM QMI Service part into SLIM core
  using SLIM Client API.

  The following operations are available to controller:
  - @ref slim_QSQmiSvcTask_Init
  - @ref slim_QSQmiSvcTask_Deinit
  - @ref slim_QSQmiSvcTask_HandleIpc

  @ingroup slim_QSQmiSvcTask
 */
typedef struct slim_QSQmiSvcTaskS
{
  slim_QSQmiSvcT z_QmiService;
  /**< @brief QMI-SLIM QMI Service implementation */
  qmi_csi_os_params z_OsParams;
  /**< @brief QMI CSI polling parameters */
  const slim_QSQmiSvcCbFactory_VtT *pz_ICbFactory;
  /**< @brief Callback interface for QMI-SLIM QMI service */

  slim_QSQmiSvcTaskSvcCbT z_SvcCbs[SLIM_QS_CONNECTION_COUNT];
  /**< @brief Client proxy for connections. */

  uint32   q_TaskId;
  /**< @brief Thread identifier */
} slim_QSQmiSvcTaskT;

/**
  @brief QMI-SLIM SLIM provider task structure

  This object integrates SLIM Core, QMI-SLIM provider implementation and platform
  IPC facilities.

  The following operations are available to controller:
  - @ref slim_QSProviderTask_Init
  - @ref slim_QSProviderTask_Deinit
  - @ref slim_QSProviderTask_HandleIpc
  - @ref slim_QSProviderTask_HandleTimer
  - @ref slim_QSProviderTask_CheckEventTimer
  - @ref slim_QSProviderTask_NewProvider

  @ingroup slim_QSProviderTask
 */
typedef struct slim_QSProviderTaskS
{
  slim_QSProviderCoreT         z_ProviderCore;
  /**< @brief Provider registry */
  os_TimerMsgType             *pz_EventTimer;
  /**< @brief Timer for transaction timeouts and retry operations */
  uint32                       q_ThreadId;
  /**< @brief Task thread identifier */
  uint32                       q_TimerId;
  /**< @brief Timer hint */
} slim_QSProviderTaskT;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @brief Initializes QMI-SLIM Provider Task

  @retval TRUE on success
  @retval FALSE on error
  @sa slim_QSProviderTask
  @ingroup slim_QSProviderTask
 */
boolean slim_QSProviderTask_Init
(
  slim_QSProviderTaskT *pz_Object,
  /**< [in] QMI-SLIM Provider Task object */
  slim_QSNodeT          e_Node,
  /**< [in] Local node type */
  uint32                q_ThreadId,
  /**< [in] Thread of SLIM Core */
  uint32                q_TimerId
  /**< [in] Timer number for timed operations */
);
/**
  @brief Destroys QMI-SLIM Provider Task

  @retval TRUE on success
  @retval FALSE on error
  @sa slim_QSProviderTask
  @ingroup slim_QSProviderTask
 */
boolean slim_QSProviderTask_Deinit
(
  slim_QSProviderTaskT *pz_Object
  /**< [in] QMI-SLIM Provider Task object */
);
/**
  @brief Starts QMI-SLIM provider task operations

  This call performs post-startup initialization of QMI-SLIM provider task
  facilities. In particular, this relates to QMI stack actions, as they
  may deadlock if called

  @retval TRUE on success
  @retval FALSE on error
  @sa slim_QSProviderTask
  @ingroup slim_QSProviderTask
 */
void slim_QSProviderTask_Start
(
  slim_QSProviderTaskT *pz_Object
  /**< [in] QMI-SLIM Provider Task object */
);
/**
  @brief Handles IPC message

  @retval TRUE on success
  @retval FALSE on error
  @sa slim_QSProviderTask
  @ingroup slim_QSProviderTask
 */
boolean slim_QSProviderTask_HandleIpc
(
  slim_QSProviderTaskT *pz_Object,
  /**< [in] QMI-SLIM Provider Task object */
  const os_IpcMsgType *pz_IpcMsgIn
  /**< [in] IPC message to handle */
);
/**
  @brief Handles timer message

  @retval TRUE on success
  @retval FALSE on error
  @sa slim_QSProviderTask
  @ingroup slim_QSProviderTask
 */
boolean slim_QSProviderTask_HandleTimer
(
  slim_QSProviderTaskT *pz_Object,
  /**< [in] QMI-SLIM Provider Task object */
  uint32 q_TimerId,
  /**< [in] Timer message identifier */
  uint32 q_TimerParam
  /**< [in] Timer parameter */
);
/**
  @brief Fires any timed events

  @sa slim_QSProviderTask
  @ingroup slim_QSProviderTask
 */
void slim_QSProviderTask_CheckEventTimer
(
  slim_QSProviderTaskT *pz_Object
  /**< [in] QMI-SLIM Provider Task object */
);
/**
  @brief Creates new provider

  @return New provider instance of NULL on error
  @sa slim_QSProviderTask
  @ingroup slim_QSProviderTask
 */
slim_ServiceProviderInterfaceType *slim_QSProviderTask_NewProvider
(
  slim_QSProviderTaskT *pz_Object,
  /**< [in] QMI-SLIM Provider Task object */
  slim_QSNodeT          e_Node
  /**< [in] Peer node type */
);

/**
  @brief Initializes QMI-SLIM QMI Service Task

  @retval TRUE  On success.
  @retval FALSE On error.

  @sa slim_QSQmiSvcTask
  @ingroup slim_QSQmiSvcTask
 */
boolean slim_QSQmiSvcTask_Init
(
  slim_QSQmiSvcTaskT *pz_Object,
  /**< [in] QMI-SLIM QMI Service Task object */
  slim_QSNodeT        e_Node,
  /**< [in] Local node type */
  uint32              q_TaskId
  /**< [in] Service task identifier */
);
/**
  @brief Destroys QMI-SLIM QMI Service Task

  @retval TRUE  On success.
  @retval FALSE On error.

  @sa slim_QSQmiSvcTask
  @ingroup slim_QSQmiSvcTask
 */
boolean slim_QSQmiSvcTask_Deinit
(
  slim_QSQmiSvcTaskT *pz_Object
  /**< [in] QMI-SLIM QMI Service Task object */
);
/**
  @brief Initializes QMI-SLIM QMI Service Task

  Task handles messages from two primary sources:
  - From SLIM Core - responses and indications
  - From QCSI framework - QMI events


  @retval TRUE  On success.
  @retval FALSE On error.

  @sa slim_QSQmiSvcTask
  @ingroup slim_QSQmiSvcTask
 */
boolean slim_QSQmiSvcTask_HandleIpc
(
  slim_QSQmiSvcTaskT  *pz_Object,
  /**< [in] QMI-SLIM QMI Service Task object */
  const os_IpcMsgType *pz_IpcMsgIn
  /**< [in] Message to handle */
);
/**
  @brief Enables or disables peer node handling according to mask.

  The function enables or disables peer node handling. Disabled nodes are
  still permitted to connect and invoke APIs, but never perform actual
  SLIM registration and do not receive SLIM events.

  @retval TRUE  On success.
  @retval FALSE On error.

  @sa slim_QSQmiSvcTask
  @sa slim_QSNodeT
  @ingroup slim_QSQmiSvcTask
 */
boolean slim_QSQmiSvcTask_SetPeerMask
(
  slim_QSQmiSvcTaskT  *pz_Object,
  /**< [in] QMI-SLIM QMI Service Task object */
  uint32               q_Mask
  /**< [in] Peer mask, where 1 indicates peer is enabled.
   * @sa slim_QSNodeT */
);

/**
  @brief Initializes task

  @retval TRUE  On success.
  @retval FALSE On error.

  @ingroup slim_QSQmiMonitorTask
  @sa slim_QSQmiMonitorTask
 */
boolean slim_QSQmiMonitorTask_Init
(
  slim_QSQmiMonitorTaskT *pz_Object,
  /**< [in] Task object */
  uint32                  q_MessageThreadId
  /**< [in] Identifier of the task, that receives monitor notifications */
);
/**
  @brief Starts monitor operation

  @retval TRUE  On success.
  @retval FALSE On error.

  @ingroup slim_QSQmiMonitorTask
  @sa slim_QSQmiMonitorTask
 */
boolean slim_QSQmiMonitorTask_Start
(
  slim_QSQmiMonitorTaskT *pz_Object
  /**< [in] Task object */
);
/**
  @brief Destroys task task

  @retval TRUE  On success.
  @retval FALSE On error.

  @ingroup slim_QSQmiMonitorTask
  @sa slim_QSQmiMonitorTask
 */
boolean slim_QSQmiMonitorTask_Deinit
(
  slim_QSQmiMonitorTaskT *pz_Object
  /**< [in] Task object */
);

/**
  @brief Instance of QMI-SLIM Provider Task
  @ingroup slim_QSProviderTask
  @sa slim_QSProviderTask
 */
extern slim_QSProviderTaskT slimz_QSProviderTask;
/**
  @brief Instance of QMI-SLIM QMI Monitor Task
  @ingroup slim_QSQmiMonitorTask
  @sa slim_QSQmiMonitorTask
 */
extern slim_QSQmiMonitorTaskT slimz_QSQmiMonitorTask;
/**
  @brief Instance of QMI-SLIM QMI Service Task
  @ingroup slim_QSQmiSvcTask
  @sa slim_QSQmiSvcTask
 */
extern slim_QSQmiSvcTaskT slimz_QSQmiSvcTask;

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* QMISLIM_TASK_API_H */
