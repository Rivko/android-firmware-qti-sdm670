#ifndef QMISLIM_QMI_SERVICE_API_H
#define QMISLIM_QMI_SERVICE_API_H
/**
  @file
  @brief QMI-SLIM QMI Service API
  @ingroup slim_QSQmiSvc
*/
/*
Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.

  Copyright (c) 2014-2015 Qualcomm Atheros, Inc.
  All Rights Reserved.
  Qualcomm Atheros Confidential and Proprietary.
 */
/* $Header: //components/rel/gnss8.mpss/8.5/gnss/slim/qmislim/qmiservice/inc/qmislim_qmi_service_api.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qmislim_qmi_api.h"
#include "qmislim_qmi_dm_log_api.h"
#include "slim_client_types.h"

#include "qmi_idl_lib.h"
#include "qmi_csi.h"
#include "qmi_client.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

//! @addtogroup slim_QSQmiSvc
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/**
 * @brief Minimum number of client connection identifier.
 * @sa SLIM_QS_SVC_CLIENT_ID_MAX
 * @sa SLIM_QS_SVC_CLIENT_COUNT_MAX
 */
#define SLIM_QS_SVC_CLIENT_ID_MIN  1
/**
 * @brief Maximum number of client connection identifier.
 * @sa SLIM_QS_SVC_CLIENT_ID_MIN
 * @sa SLIM_QS_SVC_CLIENT_COUNT_MAX
 */
#define SLIM_QS_SVC_CLIENT_ID_MAX  65535

/**
 * @brief Maximum number of client connections.
 * @sa SLIM_QS_SVC_CLIENT_ID_MIN
 * @sa SLIM_QS_SVC_CLIENT_ID_MAX
 */
#define SLIM_QS_SVC_CLIENT_COUNT_MAX 16

/**
 * @brief Maximum encoded message length.
 *
 * This value shall be the maximum encoded message size in bytes. It shall
 * correlate with QMI IDL type library for QMI-SLIM.
 *
 * @sa slim_QSQmiSvcS
 */
#define SLIM_QS_SVC_ENCODED_MESSAGE_LEN_MAX 1300
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Class Definitions
 * -------------------------------------------------------------------------*/
//! @copydoc slim_QSQmiSvcCb_VtS
typedef const slim_QSQmiSvcCb_VtT * const slim_QSQmiSvcCbT;
//! @copydoc slim_QSQmiSvcCbFactory_VtS
typedef const slim_QSQmiSvcCbFactory_VtT * const slim_QSQmiSvcCbFactoryT;

/**
 * @brief QMI-SLIM QMI Service Object
 *
 * This object handles operations for QMI framework registration, serving
 * inbound QMI-SLIM connections and performing generic QMI service maintenance
 * tasks.
 *
 * For each opened connection, an instance of #slim_QSQmiSvcCltS object
 * is created, which actually handles all QMI messages.
 *
 * The service does not forward calls to SLIM directly, instead it uses
 * own callback interface @ref slim_QSQmiSvcCbT. This is done to decouple
 * implementation of SLIM core and service to enable test tools development.
 *
 * @sa slim_QSQmiSvc_Init             Constructor
 * @sa slim_QSQmiSvc_Deinit           Destructor
 * @sa slim_QSQmiSvc_Register         Registers QMI service
 * @sa slim_QSQmiSvc_Unregister       Unregisters QMI service
 * @sa slim_QSQmiSvc_HandleQmiEvent   Processes pending QMI events
 * @sa slim_QSQmiSvc_HandleSlimEvent  Processes inbound SLIM event
 *
 * @sa slim_QSQmiSvcCbFactory Callback factory interface
 * @sa slim_QSQmiSvcCb        Callback interface
 * @sa slim_QSQmiSvcClt       Client connection object
 * @ingroup slim_QSQmiSvc
 */
struct slim_QSQmiSvcS
{
  slim_QSNodeT e_Node;
  /**< @brief Type of the node for this service.
  The solution globally might have different services on the same or
  different processors (AP/MP/etc), but the node shall be non-matching.
  */
  SLIM_QS_LIST(slim_QSQmiSvcCltT,z_Clients);
  /**< List of QCCI connection objects */
  boolean                     b_Registered;
  /**< @brief Flag if the service is registered in QMI stack */
  qmi_service_instance        u_QmiServiceInstanceId;
  /**<
    @brief QMI service instance id to use with this service.

    QMI service instance corresponds to QMI-SLIM node type
    @sa slim_QSNodeT
  */
  qmi_idl_service_object_type pz_QmiServiceObject;
  /**< @brief QMI IDL service object */
  qmi_csi_service_handle      z_QmiServiceHandle;
  /**< @brief QMI service connection object */
  uint32                      q_ClientIdCounter;
  /**< @brief Counter for client connection enumeration */
  slim_QSQmiSvcCbFactoryT    *pz_CallbackFactory;
  /**<
    @brief Callback factory interface for integration

    This interface enables service integration into SLIM or testing framework
    */
  slim_QSLogT              z_DMLogger;
  /**< @brief DM logger object */


