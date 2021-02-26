/**
@file adsp_license_db.h
@brief This file declares the License Database APIs for the ADSP.******

The license database APIs allow access to a global store of license
information that can be read by other services to validate certain
modules. The get license API may be called from multiple threads at once.
*/

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/23/13    DG       Created file.
==========================================================================*/

/*------------------------------------------------------------------------
   Copyright (c) 2013, 2017 QUALCOMM Technologies, Incorporated.
   All Rights Reserved.
   QUALCOMM Technologies Proprietary.
--------------------------------------------------------------------------*/

#ifndef ADSP_LICENSE_DB_H
#define ADSP_LICENSE_DB_H

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/

#ifndef TST_LICENSE_DB
#include "qurt_elite_types.h"

/* ADSP error codes */
#include "adsp_error_codes.h"
#else
#include "tst_license_db.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

/**
  Adds a license to the database. If the license already exists with the given
  id it will be overwritten. If there is a validation function associated with
  this license then it will be executed.

  @param[in] id  Identifier used to specify the type of license.
  @param[in] license_ptr  Pointer to the buffer that holds the license data. This
                          buffer can be safely freed after the call returns.
  @param[in] size  Size of the license data in bytes.

  @return
  Success/failure error code. If a failure code is returned, the license was not stored.

  @dependencies
  None.
*/
ADSPResult adsp_license_db_add_license(uint32_t id, const void *license_ptr, uint32_t size);

/**
  Query for the size of license data for a particular id. If there is no validation function
  associated with this id, this will be the size of the actual license data that was set. If
  there is a validation function, this will be the size of the output of the validation function.
  The function adsp_license_db_is_validated() function can be used to determine which of
  these cases is exercised for the given id.

  @param[in] id  Identifier used to specify the type of license.
  @param[out] size_ptr  The size of the license data is stored in this variable.

  @return
  Success/failure error code. If a failure code is returned, the value at size_ptr is undefined.

  @dependencies
  None.
*/
ADSPResult adsp_license_db_get_size(uint32_t id, uint32_t *size_ptr);

/**
  Query for the license data for a particular id. If there is no validation function
  associated with this id, this will be the actual license data that was set. If
  there is a validation function, this will be the output of the validation function.
  The function adsp_license_db_is_validated() function can be used to determine which of
  these cases is exercised for the given id.

  @param[in] id  Identifier used to specify the type of license.
  @param[out] buf_ptr  Buffer where the license data will be written. This memory must be
                       allocated by the caller.
  @param[in] buf_size  Size of the buffer memory in bytes.
  @param[out] license_size_ptr  The size of the license data that was written in the buffer.

  @return
  Success/failure error code. If a failure code is returned, the data at buf_ptr and
  license_size_ptr is undefined.

  @dependencies
  None.
*/
ADSPResult adsp_license_db_get_license(uint32_t id, void *buf_ptr, uint32_t buf_size, uint32_t *license_size_ptr);

/**
  Query for whether the license at a particular id is validated using a validation function
  or not.

  @param[in] id  Identifier used to specify the type of license.
  @param[out] is_validated_ptr  Set to TRUE if the license is validated and FALSE otherwise.

  @return
  Success/failure error code. If a failure code is returned, the value at is_validated_ptr is undefined.

  @dependencies
  None.
*/
ADSPResult adsp_license_db_is_validated(uint32_t id, bool_t *is_validated_ptr);

/**
   Data type for the license validation function. This function will be called when the license is
   set for the given id. If any license data was set previously, the pointer to this data will be
   sent to the function. The function can decide to reuse the memory or free it. Even if a failure
   is returned by the function, the values returned in license_info_size and license_info_ptr must
   be valid. If there was a memory allocation failure, the function can set *license_info_size = 0
   and *license_info_ptr = NULL.

  @param[in] license_ptr Pointer to the block of memory that holds the license data that was set.
  @param[in] license_size Size of the license data that was set.
  @param[out] license_info_ptr Pointer to the block of memory that holds the result of the license validation.
                               The format of the result can be arbitrary.
  @param[out] license_info_size Size of the data that is stored in license_info_ptr.
  @param[in] prev_license_info_ptr Pointer to the previously set license info, if any. If no license
                                   information was set previously, this pointer will be NULL.

  @return
  Success/failure error code. If a failure code is returned, the caller will still store the pointer
               and size retuned by the function. These values must be set to valid values.

  @dependencies
  None.
 */
typedef ADSPResult (*adsp_license_validation_fn)(const void  *license_ptr,
		uint32_t license_size,
		const void **license_info_ptr,
		uint32_t *license_info_size,
		const void *prev_license_info_ptr);

/**
  Function to set the validation function for a license with a given id. A function
  can only be set once for any id. This function pointer should always stay valid.
  Functions implemented in shared objects which may be unloaded are not supported.

  @param[in] id  Identifier used to specify the type of license.
  @param[in] fn  Pointer to the license validation function.

  @return
  Success/failure error code. If a failure code is returned, the validation function was not set.

  @dependencies
  None.
*/
ADSPResult adsp_license_db_add_validation_fn(uint32_t id, adsp_license_validation_fn fn);

/**
  Initializes the license database. This function must be called only once.

  @return
  Success/failure of initialization.

  @dependencies
  None.
*/
ADSPResult adsp_license_db_init(void);

/**
  Frees up all the resources held by the license database. No API except adsp_license_db_init()
  should be called after this function is called.

  @dependencies
  None.
*/
void adsp_license_db_deinit(void);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //ADSP_LICENSE_DB_H

