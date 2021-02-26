/*
Copyright (c) 2014-2015 Qualcomm Technologies, Inc. All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/
/**
 * @file
 * @brief QMI-LOC Client Listener implementation file
 *
 * QMI LOC client listener is interface to modem sensor clients (SAP & SAMLITE)
 * Sensor data requests and sensor data injection is through QMI LOC transport.
 *
 * @ingroup slim_APSS
 */

/* Platform library headers */
#include <inttypes.h>
#include <stdlib.h>

#include <QLClientListener.h>
#include <location_service_v02.h>
#include <loc_api_sync_req.h>
#include <LocDualContext.h>

#include <slim_core.h>
#include <slim_processor.h>

//! @addtogroup slim_APSS
//! @{

/* This unit logs as QMI-LOC component. */
#undef SLIM_LOG_LOCAL_MODULE
//! @brief Component logs as QMI-LOC Module
#define SLIM_LOG_LOCAL_MODULE SLIM_LOG_MOD_QL

//! @}

namespace slim
{

//! @brief LOC API Event Mask for processing service events.
const LOC_API_ADAPTER_EVENT_MASK_T QLClientListener::s_qMask =
    LOC_API_ADAPTER_BIT_SENSOR_STATUS |
    LOC_API_ADAPTER_BIT_REQUEST_TIME_SYNC |
    LOC_API_ADAPTER_BIT_MOTION_CTRL |
    LOC_API_ADAPTER_BIT_PEDOMETER_CTRL;
    //LOC_API_ADAPTER_BIT_REPORT_SPI

//! @brief Instance-locking mutex
Mutex QLClientListener::s_zInstanceMutex;

//! @brief Singleton instance
QLClientListener* QLClientListener::s_pzInstance = 0;

/**
@brief Constructor

Function class initialization

@param[in] eProvider Preferred provider
*/
QLClientListener::QLClientListener
(
  slimServiceProviderEnumT eProvider
):
    LBSAdapterBase(
        s_qMask,
        LocDualContext::getLocFgContext(
            LocDualContext::mLocationHalName)),
    ClientListener(),
    m_qClientId(SLIM_CLIENT_MPSS),
    m_uClientTxnId(1),
    m_eProvider(eProvider)
{
  SLIM_LOGD("QLClientListener created.");

  memset((void*)m_lSensorSessionInfo, 0, sizeof(m_lSensorSessionInfo));

  slimOpenFlagsT qOpenFlags = 0;
  slimOpen(m_qClientId, qOpenFlags);
}

/**
@brief Destructor

When object is destroyed, all services are disabled and SLIM client connection
is closed.
*/
QLClientListener::~QLClientListener()
{
  SLIM_LOGD( "Calling slim_Close");

  slimClose();
}

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
ClientListener* QLClientListener::get
(
  slimServiceProviderEnumT eProvider
)
{
  SLIM_LOG_ENTRY();

  MutexLock _l(s_zInstanceMutex);
  if (0 == s_pzInstance)
  {
    s_pzInstance = new QLClientListener(eProvider);
  }
  return s_pzInstance;
}

#ifdef FEATURE_LOCTECH_SLIM_DEBUG
/**
@brief Releases object

The method releases class object if it has been previously instantiated.
*/
void QLClientListener::disable()
{
  QLClientListener *pzInstance;
  {
    MutexLock _l(s_zInstanceMutex);
    pzInstance = s_pzInstance;
    s_pzInstance = 0;
  }
  if (0 != pzInstance)
  {
    delete pzInstance;
  }
}
#endif

/**
@brief Called after sub system restart for session recovery

Function Basic handling after SSR if there is an active session and sensor providers are engaged.
*/
void QLClientListener::handleEngineUpEvent()
{
  // Nothing
}

/**
@brief Handling of client connection termination.

This function is called whenever QMI-LOC connection termination is detected.
If there are enabled services, this functions requests service disable.
*/
void QLClientListener::handleEngineDownEvent()
{
  SLIM_LOG_CALLFLOW();
  //If any sensor session in progress send stop request to
  //the sensor providers

  for (int i = 0; i <= eSLIM_SERVICE_LAST; i++)
  {
    SLIM_LOGD("mSensorSessionInfo[%d]=%d", i, m_lSensorSessionInfo[i]);
    if (m_lSensorSessionInfo[i] == 1)
    {
      switch (i)
      {
      case eSLIM_SERVICE_SENSOR_ACCEL:
      case eSLIM_SERVICE_SENSOR_GYRO:
      case eSLIM_SERVICE_SENSOR_ACCEL_TEMP:
      case eSLIM_SERVICE_SENSOR_GYRO_TEMP:
      case eSLIM_SERVICE_SENSOR_MAG_CALIB:
      case eSLIM_SERVICE_SENSOR_MAG_UNCALIB:
        SensorRequest sensorRequest;
        memset(&sensorRequest, 0, sizeof(sensorRequest));
        sensorRequest.requestType = (SensorType) i;
        sensorRequest.requestValue = REQUEST_STOP;
        requestSensorData(sensorRequest);
        m_lSensorSessionInfo[i] = 0;
        break;
      case eSLIM_SERVICE_PEDOMETER:
        PedometerRequest pedometerRequest;
        memset(&pedometerRequest, 0, sizeof(pedometerRequest));
        pedometerRequest.requestValue = REQUEST_STOP;
        requestPedometerData(pedometerRequest);
        m_lSensorSessionInfo[i] = 0;
        break;
      case eSLIM_SERVICE_MOTION_DATA:
        SensorRequest motionDataRequest;
        memset(&motionDataRequest, 0, sizeof(motionDataRequest));
        motionDataRequest.requestType = (SensorType) i;
        motionDataRequest.requestValue = REQUEST_STOP;
        requestMotionData(motionDataRequest);
        m_lSensorSessionInfo[i] = 0;
        break;
      default:
        break;
      }
    }
  }
}

/**
 * @brief Enabling or disabling of sensor data streaming for MPSS client
 *
 * Function handles enabling or disabling of sensor data streaming for MPSS
 * client
 *
 * @param[in] request Request parameters for sensor streaming
 *
 * @return Operation result.
 * @retval true On success.
 */
bool QLClientListener::requestSensorData
(
  const SensorRequest &request
)
{
  bool returnValue = true;

  SLIM_CB_CTX_PUSH("QL");
  SLIM_LOG_ENTRY();
  slimEnableSensorDataRequestStructT z_Request;
  memset(&z_Request, 0, sizeof(z_Request));

  SLIM_MASK_SET(z_Request.enableConf.providerFlags, m_eProvider);

  switch (request.requestType)
  {
  case ACCEL:
    z_Request.sensor = eSLIM_SERVICE_SENSOR_ACCEL;
    break;
  case GYRO:
    z_Request.sensor = eSLIM_SERVICE_SENSOR_GYRO;
    break;
  case ACCEL_TEMPERATURE:
    z_Request.sensor = eSLIM_SERVICE_SENSOR_ACCEL_TEMP;
    break;
  case GYRO_TEMPERATURE:
    z_Request.sensor = eSLIM_SERVICE_SENSOR_GYRO_TEMP;
    break;
  case MAG_CALIB:
    z_Request.sensor = eSLIM_SERVICE_SENSOR_MAG_CALIB;
    break;
  case MAG_UNCALIB:
    z_Request.sensor = eSLIM_SERVICE_SENSOR_MAG_UNCALIB;
    break;
  default:
    returnValue = false;
    SLIM_LOGI("%s: Request for unknown sensor type", __func__);
    break;
  }

  if (returnValue)
  {
    z_Request.enableConf.enable = request.requestValue;
    z_Request.sampleCount = request.requestRate.samplesPerBatch;
    z_Request.reportRate = request.requestRate.batchesPerSecond;

    SLIM_LOGD(
        "Requesting sensor data: service=%d flags=%08" PRIx32 " (provider=%d) enable=%d",
        z_Request.sensor, z_Request.enableConf.providerFlags, m_eProvider,
        z_Request.enableConf.enable);

    MutexLock _lock(m_zStateMutex);
    m_lSensorSessionInfo[z_Request.sensor] = z_Request.enableConf.enable;
    processSensorDataRequest(m_uClientTxnId, z_Request);
  }
  SLIM_LOG_EXIT(%d, 0);
  SLIM_CB_CTX_POP();
  return returnValue;
}

/**
 * @brief Enabling or disabling of pedometer data streaming for MPSS client
 *
 * Function handles enabling or disabling of pedometer data streaming for MPSS
 * client
 *
 * @param[in] request Request parameters for pedometer streaming
 *
 * @return Operation result.
 * @retval true On success.
 */
bool QLClientListener::requestPedometerData
(
  const PedometerRequest &request
)
{
  SLIM_CB_CTX_PUSH("QL");
  SLIM_LOG_ENTRY();

  slimEnablePedometerRequestStructT z_Request;
  memset(&z_Request, 0, sizeof(z_Request));

  SLIM_MASK_SET(z_Request.enableConf.providerFlags,
      QLClientListener::m_eProvider);

  z_Request.enableConf.enable = request.requestValue;
  z_Request.resetStepCount_valid = request.resetStepCountValid;
  z_Request.resetStepCount = request.resetStepCount;
  z_Request.stepCountThreshold_valid = request.stepCountThresholdValid;
  z_Request.stepCountThreshold = request.stepCountThreshold;

  {
    MutexLock _lock(m_zStateMutex);
    m_lSensorSessionInfo[eSLIM_SERVICE_PEDOMETER] = z_Request.enableConf.enable;
    processPedometerDataRequest(m_uClientTxnId, z_Request);
  }

  SLIM_LOG_EXIT(%d, 0);
  SLIM_CB_CTX_POP();
  return true;
}

/**
 * @brief Enabling or disabling of motion data streaming for MPSS client
 *
 * Function handles enabling or disabling of motion data streaming for MPSS
 * client
 *
 * @param[in] request Request parameters for motion data streaming
 *
 * @return Operation result.
 * @retval true On success.
 */
bool QLClientListener::requestMotionData
(
  const SensorRequest &request
)
{
  SLIM_CB_CTX_PUSH("QL");
  SLIM_LOG_ENTRY();

  slimEnableMotionDataRequestStructT z_Request;
  memset(&z_Request, 0, sizeof(z_Request));

  SLIM_MASK_SET(z_Request.enableConf.providerFlags, m_eProvider);

  z_Request.enableConf.enable = request.requestValue;

  {
    MutexLock _lock(m_zStateMutex);
    m_lSensorSessionInfo[eSLIM_SERVICE_MOTION_DATA] = z_Request.enableConf.enable;
    processMotionDataRequest(m_uClientTxnId, z_Request);
  }

  SLIM_LOG_EXIT(%d, 0);
  SLIM_CB_CTX_POP();
  return true;
}

/**
 * @brief Request time sync data for MPSS client
 *
 * Function requests time sync data for MPSS client. SLIM core will report
 * service time using "common" base (SLIM common).
 *
 * @param[in] zTimeRequest Request data for time sync
 *
 * @return Operation result.
 * @retval true On success.
 */
bool QLClientListener::requestTimeData
(
  const TimeRequest &zTimeRequest
)
{
  SLIM_CB_CTX_PUSH("QL");
  SLIM_LOG_ENTRY();

  SLIM_LOGD("Requesting time for provider=%d", m_eProvider);

  TimeData timeData;
  memset(&timeData, 0, sizeof(timeData));

  timeData.clientTxTime = zTimeRequest.clientTxTime;
  timeData.remoteProcRxTime = timeData.remoteProcTxTime = (uint32_t)slim_TimeTickGetMilliseconds();

  SLIM_LOGI("Sending Time Data to LocApi");
  injectTimeData(timeData);

  SLIM_LOG_EXIT(%d, 0);
  SLIM_CB_CTX_POP();
  return true;
}

/**
 * @brief Handle sensor data injection
 *
 * Function handles sensor data injection
 *
 * @param[in] zSensorData Sensor data
 *
 * @return Operation result.
 * @retval true On success.
 */
bool QLClientListener::injectSensorData
(
  const SensorDataBundle &zSensorData
)
{
  struct QLInjectSensorData: public LocMsg
  {
    LBSApiBase* const m_pzLBSApi;
    SensorDataBundle const m_zDataBundle;
    QLInjectSensorData
    (
      LBSApiBase* pzLBSApi,
      const SensorDataBundle &zDataBundle
    ) :
        LocMsg(),
        m_pzLBSApi(pzLBSApi),
        m_zDataBundle(zDataBundle)
    {
    }
    virtual void proc() const
    {
      SLIM_LOGV("Calling mLBSApi->injectSensorData");
      m_pzLBSApi->injectSensorData(m_zDataBundle);
    }
  };
  sendMsg(new QLInjectSensorData(mLBSApi, zSensorData));

  return true;
}

/**
 * @brief Handle pedometer data injection
 *
 * Function handles pedometer data injection
 *
 * @param[in] zPedometerData Pedometer data
 *
 * @return Operation result.
 * @retval true On success.
 */
bool QLClientListener::injectPedometerData
(
  const PedometerData &zPedometerData
)
{
  struct QLInjectPedometerData: public LocMsg
  {
    LBSApiBase* const m_pzLBSApi;
    PedometerData const m_pzPedometerData;
    QLInjectPedometerData
    (
      LBSApiBase* pzLBSApi,
      const PedometerData &zPedometerData
    ) :
        LocMsg(),
        m_pzLBSApi(pzLBSApi),
        m_pzPedometerData(zPedometerData)
    {
    }
    virtual void proc() const
    {
      SLIM_LOGV("Calling mLBSApi->injectPedometerData");
      m_pzLBSApi->injectPedometerData(m_pzPedometerData);
    }
  };
  sendMsg(new QLInjectPedometerData(mLBSApi, zPedometerData));

  return true;
}

/**
 * @brief Handle motion data injection
 *
 * Function handles motion data injection
 *
 * @param[in] zMotionData Motion data
 *
 * @return Operation result.
 * @retval true On success.
 */
bool QLClientListener::injectMotionData
(
  const MotionData &zMotionData
)
{
  struct QLInjectMotionData: public LocMsg
  {
    LBSApiBase* const m_pzLBSApi;
    MotionData const m_zMotionData;
    QLInjectMotionData
    (
      LBSApiBase* pzLBSApi,
      const MotionData &zMotionData
    ) :
        LocMsg(),
        m_pzLBSApi(pzLBSApi),
        m_zMotionData(zMotionData)
    {
    }
    virtual void proc() const
    {
      SLIM_LOGV("Calling mLBSApi->injectMotionData");
      m_pzLBSApi->injectMotionData(m_zMotionData);
    }
  };
  sendMsg(new QLInjectMotionData(mLBSApi, zMotionData));

  return true;
}

/**
 * @brief Handle time sync data injection
 *
 * Function handles time sync data injection
 *
 * @param[in] zTimeData Time sync data
 *
 * @return Operation result.
 * @retval true On success.
 */
bool QLClientListener::injectTimeData
(
  const TimeData &zTimeData
)
{
  struct QLInjectTimeData: public LocMsg
  {
    LBSApiBase* mLBSApi;
    TimeData mTimeData;
    QLInjectTimeData
    (
      LBSApiBase* lbsApi,
      const TimeData &timeData
    ) :
        LocMsg(),
        mLBSApi(lbsApi),
        mTimeData(timeData)
    {
    }
    virtual void proc() const
    {
      SLIM_LOGV("Calling mLBSApi->injectTimeData");
      mLBSApi->injectTimeData(mTimeData);
    }
  };
  sendMsg(new QLInjectTimeData(mLBSApi, zTimeData));

  return true;
}

/**
 * @brief Handler for all sensor service messages
 *
 * Function is handler for all sensor service messages
 *
 * @param[in] zHeader Message header
 * @param[in] zData   Message payload
 */
void QLClientListener::handleSensorData
(
  const slimMessageHeaderStructT &zHeader,
  const slimSensorDataStructT &zData
)
{

  if (eSLIM_TIME_SOURCE_COMMON != zData.timeSource)
  {
    // When SLIM has failed to compute common time for a sample, this sample
    // is dropped not to confuse consumer with a value that can't be normalized
    // later.
    SLIM_LOGD("Ignoring sensor data");
    return;
  }

  SensorDataBundle dataBundle;
  memset(&dataBundle, 0, sizeof(dataBundle));

  /*Flags are currently not set by either provider so assumed to be 0
   Also no 1-1 mapping between SLIM and QMI LOC flags*/
  dataBundle.flags = 0;

  SLIM_LOGV("Received sensor data");
  switch (zHeader.service)
  {
  case eSLIM_SERVICE_SENSOR_ACCEL:
    dataBundle.sensorType = ACCEL;
    break;
  case eSLIM_SERVICE_SENSOR_ACCEL_TEMP:
    dataBundle.sensorType = ACCEL_TEMPERATURE;
    break;
  case eSLIM_SERVICE_SENSOR_GYRO:
    dataBundle.sensorType = GYRO;
    break;
  case eSLIM_SERVICE_SENSOR_GYRO_TEMP:
    dataBundle.sensorType = GYRO_TEMPERATURE;
    break;
  case eSLIM_SERVICE_SENSOR_MAG_CALIB:
    dataBundle.sensorType = MAG_CALIB;
    dataBundle.flags |= CALIBRATED_DATA;
    break;
  case eSLIM_SERVICE_SENSOR_MAG_UNCALIB:
    dataBundle.sensorType = MAG_UNCALIB;
    break;
  default:
    break;
  }
  dataBundle.timeOfFirstSample = zData.timeBase;

  /*Time source is set to unspecified for modem clients so that time sync
    can be done, cannot be assumed to be common*/
  dataBundle.timeSource = UNSPECIFIED;
  dataBundle.sensorDataLength = zData.samples_len;

  SLIM_LOGV("Received sensor-%d data with len-%" PRIu8,
      zData.sensorType, zData.samples_len);

  for(uint32_t i=0;i<zData.samples_len;i++)
  {
    dataBundle.samples[i].timeOffset =
            slim_TimeToMilliseconds(zData.samples[i].sampleTimeOffset);
    dataBundle.samples[i].xAxis = zData.samples[i].sample[0];
    dataBundle.samples[i].yAxis= zData.samples[i].sample[1];
    dataBundle.samples[i].zAxis = zData.samples[i].sample[2];

    SLIM_LOGV("Received data with time offset-%d, data-(%f,%f,%f)",
             dataBundle.samples[i].timeOffset,
             dataBundle.samples[i].xAxis,
             dataBundle.samples[i].yAxis,
             dataBundle.samples[i].zAxis);
  }

  SLIM_LOGV("Sending Sensor Data to LocApi.");
  injectSensorData(dataBundle);
}

/**
 * @brief Handler for all pedometer service messages
 *
 * Function is handler for all pedometer service messages
 *
 * @param[in] zHeader Message header
 * @param[in] zData   Message payload
 */
void QLClientListener::handlePedometerData
(
  const slimMessageHeaderStructT &zHeader,
  const slimPedometerDataStructT &zData
)
{
  SLIM_UNUSED(zHeader);
  SLIM_LOGD("Received pedometer inject indication ");

  if (eSLIM_TIME_SOURCE_COMMON != zData.timeSource)
  {
    // When SLIM has failed to compute common time for a sample, this sample
    // is dropped not to confuse consumer with a value that can't be normalized
    // later.
    SLIM_LOGD("Ignoring pedometer data");
    return;
  }

  PedometerData pedometerData;
  memset(&pedometerData, 0, sizeof(pedometerData));

  // Time source in QMI-LOC is always unspecified, as SLIM time always differs
  // from MPSS time.
  pedometerData.timeSource = UNSPECIFIED;
  pedometerData.timestamp = zData.timestamp;
  pedometerData.timeInterval = zData.timeInterval;

  pedometerData.stepCount = zData.stepCount;

  pedometerData.stepConfidenceValid = zData.stepConfidence_valid;
  pedometerData.stepConfidence = zData.stepConfidence;

  pedometerData.stepCountUncertaintyValid = zData.stepCountUncertainty_valid;
  pedometerData.stepCountUncertainty = zData.stepCountUncertainty;

  pedometerData.stepRateValid = zData.stepRate_valid;
  pedometerData.stepRate = zData.stepRate;


  SLIM_LOGV("Received Pedometer data,"
           "step_confidence = %" PRIu8 ", step_count = %" PRIu32 ", "
           "step_count_uncertainity = %f, step_rate = %4.2f Hz",
           zData.stepConfidence,
           zData.stepCount, zData.stepCountUncertainty,
           zData.stepRate);

  SLIM_LOGV("Sending Pedometer Data to LocApi");
  injectPedometerData(pedometerData);
}

/**
 * @brief Handler for all motion data service messages
 *
 * Function is handler for all motion data service messages
 *
 * @param[in] zHeader Message header
 * @param[in] zData   Message payload
 */
void QLClientListener::handleMotionData
(
  const slimMessageHeaderStructT &zHeader,
  const slimMotionDataStructT &zData
)
{
  SLIM_UNUSED(zHeader);

  SLIM_LOGD("Received motion data inject indication");

  MotionData motionData;
  memset(&motionData, 0, sizeof(motionData));

  motionData.state = (MotionStateType)zData.motionState;
  motionData.mode = (MotionModeType)zData.motionMode;

  motionData.probability = zData.probabilityOfState;
  motionData.age = zData.age;
  motionData.timeout = zData.timeout;

  SLIM_LOGV("Motion data update sent with state = %d and probability = %f",
            zData.motionState, zData.probabilityOfState);

  SLIM_LOGV("Sending Motion Data to LocApi");
  injectMotionData(motionData);
}

/**
 * @brief Update service state according to result code.
 *
 * @param zHeader Response header
 *
 * @todo Check if the timed re-request is required when there is an operation
 *       failure. For example, when "enable" fails due to service availability.
 */
void QLClientListener::handleServiceResponse
(
  const slimMessageHeaderStructT &zHeader
)
{
  if (eSLIM_MESSAGE_TYPE_RESPONSE == zHeader.msgType &&
      (eSLIM_MESSAGE_ID_SENSOR_DATA_ENABLE_RESP == zHeader.msgId ||
       eSLIM_MESSAGE_ID_PEDOMETER_ENABLE_RESP == zHeader.msgId ||
       eSLIM_MESSAGE_ID_MOTION_DATA_ENABLE_RESP == zHeader.msgId) &&
      zHeader.service >= 0 && zHeader.service < SLIM_SERVICE_COUNT)
  {
    MutexLock _lock(m_zStateMutex);
    if (0 != m_lSensorSessionInfo[zHeader.service] &&
        eSLIM_SUCCESS != zHeader.msgError)
    {
      m_lSensorSessionInfo[zHeader.service] = 0;

      SLIM_LOGW("Failed to enable service %d with error %d",
          zHeader.service, zHeader.msgError);
    }
  }
  else
  {
    ClientListener::handleServiceResponse(zHeader);
  }
}

/*!
 * @brief Handles SLIM open result
 *
 * Handles SLIM open result. In case of error, the process is terminated.
 *
 * @param[in] zHeader Response header.
 */
void QLClientListener::handleOpenResponse
(
  const slimMessageHeaderStructT &zHeader
)
{
  if (eSLIM_SUCCESS != zHeader.msgError)
  {
    SLIM_LOGF("Failed to open connection with error %d", zHeader.msgError);
    exit(1);
  }
  SLIM_LOGD("QMI-LOC client connection opened");
}

}; /* namespace slim */

//! @}
