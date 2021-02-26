 /*****************************************************************************
 * sdcc_mini_api.c
 *
 * SDCC(Secure Digital Card Controller) driver minimized API for memory device.
 *
 * This file implements the SDCC driver minimized APIs for the memory device.
 *
 * Copyright (c) 2014-2017 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *****************************************************************************/
/*=============================================================================
                        Edit History


YYYY-MM-DD   who     what, where, why
----------   ---     ----------------------------------------------------------- 
2017-06-12   jt      Update sdcc_sdhci_mode_enable()
2017-05-12   sb      Add partial enumeration for eMMC 
2016-12-14   jt      Remove legacy code 
2016-01-18   eo      Reduced sdcc code size
2015-12-18   jt      Enable bus power before reset  
2015-06-17   bn      Fixed 32-bit compilation warning      
2015-04-15   bn      Added sdcc_emmc_info_raw_data()
2015-03-13   bn      Added controller reset to sdcc_sdhci_mode_enable()
2015-02-10   bn      Added busywait() for use as delay function
2015-01-26   rm      Integraged HS200/HS400 support for eMMC from UEFI 3.0
2015-01-26   bn      Add check in sdcc_handle_read to return for rpmb partition
2014-11-10   bn      Add null pointer check for sdcc_get_slot_handle
2014-11-06   bn      Added eMMC FW Update support
2014-10-14   bn      Added sdcc_sdhci_mode_enable()
2014-09-24   bn      Code cleanup
2014-06-18   bn      Initial version. Branched from 8994 SBL

=============================================================================*/
#include "sdcc_priv.h"
#include "sdcc_util.h"
#include "sdcc_handle_util.h"
#include "sdcc_mmc_util.h"
#include "SdccBsp.h"
#include "sdcc_hc.h"
#include "sdcc_pal_tool.h"
#include "SdccCommon.h"
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/SdccCommonLib/SdccHalLib/SdccHal.h>
#include <Library/PcdLib.h>
#include <api/systemdrivers/busywait.h>
#include <BootConfig.h>

UINT8 ext_csd_raw_data[SDCC_MMC_EXT_CSD_SIZE];
UINT8 emmc_cid_raw_data[SDCC_MMC_CID_SIZE];
UINT8 emmc_csd_raw_data[SDCC_MMC_CSD_SIZE];

sdcc_mini_boot_device_info_type sdcc_mini_boot_device_info = {0};

/*lint -save -e641 Suppress 'Converting enum to int' warning */
/******************************************************************************
* Name: sdcc_handle_open
*
* Description:
*    This function first initializes the SDCC controller such as turning on the
*    clock, configuring the GPIOs, and turning on the power supply to the
*    device.  It returns NULL if there is no memory device attached.  If there
*    is a memory device, it will initialize the device and take it out of the
*    identification state.  It then sets up the device for data transfer.
*    A non-null pointer to the SDCC device is returned.
*
* Arguments:
*    driveno           [IN] : a valid drive number between 0 and the total
*                            number of card slots - 1
*    phy_partition_num [IN] : physical partition number.  Use
*                             \ref SDCC_HANDLE_PHY_PARTITION_ENTIRE_MEM_DEVICE
*                             to get a handle representing the entire memory
*                             device.  For eMMC v4.3, the valid boot partition
*                             numbers are 1 and 2.  User area partition number
*                             is 0.
*
* Returns:
*    A pointer to the SDCC device if successful.  NULL if failed.
*
* Note:
*    sdcc_init() is called in sdcc_handle_open().
*
******************************************************************************/
struct sdcc_device *
sdcc_handle_open( INT16 driveno, UINT32 phy_partition_num )
{
   SDCC_STATUS         rc         = SDCC_NO_ERROR;
   sdcc_slot_type     *sdcc_pdata = NULL;
   sdcc_dev_type      *sdcc_pdev  = NULL;
   BOOLEAN             ret        = TRUE;
   struct sdcc_device *ret_handle = NULL;
   BOOLEAN             still_in_use = TRUE;

   if ( !SDCC_DRIVENO_IS_VALID(driveno) )
   {
      DEBUG ((EFI_D_ERROR, "Invalid Drive Number: %d", driveno));
      return NULL;
   }

   /* initialize delay function */
   busywait_init();
   
   ret = sdcc_init_sdhci_mode(driveno);
   
   if ( FALSE == ret )
   {
      DEBUG ((EFI_D_ERROR, "Failed to initialize drive number %d", driveno));
      return NULL;
   }

   sdcc_pdata = sdcc_get_slot_handle (driveno);
   if (NULL == sdcc_pdata)
   {  
      DEBUG ((EFI_D_ERROR, "sdcc_handle_open: can't get slot handle\n"));
      return NULL;
   }

   sdcc_pdev = &(sdcc_pdata->dev);

   do
   {
      /* register the client handle first */
      ret_handle = sdcc_register_client_handle( (UINT32)driveno,
                                                phy_partition_num );
      if ( NULL == ret_handle )
      {
         /* check if memory device in driveno is still in use */
         still_in_use = sdcc_is_device_still_in_use( sdcc_pdev );
         if ( !still_in_use )
         {
            /* client handle list is full */
            (void) sdcc_deinit( sdcc_pdev );
         }
         break;
      }

      /* return if it has been initialized */
      if (( SDCC_CARD_SD  == sdcc_pdev->card_type ) ||
          ( SDCC_CARD_SDHC == sdcc_pdev->card_type ) ||
          ( SDCC_CARD_MMC == sdcc_pdev->card_type ) ||
          ( SDCC_CARD_MMCHC == sdcc_pdev->card_type ))
      {
         /* no need to select the partition region on the device as it is */
         /* already in use */
         /* will return a new handle to client */
         break;
      }

      /* find the attached device */
      sdcc_pdev->card_type = sdcc_handle_find_card( ret_handle );
      /* update card_type field in sdcc_mem_info struct */
      sdcc_pdev->mem.mem_info.card_type = sdcc_pdev->card_type;

      /* return and close controller if none attached */
      if ( SDCC_CARD_UNKNOWN == sdcc_pdev->card_type )
      {
         (void) sdcc_handle_close(ret_handle);
         ret_handle = NULL;
         break;
      }

      /* proceed if this is SD/MMC device */
      /* finish identification mode */
      rc = sdcc_init_memory_device(sdcc_pdev);
      if (SDCC_NO_ERROR != rc)
      {
         DEBUG ((EFI_D_ERROR, "Failed to initialize the card.\n"));
         sdcc_pdev->errno = rc;

         (void) sdcc_handle_close(ret_handle);
         ret_handle = NULL;
         break;
      }

      /* get memory device info  */
      rc = sdcc_get_memory_info(sdcc_pdev);
      if (SDCC_NO_ERROR != rc)
      {
         DEBUG ((EFI_D_ERROR, "Failed to get memory card info.\n"));
         sdcc_pdev->errno = rc;

         (void) sdcc_handle_close(ret_handle);
         ret_handle = NULL;
         break;
      }

      /* re-program the clock for data transfer */
      sdcc_config_clk(sdcc_pdev, SDCC_DATA_TRANSFER_MODE);

      /* config device for data transfer */
      rc = sdcc_config_memory_device(sdcc_pdev);
      if (SDCC_NO_ERROR == rc)
      {
         sdcc_pdev->host_state = SDCC_HOST_TRAN;
         /* set to true if card is removed / reinserted */
         sdcc_pdata->slot_state_changed = FALSE;
      }
      else
      {
         sdcc_pdev->errno     = rc;
         sdcc_pdev->card_type = SDCC_CARD_UNKNOWN;
         (void) sdcc_handle_close(ret_handle);
         ret_handle = NULL;
      }
   }
   while (0);

   return ret_handle;
} /* sdcc_handle_open */

