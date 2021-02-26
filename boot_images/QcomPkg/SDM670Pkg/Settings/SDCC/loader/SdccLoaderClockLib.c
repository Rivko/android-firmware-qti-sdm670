/** @file SdccLoaderClockLib.c

  SDCC Loader clock driver BSP
  This file implements the SDCC driver BSP for XBL Loader clocks

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential

**/

/*=============================================================================
                              EDIT HISTORY

when         who     what, where, why
----------   ---     ----------------------------------------------------------- 
2017-09-20   sb      Don't restrict SDC2 clocks to 100 MHz
2017-06-12   sb      Add eMMC support for SDM670  
2016-05-13   jt      Use busywait instead of MicroSecondDelay  
2016-01-18   jt      Initial version. Branch from 8996 XBL 

=============================================================================*/

#include "SdccBsp.h"
#include "msmhwiobase.h"
#include "HALhwio.h"
#include <api/systemdrivers/busywait.h>
#include "ClockBoot.h"

/* SDCC Clock Registers */
/* Note: These registers are target specific. */
/* Refer to the SWI to update the register addresses and their usage */
/* TODO: Use a different method to acquire this info */
#define HWIO_GCC_SDCC1_AHB_CBCR    0x126008
#define HWIO_GCC_SDCC2_AHB_CBCR    0x114008
#define HWIO_GCC_SDCC4_AHB_CBCR    0x116008
#define SDCC_AHB_CBCR_CLK_ENABLE   0x1

#define HWIO_GCC_SDCC1_BCR         0x126000
#define HWIO_GCC_SDCC2_BCR         0x114000
#define HWIO_GCC_SDCC4_BCR         0x116000
#define SDCC_BCR_BLK_ARES          0x1


/******************************************************************************
                           S D C C    B S P
******************************************************************************/

/******************************************************************************
* Name: sdcc_enable_mclk
*
* Description:
*    This function is to enable the SDCC controller gcc1_sdcc_apps clock.
*
* Arguments:
*    driveno          [IN] : drive number
*
* Returns:
*    sdcc_bsp_err_type
*
******************************************************************************/
sdcc_bsp_err_type
sdcc_enable_mclk (UINT32 driveno)
{
   /* The mclock is enabled by Clock function as part of sdcc_set_mclk_frequency() */ 
   return SDCC_BSP_NO_ERROR;
}

/******************************************************************************
* Name: sdcc_disable_mclk
*
* Description:
*    This function is to disable the SDCC controller gcc1_sdcc_apps_clk.
*
* Arguments:
*    driveno          [IN] : drive number
*
* Returns:
*    sdcc_bsp_err_type
*
******************************************************************************/
sdcc_bsp_err_type
sdcc_disable_mclk (UINT32 driveno)
{
   return SDCC_BSP_NO_ERROR;
}

/******************************************************************************
* Name: sdcc_set_mclk_frequency
*
* Description:
*    This function is to select the SDCC mclock rate.
*
* Arguments:
*    driveno          [IN] : drive number
*    sdcc_mclk        [IN] : clock speed
*
* Returns:
*    sdcc_bsp_err_type
*
******************************************************************************/
sdcc_bsp_err_type
sdcc_set_mclk_frequency (UINT32 driveno, sdcc_bsp_clk_type sdcc_mclk)
{
   sdcc_bsp_err_type BspStatus = SDCC_BSP_NO_ERROR;
   ClockSDCType ClockType;
   ClockBootPerfLevelType ClockFrequency;

   if (driveno >= sdcc_bsp_get_slot_count())
   {
      BspStatus = SDCC_BSP_ERR_INVALID_PARAM;
      goto sdcc_set_mclk_frequency_exit;
   }

   switch (driveno)
   {
   case SDCC_BSP_SLOT_0:
      ClockType = CLK_SDC1;
      break;
   case SDCC_BSP_SLOT_1:
      ClockType = CLK_SDC2;
      break;
   case SDCC_BSP_SLOT_3:
      ClockType = CLK_SDC4;
      break;
   default:
      ClockType = CLK_SDC_NONE;
   }

   ClockFrequency = CLOCK_BOOT_PERF_DEFAULT;

   if (sdcc_mclk <= SDCC_BSP_CLK_400_KHZ)
   {
      ClockFrequency = CLOCK_BOOT_PERF_DEFAULT;
      Clock_SetSDCClockFrequency (ClockFrequency, ClockType);
   }
   else if (sdcc_mclk <= SDCC_BSP_CLK_25_MHZ)
   {
      ClockFrequency = CLOCK_BOOT_PERF_NOMINAL;
      Clock_SetSDCClockFrequency (ClockFrequency, ClockType);
   }
   else if (sdcc_mclk <= SDCC_BSP_CLK_50_MHZ)
   {
      ClockFrequency = CLOCK_BOOT_PERF_TURBO;
      Clock_SetSDCClockFrequency (ClockFrequency, ClockType);
   }
   else /* 192MHz - 384MHz use a different clock API */
   {
      if ((0 == driveno) || (1 == driveno))
      {
        /* eMMC/SD (SDC1/SDC2) slots. The input frequency param is in unit of KHz */
        Clock_SetSDCClockFrequencyExt (sdcc_mclk/1000, ClockType);
      }
      else
      {
        /* Other non-eMMC slots */
        Clock_SetSDCClockFrequencyExt (SDCC_BSP_CLK_100_MHZ/1000, ClockType);
      }
   }
   
   BspStatus = SDCC_BSP_NO_ERROR;

sdcc_set_mclk_frequency_exit:
   return BspStatus;
}

