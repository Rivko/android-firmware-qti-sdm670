/*===========================================================================

                     S E C U R I T Y    S E R V I C E S

                S E C U R E  B O O T   X.5 0 9  P A R S E R

                               M O D U L E

FILE:  secboot_util.c

DESCRIPTION:
  Secure Boot wraper function

EXTERNALIZED FUNCTIONS


Copyright (c) 2013-2014 by Qualcomm Technologies, Inc. All Rights Reserved.
===========================================================================*/

/*=========================================================================

                          EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when         who                what, where, why
--------   ----               ------------------------------------------- 
05/05/14   hw                  add engg cert support
01/21/14   hw                  enable mrc signed image (with root 0) on non-secure device
01/20/14   hw                  add parm define to enable unsigned image on 8092
01/10/14   hw                  disallow unsigned image to be loaded
11/06/13   hw                  Init version.
===========================================================================*/


/*==========================================================================

           Include Files for Module

==========================================================================*/
#include "SecImgUtil.h"
#include "boot_util.h"
#include "SecDbg.h"

// check data + len is always smaller than the bound and no overflow
#define DATA_IN_BOUND(data, len, bound) ((data + len > data) && (data + len < bound))

/**
 * @brief checks if the image is signed or not
 *
 * @param[in] image_info image information
 *
 * @return TRUE for signed image, return FALSE for unsigned image
 */
static boolean sec_img_auth_is_signed_image(secboot_image_info_type *image_info)
{
  if (image_info != NULL)
  {
    if ( (image_info->signature_ptr != NULL) &&
         (image_info->signature_len != 0) && 
         (image_info->x509_chain_ptr != NULL) && 
         (image_info->x509_chain_len != 0))
    {
      return TRUE;
    }
  }
  return FALSE;
}

/*!
* 
* @brief
*    This function searches fuse info from the image
*
* @param[in]  image_info      image information
*             
* @param[in/out]  fuse_info   fuse information from the image
* 
* @retval
*    sec_img_auth_error_type Returns error code in case of authentication error
*                          Returns 0 in case of no error
* 
*/
static sec_img_auth_error_type sec_img_auth_get_fuse_info_from_image(
  secboot_fuse_info_type *fuse_info, 
  secboot_image_info_type* image_info)
{
  /* return failure, if the image pointer is NULL */
  if ( (image_info == NULL) || 
     (image_info->x509_chain_ptr == NULL) ||
     (image_info->signature_ptr == NULL) )
  {
    return SEC_IMG_AUTH_INVALID_ARG;
  }
  
  /* default specific fuse information here: */
  memset(fuse_info->root_of_trust, 0, sizeof(fuse_info->root_of_trust));
  
  fuse_info->use_root_of_trust_only = FALSE;
  fuse_info->auth_use_serial_num = 0;
  fuse_info->serial_num = 0;

  /* init multiple root cert fuse value  for default one root qpsa signed image */
  fuse_info->rot_fuse_info.disable_rot_transfer = 0;
  fuse_info->rot_fuse_info.current_sw_rot = 0;
  fuse_info->rot_fuse_info.sw_rot_use_serial_num = 0;

  fuse_info->msm_hw_id = 0x0;

  return SEC_IMG_AUTH_SUCCESS;
}

/*!
* 
* @brief
*    This function checks if image authentication is required or not.
*    The auth is required for both secboot-enabled device and secboot-disabled device.
*    The unsigned image is not allowed, if FEATURE_ALLOW_UNSIGNED_IMAGE is not defined.
*
* @param[in]  context              context that stores function pointers and related info.
*             image_info           image information
*
* @param[in/out]  is_auth_required   flag to indicate if authentication is required
* 
* @retval
*    sec_img_auth_error_type Returns error code in case of authentication error
*                          Returns 0 in case of no error
* 
*/
static sec_img_auth_error_type auth_img_is_auth_required(
  secboot_auth_image_context*  context,
  secboot_image_info_type*     image_info,
  uint32*                      is_auth_required
)
{
  secboot_hw_etype   hw_status = E_SECBOOT_HW_FAILURE;
  uint32             is_auth_enabled = FALSE;

  if ((NULL == context) || (NULL == context->secboot_hw_ftbl_ptr))
    return SEC_IMG_AUTH_INVALID_ARG;

  hw_status = context->secboot_hw_ftbl_ptr->secboot_hw_is_auth_enabled(context->code_segment, &is_auth_enabled);  
  if(E_SECBOOT_HW_SUCCESS != hw_status)
  {
    return SEC_IMG_AUTH_HW_FAILURE;
  }

  /* secure device, auth is required */
  if (TRUE == is_auth_enabled)
  {
    *is_auth_required = TRUE;
  }
  /* non-secure device: auth signed image */
  else if ((FALSE == is_auth_enabled) && 
           (TRUE == sec_img_auth_is_signed_image(image_info)))
  {
    *is_auth_required = TRUE;
  }
  else
  {
#ifdef FEATURE_ALLOW_UNSIGNED_IMAGE
    *is_auth_required = FALSE;
    status = E_SECBOOT_SUCCESS;
#else
    /* unsigned image in non-secure device is not allowed.*/
    return SEC_IMG_AUTH_IMAGE_NOT_SUPPORTED;
#endif
  }

  return SEC_IMG_AUTH_SUCCESS;
}

