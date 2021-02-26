#ifndef SLIM_PROVIDER_VNW_VEHICLENETWORKMESSAGEPROCESSOR_H_
#define SLIM_PROVIDER_VNW_VEHICLENETWORKMESSAGEPROCESSOR_H_

/**
@file
@brief Vehicle Network message processors interface file.

This file declares message processing classes.

*/

/*
Copyright (c) 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/


#include <inttypes.h>
#include <slim_client_types.h>
#include <CanWrapper.h>
#include "CanFrameEvent.h"
#include "VehicleNetworkConfiguration.h"

namespace slim
{

class VehicleNetworkProvider;

/* ------------------------------------------------------------------------- */
/**
@brief Base class for message processing classes

Classes inherited from VehicleNetworkMessageProcessor class are to be used to
extend functionality of @sa VehicleNetworkProvider class, in particular
to implement processing of messages in provider's internal message processing
loop.

A separate message processor should be implemented for each service,
i.e. type of sensor. Then @sa CreateProcessor factory method should be updated
to create objects of newly declared class.

All classes inherited from @sa VehicleNetworkMessageProcessor must be declared
as friend classes in declaration of @sa VehicleNetworkProvider class.

*/
class VehicleNetworkMessageProcessor
{
public:
  /**
  @brief Class constructor

  Creates and initialises new instance of the class.

  @param[in] pzParent pointer to parent, i.e. instance of
             @sa VehicleNetworkProvider class

  */
  VehicleNetworkMessageProcessor(VehicleNetworkProvider* pzParent);

  /**
  @brief Class destructor

  Destroys current instance

  */
  virtual ~VehicleNetworkMessageProcessor();

  /**
  @brief Create new message processor

  This method creates new message processor based on the configuration
  object passed.

  @param[in] pzParent pointer to parent, i.e. instance of
             @sa VehicleNetworkProvider class
  @param[in] pzCfg configuration object, i.e class inherited
             from @sa SensorConfig
  @param[in] eService service that this message processor is to be created for
  */
  static VehicleNetworkMessageProcessor* CreateProcessor
   (
    VehicleNetworkProvider* pzParent,
    SensorConfig* pzCfg,
    slimServiceEnumT eService
   );

  virtual void OnNewCanFrameEvent(CanFrameEvent* pzEvent);
  virtual void OnNewTimerExpiryEvent(os_TimerExpiryType* pzEvent);
  virtual void OnNewIpcMessage(slim_IpcMsgT* pzMsg);
  virtual void ToString(char* pStr, size_t sLen)  = 0;

protected:
  void UpdateCanControllerTimeDelta(uint64_t tMCTime);
  uint64_t VnTimeToApTime(uint64_t tVnTime);

protected:
  VehicleNetworkProvider* m_pzParent;
};

/* ------------------------------------------------------------------------- */
/**
@brief Default Message Processor for accelerometer sensor implementation

This class implements processing of messages for configuration
eACCEL_CONFIG_DEFAULT.

Refer to @sa VehicleNetworkMessageProcessor class documentation for details.

*/
class VehicleNetworkDefaultAccelMP
  : public VehicleNetworkMessageProcessor
{
public:
  VehicleNetworkDefaultAccelMP(VehicleNetworkProvider* pzParent,
                               AccelConfig* pzCfg);
  virtual void OnNewTimerExpiryEvent(os_TimerExpiryType* pzEvent);
  virtual void ToString(char* pStr, size_t sLen);

protected:
  AccelConfig* m_pzCfg;
};

/* ------------------------------------------------------------------------- */
/**
@brief CONF1 Message Processor for accelerometer sensor implementation

This class implements processing of messages for configuration
eACCEL_CONFIG_CONF1.

Refer to @sa VehicleNetworkMessageProcessor class documentation for details.

*/
class VehicleNetworkCONF1AccelMP
  : public VehicleNetworkMessageProcessor
{
public:
  slimVehicleSensorDataStructT mZData;
  VehicleNetworkCONF1AccelMP(VehicleNetworkProvider* pzParent,
                             AccelCONF1Config* pzCfg);
  virtual void OnNewCanFrameEvent(CanFrameEvent* pzEvent);
  virtual void ToString(char* pStr, size_t sLen);

protected:
  AccelCONF1Config* m_pzCfg;
};

