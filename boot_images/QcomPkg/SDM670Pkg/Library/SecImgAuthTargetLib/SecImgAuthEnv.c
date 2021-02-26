/**
* @file sec_img_auth_env.c
* @brief Secure PIL MBA/TZ specific implementation.
*
* This file implements the TZ/MBA specific routines for secure PIL.
*
*/
/*===========================================================================
   Copyright (c) 2012-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
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
#include "HALhwio.h"
#include "SecImgAuthEnv.h"
#include "msmhwiobase.h"
#include "secboot_hwio.h"
#include "MIheader.h"

/**
 * @brief This funnction returns sw version for the peripheral image. 
 *
 * @param[in]      sec_img_id   The peripheral image.
 * @param[in,out]  sw_id  SW version for the peripheral image. 
 *
 * @return SEC_IMG_AUTH_SUCCESS on success and E_INVALID_ARG for invalid args.
 */
sec_img_auth_error_type 
sec_img_auth_get_sw_version(uint32 sec_img_id, 
  uint32 *sw_version)
{
  sec_img_auth_error_type ret = SEC_IMG_AUTH_FAILURE;
  uint32 sw_ver_fuse_value = 0;

  do
  {
    if(sw_version == NULL)
    {
      ret = SEC_IMG_AUTH_INVALID_ARG;
      break;
    }
  
    *sw_version = 0;

    if (sec_img_id == SECBOOT_QSEE_SW_TYPE)
    {
      sw_ver_fuse_value =  HWIO_INF(QFPROM_CORR_ANTI_ROLLBACK_2_MSB, TZ);
      *sw_version = num_of_bits_set(sw_ver_fuse_value);
      ret = SEC_IMG_AUTH_SUCCESS;
    }
    else if (sec_img_id == SECBOOT_QHEE_SW_TYPE)
    {
      sw_ver_fuse_value =  HWIO_INF(QFPROM_CORR_ANTI_ROLLBACK_3_MSB, HYPERVISOR);
      *sw_version = num_of_bits_set(sw_ver_fuse_value);
      ret = SEC_IMG_AUTH_SUCCESS;
    }
    else if (sec_img_id == SECBOOT_QSEE_DEVCFG_SW_TYPE)
    {
      sw_ver_fuse_value =  HWIO_INF(QFPROM_CORR_ANTI_ROLLBACK_3_MSB, DEVICE_CFG);
      *sw_version = num_of_bits_set(sw_ver_fuse_value);
      ret = SEC_IMG_AUTH_SUCCESS;
    }
    else if (sec_img_id == SECBOOT_APDP_SW_TYPE)
    {
      sw_ver_fuse_value =  HWIO_INF(QFPROM_CORR_ANTI_ROLLBACK_3_MSB, DEBUG_POLICY);
      *sw_version = num_of_bits_set(sw_ver_fuse_value);
      ret = SEC_IMG_AUTH_SUCCESS;
    }
    else if (sec_img_id == SECBOOT_WDT_SW_TYPE || sec_img_id == SECBOOT_PMIC_SW_TYPE || sec_img_id == SECBOOT_APPSBL_SW_TYPE || sec_img_id == SECBOOT_ABL_SW_TYPE)
    {
      sw_ver_fuse_value =  HWIO_INF(QFPROM_CORR_ANTI_ROLLBACK_1_LSB, XBL0);
      *sw_version = num_of_bits_set(sw_ver_fuse_value);
      sw_ver_fuse_value =  HWIO_INF(QFPROM_CORR_ANTI_ROLLBACK_1_MSB, XBL1);
      *sw_version += num_of_bits_set(sw_ver_fuse_value);      
      ret = SEC_IMG_AUTH_SUCCESS;
    }
    else if (sec_img_id == SECBOOT_RPM_FW_SW_TYPE)
    {
      sw_ver_fuse_value =  HWIO_INF(QFPROM_CORR_ANTI_ROLLBACK_2_MSB, RPM);
      *sw_version = num_of_bits_set(sw_ver_fuse_value);
      ret = SEC_IMG_AUTH_SUCCESS;
    }
    else
    {
      ret = SEC_IMG_AUTH_GET_SW_VERSION_FAILED;
      break;
    } 
    
  }while (0);
  
  return ret;
}

