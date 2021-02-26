/*
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/
/*!
 * @file
 * @brief C++ implementation for QMI-SLIM QMI client component.
 *
 * @ingroup slim_ClientLibrary
 */
#include <stdint.h>
#include <inttypes.h>
#include <limits.h>

#include <qmislim_qmi_client_api.h>
#include <QmiSlimClient.h>

namespace qmislim
{

//! addtogroup slim_ClientLibrary
//! @{
/**
 * @brief Private class for QMI-SLIM QMI Client
 *
 * Class hides all implementation details from the user of QMI-SLIM QMI client.
 */
class SLIM_CLIENT_LOCAL ClientPrivate
{
  friend class Client;

  /**
   * QMI-SLIM QMI client interface function table.
   */
  static const slim_QSQmiClientCb_VtT zCallback;

  /** Internal QMI-SLIM QMI client implementation */
  slim_QSQmiClientT       m_zClient;
  /** QMI-SLIM QMI client callback function table pointer */
  const slim_QSQmiClientCb_VtT *const m_pzCallbackVt;
  /** Container pointer */
  Client * const m_pzClient;
  /** Callback pointer */
  Client::ICallback   *m_pzCallback;

  /**
   * Converts C++ node type into internal type.
   * @return QMI-SLIM internal node type.
   */
  static slim_QSNodeT TranslateNode
  (
    Node eNode
    /**<[in] QMI-SLIM node type */
  );
  /**
   * Helper for converting QMI-SLIM callback interface into object
   */
  static ClientPrivate *QmiClientCbI2O
  (
     slim_QSQmiClientCbT *p_Iface
     /**<[in] Internal QMI-SLIM callback interface */
  );
  /**
   * Helper for converting QMI-SLIM callback interface into C++ callback
   */
  static Client::ICallback *GetCallback
  (
     slim_QSQmiClientCbT *p_Iface
     /**<[in] Internal QMI-SLIM callback interface */
  );

  /* QMI-SLIM callback methods */
  static slim_QSQmiClientCb_OnRespFnT          OnResponse;
  static slim_QSQmiClientCb_OnDataFnT          OnData;
  static slim_QSQmiClientCb_OnTimeUpdateFnT    OnTimeUpdate;
  static slim_QSQmiClientCb_OnErrFnT           OnError;
  static slim_QSQmiClientCb_OnServiceChangeFnT OnServiceChange;
  static slim_QSQmiClientCb_OnDisconnectFnT    OnDisconnect;
  static slim_QSQmiClientCb_OnRegisteredFnT    OnRegistered;
  static slim_QSQmiClientCb_OnUnregisteredFnT  OnUnregistered;

