#ifndef __JTAG_PROGRAMMER_BSP_H
#define __JTAG_PROGRAMMER_BSP_H
/******************************************************************************
 * JtagProgrammerBsp.h
 *
 * This file provides SDCC abstraction for dependent drivers for Non-OS(BOOT).
 *
 * Copyright (c) 2014 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *****************************************************************************/
/*=============================================================================

                        EDIT HISTORY FOR MODULE


when         who     what, where, why
----------   ---     ----------------------------------------------------------
2014-11-06   bn      Initial release
=============================================================================*/


/******************************************************************************
* Name: jsdcc_bsp_tool_config_pll
*
* Description:
*    Setup the PLL for default frequency required so sdcc_bsp_boot_config_clock
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
void jsdcc_bsp_tool_config_pll(void);

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
void jsdcc_bsp_tool_tune_pll(UINT32 arg_L);

/******************************************************************************
* Name: jsdcc_bsp_tool_enable_pmic_ssbi_clk
*
* Description:
*    Setup the clock source for PMIC SSBI bus
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
void jsdcc_bsp_tool_enable_pmic_ssbi_clk(void);

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
void jsdcc_bsp_kill_watchdog(void);


#endif /* ifndef __JTAG_PROGRAMMER_BSP_H */

