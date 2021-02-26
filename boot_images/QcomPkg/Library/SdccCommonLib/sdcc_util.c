/**********************************************************************
 * sdcc_util.c
 *
 * SDCC(Secure Digital Card Controller) driver utility functions.
 *
 * This file implements the utility functions for SDCC driver.
 *
 * Copyright (c) 2014 - 2017
 * Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/
/*=======================================================================
                        Edit History


when        who    what, where, why
---------   ----   ----------------------------------------------------- 
2017-10-07  sb     Keep set clock frequency for eMMC as same as starlord
2017-01-17  jt     Use clock divider to set clock frequency 
2016-12-13  jt     Remove legacy code  
2016-01-18  eo     Reduced sdcc code size
2015-12-18  jt     Enable bus power before reset  
2015-07-15  bn     Code clean up
2015-04-15  bn     Added GetStorageInfo support
2015-04-02  bn     Added code to qurey HS200/400 supported freq from BSP. Clean up
2015-02-25  bn     Moved mclk init to main init     
2015-02-24  bn     Removed the max supported frequency check
2015-01-26  rm     Integraged HS200/HS400 support for eMMC from UEFI 3.0
2014-11-10  bn     Add null pointer check for sdcc_get_slot_handle
2008-09-24  bn     Use SdccHal from SdccHalLib
2008-05-01  bn     Initial release. Branched from 8994 SBL
===========================================================================*/
#include "sdcc_priv.h"
#include "sdcc_util.h"
#include "sdcc_sd_util.h"
#include "sdcc_mmc_util.h"
#include "sdcc_handle_util.h"
#include "sdcc_hc.h"
#include "SdccBsp.h"
#include "SdccCommon.h"
#include <Library/SdccCommonLib/SdccHalLib/SdccHal.h>

/* SDCC slot info structure is internal, it is not expected to be externally used */
static struct sdcc_slot_data sdcc_info[SDCC_MAX_NUM_CONTROLLER];

extern UINT8 emmc_csd_raw_data[SDCC_MMC_CSD_SIZE];

/*********************************************************************/
/*             PROTOTYPES FOR LOCAL FUNCTIONS                        */
/*********************************************************************/
static SDCC_STATUS sdcc_cmd_send_verify(sdcc_dev_type *);
static SDCC_STATUS sdcc_get_resp(sdcc_dev_type *, sdcc_cmd_type *);
SDCC_STATUS sdcc_poll_status(sdcc_dev_type *, sdcc_cmd_type *);
static void sdcc_decode_csd(sdcc_dev_type *, UINT32 *, sdcc_csd_type *);
static void sdcc_decode_std_csd(sdcc_dev_type *, UINT32 *, sdcc_csd_type *);
static void sdcc_decode_hc_csd(UINT32 *, sdcc_csd_type *);
static void sdcc_calc_time_out(sdcc_dev_type *, const sdcc_csd_type *); 
static void sdcc_get_std_memory_info(sdcc_dev_type *, UINT32, UINT8, UINT32);
static void sdcc_get_hc_memory_info(sdcc_dev_type *, UINT32, UINT32);

/* TAAC time unit definition 
 *	0 = 1ns, 1 = 10ns, 2 = 100ns, 3 = 1us, 4 = 10us, 5 = 100us, 6 = 1ms, 7 = 10ms
 */
static const UINT32 tacc_unit[] = { /* units in ns */
	1, 10, 100, 1000, 10000, 100000, 1000000, 10000000
};

/* TAAC multiplier factor definition
 *  0 = reserved, 1 = 1.0, 2 = 1.2, 3 = 1.3, 4 = 1.5, 5 = 2.0, 6 = 2.5, 7 = 3.0, 
 *  8 = 3.5, 9 = 4.0, A = 4.5, B = 5.0, C = 5.5, D = 6.0, E = 7.0, F = 8.0
 */
static const UINT32 tacc_mult[] = {
   0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80
};

/******************************************************************************
* Name: sdcc_get_slot_handle
*
* Description:
*    This function return the handle to the sdcc data structure
*
* Arguments:
*    driveno          [IN] : sdcc drive number
*
* Returns:
*    None
*
******************************************************************************/
struct sdcc_slot_data *sdcc_get_slot_handle (UINT32 driveno)
{
   if (!SDCC_DRIVENO_IS_VALID(driveno))
   {
      DEBUG ((EFI_D_ERROR, "Invalid Drive Number:%d", driveno));
      return NULL;
   }
   
   return &(sdcc_info[driveno]);
}

/******************************************************************************
* Name: sdcc_reset
*
* Description:
*    This function reset the SDCC controller by assert/deassert the MCLK reset
*    signal.
*
* Arguments:
*    driveno          [IN] : drive number
*
* Returns:
*     None
*
******************************************************************************/
void
sdcc_reset( UINT32 driveno )
{
   sdcc_reset_mclk (driveno);
   return;
} /* sdcc_reset */