/******************************************************************************
* Name: sdcc_handle_mem_get_device_info
*
* Description:
*    This function gets the memory card specific information.  If there is no
*    memory device attached or device is not initialized, all fields in the
*    card specific information are zero.
*
* Dependencies:
*    sdcc_handle_open() must be called before getting card info for the
*    specified SDCC handle.
*
* Parameters:
*    handle          [IN] : a pointer to the SDCC device that was returned from
*                           sdcc_handle_open()
*    sdcc_pmem_info [OUT] : pointer to the structure to save the memory card
*                           specific information
*
* Returns:
*    SDCC_NO_ERROR on success
*    error code otherwise
*
******************************************************************************/
SDCC_STATUS sdcc_handle_mem_get_device_info
(
   struct sdcc_device *handle,
   sdcc_mem_info_type *sdcc_pmem_info
)
{
   SDCC_STATUS      rc = SDCC_NO_ERROR;
   sdcc_dev_type   *sdcc_pdev  = NULL;
   UINT32           driveno    = 0;
   UINT32           phy_partition_num = 0;
   UINT32           partition_size_in_sectors = 0;
   UINT32           phy_partition_type = 0;

   if ( NULL == handle || NULL == handle->hdev )
   {
      DEBUG ((EFI_D_ERROR, "Invalid SDCC handle"));
      return SDCC_ERR_INVALID_PARAM;
   }

   driveno = SDCC_HANDLE_GET_DRIVENO(handle);
   if ( !SDCC_DRIVENO_IS_VALID(driveno) )
   {
      DEBUG ((EFI_D_ERROR, "Invalid drive number %d", driveno));
      return SDCC_ERR_INVALID_PARAM;
   }

   if ( NULL == sdcc_pmem_info )
   {
      DEBUG ((EFI_D_ERROR, "Passed in NULL sdcc_pmem_info pointer"));
      return SDCC_ERR_INVALID_PARAM;
   }

   sdcc_pdev = handle->hdev;

   /* zero out all the memory info fields */
   SetMem( sdcc_pmem_info, sizeof(sdcc_mem_info_type), 0);

   /* translate physical partition number of the client handle */
   /* SDCC_HANDLE_PHY_PARTITION_ENTIRE_MEM_DEVICE is translated as */
   /* partition number 0 which is the user area. */
   phy_partition_num = SDCC_XLATE_PARTITION_NUM(handle->phy_partition_num);

   do
   {
      if ( SDCC_CARD_UNKNOWN == sdcc_pdev->card_type )
      {
         rc = SDCC_ERR_CARD_UNDETECTED;
         break;
      }

      /* populate the sdcc_pmem_info out pointer */
      sdcc_pmem_info->card_type = sdcc_pdev->card_type;
      sdcc_pmem_info->block_len = sdcc_pdev->mem.mem_info.block_len;

      /* Query the BSP to see if the slot is embedded */
      if (sdcc_bsp_get_slot_type(driveno) & SDCC_BSP_EMBEDDED_MEM_SLOT)
      {
         sdcc_pmem_info->is_embedded_device = TRUE;
      }
      else
      {
         sdcc_pmem_info->is_embedded_device = FALSE;
      }

      if ( SDCC_CARD_MMC == sdcc_pdev->card_type ||
           SDCC_CARD_MMCHC == sdcc_pdev->card_type )
      {
         rc = sdcc_mmc_get_phy_partition_info( handle,
                  &partition_size_in_sectors, &phy_partition_type );

         /* regardless rc, populate the partition size and type */
         sdcc_pmem_info->card_size_in_sectors = partition_size_in_sectors;
         sdcc_pmem_info->speed_class.mmc_speed_class =
            (sdcc_mmc_speed_class_type)sdcc_pdev->mem.speed_class;
         sdcc_pmem_info->reliable_write_sector_count =
            sdcc_pdev->mem.mem_info.reliable_write_sector_count;
         sdcc_pmem_info->is_phy_partition_bootable =
            sdcc_mmc_is_phy_partition_bootable(handle);
         sdcc_pmem_info->rpmb_parti_size_in_byte = 
               (sdcc_pdev->mem.mem_info.rpmb_parti_size_in_byte);
         /* eMMC Write Protect support */
         sdcc_pmem_info->write_prot_support = sdcc_pdev->mem.write_prot_support; 
         /* eMMC Write Protect Group Size */
         sdcc_pmem_info->write_protect_group_size_in_sectors =
            sdcc_pdev->mem.mem_info.write_protect_group_size_in_sectors;
         /* HW Reset Function. RST_n_ENABLE bis of the EXT_CSD[162] */
         sdcc_pmem_info->hw_reset_function = sdcc_pdev->mem.mem_info.hw_reset_function;
         /* eMMC Erase Group Size */
         sdcc_pmem_info->erase_group_size_in_sectors = 
               sdcc_pdev->mem.mem_info.erase_group_size_in_sectors;            
      }
      else
      {
         if ( 0 == phy_partition_num )
         {
            sdcc_pmem_info->card_size_in_sectors =
               sdcc_pdev->mem.mem_info.card_size_in_sectors;
         }
         else
         {
            /* regardless rc, populate the partition size and type */
            rc = SDCC_ERR_FEATURE_UNSUPPORTED;
            sdcc_pmem_info->card_size_in_sectors = 0;
         }
         /* Card type is SD or SDHC. */
         sdcc_pmem_info->speed_class.sd_speed_class =
            (sdcc_sd_speed_class_type)sdcc_pdev->mem.speed_class;
         sdcc_pmem_info->reliable_write_sector_count = 0;
         sdcc_pmem_info->is_phy_partition_bootable = FALSE;
      }

      sdcc_pmem_info->phy_partition_type = phy_partition_type;
      sdcc_pmem_info->num_phy_partition_created =
         sdcc_pdev->mem.mem_info.num_phy_partition_created;

      /* card identification info */
      sdcc_pmem_info->mfr_id = sdcc_pdev->mem.mem_info.mfr_id;
      sdcc_pmem_info->oem_id = sdcc_pdev->mem.mem_info.oem_id;

      CopyMem( sdcc_pmem_info->prod_name, sdcc_pdev->mem.mem_info.prod_name,
         sizeof(sdcc_pdev->mem.mem_info.prod_name) );

      sdcc_pmem_info->prod_rev = sdcc_pdev->mem.mem_info.prod_rev;
      sdcc_pmem_info->prod_serial_num = sdcc_pdev->mem.mem_info.prod_serial_num;

      CopyMem( sdcc_pmem_info->mfr_date, sdcc_pdev->mem.mem_info.mfr_date,
         sizeof(sdcc_pdev->mem.mem_info.mfr_date) );

   } while (0);

   return rc;
} /* sdcc_handle_mem_get_device_info */

/******************************************************************************
* Name: sdcc_get_emmc_info_raw_data
*
* Description:
*    This function gets the raw data of eMMC's Extended CSD, CID and CSD.  
*    If there is no memory device attached or device is not initialized, 
*    all fields in the card specific information are zero.
*
* Dependencies:
*    sdcc_handle_open() must be called before getting card info for the
*    specified SDCC handle.
*
* Parameters:
*    handle           [IN] : a pointer to the SDCC device that was returned from
*                            sdcc_handle_open()
*    emmc_info_raw   [OUT] : pointer to the buffer to save the device info raw data
*    buff_len     [IN/OUT] : pointer to the size of buffer on input. 
*                            Upon exit, return the size of the data written
*
* Returns:
*    SDCC_NO_ERROR on success
*    error code otherwise
*
******************************************************************************/
SDCC_STATUS sdcc_get_emmc_info_raw_data
(
   struct sdcc_device *handle,
   VOID *emmc_info_raw,
   UINT32 *buff_len
)
{
   UINT32 driveno = 0;
   UINT32 buff_index = 0;
   sdcc_dev_type  *sdcc_pdev  = NULL;
   sdcc_slot_type *sdcc_pdata = NULL;
   
   if ( NULL == handle || NULL == handle->hdev )
   {
      DEBUG ((EFI_D_ERROR, "SDCC GetDeviceInfo raw data:: Invalid SDCC handles 0x%x", handle));
      return SDCC_ERR_INVALID_PARAM;
   }
   
   if ( NULL == emmc_info_raw )
   {
      DEBUG ((EFI_D_ERROR, "Get eMMC Raw Data:: emmc_info_raw is NULL"));
      return SDCC_ERR_INVALID_PARAM;
   }

   if ( *buff_len < EMMC_RAW_DATA_SIZE)
   {
      DEBUG ((EFI_D_ERROR, "Get eMMC Raw Data:: buff_len = %d is too small", buff_len));
      return SDCC_ERR_INVALID_PARAM;      
   }
   
   driveno = SDCC_HANDLE_GET_DRIVENO(handle);
   if ( !SDCC_DRIVENO_IS_VALID(driveno) )
   {
      DEBUG ((EFI_D_ERROR, "Get eMMC Raw Data:: Invalid drive number %d", driveno));
      return SDCC_ERR_INVALID_PARAM;
   }

   sdcc_pdata = sdcc_get_slot_handle (driveno);
   if (NULL == sdcc_pdata)
   {  
      DEBUG ((EFI_D_ERROR, "Get eMMC Raw Data:: can't get slot handle\n"));
      return SDCC_ERR_INVALID_PARAM;
   }
   
   sdcc_pdev = &(sdcc_pdata->dev);
   
   /* eMMC must have been initialized */
   if (( SDCC_CARD_MMC != sdcc_pdev->card_type ) &&
	  ( SDCC_CARD_MMCHC != sdcc_pdev->card_type ))
   {
	   DEBUG ((EFI_D_ERROR, "Get eMMC Raw Data:: eMMC not intialized\n"));
	   return SDCC_ERR_INVALID_PARAM;
   }
   
   buff_index = 0;
   CopyMem (emmc_info_raw, ext_csd_raw_data, sizeof(ext_csd_raw_data));
   buff_index += sizeof(ext_csd_raw_data);
   
   CopyMem ((VOID *)((UINTN)emmc_info_raw + buff_index), emmc_cid_raw_data, sizeof(emmc_cid_raw_data));
   buff_index += sizeof(emmc_cid_raw_data);
   
   CopyMem ((VOID *)((UINTN)emmc_info_raw + buff_index), emmc_csd_raw_data, sizeof(emmc_csd_raw_data));

   /* return the buffer len that is returned by this function */
   *buff_len = sizeof(ext_csd_raw_data) + sizeof(emmc_cid_raw_data) +
               sizeof(emmc_csd_raw_data);

   return SDCC_NO_ERROR;
}

