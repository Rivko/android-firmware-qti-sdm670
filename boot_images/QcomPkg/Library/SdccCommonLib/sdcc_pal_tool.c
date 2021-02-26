/******************************************************************************
 * sdcc_pal_tool.c
 *
 * This file provides SDCC PAL definitions for TARGET TOOL platform
 *
 * Copyright (c) 2014-2017 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *****************************************************************************/
/*======================================================================
                        Edit History

when         who     what, where, why
----------   ---     --------------------------------------------------- 
2017-05-12   sb      Add partial enumeration for eMMC 
2016-12-14   jt      Remove legacy code 
2016-01-18   eo      Reduced sdcc code size
2015-07-15   bn      Added SDHCi Flow Control
2015-01-26   rm      Integraged HS200/HS400 support for eMMC from UEFI 3.0
2014-12-03   bn      Separated Core and Loader clock libs
2014-11-10   bn      Add null pointer check for sdcc_get_slot_handle
2014-10-01   bn      Removed partial enum support temporarily
2014-09-24   bn      Added MCI_CLK(ENABLE) workaround for 8996 virtio 
2014-06-18   bn      Initial Revision. Branched from 8994 SBL
=======================================================================*/

/*======================================================================
                            INCLUDE FILES FOR MODULE
=======================================================================*/
#include "sdcc_priv.h"
#include "sdcc_util.h"
#include "sdcc_sd_util.h"
#include "sdcc_mmc_util.h"
#include "sdcc_handle_util.h"
#include "sdcc_pal_tool.h"
#include "sdcc_hc.h"
#include "SdccBsp.h"
#include "SdccCommon.h"
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/SdccCommonLib/SdccHalLib/SdccHal.h>

/*********************************************************************/
/*             PROTOTYPES FOR LOCAL FUNCTIONS                        */
/*********************************************************************/
static SDCC_STATUS sdcc_set_blksz (sdcc_dev_type *sdcc_pdev);
SDCC_STATUS sdcc_send_app_command(sdcc_dev_type *p_device, sdcc_cmd_type *p_app_command);
extern sdcc_mini_boot_device_info_type sdcc_mini_boot_device_info;

/******************************************************************************
* Name: sdcc_handle_find_card
*
* Description: 
*    This function finds out the card type specified in handle parameter. 
*
* Arguments:
*    handle [IN] : a pointer to the SDCC device that was returned from
*                  sdcc_handle_open()
*
* Returns:
*    card type that indicates if device is SD, MMC, SDIO, or unknown
*
******************************************************************************/
SDCC_CARD_TYPE sdcc_handle_find_card( struct sdcc_device *handle )
{
   SDCC_CARD_TYPE card_type = SDCC_CARD_UNKNOWN;
   UINT32 driveno = 0;

   driveno = SDCC_HANDLE_GET_DRIVENO( handle );
   if (!SDCC_DRIVENO_IS_VALID( driveno ))
   {
      DEBUG ((EFI_D_ERROR, "Invalid Drive Number:%d", driveno));
      return SDCC_CARD_UNKNOWN;
   }

   /* set the power mode to 'power on' */
   HAL_sdhci_SetPower( driveno, TRUE);
 
   HAL_sdhci_PowerSave( driveno, FALSE);
   sdcc_mdelay(SDCC_SUPPLY_RAMP_UP_TIME_MS);
   HAL_sdhci_PowerSave( driveno, TRUE);

   /* lower the clock to < 400KHz for card identification */
   sdcc_config_clk( handle->hdev, SDCC_IDENTIFICATION_MODE );
   /* look for memory card */

   if (sdcc_bsp_get_slot_type(driveno) & SDCC_BSP_EMBEDDED_MEM_SLOT)
   {
      /* look for MMC device */
      card_type = sdcc_find_mmc_device( handle );
   }
   else if (sdcc_bsp_get_slot_type(driveno) & SDCC_BSP_SD_SLOT)
   {
      /* look for SD device */
      card_type = sdcc_find_sd_device( handle );
   }

   return card_type;
} /* sdcc_handle_find_card */