/******************************************************************************
* Name: sdcc_config_clk
*
* Description:
*    This function is to configure the SDCC controller clock depending
*    on current mode.
*
* Arguments:
*    sdcc_pdev        [IN] : pointer to device type 
*    mode             [IN] : card mode
*
* Returns:
*    None
*
******************************************************************************/
void sdcc_config_clk
(
   sdcc_dev_type *sdcc_pdev,
   SDCC_CARD_MODE mode
)
{
   sdcc_bsp_clk_type sdcc_mclk = SDCC_BSP_CLK_20_MHZ;
   SDCC_CARD_TYPE card_type = sdcc_pdev->card_type;
   UINT32 driveno = sdcc_pdev->driveno, nDivider = 0; 
   sdcc_bsp_clk_type bsp_max_clk = sdcc_bsp_get_max_supported_clk (driveno);

   switch (mode)
   {
   case SDCC_INIT_MODE:
      /* MCLK to be set for doing controller reset */
      /* controller reset is not happening at 144KHz hence increasing it to 20MHz */
      sdcc_mclk = SDCC_BSP_CLK_20_MHZ;
      break;
   case SDCC_IDENTIFICATION_MODE:
      /* lower the clock to < 400KHz for card identification */
      sdcc_mclk = SDCC_BSP_CLK_400_KHZ;
      sdcc_pdev->mem.clk_freq = SDCC_MMC_DEFAULT_FREQUENCY;
      break;
   case SDCC_HS_TRANSFER_MODE:
      /* High clock 49MHz for High Speed cards */
      DEBUG ((EFI_D_INFO, "sdcc_config_clk: High Speed card type.\n"));
      if ((card_type == SDCC_CARD_MMC) || (card_type == SDCC_CARD_MMCHC))
      {
          sdcc_pdev->mem.clk_freq = SDCC_MMC_FREQUENCY_52MHZ;
      }
      else if ((card_type == SDCC_CARD_SD) || (card_type == SDCC_CARD_SDHC))
      {
          sdcc_pdev->mem.clk_freq = SDCC_SD_HIGH_FREQUENCY;
      }
      /* Fall Through */

   case SDCC_DATA_TRANSFER_MODE:
   case SDCC_READ_TRANSFER_MODE:
   case SDCC_WRITE_TRANSFER_MODE:
      if ((card_type == SDCC_CARD_MMC) || (card_type == SDCC_CARD_MMCHC))
      {
         /* check the MMC clock freq */
         if (sdcc_pdev->mem.clk_freq == SDCC_MMC_FREQUENCY_52MHZ)
         {
            sdcc_mclk = SDCC_BSP_CLK_50_MHZ;
         }
         else if (sdcc_pdev->mem.clk_freq == SDCC_MMC_FREQUENCY_26MHZ)
         {
            sdcc_mclk = SDCC_BSP_CLK_25_MHZ;
         }
         else
         {
            /* MMC cards at Normal Speed */
            /* MMC 3.x compliant cards support only upto 20MHz */
            /* MMC 4.x compliant cards can be used at normal speed */
            /* without enabling HS_TIMING in EXT_CSD or by enumerating */
            /* as a MMC 3.x card */
            sdcc_mclk = SDCC_BSP_CLK_20_MHZ;
         }
      }
      else if ((card_type == SDCC_CARD_SD) || (card_type == SDCC_CARD_SDHC))
      {
         /* check the SD clock freq */
         if (sdcc_pdev->mem.clk_freq == SDCC_SD_HIGH_FREQUENCY)
         {
            sdcc_mclk = SDCC_BSP_CLK_50_MHZ;
         }
         else
         {
            sdcc_mclk = SDCC_BSP_CLK_25_MHZ;
         }
      }
      else
      {
         sdcc_mclk = SDCC_BSP_CLK_20_MHZ;
         DEBUG ((EFI_D_INFO, "sdcc_config_clk: Unknown card type.\n"));
      }
      break;

   case SDCC_DATA_TRANSFER_DDR_MODE:
      if ((SDCC_CARD_MMC == card_type) || (SDCC_CARD_MMCHC == card_type))
      {
         if (sdcc_pdev->mem.clk_freq == SDCC_MMC_FREQUENCY_52MHZ)
         {
           /* DDR mode, double the mclk */
            sdcc_mclk = SDCC_BSP_CLK_100_MHZ;
         }
         else
         {
            DEBUG((EFI_D_INFO, "sdcc_config_clk: Error! Unexpected clk frequency. \n"
                               "Default frequency is used\r\n"));
            sdcc_mclk = SDCC_BSP_CLK_20_MHZ;
         }
      }
      else
      {
         sdcc_mclk = SDCC_BSP_CLK_20_MHZ;
         DEBUG ((EFI_D_INFO, "sdcc_config_clk: DDR is only supported on MMC card\n"));
      }
      break;

    case SDCC_DATA_TRANSFER_HS200_MODE:
       if ((SDCC_CARD_MMC == card_type) || (SDCC_CARD_MMCHC == card_type))
       {
          if (SDCC_MMC_FREQUENCY_200MHZ == sdcc_pdev->mem.clk_freq)
          {
             /* HS200 mode. Get the HS200 supported frquency from BSP */
             sdcc_mclk = sdcc_bsp_get_hs200_supported_freq (driveno);
          }
       }
       else
       {
          sdcc_mclk = SDCC_BSP_CLK_20_MHZ;
          DEBUG((EFI_D_INFO, "sdcc_config_clk: HS200 is only supported on MMC card\r\n"));
       }
       break;

    case SDCC_DATA_TRANSFER_HS400_MODE:
       if ((SDCC_CARD_MMC == card_type) || (SDCC_CARD_MMCHC == card_type))
       {
          if (SDCC_MMC_FREQUENCY_400MHZ == sdcc_pdev->mem.clk_freq)
          {
             /* HS400 mode. Get the HS400 supported frquency from BSP */
             sdcc_mclk = sdcc_bsp_get_hs400_supported_freq (driveno);
          }
       }
       else
       {
          sdcc_mclk = SDCC_BSP_CLK_20_MHZ;
          DEBUG((EFI_D_INFO, "sdcc_config_clk: HS400 is only supported on MMC card\r\n"));
       }
       break;
         
   default:
      sdcc_mclk = SDCC_BSP_CLK_20_MHZ;
      DEBUG ((EFI_D_INFO, "sdcc_config_clk: mode unknown.\n"));
      break;
   }

   /* Check if freq exceeds max freq */
   if (sdcc_mclk > bsp_max_clk)
   {
      /* When PRE_SIL is set, the clock may be set to lower supported frequency */
      DEBUG ((EFI_D_ERROR, "sdcc_config_clk: WARNING!! SDCC mclk exceeds max supported clock.\n"));
      DEBUG ((EFI_D_ERROR, "Lower SDCC mclk to max supported clock: %d \n", bsp_max_clk));

      sdcc_mclk = bsp_max_clk;      
   }

   if(driveno == 0)
   {
     /* Clock to SDCC and to the card are 1:1 map */
     sdcc_set_mclk_frequency (driveno, sdcc_mclk);
     /* The SCLK frequency going to the memory card is controlled via the HC's register 0x2C */
     (void) sdcc_hc_set_clock(driveno, nDivider);
     sdcc_pdev->clk_freq_in_khz = sdcc_mclk/1000;
   }   
   else
   {
     if (sdcc_pdev->clk_freq_in_khz == 0)
     {
       /* Clock to SDCC and to the card are 1:1 map */
       sdcc_set_mclk_frequency (driveno, bsp_max_clk);
       sdcc_pdev->clk_freq_in_khz = bsp_max_clk/1000;
     }

     /* Figure out the divider */
     nDivider = ((1000 * sdcc_pdev->clk_freq_in_khz) / (2 * sdcc_mclk)); 

     /* The SCLK frequency going to the memory card is controlled via the HC's register 0x2C */
     (void) sdcc_hc_set_clock(driveno, nDivider);
   }
} /* sdcc_config_clk */

