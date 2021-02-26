/*
Copyright (c) 2011, 2014-2015 Qualcomm Technologies, Inc. All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.

Copyright (c) 2012 Qualcomm Atheros, Inc. All Rights Reserved.
Qualcomm Atheros Confidential and Proprietary.
*/
/**
@file
@brief GNSS / Sensor Interface Framework Support

This file defines the implementation for sensor provider interface using
Qualcomm proprietary DSPS Sensor1 API.

@ingroup slim_Sensor1Provider
*/

#include <sys/ioctl.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
extern "C"
{
/* Sensor1 API has plain C bindings and is missing guards */
#include "sensor1.h"
}
#include "SlimSensor1Provider.h"
#include "slim_client_types.h"

#include "sns_sam_qmd_v01.h"
#include "sns_sam_amd_v01.h"
#include "sns_sam_rmd_v01.h"
#include "sns_sam_vmd_v01.h"
#include "sns_sam_ped_v01.h"
#include "sns_smgr_api_v01.h"
#include "sns_time_api_v02.h"
#include "sns_reg_api_v02.h"


//! @addtogroup slim_Sensor1Provider
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/* This unit logs as Sensor1 component. */
#undef SLIM_LOG_LOCAL_MODULE
//! @brief Logging module constant for Sensor1 Provider.
#define SLIM_LOG_LOCAL_MODULE SLIM_LOG_MOD_S1

//! @brief Age of the motion data in milliseconds at the time of injection.
#define MOTION_DATA_TLV_AGE 100
//! @brief Motion data timeout value in milliseconds
#define MOTION_DATA_TLV_TIMEOUT 65535

//! @brief Number conversion from Q16 to floating point
#define UNIT_CONVERT_Q16             (1.0f / 65536.0f)
//! @brief Number conversion from Q16 to floating point with sign inversion
#define UNIT_CONVERT_Q16_NEG         (-1.0f / 65536.0f)
//! @brief Number conversion from Q16 to floating point (uTesla)
#define UNIT_CONVERT_MAGNETIC_FIELD      (100 * UNIT_CONVERT_Q16)
//! @brief Number conversion from Q16 to floating point with sign inversion
//!        (uTesla)
#define UNIT_CONVERT_MAGNETIC_FIELD_NEG  (100 * UNIT_CONVERT_Q16_NEG)
//! @brief Constant for undefined sensor value.
#define SENSOR_TYPE_UNDEFINED ((slimSensorTypeEnumT)-1)

//! @brief Supported services by Sensor1
#define SLIM_SENSOR1_SERVICE_MASK ((1 <<  eSLIM_SERVICE_SENSOR_ACCEL) |\
                                   (1 <<  eSLIM_SERVICE_SENSOR_ACCEL_TEMP) |\
                                   (1 <<  eSLIM_SERVICE_SENSOR_GYRO) |\
                                   (1 <<  eSLIM_SERVICE_SENSOR_GYRO_TEMP) |\
                                   (1 <<  eSLIM_SERVICE_MOTION_DATA) |\
                                   (1 <<  eSLIM_SERVICE_PEDOMETER) |\
                                   (1 <<  eSLIM_SERVICE_SENSOR_MAG_CALIB) |\
                                   (1 <<  eSLIM_SERVICE_SENSOR_MAG_UNCALIB) |\
                                   (1 <<  eSLIM_SERVICE_SENSOR_BARO))

using namespace slim;

Mutex Sensor1Provider::s_zInstanceMutex;
Sensor1Provider *Sensor1Provider::s_pzInstance;

/* Static function declarations */
// static void process_time_resp( sensor1_msg_header_s *msg_hdr, void *msg_ptr );

/**
 * @brief Thread main function
 *
 * @param[in] pData User data pointer.
 */
void Sensor1Provider::threadMain
(
  void *pData
)
{
  Sensor1Provider *pzProvider = reinterpret_cast<Sensor1Provider*>(pData);
  slim_IpcStart(THREAD_ID_SENSOR1);
  /* Service support is enabled */
  pzProvider->m_qServiceMask = 0;
  pzProvider->runEventLoop();
}

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
bool Sensor1Provider::allocateLocalTxnId
(
  int32_t lServiceTxnId,
  uint8_t &uLocalTxnIdOut
)
{
  uint64_t tMask;
  uint8_t uI;

  for (uI = 0, tMask = 1; uI< ARR_SIZE(m_lServiceTxnIds); ++uI, tMask <<= 1)
  {
    if (0 == (m_tServiceTxnIdMask & tMask))
    {
      m_tServiceTxnIdMask |= tMask;
      uLocalTxnIdOut = uI;
      m_lServiceTxnIds[uI] = lServiceTxnId;
      return true;
    }
  }
  return false;
}

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
bool Sensor1Provider::releaseLocalTxnId
(
  uint8_t uLocalTxnId,
  int32_t &lServiceTxnIdOut
)
{
  uint64_t tMask = (uint64_t)1 << uLocalTxnId;
  if (0 != (m_tServiceTxnIdMask & tMask))
  {
    m_tServiceTxnIdMask ^= tMask;
    lServiceTxnIdOut = m_lServiceTxnIds[uLocalTxnId];
    return true;
  }
  return false;
}


/**
@brief Constructs object

Function performs partial initialization of object state.
*/
Sensor1Provider::Sensor1Provider() :
    MultiplexingProvider(SLIM_PROVIDER_SENSOR1, false, 0),
    m_zThread(),
    m_zMutex(),
    m_pClientHandle(0),
    m_uSpiRunning(false),
    m_lSpiAlgoInstanceId(-1),
    m_zMotionState(),
    m_zPedometerState(),
    m_zGyroState(),
    m_zAccelState(),
    m_zBaroState(),
    m_zGyroTempState(),
    m_zAccelTempState(),
    m_zMagCalibState(),
    m_zMagUncalibState(),
    m_eOpenResult(SENSOR1_SUCCESS),
    m_tServiceTxnIdMask(0),
    m_lServiceTxnIds(),
    m_uBatchMode(true)
{
  memset(m_lServiceTxnIds, 0, sizeof(m_lServiceTxnIds));
}