  /*--------------------------------------------------------------------------
   * QMI message encoding and decoding
   *------------------------------------------------------------------------*/
  /** @brief Buffer for message encoding and decoding */
  uint8                       pu_Buffer[SLIM_QS_SVC_ENCODED_MESSAGE_LEN_MAX];
};

/**
  @brief Constructs new service callback instance

  Callback instance is created when a new QMI client connection is established.
  The interface shall provide bridge to SLIM Client API.

  @sa slim_QSQmiSvc
  @sa slim_QSQmiSvcCb
  @sa slim_QSQmiSvcCbFactory
  @ingroup slim_QSQmiSvcCbFactory
 */
typedef slim_QSQmiSvcCbT *slim_QSQmiSvcCbFactory_NewCallbackFnT
(
  slim_QSQmiSvcCbFactoryT *pz_Object,
  /**< [in] Callback factory object */
  slim_QSNodeT             e_PeerNode
  /**< [in] Peer node type to create callback for */
);
/**
  @brief Releases callback

  Callback is released when QMI client connection is terminated.

  @ingroup slim_QSQmiSvcCbFactory
  @sa slim_QSQmiSvc
  @sa slim_QSQmiSvcCb
  @sa slim_QSQmiSvcCbFactory
 */
typedef void slim_QSQmiSvcCbFactory_DeleteCallbackFnT
(
  slim_QSQmiSvcCbFactoryT *pz_Object,
  /**< [in] Callback factory object */
  slim_QSQmiSvcCbT        *pz_Callback
  /**< [in] Callback object */
);

/**
  @brief Virtual function table for callback factory
  @ingroup slim_QSQmiSvcCbFactory
 */
struct slim_QSQmiSvcCbFactory_VtS
{
  slim_QSQmiSvcCbFactory_NewCallbackFnT    *pfn_NewCallback;
  /**< @brief Allocate new callback interface */
  slim_QSQmiSvcCbFactory_DeleteCallbackFnT *pfn_DeleteCallback;
  /**< @brief Release callback interface */
};

/**
  @brief Opens SLIM client connection
  @sa slim_Open()
  @ingroup slim_QSQmiSvcCb
 */
typedef slimErrorEnumT slim_QSQmiSvcCb_OpenFnT
(
  slim_QSQmiSvcCbT            *pz_Iface,
  /**< [in] Callback for using with SLIM */
  slimOpenFlagsT               q_OpenFlags,
  /**< [in] Open flags */
  uint32                       q_ClientId
  /**< [in] Client identifier within service context */
);
/**
  @brief Terminates SLIM client connection
  @sa slim_Close()
  @ingroup slim_QSQmiSvcCb
 */
typedef slimErrorEnumT slim_QSQmiSvcCb_CloseFnT
(
  slim_QSQmiSvcCbT *pz_Iface
  /**< [in] Interface object */
);
/** @sa slim_EnableSensorData
  @ingroup slim_QSQmiSvcCb
 */
typedef slimErrorEnumT slim_QSQmiSvcCb_EnableSensorDataFnT
(
  slim_QSQmiSvcCbT                         *pz_Iface,
  /**< [in] Interface object */
  const slimEnableSensorDataRequestStructT *pz_Request,
  /**< [in] Service request message  */
  uint8                                     u_TxnId
  /**< [in] Request transaction number */
);
/** @sa slim_EnablePedometer
  @ingroup slim_QSQmiSvcCb
 */
typedef slimErrorEnumT slim_QSQmiSvcCb_EnablePedometerFnT
(
  slim_QSQmiSvcCbT                        *pz_Iface,
  /**< [in] Interface object */
  const slimEnablePedometerRequestStructT *pz_Request,
  /**< [in] Service request message  */
  uint8                                    u_TxnId
  /**< [in] Request transaction number */
);
/** @sa slim_EnableQmdData
  @ingroup slim_QSQmiSvcCb
 */
typedef slimErrorEnumT slim_QSQmiSvcCb_EnableQmdDataFnT
(
  slim_QSQmiSvcCbT                      *pz_Iface,
  /**< [in] Interface object */
  const slimEnableQmdDataRequestStructT *pz_Request,
  /**< [in] Service request message  */
  uint8                                  u_TxnId
  /**< [in] Request transaction number */
);
/** @sa slim_EnableVehicleData
  @ingroup slim_QSQmiSvcCb
 */
