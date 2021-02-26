/**********************************************************************
 * sdcc_hc.c
 *
 * Implementation of SD Host Controller Standard interface
 *
 * Copyright (c) 2014-2017 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 **********************************************************************/
/*=======================================================================
                        Edit History


when        who     what, where, why
----------  ---     --------------------------------------------------- 
2017-05-12  sb      Add partial enumeration for eMMC 
2017-01-17  jt      Update sdcc_hc_set_clock() definition
2016-12-13  jt      Remove legacy code  
2016-01-18  eo      Reduced sdcc code size
2016-01-08  jt      Make sure command errors/interrupts are cleared 
2015-12-18  jt      Enable bus power before reset  
2015-12-17  jt      Correct CDR Enable logic   
2015-10-13  rm      Added BlockIO2 functions
2015-07-15  bn      Fixed reset sequence
2015-04-02  bn      Code Clean up. Removed unused cache_align_size
2015-03-30  bn      Fixed ADMA 64-bit mode. Removed Clock ramp up time.
2015-03-26  bn      Changed error print to D_INFO. Corrected mem_barrier functions
2015-02-25  bn      Enabled mclk during init
2015-01-24  rm      Integraged HS200/HS400 support for eMMC from UEFI 3.0
2014-11-10  bn      Add null pointer check for sdcc_get_slot_handle
2014-10-18  jb      Add target specific cap override support
2014-09-24  bn      Moved SDHCi support flags to PCD. Added SetCapabilities()
2014-09-03  bn      Initial version. Branched from 8994 SBL

======================================================================*/

#include "sdcc_hc.h"
#include "HALhwio.h"
#include "sdcc_util.h"
#include "SdccBsp.h"
#include "sdcc_pal_tool.h"
#include "sdcc_mmc_util.h"
#include "sdcc_handle_util.h"
#include <Library/SdccCommonLib/SdccHalLib/SdccHal.h>
#include <Library/PcdLib.h>

static UINT8  pDescTableAddr[ADMA_DESC_SIZE*ADMA_MAX_DESCRIPTOR_COUNT];
BLKIO2_XFER_INFO BlkIo2_Xfer_Info;

/******************************************************************************
* Name: sdcc_init_sdhci_mode
*
* Description:
*    This function initializes the SDCC controller to using the Host Controller 
*    interface for communication with the memory devices.
*    It first turns on the clock, configures the GPIOs and then turns on the 
*    power supply to the device.  It also initializes the related software data
*    structures.
*
* Arguments:
*    driveno   [IN] : drive number
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
boolean
sdcc_init_sdhci_mode (UINT32 driveno)
{
   sdcc_slot_type   *sdcc_pdata = NULL;
   sdcc_dev_type    *sdcc_pdev  = NULL;
   UINT32 eMMCSlot = PcdGet32(SDCCEmmcSlotNumber);

   sdcc_pdata = sdcc_get_slot_handle (driveno);
   if (NULL == sdcc_pdata)
   {
      DEBUG ((EFI_D_ERROR, "sdcc_init_sdhci_mode: can't get slot handle\n"));
      return FALSE;
   }
   
   sdcc_pdev = &(sdcc_pdata->dev);
   sdcc_pdev->pslot = sdcc_pdata;

   /* The controller already turned on */
   if ((sdcc_pdev->host_state  == SDCC_HOST_IDENT) ||
       (sdcc_pdev->host_state  == SDCC_HOST_TRAN))
   {
      return TRUE;
   }

   if (FALSE == sdcc_pdata->drive_in_use)
   {
      /* intialize the dev data structs */
      SetMem(&sdcc_pdata->dev, sizeof(sdcc_pdata->dev), 0);
      sdcc_pdev->pslot = sdcc_pdata;
      sdcc_pdev->driveno = driveno;
      sdcc_pdata->driveno = driveno;
      sdcc_pdata->drive_in_use = TRUE;
   }

   /* Configure GPIOs */
   sdcc_bsp_gpio_control(driveno, SDCC_BSP_GPIO_ON);

   /* Initialize other SDCC clocks */
   sdcc_ahb_clk_init (driveno); 
   sdcc_enable_mclk (driveno);
   
   /* initialize the controller */
   HAL_sdhci_InitializeRegisters ();
   
   /* Turn on the SDCC clock */
   sdcc_config_clk(sdcc_pdev, SDCC_INIT_MODE);

   /* update device info */
   sdcc_pdev->driveno        = driveno;
   sdcc_pdev->host_state     = SDCC_HOST_IDENT;
   sdcc_pdev->mem.block_mode = SDCC_MEM_BLK_MODE;
   sdcc_pdev->selected_phase = HS200_INVALID_TUNING_PHASE;
   sdcc_pdev->tuning_needed  = FALSE;
   sdcc_pdev->tuning_in_progress = FALSE;
   sdcc_pdev->tuning_done    =  FALSE;

   if (driveno == eMMCSlot)
   {
      /* Disable HC Select In */
      HAL_sdhci_HCSelectInDisable(driveno);
   }   

   /* Reset SDHCi */
   HAL_sdhci_SetPower (driveno, TRUE);
   HAL_sdhci_SoftwareReset(driveno, HAL_SDHCI_RESET_ALL);

   /* Set the controller's capabilities */
   HAL_sdhci_SetCapabilities(driveno);   
   sdcc_bsp_target_cap_overrides (driveno);
   /* Get card capabilities to see what features it supports */
   HAL_sdhci_GetCapabilities (driveno, &(sdcc_pdata->capabilities));
   /* Turn on power to the memory device */
   (void)sdcc_hc_set_power_mode (driveno, TRUE);
   /* Disable interrupts */
   HAL_sdhci_InterruptSignalEnable(driveno, HAL_SDHCI_INTERRUPT_MSK_ALL, FALSE);
   /* Enable Statuses */
   HAL_sdhci_InterruptStatusEnable (driveno, HAL_SDHCI_INTERRUPT_MSK_ALL, TRUE);
   /* Clear Statuses */
   HAL_sdhci_ResetInterruptStatus(driveno, HAL_SDHCI_INTERRUPT_MSK_ALL);
   /* Is DDR supported? This ddr_support flag will be used later in the 
    * card initialization to determine if we should turn on DDR mode for 
    * data transfer */
   sdcc_pdev->sdhci_ddr_support = ((sdcc_pdata->capabilities.HAL_sdhci_Reg_44_46) & 
               HAL_SDHCI_CAPS_DDR_50_SUPPORT_BMSK) ? TRUE : FALSE;
   
   /* Is ADMA supported? */
   if ((TRUE == PcdGetBool(SDCCDmaEnabled)) && 
       (sdcc_pdata->capabilities.HAL_sdhci_Reg_40_42 & HAL_SDHCI_CAPS_ADMA2_SUPPORT_BMSK))       
   {
      sdcc_pdev->sdhci_adma_enable = TRUE;
      HAL_sdhci_ADMAEnable (driveno, TRUE);
   }
   else
   {
      sdcc_pdev->sdhci_adma_enable = FALSE;
      HAL_sdhci_ADMAEnable (driveno, FALSE);
   }
   
   return(TRUE);
}

