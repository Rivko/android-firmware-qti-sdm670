/** @file SdccLoaderClockLibNull.c

  SDCC Loader clock driver BSP Stubs
  This file implements stubs for SDCC driver BSP, for XBL Loader clocks

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential

**/

/*=============================================================================
                              EDIT HISTORY

when         who     what, where, why
----------   ---     ----------------------------------------------------------- 
2016-06-15   kpa      Initial version.

=============================================================================*/

#include "SdccBsp.h"
#include "ClockBoot.h"



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
   return SDCC_BSP_NO_ERROR;
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
   return SDCC_BSP_NO_ERROR;
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
  return SDCC_BSP_NO_ERROR;
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