typedef slimErrorEnumT slim_QSQmiSvcCb_EnableVehicleDataFnT
(
  slim_QSQmiSvcCbT                          *pz_Iface,
  /**< [in] Interface object */
  const slimEnableVehicleDataRequestStructT *pz_Request,
  /**< [in] Service request message  */
  uint8                                      u_TxnId
  /**< [in] Request transaction number */
);

/**
  @brief Virtual function table for QMI-SLIM QMI Service Callback.

  @sa #slim_QSQmiSvcCbT
  @ingroup slim_QSQmiSvcCb
 */
struct slim_QSQmiSvcCb_VtS
{
  slim_QSQmiSvcCb_OpenFnT                      *pfn_Open;
  /**< @brief Open connection */
  slim_QSQmiSvcCb_CloseFnT                     *pfn_Close;
  /**< @brief Close connection */
  slim_QSQmiSvcCb_EnableSensorDataFnT          *pfn_EnableSensorData;
  /**< @brief Enable/disable sensor data */
  slim_QSQmiSvcCb_EnablePedometerFnT           *pfn_EnablePedometer;
  /**< @brief Enable/disable pedometer */
  slim_QSQmiSvcCb_EnableQmdDataFnT             *pfn_EnableQmdData;
  /**< @brief Enable/disable AMD/MDr data */
  slim_QSQmiSvcCb_EnableVehicleDataFnT         *pfn_EnableVehicleData;
  /**< @brief Enable vehicle sensor data */
};

/**
  @brief QMI-SLIM QMI Service Transaction object

  Service transaction object exists for every request to SLIM core that
  requires a matching response.

  The following methods are available:
  - @ref QSQmiSvcTx_Init
  - @ref QSQmiSvcTx_Deinit

  @ingroup slim_QSQmiSvc
 */
struct slim_QSQmiSvcTxS
{
  boolean            b_Opened;
  /**< @brief Flag if the object is in use */
  qmi_req_handle     z_QmiReqHandle;
  /**< @brief Transaction expiration time */
  uint32             q_MsgId;
  /**< @brief QMI message id */
  uint8              u_Id;
  /**< @brief SLIM transaction id */
};

/**
  @brief States of QMI-SLIM service connections

  @sa slim_QSQmiSvcCltT
  @ingroup slim_QSQmiSvc
 */
typedef enum
{
  eSLIM_QS_CLT_CONN_STATE_CLOSED,
  /**< Connection exists, but client is not registered */
  eSLIM_QS_CLT_CONN_STATE_OPENING,
  /**< Client registration is in progress */
  eSLIM_QS_CLT_CONN_STATE_OPENED,
  /**< Client is registered */
} slim_QSQmiSvcCltConnStateT;

/**
  @brief QMI-SLIM QMI Service Connection object

  The following operations are applicable:
  - General methods
     - @ref slim_QSQmiSvcClt_Init
     - @ref slim_QSQmiSvcClt_Deinit
     - @ref slim_QSQmiSvcClt_ProcessReq
     - @ref slim_QSQmiSvcClt_HandleSlimEvent

  - QMI Request Processing
     - @ref QSQmiSvcClt_ProcessRegisterReq
     - @ref QSQmiSvcClt_ProcessUnregisterReq
     - @ref QSQmiSvcClt_ProcessGetTimeUpdateReq
     - @ref QSQmiSvcClt_ProcessEnableSensorDataReq
     - @ref QSQmiSvcClt_ProcessEnablePedometerReq
     - @ref QSQmiSvcClt_ProcessEnableQmdDataReq
     - @ref QSQmiSvcClt_ProcessEnableVehicleDataReq
     - @ref QSQmiSvcClt_ProcessUnknownReq
     - @ref QSQmiSvcClt_ProcessDisableServiceReq
  - QMI Response Handling
     - @ref QSQmiSvcClt_SendResponse
     - @ref QSQmiSvcClt_SendGenericResponse
     - @ref QSQmiSvcClt_SendErrorResponse
  - QMI Indication Handling
     - @ref QSQmiSvcClt_SendIndication
     - @ref QSQmiSvcClt_SendErrorIndication
  - SLIM Event Processing
     - @ref QSQmiSvcClt_HandleSlimResponse
     - @ref QSQmiSvcClt_HandleSlimIndication
     - @ref QSQmiSvcClt_HandleSlimServiceStatusIndication
  - Utilities
     - @ref QSQmiSvcClt_AllocTx
     - @ref QSQmiSvcClt_ReclaimTx

  @ingroup slim_QSQmiSvc
 */