/******************************************************************************
* Name: sdcc_partitial_init_sdhci_mode
*
* Description:
*    This function initializes the SDCC controller to using the Host Controller 
*    interface for communication with the memory devices.
*
* Arguments:
*    sdcc_pdev       [IN/OUT]: pointer to device structure
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
boolean
sdcc_partial_init_sdhci_mode (sdcc_dev_type *sdcc_pdev)
{
   /* Disable HC Select In */
   HAL_sdhci_HCSelectInDisable(sdcc_pdev->driveno);

   /* Set the controller's capabilities */
   HAL_sdhci_SetCapabilities(sdcc_pdev->driveno);   
   sdcc_bsp_target_cap_overrides (sdcc_pdev->driveno);
   /* Get card capabilities to see what features it supports */
   HAL_sdhci_GetCapabilities (sdcc_pdev->driveno, &(sdcc_pdev->pslot->capabilities));
   /* Disable interrupts */
   HAL_sdhci_InterruptSignalEnable(sdcc_pdev->driveno, HAL_SDHCI_INTERRUPT_MSK_ALL, FALSE);
   /* Enable Statuses */
   HAL_sdhci_InterruptStatusEnable (sdcc_pdev->driveno, HAL_SDHCI_INTERRUPT_MSK_ALL, TRUE);
   /* Clear Statuses */
   HAL_sdhci_ResetInterruptStatus(sdcc_pdev->driveno, HAL_SDHCI_INTERRUPT_MSK_ALL);
   /* Is DDR supported? This ddr_support flag will be used later in the 
    * card initialization to determine if we should turn on DDR mode for 
    * data transfer */
   sdcc_pdev->sdhci_ddr_support = ((sdcc_pdev->pslot->capabilities.HAL_sdhci_Reg_44_46) & 
               HAL_SDHCI_CAPS_DDR_50_SUPPORT_BMSK) ? TRUE : FALSE;
   
   /* Is ADMA supported? */
   if ((TRUE == PcdGetBool(SDCCDmaEnabled)) && 
       (sdcc_pdev->pslot->capabilities.HAL_sdhci_Reg_40_42 & HAL_SDHCI_CAPS_ADMA2_SUPPORT_BMSK))       
   {
      sdcc_pdev->sdhci_adma_enable = TRUE;
      HAL_sdhci_ADMAEnable (sdcc_pdev->driveno, TRUE);
   }
   else
   {
      sdcc_pdev->sdhci_adma_enable = FALSE;
      HAL_sdhci_ADMAEnable (sdcc_pdev->driveno, FALSE);
   }
   
   return(TRUE);
}

/******************************************************************************
* Name: sdcc_hc_set_power_mode
*
* Description:
*    This function sets the voltage level supported in the Capabilities registers. 
*    It then turns the bus power on/off as indicated in the bOn passed in parameter

* Arguments:
*    driveno     [IN]: drive number
*    bOn         [IN]: TRUE to turn bus pwer ON. OFF otherwise
*
* Returns:
*    Error Code
*
******************************************************************************/
static SDCC_STATUS sdcc_hc_set_power_mode (UINT32 driveno, boolean bOn)
{
   UINT32 nCapabilitiesRegVal = 0;
   HAL_sdhci_BusVolSelect voltage = HAL_SDHCI_SD_BUS_VOLTAGE_18_BMSK;
   UINT8 CurrentState = 0, NewState = 0;
   struct sdcc_slot_data *pslot = NULL;

   pslot = sdcc_get_slot_handle (driveno);
   if (NULL == pslot)
   {
      DEBUG ((EFI_D_ERROR, "sdcc_hc_set_power_mode: can't get slot handle\n"));     
      return SDCC_ERR_INVALID_PARAM;
   }

   nCapabilitiesRegVal = pslot->capabilities.HAL_sdhci_Reg_40_42;
   if (nCapabilitiesRegVal & HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_3_3V_BMSK)
   {
      voltage = HAL_SDHCI_SD_BUS_VOLTAGE_33_BMSK;
   }
   else if (nCapabilitiesRegVal & HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_3_0V_BMSK)
   {
      voltage = HAL_SDHCI_SD_BUS_VOLTAGE_30_BMSK;
   }
   else if (nCapabilitiesRegVal & HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_1_8V_BMSK)
   {
      voltage = HAL_SDHCI_SD_BUS_VOLTAGE_18_BMSK;
   }  
   else
   {
      return SDCC_ERR_UNKNOWN;
   }

   /* change voltage and power state if it's different with the existing setting */
   CurrentState = HAL_sdhci_GetPowerState (driveno);
   NewState = voltage | (UINT8)bOn;
   if (NewState == CurrentState)
   {
      return SDCC_NO_ERROR;;
   }
   
   /* Turn the bus power off before changing the voltage */
   HAL_sdhci_SetPower (driveno, FALSE);
   
   if (TRUE == bOn)
   {
      /* set the voltage level first, then set the power on bit */
      HAL_sdhci_VoltageSelect (driveno, voltage);   
      /* turn the bus power on */
      HAL_sdhci_SetPower (driveno, TRUE);
      /* handle all the power interrupt/status sequence */
      HAL_sdhci_HandlePowerInterrupts (driveno);
   }

   return SDCC_NO_ERROR;
}

/******************************************************************************
 * Name: sdcc_hc_set_clock
 *
 * Description:
 *    This function sets the SCLK frequency that goes to the memory device
 *
 * Arguments:
 *    driveno     [IN]: drive number
 *    nDivider    [IN]: divider to use
 *
 * Returns:
 *    Returns error code
 *
 *****************************************************************************/
SDCC_STATUS sdcc_hc_set_clock (UINT32 driveno, UINT32 nDivider)
{
   UINT32 count = 0; 

   if (!SDCC_DRIVENO_IS_VALID(driveno))
   {
      DEBUG ((EFI_D_ERROR, "sdcc_hc_set_clock: Invalid Drive Number:%d", driveno));
      return SDCC_ERR_INVALID_PARAM;
   }
 
   /* SetClock */
   /* Use 10-bit clock mode. See HC specs for details */   
   HAL_sdhci_SetClock_10Bits_Mode (driveno, nDivider);

   count = SDHCI_STATUS_POLL_MAX;

   /* Wait for internal CLK to stable */
   do
   {
      if (TRUE == HAL_sdhci_IsClkStable(driveno))
      {
        /* Enable the SCLK */
        HAL_sdhci_EnableSDClk(driveno, TRUE);   
        return SDCC_NO_ERROR;
      }

      sdcc_udelay(1);
   } while (count--);

   /* count timeout expired */
   return SDCC_ERR_TIMEOUT;
}

