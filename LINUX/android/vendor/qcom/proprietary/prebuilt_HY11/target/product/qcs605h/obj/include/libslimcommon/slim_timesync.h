#ifndef SLIM_TIMESYNC_H
#define SLIM_TIMESYNC_H
/**
  @file
  @brief SLIM time synchronization filter API.

  SLIM time synchronization filter implementation.
  @ingroup slim_CoreTimeSync
*/

/*============================================================================
               Copyright (c) 2013-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/gnss8.mpss/8.5/gnss/slim/common/core/inc/slim_timesync.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "slim_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @addtogroup slim_CoreTimeSync
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
@brief Time synchronization operation result code

@sa slim_TimeSyncRttUpdate
*/
typedef enum
{
  /*! Filter state is invalid */
  SLIM_TIME_SYNC_UPDATE_STATUS_INVALID,

  /*! Filter update was successful */
  SLIM_TIME_SYNC_UPDATE_STATUS_SUCCESS,

  /*! Filter update was an outlier */
  SLIM_TIME_SYNC_UPDATE_STATUS_OUTLIER,

  /*! Filter was reset */
  SLIM_TIME_SYNC_UPDATE_STATUS_RESET
} slim_TimeSyncUpdateStatusEnumType;

/*!
@brief Struct for SLIM time sync filter configuration
*/
typedef struct
{
  DBL d_ProcessNoiseDensity[2];
  /**< @brief Process noise filter configuration */

  /* Outlier detection thresholds */
  DBL d_OutlierThreshold;
  /**< @brief Outlier threshold in milliseconds */
  DBL d_OutlierSigmaThreshold;
  /**< @brief Outlier threshold sigma */
  uint32 q_MaxOutlierCount;
  /**< @brief Maximum number of outlier states before filter reset */
} slim_TimeSyncFilterConfigStructType;

/*! Struct for SLIM time sync filter */
typedef struct
{
  /** @brief Timetick of last filter update */
  uint64 t_LastUpdatedTimeTick;
  /** @brief Outlier counter.

      This counter is incremented every time when a negative remote time jump
      is detected. When the value exceeds the limit, the filter state will be
      reset.
   */
  uint32 q_OutlierCount;

  /** @brief Time sync validity flag.

      TRUE if time sync is valid, FALSE if not
  */
  boolean b_Valid;

  /**
     @brief Filter state and covariance matrix.

     Filter state and covariance matrix:
     - [0] -- Remote to local offset
     - [1] -- Remote to local offset drift
   */
  DBL d_KfState[2];

  /** @brief Upper diagonal of the KF state covariance matrix. */
  DBL d_KfStateCov[3];

  /** @brief Filter state as integer offset */
  int64 r_FilteredRemoteToLocalOffset;

  /** @brief Filter state as integer offset uncertainty */
  int64 r_FilteredRemoteToLocalOffsetUnc;

  /** @brief Filter configuration */
  slim_TimeSyncFilterConfigStructType z_Config;
} slim_TimeSyncFilterStateStructType;

/*! Struct for monotonous offset for service time sync */
typedef struct
{
  boolean b_Valid;
  /**< Flag if the offset value is valid */
  int64 r_MonotonousRemoteToLocalOffset;
  /**< Time offset value in milliseconds */
} slim_TimeSyncMtOffsetStructType;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
/** Default filter configuration */
extern const slim_TimeSyncFilterConfigStructType
  slimz_DefaultTimeSyncFilterConfig;

/**
@brief Initialize time sync filter state.

This function initializes time sync filter state using given filter
configuration. The configuration is copied to the filter state.

@param[out] pz_FilterOut Pointer to time sync filter state
@param[in]  pz_Config    Pointer to time sync filter config

@return None
*/
void slim_TimeSyncInit
(
  slim_TimeSyncFilterStateStructType *pz_FilterOut,
  const slim_TimeSyncFilterConfigStructType *pz_Config
);

