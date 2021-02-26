/**********************************************************************
 * File: sdcc_mmc_util.c
 *
 * Services: 
 *    This file provides SDCC services for MMC device.
 *
 * Description:
 *    This file contains the MMC specific utility function.
 *
 * Copyright (c) 2014-2017, 2019 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 **********************************************************************/
/*=======================================================================
                        Edit History


when         who     what, where, why
----------   ---     ----------------------------------------------- 
2019-02-08   sb      Ignore cmd end bit error in tuning process
2017-10-02   sb      Correct the card size in sectors calculation
2017-09-13   jt      Disable clock before changing clock parameters
2017-06-12   sb      Get platform specific speed config 
2017-05-12   sb      Add has400 enhanced strobe mode support
2016-01-18   eo      Reduced sdcc code size
2015-12-18   jt      Cleanup CDR tuning  
2015-12-17   jt      Change tuning buffer  
2015-09-18   whc     Change tuning sequence
2015-06-29   whc     fixed potential null pointer
2015-04-19   bn      Replaced invalid partitions error message with warning
2015-04-15   bn      Added GetStorageInfo support
2015-03-26   bn      Cbanged HS200 tuning CRC error message to D_INFO
2015-02-27   rm      Get SpeedMode from PCD, Added HAL_sdhci_DLLClockEnable
2015-01-30   bn      Set DDR50 DMA to DISABLE for SUFI 
2015-01-26   rm      Integraged HS200/HS400 support for eMMC from UEFI 3.0
2014-11-06   bn      Added option to disable DDR50 by query the BSP config
2014-11-07   bn      Added eMMC FW Update
2014-07-15   bn      Initial version. Branched from 8994 SBL

===========================================================================*/
#include "sdcc_priv.h"
#include "sdcc_util.h"
#include "sdcc_mmc_util.h"
#include "sdcc_handle_util.h"
#include "sdcc_partition_types.h"
#include "sdcc_hc.h"
#include "SdccBsp.h"
#include "sdcc_pal_tool.h"
#include <Library/PcdLib.h>

/*lint -save -e641 Suppress 'Converting enum to int' warning */
/* buffer to retrieve the extended CSD data */
extern UINT8 ext_csd_raw_data[SDCC_MMC_EXT_CSD_SIZE];
extern UINT8 emmc_cid_raw_data[SDCC_MMC_CID_SIZE];

static sdcc_ext_csd_type ext_csd_decoded;

static SDCC_STATUS sdcc_mmc_check_switch_error(sdcc_dev_type *sdcc_pdev);
static SDCC_STATUS sdcc_emmc_get_boot_partition_enable_val(
   UINT8 part_config_reg, UINT8 *boot_enabled_val);
static SDCC_STATUS sdcc_emmc_calc_write_prot_grp_size(
   sdcc_dev_type *sdcc_pdev, sdcc_ext_csd_type *ext_csd);
static SDCC_STATUS sdcc_emmc_check_addr_out_of_range(sdcc_dev_type *sdcc_pdev);
static SDCC_STATUS sdcc_emmc_set_user_pwr_on_wp(
   struct sdcc_device *handle, UINT32 data_address, UINT32 num_of_blocks);
static SDCC_STATUS sdcc_emmc_set_boot_pwr_on_wp(struct sdcc_device *handle);
static SDCC_STATUS sdcc_emmc_calc_and_set_hc_erase_grp_size(
   sdcc_dev_type *sdcc_pdev, sdcc_ext_csd_type *ext_csd);
static inline void sdcc_emmc_dll_mclk_freq_set(sdcc_dev_type *sdcc_pdev);
static inline SDCC_STATUS sdcc_emmc_init_cm_dll_sdc4 (sdcc_dev_type *sdcc_pdev);
UINT8 gTempBuffer[128];

/******************************************************************************
* Name: sdcc_mmc_check_switch_error
*
* Description:
*    This function sends CMD13 SEND_STATUS to the memory device to get the
*    card status.  It then checks the card state and switch error status.
*
* Arguments:
*    sdcc_pdev [IN] : pointer to device type
*
* Returns:
*    SDCC_NO_ERROR if card state is transfer state and no switch error occurs
*    Otherwise, returns error code
*
******************************************************************************/
static
SDCC_STATUS sdcc_mmc_check_switch_error
(
   sdcc_dev_type *sdcc_pdev
)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_cmd_type sdcc_cmd;

   sdcc_cmd.cmd = SD_CMD13_SEND_STATUS;
   sdcc_cmd.cmd_arg = SDCC_ARG_RCA(sdcc_pdev->rca);
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   rc = sdcc_command(sdcc_pdev, &sdcc_cmd);
   if (rc != SDCC_NO_ERROR)
   {
      DEBUG ((EFI_D_ERROR, "Failed to get card status"));
      return rc;
   }

   /* check if current card state is in TRAN state */
   if ((SDCC_CARD_STATUS)SDCC_XLATE_CARD_CURRENT_STATE(sdcc_cmd.resp[0]) !=
       SDCC_CARD_TRAN)
   {
      DEBUG ((EFI_D_ERROR, "eMMC is not ready"));
      return SDCC_ERR_CARD_READY;
   }

   /* check if switch error occurs */
   if (SDCC_XLATE_SWITCH_ERROR(sdcc_cmd.resp[0]) == SDCC_MMC_SWITCH_ERROR)
   {
      DEBUG ((EFI_D_ERROR, "Switch error occurs"));
      return SDCC_ERR_SWITCH;
   }

   return SDCC_NO_ERROR;
} /* sdcc_mmc_check_switch_error */

/******************************************************************************
* Name: sdcc_mmc_switch
*
* Description:
*    This function sends CMD6 SWITCH to the memory device to modify the EXT_CSD
*    registers.  It then checks if switch error occurs.
*
* Arguments:
*    sdcc_pdev  [IN] : pointer to device type
*    switch_arg [IN] : command argument for CMD6 SWITCH
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS sdcc_mmc_switch
(
   sdcc_dev_type *sdcc_pdev,
   UINT32 switch_arg
)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_cmd_type sdcc_cmd;

   sdcc_cmd.cmd = SD_CMD6_SWITCH;
   sdcc_cmd.cmd_arg = switch_arg;
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 1;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   rc = sdcc_command(sdcc_pdev, &sdcc_cmd);
   if (rc != SDCC_NO_ERROR)
   {
      return rc;
   }

   /* San Disk Workaround: Add 1ms delay between CMD6 and CMD13 for LS to HS switching */
   /* Switching from LS to HS mode by sending CMD6 might fail in some command sequence scenarios: */
   /* - The problem occurs when the host sends CMD13 (check status) during CMD6 busy period.*/
   /* - The problem occurs when the Switch CMD6 LS to HS is sent before any */
   /*   other CMD6 used during the initialization.*/
   if( switch_arg == (SDCC_EMMC_ENABLE_ERASE_GROUP_DEF | ((UINT32) 1 << 8)))
   {
      sdcc_mdelay(1);
   }

   rc = sdcc_mmc_check_switch_error(sdcc_pdev);
   if (rc != SDCC_NO_ERROR)
   {
      DEBUG ((EFI_D_ERROR, "Failed %d in switching to 0x%x", rc, switch_arg));
      return rc;
   }

   return rc;
} /* sdcc_mmc_switch */

/******************************************************************************
* Name: sdcc_emmc_get_boot_partition_enable_val
*
* Description: 
*    This function extracts the BOOT_PARTITION_ENABLE bits from the
*    PARTITION_CONFIG slice in EXT_CSD register.
*
* Parameters:
*    part_config_reg   [IN] : PARTITION_CONFIG slice in EXT_CSD register
*    boot_enabled_val [OUT] : pointer to the value of BOOT_PARTITION_ENABLE bits
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
static
SDCC_STATUS sdcc_emmc_get_boot_partition_enable_val
(
   UINT8 part_config_reg,
   UINT8 *boot_enabled_val
)
{
   UINT8 boot_val = 0;

   boot_val = SDCC_EXTRACT_BIT_VAL_FROM_BYTE_REG(part_config_reg,
                 SDCC_EMMC_BIT_BOOT_PARTITION_ENABLE,
                 SDCC_EMMC_NUM_BITS_BOOT_PARTITION_ENABLE);
   /* validate the boot value */
   /* Per PARTITION_CONFIG definition in JEDEC Standard No. 84-A44, */
   /* 0x3 to 0x6 is reserved. */
   if (!SDCC_EMMC_IS_BOOT_PARTITION_VAL_VALID(boot_val))
   {
      DEBUG ((EFI_D_ERROR, "Invalid boot partition enable value 0x%x", boot_val));
      return SDCC_ERR_INVALID_PARAM;
   }

   *boot_enabled_val = boot_val;

   return SDCC_NO_ERROR;
} /* sdcc_emmc_get_boot_partition_enable_val */

/******************************************************************************
* Name: sdcc_emmc_calc_write_prot_grp_size
*
* Description: 
*    This function calculates the write protection group size, in sectors.  It
*    examines if the memory device is embedded MMC, high-capacity erase unit
*    size and high-capacity write protect group size are non-zero.  If so,
*    ERASE_GROUP_DEF bit in extended CSD register is enabled to use the
*    high-capacity write protect group size definition.  If the memory device
*    is embedded MMC and either high-capacity erase unit size or high-capacity
*    write protect group size is zero, default write group size definition
*    from the CSD register is used.  If the memory device is a removable MMC
*    card, write protection should not be supported and the write protect group
*    size would be zero.
*
* Parameters:
*    sdcc_pdev [IN] : pointer to the device type
*    ext_csd   [IN] : pointer to the extended CSD structure that data is read from
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
static
SDCC_STATUS sdcc_emmc_calc_write_prot_grp_size
(
   sdcc_dev_type *sdcc_pdev,
   sdcc_ext_csd_type *ext_csd
)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;

   /* examine if MMC device is embedded */
   if (sdcc_pdev->mem.mmc_device_type == SDCC_MMC_DEVICE_TYPE_REMOVABLE_CARD)
   {
      sdcc_pdev->mem.mem_info.write_protect_group_size_in_sectors = 0;
      /* only support write protect feature on embedded MMC */
      sdcc_pdev->mem.write_prot_support = FALSE;
      return SDCC_NO_ERROR;
   }

   /* make sure high-capacity erase-unit size or high-capacity */
   /* write protect group size are non-zero */
   if (ext_csd->hc_erase_grp_size == 0 || ext_csd->hc_wp_grp_size == 0)
   {
      /* Per section 8.4 in JEDEC Standard No. 84-A44, write protect group */
      /* size definition would be from CSD register.  As such, enabling */
      /* ERASE_GROUP_DEF is not needed. */
      /* The non high-capacity write protect group size has been calculated */
      /* when decoding CSD register. */
      return SDCC_NO_ERROR;
   }

   /* MMC device is embedded, either BGA or POP */
   /* enable ERASE_GROUP_DEF to use high-capacity write protect definition */
   rc = sdcc_mmc_switch(sdcc_pdev,
           SDCC_EMMC_ENABLE_ERASE_GROUP_DEF | ((UINT32) 1 << 8));

   if (rc != SDCC_NO_ERROR)
   {
      DEBUG ((EFI_D_INFO, "Failed in enabling ERASE_GROUP_DEF "
               "- Use default erase group size"));
      /* If the switch failed, use the write protect group size calculated */
      /* from CSD register */
      return SDCC_NO_ERROR;
   }

   /* calculate the write protect group size per */
   /* section 8.4 in JEDEC Standard No. 84-A44 */
   sdcc_pdev->mem.mem_info.write_protect_group_size_in_sectors =
      (512 * 1024 / SDCC_DEFAULT_BLK_LENGTH_SIZE) *
      ext_csd->hc_erase_grp_size * ext_csd->hc_wp_grp_size;

   return SDCC_NO_ERROR;
} /* sdcc_emmc_calc_write_prot_grp_size */

/******************************************************************************
* Name: sdcc_emmc_calc_and_set_hc_erase_grp_size
*
* Description: 
*    This function calculates the erase group size, in sectors.  It
*    examines if the memory device is embedded MMC, high-capacity erase unit
*    size and high-capacity write protect group size are non-zero.  If so,
*    ERASE_GROUP_DEF bit in extended CSD register is enabled to use the
*    high-capacity erase group size definition.  If the memory device
*    is embedded MMC and either high-capacity erase unit size or high-capacity
*    write protect group size is zero, default erase size definition
*    from the CSD register is used.  If the memory device is a removable MMC
*    card, erase should not be supported and the erase group size would be zero.
*
* Parameters:
*    sdcc_pdev [IN] : pointer to the device type
*    ext_csd   [IN] : pointer to the extended CSD structure that data is read from
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
static
SDCC_STATUS sdcc_emmc_calc_and_set_hc_erase_grp_size
(
   sdcc_dev_type *sdcc_pdev,
   sdcc_ext_csd_type *ext_csd
)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;

   /* examine if MMC device is embedded */
   if (sdcc_pdev->mem.mmc_device_type == SDCC_MMC_DEVICE_TYPE_REMOVABLE_CARD)
   {
      /* only support erase feature on embedded MMC */
      sdcc_pdev->mem.mem_info.erase_group_size_in_sectors = 0;
      return SDCC_NO_ERROR;
   }

   /* make sure high-capacity erase-unit size or high-capacity */
   /* write protect group size are non-zero */
   if (ext_csd->hc_erase_grp_size == 0 || ext_csd->hc_wp_grp_size == 0)
   {
      /* Per section 8.4 in JEDEC Standard No. 84-A44, erase group size */
      /* definition would be from CSD register.  As such, enabling */
      /* ERASE_GROUP_DEF is not needed. */
      /* The non high-capacity erase group size has been calculated */
      /* when decoding CSD register. */
      return SDCC_NO_ERROR;
   }

   /* Enable ERASE_GROUP_DEF to use high-capacity erase definition */
   rc = sdcc_mmc_switch(sdcc_pdev,
           SDCC_EMMC_ENABLE_ERASE_GROUP_DEF | ((UINT32) 1 << 8));

   if (rc != SDCC_NO_ERROR)
   {
      DEBUG ((EFI_D_INFO, "Failed in enabling ERASE_GROUP_DEF "
               "- Use default erase group size"));
      /* If the switch failed, use the erase group size calculated */
      /* from CSD register */
      return SDCC_NO_ERROR;
   }

   /* calculate the erase group size per */
   /* section 8.4 in JEDEC Standard No. 84-A44: */
   /* Erase Unit Size = 512Kbyte × HC_ERASE_GRP_SIZE */
   sdcc_pdev->mem.mem_info.erase_group_size_in_sectors =
      (512 * 1024 * ext_csd->hc_erase_grp_size) / SDCC_DEFAULT_BLK_LENGTH_SIZE;

   /* Erase Timeout = 300ms × ERASE_TIMEOUT_MULT*/
   sdcc_pdev->mem.hc_erase_timeout_in_ms = 300 * ext_csd->erase_timeout_mult;

   /* Trim Timeout = 300ms × TRIM_MULT*/
   sdcc_pdev->mem.trim_timeout_in_ms = 300 * ext_csd->trim_mult;

   return SDCC_NO_ERROR;
} /* sdcc_emmc_calc_and_set_hc_erase_grp_size */
 

