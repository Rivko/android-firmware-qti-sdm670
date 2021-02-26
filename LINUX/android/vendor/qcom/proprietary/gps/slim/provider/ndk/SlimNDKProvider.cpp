/*
Copyright (c) 2011-2012, 2014-2015, 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

2012 Qualcomm Atheros, Inc.
All Rights Reserved.
Qualcomm Atheros Proprietary and Confidential.
*/
/**
@file
@brief GNSS / Sensor Interface Framework Support

This file defines the implementation for sensor provider interface
using the generic Android Sensors API.

@dependencies
- FEATURE_LOCTECH_SLIM_NDK_CLOCK_ELAPSED -- Use Android elapsed real time
- FEATURE_LOCTECH_SLIM_NDK_CLOCK_BOOTTIME -- Use CLOCK_BOOTTIME
- FEATURE_LOCTECH_SLIM_NDK_CLOCK_MONOTONIC -- Use CLOCK_MONOTONIC
- FEATURE_LOCTECH_SLIM_NDK_CLOCK_REALTIME -- Use CLOCK_REALTIME

@ingroup slim_NDKProvider
*/

#include <inttypes.h>
#include <stdlib.h>
#include <algorithm>
#include <limits>

#include <sys/signalfd.h>
#include <sys/eventfd.h>
#include <signal.h>
#include <pthread.h>

#include "SlimNDKProvider.h"

#include <slim_internal.h>
#include <hardware/sensors.h>

#include <utils/SystemClock.h>
#include <utils/Timers.h>

#include "ALooper.h"
#include "ASensorManager.h"
#include <sensor/Sensor.h>

//! @addtogroup slim_NDKProvider
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/* This unit logs as NDK component. */
#undef SLIM_LOG_LOCAL_MODULE
//! @brief Logging module for NDK Provider.
#define SLIM_LOG_LOCAL_MODULE SLIM_LOG_MOD_NDK