/******************************************************************************
* Name: sdcc_command
*
* Description:
*    This is a wrapper function for sending the command and poll 
*    the status. It decides whether to use the Qualcomm or standard 
*    Host Controller interface to communicate with the memory device
*
* Arguments:
*    sdcc_pdev         [IN] : pointer to device type 
*    sdcc_cmd          [IN] : pointer to command data structure
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS
sdcc_command(sdcc_dev_type *sdcc_pdev, sdcc_cmd_type *sdcc_cmd)
{
   if ((NULL == sdcc_pdev) || (NULL == sdcc_cmd))
   {
      return SDCC_ERR_INVALID_PARAM;
   }

   return sdcc_command_sdhci_mode(sdcc_pdev, sdcc_cmd);

} /* sdcc_command */

/******************************************************************************
* Name: sdcc_send_status
*
* Description:
*    This function is to let the currently selected device send
*    the device status.
*
* Arguments:
*    sdcc_pdev          [IN] : pointer to device structure
*
* Returns:
*    Returns the current device status
*
******************************************************************************/
SDCC_CARD_STATUS
sdcc_send_status( sdcc_dev_type *sdcc_pdev )
{
   sdcc_cmd_type        sdcc_cmd;
   SDCC_CARD_STATUS     card_status = SDCC_CARD_IGNORE;
   UINT32               rc          = SDCC_ERR_UNKNOWN;

   sdcc_cmd.cmd       = SD_CMD13_SEND_STATUS;
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.cmd_arg   = SDCC_ARG_RCA(sdcc_pdev->rca);
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   rc = sdcc_command(sdcc_pdev, &sdcc_cmd);
   if (SDCC_NO_ERROR == rc)
   {
      card_status = (SDCC_CARD_STATUS)(
                     ( sdcc_cmd.resp[0] >> SDCC_CARD_STATUS_SHFT )
                     & (SDCC_CARD_STATUS_BMSK));
   }

   return card_status;
} /* sdcc_send_status */

