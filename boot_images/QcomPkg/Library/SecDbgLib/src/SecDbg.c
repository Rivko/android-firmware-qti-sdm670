/*===========================================================================

FILE:  debug_policy.c

DESCRIPTION:
  Debug Policy functionality

EXTERNALIZED FUNCTIONS


Copyright (c) 2013-2014 by Qualcomm Technologies, Inc. All Rights Reserved.
===========================================================================*/

/*=========================================================================

                          EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when         who                what, where, why
--------   ----               ------------------------------------------- 
11/06/13   hw                  Init version.
===========================================================================*/


/*==========================================================================

           Include Files for Module

==========================================================================*/
#include <comdef.h>
#include "SecDbg.h"
#include "SecDbgHw.h"
#include "boot_util.h"
#include "secboot_hw.h"
#include "secboot.h"
#include "SecDbg_oem.h"

/* Debug Policy Magic Number */
#define SEC_DBG_MAGIC 0x50474244

/* Copy locally as it can be cleaned up by TZ  */
static sec_dbg_t sec_dbg_local;

/* The debug policy is valid */ 
static boolean debug_policy_is_valid = FALSE;

/* Forward declarations */
sec_dbg_error_type sec_dbg_verify_validity(sec_dbg_t *dbgp_ptr);

/**
 * @brief See documentation in the header 
 */
sec_dbg_error_type sec_dbg_init(uint64 dbg_addr)
{
    sec_dbg_error_type status = SEC_DBG_STATUS_FAILURE;
    sec_dbg_t *dbgp_ptr = NULL;
	
    do
    {
      /*TRUE = fuse bit = 1 means to bypass DP */
      if (sec_dbg_is_disable_fuse_bit_set())
      {
        status = SEC_DBG_STATUS_HASH_IMAGE_ID_MISMATCH;
        break;
      }
	  if(dbg_addr)
	  {	
		/* TODO: set the dbgp_ptr to an input param */
		dbgp_ptr = (sec_dbg_t *)dbg_addr;
		/* Verify the format of the debug policy */
		status = sec_dbg_verify_validity(dbgp_ptr);
		if (status != SEC_DBG_STATUS_SUCCESS)
        {
          /* If the debug policy is invalid, silently ignore it */
          memset((sec_dbg_t *)dbg_addr, 0, sizeof(sec_dbg_t));
          break;
        }
	  }
      /*If it reaches here the debug policy was Validated correctly */
      qmemscpy(&sec_dbg_local, sizeof(*dbgp_ptr), ((sec_dbg_t *)(dbg_addr)), sizeof(*dbgp_ptr));
    }while (0);

    return status;
}

/*
 * 
 * @brief
 *    This function checks if debug policy ELF file passes basic sanity checks
 *
 * @retval
 * @param[in/out]  TRUE   The debug policy is valid and is OK to enable it.
 *                 FALSE  The debug policy contains bogus information. Disable the policy.
 *
 */