/******************************************************************************
* Name: sdcc_config_rca
*
* Description:
*    This function is to set/get the RCA from the card.
*
* Arguments:
*    sdcc_pdev          [IN] : pointer to the device structure
*
* Returns:
*    rca value
*
******************************************************************************/
UINT16
sdcc_config_rca( sdcc_dev_type *sdcc_pdev )
{
   sdcc_cmd_type    sdcc_cmd;
   UINT16           rca = 0;

   sdcc_cmd.cmd       = SD_CMD3_SET_RELATIVE_ADDR;
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   /* CMD3: assign/get relative card address */
   if ( SDCC_CARD_MMC == sdcc_pdev->card_type ||
        SDCC_CARD_MMCHC == sdcc_pdev->card_type )
   {
      /* assign RCA to the card */
      rca               = 2;
      sdcc_cmd.cmd_arg  = SDCC_ARG_RCA(rca);

      (void)sdcc_command(sdcc_pdev, &sdcc_cmd);
   }
   else
   {
      sdcc_cmd.cmd_arg = SDCC_CMD_ARGU_NULL;
      (void)sdcc_command(sdcc_pdev, &sdcc_cmd);

      /* get RCA from the card */
      rca = (UINT16)(sdcc_cmd.resp[0] >> SDCC_SEND_STATUS_RCA_SHFT);
   }

   return rca;
} /* sdcc_config_rca */

/******************************************************************************
* Name: sdcc_wait_card_ready
*
* Description:
*    This function is to check the card busy status by calling "send
*    status" command.
*
* Arguments:
*    sdcc_pdev          [IN] : pointer to device structure
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS
sdcc_wait_card_ready( sdcc_dev_type *sdcc_pdev )
{
   UINT32               count       = 0;
   SDCC_STATUS          rc          = SDCC_ERR_CARD_READY;
   SDCC_CARD_STATUS     card_status = SDCC_CARD_IGNORE;

   while (count < SDCC_PROG_DONE_MAX)
   {
      card_status = sdcc_send_status(sdcc_pdev);

      if ( SDCC_CARD_TRAN == card_status )
      {
         rc = SDCC_NO_ERROR;
         break;
      }

      sdcc_udelay(10);
      count += 10; 
   }

   return rc;
} /* sdcc_wait_card_ready */

/******************************************************************************
* Name: sdcc_init_memory_device
*
* Description:
*    This function sends CMD2 to get the device ID and product related
*    information.  It then configures the device Relative Card Address and
*    completes the card identification process.
*
* Arguments:
*    sdcc_pdev          [IN/OUT] : pointer to device structure
*
* Returns:
*    Returns error code
*
* Note:
*   Card is in Identification mode.
*
*****************************************************************************/
SDCC_STATUS
sdcc_init_memory_device( sdcc_dev_type  *sdcc_pdev )
{
   sdcc_cmd_type   sdcc_cmd;
   SDCC_STATUS     rc = SDCC_ERR_UNKNOWN;
   BOOLEAN         decode_status = FALSE;

   /* CMD2: request card ID */
   sdcc_cmd.cmd       = (UINT32)SD_CMD2_ALL_SEND_CID;
   sdcc_cmd.cmd_arg   = SDCC_CMD_ARGU_NULL;
   sdcc_cmd.resp_type = SDCC_RESP_LONG;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   rc = sdcc_command(sdcc_pdev, &sdcc_cmd);
   if (SDCC_NO_ERROR != rc)
   {
      return rc;
   }

   if ( SDCC_CARD_MMC == sdcc_pdev->card_type ||
        SDCC_CARD_MMCHC == sdcc_pdev->card_type )
   {
      decode_status = sdcc_decode_mmc_cid( &sdcc_cmd.resp[0], sdcc_pdev );
      if ( FALSE == decode_status )
      {
         return SDCC_ERR_INVALID_PARAM;
      }
   }
   else
   {
      /* Card type is SD or SDHC. */
      decode_status = sdcc_decode_sd_cid( &sdcc_cmd.resp[0], sdcc_pdev );
      if ( FALSE == decode_status )
      {
         return SDCC_ERR_INVALID_PARAM;
      }
   }

   /* CMD3: configure card RCA */
   sdcc_pdev->rca = sdcc_config_rca(sdcc_pdev);

   return (rc);
} /* sdcc_init_memory_device */

