/**
* @file sec_img_auth_env.c
* @brief Secure PIL MBA/TZ specific implementation.
*
* This file implements the TZ/MBA specific routines for secure PIL.
*
*/
/*===========================================================================
   Copyright (c) 2014-2015 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE



when       who      what, where, why
--------   ---      ------------------------------------
08/07/14   hw       check sw version for PMIC, WDT 
26/03/13   dm       Added overflow checks & error checks
06/21/12   vg       fixed issues found while testing.
02/28/12   vg       Fixed review comments
02/20/12   vg       Ported from TZ PIL.
===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "SecImgAuthEnv.h"

/*----------------------------------------------------------------------------
 * Function Definitions 
 * -------------------------------------------------------------------------*/
 
/**
 * @brief This funnction returns code segment in SECURE_BOOTn register
 *        containing authentication information of the image.
 *
 * @param[in]      sec_img_id          The peripheral image.
 * @param[in,out]  code_segment  Code segment in SECURE_BOOTn register
 *                               containing authentication information
 *                               of the image.
 *
 * @return SEC_IMG_AUTH_SUCCESS on success and E_INVALID_ARG for invalid args.
 */
sec_img_auth_error_type 
sec_img_auth_get_code_segment(uint32 sec_img_id, 
  uint32 *code_segment)
{
  sec_img_auth_error_type ret = SEC_IMG_AUTH_FAILURE;

  do
  {

    if(code_segment == NULL)
    {
      ret = SEC_IMG_AUTH_INVALID_ARG;
      break;
    }
    *code_segment = SECBOOT_HW_APPS_CODE_SEGMENT;
    ret = SEC_IMG_AUTH_SUCCESS;
  }while (0);
  return ret;
}

/*  This function counts the number of "1's" in the 32-bit data passed. */
uint32 num_of_bits_set (uint32 data_to_check)
{
  uint32 no_of_ones = 0x0;

  while(data_to_check)
  {
    data_to_check &= (data_to_check - 1);
    no_of_ones++;
  }

  return no_of_ones;
}

