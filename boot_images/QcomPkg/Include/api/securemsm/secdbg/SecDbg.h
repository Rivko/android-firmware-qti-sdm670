/**
@file SecDbg.h
@brief Seecure debug policy specific definitions/routines

This file contains the target specific information for debug policy Provisioning

* Copyright (c) 2014-2015 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
*/

/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2014/04/14     mm          Initial version
 2014/07/08     st          Initial version
=============================================================================*/

#ifndef SECBOOT_DEBUG_POLICY_H
#define SECBOOT_DEBUG_POLICY_H

#include "CeML.h"
#include "secboot.h"
#include "SecImgUtil.h"

typedef enum sec_dbg_error_type
{
  SEC_DBG_STATUS_SUCCESS = 0x0,
  SEC_DBG_STATUS_FAILURE = 0x101,
  SEC_DBG_STATUS_INVALID_ARG = 0x102,
  SEC_DBG_STATUS_OUT_OF_RANGE = 0x103,
  SEC_DBG_STATUS_HASH_IMAGE_ID_MISMATCH = 0x104,
  SEC_DBG_STATUS_INVALID_SIZE= 0x105,
  SEC_DBG_STATUS_NO_MEMORY = 0x106,
  SEC_DBG_STATUS_INVALID_REVISION_NUMBER = 0x107,
  SEC_DBG_STATUS_INVALID_COUNT = 0x108,
  SEC_DBG_STATUS_INVALID_RESERVED_BITS= 0x109,
  SEC_DBG_STATUS_HASH_ARRAY_COUNT_INVALID = 0x10A,
  SEC_DBG_STATUS_SERIAL_NUM_READ_ERROR= 0x10B,
  SEC_DBG_STATUS_INCORRECT_HASH_TOTAL_LEN = 0x10C,
  SEC_DBG_STATUS_CORRUPTED_IMAGE = 0x10D,
  SEC_DBG_STATUS_INVALID_MAGIC = 0x10E,
  SEC_DBG_STATUS_NO_DP_ROT  = 0x10F,
  SEC_DBG_STATUS_MAX                    = 0x7FFFFFFF /**< Force to 32 bits */
}sec_dbg_error_type;


/* The number of certificates that can be tried for testing/engineering */
#define SEC_DBG_CERT_ARRAY_SIZE                4 

/** The value of these flags must match the 'flags' element
  * of debug_policy_t
*/
#define SEC_DBG_ENABLE_ONLINE_CRASH_DUMPS       0
#define SEC_DBG_ENABLE_OFFLINE_CRASH_DUMPS      1
#define SEC_DBG_ENABLE_JTAG                     2 
#define SEC_DBG_ENABLE_LOGGING                  3 

/* Revision number of debug policy.  */
#define SEC_DBG_REVISION_NUMBER                 2 

/* Atleast one is required */ 
#define SEC_DBG_SERIAL_NUM_MIN_COUNT 1

/* unique serial numbers that are allowed */
#define SEC_DBG_SERIAL_NUM_MAX_COUNT 200

typedef struct __attribute__((__packed__))  
{
        uint32 magic; /* Version of Debug Policy */
        uint32 size;  /* Fixed size debug policy*/
        uint32 revision; /* Revision of the debug policy*/
        struct {
                uint64 enable_online_crash_dumps   :  1;
                uint64 enable_offline_crash_dumps  :  1;
                uint64 enable_jtag                 :  1;
                uint64 enable_logs                 :  1;
                uint64 reserved_bits               : 44; // reserved for QCT, must be 0
                uint64 oem_reserved_bits           : 16; // reserved for OEM
        } flags;
        uint32 image_id_bitmap; /* Image ID Bit Map */
        uint32 root_cert_hash_count; /* Root Cert Hash Count */
        uint8  root_cert_hash_array[SEC_DBG_CERT_ARRAY_SIZE][CEML_HASH_DIGEST_SIZE_SHA256]; /* Hash Array*/
        uint32 serial_num_count; /* Serial Number Count */
        uint32 serial_num_array[SEC_DBG_SERIAL_NUM_MAX_COUNT]; /* Serial Numbers */
} sec_dbg_t;

/**
 * @brief 
 *       Debug Policy Initialization 
 *
 */
sec_dbg_error_type sec_dbg_init(uint64 dp_addr);

/**
 * @brief 
 *        Check whether a feature flag is set in the debug policy.
 *
 * @param[in] flag The feature flag in question
 * @retval    TRUE if the flag is set, FALSE otherwise
 *
 */
boolean sec_dbg_is_flag_set ( uint32 flag );
/**
 * @brief 
 * Authenticates against the Root of Trusts in the debug policy if any
 */

sec_dbg_error_type sec_dbg_auth_image(
  secboot_handle_type*            sec_handle,
  secboot_auth_image_context*     context,
  secboot_image_info_type*        image_info,
  secboot_verified_info_type*     verified_info
);

/**
* @brief
 * Process the debug policy and apply any relevant settings
 */
void sec_dbg_apply_policy();
#endif