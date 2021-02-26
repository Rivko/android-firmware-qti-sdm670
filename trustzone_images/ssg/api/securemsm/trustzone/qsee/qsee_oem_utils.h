#ifndef SFS_OEM_UTILS_H
#define SFS_OEM_UTILS_H

/**
@file qsee_oem_utils.h
@brief Provides API's for OEM utility functions
 */

/*=============================================================================
              Copyright (c) 2012-2013 QUALCOMM Technologies Incorporated.
                      All Rights Reserved.
              Qualcomm Confidential and Proprietary

=============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/ssg.tz/1.0.2/api/securemsm/trustzone/qsee/qsee_oem_utils.h#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/18/12   yh      Initial version
=============================================================================*/

#include <stdint.h>
#include <stdbool.h>
#include "qsee_heap.h"


/**
* @brief Retrieves derivation key for sfs secret key. key_len value shall not less than 16,
* otherwise qsee will still use the HW based key as the sfs kdf derivation key.
* If key_len value exceeds 32, qsee will only take first 32 bytes as the sfs kdf derivation key.
*
* @details When not using the OEM key, sfs secret key will be different depending on whether the JTAG_DISABLE eFuse is blown.
* Before the fuse is blown, sfs kdf uses the dummy HW derived key as the kdf derivation key.
* After the fuse is blown, sfs kdf uses the HW derived key as kdf derivation key.
*
* If key is set by OEM, sfs kdf always uses OEM key as kdf derivation key to create sfs secret key,
* and JTAG_DISABLE eFuse won't affect the sfs secret key value.
*
*@return size_t length of the OEM SFS derivation key
*
 */
size_t qsee_oem_set_kdf_derive_key(void);

/**
* @brief Controls SFS file segment size
*
* @details SFS creates several sub-files of equal size (file segments, or simply segments for short)
*  which are then encrypted and integrity-protected before being written to the underlying file system.
*  This function let the OEM decide the segment size.
*  For example, OEM have 2000 bytes plain text to save to SFS. The default SFS segment size is 1024 byte,
*  so SFS creates 2 segments (1024 bytes, 976 bytes + 48 bytes padding) on HLOS file system.
*  If OEM change the segment size to 2048 bytes, SFS creates 1 segment (2000 bytes + 48 bytes padding) on
*  HLOS file system.
*
*  OEM should be very cautious to make the change of segment size.
*
*@return segment size
*@side effect: After the segment size change, The SFS files saved with old segment are useless.
* Must clean up the old SFS files on HLOS file system manually.
*/

uint32_t qsee_sfs_get_segment_size(void);

/**
* @brief Controls SFS sanity mode
*
* @details  when qsee_sfs_open is called, SFS checks each segment's sanity,
*  this process includes data segment authentication and decryption, and it is expensive.
*  OEM can skip this process to improve performance by set safe mode to FALSE.
*
*@return safe or fast mode
*@side effect: qsee_sfs_open will not return error if data segment is corrupted.
*  However afterward qsee_sfs_ API call (read/write) will fail.
*/
bool qsee_sfs_set_safe_mode(void);

/**
* @brief Set SFS and TZ counter file root path
*
* @details  By default SFS files are saved under /persist/data/sfs/ folder
*  TZ counter files are saved under /persist/data/tz/ folder
*  default root path is /persist/data/
*  This function let OEM to set root folder other than /persist/data/
*  For example, if OEM set root path to /OEM/subfolder/
*  SFS and TZ counter files will be generated under /OEM/subfolder/sfs/ and
*  /OEM/subfolder/tz/
*  The path must start with '/' and end with '/', path length < 128 bytes
*
*@return OEM defined root path string
*@side effect: The value is defined at compile time, and the path should not
* be changed for a production build.
*/
const char * qsee_sfs_set_root_path(void);


#endif /* SFS_OEM_UTILS_H */