/******************************************************************************
* Name: sdcc_emmc_check_addr_out_of_range
*
* Description:
*    This function sends CMD13 SEND_STATUS to the memory device to get the
*    card status.  It then checks if address out of range error bit is set.
*
* Arguments:
*    sdcc_pdev [IN] : pointer to device type
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
static
SDCC_STATUS sdcc_emmc_check_addr_out_of_range
(
   sdcc_dev_type *sdcc_pdev
)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_cmd_type sdcc_cmd;

   sdcc_cmd.cmd       = SD_CMD13_SEND_STATUS;
   sdcc_cmd.cmd_arg   = SDCC_ARG_RCA(sdcc_pdev->rca);
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   rc = sdcc_command(sdcc_pdev, &sdcc_cmd);
   if (rc != SDCC_NO_ERROR)
   {
      return rc;
   }

   /* check if address out of range error occurs */
   if (SDCC_MMC_ADDR_OUT_OF_RANGE_ERROR ==
       SDCC_XLATE_ADDR_OUT_OF_RANGE(sdcc_cmd.resp[0]))
   {
      DEBUG ((EFI_D_ERROR, "Address out of range"));
      return SDCC_ERR_PWR_ON_WRITE_PROT;
   }

   return SDCC_NO_ERROR;
} /* sdcc_emmc_check_addr_out_of_range */

/******************************************************************************
* Name: sdcc_emmc_set_user_pwr_on_wp
*
* Description:
*    This function enables the power-on period write protection to the user
*    area.  The write protection is applied to the specified parameter
*    data_address.  Parameter num_of_blocks should be aligned with the write
*    protect group size specified in write_protect_group_size_in_sectors in
*    structure sdcc_mem_info_type.  CMD28 SET_WRITE_PROT is sent to the eMMC
*    device repeatedly until the requested regions are write protected.
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
*    data_address  [IN] : start of the sector to get power-on write protected
*    num_of_blocks [IN] : number of blocks to be power-on write protected
*                         (512 bytes per block)
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
static
SDCC_STATUS sdcc_emmc_set_user_pwr_on_wp
(
   struct sdcc_device *handle,
   UINT32 data_address,
   UINT32 num_of_blocks
)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_dev_type *sdcc_pdev = NULL;
   sdcc_cmd_type sdcc_cmd;
   UINT32 loop_cnt = 0;
   UINT32 i = 0;

   if (num_of_blocks == 0)
   {
      DEBUG ((EFI_D_ERROR, "Invalid number of blocks for enabling power-on WP"));
      return SDCC_ERR_INVALID_PARAM;
   }

   sdcc_pdev = handle->hdev;

   SetMem(ext_csd_raw_data, sizeof(ext_csd_raw_data), 0);
   /* read the extended CSD register */
   rc = sdcc_get_ext_csd(sdcc_pdev, ext_csd_raw_data);
   if (rc != SDCC_NO_ERROR)
   {
      return rc;
   }

   /* refer to annex A.5 in JEDEC Standard No. 84-A44 for setting */
   /* the US_PWR_WP_EN */
   /* make sure power-on write protect is not disabled */
   if (SDCC_EXTRACT_BIT_VAL_FROM_BYTE_REG(
       ext_csd_raw_data[SDCC_EMMC_EXT_CSD_SLICE_USER_WP],
       SDCC_EMMC_BIT_US_PWR_WP_DIS, 1))
   {
      /* power-on period write protection for user area is disabled */
      DEBUG ((EFI_D_ERROR, "Use of user power-on WP has been disabled"));
      return SDCC_ERR_PWR_ON_WRITE_PROT;
   }
   /* make sure permanent write protect is not enabled */
   /* issuing CMD28 when both US_PERM_WP_EN and US_PWR_WP_EN are set will */
   /* result in write protection group permanently protected */
   if (SDCC_EXTRACT_BIT_VAL_FROM_BYTE_REG(
       ext_csd_raw_data[SDCC_EMMC_EXT_CSD_SLICE_USER_WP],
       SDCC_EMMC_BIT_US_PERM_WP_EN, 1))
   {
      /* permanent write protection for user area is enabled */
      DEBUG ((EFI_D_ERROR, "User permanent WP has been enabled"));
      return SDCC_ERR_PWR_ON_WRITE_PROT;
   }

   /* enable power-on period write protection for the user area */
   rc = sdcc_mmc_switch(sdcc_pdev, SDCC_EMMC_ENABLE_USER_PWR_WP);
   if (rc != SDCC_NO_ERROR)
   {
      DEBUG ((EFI_D_ERROR, "Failed in enabling user power-on write protect %d", rc));
      return rc;
   }

   /* sanity check to make sure write protect group size is non-zero */
   if (sdcc_pdev->mem.mem_info.write_protect_group_size_in_sectors == 0)
   {
      DEBUG ((EFI_D_ERROR, "Write protect group size is 0"));
      return SDCC_ERR_PWR_ON_WRITE_PROT;
   }

   /* calculate the number of loops that SET_WRITE_PROT needs to be sent */
   if (num_of_blocks % sdcc_pdev->mem.mem_info.write_protect_group_size_in_sectors)
   {
      loop_cnt = (num_of_blocks /
                 sdcc_pdev->mem.mem_info.write_protect_group_size_in_sectors) + 1;
   }
   else
   {
      loop_cnt = num_of_blocks /
                 sdcc_pdev->mem.mem_info.write_protect_group_size_in_sectors;
   }

   /* send CMD28 to set the write protection bit of the addressed group */
   sdcc_cmd.cmd       = SD_CMD28_SET_WRITE_PROT;
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 1;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   for (i = 0; i < loop_cnt; i++)
   {
      sdcc_cmd.cmd_arg = SDCC_MEM_ACCESS_CMD_ARG(sdcc_pdev->card_type,
         data_address +
         i * sdcc_pdev->mem.mem_info.write_protect_group_size_in_sectors,
         sdcc_pdev->mem.mem_info.block_len);

      rc = sdcc_command(sdcc_pdev, &sdcc_cmd);
      /* check if error occurs in sending CMD28 */
      if (rc != SDCC_NO_ERROR)
      {
         DEBUG ((EFI_D_ERROR, "CMD28 failed %d", rc));
         return rc;
      }

      /* As recommended by one of the eMMC device manufacturers, */
      /* check for ADDRESS_OUT_OF_RANGE in both responses as below: */
      /* 1) response to CMD28 per JEDEC Standard No. 84-A44 */
      /* 2) response to CMD13 */

      /* check if address is out of range in the CMD28 response */
      if (SDCC_MMC_ADDR_OUT_OF_RANGE_ERROR ==
          SDCC_XLATE_ADDR_OUT_OF_RANGE(sdcc_cmd.resp[0]))
      {
         DEBUG ((EFI_D_ERROR, "CMD28 response with address out of range"));
         return SDCC_ERR_PWR_ON_WRITE_PROT;
      }

      /* send CMD13 to check if address is out of range */
      rc = sdcc_emmc_check_addr_out_of_range(sdcc_pdev);
      if (rc != SDCC_NO_ERROR)
      {
         DEBUG ((EFI_D_ERROR, "CMD28 failed with address 0x%x", sdcc_cmd.cmd_arg));
         return rc;
      }
   }

   return SDCC_NO_ERROR;
} /* sdcc_emmc_set_user_pwr_on_wp */

/******************************************************************************
* Name: sdcc_emmc_set_boot_pwr_on_wp
*
* Description:
*    This function enables the power-on period write protection to the boot
*    areas.  The write protection is applied to the entire boot partition and
*    both boot areas would get write protected.
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
static
SDCC_STATUS sdcc_emmc_set_boot_pwr_on_wp
(
   struct sdcc_device *handle
)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_dev_type *sdcc_pdev = NULL;
   sdcc_pdev = handle->hdev;

   SetMem(ext_csd_raw_data, sizeof(ext_csd_raw_data), 0);
   /* read the extended CSD register */
   rc = sdcc_get_ext_csd(sdcc_pdev, ext_csd_raw_data);
   if (rc != SDCC_NO_ERROR)
   {
      return rc;
   }

   /* make sure power-on write protect is not disabled */
   if (SDCC_EXTRACT_BIT_VAL_FROM_BYTE_REG(
       ext_csd_raw_data[SDCC_EMMC_EXT_CSD_SLICE_BOOT_WP],
       SDCC_EMMC_BIT_BOOT_PWR_WP_DIS, 1))
   {
      /* power-on period write protection for boot area is disabled */
      DEBUG ((EFI_D_WARN, "Use of boot power-on WP has been disabled"));
      return SDCC_ERR_PWR_ON_WRITE_PROT;
   }
   /* make sure permanent write protect is not enabled */
   /* setting both B_PERM_WP_EN and B_PWR_WP_EN will result in the boot area */
   /* being permanently protected */
   if (SDCC_EXTRACT_BIT_VAL_FROM_BYTE_REG(
       ext_csd_raw_data[SDCC_EMMC_EXT_CSD_SLICE_BOOT_WP],
       SDCC_EMMC_BIT_BOOT_PERM_WP_EN, 1))
   {
      /* permanent write protection for boot area is enabled */
      DEBUG ((EFI_D_ERROR,"Boot permanent WP has been enabled"));
      return SDCC_ERR_PWR_ON_WRITE_PROT;
   }

   /* check if power-on write protection has already been enabled */
   if (SDCC_EXTRACT_BIT_VAL_FROM_BYTE_REG(
       ext_csd_raw_data[SDCC_EMMC_EXT_CSD_SLICE_BOOT_WP],
       SDCC_EMMC_BIT_BOOT_PWR_WP_EN, 1))
   {
      /* power-on write protection has already been enabled */
      /* sending CMD6 is not needed */
      return SDCC_NO_ERROR;
   }

   /* enable power-on period write protection for the boot area */
   rc = sdcc_mmc_switch(sdcc_pdev, SDCC_EMMC_ENABLE_BOOT_PWR_WP);
   if (rc != SDCC_NO_ERROR)
   {
      DEBUG ((EFI_D_ERROR, "Failed in enabling boot power-on write protect %d", rc));
   }

   return rc;
} /* sdcc_emmc_set_boot_pwr_on_wp */

/******************************************************************************
* Name: sdcc_emmc_get_user_prot_type
*
* Description:
*    This function gets the write protection type for the write protection
*    groups that belong to the User area.  The 8-byte pointer (64 write
*    protection bits) represent 32 write protect groups starting at the
*    specified data_address.
*    CMD31 SEND_WR_PROTECT_TYPE is sent to the eMMC device with the data_address
*    as an argument. Then a Read is performed to retrieve the write protection
*    type bits that have been applied to write protection groups 
*
* Parameters:
*    handle [IN]           : a pointer to the SDCC device that was returned from
*                            sdcc_handle_open()
*    data_address  [IN]    : start of the sector to get write protection type
* 
*    write_prot_type [OUT] : A pointer to 8-byte array. This array contains 64 write
*                            protection bits (representing 32 write protect
*                            groups starting at the specified data_address)
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
static
SDCC_STATUS sdcc_emmc_get_user_prot_type
(
   struct sdcc_device *handle,
   UINT32 data_address,
   UINT8 *write_prot_type
)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_dev_type *sdcc_pdev = NULL;
   sdcc_cmd_type sdcc_cmd;
   UINT32 mode_temp;

   /* get the device info specified by the current interface */
   sdcc_pdev = handle->hdev;
       
   /* CMD31 - SEND_WRITE_PROT_TYPE */
   sdcc_cmd.cmd       = SD_CMD31;
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.flags = SDCC_TRANSFER_FLAG_IS_READ_TRANSFER;

   if ( SDCC_CARD_SDHC == sdcc_pdev->card_type ||
        SDCC_CARD_MMCHC == sdcc_pdev->card_type )
   {
      /* in high capacity cards, the 32-bit argument of memory access
         commands uses the memory address in block address format */
      sdcc_cmd.cmd_arg = data_address;
   }
   else
   {
      /* in standard capacity cards, the 32-bit argument of memory access
         commands uses the memory address in byte address format */
      sdcc_cmd.cmd_arg = data_address * (sdcc_pdev->mem.mem_info.block_len);   
   }

   mode_temp = sdcc_pdev->mem.block_mode;
   sdcc_pdev->mem.block_mode = SDCC_MEM_BYTE_MODE;

   /* Send CMD31 and read 8 bytes (64 bits representing 32 protection groups) */
   rc = sdcc_do_transfer(sdcc_pdev, &sdcc_cmd, (UINT8 *)write_prot_type, 8);

   /* Work around for San Disk SDIN5C2 and SDIN5C4 parts. The issue happens */
   /* when two CMD31 commands are sent back to back The first CMD31 always */
   /* succeeds. The second CMD31 that comes immediately would cause the */
   /* mentioned San Disk parts to stop responding to the host controller. */
   /* A delay of 5us would work around the issue */
   sdcc_udelay(5);

   sdcc_pdev->mem.block_mode = mode_temp;

   return rc;
}

