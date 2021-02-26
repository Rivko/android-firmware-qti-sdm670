/*
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/
/*!
 * @file
 * @brief C++ interface for QMI-SLIM QMI client component.
 *
 * @ingroup slim_ClientLibrary
 */
#ifndef __QMISLIM_CLIENT_H_INCLUDED__
#define __QMISLIM_CLIENT_H_INCLUDED__

#include <slim_api.h>

//! @addtogroup slim_ClientLibrary
//! @{

/*----------------------------------------------------------------------------
* Preprocessor Definitions and Constants
* -------------------------------------------------------------------------*/
#ifndef SLIM_CLIENT_API
/**
@brief Exported or imported API types
*/
#define SLIM_CLIENT_API __attribute__ ((visibility ("default")))
/**
@brief Local API types
*/
#define SLIM_CLIENT_LOCAL __attribute__ ((visibility ("hidden")))
#endif /* SLIM_CLIENT_API */

//! @}

/*!
 * @brief C++ adapter objects for QMI-SLIM components.
 *
 * @ingroup slim_ClientLibrary
 */
namespace qmislim
{

class ClientPrivate;
class Client;

//! @addtogroup slim_ClientLibrary
//! @{

/*!
 * @brief QMI-SLIM Node Type Enumeration
 *
 * This enumeration defines known SLIM node types. Generally node is an
 * equivalent of processor with SLIM component.
 */
enum Node
{
  eNODE_UNKNOWN,//!< Unknown node type.
  eNODE_AP,     //!< Application processor node.
  eNODE_MP,     //!< Modem processor node.
  eNODE_TEST,   //!< Test node.
};

/**
 * @brief C++ wrapper for QMI-SLIM QMI Client interface
 *
 * The object encapsulates plain C interface of QMI-SLIM QMI Client API into C++
 * object.
 *
 * @sa QmiClient::ICallback
 * @sa slim_QSQmiClientT
 * @ingroup slim_ClientLibrary
 */
class SLIM_CLIENT_API Client
{
private:
  ClientPrivate *m_pzPrivate;
  /**< Private data object */
public:
  /**
   * @brief Callback interface for @a QmiClient object
   *
   * All interface methods are executed from QCCI thread context unless
   * there is an asynchronous error handling invoked.
   */
  class SLIM_CLIENT_API ICallback
  {
  protected:
    /**
     * @brief Pure virtual destructor for V-table completeness.
     */
    virtual ~ICallback() = 0;
  public:
    /**
     * @brief Callback for handling service enable response messages.
     */
    virtual void onQmiClientResponse
    (
      void                *pUserCookie,
      /**< [in] User-provided cookie pointer */
      slimErrorEnumT       eError,
      /**< [in] Operation status */
      slimMessageIdEnumT   eMsgId,
      /**< [in] SLIM message id that corresponds to response */
      const void          *pData,
      /**< [in] SLIM message payload */
      uint32_t             qDataSize
      /**< [in] SLIM message payload size */
    ) = 0;
    /**
     * @brief Callback for handling service data and error indications.
     */
    virtual void onQmiClientData
    (
      slimServiceEnumT     eService,
      /**< [in] Reporting service */
      slimErrorEnumT       eError,
      /**< [in] Indication status */
      slimMessageIdEnumT   eMsgId,
      /**< [in] SLIM message id that corresponds to indication */
      const void          *pData,
      /**< [in] SLIM message payload */
      uint32_t             qDataSize
      /**< [in] SLIM message payload size */
    ) = 0;
    /**
     * @brief Callback for handling time update response.
     */
    virtual void onQmiClientTimeUpdate
    (
      void                *pUserCookie,
      slimErrorEnumT       eError,
      uint64_t             tRemoteRxTimeMs,
      uint64_t             tRemoteTxTimeMs
    ) = 0;
    /**
     * @brief Callback for handling QCCI error notification.
     */
    virtual void onQmiClientError
    (
       int32_t             lError
       /**< [in] Operation status. */
    ) = 0;
    /**
     * @brief Callback for handling service change indications.
     */
   virtual void onQmiClientServiceChange
   (
      slimServiceEnumT         eService,
      /**< [in] Reporting service */
      slimServiceProviderMaskT qServiceProviderMask
      /**< [in] Available service providers */
   ) = 0;
   /**
    * @brief Callback for handling QCCI disconnect result.
    */
   virtual void onQmiClientDisconnect() = 0;
   /**
    * @brief Callback for handling QMI-SLIM registration result.
    */
   virtual void onQmiClientRegistered
   (
     slimErrorEnumT       eError
     /**< [in] Operation status. */
   ) = 0;
   /**
    * @brief Callback for handling QMI-SLIM deregistration result.
    */
   virtual void onQmiClientUnregistered
   (
     slimErrorEnumT       eError
     /**< [in] Operation status. */
   ) = 0;
  };