/**
@brief Releases object data
*/
Sensor1Provider::~Sensor1Provider()
{
}

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
slimErrorEnumT Sensor1Provider::doUpdateMotionDataStatus
(
  bool uEnable
)
{
  sensor1_error_e eError;
  bool uMessagesSent = true;

  /* Turn on AMD and RMD reporting */
  if (uEnable && !m_zMotionState.running)
  {
    /* Message header */
    sensor1_msg_header_s zReqHdr;
    sns_sam_qmd_enable_req_msg_v01 *pzQmdEnableReq= 0;

    SLIM_LOGD("Turning on AMD reporting");

    zReqHdr.service_number = SNS_SAM_AMD_SVC_ID_V01;
    zReqHdr.txn_id = 0;
    zReqHdr.msg_id = SNS_SAM_AMD_ENABLE_REQ_V01;
    zReqHdr.msg_size = sizeof(*pzQmdEnableReq);

    eError = sensor1_alloc_msg_buf(
        m_pClientHandle,
        sizeof(*pzQmdEnableReq),
        (void**) &pzQmdEnableReq);
    if (SENSOR1_SUCCESS != eError)
    {
      SLIM_LOGE("sensor1_alloc_msg_buf add returned %d", eError);
      return eSLIM_ERROR_ALLOCATION;
    }

    /* Only report on new AMD events */
    pzQmdEnableReq->report_period = 0;

    /* Send across Sensor1 Interface */
    if ((eError = sensor1_write(m_pClientHandle, &zReqHdr, pzQmdEnableReq))
        != SENSOR1_SUCCESS)
    {
      /* Error so we deallocate QMI message */
      sensor1_free_msg_buf(m_pClientHandle, pzQmdEnableReq);
      SLIM_LOGE("sensor1_write() error: %u", eError);
      uMessagesSent = false;
    }

    SLIM_LOGD("Turning on RMD reporting");

    zReqHdr.service_number = SNS_SAM_RMD_SVC_ID_V01;
    zReqHdr.txn_id = 0;
    zReqHdr.msg_id = SNS_SAM_RMD_ENABLE_REQ_V01;
    zReqHdr.msg_size = sizeof(*pzQmdEnableReq);

    eError = sensor1_alloc_msg_buf(
        m_pClientHandle,
        sizeof(*pzQmdEnableReq),
        (void**) &pzQmdEnableReq);
    if (SENSOR1_SUCCESS != eError)
    {
      SLIM_LOGE("sensor1_alloc_msg_buf add returned %d", eError);
      return eSLIM_ERROR_ALLOCATION;
    }

    /* Only report on new RMD events */
    pzQmdEnableReq->report_period = 0;

    /* Send across Sensor1 Interface */
    if ((eError = sensor1_write(m_pClientHandle, &zReqHdr, pzQmdEnableReq))
        != SENSOR1_SUCCESS)
    {
      /* Error so we deallocate QMI message */
      sensor1_free_msg_buf(m_pClientHandle, pzQmdEnableReq);
      SLIM_LOGE("sensor1_write() error: %u", eError);
      uMessagesSent = false;
    }
  }
  /* Turn off AMD amd RMD reporting */
  else if (!uEnable && m_zMotionState.running)
  {
    /* Message header */
    sensor1_msg_header_s zReqHdr;
    sns_sam_qmd_disable_req_msg_v01* pzQmdDisableReq = 0;

    SLIM_LOGD("Turning off AMD reporting");

    zReqHdr.service_number = SNS_SAM_AMD_SVC_ID_V01;
    zReqHdr.txn_id = 0;
    zReqHdr.msg_id = SNS_SAM_AMD_DISABLE_REQ_V01;
    zReqHdr.msg_size = sizeof(*pzQmdDisableReq);
    eError = sensor1_alloc_msg_buf(
        m_pClientHandle,
        sizeof(*pzQmdDisableReq),
        (void**) &pzQmdDisableReq);

    if (SENSOR1_SUCCESS != eError)
    {
      SLIM_LOGE("sensor1_alloc_msg_buf add returned %d", eError);
      return eSLIM_ERROR_ALLOCATION;
    }

    pzQmdDisableReq->instance_id = m_zMotionState.amd_algo_instance_id;

    /* Send across Sensor1 Interface */
    if ((eError = sensor1_write(m_pClientHandle, &zReqHdr, pzQmdDisableReq))
        != SENSOR1_SUCCESS)
    {
      /* Error so we deallocate QMI message */
      sensor1_free_msg_buf(m_pClientHandle, pzQmdDisableReq);
      SLIM_LOGE("sensor1_write() error: %u", eError);
      uMessagesSent = false;
    }

    SLIM_LOGD("Turning off RMD reporting");

    zReqHdr.service_number = SNS_SAM_RMD_SVC_ID_V01;
    zReqHdr.txn_id = 0;
    zReqHdr.msg_id = SNS_SAM_RMD_DISABLE_REQ_V01;
    zReqHdr.msg_size = sizeof(*pzQmdDisableReq);
    eError = sensor1_alloc_msg_buf(
        m_pClientHandle,
        sizeof(*pzQmdDisableReq),
        (void**) &pzQmdDisableReq);

    if (SENSOR1_SUCCESS != eError)
    {
      SLIM_LOGE("sensor1_alloc_msg_buf add returned %d", eError);
      return eSLIM_ERROR_ALLOCATION;
    }

    pzQmdDisableReq->instance_id = m_zMotionState.rmd_algo_instance_id;

    /* Send across Sensor1 Interface */
    if ((eError = sensor1_write(m_pClientHandle, &zReqHdr, pzQmdDisableReq))
        != SENSOR1_SUCCESS)
    {
      /* Error so we deallocate QMI message */
      sensor1_free_msg_buf(m_pClientHandle, pzQmdDisableReq);
      SLIM_LOGE("sensor1_write() error: %u", eError);
      uMessagesSent = false;
    }
  }
  /* Enabling if already enabled or disabling while already disabled. */
  else
  {
    return eSLIM_SUCCESS;
  }

  if (uMessagesSent)
  {
    m_zMotionState.running = uEnable;
  }
  m_zMotionState.amd = SNS_SAM_MOTION_UNKNOWN_V01;
  m_zMotionState.rmd = SNS_SAM_MOTION_UNKNOWN_V01;

  return eSLIM_SUCCESS;
}

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
slimErrorEnumT Sensor1Provider::doUpdatePedometerStatus
(
  bool uEnable,
  bool uResetStepCount,
  uint32_t qStepCountThreshold
)
{
    void* sam_req = NULL;
    sensor1_error_e error;

    /* Message header */
    sensor1_msg_header_s sam_req_hdr;
    sam_req_hdr.service_number = SNS_SAM_PED_SVC_ID_V01;
    sam_req_hdr.txn_id = 0;

    /* Turn off Pedometer reporting */
    if( (!uEnable && m_zPedometerState.running) ||
        (uEnable && m_zPedometerState.running &&
         qStepCountThreshold != m_zPedometerState.step_count_threshold))
    {
        SLIM_LOGD("Turning off Pedometer reporting");

        sam_req_hdr.msg_id = SNS_SAM_PED_DISABLE_REQ_V01;
        sam_req_hdr.msg_size = sizeof( sns_sam_ped_disable_req_msg_v01 );
        error = sensor1_alloc_msg_buf( m_pClientHandle,
                                       sizeof(sns_sam_ped_disable_req_msg_v01),
                                       (void**)&sam_req );

        if ( SENSOR1_SUCCESS != error )
        {
            SLIM_LOGE("sensor1_alloc_msg_buf add returned %d", error);
            return eSLIM_ERROR_ALLOCATION;
        }

        sns_sam_ped_disable_req_msg_v01* disable_ped_sam_req = (sns_sam_ped_disable_req_msg_v01*)sam_req;
        disable_ped_sam_req->instance_id = m_zPedometerState.algo_instance_id;

        /* Send across Sensor1 Interface */
        if ( (error = sensor1_write( m_pClientHandle, &sam_req_hdr,
                                     sam_req )) != SENSOR1_SUCCESS )
        {
            /* Error so we deallocate QMI message */
            sensor1_free_msg_buf( m_pClientHandle, sam_req );
            SLIM_LOGE("sensor1_write() error: %u", error );
            return eSLIM_ERROR_QMI_INTERFACE;
        }
        else
        {
            m_zPedometerState.running = false;
        }
    }
    /* Resetting the step count. */
    else if( uEnable && m_zPedometerState.running && uResetStepCount )
    {
        SLIM_LOGD("Resetting the Pedometer step count.");

        sam_req_hdr.msg_id = SNS_SAM_PED_RESET_REQ_V01;
        sam_req_hdr.msg_size = sizeof( sns_sam_ped_reset_req_msg_v01 );
        error = sensor1_alloc_msg_buf( m_pClientHandle,
                                       sizeof(sns_sam_ped_reset_req_msg_v01),
                                       (void**)&sam_req );

        if ( SENSOR1_SUCCESS != error )
        {
            SLIM_LOGE("sensor1_alloc_msg_buf add returned %d", error);
            return eSLIM_ERROR_ALLOCATION;
        }

        sns_sam_ped_reset_req_msg_v01* reset_ped_sam_req = (sns_sam_ped_reset_req_msg_v01*)sam_req;
        reset_ped_sam_req->instance_id = m_zPedometerState.algo_instance_id;

        /* Send across Sensor1 Interface */
        if ( (error = sensor1_write( m_pClientHandle, &sam_req_hdr,
                                     sam_req )) != SENSOR1_SUCCESS )
        {
            /* Error so we deallocate QMI message */
            sensor1_free_msg_buf( m_pClientHandle, sam_req );
            SLIM_LOGE("sensor1_write() error: %u", error );
            return eSLIM_ERROR_QMI_INTERFACE;
        }
    }
    /* Turn on Pedometer reporting */
    if( uEnable && !m_zPedometerState.running )
    {
        SLIM_LOGD("Turning on Pedometer reporting");

        sam_req_hdr.msg_id = SNS_SAM_PED_ENABLE_REQ_V01;
        sam_req_hdr.msg_size = sizeof( sns_sam_ped_enable_req_msg_v01 );
        error = sensor1_alloc_msg_buf( m_pClientHandle,
                                       sizeof(sns_sam_ped_enable_req_msg_v01),
                                       (void**)&sam_req );
        if ( SENSOR1_SUCCESS != error )
        {
            SLIM_LOGE("sensor1_alloc_msg_buf add returned %d", error);
            return eSLIM_ERROR_ALLOCATION;
        }

        sns_sam_ped_enable_req_msg_v01* enable_ped_sam_req = (sns_sam_ped_enable_req_msg_v01*)sam_req;
        /* SLIM supports only event based pedometer reports. */
        enable_ped_sam_req->report_period = 0;
        if (qStepCountThreshold > 0)
        {
            enable_ped_sam_req->step_count_threshold_valid = true;
            enable_ped_sam_req->step_count_threshold = qStepCountThreshold;
        }

        /* Send across Sensor1 Interface */
        if ( (error = sensor1_write( m_pClientHandle, &sam_req_hdr,
                                     sam_req )) != SENSOR1_SUCCESS )
        {
            /* Error so we deallocate QMI message */
            sensor1_free_msg_buf( m_pClientHandle, sam_req );
            SLIM_LOGE("sensor1_write() error: %u", error );
            return eSLIM_ERROR_QMI_INTERFACE;
        }
        else
        {
            m_zPedometerState.running = uEnable;
        }
    }

    return eSLIM_SUCCESS;
}

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
bool Sensor1Provider::updateSPIStatus
(
  bool uRunning
)
{
    void* sam_req = NULL;
    sensor1_error_e eError;

    /* Message header */
    sensor1_msg_header_s req_hdr;
    req_hdr.service_number = SNS_SAM_VMD_SVC_ID_V01;
    req_hdr.txn_id = 0;

    /* Turn on spi reporting */
    if( uRunning && !m_uSpiRunning )
    {
        SLIM_LOGD("Turning on spi reporting");

        req_hdr.msg_id = SNS_SAM_VMD_ENABLE_REQ_V01;
        req_hdr.msg_size = sizeof( sns_sam_qmd_enable_req_msg_v01 );

        eError = sensor1_alloc_msg_buf( m_pClientHandle,
                                       sizeof(sns_sam_qmd_enable_req_msg_v01),
                                       (void**)&sam_req );
        if ( SENSOR1_SUCCESS != eError )
        {
            SLIM_LOGE("sensor1_alloc_msg_buf add returned %d", eError);
            return false;
        }

        sns_sam_qmd_enable_req_msg_v01* enable_sam_req = (sns_sam_qmd_enable_req_msg_v01*)sam_req;
        enable_sam_req->report_period = 0;               /* Only report on new spi events */
    }
    /* Turn off spi reporting */
    else if( !uRunning && m_uSpiRunning )
    {
        SLIM_LOGD("Turning off spi reporting");

        req_hdr.msg_id = SNS_SAM_VMD_DISABLE_REQ_V01;
        req_hdr.msg_size = sizeof( sns_sam_qmd_disable_req_msg_v01 );
        eError = sensor1_alloc_msg_buf( m_pClientHandle,
                                       sizeof(sns_sam_qmd_disable_req_msg_v01),
                                       (void**)&sam_req );

        if ( SENSOR1_SUCCESS != eError )
        {
            SLIM_LOGE("sensor1_alloc_msg_buf add returned %d", eError);
            return false;
        }

        sns_sam_qmd_disable_req_msg_v01* disable_sam_req = (sns_sam_qmd_disable_req_msg_v01*)sam_req;
        disable_sam_req->instance_id = m_lSpiAlgoInstanceId;
    }
    /* Enabling if already enabled or disabling while already disabled. */
    else
    {
        return true;
    }

    /* Send across Sensor1 Interface */
    if ( (eError = sensor1_write( m_pClientHandle, &req_hdr,
                                 sam_req )) != SENSOR1_SUCCESS )
    {
        /* Error so we deallocate QMI message */
        sensor1_free_msg_buf( m_pClientHandle, sam_req );
        SLIM_LOGE("sensor1_write() error: %u", eError );
        return false;
    }
    else
    {
        m_uSpiRunning = uRunning;
    }

    return true;
}

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
slimErrorEnumT Sensor1Provider::doUpdateSensorStatus
(
  bool uEnable,
  uint16_t wReportRateHz,
  uint16_t wSampleCount,
  slimServiceEnumT eService
)
{
  SensorState* sensor_state;
  uint32_t sampling_rate;

  const char* sensor_str;
  uint8_t sns1_sensor_id;
  uint8_t sns1_data_type;

  MutexLock _l(m_zMutex);

  sampling_rate = wSampleCount * wReportRateHz;
  switch (eService)
  {
  case eSLIM_SERVICE_SENSOR_ACCEL:
    sensor_str = "accel";
    sns1_sensor_id = SNS_SMGR_ID_ACCEL_V01;
    sns1_data_type = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
    sensor_state = &m_zAccelState;
    break;

  case eSLIM_SERVICE_SENSOR_GYRO:
    sensor_str = "gyro";
    sns1_sensor_id = SNS_SMGR_ID_GYRO_V01;
    sns1_data_type = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
    sensor_state = &m_zGyroState;
    break;

  case eSLIM_SERVICE_SENSOR_ACCEL_TEMP:
    sensor_str = "accel_temperature";
    sns1_sensor_id = SNS_SMGR_ID_ACCEL_V01;
    sns1_data_type = SNS_SMGR_DATA_TYPE_SECONDARY_V01;
    sensor_state = &m_zAccelTempState;
    break;

  case eSLIM_SERVICE_SENSOR_GYRO_TEMP:
    sensor_str = "gyro_temperature";
    sns1_sensor_id = SNS_SMGR_ID_GYRO_V01;
    sns1_data_type = SNS_SMGR_DATA_TYPE_SECONDARY_V01;
    sensor_state = &m_zGyroTempState;
    break;

  case eSLIM_SERVICE_SENSOR_BARO:
    sensor_str = "pressure";
    sns1_sensor_id = SNS_SMGR_ID_PRESSURE_V01;
    sns1_data_type = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
    sensor_state = &m_zBaroState;
    break;

  case eSLIM_SERVICE_SENSOR_MAG_CALIB:
    sensor_str = "mag_calib";
    sns1_sensor_id = SNS_SMGR_ID_MAG_V01;
    sns1_data_type = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
    sensor_state = &m_zMagCalibState;
    break;

  case eSLIM_SERVICE_SENSOR_MAG_UNCALIB:
    sensor_str = "mag_uncalib";
    sns1_sensor_id = SNS_SMGR_ID_MAG_V01;
    sns1_data_type = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
    sensor_state = &m_zMagUncalibState;
    break;

  default:
    SLIM_LOGE("Unknown Sensor Type %d", eService);
    return eSLIM_ERROR_BAD_ARGUMENT;
  }

  /* No sensor state to use. */
  if (0 == sensor_state)
  {
    SLIM_LOGE("No %s sensor state provided to start/stop", sensor_str);
    return eSLIM_ERROR_BAD_ARGUMENT;
  }

  /* No State Change */
  if (!uEnable && !sensor_state->running)
  {
    /* Sensors stay Off */
    return eSLIM_SUCCESS;
  }
  /* Sensors stay On but no change in sensor sampling rate */
  else if (uEnable &&
           sensor_state->running &&
           sampling_rate == sensor_state->reporting_rate)
  {
    return eSLIM_SUCCESS;
  }

  sensor1_msg_header_s req_hdr;
  sns_smgr_buffering_req_msg_v01* smgr_req = NULL;

  sensor1_error_e error = sensor1_alloc_msg_buf(m_pClientHandle,
      sizeof(sns_smgr_buffering_req_msg_v01), (void**) &smgr_req);

  if (SENSOR1_SUCCESS != error)
  {
    SLIM_LOGE("sensor1_alloc_msg_buf add returned %d", error);
    return eSLIM_ERROR_ALLOCATION;
  }

  /* Message header */
  req_hdr.service_number = SNS_SMGR_SVC_ID_V01;
  req_hdr.msg_id = SNS_SMGR_BUFFERING_REQ_V01;
  req_hdr.msg_size = sizeof(sns_smgr_buffering_req_msg_v01);
  req_hdr.txn_id = 0;

  if (uEnable)
  {
    uint32_t sensor1_reporting_rate;
    SENSOR1_Q16_FROM_FLOAT(sensor1_reporting_rate, (uint32_t )wReportRateHz);

    /* Turn on sensor */
    if (!sensor_state->running)
    {
      SLIM_LOGD(
          "Turning on %s reporting, reporting-rate %" PRIu16 " (q16 %" PRIu32
          "), sampling-rate %" PRIu32,
          sensor_str, wReportRateHz, sensor1_reporting_rate, sampling_rate);
    }
    /* Sensor sampling rate change */
    else if (sensor_state->running)
    {
      SLIM_LOGD("Changing sampling rate on %s reporting", sensor_str);
    }
    sensor_state->reporting_rate = sampling_rate;
    sensor_state->batching_rate = wReportRateHz;
    sensor_state->reporting_interval = 1000.0f / sampling_rate;
    sensor_state->batching_interval = 1000.0f / wReportRateHz;

    smgr_req->ReportId = eService;
    smgr_req->Action = SNS_SMGR_BUFFERING_ACTION_ADD_V01;

    //Default behavior is to stream sensor data independent of whether screen is on/off
    smgr_req->notify_suspend_valid = true;
    smgr_req->notify_suspend.proc_type = SNS_PROC_APPS_V01;
    smgr_req->notify_suspend.send_indications_during_suspend = true;

    smgr_req->ReportRate = sensor1_reporting_rate;
    smgr_req->Item_len = 1;
    smgr_req->Item[0].SensorId = sns1_sensor_id;
    smgr_req->Item[0].DataType = sns1_data_type;
    smgr_req->Item[0].Decimation = SNS_SMGR_DECIMATION_RECENT_SAMPLE_V01;
    smgr_req->Item[0].SampleQuality = 0;
    smgr_req->Item[0].SamplingRate = sampling_rate;

    /* Full calibration for calibrated magnetometer service */
    if (eSLIM_SERVICE_SENSOR_MAG_CALIB == eService)
    {
      smgr_req->Item[0].Calibration = SNS_SMGR_CAL_SEL_FULL_CAL_V01;
    }
    else
    {
      smgr_req->Item[0].Calibration = SNS_SMGR_CAL_SEL_FACTORY_CAL_V01;
    }
  }
  /* Turn off sensor */
  else if (!uEnable && sensor_state->running)
  {
    SLIM_LOGD("Turning off %s reporting", sensor_str);
    smgr_req->ReportId = eService;
    smgr_req->Action = SNS_SMGR_BUFFERING_ACTION_DELETE_V01;
    smgr_req->ReportRate = 0;
    smgr_req->Item_len = 0;
  }

  /* Clear out any old samples that may be lingering from previous starts/stops. */

  /* Send across Sensor1 Interface */
  if ((error = sensor1_write(m_pClientHandle, &req_hdr, smgr_req))
      != SENSOR1_SUCCESS)
  {
    /* Error so we deallocate QMI message */
    sensor1_free_msg_buf(m_pClientHandle, smgr_req);
    SLIM_LOGE("sensor1_write() error: %u", error);
    return eSLIM_ERROR_QMI_INTERFACE;
  }
  else
  {
    /* Update running state after performing MSI to make the function generic */
    sensor_state->running = uEnable;
  }
  return eSLIM_SUCCESS;
}