/******************************************************************************
* Name: sdcc_emmc_get_boot_prot_type
*
* Description:
*    This function gets the write protection type of the Boot partitions.
*    The 2 least significant bits (bits[1:0] of the write_prot_type[7]) 
*    represent the write protection type applied to both the BOOT 1 and BOOT 2
*    partitions.
*
* Parameters:
*    handle [IN]           : a pointer to the SDCC device that was returned from
*                            sdcc_handle_open()
* 
*    write_prot_type [OUT] : A pointer to 8-byte array. The 2 least significant
*                            bits (bits[1:0] of the write_prot_type[7])
*                            represent the write protection type for the BOOT 1
*                            and BOOT 2 partitions
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
static
SDCC_STATUS sdcc_emmc_get_boot_prot_type
(
   struct sdcc_device *handle,
   UINT8 *write_prot_type
)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_dev_type *sdcc_pdev = NULL;

   /* get the device info specified by the current interface */
   sdcc_pdev = handle->hdev;

   SetMem(ext_csd_raw_data, sizeof(ext_csd_raw_data), 0);

   /* read the extended CSD register */
   rc = sdcc_get_ext_csd(sdcc_pdev, ext_csd_raw_data);

   if (rc != SDCC_NO_ERROR)
   {
      return rc;
   }

   /* make sure permanent write protect is not enabled
      setting both B_PERM_WP_EN and B_PWR_WP_EN will result in the boot area
      being permanently protected */
   if (SDCC_EXTRACT_BIT_VAL_FROM_BYTE_REG(
       ext_csd_raw_data[SDCC_EMMC_EXT_CSD_SLICE_BOOT_WP],
       SDCC_EMMC_BIT_BOOT_PERM_WP_EN, 1))
   {
      /* permanent write protection for boot area is enabled */
      *(write_prot_type + 7) = SDCC_EMMC_WP_PERM_PROTECTION; 
   }
   /* check if power-on write protection has already been enabled */
   else if (SDCC_EXTRACT_BIT_VAL_FROM_BYTE_REG(
       ext_csd_raw_data[SDCC_EMMC_EXT_CSD_SLICE_BOOT_WP],
       SDCC_EMMC_BIT_BOOT_PWR_WP_EN, 1))
   {
      /* power-on write protection for boot area is enabled */
      *(write_prot_type + 7) = SDCC_EMMC_WP_POWER_ON_PROTECTION;
   }
   else
   {
      *(write_prot_type + 7) = SDCC_EMMC_WP_NO_PROTECTION;
   }

   return SDCC_NO_ERROR;;
}

/******************************************************************************
* Name: sdcc_find_mmc_device
*
* Description: 
*    This function looks for the MMC device of the specified driveno.
*
* Parameters:
*    handle [IN] : a pointer to the SDCC device 
* 
* Returns:
*    card type that indicates if device is MMC or unknown
*
******************************************************************************/
SDCC_CARD_TYPE
sdcc_find_mmc_device ( struct sdcc_device *handle )
{
   SDCC_CARD_TYPE card_type = SDCC_CARD_UNKNOWN;
   sdcc_dev_type *sdcc_pdev = handle->hdev;
   SDCC_STATUS rc = SDCC_ERR_UNKNOWN;
   sdcc_cmd_type sdcc_cmd;
   UINT32 i;

   /* CMD0: reset card first */
   sdcc_cmd.cmd       = SD_CMD0_GO_IDLE_STATE;
   sdcc_cmd.cmd_arg   = SDCC_CMD_ARGU_NULL;
   sdcc_cmd.resp_type = SDCC_RESP_NONE;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   (void) sdcc_command (sdcc_pdev, &sdcc_cmd);

   /* continue on to look for MMC card */
   for ( i = 0; i < SDCC_MMC_DETECT_MAX_RETRIES; i++ )
   {
      /* CMD1: sent voltage to be used */
      sdcc_cmd.cmd       = SD_CMD1_SEND_OP_COND;
      sdcc_cmd.cmd_arg   = SDCC_MMC_HCS_NEGOTIATE_OCR;
      sdcc_cmd.resp_type = SDCC_RESP_SHORT;
      sdcc_cmd.prog_scan = 0;
      sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

      rc = sdcc_command( sdcc_pdev, &sdcc_cmd );

      if ( SDCC_NO_ERROR != rc )
      {
         /* exit the for-loop for MMC card detection if there is issue in */
         /* sending the command to the card */
         break;
      }
      else if ( sdcc_cmd.resp[0] & SDCC_DEVICE_RDY )
      {
         /* access mode validation: */
         /* byte access mode represents <= 2GB card size */
         /* sector access mode represents > 2GB card size, however */
         /* this needs to be reconfirmed by reading SEC_COUNT */
         /* in EXT_CSD */
         /* >2GB: [30:29] = 1,0 */
         if ( ((sdcc_cmd.resp[0] >> 30) & 1) &&
               !((sdcc_cmd.resp[0] >> 29) & 1) )
         {
            card_type = SDCC_CARD_MMCHC;
            DEBUG ((EFI_D_INFO, "Found MMCHC card after %dth retry\n", i));
         }
         else
         {
            card_type = SDCC_CARD_MMC;
            DEBUG ((EFI_D_INFO, "Found MMC card after %dth retry\n", i));
         }
         return card_type;
      }

      sdcc_mdelay( SDCC_MMC_DETECT_BUSY_RETRY_INTERVAL );
   }

   return card_type;
} /* sdcc_find_mmc_device */

/******************************************************************************
* Name: sdcc_config_buswidth
*
* Description:
*    This function configures the MMC bus width.
*
* Arguments:
*    sdcc_pdev        [IN] : pointer to device type
*    DDRSupport       [IN] : DDR buswidth or not
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
static SDCC_STATUS sdcc_config_buswidth (sdcc_dev_type *sdcc_pdev, BOOLEAN DDRSupport)
{
   SDCC_MMC_BUSWIDTH bus_width;
   SDCC_STATUS rc = SDCC_NO_ERROR;
   UINT32 driveno = 0;

   if (NULL == sdcc_pdev)
   {
      return SDCC_ERR_INVALID_PARAM;
   }

   driveno = sdcc_pdev->driveno;

   if (sdcc_bsp_get_slot_type(driveno) & SDCC_BSP_8BIT_SLOT)
   {
      bus_width = (TRUE == DDRSupport) ? SDCC_MMC_BUSWIDTH_DDR_8BIT : SDCC_MMC_BUSWIDTH_8BIT;
   }
   else
   {
      bus_width = (TRUE == DDRSupport) ? SDCC_MMC_BUSWIDTH_DDR_4BIT : SDCC_MMC_BUSWIDTH_4BIT;
   }

   /* configure MMC BUS_WIDTH on card and Host */
   rc = sdcc_mmc_switch(sdcc_pdev, SDCC_MMCPLUS_CHANGE_BUSWIDTH |
                        ((UINT32)(bus_width << 8)));
   if (rc != SDCC_NO_ERROR)
   {
      DEBUG ((EFI_D_INFO, "SDCC failed in changing bus width %d", rc));
      return rc;
   }

   rc = sdcc_hc_config_buswidth (driveno, bus_width);
   if (rc != SDCC_NO_ERROR)
   {
      DEBUG ((EFI_D_INFO, "SDCC failed to configure bus width %d in SDHCi mode", rc));
      return rc;
   }

   return SDCC_NO_ERROR; 
} /* sdcc_config_buswidth */

/******************************************************************************
* Name: sdcc_emmc_dll_mclk_freq_set
*
* Description:
*    This function is to configure the MCLK of DLL_CONFIG reg
*
* Arguments:
*    sdcc_pdev        [IN] : pointer to device type
*
* Returns:
*    None
*
* Note: For clocks greater than 200MHz, set mclk_freq to 7.
*
******************************************************************************/
static inline void
sdcc_emmc_dll_mclk_freq_set(sdcc_dev_type *sdcc_pdev)
{
   UINT32 mclk_freq = 0;

   if (sdcc_pdev->clk_freq_in_khz <= 112000)
      mclk_freq = MCLK_100_112;
   else if (sdcc_pdev->clk_freq_in_khz <= 125000)
      mclk_freq = MCLK_112_125;
   else if (sdcc_pdev->clk_freq_in_khz <= 137000)
      mclk_freq = MCLK_125_137;
   else if (sdcc_pdev->clk_freq_in_khz <= 150000)
      mclk_freq = MCLK_137_150;
   else if (sdcc_pdev->clk_freq_in_khz <= 162000)
      mclk_freq = MCLK_150_162;
   else if (sdcc_pdev->clk_freq_in_khz <= 175000)
      mclk_freq = MCLK_162_175;
   else if (sdcc_pdev->clk_freq_in_khz <= 187000)
      mclk_freq = MCLK_175_187;
   else if (sdcc_pdev->clk_freq_in_khz <= 200000)
      mclk_freq = MCLK_187_200;
   else
      mclk_freq = MCLK_187_200;

   /* Program the MCLK value to MCLK_FREQ bit field */
   HAL_sdhci_DllMclkSet(sdcc_pdev->driveno, mclk_freq);

} /* sdcc_emmc_dll_mclk_freq_set */

/******************************************************************************
* Name: sdcc_emmc_init_cm_dll_sdc4
*
* Description:
*    This function initializes cm_dll_sdc4 as part of tuning process
*
* Arguments:
*    sdcc_pdev        [IN] : pointer to device type
*
* Returns:
*    Status
*
******************************************************************************/
static inline SDCC_STATUS
sdcc_emmc_init_cm_dll_sdc4 (sdcc_dev_type *sdcc_pdev)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   UINT32 driveno = sdcc_pdev->driveno;

   HAL_sdhci_PowerSave( driveno, FALSE);
   HAL_sdhci_DllRstPdn(driveno, TRUE);
   sdcc_emmc_dll_mclk_freq_set(sdcc_pdev);
   HAL_sdhci_DllRstPdn(driveno, FALSE);
   HAL_sdhci_DllEnable(driveno);
   HAL_sdhci_DllCkOutEnable(driveno);
   HAL_sdhci_DLLClockEnable(driveno, TRUE);
   rc = HAL_sdhci_DllPollStatusLock(driveno);
   HAL_sdhci_PowerSave(driveno, TRUE);

   return rc;
} /* sdcc_emmc_init_cm_dll_sdc4 */

/******************************************************************************
* Name: sdcc_hs200_execute_tuning
*
* Description:
*    This function executes tuning sequence to select the best phase
*    for enabling HS200 mode.
*
* Arguments:
*    sdcc_pdev        [IN/OUT] : pointer to device type
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS 
sdcc_hs200_execute_tuning( sdcc_dev_type *sdcc_pdev )
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   UINT32 driveno = sdcc_pdev->driveno;
   sdcc_cmd_type sdcc_cmd;
   UINT8 phase = 0, tuned_phase_cnt = 0;
   BOOLEAN is_tuning_all_phases = FALSE;
   UINT8 *tuning_buff = gTempBuffer;

   /* Tuning pattern array */
   static const UINT32 tuning_block_64[] = {
      0x00FF0FFF, 0xCCC3CCFF, 0xFFCC3CC3, 0xEFFEFFFE,
      0xDDFFDFFF, 0xFBFFFBFF, 0xFF7FFFBF, 0xEFBDF777,
      0xF0FFF0FF, 0x3CCCFC0F, 0xCFCC33CC, 0xEEFFEFFF,
      0xFDFFFDFF, 0xFFBFFFDF, 0xFFF7FFBB, 0xDE7B7FF7
   };

   static const UINT32 tuning_block_128[] = {
      0xFF00FFFF, 0x0000FFFF, 0xCCCCFFFF, 0xCCCC33CC,
      0xCC3333CC, 0xFFFFCCCC, 0xFFFFEEFF, 0xFFEEEEFF,
      0xFFDDFFFF, 0xDDDDFFFF, 0xBBFFFFFF, 0xBBFFFFFF,
      0xFFFFFFBB, 0xFFFFFF77, 0x77FF7777, 0xFFEEDDBB,
      0x00FFFFFF, 0x00FFFFFF, 0xCCFFFF00, 0xCC33CCCC,
      0x3333CCCC, 0xFFCCCCCC, 0xFFEEFFFF, 0xEEEEFFFF,
      0xDDFFFFFF, 0xDDFFFFFF, 0xFFFFFFDD, 0xFFFFFFBB,
      0xFFFFBBBB, 0xFFFF77FF, 0xFF7777FF, 0xEEDDBB77
   };

   const UINT32 *tuning_block_pattern = tuning_block_64;
   
   /* set tuning buff size to 64 byte to use tuning_block_64*/
   UINT32 size = 64;

   /* Check if the device is HS200/400 supported MMC or not */
   if ((SDCC_DATA_TRANSFER_HS200_MODE != sdcc_pdev->speed_mode) &&
       (SDCC_DATA_TRANSFER_HS400_MODE != sdcc_pdev->speed_mode))
   {
      return SDCC_ERR_FEATURE_UNSUPPORTED;
   }

   if(!sdcc_pdev->tuning_needed)
   {
      return SDCC_ERR_UNKNOWN;
   }

   if (sdcc_bsp_get_slot_type(driveno) & SDCC_BSP_8BIT_SLOT)
   {
      tuning_block_pattern = tuning_block_128;
      size = 128;
   }
   
   SetMem(tuning_buff, size, 0);

   sdcc_pdev->tuning_in_progress = TRUE;

   /* Reset the tuning block  */
   rc = sdcc_emmc_init_cm_dll_sdc4(sdcc_pdev);
   if (rc != SDCC_NO_ERROR)
   {
      DEBUG ((EFI_D_ERROR,"Error reset tuning block:error code %d\n", rc));
      goto ErrorExit;
   }

   if (sdcc_pdev->selected_phase == HS200_INVALID_TUNING_PHASE)
   {
      is_tuning_all_phases = TRUE;
   }

   if (is_tuning_all_phases)
   {
      phase = 0;
   }
   else
   {
      phase = sdcc_pdev->selected_phase;
   }
   do
   {
      /* Note: CDR Enable needs to be set as part of CMD21. It will
         be done in sdcc_command() */

      /* Send tuning block cmd */
      sdcc_cmd.cmd       = SD_CMD21;
      sdcc_cmd.cmd_arg   = SDCC_CMD_ARGU_NULL;
      sdcc_cmd.resp_type = SDCC_RESP_SHORT;
      sdcc_cmd.prog_scan = 0;
      sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_READ_TRANSFER;

      sdcc_pdev->mem.block_mode = SDCC_MEM_BYTE_MODE;
      rc = sdcc_do_transfer(sdcc_pdev, &sdcc_cmd, tuning_buff, size);
      sdcc_pdev->mem.block_mode = SDCC_MEM_BLK_MODE;

      if (rc != SDCC_NO_ERROR)
      {
         if ((sdcc_cmd.status & SDHCI_STATUS_DATA_TIMEOUT_ERR_BMSK) || 
             (sdcc_cmd.status & SDHCI_STATUS_DATA_CRC_ERR_BMSK) || 
             (sdcc_cmd.status & SDHCI_STATUS_DATA_END_BIT_ERR_BMSK) || 
             (sdcc_cmd.status & SDHCI_STATUS_CMD_CRC_ERR_BMSK) || 
             (sdcc_cmd.status & SDHCI_STATUS_CMD_TIMEOUT_ERR_BMSK) || 
             (sdcc_cmd.status & SDHCI_STATUS_CMD_END_BIT_ERR_BMSK) || 
             (sdcc_cmd.status & SDHCI_STATUS_CMD_INDX_ERR_BMSK))
         {
            /* in case of crc/timeout/end-bit/indx errors at a phase, continue with next phase */
            DEBUG ((EFI_D_INFO, "Ignoring CRC/TimeOut error @ phase %d in tuning process\n", phase));
            sdcc_mdelay(2);
            continue;
         }
         else
         {
            goto ErrorExit; 
         }
      }

      /* Tuning block is sent by card as read data */
      if (!CompareMem(tuning_buff, (void*)tuning_block_pattern, size))
      {
         /* tuning is successful at this phase, if not tuning for all
          * phase Exit gracefully */
         tuned_phase_cnt++;
         DEBUG ((EFI_D_INFO, "Found good phase = %d\n", phase));
         // Exit immidiately after a good phase
         rc = SDCC_NO_ERROR;
         goto ErrorExit;
      }
   } while (++phase < MAX_TUNING_PHASES);

