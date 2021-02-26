/*
Copyright (c) 2014-2015 Qualcomm Technologies, Inc. All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/
/**
 * @file
 * @brief QMI-LOC Client Listener header file
 *
 * QMI LOC client listener is interface to modem sensor clients (SAP & SAMLITE)
 * Sensor data requests and sensor data injection is through QMI LOC transport.
 *
 * @ingroup slim_APSS
 */
#ifndef QL_CLIENT_LISTENER_H
#define QL_CLIENT_LISTENER_H

#include <LBSAdapterBase.h>
#include <ClientListener.h>
#include "slim_utils.h"
#include <SlimSensor.h>

//! @addtogroup slim_APSS
//! @{

namespace slim
{

/**
@brief QMI-LOC client interface

@todo Add handling of SLIM service notifications to prevent service
      requests when service is not available.
@todo Add handling of service errors with restart as in SLIM client library
      (slimproxy::Connection).
*/
class QLClientListener :
  protected lbs_core::LBSAdapterBase,
  protected ClientListener
{
  //! @brief Client identifier (MPSS)
  const uint32_t m_qClientId;
  //! @brief Transaction number for outgoing requests.
  /*!
   * Currently only one transaction identifier is used even when there are
   * multiple open transactions.
   */
  const uint8_t  m_uClientTxnId;
  //! @brief Service provider to use when enabling services.
  const slimServiceProviderEnumT m_eProvider;

  /*!
  @brief Array of service states

  Whenever service is enabled, the appropriate entry is set to 1 to jeep
  track of enabled services.
  */
  volatile uint8_t m_lSensorSessionInfo[eSLIM_SERVICE_LAST+1];

  //! @brief Instance-locking mutex
  static Mutex s_zInstanceMutex;
  //! @brief Singleton instance
  static QLClientListener* s_pzInstance;
  //! @brief LOC API Event Mask for processing service events.
  static const LOC_API_ADAPTER_EVENT_MASK_T s_qMask;
public:
  /**
   * @brief Get class object
   *
   * Function to get class object. Class is a singleton.
   *
   * @param[in] eProvider Preferred provider, this information is required so as
   *                      to send as parameter for time sync request.
   *
   * @return Listener object instance.
   */
  static ClientListener* get
  (
    slimServiceProviderEnumT provider
  );
  /**
  @brief Releases object

  The method releases class object if it has been previously instantiated.
  */
  static void disable();
protected:
  QLClientListener
  (
    slimServiceProviderEnumT provider
  );
  ~QLClientListener();

  //LBSAdapterBase inherited functions
  virtual bool requestTimeData(const TimeRequest &timeRequest);
  virtual bool requestSensorData(const SensorRequest &request);
  virtual bool requestPedometerData(const PedometerRequest &request);
  virtual bool requestMotionData(const SensorRequest &request);

  virtual void handleEngineUpEvent();
  virtual void handleEngineDownEvent();

  // ClientListener inherited functions
  virtual void handleServiceResponse
  (
    const slimMessageHeaderStructT &zHeader
  );
  virtual void handleOpenResponse
  (
    const slimMessageHeaderStructT &zHeader
  );
  virtual void handleSensorData
  (
    const slimMessageHeaderStructT &zHeader,
    const slimSensorDataStructT &zData
  );
  virtual void handlePedometerData
  (
    const slimMessageHeaderStructT &zHeader,
    const slimPedometerDataStructT &zData
  );
  virtual void handleMotionData
  (
    const slimMessageHeaderStructT &zHeader,
    const slimMotionDataStructT &zData
  );

private:
  //class specific functions
  bool injectSensorData(const SensorDataBundle &dataBundle);
  bool injectMotionData(const MotionData &motionData);
  bool injectPedometerData(const PedometerData &pedometerData);
  bool injectTimeData(const TimeData &timeData);

};

}; //namespace slim

//! @}

#endif //QL_CLIENT_LISTENER_H