/******************************************************************************
* Name: sdcc_config_memory_device
*
* Description:
*    This function is to programm the card into data transfer mode.  It
*    selects the card, sets up the bus width and block data transfer mode.
*
* Arguments:
*    sdcc_pdev          [IN/OUT] : pointer to device structure
*
* Returns:
*    Returns error code
*
* Note:
*    Card is in data transfer mode.
*
******************************************************************************/
SDCC_STATUS
sdcc_config_memory_device( sdcc_dev_type  *sdcc_pdev )
{
   SDCC_STATUS      rc          = SDCC_NO_ERROR;
   SDCC_CARD_STATUS card_status = SDCC_CARD_IGNORE;
   UINT32           driveno     = sdcc_pdev->driveno;
   sdcc_scr_type    scr;
   SDCC_MMC_BUSWIDTH bus_width;

   /* CMD7: select the card */
   rc = sdcc_select_card(sdcc_pdev, TRUE);
   if (SDCC_NO_ERROR != rc)
   {
      return rc;
   }

   sdcc_pdev->host_state = SDCC_HOST_TRAN;

   /* CMD16: set the data block length */
   sdcc_pdev->mem.block_mode = SDCC_MEM_BLK_MODE;
   rc = sdcc_set_blksz(sdcc_pdev);

   /* program the bus width for MMC */
   /* It has to be programmed after setting the block size */
   /* because adtc command will be sent to the MMC card */
   /* that has spec version 4.x. */
   if ( SDCC_CARD_MMC == sdcc_pdev->card_type ||
        SDCC_CARD_MMCHC == sdcc_pdev->card_type )
   {
      /* default MMC bus width to 1-bit mode */
      bus_width = SDCC_MMC_BUSWIDTH_1BIT;

      /* If MMC denotes a System Specification version 4.0 or higher, */
      /* this indicates the support of the SEND_EXT_CSD command (CMD8). */
      if ( sdcc_pdev->mem.spec_vers >= 4 )
      {
         rc = sdcc_config_mmc_modes_segment( sdcc_pdev );
         if ( SDCC_NO_ERROR != rc )
         {
            return rc;
         }
      }
      else
      {
         /* configure MMC bus width */
         rc = sdcc_hc_config_buswidth (driveno, bus_width);
         if (rc != SDCC_NO_ERROR)
         {
            DEBUG ((EFI_D_ERROR, "SDCC failed to configure bus width %d in SDHCi mode", rc));
            return rc;
         }
      }
   }
   else if ((SDCC_CARD_SD   == sdcc_pdev->card_type) ||
            (SDCC_CARD_SDHC == sdcc_pdev->card_type))
   {
      /* init num_phy_partition_created to 1 for now until new func to */
      /* create partition is in place */
      sdcc_pdev->mem.mem_info.num_phy_partition_created = 1;

      SetMem(&scr, sizeof(sdcc_scr_type), 0);

      /* read the SD configuration register (SCR) -- ACMD51 */
      rc = sdcc_get_scr( sdcc_pdev, &scr );
      if ( SDCC_NO_ERROR != rc )
      {
         return rc;
      }

      if ( SDCC_SD_WIDE_BUS_WIDTH == scr.sd_bus_widths )
      {
         /* support 4-bit bus mode -- ACMD6 */
         rc = sdcc_set_sd_bus_width( sdcc_pdev );
         if ( SDCC_NO_ERROR != rc )
         {
            return rc;
         }
      }
      else
      {
         /* support 1-bit bus mode */
         /* by default, the SD card data bus width is 1-bit */
         rc = sdcc_hc_config_buswidth(driveno, SDCC_MMC_BUSWIDTH_1BIT);
      }

      /* send ACMD13 to get the speed class from SD Status */
      /* if the SD card supports the high speed access mode */
      rc = sdcc_get_sd_speed_class( sdcc_pdev );
      if ( SDCC_NO_ERROR != rc )
      {
         /* re-init if getting SD Status failed */
         return rc;
      }

      /* only proceed to switch to high speed mode if */
      /* sd_spec >= 1 which means CMD6 is supported */
      /* If SD_SPEC field in the SCR register is of version 1.10 or higher */
      /* the SD card supports CMD6. */
      /* sd_spec = 1 means version 1.10 */
      /* sd_spec = 2 means version 2.00 */
      if ( scr.sd_spec >= 1 )
      {
         /* send CMD6 to query and set to high speed mode */
         (void) sdcc_sd_switch_to_hs( sdcc_pdev );
      }
   }

   /* CMD13: confirm now in TRAN state */
   card_status = sdcc_send_status(sdcc_pdev);
   if ( SDCC_CARD_TRAN != card_status )
   {
      rc = SDCC_ERR_INVALID_TX_STATE;
   }
   return rc;
} /* sdcc_config_memory_device */