/**
@brief Handler for messages targeted for provider.

Function handles messages sent via SLIM core to provider.

@param[in] qMessageId      Message id.
@param[in] qMsgPayloadSize Message size.
@param[in] pMsgPayload     Pointer to the message payload.
*/
void Sensor1Provider::handleProviderMessage
(
  uint32_t qMessageId,
  uint32_t qMsgPayloadSize,
  const void *pMsgPayload
)
{
  switch (qMessageId)
  {
  default:
    MultiplexingProvider::handleProviderMessage(qMessageId, qMsgPayloadSize, pMsgPayload);
    break;
  }
}

/*===========================================================================
FUNCTION    process_smgr_resp

DESCRIPTION
  Handler for Sensor1 SMGR (Sensor Manager) Service Responses.

DEPENDENCIES
   N/A

RETURN VALUE
   N/A

SIDE EFFECTS
   N/A

===========================================================================*/
void Sensor1Provider::process_smgr_resp
(
  const sensor1_msg_header_s &msg_hdr,
  const void* msg_ptr
)
{
    SLIM_LOGV("msg_id = %d txn_id = %d", msg_hdr.msg_id, msg_hdr.txn_id );
    bool error = false;

    if ( SNS_SMGR_BUFFERING_RESP_V01 == msg_hdr.msg_id )
    {
        sns_smgr_buffering_resp_msg_v01* smgr_resp = (sns_smgr_buffering_resp_msg_v01*)msg_ptr;
        slimErrorEnumT e_Error = eSLIM_SUCCESS;
        slimServiceEnumT e_Service = eSLIM_SERVICE_NONE;

        if ( smgr_resp->Resp.sns_result_t != SNS_RESULT_SUCCESS_V01 || smgr_resp->Resp.sns_err_t != SENSOR1_SUCCESS )
        {
            SLIM_LOGE("Result: %u, Error: %u",
                     smgr_resp->Resp.sns_result_t, smgr_resp->Resp.sns_err_t );
            error = true;
        }

        if ( smgr_resp->AckNak_valid &&
             smgr_resp->AckNak != SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 &&
             smgr_resp->AckNak != SNS_SMGR_RESPONSE_ACK_MODIFIED_V01 )
        {
            SLIM_LOGE("%d Error: %u Reason: %u", smgr_resp->ReportId,
                     smgr_resp->AckNak, smgr_resp->ReasonPair[0].Reason );
            error = true;
        }
        SLIM_LOGV("ReportId: %u Resp: %u", smgr_resp->ReportId,
                 smgr_resp->AckNak  );

        if(error)
        {
            e_Error = eSLIM_ERROR_INTERNAL;
            e_Service = static_cast<slimServiceEnumT>(smgr_resp->ReportId);
        }
        //! @todo notify response TO CORE
    }
    else
    {
        SLIM_LOGE("Received unexpected smgr resp msg_id = %d",
                  msg_hdr.msg_id);
    }
}