/******************************************************************************
* Name: sdcc_wait_prog_done
*
* Description:
*    This function is to poll the status to make sure the device is not
*    busy.  CMD12(stop) and CMD38(erase) need to call this to make sure
*    the device programming has finished.
*
* Arguments:
*    sdcc_pdev          [IN] : pointer to device structure
*    timeout_in_usec    [IN] : time wait for busy signal in microseconds unit
*    
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS
sdcc_wait_prog_done( sdcc_dev_type *sdcc_pdev, UINT32 timeout_in_usec )
{
   UINT32            count = 0;
   SDCC_STATUS       rc = SDCC_ERR_PROG_DONE;
   SDCC_CARD_STATUS  card_status = SDCC_CARD_IGNORE;
   UINT32            driveno = sdcc_pdev->driveno;

   while (count < timeout_in_usec)
   {
      UINT32 status;
      status = HAL_sdhci_GetInterruptStatus (driveno);

      if ((status & SDHCI_STATUS_TRANS_COMPLETE_BMSK))
      {
         HAL_sdhci_ResetInterruptStatus(driveno, SDHCI_STATUS_TRANS_COMPLETE_BMSK);
         rc = SDCC_NO_ERROR;
         break;
      }		  

      sdcc_udelay (10);
      count += 10; 
   }

   /* CMD13: if no prog_done indication, give it one more
      chance by polling the card directly */
   if (SDCC_NO_ERROR != rc)
   {
      DEBUG ((EFI_D_INFO, "timeout waiting for prog done \n"));
      card_status = sdcc_send_status(sdcc_pdev);
      if ( SDCC_CARD_TRAN == card_status )
      {
         rc = SDCC_NO_ERROR;
      }
   }

   return rc;
} /* sdcc_wait_prog_done */

/******************************************************************************
 * Name: sdcc_deinit
 *
 * Description:
 *    This function de-selects the device, turns off the power supply to the
 *    device and switches off the controller clock.
 *
 * Arguments:
 *    driveno     [IN] drive number
 *
 * Returns:
 *    SDCC_NO_ERROR if successful.  Error code otherwise.
 *
 ******************************************************************************/
SDCC_STATUS
sdcc_deinit( sdcc_dev_type  *sdcc_pdev )
{
   sdcc_slot_type   *sdcc_pdata = NULL;
   UINT32 driveno;

   sdcc_pdata = sdcc_pdev->pslot;
   driveno = sdcc_pdata->driveno;

   do
   {
      if (SDCC_HOST_IDLE == sdcc_pdev->host_state)
      {
         break;
      }
  
      if (sdcc_pdata->drive_in_use)
      {    
         /* set the power mode to 'power OFF' */
         HAL_sdhci_SetPower (driveno, FALSE);

         /* close the GPIO interfaces and turn off the clocks */
         sdcc_bsp_gpio_control(driveno, SDCC_BSP_GPIO_OFF);
	
         sdcc_disable_mclk (driveno); 
	 
         /* meaning the controller is not initialized any more*/
         sdcc_pdata->drive_in_use = FALSE;
      }

      sdcc_pdev->card_type = SDCC_CARD_UNKNOWN;
      sdcc_pdev->host_state = SDCC_HOST_IDLE;
      sdcc_pdev->clk_freq_in_khz = 0;
   }
   while(0); 

   return SDCC_NO_ERROR;
} /* sdcc_deinit */

