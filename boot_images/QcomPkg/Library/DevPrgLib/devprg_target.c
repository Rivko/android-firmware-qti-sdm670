/**************************************************************************
 * FILE: devprg_target.c
 *
 * This is target specific file; has definitions of APIs as per target
 * If target doesn't support a functionality then a dummy API has to be
 * maintained in its place.
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Proprietary
 *
 *************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/DevPrgLib/devprg_target.c#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-07-14   svl   Create.

===========================================================================*/

#include "boothw_target.h"
#include "boot_dload.h"
#include "boot_cache.h"
#include "pm.h"
#include "pm_pon.h"

#include "devprg_log.h"
#include "devprg_target.h"





void devprg_target_reset(void)
{
  boot_hw_reset(BOOT_HARD_RESET_TYPE);
}

void devprg_target_poweroff(void) {
  boot_hw_powerdown();
}

void devprg_target_reset_edl(void)
{
  boot_dload_transition_pbl_forced_dload();
}

int devprg_target_init(void)
{
  pm_err_flag_type pm_err;

  pm_err = pm_device_programmer_init();
  if (pm_err != PM_ERR_FLAG__SUCCESS)
    return -1;

  return 0;
}

int devprg_get_main_serial_number(uint32 *serial_number)
{
  *serial_number = ChipInfo_GetSerialNumber();

  return 0;
}

int devprg_is_secure_boot_enabled(void)
{
  boot_boolean enabled;
  if (boot_is_auth_enabled(&enabled) == E_SECBOOT_HW_SUCCESS )
  {
    if (enabled == 1)
      return 1;
  }
  return 0;
}

static sec_img_auth_error_type boot_sec_img_auth_verify_signature(uint8 *buf, uint32 size, uint32 sw_id, uint32 sw_ver, uint32 code_segment, uint8 **data_address, uint32 *data_size)
{
  sec_img_auth_verified_info_s v_info_inst;
  sec_img_auth_verified_info_s *v_info = &v_info_inst;
  sec_img_auth_error_type auth_error_type = SEC_IMG_AUTH_FAILURE;
  boot_tz_indirect_arg_list_t indirect_arg_list;
  int smc_status;
  boot_tzt_result_regs_t rsp_regs;

  indirect_arg_list.arg[0] = (uintnt) sw_ver;
  indirect_arg_list.arg[1] = (uintnt) code_segment;
  indirect_arg_list.arg[2] = (uintnt) v_info;
  indirect_arg_list.arg[3] = (uintnt) sizeof(*v_info);

  dcache_flush_region(buf, size);
  dcache_flush_region(&size, sizeof(size));
  dcache_flush_region(&sw_id, sizeof(sw_id));
  dcache_flush_region(v_info, sizeof(*v_info));
  dcache_flush_region(&indirect_arg_list, sizeof(indirect_arg_list));
  memset(&rsp_regs, 0, sizeof(rsp_regs));
  dcache_flush_region(&rsp_regs, sizeof(rsp_regs));
  smc_status = boot_fastcall_tz (TZ_PIL_SECBOOT_VERIFY_SIG_ID,
                                 TZ_PIL_SECBOOT_VERIFY_SIG_ID_PARAM_ID,
                                 buf,
                                 size,
                                 sw_id,
                                 &indirect_arg_list,
                                 &rsp_regs);

  if (smc_status == SMC_SUCCESS)
  {
    auth_error_type = SEC_IMG_AUTH_SUCCESS;
    *data_address = (uint8 *)v_info->v_info.image_hash_info.code_address;
    *data_size = v_info->v_info.image_hash_info.code_length;
  }
  else
  {
    auth_error_type = rsp_regs.reg[0];
    *data_address = buf;
    *data_size = 0;
  }

  return auth_error_type;
}

int devprg_authenticate_blob(uint8 *buf, uint32 size,
                             uint8 **data_out, uint32 *data_size)
{
  uint32 code_segment = SECBOOT_HW_APPS_CODE_SEGMENT;
  uint32 sw_id = DEVICEPROGRAMMER_SW_ID;
  uint32 sw_ver = 0;
  sec_img_auth_error_type auth_error_type = SEC_IMG_AUTH_FAILURE;
  auth_error_type = boot_sec_img_auth_verify_signature(buf, size,  sw_id, sw_ver, code_segment, data_out, data_size);
  DP_LOGD("Returned from verifying signature/authentication %d",
          auth_error_type);
  if (auth_error_type == SEC_IMG_AUTH_SUCCESS)
  {
    return 1;
  }
  DP_LOGE("Verifying signature failed with %d", auth_error_type);

  return 0;
}