/**
@brief Checks input sample quality.

Function performs check of the input sample quality.

@param[in] uSampleQuality SNS_SMGR_SVC service quality.

@return true if valid sample, false otherwise
*/
bool Sensor1Provider::checkSampleQuality
(
  uint8_t uSampleQuality
)
{
  bool uRetVal = false;
  switch (uSampleQuality)
  {
  case SNS_SMGR_ITEM_QUALITY_INVALID_FAILED_SENSOR_V01:
  case SNS_SMGR_ITEM_QUALITY_INVALID_NOT_READY_V01:
  case SNS_SMGR_ITEM_QUALITY_INVALID_SUSPENDED_V01:
    uRetVal = false;
    break;
  default:
    uRetVal = true;
    break;
  }
  return uRetVal;
}

/**
@brief Converts and dispatches barometer samples in this batch to the client.

@param[in] zIndexData
@param[in] pzSamples
@param[in] eSensorType
@param[in] eServiceType

*/

slimErrorEnumT Sensor1Provider::convert_sensor_samples
(
  const sns_smgr_buffering_sample_index_s_v01 &zIndexData,
  const sns_smgr_buffering_sample_s_v01 *pzSamples,
  slimSensorTypeEnumT eSensorType,
  slimServiceEnumT eServiceType
)
{
  /* We support maximum of 50 samples */
  const uint8_t uSampleCount = zIndexData.SampleCount > SLIM_SENSOR_MAX_SAMPLE_SETS ?
      SLIM_SENSOR_MAX_SAMPLE_SETS : zIndexData.SampleCount;
  const size_t qLastSampleIndex = zIndexData.FirstSampleIdx + uSampleCount - 1;

  if (0 == uSampleCount)
  {
      SLIM_LOGE("Send3AxisSamples: No sample data");
      return eSLIM_SUCCESS;
  }

  slimSensorDataStructT zSensorData;
  memset(&zSensorData, 0, sizeof(zSensorData));
  zSensorData.sensorType = eSensorType;

  /* Store the received timestamp in milliseconds */
  zSensorData.timeBase =
          slim_TimeToMillisecondsFloor(zIndexData.FirstSampleTimestamp);
  zSensorData.timeSource = eSLIM_TIME_SOURCE_UNSPECIFIED;

  /* Calculate offset in clock ticks between time base and first sensor sample */
  int32_t lTotalTimeOffset =
          zIndexData.FirstSampleTimestamp - slim_TimeToClockTicks(zSensorData.timeBase);
  zSensorData.samples_len = uSampleCount;

  SLIM_LOGV("Received sensor-%d data with sample count-%" PRIu8,
           zSensorData.sensorType,
           uSampleCount);

  for (size_t qIndexIn = zIndexData.FirstSampleIdx, qIndexOut = 0;
       qIndexIn <= qLastSampleIndex;
       qIndexIn++, qIndexOut++)
  {
    slimSensorSampleStructT &zSampleOut = zSensorData.samples[qIndexOut];
    const sns_smgr_buffering_sample_s_v01 &zSampleIn = pzSamples[qIndexIn];
    /* Report contains the offset from timestamps of previous sample in report.
       We need to report the offset to first timestamp of report. */
    lTotalTimeOffset += zSampleIn.TimeStampOffset;

    if (checkSampleQuality(zSampleIn.Quality))
    {
      zSampleOut.sampleTimeOffset = lTotalTimeOffset;

      /* ACCEL: 3 axes, each in meter/second squared (m/s2)
         GYRO: 3 axes, each in radian/second (rad/s)
      */
      if (eSLIM_SENSOR_TYPE_ACCEL == zSensorData.sensorType ||
          eSLIM_SENSOR_TYPE_GYRO == zSensorData.sensorType)
      {
        /* Convert from SAE to Android co-ordinates and scale
           x' = y; y' = x; z' = -z; sample indexes 0: X, 1: Y, 2: Z */
        zSampleOut.sample[0] = zSampleIn.Data[1] * UNIT_CONVERT_Q16;
        zSampleOut.sample[1] = zSampleIn.Data[0] * UNIT_CONVERT_Q16;
        zSampleOut.sample[2] = zSampleIn.Data[2] * UNIT_CONVERT_Q16_NEG;
      }
      /* MAG: 3 axes, each in uTesla */
      else if (eSLIM_SENSOR_TYPE_MAGNETOMETER == zSensorData.sensorType)
      {
        /* Convert from SAE to Android co-ordinates and scale
           x' = y; y' = x; z' = -z; sample indexes 0: X, 1: Y, 2: Z */
        zSampleOut.sample[0] = zSampleIn.Data[1] * UNIT_CONVERT_MAGNETIC_FIELD;
        zSampleOut.sample[1] = zSampleIn.Data[0] * UNIT_CONVERT_MAGNETIC_FIELD;
        zSampleOut.sample[2] = zSampleIn.Data[2] * UNIT_CONVERT_MAGNETIC_FIELD_NEG;
      }
      /* ACCEL TEMP: 1 axis, in Celsius
         GYRO TEMP: 1 axis, in Celsius
         PRESSURE (BARO): 1 axis, in hectopascal (hPa)
      */
      else
      {
        zSampleOut.sample[0] = zSampleIn.Data[0] * UNIT_CONVERT_Q16;
      }
    }
  }

  IF_SLIM_LOGV
  {
    SLIM_LOGV("Sensor1 data of sensor %d with data len-%" PRIu8,
            zSensorData.sensorType, zSensorData.samples_len);

    for(size_t i=0; i<zSensorData.samples_len; i++)
    {
      const slimSensorSampleStructT &zSample = zSensorData.samples[i];
      SLIM_LOGV("[%" PRIuPTR "] offset-%" PRIu32 ", data-(%.6f,%.6f,%.6f)",
        i,
        zSample.sampleTimeOffset,
        zSample.sample[0],
        zSample.sample[1],
        zSample.sample[2]);
    }
  }

  return routeIndication(eServiceType, zSensorData);
}

