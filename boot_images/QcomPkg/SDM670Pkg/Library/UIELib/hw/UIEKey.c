/*===========================================================================
                Unified Image Encryption (UIE) L1 Fuse Key 

GENERAL DESCRIPTION
  Chipset-specific key source retrieval

Copyright (c) 2014 QUALCOMM Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                                 INCLUDE FILES
===========================================================================*/
//#include <stddef.h>
#include <string.h>
#include "HALhwio.h"
#include "secboot_hwio.h"
#include "secboot_hw.h"
#include "UIE.h"
#include "UIEPriv.h"
#include "UIEEnv.h"

#define BREAKIF_SETSTATUS(cond, status_code) \
  if (cond) { ret = status_code; break; }

#define UIE_KEY_IMAGE_ENCR_ENABLED (0x1)
#define UIE_LEN_4BYTES (0x4)

bool uie_is_image_encryption_fuse_enabled(void)
{
  bool ret = false;
  uint32 encryption_enabled = 0;

  uie_map_fuse_area(HWIO_ADDR(OEM_CONFIG0), 0x1000);

  encryption_enabled = HWIO_INF(OEM_CONFIG0, IMAGE_ENCRYPTION_ENABLE);
    
  if (encryption_enabled == UIE_KEY_IMAGE_ENCR_ENABLED)
  {
    ret = true;
  }

  uie_unmap_fuse_area(HWIO_ADDR(OEM_CONFIG0), 0x1000);

  return ret;
}

int uie_get_l1_fuse_key(uie_key_src_t key_source, uint8 *key, uint32 keylen)
{
  int ret = UIE_STATUS_FAILURE;
  secboot_hw_etype secboot_hw_err = E_SECBOOT_HW_FAILURE;
  uint32 index = 0;
  uint32 rot_trans_disabled = 0;
  uint32 rot_index= 0;
  uint32 sw_rot_use_serial_num = 0;
  uint32 fuse_row_value = 0;

  uint32 code_segment = uie_get_secboot_hw_code_segment();
  
  do
  {
    BREAKIF_SETSTATUS(key == NULL || keylen != UIE_KEY_LEN ||
                      (key_source != UIE_KEY_SRC_OTP_OEM &&
                       key_source != UIE_KEY_SRC_OTP_QC),
                      UIE_STATUS_INVALID_PARAM);

    uie_map_fuse_area(HWIO_ADDRI(OEM_IMAGE_ENCR_KEYn, 0), 0x1000);
    uie_map_fuse_area(HWIO_ADDR(IMAGE_ENCR_KEY1_0), 0x1000);

    /*
    * (1) If  Feature ROT Transfer disabled (by DISABLE_ROT_TRANSFER = 1 or pk_hash_in_fuse = 0 for CSMS case), 
    *     only one root of trust = 0, always use encryption key 0.
    * (2) If  Feature ROT Transfer is not disabled (DISABLE_ROT_TRANSFER = 0) and CURRENT_SW_ROT index = 0, 
    *     then use encryption key 0.
    * (3) If  Feature ROT TRansfer is not disabled (DISABLE_ROT_TRANSFER = 0) and CURRENT_SW_ROT Index = 1, 
    *     then use encryption key 1.
    */
    secboot_hw_err = secboot_hw_get_rot_fuse_info(code_segment, &rot_trans_disabled, &rot_index, &sw_rot_use_serial_num);
    if (E_SECBOOT_HW_SUCCESS != secboot_hw_err)
    {
      ret = UIE_STATUS_HW_GET_ROT_INFO_FAILURE;
      break;
    }

    /*  use enc key 0 for case (1) and (2) */
    if ((rot_trans_disabled == SECBOOT_HW_ROT_TRANSFER_DISABLE)
        || ((rot_trans_disabled == SECBOOT_HW_ROT_TRANSFER_ENABLE) && (rot_index == 0)))
    {
      /* OEM table */
      for (index=0; index < 4; index++)
      {
        /* Read image encryption key LSB */
        fuse_row_value =  HWIO_INI (OEM_IMAGE_ENCR_KEYn, index);
        UIE_MEMSCPY(&(key[4*index]), UIE_LEN_4BYTES, &fuse_row_value, UIE_LEN_4BYTES);
      }

      /* get the key and the function could return */
      ret = UIE_STATUS_SUCCESS;
      break;
    }

    /* use enc key 1 for case (3) */
    if ((rot_trans_disabled == SECBOOT_HW_ROT_TRANSFER_ENABLE) && (rot_index == 1))
    {
      fuse_row_value = HWIO_INF(IMAGE_ENCR_KEY1_0, KEY_DATA0);
      UIE_MEMSCPY(&(key[0]), UIE_LEN_4BYTES, &fuse_row_value, UIE_LEN_4BYTES);
      fuse_row_value = HWIO_INF(IMAGE_ENCR_KEY1_1, KEY_DATA0);
      UIE_MEMSCPY(&(key[4]), UIE_LEN_4BYTES, &fuse_row_value, UIE_LEN_4BYTES);
      fuse_row_value = HWIO_INF(IMAGE_ENCR_KEY1_2, KEY_DATA0);
      UIE_MEMSCPY(&(key[8]), UIE_LEN_4BYTES, &fuse_row_value, UIE_LEN_4BYTES);
      fuse_row_value = HWIO_INF(IMAGE_ENCR_KEY1_3, KEY_DATA0);
      UIE_MEMSCPY(&(key[12]), UIE_LEN_4BYTES, &fuse_row_value, UIE_LEN_4BYTES);

      /* get the key and the function could return */
      ret = UIE_STATUS_SUCCESS;
      break;
    }

    BREAKIF_SETSTATUS(uie_fec_error_occurred(), UIE_STATUS_KEY_READ_FAIL);
  } while(0);

  uie_unmap_fuse_area(HWIO_ADDRI(OEM_IMAGE_ENCR_KEYn, 0), 0x1000);
  uie_unmap_fuse_area(HWIO_ADDR(IMAGE_ENCR_KEY1_0), 0x1000);

  return ret;  
}

uint16 uie_get_fec()
{
  // Get the FEC out
  return HWIO_INM(FEC_ESR, HWIO_FMSK(FEC_ESR, CORR_SEEN));
}

void uie_clear_fec()
{
  // Writing a 0x1 clears the FEC
  HWIO_OUT(FEC_ESR, 0x1);
}