/******************************************************************************
* Name: sdcc_handle_is_mem_present
*
* Description:
*    This function gets the card status of the specified SDCC handle.
*
* Parameters:
*    handle     [IN] : a pointer to the SDCC device that was returned from
*                      sdcc_handle_open
*
* Returns:
*    TRUE if memory device is present.  FALSE otherwise.
*
******************************************************************************/
BOOLEAN sdcc_handle_is_mem_present( struct sdcc_device *handle )
{
   sdcc_dev_type  *sdcc_pdev  = NULL;
   SDCC_CARD_STATUS card_status = SDCC_CARD_IDLE;
   UINT32 driveno = 0;

   if ( NULL == handle || NULL== handle->hdev )
   {
      DEBUG ((EFI_D_ERROR, "Invalid SDCC handle 0x%x", handle));
      return FALSE;
   }

   driveno = SDCC_HANDLE_GET_DRIVENO(handle);
   if ( !SDCC_DRIVENO_IS_VALID(driveno) )
   {
      DEBUG ((EFI_D_ERROR, "Invalid drive number %d", driveno));
      return FALSE;
   }

   sdcc_pdev = handle->hdev;
   if ( sdcc_pdev->pslot->slot_state_changed ||
        SDCC_CARD_UNKNOWN == sdcc_pdev->card_type )
   {
      return FALSE;
   }

   card_status = sdcc_send_status( sdcc_pdev );
   if ( SDCC_CARD_IGNORE == card_status )
   {
      sdcc_pdev->card_type = SDCC_CARD_UNKNOWN;
      return FALSE;
   }

   return TRUE;
} /* sdcc_handle_is_mem_present */

/******************************************************************************
* Name: sdcc_handle_read
*
* Description:
*    This function reads num_blocks blocks of data from the data_address in
*    the device, storing them at the memory location given by buff.
*
* Arguments:
*    handle           [IN] : a pointer to the SDCC device that was returned
*                            from sdcc_handle_open()
*    data_address     [IN] : start of the sector.  This is the sector unit
*                            address (512 bytes per block).
*    buff            [OUT] : pointer to buffer for the incoming data
*    num_blocks       [IN] : number of blocks to be read (512 bytes per
*                            block)
*
* Returns:
*    SDCC_NO_ERROR if successful
*    Error code otherwise
*
* Note:
*    Multiblock transfer is used when more than 1 block data is transfered.
*
******************************************************************************/
SDCC_STATUS
sdcc_handle_read
(
   struct sdcc_device *handle,
   UINT32              data_address,
   UINT8               *buff,
   UINT16              num_blocks
)
{
   UINT32           driveno = 0;
   BOOLEAN          multi_block = FALSE;
   sdcc_dev_type   *sdcc_pdev   = NULL;
   sdcc_cmd_type    sdcc_cmd;
   SDCC_STATUS rc = SDCC_NO_ERROR;

   if ( NULL == handle || NULL == handle->hdev )
   {
      DEBUG ((EFI_D_ERROR, "Invalid SDCC handle 0x%x", handle));
      return SDCC_ERR_INVALID_PARAM;
   }

   driveno = SDCC_HANDLE_GET_DRIVENO(handle);
   if ( !SDCC_DRIVENO_IS_VALID(driveno) )
   {
      DEBUG ((EFI_D_ERROR, "Invalid drive number %d", driveno));
      return SDCC_ERR_INVALID_PARAM;
   }

   sdcc_pdev = handle->hdev;

   /* fail the call if card is removed or reinserted */
   if ( sdcc_pdev->pslot->slot_state_changed )
   {
      DEBUG ((EFI_D_ERROR, "Slot %d state changed", driveno));
      return SDCC_ERR_CARD_REMOVED;
   }

   if (handle->phy_partition_num == 3)
   {
      return SDCC_ERR_INVALID_PARAM;   
   }   
   
   if ( SDCC_HOST_TRAN != sdcc_pdev->host_state )
   {
      if ( FALSE == sdcc_handle_silent_reinit(handle) )
      {
         rc = SDCC_ERR_CARD_INIT;
         return rc;
      }
   }

   if ( SDCC_CARD_UNKNOWN == sdcc_pdev->card_type )
   {
      rc = SDCC_ERR_CARD_UNDETECTED;
      return rc;
   }
   else if ( SDCC_CARD_SD == sdcc_pdev->card_type ||
             SDCC_CARD_SDHC == sdcc_pdev->card_type ||
             SDCC_CARD_MMC == sdcc_pdev->card_type ||
             SDCC_CARD_MMCHC == sdcc_pdev->card_type )
   {
      /* set the client handle partition number before data transfer */
      rc = sdcc_set_partition( handle );
      if ( SDCC_NO_ERROR != rc )
      {
         /* failed to select the requested partition number */
         return rc;
      }

      multi_block = (1 == num_blocks) ? FALSE : TRUE;
        
      /* CMD17/CMD18 */
      sdcc_cmd.cmd = ( multi_block ) ? SD_CMD18_READ_MULTIPLE_BLOCK :
                                      SD_CMD17_READ_BLOCK;
      sdcc_cmd.resp_type = SDCC_RESP_SHORT;
      sdcc_cmd.prog_scan = 0;
      sdcc_cmd.flags = SDCC_TRANSFER_FLAG_IS_READ_TRANSFER;
  
      if ( SDCC_CARD_SDHC == sdcc_pdev->card_type ||
           SDCC_CARD_MMCHC == sdcc_pdev->card_type )
      {
         /* in high capacity cards, the 32-bit argument of memory access */
         /* commands uses the memory address in block address format */
         sdcc_cmd.cmd_arg   = data_address;
      }
      else
      {
         /* in standard capacity cards, the 32-bit argument of memory access */
         /* commands uses the memory address in byte address format */
         sdcc_cmd.cmd_arg   = data_address * (sdcc_pdev->mem.mem_info.block_len);   
      }

      rc = sdcc_do_transfer(sdcc_pdev, &sdcc_cmd, buff, num_blocks);
   }
   else
   {
      /* card type is SDIO */
      DEBUG ((EFI_D_ERROR, "Operation not supported on SDIO card"));
      rc = SDCC_ERR_SDIO;
      return rc;
   }

   return rc;
} /* sdcc_handle_read */

/******************************************************************************
* Name: sdcc_handle_write
*
* Description:
*    This function writes num_blocks blocks of data, obtained from the
*    memory location buff, to the data_address on the device.
*
* Arguments:
*    handle           [IN] : a pointer to the SDCC device that was returned
*                            from sdcc_handle_open()
*    data_address     [IN] : start of the sector.  This is the sector unit
*                            address (512 bytes per block).
*    buff             [IN] : pointer to buffer for the outgoing data
*    num_blocks       [IN] : number of blocks to be written (512 bytes per
*                            block)
*
* Returns:
*    SDCC_NO_ERROR if successful
*    Error code otherwise
*
* Note:
*    Multiblock transfer is used when more than 1 block data is transfered.
*
******************************************************************************/
SDCC_STATUS
sdcc_handle_write
(
   struct sdcc_device *handle,
   UINT32              data_address,
   UINT8               *buff,
   UINT16              num_blocks
)
{
   UINT32 driveno = 0;
   BOOLEAN          multi_block = FALSE;
   sdcc_dev_type   *sdcc_pdev   = NULL;
   sdcc_cmd_type    sdcc_cmd;
   SDCC_STATUS rc = SDCC_NO_ERROR;

   if ( NULL == handle || NULL == handle->hdev )
   {
      DEBUG ((EFI_D_ERROR, "Invalid SDCC handle 0x%x", handle));
      return SDCC_ERR_INVALID_PARAM;
   }

   driveno = SDCC_HANDLE_GET_DRIVENO(handle);
   if ( !SDCC_DRIVENO_IS_VALID(driveno) )
   {
      DEBUG ((EFI_D_ERROR, "Invalid drive number %d", driveno));
      return SDCC_ERR_INVALID_PARAM;
   }

   sdcc_pdev = handle->hdev;

   /* fail the call if card is removed or reinserted */
   if ( sdcc_pdev->pslot->slot_state_changed )
   {
      DEBUG ((EFI_D_ERROR, "Slot %d state changed", driveno));
      return SDCC_ERR_CARD_REMOVED;
   }

   if ( SDCC_HOST_TRAN != sdcc_pdev->host_state )
   {
      if ( FALSE == sdcc_handle_silent_reinit(handle) )
      {
         rc = SDCC_ERR_CARD_INIT;
         return rc;
      }
   }

   if ( SDCC_CARD_UNKNOWN == sdcc_pdev->card_type )
   {
      rc = SDCC_ERR_CARD_UNDETECTED;
      return rc;
   }
   else if ( SDCC_CARD_SD == sdcc_pdev->card_type ||
             SDCC_CARD_SDHC == sdcc_pdev->card_type ||
             SDCC_CARD_MMC == sdcc_pdev->card_type ||
             SDCC_CARD_MMCHC == sdcc_pdev->card_type )
   {
      /* set the client handle partition number before data transfer */
      rc = sdcc_set_partition( handle );
      if ( SDCC_NO_ERROR != rc )
      {
         /* failed to select the requested partition number */
         return rc;
      }

      multi_block = (1 == num_blocks) ? FALSE : TRUE;
        
      /* CMD24/CMD25 */
      sdcc_cmd.cmd = (multi_block) ? SD_CMD25_WRITE_MULTIPLE_BLOCK :
                                     SD_CMD24_WRITE_BLOCK;
      sdcc_cmd.resp_type = SDCC_RESP_SHORT;
      sdcc_cmd.prog_scan = 0;
      sdcc_cmd.flags = SDCC_TRANSFER_FLAG_IS_WRITE_TRANSFER;
  
      if ( SDCC_CARD_SDHC == sdcc_pdev->card_type ||
           SDCC_CARD_MMCHC == sdcc_pdev->card_type )
      {
         /* in high capacity cards, the 32-bit argument of memory access */
         /* commands uses the memory address in block address format */
         sdcc_cmd.cmd_arg   = data_address;
      }
      else
      {
         /* in standard capacity cards, the 32-bit argument of memory access */
         /* commands uses the memory address in byte address format */
         sdcc_cmd.cmd_arg   = data_address * (sdcc_pdev->mem.mem_info.block_len);   
      }

      rc = sdcc_do_transfer(sdcc_pdev, &sdcc_cmd, buff, num_blocks);
   }
   else
   {
      /* card type is SDIO */
      DEBUG ((EFI_D_ERROR, "Operation not supported on SDIO card"));
      rc = SDCC_ERR_SDIO;
      return rc;
   }

   return rc;
} /* sdcc_handle_write */