/******************************************************************************
* Name: sdcc_hc_set_uhs_mode_select
*
* Description:
*    This function sets the UHS mode select field to the provided mode. 
*
* Arguments:
*    p_device       [IN/OUT]: pointer to device structure
*    mode           [IN]: UHS mode 
*
* Returns:
*    Error Code
*
******************************************************************************/
SDCC_STATUS sdcc_hc_set_uhs_mode_select (sdcc_dev_type *p_device, SDHCI_UHS_MODE mode)
{
   SDCC_CARD_MODE speed_mode; 
   UINT32 driveno = 0; 
   /* SDCC_MCLK_FEEDBACK_CLOCK is default value as per SWI */
   UINT32 select_in = HAL_SDCC_CLOCK_EDGE_FEEDBACK; 

   if (NULL == p_device)
   {
      return SDCC_ERR_INVALID_PARAM;
   }

   driveno = p_device->driveno; 

   /* First disable the clock before making any change */ 
   HAL_sdhci_EnableSDClk(driveno, FALSE);
   
   /* Enable selected mode, update clock with new frequency */
   switch (mode)
   {
   case SDHCI_SDR104_MODE:
      p_device->mem.clk_freq = SDCC_MMC_FREQUENCY_200MHZ; 
      speed_mode = SDCC_DATA_TRANSFER_HS200_MODE;
      select_in = HAL_SDCC_CLOCK_UHS_MCLK;      
      break;
      
   case SDHCI_DDR50_MODE:
      p_device->mem.clk_freq = SDCC_MMC_FREQUENCY_52MHZ;
      speed_mode = SDCC_DATA_TRANSFER_DDR_MODE; 
      select_in = HAL_SDCC_CLOCK_EDGE_DDR; 
      break;
      
   case SDHCI_SDR25_MODE:
      p_device->mem.clk_freq = SDCC_MMC_FREQUENCY_52MHZ;
      speed_mode = SDCC_DATA_TRANSFER_MODE;
      select_in = HAL_SDCC_CLOCK_EDGE_FEEDBACK; 
      break;   
      
   case SDHCI_SDR12_MODE:
      p_device->mem.clk_freq = SDCC_MMC_FREQUENCY_26MHZ;
      speed_mode = SDCC_DATA_TRANSFER_MODE;
      select_in = HAL_SDCC_CLOCK_EDGE_FEEDBACK;
      break;
      
   default:
      return SDCC_ERR_INVALID_PARAM; 
   }

   /* Enable selected mode */
   HAL_sdhci_SetUHSMode(driveno, mode); 
   HAL_sdhci_HCSelectIn(driveno, select_in);

   /* Update clock with new frequency */
   sdcc_config_clk(p_device, speed_mode);

   return SDCC_NO_ERROR; 
}

/******************************************************************************
* Name: sdcc_hc_wait_cmd_ready
*
* Description:
*    This function checks the CMD and DATA Inhibit states. 
*    It returns error if the Inhibit conditions continue after the 
*    timeout period.
*
* Arguments:
*    driveno        [IN]: drive number
*
* Returns:
*    Error Code
*
******************************************************************************/
static SDCC_STATUS sdcc_hc_wait_cmd_ready (UINT32 driveno)
{
   UINT32 count = 0;

   /* Waiting for Command Inhibit clear */
   while (count < SDHCI_CMD_POLL_MAX)
   {
      if ((HAL_sdhci_GetPresentState(driveno) & 
          (HAL_SDHCI_PRESENT_STATE_CMD_INHIBIT_CMD_BMSK | 
           HAL_SDHCI_PRESENT_STATE_CMD_INHIBIT_DAT_BMSK)) == 0)
      {
         return SDCC_NO_ERROR;
      }
	  
      sdcc_udelay (100);
      count += 100;
   }

   /* Timeout error if command inhibit state is stuck */
   return SDCC_ERR_TIMEOUT;
}

/******************************************************************************
* Name: sdcc_hc_prepare_dma_descriptors
*
* Description:
*    This function prepares the ADMA2 descriptor per the HC specs.
*
* Arguments:
*    driveno              [IN]: drive number
*    buffer               [IN]: pointer to the data buffer used in the DMA transfer
*    nTransferSizeInByte  [IN]: size of the transfer in unit of byte
*
* Returns:
*    Error Code
*
******************************************************************************/
static SDCC_STATUS
sdcc_hc_prepare_dma_descriptors (UINT32 driveno, byte *buffer,
                                 UINT32 nTransferSizeInByte)
{
   sSDHCADMADescriptor *pDescTable;
   UINT32 nCurrSize = 0;
   UINT32 nDescCount = 0, nDescTableSize = 0;  
   UINTN pData;
   UINT32 nDataLeft = nTransferSizeInByte;
   UINT32 pDescTableAddrHi = 0, pDescTableAddrLo = 0;

   if (pDescTableAddr == NULL)
   {
      DEBUG ((EFI_D_ERROR, "pDescTableAddr = NULL \n"));
      return SDCC_ERR_INVALID_PARAM;
   }

   sdcc_bsp_memory_barrier();
   sdcc_bsp_cache_operation(SDCC_BSP_CACHE_FLUSH, buffer, nTransferSizeInByte);
   sdcc_bsp_memory_barrier();	 
	 
   pDescTable = (sSDHCADMADescriptor*)pDescTableAddr;
   nDescCount = (nDataLeft/MAX_DESCRIPTOR_SIZE);
   
   if (nDataLeft % MAX_DESCRIPTOR_SIZE)
   {
      nDescCount++;
   }

   nDescTableSize = nDescCount * sizeof(sSDHCADMADescriptor);
   SetMem(pDescTableAddr, nDescTableSize, 0);

   /* First descriptor table entry points to the buffer */
   pData = (UINTN) buffer;

   while (nDataLeft > 0)
   {
      nCurrSize = MAX_DESCRIPTOR_SIZE;

      if (nCurrSize > nDataLeft) 
      {   
         nCurrSize = nDataLeft;
      }

      pDescTable->nDataAddress = pData;
      pDescTable->nDataSize = (uint16)(nCurrSize & 0xFFFF);
      pDescTable->nADMAFlags = ADMA_FLAGS_DESC_VALID;

      nDataLeft -= nCurrSize;

      if (nCurrSize == 0)
      {
         pDescTable->nADMAFlags |= ADMA_FLAGS_OPERATION_NOP;
      }
      else
      {
         pDescTable->nADMAFlags |= ADMA_FLAGS_OPERATION_TRAN;
      }

      pData += nCurrSize;

      /* the last descriptor entry has a DESC_END flag set */
      if (nDataLeft == 0)
      {
         pDescTable->nADMAFlags |= ADMA_FLAGS_DESC_END;
         break;
      }
	  
      pDescTable++;
   }

   sdcc_bsp_memory_barrier();
   sdcc_bsp_cache_operation(SDCC_BSP_CACHE_FLUSH, pDescTableAddr, nDescTableSize);
   sdcc_bsp_memory_barrier();

   if (sizeof (UINTN) == 0x8)
   {
      /* 64-bit */
      pDescTableAddrHi = ((UINT64)pDescTableAddr >> 32) & 0xFFFFFFFF;
   }
   else
   {
      /* 32-bit */
      pDescTableAddrHi = 0;
   }

   pDescTableAddrLo = ((UINT64)pDescTableAddr & 0xFFFFFFFF);

   /* write the descriptor addr to the ADMA System Address Registers */   
   HAL_sdhci_SetADMASystemAddress64bit(driveno, pDescTableAddrHi);
   HAL_sdhci_SetADMASystemAddress(driveno, pDescTableAddrLo);

   return SDCC_NO_ERROR;
}

