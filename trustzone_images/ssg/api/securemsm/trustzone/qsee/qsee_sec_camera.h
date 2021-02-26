#ifndef QSEE_SECCAM_H
#define QSEE_SECCAM_H

/**
@file qsee_sec_camera.h
@brief Provide Secure Display functionality
*/

/*===========================================================================
   Copyright (c) 2016, 2017 by QUALCOMM, Technology Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tz/1.0.2/api/securemsm/trustzone/qsee/qsee_sec_camera.h#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
07/16/17   dr       Add mink callback object registration
03/20/17   dr       Add Haven license support
10/30/16   dr       Add 8998 support and ISPIF/CSID regions
05/12/16   ak       Add streaming status control functions
04/25/16   ak       Add CCI address space access functions
04/18/16   sn       Initial version
===========================================================================*/

#include <stdint.h>
#include <stddef.h>
#include "object.h"

typedef enum {
  QSEE_SEC_CAM_REG_MMSS_A_CCI    = 0,
  QSEE_SEC_CAM_REG_MMSS_A_CSID_0 = 1,
  QSEE_SEC_CAM_REG_MMSS_A_CSID_1 = 2,
  QSEE_SEC_CAM_REG_MMSS_A_CSID_2 = 3,
  QSEE_SEC_CAM_REG_MMSS_A_CSID_3 = 4,
  QSEE_SEC_CAM_REG_MMSS_A_ISPIF   = 5,
} qsee_sec_cam_register_region_t;


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * Get the current secure camera session ID
 *
 * @param [out] sessionID   the current secure camera session ID
 *
 * @return \c 0 on success; -1 on failure. \c
 */
int qsee_sec_camera_get_session(uint32_t *sessionID);

/**
 * Mark secure streaming session start. By calling this, TA prevents
 * secure camera switching to non-secure mode.
 *
 * @param [in] sessionID    the current secure camera session ID
 *
 * @return \c 0 on success; -1 on failure. \c
 */
int qsee_sec_camera_acquire_camera(uint32_t sessionID);

/**
 * Mark secure streaming session end. By calling this, TA allows the
 * secure camera to be switched to non-secure mode.
 *
 * @param [in] sessionID    the current secure camera session ID
 *
 * @return \c 0 on success; -1 on failure. \c
 */
int qsee_sec_camera_release_camera(uint32_t sessionID);

/**
 * Write 32-bit data to the register to the specified address space.
 *
 * @param [in] registerRegionId   address region ID
 * @param [in] offset             register offset in the region
 * @param [in] data               data to write
 *
 * @return \c 0 on success; -1 on failure. \c
 */
int qsee_sec_camera_register_write(qsee_sec_cam_register_region_t registerRegionId,
                                   uint32_t offset, uint32_t data);

/**
 * Bulk write 32-bit data to a number of registers of the specified address space.
 *
 * @param [in] registerRegionId   address region ID
 * @param [in] offset             array of register offsets in the region
 * @param [in] offset_len         number of items in the offset array
 * @param [in] data               array of data values to write
 * @param [in] data_len           number of items in the data array
 *
 * @return \c 0 on success; -1 on failure. \c
 */
int qsee_sec_camera_register_bulk_write(qsee_sec_cam_register_region_t registerRegionId,
                                        uint32_t offset[], size_t offset_len,
                                        uint32_t data[], size_t data_len);

/**
 * Read 32-bit data to the register to the specified address space.
 *
 * @param [in] registerRegionId   address region ID
 * @param [in] offset             register offset in the region
 * @param [out] data_ptr          returned data
 *
 * @return \c 0 on success; -1 on failure. \c
 */
int qsee_sec_camera_register_read(qsee_sec_cam_register_region_t registerRegionId,
                                  uint32_t offset, uint32_t *data_ptr);

/**
 * Set and validate Haven License Certificate for Secure Camera feature
 *
 * Need to be called at least once after the device was rebooted to enable the secure camera.
 * Invoking secure camera QSEE APIs without calling this method first, or if the provided
 * certificate is not valid will result in feature been disabled on production devices.
 * If the function returns with a failure havenError_ptr value should be checked.
 *
 * Note that on non-production device the function will always return with success to allow
 * full functionality for debug. In that case it is still recommended to check havenError_ptr
 * value to make sure that license validation will succeed on production device as well.
 *
 * @param [in]  licenseCert_ptr   pointer to buffer containing the certificate in DER format
 * @param [in]  licenseCert_len   certificate data length in bytes
 * @param [out] havenError_ptr    haven license validation result - 0 means success
 *                                (see IHavenTokenApp.h for more details on the  error values)
 *
 * @return \c 0 on success; -1 on failure. \c
 */
int qsee_sec_camera_set_haven_license(const uint8_t* licenseCert_ptr, size_t licenseCert_len,
                                      int32_t* havenError_ptr);

/**
 * Register TA's callback object of type ISecureCameraClientEvent to TZ secure camera module
 *
 * This functional should be called by TA to receive notification about secure mode change.
 * The object provided should be mink object implementing ISecureCameraClientEvent interface
 * which is defined in ISecureCameraClientEvent.h
 * The callback object provided will be called by TZ kernel camera module when camera protect
 * or unprotect event takes place.
 * For TA to receive this notification successfully the 'SecureCameraClientEvent' has to be
 * added to the list of supported TA services.

 * @param [in]  callback_object   Mink object of type ISecureCameraClientEvent
 *
 * @return \c 0 on success; -1 on failure. \c
 */
int qsee_sec_camera_register_callback(Object callback_object);

#endif /* QSEE_SECCAM_H */