  ClientPrivate(Client *pzClient);
};

//! @}

const slim_QSQmiClientCb_VtT ClientPrivate::zCallback =
{
  OnResponse,
  OnData,
  OnTimeUpdate,
  OnError,
  OnServiceChange,
  OnDisconnect,
  OnRegistered,
  OnUnregistered,
};

slim_QSNodeT ClientPrivate::TranslateNode(Node eNode)
{
  switch (eNode)
  {
  case eNODE_UNKNOWN:
    return eSLIM_QS_NODE_UNKNOWN;
  case eNODE_MP:
    return eSLIM_QS_NODE_MP;
  case eNODE_AP:
    return eSLIM_QS_NODE_AP;
  case eNODE_TEST:
    return eSLIM_QS_NODE_TEST;
  default:
    return eSLIM_QS_NODE_UNKNOWN;
  }
}

ClientPrivate *ClientPrivate::QmiClientCbI2O
(
  slim_QSQmiClientCbT *p_Iface
)
{
   return reinterpret_cast<ClientPrivate*>(
       slim_QSMember2Container(
          (void*)p_Iface,
          SLIM_QS_P2M_OFFSET(ClientPrivate, m_pzCallbackVt)));
}

Client::ICallback *ClientPrivate::GetCallback
(
  slim_QSQmiClientCbT *pzInterface
)
{
  ClientPrivate *pzObject = QmiClientCbI2O(pzInterface);
  if (0 != pzObject)
  {
    return pzObject->m_pzCallback;
  }
  return 0;
}

ClientPrivate::ClientPrivate(Client *pzClient) :
    m_pzCallbackVt(&zCallback),
    m_pzClient(pzClient),
    m_pzCallback(0)
{
}

void ClientPrivate::OnResponse
(
  slim_QSQmiClientCbT *pz_Iface,
  void                *p_UserCookie,
  slimErrorEnumT       e_Error,
  slimMessageIdEnumT   e_MsgId,
  const void          *p_Data,
  uint16               w_DataSize
)
{
  Client::ICallback     *pzCallback;

  pzCallback = GetCallback(pz_Iface);
  if (0 != pzCallback)
  {
    pzCallback->onQmiClientResponse(
        p_UserCookie,
        e_Error,
        e_MsgId,
        p_Data,
        w_DataSize);
  }
}
void ClientPrivate::OnData
(
  slim_QSQmiClientCbT *pz_Iface,
  slimServiceEnumT     e_Service,
  slimErrorEnumT       e_Error,
  slimMessageIdEnumT   e_MsgId,
  const void          *p_Data,
  uint32               q_DataSize
)
{
  Client::ICallback     *pzCallback;

  pzCallback = GetCallback(pz_Iface);
  if (0 != pzCallback)
  {
    pzCallback->onQmiClientData(
        e_Service,
        e_Error,
        e_MsgId,
        p_Data,
        q_DataSize);
  }
}
void ClientPrivate::OnTimeUpdate
(
  slim_QSQmiClientCbT *pz_Iface,
  void                *p_UserCookie,
  slimErrorEnumT       e_Error,
  uint64               t_RemoteRxTimeMs,
  uint64               t_RemoteTxTimeMs
)
{
  Client::ICallback     *pzCallback;

  pzCallback = GetCallback(pz_Iface);
  if (0 != pzCallback)
  {
    pzCallback->onQmiClientTimeUpdate(
        p_UserCookie,
        e_Error,
        t_RemoteRxTimeMs,
        t_RemoteTxTimeMs);
  }
}
void ClientPrivate::OnError
(
  slim_QSQmiClientCbT *pz_Iface,
  int32                l_QmiErr
)
{
  Client::ICallback     *pzCallback;

  pzCallback = GetCallback(pz_Iface);
  if (0 != pzCallback)
  {
    pzCallback->onQmiClientError(l_QmiErr);
  }
}
void ClientPrivate::OnServiceChange
(
  slim_QSQmiClientCbT     *pz_Iface,
  slimServiceEnumT         e_Service,
  slimServiceProviderMaskT q_ServiceProviderMask
)
{
  Client::ICallback     *pzCallback;

  pzCallback = GetCallback(pz_Iface);
  if (0 != pzCallback)
  {
    pzCallback->onQmiClientServiceChange(e_Service, q_ServiceProviderMask);
  }
}
void ClientPrivate::OnDisconnect
(
   slim_QSQmiClientCbT *pz_Iface
)
{
  Client::ICallback     *pzCallback;

  pzCallback = GetCallback(pz_Iface);
  if (0 != pzCallback)
  {
    pzCallback->onQmiClientDisconnect();
  }
}

void ClientPrivate::OnRegistered
(
   slim_QSQmiClientCbT *pz_Iface,
   slimErrorEnumT       e_Error
)
{
  Client::ICallback     *pzCallback;

  pzCallback = GetCallback(pz_Iface);
  if (0 != pzCallback)
  {
    pzCallback->onQmiClientRegistered(e_Error);
  }
}

void ClientPrivate::OnUnregistered
(
   slim_QSQmiClientCbT *pz_Iface,
   slimErrorEnumT       e_Error
)
{
  Client::ICallback     *pzCallback;

  pzCallback = GetCallback(pz_Iface);
  if (0 != pzCallback)
  {
    pzCallback->onQmiClientUnregistered(e_Error);
  }
}


Client::ICallback::~ICallback()
{
}

/**
 * @brief Constructs object
 */
Client::Client
(
  Node           eThisNode,
  /**< [in] Local node type. */
  Node           ePeerNode
  /**< [in] Peer node type. */
)
: m_pzPrivate(0)
{
  m_pzPrivate = new ClientPrivate(this);
  m_pzPrivate->m_pzCallback = 0;
  if (TRUE != slim_QSQmiClient_Init(
      &m_pzPrivate->m_zClient,
      ClientPrivate::TranslateNode(eThisNode),
      ClientPrivate::TranslateNode(ePeerNode),
      &m_pzPrivate->m_pzCallbackVt))
  {
    SLIM_LOGE("Failed to initialize QMI-SLIM QMI Client");
    delete m_pzPrivate;
    m_pzPrivate = 0;
  }
}
/**
 * @brief Object destructor
 */
Client::~Client()
{
  if (0 != m_pzPrivate)
  {
    slim_QSQmiClient_Deinit(&m_pzPrivate->m_zClient);
    delete m_pzPrivate;
  }
}

/**
 * @brief Change nodes
 */
void Client::reinit
(
  Node           eThisNode,
  /**< [in] Local node type. */
  Node           ePeerNode
  /**< [in] Peer node type. */
)
{
  if (0 != m_pzPrivate)
  {
    slim_QSQmiClient_Deinit(&m_pzPrivate->m_zClient);
    if (TRUE != slim_QSQmiClient_Init(
        &m_pzPrivate->m_zClient,
        ClientPrivate::TranslateNode(eThisNode),
        ClientPrivate::TranslateNode(ePeerNode),
        &m_pzPrivate->m_pzCallbackVt))
    {
      SLIM_LOGE("Failed to initialize QMI-SLIM QMI Client");
      delete m_pzPrivate;
      m_pzPrivate = 0;
    }
  }
}

/**
 * @brief Set or clear callback interface
 */
void Client::setCallback
(
  ICallback      *pzCallback
  /**< [in] Callback interface for event notifications. */
)
{
  if (0 != m_pzPrivate)
  {
    m_pzPrivate->m_pzCallback = pzCallback;
  }
}

/**
 * @brief Requests QMI Client to connect to service if available
 *
 * @retval true on success
 * @retval false QMI error or connection is not available
 */
bool Client::Connect()
{
  return slim_QSQmiClient_Connect(&m_pzPrivate->m_zClient) != FALSE;
}

/**
 * @brief Initiates disconnect procedure.
 *
 * @retval true on success
 * @retval false on error
 */
bool Client::Disconnect
(
)
{
  if (0 != m_pzPrivate)
  {
    return slim_QSQmiClient_Disconnect(&m_pzPrivate->m_zClient) != FALSE;
  }
  else
  {
    return false;
  }
}
/**
 * @brief Registers client with remote peer
 *
 * Before any request can be sent, the client must be registered with
 * this call. If the call succeeds, the callback will be invoked when
 * operation is completed.
 *
 * @return Error status, eSLIM_SUCCESS when no error.
 */
slimErrorEnumT Client::Register
(
)
{
  if (0 != m_pzPrivate)
  {
    return slim_QSQmiClient_Register(&m_pzPrivate->m_zClient);
  }
  else
  {
    return eSLIM_ERROR_INTERNAL;
  }
}
/**
 * @brief Unregisters client with remote peer
 *
 * Removes client registration (but keeps QMI connection).
 *
 * @return Error status, eSLIM_SUCCESS when no error.
 */
slimErrorEnumT Client::Unregister
(
)
{
  if (0 != m_pzPrivate)
  {
    return slim_QSQmiClient_Unregister(&m_pzPrivate->m_zClient);
  }
  else
  {
    return eSLIM_ERROR_INTERNAL;
  }
}

/**
 * @brief Sends time update request to peer.
 *
 * @return Error status, eSLIM_SUCCESS when no error.
 */
slimErrorEnumT Client::GetTimeUpdate
(
  uint64_t tCurrentMs,
  /**< [in] Current time for peer time conversion */
  void *pUserCookie
  /**< [in] User cookie for identifying response. */
)
{
  if (0 != m_pzPrivate)
  {
    return slim_QSQmiClient_GetTimeUpdate(
        &m_pzPrivate->m_zClient, tCurrentMs, pUserCookie);
  }
  else
  {
    return eSLIM_ERROR_INTERNAL;
  }
}

/**
 * @brief Helper method to disable any service
 *
 * This method allow direct service disabling-
 *
 * @return Error status, eSLIM_SUCCESS when no error.
 */
slimErrorEnumT Client::DisableData
(
  slimServiceEnumT e_Service,
  /**< [in] SLIM service to disable. */
  void *p_UserCookie,
  /**< [in] User cookie for identifying response. */
  slimMessageIdEnumT e_SlimMsgId
  /**< [in] SLIM message type for response. */
)
{
  if (0 != m_pzPrivate)
  {
    return slim_QSQmiClient_DisableData(
        &m_pzPrivate->m_zClient,
        e_Service,
        p_UserCookie,
        e_SlimMsgId);
  }
  else
  {
    return eSLIM_ERROR_INTERNAL;
  }
}

/**
 * @brief Sends sensor data request to peer.
 *
 * @return Error status, eSLIM_SUCCESS when no error.
 */
slimErrorEnumT Client::EnableSensorData
(
  const slimEnableSensorDataRequestStructT &z_Request,
  /**< [in] SLIM request data. */
  void *p_UserCookie
  /**< [in] User cookie for identifying response. */
)
{
  if (0 != m_pzPrivate)
  {
    return slim_QSQmiClient_EnableSensorData(
        &m_pzPrivate->m_zClient,
        &z_Request,
        p_UserCookie);
  }
  else
  {
    return eSLIM_ERROR_INTERNAL;
  }
}
/**
 * @brief Sends pedometer request to peer.
 *
 * @return Error status, eSLIM_SUCCESS when no error.
 */
slimErrorEnumT Client::EnablePedometer
(
  const slimEnablePedometerRequestStructT &z_Request,
  /**< [in] SLIM request data. */
  void *p_UserCookie
  /**< [in] User cookie for identifying response. */
)
{
  if (0 != m_pzPrivate)
  {
    return slim_QSQmiClient_EnablePedometer(
        &m_pzPrivate->m_zClient,
        &z_Request,
        p_UserCookie);
  }
  else
  {
    return eSLIM_ERROR_INTERNAL;
  }
}
/**
 * @brief Sends AMD/RMD data request to peer
 *
 * @return Error status, eSLIM_SUCCESS when no error.
 */
slimErrorEnumT Client::EnableQmdData
(
  const slimEnableQmdDataRequestStructT &z_Request,
  /**< [in] SLIM request data. */
  void *p_UserCookie
  /**< [in] User cookie for identifying response. */
)
{
  if (0 != m_pzPrivate)
  {
    return slim_QSQmiClient_EnableQmdData(
        &m_pzPrivate->m_zClient,
        &z_Request,
        p_UserCookie);
  }
  else
  {
    return eSLIM_ERROR_INTERNAL;
  }
}

/**
 * @brief Sends vehicle data request to peer
 *
 * @return Error status, eSLIM_SUCCESS when no error.
 */
slimErrorEnumT Client::EnableVehicleData
(
  const slimEnableVehicleDataRequestStructT &z_Request,
  /**< [in] SLIM request data. */
  void *p_UserCookie
  /**< [in] User cookie for identifying response. */
)
{
  if (0 != m_pzPrivate)
  {
    return slim_QSQmiClient_EnableVehicleData(
        &m_pzPrivate->m_zClient,
        &z_Request,
        p_UserCookie);
  }
  else
  {
    return eSLIM_ERROR_INTERNAL;
  }
}

/**
 * @brief Checks if QMI client has any pending timed events.
 *
 * @return Time in milliseconds till next event check
 * @retval false if there are no events
 * @retval true if there are no events
 */
bool Client::GetNextEventTime
(
    uint64_t &tEventTimeMs
)
{
  if (0 != m_pzPrivate)
  {
    uint64 tResult = slim_QSQmiClient_GetNextEventTime(&m_pzPrivate->m_zClient);
    if (SLIM_QS_QMI_CLIENT_EXPIRE_TIME_NEVER != tResult)
    {
      tEventTimeMs = tResult;
      return true;
    }
  }
  return false;
}
/**
 * @brief Performs pending event processing.
 *
 * @retval true on success
 * @retval false on error
 */
bool Client::ProcessTimeEvents
(
)
{
  if (0 != m_pzPrivate)
  {
    return slim_QSQmiClient_ProcessTimeEvents(&m_pzPrivate->m_zClient) != FALSE;
  }
  return false;
}
/**
 * @brief Tests if the object is disconnected.
 * @retval true if the state is disconnected.
 */
bool Client::isDisconnected() const
{
  return eSTATE_DISCONNECTED == getState();
}

/**
 * @brief Returns current object state.
 * @return Object state.
 */
Client::State Client::getState() const
{
  if (0 != m_pzPrivate)
  {
    switch (m_pzPrivate->m_zClient.e_State)
    {
    case eSLIM_QS_QMI_CLIENT_STATE_DISCONNECTED:
      return eSTATE_DISCONNECTED;
    case eSLIM_QS_QMI_CLIENT_STATE_CONNECTED:
      return eSTATE_CONNECTED;
    case eSLIM_QS_QMI_CLIENT_STATE_REGISTERING:
      return eSTATE_REGISTERING;
    case eSLIM_QS_QMI_CLIENT_STATE_REGISTERED:
      return eSTATE_REGISTERED;
    case eSLIM_QS_QMI_CLIENT_STATE_UNREGISTERING:
      return eSTATE_UNREGISTERING;
    case eSLIM_QS_QMI_CLIENT_STATE_DISCONNECTING:
      return eSTATE_DISCONNECTING;
    default:
      break;
    }
  }
  return eSTATE_ERROR;
}

} /* namespace qmislim */