/**
@brief Update time sync filter state with RTT measurement.

This function computes the offset between remote and local times from RTT
(round-trip-time) measurements, and updates the time sync filter to improve
offset estimate. If the filter has detected too many outliers, it will reset and
return TRUE to inform the user about a time jump.

@param[in,out] pz_Filter      Pointer to time sync filter state
@param[in] t_LocalTxTimeMsec  Local transmit time in milliseconds
@param[in] t_RemoteRxTimeMsec Remote receive time in milliseconds
@param[in] t_RemoteTxTimeMsec Remote transmit time in milliseconds
@param[in] t_LocalRxTimeMsec  Local receive time in milliseconds

@retval SLIM_TIME_SYNC_UPDATE_STATUS_INVALID: when filter state
        was, or has become invalid

@retval SLIM_TIME_SYNC_UPDATE_STATUS_SUCCESS: when measurement was
        successfully applied to filter state

@retval SLIM_TIME_SYNC_UPDATE_STATUS_OUTLIER: when measurement was
        discarded due to being an outlier

@retval SLIM_TIME_SYNC_UPDATE_STATUS_RESET: when filter has been
        reset due to consecutive outliers
*/
slim_TimeSyncUpdateStatusEnumType slim_TimeSyncRttUpdate
(
  slim_TimeSyncFilterStateStructType *pz_Filter,
  uint64 t_LocalTxTimeMsec,
  uint64 t_RemoteRxTimeMsec,
  uint64 t_RemoteTxTimeMsec,
  uint64 t_LocalRxTimeMsec
);

/**
@brief Propagate time sync filter to current time.

This function propagates the time sync filter to current
time. Essentially this is a KF propagation step.

@param[in,out] pz_Filter       Pointer to time sync filter state
@param[in]     t_CurrentTimeMs Current time (msec)

@return None
*/
void slim_TimeSyncTimeUpdate
(
  slim_TimeSyncFilterStateStructType *pz_Filter,
  uint64 t_CurrentTimeMs
);

/**
@brief Helper function for determining if the filter should be time updated.

This function determines if the remote-to-local offset could have
drifted more than half a millisecond since last filter update. To do
this, it estimates the amount of drift based on the offset drift
estimate, and compares it against the difference between current
time and last filter update time.

@param[in] pz_Filter       Pointer to time sync filter state
@param[in] t_CurrentTimeMs Current time (msec)

@return TRUE if offset might have drifted enough that update is needed
        FALSE otherwise
*/
boolean slim_TimeSyncShouldTimeUpdate
(
  slim_TimeSyncFilterStateStructType *pz_Filter,
  uint64 t_CurrentTimeMs
);

/**
@brief Is time sync valid.

This function returns TRUE if time sync is valid, and FALSE otherwise. If time
sync is valid, then it can be used to convert timestamps from remote time to
local time. One might also want to check the time sync uncertainty.

@param[in] pz_Filter Pointer to time sync filter state

@retval TRUE if time sync is valid
@retval FALSE otherwise
*/
boolean slim_TimeSyncIsValid
(
  const slim_TimeSyncFilterStateStructType *pz_Filter
);

/**
@brief Time sync uncertainty.

This function returns the uncertainty estimate of remote to local timestamp
conversion.

@param[in] pz_Filter Pointer to time sync filter state

@return Uncertainty of remote to local time conversion
*/
uint64 slim_TimeSyncUnc(const slim_TimeSyncFilterStateStructType *pz_Filter);

/**
@brief Convert remote time to local time.

This function converts remote time to local time using time sync filter.

@param[in]  pz_Filter         Pointer to time sync filter state
@param[in]  t_RemoteTimeMs    Remote timestamp in milliseconds
@param[out] pt_LocalTimeMsOut Pointer to 64bit local time in milliseconds

@return TRUE if conversion was successful (time sync was valid)
        FALSE if conversion failed (time sync was invalid)
*/
boolean slim_TimeSyncRemoteToLocal
(
  const slim_TimeSyncFilterStateStructType *pz_Filter,
  uint64 t_RemoteTimeMs,
  uint64 *pt_LocalTimeMsOut
);