/******************************************************************************
* Name: sdcc_handle_close
*
* Description:
*    This function de-select the device, turn off the power supply to
*    the device and switch off the controller clock.
*
* Arguments:
*    handle     [IN] : a pointer to a SDCC device
*
* Returns:
*    SDCC_NO_ERROR if successful.  Erro code otherwise
*
******************************************************************************/
SDCC_STATUS sdcc_handle_close( struct sdcc_device *handle )
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   BOOLEAN still_in_use = TRUE;
   sdcc_dev_type *sdcc_pdev = NULL; 

   /* sanity check the handle */
   if ( NULL == handle || NULL == handle->hdev )
   {
      DEBUG ((EFI_D_ERROR, "Invalid SDCC handle"));
      return SDCC_ERR_INVALID_PARAM;
   }

   sdcc_pdev = handle->hdev;

   /* de-register the client handle first */
   sdcc_deregister_client_handle( &handle );
   /* check if memory device in driveno is still in use */
   still_in_use = sdcc_is_device_still_in_use( sdcc_pdev );
   if ( !still_in_use )
   {
      rc = sdcc_deinit( sdcc_pdev );
   }

   return rc;
} /* sdcc_handle_close */

/*=============================================================================
 * FUNCTION      sdcc_get_config_info
 *
 * DESCRIPTION   This function gets the hardware/target specific information
 *               such as total number of card slots, block size, and maximum
 *               number of blocks that the target and SDCC controller support.
 *
 * DEPENDENCIES  None
 *
 * PARAMETERS    sdcc_pconfig_info [OUT]: pointer to the structure to save the
 *                                        hardware/target specific information
 *
 * RETURN VALUE  SDCC_STATUS: SDCC_NO_ERROR or SDCC_ERR_UNKNOWN
 *
 * SIDE EFFECTS  None
 *
 *===========================================================================*/
SDCC_STATUS
sdcc_get_config_info( sdcc_config_info_type *sdcc_pconfig_info )
{
   if ( NULL == sdcc_pconfig_info )
   {
      DEBUG ((EFI_D_ERROR, "Invalid config info pointer\n"));
      return SDCC_ERR_INVALID_PARAM;
   }

   /* total number of card slots supported */
   sdcc_pconfig_info->num_slots = SDCC_MAX_NUM_CONTROLLER;
   /* Block size supported by the SDCC controller */
   sdcc_pconfig_info->blk_size  = SDCC_DEFAULT_BLK_LENGTH_SIZE;
   /*
    * Maximum number of blocks can be requested at one time
    * If the value is 0, EFS takes the default hardcoded values.
    */
   sdcc_pconfig_info->max_blks  = 0;

   return SDCC_NO_ERROR;
} /* sdcc_get_config_info */

/*=============================================================================
 * FUNCTION      sdcc_slot_has_support
 *
 * DESCRIPTION   This function checks if a slot can be used
 *               for a certain type of device or not
 *
 * DEPENDENCIES  Slot configuration should exist in BSP
 *
 * PARAMETERS    driveno [IN]: drive number
 *               stype [IN] : type of slot (memory or IO)
 *
 * RETURN VALUE  FALSE
 *
 * SIDE EFFECTS  None
 *
 *===========================================================================*/
BOOLEAN
sdcc_slot_has_support (INT16 driveno, enum sdcc_slot_supports stype)
{
   sdcc_bsp_slot_type bsp_slot_type;
   BOOLEAN status = FALSE;

   if (!SDCC_DRIVENO_IS_VALID(driveno))
      return FALSE;

   /* Get Slot configuration from BSP */
   bsp_slot_type = sdcc_bsp_get_slot_type (driveno);

   if (bsp_slot_type & SDCC_BSP_EMBEDDED_MEM_SLOT)
   {
      /* Slot type is exclusive memory slot */
      if (SDCC_SLOT_MEMORY == stype)
         status = TRUE;
   }

   if (bsp_slot_type & SDCC_BSP_SD_SLOT)
   {
      /* Slot type is a open slot */
      status = TRUE;
   }

   return status;
}

/*=============================================================================
 * FUNCTION      sdcc_activity_timeout_event
 *
 * DESCRIPTION   This function is not supported in minimalist SDCC API
 *
 * DEPENDENCIES  None
 *
 * PARAMETERS    driveno [IN]: drive number
 *
 * RETURN VALUE  SDCC_ERR_UNKNOWN
 *
 *===========================================================================*/
SDCC_STATUS sdcc_activity_timeout_event( INT16 driveno )
{
   (void)driveno;
   return SDCC_ERR_UNKNOWN;
}

/******************************************************************************
* Name: sdcc_poll_card_status
*
* Description:
*    This function polls the card state till card is READY_FOR_DATA and in the
*    state indicated in parameter card_state.
*
* Parameters:
*    driveno    [IN] : a valid drive number between 0 and the total number of
*                      card slots - 1
*    card_state [IN] : card state such as transfer state, receive state, data
*                      state
*
* Returns:
*    SDCC_NO_ERROR on success
*    error code otherwise
*
******************************************************************************/
SDCC_STATUS sdcc_poll_card_status
(
   INT16 driveno,
   SDCC_CARD_STATUS card_state
)
{
   SDCC_STATUS rc_status = SDCC_ERR_CARD_READY;
   sdcc_dev_type   *sdcc_pdev  = NULL;
   UINT32 t = 0;
   sdcc_cmd_type sdcc_cmd;
   SDCC_CARD_STATUS current_card_state;
   UINT32 ready_for_data;
   sdcc_slot_type *pslot = NULL;

   if (!SDCC_DRIVENO_IS_VALID(driveno))
      return SDCC_ERR_INVALID_PARAM;

   pslot = sdcc_get_slot_handle (driveno);
   if (NULL == pslot)
   {
      DEBUG ((EFI_D_ERROR, "sdcc_poll_card_status: can't get slot handle\n"));
      return SDCC_ERR_INVALID_PARAM;
   }
   
   sdcc_pdev = &(pslot->dev);

   while ( t++ < SDCC_POLL_CARD_MAX )
   {
      SetMem(&sdcc_cmd, sizeof(sdcc_cmd), 0);

      sdcc_cmd.cmd = SD_CMD13_SEND_STATUS;
      sdcc_cmd.resp_type = SDCC_RESP_SHORT;
      sdcc_cmd.prog_scan = 0;
      sdcc_cmd.cmd_arg =  SDCC_ARG_RCA(sdcc_pdev->rca);
      sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

      (void) sdcc_command( sdcc_pdev, &sdcc_cmd );

      current_card_state = (SDCC_CARD_STATUS) SDCC_XLATE_CARD_CURRENT_STATE(sdcc_cmd.resp[0]);
      ready_for_data = SDCC_XLATE_READY_FOR_DATA(sdcc_cmd.resp[0]);

      if ( card_state == current_card_state &&
           SDCC_MEM_READY_FOR_DATA == ready_for_data )
      {
         rc_status = SDCC_NO_ERROR;
         break;
      }
   }

   return rc_status;
} /* sdcc_poll_card_status */

