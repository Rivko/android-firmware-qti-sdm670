#ifndef QSEE_OEM_BUFFER_H
#define QSEE_OEM_BUFFER_H

/**
   @file qsee_oem_buffer.h
   @brief Provide APIs to mask and unmap various interrupt sources
*/

/*===========================================================================
   Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tz/1.0.2/api/securemsm/trustzone/qsee/qsee_oem_buffer.h#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
09/29/15   pre      Initial version.

===========================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/
#include <stdlib.h>

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

#define QSEE_OEM_BUFFER_SUCCESS                          0
#define QSEE_OEM_BUFFER_ERROR                           -1
#define QSEE_OEM_BUFFER_ERROR_OFFSET_OUT_OF_BOUNDS      -2
#define QSEE_OEM_BUFFER_ERROR_SOURCE_BUFFER_TOO_LARGE   -3
#define QSEE_OEM_BUFFER_ERROR_INVALID_BUFFER_AND_OFFSET -4

/**
 * Retrieves the length of the OEM buffer supported by QSEE.
 *
 * param[out] len Length of the internal OEM buffer supported by QSEE.
 *
 * @return size of OEM buffer, 0 for failure
 */
size_t qsee_get_oem_buffer_length(void);

/**
 * Copies from OEM buffer at the given offset into the output buffer
 * up to the size of the output buffer.  If the output buffer is
 * larger than the OEM buffer at the provided offset, the remaining
 * bytes of the output buffer remain unchanged.
 *
 * param[in]  offset   The offset to begin reading from the OEM
 *                     buffer into.
 * param[out] data     Destination for copying the source data into.
 * param[in]  data_len Amount of data to read from OEM buffer.
 *
 * @return QSEE_OEM_BUFFER_SUCCESS on success
 * @return QSEE_OEM_BUFFER_ERROR on error in mink IPC
 * @return QSEE_OEM_BUFFER_ERROR_INVALID_BUFFER_AND_OFFSET if the buffer
 * length and offset parameters cause an integer overflow
 */
int qsee_read_oem_buffer(size_t offset, void* data, size_t data_len);

/**
 * Copies into the OEM buffer at the given offset up to the size of
 * the parameter buffer.  If the parameter buffer length and offset
 * combine to a greater length than the OEM buffer length, an error is
 * returned and no data is copied.
 *
 * param[in] offset   The offset to begin writing into the
 *                    destination.
 * param[in] data     Source of data to copy to OEM buffer.
 * param[in] data_len Length of data to copy from source.
 *
 * @return QSEE_OEM_BUFFER_SUCCESS on success
 * @return QSEE_OEM_BUFFER_ERROR on error in mink IPC
 * @return QSEE_OEM_BUFFER_ERROR_OFFSET_OUT_OF_BOUNDS if the offset
 * parameter falls outside of the size of theOEM buffer or if this target
 * does not support OEM buffer.
 * @return QSEE_OEM_BUFFER_ERROR_SOURCE_BUFFER_TOO_LARGE if the provided
 * buffer cannot fit entirely in the OEM buffer when written at the given
 * offset.
 * @return QSEE_OEM_BUFFER_ERROR_INVALID_BUFFER_AND_OFFSET if the buffer
 * length and offset parameters cause an integer overflow
 */
int qsee_write_oem_buffer(size_t offset, const void* data, size_t data_len);

#endif /* QSEE_OEM_BUFFER_H */