struct slim_QSQmiSvcCltS
{
  SLIM_QS_LINK(slim_QSQmiSvcCltT,z_Link);
  /**< @brief Linked list entry field */
  slim_QSQmiSvcT            *pz_Service;
  /**< @brief Parent service */
  qmi_client_handle          z_QmiClientHandle;
  /**< @brief QMI connection handle */
  uint32                     q_Id;
  /**< @brief Identifier */
  slim_QSNodeT               e_PeerNode;
  /**< @brief Type of remote client after registration */
  slim_QSQmiSvcTxT           z_Txs[256];
  /**< @brief Transactions object pool */
  slim_QSQmiSvcTxT           z_COTx;
  /**< @brief Transaction object for connection open operations */
  slim_QSQmiSvcCltConnStateT e_State;
  /**< @brief State of connection */
  slim_QSQmiSvcCbT          *pz_Callback;
  /**< @brief Service callback interface for SLIM integration */
};

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * @brief Constructor for QMI-SLIM QMI Service object
 *
 * @sa slim_QSQmiSvcT
 * @ingroup slim_QSQmiSvc
 */
boolean slim_QSQmiSvc_Init
(
  slim_QSQmiSvcT          *pz_Object,
  /**< [in] QMI service object */
  slim_QSNodeT             e_Node,
  /**< [in] Local node type. This determines QMI service instance identifier
  */
  slim_QSQmiSvcCbFactoryT *pz_CallbackFactory
  /**< [in] Callback factory interface. @sa slim_QSQmiSvcCbFactory */
);
/**
 * @brief Destructor for object
 *
 * @sa slim_QSQmiSvcT
 * @ingroup slim_QSQmiSvc
 */
boolean slim_QSQmiSvc_Deinit
(
  slim_QSQmiSvcT     *pz_Object
  /**< [in] QMI service object */
);
/**
 * @brief Registers object in QMI framework
 *
 * @sa slim_QSQmiSvcT
 * @ingroup slim_QSQmiSvc
 */
boolean slim_QSQmiSvc_Register
(
  slim_QSQmiSvcT     *pz_Object,
  /**< [in] QMI service object */
  qmi_csi_os_params  *pz_QmiOsParams
  /**< [in] QMI event parameters */
);
/**
 * @brief Unregisters object from QMI framework
 *
 * @sa slim_QSQmiSvcT
 * @ingroup slim_QSQmiSvc
 */
boolean slim_QSQmiSvc_Unregister
(
  slim_QSQmiSvcT     *pz_Object
  /**< [in] QMI service object */
);
/**
 * @brief Processes pending QMI events
 * @sa slim_QSQmiSvcT
 *
 * @ingroup slim_QSQmiSvc
 */
boolean slim_QSQmiSvc_HandleQmiEvent
(
  slim_QSQmiSvcT     *pz_Object,
  /**< [in] QMI service object */
  qmi_csi_os_params  *pz_QmiOsParams
  /**< [in] QMI event parameters */
);
/**
 * @brief Processes given SLIM event
 * @sa slim_QSQmiSvcT
 */
boolean slim_QSQmiSvc_HandleSlimEvent
(
  slim_QSQmiSvcT                 *pz_Object,
  /**< [in] QMI service object */
  uint32                          q_ClientId,
  /**< [in] Identifier for the target client connection */
  const slimMessageHeaderStructT *pz_Hdr,
  /**< [in] SLIM message header */
  const slim_QSSlimMessageDataT  *pz_Data
  /**< [in] SLIM Message payload (or NULL)  */
);

/**
 * @brief Encodes message into internal buffer
 *
 * @param[in]  pz_Object        Service object.
 * @param[in]  e_MsgType        QMI message type.
 * @param[in]  q_MsgId          QMI message identifier.
 * @param[in]  q_MsgLength      Size of encoded message payload.
 * @param[in]  p_Msg            Optional encoded message payload.
 * @param[out] pq_EncodedLenOut Encoded message length if succeeded.
 *
 * @retval TRUE  Encoding has succeeded.
 * @retval FALSE Encoding has failed.
 *
 * @ingroup slim_QSQmiSvc
 */
boolean slim_QSQmiSvc_EncodeMessage
(
  slim_QSQmiSvcT *pz_Object,
  qmi_idl_type_of_message_type e_MsgType,
  uint32 q_MsgId,
  uint32 q_MsgLength,
  const void *p_Msg,
  uint32 *pq_EncodedLenOut
);

//! @}

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* QMISLIM_QMI_SERVICE_API_H */