ErrorExit:
   sdcc_pdev->tuning_in_progress = FALSE; 
   if (SDCC_NO_ERROR == rc)
   {
      sdcc_pdev->tuning_done = TRUE;
   }

   return rc;
} /* sdcc_hs200_execute_tuning */

/******************************************************************************
* Name: sdcc_configure_dll_hs400
*
* Description: 
*    This function configures the DLL for HS400 use
*
* Parameters:
*    sdcc_pdev [IN] : pointer to the device type
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
static SDCC_STATUS sdcc_configure_dll_hs400(sdcc_dev_type *sdcc_pdev)
{
   UINT32 timeout = 50;    
   UINT32 driveno = 0;
   UINT8  rclk_delay = 115;
   
   if ((NULL == sdcc_pdev) || (SDCC_DATA_TRANSFER_HS400_MODE != sdcc_pdev->speed_mode))
   {
      return SDCC_ERR_INVALID_PARAM; 
   }

   driveno = sdcc_pdev->driveno;

   /* Re-initialize and restore the initial DLL phase */
   if (SDCC_NO_ERROR != sdcc_emmc_init_cm_dll_sdc4 (sdcc_pdev))
   {
      DEBUG ((EFI_D_INFO, "SDCC HS400 failed to initialize DLL"));
      return SDCC_ERR_UNKNOWN;
   }

   if (HAL_sdhci_ConfigCmDllPhase(driveno, sdcc_pdev->selected_phase))
   {
      DEBUG ((EFI_D_INFO, "SDCC HS400 failed to restore DLL configuration"));
      return SDCC_ERR_UNKNOWN; 
   }

   /* Enable CMD line tracking in SDCDC */
   HAL_sdhci_EnableCmdLineTracking (driveno);

   /* Configure DDR */
   HAL_sdhci_ConfigDDR(driveno, rclk_delay); 

   /* Write 1 to DDR_CAL_EN */
   HAL_sdhci_CalibrateDDR (driveno, TRUE);
   
   /* Poll on DDR_DLL_LOCK_JDR bit to be 1 (up to 50ms) */
   while (FALSE == HAL_sdhci_DDRDllLock (driveno))  
   {
      timeout--; 
      if (!timeout)
      {
         DEBUG ((EFI_D_INFO, "SDCC HS400 DDR DLL Lock not set"));
         return SDCC_ERR_TIMEOUT;
      }
      sdcc_mdelay(1); 
   }

   /* Unset PWRSAVE_DLL */
   HAL_sdhci_PowerSaveDll (driveno, FALSE); 

   return SDCC_NO_ERROR; 
}

/******************************************************************************
* Name: sdcc_enter_hs_mode
*
* Description:
*    This function performs the necessary steps to allow the card to operate
*    in HS mode. 
*
* Arguments:
*    sdcc_pdev    [IN] : pointer to device type
*
* Returns:
*    Returns error code
*
******************************************************************************/
static SDCC_STATUS sdcc_enter_hs_mode (sdcc_dev_type *sdcc_pdev)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;

   if (NULL == sdcc_pdev)
   {
      return SDCC_ERR_INVALID_PARAM; 
   }

   /* enable HS_TIMING */
   rc = sdcc_mmc_switch(sdcc_pdev, SDCC_MMCPLUS_ENABLE_HS_TIMING | 
                        ((UINT32)SDCC_MMCPLUS_HIGH_SPEED));
   if (rc != SDCC_NO_ERROR)
   {
      DEBUG ((EFI_D_INFO, "Failed to enable HS_TIMING %d", rc));
      return rc;
   }

   /* re-program the clock according to the MMC card type */
   sdcc_config_clk(sdcc_pdev, SDCC_DATA_TRANSFER_MODE);

   return SDCC_NO_ERROR; 
}

/******************************************************************************
* Name: sdcc_enter_hs200_mode
*
* Description:
*    This function performs the necessary steps to allow the card to operate
*    in HS200 mode. 
*
* Arguments:
*    sdcc_pdev    [IN/OUT] : pointer to device type
*
* Returns:
*    Returns error code
*
******************************************************************************/
static SDCC_STATUS sdcc_enter_hs200_mode (sdcc_dev_type *sdcc_pdev)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;

   if ((NULL == sdcc_pdev) ||
       ((SDCC_DATA_TRANSFER_HS200_MODE != sdcc_pdev->speed_mode) && (SDCC_DATA_TRANSFER_HS400_MODE != sdcc_pdev->speed_mode)))
   {
      return SDCC_ERR_INVALID_PARAM; 
   }

   /* Switch to HS200 Mode */
   rc = sdcc_mmc_switch(sdcc_pdev, SDCC_MMCPLUS_ENABLE_HS_TIMING |
                        ((UINT32)SDCC_MMCPLUS_HIGH_HS200));
   if (rc != SDCC_NO_ERROR)
   {
      DEBUG ((EFI_D_INFO, "Failed to enable HS200 %d", rc));
      return rc;
   }

   HAL_sdhci_EnableSDClk(sdcc_pdev->driveno, FALSE);

   if (SDCC_DATA_TRANSFER_HS400_MODE == sdcc_pdev->speed_mode)
   {
      /* Write 3 to SDC4_MCLK_SEL (free running MCLK / 2) */
      HAL_sdhci_Sdc4MclkSelect (sdcc_pdev->driveno, SDCC_FREE_RUNNING_MCLK_DIV_2); 

      /* Set clock to 384 MHz */
      sdcc_pdev->mem.clk_freq = SDCC_MMC_FREQUENCY_400MHZ; 
   }
   else if (SDCC_DATA_TRANSFER_HS200_MODE == sdcc_pdev->speed_mode)
   {
      /* Set clock to 200 MHz */
      sdcc_pdev->mem.clk_freq = SDCC_MMC_FREQUENCY_200MHZ;
   }
   
   sdcc_pdev->tuning_needed = TRUE;

   /* Set UHS Mode Select field = SDHCI_SDR104_MODE, Select In = 4 */
   HAL_sdhci_SetUHSMode(sdcc_pdev->driveno, (UINT16)SDHCI_SDR104_MODE); 
   HAL_sdhci_HCSelectIn(sdcc_pdev->driveno, (UINT32)HAL_SDCC_CLOCK_UHS_MCLK); 

   sdcc_config_clk (sdcc_pdev, sdcc_pdev->speed_mode); 

   /* Run the HS200 tuning sequence */
   rc = sdcc_hs200_execute_tuning(sdcc_pdev);
   if (rc != SDCC_NO_ERROR)
   {
      DEBUG ((EFI_D_INFO, "Failed HS200 tuning sequence %d", rc));
      return rc;
   }

   return SDCC_NO_ERROR; 
}

/******************************************************************************
* Name: sdcc_enter_hs400_mode
*
* Description:
*    This function performs the necessary steps to allow the card to operate
*    in HS400 mode. 
*
* Arguments:
*    sdcc_pdev    [IN/OUT] : pointer to device type
*
* Returns:
*    SDCC_NO_ERROR if successful. Error code otherwise.
*
******************************************************************************/
static SDCC_STATUS sdcc_enter_hs400_mode (sdcc_dev_type *sdcc_pdev)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;

   if ((NULL == sdcc_pdev) || (SDCC_DATA_TRANSFER_HS400_MODE != sdcc_pdev->speed_mode))
   {
      return SDCC_ERR_INVALID_PARAM; 
   }

   rc = sdcc_enter_hs200_mode(sdcc_pdev); 
   if (rc != SDCC_NO_ERROR)
   {
      DEBUG ((EFI_D_INFO, "Failed to switch to HS200 mode %d", rc));
      return rc;
   }

   /* Disable the DLL */
   HAL_sdhci_DisableDLLClock (sdcc_pdev->driveno); 

   /* Lower the clock to 25 MHz (clock on bus will be 12.5 MHz) */
   sdcc_hc_set_uhs_mode_select(sdcc_pdev, SDHCI_SDR12_MODE); 

   /* Switch to Highspeed Mode */
   rc = sdcc_mmc_switch(sdcc_pdev, SDCC_MMCPLUS_ENABLE_HS_TIMING | 
                        ((UINT32)SDCC_MMCPLUS_HIGH_SPEED));
   if (rc != SDCC_NO_ERROR)
   {
      DEBUG ((EFI_D_INFO, "Failed to enable HS_TIMING %d", rc));
      return rc;
   }

   /* Switch to 8-bit DDR buswidth */
   rc = sdcc_mmc_switch(sdcc_pdev, SDCC_MMCPLUS_CHANGE_BUSWIDTH | 
                        ((UINT32)(SDCC_MMC_BUSWIDTH_DDR_8BIT << 8)));
   if (rc != SDCC_NO_ERROR)
   {
      DEBUG ((EFI_D_INFO, "Failed to change to 8-bit DDR bus width %d", rc));
      return rc;
   }

   /* Switch from HS200 to HS400 */
   rc = sdcc_mmc_switch(sdcc_pdev, SDCC_MMCPLUS_ENABLE_HS_TIMING | 
                        ((UINT32)SDCC_MMCPLUS_HIGH_HS400));
   if (rc != SDCC_NO_ERROR)
   {
      DEBUG ((EFI_D_INFO, "Failed to switch to HS400, %d", rc));
      return rc;
   }

   /* Change Select In to switch SDCC5 to HS400 mode */
   HAL_sdhci_HCSelectIn(sdcc_pdev->driveno, HAL_SDCC_CLOCK_DDR200); 

   /* Restore clock to 384 MHz */
   sdcc_pdev->mem.clk_freq = SDCC_MMC_FREQUENCY_400MHZ; 
   sdcc_pdev->tuning_needed = TRUE;
   sdcc_config_clk(sdcc_pdev, SDCC_DATA_TRANSFER_HS400_MODE); 

   /* Initialize and calibrate DLL for HS400 */
   rc = sdcc_configure_dll_hs400(sdcc_pdev); 
   if (rc != SDCC_NO_ERROR)
   {
      DEBUG ((EFI_D_INFO, "Failed to configure DLL for HS400, %d", rc));
      return rc;
   }

   return SDCC_NO_ERROR; 
}