#ifndef USE_LOADER_UTILS
/******************************************************************************
* Name: sdcc_handle_erase
*
* Description:
*    This function is to erase data on the device, the commands used
*    to perform the erase operation should only be used on eMMC cards
*
* Arguments:
*    handle     [IN] : a pointer to a SDCC device
*    start_addr [IN] : the start of the sector
*    num_blocks [IN] : number of sectors to be erased
*
* Returns:
*    SDCC_NO_ERROR if successful.  Erro code otherwise
*
******************************************************************************/
SDCC_STATUS sdcc_handle_erase
(
   struct sdcc_device *handle,
   UINT32 start_addr,
   UINT32 num_blocks
)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_cmd_type sdcc_cmd;
   UINT32 driveno = 0;
   sdcc_dev_type  *sdcc_pdev   = NULL;
   UINT32 EraseGrpSize = 0, EraseGrpCount = 0, timeout_in_usec = 0;
   emmc_erase_type erase_type = EMMC_ERASE;

   if ( NULL == handle || NULL == handle->hdev )
   {
      DEBUG ((EFI_D_ERROR, "Invalid SDCC handle 0x%x", handle));
      return SDCC_ERR_INVALID_PARAM;
   }

   driveno = SDCC_HANDLE_GET_DRIVENO(handle); 

   if ( !SDCC_DRIVENO_IS_VALID(driveno) )
   {
      DEBUG ((EFI_D_ERROR, "Invalid drive number %d", driveno));
      return SDCC_ERR_INVALID_PARAM;
   }

   sdcc_pdev = handle->hdev; 

   if ( (SDCC_CARD_MMCHC != sdcc_pdev->card_type ) && 
        (SDCC_CARD_MMC != sdcc_pdev->card_type ) )
   {
      DEBUG ((EFI_D_ERROR, "Erase not supported for the inserted card"));
      return SDCC_ERR_FEATURE_UNSUPPORTED;
   }

   EraseGrpSize = sdcc_pdev->mem.mem_info.erase_group_size_in_sectors;

   if ((start_addr % EraseGrpSize) || (num_blocks % EraseGrpSize))
   {
      erase_type = EMMC_TRIM;
   }
   else
   {
      erase_type = EMMC_ERASE;
   }

   if (erase_type == EMMC_ERASE)
   {
      if (sdcc_pdev->mem.hc_erase_timeout_in_ms)
      {
         /* If multiple erase groups are being erased, the total timeout value */
         /* should be the multiple of the number of erase groups multiply the timeout */
         /* for each erase group. Refer to ERASE_TIMEOUT_MULT definition */
         /* of the JEDEC Standard No. 84-A441 */
         EraseGrpCount = num_blocks / EraseGrpSize;
         timeout_in_usec = EraseGrpCount * sdcc_pdev->mem.hc_erase_timeout_in_ms * 1000;
      }
      else
      {
         /* SDCC_PROG_DONE_MAX is defined in the unit of micro second */
         timeout_in_usec = SDCC_PROG_DONE_MAX;
      }
   }
   else if (erase_type == EMMC_TRIM)
   {
      if (num_blocks % EraseGrpSize)
      {
         EraseGrpCount = ((num_blocks / EraseGrpSize) + 1);
      }
      else
      {
         EraseGrpCount = num_blocks / EraseGrpSize;
      }

      if (sdcc_pdev->mem.trim_timeout_in_ms)
      {
         timeout_in_usec = EraseGrpCount * sdcc_pdev->mem.trim_timeout_in_ms * 1000; 
      }
      else
      {
         /* SDCC_PROG_DONE_MAX is defined in the unit of micro second */
         timeout_in_usec = SDCC_PROG_DONE_MAX;
      }
   }

   /* set the client handle partition number before data transfer */
   rc = sdcc_set_partition( handle );

   if ( SDCC_NO_ERROR != rc )
   {
      /* failed to select the requested partition number */
      return rc;
   }

   /* CMD35: set erase start */
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.cmd_arg   = start_addr;
   sdcc_cmd.cmd       = SD_CMD35_TAG_ERASE_GROUP_START;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   rc = sdcc_command(sdcc_pdev, &sdcc_cmd);

   if (SDCC_NO_ERROR != rc)
   {
      return rc;
   }

   /* CMD36: set erase end */
   if (sdcc_pdev->card_type == SDCC_CARD_MMCHC)
   {
      sdcc_cmd.cmd_arg = start_addr + num_blocks - 1; 
   }
   else 
   {
      sdcc_cmd.cmd_arg = start_addr + (num_blocks - 1) * SDCC_DEFAULT_BLK_LENGTH_SIZE; 
   }

   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.cmd       = SD_CMD36_TAG_ERASE_GROUP_END;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   rc = sdcc_command(sdcc_pdev, &sdcc_cmd);
   if (SDCC_NO_ERROR != rc)
   {
      return rc;
   }
  
   /* CMD38: erase those blocks */
   sdcc_cmd.prog_scan = 1;
   sdcc_cmd.cmd       = SD_CMD38_ERASE;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   if (erase_type == EMMC_TRIM)
   {
      /* CMD38: Trim those blocks */
      sdcc_cmd.cmd_arg   = 1;
   } 
   else if (erase_type == EMMC_ERASE)
   {
      /* CMD38: erase those blocks */
      sdcc_cmd.cmd_arg   = 0;
   }

   rc = sdcc_command(sdcc_pdev, &sdcc_cmd);

   /* if response indicates error, return immediately */
   if (sdcc_cmd.resp[0] & SDCC_CARD_ERASE_FAIL)
   {
      return SDCC_ERR_ERASE;
   }
   
   /* Erase command can take multiple of seconds to complete, if the sdcc_command 
    * indicates program done error, continue to wait */
   if (SDCC_ERR_PROG_DONE == rc)     
   {
      rc = sdcc_wait_prog_done (sdcc_pdev, timeout_in_usec);
      if (SDCC_NO_ERROR != rc)
      {
         DEBUG ((EFI_D_ERROR, "sdcc_erase failed\n"));
      }
   }
   
   return rc;
} /* sdcc_handle_erase */

#else
SDCC_STATUS sdcc_handle_erase
(
   struct sdcc_device *handle,
   UINT32 start_addr,
   UINT32 num_blocks
)
{
   return SDCC_ERR_FEATURE_UNSUPPORTED;
}
#endif

/******************************************************************************
* Name: sdcc_handle_secure_erase
*
* Description:
*    This function is to securely erase data on the device, the commands used
*    to perform the secure erase operation should only be used on eMMC cards
*
* Arguments:
*    handle     [IN] : a pointer to a SDCC device
*    start_addr [IN] : the start of the sector
*    num_blocks [IN] : number of sectors to be erased
*
* Returns:
*    SDCC_NO_ERROR if successful.  Erro code otherwise
*
******************************************************************************/
SDCC_STATUS sdcc_handle_secure_erase
(
   struct sdcc_device *handle,
   UINT32 start_addr,
   UINT32 num_blocks
)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_cmd_type sdcc_cmd;
   UINT32 driveno = 0;
   sdcc_dev_type  *sdcc_pdev   = NULL;
   UINT32 EraseGrpSize = 0, EraseGrpCount = 0;
   UINT64 timeout_in_usec = 0;

   if ( NULL == handle || NULL == handle->hdev )
   {
      DEBUG ((EFI_D_ERROR, "Invalid SDCC handle 0x%x", handle));
      return SDCC_ERR_INVALID_PARAM;
   }

   driveno = SDCC_HANDLE_GET_DRIVENO(handle); 

   if ( !SDCC_DRIVENO_IS_VALID(driveno) )
   {
      DEBUG ((EFI_D_ERROR, "Invalid drive number %d", driveno));
      return SDCC_ERR_INVALID_PARAM;
   }

   sdcc_pdev = handle->hdev;

   if ( sdcc_pdev->mem.mmcv45_support != TRUE || sdcc_pdev->mem.sanitize_support != TRUE)
   {
      DEBUG ((EFI_D_WARN, "Sanitize feature not supported by eMMC card", driveno));
      return SDCC_ERR_FEATURE_UNSUPPORTED;
   }

   EraseGrpSize = sdcc_pdev->mem.mem_info.erase_group_size_in_sectors;
   if (num_blocks % EraseGrpSize)
   {
        EraseGrpCount = ((num_blocks / EraseGrpSize) + 1);
   }
   else
   {
        EraseGrpCount = num_blocks / EraseGrpSize;
   }

   if (sdcc_pdev->mem.trim_timeout_in_ms)
   {
       timeout_in_usec = EraseGrpCount * sdcc_pdev->mem.trim_timeout_in_ms * 1000; 
   }
   else
   {
       /* SDCC_PROG_DONE_MAX is defined in the unit of micro second */
       timeout_in_usec = SDCC_PROG_DONE_MAX;
   }

   /* set the client handle partition number before data transfer */
   rc = sdcc_set_partition( handle );

   if ( SDCC_NO_ERROR != rc )
   {
       /* failed to select the requested partition number */
       DEBUG ((EFI_D_ERROR, "failed to select the requested partition number %d", driveno));
       return rc;
    }

   /* CMD35: set erase start */
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.cmd_arg   = start_addr;
   sdcc_cmd.cmd       = SD_CMD35_TAG_ERASE_GROUP_START;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   rc = sdcc_command(sdcc_pdev, &sdcc_cmd);

   if (SDCC_NO_ERROR != rc)
   {
      return rc;
   }

   /* CMD36: set erase end */
   sdcc_cmd.cmd_arg   = start_addr + num_blocks - 1;
   sdcc_cmd.cmd       = SD_CMD36_TAG_ERASE_GROUP_END;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;
   sdcc_cmd.prog_scan = 0;

   rc = sdcc_command(sdcc_pdev, &sdcc_cmd);
   if (SDCC_NO_ERROR != rc)
   {
      return rc;
   }

   /* CMD38: Discard those blocks */
   sdcc_cmd.cmd       = SD_CMD38_ERASE;
   sdcc_cmd.prog_scan = 1;
   sdcc_cmd.cmd_arg   = 3;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   rc = sdcc_command(sdcc_pdev, &sdcc_cmd);

   /* if response indicates error, return immediately */
   if (sdcc_cmd.resp[0] & SDCC_CARD_ERASE_FAIL)
   {
      return SDCC_ERR_ERASE;
   }
   
   if (SDCC_ERR_PROG_DONE == rc)     
   {
      /* wait for finishing program the device */
      rc = sdcc_wait_prog_done(sdcc_pdev, timeout_in_usec);	  
      if (SDCC_NO_ERROR != rc)
      {
        DEBUG ((EFI_D_ERROR, "sdcc_secure_erase failed %d\n", rc));
         return rc;
      }
   }

   rc = sdcc_mmc_switch(sdcc_pdev, SDCC_EMMC_SANITIZE_START);
   if (rc != SDCC_NO_ERROR)
   {
      DEBUG ((EFI_D_ERROR, " Sanitize failed with error %d\n", rc));
      return rc;
   }

   return rc;
} /* sdcc_handle_secure_erase */

