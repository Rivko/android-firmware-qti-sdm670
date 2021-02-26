/**********************************************************************
* JtagPromgrammerBsp.c
*
* SDCC (Secure Digital Card Controller) driver BSP for TOOL.
*
* This file implements the SDCC driver BSP for the board in use for TOOL.
*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Confidential and Proprietary
*
**********************************************************************/

/*=======================================================================
                        Edit History


YYYY-MM-DD   who     what, where, why
----------------------------------------------------------------------
2014-11-07   bn      Initial version
===========================================================================*/

/*==============================================================================
  Includes for this module.
  ----------------------------------------------------------------------------*/


/*==============================================================================
                               DEFINES
==============================================================================*/


/*==============================================================================
================================================================================
                         J S D C C    B S P
==============================================================================*/

/******************************************************************************
* Name: jsdcc_bsp_tool_config_pll
*
* Description:
*    Setup the PLL for default frequency required so sdcc_boot_config_clock
*    can get the proper clock source.  
*
* Arguments:
*    None
*
* Returns:
*    None
*
* Note:
*
******************************************************************************/
void jsdcc_bsp_tool_config_pll(void)
{
   /* Stub out function - use CMM script to configure PLL for 8660 */
}  /* sdcc_bsp_boot_config_pll */


/******************************************************************************
* Name: jsdcc_bsp_tool_tune_pll
*
* Description:
*    Change the output frequency of the PLL.  This function is used to
*    fine tune the output frequency of the PLL which is used mainly to
*    detect data corruption on a specific clock frequency.  
*
* Arguments:
*    arg_L - Multiplier settting for the PLL
*
* Returns:
*    None
*
* Note:
*
******************************************************************************/
void jsdcc_bsp_tool_tune_pll(UINT32 arg_L)
{
   (void) arg_L;
   /* Stub out function - use CMM script to configure PLL for 8660 */
}  /* sdcc_bsp_boot_config_pll */


/******************************************************************************
* Name: jsdcc_bsp_tool_enable_pmic_ssbi_clk
*
* Description:
*    Setup the clock source for PMIC SSBI bus 
*    This function has undergone a slight change in 7x30, which also turn
*    on the power supply after enabling the SSBI clock source
*    
* Arguments:
*    None
*
* Returns:
*    None
*
* Note:
*
******************************************************************************/
void jsdcc_bsp_tool_enable_pmic_ssbi_clk(void)
{
   /* Stub out function - RUMI do not have PMIC */
}  /* jsdcc_bsp_tool_enable_pmic_ssbi_clk */

 
/******************************************************************************
* Name: jsdcc_bsp_kill_watchdog
*
* Description:
*    This function put the watchdog timer in freeze state
*    
* Arguments:
*    None
*
* Returns:
*    None
*
* Note:
*
******************************************************************************/
void jsdcc_bsp_kill_watchdog(void)
{
   // Watchdog timer is not accessable from Scorpion processor
}