/******************************************************************************
* Name: sdcc_set_blksz
*
* Description:
*    This function sets the block length to default block length 512Bytes.  It
*    sends CMD16 (SET_BLOCKLEN) to the memory device specified in parameter
*    sdcc_pdev.  The memory device sets the BLOCK_LEN_ERROR bit if the block
*    length is set larger than 512Bytes.
*
* Arguments:
*    sdcc_pdev          [IN/OUT] : pointer to device structure
*
* Returns:
*    Returns error code
*
******************************************************************************/
static SDCC_STATUS
sdcc_set_blksz ( sdcc_dev_type *sdcc_pdev )
{
   SDCC_STATUS      rc          = SDCC_ERR_SET_BLKSZ;
   UINT32           block_len   = 0;
   sdcc_cmd_type    sdcc_cmd;

   block_len   =  SDCC_DEFAULT_BLK_LENGTH_SIZE;

   sdcc_cmd.cmd       = (UINT32)SD_CMD16_SET_BLOCKLEN;
   sdcc_cmd.cmd_arg   = block_len;
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   rc = sdcc_command(sdcc_pdev, &sdcc_cmd);
   if (SDCC_NO_ERROR == rc)
   {
      if (!(sdcc_cmd.resp[0] & SDCC_CARD_STATUS_BLKLEN_ERR))
      {
         rc = SDCC_NO_ERROR;

      }
   }

   /* re-adjust the block size */
   if (SDCC_NO_ERROR == rc)
   {
      sdcc_pdev->mem.mem_info.block_len = block_len;
      HAL_sdhci_SetBlockSize(sdcc_pdev->driveno, block_len);
   }

   return rc;
} /* sdcc_set_blksz */

/******************************************************************************
 * Name: sdcc_set_partition
 *
 * Description:
 *    This function sets the partition region on the memory device by sending
 *    a command to the device specified by the handle parameter.
 *
 * Arguments:
 *    handle       [IN] : a pointer to the SDCC device that was returned from
 *                        sdcc_handle_open()
 *
 * Returns:
 *    SDCC_NO_ERROR if successful, error code otherwise
 *
 ******************************************************************************/
SDCC_STATUS
sdcc_set_partition( struct sdcc_device *handle )
{
   SDCC_STATUS      rc = SDCC_NO_ERROR;
   sdcc_dev_type   *sdcc_pdev  = NULL;
   UINT32           phy_partition_num = 0;

   if ( NULL == handle )
   {
      DEBUG ((EFI_D_ERROR, "Invalid NULL handle"));
      return SDCC_ERR_INVALID_PARAM;
   }

   sdcc_pdev = handle->hdev;

   /* translate phyiscal partition number of the client handle */
   phy_partition_num = SDCC_XLATE_PARTITION_NUM(handle->phy_partition_num);

   /* check if the active partition number is same as the requested one */
   if ( phy_partition_num == sdcc_pdev->active_partition_num )
   {
      /* Workaround for Micron data timeout issue. Always set partition for partition 2 */
      if (phy_partition_num != 2)
      {
         /* partition number is already set */
         return SDCC_NO_ERROR;
      }
   }

   if ( SDCC_CARD_MMC == sdcc_pdev->card_type ||
        SDCC_CARD_MMCHC == sdcc_pdev->card_type )
   {
      /* send CMD6 SWITCH to set the partition access */
      rc = sdcc_mmc_set_partition_access( handle );
   }
   else
   {
      DEBUG ((EFI_D_ERROR, "Set partition region not supported on card type %d",
         sdcc_pdev->card_type));
      rc = SDCC_ERR_FEATURE_UNSUPPORTED;
   }

   return rc;
} /* sdcc_set_partition */

/******************************************************************************
 * Name: sdcc_send_app_command
 *
 * Description:
 *    This function is to send an App-command to the SD-card
 *
 * Arguments:
 *    p_app_command          [IN] : pointer to a structure describing the
 *                                  App-Command to send.  The command should
 *                                  be one of the SD_ACMDXXXX enums.
 *    p_device               [IN] : pointer to the device structure
 *
 * Returns:
 *    Returns error code
 *
 *****************************************************************************/