sec_dbg_error_type sec_dbg_verify_validity(sec_dbg_t *dbgp_ptr)
{
  sec_dbg_error_type dbg_status = SEC_DBG_STATUS_FAILURE;
  secboot_hw_etype   status = E_SECBOOT_HW_FAILURE;
  secboot_fuse_info_type fuse_info;
  uint32 i = 0;

  do
  {
    /* Debug policy already verified */
    if(debug_policy_is_valid)
    {
      dbg_status = SEC_DBG_STATUS_SUCCESS;
      break;
    }

    if (dbgp_ptr->magic != SEC_DBG_MAGIC)
    {
      dbg_status = SEC_DBG_STATUS_INVALID_MAGIC;
      break;
    }

    /* Cross check the size to make sure it matches */ 
    if (dbgp_ptr->size != sizeof(sec_dbg_t))
    {
      dbg_status = SEC_DBG_STATUS_INVALID_SIZE;
      break;
    }

    /* Check for revision */
    if (dbgp_ptr->revision != SEC_DBG_REVISION_NUMBER)
    {
      dbg_status = SEC_DBG_STATUS_INVALID_REVISION_NUMBER;
      break;
    }
  
    /* Check the serial number count */
    if ((dbgp_ptr->serial_num_count  < SEC_DBG_SERIAL_NUM_MIN_COUNT) || 
       (dbgp_ptr->serial_num_count > SEC_DBG_SERIAL_NUM_MAX_COUNT))
    {
      dbg_status = SEC_DBG_STATUS_INVALID_COUNT;
      break;
    }

    if (dbgp_ptr->flags.reserved_bits != 0)
    {
      /* must be 0 */
      dbg_status = SEC_DBG_STATUS_INVALID_RESERVED_BITS;
      break;
    }

    if (dbgp_ptr->root_cert_hash_count > SEC_DBG_CERT_ARRAY_SIZE)
    {
      /* out of bounds */
      dbg_status = SEC_DBG_STATUS_HASH_ARRAY_COUNT_INVALID;
      break;
    }

    if (0 == dbgp_ptr->root_cert_hash_count && 0 != dbgp_ptr->image_id_bitmap)
    {
      /* Hash specified but no image specified.*/
      dbg_status = SEC_DBG_STATUS_HASH_IMAGE_ID_MISMATCH;
      break;
    }

    memset(&fuse_info, 0, sizeof(secboot_fuse_info_type));
    status = secboot_hw_get_serial_num(&fuse_info.serial_num);

    if (E_SECBOOT_HW_SUCCESS != status)
    {
      /* Serial Number read error.*/
      dbg_status = SEC_DBG_STATUS_SERIAL_NUM_READ_ERROR;
      break;
    }

    /* Serial Number should match one in the array */ 
    if(!is_sec_dbg_skip_serial_number())
    {
      for ( i = 0 ; i < dbgp_ptr->serial_num_count; i++)
      {
        if ((fuse_info.serial_num) == (dbgp_ptr->serial_num_array[i])) 
        {
          dbg_status = SEC_DBG_STATUS_SUCCESS;
          break;
        }  
      }
    }
    else
    {
      dbg_status = SEC_DBG_STATUS_SUCCESS;
    }
	/* Valid Debug Policy found */
    debug_policy_is_valid = TRUE;
  }while(0);

  return dbg_status;
}

/**
 * @brief 
 *        Check whether a feature flag is set in the debug policy.
 *
 * @param[in] flag The feature flag in question
 * @retval    TRUE if the flag is set, FALSE otherwise
 *
 */
boolean sec_dbg_is_flag_set ( uint32 flag )
{
  boolean enable_flag = FALSE;

  if(!debug_policy_is_valid)
  {
      return FALSE;
  }

  switch( flag )
  {
   case SEC_DBG_ENABLE_ONLINE_CRASH_DUMPS:   
        enable_flag = sec_dbg_local.flags.enable_online_crash_dumps;   
        break;   
   case SEC_DBG_ENABLE_OFFLINE_CRASH_DUMPS:   
        enable_flag = sec_dbg_local.flags.enable_offline_crash_dumps;   
        break;   
   case SEC_DBG_ENABLE_JTAG:   
        enable_flag = sec_dbg_local.flags.enable_jtag;   
        break;   
   case SEC_DBG_ENABLE_LOGGING:   
        enable_flag = sec_dbg_local.flags.enable_logs;   
        break;   
   default: break;
  }

  return( enable_flag );
}

/**
 * @brief 
 *        Check the current image ID is in the image_id bitmap
 *
 * @param[in] sw_type is the current image ID being authenticated.
 *
 * @retval    TRUE if the id is present id the debug policy, FALSE otherwise
 *
 */
static boolean sec_dbg_check_image_id( uint32 sw_type )
{

  
    /* TODO:Image Id BitMap */    
	
    return( FALSE );  
}

/**
 * @brief See documentation in the header
 *
 */
