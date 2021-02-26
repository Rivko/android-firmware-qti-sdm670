#ifndef QMISLIM_QMI_MONITOR_API_H
#define QMISLIM_QMI_MONITOR_API_H
/**
  @file
  @brief QMI-SLIM QMI Monitor API

  QMI Monitor is responsible for handling QMI service discovery operations.
  In particular, it is responsible for notifying clients whenever new service
  instance appears to be registered in QMI bus.

  Unfortunately QMI framework requires different approaches when working
  on different platform, in particular Linux/Android and REX. Because of that
  some of the operations must be implemented on integration layer.

  @ingroup slim_QSQmiMonitor

  Copyright (c) 2014 Qualcomm Atheros, Inc.
  All Rights Reserved.
  Qualcomm Atheros Confidential and Proprietary.

 */
/* $Header: //components/rel/gnss8.mpss/8.5/gnss/slim/qmislim/monitor/inc/qmislim_qmi_monitor_api.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qmislim_qmi_api.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
  @ingroup slim_QSQmiMonitor
 */
typedef struct slim_QSQmiMonitorS slim_QSQmiMonitorT;
/**
  @ingroup slim_QSQmiMonitorCb
 */
typedef struct slim_QSQmiMonitorCb_VtS slim_QSQmiMonitorCb_VtT;
/**
  @brief QMI Monitor callback interface.

  QMI Monitor uses this interface for delivering asynchronous events. The
  events are usually delivered by different thread that the one, which
  requested QMI monitor operation.

  @sa slim_QSQmiMonitorCb_VtS Virtual function table
  @ingroup slim_QSQmiMonitorCb
 */
typedef const slim_QSQmiMonitorCb_VtT * const slim_QSQmiMonitorCbT;

/**
  @brief QMI Monitor callback notification: service added

  QMI monitor performs this call every time it detects a service
  has been registered.

  @param[in] pz_Iface Callback object instance

  @sa slim_QSQmiMonitorCbT
  @ingroup slim_QSQmiMonitorCb
 */
typedef void slim_QSQmiMonitorCb_ServiceAddedFn
(
  slim_QSQmiMonitorCbT *pz_Iface
);
/**
  @brief QMI Monitor callback notification: service removed

  QMI monitor performs this call every time it detects a service
  has been removed.

  @param[in] pz_Iface Callback object instance

  @sa slim_QSQmiMonitorCbT
  @ingroup slim_QSQmiMonitorCb
 */
typedef void slim_QSQmiMonitorCb_ServiceRemovedFn
(
  slim_QSQmiMonitorCbT *pz_Iface
);
/**
  @brief Notification of QMI monitor asynchronous release

  This method is called when QMI monitor completes asynchronous termination.

  @param[in] pz_Iface Callback object instance
  @ingroup slim_QSQmiMonitorCb
 */
typedef void slim_QSQmiMonitorCb_HandleReleasedFn
(
  slim_QSQmiMonitorCbT *pz_Iface
);

/**
  @brief Virtual function table for slim_QSQmiMonitorCbT interface
  @ingroup slim_QSQmiMonitorCb
 */
struct slim_QSQmiMonitorCb_VtS
{
  slim_QSQmiMonitorCb_ServiceAddedFn   *pfn_ServiceAdded;
  /*!< @brief Service added notification */
  slim_QSQmiMonitorCb_ServiceRemovedFn *pfn_ServiceRemoved;
  /*!< @brief Service removed notification */
  slim_QSQmiMonitorCb_HandleReleasedFn *pfn_HandleReleased;
  /*!< @brief Monitor released notification */
};

/**
  @brief QMI Monitor object

  QMI monitor interfaces with QMI client framework and provides
  callback notifications.

  @sa slim_QSQmiMonitorCbT Callback interface

  @sa slim_QSQmiMonitor_Init
  @sa slim_QSQmiMonitor_Deinit
  @sa slim_QSQmiMonitor_Start
  @sa slim_QSQmiMonitor_Stop
  @sa slim_QSQmiMonitor_SetCallback
  @ingroup slim_QSQmiMonitor
 */
struct slim_QSQmiMonitorS
{
  SLIM_QS_LINK(slim_QSQmiMonitorT, z_Link);
  /*!< @brief Link entry for container object (if any) */

  qmi_idl_service_object_type pz_QmiServiceObject;
  /*!< @brief QMI IDL service object, that corresponds to QMI-SLIM IDL */
  qmi_client_os_params        z_QmiOsParams;
  /*!< @brief QMI OS parameters for notify operation.
  *   This data type is platform-specific and requires custom initialization
  *   in user code. */
  qmi_client_type             pz_QmiHandle;
  /*!< @brief QMI client notifier handle */
  volatile boolean            b_Started;
  /*!< @brief Current monitor state
  * @sa slim_QSQmiMonitor_Start
  * @sa slim_QSQmiMonitor_Stop */

  slim_QSQmiMonitorCbT       *pz_Callback;
  /**< @brief User-supplied callback interface
  * @sa slim_QSQmiMonitor_SetCallback */

  slim_QSMutexT               z_Mutex;
  /*!< @brief OS-specific mutex for synchronizing threads */
};

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @brief Initializes object

  Method performs initial object initialization.

  @retval TRUE on success
  @retval FALSE on error
  @ingroup slim_QSQmiMonitor
 */
boolean slim_QSQmiMonitor_Init
(
  slim_QSQmiMonitorT  *pz_Object
  /**< [in] QMI monitor object */
);
/**
  @brief Destroys the object

  If the object is in started state, it is stopped.

  @retval TRUE on success
  @retval FALSE on error
  @ingroup slim_QSQmiMonitor
 */
boolean slim_QSQmiMonitor_Deinit
(
  slim_QSQmiMonitorT *pz_Object
  /**< [in] QMI monitor object */
);
/**
  @brief Opens QMI stack

  @retval TRUE on success
  @retval FALSE on error
  @ingroup slim_QSQmiMonitor
 */
boolean slim_QSQmiMonitor_Start
(
  slim_QSQmiMonitorT *pz_Object
  /**< Monitor object */
);
/**
  @brief Closes QMI stack

  @retval TRUE on success
  @retval FALSE on error
  @ingroup slim_QSQmiMonitor
 */
boolean slim_QSQmiMonitor_Stop
(
  slim_QSQmiMonitorT *pz_Object,
  /**< Monitor object */
  boolean             u_Sync
  /**< When @a TRUE performs synchronous stop, otherwise asynchronous */
);
/**
  @brief Sets callback interface

  QMI monitor will use provided interface for notifying when service list
  changes.

  @retval TRUE on success
  @retval FALSE on error
  @ingroup slim_QSQmiMonitor
 */
boolean slim_QSQmiMonitor_SetCallback
(
  slim_QSQmiMonitorT   *pz_Object,
  /**< [in] QMI monitor object */
  slim_QSQmiMonitorCbT *pz_Callback
  /**< [in] Callback interface object */
);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* QMISLIM_QMI_MONITOR_API_H */