SDCC_STATUS
sdcc_send_app_command
(
   sdcc_dev_type   *p_device,
   sdcc_cmd_type   *p_app_command
)
{
   sdcc_cmd_type command;
   SDCC_STATUS   status;

   /* First, we must send 'CMD55_APP_CMD' to tell the SD-card to expect */
   /* an app-command in the next command we send.  Set up the command */
   /* structure and send the command.  The argument to this command is */
   /* the card address (RCA) in the upper 16 bits. */
   command.cmd       = SD_CMD55_APP_CMD;
   command.cmd_arg   = SDCC_ARG_RCA(p_device->rca);
   command.resp_type = SDCC_RESP_SHORT;
   command.prog_scan = 0;
   command.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   status = sdcc_command(p_device, &command);

   /* Send the App command to the SD-card*/
   status = sdcc_command(p_device, p_app_command);

   return status;
} /* sdcc_send_app_command */

/******************************************************************************
 * Name: sdcc_do_transfer
 *
 * Description:
 *    This is a wrapper function that decides whether to use the Qualcomm
 *    or Host Controller standard interface for the data transfer.
 *
 * Arguments:
 *    p_device             [IN] : SDCC context data for this SD-card
 *    p_command            [IN] : command to send to the SD-card to perform
 *                                the read or write
 *    buffer           [IN/OUT] : buffer of data to read or write
 *    transfer_size        [IN] : When in block mode, this is the number of
 *                                blocks to transfer.  When in byte mode, it
 *                                is the number of bytes to transfer.
 *
 * Returns:
 *    Returns error code
 *
 *****************************************************************************/
SDCC_STATUS
sdcc_do_transfer
(
   sdcc_dev_type     *p_device,
   sdcc_cmd_type     *p_command,
   UINT8              *buffer,
   UINT16             transfer_size
)
{
   return sdcc_do_transfer_sdhci_mode(p_device, p_command,
                                      buffer, transfer_size);
} /* sdcc_do_transfer */

/******************************************************************************
* Name: sdcc_get_cid
*
* Description:
*    This function sends CMD10 to get the device ID and product related
*    information.
*
* Arguments:
*    sdcc_pdev          [IN/OUT] : pointer to device structure
*
* Returns:
*    Returns error code
*
*****************************************************************************/
SDCC_STATUS
sdcc_get_cid( sdcc_dev_type  *sdcc_pdev )
{
   sdcc_cmd_type   sdcc_cmd;
   SDCC_STATUS     rc = SDCC_ERR_UNKNOWN;
   BOOLEAN         decode_status = FALSE;

   /* CMD2: request card ID */
   sdcc_cmd.cmd       = (UINT32)SD_CMD10_SEND_CID;
   sdcc_cmd.cmd_arg   = SDCC_ARG_RCA(sdcc_pdev->rca);
   sdcc_cmd.resp_type = SDCC_RESP_LONG;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   rc = sdcc_command(sdcc_pdev, &sdcc_cmd);
   if (SDCC_NO_ERROR != rc)
   {
      return rc;
   }
   
   if ( SDCC_CARD_MMC == sdcc_pdev->card_type ||
        SDCC_CARD_MMCHC == sdcc_pdev->card_type )
   {
      decode_status = sdcc_decode_mmc_cid( &sdcc_cmd.resp[0], sdcc_pdev );
      if ( FALSE == decode_status )
      {
         return SDCC_ERR_INVALID_PARAM;
      }
   }
   else
   {
      return SDCC_ERR_INVALID_PARAM;
   }
   return (rc);
} /* sdcc_get_cid */