/* ------------------------------------------------------------------------- */
/**
@brief Default Message Processor for gyro sensor implementation

This class implements processing of messages for configuration
eGYRO_CONFIG_DEFAULT.

Refer to @sa VehicleNetworkMessageProcessor class documentation for details.

*/
class VehicleNetworkDefaultGyroMP
  : public VehicleNetworkMessageProcessor
{
public:
  VehicleNetworkDefaultGyroMP(VehicleNetworkProvider* pzParent,
                              GyroConfig* pzCfg);
  virtual void OnNewTimerExpiryEvent(os_TimerExpiryType* pzEvent);
  virtual void ToString(char* pStr, size_t sLen);

protected:
  GyroConfig* m_pzCfg;
};

/* ------------------------------------------------------------------------- */
/**
@brief CONF1 Message Processor for gyro sensor implementation

This class implements processing of messages for configuration
eGYRO_CONFIG_CONF1.

Refer to @sa VehicleNetworkMessageProcessor class documentation for details.

*/
class VehicleNetworkCONF1GyroMP
  : public VehicleNetworkMessageProcessor
{
public:
  slimVehicleSensorDataStructT mZData;
  VehicleNetworkCONF1GyroMP(VehicleNetworkProvider* pzParent,
                            GyroCONF1Config* pzCfg);
  virtual void OnNewCanFrameEvent(CanFrameEvent* pzEvent);
  virtual void ToString(char* pStr, size_t sLen);

protected:
  GyroCONF1Config* m_pzCfg;
};

/* ------------------------------------------------------------------------- */
/**
@brief Default Message Processor for odometry sensor implementation

This class implements processing of messages for configuration
eODOMETRY_CONFIG_DEFAULT.

Refer to @sa VehicleNetworkMessageProcessor class documentation for details.

*/
class VehicleNetworkDefaultOdometryMP
  : public VehicleNetworkMessageProcessor
{
public:
  VehicleNetworkDefaultOdometryMP(VehicleNetworkProvider* pzParent,
                                  OdometryConfig* pzCfg);
  virtual void OnNewTimerExpiryEvent(os_TimerExpiryType* pzEvent);
  virtual void ToString(char* pStr, size_t sLen);

protected:
  OdometryConfig* m_pzCfg;
};

/* ------------------------------------------------------------------------- */
/**
@brief CONF1 Message Processor for odometry sensor implementation

This class implements processing of messages for configuration
eODOMETRY_CONFIG_CONF1.

Refer to @sa VehicleNetworkMessageProcessor class documentation for details.

*/
class VehicleNetworkCONF1OdometryMP
  : public VehicleNetworkMessageProcessor
{
public:
  slimVehicleOdometryDataStructT mZData;
  VehicleNetworkCONF1OdometryMP(VehicleNetworkProvider* pzParent,
                                OdometryCONF1Config* pzCfg);
  virtual void OnNewCanFrameEvent(CanFrameEvent* pzEvent);
  virtual void ToString(char* pStr, size_t sLen);

protected:
  OdometryCONF1Config* m_pzCfg;
};



/* ------------------------------------------------------------------------- */
/**
@brief Default Message Processor for speed sensor implementation

This class implements processing of messages for configuration
eSPEED_CONFIG_DEFAULT.

Refer to @sa VehicleNetworkMessageProcessor class documentation for details.

*/
class VehicleNetworkDefaultSpeedMP
  : public VehicleNetworkMessageProcessor
{
public:
  VehicleNetworkDefaultSpeedMP(VehicleNetworkProvider* pzParent,
                               SpeedConfig* pzCfg);
  virtual void OnNewTimerExpiryEvent(os_TimerExpiryType* pzEvent);
  virtual void ToString(char* pStr, size_t sLen);

protected:
  SpeedConfig* m_pzCfg;
};