/******************************************************************************
* Name: sdcc_enter_hs400_mode_with_es
*
* Description:
*    This function performs the necessary steps to allow the card to operate
*    in HS400 Enhance strobe mode. 
*
* Arguments:
*    sdcc_pdev    [IN/OUT] : pointer to device type
*
* Returns:
*    SDCC_NO_ERROR if successful. Error code otherwise.
*
******************************************************************************/
static SDCC_STATUS sdcc_enter_hs400_mode_with_es (sdcc_dev_type *sdcc_pdev)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   UINT32 timeout = 50;    
   UINT32 driveno = 0;
   UINT8  rclk_delay = 104;
   
   if ((NULL == sdcc_pdev) || (SDCC_DATA_TRANSFER_HS400_MODE != sdcc_pdev->speed_mode))
   {
      return SDCC_ERR_INVALID_PARAM; 
   }

   driveno = sdcc_pdev->driveno;

   /* Switch to HS200 Mode */
   rc = sdcc_mmc_switch(sdcc_pdev, SDCC_MMCPLUS_ENABLE_HS_TIMING |
                        ((UINT32)SDCC_MMCPLUS_HIGH_HS200));
   if (rc != SDCC_NO_ERROR)
   {
      DEBUG ((EFI_D_INFO, "Failed to enable HS200 %d", rc));
      return rc;
   }

   /* Lower the clock to 25 MHz (clock on bus will be 12.5 MHz) */
   sdcc_hc_set_uhs_mode_select(sdcc_pdev, SDHCI_SDR12_MODE); 

   /* Switch to Highspeed Mode */
   rc = sdcc_mmc_switch(sdcc_pdev, SDCC_MMCPLUS_ENABLE_HS_TIMING | 
                        ((UINT32)SDCC_MMCPLUS_HIGH_SPEED));
   if (rc != SDCC_NO_ERROR)
   {
      DEBUG ((EFI_D_INFO, "Failed to enable HS_TIMING %d", rc));
      return rc;
   }

   /* Switch to 8-bit DDR buswidth */
   rc = sdcc_mmc_switch(sdcc_pdev, SDCC_MMCPLUS_CHANGE_BUSWIDTH | 
                        ((UINT32)((0x80|SDCC_MMC_BUSWIDTH_DDR_8BIT) << 8)));
   if (rc != SDCC_NO_ERROR)
   {
      DEBUG ((EFI_D_INFO, "Failed to change to 8-bit DDR bus width %d", rc));
      return rc;
   }

   /* Switch from HS200 to HS400 */
   rc = sdcc_mmc_switch(sdcc_pdev, SDCC_MMCPLUS_ENABLE_HS_TIMING | 
                        ((UINT32)SDCC_MMCPLUS_HIGH_HS400));
   if (rc != SDCC_NO_ERROR)
   {
      DEBUG ((EFI_D_INFO, "Failed to switch to HS400, %d", rc));
      return rc;
   }

   /* Change Select In to switch SDCC5 to HS400 mode */
   HAL_sdhci_HCSelectIn(driveno, HAL_SDCC_CLOCK_DDR200); 

   /* Restore clock to 384 MHz */
   sdcc_pdev->mem.clk_freq = SDCC_MMC_FREQUENCY_400MHZ; 
   sdcc_pdev->tuning_needed = TRUE;
   sdcc_config_clk(sdcc_pdev, SDCC_DATA_TRANSFER_HS400_MODE); 

   /*Initialize the DLL */
   HAL_sdhci_PowerSave( driveno, FALSE);
   HAL_sdhci_DllRstPdn(driveno, TRUE);
   sdcc_emmc_dll_mclk_freq_set(sdcc_pdev);
   HAL_sdhci_DllRstPdn(driveno, FALSE);
   HAL_sdhci_DllEnable(driveno);
   HAL_sdhci_DllCkOutEnable(driveno);
   HAL_sdhci_DLLClockEnable(driveno, TRUE);

   /* Configure DDR */
   HAL_sdhci_ConfigDDR(driveno, rclk_delay); 

   /* Enable CMDIN RCLK in SDCDC */
   HAL_sdhci_EnableCmdInRclk (driveno);

   /* Write 1 to DDR_CAL_EN */
   HAL_sdhci_CalibrateDDR (driveno, TRUE);
   
   /* Poll on DDR_DLL_LOCK_JDR bit to be 1 (up to 50ms) */
   while (FALSE == HAL_sdhci_DDRDllLock (driveno))  
   {
      timeout--; 
      if (!timeout)
      {
         DEBUG ((EFI_D_INFO, "SDCC HS400 DDR DLL Lock not set"));
         return SDCC_ERR_TIMEOUT;
      }
      sdcc_mdelay(1); 
   }

   /* Unset PWRSAVE_DLL */
   HAL_sdhci_PowerSaveDll (driveno, FALSE); 
  
   return SDCC_NO_ERROR; 
}

/******************************************************************************
* Name: sdcc_config_mmc_modes_segment
*
* Description:
*    This function programs the modes segment in EXT_CSD register of the
*    MMC/eMMC device.  It gets the EXT_CSD information and sends CMD6 SWITCH
*    command to configure the HS_TIMING and BUS_WIDTH.  It also calculates the
*    card size for the device that is higher than 2GB, boot partition size and
*    write protect group size.
*
* Arguments:
*    sdcc_pdev    [IN/OUT] : pointer to device type
*
* Returns:
*    Returns error code
*
* Note:
*    Card is in data transfer mode.
*
******************************************************************************/
SDCC_STATUS 
sdcc_config_mmc_modes_segment( sdcc_dev_type *sdcc_pdev )
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   UINT32 driveno = sdcc_pdev->driveno;
   sdcc_ext_csd_type ext_csd;
   UINT8 boot_enabled_val = 0;
   int i; 
   BOOLEAN DDRModeEnable = FALSE; 
   SDCC_SPEED_MODE SpeedMode = PcdGet32(SDCCEmmcSpeedMode);

   /* get the MMC EXT_CSD information */
   SetMem(&ext_csd, sizeof(ext_csd), 0);

   rc = sdcc_get_ext_csd(sdcc_pdev, ext_csd_raw_data);
   if (rc != SDCC_NO_ERROR)
   {
      return rc;
   }

   /* decode ext_csd_raw_data */
   (void) sdcc_decode_extCSD(ext_csd_raw_data, &ext_csd);

   /* sanity check for high density MMC card that the sec_count */
   /* should not be 0 */
   if (sdcc_pdev->card_type == SDCC_CARD_MMCHC && ext_csd.sec_count == 0)
   {
      return SDCC_ERR_UNKNOWN;
   }

   /* Check MMC v4.5 and sanitize feature support */
   if (ext_csd.ext_csd_rev == 6)
   {
      sdcc_pdev->mem.mmcv45_support = TRUE;
      if ( ext_csd.sec_feature_support & (1 << 6))
      {
         sdcc_pdev->mem.sanitize_support = TRUE;
      }
      else
      {
        sdcc_pdev->mem.sanitize_support = FALSE;
      }
   }

   /* Some MMC cards with card size <= 2GB may have invalid non-zero */
   /* SEC_COUNT value.  Ignore the SEC_COUNT if card size <= 2GB. */
   /* Since some MMC cards with card size > 4GB may not comply with */
   /* MMC v4.2, the sdcc_pdev->mem.mem_info.card_size_in_sectors may have */
   /* been populated. */
   /* That is only reconfirm device is MMCHC if */
   /* sdcc_pdev->mem.mem_info.card_size_in_sectors is 0 or > 2GB in */
   /* absolute block length. */
   if (ext_csd.sec_count != 0 &&
       (sdcc_pdev->mem.mem_info.card_size_in_sectors == 0 ||
        sdcc_pdev->mem.mem_info.card_size_in_sectors >
        (UINT32)(SDCC_MMC_2GB_CARD_SIZE / SDCC_DEFAULT_BLK_LENGTH_SIZE)) )
   {
      /* calculate the card size for high density MMC card */
      /* device density is calculated from the EXT_CSD by */
      /* multiplying the value of the sector count by 512B/sector */
      /* for example, 2Tera bytes is (4294967296 x 512B) */
      sdcc_pdev->mem.mem_info.card_size_in_sectors = 
         ((UINT64)ext_csd.sec_count * 512) / SDCC_DEFAULT_BLK_LENGTH_SIZE;
   }

   /* save the number of reliable write sector count supported */
   if (ext_csd.rel_wr_sec_c != 0)
   {
      DEBUG ((EFI_D_INFO, "Reliable write is supported with sector count %d",
              ext_csd.rel_wr_sec_c));
      sdcc_pdev->mem.mem_info.reliable_write_sector_count =
         (UINT32)ext_csd.rel_wr_sec_c;
   }

   /* determine which physical partition is bootable */
   rc = sdcc_emmc_get_boot_partition_enable_val(ext_csd.partition_config,
                                                &boot_enabled_val);
   if (rc != SDCC_NO_ERROR)
   {
      return rc;
   }

   sdcc_pdev->mem.boot_partition_enable_val = boot_enabled_val;

   /* configure MMC BUS_WIDTH on card and Host */
   rc = sdcc_config_buswidth(sdcc_pdev, FALSE); 
   if (rc != SDCC_NO_ERROR)
   {
      DEBUG ((EFI_D_INFO, "Failed to configure bus width %d", rc));
      return rc;
   }

   SpeedMode = 
      (SDCC_SPEED_MODE)sdcc_bsp_get_platform_speed_config((UINT8)SpeedMode);

   /* Read the the speed mode flag */
   if (SDCC_DDR50_MODE == SpeedMode)
   {
      if (sdcc_bsp_get_slot_type(driveno) & SDCC_BSP_DDR50_DISABLED_SLOT)
      {
         DDRModeEnable = FALSE;
      }
      else
      {
         DDRModeEnable = sdcc_pdev->sdhci_ddr_support; 
      }
   }
   
   /* Check the speed mode that the card can support */
   if ((SDCC_HS400_MODE == SpeedMode) && (ext_csd.card_type & SDCC_HS400_MMC_DDR_200MHZ_1_8V))
   {
      sdcc_pdev->speed_mode = SDCC_DATA_TRANSFER_HS400_MODE; 

      if (0 == ext_csd.strobe_support)
      {
        rc = sdcc_enter_hs400_mode(sdcc_pdev);
      }
      else
      {
        rc = sdcc_enter_hs400_mode_with_es(sdcc_pdev);
      }

      if (rc != SDCC_NO_ERROR)
      {
         DEBUG ((EFI_D_INFO, "Failed to switch to HS400 mode %d", rc));
         return rc;
      }
   }
   else if ((SDCC_HS200_MODE == SpeedMode) && (ext_csd.card_type & SDCC_HS200_SDR_200MHZ_1_8V))
   {
      sdcc_pdev->speed_mode = SDCC_DATA_TRANSFER_HS200_MODE; 

      rc = sdcc_enter_hs200_mode(sdcc_pdev); 
      if (rc != SDCC_NO_ERROR)
      {
         DEBUG ((EFI_D_INFO, "Failed to switch to HS200 mode %d", rc));
         return rc;
      }
   }
   else 
   {
      if (DDRModeEnable && (ext_csd.card_type & SDCC_MMC_DDR_52MHZ))
      {
         sdcc_pdev->mem.clk_freq = SDCC_MMC_FREQUENCY_52MHZ;
         sdcc_pdev->speed_mode = SDCC_DATA_TRANSFER_DDR_MODE;

         rc = sdcc_enter_hs_mode(sdcc_pdev); 
         if (rc != SDCC_NO_ERROR)
         {
            DEBUG ((EFI_D_INFO, "Failed to enter HS mode %d", rc));
            return rc;
         }

         rc = sdcc_config_buswidth(sdcc_pdev, TRUE); 
         if (rc != SDCC_NO_ERROR)
         {
            DEBUG ((EFI_D_INFO, "Failed to configure bus width %d", rc));
            return rc;
         }

         /* In Qualcomm interface mode, DDR50 mode doubles the mCLK frequency
          * (the SCLK seen by the memory device is still 50MHz). 
          * In SDHCi mode, the below functions set the SCLK frequency directly.
          * Set the clock to 50MHz */   
          rc = sdcc_hc_set_uhs_mode_select(sdcc_pdev, SDHCI_DDR50_MODE); 
          if (rc != SDCC_NO_ERROR)
          {
             DEBUG ((EFI_D_INFO, "Failed to switch UHS mode %d", rc));
             return rc;
          }
      }
      else
      {
         /* set clock freq to high speed- clk freq based on card type */
         if (ext_csd.card_type & SDCC_MMC_52MHZ)
         {
            /* Cards support 52MHz */
            sdcc_pdev->mem.clk_freq = SDCC_MMC_FREQUENCY_52MHZ;
         }
         else
         {
            /* Cards support 26MHz */
            sdcc_pdev->mem.clk_freq = SDCC_MMC_FREQUENCY_26MHZ;
         }

         rc = sdcc_enter_hs_mode(sdcc_pdev); 
         if (rc != SDCC_NO_ERROR)
         {
            DEBUG ((EFI_D_INFO, "Failed to enter HS mode %d", rc));
            return rc;
         }

         rc = sdcc_config_buswidth(sdcc_pdev, FALSE); 
         if (rc != SDCC_NO_ERROR)
         {
            DEBUG ((EFI_D_INFO, "Failed to configure bus width %d", rc));
            return rc;
         }
      }
   }

#ifndef USE_LOADER_UTILS
   if (sdcc_pdev->mem.write_prot_support == TRUE)
   {
      /* calculate the write protect group size (in bytes) */
      rc = sdcc_emmc_calc_write_prot_grp_size(sdcc_pdev, &ext_csd);
   }

   if (rc == SDCC_NO_ERROR)
   {
      /* calculate the erase group size */
       rc = sdcc_emmc_calc_and_set_hc_erase_grp_size(sdcc_pdev, &ext_csd);
   }
#endif

   /* Decode and parse which physical parition is in use */
   /* First, user partition is always in use */
   sdcc_pdev->mem.partition_inuse_flag = SDCC_BMSK_USER_PARTITION;

   /* update num_phy_partition_created field */
   if (ext_csd.boot_size_multi == 0)
   {
      sdcc_pdev->mem.partition_support = FALSE;
      /* per section 8.4 in JEDEC Standard No. 84-A43 */
      /* no boot partition available or boot mode not supported */
      /* User area is a partition. */
      sdcc_pdev->mem.mem_info.num_phy_partition_created = 1;
   }
   else
   {
      /* boot partition or boot mode is supported */
      sdcc_pdev->mem.partition_support = TRUE;
      sdcc_pdev->mem.mem_info.num_phy_partition_created = 3;
      /* per section 8.4 in JEDEC Standard No. 84-A43 */
      /* boot partition size is calculated as */
      /* boot partition size = 128Kbytes x BOOT_SIZE_MULT */
      sdcc_pdev->mem.partition_boot_size = 128 * ext_csd.boot_size_multi;
      /* Mark the boot partition inuse flag */
      sdcc_pdev->mem.partition_inuse_flag |= SDCC_BMSK_BOOT1_PARTITION |
                                             SDCC_BMSK_BOOT2_PARTITION;
   }

   /* Check to see if the RPMB partition is in use */
   if (ext_csd.rpmb_size_mult != 0) 
   {
      sdcc_pdev->mem.mem_info.num_phy_partition_created++; 
      sdcc_pdev->mem.partition_inuse_flag |= SDCC_BMSK_RPMB_PARTITION;
      sdcc_pdev->mem.mem_info.rpmb_parti_size_in_byte = (UINT32)ext_csd.rpmb_size_mult * 128 * 1024; 
   }

   /* Check to see if any of the GPP partition is in use */
   /* per section 7.4.64 in JEDEC Standard No. 84-B45 */
   /* GPP partition size is calculated as */
   /* GPP partition size = WP_GRP_SIZE x ERASE_GRP_SIZE x GP_SIZE_MULT */

   for (i = 0; i < 4; i++)
   {
      if (ext_csd.gp_size_mult[i] != 0)
      {
         sdcc_pdev->mem.mem_info.num_phy_partition_created++;
         sdcc_pdev->mem.partition_inuse_flag |= (SDCC_BMSK_GP1_PARTITION << i);
         sdcc_pdev->mem.partition_gp_size[i] = ext_csd.gp_size_mult[i] * 
                    sdcc_pdev->mem.mem_info.write_protect_group_size_in_sectors;
      }
   }

   if (ext_csd.rst_n_function != 0)
   {
      sdcc_pdev->mem.mem_info.hw_reset_function = ext_csd.rst_n_function;
   }

   sdcc_pdev->mem.partition_conf_done = ext_csd.partition_setting_completed;

   return rc;
} /* sdcc_config_mmc_modes_segment */