/******************************************************************************
* Name: sdcc_partial_enumeration
*
* Description:
*    This function does partial enumeration of MMC card.
*    1. Switch to 1Bit Mode
*    2. Deselects the card to STBY state
*    3. Reduces the clock to fOD
*    4. Gets the CSD & CID information
*    5. Bumps up the clock
*    6. Selects the card & moves to TRANS state
*    7. Gets the EXT_CSD
*    8. Switches to HS mode
*    9. Switches the bus width
*
* Arguments:
*    sdcc_pdev          [IN] : pointer to device structure
*
* Returns:
*    Returns error code
*
******************************************************************************/
static SDCC_STATUS sdcc_partial_enumeration ( sdcc_dev_type *sdcc_pdev )
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   INT16 driveno = 0;
   SDCC_MMC_BUSWIDTH bus_width;
   
   if (sdcc_pdev != NULL){
      driveno = sdcc_pdev->driveno;
   }
   else
   {
      DEBUG ((EFI_D_ERROR, "sdcc_partial_enumeration: sdcc_pdev is NULL!\n"));
      ASSERT(FALSE);       
   }
   
   do
   {
      /* Switch to 1Bit mode */
      bus_width = SDCC_MMC_BUSWIDTH_1BIT;
      rc = sdcc_mmc_switch(sdcc_pdev,
                           SDCC_MMCPLUS_CHANGE_BUSWIDTH |
                           ((UINT32)(bus_width << 8)));
      if (rc != SDCC_NO_ERROR)
      {
         DEBUG ((EFI_D_ERROR, "Failed in changing bus width %d", rc));
         return rc;
      }

      /* configure MMC bus width */ 
      rc = sdcc_hc_config_buswidth (driveno, bus_width);
      if (rc != SDCC_NO_ERROR)
      {
        DEBUG ((EFI_D_ERROR, "SDCC failed to configure bus width %d in SDHCi mode", rc));
        return rc;
      }

      rc = sdcc_select_card(sdcc_pdev, FALSE);
      if (rc != SDCC_NO_ERROR)
      {
         break;
      }

      rc = sdcc_get_memory_info(sdcc_pdev);
      if (rc != SDCC_NO_ERROR)
      {
         break;
      }
      
      rc = sdcc_get_cid(sdcc_pdev);
      if (rc != SDCC_NO_ERROR)
      {
         break;
      }
	  
      HAL_sdhci_FlowControl (driveno, TRUE);
      
      /* re-program the clock for data transfer */
      sdcc_config_clk(sdcc_pdev, SDCC_DATA_TRANSFER_MODE );
    
      /* config device for data transfer */
      rc = sdcc_config_memory_device(sdcc_pdev);
      if (rc == SDCC_NO_ERROR)
      {
         sdcc_pdev->host_state = SDCC_HOST_TRAN;
         // set to true if card is removed / reinserted
         sdcc_pdev->pslot->slot_state_changed = FALSE;
      }
   } while(0);

   return rc;
}

/******************************************************************************
* Name: sdcc_partial_init
*
* Description:
*    This function partial initializes the SDCC controller.
*    It also initializes the related software data
*    structures.
*
* Arguments:
*    driveno       [IN] : drive number
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
static BOOLEAN sdcc_partial_init (UINT32 driveno)
{
   sdcc_slot_type   *sdcc_pslot = NULL;
   sdcc_dev_type    *sdcc_pdev  = NULL;

   if (!SDCC_DRIVENO_IS_VALID (driveno))
   {
      return FALSE;
   }
   
   sdcc_pslot = sdcc_get_slot_handle (driveno);

   if (sdcc_pslot == NULL)
   {
      DEBUG ((EFI_D_ERROR, "sdcc_partial_init: can't get slot handle\n"));     
      return FALSE;
   }
   
   sdcc_pdev = &(sdcc_pslot->dev);

   if (sdcc_pdev == NULL)
   	  return FALSE;

   /* The controller already turned on */
   if ((sdcc_pdev->host_state  == SDCC_HOST_IDENT) ||
       (sdcc_pdev->host_state  == SDCC_HOST_TRAN))
   {
      return TRUE;
   }

   /* Turning on the clock to the SDCC controller */
   sdcc_ahb_clk_init(driveno);

   /* initialize the controller registers */
   HAL_sdhci_InitializeRegisters();

   /* intialize the dev data structs */
   SetMem(sdcc_pdev, sizeof(sdcc_pslot->dev), 0);
   sdcc_pdev->pslot         = sdcc_pslot;
   sdcc_pslot->driveno      = driveno;
   sdcc_pslot->drive_in_use = TRUE;
   
   /* update device info */
   sdcc_pdev->driveno        = driveno;
   sdcc_pdev->host_state     = SDCC_HOST_IDENT;
   sdcc_pdev->mem.block_mode = SDCC_MEM_BLK_MODE;
   sdcc_pdev->selected_phase = HS200_INVALID_TUNING_PHASE;
   sdcc_pdev->tuning_needed  = FALSE;
   sdcc_pdev->tuning_in_progress = FALSE;
   sdcc_pdev->tuning_done    =  FALSE;

   sdcc_partial_init_sdhci_mode(sdcc_pdev);
   
   return(TRUE);
} /* sdcc_partial_init */