/******************************************************************************
* Name: sdcc_hc_fifo_mode_rw
*
* Description:
*    This function is to read/write data from the memory device using
*    non-dma in Host Controller interface mode.
*
* Arguments:
*    p_device               [IN]: pointer to device structure
*    p_command              [IN]: pointer to command structure
*    buffer                 [IN/OUT]: pointer of the data buffer to write to or read from
*    transfer_size_in_byte  [IN]: transfer size in unit of bytes
*
* Returns:
*    Error Code
*
******************************************************************************/
static SDCC_STATUS sdcc_hc_fifo_mode_rw (sdcc_dev_type *p_device,
                                         sdcc_cmd_type *p_command,
                                         UINT8 *buffer,
                                         UINT32 transfer_size_in_byte)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   UINT32 nWaitMask = 0;
   UINT32 nIntStatus = 0, i = 0;
   UINT32 nBlockSize = 0, nBlockSizeDWords = 0;
   UINT32 nBlockCount = 0, nLeftBlockCount = 0;
   UINT32 driveno = p_device->driveno;
   UINT32 *pCurrData = (UINT32 *)(buffer);

   if (SDCC_MEM_BLK_MODE == p_device->mem.block_mode)
   {
      nBlockSize = p_device->mem.mem_info.block_len;
      nBlockCount = (transfer_size_in_byte/nBlockSize) ;
   }
   else
   {
      /* Byte mode, transfer less than 512 bytes */
      nBlockSize = transfer_size_in_byte;
      nBlockCount = 1;
   }

   nBlockSizeDWords = (nBlockSize + 3) >> 2;
   nLeftBlockCount = nBlockCount;

   if (SDCC_IS_READ_TRANSFER(p_command->flags))
   {
      nWaitMask = SDHCI_STATUS_BUF_RD_READY_BMSK;
   }
   else if (SDCC_IS_WRITE_TRANSFER(p_command->flags))
   {
      nWaitMask = SDHCI_STATUS_BUF_WR_READY_BMSK;
   }
   else
   {
      return SDCC_ERR_UNKNOWN;
   }

   do
   {
      rc = sdcc_hc_wait_interrupt_status (driveno, nWaitMask, &nIntStatus);
      if (rc != SDCC_NO_ERROR)
      {
         p_command->status = nIntStatus;
         return rc;
      }

      HAL_sdhci_ResetInterruptStatus(driveno, nWaitMask);

      i = 0;
      if (SDCC_IS_READ_TRANSFER(p_command->flags))
      {
         while (i < nBlockSizeDWords)
         {
            *pCurrData = HAL_sdhci_ReadDataBufPort(driveno);
            pCurrData++;
            i++;
         }
      }
      else if (SDCC_IS_WRITE_TRANSFER(p_command->flags))
      {
         while (i < nBlockSizeDWords)
         {
            HAL_sdhci_WriteDataBufPort(driveno, *pCurrData);
            pCurrData++;
            i++;
         }
      }

      nLeftBlockCount--;
   }
   while (nLeftBlockCount > 0);

   return rc;
}

/******************************************************************************
 * Name: sdcc_do_transfer_sdhci_mode
 *
 * Description:
 *    This is a generic function to perform a read or write data transfer on
 *    the SD-card using the Host Controller standard interface.
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
SDCC_STATUS sdcc_do_transfer_sdhci_mode (sdcc_dev_type *p_device, 
                                         sdcc_cmd_type *p_command,
                                         UINT8 *buffer,
                                         UINT16 transfer_size)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   UINT32 nIntStatus = 0, nWaitMask = 0;
   UINT32 nTransferSize = 0, nBlkSize = 0, nBlkCount = 0;
   UINT32 driveno = p_device->driveno;

   nBlkSize = (SDCC_MEM_BYTE_MODE == p_device->mem.block_mode) ?
                transfer_size : p_device->mem.mem_info.block_len;

   nBlkCount = (SDCC_MEM_BYTE_MODE == p_device->mem.block_mode) ?
                1 : transfer_size;

   /* transfer size in unit of bytes */
   nTransferSize = nBlkSize*nBlkCount;

   p_device->enable_dma = p_device->sdhci_adma_enable;

   nIntStatus = HAL_sdhci_GetInterruptStatus(driveno);
   
   /* clear all previous interrupts */
   if (HAL_SDHCI_INTERRUPT_MSK_ALL & nIntStatus)
   {
      HAL_sdhci_ResetInterruptStatus(driveno, HAL_SDHCI_INTERRUPT_MSK_ALL);
      /* reset CPSM & DPSM on errors, abort transaction if not finished */
      if (HAL_SDHCI_INTERRUPT_ERROR_MSK_ALL & nIntStatus)
      {
         HAL_sdhci_SoftwareReset(driveno, HAL_SDHCI_RESET_CMD_DAT);
      }
   }

   if (TRUE == p_device->enable_dma)
   {
      /* Create Descriptor table for ADMA2 in system memory */
      (void) sdcc_hc_prepare_dma_descriptors(driveno, buffer, nTransferSize);	
   }

   /* configure the number of bytes in a data block */
   HAL_sdhci_SetBlockSize(driveno, nBlkSize);

   /* configure the numbe of blocks in a data transfer */
   HAL_sdhci_SetBlockCount(driveno, nBlkCount);

   /* set up the Transfer Mode register. This is done before sending CMD */
   sdcc_hc_set_transfer_mode(p_device, p_command, nBlkCount);

   /* send command now */
   if (SDCC_IS_APP_COMMAND(p_command->flags))
   {
      rc = sdcc_send_app_command(p_device, p_command);
   }
   else
   {
      rc = sdcc_command(p_device, p_command);
   }

   if (SDCC_NO_ERROR != rc)
   {
      goto ErrorExit;
   }

#ifndef USE_LOADER_UTILS    
   /* CMD sent. Response received. Now read or write data */
   nWaitMask = SDHCI_STATUS_TRANS_COMPLETE_BMSK;

   if (FALSE == p_device->enable_dma)
   {
      rc = sdcc_hc_fifo_mode_rw(p_device, p_command, buffer, nTransferSize);
      if (SDCC_NO_ERROR != rc)
      {
         goto ErrorExit;
      }
   }
   else
   {
      nWaitMask |= SDHCI_STATUS_ADMA_ERR_BMSK;
   }
#else
   nWaitMask = SDHCI_STATUS_TRANS_COMPLETE_BMSK | SDHCI_STATUS_ADMA_ERR_BMSK;