/*!
* 
* @brief
*    This function authenticates the image using the prod cert fuse info. It accepts fuse
*    info provided outside the function.
*
* @param[in] code_segment         code segment.
*            image_info           secboot_image_info_type info for image
*            verified_info        secboot_verified_info_type verification info
*            secboot_ftbl_ptr     the pointer to secboot functions
*            secboot_hw_ftbl_ptr  the pointer to secboot hardware dependent functions
*            crypto_hash_ftbl_ptr the pointer to hash functions
* 
* @retval
*    sec_img_auth_error_type Returns error code in case of authentication error
*                          Returns 0 in case of no error
* 
*/
static sec_img_auth_error_type auth_image(
  secboot_auth_image_context*     context,
  secboot_image_info_type*        image_info,
  secboot_verified_info_type*     verified_info,
  secboot_fuse_info_type*         image_fuse_info
)
{
  sec_img_auth_error_type status = SEC_IMG_AUTH_FAILURE;
  sec_dbg_error_type dbg_status = SEC_DBG_STATUS_FAILURE;
  secboot_error_type deinit_status = E_SECBOOT_FAILURE;
       
  secboot_handle_type    sec_handle = {{0}};
   
  if ( (NULL == context)
       || (context->secboot_ftbl_ptr == NULL) 
       || (context->secboot_hw_ftbl_ptr == NULL))
  {
    return SEC_IMG_AUTH_INVALID_ARG;
  }

  if ( (context->secboot_ftbl_ptr->secboot_init == NULL)
       || (context->secboot_ftbl_ptr->secboot_init_fuses == NULL)
       || (context->secboot_ftbl_ptr->secboot_authenticate == NULL)
       || (context->secboot_ftbl_ptr->secboot_deinit == NULL))
  {
    return SEC_IMG_AUTH_INVALID_ARG;
  }

  /* Initializes the secboot handle and Crypto HW engine */
  status = (sec_img_auth_error_type)context->secboot_ftbl_ptr->secboot_init( context->crypto_hash_ftbl_ptr, &sec_handle);
  if( SEC_IMG_AUTH_SUCCESS == status )
  {
    do
    {
      /* initialize fuse info if that is provided. */
      if ( NULL != image_fuse_info )
      {
        status = (sec_img_auth_error_type)context->secboot_ftbl_ptr->secboot_init_fuses(&sec_handle, *image_fuse_info);
        if (SEC_IMG_AUTH_SUCCESS != status)
        {
          break;
        }
      }

      status = (sec_img_auth_error_type)context->secboot_ftbl_ptr->secboot_authenticate( 
                                                       &sec_handle,
                                                       context->code_segment,
                                                       image_info,
                                                       verified_info);
      /* Check the result of authentication */ 
      if(SEC_IMG_AUTH_SUCCESS != status)
      {
        /* Authenticate using Debug Policy Root Certs if production root cert fails */
        dbg_status = sec_dbg_auth_image( &sec_handle, context, image_info, verified_info);
        if (dbg_status == SEC_DBG_STATUS_SUCCESS)
        {
          status = SEC_IMG_AUTH_SUCCESS;
	    }
        break;
      }
    }
    while(0);
      
    /* Deinitializes the secboot handle and Crypto HW engine*/
    /* deinit always be run, and don't report the status if already an error from previous function */
    deinit_status = context->secboot_ftbl_ptr->secboot_deinit(&sec_handle);
    if (SEC_IMG_AUTH_SUCCESS == status)
    {
      status = (sec_img_auth_error_type)deinit_status;
    }
  }
    
  return status;  
}

