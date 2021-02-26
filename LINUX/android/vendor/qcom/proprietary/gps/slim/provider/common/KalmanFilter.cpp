/*
Copyright (c) 2011, 2015 Qualcomm Technologies, Inc.
All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
*/
/**
 * @file
 * @brief 1-State KF w/outlier detection
 *
 * This header file is a refactored version of os_kf.c header file from
 * GSIFF daemon.
 *
 * @ingroup slim_ProviderCommon
 */

#include <KalmanFilter.h>

#include <slim_os_log_api.h>

#include <inttypes.h>

//! @addtogroup slim_ProviderCommon
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/* This unit logs as MAIN component. */
#undef SLIM_LOG_LOCAL_MODULE
//! @brief Kalman filter logs as MAIN module
#define SLIM_LOG_LOCAL_MODULE SLIM_LOG_MOD_MAIN

//! @}

namespace slim
{

/**
 * @brief Constructs filter object with given parameters
 *
 * @param[in] uOutlierLimit       Number of outliers before a reset
 * @param[in] uVarThresh          Expect/Meas variance threshold scalar
 * @param[in] qMinUncToInit       Min unc allowed to initialize filter
 * @param[in] qDivergeThreshold   Min Residual diff to check for outlier
 * @param[in] tTimeUpdateInterval How often to update the filter
 */
KalmanFilter::KalmanFilter
(
  uint8_t uOutlierLimit,
  uint8_t uVarThresh,
  uint32_t qMinUncToInit,
  uint32_t qDivergeThreshold,
  uint64_t tTimeUpdateInterval
) :
    m_uOutlierLimit(uOutlierLimit),
    m_uVarThresh(uVarThresh),
    m_qMinUncToInit(qMinUncToInit),
    m_qDivergeThreshold(qDivergeThreshold),
    m_tTimeUpdateInterval(tTimeUpdateInterval),
    m_bFilterInit(false),
    m_tTimeUpdate(0),
    m_uOutlierCount(0),
    m_dFiltMeas(0.),
    m_dIntFiltMeas(0.),
    m_dFiltMeasVar2(0.)
{
}

/**
 * @brief Destroys filter and releases it's resources.
 */
KalmanFilter::~KalmanFilter()
{
}

/**
 * @brief Initializes the filter with the given value if not initialized yet.
 *
 * @param[in] dMeas        Unfiltered measurement.
 * @param[in] dMeasUnc     Unfiltered uncertainty.
 * @param[in] tTimeUpdate  Time of this measurement in any time base as long
 *                         as it is consistent.
 *
 * @retval true  On success.
 * @retval false On failure.
 */
bool KalmanFilter::init
(
  double dMeas,
  double dMeasUnc,
  uint64_t tTimeUpdate
)
{
   if ( (!m_bFilterInit) &&
        (dMeasUnc <= m_qMinUncToInit) )
   {
      SLIM_LOGD("Filter initialized");

      m_tTimeUpdate = tTimeUpdate;
      m_uOutlierCount = 0;
      m_bFilterInit = true;
      m_dIntFiltMeas = dMeas;
      m_dFiltMeas = dMeas;
      m_dFiltMeasVar2 = (dMeas*dMeas);

      SLIM_LOGD("Filtered timeMs=%" PRIu64 ", FiltMeas=%lf",
                tTimeUpdate, m_dFiltMeas);
   }

   return true;
}

/**
 * @brief Updates the filter given the provided unfiltered measurement
 *
 * @param[in] dMeas        Unfiltered measurement.
 * @param[in] dMeasUnc     Unfiltered uncertainty.
 * @param[in] tTimeUpdate  Time of this measurement in any time base as long
 *                         as it is consistent.
 *
 * @retval true  On success.
 * @retval false On failure.
 */
bool KalmanFilter::update
(
  double dMeas,
  double dMeasUnc,
  uint64_t tTimeUpdate
)
{
  double dDeltaTime = 0;
  double dFiltVarProp2 = 0;
  double dResidual = 0;
  double dMeasVar2 = 0;
  double dExpectedVar2 = 0;
  double dKfGain = 0;
  bool bUpdateFilter = true;

  /* Filter not initialized. Use the unfiltered meas. */
  if ( !m_bFilterInit )
  {
    SLIM_LOGW("Filter not initialized. Using unfiltered meas.");
    m_dFiltMeas = dMeas;
    return true;
  }

  SLIM_LOGV("Unfiltered timeMs=%" PRIu64 ", unFiltMeas=%lf, unFiltMeasUnc=%lf",
            tTimeUpdate, dMeas, dMeasUnc);

  /* A filter update is possible. Set as true initially, and then lower to
     FALSE if a condition below is met. */
  bUpdateFilter = true;

  /* Compute time difference between time of current TsOffset estimate and last
     filter update */
  dDeltaTime = (double)(tTimeUpdate - m_tTimeUpdate);

  /* Only update the filter when specified. */
  if ( dDeltaTime >= m_tTimeUpdateInterval )
  {
    dFiltVarProp2 = m_dFiltMeasVar2;

    /* Form Kalman filter measurement residual and measured variance */
    dResidual = dMeas - m_dIntFiltMeas;
    dMeasVar2 = dResidual * dResidual;

    /* Compute the expected variance */
    dExpectedVar2 = dFiltVarProp2 + (dMeasUnc * dMeasUnc);

    /* Perform chi-squared test and residual threshold test for outlier
     * rejection.
     */
    SLIM_LOGD("Filter dResidual=%lf dExpectedVar2=%lf, dMeasVar2=%lf",
              dResidual,
              dExpectedVar2,
              dMeasVar2);
    if ( (fabs(dResidual) > m_qDivergeThreshold) &&
         (dMeasVar2 > ( m_uVarThresh * dExpectedVar2)) )
    {
      bUpdateFilter = false;
      //kf_state->t_TimeUpdate = t_TimeUpdate;
      m_uOutlierCount++;

      SLIM_LOGD("Filter outlier detection. timeMs=%" PRIu64
                ", d_MeasVar2=%.3lf, d_ExpectedVar2=%.3lf",
                tTimeUpdate, dMeasVar2, dExpectedVar2);

      if (m_uOutlierLimit <= m_uOutlierCount)
      {
        /* Reset KF */
        m_bFilterInit = false;

        SLIM_LOGD("Filter reset. timeMs=%" PRIu64 ", outlier count=%" PRIu8,
                  tTimeUpdate, m_uOutlierCount);

        /* Populate the WLS output with the TsOffset filter state */
        m_dIntFiltMeas = dMeas;
        m_dFiltMeas = dMeas;
        m_uOutlierCount = 0;

        SLIM_LOGD("Filtered timeMs=%" PRIu64 ", FiltMeas=%lf",
                  tTimeUpdate, m_dFiltMeas);

        /* return early */
        return true;
      }
    }
    else
    {
      /* filter reset only if consecutive outliers */
       m_uOutlierCount = 0;
    }

    /* If the above test passes, then update the Kalman filter */
    if ( bUpdateFilter )
    {
      /* Add process noise/measurement noise to input uncertainty. */
      dFiltVarProp2 += getProceNoise(dDeltaTime);
      dExpectedVar2 += getMeasNoise(dDeltaTime);

      /* Compute Kalman gain */
      dKfGain = dFiltVarProp2 / dExpectedVar2;

      SLIM_LOGV("Filter. dDeltaTimeMs=%" PRId32 ", dFiltVarProp2=%" PRId32
                ", dKfGain(x1000)=%" PRId32,
                (int32_t)(dDeltaTime), (int32_t)(dFiltVarProp2),
                (int32_t)(1000*dKfGain));

      /* Update filter state */
      m_dIntFiltMeas += dKfGain * dResidual;

      /* Update state error covariance */
      m_dFiltMeasVar2 = ( 1.0 - dKfGain ) * dFiltVarProp2;

      /* Update filter time to the time of the raw data */
      m_tTimeUpdate = tTimeUpdate;

      m_dFiltMeas = m_dIntFiltMeas;
    }

    SLIM_LOGD("Filtered timeMs=%" PRIu64 ", FiltMeas=%lf",
              tTimeUpdate, m_dFiltMeas);

  }

  return true;
}

} // namespace slim

