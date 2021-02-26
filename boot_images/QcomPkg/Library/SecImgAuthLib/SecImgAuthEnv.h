#ifndef SEC_IMG_AUTH_ENV_H
#define SEC_IMG_AUTH_ENV_H

/**
* @file sec_img_auth_env.h
* @brief Contains TZ/MBA specific macros,type declarations and function declarations
*        whose implementation is specifc to MBA/TZ.
*/

/*===========================================================================
   Copyright (c) 2012 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE


when       who      what, where, why
--------   ---      ------------------------------------
02/28/12   vg       Fixed review comments
02/20/12   vg       Initial version.

===========================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>
#include "secboot.h"
#include "secboot_hw.h"
#include <stdlib.h>
#include <string.h>
#include "sec_img_auth.h"
//#include "boot_util.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define MEMSET  memset
#define MEMCMP  memcmp
#define MEMCPY  memcpy

/*----------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/

/**
 * @brief This funnction returns code segment in SECURE_BOOTn register
 *        containing authentication information of the image.
 *
 * @param[in]      proc          Image Id.
 * @param[in,out]  code_segment  Code segment in SECURE_BOOTn register
 *                               containing authentication information
 *                               of the image.
 *
 * @return E_SUCCESS on success and E_INVALID_ARG for invalid args.
 */
sec_img_auth_error_type 
sec_img_auth_get_code_segment(uint32 img_id, 
  uint32 *code_segment);

/**
 * @brief This function returns sw version for Image Id. 
 *
 * @param[in]      img_id   Image Id.
 * @param[in,out]  sw_id  SW version for Image Id. 
 *
 * @return E_SUCCESS on success and E_INVALID_ARG for invalid args.
 */
sec_img_auth_error_type 
sec_img_auth_get_sw_version(uint32 img_id, 
  uint32 *sw_version);

/**
 * @brief This function returns sw type for Image Id. 
 *
 * @param[in]      img_id   Image Id.
 * @param[in,out]  sw_id  SW type for Image Id. 
 *
 * @return E_SUCCESS on success and E_INVALID_ARG for invalid args.
 */
sec_img_auth_error_type 
sec_img_auth_get_sw_id(uint32 img_id,uint32 *sw_id);

/**
 * @brief This function counts the number of bits set to 1 in the 32-bit data
 *        passed to it
 *
 * @param[in]      data_to_check   Data to count the number of bits in.
 *
 * @return uint32 value containing the count of the number of bits set.
 */
uint32 num_of_bits_set (uint32 data_to_check);

#endif /* SEC_IMG_AUTH_ENV_H */