/******************************************************************************
* Name: sdcc_enable_hw_reset
*
* Description:
*    This function enable the hardware reset function provided by the MMC
*    hardware.  Note, this function only work once.  Once reset is enabled
*    on a MMC device, it can not be disabled in the future.  This is a 
*    non-volatile update.
*
* Arguments:
*    handle     [IN] : a pointer to a SDCC device
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS sdcc_enable_hw_reset (struct sdcc_device *handle)
{
   SDCC_STATUS    rc = SDCC_NO_ERROR;
   UINT32         driveno = 0;
   sdcc_dev_type *sdcc_pdev   = NULL;

   if ( NULL == handle || NULL == handle->hdev )
   {
      DEBUG ((EFI_D_ERROR, "Invalid SDCC handle 0x%x", handle));
      return SDCC_ERR_INVALID_PARAM;
   }

   driveno = SDCC_HANDLE_GET_DRIVENO(handle);
   sdcc_pdev = handle->hdev;

   if ( !SDCC_DRIVENO_IS_VALID(driveno) )
   {
      DEBUG ((EFI_D_ERROR, "Invalid drive number %d", driveno));
      return SDCC_ERR_INVALID_PARAM;
   }

   if ( SDCC_CARD_MMCHC != sdcc_pdev->card_type )
   {
      DEBUG ((EFI_D_ERROR, "Function not supported by inserted card", driveno));
      return SDCC_ERR_FEATURE_UNSUPPORTED;
   }
   
   rc = sdcc_mmc_switch(sdcc_pdev, SDCC_EMMC_ENABLE_HW_RESET);
   if (rc != SDCC_NO_ERROR)
   {
      DEBUG ((EFI_D_ERROR, "Failed in changing bus width %d", rc));
      return rc;
   }

   return rc;
}

/******************************************************************************
* Name: sdcc_handle_set_active_bootable_partition
*
* Description:
*    This function sets a bootable partition on the embedded device specified
*    in the SDCC \c handle.  Only one partition can be set as active bootable
*    partition -- boot partition 1, boot partition 2, or user area.
*
* Parameters:
*    handle     [IN] : a pointer to the SDCC device that was returned from
*                      sdcc_handle_open
*
* Returns:
*    TRUE if memory device is present.  FALSE otherwise.
*
******************************************************************************/
SDCC_STATUS
sdcc_handle_set_active_bootable_partition( struct sdcc_device *handle )
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_dev_type *sdcc_pdev = NULL;
   UINT32 driveno = 0;

   if (handle == NULL || handle->hdev == NULL)
   {
      return SDCC_ERR_INVALID_PARAM;
   }

   driveno = SDCC_HANDLE_GET_DRIVENO(handle);
   if (!SDCC_DRIVENO_IS_VALID(driveno))
   {
      return SDCC_ERR_INVALID_PARAM;
   }

   do
   {
      sdcc_pdev = handle->hdev;

      if (sdcc_pdev->host_state != SDCC_HOST_TRAN &&
          sdcc_handle_silent_reinit(handle) == FALSE)
      {
         rc = SDCC_ERR_CARD_INIT;
         break;
      }

      /* only handle setting active boot partition for eMMC device */
      if (sdcc_pdev->card_type == SDCC_CARD_MMC ||
          sdcc_pdev->card_type == SDCC_CARD_MMCHC)
      {
         rc = sdcc_mmc_set_active_bootable_partition(handle);
      }
      else
      {
         rc = SDCC_ERR_FEATURE_UNSUPPORTED;
      }
   } while (0);

   return rc;
} /* sdcc_handle_set_active_bootable_partition */

#ifndef USE_LOADER_UTILS
/******************************************************************************
* Name: sdcc_handle_set_power_on_write_protection
*
* Description:
*    This function sets the power-on write protection bit of the addressed
*    group provided the memory device specified in the SDCC handle is an
*    embedded device and write protect feature is supported on the device.
*    The power-on write protection is set based on the write protect group.
*    The write protect group size is memory device specific.  This group size
*    information write_protect_group_size_in_sectors can be queried via
*    sdcc_handle_mem_get_device_info.  For boot area partitions, power-on write
*    protection would be applied to the entire partition and both boot areas
*    would be power-on write protected.
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open
*    data_address  [IN] : start of the sector to get power-on write protected
*                         This is the sector unit address.  This is ignored for
*                         boot partition.
*    num_of_blocks [IN] : number of blocks to be power-on write protected
*                         (512 bytes per block).  For user area partition, this
*                         is based on the write protect group size.  If
*                         num_of_blocks < write protect group size, the entire
*                         write protect group is write protected.  This is
*                         ignored for boot partition.
*
* Returns:
*    SDCC_NO_ERROR on success.  Error code otherwise
*
* Note:
*    After a power failure or hardware reset, the power-on write protection
*    must be reapplied.
*
******************************************************************************/
SDCC_STATUS sdcc_handle_set_power_on_write_protection
(
   struct sdcc_device *handle,
   UINT32 data_address,
   UINT32 num_of_blocks
)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_slot_type *sdcc_pdata = NULL;
   sdcc_dev_type *sdcc_pdev = NULL;
   UINT32 driveno = 0;
   UINT32 WriteProtectGrpSize = 0;

   if ((NULL == handle) || (NULL == handle->hdev))
   {
      DEBUG ((EFI_D_ERROR, "Invalid SDCC handle"));
      return SDCC_ERR_INVALID_PARAM;
   }

   sdcc_pdev = handle->hdev;
   sdcc_pdata = sdcc_pdev->pslot;
   driveno = sdcc_pdata->driveno;

   if (!SDCC_DRIVENO_IS_VALID(driveno))
   {
      DEBUG ((EFI_D_ERROR, "Invalid drive number %d", driveno));
      return SDCC_ERR_INVALID_PARAM;
   }

   /* For User area, the start sector must allign with WP_GRP_SIZE and number
      of sectors to be protected must be multiple of WP_GRP_SIZE */ 
   if (SDCC_EMMC_IS_FROM_USER_AREA_PART(handle->phy_partition_num))
   {
      WriteProtectGrpSize = 
         sdcc_pdev->mem.mem_info.write_protect_group_size_in_sectors;

      if ((num_of_blocks % WriteProtectGrpSize) || 
          (data_address % WriteProtectGrpSize))
      {
         return SDCC_ERR_INVALID_PARAM;       
      }
   }

   do
   {
      if (sdcc_pdev->host_state != SDCC_HOST_TRAN &&
          sdcc_handle_silent_reinit(handle) == FALSE)
      {
         rc = SDCC_ERR_CARD_INIT;
         break;
      }

      /* only handle write protection for eMMC device */
      if ((sdcc_pdev->card_type == SDCC_CARD_MMC ||
          sdcc_pdev->card_type == SDCC_CARD_MMCHC) &&
          sdcc_pdev->mem.write_prot_support == TRUE)
      {
         rc = sdcc_mmc_set_pwr_on_write_prot(handle, data_address, num_of_blocks);
      }
      else
      {
         DEBUG ((EFI_D_ERROR, "Write protect feature not supported in driveno %d", driveno));
         rc = SDCC_ERR_FEATURE_UNSUPPORTED;
      }
   } while (0);

   return rc;
} /* sdcc_handle_set_power_on_write_protection */

#else
SDCC_STATUS sdcc_handle_set_power_on_write_protection
(
   struct sdcc_device *handle,
   UINT32 data_address,
   UINT32 num_of_blocks
)
{
   return SDCC_ERR_FEATURE_UNSUPPORTED;
} /* sdcc_handle_set_power_on_write_protection */
#endif