/**
@brief Handler for SMGR service indications.

Handler for Sensor1 SMGR (Sensor Manager) Service Indications. Sensor data
input is handled here.

@param[in] zMsgHdr Message header
@param[in] pzData  Pointer to message data
*/
void Sensor1Provider::process_smgr_ind
(
  const sensor1_msg_header_s &zMsgHdr,
  const void* pzData
)
{
  if (SNS_SMGR_BUFFERING_IND_V01 == zMsgHdr.msg_id)
  {
    const sns_smgr_buffering_ind_msg_v01 &zSmgrInd =
        *(const sns_smgr_buffering_ind_msg_v01 *) pzData;

    SLIM_LOGV(
        "ReportId: %" PRIu8 " Indices: %" PRIu32 ", Samples: %" PRIu32,
        zSmgrInd.ReportId,
        zSmgrInd.Indices_len,
        zSmgrInd.Samples_len);

    /* Indication should contain only one type of sensor data per report id
     but we go through all of the indixes to make sure.
     */
    for (uint32_t qIndex = 0; qIndex < zSmgrInd.Indices_len; qIndex++)
    {
      const sns_smgr_buffering_sample_index_s_v01 &zIndexData =
          zSmgrInd.Indices[qIndex];
      if (0 == zIndexData.SampleCount)
      {
        continue;
      }

      size_t qLastSampleIndex = zIndexData.FirstSampleIdx
          + zIndexData.SampleCount - 1;
      slimSensorTypeEnumT eSensorType = SENSOR_TYPE_UNDEFINED;

      SLIM_LOGV("buffering-index = %" PRIu32 " sensor-id = %" PRIu8 " "
          "first-sample-idx = %" PRIu8 " sample-cnt = %" PRIu8 " "
          "first sample time stamp = %" PRIu32,
          qIndex, zIndexData.SensorId,
          zIndexData.FirstSampleIdx, zIndexData.SampleCount,
          zIndexData.FirstSampleTimestamp);

      if ((zIndexData.FirstSampleIdx <= qLastSampleIndex) &&
          (qLastSampleIndex < zSmgrInd.Samples_len))
      {
        switch (zIndexData.SensorId)
        {
        case SNS_SMGR_ID_ACCEL_V01:
          if (m_zAccelState.running)
          {
            if (SNS_SMGR_DATA_TYPE_PRIMARY_V01 == zIndexData.DataType)
            {
              SLIM_LOGV("Got sensor - accel");
              eSensorType = eSLIM_SENSOR_TYPE_ACCEL;
            }
            else if (SNS_SMGR_DATA_TYPE_SECONDARY_V01 == zIndexData.DataType)
            {
              SLIM_LOGV("Got sensor - accel temp");
              eSensorType = eSLIM_SENSOR_TYPE_ACCEL_TEMP;
            }
          }
          break;
        case SNS_SMGR_ID_GYRO_V01:
          if (m_zGyroState.running)
          {
            if (SNS_SMGR_DATA_TYPE_PRIMARY_V01 == zIndexData.DataType)
            {
              SLIM_LOGV("Got sensor - gyro");
              eSensorType = eSLIM_SENSOR_TYPE_GYRO;
            }
            else if (SNS_SMGR_DATA_TYPE_SECONDARY_V01 == zIndexData.DataType)
            {
              SLIM_LOGV("Got sensor - gyro temp");
              eSensorType = eSLIM_SENSOR_TYPE_GYRO_TEMP;
            }
          }
          break;
        case SNS_SMGR_ID_PRESSURE_V01:
          if (m_zBaroState.running)
          {
            if (SNS_SMGR_DATA_TYPE_PRIMARY_V01 == zIndexData.DataType)
            {
              eSensorType = eSLIM_SENSOR_TYPE_BAROMETER;
            }
          }
          break;
        case SNS_SMGR_ID_MAG_V01:
          if (eSLIM_SERVICE_SENSOR_MAG_CALIB == slimServiceEnumT(zSmgrInd.ReportId) &&
              m_zMagCalibState.running)
          {
            if (SNS_SMGR_DATA_TYPE_PRIMARY_V01 == zIndexData.DataType)
            {
              eSensorType = eSLIM_SENSOR_TYPE_MAGNETOMETER;
            }
          }
          if (eSLIM_SERVICE_SENSOR_MAG_UNCALIB == slimServiceEnumT(zSmgrInd.ReportId) &&
              m_zMagUncalibState.running)
          {
            if (SNS_SMGR_DATA_TYPE_PRIMARY_V01 == zIndexData.DataType)
            {
              eSensorType = eSLIM_SENSOR_TYPE_MAGNETOMETER;
            }
          }
          break;
        default:
          break;
        }
        /* If sensor type is supported, convert samples and send them to client */
        if (SENSOR_TYPE_UNDEFINED != eSensorType)
        {

          SLIM_LOGV("sensor-type is %d", eSensorType);

          convert_sensor_samples(zIndexData, &zSmgrInd.Samples[0],
              eSensorType, slimServiceEnumT(zSmgrInd.ReportId));
        }
        else
        {
          SLIM_LOGE(
              "buffering api not supported for this sensor-id %" PRIu8 ", sensor-type %" PRIu8,
              zIndexData.SensorId, zIndexData.DataType);
        }
      }
    }
  }
  else
  {
    SLIM_LOGE("Received invalid indication, msg_id = %" PRIu32, zMsgHdr.msg_id);
  }

}

/**
@brief Handler for Sensor1 SAM Service Responses.

@param[in] zMsgHdr Message header
@param[in] pzData  Pointer to message data
*/
void Sensor1Provider::process_sam_resp
(
  const sensor1_msg_header_s &zMsgHdr,
  const void* pzData
)
{
  SLIM_LOGV("msg_id = %" PRIu32, zMsgHdr.msg_id);
  //! @todo Send response to slim core
  switch (zMsgHdr.service_number)
  {
  case SNS_SAM_AMD_SVC_ID_V01:
    if (SNS_SAM_AMD_ENABLE_RESP_V01 == zMsgHdr.msg_id)
    {
      const sns_sam_qmd_enable_resp_msg_v01 &zResp =
          *(const sns_sam_qmd_enable_resp_msg_v01*) pzData;

      if (SNS_RESULT_SUCCESS_V01 != zResp.resp.sns_result_t ||
          SENSOR1_SUCCESS != zResp.resp.sns_err_t)
      {
        SLIM_LOGE(
            "Bad Enable response result = %" PRIu8 ", error = %" PRIu8,
            zResp.resp.sns_result_t,
            zResp.resp.sns_err_t);
      }
      else
      {
        SLIM_LOGD(
            "Got ENABLE response, status = %" PRIu8 ", instance_id = %" PRIu8,
            zResp.resp.sns_result_t,
            zResp.instance_id);
        m_zMotionState.amd_algo_instance_id = zResp.instance_id;
      }
    }
    else if (SNS_SAM_AMD_DISABLE_RESP_V01 == zMsgHdr.msg_id)
    {
      const sns_sam_qmd_disable_resp_msg_v01 &zResp =
          *(sns_sam_qmd_disable_resp_msg_v01*) pzData;

      if (SNS_RESULT_SUCCESS_V01 != zResp.resp.sns_result_t ||
          SENSOR1_SUCCESS != zResp.resp.sns_err_t)
      {
        SLIM_LOGE(
            "Bad Disable response result = %" PRIu8 ", error = %" PRIu8,
            zResp.resp.sns_result_t,
            zResp.resp.sns_err_t);
      }
      else
      {
        SLIM_LOGD(
            "Got DISABLE response, status = %" PRIu8 ", instance_id = %" PRIu8,
            zResp.resp.sns_result_t,
            zResp.instance_id);
      }
    }
    else
    {
      SLIM_LOGE("Received invalid sam response, msg_id = %d", zMsgHdr.msg_id);
    }
    break;

  case SNS_SAM_RMD_SVC_ID_V01:
    if (SNS_SAM_RMD_ENABLE_RESP_V01 == zMsgHdr.msg_id)
    {
      const sns_sam_qmd_enable_resp_msg_v01 &zResp =
          *(const sns_sam_qmd_enable_resp_msg_v01*) pzData;

      if (SNS_RESULT_SUCCESS_V01 != zResp.resp.sns_result_t ||
          SENSOR1_SUCCESS != zResp.resp.sns_err_t)
      {
        SLIM_LOGE(
            "Bad Enable response result = %" PRIu8 ", error = %" PRIu8,
            zResp.resp.sns_result_t,
            zResp.resp.sns_err_t);
      }
      else
      {
        SLIM_LOGD(
            "Got ENABLE response, status = %" PRIu8 ", instance_id = %" PRIu8,
            zResp.resp.sns_result_t,
            zResp.instance_id);
        m_zMotionState.rmd_algo_instance_id = zResp.instance_id;
      }
    }
    else if (SNS_SAM_RMD_DISABLE_RESP_V01 == zMsgHdr.msg_id)
    {
      const sns_sam_qmd_disable_resp_msg_v01 &zResp =
          *(const sns_sam_qmd_disable_resp_msg_v01*) pzData;

      if (SNS_RESULT_SUCCESS_V01 != zResp.resp.sns_result_t ||
          SENSOR1_SUCCESS != zResp.resp.sns_err_t)
      {
        SLIM_LOGE(
            "Bad Disable response result = %" PRIu8 ", error = %" PRIu8,
            zResp.resp.sns_result_t,
            zResp.resp.sns_err_t);
      }
      else
      {
        SLIM_LOGD(
            "Got DISABLE response, status = %" PRIu8 ", instance_id = %" PRIu8,
            zResp.resp.sns_result_t,
            zResp.instance_id);
      }
    }
    else
    {
      SLIM_LOGE(
          "Received invalid sam response, msg_id = %" PRIu32,
          zMsgHdr.msg_id);
    }
    break;

  case SNS_SAM_VMD_SVC_ID_V01:
    if (SNS_SAM_VMD_ENABLE_RESP_V01 == zMsgHdr.msg_id)
    {
      const sns_sam_qmd_enable_resp_msg_v01 &zResp =
          *(const sns_sam_qmd_enable_resp_msg_v01*) pzData;

      if (SNS_RESULT_SUCCESS_V01 != zResp.resp.sns_result_t ||
          SENSOR1_SUCCESS != zResp.resp.sns_err_t)
      {
        SLIM_LOGE(
            "Bad Enable response result = %" PRIu8 ", error = %" PRIu8,
            zResp.resp.sns_result_t,
            zResp.resp.sns_err_t);
      }
      else
      {
        SLIM_LOGD(
            "Got ENABLE response, status = %" PRIu8 ", instance_id = %" PRIu8,
            zResp.resp.sns_result_t,
            zResp.instance_id);
        m_lSpiAlgoInstanceId = zResp.instance_id;
      }
    }
    else if ( SNS_SAM_VMD_DISABLE_RESP_V01 == zMsgHdr.msg_id)
    {
      const sns_sam_qmd_disable_resp_msg_v01 &zResp =
          *(const sns_sam_qmd_disable_resp_msg_v01*) pzData;

      if (SNS_RESULT_SUCCESS_V01 != zResp.resp.sns_result_t ||
          SENSOR1_SUCCESS != zResp.resp.sns_err_t)
      {
        SLIM_LOGE(
            "Bad Disable response result = %" PRIu8 ", error = %" PRIu8,
            zResp.resp.sns_result_t,
            zResp.resp.sns_err_t);
      }
      else
      {
        SLIM_LOGD(
            "Got DISABLE response, status = %" PRIu8 ", instance_id = %" PRIu8,
            zResp.resp.sns_result_t,
            zResp.instance_id);
      }
    }
    else
    {
      SLIM_LOGE(
          "Received invalid sam response, msg_id = %" PRIu32,
          zMsgHdr.msg_id);
    }
    break;

  case SNS_SAM_PED_SVC_ID_V01:
    if (SNS_SAM_PED_ENABLE_RESP_V01 == zMsgHdr.msg_id)
    {
      const sns_sam_ped_enable_resp_msg_v01 &zResp =
          *(const sns_sam_ped_enable_resp_msg_v01*) pzData;

      if (SNS_RESULT_SUCCESS_V01 != zResp.resp.sns_result_t ||
          SENSOR1_SUCCESS != zResp.resp.sns_err_t)
      {
        SLIM_LOGE(
            "Bad ENABLE response result = %" PRIu8 ", error = %" PRIu8,
            zResp.resp.sns_result_t,
            zResp.resp.sns_err_t);
        m_zPedometerState.running = false;
      }
      else
      {
        SLIM_LOGD(
            "Got ENABLE response, status = %" PRIu8 ", instance_id = %" PRIu8,
            zResp.resp.sns_result_t,
            zResp.instance_id);
        clock_gettime(CLOCK_MONOTONIC, &(m_zPedometerState.reset_time));
        m_zPedometerState.algo_instance_id = zResp.instance_id;

      }
    }
    else if (SNS_SAM_PED_DISABLE_RESP_V01 == zMsgHdr.msg_id)
    {
      const sns_sam_ped_disable_resp_msg_v01 &zResp =
          *(const sns_sam_ped_disable_resp_msg_v01*) pzData;

      if (SNS_RESULT_SUCCESS_V01 != zResp.resp.sns_result_t ||
          SENSOR1_SUCCESS != zResp.resp.sns_err_t)
      {
        SLIM_LOGE(
            "Bad DISABLE response result = %" PRIu8 ", error = %" PRIu8,
            zResp.resp.sns_result_t,
            zResp.resp.sns_err_t);
      }
      else
      {
        SLIM_LOGD(
            "Got DISABLE response, status = %" PRIu8 ", instance_id = %" PRIu8,
            zResp.resp.sns_result_t,
            zResp.instance_id);
      }
    }
    else if ( SNS_SAM_PED_RESET_RESP_V01 == zMsgHdr.msg_id)
    {
      const sns_sam_ped_reset_resp_msg_v01 &zResp =
          *(const sns_sam_ped_reset_resp_msg_v01*) pzData;

      if (SNS_RESULT_SUCCESS_V01 != zResp.resp.sns_result_t ||
          SENSOR1_SUCCESS != zResp.resp.sns_err_t)
      {
        SLIM_LOGE(
            "Bad RESET response result = %" PRIu8 ", error = %" PRIu8,
            zResp.resp.sns_result_t,
            zResp.resp.sns_err_t);
      }
      else
      {
        SLIM_LOGD(
            "Got RESET response, status = %" PRIu8 ", instance_id = %" PRIu8,
            zResp.resp.sns_result_t,
            zResp.instance_id);
        clock_gettime(CLOCK_MONOTONIC, &(m_zPedometerState.reset_time));
      }
    }
    else
    {
      SLIM_LOGE(
          "Received invalid sam response, msg_id = %" PRIu32,
          zMsgHdr.msg_id);
    }
    break;

  default:
    break;
  }

  return;
}