/*===========================================================================
**  Public functions
** ==========================================================================
*/
/**
 * @brief See documentation in the header
 *
 */
sec_img_auth_error_type sec_img_auth_authenticate_image_context(
  secboot_auth_image_context*  context,
  secboot_image_info_type* image_info,
  secboot_verified_info_type* verified_info
)
{
  sec_img_auth_error_type  status = SEC_IMG_AUTH_FAILURE;
  secboot_hw_etype    hw_status = E_SECBOOT_HW_FAILURE;
  uint32       is_auth_required = FALSE;
  uint32        is_auth_enabled = FALSE;

  secboot_fuse_info_type  image_fuse_info;
  secboot_fuse_info_type*  image_fuse_info_ptr = NULL;

  memset(&image_fuse_info, 0, sizeof(image_fuse_info));

  if ((NULL == context) || (NULL == image_info) || (NULL == verified_info))
  {
    return SEC_IMG_AUTH_INVALID_ARG;
  }

  if ((context->secboot_ftbl_ptr == NULL) 
       || (context->secboot_hw_ftbl_ptr == NULL)
       || (context->secboot_hw_ftbl_ptr->secboot_hw_is_auth_enabled == NULL) )
  {
    return SEC_IMG_AUTH_INVALID_ARG;
  }

  /* If FEATURE_ALLOW_UNSIGNED_IMAGE is not defined, unsigned image is not accepted and
     this function returns failure. If FEATURE_ALLOW_UNSIGNED_IMAGE is defined, unsigned 
     image is accepted, the call passes and is_auth_required is set to FALSE. */
  status = auth_img_is_auth_required(context, image_info, &is_auth_required);
  if (SEC_IMG_AUTH_SUCCESS != status)
  {
    return status;
  }

  if (is_auth_required)
  {
      /* fuse info is extracted from the image for secboot disabled device. */
      /* for secboot enabled device, fuse info is always from the qfprom hardware. */
      hw_status = context->secboot_hw_ftbl_ptr->secboot_hw_is_auth_enabled(context->code_segment, &is_auth_enabled); 
      if(E_SECBOOT_HW_SUCCESS != hw_status)
      {
        return SEC_IMG_AUTH_HW_FAILURE;
      }

      if ( FALSE == is_auth_enabled )
      {
        status = sec_img_auth_get_fuse_info_from_image(&image_fuse_info, image_info);
        if(SEC_IMG_AUTH_SUCCESS != status)
        {
          return status;
        }
        image_fuse_info_ptr = &image_fuse_info;
      }

      status = auth_image(
                context,
                image_info,
                verified_info,
                image_fuse_info_ptr
               );
      if (SEC_IMG_AUTH_SUCCESS == status)
      {
        /* return if auth passes with production cert*/
        return status;
      }

      /* for non-sec device: ignore the untrusted root, as oem might use oem-signed image */
      if ( ( FALSE == is_auth_enabled ) && (E_SECBOOT_UNTRUSTED_ROOT == (secboot_error_type)status) )
      {
        status = SEC_IMG_AUTH_SUCCESS;
        return status;
      }
  }
  else /* when unsigned image is accepted and auth is not required, just return pass*/
  {
    status = SEC_IMG_AUTH_SUCCESS;
  }

  return status;
}

/**
 * @brief See documentation in the header
 *
 */
secboot_error_type secboot_authenticate_image(
  uint32 code_segment,
  secboot_image_info_type* image_info,
  secboot_verified_info_type* verified_info,
  secboot_ftbl_type* secboot_ftbl_ptr,
  secboot_hw_ftbl_type* secboot_hw_ftbl_ptr,
  secboot_crypto_hash_ftbl_type* crypto_hash_ftbl_ptr
)
{
  secboot_error_type status = E_SECBOOT_FAILURE;
  secboot_auth_image_context  context;
  memset(&context, 0, sizeof(context));

  context.code_segment = code_segment;
  context.secboot_ftbl_ptr = secboot_ftbl_ptr;
  context.secboot_hw_ftbl_ptr = secboot_hw_ftbl_ptr;
  context.crypto_hash_ftbl_ptr = crypto_hash_ftbl_ptr;

  status = (secboot_error_type)sec_img_auth_authenticate_image_context(
                                &context,
                                image_info,
                                verified_info
                                );

  return status;
}