/******************************************************************************
* Name: sdcc_calc_time_out
*
* Description:
*    This function is to read the timing parameters from the CSD structure and
*    to calculate the JEDEC time out for SD/eMMC cards;
*
* Arguments:
*    sdcc_pdev     [IN/OUT] : pointer to device type
*    csd           [IN] : pointer to the CSD stucture 
*
* Returns:
*    None
*
******************************************************************************/
static void
sdcc_calc_time_out
(
   sdcc_dev_type  *sdcc_pdev,
   const sdcc_csd_type  *csd
)
{
   UINT32 time_index, mult_index; 
   UINT32 taac_time = 0, read_access_time = 0; 
   UINT32 mult = 10; /* By default initialize with mmc timeout
                       multipler */ 
   UINT32 clk_rate = 0;  

   time_index = csd->taac & 0x07; 	/* bit 2:0 in TAAC represent time unit */
   mult_index = (csd->taac & 0x78) >> 0x3; /* bit 6:3 in TAAC represent multiplier factor */
   taac_time = tacc_mult[mult_index] * tacc_unit[time_index]; 

   /* Get the current clk rate */
   if (sdcc_pdev->clk_freq_in_khz == 0)
   {
      clk_rate = SDCC_DEFAULT_CLK_KHZ;
   } 
   else
   {
      clk_rate = sdcc_pdev->clk_freq_in_khz;
   }

    /* SD card timeout is 100 times normal access time,
       while MMC card timeout is 10 times normal access time */
   if ((sdcc_pdev->card_type == SDCC_CARD_SD) || 
       (sdcc_pdev->card_type == SDCC_CARD_SDHC))
   {
      mult = 100; 
   }
   else if ((sdcc_pdev->card_type == SDCC_CARD_MMC) || 
            (sdcc_pdev->card_type == SDCC_CARD_MMCHC))
   {
      mult = 10; 
   }

   /* Read access time (in terms of clk cycles) = (TAAC * clk_rate) + (100 * NSAC)
 
      We need (TAAC * clk_rate) to be in terms of clk cycles. Since clk_rate is in
      terms of khz, this means that TAAC should in terms of ms. However, the taac_time
      calculation made above is 10^7 times larger than its intended value (in ms), since
      the TAAC time unit is in terms of ns and the TAAC multiplier factor was scaled by 10.
 
      To avoid any potential overflow or underflow from ((TAAC * clk_rate)/10^7),
      we will split up the division by first reducing taac_time and clk_rate by a factor
      of 100 and then reducing the subsequent value by 1000. */

   read_access_time = (100 * csd->nsac) + (((taac_time / 100) * (clk_rate / 100)) / 1000);

   /* Read timeout (in terms of clk cycles) = mult * read_access_time */
   sdcc_pdev->mem.read_timeout_in_ms = (mult * read_access_time) / clk_rate; 

   /* Write timeout (in terms of clk cycles) = mult * 2^r2w_factor * read_access_time */
   sdcc_pdev->mem.write_timeout_in_ms = sdcc_pdev->mem.read_timeout_in_ms << csd->r2w_factor; 

   /* SD cards have a different timeout than MMC cards */
   if ((sdcc_pdev->card_type == SDCC_CARD_SD) || 
       (sdcc_pdev->card_type == SDCC_CARD_SDHC))
   {
      /* timeout limit for SD cards */
      const UINT32 sd_write_limit_ms = 250;
      const UINT32 sd_read_limit_ms = 100;

      /* SDHC uses the timeout limit */ 
      if ((sdcc_pdev->card_type == SDCC_CARD_SDHC) || 
          (sdcc_pdev->mem.read_timeout_in_ms > sd_read_limit_ms))
      {
         sdcc_pdev->mem.read_timeout_in_ms = sd_read_limit_ms;
      }

      if ((sdcc_pdev->card_type == SDCC_CARD_SDHC) || 
          (sdcc_pdev->mem.write_timeout_in_ms > sd_write_limit_ms))
      {
         sdcc_pdev->mem.write_timeout_in_ms = sd_write_limit_ms;
      }
   }
} /* sdcc_calc_time_out */

/******************************************************************************
* Name: sdcc_decode_csd
*
* Description:
*    This function is to parse out the CSD data.
*
* Arguments:
*    sdcc_pdev          [IN/OUT] : pointer to device type
*    data                   [IN] : pointer to data read in from response
*                                  registers
*    csd                    [IN] : pointer to the stucture to save the
*                                  csd data to
*
* Returns:
*    None
*
******************************************************************************/
static void
sdcc_decode_csd
(
   sdcc_dev_type  *sdcc_pdev,
   UINT32         *data,
   sdcc_csd_type  *csd
)
{
   UINT32   value;

   if (NULL == csd)
   {
      return;
   }

   SetMem (emmc_csd_raw_data, sizeof(emmc_csd_raw_data), 0);
   CopyMem (emmc_csd_raw_data, data, sizeof(emmc_csd_raw_data));

   /* In resp[0] */
   value = *data;
   csd->csd_structure = (UINT8)(value >> 30);

   switch (sdcc_pdev->card_type)
   {
      case SDCC_CARD_SD:
      case SDCC_CARD_SDHC:
      {
         if (1 == csd->csd_structure)
         {
            /* CSD Version 2.0: CSD_STRUCTURE = 1 */
            sdcc_decode_hc_csd(data, csd);
         }
         else
         {
            /* CSD Version 1.0: CSD_STRUCTURE = 0 */
            sdcc_decode_std_csd(sdcc_pdev, data, csd);
         }
      }
      break;

      case SDCC_CARD_MMC:
      case SDCC_CARD_MMCHC:
      default:
      {
         sdcc_decode_std_csd(sdcc_pdev, data, csd);
         /* System Specification version is for MMC only. */
         sdcc_pdev->mem.spec_vers = csd->spec_vers;
         /* check if write protection is supported */
         if (csd->wp_grp_enable && csd->spec_vers >= SDCC_MMC_SPEC_VERS_3X)
         {
            sdcc_pdev->mem.write_prot_support = TRUE;
            sdcc_pdev->mem.perm_write_prot_enable = csd->perm_write_protect;

            /* calculate the default write protect group size from CSD */
            /* refer to section 8.3 in JEDEC Standard No. 84-A44 */
            sdcc_pdev->mem.mem_info.write_protect_group_size_in_sectors =
               512 * (csd->wp_grp_size + 1) *
               (csd->erase.v31.erase_grp_size + 1) *
               (csd->erase.v31.erase_grp_mult + 1) /
               SDCC_DEFAULT_BLK_LENGTH_SIZE;
         }
         else
         {
            sdcc_pdev->mem.write_prot_support = FALSE;
         }

         if (csd->spec_vers >= SDCC_MMC_SPEC_VERS_3X)
         {
            /* calculate the default erase group size from CSD */
            /* refer to section 8.3 in JEDEC Standard No. 84-A44 */
            /* The default non high capacity erasable unit is: */
            /* EraseGroupSize = (ERASE_GRP_SIZE + 1) * (ERASE_GRP_MULT + 1) */
            sdcc_pdev->mem.mem_info.erase_group_size_in_sectors =
               ((csd->erase.v31.erase_grp_size + 1) * 
               (csd->erase.v31.erase_grp_mult + 1));
         }
      }
      break;
   }

   sdcc_calc_time_out(sdcc_pdev, csd); 
} /* sdcc_decode_csd */