/******************************************************************************
* Name: sdcc_get_ext_csd
*
* Description:
*    This function gets the EXT_CSD register as a block of data from the
*    MMCPlus card.  It performs the adtc command with the read operation.
*
* Arguments:
*    sdcc_pdev              [IN] : pointer to device type
*    buff                  [OUT] : pointer to buffer for the incoming data
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS
sdcc_get_ext_csd
(
   sdcc_dev_type   *sdcc_pdev,
   UINT8                  *buff
)
{
   sdcc_cmd_type   sdcc_cmd;
   SDCC_STATUS     rc          = SDCC_ERR_READ_FIFO;

   sdcc_cmd.cmd       = SD_CMD8_SEND_EXT_CSD;
   sdcc_cmd.cmd_arg   = SDCC_CMD_ARGU_NULL;
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_READ_TRANSFER;

   rc = sdcc_do_transfer(sdcc_pdev, &sdcc_cmd, buff, 1/*block*/);

   if (rc == SDCC_NO_ERROR)
   {
     sdcc_decode_extCSD (buff, &ext_csd_decoded);
   }

   return rc;
} /* sdcc_get_ext_csd */

/******************************************************************************
* Name: sdcc_decode_extCSD
*
* Description:
*    This function is to parse out the Extended CSD data.
*
* Arguments:
*    data          [IN] : pointer to buffer that contains ext_csd data
*    ext_csd      [OUT] : pointer to the structure to save the ext csd data to
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
BOOLEAN
sdcc_decode_extCSD
(
   const UINT8          *data,
   sdcc_ext_csd_type   *ext_csd
)
{
   UINT8 value;
   UINT32 lvalue, i;

   if ( NULL == ext_csd )
   {
      return FALSE;
   }

   /* in data[143:145] */
   /* GP_SIZE_MULT_1: general purpose partition support --> ext_csd[143:145] in wp_grp_size */
   lvalue = *(data + 143);
   lvalue |= *(data + 144) << 8;
   lvalue |= *(data + 145) << 16;
   ext_csd->gp_size_mult[0] = lvalue;

   /* in data[146:148] */
   /* GP_SIZE_MULT_2: general purpose partition support --> ext_csd[146:148] in wp_grp_size */
   lvalue = *(data + 146);
   lvalue |= *(data + 147) << 8;
   lvalue |= *(data + 148) << 16;
   ext_csd->gp_size_mult[1] = lvalue;

   /* in data[149:151] */
   /* GP_SIZE_MULT_3: general purpose partition support --> ext_csd[149:151] in wp_grp_size */
   lvalue = *(data + 149);
   lvalue |= *(data + 150) << 8;
   lvalue |= *(data + 151) << 16;
   ext_csd->gp_size_mult[2] = lvalue;

   /* in data[152:154] */
   /* GP_SIZE_MULT_4: general purpose partition support --> ext_csd[152:154] in wp_grp_size */
   lvalue = *(data + 152);
   lvalue |= *(data + 153) << 8;
   lvalue |= *(data + 154) << 16;
   ext_csd->gp_size_mult[3] = lvalue;

   /* in data[155] */
   value = *(data + 155);
   /* partition_setting_completed: partition_setting_completed --> ext_csd[155] in bytes */
   ext_csd->partition_setting_completed = value;

   /* in data[160] */
   value = *(data + 160);
   /* partitioning_support: partitioning support --> ext_csd[160] in bytes */
   ext_csd->partitioning_support = value;

   /* in data[162] */
   value = *(data + 162);
   /* H/W reset function. Bits[0:1] */
   ext_csd->rst_n_function = value & 0x03;

   /* in data[168] */
   value = *(data + 168);
   /* RPMB size in multiple of 128KB unit */
   ext_csd->rpmb_size_mult = value;

   /* in data[173] */
   value = *(data + 173);
   /* boot_wp: boot area write protection register --> ext_csd[173] in bytes */
   ext_csd->boot_wp = value;

   /* in data[174] */
   value = *(data + 174);
   /* user_wp: user area write protection register --> ext_csd[174] in bytes */
   ext_csd->user_wp = value;

   /* in data[175] */
   value = *(data + 175);
   /* erase_group_def: high-density erase group definition */
   /* --> ext_csd[175] in bytes */
   ext_csd->erase_group_def = value;

   /* in data[179] */
   value = *(data + 179);
   /* partition_config: partition configuration --> ext_csd[179] in bytes */
   ext_csd->partition_config = value;

   /* in data[184] */
   value = *(data + 184);
   /* Enhanced Strobe mode support --> ext_csd[184] in bytes */
   ext_csd->strobe_support = value;
   
   /* in data[189] */
   value = *(data + 189);
   /* cmd_set_rev: command set revision --> ext_csd[189] in bytes */
   ext_csd->cmd_set_rev = value;

   /* in data[191] */
   value = *(data + 191);
   /* cmd_set: command set --> ext_csd[191] in bytes */
   ext_csd->cmd_set = value;

   /* in data[192] */
   value = *(data + 192);
   /* ext_csd_rev: extended CSD revision --> ext_csd[192] in bytes */
   ext_csd->ext_csd_rev = value;

   /* in data[194] */
   value = *(data + 194);
   /* csd_structure: CSD structure version --> ext_csd[194] in bytes */
   ext_csd->csd_structure = value;

   /* in data[196] */
   value = *(data + 196);
   /* card_type: card type --> ext_csd[196] in bytes */
   ext_csd->card_type = value;

   /* in data[200] */
   value = *(data + 200);
   /* pwr_cl_52_195: power class for 52MHz @ 1.95V --> ext_csd[200] in bytes */
   ext_csd->pwr_cl_52_195 = value;

   /* in data[201] */
   value = *(data + 201);
   /* pwr_cl_26_195: power class for 26MHz @ 1.95V --> ext_csd[201] in bytes */
   ext_csd->pwr_cl_26_195 = value;

   /* in data[202] */
   value = *(data + 202);
   /* pwr_cl_52_360: power class for 52MHz @ 3.6V --> ext_csd[202] in bytes */
   ext_csd->pwr_cl_52_360 = value;

   /* in data[203] */
   value = *(data + 203);
   /* pwr_cl_26_360: power class for 26MHz @ 3.6V --> ext_csd[203] in bytes */
   ext_csd->pwr_cl_26_360 = value;

   /* in data[205] */
   value = *(data + 205);
   /* min_perf_r_4_26: minimum read performance for 4bit at 26MHz */
   /* --> ext_csd[205] in bytes */
   ext_csd->min_perf_r_4_26 = value;

   /* in data[206] */
   value = *(data + 206);
   /* min_perf_w_4_26: minimum write performance for 4bit at 26MHz */
   /* --> ext_csd[206] in bytes */
   ext_csd->min_perf_w_4_26 = value;

   /* in data[207] */
   value = *(data + 207);
   /* min_perf_r_8_26_4_52: minimum read performance for 8bit at 26MHz, */
   /* for 4bit at 52MHz --> ext_csd[207] in bytes */
   ext_csd->min_perf_r_8_26_4_52 = value;

   /* in data[208] */
   value = *(data + 208);
   /* min_perf_w_8_26_4_52: minimum write performance for 8bit at 26MHz, */
   /* for 4bit at 52MHz --> ext_csd[208] in bytes */
   ext_csd->min_perf_w_8_26_4_52 = value;

   /* in data[209] */
   value = *(data + 209);
   /* min_perf_r_8_52: minimum read performance for 8bit at 52MHz */
   /* --> ext_csd[209] in bytes */
   ext_csd->min_perf_r_8_52 = value;

   /* in data[210] */
   value = *(data + 210);
   /* min_perf_w_8_52: minimum write performance for 8bit at 52MHz */
   /* --> ext_csd[210] in bytes */
   ext_csd->min_perf_w_8_52 = value;

   /* in data[215:212] */
   value = *(data + 212);
   /* sec_count: sector count --> ext_csd[212] in bytes */
   ext_csd->sec_count = (UINT32)value;
   value = *(data + 213);
   /* sec_count: sector count --> ext_csd[213:212] in bytes */
   ext_csd->sec_count |= (UINT32)value << 8;
   value = *(data + 214);
   /* sec_count: sector count --> ext_csd[214:212] in bytes */
   ext_csd->sec_count |= (UINT32)value << 16;
   value = *(data + 215);
   /* sec_count: sector count --> ext_csd[215:212] in bytes */
   ext_csd->sec_count |= (UINT32)value << 24;

   /* in data[221] */
   value = *(data + 221);
   /* hc_wp_grp_size: high-capacity wrie protect group size */
   /* --> ext_csd[221] in bytes */
   ext_csd->hc_wp_grp_size = value;

   /* in data[222] */
   value = *(data + 222);
   /* rel_wr_sec_c: reliable write sector count --> ext_csd[222] in bytes */
   ext_csd->rel_wr_sec_c = value;

   /* in data[223] */
   value = *(data + 223);
   /* erase_timeout_mult: high-capacity erase timeout mult */
   ext_csd->erase_timeout_mult = value;

   /* in data[224] */
   value = *(data + 224);
   /* hc_erase_grp_size: high-capacity erase unit size --> ext_csd[224] in bytes */
   ext_csd->hc_erase_grp_size = value;

   /* in data[226] */
   value = *(data + 226);
   /* boot_size_multi: boot parition size --> ext_csd[226] in bytes */
   ext_csd->boot_size_multi = value;

   /* in data[231] */
   value = *(data + 231);
   /* sec_feature_support: secure data management feature --> ext_csd[231] in bytes */
   ext_csd->sec_feature_support = value;

   /* in data[232]*/
   value = *(data + 232);
   /* trim_mult: TRIM muiltiplier --> ext_csd[232] in bytes */
   ext_csd->trim_mult = value;

   /* in data[504] */
   value = *(data + 504);
   /* s_cmd_set: supported command sets --> ext_csd[504] in bytes */
   ext_csd->s_cmd_set = value;

    /* in data[26] */
    value = *(data + 26);
    /* ffu_status: FW Update status --> ext_csd[26] in bytes */
    ext_csd->ffu_status = value;

    /* in data[30] */
    value = *(data + 30);
    /* mode_config: Mode Configuration --> ext_csd[30] in bytes */
    ext_csd->mode_config = value;

    /* in data[170] */
    value = *(data + 170);
    /* fw_config: Firmware Configuration --> ext_csd[170] in bytes */
    ext_csd->fw_config = value;

    /* in data[254:261] */
    value = *(data + 254);
    ext_csd->fw_ver = (UINT64)value;
    /* fw_ver: Firmware Version of Device --> ext_csd[254:261] in bytes */
    for (i = 1 ; i < SDCC_MMC_NUM_BYTES_FW_VER; i++)
    {
      value = *(data + (254 + i));
      ext_csd->fw_ver |= (UINT64)value << (8 * i);
    }

    /* in data[487:490] */
    value = *(data + 487);
    ext_csd->ffu_arg = (UINT32)value;
    /* fw_ver: Firmware Version of Device --> ext_csd[254:261] in bytes */
    for (i = 1 ; i < SDCC_MMC_NUM_BYTES_FFU_ARG; i++)
    {
      value = *(data + (487 + i));
      ext_csd->ffu_arg |= (UINT32)value << (8 * i);
    }

   /* in data[493] */
   value = *(data + 493);
   /* supported_modes: Supported Device Modes --> ext_csd[493] in bytes */
   ext_csd->supported_modes = value;

   /* Add new field here */

   return TRUE;
} /* sdcc_decode_extCSD */

/******************************************************************************
* Name: sdcc_decode_mmc_cid
*
* Description:
*    This function is to parse out the Card Identification (CID) register.
*
* Arguments:
*    raw_cid        [IN] : pointer to raw CID info
*    sdcc_pdev     [OUT] : pointer to device type
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
BOOLEAN sdcc_decode_mmc_cid( const UINT32   *raw_cid,
                             sdcc_dev_type  *sdcc_pdev )
{
   int i;
   UINT32 prod_name;
   UINT8 mfr_date;

   if ( NULL == raw_cid || NULL == sdcc_pdev )
   {
      return FALSE;
   }

   SetMem(emmc_cid_raw_data, sizeof(emmc_cid_raw_data), 0);
   CopyMem (emmc_cid_raw_data, raw_cid, sizeof(emmc_cid_raw_data));

   /* manufacturer ID: CID[127:120] */
   sdcc_pdev->mem.mem_info.mfr_id = (UINT16)(raw_cid[0] >> 24);

   /* poke into CID[113:112] to determine the CBX device type */
   sdcc_pdev->mem.mmc_device_type = (UINT8)((raw_cid[0] >> 16) & 0x00000003);

   /* OEM/application ID: CID[111:104] */
   sdcc_pdev->mem.mem_info.oem_id = (UINT16)((raw_cid[0] >> 8) & 0x000000FF);

   /* product name: CID[103:56] */
   sdcc_pdev->mem.mem_info.prod_name[0] = (UINT8)(raw_cid[0]);
   prod_name = raw_cid[1];
   for ( i = 4; i > 0; i-- )
   {
      sdcc_pdev->mem.mem_info.prod_name[i] = (UINT8)(prod_name & 0xFF);
      prod_name >>= 8;
   }
   sdcc_pdev->mem.mem_info.prod_name[5] = (UINT8)(raw_cid[2] >> 24);
   sdcc_pdev->mem.mem_info.prod_name[6] = '\0';

   /* product revision: CID[55:48] */
   sdcc_pdev->mem.mem_info.prod_rev = (UINT8)(raw_cid[2] >> 16);

   /* product serial number: CID[47:16] */
   sdcc_pdev->mem.mem_info.prod_serial_num =
      ((raw_cid[2] & 0x0000FFFF) << 16) | (raw_cid[3] >> 16);

   /* manufacturing date: CID[15:8] */
   mfr_date = (UINT8)(raw_cid[3] >> 8);
   (void)sdcc_decode_mmc_mfr_date( mfr_date, sdcc_pdev );

   return TRUE;
} /* sdcc_decode_mmc_cid */