//! @brief Value conversion for Android accelerator sensor
#define UNIT_CONVERT_ACCELERATION(x) x
//! @brief Value conversion for Android gyroscope sensor
#define UNIT_CONVERT_GYRO(x) x
//! @brief Value conversion for Android pressure sensor
#define UNIT_CONVERT_PRESSURE(x) x
//! @brief Value conversion for Android magnetometer sensor
#define UNIT_CONVERT_MAG(x) x
using namespace android;
namespace slim
{
/** @brief Singleton instance lock */
Mutex           NDKProvider::s_zInstanceLock;
/** @brief Singleton instance strong pointer. */
sp<NDKProvider> NDKProvider::s_pzInstance;

/*----------------------------------------------------------------------------
 * Provider timeouts
 * -------------------------------------------------------------------------*/

/** @brief Timeout in ms between rebind attempts */
const uint32_t  NDKProvider::s_qNdkRebindTimeoutMs = 1000;
/*! @brief Timeout in microseconds between time diff updates */
const uint64_t  NDKProvider::s_tTimeDiffUpdateTimeoutUs = 1000000;
/*! @brief Estimated delay between HAL and SLIM process in microseconds */
const uint64_t  NDKProvider::s_tEstimatedSampleDelayUs = 10000;

/*----------------------------------------------------------------------------
 * Kalman filter initialization parameters
 * -------------------------------------------------------------------------*/

//! @brief Min Residual diff to check for outlier
const uint32_t NDKProvider::KF::s_qDivergeThresholdUs = 1250;
//! @brief Minimum uncertainty to initialize filter
const uint32_t NDKProvider::KF::s_qMinUncToInitUs = 10;
//! @brief Expect/Meas variance threshold scalar
const uint8_t NDKProvider::KF::s_uVarThreshold = 25;
//! @brief Number of outliers before filter reset
const uint8_t NDKProvider::KF::s_uOutlierLimit = 15;
//! @brief Minimum time update interval in microseconds.
const uint64_t NDKProvider::KF::s_tTimeUpdateIntervalUs = 1000000.0;

/*----------------------------------------------------------------------------
 * Kalman filter process and measurement noise parameters
 * -------------------------------------------------------------------------*/

//! @brief Time limit for minimum process noise in microseconds
const double NDKProvider::KF::s_dMinTsOffsetQDtUs = 1000000.0;
//! @brief Minimum process noise
const double NDKProvider::KF::s_dMinTsOffsetQUs2 = 250.0;
//! @brief Process noise rate between time limits (us^2)/us
const double NDKProvider::KF::s_dTsOffsetQSfUs2Us = 0.15;
//! @brief Measurement noise
const double NDKProvider::KF::s_dTsOffsetMeasNoiseUs2 = 400.0*400.0;
//! @brief Measurement noise rat between time limits (us^2)/us
const double NDKProvider::KF::s_dTsOffsetRSfUs2Us = 0.5;

/*! @brief Reporting interval moving average factor
 *
 * Moving average factor.
 *
 * - Type is double
 * - Value of 0.20 indicates that reporting interval is updated with 80% weight.
 *
 * @sa sendDataThrottle
 */
#define SLIM_DATA_REPORT_INTERVAL_AVGING_FACTOR    (0.20f)

/*! @brief Minimum allowed reporting interval threshold
 *
 * Reporting interval threshold.
 *
 * - Type is uint64
 * - Reporting threshold below which data will be throttled.
 *
 * @sa updateStatus
 */
#define SLIM_DATA_MIN_ALLOWED_REPORTING_INTERVAL_MSEC     (2)


/**
 * @brief Returns printable literal for Android sensor type.
 *
 * @param[in] lNdkSensorType Sensor type
 *
 * @return String with sensor type string.
 */
static inline const char *getSensorTypeLiteral
(
    int32_t lNdkSensorType
)
{
    const char *pLiteral = "unknown";

    //! @brief Array with sensor literals.
    static const char *c_pLiterals[] =
    {
        "META_DATA",
        "ACCELEROMETER",
        "MAGNETIC_FIELD",
        "ORIENTATION",
        "GYROSCOPE",
        "LIGHT",
        "PRESSURE",
        "TEMPERATURE",
        "PROXIMITY",
        "GRAVITY",
        "LINEAR_ACCELERATION",
        "ROTATION_VECTOR",
        "RELATIVE_HUMIDITY",
        "AMBIENT_TEMPERATURE",
        "MAGNETIC_FIELD_UNCALIBRATED",
        "GAME_ROTATION_VECTOR",
        "GYROSCOPE_UNCALIBRATED",
        "SIGNIFICANT_MOTION",
        "STEP_DETECTOR",
        "STEP_COUNTER",
        "GEOMAGNETIC_ROTATION_VECTOR",
        "HEART_RATE",
        "TILT_DETECTOR",
        "WAKE_GESTURE",
        "TYPE_GLANCE_GESTURE",
        "PICK_UP_GESTURE"
    };

    if (lNdkSensorType < 0)
    {
        pLiteral = "UNKNOWN";
    }
    else if (lNdkSensorType < (int32_t)ARR_SIZE(c_pLiterals))
    {
        pLiteral = c_pLiterals[lNdkSensorType];
    }
    else if (lNdkSensorType >= SENSOR_TYPE_DEVICE_PRIVATE_BASE)
    {
        pLiteral = "PRIVATE";
    }
    else
    {
        pLiteral = "UNKNOWN";
    }

    return pLiteral;
}

/**
 * @brief Initializes Kalman filter with provider-specific parameters.
 */
NDKProvider::KF::KF() : KalmanFilter
    (
        s_uOutlierLimit,
        s_uVarThreshold,
        s_qMinUncToInitUs,
        s_qDivergeThresholdUs,
        s_tTimeUpdateIntervalUs
    )
{
}

/**
 * @brief Function used to calculate process noise
 *
 * @param[in] dDeltaTimeUs elapsed time between updates in microseconds.
 *
 * @return Process noise of clock offset in microseconds.
 */
double NDKProvider::KF::getProceNoise
(
    double dDeltaTimeUs
)
{
    /* Add process noise to input uncertainty. */
    if ( dDeltaTimeUs <= s_dMinTsOffsetQDtUs )
    {
        return s_dMinTsOffsetQUs2;
    }
    else
    {
        return (s_dTsOffsetQSfUs2Us * (dDeltaTimeUs - s_dMinTsOffsetQDtUs)) +
                s_dMinTsOffsetQUs2;
    }
}

/**
 * @brief Function used to calculate measurement noise
 *
 * @param[in] dDeltaTimeUs elapsed time between updates in microseconds.
 *
 * @return Measurement noise of clock offset in microseconds.
 */
double NDKProvider::KF::getMeasNoise
(
    double dDeltaTimeUs
)
{
    /* Add measurement noise to input uncertainty. */
    if ( dDeltaTimeUs <= s_dMinTsOffsetQDtUs )
    {
        return s_dTsOffsetMeasNoiseUs2;
    }
    else
    {
        return (s_dTsOffsetRSfUs2Us * (dDeltaTimeUs - s_dMinTsOffsetQDtUs)) +
                s_dTsOffsetMeasNoiseUs2;
    }
}

/**
@brief Reads events from queue

Callback method for reading sensor events and dispatching them to proper
state processors.

@param[in] lFd        Queue descriptor
@param[in] lEventMask Available events
@param[in] pData      User handle (0)

@return 1 to continue processing.
*/
int NDKProvider::handleEvent
(
    int lFd,
    int lEventMask,
    void *pData
)
{
    SLIM_UNUSED(lFd);
    SLIM_UNUSED(lEventMask);
    SLIM_UNUSED(pData);
    return 1;
}

/**
@brief Initializes object

After initialization a @a bind operation shall be invoked to complete startup.

@param[in] pzNDKProvider Parent container.
@param[in] eSensorType   SLIM sensor type.
@param[in] eServiceType  SLIM service type.
@param[in] lNdkType      Android sensor type.
@param[in] pzName        Literal name for logging.
*/
NDKProvider::ServiceState::ServiceState
(
    NDKProvider *pzNDKProvider,
    slimSensorTypeEnumT eSensorType,
    slimServiceEnumT eServiceType,
    int32_t          lNdkType,
    const char *pzName
) :
    m_pzNDKProvider(pzNDKProvider),
    m_eSlimSensorType(eSensorType),
    m_eSlimServiceType(eServiceType),
    m_lNdkSensorType(lNdkType),
    m_pzName(pzName),
    m_pzSensor(nullptr),
    m_uRunning(false),
    m_uBatchMode(false),
    m_qSampleRateHz(0),
    m_qBatchingRateHz(0),
    m_qBatchSize(0),
    m_zSensorData(),
    m_tLastSampleDelay(0),
    m_uBound(false),
    m_tLastReportTimestampMs(0),
    m_tAvgReportIntervalMs(0)
{
    resetData();
}
NDKProvider::ServiceState::~ServiceState()
{
    /*
    There is no need to disable sensors here
    since by the time this destructor is reached
    the sensor object(s) died
    */
}

/**
@brief Checks if data should be throttled.

Function checks if data should be throttled.

Updated logic: Given T0, t0 & input t1
  --> T1 = alp * (t1 - t0) + (1 - alp) * T0
  --> if T1 < T_THRESHOLD, then THROTTLE

@retval TRUE  if send data should be throttled.
@retval FALSE if data should be sent.
*/
bool NDKProvider::ServiceState::sendDataThrottle()
{
    bool b_Throttle = false;
    const uint64_t t_DataTimestamp = readCoreTimeUs() / 1000;

    if (m_tLastReportTimestampMs > 0)
    {
        m_tAvgReportIntervalMs = (uint64) (
                SLIM_DATA_REPORT_INTERVAL_AVGING_FACTOR * (float)( t_DataTimestamp - m_tLastReportTimestampMs ) +
                (1 - SLIM_DATA_REPORT_INTERVAL_AVGING_FACTOR) * (float)m_tAvgReportIntervalMs );

        if ( m_tAvgReportIntervalMs < m_tReportIntervalThresholdMs )
        {
            /* Drop the data if average interval is less than threshold */
            SLIM_LOGE("Interval < threshold: Curr msec %" PRIu64 ", AvgInterval msec %" PRIu64
                    ", Thresh %" PRIu64, t_DataTimestamp, m_tAvgReportIntervalMs,
                    m_tReportIntervalThresholdMs);
            b_Throttle = true;
        }
    }
    m_tLastReportTimestampMs = t_DataTimestamp;

    return b_Throttle;
}

/**
@brief Sends data to SLIM core.

The method routes collected data to the provider itself for multiplexing and
dispatching to individual clients using SLIM Core API.

@param eSendReason Batch generation reason.

@return eSLIM_SUCCESS if operation is successful.
*/
slimErrorEnumT NDKProvider::ServiceState::sendDataBatch
(
    BatchSendReason eSendReason
)
{
    if (0 == m_zSensorData.samples_len || !m_uRunning)
    {
        return eSLIM_SUCCESS;
    }
    if (m_zSensorData.samples_len > ARR_SIZE(m_zSensorData.samples))
    {
        SLIM_LOGE("Error: sample index is too large. Aborting.");
        abort();
    }

    if(sendDataThrottle())
    {
        SLIM_LOGW("Data throttled.");
        return eSLIM_SUCCESS;
    }

    IF_SLIM_LOGD
    {
        // SLIM time
        const uint64_t tCoreTimeMs = readCoreTimeUs() / 1000;
        // Samples time range
        const uint64_t tTimeElapsedMs = slim_TimeToMilliseconds(
                    m_zSensorData.samples[m_zSensorData.samples_len - 1].sampleTimeOffset);
        // Time range in SLIM time since first sample
        const uint64_t tTimeSince1Sample = tCoreTimeMs - m_zSensorData.timeBase;

        SLIM_LOGD(
                "%s batch sent. sensor=%d reason=%d samples=%" PRIu8 " time=%" PRIu64
                " range=%" PRIu64 " delay1=%" PRId64 " core=%" PRIu64,
                m_pzName,
                m_zSensorData.sensorType,
                eSendReason,
                m_zSensorData.samples_len,
                m_zSensorData.timeBase,
                tTimeElapsedMs,
                tCoreTimeMs - m_zSensorData.timeBase,
                tCoreTimeMs);

        if (0 == m_zSensorData.timeBase)
        {
            SLIM_LOGW("Timebase = %" PRIu64 " batch dropped", m_zSensorData.timeBase);
            resetData();
            return eSLIM_SUCCESS;
        }
    }

    // Route for multiplexing
    slimErrorEnumT eStatus =
            m_pzNDKProvider->routeIndication(m_eSlimServiceType, m_zSensorData);

    /* Reset data that was sent off */
    resetData();

    return eStatus;
}

/**
@brief Conditional data send.

Method sends accumulated data only if sufficient amount is collected, or
time check check reports data is delayed.

@param[in] uCheckTime Flag if time check is required.

@return eSLIM_SUCCESS if data is not sent, or operation is successful.
*/
slimErrorEnumT NDKProvider::ServiceState::sendIfReady
(
    bool uCheckTime
)
{
    if (0 == m_zSensorData.samples_len || !m_uRunning)
    {
        return eSLIM_SUCCESS;
    }

    BatchSendReason eSendReason = eBATCH_SEND_REASON_NONE;
    if (m_zSensorData.samples_len >= m_qBatchSize ||
            m_zSensorData.samples_len >= SLIM_SENSOR_MAX_SAMPLE_SETS)
    {
        eSendReason = eBATCH_SEND_REASON_SIZE;
    }
    else if (uCheckTime)
    {
        const uint64_t tCoreTimeMs = readCoreTimeUs() / 1000;
        const uint64_t tBatchingIntervalMs =
                (uint64_t)(1000.0f / m_qBatchingRateHz + .5f);
        const uint64_t tSampleIntervalMs =
                (uint64_t)(1000.0f / m_qSampleRateHz +.5f);

        /* Send data off to the message queue when the allotted time has passed */
        const uint64_t tTimeElapsedMs = slim_TimeToMilliseconds(
                m_zSensorData.samples[m_zSensorData.samples_len - 1].sampleTimeOffset);
        // Time since sample 1: this time includes overall delay of sample report,
        // as it compares with AP clock.
        const uint64_t tTimeSince1Sample = tCoreTimeMs - m_zSensorData.timeBase;

        // Time conditions:
        // Collected samples time interval plus one is too large OR
        // Time interval since first sample is too large (clock comparison) OR
        // Time interval for the next sample is too large (clock comparison)
        if (tTimeElapsedMs + tSampleIntervalMs >= tBatchingIntervalMs ||
                // tTimeSince1Sample >= tBatchingIntervalMs || //< Condition is redundant
                tTimeSince1Sample + tSampleIntervalMs >= tBatchingIntervalMs + m_tLastSampleDelay)
        {
            eSendReason = eBATCH_SEND_REASON_TIMEOUT;
        }
    }
    if (eBATCH_SEND_REASON_NONE != eSendReason)
    {
        return sendDataBatch(eSendReason);
    }
    return eSLIM_SUCCESS;
}

/**
@brief Enables or disables data collection.

When enabling, the actual sampling rate equals @a wReportRateHz multiplied
by @a wSampleCount.

@param[in] uRunning      Flag to enable (true) or disable(false)
                         data collection.
@param[in] wReportRateHz Batch reporting rate in Hz.
@param[in] wSampleCount  Sample count in one batch.
@return Operation status; eSLIM_SUCCESS if successful.
*/
slimErrorEnumT NDKProvider::ServiceState::updateStatus
(
    bool uEnable,
    uint16_t wReportRateHz,
    uint16_t wBatchSampleCount
)
{
    /* No State Change */
    if ( !uEnable && !m_uRunning )
    {
        return eSLIM_SUCCESS;
    }
    /* Sensors stay On but no change in sensor sampling rate */
    else if ( (uEnable && m_uRunning) &&
            (wBatchSampleCount == m_qBatchSize) &&
            (wReportRateHz == m_qBatchingRateHz))
    {
        return eSLIM_SUCCESS;
    }

    SLIM_LOGD("Updating on %s reporting = %d report_rate= %" PRIu16 " sampling_rate = %" PRIu32,
            m_pzName,
            uEnable,
            wReportRateHz,
            (uint32_t)wReportRateHz * wBatchSampleCount);
    m_qSampleRateHz = wReportRateHz * wBatchSampleCount;
    m_qBatchingRateHz = wReportRateHz;
    m_qBatchSize = wBatchSampleCount;
    if (wReportRateHz > 0)
    {
        m_tAvgReportIntervalMs = 1000u / (uint32_t)( wReportRateHz );
    }
    m_tReportIntervalThresholdMs = SLIM_DATA_MIN_ALLOWED_REPORTING_INTERVAL_MSEC;

    /* Clear out any old samples that may be lingering from previous starts/stops. */
    resetData();

    if ( uEnable && 0 != wReportRateHz && 0 != wBatchSampleCount )
    {
        if (m_uBatchMode)
        {
            /* Adjust sampling rate only if we are turning on the sensor */
            int lSensorRateUs = (int)( 1000000 / m_qSampleRateHz );
            int lMaxBatchLatencyUs = lSensorRateUs * m_qBatchSize;
            int ret = m_pzNDKProvider->m_pzEventQueue->registerSensor(
                                    m_pzSensor, lSensorRateUs, lMaxBatchLatencyUs);

            if (OK != ret)
            {
                /* Try to disable batch mode */
                m_uBatchMode = false;
                SLIM_LOGD("Disabling batch mode support for %s", m_pzName);
            }
            else
            {
                SLIM_LOGD("Enabling(batch mode): period=%" PRIi32 "us delay=%" PRIi64 "us",
                    lSensorRateUs, (int64_t)lMaxBatchLatencyUs);
            }
        }
        if (!m_uBatchMode)
        {
            int lSensorRateUs = (int)(1000000 / m_qSampleRateHz);
            int lMaxBatchLatencyUs = lSensorRateUs * m_qBatchSize;
            int ret = m_pzNDKProvider->m_pzEventQueue->registerSensor(
                            m_pzSensor, lSensorRateUs, lMaxBatchLatencyUs);
            if (OK != ret)
            {
                SLIM_LOGE("Error starting %s sensor", m_pzName);
                return eSLIM_ERROR_IPC_COMMUNICATION;
            }
            else
            {
                SLIM_LOGD("Enabling(non-batch mode): period=%" PRIi32 "us delay=%" PRIi64 "us",
                    lSensorRateUs, (int64_t)lMaxBatchLatencyUs);
            }
        }
    }
    else
    {
        uEnable = false;
        int ret = m_pzNDKProvider->m_pzEventQueue->disableSensor(m_pzSensor);
        if (OK != ret)
        {
            SLIM_LOGE("Error stopping %s sensor", m_pzName);
            return eSLIM_ERROR_IPC_COMMUNICATION;
        }
        else
        {
            SLIM_LOGD("Stopping %s sensor was OK", m_pzName);
        }
    }

    /* Update running state after performing MSI to make the function generic */
    m_uRunning = uEnable;

    return eSLIM_SUCCESS;
}
/**
@brief Adds sample data to buffer

@param[in] zData        Sample data
@param[in] zTimestampMs Sample time (using SLIM timer).
*/
void NDKProvider::ServiceState::processRawData
(
    float zData[3],
    uint64_t tTimestampMs
)
{
    /* Common handling for all sensors */
    if (m_zSensorData.samples_len == 0)
    {
        m_zSensorData.timeBase = tTimestampMs;
    }
    if (m_zSensorData.samples_len >= ARR_SIZE(m_zSensorData.samples))
    {
        return;
    }

    slimSensorSampleStructT &zSample = m_zSensorData.samples[m_zSensorData.samples_len];

    zSample.sampleTimeOffset = calcMsTimeDiffTick(m_zSensorData.timeBase, tTimestampMs);
    zSample.sample[0] = zData[0];
    zSample.sample[1] = zData[1];
    zSample.sample[2] = zData[2];
    m_tLastSampleDelay = readCoreTimeUs() / 1000 - tTimestampMs;

    // Print: index, sample time, sample time offset in clock ticks, sample data
    SLIM_LOGV(
            "%s [%" PRIu8 "] time=%" PRIu64 "ms (+%" PRIu32 "ct; d=%" PRId64 "ms) (%.6f, %.6f, %.6f)",
            m_pzName,
            m_zSensorData.samples_len,
            tTimestampMs,
            m_zSensorData.samples[m_zSensorData.samples_len].sampleTimeOffset,
            m_tLastSampleDelay,
            zData[0], zData[1], zData[2]);

    m_zSensorData.samples_len++;
}

/**
@brief Get batch expiration time

Batch expiration time is computed when the service is enabled, and there is
at least one sample available.

@param[out] tTimeMsOut Expiration time in milliseconds.
@return true if expiration time is present.
*/
bool NDKProvider::ServiceState::getExpireTime
(
    uint64_t &tTimeMsOut
) const
{
    if (m_uRunning && m_zSensorData.samples_len > 0)
    {
        const uint64_t tBatchingIntervalMs = (uint64_t) (1000.0f / m_qBatchingRateHz + .5f);
        tTimeMsOut = m_zSensorData.timeBase + tBatchingIntervalMs + m_tLastSampleDelay;
        return true;
    }
    return false;
}

/**
@brief Reset data buffer

Method clears output data buffer.
*/
void NDKProvider::ServiceState::resetData()
{
    /* Reset data that was sent off */
    memset(&m_zSensorData, 0, sizeof(m_zSensorData));
    switch (m_pzNDKProvider->m_eTimeSource)
    {
    case eTIME_SOURCE_UNKNOWN:
        // Time source is unknown.
        m_zSensorData.timeSource = eSLIM_TIME_SOURCE_UNSPECIFIED;
        break;
    default:
        // Time source is COMMON as provider either gets SLIM time in events or
        // converts timestamps to SLIM time
        m_zSensorData.timeSource = eSLIM_TIME_SOURCE_COMMON;
        break;
    }

    m_zSensorData.provider = eSLIM_SERVICE_PROVIDER_NATIVE;
    m_zSensorData.sensorType = m_eSlimSensorType;
}

/**
@brief Attempts to find NDK provider for the given object.

Method queries ISensorManager provider for existence of sensor provider
with appropriate NDK sensor type. When multiple found, the one with
best (maximum) rate reporting capability is selected.

@return @a true on success; or @a false on failure.
*/
bool NDKProvider::ServiceState::bind
(
    const ASensor &zSensor
)
{
    if (m_lNdkSensorType == ASensor_getType(&zSensor) &&
            (!m_uBound ||
            ((nullptr != m_pzSensor) &&
            (ASensor_getMinDelay(m_pzSensor) > ASensor_getMinDelay(&zSensor)))))
    {
        m_uBound = true;
        m_pzSensor = (ASensor*)&zSensor;
        m_uBatchMode = false;
        return true;
    }
    return false;
}

/**
@brief Constructs object.

Constructs object that produces eSLIM_SERVICE_SENSOR_ACCEL service data from
SENSOR_TYPE_ACCELEROMETER sensor.

@param[in] pzNDKProvider Parent container.
*/
NDKProvider::AccelState::AccelState
(
    NDKProvider *pzNDKProvider
) :
    ServiceState(pzNDKProvider,
            eSLIM_SENSOR_TYPE_ACCEL,
            eSLIM_SERVICE_SENSOR_ACCEL,
            ASENSOR_TYPE_ACCELEROMETER,
            "ACCEL_DATA")
{
}

/**
@brief Collects sensor data.

@param[in] e       Android sensor API event.
@param[in] tEventTimeMs Event time.
@return true if event type matches @ref SENSOR_TYPE_ACCELEROMETER
*/
bool NDKProvider::AccelState::processEvent
(
    const ASensorEvent &zEvent,
    uint64_t tEventTimeMs
)
{
    if (m_lNdkSensorType == zEvent.type)
    {
        float fSensorSamples[3] =
        {
            UNIT_CONVERT_ACCELERATION(zEvent.acceleration.x),
            UNIT_CONVERT_ACCELERATION(zEvent.acceleration.y),
            UNIT_CONVERT_ACCELERATION(zEvent.acceleration.z)
        };
        processRawData(fSensorSamples, tEventTimeMs);
        return true;
    }
    return false;
}

/**
@brief Constructs object.

Constructs object that produces eSLIM_SERVICE_SENSOR_GYRO service data
from SENSOR_TYPE_GYROSCOPE_UNCALIBRATED sensor.

@param[in] pzNDKProvider Parent container.
*/
NDKProvider::GyroState::GyroState
(
    NDKProvider *pzNDKProvider
) :
    ServiceState(pzNDKProvider,
            eSLIM_SENSOR_TYPE_GYRO,
            eSLIM_SERVICE_SENSOR_GYRO,
            SENSOR_TYPE_GYROSCOPE_UNCALIBRATED,
            "GYRO_DATA")
{
}
/**
@brief Collects sensor data.

@param[in] e       Android sensor API event.
@param[in] tEventTimeMs Event time.
@return true if event type matches @ref SENSOR_TYPE_GYROSCOPE_UNCALIBRATED
*/
bool NDKProvider::GyroState::processEvent
(
    const ASensorEvent &zEvent,
    uint64_t tEventTimeMs
)
{
    if (m_lNdkSensorType == zEvent.type)
    {
        float fSensorSamples[3] =
        {
            UNIT_CONVERT_GYRO(zEvent.uncalibrated_gyro.x_uncalib),
            UNIT_CONVERT_GYRO(zEvent.uncalibrated_gyro.y_uncalib),
            UNIT_CONVERT_GYRO(zEvent.uncalibrated_gyro.z_uncalib)
        };
        processRawData(fSensorSamples, tEventTimeMs);
        return true;
    }
    return false;
}

/**
@brief Constructs object.

Constructs object that produces eSLIM_SERVICE_SENSOR_BARO service data
from SENSOR_TYPE_PRESSURE sensor.

@param[in] pzNDKProvider Parent container.
*/
NDKProvider::BaroState::BaroState
(
    NDKProvider *pzNDKProvider
) :
    ServiceState(pzNDKProvider,
            eSLIM_SENSOR_TYPE_BAROMETER,
            eSLIM_SERVICE_SENSOR_BARO,
            SENSOR_TYPE_PRESSURE,
            "PRESSURE_DATA")
{
}

/**
@brief Collects sensor data.

@param[in] e       Android sensor API event.
@param[in] tEventTimeMs Event time.
@return true if event type matches @ref SENSOR_TYPE_PRESSURE
*/
bool NDKProvider::BaroState::processEvent
(
    const ASensorEvent &zEvent,
    uint64_t tEventTimeMs
)
{
    if (m_lNdkSensorType == zEvent.type)
    {
        float fSensorSamples[3] =
        {
            UNIT_CONVERT_PRESSURE(zEvent.pressure),
            0,
            0
        };
        processRawData(fSensorSamples, tEventTimeMs);
        return true;
    }
    return false;
}

/**
@brief Constructs object.

Constructs object that produces eSLIM_SERVICE_SENSOR_MAG_CALIB service data
from SENSOR_TYPE_MAGNETIC_FIELD sensor.

@param[in] pzNDKProvider Parent container.
*/
NDKProvider::MagCalibState::MagCalibState
(
    NDKProvider *pzNDKProvider
):
    ServiceState(pzNDKProvider,
            eSLIM_SENSOR_TYPE_MAGNETOMETER,
            eSLIM_SERVICE_SENSOR_MAG_CALIB,
            ASENSOR_TYPE_MAGNETIC_FIELD,
            "MAG_CALIB")
{
}

/**
@brief Collects sensor data.

@param[in] e       Android sensor API event.
@param[in] tEventTimeMs Event time.
@return true if event type matches @ref SENSOR_TYPE_MAGNETIC_FIELD
*/
bool NDKProvider::MagCalibState::processEvent
(
    const ASensorEvent &zEvent,
    uint64_t tEventTimeMs
)
{
    if (m_lNdkSensorType == zEvent.type)
    {
        float fSensorSamples[3] =
        {
            UNIT_CONVERT_MAG(zEvent.magnetic.x),
            UNIT_CONVERT_MAG(zEvent.magnetic.y),
            UNIT_CONVERT_MAG(zEvent.magnetic.z)
        };
        processRawData(fSensorSamples, tEventTimeMs);
        return true;
    }
    return false;
}

/**
@brief Constructs object.

Constructs object that produces eSLIM_SERVICE_SENSOR_MAG_UNCALIB service
data from SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED sensor.

@param[in] pzNDKProvider Parent container.
*/
NDKProvider::MagUncalibState::MagUncalibState
(
    NDKProvider *pzNDKProvider
):
    ServiceState(pzNDKProvider,
            eSLIM_SENSOR_TYPE_MAGNETOMETER,
            eSLIM_SERVICE_SENSOR_MAG_UNCALIB,
            SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED,
            "MAG_UNCALIB")
{
}

/**
@brief Collects sensor data.

@param[in] e       Android sensor API event.
@param[in] tEventTimeMs Event time.
@return true if event type matches @ref SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED
*/
bool NDKProvider::MagUncalibState::processEvent
(
    const ASensorEvent &zEvent,
    uint64_t tEventTimeMs
)
{
    if (m_lNdkSensorType == zEvent.type)
    {
        float fSensorSamples[3] =
        {
            UNIT_CONVERT_MAG(zEvent.uncalibrated_magnetic.x_uncalib),
            UNIT_CONVERT_MAG(zEvent.uncalibrated_magnetic.y_uncalib),
            UNIT_CONVERT_MAG(zEvent.uncalibrated_magnetic.z_uncalib)
        };
        processRawData(fSensorSamples, tEventTimeMs);
        return true;
    }
    return false;
}

/**
@brief Computes time difference in clock ticks.

Utility function to return the difference in two timestamps in clock ticks.
Accounts for any timestamp rollovers due to the limited range of milliseconds
in uint32_t. (Rolls over ~ every 50 days)

@param[in] tTs1Ms First timestamp in milliseconds. (Comes first in monotonic time)
@param[in] tTs2Ms Second timestamp in milliseconds. (Comes second in monotonic time)

@return Time difference in clock ticks
*/
uint32_t NDKProvider::calcMsTimeDiffTick(uint64_t tTs1Ms, uint64_t tTs2Ms)
{
    uint64_t tDiffMs = 0;
    if (tTs1Ms <= tTs2Ms)
    {
        tDiffMs = tTs2Ms - tTs1Ms;
    }
    else
    {
        tDiffMs = std::numeric_limits<uint64_t>::max() - tTs1Ms + tTs2Ms + 1;
    }
    return slim_TimeToClockTicks(tDiffMs);
}

/**
@brief Constructs provider instance.

Initialization function for NDK provider internal state.
*/
NDKProvider::NDKProvider() :
    Thread(false),
    MultiplexingProvider(SLIM_PROVIDER_NDK, false, 0),
    m_zLooper(0),
    m_pzEventQueue(0),
    m_pzBinderTimer(0),
    m_pzBatchTimer(0),
    m_tTimestampDeltaUs(0),
    m_tTimestampExpireTimeUs(0),
    m_zLock(),
    m_zKF(),
    m_eTimeSource(eTIME_SOURCE_UNKNOWN),
    m_zStateList()
{
    m_zStateList.reserve(5);
}

/**
@brief Main polling task function.

Task that polls the sensor for sensor data. This thread will read a sample,
convert it and batch to send to the SLIM task for multiplexing.

@return false This function runs only once.
*/
bool NDKProvider::threadLoop()
{
    SLIM_CB_CTX_PUSH("NDKPoll");
    SLIM_LOGD("Starting polling task");

    slim_ThreadT *pzThreadSelf = slim_ThreadSelf();

    if (0 == pzThreadSelf)
    {
        SLIM_LOGF("No local thread handle");
        abort();
    }

    slim_IpcStart(THREAD_ID_NDK);
    m_pzBinderTimer = slim_TimerCreate(eIPC_TIMER_ID_REBIND, THREAD_ID_NDK);
    m_pzBatchTimer = slim_TimerCreate(eIPC_TIMER_ID_BATCH, THREAD_ID_NDK);

    m_zLooper = new Looper(false);

    m_zLooper->addFd(
            pzThreadSelf->i_EventFd,
            ALOOPER_POLL_CALLBACK,
            ALOOPER_EVENT_INPUT,
            new EventHandler(this),
            pzThreadSelf);
    m_zLooper->addFd(
            pzThreadSelf->i_SignalFd,
            ALOOPER_POLL_CALLBACK,
            ALOOPER_EVENT_INPUT,
            new SignalHandler(this),
            pzThreadSelf);

    slim_TaskReadyAck();

    m_pLooper = ALooper_prepare(0);

    SLIM_LOGD("Initiating sensor bind operation.");
    bindNDKSensors(0);

    SLIM_LOGD("Wait in a loop until exit ...");
    /* Wait indefinitely until stop. */
    while (!exitPending())
    {
        int lIdent = m_pLooper->pollOnce(-1, NULL, NULL, NULL);
        switch (lIdent)
        {
        case ALOOPER_POLL_WAKE:
            SLIM_LOGV("Looper::pollOnce wake");
            break;
        case ALOOPER_POLL_ERROR:
            SLIM_LOGF("Android Sensor API poll error: %d %s",
                    errno, strerror(errno));
            exit(1);
            break;
        case ALOOPER_POLL_CALLBACK:
            SLIM_LOGV("ALOOPER_POLL_CALLBACK");
            break;
        case ALOOPER_POLL_TIMEOUT:
        default:
            SLIM_LOGW("Unexpected poll result %d", lIdent);
            break;
        }
    }
    SLIM_LOGD("Finishing polling task");

    cleanupSensors();

    m_zLooper->removeFd(pzThreadSelf->i_EventFd);
    m_zLooper->removeFd(pzThreadSelf->i_SignalFd);
    slim_TimerDestroy(m_pzBinderTimer);
    m_pzBinderTimer = 0;
    slim_TimerDestroy(m_pzBatchTimer);
    m_pzBatchTimer = 0;

    SLIM_CB_CTX_POP();
    return false;
}


/**
@brief Internal helper for handling thread signal events.

@param[in] lFd     File handler.
@param[in] lEvents Event mask.
@param[in] pData   Event cookie.
*/
int NDKProvider::processSignalEvent
(
    int lFd,
    int lEvents,
    void *pData
)
{
    slim_ThreadT *pz_Thread = (slim_ThreadT*)(pData);
    struct signalfd_siginfo z_SigInfo;
    ssize_t tRes;

    SLIM_LOGV("Processing signal event: fd=%d events=%04x", lFd, lEvents);

    tRes = read(lFd, &z_SigInfo, sizeof(z_SigInfo));
    if (tRes < 0)
    {
        SLIM_LOGW("Signal reading error: %d %s", errno, strerror(errno));
    }
    else if (sizeof(z_SigInfo) == tRes)
    {
        SLIM_LOGV("Processing signal %u", (unsigned)z_SigInfo.ssi_signo);
        switch (z_SigInfo.ssi_signo)
        {
        case SIGUSR1:
        case SIGUSR2:
            return 1;
        default:
            if ((unsigned)SLIM_SIG_TIMER == z_SigInfo.ssi_signo)
            {
                SLIM_LOGV("Detected TIMER");
                {
                    slim_TimerMsgT z_TimerMsg;

                    if (slim_TimerMarkStopped(
                            (slim_TimerT*) (uintptr_t) z_SigInfo.ssi_ptr,
                            &z_TimerMsg,
                            0))
                    {
                        handleTimerEvent(z_TimerMsg.q_TimerId, z_TimerMsg.q_Param);
                        return 1;
                    }
                }
            }
            break;
        }
    }

    return 1;
}

/**
@brief Internal helper for handling thread events.

@param[in] lFd     File handler.
@param[in] lEvents Event mask.
@param[in] pData   Event cookie.
*/
int NDKProvider::processThreadEvent
(
    int lFd,
    int lEvents,
    void *pData
)
{
    slim_ThreadT *pz_Thread = (slim_ThreadT*)(pData);
    slim_IpcMsgT *pz_Msg = 0;

    bool b_Abort = false;
    bool b_Reset = false;
    SLIM_LOGV("Processing thread event: fd=%d events=%04x", lFd, lEvents);
    {
        MutexLock _l(pz_Thread->z_Mutex);
        b_Abort = 0 != (pz_Thread->q_Flags & SLIM_THREAD_FLAG_ABORT);
        if (!b_Abort)
        {
            pz_Msg = SLIM_LIST_HEAD(pz_Thread->z_IpcQueue);
            if (0 != pz_Msg)
            {
                SLIM_LIST_REMOVE(pz_Thread->z_IpcQueue, pz_Msg, z_Link);
                if (0 == SLIM_LIST_HEAD(pz_Thread->z_IpcQueue))
                {
                    pz_Thread->b_EventState = false;
                    b_Reset = true;
                }
            }
            else
            {
                pz_Thread->b_EventState = false;
                b_Reset = true;
            }
        }
        if (b_Abort || b_Reset)
        {
            uint64_t tTemp;
            read(lFd, &tTemp, sizeof(tTemp));
        }
    }

    if (0 != pz_Msg)
    {
        handleIpc(
                pz_Msg->q_MsgId,
                pz_Msg->q_Size,
                0 != pz_Msg->q_Size ? pz_Msg->p_Data : 0);
                slim_IpcDelete(pz_Msg);
    }

    return 1;
}

sp<NDKProvider>           myPzNDKProvider;   /**< Owner */

/**
@brief Handler for timer messages

Timer messages are belonging to the worker thread.

@param[in] qTimerId Timer identifier.
@param[in] qParam   Timer parameter.

@sa slim_Timer
*/
void NDKProvider::handleTimerEvent
(
    uint32_t qTimerId,
    uint32_t qParam
)
{
    SLIM_LOGV("Timer event: timer=%" PRIu32 " param=%" PRIu32, qTimerId, qParam);
    switch (qTimerId)
    {
    case eIPC_TIMER_ID_REBIND:
        {
        // Attempt connect to ISensorManager and query available sensors
        bindNDKSensors(qParam);
        }
        break;
    case eIPC_TIMER_ID_BATCH:
        {
        // Lock object state
        MutexLock _l(myPzNDKProvider->m_zLock);
        processBatchTimeout();
        }
        break;
    default:
        SLIM_LOGW("Unexpected timer: %" PRIu32, qTimerId);
        break;
    }
}

/**
@brief Handler for IPC messages

IPC messages are belonging to the worker thread.

@param[in] qMsgId   Message identifier.
@param[in] qMsgSize Message payload size.
@param[in] pMsgData Message payload.

@sa slim_IPC
*/
void NDKProvider::handleIpc
(
    uint32_t qMsgId,
    uint32_t qMsgSize,
    const void *pMsgData
)
{
    SLIM_UNUSED(pMsgData);
    SLIM_LOGV("Got IPC: %" PRIu32 " size=%" PRIu32, qMsgId, qMsgSize);
    switch (qMsgId)
    {
    case eIPC_MSG_ID_REBIND:
        // Attempt connect to ISensorManager and query available sensors
        bindNDKSensors(0);
        break;
    default:
        SLIM_LOGW("Unexpected message: %" PRIu32, qMsgId);
        break;
    }
}

// slim::MultiplexingProvider
slimErrorEnumT NDKProvider::doUpdateSensorStatus
(
    bool uEnable,
    uint16_t wReportRateHz,
    uint16_t wSampleCount,
    slimServiceEnumT eService
)
{
    if (!isRunning() || exitPending())
    {
        SLIM_LOGE("Worker thread is not started or exiting");
        return eSLIM_ERROR_INTERNAL;
    }

    // Define sensor string and state based on the sensor type.
    MutexLock _l(m_zLock);
    for (StateVector::const_iterator it = m_zStateList.begin();
            it != m_zStateList.end();
            ++it)
    {
        sp<ServiceState> pzServiceState = *it;
        if (pzServiceState->m_eSlimServiceType == eService)
        {
            return pzServiceState->updateStatus(uEnable, wReportRateHz, wSampleCount);
        }
    }

    SLIM_LOGE("No %d service to start/stop", eService);
    return eSLIM_ERROR_UNSUPPORTED;
}

void NDKProvider::serviceDied
(
    uint64_t cookie,
    const ::android::wp<::android::hidl::base::V1_0::IBase>& who
)
{
    SLIM_UNUSED(cookie);
    SLIM_UNUSED(who);

    SLIM_CB_CTX_PUSH("IBinder");
    SLIM_LOGW("Binder service disconnected, reinitializing.");

    cleanupSensors();
    SLIM_CB_CTX_POP();
}

/**
@brief Performs reinitialization of ISensorManager connection.

Unregisters self as a death recipient from manager interface, requests all
sensors to stop, clears sensor list, updates SLIM core with service change
notification and enqueues reinitialization.
*/
void NDKProvider::cleanupSensors()
{
    slimAvailableServiceMaskT qServiceMask = 0;
    SLIM_LOG_ENTRY();

    // This method can be called from two threads: worker, on error or cleanup
    // and from IBinder callback.
    StateVector zStateList;
    sp<Looper> pzLooper;
    {
        MutexLock _l(m_zLock);
        qServiceMask = m_qServiceMask;
        m_qServiceMask = 0;
        slim_ServiceStatusReset(&m_zProviderStatus);
        m_zStateList.clear();
        pzLooper = m_zLooper;
    }
    m_pzEventQueue = nullptr;
    if (!slim_TimerStop(m_pzBinderTimer))
    {
        SLIM_LOGE("Failed to stop binder timer");
    }
    if (!slim_TimerStop(m_pzBatchTimer))
    {
        SLIM_LOGE("Failed to stop batch timer");
    }
    if (0 != qServiceMask)
    {
        // Send message to SLIM core via provider routing message API.
        routeConfigurationChange(qServiceMask);
    }
    // Send message to self to ensure correct order of messages:
    // - Disconnect -> Service unavailable
    // - Reopen -> Bind -> Service available
    routeMessageToSelf(ePROVIDER_MSG_ID_REOPEN, 0, 0);
}

/**
@brief Handler for messages targeted for provider.

Function handles messages sent via SLIM core to provider.

@param[in] qMessageId      Message id.
@param[in] qMsgPayloadSize Message size.
@param[in] pMsgPayload     Pointer to the message payload.
*/
void NDKProvider::handleProviderMessage
(
    uint32_t qMessageId,
    uint32_t qMsgPayloadSize,
    const void *pMsgPayload
)
{
    switch (qMessageId)
    {
    case ePROVIDER_MSG_ID_REOPEN:
    {
        SLIM_LOGD("Processing REOPEN message");
        slim_IpcSendEmpty(THREAD_ID_NDK, C_USR_MSG_ID_FIRST);
        break;
    }
    default:
        MultiplexingProvider::handleProviderMessage(
                qMessageId,
                qMsgPayloadSize,
                pMsgPayload);
        break;
    }
}

int myAcallback(int lFd, int lEventMask, void* pData)
{
    SLIM_UNUSED(lFd);
    SLIM_UNUSED(lFd);
    SLIM_UNUSED(lEventMask);
    if (0 != (ALOOPER_EVENT_HANGUP & lEventMask) ||
        0 != (ALOOPER_EVENT_ERROR & lEventMask))
    {
        SLIM_LOGW("Error event fd=%d mask=%08x", lFd, lEventMask);
        myPzNDKProvider->cleanupSensors();
        return 0;
    }

    // Lock object state
    MutexLock _l(myPzNDKProvider->m_zLock);

    // Buffer for reading raw events from the queue
    // The events are sorted by timestamp by event queue facility.
    ASensorEvent         zEventBuffer[SLIM_SENSOR_MAX_SAMPLE_SETS * 4];

    ssize_t lNumRead = myPzNDKProvider->m_pzEventQueue->getEvents(zEventBuffer, ARR_SIZE(zEventBuffer));

    SLIM_LOGV(
              "Samples read = %" PRIdPTR "; batch mode: %d fd=%d mask=%08x",
              lNumRead,
              false,
              lFd,
              lEventMask);

    /* Events processing.
    * ~~~~~~~~~~~~~~~~~~
    * When provider gets events from the Android sensor stack, each sample has
    * a timestamp. The time source is not strictly defined in Android API and
    * depends on implementation of the sensor driver in kernel.
    *
    * For newer kernels, the time source for sensor samples matches Android
    * android::elapsedRealtimeNano() time source, but in older platforms other
    * time source have been used. Generally there are 5 known time sources
    * for samples:
    * - android::elapsedRealtimeNano()
    * - systemTime(SYSTEM_TIME_BOOTTIME)
    * - systemTime(SYSTEM_TIME_MONOTONIC)
    * - systemTime(SYSTEM_TIME_REALTIME)
    * - Unknown time source.
    *
    * The above time sources can be split into three categories:
    * - If time source matches SLIM time source, the provider acts in SLIM
    * common time domain.
    * - If time source doesn't match SLIM time source, but known, provider
    * can compute time offset for each sample and thus convert time to
    * SLIM time domain. In this case the error is minimal and relates to time
    * difference between two system calls and operates well event when clock
    * value jumps.
    * - If time source is not known, a heuristic algorithm is used based on
    * Kalman Filter implementation. In practice, this means two Kalman filters
    * are used: one inside the provider, and another one inside SLIM core. This
    * option is not very reliable and subject to clock source time jumps, as well
    * as it is problematic to use with data batching.
    */

    // Get time difference between sensor time and SLIM time if it is known
    const int64_t rTimestampDeltaUs = myPzNDKProvider->getTimestampDeltaUs();

    //! @todo Add support for clock drifts with KF.\n
    //!       Currently clock drifts are compensated by timeout control.\n
    //!       Clock time may drift due to the following reasons:
    //!       - NTP time adjustments. This affects CLOCK_MONOTONIC and
    //!         CLOCK_REALTIME clock sources.
    //!       - Suspend/power on. This affect CLOCK_MONOTONIC clock source.
    //!       - Time change due to user request. This affects CLOCK_REALTIME clock
    //!         source.

    if (NDKProvider::eTIME_SOURCE_UNKNOWN == myPzNDKProvider->m_eTimeSource)
    {
        // When KF is used, only last acceptable timestamp from a potential sample
        // batch shall be used. Also values with invalid timestamps shall be ignored

        // KF update is done here because time query to it occurs right after a
        // first sample batch is dispatched and KF structures must be initialized.

        int64_t rLastSampleTimestampNs = 0;

        for (ssize_t lI = 0; lI < lNumRead; lI++)
        {
            const ASensorEvent &zSensorEvent = zEventBuffer[lI];
            if (SENSOR_TYPE_META_DATA != zSensorEvent.type &&
                        rLastSampleTimestampNs < zSensorEvent.timestamp)
            {
                rLastSampleTimestampNs = zSensorEvent.timestamp;
            }
        }
        if (0 != rLastSampleTimestampNs)
        {
            // Update Kalman Filter with time difference value.
            // Take time stamp to adjust for time sync later.
            // Add fixed estimated RPC transport delay between Sensor HAL and SLIM
            uint64_t tSysTimeUs = myPzNDKProvider->readCoreTimeUs();
            double dClockOffsetUs = tSysTimeUs -
                nanoseconds_to_microseconds(rLastSampleTimestampNs) -
                myPzNDKProvider->s_tEstimatedSampleDelayUs;
            if (!myPzNDKProvider->m_zKF.isInitialized())
            {
                myPzNDKProvider->m_zKF.init(dClockOffsetUs, 0, tSysTimeUs);
            }
            else if (myPzNDKProvider->m_zKF.needsUpdate(tSysTimeUs))
            {
                myPzNDKProvider->m_zKF.update(dClockOffsetUs, 0, tSysTimeUs);
            }
        }
    }

    for (ssize_t lI = 0; lI < lNumRead; lI++)
    {
        const ASensorEvent &zSensorEvent = zEventBuffer[lI];
        // Get sensor time in microseconds
        // add constant time shift if known
        int64_t tTimestampUs = nanoseconds_to_microseconds(zSensorEvent.timestamp) +
            rTimestampDeltaUs;

        // Get SLIM time in milliseconds.
        const int64_t tTimestampMs = tTimestampUs / 1000;

        typedef android::Vector<sp<NDKProvider::ServiceState> > StateVector;
        for (StateVector::const_iterator it = myPzNDKProvider->m_zStateList.begin();
            it != myPzNDKProvider->m_zStateList.end();
            ++it)
        {
            sp<NDKProvider::ServiceState> pzServiceState = *it;
            // virtual call to handle particular sensor type.
            if (pzServiceState->m_uRunning &&
                pzServiceState->processEvent(zSensorEvent, tTimestampMs))
            {
                // When event is consumed, test if data send is required.
                // force send data if batch size reached or time exceeded
                // (for non-batch mode)
                pzServiceState->sendIfReady(pzServiceState->m_uBatchMode ? false : true);
            }
        }
    }
    myPzNDKProvider->processBatchTimeout();

    return 1;
}

void NDKProvider::bindNDKSensors
(
    uint32_t qAttemptNo
)
{
    slimAvailableServiceMaskT qServiceMask = 0;
    bool                      vSuccess = false;

    do
    {
        MutexLock _l(m_zLock);

        qServiceMask = m_qServiceMask;
        m_zStateList.clear();
        m_qServiceMask = 0;

        /* Get the available sensors */
        ASensorList mySensorList;
        ASensorManager* aManager;
        ASensorEventQueue* aQueue;
        int ident = 0;

        SLIM_LOGD("Connecting ASensorManager; attempt=%" PRIu32, qAttemptNo);

        myPzNDKProvider = this;   /**< Owner */

        aManager = ASensorManager::getInstance();
        if (nullptr == aManager)
        {
            SLIM_LOGE("ASensorManager manager is NULL.");
            break;
        }
        aQueue = aManager->createEventQueue(m_pLooper, ident, myAcallback, NULL);
        m_pzEventQueue = aQueue;

        if (nullptr == m_pzEventQueue) {
            /* error handling */
            SLIM_LOGE("createEventQueue returned nullptr!!!");
            break;
        }

        int noOfSensors = aManager->getSensorList(&mySensorList);
        if (noOfSensors <= 0) {
            /* error handling */
            SLIM_LOGE("getSensorList failed noOfSensors=%d",
                noOfSensors);
            break;
        }
        // Sensor connection:
        // - Create all state objects
        // - Iterate trough all available sensors
        //   - Iterate though all available state objects
        //     - Bind state object if first or best match
        // - Iterate through all state objects
        //   - Remove object if not bound
        //   - Update service mask if bound
        m_zStateList.push_back(new AccelState(this));
        m_zStateList.push_back(new GyroState(this));
        m_zStateList.push_back(new BaroState(this));
        m_zStateList.push_back(new MagCalibState(this));
        m_zStateList.push_back(new MagUncalibState(this));

        IF_SLIM_LOGV
        {
            for (ssize_t i = 0; i < noOfSensors; i++)
            {
                // Print discovered Android sensors.
                if (nullptr == mySensorList[i]) {
                    break;
                }
                SLIM_LOGV(
                    "Detected Sensor Name: %s Vendor: %s Type: %s (%" PRId32 ")"
                    " Min Delay: %" PRId32,
                    ASensor_getName(mySensorList[i]),
                    ASensor_getVendor(mySensorList[i]),
                    ASensor_getStringType(mySensorList[i]),
                    ASensor_getType(mySensorList[i]),
                    ASensor_getMinDelay(mySensorList[i]));
            }
        }

        for (ssize_t i = 0; i < noOfSensors; i++)
        {
            for (StateVector::iterator it = m_zStateList.begin();
                it != m_zStateList.end(); ++it)
            {
                (*it)->bind(*(mySensorList[i]));
            }
        }

        for (StateVector::iterator it = m_zStateList.begin(); it != m_zStateList.end();)
        {
            const sp<ServiceState> &pzState = *it;
            if (pzState->isBound())
            {
                if (NULL != pzState->m_pzSensor) {
                    SLIM_LOGD(
                        "Found Sensor m_pzSensor=%p Name: %s Vendor: %s Type: %s (%" PRId32 ")"
                        " Min Delay: %" PRId32,
                        pzState->m_pzSensor,
                        ASensor_getName(pzState->m_pzSensor),
                        ASensor_getVendor(pzState->m_pzSensor),
                        ASensor_getStringType(pzState->m_pzSensor),
                        ASensor_getType(pzState->m_pzSensor),
                        ASensor_getMinDelay(pzState->m_pzSensor));
                }
                else {
                    SLIM_LOGE("pzState->m_pzSensor is NULL!");
                }

                SLIM_MASK_SET(m_qServiceMask, pzState->m_eSlimServiceType);
                ++it;
            }
            else
            {
                it = m_zStateList.erase(it);
            }
        }

        /* No sensors were found for use. */
        if (0 == m_qServiceMask)
        {
            SLIM_LOGW("No NDK sensors found!");
        }
        else
        {
            SLIM_LOGD("Available sensor mask: %08" PRIX32, m_qServiceMask);
        }

        qServiceMask ^= m_qServiceMask;
        vSuccess = true;
    } while (0);
    if (!vSuccess)
    {
        if (slim_TimerStart(m_pzBinderTimer, s_qNdkRebindTimeoutMs, qAttemptNo + 1))
        {
            SLIM_LOGD("Delaying bind operation by %" PRIu32 "ms.", s_qNdkRebindTimeoutMs);
        }
        else
        {
            SLIM_LOGE("Bind operation delay failed.");
        }
    }
    if (0 != qServiceMask)
    {
        // Notify SLIM core
        routeConfigurationChange(qServiceMask);
    }
    SLIM_LOGE("Exit bindNDKSensors");
}

/**
@brief Sets up batch timer.

The method iterates through services and for active ones dispatches expired
ones and computes timeout for others.

If the timeout is computed, the batch timer is set.

Note:
Callers need to lock object state as this is called from multiple threads
*/
void NDKProvider::processBatchTimeout()
{
    const uint64_t tCoreTimeMs = readCoreTimeUs() / 1000;
    uint64_t tNextExpireMs = UINT64_MAX;
    for (StateVector::const_iterator it = m_zStateList.begin();
            it != m_zStateList.end();
            ++it)
    {
        sp<ServiceState> pzServiceState = *it;
        uint64_t tServiceExpireTimeMs;
        if (pzServiceState->getExpireTime(tServiceExpireTimeMs))
        {
            if (tCoreTimeMs >= tServiceExpireTimeMs)
            {
                pzServiceState->sendDataBatch(eBATCH_SEND_REASON_TIMER);
            }
            else if (tNextExpireMs > tServiceExpireTimeMs)
            {
                tNextExpireMs = tServiceExpireTimeMs;
            }
        }
    }

    if (UINT64_MAX == tNextExpireMs)
    {
        slim_TimerStop(m_pzBatchTimer);
    }
    else
    {
        // Timeout is a difference between next expected event time and current
        // time plus few milliseconds to let system schedule appropriate event.
        uint32_t qTimeoutMs = tNextExpireMs - tCoreTimeMs + 50;
        if (qTimeoutMs > 1050)
        {
            qTimeoutMs = 1050;
        }
        slim_TimerStart(m_pzBatchTimer, qTimeoutMs, 0);
    }
}

/**
@brief Compute and provide timestamp delta

Timestamp delta is a difference between SLIM Core time and sensor time.
When the Android Sensor HAL time source matches SLIM time source, the
difference is always 0. Otherwise it is a delta in milliseconds between
Sensor HAL clock and SLIM Core.

Due to nature of times, the raw difference between two calls may jump within
+-10ms (or thread quantum range).

@return time difference between clocks.
*/
int64_t NDKProvider::getTimestampDeltaUs()
{
    int64_t rRetValUs = 0;
    switch(m_eTimeSource)
    {
    case eTIME_SOURCE_UNKNOWN:
        // In case of unknown time source, the constant offset is not used
        rRetValUs = 0;
        break;
    case eTIME_SOURCE_BOOTTIME:
    case eTIME_SOURCE_ELAPSED:
        rRetValUs = 0;
        break;
    case eTIME_SOURCE_MONOTONIC:
    case eTIME_SOURCE_REALTIME:
    {
        const uint64_t tCoreTimeUs = readCoreTimeUs();
        if (tCoreTimeUs >= m_tTimestampExpireTimeUs)
        {
            // Get time difference between sensor time and SLIM time
            const int64_t tTimestampDeltaUs = tCoreTimeUs - readSensorTimeUs();
            SLIM_LOGD("Time adjustment: prev=%" PRId64 "us new=%" PRId64 "us",
                    m_tTimestampDeltaUs, tTimestampDeltaUs);
            m_tTimestampDeltaUs = tTimestampDeltaUs;
            m_tTimestampExpireTimeUs = tCoreTimeUs + s_tTimeDiffUpdateTimeoutUs;
        }
        rRetValUs = m_tTimestampDeltaUs;
    }
        break;
    default:
        SLIM_LOGF("Bad time source value: %d", (int)m_eTimeSource);
        abort();
        break;
    }
    return rRetValUs;
}

/**
@brief Method for completing provider initialization

Method connects to Android Sensor objects and launches polling thread.

@retval true on success.
*/
bool NDKProvider::initialize()
{
    SLIM_LOGD("Initializing Android Sensor NDK Instance");
    SLIM_LOGV("Starting sensor polling thread");
    android::status_t lError = run("NDK");

    if (NO_ERROR != lError)
    {
        SLIM_LOGE(
                "Could not properly start the sensor polling thread! Error: %" PRIi32,
                (int32_t)lError);
        return false;
    }

    SLIM_LOGV("Polling thread initialized!");

    return true;
}

/**
@brief Destroys provider.
*/
NDKProvider::~NDKProvider()
{
    if ( isRunning() )
    {
        SLIM_LOGD("Stopping sensor polling thread");
        requestExit();
        m_zLooper->wake();
        join();

        SLIM_LOGD("Finished joining sensor polling thread.");
    }
}

/**
@brief Get SLIM core timer time in microseconds.

@return SLIM core time in microseconds.
*/
uint64_t NDKProvider::readCoreTimeUs()
{
    return nanoseconds_to_microseconds(android::elapsedRealtimeNano());
}
/**
@brief Get Android Sensor timer time in microseconds.

@return Time in microseconds from Android's Sensor source.
 */
uint64_t NDKProvider::readSensorTimeUs()
{
    int64_t rTimeNSec = 0;

    switch (m_eTimeSource)
    {
    case eTIME_SOURCE_ELAPSED:
        rTimeNSec = android::elapsedRealtimeNano();
        break;
    case eTIME_SOURCE_BOOTTIME:
        rTimeNSec = systemTime(SYSTEM_TIME_BOOTTIME);
        break;
    case eTIME_SOURCE_MONOTONIC:
        rTimeNSec = systemTime(SYSTEM_TIME_MONOTONIC);
        break;
    case eTIME_SOURCE_REALTIME:
        rTimeNSec = systemTime(SYSTEM_TIME_REALTIME);
        break;
    default:
        SLIM_LOGE("Invalid call.");
        break;
    }

    return nanoseconds_to_microseconds(rTimeNSec);
}

/**
 * @brief Initiates time offset request.
 *
 * Function for making the time request. Successful response enable SLIM to
 * calculate the offset between modem time and sensor time.
 *
 * @param[in] lTxnId Service transaction id.
 *
 * @return eSLIM_SUCCESS if time request is made successfully.
 */
slimErrorEnumT NDKProvider::getTimeUpdate
(
    int32_t lTxnId
)
{
    uint64_t tSensorTimeMs = 0;

    switch (m_eTimeSource)
    {
    case eTIME_SOURCE_UNKNOWN:
    {
        uint64_t tSnsApOffsetUs = 0;
        {
            double dMeas = 0;
            MutexLock _lock(m_zLock);
            if (m_zKF.getFiltMeas(dMeas))
            {
                tSnsApOffsetUs = (uint64_t)dMeas;
            }
        }
        uint64_t tCurrentSysTimeUs = readCoreTimeUs();
        if ( tCurrentSysTimeUs < tSnsApOffsetUs  )
        {
            SLIM_LOGE("Invalid Sensor-to-AP offset.");
            return eSLIM_ERROR_INTERNAL;
        }
        uint64_t tCurrentSensorTimeUs = tCurrentSysTimeUs - tSnsApOffsetUs;
        tSensorTimeMs = tCurrentSensorTimeUs / UINT64_C(1000);
        break;
    }
    default:
        tSensorTimeMs = readSensorTimeUs() / UINT64_C(1000);
        break;
    }

    dispatchTimeResponse(lTxnId, tSensorTimeMs, tSensorTimeMs);

    return eSLIM_SUCCESS;
}

/**
 * @brief Returns SLIM interface
 *
 * The method initializes singleton instance and returns SLIM provider
 * interface.
 *
 * @return Provider object
 */
sp<NDKProvider> NDKProvider::getInstance()
{
    MutexLock _l(s_zInstanceLock);
    if (0 == s_pzInstance.get())
    {
        s_pzInstance = new slim::NDKProvider;
        if (!s_pzInstance->initialize())
        {
            SLIM_LOGE("Failed to initialize NDK provider");
            exit(1);
        }
    }
    return s_pzInstance;
}

/**
 * @brief Set sensor HAL sample time source.
 *
 * @param[in] eTimeSource Sensor HAL time source
 */
void NDKProvider::setTimeSource
(
    TimeSource eTimeSource
)
{
    switch (eTimeSource)
    {
    case eTIME_SOURCE_BOOTTIME:
    case eTIME_SOURCE_ELAPSED:
    case eTIME_SOURCE_MONOTONIC:
    case eTIME_SOURCE_REALTIME:
    case eTIME_SOURCE_UNKNOWN:
        m_eTimeSource = eTimeSource;
        break;
    default:
        SLIM_LOGE("Unknown time source value: %d; UNKNOWN source will be used",
                (int)eTimeSource);
        m_eTimeSource = eTIME_SOURCE_UNKNOWN;
        break;
    }
}


} /* namespace slim */

//! @}