/******************************************************************************
* Name: sdcc_decode_std_csd
*
* Description:
*    This function is to read the passed in data of the Standard Capacity
*    SD/MMC memory card and assign the proper bit values to the fields in
*    csd structure.
*
* Arguments:
*    sdcc_pdev     [IN] : pointer to device type
*    data          [IN] : pointer to data read in from response registers
*    csd          [OUT] : pointer to the stucture to save the csd data to
*
* Returns:
*    None
*
******************************************************************************/
static void
sdcc_decode_std_csd
(
   sdcc_dev_type  *sdcc_pdev,
   UINT32         *data,
   sdcc_csd_type  *csd
)
{
   UINT32    value;
   UINT32    tmp;

   if (NULL == csd)
   {
      return;
   }

   /* In resp[0] */
   value                   = *data;

   if ((SDCC_CARD_MMC == sdcc_pdev->card_type) ||
       (SDCC_CARD_MMCHC == sdcc_pdev->card_type))
   {
      /* For MMC only: spec_vers: system specification version --> csd[125:122] */
      csd->spec_vers          = (UINT8)((value & 0x3C000000) >> 26);
   }
   /* taac: data read access-time-1 --> csd[119:112] */
   csd->taac               = (UINT8)((value & 0x00FF0000) >> 16);
   /* nsac: data read access-time-2 in CLK cycles --> csd[111:104] */
   csd->nsac               = (UINT8)((value & 0x0000FF00) >> 8);
   /* tran_speed: max. data transfer rate --> csd[103:96] */
   csd->tran_speed         = (UINT8)(value & 0x000000FF);

   /* read_bl_len: max. read data block length --> csd[83:80] */
   value = *(++data);
   csd->read_bl_len = (UINT8)((value & 0x000F0000) >> 16);

   /* c_size: device size --> csd[73:62] */
   tmp         = value & 0x000003FF;
   value       = *(++data);
   csd->c_size = (UINT32)((tmp << 2) | ((value & 0xC0000000) >> 30));

   /* c_size_mult: device size multiplier --> csd[49:47] */
   csd->c_size_mult = (UINT16)((value & 0x00038000) >> 15);

   if ((sdcc_pdev->card_type == SDCC_CARD_MMC ||
       sdcc_pdev->card_type == SDCC_CARD_MMCHC) &&
       csd->spec_vers >= SDCC_MMC_SPEC_VERS_3X)
   {
      /* erase_grp_size: erase group size --> csd[46:42] */
      csd->erase.v31.erase_grp_size = (UINT8)((value & 0x00007C00) >> 10);

      /* erase_grp_mult: erase group size multiplier --> csd[41:37] */
      csd->erase.v31.erase_grp_mult = (UINT8)((value & 0x000003E0) >> 5);

      /* wp_grp_size: write protect group size --> csd[36:32] */
      csd->wp_grp_size = (UINT8)(value & 0x0000001F);
   }

   /* wp_grp_enable: write protect group enable --> csd[31:31] */
   value = *(++data);
   csd->wp_grp_enable = (UINT8)((value & 0x80000000) >> 31);

   /* r2w_factor: write speed factor --> csd[28:26] */
   csd->r2w_factor = (UINT8)((value & 0x1C000000) >> 26); 

   /* Permanent write protection --> csd[13] */
   csd->perm_write_protect = (UINT8)((value & 0x00002000) >> 13);
} /* sdcc_decode_std_csd */