/******************************************************************************
* Name: sdcc_handle_get_write_protection_type
*
* Description:
* This function retrieves the type of write protection that is set for the different 
* write protection groups. Each set of two protection bits shows the type of protection
* set for each of the write protection groups. The definition of the different bit
* settings are shown below. The last (least significant) two bits of the protection 
* bits correspond to the first addressed group. 
* 
* b00: Write protection group not protected
* b01: Write protection group is protected by temporary write protection
* b10: Write protection group is protected by power-on write protection
* b11: Write protection group is protected by permanent write protection
*
* Parameters:
*    handle [IN]           : a pointer to the SDCC device that was returned from
*                            sdcc_handle_open
* 
*    data_address [IN]     : starting address of the write protection group to retrieve
*                            the write protection bits
*                                           
*    write_prot_type [OUT] : A pointer to 8-byte array. This array contains 64 write
*                            protection bits (representing 32 write protect
*                            groups starting at the specified data_address)
*
* Returns:
*    SDCC_NO_ERROR on success.  Error code otherwise
*
* Note:
*
******************************************************************************/
SDCC_STATUS sdcc_handle_get_write_protection_type
(
   struct sdcc_device *handle,
   UINT32 data_address,
   UINT8  *write_prot_type
)
{
   UINT32 driveno = 0;
   sdcc_slot_type  *sdcc_pdata = NULL;
   sdcc_dev_type   *sdcc_pdev   = NULL;
   SDCC_STATUS rc = SDCC_NO_ERROR;

   if ( NULL == handle || NULL == handle->hdev || 
        NULL == write_prot_type)
   {
      return SDCC_ERR_INVALID_PARAM;
   }

   sdcc_pdev = handle->hdev;
   sdcc_pdata = sdcc_pdev->pslot;
   driveno = sdcc_pdata->driveno;

   if ( !SDCC_DRIVENO_IS_VALID(driveno) )
   {
      DEBUG ((EFI_D_ERROR, "Invalid drive number %d", driveno));
      return SDCC_ERR_INVALID_PARAM;
   }

   do
   {
      if (sdcc_pdev->host_state != SDCC_HOST_TRAN &&
          sdcc_handle_silent_reinit(handle) == FALSE)
      {
         rc = SDCC_ERR_CARD_INIT;
         break;
      }

      /* only support write protection for eMMC device */
      if ((sdcc_pdev->card_type == SDCC_CARD_MMC ||
          sdcc_pdev->card_type == SDCC_CARD_MMCHC) &&
          sdcc_pdev->mem.write_prot_support == TRUE)
      {
         rc = sdcc_mmc_get_write_prot_type(handle, data_address, write_prot_type);
      }
      else
      {
         DEBUG ((EFI_D_ERROR, "Write protect not supported in driveno %d", driveno));
         rc = SDCC_ERR_FEATURE_UNSUPPORTED;
      }
   } while (0);

   return rc;
}

/******************************************************************************
* Name: sdcc_boot_set_device_info
*
* Description:
*    This function sets/sends the device information from boot to
*    the driver to perform partial initialization.
*    This function will be called at DBL/QCSBL to set the pass
*    the device info obtained from PBL
*
* Arguments:
*    device_info       [IN] : pointer to device info
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
void sdcc_boot_set_device_info( sdcc_boot_device_info_type *device_info )
{
   if (device_info != NULL)
   {
      /* currently we support this feature only for MMC */
      if ((device_info->card_type == SDCC_CARD_MMC) ||
          (device_info->card_type == SDCC_CARD_MMCHC))
      {
         sdcc_mini_boot_device_info.driveno = device_info->driveno;
         sdcc_mini_boot_device_info.card_type = device_info->card_type;
         sdcc_mini_boot_device_info.rca = device_info->rca;
      }
   }
} /* sdcc_boot_set_device_info */

#ifndef USE_LOADER_UTILS
/******************************************************************************
* Name: sdcc_config_emmc_gpp_enh
*
* Description:
*    This function configures the General Purpose Partition and 
*    enhanced user data area information
*    Note: This routine can only be used once for the life of each
*    emmc chip!
* 
*
* Arguments:
*    handle     [IN] : a pointer to a SDCC device
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise
*
******************************************************************************/
SDCC_STATUS sdcc_config_emmc_gpp_enh (struct sdcc_device *handle,
                                      sdcc_emmc_gpp_enh_desc_type *gpp_enh_desc)
{
   if ( NULL == handle || NULL == handle->hdev )
   {
      DEBUG ((EFI_D_ERROR, "Invalid SDCC handle 0x%x", handle));
      return SDCC_ERR_INVALID_PARAM;
   }
   
   return sdcc_mmc_config_gpp_enh (handle, gpp_enh_desc);
}
#else
SDCC_STATUS sdcc_config_emmc_gpp_enh (struct sdcc_device *handle,
                                      sdcc_emmc_gpp_enh_desc_type *gpp_enh_desc)
{
   return SDCC_ERR_FEATURE_UNSUPPORTED;
}
#endif

/*=============================================================================
 * FUNCTION      sdcc_get_slot_type
 *
 * DESCRIPTION   This function queries the type of sdcc slot
 *
 * PARAMETERS    driveno [IN]: drive number
 *               sdcc_slot_mem_type [OUT]: pointer to memory slot type. Slot is 
 *                                         emmc or sd memory device
 *               sdcc_slot_access_type [OUT]: pointer to slot access type. Slot is 
 *                                            internal or external memory device
 *
 * Returns:
 *    SDCC_NO_ERROR if successful.  Error code otherwise
 *
 * SIDE EFFECTS  None
 *
 *===========================================================================*/
SDCC_STATUS sdcc_get_slot_type( INT16 driveno, sdcc_slot_mem_type* slot_mem_type,
                                sdcc_slot_access_type* slot_access_type)
{
   sdcc_bsp_slot_type bsp_slot_mem_type = SDCC_BSP_NO_SLOT;
   sdcc_bsp_slot_access_type bsp_slot_access_type = SDCC_BSP_NO_SLOT;
   
   /* Get Slot configuration from BSP */
   bsp_slot_mem_type = sdcc_bsp_get_slot_type (driveno);

   if (bsp_slot_mem_type & SDCC_BSP_EMBEDDED_MEM_SLOT)
   {
      *slot_mem_type = SDCC_SLOT_EMMC;
   }
   else if (bsp_slot_mem_type & SDCC_BSP_SD_SLOT)
   {
      /* Slot type is a SD slot */
      *slot_mem_type = SDCC_SLOT_SD;
   }
   else
   {
      *slot_mem_type = SDCC_SLOT_UNKNOWN;   
   }

   /* Get External or Internal Slot configuration from BSP */
   bsp_slot_access_type = sdcc_bsp_get_slot_access_type (driveno);
   
   if (bsp_slot_access_type & SDCC_BSP_SLOT_INTERNAL)
   {
      /* Slot type is a internal slot */
      *slot_access_type = SDCC_SLOT_INTERNAL;
   }
   else if (bsp_slot_access_type & SDCC_BSP_SLOT_EXTERNAL)
   {
      /* Slot type is a external slot */
      *slot_access_type = SDCC_SLOT_EXTERNAL;
   }
   else
   {
      *slot_access_type = SDCC_SLOT_INVALID;   
   }
   
   return SDCC_NO_ERROR;
}

/******************************************************************************
 *  Name: sdcc_sdhci_mode_enable
 *
 * Description:
 *    This function enables the Host Controller specification.
 *    If the SDCC is not in SDHCi mode, it resets the SDCC's to SDHCi mode
 *
 * Arguments: None
 *
 * Returns:
 *	  SDCC_NO_ERROR if successful.	Error code otherwise
*
******************************************************************************/
SDCC_STATUS sdcc_sdhci_mode_enable (void)
{
   sdcc_bsp_clk_type clk_freq; 
   UINT32 slot_num = 0;
   UINT32 extSDSlot = PcdGet32(SDCCExtSlotNumber);
   UINT32 eMMCSlot = PcdGet32(SDCCEmmcSlotNumber);
   UINT32 maxSlot = PcdGet32(SDCCMaxSlotNumber);

   for (slot_num = 0; slot_num < maxSlot; slot_num++)
   {
      if ((eMMCSlot == slot_num) && (!boot_from_emmc ()))
      {         
         continue; 
      }

      if ((extSDSlot == slot_num) || (eMMCSlot == slot_num))
      {
         sdcc_bsp_gpio_control(slot_num, SDCC_BSP_GPIO_ON);

         /* Make sure SDCC's clocks are enabled */
         sdcc_ahb_clk_init (slot_num);
         sdcc_enable_mclk (slot_num);

         clk_freq = sdcc_bsp_get_max_supported_clk (slot_num);
         if (SDCC_BSP_CLK_INVALID != clk_freq)
         {
            sdcc_set_mclk_frequency (slot_num, clk_freq);
         }
         
         /* Reset the controller */
         sdcc_reset_mclk (slot_num);

         /* Reset SDHCi */
         HAL_sdhci_InitializeRegisters(); 
         HAL_sdhci_SetPower (slot_num, TRUE);
         HAL_sdhci_SoftwareReset(slot_num, HAL_SDHCI_RESET_ALL);

         /* Set the Capabilities supported by the SDCC controller */
         HAL_sdhci_SetCapabilities(slot_num);        
      }
      if (eMMCSlot == slot_num)
      {
         /* Disable HC Select In for HLOS */
         HAL_sdhci_HCSelectInDisable(slot_num);
      }   
   }

   return SDCC_NO_ERROR;
}