/**
@brief Handler for Sensor1 SAM Service Indications.

@param[in] zMsgHdr Message header
@param[in] pzData  Pointer to message data
*/
void Sensor1Provider::process_amd_and_rmd_ind
(
  const sensor1_msg_header_s &zMsgHdr,
  const void *pzData
)
{
  SLIM_LOGV("msg_id = %d", zMsgHdr.msg_id);

  switch (zMsgHdr.msg_id)
  {
  /* Handler for both AMD and RMD (same message id value) */
  case SNS_SAM_AMD_REPORT_IND_V01:
    //case SNS_SAM_RMD_REPORT_IND_V01:
  {
    const sns_sam_qmd_report_ind_msg_v01 &zInd =
       *(const sns_sam_qmd_report_ind_msg_v01*) pzData;
    uint32_t dsps_time_ms = slim_TimeToMilliseconds(zInd.timestamp);

    if (SNS_SAM_AMD_SVC_ID_V01 == zMsgHdr.service_number)
    {
      SLIM_LOGV(
          "Received AMD state indication, instance_id = %" PRIu8
          ", timestamp = %" PRIu32 ", state = %d",
          zInd.instance_id,
          dsps_time_ms,
          zInd.state);
      m_zMotionState.amd = zInd.state;
    }
    else if (SNS_SAM_RMD_SVC_ID_V01 == zMsgHdr.service_number)
    {
      SLIM_LOGV(
          "Received RMD state indication, instance_id = %" PRIu8
          ", timestamp = %" PRIu32 ", state = %d",
          zInd.instance_id,
          dsps_time_ms,
          zInd.state);
      m_zMotionState.rmd = zInd.state;
    }

    slimMotionDataStructT motionData;
    memset(&motionData, 0, sizeof(motionData));

    motionData.timeSource = eSLIM_TIME_SOURCE_UNSPECIFIED;
    motionData.age = MOTION_DATA_TLV_AGE;
    motionData.motionMode = eSLIM_MOTION_MODE_UNKNOWN;
    motionData.timeout = MOTION_DATA_TLV_TIMEOUT;

    if (SNS_SAM_MOTION_UNKNOWN_V01 == m_zMotionState.amd)
    {
      motionData.motionState = eSLIM_MOTION_STATE_UNKNOWN;
      motionData.probabilityOfState = 99.9;
    }
    else if (SNS_SAM_MOTION_REST_V01 == m_zMotionState.amd)
    {
      motionData.motionState = eSLIM_MOTION_STATE_STATIONARY;
      motionData.probabilityOfState = 99.9;
    }
    else if (SNS_SAM_MOTION_UNKNOWN_V01 == m_zMotionState.rmd)
    {
      motionData.motionState = eSLIM_MOTION_STATE_UNKNOWN;
      motionData.probabilityOfState = 99.9;
    }
    else if (SNS_SAM_MOTION_REST_V01 == m_zMotionState.rmd)
    {
      motionData.motionState = eSLIM_MOTION_STATE_STATIONARY;
      motionData.probabilityOfState = 90.0;
    }
    else if (SNS_SAM_MOTION_MOVE_V01 == m_zMotionState.rmd)
    {
      motionData.motionState = eSLIM_MOTION_STATE_IN_MOTION;
      motionData.probabilityOfState = 99.9;
    }

    if (0 != dsps_time_ms)
    {
      motionData.timestamp = dsps_time_ms;
      motionData.age = 0; //! @todo TBD is reset is needed
    }

    SLIM_LOGV(
        "Motion data update sent with state = %d and probability = %f",
        motionData.motionState,
        motionData.probabilityOfState);

    routeIndication(motionData);
  }
    break;
  default:
    SLIM_LOGE("Received invalid indication, msg_id = %d", zMsgHdr.msg_id);
    break;
  }
}

/**
@brief Handler for Sensor1 SAM Service Indications.

@param[in] zMsgHdr Message header
@param[in] pzData  Pointer to message data
*/
void Sensor1Provider::process_vmd_ind
(
  const sensor1_msg_header_s &zMsgHdr,
  const void* pzData
)
{
  switch (zMsgHdr.msg_id)
  {
  case SNS_SAM_VMD_REPORT_IND_V01:
  {
    const sns_sam_qmd_report_ind_msg_v01 &zInd =
        *(const sns_sam_qmd_report_ind_msg_v01*) pzData;
    uint32_t dsps_time_ms = slim_TimeToMilliseconds(zInd.timestamp);

    SLIM_LOGV(
        "Received SPI indication, instance_id = %" PRIu8
        ", timestamp = %" PRIu32 ", state = %d",
        zInd.instance_id,
        dsps_time_ms,
        zInd.state);

    m_lSpiAlgoInstanceId = zInd.instance_id;

    /* Only send SPI updates for moving or stationary */
    if (SNS_SAM_MOTION_REST_V01 == zInd.state ||
        SNS_SAM_MOTION_MOVE_V01 == zInd.state)
    {
      uint8_t is_stationary = 0xFF;
      if (SNS_SAM_MOTION_REST_V01 == zInd.state)
      {
        is_stationary = 1;
      }
      else if (SNS_SAM_MOTION_MOVE_V01 == zInd.state)
      {
        is_stationary = 0;
      }
      //! @todo Send to data task through message queue.
    }
    break;
  }
  default:
    SLIM_LOGE("Received invalid indication, msg_id = %" PRId32, zMsgHdr.msg_id);
    break;
  }
}