/* ------------------------------------------------------------------------- */
/**
@brief CONF1 Message Processor for Speed sensor implementation

This class implements processing of messages for configuration
eSPEED_CONFIG_CONF1.

Refer to @sa VehicleNetworkMessageProcessor class documentation for details.

*/
class VehicleNetworkCONF1SpeedMP
  : public VehicleNetworkMessageProcessor
{
public:
  slimVehicleMotionDataStructT mZData;
  VehicleNetworkCONF1SpeedMP(VehicleNetworkProvider* pzParent,
                             SpeedCONF1Config* pzCfg);
  virtual void OnNewCanFrameEvent(CanFrameEvent* pzEvent);
  virtual void ToString(char* pStr, size_t sLen);

protected:
  SpeedCONF1Config* m_pzCfg;
};

/* ------------------------------------------------------------------------- */
/**
@brief Default Message Processor for DWS sensor implementation

This class implements processing of messages for configuration
eDWS_CONFIG_DEFAULT.

Refer to @sa VehicleNetworkMessageProcessor class documentation for details.

*/
class VehicleNetworkDefaultDwsMP
  : public VehicleNetworkMessageProcessor
{
public:
  VehicleNetworkDefaultDwsMP(VehicleNetworkProvider* pzParent,
                               DwsConfig* pzCfg);
  virtual void OnNewTimerExpiryEvent(os_TimerExpiryType* pzEvent);
  virtual void ToString(char* pStr, size_t sLen);

protected:
  DwsConfig* m_pzCfg;
};

/* ------------------------------------------------------------------------- */
/**
@brief CONF1 Message Processor for DWS sensor implementation

This class implements processing of messages for configuration
eDWS_CONFIG_CONF1.

Refer to @sa VehicleNetworkMessageProcessor class documentation for details.

*/
class VehicleNetworkCONF1DwsMP
  : public VehicleNetworkMessageProcessor
{
public:
  slimVehicleMotionDataStructT mZData;
  VehicleNetworkCONF1DwsMP(VehicleNetworkProvider* pzParent,
                             DwsCONF1Config* pzCfg);
  virtual void OnNewCanFrameEvent(CanFrameEvent* pzEvent);
  virtual void ToString(char* pStr, size_t sLen);

protected:
  DwsCONF1Config* m_pzCfg;
};

/* ------------------------------------------------------------------------- */
/**
@brief Default Message Processor for Gear sensor implementation

This class implements processing of messages for configuration
eGEAR_CONFIG_DEFAULT.

Refer to @sa VehicleNetworkMessageProcessor class documentation for details.

*/
class VehicleNetworkDefaultGearMP
  : public VehicleNetworkMessageProcessor
{
public:
  VehicleNetworkDefaultGearMP(VehicleNetworkProvider* pzParent,
                               GearConfig* pzCfg);
  virtual void OnNewTimerExpiryEvent(os_TimerExpiryType* pzEvent);
  virtual void ToString(char* pStr, size_t sLen);

protected:
  GearConfig* m_pzCfg;
};

/* ------------------------------------------------------------------------- */
/**
@brief CONF1 Message Processor for Gear sensor implementation

This class implements processing of messages for configuration
eGEAR_CONFIG_CONF1.

Refer to @sa VehicleNetworkMessageProcessor class documentation for details.

*/
class VehicleNetworkCONF1GearMP
  : public VehicleNetworkMessageProcessor
{
public:
  slimVehicleMotionDataStructT mZData;
  VehicleNetworkCONF1GearMP(VehicleNetworkProvider* pzParent,
                             GearCONF1Config* pzCfg);
  virtual void OnNewCanFrameEvent(CanFrameEvent* pzEvent);
  virtual void ToString(char* pStr, size_t sLen);

protected:
  GearCONF1Config* m_pzCfg;
};

}

#endif /* SLIM_PROVIDER_VNW_VEHICLENETWORKMESSAGEPROCESSOR_H_ */
