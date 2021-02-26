#ifndef SECBOOT_UTIL_H
#define SECBOOT_UTIL_H
/*****************************************************************************
*
* @file  secboot_util.h (Secboot wrapper API)
*
* @brief API to support extra/wrapper secboot logic
*       
* Copyright (c) 2010-2013 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*
*****************************************************************************/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when         who     what, where, why
--------   ---     ----------------------------------------------------------
11/06/13   hw      Init version

============================================================================*/
#include "sec_img_auth.h"
#include "secboot.h"
#include "secboot_hw.h"

typedef struct 
{
  uint32                         code_segment;
  uint64                         image_sw_id;
  uint32                         pbl_reenable_debug;
  secboot_ftbl_type*             secboot_ftbl_ptr;
  secboot_hw_ftbl_type*          secboot_hw_ftbl_ptr;
  secboot_crypto_hash_ftbl_type* crypto_hash_ftbl_ptr;
} secboot_auth_image_context;

/**
 * @brief This function authenticates the signed image for secure and non-secure device, but ignores 
 *        UNTRUSTED ROOT failure for non-secure device. It passes with unsigned image in non-secure 
 *        device, if FEATURE_ALLOW_UNSIGNED_IMAGE is defined. It support engg cert functionality.
 *
 * @param[in]        context                context that stores function pointers and related info.
 *                   code_segment           indicate which processor the code is running on
 *                   image_info             image information
 *
 * @param[in, out] verified_info            verified information
 *
 * @return E_AUTH_IMG_SUCCESS on success. Appropriate error code on failure.
 *
 * @sideeffects  None
 *
 */
sec_img_auth_error_type sec_img_auth_authenticate_image_context(
  secboot_auth_image_context*  context,
  secboot_image_info_type* image_info,
  secboot_verified_info_type* verified_info
);

/**
 * @brief This function authenticates the signed image for secure and non-secure device, but ignores UNTRUSTED ROOT
 *        failure for non-secure device. It passes for unsigned image in non-secure device, 
 *        if FEATURE_ALLOW_UNSIGNED_IMAGE is defined                 
 *
 * @param[in] code_segment             indicate which processor the code is running on
 *            image_info                 image information
 *            secboot_ftbl_ptr          secboot function table pointer
 *            secboot_hw_ftbl_ptr    secboot hw function table pointer
 *            crypto_hash_ftbl_ptr    secboot hashing function table pointer
 *
 * @param[in, out] code_segment             indicate which processor the code is running on
 *
 * @return E_SECBOOT_SUCCESS on success. Appropriate error code on failure.
 *
 * @sideeffects  None
 *
 */
secboot_error_type secboot_authenticate_image(
  uint32 code_segment,
  secboot_image_info_type* image_info,
  secboot_verified_info_type* verified_info,
  secboot_ftbl_type* secboot_ftbl_ptr,
  secboot_hw_ftbl_type* secboot_hw_ftbl_ptr,
  secboot_crypto_hash_ftbl_type* crypto_hash_ftbl_ptr
);

#endif //SECBOOT_UTIL_H