/**
@brief Handler for Sensor1 SAM Service Indications.

@param[in] zMsgHdr Message header
@param[in] pzData  Pointer to message data
*/
void Sensor1Provider::process_ped_ind
(
  const sensor1_msg_header_s &zMsgHdr,
  const void *pzData
)
{
  SLIM_LOGV("msg_id = %d",  zMsgHdr.msg_id );

  switch (zMsgHdr.msg_id)
  {
  case SNS_SAM_PED_REPORT_IND_V01:
  {
    const sns_sam_ped_report_ind_msg_v01 &zInd =
        *(const sns_sam_ped_report_ind_msg_v01*) pzData;
    uint32_t dsps_time_ms = slim_TimeToMilliseconds(zInd.timestamp);
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);
    uint32_t time_interval = (uint32_t) (
            (current_time.tv_sec - m_zPedometerState.reset_time.tv_sec) * 1000 +
            (float) (current_time.tv_nsec - m_zPedometerState.reset_time.tv_nsec) / 1000);

    SLIM_LOGV(
        "Received Pedometer indication, instance_id = %d, timestamp = %" PRIu32
        ", time_interval = %" PRIu32 " ms, step_event = %u, "
        "step_confidence = %u, " "step_count = %" PRIu32 ", "
        "step_count_error = %" PRId32 ", step_rate = %4.2f Hz",
        zInd.instance_id,
        dsps_time_ms,
        time_interval,
        zInd.report_data.step_event,
        zInd.report_data.step_confidence,
        zInd.report_data.step_count,
        zInd.report_data.step_count_error,
        zInd.report_data.step_rate);

    /* SLIM does not support periodic pedometer reports. A report will be
     generated only when a step event is detected. */
    if (zInd.report_data.step_event)
    {
      slimPedometerDataStructT pedometerData;
      memset(&pedometerData, 0, sizeof(pedometerData));

      /* Update SSC time and store the received timestamp in milliseconds */
      pedometerData.timestamp = dsps_time_ms;
      pedometerData.timeSource = eSLIM_TIME_SOURCE_UNSPECIFIED;

      /* Calculate interval from the last reset */
      pedometerData.timeInterval = time_interval;

      pedometerData.stepCount = zInd.report_data.step_count;
      pedometerData.stepConfidence_valid = TRUE;
      pedometerData.stepConfidence = zInd.report_data.step_confidence;
      pedometerData.stepCountUncertainty_valid = TRUE;
      pedometerData.stepCountUncertainty =
          (float) zInd.report_data.step_count_error;
      pedometerData.stepRate_valid = TRUE;
      pedometerData.stepRate = zInd.report_data.step_rate;
      pedometerData.stepCountFractional_valid = FALSE;
      pedometerData.strideLength_valid = FALSE;
      pedometerData.strideLengthUncertainty_valid = FALSE;

      pedometerData.reportType_valid = TRUE;
      pedometerData.reportType = eSLIM_INJECT_PEDOMETER_REPORT_ON_STEP_EVENT;

      routeIndication(pedometerData);
    }
    else
    {
      SLIM_LOGV("Pedometer indication ignored.");
    }
    break;
  }
  default:
    SLIM_LOGE("Received invalid indication, msg_id = %d", zMsgHdr.msg_id);
    break;
  }
}

/**
@brief Handler for Sensor1 SAM Service Time Indications.

@param[in] zMsgHdr Message header
@param[in] pzData  Pointer to message data
*/
void Sensor1Provider::process_time_resp
(
  const sensor1_msg_header_s &zMsgHdr,
  const void *pzData
)
{
  if (SNS_TIME_TIMESTAMP_RESP_V02 == zMsgHdr.msg_id)
  {
    const sns_time_timestamp_resp_msg_v02 &zTimeMsg =
        *(const sns_time_timestamp_resp_msg_v02*) pzData;

    if (0 == zTimeMsg.resp.sns_result_t)
    {
      int32_t lServiceTxnId;

      if (releaseLocalTxnId(zMsgHdr.txn_id, lServiceTxnId))
      {
        if (zTimeMsg.timestamp_dsps_valid)
        {
          uint32_t qDspsClkTicks = zTimeMsg.timestamp_dsps;
          uint32_t qProviderTime = slim_TimeToMilliseconds(qDspsClkTicks);

          SLIM_LOGV(
              "Apps: %" PRIu64 "; DSPS: %u; Ms: %" PRIu32,
              zTimeMsg.timestamp_apps,
              zTimeMsg.timestamp_dsps,
              qProviderTime);

          routeTimeResponse(
              lServiceTxnId,
              eSLIM_SUCCESS,
              qProviderTime,
              qProviderTime);
        }
        else if (zTimeMsg.error_code_valid)
        {
          SLIM_LOGE("Error in time response: %" PRIi32, zTimeMsg.error_code);
          routeTimeResponse(lServiceTxnId, eSLIM_ERROR_QMI_INTERFACE, 0, 0);
        }
        else
        {
          SLIM_LOGE("Unknown response");
          routeTimeResponse(lServiceTxnId, eSLIM_ERROR_QMI_INTERFACE, 0, 0);
        }
      }
      else
      {
        SLIM_LOGE("Can not locate transaction id.");
      }
    }
    else
    {
      SLIM_LOGE("Received 'Failed' in time response result");
    }
  }
  else
  {
    SLIM_LOGE("Unhandled message id received: %" PRIi32, zMsgHdr.msg_id);
  }
}

/**
@brief Sensor1 interface callback method.

Method routes call to provider's instance.

@param[in] lCbData  Callback data. Provider instance identifier.
@param[in] pzMsgHdr Sensor1 header block.
@param[in] eMsgType Sensor1 message type.
@param[in] pData    Sensor1 message payload.
*/
void Sensor1Provider::sensor1NotifyDataCallback
(
  intptr_t lCbData,
  sensor1_msg_header_s *pzMsgHdr,
  sensor1_msg_type_e eMsgType,
  void* pData
)
{
  SLIM_CB_CTX_PUSH("SENSOR1");
  Sensor1Provider *pzObject = reinterpret_cast<Sensor1Provider*>(lCbData);
  if (0 != pzObject)
  {
    pzObject->processSensor1Message(pzMsgHdr, eMsgType, pData);
    /* Free message received as we are done with it and to comply with Sensor1 Documentation */
    if (0 != pData)
    {
      sensor1_free_msg_buf(pzObject->m_pClientHandle, pData);
    }
  }

  SLIM_CB_CTX_POP();
}

/**
@brief Validate and dispatch Sensor1 message

Method performs parameter validation and dispatches message to appropriate
handler.

@param[in] pzMsgHdr Sensor1 header block.
@param[in] eMsgType Sensor1 message type.
@param[in] pData    Sensor1 message payload.
*/
void Sensor1Provider::processSensor1Message
(
  const sensor1_msg_header_s *pzMsgHdr,
  sensor1_msg_type_e eMsgType,
  const void* pData
)
{
  MutexLock _l(m_zMutex);

  if (0 != pzMsgHdr)
  {
    SLIM_LOGV("Message type = %d Service Num = %d, Message id = %d", eMsgType,
        pzMsgHdr->service_number, pzMsgHdr->msg_id);
  }
  else
  {
    SLIM_LOGV("Message Type = %d; no header", eMsgType);
  }

  switch ( eMsgType )
  {
  case SENSOR1_MSG_TYPE_RETRY_OPEN:
    if (open())
    {
      routeConfigurationChange(SLIM_SENSOR1_SERVICE_MASK);
    }
    else
    {
      SLIM_LOGE("Failed reopen attempt.");
      close();
    }
    break;
  case SENSOR1_MSG_TYPE_RESP:
    if(0 == pData || 0 == pzMsgHdr)
    {
      SLIM_LOGE("Invalid pData/pzMsgHdr for response");
    }
    else if (SNS_SMGR_SVC_ID_V01 == pzMsgHdr->service_number)
    {
      process_smgr_resp(*pzMsgHdr, pData);
    }
    else if (SNS_SAM_AMD_SVC_ID_V01 == pzMsgHdr->service_number ||
             SNS_SAM_RMD_SVC_ID_V01 == pzMsgHdr->service_number ||
             SNS_SAM_VMD_SVC_ID_V01 == pzMsgHdr->service_number ||
             SNS_SAM_PED_SVC_ID_V01 == pzMsgHdr->service_number)
    {
      process_sam_resp(*pzMsgHdr, pData);
    }
    else if (SNS_TIME2_SVC_ID_V01 == pzMsgHdr->service_number)
    {
      process_time_resp(*pzMsgHdr, pData );
    }
    else
    {
      SLIM_LOGE("Unexpected response service_number = %" PRIu32,
        pzMsgHdr->service_number);
    }
    break;

  case SENSOR1_MSG_TYPE_IND:
    if (0 == pData || 0 == pzMsgHdr)
    {
      SLIM_LOGE("Invalid msg_ptr/msg_hdr for ind");
    }
    else if (SNS_SMGR_SVC_ID_V01 == pzMsgHdr->service_number )
    {
      process_smgr_ind(*pzMsgHdr, pData);
    }
    else if (SNS_SAM_AMD_SVC_ID_V01 == pzMsgHdr->service_number ||
             SNS_SAM_RMD_SVC_ID_V01 == pzMsgHdr->service_number)
    {
      process_amd_and_rmd_ind(*pzMsgHdr, pData);
    }
    else if (SNS_SAM_VMD_SVC_ID_V01 == pzMsgHdr->service_number)
    {
      process_vmd_ind(*pzMsgHdr, pData);
    }
    else if (SNS_SAM_PED_SVC_ID_V01 == pzMsgHdr->service_number)
    {
      process_ped_ind(*pzMsgHdr, pData);
    }
    else
    {
      SLIM_LOGE("Unexpected ind service_number = %" PRIu32,
        pzMsgHdr->service_number);
    }
    break;

  case SENSOR1_MSG_TYPE_BROKEN_PIPE:
    SLIM_LOGD("Sensor1 Broken Pipe msg type in cb. Closing Sensor1");
    close();
    routeConfigurationChange(SLIM_SENSOR1_SERVICE_MASK);

    if (!slim_IpcSendEmpty(m_zThread.q_IpcId,
                           (uint32_t)ePROVIDER_IPC_MSG_ID_REOPEN))
    {
      SLIM_LOGE("Failed to route REOPEN message to self.");
    }
    break;

  case SENSOR1_MSG_TYPE_RESP_INT_ERR:
  case SENSOR1_MSG_TYPE_REQ:
  default:
      SLIM_LOGE("Invalid msg type in cb = %u",  eMsgType);
      break;
  }
}