/******************************************************************************
* Name: sdcc_decode_mmc_mfr_date
*
* Description:
*    This function decodes the manufacturing date as mm/yyyy format.
*
* Arguments:
*    mfr_date       [IN] : raw manufacturing date
*    sdcc_pdev     [OUT] : pointer to device type
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
BOOLEAN
sdcc_decode_mmc_mfr_date
(
   UINT8          mfr_date,
   sdcc_dev_type *sdcc_pdev
)
{
   (void) mfr_date;
   (void) sdcc_pdev;
   return TRUE;
} /* sdcc_decode_mmc_mfr_date */

/******************************************************************************
* Name: sdcc_mmc_set_partition_access
*
* Description:
*    This function sets the partition for read and write operation by
*    sending CMD6 SWITCH to the eMMC device.
*
* Arguments:
*    handle     [IN] : a pointer to the SDCC device that was returned from
*                      sdcc_handle_open()
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS
sdcc_mmc_set_partition_access( struct sdcc_device *handle )
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_dev_type *sdcc_pdev = NULL;
   UINT32 phy_partition_num = 0;

   sdcc_pdev = handle->hdev;

   /* translate physical partition number of the client handle */
   phy_partition_num = SDCC_XLATE_PARTITION_NUM(handle->phy_partition_num);

   /* check if device supports partition */
   /* User area has partition number as 0. */
   if ( phy_partition_num && FALSE == sdcc_pdev->mem.partition_support )
   {
      DEBUG ((EFI_D_ERROR, "Partition not supported"));
      return SDCC_ERR_FEATURE_UNSUPPORTED;
   }

   if ( !(SDCC_XLATE_PARTITION_ID_TO_MASK (phy_partition_num) & 
                         sdcc_pdev->mem.partition_inuse_flag) )
   {
      DEBUG ((EFI_D_INFO, "Invalid partition number %d", phy_partition_num));
      return SDCC_ERR_INVALID_PARAM;
   }

   rc = sdcc_mmc_switch(sdcc_pdev,
           SDCC_EMMC_SET_PARTITION_ACCESS |
           SDCC_EMMC_CONSTRUCT_PARTITION_CONFIG_VAL_FOR_SWITCH(
              sdcc_pdev->mem.boot_partition_enable_val, phy_partition_num));

   if (rc != SDCC_NO_ERROR)
   {
      DEBUG ((EFI_D_ERROR, "Failed to set EMMC partition %d access, rc %d",
              phy_partition_num, rc));
      return rc;
   }

   /* set the active partition number on the device list */
   sdcc_pdev->active_partition_num = phy_partition_num;
   DEBUG ((EFI_D_INFO, "Active partition is updated to %d", phy_partition_num));

   return rc;
} /* sdcc_mmc_set_partition_access */

/******************************************************************************
* Name: sdcc_mmc_get_phy_partition_info
*
* Description: 
*    This function gets the physical partition information on eMMC device.
*
* Parameters:
*    handle                     [IN] : a pointer to the SDCC device that was
*                                      returned from sdcc_handle_open()
*    partition_size_in_sectors [OUT] : physical partition size in sectors
*    phy_partition_type        [OUT] : type of physical partition to indicate
*                                      if partition is the user area, boot
*                                      partition, general purpose partition,
*                                      etc.
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS sdcc_mmc_get_phy_partition_info
(
   struct sdcc_device *handle,
   UINT32 *partition_size_in_sectors,
   UINT32 *phy_partition_type
)
{
   sdcc_dev_type *sdcc_pdev = NULL;
   UINT32 phy_partition_num = 0;

   sdcc_pdev = handle->hdev;

   /* translate physical partition number of the client handle */
   /* SDCC_HANDLE_PHY_PARTITION_ENTIRE_MEM_DEVICE is translated as */
   /* partition number 0 which is the user area. */
   phy_partition_num = SDCC_XLATE_PARTITION_NUM(handle->phy_partition_num);

   /* check if device supports partition */
   /* User area has partition number as 0. */
   if ( phy_partition_num && FALSE == sdcc_pdev->mem.partition_support )
   {
      DEBUG ((EFI_D_ERROR, "Partition is not supported"));
      return SDCC_ERR_FEATURE_UNSUPPORTED;
   }

   if ( !(SDCC_XLATE_PARTITION_ID_TO_MASK (phy_partition_num) & 
                         sdcc_pdev->mem.partition_inuse_flag) )
   {
      DEBUG ((EFI_D_INFO, "Invalid partition number %d", phy_partition_num));
      return SDCC_ERR_INVALID_PARAM;
   }

   if (SDCC_EMMC_PHY_PARTITION_USER_AREA == phy_partition_num)
   {
      /* Physical partition number 0 is the user area. */
      /* As such, returns card size. */
      *partition_size_in_sectors = sdcc_pdev->mem.mem_info.card_size_in_sectors;
      *phy_partition_type = PARTITION_USER_AREA;
   }
   else if ((SDCC_EMMC_PHY_PARTITION_BOOT_PART_1 == phy_partition_num) ||
            (SDCC_EMMC_PHY_PARTITION_BOOT_PART_2 == phy_partition_num))
   {
      /* Boot partitions 1 and 2 have the same boot partition size. */
      /* Refer to section 7.2.1 in JEDEC Standard No. 84-A43 */
      *partition_size_in_sectors =
         SDCC_CALC_PARTITION_SIZE_IN_SECTORS(sdcc_pdev->mem.partition_boot_size);
      if (SDCC_EMMC_PHY_PARTITION_BOOT_PART_1 == phy_partition_num)
      {
         *phy_partition_type = PARTITION_BOOT_1;
      }
      else
      {
         *phy_partition_type = PARTITION_BOOT_2;
      }
   }
   else if (SDCC_EMMC_PHY_PARTITION_RPMB == phy_partition_num)
   {
      *partition_size_in_sectors =
         SDCC_CALC_PARTITION_SIZE_IN_SECTORS(128 * ext_csd_raw_data[168]); 
      *phy_partition_type = PARTITION_RPMB; 
   }
   else if (SDCC_EMMC_PHY_PARTITION_GPP_1 == phy_partition_num)
   {
      *partition_size_in_sectors = sdcc_pdev->mem.partition_gp_size[0];
      *phy_partition_type = PARTITION_GPP_1;
   }
   else if (SDCC_EMMC_PHY_PARTITION_GPP_2 == phy_partition_num)
   {
      *partition_size_in_sectors = sdcc_pdev->mem.partition_gp_size[1];
      *phy_partition_type = PARTITION_GPP_2;
   }
   else if (SDCC_EMMC_PHY_PARTITION_GPP_3 == phy_partition_num)
   {
      *partition_size_in_sectors = sdcc_pdev->mem.partition_gp_size[2];
      *phy_partition_type = PARTITION_GPP_3;
   }
   else if (SDCC_EMMC_PHY_PARTITION_GPP_4 == phy_partition_num)
   {
      *partition_size_in_sectors = sdcc_pdev->mem.partition_gp_size[3];
      *phy_partition_type = PARTITION_GPP_4;
   }
   else
   {
      return SDCC_ERR_INVALID_PARAM;
   }

   return SDCC_NO_ERROR;
} /* sdcc_mmc_get_phy_partition_info */

/******************************************************************************
* Name: sdcc_mmc_set_pwr_on_write_prot
*
* Description:
*    This function sets the power-on period write protection to the physical
*    partition specified in parameter handle.  If the physical partition is
*    the user area, the write protection is applied to the specified parameter
*    data_address.  Parameter num_of_blocks should be aligned with the write
*    protect group size specified in write_protect_group_size_in_sectors in
*    structure sdcc_mem_info_type.  If the physical partition is the boot area,
*    the write protection is enabled to the entire boot partition and both boot
*    areas would get write protected.
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
*    data_address  [IN] : start of the sector to get power-on write protected
*    num_of_blocks [IN] : number of blocks to be power-on write protected
*                         (512 bytes per block)
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS sdcc_mmc_set_pwr_on_write_prot
(
   struct sdcc_device *handle,
   UINT32 data_address,
   UINT32 num_of_blocks
)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_dev_type *sdcc_pdev = NULL;

   sdcc_pdev = handle->hdev;

   /* select the requested partition */
   rc = sdcc_set_partition(handle);

   if (rc != SDCC_NO_ERROR)
   {
      return rc;
   }

   /* examine which physical partition would like to have the power-on */
   /* write protect enabled */
   if (SDCC_EMMC_IS_FROM_USER_AREA_PART(sdcc_pdev->active_partition_num))
   {
      rc = sdcc_emmc_set_user_pwr_on_wp(handle, data_address, num_of_blocks);
   }
   else if (SDCC_EMMC_IS_FROM_BOOT_AREA_PART(sdcc_pdev->active_partition_num))
   {
      rc = sdcc_emmc_set_boot_pwr_on_wp(handle);
   }
   else
   {
      /* active partition number is RPMB which is not supported */
      DEBUG ((EFI_D_ERROR, "RPMB is not supported"));
      return SDCC_ERR_FEATURE_UNSUPPORTED;
   }

   return rc;
} /* sdcc_mmc_set_pwr_on_write_prot */

/******************************************************************************
* Name: sdcc_mmc_get_write_prot_type
*
* Description:
*    This function gets the write protection type applied to the write protection
*    groups starting from the group containing the passed in data_address. If the
*    physical partition is the user area, this function returns an array of
*    8 bytes (64 bits) representing the write protection type of the 32 write
*    protection groups starting from the data_address group. The least significant
*    two bits (i.e bits[1:0] of write_prot_type[7]) correspond to the first
*    addressed group. If the physical partition is the boot area, bits[1:0] of
*    write_prot_type[7] represent the write protection type for both the BOOT partitions.
*
* Parameters:
*    handle [IN]           : a pointer to the SDCC device that was returned from
*                            sdcc_handle_open()
*    data_address [IN]     : start of the sector to get the write protection type
*    write_prot_type [OUT] : a pointer to an 8-byte array. This array contains 64 bits
*                            representing write protection type applied to
*                            the protection groups
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS sdcc_mmc_get_write_prot_type
(
   struct sdcc_device *handle,
   UINT32 data_address,
   UINT8  *write_prot_type
)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_dev_type *sdcc_pdev = NULL;

   /* get the device info specified by the current interface */
   sdcc_pdev = handle->hdev;

   /* Permanent write protection CSD[13] takes precedence over other
      write protection mechanism */
   if (TRUE == sdcc_pdev->mem.perm_write_prot_enable)
   {
      /* Permanent write protection (11b) is enabled for all */
      SetMem(write_prot_type, 8, 0xFF);
      return SDCC_NO_ERROR;
   }

   /* select the requested partition */
   rc = sdcc_set_partition(handle);

   if (rc != SDCC_NO_ERROR)
   {
      return rc;
   }

   if (SDCC_EMMC_IS_FROM_USER_AREA_PART(sdcc_pdev->active_partition_num))
   {
      rc = sdcc_emmc_get_user_prot_type(handle, data_address, write_prot_type);
   }
   else if (SDCC_EMMC_IS_FROM_BOOT_AREA_PART(sdcc_pdev->active_partition_num))
   {
      rc = sdcc_emmc_get_boot_prot_type(handle, write_prot_type);
   }
   else
   {
      /* active partition number is RPMB which is not supported */
      DEBUG ((EFI_D_ERROR, "RPMB is not supported"));
      return SDCC_ERR_FEATURE_UNSUPPORTED;
   }

   return rc;
} /* sdcc_mmc_get_pwr_on_write_prot */

/******************************************************************************
* Name: sdcc_mmc_set_active_bootable_partition
*
* Description: 
*    This function sets the BOOT_PARTITION_ENABLE bits specified in parameter
*    handle.  It clears the PARTITION_ACCESS bits if the current active
*    partition is not the user area.  It then sends the SWITCH command to the
*    eMMC device to set the BOOT_PARTITION_ENABLE bits in the EXT_CSD register.
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS
sdcc_mmc_set_active_bootable_partition
(
   struct sdcc_device *handle
)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_dev_type *sdcc_pdev = NULL;
   UINT32 phy_partition_num = 0;
   UINT32 boot_part_val = 0;

   sdcc_pdev = handle->hdev;

   /* translate physical partition number of the client handle */
   phy_partition_num = SDCC_XLATE_PARTITION_NUM(handle->phy_partition_num);
   /* validate phy_partition_num */
   if (phy_partition_num >= sdcc_pdev->mem.mem_info.num_phy_partition_created)
   {
      DEBUG ((EFI_D_ERROR, "Invalid partition number %d", phy_partition_num));
      return SDCC_ERR_INVALID_PARAM;
   }

   if (sdcc_mmc_is_phy_partition_bootable(handle) == TRUE)
   {
      /* BOOT_PARTITION_ENABLE already has the value set */
      return SDCC_NO_ERROR;
   }

   /* Clear the PARTITION_ACCESS bits if the active partition is not user */
   /* area.  Refer to section 7.3.5 in JEDEC Standard No. 84-A44 */
   if (sdcc_pdev->active_partition_num != SDCC_EMMC_PHY_PARTITION_USER_AREA)
   {
      rc = sdcc_mmc_switch(sdcc_pdev,
              SDCC_EMMC_SET_PARTITION_ACCESS |
              SDCC_EMMC_CONSTRUCT_PARTITION_CONFIG_VAL_FOR_SWITCH(
                 sdcc_pdev->mem.boot_partition_enable_val, 0));

      if (rc != SDCC_NO_ERROR)
      {
         DEBUG ((EFI_D_ERROR, "Failed to clear the PARTITION_ACCESS bits, rc %d", rc));
         return rc;
      }
   }

   /* map phy_partition_num to the value for BOOT_PARTITION_ENABLE */
   /* refer to section 8.4 in JEDEC Standard No. 84-A44 */
   boot_part_val =
      SDCC_EMMC_MAP_PHY_PART_NUM_TO_BOOT_PART_VAL(phy_partition_num);

   /* set the active boot partition */
   rc = sdcc_mmc_switch(sdcc_pdev,
           SDCC_EMMC_SET_BOOT_PARTITION_ENABLE |
           SDCC_EMMC_CONSTRUCT_PARTITION_CONFIG_VAL_FOR_SWITCH(boot_part_val,
              sdcc_pdev->active_partition_num));

   if (rc != SDCC_NO_ERROR)
   {
      DEBUG ((EFI_D_ERROR, "Failed to set boot partition enable value %d, rc %d",
         boot_part_val, rc));
      return rc;
   }

   sdcc_pdev->mem.boot_partition_enable_val = boot_part_val;

   return rc;
} /* sdcc_mmc_set_active_bootable_partition */