/******************************************************************************
* Name: sdcc_decode_hc_csd
*
* Description:
*    This function is to read the passed in data of the High Capacity
*    SD memory card and assign the proper bit values to the fields in
*    csd structure.
*
* Arguments:
*    data          [IN] : pointer to data read in from response registers
*    csd          [OUT] : pointer to the stucture to save the csd data to
*
* Returns:
*    None
*
******************************************************************************/
static void
sdcc_decode_hc_csd
(
   UINT32         *data,
   sdcc_csd_type  *csd
)
{
   UINT32    value;
   UINT32    tmp;

   if (NULL == csd)
   {
      return;
   }

   /* In resp[0] */
   value                   = *data;
   /* taac: data read access-time is fixed to 0Eh (means 1ms) --> csd[119:112]*/
   csd->taac               = SDCC_CSD_TAAC;
   /* nsac: data read access-time in CLK cycles is fixed to 00h --> csd[111:104]*/
   csd->nsac               = SDCC_CSD_NSAC;
   /* tran_speed: max. data transfer rate --> csd[103:96] */
   csd->tran_speed         = (UINT8)(value & 0x000000FF);

   /* read_bl_len: max. read data block length is fixed to */
   /* 9h (means 512 Bytes) --> csd[83:80] */
   value                   = *(++data);
   csd->read_bl_len        = SDCC_CSD_READ_BL_LEN;

   /* c_size: device size --> csd[69:48] */
   /* This field is expanded to 22 bits and can indicate up to 2TBytes. */
   tmp         = value & 0x0000003F;
   value       = *(++data);
   csd->c_size = (UINT32)((tmp << 16) | ((value & 0xFFFF0000) >> 16));

   /* r2w_factor: write speed factor is fixed to 2h, which indicates 4 multiples --> csd[28:26] */
   value = *(++data);
   csd->r2w_factor = SDCC_CSD_R2W_FACTOR;
} /* sdcc_decode_hc_csd */

/******************************************************************************
* Name: sdcc_get_std_memory_info
*
* Description:
*    This function is to calculate the block length and memory capacity
*    of the Standard Capacity SD/MMC memory card.
*
* Arguments:
*    sdcc_pdev          [IN/OUT] : pointer to device type
*    block_len              [IN] : data block length
*    c_size_mult            [IN] : device size multiplier
*    c_size                 [IN] : device size
*
* Returns:
*    None
*
******************************************************************************/
static void
sdcc_get_std_memory_info
(
   sdcc_dev_type   *sdcc_pdev,
   UINT32           block_len,
   UINT8            c_size_mult,
   UINT32           c_size
)
{
   UINT32 mult = 0;
   UINT32 card_size_in_sectors = 0;

   mult = 1UL << (c_size_mult + 2);
   card_size_in_sectors = mult * (c_size + 1);

   sdcc_pdev->mem.mem_info.block_len = 1UL << block_len; /* in Bytes */

   /* Since the block length can be in diffrent size
   * 512, 1024, 2048 etc.  Calculate the card size in the absolute
   * block length of multiple 512 (i.e., SDCC_DEFAULT_BLK_LENGTH_SIZE)
   */
   sdcc_pdev->mem.mem_info.card_size_in_sectors = card_size_in_sectors *
      (sdcc_pdev->mem.mem_info.block_len/SDCC_DEFAULT_BLK_LENGTH_SIZE);
} /* sdcc_get_std_memory_info */

/******************************************************************************
* Name: sdcc_get_hc_memory_info
*
* Description:
*    This function is to calculate the block length and memory capacity
*    of the High Capacity SD/MMC memory card.
*
* Arguments:
*    sdcc_pdev          [IN/OUT] : pointer to device type
*    block_len              [IN] : data block length
*    c_size                 [IN] : device size
*
* Returns:
*    None
*
******************************************************************************/
static void
sdcc_get_hc_memory_info
(
   sdcc_dev_type   *sdcc_pdev,
   UINT32           block_len,
   UINT32           c_size
)
{
   sdcc_pdev->mem.mem_info.block_len = 1UL << block_len; /* in Bytes */

   /* The user data area capacity is calculated from the c_size as follows: */
   /*    memory capacity = (c_size+1)*512K byte    */
   /* where 512 is the block len of the High Capacity SD memory card and */
   /*       1K byte = 1024 bytes */
   /* Calculate the card size in the absolute block length of muliple */
   /* 512 (i.e., SDCC_DEFAULT_BLK_LENGTH_SIZE) */
   sdcc_pdev->mem.mem_info.card_size_in_sectors = (c_size + 1) *
      ((sdcc_pdev->mem.mem_info.block_len*1024) / SDCC_DEFAULT_BLK_LENGTH_SIZE);
} /* sdcc_get_hc_memory_info */