/******************************************************************************
* Name: sdcc_handle_optimized_open
*
* Description:
*    This function does  optimized initialization of the MMC device.
*    It returns NULL if there is no memory device attached.
*    It then sets up the device for data transfer.
*    A non-null pointer to the SDCC device is returned.
*
* Arguments:
*    phy_partition_num [IN] : physical partition number.
*
* Returns:
*    A pointer to the SDCC device if successful.  NULL if failed.
*
******************************************************************************/
struct sdcc_device *
sdcc_handle_optimized_open (UINT32 phy_partition_num)
{
   BOOLEAN ret;
   SDCC_STATUS          rc = SDCC_NO_ERROR;
   sdcc_mini_boot_device_info_type *device_info = &sdcc_mini_boot_device_info;
   UINT32               driveno = device_info->driveno;
   struct sdcc_device   *ret_handle = NULL;
   BOOLEAN              still_in_use = TRUE;
   sdcc_slot_type       *sdcc_pslot = NULL;
   sdcc_dev_type        *sdcc_pdev  = NULL;

   sdcc_pslot = sdcc_get_slot_handle (driveno);
   if (NULL == sdcc_pslot)
   {
      DEBUG ((EFI_D_ERROR, "sdcc_handle_optimized_open: can't get slot handle\n"));
      return NULL;
   }
   
   sdcc_pdev = &(sdcc_pslot->dev);

   do
   {
      if (!SDCC_DRIVENO_IS_VALID (driveno))
      {
         break;
      }
      
      ret = sdcc_partial_init (driveno);
      if ( FALSE == ret )
      {
         break;
      }

      /* register the client handle first */
      ret_handle = sdcc_register_client_handle (driveno,
                                                phy_partition_num);
      if ( ret_handle == NULL)
      {
         /* check if memory device in driveno is still in use */
         still_in_use = sdcc_is_device_still_in_use (sdcc_pdev);
         if ( !still_in_use )
         {
            /* client handle list is full */
            (void) sdcc_deinit( sdcc_pdev );
         }
         break;
      }
      
      /* return if it has been initialized */
      if (( sdcc_pdev->card_type == SDCC_CARD_MMC ) ||
          ( sdcc_pdev->card_type == SDCC_CARD_MMCHC ))
      {
         /* no need to select the partition region on the device as it is */
         /* already in use */
         /* will return a new handle to client */
         break;
      }

      sdcc_pdev->card_type = device_info->card_type;
      sdcc_pdev->rca = device_info->rca;
      sdcc_pdev->mem.mem_info.card_type = sdcc_pdev->card_type;

      /* return and close controller if none attached */
      if ( sdcc_pdev->card_type == SDCC_CARD_UNKNOWN )
      {
         (void) sdcc_handle_close(ret_handle);
         ret_handle = NULL;
         break;
      }

      rc = sdcc_partial_enumeration(sdcc_pdev);
      if (rc != SDCC_NO_ERROR)
      {
         sdcc_pdev->errno = rc;
         (void) sdcc_handle_close(ret_handle);
         ret_handle = NULL;
         break;
      }

      /* Invalidate the active partition number, this force setting of the 
       * active partition on the next access */
      sdcc_pdev->active_partition_num = SDCC_HANDLE_PHY_PARTITION_ENTIRE_MEM_DEVICE;
   } while(0);

   if ( ret_handle == NULL)
   {
      SetMem(device_info, sizeof(sdcc_mini_boot_device_info_type), 0);
   }

   return ret_handle;
}

/******************************************************************************
* Name: sdcc_is_partial_enumeration
*
* Description:
*    This function checks if partial Initialization is to be done.
*
* Arguments:
*    driveno       [IN] : drive number
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
BOOLEAN sdcc_is_partial_enumeration ( INT16 driveno )
{
   sdcc_mini_boot_device_info_type *device_info = &sdcc_mini_boot_device_info;

   if (device_info->driveno == (UINT32)driveno)
   {
      /* Support this feature for MMC */
      if ((device_info->card_type == SDCC_CARD_MMC) ||
          (device_info->card_type == SDCC_CARD_MMCHC))
      {
         return FALSE;
      }
   }
   return FALSE;
}