#endif

   /* poll the transfer complete or any error status bits */
   rc = sdcc_hc_wait_interrupt_status(driveno, nWaitMask, &nIntStatus);
   if (SDCC_NO_ERROR != rc)
   {
      p_command->status = nIntStatus;

      if (p_device->enable_dma && (nIntStatus & SDHCI_STATUS_ADMA_ERR_BMSK))
      {
         DEBUG ((EFI_D_ERROR,"SDCC: DATA ADMA ERROR %d\n",p_command->cmd));
         /* ADMA error */
         rc = SDCC_ERR_DMA;
      }
      else if((nIntStatus & SDHCI_STATUS_DATA_CRC_ERR_BMSK) || 
         (nIntStatus & SDHCI_STATUS_DATA_TIMEOUT_ERR_BMSK))
      {
         /* If DATA CRC/timeout error occurs, execute tuning again */
         if(p_device->tuning_needed && (!p_device->tuning_in_progress))
         {
            p_device->tuning_done = FALSE;
         }
         rc = SDCC_ERR_TIMEOUT;
      }
      goto ErrorExit;
   }
   else if (nIntStatus &  SDHCI_STATUS_TRANS_COMPLETE_BMSK)
   {
      /* Data transfer succeeded. Clear Transfer Complete status */
      HAL_sdhci_ResetInterruptStatus (driveno, SDHCI_STATUS_TRANS_COMPLETE_BMSK);

      if (SDCC_IS_APP_COMMAND(p_command->flags) != 0)
      {
         goto TransferDone;
      }
#ifndef USE_LOADER_UTILS 
      /* Do not send CMD12 for RPMB multi block transfer */
      if ((driveno == 0) && (p_device->active_partition_num == SDCC_EMMC_PHY_PARTITION_RPMB))
      {
         goto TransferDone;
      }
#endif
      /* Multi block transfer, send CMD12 to stop transfer */
      if (nBlkCount > 1)
      {
         sdcc_cmd_type sdcc_cmd;
	  
         sdcc_cmd.cmd       = SD_CMD12_STOP_TRANSMISSION;
         sdcc_cmd.resp_type = SDCC_RESP_SHORT;
         sdcc_cmd.prog_scan = (SDCC_IS_WRITE_TRANSFER(p_command->flags)) ? 1 : 0;
         sdcc_cmd.cmd_arg   = SDCC_CMD_ARGU_NULL;
         sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;
         rc = sdcc_command(p_device, &sdcc_cmd);
         
         if (SDCC_XLATE_WP_VIOLATION(sdcc_cmd.resp[0]) == SDCC_WP_VIOLATION_ERROR)
         {
            DEBUG ((EFI_D_WARN, "Attempted to program a write protected block\n"));
            return SDCC_ERR_WP_VIOLATION;
         }
         
         goto TransferDone;
      }
      else
      {
         if (SDCC_IS_WRITE_TRANSFER(p_command->flags))
         {
            /* In single block write transfer, check the WP error in */
            /* CMD13's response. The following function sends CMD13 to get */
            /* the card status. It checks for Write Protection error and waits */
            /* until the card is ready */
            rc = wait_card_ready_check_wp_violation(p_device);
         }
      }
   }

TransferDone:
   if (SDCC_IS_READ_TRANSFER(p_command->flags) && (TRUE == p_device->enable_dma))
   {
      /* invalidate data buffer cache */
      sdcc_bsp_memory_barrier();
      sdcc_bsp_cache_operation(SDCC_BSP_CACHE_INVAL, buffer, nTransferSize);
      sdcc_bsp_memory_barrier();
   }

   HAL_sdhci_SoftwareReset(driveno, HAL_SDHCI_RESET_CMD_DAT);
   return rc;

ErrorExit:
   sdcc_sdhci_print_error(driveno);
   
   HAL_sdhci_SoftwareReset(driveno, HAL_SDHCI_RESET_CMD);
   HAL_sdhci_SoftwareReset(driveno, HAL_SDHCI_RESET_DAT);
   HAL_sdhci_ResetInterruptStatus(driveno, HAL_SDHCI_INTERRUPT_MSK_ALL);
   
   return rc;
}

/******************************************************************************
* Name: sdcc_hc_set_transfer_mode
*
* Description:
*    This function sets up the data transfer. The Host Driver must set the 
*    Transfer Mode Register before issuing a command which transfers data. 
*    Writes to this reigster are ignored when the DAT Command Inhibit bit 
*    in the Present State register is set.
*
* Arguments:
*    p_device       [IN]: pointer to device structure
*    p_command      [IN]: pointer to command structure
*    block_count    [IN]: transfer size in unit of 512-byte blocks
*
* Returns:
*    Error Code
*
******************************************************************************/
static void sdcc_hc_set_transfer_mode (sdcc_dev_type *p_device, 
                                       sdcc_cmd_type *p_command, 
                                       uint16 block_count)
{
   struct HAL_sdhci_TransMode transfer_mode;
   SetMem(&transfer_mode, sizeof(transfer_mode), 0);

   if (SDCC_IS_READ_TRANSFER(p_command->flags))
   {
      transfer_mode.direction = 1;
   }

   if (block_count > 1)
   {
      transfer_mode.multiblock = 1;
   }
   
   /* Not doing any auto command for now */
   transfer_mode.autocmd_en = 0;

   transfer_mode.blk_count_en = 1;

   transfer_mode.dma_en = p_device->enable_dma;
   
   HAL_sdhci_SetTransferMode (p_device->driveno, &transfer_mode); 
}

/******************************************************************************
* Name: sdcc_hc_config_buswidth
*
* Description:
*    This function configures the MMC bus width in Host Controller interface mode.
*
* Arguments:
*    driveno          [IN] : drive number
*    bus_width        [IN] : MMC bus width
*
* Returns:
*    Error Code
*
******************************************************************************/
SDCC_STATUS sdcc_hc_config_buswidth (UINT32 driveno, SDCC_MMC_BUSWIDTH buswidth)
{

   switch (buswidth)
   {
   case SDCC_MMC_BUSWIDTH_1BIT:
      HAL_sdhci_SetBusWidth (driveno, HAL_SDCC_BUS_WIDTH_1_BIT);
      break;

   case SDCC_MMC_BUSWIDTH_4BIT:
   case SDCC_MMC_BUSWIDTH_DDR_4BIT:
      HAL_sdhci_SetBusWidth (driveno, HAL_SDCC_BUS_WIDTH_4_BIT);
      break;

   case SDCC_MMC_BUSWIDTH_8BIT:
   case SDCC_MMC_BUSWIDTH_DDR_8BIT:
      HAL_sdhci_SetBusWidth (driveno, HAL_SDCC_BUS_WIDTH_8_BIT);
      break;

   default:
      return SDCC_ERR_INVALID_PARAM;
   }

   return SDCC_NO_ERROR;
}

/******************************************************************************
* Name: sdcc_hc_wait_interrupt_status
*
* Description:
*    This function reads the Normal and Error Interrupt Status registers and 
*    waits until the first specified Status bit is set. It returns error if the 
*    expected Status bits are not set within a timeout period
*
* Arguments:
*    driveno        [IN]: drive number
*    mask           [IN]: Status bits that we are waiting for
*    pnIntStatus    [OUT]: returned copy from reading the Status Regs
*
* Returns:
*    Error Code
*
******************************************************************************/
static SDCC_STATUS 
sdcc_hc_wait_interrupt_status(UINT32 driveno, UINT32 mask, 
                              UINT32 *pnIntStatus)
{
   UINT32 status = 0, count = 0;
   
   /* Check if the mask contains an interrupt status bit */
   if ((mask & HAL_SDHCI_INTERRUPT_MSK_ALL) == FALSE)
   {
      return SDCC_ERR_INVALID_PARAM;
   }

   while (count < SDHCI_STATUS_POLL_MAX)
   {
      /* Wait for first Status bit that is set, either error or normal status bit */
      status = HAL_sdhci_GetInterruptStatus (driveno);

      if ((status & mask) || (status & SDHCI_STATUS_ERR_INT_BMSK))
      {
         /* save a copy of the Status and pass it back to caller */
         *pnIntStatus = status;
		 
         if (status & HAL_SDHCI_INTERRUPT_ERROR_MSK_ALL)
         {
            return SDCC_ERR_UNKNOWN;
         }
         else
         {
            return SDCC_NO_ERROR;
         }
      }

      sdcc_udelay(100);
      count += 100;
   }

   return SDCC_ERR_TIMEOUT;
}

