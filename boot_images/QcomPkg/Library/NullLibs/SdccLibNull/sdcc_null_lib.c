 /*****************************************************************************
 * sdcc_mini_api.c
 *
 * SDCC(Secure Digital Card Controller) driver - Null Version.
 *
 * This file implements the SDCC driver minimized APIs for the memory device.
 *
 * Copyright (c) 2015-2017 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *****************************************************************************/
/*=============================================================================
                        Edit History


YYYY-MM-DD   who     what, where, why
----------   ---     ----------------------------------------------------------- 
2017-01-17   jt      Update sdcc_hc_set_clock() 
2015-11-08   bb      Initial version.

=============================================================================*/
#include "SdccCommon.h"

UINTN sdcc_reg_base[1];
UINTN sdcc_reg_hc[1];

struct sdcc_device * sdcc_handle_open( INT16 driveno, UINT32 phy_partition_num )
{
   return NULL;
} /* sdcc_handle_open */

SDCC_STATUS sdcc_handle_mem_get_device_info(struct sdcc_device *handle,
   sdcc_mem_info_type *sdcc_pmem_info)
{
   return 0;
}

SDCC_STATUS sdcc_get_emmc_info_raw_data(struct sdcc_device *handle,
   VOID *emmc_info_raw, UINT32 *buff_len)
{
   return 0;
}

BOOLEAN sdcc_handle_is_mem_present( struct sdcc_device *handle )
{
   return 0;
}

SDCC_STATUS sdcc_handle_read ( struct sdcc_device *handle,
   UINT32 data_address, UINT8 *buff, UINT16 num_blocks)
{
   return 0;
}

SDCC_STATUS sdcc_handle_write (struct sdcc_device *handle,
   UINT32 data_address, UINT8 *buff, UINT16 num_blocks)
{
   return 0;
}

SDCC_STATUS sdcc_handle_close( struct sdcc_device *handle )
{
   return 0;
}

SDCC_STATUS sdcc_get_config_info( sdcc_config_info_type *sdcc_pconfig_info )
{
   return 0;
}

BOOLEAN sdcc_slot_has_support (INT16 driveno, enum sdcc_slot_supports stype)
{
   return 0;
}

SDCC_STATUS sdcc_activity_timeout_event( INT16 driveno )
{
   (void)driveno;
   return 0;
}

SDCC_STATUS sdcc_poll_card_status (INT16 driveno, SDCC_CARD_STATUS card_state)
{
   return 0;
}

SDCC_STATUS sdcc_handle_erase (struct sdcc_device *handle,UINT32 start_addr,
   UINT32 num_blocks)
{
   return 0;
}

SDCC_STATUS sdcc_handle_secure_erase(struct sdcc_device *handle,
   UINT32 start_addr, UINT32 num_blocks)
{
   return 0;
}

SDCC_STATUS sdcc_enable_hw_reset (struct sdcc_device *handle)
{
   return 0;
}

SDCC_STATUS sdcc_handle_set_active_bootable_partition(struct sdcc_device *handle)
{
   return 0;
}

SDCC_STATUS sdcc_handle_set_power_on_write_protection(struct sdcc_device *handle,
   UINT32 data_address, UINT32 num_of_blocks)
{
   return 0;
}

SDCC_STATUS sdcc_handle_get_write_protection_type(struct sdcc_device *handle,
   UINT32 data_address, UINT8  *write_prot_type)
{
   return 0;
}

void sdcc_boot_set_device_info( sdcc_boot_device_info_type *device_info )
{

}

SDCC_STATUS sdcc_config_emmc_gpp_enh (struct sdcc_device *handle,
   sdcc_emmc_gpp_enh_desc_type *gpp_enh_desc)
{
   return 0;
}

SDCC_STATUS sdcc_get_slot_type( INT16 driveno, sdcc_slot_mem_type* slot_mem_type,
   sdcc_slot_access_type* slot_access_type)
{
   return 0;
}

SDCC_STATUS sdcc_sdhci_mode_enable (void)
{
   return 0;
}

static SDCC_STATUS sdcc_mmc_handle_fw_update (struct sdcc_device *handle,
  UINT8 *buff, UINT16 num_blocks)
{
   return 0;
}

SDCC_STATUS sdcc_handle_fw_update(struct sdcc_device *handle,
   UINT8 *buff, UINT16 num_blocks)
{
   return 0;
}

SDCC_STATUS sdcc_handle_get_ffu_status  (struct sdcc_device *handle,
  int *ffu_status)
{
   return 0;
}

SDCC_STATUS sdcc_handle_get_fw_version  (struct sdcc_device *handle,
  UINT64 *fw_version)
{
   return 0;
}

SDCC_STATUS sdcc_deinit_device ( INT16 driveno )
{
   return 0;
}

SDCC_STATUS sdcc_hc_set_clock (UINT32 driveno, UINT32 nDivider)
{
   return SDCC_NO_ERROR;
}

SDCC_STATUS sdcc_hc_set_uhs_mode_select (sdcc_dev_type *p_device, SDHCI_UHS_MODE mode)
{
   return SDCC_NO_ERROR;
}

#if 0
SDCC_STATUS sdcc_do_transfer_sdhci_mode (sdcc_dev_type *p_device,
   sdcc_cmd_type *p_command, UINT8 *buffer, UINT16 transfer_size)
{
   return SDCC_NO_ERROR;
}

SDCC_STATUS sdcc_hc_config_buswidth (UINT32 driveno, SDCC_MMC_BUSWIDTH buswidth)
{
   return SDCC_NO_ERROR;
}

SDCC_STATUS sdcc_command_sdhci_mode (sdcc_dev_type *sdcc_pdev, sdcc_cmd_type *cmd)
{
   return SDCC_NO_ERROR;
}
#endif

DEVICE_STATUS sdcc_get_device_status (UINT32 driveno, SDCC_STATUS  *rc)
{
   return DEVICE_IDLE;
}

DEVICE_STATUS sdcc_sdhci_check_transfer_status (
   UINT32 driveno, SDCC_STATUS *rc)
{
  return DEVICE_IDLE;
}/*For Block IO 2 only*/

#if 0
SDCC_STATUS sdcc_do_transfer_async_sdhci_mode (sdcc_dev_type *p_device,
  sdcc_cmd_type *p_command, byte *buffer, uint16 transfer_size)
{
   return SDCC_NO_ERROR;
}
#endif

void sdcc_complete_transfer (void)
{

}

SDCC_STATUS rpmb_read_counter_pkt (struct sdcc_device *handle,
   UINT32* RespPktBuffer)
{
   return SDCC_NO_ERROR;
}

SDCC_STATUS rpmb_read_sectors (struct sdcc_device *handle, UINT32* CmdPktBuffer,
                               UINT32 HalfSectorCount, UINT32* DataBuffer)
{
   return SDCC_NO_ERROR;
}

SDCC_STATUS rpmb_write_sectors (struct sdcc_device *handle, UINT32* CmdPktBuffer,
                 UINT32 HalfSectorCount, UINT32 WriteType, UINT32 *RespPktBuffer)
{
   return SDCC_NO_ERROR;
}

SDCC_STATUS sdcc_handle_request(struct sdcc_device *handle,
   UINT32 data_address, UINT8 *buff, UINT16 num_blocks, OPERATION_TYPE type)
{
   return SDCC_NO_ERROR;
}

UINT32 sdcc_bsp_get_slot_count (VOID)
{
   return 0;
}

UINT32 sdcc_bsp_get_slot_access_type (UINT32 slot_num)
{
  return 0;
}

/*lint -restore */