/******************************************************************************
* Name: sdcc_set_block_count
*
* Description:
*    This function sends CMD23 to set the number of blocks and reliable write parameter
*    for a block read or write command
*
* Arguments:
*    sdcc_pdev  [IN] : pointer to device structure
*    cmd_arg  [IN]   : Bit[31] - Reliable write request
*                      Bit[0:15] - Number of blocks
* Returns:
*    Returns error code
*
*****************************************************************************/
SDCC_STATUS
sdcc_set_block_count(sdcc_dev_type *sdcc_pdev, UINT32 cmd_arg)
{
   sdcc_cmd_type   sdcc_cmd;

   /* CMD23: Set Block Count */
   sdcc_cmd.cmd       = (UINT32)SD_CMD23_SET_BLOCK_COUNT;
   sdcc_cmd.cmd_arg   = cmd_arg;
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   return sdcc_command(sdcc_pdev, &sdcc_cmd);
}

/******************************************************************************
 * Name: sdcc_rpmb_do_transfer
 *
 * Description:
 *    This function is a wrapper function. It decides whether to use Qualcomm
 *    or standard Host Controller interface to read/write to the RPMB partition.
 *
 * Arguments:
 *    p_device             [IN] : SDCC context data for this SD-card 
 *    p_command            [IN] : command to send to the SD-card to perform
 *                                the read or write
 *    buffer           [IN/OUT] : buffer of data to read or write
 *    transfer_size        [IN] : The number of blocks to transfer.
 *
 * Returns:
 *    Returns error code
 *
 *****************************************************************************/
SDCC_STATUS
sdcc_rpmb_do_transfer
(
   sdcc_dev_type     *p_device, 
   sdcc_cmd_type     *p_command,
   UINT8              *buffer,
   UINT16             transfer_size
)
{
   return sdcc_do_transfer_sdhci_mode(p_device, p_command,
                                      buffer, transfer_size); 
   }

/******************************************************************************
* Name: wait_card_ready_check_wp_violation
*
* Description:
*    This function sends CMD13 SEND_STATUS to the memory device to get the card
*    status.  It then checks if WP_VIOLATION error bit is set. It continues to
*    send CMD13 until the card is ready (card returns to Transfer state in the
*    CMD13's response) or it will time out after SDCC_PROG_DONE_MAX tries.
*
* Arguments:
*    sdcc_pdev          [IN/OUT] : pointer to device structure
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS wait_card_ready_check_wp_violation
(
   sdcc_dev_type *sdcc_pdev
)
{
   SDCC_STATUS      wp_status = SDCC_NO_ERROR;
   SDCC_STATUS      rc = SDCC_ERR_UNKNOWN;
   SDCC_CARD_STATUS card_status = SDCC_CARD_IGNORE;
   sdcc_cmd_type    sdcc_cmd;
   UINT32           count  = 0;

   sdcc_cmd.cmd       = SD_CMD13_SEND_STATUS;
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.cmd_arg   = SDCC_ARG_RCA(sdcc_pdev->rca);
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   while (count < SDCC_PROG_DONE_MAX)
   {
      rc = sdcc_command(sdcc_pdev, &sdcc_cmd);

      if (SDCC_NO_ERROR == rc)
      {
         /* Check if write protect violation error occurs. */
         /* The WP error bit is cleared as soon as the response is sent out */
         /* in response to the CMD13 SEND_STATUS command */
         /* Refer to Secton 7.13 JEDEC Standard No. 84-A441 */
         if (SDCC_WP_VIOLATION_ERROR == 
             SDCC_XLATE_WP_VIOLATION(sdcc_cmd.resp[0]))
         {
            DEBUG ((EFI_D_WARN, "SDCC Write protection violation\n"));
            wp_status = SDCC_ERR_WP_VIOLATION;
         }

         /* Check if the card is ready */
         card_status = (SDCC_CARD_STATUS)(
                        ( sdcc_cmd.resp[0] >> SDCC_CARD_STATUS_SHFT )
                        & (SDCC_CARD_STATUS_BMSK));

         if (SDCC_CARD_TRAN == card_status)
         {
            break;
         }
      }

      sdcc_udelay(10);
      count += 10; 
   }

   /* If the card is in Trans state, return the WP status */
   /* Otherwise, timeout occured, return SDCC_ERR_CARD_READY error */
   return (card_status == SDCC_CARD_TRAN) ? wp_status : SDCC_ERR_CARD_READY;
}
