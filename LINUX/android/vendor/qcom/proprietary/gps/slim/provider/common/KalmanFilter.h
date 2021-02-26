/*
Copyright (c) 2011, 2015 Qualcomm Technologies, Inc.
All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
*/
/**
 * @file  os_kf.h
 * @brief 1-State KF w/outlier detection
 *
 * This header file is a refactored version of os_kf.h header file from
 * GSIFF daemon.
 *
 * @ingroup slim_ProviderCommon
 */

#ifndef __KALMAN_FILTER_H_INCLUDED__
#define __KALMAN_FILTER_H_INCLUDED__

#include <stdint.h>
#include <math.h>

namespace slim
{

/*!
 * @brief 1-State Kalman filter with outlier detection
 *
 * @ingroup slim_ProviderCommon
 */
class KalmanFilter
{
private:
  // --------------------------------------------------------------------------
  // Configuration parameters
  // --------------------------------------------------------------------------
  //! @brief Number of outliers before reset
  const uint8_t m_uOutlierLimit;
  //! @brief Expect/Meas variance scalar to determine outliers
  const uint8_t m_uVarThresh;
  //! @brief Minimum unc allowed to initialize filter
  const uint32_t m_qMinUncToInit;
  //! @brief Min Residual used to check for outliers
  const uint32_t m_qDivergeThreshold;
  //! @brief How often to update the filter
  const uint64_t m_tTimeUpdateInterval;

  // --------------------------------------------------------------------------
  // State variables
  // --------------------------------------------------------------------------
  /**
   * @brief Filter status
   *
   * Filter status:
   * - true:  Filter initialized
   * - false: Filter not initialized
   */
  bool m_bFilterInit;

  //! @brief System time of the last filter update.
  uint64_t m_tTimeUpdate;
  //! @brief Number of outliers encountered during operation.
  uint8_t  m_uOutlierCount;
  /*!
   * @brief Filtered Measurement
   *
   * This value is given when retrieved.
   *
   * Not used for calculations because this value is used even when filter is
   * not properly initialized.
   */
  double  m_dFiltMeas;
  /**
   * @brief Internal Filtered Measurement.
   *
   * Internal Filtered Measurement used for all calculations.
   */
  double  m_dIntFiltMeas;
  //! @brief Variance of filtered measurement.
  double  m_dFiltMeasVar2;

protected:
  /**
   * @brief Constructs filter object with given parameters
   *
   * @param[in] uOutlierLimit       Number of outliers before a reset
   * @param[in] uVarThresh          Expect/Meas variance threshold scalar
   * @param[in] qMinUncToInit       Min unc allowed to initialize filter
   * @param[in] qDivergeThreshold   Min Residual diff to check for outlier
   * @param[in] tTimeUpdateInterval How often to update the filter
   */
  KalmanFilter
  (
    uint8_t uOutlierLimit,
    uint8_t uVarThresh,
    uint32_t qMinUncToInit,
    uint32_t qDivergeThreshold,
    uint64_t tTimeUpdateInterval
  ) ;
  /**
   * @brief Function used to calculate process noise
   *
   * @param dDeltaTime
   * @return
   */
  virtual double getProceNoise
  (
    double dDeltaTime
  ) = 0;
  /**
   * @brief Function used to calculate measurement noise
   *
   * @param dDeltaTime
   * @return
   */
  virtual double getMeasNoise
  (
    double dDeltaTime
  ) = 0;
public:
  /**
   * @brief Destroys filter and releases it's resources.
   */
  virtual ~KalmanFilter();

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
  bool init
  (
    double dMeas,
    double dMeasUnc,
    uint64_t tTimeUpdate
  );

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
  bool update
  (
    double dMeas,
    double dMeasUnc,
    uint64_t tTimeUpdate
  );

  /**
   * @brief Checks if the filter is initialized.
   *
   * @retval true  Filter is initialized.
   * @retval false Filter is not initialized.
   */
  bool isInitialized() const;

  /**
   * @brief Checks if the filter can be updated with the given time.
   *
   * @param[in] tTimeUpdate  Time of this measurement in any time base as long
   *                         as it is consistent.
   *
   * @retval true  Filter can be updated.
   * @retval false Time is ignored.
   */
  bool needsUpdate
  (
    uint64_t tTimeUpdate
  ) const;

  /**
   * @brief Utility function to return the current system time in microseconds.
   *
   * @param[out] dFiltMeasOut Output param for the current filtered measurement
   *
   * @retval true  On success.
   * @retval false On failure.
   */
  bool getFiltMeas
  (
    double &dFiltMeasOut
  ) const;

  /**
   * @brief Utility function to return the current system time in microseconds.
   *
   * @param[out] dFiltMeasUncOut Output param for the current filtered
   *                             measurement uncertainty
   *
   * @retval true  On success.
   * @retval false On failure.
   */
  bool getFiltMeasUnc
  (
    double &dFiltMeasUncOut
  ) const;
};

/**
 * @brief Checks if the filter is initialized.
 *
 * @retval true  Filter is initialized.
 * @retval false Filter is not initialized.
 */
inline bool KalmanFilter::isInitialized() const
{
  return m_bFilterInit;
}

/**
 * @brief Checks if the filter can be updated with the given time.
 *
 * @param[in] tTimeUpdate  Time of this measurement in any time base as long
 *                         as it is consistent.
 * @return
 */
inline bool KalmanFilter::needsUpdate
(
  uint64_t tTimeUpdate
) const
{
  return m_tTimeUpdate + m_tTimeUpdateInterval <= tTimeUpdate;
}

/**
 * @brief Utility function to return the current system time in microseconds.
 *
 * @param[out] dFiltMeasOut Output param for the current filtered measurement
 *
 * @retval true  On success.
 * @retval false On failure.
 */
inline bool KalmanFilter::getFiltMeas
(
  double &dFiltMeasOut
) const
{
  dFiltMeasOut = m_dFiltMeas;

  return true;
}

/**
 * @brief Utility function to return the current system time in microseconds.
 *
 * @param[out] dFiltMeasUncOut Output param for the current filtered
 *                             measurement uncertainty
 *
 * @retval true  On success.
 * @retval false On failure.
 */
inline bool KalmanFilter::getFiltMeasUnc
(
  double &dFiltMeasUncOut
) const
{
  dFiltMeasUncOut = sqrt(m_dFiltMeasVar2);

  return true;
}

}

#endif /* __KALMAN_FILTER_H_INCLUDED__ */
