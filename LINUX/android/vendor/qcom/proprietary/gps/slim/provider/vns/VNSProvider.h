/**
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

/**
@file
@brief Vehicle Network Services provider interface file.

This file declares VNS provider object.

@ingroup VNSProvider
*/
#ifndef __VEHICLE_NETWORK_SERVICES_PROVIDER_H_INCLUDED__
#define __VEHICLE_NETWORK_SERVICES_PROVIDER_H_INCLUDED__

#include <MultiplexingProvider.h>

#include "VNSUtils.h"

namespace slim
{
  enum
  {
    eIPC_MSG_NEW_VNS_EVENT = C_USR_MSG_ID_FIRST + 2,
  };

/*!
 * @brief Vehicle Network Provider
 *
 * This provider enables access to vehicle Network services.
 *
 * @ingroup slim_VNSProvider
 */
class VNSProvider :
  public MultiplexingProvider
{
  enum
  {
    eTIMER_DEFAULT,
  };


  static const uint32_t s_qThreadId;
  static Mutex s_zInstanceMutex;
  static VNSProvider *s_pzInstance;
  static const slimAvailableServiceMaskT s_qSupportedServices;

  slim_ThreadT m_zThread;
  Mutex m_zMutex;
  slim_TimerT *m_pzTimer;
  mutable slimAvailableServiceMaskT m_qEnabledServices;

  bool m_isVehicleIntrfaceReady;

  static void threadMain
  (
    void *pData
  );

  VNSProvider();
  ~VNSProvider();
  bool initialize();
  void runEventLoop();
  void FormatData(VNS_Event_Struct *pzNewEvent, int64_t time);

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


#endif /* __VEHICLE_NETWORK_SERVICES_PROVIDER_H_INCLUDED__ */