/******************************************************************************
* Name: sdcc_get_memory_info
*
* Description:
*    This function is to get the specific information related to the
*    memory device via CMD9 and/or CMD10 before it goes into data
*    transfer mode.
*
* Arguments:
*    sdcc_pdev          [IN/OUT] : pointer to device structure
*
* Returns:
*    Returns error code
*
* Note:
*    Card is in stand-by mode.
*
******************************************************************************/
SDCC_STATUS
sdcc_get_memory_info
(
   sdcc_dev_type *sdcc_pdev
)
{
   SDCC_STATUS      rc = SDCC_NO_ERROR;
   sdcc_cmd_type    sdcc_cmd;
   sdcc_csd_type    csd;
   SDCC_CARD_STATUS card_status = SDCC_CARD_IGNORE;

   /* CMD13: make sure in STBY state */
   card_status = sdcc_send_status(sdcc_pdev);
   if ( SDCC_CARD_STBY != card_status )
   {
      DEBUG ((EFI_D_INFO, "In invalid data TX state \n"));
      rc = SDCC_ERR_INVALID_TX_STATE;

      return rc;
   }

   /* CMD9: get card specific data */
   sdcc_cmd.cmd       = SD_CMD9_SEND_CSD;
   sdcc_cmd.resp_type = SDCC_RESP_LONG;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.cmd_arg   = SDCC_ARG_RCA(sdcc_pdev->rca);
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   rc = sdcc_command(sdcc_pdev, &sdcc_cmd);

   if (SDCC_NO_ERROR == rc)
   {
      SetMem(&csd, sizeof(csd), 0);
      sdcc_decode_csd(sdcc_pdev, sdcc_cmd.resp, &csd);

      switch (sdcc_pdev->card_type)
      {
         case SDCC_CARD_SD:
         case SDCC_CARD_SDHC:
         {
            if (1 == csd.csd_structure)
            {
               /* csd_structure = 1 means CSD Version 2.0 for */
               /* High Capacity SD memory card */
               sdcc_get_hc_memory_info(sdcc_pdev, csd.read_bl_len, csd.c_size);
            }
            else
            {
               /* csd_structure == 0 means CSD Version 1.0 for */
               /* Standard Capacity SD memory card */
               sdcc_get_std_memory_info(sdcc_pdev, csd.read_bl_len,
                                        csd.c_size_mult, csd.c_size);
            }

            /* set the mandatory maximum operating frequency */
            /* refer to section 5.3.2 in SD 2.0 Spec. */
            sdcc_pdev->mem.clk_freq = SDCC_SD_DEFAULT_FREQUENCY;
         }
         break;

         case SDCC_CARD_MMCHC:
         {
            /* defer to sdcc_config_mmc_modes_segment as EXT_CSD register */
            /* is read there.  SEC_COUNT must be used to calculate the high */
            /* density card size. */

            /* set the default clock frequency per TRAN_SPEED field in CSD */
            /* register */
            /* refer to section 8.3 in JEDEC Standard No. 84-A43 */
            if ( SDCC_MMC_TRAN_SPEED_26MHZ == csd.tran_speed )
            {
               sdcc_pdev->mem.clk_freq = SDCC_MMC_FREQUENCY_26MHZ;
            }
            else
            {
               sdcc_pdev->mem.clk_freq = SDCC_MMC_DEFAULT_FREQUENCY;
            }
         }
         break;

         case SDCC_CARD_MMC:
         default:
         {
            /* get the standard memory info anyway */
            sdcc_get_std_memory_info(sdcc_pdev, csd.read_bl_len,
                                     csd.c_size_mult, csd.c_size);

            /* set the default clock frequency per TRAN_SPEED field in CSD */
            /* register. Refer to section 8.3 in JEDEC Standard No. 84-A43 */
            if ( SDCC_MMC_TRAN_SPEED_26MHZ == csd.tran_speed )
            {
               sdcc_pdev->mem.clk_freq = SDCC_MMC_FREQUENCY_26MHZ;
            }
            else
            {
               sdcc_pdev->mem.clk_freq = SDCC_MMC_DEFAULT_FREQUENCY;
            }
         }
         break;
      }
   }

   return rc;
} /* sdcc_get_memory_info */

/******************************************************************************
* Name: sdcc_select_card
*
* Description:
*    This function is to select/deselect the specified card.
*
* Arguments:
*    sdcc_pdev          [IN] : pointer to device structure
*    select             [IN] : select if TRUE, deselect if FALSE
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS sdcc_select_card
(
   sdcc_dev_type  *sdcc_pdev,
   BOOLEAN         select
)
{
   sdcc_cmd_type    sdcc_cmd;
   SDCC_STATUS      rc = SDCC_ERR_UNKNOWN;

   /* CMD7: select the card */
   sdcc_cmd.cmd_arg   = (select) ? SDCC_ARG_RCA(sdcc_pdev->rca) : 0;
   sdcc_cmd.cmd       = SD_CMD7_SELECT_CARD;
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   rc = sdcc_command (sdcc_pdev, &sdcc_cmd);
   if (FALSE == select)
   {
      /* Ignore the error as it is expected behavior of the card to return 
       * CMD_TMOUT when CMD7 deselect is sent with 0x0 argument */
      rc = SDCC_NO_ERROR;
   }

   return rc;
} /* sdcc_select_card */

/*lint -restore */