/* ----------------------- END INTERNAL FUNCTIONS ---------------------------------------- */

/**
@brief Opens Sensor1 connection.

Opens connection to Sensor1 API. The result can be one of the following:
- Success
- Partial success
- Error

@return true if result is success or partial success; false otherwise.
*/
bool Sensor1Provider::open()
{
  bool b_Success = false;

  m_eOpenResult = sensor1_open(
      &m_pClientHandle,
      sensor1NotifyDataCallback,
      (intptr_t) this);
  SLIM_LOGV("Sensor1 open: %d", m_eOpenResult);

  m_qServiceMask = 0;
  switch (m_eOpenResult)
  {
  case SENSOR1_SUCCESS:
    SLIM_LOGD("Sensor1 connection opened successfully!");
    b_Success = true;
    m_qServiceMask = SLIM_SENSOR1_SERVICE_MASK;
    break;

  case SENSOR1_EWOULDBLOCK:
    SLIM_LOGD("Pending Sensor1 connection opening!");
    b_Success = true;
    break;

  default:
    SLIM_LOGW("Fall back to Android NDK as Sensor Core is not available!");
    break;
  }

  return b_Success;
}

/**
@brief Closes Sensor1 connection.

Closes connection to Sensor1 API.

@return true if operation is successful; false otherwise.
*/
bool Sensor1Provider::close()
{
  if (0 != m_pClientHandle)
  {
    sensor1_error_e eError;
    eError = sensor1_close(m_pClientHandle);
    m_pClientHandle = 0;
    m_qServiceMask = 0;
    m_eOpenResult = SENSOR1_SUCCESS;
    memset(&m_zMotionState, 0, sizeof(m_zMotionState));
    memset(&m_zPedometerState, 0, sizeof(m_zPedometerState));
    memset(&m_zGyroState, 0, sizeof(m_zGyroState));
    memset(&m_zAccelState, 0, sizeof(m_zAccelState));
    memset(&m_zBaroState, 0, sizeof(m_zBaroState));
    memset(&m_zGyroTempState, 0, sizeof(m_zGyroTempState));
    memset(&m_zAccelTempState, 0, sizeof(m_zAccelTempState));
    memset(&m_zMagCalibState, 0, sizeof(m_zMagCalibState));
    memset(&m_zMagUncalibState, 0, sizeof(m_zMagUncalibState));
    m_zMotionState.amd = SNS_SAM_MOTION_UNKNOWN_V01;
    m_zMotionState.rmd = SNS_SAM_MOTION_UNKNOWN_V01;
    m_tServiceTxnIdMask = 0;
    m_lSpiAlgoInstanceId = -1;
    memset(m_lServiceTxnIds, 0, sizeof(m_lServiceTxnIds));
    slim_ServiceStatusReset(&m_zProviderStatus);

    if (SENSOR1_SUCCESS != eError)
    {
      SLIM_LOGE("sensor1_close() error: %u", eError);
      return false;
    }
  }

  return true;
}

/**
@brief Initializes internal structures for sensor provider.

The method prepares provider for operation.
@return true if operation is successful; false otherwise.
*/
bool Sensor1Provider::init()
{
  SLIM_LOGD("Initializing Sensor1");

  if (!slim_ThreadCreate(&m_zThread, threadMain, this, "Sensor1"))
  {
    return false;
  }
  slim_TaskReadyWait(THREAD_ID_SENSOR1);

  if (slim_IpcSendEmpty(m_zThread.q_IpcId, (uint32_t)ePROVIDER_IPC_MSG_ID_REOPEN))
  {
    return true;
  }
  return false;
}
void Sensor1Provider::runEventLoop()
{
  SLIM_LOGD("Starting event loop");
  slim_TaskReadyAck();
  while (1)
  {
    slim_IpcMsgT *pz_Msg = 0;
    while (0 == (pz_Msg = slim_IpcReceive()));

    SLIM_LOGD("IPC message received. q_MsgId:%" PRIu32 ", q_SrcThreadId:%" PRIu32,
        pz_Msg->q_MsgId,
        pz_Msg->q_SrcThreadId);

    switch(pz_Msg->q_MsgId)
    {
    case ePROVIDER_IPC_MSG_ID_REOPEN:
    {
      bool uOpenResult;
      {
        MutexLock _l(m_zMutex);
        uOpenResult = open();
      }
      if (!uOpenResult)
      {
        SLIM_LOGE("Failed reopen attempt.");
      }
      else
      {
        routeConfigurationChange(SLIM_SENSOR1_SERVICE_MASK);
      }
      break;
    }
    default:
      break;
    }
    slim_IpcDelete(pz_Msg);
  }
}

/**
 @brief Initiates time offset request.

 Function for making the time request. Successful response enable SLIM to
 calculate the offset between modem time and sensor time.

 @param[in] lTxnId Service transaction id.
 @return eSLIM_SUCCESS if time request is made successfully.
 */
slimErrorEnumT Sensor1Provider::getTimeUpdate
(
  int32_t lTxnId
)
{
  sensor1_error_e                 eError;
  sensor1_msg_header_s            zMsgHdr;
  sns_reg_single_read_req_msg_v02 *pzMsg = 0;

  MutexLock _l(m_zMutex);

  eError = sensor1_alloc_msg_buf( m_pClientHandle, 0, (void**)&pzMsg );
  if( SENSOR1_SUCCESS != eError )
  {
    SLIM_LOGE( "sensor1_alloc_msg_buf returned(get) %d", eError );
    return eSLIM_ERROR_ALLOCATION;
  }

  /* Create the message header */
  zMsgHdr.service_number = SNS_TIME2_SVC_ID_V01;
  zMsgHdr.msg_id = SNS_TIME_TIMESTAMP_REQ_V02;
  zMsgHdr.msg_size = 0;

  if (!allocateLocalTxnId(lTxnId, zMsgHdr.txn_id))
  {
    SLIM_LOGE("Failed to map transaction");
    sensor1_free_msg_buf( m_pClientHandle, pzMsg );
    return eSLIM_ERROR_MAX_TXN_COUNT_REACHED;
  }

  eError = sensor1_write( m_pClientHandle, &zMsgHdr, pzMsg );
  if( SENSOR1_SUCCESS != eError )
  {
    SLIM_LOGE( "sensor1_write returned %d", eError );
    sensor1_free_msg_buf( m_pClientHandle, pzMsg );
    releaseLocalTxnId(zMsgHdr.txn_id, lTxnId);
    return eSLIM_ERROR_QMI_INTERFACE;
  }

  return eSLIM_SUCCESS;
}

/**
@brief Returns SLIM provider interface

Method constructs provider instance, initializes it, and returns SLIM
provider interface.

@return SLIM provider interface of 0 on error.
*/
slim_ServiceProviderInterfaceType *Sensor1Provider::getServiceProvider()
{
  Sensor1Provider *pzInstance = getInstance();
  if (0 != pzInstance)
  {
    return pzInstance->getSlimInterface();
  }
  else
  {
    return 0;
  }
}

/**
@brief Returns SLIM interface

The method initializes singleton instance and returns SLIM provider
interface.

@return SLIM interface pointer
*/
Sensor1Provider *Sensor1Provider::getInstance()
{
  MutexLock _l(s_zInstanceMutex);
  if (0 == s_pzInstance)
  {
    s_pzInstance = new Sensor1Provider;
    if (!s_pzInstance->init())
    {
      SLIM_LOGE("Sensor1 provider initialization failed;");
      delete s_pzInstance;
      s_pzInstance = 0;
    }
  }
  return s_pzInstance;
}

/**
@brief Configures batch mode support

@param[in] uFlag Flag to enable (true) or disable batch mode.
*/
void Sensor1Provider::setBatchMode
(
  bool uFlag
)
{
  if (!uFlag)
  {
    //! @todo Add support for non-batching mode
    SLIM_LOGW("Can not disable batch mode for Sensor1 provider");
    return;
  }
  m_uBatchMode = uFlag;
}

//! @}
