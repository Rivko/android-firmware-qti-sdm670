/* Copyright (c) 2017 Qualcomm Technologies, Inc.
All rights reserved.

Confidential and Proprietary - Qualcomm Technologies, Inc.

*/

/**
 * @file qc_geofence.h
 *
 * @details Functions for geofence location driver
 *
 */

#ifndef _QC_CHRE_GEOFENCE_H_
#define _QC_CHRE_GEOFENCE_H_

#include <stdint.h>
#include <stddef.h>
#include "gnss.h"

#ifdef __cplusplus
extern "C" {
    namespace qcgf {
#endif

#define CHRE_EVENT_GNSS_GF_RESPONSE     CHRE_GNSS_EVENT_ID(3)
#define CHRE_EVENT_GNSS_GF_BREACH       CHRE_GNSS_EVENT_ID(4)

 /*
 * GNSS geofence error codes.
 */
typedef enum {
    QC_GEOFENCE_ERROR_SUCCESS = 0,        /**< Success. */
    QC_GEOFENCE_ERROR_GENERAL_FAILURE,    /**< General failure. */
    QC_GEOFENCE_ERROR_CALLBACK_MISSING,   /**< Callback is missing. */
    QC_GEOFENCE_ERROR_INVALID_PARAMETER,  /**< Invalid parameter. */
    QC_GEOFENCE_ERROR_ID_EXISTS,          /**< ID already exists. */
    QC_GEOFENCE_ERROR_ID_UNKNOWN,         /**< ID is unknown. */
    QC_GEOFENCE_ERROR_ALREADY_STARTED,    /**< Already started. */
    QC_GEOFENCE_ERROR_NOT_INITIALIZED,    /**< Not initialized. */
    QC_GEOFENCE_ERROR_GEOFENCES_AT_MAX    /**< Maximum number of geofences reached. */
} qcGeofenceErrorType;

/**
* Flags to indicate which values are valid in a location.
*/
typedef enum {
    QC_GEOFENCE_LOCATION_HAS_LAT_LONG_BIT = (1 << 0),  /**< Location has a valid latitude and longitude. */
    QC_GEOFENCE_LOCATION_HAS_ALTITUDE_BIT = (1 << 1),  /**< Location has a valid altitude. */
    QC_GEOFENCE_LOCATION_HAS_SPEED_BIT =    (1 << 2),  /**< Location has a valid speed. */
    QC_GEOFENCE_LOCATION_HAS_BEARING_BIT =  (1 << 3),  /**< Location has a valid bearing. */
    QC_GEOFENCE_LOCATION_HAS_ACCURACY_BIT = (1 << 4),  /**< Location has valid accuracy. */
} qcGeofenceLocationFlagsType;

/**
 * Flags to indicate Geofence breach status.
 */
typedef enum {
    QC_GEOFENCE_BREACH_ENTER = 0, /**< Entering Geofence breach. */
    QC_GEOFENCE_BREACH_EXIT,      /**< Exiting Geofence breach. */
    QC_GEOFENCE_BREACH_DWELL_IN,  /**< Dwelling in a breached Geofence. */
    QC_GEOFENCE_BREACH_DWELL_OUT, /**< Dwelling outside of a breached Geofence. */
    QC_GEOFENCE_BREACH_UNKNOWN,   /**< Breach is unknown. */
} qcGeofenceBreachType;

/** Flags to indicate Geofence breach mask bit. */
typedef enum {
    QC_GEOFENCE_BREACH_ENTER_BIT       = (1 << QC_GEOFENCE_BREACH_ENTER),      /**< Breach enter bit. */
    QC_GEOFENCE_BREACH_EXIT_BIT        = (1 << QC_GEOFENCE_BREACH_EXIT),       /**< Breach exit bit. */
    QC_GEOFENCE_BREACH_DWELL_IN_BIT    = (1 << QC_GEOFENCE_BREACH_DWELL_IN),   /**< Breach dwell in bit. */
    QC_GEOFENCE_BREACH_DWELL_OUT_BIT   = (1 << QC_GEOFENCE_BREACH_DWELL_OUT),  /**< Breach dwell out bit. */
} qcGeofenceBreachMaskType;

/** Structure for Geofence options. */
typedef struct {
    size_t size;                /**< Size. Set to the size of GeofenceOptions. */
    uint8_t breach_type_mask;   /**< Bitwise OR of qcGeofenceBreachMaskType bits. */
    uint32_t responsiveness;    /**< Responsiveness in milliseconds. */
    uint32_t dwell_time;        /**< Dwell time in seconds. */
} qcGeofenceOptionsType;

/** Structure for Geofence data. */
typedef struct {
    size_t size;      /**< Size. Set to the size of GeofenceData. */
    double latitude;  /**< Latitude in degrees. */
    double longitude; /**< Longitude in degrees. */
    double radius;    /**< Radius in meters. */
} qcGeofenceDataType;

/** Structure for location information. */
typedef struct {
    size_t size;        /**< Size. Set to the size of Location. */
    uint64_t timestamp; /**< UTC timestamp for a location fix; milliseconds since Jan. 1, 1970. */
    double latitude;    /**< Latitude in degrees. */
    double longitude;   /**< Longitude in degrees. */
    float altitude;     /**< Altitude in meters above the WGS 84 reference ellipsoid. */
    float speed;        /**< Speed in meters per second. */
    float bearing;      /**< Bearing in degrees; range: 0 to 360. */
    float accuracy;     /**< Accuracy in meters. */
    uint8_t flags;      /**< Bitwise OR of qcGeofenceLocationFlagsType. */
} qcGeofenceLocationType;

/** Structure for response event information. 
    This is used to inform the nanoapp when one of the following 
    functions (except chreQcGeofenceInit and chreQcGeofenceDeinit was
    completed
*/
typedef struct {
    size_t count;                   /**< Number of err/ids in arrays. */
    uint8_t const* err;             /**< Array of #qcGeofenceErrorType associated with the request. */
    uint32_t const* ids;            /**< Array of IDs to be associated with the request. */
} qcGeofenceResponseEventType;

/** Structure for breach event information. 
    This is used to inform the nanoapp when a geofence breach has occurred
*/

typedef struct {
    size_t count;                       /**< Number of IDs in the array. */
    uint32_t const* ids;                /**< Array of IDs that have been breached. */
    qcGeofenceLocationType location;    /**< Location associated with a breach. */
    uint8_t breach_type;                /**< Type of breach (see qcGeofenceBreachType). */
    uint64_t timestamp;                 /**< Timestamp of the breach. */
} qcGeofenceBreachEventType;

/**
* @brief Initializes a geofence session.

  @return
  QC_GEOFENCE_ERROR_SUCCESS if successful. \n
  QC_GEOFENCE_ERROR_GENERAL_FAILURE if there is an internal error. \n
  QC_GEOFENCE_ERROR_ALREADY_STARTED if a geofence session has
            already been initialized.
*/
uint8_t chreQcGeofenceInit(void);

/**
* @brief Deinitializes a geofence session.

  @return
  QC_GEOFENCE_ERROR_SUCCESS if successful. \n
  QC_GEOFENCE_ERROR_NOT_INITIALIZED if no geofence session has
            been initialized.
*/
uint8_t chreQcGeofenceDeinit(void);

/**
* @brief Adds a specified number of Geofences and returns an 
         array of Geofence IDs that will be used by the other Geofence APIs, 
         as well as in the Geofence response callback to match the command with 
         a response. The Geofence breach callback delivers the status of each
         Geofence according to the Geofence options for each.

  @param[in] count     Number of Geofences to be added.
  @param[in] pOptions  Array of structures containing the options:
                       - breach_type_mask -- Bitwise OR of GeofenceBreachTypeMask bits
                       - responsiveness in milliseconds
                       - dwell_time in seconds
  @param[in] pData     Array of structures containing the data:
                       - latitude of the center of the Geofence in degrees 
                       - longitude of the center of the Geofence in degrees
                       - radius of the Geofence in meters
  @param[in] pIdArray  Array of IDs of Geofences to be returned.

  @return
  QC_GEOFENCE_ERROR_SUCCESS if successful. \n
  QC_GEOFENCE_ERROR_NOT_INITIALIZED if no geofence session has
            been initialized.
*/
uint8_t chreQcGeofenceAdd(size_t count,
                    qcGeofenceOptionsType const* pOptions,
                    qcGeofenceDataType const* pData,
                    uint32_t** pIdArray);

/**
* @brief Removes a specified number of Geofences.

  @param[in] count  Number of Geofences to be removed.
  @param[in] pIDs   Array of IDs of the Geofences to be removed.

  @return
  QC_GEOFENCE_ERROR_SUCCESS if successful. \n
  QC_GEOFENCE_ERROR_NOT_INITIALIZED if no geofence session has
            been initialized.
*/
uint8_t chreQcGeofenceRemove(size_t count, uint32_t const* pIDs);

/**
* @brief Modifies a specified number of Geofences.

  @param[in] count     Number of Geofences to be modified.
  @param[in] pIDs      Array of IDs of the Geofences to be modified.
  @param[in] pOptions  Array of structures containing the options:
                       - breach_type_mask -- Bitwise OR of qcGeofenceBreachMaskType bits
                       - responsiveness in milliseconds
                       - dwell_time in seconds

  @return
  QC_GEOFENCE_ERROR_SUCCESS if successful. \n
  QC_GEOFENCE_ERROR_NOT_INITIALIZED if no geofence session has
            been initialized.
*/
uint8_t chreQcGeofenceModify(size_t count, uint32_t const* pIDs,
                            qcGeofenceOptionsType const* options);

/**
* @brief Pauses a specified number of Geofences, which is 
         similar to Geofence_Remove() except that they can be resumed 
         at any time.

  @param[in] count  Number of Geofences to be paused.
  @param[in] pIDs   Array of IDs of the Geofences to be paused.

  @return
  QC_GEOFENCE_ERROR_SUCCESS if successful. \n
  QC_GEOFENCE_ERROR_NOT_INITIALIZED if no geofence session has
            been initialized.
*/
uint8_t chreQcGeofencePause(size_t count, uint32_t const* pIDs);

/**
* @brief Resumes a specified number of Geofences that are 
         paused.

  @param[in] count  Number of Geofences to be resumed.
  @param[in] pIDs   Array of IDs of the Geofences to be resumed.

  @return
  QC_GEOFENCE_ERROR_SUCCESS if successful. \n
  QC_GEOFENCE_ERROR_NOT_INITIALIZED if no geofence session has
            been initialized.
*/
uint8_t chreQcGeofenceResume(size_t count, uint32_t const* pIDs);

#ifdef __cplusplus
}
}
#endif

#endif /* _QC_CHRE_GEOFENCE_H_ */