/******************************************************************************
* Name: sdcc_command_sdhci_mode
*
* Description:
*    This function is to send the command and poll the status using the standard 
*    Host Controller interface to communicate with the memory devices
*
* Arguments:
*    sdcc_pdev  [IN] : pointer to device structure
*    sdcc_cmd   [IN] : pointer to command data structure
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS sdcc_command_sdhci_mode (sdcc_dev_type *sdcc_pdev, sdcc_cmd_type *cmd)
{
   UINT32 resp_type = 0;
   SDCC_STATUS rc = SDCC_NO_ERROR;
   BOOLEAN data_present = 0;
   UINT32 nIntStatus = 0;
   UINT32 driveno = sdcc_pdev->driveno;

   struct HAL_sdhci_Cmd hal_cmd;
   SetMem(&hal_cmd, sizeof(hal_cmd), 0);

   rc = sdcc_hc_wait_cmd_ready (driveno);
   if (rc != SDCC_NO_ERROR)
   {
      return SDCC_ERR_TIMEOUT;
   }
   
   if(sdcc_pdev->tuning_needed && (!sdcc_pdev->tuning_in_progress) && (!sdcc_pdev->tuning_done))
   {
      if (SDCC_DATA_TRANSFER_HS200_MODE == sdcc_pdev->speed_mode)
      {
         rc = sdcc_hs200_execute_tuning(sdcc_pdev);
         if (rc != SDCC_NO_ERROR)
         {
            DEBUG ((EFI_D_INFO, "Failed tuning sequence, revert to DDR50. rc =%d", rc));
            return rc; 
         }
      }      
   }

   /* set the timeout counter for the DAT line timeout */
   HAL_sdhci_SetTimeout(driveno, 0xF);
   
   HAL_sdhci_SetCmdArg(driveno, cmd->cmd_arg);

   /* Enable CDR only for CMD 17/18/21 */
   if ((SDCC_DATA_TRANSFER_HS400_MODE == sdcc_pdev->speed_mode) || 
       (SDCC_DATA_TRANSFER_HS200_MODE == sdcc_pdev->speed_mode))
   {
      if ((SD_CMD17_READ_BLOCK == cmd->cmd) || 
          (SD_CMD18_READ_MULTIPLE_BLOCK == cmd->cmd) ||
          (SD_CMD21 == cmd->cmd))
      {
         HAL_sdhci_CdrEnable (driveno, TRUE);   
      }
      else
      {
         HAL_sdhci_CdrEnable (driveno, FALSE);  
      }
   }  
   
   if (SDCC_IS_READ_TRANSFER(cmd->flags) || SDCC_IS_WRITE_TRANSFER(cmd->flags))
   {
      data_present = TRUE;
   }

   hal_cmd.cmd_idx = cmd->cmd;
   hal_cmd.cmd_type = 0;
   hal_cmd.data_present = data_present;
   
   if (cmd->resp_type == SDCC_RESP_NONE)
   {
      resp_type = 0;
   }
   else if (cmd->resp_type == SDCC_RESP_LONG)
   {
      resp_type = 1;
   }
   else if (cmd->resp_type == SDCC_RESP_SHORT)
   {
      resp_type = (cmd->prog_scan == 0) ? 2 : 3;
   }

   hal_cmd.resp_type = resp_type;

   /* issue the CMD */
   HAL_sdhci_SetCommand (driveno, &hal_cmd);

   /* polling the command complete status */  
   rc = sdcc_hc_wait_interrupt_status(driveno, SDHCI_STATUS_CMD_COMPLETE_BMSK, &nIntStatus);
   if (rc != SDCC_NO_ERROR)
   {
      cmd->status = nIntStatus;
      if (sdcc_pdev->tuning_needed && sdcc_pdev->tuning_done)
      {
         if((cmd->status & SDHCI_STATUS_CMD_CRC_ERR_BMSK) || 
            (cmd->status & SDHCI_STATUS_DATA_CRC_ERR_BMSK))
         {
            /* If CRC errors occur, execute full tuning sequence */
            sdcc_pdev->selected_phase = HS200_INVALID_TUNING_PHASE;
            sdcc_pdev->tuning_done = FALSE;
         }
      }

      HAL_sdhci_SoftwareReset(driveno, HAL_SDHCI_RESET_CMD);
      return rc;       
   }

   /* CMD sent successfully. Clear the CMD Complete status */
   HAL_sdhci_ResetInterruptStatus(driveno, SDHCI_STATUS_CMD_COMPLETE_BMSK);

   /* Get the response if expect any */   
   if (resp_type)
   {
      SetMem(cmd->resp, sizeof(cmd->resp), 0);
      HAL_sdhci_GetResponse (driveno, cmd->resp, (cmd->resp_type == SDCC_RESP_LONG));
   }

   /* For CMD6, CMD38 we poll the Transfer Complete status here.
    * In case of CMD25, the transfer complete status is polled later 
    * once the data has been written */
   if ((cmd->prog_scan) && (data_present == FALSE))
   {
      rc = sdcc_hc_wait_interrupt_status(driveno, SDHCI_STATUS_TRANS_COMPLETE_BMSK, &nIntStatus);
      if (rc != SDCC_NO_ERROR)
      {
         cmd->status = nIntStatus;
         return SDCC_ERR_PROG_DONE;
      }
	  
      /* clear the Transfer Complete status */
      HAL_sdhci_ResetInterruptStatus(driveno, SDHCI_STATUS_TRANS_COMPLETE_BMSK);
   }
 
   return SDCC_NO_ERROR;
}

/******************************************************************************
* Name: sdcc_sdhci_print_error
*
* Description:
*    This function prints the error from the Error Interrupt Status register
*
* Arguments:
*    driveno        [IN]: drive number
*
* Returns: None
*
******************************************************************************/
static void sdcc_sdhci_print_error (UINT32 driveno)
{
   UINT32 status = 0;

   /* Read the Error Interrupt Status register */
   status = HAL_sdhci_GetInterruptStatus (driveno);

   if (status & SDHCI_STATUS_ERR_INT_BMSK)
      DEBUG ((EFI_D_INFO, "Storage SDHCI Error. Specific error below: \n"));
 
   if (status & SDHCI_STATUS_CMD_TIMEOUT_ERR_BMSK)
      DEBUG ((EFI_D_INFO, "Storage SDHCI driver CMD_TIMEOUT_ERR \n"));
   else if (status & SDHCI_STATUS_CMD_CRC_ERR_BMSK)
      DEBUG ((EFI_D_INFO, "Storage SDHCI driver CMD_CRC_ERR \n"));     
   else if (status & SDHCI_STATUS_CMD_END_BIT_ERR_BMSK)   
      DEBUG ((EFI_D_INFO, "Storage SDHCI driver CMD_END_BIT_ERR \n")); 
   else if (status & SDHCI_STATUS_CMD_INDX_ERR_BMSK)
      DEBUG ((EFI_D_INFO, "Storage SDHCI driver CMD_INDEX_ERR \n")); 
   else if (status & SDHCI_STATUS_DATA_TIMEOUT_ERR_BMSK)
      DEBUG ((EFI_D_INFO, "Storage SDHCI driver CMD_DATA_TIMEOUT_ERR \n")); 
   else if (status & SDHCI_STATUS_DATA_CRC_ERR_BMSK)
      DEBUG ((EFI_D_INFO, "Storage SDHCI driver CMD_DATA_CRC_ERR \n")); 
   else if (status & SDHCI_STATUS_DATA_END_BIT_ERR_BMSK)
      DEBUG ((EFI_D_INFO, "Storage SDHCI driver CMD_DATA_END_BIT_ERR \n")); 
   else if (status & SDHCI_STATUS_CURRENT_LIMIT_ERR_BMSK)
      DEBUG ((EFI_D_INFO, "Storage SDHCI driver CURRENT_LIMIT_ERR \n")); 
   else if (status & SDHCI_STATUS_AUTO_CMD_ERR_BMSK)
      DEBUG ((EFI_D_INFO, "Storage SDHCI driver AUTO_CMD_ERR \n")); 
   else if (status & SDHCI_STATUS_ADMA_ERR_BMSK)
      DEBUG ((EFI_D_INFO, "Storage SDHCI driver ADMA_ERR \n"));
   else if (status & SDHCI_STATUS_TUNING_ERR_BMSK)
      DEBUG ((EFI_D_INFO, "Storage SDHCI driver TUNING_ERR \n"));
   else
      DEBUG ((EFI_D_INFO, "Storage SDHCI driver UNKNOWN ERROR \n"));
   
   return;
}