/******************************************************************************
* Name: sdcc_reset_mclk
*
* Description:
*    This function resets the mclk
*
* Arguments:
*    driveno          [IN] : drive number
*
* Returns:
*     sdcc_bsp_err_type
*
******************************************************************************/
sdcc_bsp_err_type
sdcc_reset_mclk( UINT32 driveno )
{
   sdcc_bsp_err_type BspStatus = SDCC_BSP_NO_ERROR;

   if (driveno >= sdcc_bsp_get_slot_count())
   {
      BspStatus = SDCC_BSP_ERR_INVALID_PARAM;
      goto sdcc_reset_mclk_exit;
   }

   switch (driveno)
   {
   case SDCC_BSP_SLOT_0:
     /* Reset the SDCC clock */
     out_dword (HWIO_GCC_SDCC1_BCR, SDCC_BCR_BLK_ARES); 
     busywait(500);
     out_dword (HWIO_GCC_SDCC1_BCR, 0);		 
     break;
   
   case SDCC_BSP_SLOT_1:
     out_dword (HWIO_GCC_SDCC2_BCR, SDCC_BCR_BLK_ARES); 
     busywait(500);
     out_dword (HWIO_GCC_SDCC2_BCR, 0);	 
     break;

   case SDCC_BSP_SLOT_3:
     out_dword (HWIO_GCC_SDCC4_BCR, SDCC_BCR_BLK_ARES); 
     busywait(500);
     out_dword (HWIO_GCC_SDCC4_BCR, 0);		 
     break;

   default:
      BspStatus = SDCC_BSP_ERR_INVALID_PARAM;
      goto sdcc_reset_mclk_exit;        
   }

   BspStatus = SDCC_BSP_NO_ERROR;

sdcc_reset_mclk_exit:
   return BspStatus;
} /* sdcc_reset */

/******************************************************************************
* Name: sdcc_ahb_clk_init
*
* Description:
*    This function is to initialize the SDCC AHB clock.
*
* Arguments:
*    driveno          [IN] : drive number
*
* Returns:
*    sdcc_bsp_err_type
*
******************************************************************************/
sdcc_bsp_err_type
sdcc_ahb_clk_init ( UINT32 driveno )
{
   UINT32 reg_value = 0;
   sdcc_bsp_err_type BspStatus = SDCC_BSP_NO_ERROR;

   if (driveno >= sdcc_bsp_get_slot_count())
   {
      BspStatus = SDCC_BSP_ERR_INVALID_PARAM;
      goto sdcc_ahb_clk_init_exit;
   }

   switch (driveno)
   {
   case SDCC_BSP_SLOT_0:
      reg_value = in_dword (HWIO_GCC_SDCC1_AHB_CBCR);
      reg_value |= SDCC_AHB_CBCR_CLK_ENABLE;
      out_dword (HWIO_GCC_SDCC1_AHB_CBCR, reg_value); 
      break;
   case SDCC_BSP_SLOT_1:
      reg_value = in_dword (HWIO_GCC_SDCC2_AHB_CBCR);
      reg_value |= SDCC_AHB_CBCR_CLK_ENABLE;
      out_dword (HWIO_GCC_SDCC2_AHB_CBCR, reg_value); 
      break;
   case SDCC_BSP_SLOT_3:
      reg_value = in_dword (HWIO_GCC_SDCC4_AHB_CBCR);
      reg_value |= SDCC_AHB_CBCR_CLK_ENABLE;
      out_dword (HWIO_GCC_SDCC4_AHB_CBCR, reg_value); 
      break;
   default:
      BspStatus = SDCC_BSP_ERR_INVALID_PARAM;
      break;
  }

sdcc_ahb_clk_init_exit:
  return BspStatus;
}

/******************************************************************************
* Name: sdcc_ahb_clk_disable
*
* Description:
*    This function is to disable the SDCC AHB clock
*
* Arguments:
*    driveno          [IN] : drive number
*
* Returns:
*    sdcc_bsp_err_type
*
******************************************************************************/
sdcc_bsp_err_type
sdcc_ahb_clk_disable ( UINT32 driveno )
{
   return SDCC_BSP_NO_ERROR;
}