/**
@brief Initializes monotonous offset.

This function initializes monotonous offset with the given time sync filter.

@param[in]  pz_Filter   Pointer to time sync filter state
@param[out] pz_MtOffset Pointer to monotonous offset structure.
*/
void slim_TimeSyncInitMtOffset
(
  const slim_TimeSyncFilterStateStructType *pz_Filter,
  slim_TimeSyncMtOffsetStructType *pz_MtOffset
);

/**
@brief Introduces offset to monotonous offset.

This function is used to introduce offset to monotonous offset, in order to get
it closer to KF estimated offset. This function should be called every now and
then to keep monotonous offset in sync with KF offset.

Monotonous offset guarantees that when converting monotonous timestamps from
remote time to local time, the timestamps are monotonous also in local time.
When a gap in timestamps (for example between sensor samples) is known, this
function can be used to synchronize monotonous offset, but only by
q_MaxOffsetToIntroduceMsec at a time. For example, if gap between sensor samples
is 10ms, then by calling this between the samples with
q_MaxOffsetToIntroduceMsec = 9, we change the offset by maximum of 9ms, and the
relative ordering of sensor samples is maintained.

@param[in]     pz_Filter   Pointer to time sync filter state
@param[in,out] pz_MtOffset Pointer to monotonous offset structure.
@param[in]     q_MaxOffsetToIntroduceMsec Maximum number of milliseconds
                                          that the offset can change
*/
void slim_TimeSyncIntroduceMtOffset
(
  const slim_TimeSyncFilterStateStructType *pz_Filter,
  slim_TimeSyncMtOffsetStructType *pz_MtOffset,
  uint32 q_MaxOffsetToIntroduceMsec
);

/**
@brief Is monotonous offset valid.

This function returns TRUE if monotonous offset is valid, and FALSE otherwise.

If monotonous offset is valid, then it can be used to convert timestamps from
remote time to local time. One might also want to check the monotonous offset
uncertainty.

@param[in] pz_MtOffset Pointer to monotonous offset structure.

@retval TRUE if monotonous offset is valid
@retval FALSE otherwise
*/
boolean slim_TimeSyncMtOffsetIsValid
(
  const slim_TimeSyncMtOffsetStructType *pz_MtOffset
);

/**
@brief Convert remote time to local time using monotonous offset.

This function converts remote time to local time using time sync filter
monotonous offset. Note that the offset itself is not monotonous, but it will
change only when #slim_TimeSyncIntroduceMtOffset() is called, and only by
specified amount, which enables conversion of monotonous timestamps from
remote time to local time such that also converted timestamps are monotonous.

@param[in] pz_Filter          Pointer to time sync filter MtOffset was derived
                              from.
@param[in]  pz_MtOffset       Pointer to monotonous offset structure.
@param[in]  t_RemoteTimeMs    Remote timestamp in milliseconds.
@param[out] pt_LocalTimeMsOut Pointer to 64bit local time in milliseconds

@retval TRUE if conversion was successful (time sync was valid)
@retval FALSE if conversion failed (time sync was invalid)

@sa slim_TimeSyncRemoteToLocal
*/
boolean slim_TimeSyncMtRemoteToLocal
(
  const slim_TimeSyncFilterStateStructType *pz_Filter,
  const slim_TimeSyncMtOffsetStructType *pz_MtOffset,
  uint64 q_RemoteTimeMs,
  uint64 *pt_LocalTimeMsOut
);

/**
@brief Time sync monotonous offset uncertainty.

This function returns the uncertainty estimate of remote to local timestamp
conversion using monotonous offset.

@param[in] pz_Filter   Pointer to time sync filter state
@param[in] pz_MtOffset Pointer to monotonous offset structure.

@return Uncertainty of remote to local time conversion using monotonous offset
*/
uint64 slim_TimeSyncMtUnc
(
  const slim_TimeSyncFilterStateStructType *pz_Filter,
  const slim_TimeSyncMtOffsetStructType *pz_MtOffset
);

//! @}

#ifdef __cplusplus
}
#endif

#endif