/******************************************************************************
* Name: sdcc_get_device_status_sdhci_mode
*
* Description:
*    This function checks the device status
*
* Arguments:
*    driveno            [IN] : drive number to check
*    rc                 [OUT] : A pointer to SDCC status
* Returns:
*    return DEVICE_STATUS and SDCC_STATUS type value.
*
******************************************************************************/
static DEVICE_STATUS sdcc_get_device_status_sdhci_mode (
   UINT32 driveno,
   SDCC_STATUS *rc
)
{
   UINT32 present_state = 0, mask = 0;
   DEVICE_STATUS ds = DEVICE_ERROR;
   UINT32 nIntStatus = 0;
   
   if (rc == NULL)
   {
      DEBUG ((EFI_D_ERROR, "sdcc_get_device_status_sdhci_mode: rc is NULL \n"));
      return DEVICE_ERROR;
   }

   *rc = SDCC_NO_ERROR;
   
   nIntStatus = HAL_sdhci_GetInterruptStatus(driveno);
   
   if(nIntStatus & HAL_SDHCI_INTERRUPT_ERROR_MSK_ALL)
   {
      *rc = SDCC_ERR_UNKNOWN;
      return DEVICE_ERROR;
   }
   
   present_state = HAL_sdhci_GetPresentState (driveno); 
   mask = HAL_SDHCI_PRESENT_STATE_RD_TRANS_ACT_BMSK |
          HAL_SDHCI_PRESENT_STATE_WR_TRANS_ACT_BMSK |
          HAL_SDHCI_PRESENT_STATE_CMD_INHIBIT_DAT_BMSK |
          HAL_SDHCI_PRESENT_STATE_CMD_INHIBIT_CMD_BMSK;
          
   if (present_state & mask)
   {
      ds = DEVICE_BUSY;
   }
   else
   {    
      if (nIntStatus & SDHCI_STATUS_TRANS_COMPLETE_BMSK)
      {
         ds = TRANSFER_DONE;
      }
      else
      {
         ds = DEVICE_IDLE;
      }
   }
   return ds;
}

/******************************************************************************
* Name: sdcc_get_device_status
*
* Description:
*    This function checks the device status
*
* Arguments:
*    driveno            [IN] : drive number to check
*    rc                 [OUT] : A pointer to SDCC status
* Returns:
*    return DEVICE_STATUS and SDCC_STATUS type value.
*
******************************************************************************/
DEVICE_STATUS sdcc_get_device_status (
   UINT32        driveno,
   SDCC_STATUS  *rc
)
{
   if ( !SDCC_DRIVENO_IS_VALID(driveno) ) 
   {
      return DEVICE_ERROR;
   }
   return sdcc_get_device_status_sdhci_mode(driveno, rc);
}

/******************************************************************************
* Name: sdcc_sdhci_check_transfer_status
*
* Description:
*    This function checks the device status when transfer is in progress
*
* Arguments:
*    driveno            [IN] : drive number to check
*    rc                 [OUT] : A pointer to SDCC status
* Returns:
*    return DEVICE_STATUS and SDCC_STATUS type value.
*
******************************************************************************/
DEVICE_STATUS sdcc_sdhci_check_transfer_status (
   UINT32 driveno,
   SDCC_STATUS *rc
)
{
   DEVICE_STATUS ds = DEVICE_IDLE;
   uint32 nIntStatus = 0;  
   
   if ( !SDCC_DRIVENO_IS_VALID(driveno) ) 
   {
      return DEVICE_ERROR;
   }
   
   if (rc == NULL)
   {
      DEBUG ((EFI_D_ERROR, "sdcc_sdhci_check_transfer_status: rc is NULL \n"));
      return DEVICE_ERROR;
   } 

   *rc = SDCC_NO_ERROR;
   nIntStatus = HAL_sdhci_GetInterruptStatus(driveno);
   
   if(nIntStatus & HAL_SDHCI_INTERRUPT_ERROR_MSK_ALL)
   {
      *rc = SDCC_ERR_UNKNOWN;
      return DEVICE_ERROR;
   }  
   
   if (nIntStatus & SDHCI_STATUS_TRANS_COMPLETE_BMSK)
   {
      ds = TRANSFER_DONE;
   }
   else
   {
      ds = DEVICE_BUSY;
   }

   return ds;
}/*For Block IO 2 only*/

/******************************************************************************
 * Name: sdcc_do_transfer_async_sdhci_mode
 *
 * Description:
 *    This is a generic function to perform a read or write data transfer on
 *    the EMMC. This is only for Block IO 2 using.
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
sdcc_do_transfer_async_sdhci_mode (sdcc_dev_type *p_device, 
                                   sdcc_cmd_type *p_command,
                                   byte          *buffer, 
                                   uint16         transfer_size)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   uint32 nIntStatus = 0;
   uint32 nTransferSize = 0, nBlkSize = 0, nBlkCount = 0;
   uint32 driveno = p_device->driveno;
   
   nBlkSize = (SDCC_MEM_BYTE_MODE == p_device->mem.block_mode) ?
                transfer_size : p_device->mem.mem_info.block_len;

   nBlkCount = (SDCC_MEM_BYTE_MODE == p_device->mem.block_mode) ?
                1 : transfer_size;

   /* transfer size in unit of bytes */
   nTransferSize = nBlkSize*nBlkCount;
   
   p_device->enable_dma = p_device->sdhci_adma_enable;

   nIntStatus = HAL_sdhci_GetInterruptStatus(driveno);
   
   /* clear all previous interrupts */
   if (HAL_SDHCI_INTERRUPT_MSK_ALL & nIntStatus)
   {
      HAL_sdhci_ResetInterruptStatus(driveno, HAL_SDHCI_INTERRUPT_MSK_ALL);

      /* reset CPSM & DPSM on errors, abort transaction if not finished */
      if (HAL_SDHCI_INTERRUPT_ERROR_MSK_ALL & nIntStatus)
      {
         HAL_sdhci_SoftwareReset(driveno, HAL_SDHCI_RESET_CMD_DAT);
      }
   }

   if (TRUE == p_device->enable_dma)
   {
      /* Create Descriptor table for ADMA2 in system memory */
      (void) sdcc_hc_prepare_dma_descriptors(driveno, buffer, nTransferSize);
   }

   /* configure the number of bytes in a data block */
   HAL_sdhci_SetBlockSize(driveno, nBlkSize);

   /* configure the number of blocks in a data transfer */
   HAL_sdhci_SetBlockCount(driveno, nBlkCount);

   /* set up the Transfer Mode register. This is done before sending CMD */
   sdcc_hc_set_transfer_mode(p_device, p_command, nBlkCount);

   /* send command now */
   rc = sdcc_command_sdhci_mode(p_device, p_command);
   
   if (SDCC_NO_ERROR != rc)
   {
      goto ErrorExit;
   }

   /* Multi block transfer, send CMD12 to stop transfer */
   if (nBlkCount > 1)
   {
      /*set SendStopCmd to TRUE*/
      BlkIo2_Xfer_Info.SendStopCmd = TRUE;
   }
   else
   {
      BlkIo2_Xfer_Info.SendStopCmd = FALSE;
   }
   
   /*Save to block IO 2 buffer*/
   BlkIo2_Xfer_Info.TransferSize = nTransferSize;
   BlkIo2_Xfer_Info.Buffer = buffer;
   BlkIo2_Xfer_Info.Pdevice = p_device;
   if (SDCC_IS_READ_TRANSFER(p_command->flags))
   {
      BlkIo2_Xfer_Info.Type = READ;
   }
   else
   {
      BlkIo2_Xfer_Info.Type = WRITE;
   }  
   return rc;

