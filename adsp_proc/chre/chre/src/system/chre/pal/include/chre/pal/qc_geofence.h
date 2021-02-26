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

#ifndef _QC_GEOFENCE_H_
#define _QC_GEOFENCE_H_

#include <stdint.h>
#include <stddef.h>
#include "chre_api/chre/qc_geofence.h"

#ifdef __cplusplus
extern "C" {
    namespace qcgf {
#endif

/* GNSS Geofence Callbacks */

/**
* @brief Geofence response callback is used by Geofence APIs.
         It is called for every Geofence API call after the
         command has completed (successfully or unsuccessfully).

  @param[in] responseEvent  Structure with information about the response event

  @return None.
*/
typedef void(*qcGeofenceResponseCallback)(
    qcGeofenceResponseEventType responseEvent
);

/**
* @brief Geofence breach callback used for the Add_Geofences() API.
         This is an optional function and can be NULL.
         It is called when any number of geofences have a state change.

  @param[in] breachEvent    Structure with information about the breach event

  @return None
*/
typedef void(*qcGeofenceBreachCallback)(
    qcGeofenceBreachEventType breachEvent
);

/** Geofence callbacks. */
typedef struct {
    size_t size;
    qcGeofenceResponseCallback geofenceResponseCb;
    /**< Geofence response callback is mandatory. */
    qcGeofenceBreachCallback   geofenceBreachCb;
    /**< Geofence breach callback is optional. */
} qcGeofenceCallbacksType;

/**
* @brief Initializes a geofence session and registers the callbacks.

  @param[in] pCallbacks  Pointer to the Structure with the callback
                         functions to be registered.

  @return
  QC_GEOFENCE_ERROR_SUCCESS if successful. \n
  QC_GEOFENCE_ERROR_CALLBACK_MISSING if one of the mandatory
            callback functions is missing. \n
  QC_GEOFENCE_ERROR_GENERAL_FAILURE if there is an internal error. \n
  QC_GEOFENCE_ERROR_ALREADY_STARTED if a geofence session has
            already been initialized.
*/
uint8_t qcGeofenceInit(qcGeofenceCallbacksType* pCallbacks);

/**
* @brief Deinitializes a geofence session.

  @return
  QC_GEOFENCE_ERROR_SUCCESS if successful. \n
  QC_GEOFENCE_ERROR_NOT_INITIALIZED if no geofence session has
            been initialized.
*/
uint8_t qcGeofenceDeinit(void);

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
uint8_t qcGeofenceAdd(size_t count,
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
uint8_t qcGeofenceRemove(size_t count, uint32_t const* pIDs);

/**
* @brief Modifies a specified number of Geofences.

  @param[in] count     Number of Geofences to be modified.
  @param[in] pIDs      Array of IDs of the Geofences to be modified.
  @param[in] pOptions  Array of structures containing the options:
                       - breach_type_mask -- Bitwise OR of Geofence_Breach_Mask_t bits
                       - responsiveness in milliseconds
                       - dwell_time in seconds

  @return
  QC_GEOFENCE_ERROR_SUCCESS if successful. \n
  QC_GEOFENCE_ERROR_NOT_INITIALIZED if no geofence session has
            been initialized.
*/
uint8_t qcGeofenceModify(size_t count, uint32_t const* pIDs,
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
uint8_t qcGeofencePause(size_t count, uint32_t const* pIDs);

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
uint8_t qcGeofenceResume(size_t count, uint32_t const* pIDs);

#ifdef __cplusplus
}
}
#endif

#endif /* _QC_GEOFENCE_H_ */