  /**
   * @brief Object state enumeration
   */
  enum State
  {
    eSTATE_ERROR,
    /**< Object is in error state. Not usable. */
    eSTATE_DISCONNECTED,
    /**< Object is disconnected */
    eSTATE_CONNECTED,
    /**< Object is connected */
    eSTATE_REGISTERING,
    /**< Object is connected and registration operation is in progress */
    eSTATE_REGISTERED,
    /**< Object is connected and registered */
    eSTATE_UNREGISTERING,
    /**< Object is connected, registered and unregistration operation is in
     * progress. */
    eSTATE_DISCONNECTING,
    /**< Object is connected */
  };
  /**
   * @brief Constructs object
   */
  Client
  (
    Node           eThisNode,
    /**< [in] Local node type. */
    Node           ePeerNode
    /**< [in] Peer node type. */
  );
  /**
   * @brief Object destructor
   */
  virtual ~Client();

  /**
   * @brief Change nodes
   */
  void reinit
  (
    Node           eThisNode,
    /**< [in] Local node type. */
    Node           ePeerNode
    /**< [in] Peer node type. */
  );
  /**
   * @brief Set or clear callback interface
   */
  void setCallback
  (
    ICallback      *pzCallback
    /**< [in] Callback interface for event notifications. */
  );

  /**
   * @brief Requests QMI Client to connect to service if available
   *
   * @retval true on success
   * @retval false QMI error or connection is not available
   */
  bool Connect
  (
  );
  /**
   * @brief Initiates disconnect procedure.
   *
   * @retval true on success
   * @retval false on error
   */
  bool Disconnect
  (
  );
  /**
   * @brief Registers client with remote peer
   *
   * Before any request can be sent, the client must be registered with
   * this call. If the call succeeds, the callback will be invoked when
   * operation is completed.
   *
   * @return Error status, eSLIM_SUCCESS when no error.
   */
  slimErrorEnumT Register
  (
  );
  /**
   * @brief Unregisters client with remote peer
   *
   * Removes client registration (but keeps QMI connection).
   *
   * @return Error status, eSLIM_SUCCESS when no error.
   */
  slimErrorEnumT Unregister
  (
  );
  /**
   * @brief Sends time update request to peer.
   *
   * @return Error status, eSLIM_SUCCESS when no error.
   */
  slimErrorEnumT GetTimeUpdate
  (
    uint64_t tCurrentMs,
    /**< [in] Current time for peer time conversion */
    void *p_UserCookie
    /**< [in] User cookie for identifying response. */
  );

  /**
   * @brief Helper method to disable any service
   *
   * This method allow direct service disabling-
   *
   * @return Error status, eSLIM_SUCCESS when no error.
   */
  slimErrorEnumT DisableData
  (
    slimServiceEnumT e_Service,
    /**< [in] SLIM service to disable. */
    void *p_UserCookie,
    /**< [in] User cookie for identifying response. */
    slimMessageIdEnumT e_SlimMsgId
    /**< [in] SLIM message type for response. */
  );

  /**
   * @brief Sends sensor data request to peer.
   *
   * @return Error status, eSLIM_SUCCESS when no error.
   */
  slimErrorEnumT EnableSensorData
  (
    const slimEnableSensorDataRequestStructT &z_Request,
    /**< [in] SLIM request data. */
    void *p_UserCookie
    /**< [in] User cookie for identifying response. */
  );
  /**
   * @brief Sends pedometer request to peer.
   *
   * @return Error status, eSLIM_SUCCESS when no error.
   */
  slimErrorEnumT EnablePedometer
  (
    const slimEnablePedometerRequestStructT &z_Request,
    /**< [in] SLIM request data. */
    void *p_UserCookie
    /**< [in] User cookie for identifying response. */
  );
  /**
   * @brief Sends AMD/RMD data request to peer
   *
   * @return Error status, eSLIM_SUCCESS when no error.
   */
  slimErrorEnumT EnableQmdData
  (
    const slimEnableQmdDataRequestStructT &z_Request,
    /**< [in] SLIM request data. */
    void *p_UserCookie
    /**< [in] User cookie for identifying response. */
  );

  /**
   * @brief Sends vehicle data request to peer
   *
   * @return Error status, eSLIM_SUCCESS when no error.
   */
  slimErrorEnumT EnableVehicleData
  (
    const slimEnableVehicleDataRequestStructT &z_Request,
    /**< [in] SLIM request data. */
    void *p_UserCookie
    /**< [in] User cookie for identifying response. */
  );

  /**
   * @brief Checks if QMI client has any pending timed events.
   *
   * @return Time in milliseconds till next event check
   * @retval false if there are no events
   * @retval true if there are no events
   */
  bool GetNextEventTime
  (
      uint64_t &tEventTimeMs
      /**< [out] Time till next event in milliseconds */
  );
  /**
   * @brief Performs pending event processing.
   *
   * @retval true on success
   * @retval false on error
   */
  bool ProcessTimeEvents
  (
  );
  /**
   * @brief Tests if the object is disconnected.
   * @retval true if the state is disconnected.
   */
  bool isDisconnected() const;

  /**
   * @brief Returns current object state.
   * @return Object state.
   */
  State getState() const;
}; /* class QmiClient */

//! @}

} /* namespace qmislim */

#endif /* __QMISLIM_CLIENT_H_INCLUDED__ */