ErrorExit:
   sdcc_sdhci_print_error(driveno);
   return rc;
} /* sdcc_do_transfer_async_sdhci_mode */

/******************************************************************************
 * Name: sdcc_complete_transfer_sdhci_mode
 *
 * Description:
 *    This function is used to wrap up data transfer and can only used in Block 
 *    IO 2 driver.  
 *
 * Arguments: 
 *
 * Returns:
 *    None
 *
 *****************************************************************************/
static void sdcc_complete_transfer_sdhci_mode (void)
{
   sdcc_dev_type *p_device = BlkIo2_Xfer_Info.Pdevice;
   uint32 driveno = p_device->driveno;
   uint8 *buffer = BlkIo2_Xfer_Info.Buffer;
   uint32 nTransferSize = BlkIo2_Xfer_Info.TransferSize;
   
   /* Data transfer succeeded. Clear Transfer Complete status */
   HAL_sdhci_ResetInterruptStatus (driveno, SDHCI_STATUS_TRANS_COMPLETE_BMSK);
  
   /* Multi block transfer, send CMD12 to stop transfer */
   if (TRUE == BlkIo2_Xfer_Info.SendStopCmd)
   {
      sdcc_cmd_type sdcc_cmd;
	
      sdcc_cmd.cmd       = SD_CMD12_STOP_TRANSMISSION;
      sdcc_cmd.resp_type = SDCC_RESP_SHORT;
      sdcc_cmd.prog_scan = (BlkIo2_Xfer_Info.Type == WRITE) ? 1 : 0;
      sdcc_cmd.cmd_arg   = SDCC_CMD_ARGU_NULL;
      sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;
         
      sdcc_command_sdhci_mode(p_device, &sdcc_cmd);
   }
 
   if ((BlkIo2_Xfer_Info.Type == READ) && (TRUE == p_device->enable_dma))
   {   
      sdcc_bsp_memory_barrier();
      sdcc_bsp_cache_operation(SDCC_BSP_CACHE_INVAL, buffer, nTransferSize);
      sdcc_bsp_memory_barrier();
   }
   HAL_sdhci_SoftwareReset(driveno, HAL_SDHCI_RESET_CMD_DAT);  
} /* sdcc_complete_transfer_sdhci_mode */

/******************************************************************************
 * Name: sdcc_complete_transfer
 *
 * Description:
 *    This function is used to wrap up data transfer and can only used in Block 
 *    IO 2 driver.  
 *
 * Arguments: 
 *
 * Returns:
 *    None
 *
 *****************************************************************************/
void sdcc_complete_transfer (void)
{
   sdcc_complete_transfer_sdhci_mode();
} /* sdcc_complete_transfer */

/******************************************************************************
 * Name: sdcc_do_transfer_async
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
sdcc_do_transfer_async
(
   sdcc_dev_type     *p_device, 
   sdcc_cmd_type     *p_command,   
   byte              *buffer,
   uint16             transfer_size
)
{
   return sdcc_do_transfer_async_sdhci_mode(p_device, p_command,
                                      buffer, transfer_size);  
} /* sdcc_do_transfer_async */

/******************************************************************************
* Name: sdcc_handle_request
*
* Description:
*    This function reads num_blocks blocks of data from the data_address in
*    the device, storing them at the memory location given by buff. This is 
*    for Block IO 2 only
*
* Arguments:
*    handle           [IN] : a pointer to the SDCC device that was returned
*                            from sdcc_handle_open()
*    data_address     [IN] : start of the sector.  This is the sector unit
*                            address (512 bytes per block).
*    buff            [OUT] : pointer to buffer for the incoming data
*    num_blocks       [IN] : number of blocks to be read (512 bytes per
*                            block)
*    type            [ IN] : can be read or write
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
sdcc_handle_request
(
   struct sdcc_device *handle,
   UINT32              data_address,
   UINT8              *buff,
   UINT16              num_blocks,
   OPERATION_TYPE      type
)
{
   uint32 driveno = 0;
   boolean          multi_block = FALSE;
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

   if ( SDCC_CARD_UNKNOWN == sdcc_pdev->card_type )
   {
      return SDCC_ERR_CARD_UNDETECTED;
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

      if(type == READ)
      {   
         /* CMD17/CMD18 */
         sdcc_cmd.cmd = ( multi_block ) ? SD_CMD18_READ_MULTIPLE_BLOCK :
                                          SD_CMD17_READ_BLOCK;
         sdcc_cmd.resp_type = SDCC_RESP_SHORT;
         sdcc_cmd.prog_scan = 0;
         sdcc_cmd.flags = SDCC_TRANSFER_FLAG_IS_READ_TRANSFER;
      }
      else if(type == WRITE)
      {
         /* CMD24/CMD25 */
         sdcc_cmd.cmd = (multi_block) ? SD_CMD25_WRITE_MULTIPLE_BLOCK :
                                        SD_CMD24_WRITE_BLOCK;
         sdcc_cmd.resp_type = SDCC_RESP_SHORT;
         sdcc_cmd.prog_scan = 0;
         sdcc_cmd.flags = SDCC_TRANSFER_FLAG_IS_WRITE_TRANSFER;
      }  
  
      if ( SDCC_CARD_SDHC == sdcc_pdev->card_type ||
           SDCC_CARD_MMCHC == sdcc_pdev->card_type )
      {
         /* in high capacity cards, the 32-bit argument of memory access */
         /* commands uses the memory address in block address format */
         sdcc_cmd.cmd_arg = data_address;
      }
      else
      {
         /* in standard capacity cards, the 32-bit argument of memory access */
         /* commands uses the memory address in byte address format */
         sdcc_cmd.cmd_arg = data_address * (sdcc_pdev->mem.mem_info.block_len);   
      }

      rc = sdcc_do_transfer_async(sdcc_pdev, &sdcc_cmd, buff, num_blocks);
   }
   else
   {
      /* card type is SDIO */
      DEBUG ((EFI_D_ERROR, "Operation not supported on SDIO card"));
      return SDCC_ERR_SDIO;
   }
   return rc;
} /* sdcc_handle_request */