/******************************************************************************
* Name: sdcc_mmc_handle_fw_update
*
* Description:
*    This function performs a firmware update for MMC devices.
*
* Arguments:
*    handle           [IN] : a valid MMC device pointer.
*    buff             [IN] : pointer to buffer containing the firmware binary
*    num_blocks       [IN] : number of blocks to be written (512 bytes per
*                            block)
* Returns:
*    SDCC_NO_ERROR if successful
*    Error code otherwise
*
******************************************************************************/
static SDCC_STATUS sdcc_mmc_handle_fw_update
(
  struct sdcc_device *handle,
  UINT8               *buff,
  UINT16              num_blocks
)
{
   sdcc_dev_type  *sdcc_pdev   = NULL;
   sdcc_cmd_type   sdcc_cmd;
   SDCC_STATUS rc = SDCC_NO_ERROR;
   UINT32  ffu_arg;

   sdcc_pdev = handle->hdev;

   /* Set the client handle partition number before data transfer */
   rc = sdcc_set_partition ( handle );
   if ( SDCC_NO_ERROR != rc )
   {
      DEBUG ((EFI_D_INFO, "Failed to select requested partition %d", rc));
      return rc;
   }

   if ( sdcc_mmc_get_ext_csd_rev (handle) < SDCC_MMC_EXT_CSD_REV_FOR_5_0 )
   {
      DEBUG ((EFI_D_INFO, "MMC device is not 5.0 Compliant"));
      return SDCC_ERR_OP_NOT_SUPPORTED;
   }

   /* Check if FFU supported by device and not disabled */
   if (!sdcc_mmc_is_ffu_allowed ( handle ))
   {
      DEBUG ((EFI_D_INFO, "MMC device does not support FW UPdate"));
      return SDCC_ERR_OP_NOT_SUPPORTED;
   }

   ffu_arg = sdcc_mmc_get_ffu_argument ( handle );

   rc = sdcc_mmc_set_mode_config ( handle, SDCC_MMC_MODE_CONFIG_FFU );
   if ( rc != SDCC_NO_ERROR)
   {
      DEBUG ((EFI_D_INFO, "Could not set mode config to FFU"));
      return rc;
   }

   /* Download FW to device. CMD25 + FFU_ARG */
   sdcc_cmd.cmd = SD_CMD25_WRITE_MULTIPLE_BLOCK;
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.cmd_arg = ffu_arg;
   sdcc_cmd.flags = SDCC_TRANSFER_FLAG_IS_WRITE_TRANSFER;
   rc = sdcc_do_transfer ( sdcc_pdev, &sdcc_cmd, buff, num_blocks );

   if ( SDCC_NO_ERROR != rc )
   {
      DEBUG ((EFI_D_INFO, "sdcc_do_transfer failed for write %d", rc));
      return rc;
   }

   /* Done with FFU download. Set mode to normal */
   rc = sdcc_mmc_set_mode_config ( handle, SDCC_MMC_MODE_CONFIG_NORMAL );
   if ( SDCC_NO_ERROR != rc )
   {
      DEBUG ((EFI_D_INFO, "Failed to set mode config to Normal"));
      return rc;
   }

   return rc;
}

/******************************************************************************
* Name: sdcc_handle_fw_update
*
* Description:
*    This function performs a firmware update using the fw data supplied.
*
* Arguments:
*    handle           [IN] : a pointer to the SDCC device that was returned
*                            from sdcc_handle_open()
*    buff             [IN] : pointer to buffer containing the firmware binary
*    num_blocks       [IN] : number of blocks to be written (512 bytes per
*                            block)
*
* Returns:
*    SDCC_NO_ERROR if successful
*    Error code otherwise
*
******************************************************************************/
SDCC_STATUS sdcc_handle_fw_update
(
   struct sdcc_device *handle,
   UINT8               *buff,
   UINT16              num_blocks
)
{
   UINT32 driveno = 0;
   sdcc_dev_type *sdcc_pdev = NULL;
   SDCC_STATUS rc = SDCC_NO_ERROR;

   if ( NULL == handle || NULL == handle->hdev )
   {
      DEBUG ((EFI_D_INFO, "Invalid SDCC handle 0x%x", handle));
      return SDCC_ERR_INVALID_PARAM;
   }

   driveno = SDCC_HANDLE_GET_DRIVENO(handle);
   if ( !SDCC_DRIVENO_IS_VALID(driveno) )
   {
      DEBUG ((EFI_D_INFO, "Invalid drive number %d", driveno));
      return SDCC_ERR_INVALID_PARAM;
   }

   sdcc_pdev = handle->hdev;

   /* fail the call if card is removed or reinserted */
   if ( sdcc_pdev->pslot->slot_state_changed )
   {
      DEBUG ((EFI_D_INFO, "Slot %d state changed", driveno));
      return SDCC_ERR_CARD_REMOVED;
   }

   if ( SDCC_HOST_TRAN != sdcc_pdev->host_state )
   {
      if ( FALSE == sdcc_handle_silent_reinit(handle) )
      {
         rc = SDCC_ERR_CARD_INIT;
         return rc;
      }
   }

   switch (sdcc_pdev->card_type)
   {
     case SDCC_CARD_MMC:
     case SDCC_CARD_MMCHC:
     {
        rc = sdcc_mmc_handle_fw_update ( handle , buff, num_blocks );
     }
     break;

     case SDCC_CARD_UNKNOWN:
     {
        DEBUG ((EFI_D_INFO, "Card type unknown"));
        rc = SDCC_ERR_CARD_UNDETECTED;
     }
     break;

     default:
     {
       DEBUG ((EFI_D_INFO, "FW update operation not supported for card type %d",
                sdcc_pdev->card_type));
       rc = SDCC_ERR_OP_NOT_SUPPORTED;
     }
     break;
   }

   return rc;
}

/******************************************************************************
* Name: sdcc_handle_get_ffu_status
*
* Description:
*    This function returns the Status of the Firmware Update Operation.
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
*    ffu_status    [IN/OUT] : Buffer to hold the FFU Operation status
*
* Returns:
*    SDCC_NO_ERROR if successful
*    Error code otherwise
*
******************************************************************************/
SDCC_STATUS sdcc_handle_get_ffu_status  (struct sdcc_device *handle,
                                         int *ffu_status)
{
  /* sanity check the handle */
  if ( NULL == handle || NULL == handle->hdev )
  {
     DEBUG ((EFI_D_INFO, "Invalid SDCC handle"));
     return SDCC_ERR_INVALID_PARAM;
  }

  if ( NULL == ffu_status )
  {
     DEBUG ((EFI_D_INFO, "NULL Buffer to store FFU Status\n"));
     return SDCC_ERR_INVALID_PARAM;
  }

  *ffu_status = sdcc_mmc_get_ffu_status (handle);

  return SDCC_NO_ERROR;
}


/******************************************************************************
* Name: sdcc_handle_get_fw_version
*
* Description:
*    This function returns the device Firmware version.
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
*    fw_version    [IN/OUT] : Buffer to hold the FW Version
*
* Returns:
*    SDCC_NO_ERROR if successful
*    Error code otherwise
*
******************************************************************************/
SDCC_STATUS sdcc_handle_get_fw_version  (struct sdcc_device *handle,
                                         UINT64 *fw_version)
{
   /* sanity check the handle */
   if ( NULL == handle || NULL == handle->hdev )
   {
      DEBUG ((EFI_D_INFO, "Invalid SDCC handle"));
      return SDCC_ERR_INVALID_PARAM;
   }

   if ( NULL == fw_version )
   {
      DEBUG ((EFI_D_INFO, "NULL Buffer to store FW Version \n"));
      return SDCC_ERR_INVALID_PARAM;
   }

   *fw_version = sdcc_mmc_get_fw_version (handle);

   return SDCC_NO_ERROR;
}

/******************************************************************************
* Name: sdcc_deinit_device
*
* Description:
*  This function de-selects the device, turn off the power supply to the device
*  and switches off the controller clock.
*
* Arguments:
*    driveno           [IN] :  a valid drive number between 0 and the total
*                              number of card slots - 1
*
* Returns:
*    SDCC_NO_ERROR if successful
*    Error code otherwise
*
******************************************************************************/

SDCC_STATUS
sdcc_deinit_device ( INT16 driveno )
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_slot_type *sdcc_pdata = NULL;
   sdcc_dev_type *sdcc_pdev = NULL;

   if ( !SDCC_DRIVENO_IS_VALID(driveno) )
   {
      DEBUG ((EFI_D_INFO, "Invalid drive number %d for handle", driveno));
      return SDCC_ERR_INVALID_PARAM;
   }

   sdcc_pdata = sdcc_get_slot_handle (driveno);
   if (NULL == sdcc_pdata)
   {
      DEBUG ((EFI_D_INFO, "sdcc_deinit_device: can't get slot handle\n"));
      return SDCC_ERR_INVALID_PARAM;
   }
   
	 sdcc_pdev = &(sdcc_pdata->dev);

   if ( sdcc_is_device_still_in_use (sdcc_pdev) )
   {
      DEBUG ((EFI_D_INFO, "Card still in use"));
      return SDCC_ERR_CARD_IN_USE;
   }

   /* Deinitialize the device info */
   (void) sdcc_deinit ( sdcc_pdev );

   /* De-initialize the Boot device to avoid doing partial init the next time */
   SetMem (&sdcc_mini_boot_device_info, sizeof (sdcc_mini_boot_device_info), 0);

   return rc;
}

/*lint -restore */
