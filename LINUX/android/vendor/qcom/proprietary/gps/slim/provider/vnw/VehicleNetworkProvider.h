/*
Copyright (c) 2014-2015 Qualcomm Technologies, Inc. All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/
/**
@file
@brief Vehicle Network provider interface file.

This file declares VNW provider object.

@ingroup slim_VNWProvider
*/
#ifndef __VEHICLE_NETWORK_PROVIDER_H_INCLUDED__
#define __VEHICLE_NETWORK_PROVIDER_H_INCLUDED__

#include <MultiplexingProvider.h>
#include <CanWrapper.h>
#include "VehicleNetworkConfiguration.h"
#include "VehicleNetworkMessageProcessor.h"

namespace slim
{

/**
@brief Vehicle network provider class.

VehicleNetworkProvider class implements methods specific for vehicle
network connection.

 */

class VehicleNetworkProvider :
  public MultiplexingProvider
{
  /*! Timers enumeration */
  enum
  {
    eTIMER_DEFAULT, /**< Default timer */
  };

  /*! Enumeration of messages to be sent to the provider message processing
   * loop */
  enum
  {

    eIPC_MSG_TIMER = M_IPC_MSG_TIMER,
    /**< Timer message ID */

    eIPC_MSG_NEW_FRAME_EVENT = C_USR_MSG_ID_FIRST + 1,
    /**< New CAN frame message ID */
  };

  /* All message processors are friend classes so they
   * have access to a functionality that this class provides.
   */
  friend class VehicleNetworkMessageProcessor;
  friend class VehicleNetworkDefaultAccelMP;
  friend class VehicleNetworkDefaultGyroMP;
  friend class VehicleNetworkDefaultOdometryMP;
  friend class VehicleNetworkDefaultSpeedMP;
  friend class VehicleNetworkDefaultDwsMP;
  friend class VehicleNetworkDefaultGearMP;
  friend class VehicleNetworkCONF1AccelMP;
  friend class VehicleNetworkCONF1GyroMP;
  friend class VehicleNetworkCONF1OdometryMP;
  friend class VehicleNetworkCONF1SpeedMP;
  friend class VehicleNetworkCONF1DwsMP;
  friend class VehicleNetworkCONF1GearMP;

  static const uint32_t s_qThreadId;
  static Mutex s_zInstanceMutex;
  static VehicleNetworkProvider *s_pzInstance;
  static const slimAvailableServiceMaskT s_qSupportedServices;

  slim_ThreadT m_zThread;
  Mutex m_zMutex;
  slim_TimerT *m_pzTimer;
  mutable slimAvailableServiceMaskT m_qEnabledServices;

  /*! Instance of CanWrapper class to get access to CAN data*/
  CanWrapper *m_pzCanWrapper;

  /*! CanWrapper listener ID */
  int m_earId;

  /*! Callback function for CanWrapper to be called on new CAN frame
   *  reception */
  static void cbOnNewCanFrame(CwFrame* pFrame, void *userData, int ifNo);
  static void cbOnNewBufferedCanFrame(CwFrame* pFrame, void *userData, int ifNo);

  /*! Instance of vehicle network configuration */
  VehicleNetworkConfiguration* m_pzVehicleNetworkCfg;
  /*! Instance of accelrometer service configuration */
  VehicleNetworkMessageProcessor* m_pzAccelProc;
  /*! Instance of gyroscope service configuration */
  VehicleNetworkMessageProcessor* m_pzGyroProc;
  /*! Instance of odometry service configuration */
  VehicleNetworkMessageProcessor* m_pzOdometryProc;
  /*! Instance of speed service configuration */
  VehicleNetworkMessageProcessor* m_pzSpeedProc;
  /*! Instance of dws service configuration */
  VehicleNetworkMessageProcessor* m_pzDwsProc;
  /*! Instance of gear service configuration */
  VehicleNetworkMessageProcessor* m_pzGearProc;

  /*! AP clock time when last sync from CAN bus microcontroller was received*/
  uint64_t                        m_tLastBusControllerSyncTime;
  /*! CAN bus microcontroller clock time set to last sync */
  uint64_t                        m_tLastBusControllerSyncTimestamp;

  /*! Vehicle network provider main thread function */
  static void threadMain
  (
    void *pData
  );

  /**
  @brief Class constructor

  Creates and initialises new instance of the class.

  */
  VehicleNetworkProvider();

  /**
  @brief Class destructor

  Destroys current instance

  */
  ~VehicleNetworkProvider();
  bool initialize();
  void runEventLoop();

  /**
  @brief Method for enabling or disabling vehicle services.

  @param[in] uEnable Flag that indicates if the service shall be enabled or
                     disabled.
  @param[in] eService      Service to control.

  @return eSLIM_SUCCESS is operation succeeded.
  */
  virtual slimErrorEnumT doUpdateVehicleDataStatus
  (
    bool uEnable,
    slimServiceEnumT eService
  );

  /**
  @brief Initiates time offset request.

  Function for making the time request. Successful response enable SLIM to
  calculate the offset between modem time and sensor time.

  @param[in] lTxnId Service transaction id.
  @return eSLIM_SUCCESS if time request is made successfully.
  */
  virtual slimErrorEnumT getTimeUpdate
  (
    int32_t lTxnId
  );
public:
  /*!
   * @brief Returns provider instance.
   *
   * Method provides access to provider instance. If necessary, the instance
   * is created and initialized.
   *
   * @return Provider instance.
   * @retval 0 On error.
   */
  static slim_ServiceProviderInterfaceType *getProvider();
};

}

#endif /* __VEHICLE_NETWORK_PROVIDER_H_INCLUDED__ */