boolean sec_dbg_is_rot_for_image( uint32 sw_type )
{
    if (!debug_policy_is_valid)
    {
        return FALSE;
    }

    if (0 == sec_dbg_local.root_cert_hash_count ) // no certs in policy
    {
      return ( FALSE ); 
    }
        
    return sec_dbg_check_image_id( sw_type );
}

/**
 * @brief See documentation in the header
* 
*/
void sec_dbg_apply_policy()
{
  if (sec_dbg_is_flag_set(SEC_DBG_ENABLE_JTAG))
  {
    sec_dbg_override_jtag();   
  }
}
/*!
* 
* @brief
*    This function authenticates the image using the debug policy.
*
* @param[in] context              context
*            image_info           secboot_image_info_type info for image
*            verified_info        secboot_verified_info_type verification info
* 
* @retval
*    sec_dbg_error_type Returns error code in case of authentication error
*                          Returns 0 in case of no error
* 
*/
sec_dbg_error_type sec_dbg_auth_image(
  secboot_handle_type*            sec_handle,
  secboot_auth_image_context*     context,
  secboot_image_info_type*        image_info,
  secboot_verified_info_type*     verified_info
)
{
  secboot_error_type status = E_SECBOOT_FAILURE;
  secboot_error_type deinit_status = E_SECBOOT_FAILURE;
  secboot_fuse_info_type fuse_info = {{0}};
  secboot_fuse_info_type sec_dbg_fuse_info = {{0}};
  sec_dbg_t *dbgp_ptr = NULL;
  uint32 rootCount=0;
 
  /* Check if debug policy is valid. If not fail auth. */ 
  if (!debug_policy_is_valid)
  {
    return SEC_DBG_STATUS_FAILURE;
  }

  /* validate debug policy inputs and image ID array */
  if ( sec_dbg_is_rot_for_image( image_info->sw_type ) )
  {
    /* Initializes the secboot handle and Crypto HW engine */
    status = context->secboot_ftbl_ptr->secboot_init( context->crypto_hash_ftbl_ptr, sec_handle);
    if( E_SECBOOT_SUCCESS == status )
    {
      memset(&fuse_info, 0x0, sizeof(fuse_info));
   
      if (E_SECBOOT_SUCCESS != status)
      {
        return SEC_DBG_STATUS_FAILURE;
      }

      for ( rootCount = 0; rootCount < dbgp_ptr->root_cert_hash_count; rootCount++ )
      {
        qmemscpy( (uint8 *)&sec_dbg_fuse_info.root_of_trust[0], CEML_HASH_DIGEST_SIZE_SHA256, 
          (uint8 *)&dbgp_ptr->root_cert_hash_array[rootCount][0], CEML_HASH_DIGEST_SIZE_SHA256);

        sec_dbg_fuse_info.rot_fuse_info.sw_rot_use_serial_num = 1;   

        status = context->secboot_ftbl_ptr->secboot_init_fuses(sec_handle, sec_dbg_fuse_info);
        if (E_SECBOOT_SUCCESS != status)
        {
          break;
        }

        status = context->secboot_ftbl_ptr->secboot_authenticate( 
                                                       sec_handle,
                                                       context->code_segment,
                                                       image_info,
                                                       verified_info);
        /* Check the result of authentication */ 
        if(E_SECBOOT_SUCCESS == status)
        {
          break;
        }
      }

      /* Deinitializes the secboot handle and Crypto HW engine*/
      /* deinit always be run, and don't report the status if already an error from previous function */
      deinit_status = context->secboot_ftbl_ptr->secboot_deinit(sec_handle);
      if (E_SECBOOT_SUCCESS == status)
      {
        status = deinit_status;
      }
    }
  }
  else
  {
   /* No Debug ROT.*/ 
   return SEC_DBG_STATUS_NO_DP_ROT;
  } 

  return E_SECBOOT_SUCCESS == status
         ? SEC_DBG_STATUS_SUCCESS
         : SEC_DBG_STATUS_FAILURE;
}