/******************************************************************************
* Name: sdcc_mmc_is_phy_partition_bootable
*
* Description: 
*    This function examines if the partition specified in parameter handle is
*    the bootable partition. 
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
*
* Returns:
*    TRUE if partition is bootable.  FALSE otherwise.
*
******************************************************************************/
BOOLEAN
sdcc_mmc_is_phy_partition_bootable
(
   struct sdcc_device *handle
)
{
   sdcc_dev_type *sdcc_pdev = NULL;
   UINT32 phy_partition_num = 0;

   sdcc_pdev = handle->hdev;

   /* translate physical partition number of the client handle */
   /* SDCC_HANDLE_PHY_PARTITION_ENTIRE_MEM_DEVICE is translated as */
   /* partition number 0 which is the user area. */
   phy_partition_num = SDCC_XLATE_PARTITION_NUM(handle->phy_partition_num);

   /* determine if the queried physical partition is bootable */
   switch (sdcc_pdev->mem.boot_partition_enable_val)
   {
   case SDCC_EMMC_BOOT_DATA_USER_AREA_ENABLED:
      if (phy_partition_num == SDCC_EMMC_PHY_PARTITION_USER_AREA)
      {
         return TRUE;
      }
      break;

   case SDCC_EMMC_BOOT_DATA_BOOT_PART_1_ENABLED:
      if (phy_partition_num == SDCC_EMMC_PHY_PARTITION_BOOT_PART_1)
      {
         return TRUE;
      }
      break;

   case SDCC_EMMC_BOOT_DATA_BOOT_PART_2_ENABLED:
      if (phy_partition_num == SDCC_EMMC_PHY_PARTITION_BOOT_PART_2)
      {
         return TRUE;
      }
      break;

   case SDCC_EMMC_BOOT_DATA_NOT_BOOT_ENABLED:
   default:
      break;
   }

   return FALSE;
} /* sdcc_mmc_is_phy_partition_bootable */

/******************************************************************************
* Name: sdcc_mmc_config_gpp_enh
*
* Description:
*    This function configures the General Purpose Partition and 
*    enhanced user data area information
*    Note: This routine can only be used once for the life of each
*    emmc chip!
*
* Arguments:
*    handle     [IN] : a pointer to the SDCC device that was returned from
*                      sdcc_handle_open()
*    desc       [IN] : a pointer to the structure that describes the layout
*                      of the GPP and ENH partition information
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS
sdcc_mmc_config_gpp_enh (struct sdcc_device *handle,
                         sdcc_emmc_gpp_enh_desc_type *desc)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_dev_type *sdcc_pdev = NULL;
   UINT32 wp_grp_size;               /* write protect group size in sectors */
   UINT32 gpp_grp_size;
   UINT32 pa_size = 0;
   UINT32 pa_offset = 0;
   int i;

   sdcc_pdev = handle->hdev;
   wp_grp_size = sdcc_pdev->mem.mem_info.write_protect_group_size_in_sectors;

   /* check if device supports partition */
   if (sdcc_pdev->mem.partition_support == FALSE || wp_grp_size == 0)
   {
      DEBUG ((EFI_D_ERROR, "Partition not supported"));
      return SDCC_ERR_FEATURE_UNSUPPORTED;
   }

   /* Check if the OTP area is alreay programmed */
   if (sdcc_pdev->mem.partition_conf_done & SDCC_MMC_PARTI_SET_COMPLETED_BMSK)
   {
      DEBUG ((EFI_D_ERROR, "Device already programmed with partition information"));
      return SDCC_ERR_FEATURE_UNSUPPORTED;
   }

   /* Loop through the GPP configuration and update the extended CSD table */
   for (i = 0; i < 5; i++)
   {
      switch (i) {
         case 0:
            pa_offset = SDCC_EMMC_SET_GPP1_SIZE;
            pa_size = desc->GPP_size[0];
            break;
         case 1:
            pa_offset = SDCC_EMMC_SET_GPP2_SIZE;
            pa_size = desc->GPP_size[1];
            break;
         case 2:
            pa_offset = SDCC_EMMC_SET_GPP3_SIZE;
            pa_size = desc->GPP_size[2];
            break;
         case 3:
            pa_offset = SDCC_EMMC_SET_GPP4_SIZE;
            pa_size = desc->GPP_size[3];
            break;
         case 4:
            pa_offset = SDCC_EMMC_SET_ENH_SIZE;
            pa_size = desc->ENH_size;
            break;
      }

      if (pa_size != 0)
      {
         // A small logic to ensure we round up always during the division
         pa_size += (wp_grp_size - 1);
         gpp_grp_size = pa_size / wp_grp_size;
         rc = sdcc_mmc_switch(sdcc_pdev, pa_offset | 
                              SDCC_MMC_SET_DATA_VAL (gpp_grp_size & 0xff));
         if (rc != SDCC_NO_ERROR)
         {
            DEBUG ((EFI_D_ERROR, "Failed to set GPP partition size, partition %d, rc %d", i, rc));
            return rc;
         }

         rc = sdcc_mmc_switch (sdcc_pdev, (pa_offset + SDCC_EMMC_CFG_BYTE1_OFFSET) | 
                              SDCC_MMC_SET_DATA_VAL ((gpp_grp_size >> 8) & 0xff));
         if (rc != SDCC_NO_ERROR)
         {
            DEBUG ((EFI_D_ERROR, "Failed to set GPP partition size, partition %d, rc %d", i, rc));
            return rc;
         }

         rc = sdcc_mmc_switch (sdcc_pdev, (pa_offset + SDCC_EMMC_CFG_BYTE2_OFFSET) | 
                              SDCC_MMC_SET_DATA_VAL ((gpp_grp_size >> 16) & 0xff));
         if (rc != SDCC_NO_ERROR)
         {
            DEBUG ((EFI_D_ERROR, "Failed to set GPP partition size, partition %d, rc %d", i, rc));
            return rc;
         }
      }
   }

   /* Set the enhance user area start address if the funcion is used */
   if (desc->ENH_size != 0)
   {
      UINT32 sw_addr = SDCC_EMMC_SET_ENH_START_ADDR; 
      UINT32 enh_start;
      enh_start = desc->ENH_start_addr * ((sdcc_pdev->mem.mem_info.card_type != SDCC_CARD_MMC) ?  
                                            1 : SDCC_DEFAULT_BLK_LENGTH_SIZE);
      for (i = 0; i < 4; i++)
      {
         rc = sdcc_mmc_switch (sdcc_pdev, sw_addr | 
                              SDCC_MMC_SET_DATA_VAL (enh_start & 0xff));
         if (rc != SDCC_NO_ERROR)
         {
            DEBUG ((EFI_D_ERROR, "Failed to set ENH area start address, rc %d", rc));
            return rc;
         }
         enh_start = enh_start >> 8;
         sw_addr += SDCC_EMMC_CFG_BYTE1_OFFSET;
      }
   }

   // Set the Enhancement attribute flag
   rc = sdcc_mmc_switch (sdcc_pdev, SDCC_EMMC_SET_PARTITION_ATTR |
                        SDCC_MMC_SET_DATA_VAL (desc->GPP_enh_flag));
   if (rc != SDCC_NO_ERROR)
   {
      DEBUG ((EFI_D_ERROR, "Failed to set ENH area start address, rc %d", rc));
      return rc;
   }

   /* Finish by updateing the complete flag */
   // Disabled for debugging!
   rc = sdcc_mmc_switch (sdcc_pdev, SDCC_EMMC_PARTITION_SET_DONE);
   if (rc != SDCC_NO_ERROR)
   {
      DEBUG ((EFI_D_ERROR, "Failed to set partition completition flag, rc %d", rc));
      return rc;
   }

   return rc;
} /* sdcc_mmc_config_gpp_enh */

/******************************************************************************
* Name: sdcc_get_ext_csd_rev
*
* Description:
*    This function retrives the EXT_CSD register version which provides
*    information on the MMC Device Standard.
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
*
* Returns:
*    Returns the EXT_CSD Revision.
*
******************************************************************************/
UINT8
sdcc_mmc_get_ext_csd_rev (struct sdcc_device *handle)
{
  (VOID) handle;
  return ext_csd_decoded.ext_csd_rev;
}

/******************************************************************************
* Name: sdcc_mmc_is_ffu_allowed
*
* Description:
*    This function checks if field firmware update supported by the device
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
* Returns:
*    TRUE if FW Update is supported and False otherwise
*
******************************************************************************/
BOOLEAN sdcc_mmc_is_ffu_allowed (struct sdcc_device *handle)
{
   (VOID) handle;
   /* If FFU Mode supported and FW Update not disabled return TRUE */

   if (SDCC_EXTRACT_BIT_VAL_FROM_BYTE_REG (ext_csd_decoded.supported_modes,
       SDCC_MMC_BIT_FFU_SUPPORT, SDCC_MMC_NUM_BITS_FFU_SUPPORT) == 0x0)
   {
      return FALSE;
   }

   if (SDCC_EXTRACT_BIT_VAL_FROM_BYTE_REG (ext_csd_decoded.fw_config,
       SDCC_MMC_BIT_FW_UPDATE_DISABLE, SDCC_MMC_NUM_BITS_FW_UPDATE_DISABLE)
       == 0x1)
   {
      return FALSE;
   }

   return TRUE;
} /* sdcc_mmc_is_ffu_allowed */

/******************************************************************************
* Name: sdcc_mmc_set_mode_config
*
* Description:
*    This function sets the devcie mode config to specified mode
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
*    mode          [IN] : mode to set
*
* Returns:
*    SDCC_NO_ERROR if successful set the mode.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS sdcc_mmc_set_mode_config
(
   struct sdcc_device *handle,
   enum sdcc_mmc_mode_config mode
)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   UINT32 switch_arg;
   sdcc_dev_type *sdcc_pdev = handle->hdev;

   switch (mode)
   {
      case SDCC_MMC_MODE_CONFIG_NORMAL:
        switch_arg = SDCC_MMC_SET_MODE_CONFIG_NORMAL;
        break;

      case SDCC_MMC_MODE_CONFIG_FFU:
        switch_arg = SDCC_MMC_SET_MODE_CONFIG_FFU;
        break;

      default:
        DEBUG ((EFI_D_INFO, "Cannot support switching to mode, %d", mode));
        return SDCC_ERR_OP_NOT_SUPPORTED;
   }

   rc = sdcc_mmc_switch (sdcc_pdev, switch_arg);

   return rc;
} /* sdcc_mmc_set_mode_config */

/******************************************************************************
* Name: sdcc_mmc_get_ffu_argument
*
* Description:
*    This function returns the value of the FFU_ARG field to be used with the
*    program command.
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
* Returns:
*    FFU_ARG field value of the EXT_CSD Register.
*
******************************************************************************/
UINT32
sdcc_mmc_get_ffu_argument (struct sdcc_device *handle)
{
  (VOID) handle;
  return ext_csd_decoded.ffu_arg;
}   /* sdcc_mmc_get_ffu_argument */

/******************************************************************************
* Name: sdcc_mmc_get_ffu_status
*
* Description:
*    This function returns the status of previously issued Firmware update.
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
* Returns:
*    Status of the MMC Firmware update.
*
******************************************************************************/
SDCC_STATUS sdcc_mmc_get_ffu_status (struct sdcc_device *handle)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_dev_type *sdcc_pdev = handle->hdev;

   rc = sdcc_get_ext_csd(sdcc_pdev, ext_csd_raw_data);
   if (rc != SDCC_NO_ERROR)
   {
      return rc;
   }

  /* decode ext_csd_raw_data */
  (void) sdcc_decode_extCSD(ext_csd_raw_data, &ext_csd_decoded);

   switch ((enum sdcc_mmc_ffu_status) ext_csd_decoded.ffu_status)
   {
      case SDCC_MMC_FFU_NO_ERROR:
         return SDCC_NO_ERROR;

      case SDCC_MMC_FFU_GENERAL_ERROR:
         return SDCC_ERR_FW_GENERAL_ERROR;

      case SDCC_MMC_FFU_FW_INSTALL_ERROR:
         return SDCC_ERR_FW_INSTALL;

     case SDCC_MMC_FFU_FW_DOWNLOAD_ERROR:
         return SDCC_ERR_FW_DOWNLOAD;

     default:
         return SDCC_ERR_UNKNOWN;
   }
}   /* sdcc_mmc_get_ffu_status */

/******************************************************************************
* Name: sdcc_mmc_get_fw_version
*
* Description:
*    This function returns the Firmware Version of the device.
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
* Returns:
*    The Firmware Version of the device.
*
******************************************************************************/
UINT64
sdcc_mmc_get_fw_version  (struct sdcc_device *handle)
{
   (VOID) handle;
   return ext_csd_decoded.fw_ver;
}

/*lint -restore */

