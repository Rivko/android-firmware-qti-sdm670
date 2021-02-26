/*
Copyright (c) 2011, 2014-2015 Qualcomm Technologies, Inc. All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.

Copyright (c) 2012 Qualcomm Atheros, Inc. All Rights Reserved.
Qualcomm Atheros Confidential and Proprietary.
*/
/**
@file
@brief GNSS / Sensor Interface Framework Support

This file defines the interface to accessing sensor data through Sensor1 API.

@ingroup slim_Sensor1Provider
*/


#ifndef __SLIM_SENSOR1_PROVIDER_H__
#define __SLIM_SENSOR1_PROVIDER_H__

#include "sns_sam_qmd_v01.h"
#include "sns_smgr_api_v01.h"

extern "C"
{
/* Sensor1 API has plain C bindings and is missing guards */
#include "sensor1.h"
}

#include <MultiplexingProvider.h>

//! @addtogroup slim_Sensor1Provider
//! @{

namespace slim
{

/*!
 * @brief Sensor1 Provider implementation
 *
 * Sensor1 Provider enables access of Sensor1 services (ADSP over SSC) on APSS.
 * This provider is a functional equivalent of SSC Provider on modem.
 *
 * @ingroup slim_Sensor1Provider
 */
class Sensor1Provider :
  public MultiplexingProvider
{
  enum
  {
    ePROVIDER_IPC_MSG_ID_REOPEN = C_USR_MSG_ID_FIRST,
    /**< SSR handling within SLIM context */
  };
  struct SensorState
  {
     bool                          running;            /* Is reporting currently? */
     uint32_t                      reporting_rate;     /* Current sampling rate in Hz */
     uint32_t                      batching_rate;      /* Current batching rate in Hz */
     float                         reporting_interval; /* Current sampling interval in ms */
     float                         batching_interval;  /* Current batching interval in ms */

     SensorState() :
       running(false),
       reporting_rate(0),
       batching_rate(0),
       reporting_interval(0),
       batching_interval(0)
     {
     }
  };

  struct MotionState
  {
     bool                        running;                  /* Motion data updates coming from Sensor1? */
     uint8_t                     amd_algo_instance_id;     /* AMD algorithm instance id. Needed to disable motion data reporting */
     uint8_t                     rmd_algo_instance_id;     /* RMD algorithm instance id. Needed to disable motion data reporting */
     sns_sam_motion_state_e_v01  amd;                      /* AMD algorithm state */
     sns_sam_motion_state_e_v01  rmd;                      /* RMD algorithm state */

     MotionState() :
       running(false),
       amd_algo_instance_id(0),
       rmd_algo_instance_id(0),
       amd(SNS_SAM_MOTION_UNKNOWN_V01),
       rmd(SNS_SAM_MOTION_UNKNOWN_V01)
     {
     }
  };

  struct PedometerState {
     bool                        running;                  /* Pedometer updates coming from Sensor1? */
     uint8_t                     algo_instance_id;         /* Pedometer algorithm instance id. Needed to disable motion data reporting */
     uint32_t                    step_count_threshold;     /* Step count threshold. */
     struct timespec             reset_time;               /* The latest reset time */

     PedometerState() :
       running(false),
       algo_instance_id(0),
       step_count_threshold(0),
       reset_time()
     {
       reset_time.tv_sec = 0;
       reset_time.tv_nsec = 0;
     }
  };
  static Mutex s_zInstanceMutex;
  static Sensor1Provider *s_pzInstance;

  slim_ThreadT      m_zThread;
  Mutex             m_zMutex;

  sensor1_handle_s *m_pClientHandle;     /* Opaque Sensor1 Client Handle */
  bool              m_uSpiRunning;       /* SPI updates coming from Sensor1? */
  int32_t           m_lSpiAlgoInstanceId;/* SPI Algorithm instance id. Needed to disable SPI reporting */
  MotionState       m_zMotionState;      /* All necessary for the motion state */
  PedometerState    m_zPedometerState;   /* All necessary for the pedometer */
  SensorState       m_zGyroState;        /* All necessary state for accel sensor */
  SensorState       m_zAccelState;       /* All necessary state for gyro sensor */
  SensorState       m_zBaroState;        /* All necessary state for gyro sensor */
  SensorState       m_zGyroTempState;    /* All necessary state for gyro temperature sensor */
  SensorState       m_zAccelTempState;   /* All necessary state for accel temperature sensor */
  SensorState       m_zMagCalibState;    /* All necessary state for calib mag sensor */
  SensorState       m_zMagUncalibState;  /* All necessary state for uncalib mag sensor */

  volatile sensor1_error_e m_eOpenResult;
  uint64_t          m_tServiceTxnIdMask;  /* Mask for SLIM transaction identifiers usage map */
  int32_t           m_lServiceTxnIds[64]; /* SLIM transaction identifiers */
  bool              m_uBatchMode;
private:

  /**
   * @brief Thread main function
   *
   * @param[in] pData User data pointer.
   */
  static void threadMain
  (
    void *pData
  );
  /**
  @brief Checks input sample quality.

  Function performs check of the input sample quality.

  @param[in] uSampleQuality SNS_SMGR_SVC service quality.

  @return true if valid sample, false otherwise
  */
  static bool checkSampleQuality
  (
    uint8_t uSampleQuality
  );

  /**
  @brief Maps service transaction identifier into local one.

  SLIM uses 32-bit transaction identifiers for matching requests and responses.
  Sensor1 API supports only 8-bit identifiers. This function attempts to
  map service transaction id into local transaction id.

  @param[in]  lServiceTxnId  SLIM service transaction identifier.
  @param[out] uLocalTxnIdOut Local transaction identifier.

  @retval true on success
  @retval false if no table space found

  @sa releaseLocalTxnId
  */
  bool allocateLocalTxnId
  (
    int32_t lServiceTxnId,
    uint8_t &uLocalTxnIdOut
  );
  /**
  @brief Maps local transaction identifier back into service one.

  SLIM uses 32-bit transaction identifiers for matching requests and responses.
  Sensor1 API supports only 8-bit identifiers. This function attempts to
  restore service transaction id from local transaction id.

  @param[in]  uLocalTxnId      Local transaction identifier.
  @param[out] lServiceTxnIdOut SLIM service transaction identifier.

  @retval true on success
  @retval false if entry has not been marked as used.

  @sa allocateLocalTxnId
  */
  bool releaseLocalTxnId
  (
    uint8_t lLocalTxnId,
    int32_t &lServiceTxnIdOut
  );

  slimErrorEnumT convert_sensor_samples
  (
    const sns_smgr_buffering_sample_index_s_v01 &zIndexData,
    const sns_smgr_buffering_sample_s_v01 *pzSamples,
    slimSensorTypeEnumT eSensorType,
    slimServiceEnumT eService
  );
  void process_smgr_resp
  (
    const sensor1_msg_header_s &msg_hdr,
    const void* msg_ptr
  );
  /**
  @brief Handler for SMGR service indications.

  Handler for Sensor1 SMGR (Sensor Manager) Service Indications. Sensor data
  input is handled here.

  @param[in] zMsgHdr Message header
  @param[in] pzData  Pointer to message data
  */
  void process_smgr_ind
  (
    const sensor1_msg_header_s &zMsgHdr,
    const void* pzData
  );
  /**
  @brief Handler for Sensor1 SAM Service Responses.

  @param[in] zMsgHdr Message header
  @param[in] pzData  Pointer to message data
  */
  void process_sam_resp
  (
    const sensor1_msg_header_s &zMsgHdr,
    const void* pzData
  );
  /**
  @brief Handler for Sensor1 SAM Service Indications.

  @param[in] zMsgHdr Message header
  @param[in] pzData  Pointer to message data
  */
  void process_amd_and_rmd_ind
  (
    const sensor1_msg_header_s &zMsgHdr,
    const void *pzData
  );
  /**
  @brief Handler for Sensor1 SAM Service Indications.

  @param[in] zMsgHdr Message header
  @param[in] pzData  Pointer to message data
  */
  void process_vmd_ind
  (
    const sensor1_msg_header_s &zMsgHdr,
    const void* pzData
  );
  /**
  @brief Handler for Sensor1 SAM Service Indications.

  @param[in] zMsgHdr Message header
  @param[in] pzData  Pointer to message data
  */
  void process_ped_ind
  (
    const sensor1_msg_header_s &zMsgHdr,
    const void *pzData
  );
  /**
  @brief Handler for Sensor1 SAM Service Time Indications.

  @param[in] zMsgHdr Message header
  @param[in] pzData  Pointer to message data
  */
  void process_time_resp
  (
    const sensor1_msg_header_s &zMsgHdr,
    const void *pzData
  );
  /**
  @brief Sensor1 interface callback method.

  Method routes call to provider's instance.

  @param[in] lCbData  Callback data. Provider instance identifier.
  @param[in] pzMsgHdr Sensor1 header block.
  @param[in] eMsgType Sensor1 message type.
  @param[in] pData    Sensor1 message payload.
  */
  static void sensor1NotifyDataCallback
  (
    intptr_t lCbData,
    sensor1_msg_header_s *pzMsgHdr,
    sensor1_msg_type_e eMsgType,
    void* pData
  );
  /**
  @brief Validate and dispatch Sensor1 message

  Method performs parameter validation and dispatches message to appropriate
  handler.

  @param[in] pzMsgHdr Sensor1 header block.
  @param[in] eMsgType Sensor1 message type.
  @param[in] pData    Sensor1 message payload.
  */
  void processSensor1Message
  (
    const sensor1_msg_header_s *pzMsgHdr,
    sensor1_msg_type_e eMsgType,
    const void* pData
  );
  void runEventLoop();
  /**
  @brief Constructs object

  Function performs partial initialization of object state.
  */
  Sensor1Provider();
  /**
  @brief Releases object data
  */
  ~Sensor1Provider();

  /**
  @brief Opens Sensor1 connection.

  Opens connection to Sensor1 API. The result can be one of the following:
  - Success
  - Partial success
  - Error

  @return true if result is success or partial success; false otherwise.
  */
  bool open();

  /**
  @brief Closes Sensor1 connection.

  Closes connection to Sensor1 API.

  @return true if operation is successful; false otherwise.
  */
  bool close();

  /**
  @brief Initializes internal structures for sensor provider.

  The method prepares provider for operation.
  @return true if operation is successful; false otherwise.
  */
  bool init();
protected:

  /**
  @brief Method for enabling or disabling motion data service.

  Function updates the current running status of the Motion Data (MD) algorithm
  from sensor provider. If MD is already reporting and it is requested to start
  again, this function is a nop. If MD is already stopped and it is requested
  to stop this function is a nop.

  @param[in] uEnable Flag that indicates if the service shall be enabled or
                     disabled.

  @return eSLIM_SUCCESS is operation succeeded.
  */
  virtual slimErrorEnumT doUpdateMotionDataStatus
  (
    bool uEnable
  );

  /**
  @brief Method for enabling or disabling pedometer.

  Function updates the current running status of pedometer updates from this
  sensor provider.

  @param[in] uEnable             Flag that indicates if the service shall be
                                 enabled or disabled.
  @param[in] uResetStepCount     true - reset step count,
                                 false - do not reset step count.
  @param[in] qStepCountThreshold 0 - keep the previous value or use default value,
                                 1 - if there is no previous value,
                                 N - report after every Nth step.

  @return eSLIM_SUCCESS is operation succeeded.
  */
  virtual slimErrorEnumT doUpdatePedometerStatus
  (
    bool uEnable,
    bool u_ResetStepCount,
    uint32_t q_StepCountThreshold
  );

  /**
  @brief Enable or disable Stationary Position Indicator (SPI) algorithm.

  Function updates the current running status of the Stationary Position
  Indicator (SPI) algorithm from sensor provider. If SPI is already reporting
  and it is requested to start again, this function is a nop. If SPI is already
  stopped and it is requested to stop this function is a nop.

  Note: Function should not be called from a time sensitive thread as this
        function may block for some time to start up sensors.

  @param[in] uRunning @a true - start SPI reporting; @a false - stop SPI
                      reporting

  @retval false On failure
  @retval true  On success
  */
  bool updateSPIStatus
  (
    bool uRunning
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

  /**
  @brief Method for enabling or disabling sensor service.

  Generic function to start/stop a sensor based on provided sampling rate,
  batching rate, mounted state, and sensor information using Sensor1 Buffering
  API.

  @todo The sampling frequency is a mere suggestion to the sensor1 daemon.
  Sensor 1 will stream at the requested minimum sampling frequency requested
  by all AP clients combined. So we do see cases where buffering API does not
  help and we get single sensor data for every indication. In that case should
  SLIM AP do the batching?

  @param[in] uEnable       Flag that indicates if the service shall be enabled
                           or disabled.
  @param[in] wReportRateHz Requested reporting rate in Hz.
  @param[in] wSampleCount  Requested sample count in one batch.
  @param[in] eService      Service to control.

  @return eSLIM_SUCCESS is operation succeeded.
  */
  virtual slimErrorEnumT doUpdateSensorStatus
  (
    bool uEnable,
    uint16_t wReportRateHz,
    uint16_t wSampleCount,
    slimServiceEnumT eService
  );
  /**
  @brief Handler for messages targeted for provider.

  Function handles messages sent via SLIM core to provider.

  @param[in] qMessageId      Message id.
  @param[in] qMsgPayloadSize Message size.
  @param[in] pMsgPayload     Pointer to the message payload.
  */
  virtual void handleProviderMessage
  (
    uint32_t qMessageId,
    uint32_t qMsgPayloadSize,
    const void *pMsgPayload
  );
public:
  /**
  @brief Returns SLIM provider interface

  Method constructs provider instance, initializes it, and returns SLIM
  provider interface.

  @return SLIM provider interface of 0 on error.
  */
  static slim_ServiceProviderInterfaceType *getServiceProvider();
  /**
  @brief Returns SLIM interface

  The method initializes singleton instance and returns SLIM provider
  interface.

  @return Provider object
  */
  static Sensor1Provider *getInstance();

  /**
  @brief Configures batch mode support

  @param[in] uFlag Flag to enable (true) or disable batch mode.
   */
  void setBatchMode
  (
    bool uFlag
  );
};

}

//! @}

#endif /* __SLIM_SENSOR1_PROVIDER_H__ */
